/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaFirmware535
// FILE:		$Workfile: TashaFirmware535.cpp $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaFirmware/TashaFirmware535/TashaFirmware535.cpp $
// CHECKIN:		$Date: 2.02.05 12:35 $
// VERSION:		$Revision: 150 $
// LAST CHANGE:	$Modtime: 2.02.05 11:12 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////
   
#include <gpiotest.h>
#include <sys/exception.h>
#include <cdefblackfin.h>
#include <CdefBF535.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <math.h>
#include "..\Include\Helper.h"
#include "..\Include\TMMessages.h"
#include "SharedMemoryDef.h"
#include "BasicMon.h"
#include "TashaFirmware535.h"
#include "SPICommunication.h"
#include "Sports.h"
#include "MemoryDMA.h"
#include "Altera.h"
#include "Boot533.h"
#include "EEProm.h"
#include "LED.h"
#include "Crosspoint.h"
#include "Watchdog.h"

DWORD 	g_dwProzessID			= 0;
BOOL 	g_bCaptureOn			= FALSE;
BOOL 	g_bBF533Booting			= FALSE;
BOOL	g_bEpldDataTransfer		= FALSE;
BOOL	g_bEEPromDataTransfer 	= FALSE;
DWORD	g_dwPCIFrameBuffer		= 0;
DWORD	g_dwAlarmEdgeMask		= 0;

#define MESSAGE_QUEUE_SIZE		100
#define MAX_BOOT_LOADER_SIZE	4096
#define MAX_DEBUG_STRING_LEN	256

MessageStruct 	g_SendMessageQueue[MESSAGE_QUEUE_SIZE] = {0};
WORD			g_wSendReadIndex = 0;
WORD			g_wSendWriteIndex= 0;

MessageStruct 	g_ReceiveMessageQueue[MESSAGE_QUEUE_SIZE] = {0};
WORD			g_wReceiveReadIndex = 0;
WORD			g_wReceiveWriteIndex= 0;

WORD			g_wSendMessageQueueLevel = 0; // Gibt Auskunft darüber wieviele Elemente in der Queue sind
BOOL			g_bSportInit = FALSE;

BYTE 			g_BootLoader[MAX_BOOT_LOADER_SIZE];	// Buffer, der den Bootloader aufnimmt.
BOOL 			g_dwBootLoaderLen	= 0;

CInt64			g_i64BF533LastResponseTime[CHANNEL_COUNT] = {0};
CInt64			g_i64BF533ResponseTimeOut = 60*1000*1000;

char 			g_szDebugString[MAX_DEBUG_STRING_LEN] = {0};


/////////////////////////////////////////////////////////////////////////
int main()
{	
	*pSIC_IMASK		= 0x0000;

	// Cyclecounter einschalten!
	startc();
	
	// SRAM Initialisierung
	*pEBIU_SDRRC 	= 0x0000074A;
	*pEBIU_SDBCTL 	= 0x00000013;
	*pEBIU_SDGCTL 	= 0x0091998F;

	wFinish	  		= 0;
		
	// PF10 aktiviren
	int nVal = *(volatile uint16 *)SPI0_FLG;
	nVal = CLRBIT(nVal, FLS5_P);
	*(volatile uint16 *)SPI0_FLG = nVal;
  
	ClearLED1();
	ClearLED2();

	
	// BasicMon initialisieren...
	InitBasicMon();	
	SetInterruptCallback((void*)MessageCallback);

	// TashaUnit mitteilen, daß Initialisierung komplett.
	DoConfirmInitComplete();
	
	// BF533LastResponseTime initialisieren
	for (int nSlave = 0; nSlave < CHANNEL_COUNT; nSlave++)
		g_i64BF533LastResponseTime[nSlave] = GetCycleCounter();

	do
	{
		HandleIncomingMessages();
		HandleSPICommunication();
		HandleOutgoingMessages();
		PollAlarmState();
		PollAdapterSelectState();
		PollPCKeysState();
		BlinkLED();
		
	}while (!wFinish);

	// TashaUnit mitteilen, das Firmware beendet wurde.
	DoConfirmTermination();

	// Kontrolle der Power- und Resettaste wieder an den PC übergeben.
	SwitchPowerButton(FALSE);
	SwitchResetButton(FALSE);
	
	// Watchdog ausschalten.
	DisableWatchdog();
	
	// Alle BF533 in den Reset
	writeAlteraReg(ALT_RESET_BF533, 0x00);
	
	// SPORT Schließen
	CloseSPORTs();

	// Den Altera in den Reset setzen
	OnRequestEpldDataTransfer(NULL, 1000);

	ClearLED1();
	ClearLED2();

	// Resete den BF535 mit Hilfe des Watchdogtimers
	*(volatile uint16 *)WDOG_CNT = 1000;
	*(volatile uint16 *)WDOG_CTL = 0;
	asm("idle;");
}

/////////////////////////////////////////////////////////////////////////
void MessageCallback(void)
{
	SAVE_USER_REGS;
	int		nMessageID	= 0;
	DWORD	dwParam1	= 0;
	DWORD	dwParam2	= 0;
	DWORD	dwParam3	= 0;
	DWORD	dwParam4	= 0;
	WORD	wWriteIndex	= 0;


	// Die Eintreffende Message holen und in die ReceiveMessageQueue hängen
	if (ReceiveMessage(nMessageID, dwParam1, dwParam2, dwParam3, dwParam4))
	{
		wWriteIndex = g_wReceiveWriteIndex;
		if (g_ReceiveMessageQueue[g_wReceiveWriteIndex].nMessageID == 0)
		{
			g_ReceiveMessageQueue[wWriteIndex].nMessageID 	= nMessageID;	
			g_ReceiveMessageQueue[wWriteIndex].dwParam1 	= dwParam1;	
			g_ReceiveMessageQueue[wWriteIndex].dwParam2 	= dwParam2;	
			g_ReceiveMessageQueue[wWriteIndex].dwParam3		= dwParam3;	
			g_ReceiveMessageQueue[wWriteIndex].dwParam4 	= dwParam4;	
	
			// Schreibindex zyklisch weiterzählen.
			wWriteIndex++;
			if (wWriteIndex == MESSAGE_QUEUE_SIZE)
				wWriteIndex = 0;
			g_wReceiveWriteIndex = wWriteIndex;
		}
	}

	RESTORE_USER_REGS;
}

/////////////////////////////////////////////////////////////////////////
BOOL ReceiveMessage(int& nMessageID, DWORD& dwParam1, DWORD& dwParam2, DWORD& dwParam3, DWORD& dwParam4)
{
	// Message auslesen und ACK setzen
	nMessageID  = PC2DSPMessageBuffer.nMessageID;
	dwParam1	= PC2DSPMessageBuffer.dwParam1;
	dwParam2	= PC2DSPMessageBuffer.dwParam2;
	dwParam3	= PC2DSPMessageBuffer.dwParam3;
	dwParam4	= PC2DSPMessageBuffer.dwParam4;
	PC2DSPMessageBuffer.bAck = TRUE;
	
	return TRUE;	
}

/////////////////////////////////////////////////////////////////////////
void HandleIncomingMessages()
{
	BOOL 	bResult 	= FALSE;
	int 	nMessageID  = 0;
	DWORD	dwParam1	= 0;
	DWORD	dwParam2	= 0;
	DWORD	dwParam3	= 0;
	DWORD	dwParam4	= 0;
	WORD 	wReadIndex = g_wReceiveReadIndex;

	// Befindet sich eine Message in der ReceiveMessageQueue?	
	if (g_ReceiveMessageQueue[wReadIndex].nMessageID != 0)
	{
		bResult	= TRUE;

		// Message aus der ReceiveMessageQueue holen...
		nMessageID = g_ReceiveMessageQueue[wReadIndex].nMessageID;
		dwParam1   = g_ReceiveMessageQueue[wReadIndex].dwParam1;
		dwParam2   = g_ReceiveMessageQueue[wReadIndex].dwParam2;
		dwParam3   = g_ReceiveMessageQueue[wReadIndex].dwParam3;
		dwParam4   = g_ReceiveMessageQueue[wReadIndex].dwParam4;

		// ... und aus der ReceiveMessageQueue austragen. 
		g_ReceiveMessageQueue[wReadIndex].nMessageID = 0;
			
		// Leseposition weitersetzen
		wReadIndex++;
		if (wReadIndex == MESSAGE_QUEUE_SIZE)
			wReadIndex = 0;
	
		// Leseindex sichern
		g_wReceiveReadIndex = wReadIndex;

		// Empfangene Message verteilen
		switch (nMessageID)
		{
			case TMM_REQUEST_RESUME_CAPTURE:
				OnRequestResumeCapture();
				break;
			case TMM_REQUEST_PAUSE_CAPTURE:
				OnRequestPauseCapture();
				break;
			case TMM_REQUEST_START_CAPTURE:
				OnRequestStartCapture();
				break;
			case TMM_REQUEST_STOP_CAPTURE:
				OnRequestStopCapture();
				break;
			case TMM_REQUEST_NEW_FRAME:
				OnRequestNewFrame(dwParam1, (WORD)dwParam2);
				break;
			case TMM_REQUEST_SET_ENCODER_PARAMETER:
				OnRequestSetEncoderParam(dwParam1, (WORD)dwParam2, dwParam3);
				break;
			case TMM_REQUEST_SET_BRIGHTNESS:
				OnRequestSetBrightness(dwParam1, (WORD)dwParam2, (int)dwParam3);
				break;
			case TMM_REQUEST_SET_CONTRAST:
				OnRequestSetContrast(dwParam1, (WORD)dwParam2, (int)dwParam3);
				break;
			case TMM_REQUEST_SET_SATURATION:
				OnRequestSetSaturation(dwParam1, (WORD)dwParam2, (int)dwParam3);
				break;
			case TMM_REQUEST_GET_ENCODER_PARAMETER:
				OnRequestGetEncoderParam(dwParam1, (WORD)dwParam2, dwParam3);
				break;
			case TMM_REQUEST_GET_BRIGHTNESS:
				OnRequestGetBrightness(dwParam1, (WORD)dwParam2);
				break;
			case TMM_REQUEST_GET_CONTRAST:
				OnRequestGetContrast(dwParam1, (WORD)dwParam2);
				break;
			case TMM_REQUEST_GET_SATURATION:
				OnRequestGetSaturation(dwParam1, (WORD)dwParam2);
				break;
			case TMM_REQUEST_SET_THRESHOLD_MD:
				OnRequestSetMDTreshold(dwParam1, (WORD)dwParam2, (int)dwParam3);
				break;
			case TMM_REQUEST_SET_THRESHOLD_MASK:
				OnRequestSetMaskTreshold(dwParam1, (WORD)dwParam2, (int)dwParam3);
				break;
			case TMM_REQUEST_SET_INCREMENT_MASK:
				OnRequestSetMaskIncrement(dwParam1, (WORD)dwParam2, (int)dwParam3);
				break;
			case TMM_REQUEST_SET_DELAY_MASK:
				OnRequestSetMaskDelay(dwParam1, (WORD)dwParam2, (int)dwParam3);
				break;			
			case TMM_REQUEST_GET_THRESHOLD_MD:
				OnRequestGetMDTreshold(dwParam1, (WORD)dwParam2);
				break;
			case TMM_REQUEST_GET_THRESHOLD_MASK:
				OnRequestGetMaskTreshold(dwParam1, (WORD)dwParam2);
				break;
			case TMM_REQUEST_GET_INCREMENT_MASK:
				OnRequestGetMaskIncrement(dwParam1, (WORD)dwParam2);
				break;
			case TMM_REQUEST_GET_DELAY_MASK:
				OnRequestGetMaskDelay(dwParam1, (WORD)dwParam2);		
				break;			
			case TMM_REQUEST_SET_PERMANENT_MASK:
				OnRequestSetPermanentMask(dwParam1, (WORD)dwParam2, dwParam3, dwParam4);
				break;	
			case TMM_REQUEST_CHANGE_PERMANENT_MASK:
				OnRequestChangePermanentMask(dwParam1, (WORD)dwParam2, dwParam3, (int)dwParam4);
				break;		
			case TMM_REQUEST_CLEAR_PERMANENT_MASK:
				OnRequestClearPermanentMask(dwParam1, (WORD)dwParam2);
				break;		
			case TMM_REQUEST_INVERT_PERMANENT_MASK:
				OnRequestInvertPermanentMask(dwParam1, (WORD)dwParam2);
				break;				
			case TMM_CONFIRM_NEW_CODEC_DATA:
				OnConfirmReceiveNewCodecData();
				break;
			case TMM_REQUEST_SCAN_FOR_CAMERAS:
				OnRequestScanForCameras(dwParam1);
				break;
			case TMM_REQUEST_SET_RELAIS_STATE:
				OnRequestSetRelayState((WORD)dwParam1, (BOOL)dwParam2);
				break;
			case TMM_REQUEST_GET_RELAIS_STATE:
				OnRequestGetRelayState();
				break;
			case TMM_REQUEST_GET_ALARM_STATE:
				OnRequestGetAlarmState();
				break;
			case TMM_REQUEST_SET_ALARM_EDGE:
				OnRequestSetAlarmEdge(dwParam1);
				break;
			case TMM_REQUEST_GET_ALARM_EDGE:
				OnRequestGetAlarmEdge();
				break;
			case TMM_REQUEST_EPLD_DATA_TRANSFER:
				OnRequestEpldDataTransfer(dwParam1, dwParam2);
				break;
			case TMM_REQUEST_TRANSFER_BOOT_LOADER:
				OnRequestTransferBootloader(dwParam1, dwParam2);
				break;
			case TMM_REQUEST_BOOT_BF533:
				OnRequestBoot533((int)dwParam1, dwParam2, dwParam3);
				break;
			case TMM_REQUEST_WRITE_EEPROM:
				OnRequestWriteToEEProm(dwParam1, dwParam2, dwParam3);
				break;		
			case TMM_REQUEST_READ_EEPROM:
				OnRequestReadFromEEProm(dwParam1, dwParam2, dwParam3);
				break;		
			case TMM_REQUEST_SET_CHANNEL:
				OnRequestSetChannel((WORD)dwParam1, (WORD)dwParam2);
				break;		
			case TMM_REQUEST_SET_ANALOG_OUT:
				OnRequestSetAnalogOut(dwParam1);
				break;
			case TMM_REQUEST_GET_ANALOG_OUT:
				OnRequestGetAnalogOut();
				break;
			case TMM_REQUEST_SET_TERMINATION_STATE:
				OnRequestSetTerminationState((BYTE)dwParam1);
				break;
			case TMM_REQUEST_GET_TERMINATION_STATE:
				OnRequestGetTerminationState();
				break;
			case TMM_REQUEST_SET_VIDEO_ENABLE_STATE:
				OnRequestSetVideoEnableState((BOOL)dwParam1);
				break;
			case TMM_REQUEST_GET_VIDEO_ENABLE_STATE:
				OnRequestGetVideoEnableState();
				break;
			case TMM_REQUEST_SET_CROSSPOINT_ENABLE_STATE:
				OnRequestSetCrosspointEnableState((BOOL)dwParam1);
				break;
			case TMM_REQUEST_GET_CROSSPOINT_ENABLE_STATE:
				OnRequestGetCrosspointEnableState();
				break;
			case TMM_REQUEST_TERMINATION:
				OnRequestTermination();
				break;
			case TMM_REQUEST_SET_FRAMEBUFF_ADDR:
				OnRequestSetPCIFrameBufferAddress(dwParam1);
				break;
			case TMM_REQUEST_ENABLE_WATCHDOG:
				OnRequestEnableWatchdog((int)dwParam1);
				break;
			case TMM_REQUEST_TRIGGER_WATCHDOG:
				OnRequestTriggerWatchdog((int)dwParam1);
				break;	
			case TMM_REQUEST_SET_POWER_LED:
				OnRequestSetPowerLED((BOOL)dwParam1);
				break;	
			case TMM_REQUEST_SET_RESET_LED:
				OnRequestSetResetLED((BOOL)dwParam1);
				break;	
			case TMM_REQUEST_GET_VARIATION_REGISTER:
				OnRequestGetVariationRegister();
				break;
			case TMM_REQUEST_GET_DIPSTATE:
				OnRequestGetDIPState();
				break;
			case TMM_REQUEST_BF535_PING:
				OnRequestBF535Ping();
				break;
			case TMM_REQUEST_SET_NR:
				OnRequestSetNR(dwParam1, (WORD)dwParam2, (BOOL)dwParam3);
				break;
			case TMM_REQUEST_GET_NR:
				OnRequestGetNR(dwParam1, (WORD)dwParam2);
				break;
			
			default:
				DoNotifyInformation(INFO_UNKNOWN_MESSAGE_ID, nMessageID);
		}
	}
}

/////////////////////////////////////////////////////////////////////////
BOOL HandleOutgoingMessages()
{
	BOOL bResult 	= FALSE;
	WORD wReadIndex = g_wSendReadIndex;
	
	// Befindet sich eine Message in der SendMessageQueue?
	if (g_SendMessageQueue[wReadIndex].nMessageID != 0)
	{
		// Dann diese abschicken.
		bResult = SendMessage(	g_SendMessageQueue[wReadIndex].nMessageID,
								g_SendMessageQueue[wReadIndex].dwParam1,
								g_SendMessageQueue[wReadIndex].dwParam2,
								g_SendMessageQueue[wReadIndex].dwParam3,
								g_SendMessageQueue[wReadIndex].dwParam4);
		
		// Wenn die Message erfolgreich verschickt wurde, diese
		// Message wieder aus der SendMessageQueue austragen.
		if (bResult)
		{
			g_SendMessageQueue[wReadIndex].nMessageID = 0;
			
			// Leseposition weitersetzen
			wReadIndex++;
			if (wReadIndex == MESSAGE_QUEUE_SIZE)
				wReadIndex = 0;
		
			// Leseindex sichern
			g_wSendReadIndex = wReadIndex;
		}
	}

	return bResult;	
}

/////////////////////////////////////////////////////////////////////////
BOOL SendMessage(int nMessageID, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3, DWORD dwParam4)
{
	BOOL  bResult 	= FALSE;
	DWORD dwCounter = 0;

	// Message in den DSP2PCMessageBuffer schreiben	
	DSP2PCMessageBuffer.nMessageID  = nMessageID;
	DSP2PCMessageBuffer.dwParam1	= dwParam1;
	DSP2PCMessageBuffer.dwParam2	= dwParam2;
	DSP2PCMessageBuffer.dwParam3	= dwParam3;
	DSP2PCMessageBuffer.dwParam4	= dwParam4;
	DSP2PCMessageBuffer.bAck		= FALSE;

	// Und PC darüber informieren.
	bResult = InterruptPC();

	if (bResult)
	{
		// Auf das Ack vom PC warten
		while(!DSP2PCMessageBuffer.bAck && (dwCounter++ < 100000))
			WaitMicro(1);

		g_wSendMessageQueueLevel--;
		bResult = DSP2PCMessageBuffer.bAck;
	}

	return bResult;	
}

/////////////////////////////////////////////////////////////////////////
BOOL PostMessage(int nMessageID, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3, DWORD dwParam4)
{
	static BOOL bMutex = FALSE;
	BOOL bResult = FALSE;
	DWORD dwCounter = 0;
	
	// Das Indexregister darf nur jeweils aus einem Thread, bzw ISR heraus zugegriffen werden.
	while(bMutex && (dwCounter++ < 100000))
		WaitMicro(1);

	if (bMutex == FALSE)
	{	
		bMutex = TRUE;
		
		WORD wWriteIndex  = g_wSendWriteIndex;
		
		// Ist noch ein Platz in der SendMessageQueue frei?
		if (g_SendMessageQueue[wWriteIndex].nMessageID == 0)
		{		
			// Message in die SendMessageQueue hängen
			g_SendMessageQueue[wWriteIndex].nMessageID 	= nMessageID;	
			g_SendMessageQueue[wWriteIndex].dwParam1 	= dwParam1;	
			g_SendMessageQueue[wWriteIndex].dwParam2 	= dwParam2;	
			g_SendMessageQueue[wWriteIndex].dwParam3	= dwParam3;	
			g_SendMessageQueue[wWriteIndex].dwParam4 	= dwParam4;	
	
			// Schreibindex zyklisch weiterzählen.
			wWriteIndex++;
			if (wWriteIndex == MESSAGE_QUEUE_SIZE)
				wWriteIndex = 0;
			g_wSendWriteIndex = wWriteIndex;
			g_wSendMessageQueueLevel++;
			bResult = TRUE;	
		}
		else
			printf("PostMessage: SendMessageQueue is full!\n");

		bMutex = FALSE;
	}
	
	return bResult;	
}

///////////////////////////////////////////////////////////////////////////////////
BOOL OnRequestNewFrame(DWORD dwUserData, WORD wSlave)
{
	return SendSPIMessage(TMM_REQUEST_NEW_FRAME, dwUserData, wSlave);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmNewFrame(DWORD dwUserData, WORD wSlave)
{
	return PostMessage(TMM_CONFIRM_NEW_FRAME, dwUserData, wSlave);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL OnRequestSetEncoderParam(DWORD dwUserData, WORD wSlave, DWORD dwValue)
{
	return SendSPIMessage(TMM_REQUEST_SET_ENCODER_PARAMETER, dwUserData, wSlave, dwValue);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmSetEncoderParam(DWORD dwUserData, WORD wSlave, DWORD dwValue)
{
	return PostMessage(TMM_CONFIRM_SET_ENCODER_PARAMETER, dwUserData, wSlave, dwValue);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL OnRequestSetBrightness(DWORD dwUserData, WORD wSlave, int nValue)
{
	return SendSPIMessage(TMM_REQUEST_SET_BRIGHTNESS, dwUserData, wSlave, nValue);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmSetBrightness(DWORD dwUserData, WORD wSlave, int nValue)
{
	return PostMessage(TMM_CONFIRM_SET_BRIGHTNESS, dwUserData, wSlave, nValue);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL OnRequestSetContrast(DWORD dwUserData, WORD wSlave, int nValue)
{
	return SendSPIMessage(TMM_REQUEST_SET_CONTRAST, dwUserData, wSlave, nValue);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmSetContrast(DWORD dwUserData, WORD wSlave, int nValue)
{
	return PostMessage(TMM_CONFIRM_SET_CONTRAST, dwUserData, wSlave, nValue);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL OnRequestSetSaturation(DWORD dwUserData, WORD wSlave, int nValue)
{
	return SendSPIMessage(TMM_REQUEST_SET_SATURATION, dwUserData, wSlave, nValue);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmSetSaturation(DWORD dwUserData, WORD wSlave, int nValue)
{
	return PostMessage(TMM_CONFIRM_SET_SATURATION, dwUserData, wSlave, nValue);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL OnRequestSetNR(DWORD dwUserData, WORD wSlave, BOOL bEnable)
{
	return SendSPIMessage(TMM_REQUEST_SET_NR, dwUserData, wSlave, bEnable);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmSetNR(DWORD dwUserData, WORD wSlave, BOOL bEnable)
{
	return PostMessage(TMM_CONFIRM_SET_NR, dwUserData, wSlave, bEnable);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL OnRequestGetNR(DWORD dwUserData, WORD wSlave)
{
	return SendSPIMessage(TMM_REQUEST_GET_NR, dwUserData, wSlave);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmGetNR(DWORD dwUserData, WORD wSlave, BOOL bEnable)
{
	return PostMessage(TMM_CONFIRM_GET_NR, dwUserData, wSlave, bEnable);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL OnRequestGetEncoderParam(DWORD dwUserData, WORD wSlave, DWORD dwValue)
{
	return SendSPIMessage(TMM_REQUEST_GET_ENCODER_PARAMETER, dwUserData, wSlave, dwValue);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmGetEncoderParam(DWORD dwUserData, WORD wSlave, DWORD dwValue)
{
	return PostMessage(TMM_CONFIRM_GET_ENCODER_PARAMETER, dwUserData, wSlave, dwValue);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL OnRequestGetBrightness(DWORD dwUserData, WORD wSlave)
{
	return SendSPIMessage(TMM_REQUEST_GET_BRIGHTNESS, dwUserData, wSlave);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmGetBrightness(DWORD dwUserData, WORD wSlave, int nValue)
{
	return PostMessage(TMM_CONFIRM_GET_BRIGHTNESS, dwUserData, wSlave, nValue);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL OnRequestGetContrast(DWORD dwUserData, WORD wSlave)
{
	return SendSPIMessage(TMM_REQUEST_GET_CONTRAST, dwUserData, wSlave);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmGetContrast(DWORD dwUserData, WORD wSlave, int nValue)
{
	return PostMessage(TMM_CONFIRM_GET_CONTRAST, dwUserData, wSlave, nValue);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL OnRequestGetSaturation(DWORD dwUserData, WORD wSlave)
{
	return SendSPIMessage(TMM_REQUEST_GET_SATURATION, dwUserData, wSlave);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmGetSaturation(DWORD dwUserData, WORD wSlave, int nValue)
{
	return PostMessage(TMM_CONFIRM_GET_SATURATION, dwUserData, wSlave, nValue);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL OnRequestSetMDTreshold(DWORD dwUserData, WORD wSlave, int nValue)
{
	return SendSPIMessage(TMM_REQUEST_SET_THRESHOLD_MD, dwUserData, wSlave, nValue);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmSetMDTreshold(DWORD dwUserData, WORD wSlave, int nValue)
{
	return PostMessage(TMM_CONFIRM_SET_THRESHOLD_MD, dwUserData, wSlave, nValue);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL OnRequestSetMaskTreshold(DWORD dwUserData, WORD wSlave, int nValue)
{
	return SendSPIMessage(TMM_REQUEST_SET_THRESHOLD_MASK, dwUserData, wSlave, nValue);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmSetMaskTreshold(DWORD dwUserData, WORD wSlave, int nValue)
{
	return PostMessage(TMM_CONFIRM_SET_THRESHOLD_MASK, dwUserData, wSlave, nValue);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL OnRequestSetMaskIncrement(DWORD dwUserData, WORD wSlave, int nValue)
{
	return SendSPIMessage(TMM_REQUEST_SET_INCREMENT_MASK, dwUserData, wSlave, nValue);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmSetMaskIncrement(DWORD dwUserData, WORD wSlave, int nValue)
{
	return PostMessage(TMM_CONFIRM_SET_INCREMENT_MASK, dwUserData, wSlave, nValue);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL OnRequestSetMaskDelay(DWORD dwUserData, WORD wSlave, int nValue)
{
	return SendSPIMessage(TMM_REQUEST_SET_DELAY_MASK, dwUserData, wSlave, nValue);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmSetMaskDelay(DWORD dwUserData, WORD wSlave, int nValue)
{
	return PostMessage(TMM_CONFIRM_SET_DELAY_MASK, dwUserData, wSlave, nValue);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL OnRequestClearPermanentMask(DWORD dwUserData, WORD wSlave)
{
	return SendSPIMessage(TMM_REQUEST_CLEAR_PERMANENT_MASK, dwUserData, wSlave);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmClearPermanentMask(DWORD dwUserData, WORD wSlave)
{
	return PostMessage(TMM_CONFIRM_CLEAR_PERMANENT_MASK, dwUserData, wSlave);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL OnRequestInvertPermanentMask(DWORD dwUserData, WORD wSlave)
{
	return SendSPIMessage(TMM_REQUEST_INVERT_PERMANENT_MASK, dwUserData, wSlave);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmInvertPermanentMask(DWORD dwUserData, WORD wSlave)
{
	return PostMessage(TMM_CONFIRM_INVERT_PERMANENT_MASK, dwUserData, wSlave);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL OnRequestGetMDTreshold(DWORD dwUserData, WORD wSlave)
{
	return SendSPIMessage(TMM_REQUEST_GET_THRESHOLD_MD, dwUserData, wSlave);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmGetMDTreshold(DWORD dwUserData, WORD wSlave, int nValue)
{
	return PostMessage(TMM_CONFIRM_GET_THRESHOLD_MD, dwUserData, wSlave, nValue);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL OnRequestGetMaskTreshold(DWORD dwUserData, WORD wSlave)
{
	return SendSPIMessage(TMM_REQUEST_GET_THRESHOLD_MASK, dwUserData, wSlave);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmGetMaskTreshold(DWORD dwUserData, WORD wSlave, int nValue)
{
	return PostMessage(TMM_CONFIRM_GET_THRESHOLD_MASK, dwUserData, wSlave, nValue);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL OnRequestGetMaskIncrement(DWORD dwUserData, WORD wSlave)
{
	return SendSPIMessage(TMM_REQUEST_GET_INCREMENT_MASK, dwUserData, wSlave);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmGetMaskIncrement(DWORD dwUserData, WORD wSlave, int nValue)
{
	return PostMessage(TMM_CONFIRM_GET_INCREMENT_MASK, dwUserData, wSlave, nValue);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL OnRequestGetMaskDelay(DWORD dwUserData, WORD wSlave)
{
	return SendSPIMessage(TMM_REQUEST_GET_DELAY_MASK, dwUserData, wSlave);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmGetMaskDelay(DWORD dwUserData, WORD wSlave, int nValue)
{
	return PostMessage(TMM_CONFIRM_GET_DELAY_MASK, dwUserData, wSlave, nValue);
}

/////////////////////////////////////////////////////////////////////////////
BOOL OnRequestResumeCapture() 
{
	g_bCaptureOn = TRUE;

//	EnableSPORTsDMA();
//	EnableSPORTs();
	
	return DoConfirmResumeCapture(); 	
}

//////////////////////////////////////////////////////////////////////
BOOL DoConfirmResumeCapture()
{
	return PostMessage(TMM_CONFIRM_RESUME_CAPTURE);
}

/////////////////////////////////////////////////////////////////////////////
BOOL OnRequestPauseCapture() 
{
//	DisableSPORTs();
//	DisableSPORTsDMA();
	
	g_bCaptureOn = FALSE;
	
	return DoConfirmPauseCapture();
}

//////////////////////////////////////////////////////////////////////
BOOL DoConfirmPauseCapture() 
{
	return PostMessage(TMM_CONFIRM_PAUSE_CAPTURE);
}

/////////////////////////////////////////////////////////////////////////////
BOOL OnRequestStartCapture() 
{
	// Beim ersten Aufruf den SPORT einschalten
	if (!g_bSportInit)
	{
		g_bSportInit = TRUE;	
		InitSPORTsISR();
		EnableSPORTsDMA();
		EnableSPORTs();
	}
			
	g_bCaptureOn = TRUE;

//	EnableSPORTsDMA();
//	EnableSPORTs();
	
	return DoConfirmStartCapture();
}

//////////////////////////////////////////////////////////////////////
BOOL DoConfirmStartCapture() 
{
	return PostMessage(TMM_CONFIRM_START_CAPTURE);
}

/////////////////////////////////////////////////////////////////////////////
BOOL OnRequestStopCapture() 
{
//	DisableSPORTs();
//	DisableSPORTsDMA();

	g_bCaptureOn = FALSE;

	return DoConfirmStopCapture();
}

//////////////////////////////////////////////////////////////////////
BOOL DoConfirmStopCapture() 
{
	return PostMessage(TMM_CONFIRM_STOP_CAPTURE);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL OnRequestScanForCameras(DWORD dwUserData)
{
	return DoConfirmScanForCameras(dwUserData, 0xff);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmScanForCameras(DWORD dwUserData, DWORD dwCameraMask)
{
	return PostMessage(TMM_CONFIRM_SCAN_FOR_CAMERAS, dwUserData, dwCameraMask);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL OnRequestSetRelayState(WORD wRelayID, BOOL bOpenClose)
{
	BYTE bResult = FALSE;
	
#if (1)
	WORD wVal = 0;
	
	// Relais schalten.
	if (readAlteraReg(ALT_ALARM_OUT, wVal))
	{
		if (bOpenClose)
			wVal = SETBIT(wVal, wRelayID);
		else
			wVal = CLRBIT(wVal, wRelayID);
		
		if (writeAlteraReg(ALT_ALARM_OUT, wVal))
			bResult = DoConfirmSetRelayState(wRelayID, bOpenClose);
	}
#else
	if (writeAlteraReg(wRelayID, bOpenClose))
		bResult = DoConfirmSetRelayState(wRelayID, bOpenClose);
#endif	

	return bResult;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmSetRelayState(WORD wRelayID, BOOL bOpenClose)
{
	return PostMessage(TMM_CONFIRM_SET_RELAIS_STATE, wRelayID, bOpenClose);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL OnRequestGetRelayState()
{
	WORD wVal = 0;
	
	if (readAlteraReg(ALT_ALARM_OUT, wVal))
		return DoConfirmGetRelayState(wVal);
	else
		return FALSE;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmGetRelayState(DWORD dwRelayStateMask)
{
	return PostMessage(TMM_CONFIRM_GET_RELAIS_STATE, dwRelayStateMask);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL OnRequestGetAlarmState()
{
	WORD wVal = 0;

	if (readAlteraReg(ALT_ALARM_IN, wVal))
		return DoConfirmGetAlarmState(wVal);
	else
		return FALSE;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmGetAlarmState(DWORD dwAlarmStateMask)
{
	return PostMessage(TMM_CONFIRM_GET_ALARM_STATE, dwAlarmStateMask^g_dwAlarmEdgeMask);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL OnRequestSetAlarmEdge(DWORD dwAlarmEdgeMask)
{
	g_dwAlarmEdgeMask = dwAlarmEdgeMask;
	return DoConfirmSetAlarmEdge(dwAlarmEdgeMask);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmSetAlarmEdge(DWORD dwAlarmEdgeMask)
{
	return PostMessage(TMM_CONFIRM_SET_ALARM_EDGE, dwAlarmEdgeMask);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL OnRequestGetAlarmEdge()
{
	return DoConfirmGetAlarmEdge(g_dwAlarmEdgeMask);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmGetAlarmEdge(DWORD dwAlarmEdgeMask)
{
	return PostMessage(TMM_CONFIRM_GET_ALARM_EDGE, dwAlarmEdgeMask);
}

/////////////////////////////////////////////////////////////////////////////
BOOL OnRequestSetPCIFrameBufferAddress(DWORD dwPCIFrameBuffer)
{
	g_dwPCIFrameBuffer = dwPCIFrameBuffer;
	
	*pPCI_MBAP  = (void*)(g_dwPCIFrameBuffer & 0xf8000000);
	g_dwPCIFrameBuffer = (g_dwPCIFrameBuffer  & 0x07ffffff) | 0xE0000000;

	return DoConfirmSetPCIFrameBufferAddress(dwPCIFrameBuffer);	
}

/////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmSetPCIFrameBufferAddress(DWORD dwPCIFrameBuffer)
{
	return PostMessage(TMM_CONFIRM_SET_FRAMEBUFF_ADDR, dwPCIFrameBuffer);
}

/////////////////////////////////////////////////////////////////////////////
BOOL DoNotifySlaveInitComplete(WORD wSlave, WORD wSlave2, DWORD dwCounter)
{
	return PostMessage(TMM_NOTIFY_SLAVE_INIT_READY, wSlave, wSlave2, dwCounter);
}

/////////////////////////////////////////////////////////////////////////////
BOOL OnRequestEpldDataTransfer(DWORD dwAddr, DWORD dwLen)
{
	BOOL  bResult 				= FALSE;
	DWORD dwPCIBuffer   = dwAddr;
	DWORD dwPCI_MBAP	= *(BYTE*)pPCI_MBAP;	// Sichern des PCI_MBAP-Registers
	
	// Unterbindet, das der SPI1 von der SPI-Kommunikation benutzt wird.
	g_bEpldDataTransfer	= TRUE;

	*pPCI_MBAP  = (void*)(dwPCIBuffer & 0xf8000000);
	dwPCIBuffer = (dwPCIBuffer  & 0x07ffffff) | 0xE0000000;

	if (progAlteraInit())
	{
		if (progAlteraStart())
			bResult = progAlteraTX((BYTE*)dwPCIBuffer, dwLen);
	}

	// PCI_MBAP-Register wieder herstellen.	
	*pPCI_MBAP  = (void*)dwPCI_MBAP;

	g_bEpldDataTransfer	= FALSE;

	if (bResult)
	{
		AlteraInit();
					
		// Kontrolle der Power-und Resettaste an die Tasha übergeben.
		SwitchPowerButton(TRUE);
		SwitchResetButton(TRUE);
		
		return DoConfirmEpldDataTransfer(dwAddr, dwLen);
	}
	else
	{
		DoNotifyInformation(INFO_EPLD_PROGRAMMING_FAILED);
		return bResult;	
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmEpldDataTransfer(DWORD dwAddr, DWORD dwLen)
{
	return PostMessage(TMM_CONFIRM_EPLD_DATA_TRANSFER, dwAddr, dwLen);
}

/////////////////////////////////////////////////////////////////////////////
BOOL OnRequestTransferBootloader(DWORD dwAddr, DWORD dwLen)
{
	BOOL  bResult 		= FALSE;
	DWORD dwPCIBuffer   = dwAddr;
	DWORD dwPCI_MBAP	= *(BYTE*)pPCI_MBAP;	// Sichern des PCI_MBAP-Registers

	*pPCI_MBAP  = (void*)(dwPCIBuffer & 0xf8000000);
	dwPCIBuffer = (dwPCIBuffer  & 0x07ffffff) | 0xE0000000;
	
	if (dwLen < MAX_BOOT_LOADER_SIZE)
	{
		// Bootloader für das Booten der BF533 kopieren.
		g_dwBootLoaderLen = dwLen;
		BYTE* pBuffer = (BYTE*)dwPCIBuffer;
		for (DWORD dwI = 0; dwI < dwLen; dwI++)
			g_BootLoader[dwI] = pBuffer[dwI];
		bResult = TRUE;
	}

	// PCI_MBAP-Register wieder herstellen.	
	*pPCI_MBAP  = (void*)dwPCI_MBAP;

	if (bResult)
		bResult = OnConfirmTransferBootloader(dwAddr, dwLen);
		
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL OnConfirmTransferBootloader(DWORD dwAddr, DWORD dwLen)
{
	return PostMessage(TMM_CONFIRM_TRANSFER_BOOT_LOADER, dwAddr, dwLen);
}

/////////////////////////////////////////////////////////////////////////////
BOOL OnRequestBoot533(int nBFNr, DWORD dwAddr, DWORD dwLen)
{
	// Während des Bootens darf der SPI1 nicht anderweitig verwendet werden.
	// Außerdem darf das PCI_MBAP-Register nicht verändert werden. g_bBF533Booting
	// stellt dieses sicher.
	g_bBF533Booting		= TRUE;

	g_i64BF533LastResponseTime[nBFNr] = GetCycleCounter();

	WaitMicro(10000);
	
	BOOL  bResult 		= FALSE;
	DWORD dwPCIBuffer   = dwAddr;
	DWORD dwPCI_MBAP	= *(BYTE*)pPCI_MBAP;	// Sichern des PCI_MBAP-Registers

	*pPCI_MBAP  = (void*)(dwPCIBuffer & 0xf8000000);
	dwPCIBuffer = (dwPCIBuffer  & 0x07ffffff) | 0xE0000000;

	// Soll ein Bootloader verwendet werden?
	if (g_dwBootLoaderLen>0)
	{
		if (SPIBootTransferBootLoader(nBFNr, g_BootLoader, g_dwBootLoaderLen))
			bResult = SPIBootTransferFirmware(nBFNr, (BYTE*)dwPCIBuffer, dwLen);
	}

	WaitMicro(10000);

	// PCI_MBAP-Register wieder herstellen.	
	*pPCI_MBAP  = (void*)dwPCI_MBAP;

	// Und weitermachen...
	g_bBF533Booting = FALSE;	
	
	if (bResult)
		return DoConfirmBoot533(nBFNr, dwAddr, dwLen);
	else
	{
		DoNotifyInformation(INFO_BOOT533_FAILED, nBFNr);
		return bResult;	
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmBoot533(int nBFNr, DWORD dwAddr, DWORD dwLen)
{
	return PostMessage(TMM_CONFIRM_BOOT_BF533, nBFNr, dwAddr, dwLen);
}

/////////////////////////////////////////////////////////////////////////////
BOOL OnRequestWriteToEEProm(DWORD dwAddr, DWORD dwLen, DWORD dwStartAddr)
{
	BOOL  bResult 		= FALSE;
	DWORD dwPCIBuffer   = dwAddr;
	DWORD dwPCI_MBAP	= *(BYTE*)pPCI_MBAP;	// Sichern des PCI_MBAP-Registers
	
	*pPCI_MBAP  = (void*)(dwPCIBuffer & 0xf8000000);
	dwPCIBuffer = (dwPCIBuffer  & 0x07ffffff) | 0xE0000000;

	if (ProgrammEEPromInit())
		bResult = WriteToEEPromX((BYTE*)dwPCIBuffer, dwLen, dwStartAddr);

	// PCI_MBAP-Register wieder herstellen.	
	*pPCI_MBAP  = (void*)dwPCI_MBAP;

	if (bResult)
		return DoConfirmWriteToEEProm(dwAddr, dwLen, dwStartAddr);
	else
	{
		DoNotifyInformation(INFO_WRITE_TO_EEPROM_FAILED);
		return bResult;	
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmWriteToEEProm(DWORD dwAddr, DWORD dwLen, DWORD dwStartAddr)
{
	return PostMessage(TMM_CONFIRM_WRITE_EEPROM, dwAddr, dwLen, dwStartAddr);
}

/////////////////////////////////////////////////////////////////////////////
BOOL OnRequestReadFromEEProm(DWORD dwAddr, DWORD dwLen, DWORD dwStartAddr)
{
	BOOL  bResult 		= FALSE;
	DWORD dwPCIBuffer   = dwAddr;
	DWORD dwPCI_MBAP	= *(BYTE*)pPCI_MBAP;	// Sichern des PCI_MBAP-Registers
	
	g_bEEPromDataTransfer = TRUE;
	
	*pPCI_MBAP  = (void*)(dwPCIBuffer & 0xf8000000);
	dwPCIBuffer = (dwPCIBuffer  & 0x07ffffff) | 0xE0000000;

	if (ProgrammEEPromInit())
		bResult = ReadFromEEPromX((BYTE*)dwPCIBuffer, dwLen, dwStartAddr);

	// PCI_MBAP-Register wieder herstellen.	
	*pPCI_MBAP  = (void*)dwPCI_MBAP;
	
	g_bEEPromDataTransfer = FALSE;
	
	if (bResult)
	{
		return DoConfirmReadFromEEProm(dwAddr, dwLen, dwStartAddr);
	}
	else
	{
		DoNotifyInformation(INFO_READ_FROM_EEPROM_FAILED);
		return bResult;	
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmReadFromEEProm(DWORD dwAddr, DWORD dwLen, DWORD dwStartAddr)
{
	return PostMessage(TMM_CONFIRM_READ_EEPROM, dwAddr, dwLen, dwStartAddr);
}

/////////////////////////////////////////////////////////////////////////////
BOOL OnRequestSetPermanentMask(DWORD dwUserData, WORD wSlave, DWORD dwAddr, DWORD dwLen)
{
	BOOL bResult 		= FALSE;

	bResult = SendSPIMessage(TMM_REQUEST_SET_PERMANENT_MASK, dwUserData, wSlave, dwAddr, dwLen);
	
	return bResult;	
}

/////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmSetPermanentMask(DWORD dwUserData, WORD wSlave, DWORD dwAddr, DWORD dwLen)
{
	return PostMessage(TMM_CONFIRM_SET_PERMANENT_MASK, dwUserData, wSlave, dwAddr, dwLen);
}

/////////////////////////////////////////////////////////////////////////////
BOOL OnRequestChangePermanentMask(DWORD dwUserData, WORD wSlave, DWORD dwPos, int nVal)
{
	return SendSPIMessage(TMM_REQUEST_CHANGE_PERMANENT_MASK, dwUserData, wSlave, dwPos, nVal);
}

/////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmChangePermanentMask(DWORD dwUserData, WORD wSlave, DWORD dwPos, int nVal)
{
	return PostMessage(TMM_CONFIRM_CHANGE_PERMANENT_MASK, dwUserData, wSlave, dwPos, nVal);
}

/////////////////////////////////////////////////////////////////////////////
BOOL OnRequestSetChannel(WORD wSlave, WORD wNewSlave)
{
	return SendSPIMessage(TMM_REQUEST_SET_CHANNEL, 0, wSlave, wNewSlave, 0);

}

/////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmSetChannel(WORD wSlave, WORD wNewSlave)
{
	return PostMessage(TMM_CONFIRM_SET_CHANNEL, wSlave, wNewSlave);
}

/////////////////////////////////////////////////////////////////////////////
BOOL OnRequestSetAnalogOut(DWORD dwCrosspointMask)
{
	if (!SetCrosspoint(dwCrosspointMask))
		return FALSE;
	
	return DoConfirmSetAnalogOut(dwCrosspointMask);
}

/////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmSetAnalogOut(DWORD dwCrosspointMask)
{
	return PostMessage(TMM_CONFIRM_SET_ANALOG_OUT, dwCrosspointMask);	
}

/////////////////////////////////////////////////////////////////////////////
BOOL OnRequestGetAnalogOut()
{
	DWORD dwCrosspointMask = 0x00000000; 

	if (!GetCrosspoint(dwCrosspointMask))
		return FALSE;
	
	return DoConfirmGetAnalogOut(dwCrosspointMask);	
}

/////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmGetAnalogOut(DWORD dwCrosspointMask)
{
	return PostMessage(TMM_CONFIRM_GET_ANALOG_OUT, dwCrosspointMask);	
}

/////////////////////////////////////////////////////////////////////////////
BOOL OnRequestSetTerminationState(BYTE byTermMask)
{
	writeAlteraReg(ALT_TERMINATION, (WORD)byTermMask);

	return DoConfirmSetTerminationState(byTermMask);	
}

/////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmSetTerminationState(BYTE byTermMask)
{
	// ToDo hier muß noch sinnvoller Code hin
	return PostMessage(TMM_CONFIRM_SET_TERMINATION_STATE, byTermMask);	
}

/////////////////////////////////////////////////////////////////////////////
BOOL OnRequestGetTerminationState()
{
	WORD wVal = 0;

	if (readAlteraReg(ALT_TERMINATION, wVal))
		return DoConfirmGetTerminationState(wVal);
	else
		return FALSE;	
}

/////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmGetTerminationState(BYTE byTermMask)
{
	return PostMessage(TMM_CONFIRM_GET_TERMINATION_STATE, byTermMask);	
}

/////////////////////////////////////////////////////////////////////////////
BOOL OnRequestSetVideoEnableState(BOOL bState)
{
	WORD wVal = 0;
	
	if (readAlteraReg(ALT_CTRL, wVal))
	{
		if (bState)
			wVal = SETBIT(wVal, ALT_CTRL_VIDEO_ENABLE_BIT);
		else
			wVal = CLRBIT(wVal, ALT_CTRL_VIDEO_ENABLE_BIT);
		
		if (writeAlteraReg(ALT_CTRL, wVal))
			return DoConfirmSetVideoEnableState(bState);	
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmSetVideoEnableState(BOOL bState)
{
	return PostMessage(TMM_CONFIRM_SET_VIDEO_ENABLE_STATE, bState);	
}

/////////////////////////////////////////////////////////////////////////////
BOOL OnRequestGetVideoEnableState()
{
	WORD wVal = 0;
	BOOL bResult = FALSE;
	
	if (readAlteraReg(ALT_CTRL, wVal))
	{
		BOOL bState = TSTBIT(wVal, ALT_CTRL_VIDEO_ENABLE_BIT);
		bResult = DoConfirmGetVideoEnableState(bState); 	
	}

	return bResult;	
}

/////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmGetVideoEnableState(BOOL bState)
{
	return PostMessage(TMM_CONFIRM_GET_VIDEO_ENABLE_STATE, bState);	
}

/////////////////////////////////////////////////////////////////////////////
BOOL OnRequestSetCrosspointEnableState(BOOL bState)
{
	if (bState)
		EnableCrosspoint();
	else
		DisableCrosspoint();

	return DoConfirmSetCrosspointEnableState(bState);	
}

/////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmSetCrosspointEnableState(BOOL bState)
{
	return PostMessage(TMM_CONFIRM_SET_CROSSPOINT_ENABLE_STATE, bState);	
}

/////////////////////////////////////////////////////////////////////////////
BOOL OnRequestGetCrosspointEnableState()
{
	BOOL bState = FALSE;
	BOOL bResult = FALSE;
	
	if (GetCrosspointEnableState(bState))
		bResult = DoConfirmGetCrosspointEnableState(bState);
		
	return bResult;	
}

/////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmGetCrosspointEnableState(BOOL bState)
{
	return PostMessage(TMM_CONFIRM_GET_CROSSPOINT_ENABLE_STATE, bState);	
}

/////////////////////////////////////////////////////////////////////////////
BOOL OnRequestTriggerWatchdog(int nTimeOut)
{
	BOOL bResult = FALSE;
	
	if (TriggerWatchdog(nTimeOut))
		bResult = DoConfirmTriggerWatchdog(nTimeOut);
	
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmTriggerWatchdog(int nTimeOut)
{
	return PostMessage(TMM_CONFIRM_TRIGGER_WATCHDOG, nTimeOut);	
}

/////////////////////////////////////////////////////////////////////////////
BOOL OnRequestEnableWatchdog(int nBF533ResponseTimeOut)
{
	BOOL bResult = FALSE;
	
	// Umrechnung von ms in ys
	g_i64BF533ResponseTimeOut = CInt64(nBF533ResponseTimeOut*1000);
	
	// Watchdog ersteinmal auf 3 Minuten!
	if (EnableWatchdog(180))
		bResult = DoConfirmEnableWatchdog(nBF533ResponseTimeOut);	
	
	return bResult;	
}

/////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmEnableWatchdog(int nBF533ResponseTimeOut)
{
	return PostMessage(TMM_CONFIRM_ENABLE_WATCHDOG, nBF533ResponseTimeOut);	
}


/////////////////////////////////////////////////////////////////////////////
BOOL OnRequestSetPowerLED(BOOL bState)
{
	BOOL bResult = FALSE;
	WORD wVal	 = 0;
	
	if (readAlteraReg(ALT_PC_LEDS, wVal))
	{
		if (bState)
			wVal = SETBIT(wVal, ALT_PC_LED1_F1_BIT);
		else
			wVal = CLRBIT(wVal, ALT_PC_LED1_F1_BIT);
		
	
		bResult = writeAlteraReg(ALT_PC_LEDS, wVal);
	}

	if (bResult)
		bResult = DoConfirmSetPowerLED(bState);	
		
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmSetPowerLED(BOOL bState)
{
	return PostMessage(TMM_CONFIRM_SET_POWER_LED, bState);	
}

/////////////////////////////////////////////////////////////////////////////
BOOL OnRequestSetResetLED(BOOL bState)
{
	BOOL bResult = FALSE;
	WORD wVal	 = 0;
	
	if (readAlteraReg(ALT_PC_LEDS, wVal))
	{
		if (bState)
			wVal = SETBIT(wVal, ALT_PC_LED2_F1_BIT);
		else
			wVal = CLRBIT(wVal, ALT_PC_LED2_F1_BIT);
		
		bResult = writeAlteraReg(ALT_PC_LEDS, wVal);
	}

	if (bResult)
		bResult = DoConfirmSetResetLED(bState);	
	
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmSetResetLED(BOOL bState)
{
	return PostMessage(TMM_CONFIRM_SET_RESET_LED, bState);	
}

/////////////////////////////////////////////////////////////////////////////
BOOL OnRequestGetVariationRegister()
{
	BOOL bResult = FALSE;
	WORD wVal	 = 0;
	
	if (readAlteraReg(ALT_VARIATION, wVal))
		bResult = DoConfirmGetVariationRegister(wVal);	

	return bResult;	
}

/////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmGetVariationRegister(WORD wVal)
{
	return PostMessage(TMM_CONFIRM_GET_VARIATION_REGISTER, wVal);	
}

/////////////////////////////////////////////////////////////////////////////
BOOL OnRequestGetDIPState()
{
	BOOL bResult = FALSE;
	WORD wVal	 = 0;
	
	if (readAlteraReg(ALT_STATUS_TEST, wVal))
	{
		wVal = (wVal>>3) & 0x0003;
		
		bResult = DoConfirmGetDIPState(wVal);	
	}

	return bResult;	
}

/////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmGetDIPState(WORD wVal)
{
	return PostMessage(TMM_CONFIRM_GET_DIPSTATE, wVal);	
}

/////////////////////////////////////////////////////////////////////////////
BOOL PollAlarmState()
{
	BOOL  bResult 				= FALSE;
	WORD wCurrentAlarmState	= 0;
	static WORD m_wAlarmState 	= 0;
	
	// CurrentAlarmstatusregister lesen.
	if (readAlteraReg(ALT_ALARM_IN, wCurrentAlarmState))
	{
		if (wCurrentAlarmState != m_wAlarmState)
		{										  
			m_wAlarmState = wCurrentAlarmState;
						
			bResult = DoNotifyAlarmStateChanged(wCurrentAlarmState^g_dwAlarmEdgeMask);
		}
	}
	
	return bResult;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL DoNotifyAlarmStateChanged(BYTE byCurrentAlarmState)
{
	return PostMessage(TMM_NOTIFY_ALARM_STATE_CHANGED, byCurrentAlarmState);
}

/////////////////////////////////////////////////////////////////////////////
BOOL PollAdapterSelectState()
{
	BOOL  bResult = FALSE;
	WORD  wCurrentAdapterSelectState = 0;
	static WORD m_wCurrentAdapterSelectState = 0xffff;
	
	// CurrentAdapterselect register lesen.
	if (readAlteraReg(ALT_PC_KEYS_ADAPT_SEL, wCurrentAdapterSelectState))
	{
		wCurrentAdapterSelectState = (wCurrentAdapterSelectState >> 4) & 0x0f;
		
		if (wCurrentAdapterSelectState != m_wCurrentAdapterSelectState)
		{										  
			m_wCurrentAdapterSelectState = wCurrentAdapterSelectState;
						
			bResult = DoNotifyAdapterSelectStateChanged(wCurrentAdapterSelectState);
		}
	}
	
	return bResult;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL DoNotifyAdapterSelectStateChanged(BYTE byCurrentAdapterSelectState)
{
	return PostMessage(TMM_NOTIFY_ADAPTER_SELECT_STATE_CHANGED, byCurrentAdapterSelectState);
}

/////////////////////////////////////////////////////////////////////////////
BOOL PollPCKeysState()
{
	WORD wCurrentPCKeysState	= 0;
	BOOL  bResult = FALSE;
	static WORD m_wPCKeysState = 0xffff;
	
	// CurrentPCKeys-Status Registerlesen.
	if (readAlteraReg(ALT_PC_KEYS_ADAPT_SEL, wCurrentPCKeysState))
	{
		wCurrentPCKeysState = wCurrentPCKeysState & 0x0f;
		
		if (wCurrentPCKeysState != m_wPCKeysState)
		{										  
			m_wPCKeysState = wCurrentPCKeysState;
						
			bResult = DoNotifyPCKeysStateChanged(wCurrentPCKeysState);
		}
	}
	
	return bResult;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL DoNotifyPCKeysStateChanged(BYTE byCurrentPCKeysState)
{
	return PostMessage(TMM_NOTIFY_PC_KEYS_STATE_CHANGED, byCurrentPCKeysState);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL DoNotifyInformation(DWORD dwParam1, DWORD dwParam2, DWORD dwParam3, DWORD dwParam4)
{
	return PostMessage(TMM_NOTIFY_INFORMATION, dwParam1, dwParam2, dwParam3, dwParam4);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL DoIndicationNewData(DWORD dwSDRAMOffset, int nSPORT)
{
	if (!g_bCaptureOn)
		return TRUE;
	
	return PostMessage(TMM_NEW_CODEC_DATA, nSPORT, dwSDRAMOffset, SDRAM_DMA_BUFFER0_SIZE);
}

//////////////////////////////////////////////////////////////////////////////////
BOOL OnConfirmReceiveNewCodecData()
{
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmInitComplete()
{
	return PostMessage(TMM_CONFIRM_INIT_COMPLETE);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL OnRequestBF535Ping()
{
	return DoConfirmBF535Ping();	
}

///////////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmBF535Ping()
{
	return PostMessage(TMM_CONFIRM_BF535_PING);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL OnRequestTermination()
{
	wFinish = 1;
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmTermination()
{
	// SendMessage anstatt PostMessage, da die Messageloop ja schon terminiert ist.
	return SendMessage(TMM_CONFIRM_TERMINATION);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL InitTimerISR(int nFreq)
{
	BOOL bResult = FALSE;
	int nScale	= 128;
	int nPeriod = CORE_CLK/((nScale+1)*nFreq);
	
	if ((nPeriod > 0) && (nPeriod < 0xffff))
	{
		ex_handler_fn p = register_handler(ik_timer, TimerISR); 
		
		*pTCNTL 	= TMPWR | TAUTORLD;			// timer control register							
		*pTPERIOD 	= nPeriod;					// timer period register
		*pTSCALE 	= nScale;					// timer scale register
		*pTCOUNT 	= nPeriod;					// timer count register
		*pTCNTL 	= TMPWR | TMREN | TAUTORLD;	// enable the timer
		*pIMASK		= SETBIT(*pIMASK, 6);
		
		bResult = TRUE;
	}
	
	return bResult;
}

///////////////////////////////////////////////////////////////////////////////////
void StopTimerISR()
{
	*pTCNTL 	= 0;					// timer control register			
	*pIMASK		= CLRBIT(*pIMASK, 6);
}


///////////////////////////////////////////////////////////////////////////////////
EX_INTERRUPT_HANDLER(TimerISR) 
{ 
//	HandleIncomingMessages();
//	HandleOutgoingMessages();
//	PollAlarm();
} 

///////////////////////////////////////////////////////////////////////////////////
void BlinkLED()
{
	static int nCount = 0;
	static int nCount2= 0;
	double y = (sin(0.0314159 * nCount2)+1)/2;

	if (nCount++ < ((BYTE4) (200 * y)))
		SetLED1();
	else
		ClearLED1();
	
	if (nCount == 200)
	{
		nCount = 0;
		nCount2++;
	}
}

///////////////////////////////////////////////////////////////////////////////////
BOOL SwitchPowerButton(BOOL bState)
{
	WORD wVal 	 = 0;
	BOOL bResult = FALSE;
	
	if (readAlteraReg(ALT_CTRL, wVal))
	{
		if (bState)
			wVal = SETBIT(wVal, ALT_CTRL_SWITSH_POWER_BUTTON_BIT);
		else
			wVal = CLRBIT(wVal, ALT_CTRL_SWITSH_POWER_BUTTON_BIT);
		bResult = writeAlteraReg(ALT_CTRL, wVal);
	}

	return bResult;	
}

///////////////////////////////////////////////////////////////////////////////////
BOOL SwitchResetButton(BOOL bState)
{
	WORD wVal 	 = 0;
	BOOL bResult = FALSE;
	if (readAlteraReg(ALT_CTRL, wVal))
	{
		if (bState)
			wVal = SETBIT(wVal, ALT_CTRL_SWITCH_RESET_BUTTON_BIT);
		else
			wVal = CLRBIT(wVal, ALT_CTRL_SWITCH_RESET_BUTTON_BIT);
		bResult = writeAlteraReg(ALT_CTRL, wVal);
	}

	return bResult;		
}

///////////////////////////////////////////////////////////////////////////
// ACHTUNG: TRACE verwendet derzeit keine Queue, d.h. eine Message wird durch
//          eine weitere überschrieben, wenn die erste nicht vorher ausgelesen wurde!
void TRACE(LPCSTR lpszFormat, ...)
{
	int nLen = 0;
	g_szDebugString[0]=0;
	LPCSTR lpszLocalFormat = lpszFormat;

	va_list args;
	va_start(args, lpszFormat);

	vsprintf(g_szDebugString, lpszLocalFormat, args);
	
	// strlen arbeitet nicht mit global angelegten strings !?
	while((g_szDebugString[nLen] != '\0') && (nLen <MAX_DEBUG_STRING_LEN))
		nLen++;
	
	if (nLen > 0)	
		PostMessage(TMM_NOTIFY_DEBUG_INFORMATION, (DWORD)g_szDebugString, nLen);	
}




