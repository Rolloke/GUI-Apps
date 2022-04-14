#include <gpiotest.h>
#include <sys/exception.h>
#include <cdefblackfin.h>
#include <stdio.h>
#include <CdefBF535.h>
#include <time.h>
#include "..\Include\Helper.h"
#include "SharedMemoryDef.h"
#include "..\Include\TMMessages.h"
#include "BasicMon.h"
#include "TashaFirmware535.h"
#include "SPICommunication.h"
#include "Sports.h"
#include "MemoryDMA.h"
#include "Altera.h"
#include "Boot533.h"
#include "EEProm.h"

DWORD 	g_dwRelayStateMask	= 0;
DWORD 	g_dwProzessID		= 0;
BOOL 	g_bCaptureOn		= FALSE;
BOOL 	g_bBF533Booting		= FALSE;

DWORD	g_dwPCIFrameBuffer	= 0;

#define MESSAGE_QUEUE_SIZE	100
MessageStruct 	g_SendMessageQueue[MESSAGE_QUEUE_SIZE] = {0};
WORD			g_wSendReadIndex = 0;
WORD			g_wSendWriteIndex= 0;

MessageStruct 	g_ReceiveMessageQueue[MESSAGE_QUEUE_SIZE] = {0};
WORD			g_wReceiveReadIndex = 0;
WORD			g_wReceiveWriteIndex= 0;

WORD			g_wSendMessageQueueLevel = 0; // Gibt Auskunft darüber wieviele Elemente in der Queue sind
BOOL			g_bSportInit = FALSE;

section ("SPIBuffer") volatile BYTE SPIBuffer[1024];

/////////////////////////////////////////////////////////////////////////
void main()
{
	*pSIC_IMASK		= 0x0000;

	// SRAM Initialisierung
	*pEBIU_SDRRC 	= 0x0000074A;
	*pEBIU_SDBCTL 	= 0x00000001;
	*pEBIU_SDGCTL 	= 0x0091998F;

	wFinish	  		= 0;
		
	// PF10 aktivieen
	int nVal = *(volatile uint16 *)SPI0_FLG;
	nVal = CLRBIT(nVal, FLS5_P);
	*(volatile uint16 *)SPI0_FLG = nVal;

	initLEDs();
	clearLEDs();
	
	// BasicMon initialisieren...
	InitBasicMon();	
	SetInterruptCallback((void*)MessageCallback);

//	InitTimerISR(800);
	
	// TashaUnit mitteilen, daß Initialisierung komplett.
	DoConfirmInitComplete();

	// Main MessageLoop
	do
	{
#ifndef USE_TIMER
		HandleSPICommunication();
#endif
		HandleIncomingMessages();
		HandleOutgoingMessages();
		PollAlarm();
	
	}while (!wFinish);

	// TashaUnit mitteilen, das Firmware beendet wurde.
	DoConfirmTermination();

//	StopTimerISR();
	
	// SPORT Schließen
	CloseSPORTs();
}

/////////////////////////////////////////////////////////////////////////
void MessageCallback(void)
{
	int		nMessageID	= 0;
	DWORD	dwParam1	= 0;
	DWORD	dwParam2	= 0;
	DWORD	dwParam3	= 0;
	DWORD	dwParam4	= 0;
	WORD	wWriteIndex	= 0;

	SAVE_USER_REGS;

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
	nMessageID = PC2DSPMessageBuffer.nMessageID;
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
			case TMM_REQUEST_SET_BRIGHTNESS:
				OnRequestSetBrightness(dwParam1, (WORD)dwParam2, (int)dwParam3);
				break;
			case TMM_REQUEST_SET_CONTRAST:
				OnRequestSetContrast(dwParam1, (WORD)dwParam2, (int)dwParam3);
				break;
			case TMM_REQUEST_SET_SATURATION:
				OnRequestSetSaturation(dwParam1, (WORD)dwParam2, (int)dwParam3);
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
			case TMM_REQUEST_TERMINATION:
				OnRequestTermination();
				break;
			case TMM_REQUEST_SET_FRAMEBUFF_ADDR:
				OnRequestSetPCIFrameBufferAddress(dwParam1);
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
//		while(!DSP2PCMessageBuffer.bAck); // && (dwCounter++ < 10000));
		while(!DSP2PCMessageBuffer.bAck && (dwCounter++ < 10000))
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
	
	// Das Indexregister darf nur jeweils aus einem Thread, bzw ISR heraus zugegriffen werden.
	while(bMutex){};
	bMutex = TRUE;
	
	BOOL bResult = FALSE;
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
	
	return bResult;	
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
	//return DoConfirmSetMaskDelay(dwUserData, wSlave, nValue);
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
	if (bOpenClose)
		g_dwRelayStateMask = SETBIT(g_dwRelayStateMask, wRelayID);
	else
		g_dwRelayStateMask = CLRBIT(g_dwRelayStateMask, wRelayID);

	return DoConfirmSetRelayState(wRelayID, bOpenClose);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmSetRelayState(WORD wRelayID, BOOL bOpenClose)
{
	return PostMessage(TMM_CONFIRM_SET_RELAIS_STATE, wRelayID, bOpenClose);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL OnRequestGetRelayState()
{
	return DoConfirmGetRelayState(g_dwRelayStateMask);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmGetRelayState(DWORD dwRelayStateMask)
{
	return PostMessage(TMM_CONFIRM_GET_RELAIS_STATE, dwRelayStateMask);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL OnRequestGetAlarmState()
{
	DWORD dwAlarmState = 0; 

	return DoConfirmGetAlarmState(dwAlarmState);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL DoConfirmGetAlarmState(DWORD dwAlarmStateMask)
{
	return PostMessage(TMM_CONFIRM_GET_ALARM_STATE, dwAlarmStateMask);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL OnRequestSetAlarmEdge(DWORD dwAlarmEdgeMask)
{
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
	DWORD dwAlarmEdgeMask = 0; 
	
	return DoConfirmGetAlarmEdge(dwAlarmEdgeMask);
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
BOOL DoNotifySlaveInitComplete(WORD wSlave)
{
	return PostMessage(TMM_NOTIFY_SLAVE_INIT_READY, wSlave);
}

/////////////////////////////////////////////////////////////////////////////
BOOL OnRequestEpldDataTransfer(DWORD dwAddr, DWORD dwLen)
{
	BOOL  bResult 		= FALSE;
	DWORD dwPCIBuffer   = dwAddr;
	DWORD dwPCI_MBAP	= *(BYTE*)pPCI_MBAP;	// Sichern des PCI_MBAP-Registers
	
	*pPCI_MBAP  = (void*)(dwPCIBuffer & 0xf8000000);
	dwPCIBuffer = (dwPCIBuffer  & 0x07ffffff) | 0xE0000000;

	if (progAlteraInit())
	{
		if (progAlteraStart())
			bResult = progAlteraTX((BYTE*)dwPCIBuffer, dwLen);
	}

	// PCI_MBAP-Register wieder herstellen.	
	*pPCI_MBAP  = (void*)dwPCI_MBAP;

	if (bResult)
		return DoConfirmEpldDataTransfer(dwAddr, dwLen);
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
BOOL OnRequestBoot533(int nBFNr, DWORD dwAddr, DWORD dwLen)
{
	// Während des Bootens darf der SPI1 nicht anderweitig verwendet werden.
	// Außerdem darf das PCI_MBAP-Register nicht verändert werden. g_bBF533Booting
	// stellt dieses sicher.
	g_bBF533Booting		= TRUE;
	WaitMicro(1000);
	
	BOOL  bResult 		= FALSE;
	DWORD dwPCIBuffer   = dwAddr;
	DWORD dwPCI_MBAP	= *(BYTE*)pPCI_MBAP;	// Sichern des PCI_MBAP-Registers

	*pPCI_MBAP  = (void*)(dwPCIBuffer & 0xf8000000);
	dwPCIBuffer = (dwPCIBuffer  & 0x07ffffff) | 0xE0000000;

	bResult = Boot533(nBFNr, (BYTE*)dwPCIBuffer, dwLen);

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
	
	*pPCI_MBAP  = (void*)(dwPCIBuffer & 0xf8000000);
	dwPCIBuffer = (dwPCIBuffer  & 0x07ffffff) | 0xE0000000;

	if (ProgrammEEPromInit())
		bResult = ReadFromEEPromX((BYTE*)dwPCIBuffer, dwLen, dwStartAddr);

	// PCI_MBAP-Register wieder herstellen.	
	*pPCI_MBAP  = (void*)dwPCI_MBAP;

	if (bResult)
		return DoConfirmReadFromEEProm(dwAddr, dwLen, dwStartAddr);
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
	DWORD dwPCIBuffer   = dwAddr;
	DWORD dwPCI_MBAP	= *(BYTE*)pPCI_MBAP;	// Sichern des PCI_MBAP-Registers
	
	*pPCI_MBAP  = (void*)(dwPCIBuffer & 0xf8000000);
	dwPCIBuffer = (dwPCIBuffer  & 0x07ffffff) | 0xE0000000;

	bResult = SendSPIMessage(TMM_REQUEST_SET_PERMANENT_MASK, dwUserData, wSlave, dwPCIBuffer, dwLen);

	// PCI_MBAP-Register wieder herstellen.	
	*pPCI_MBAP  = (void*)dwPCI_MBAP;
	
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
BOOL PollAlarm()
{
	DWORD dwCurrentAlarmState	= 0;
	BOOL  bResult = FALSE;
	static DWORD m_dwAlarmState = 0;
	
	// CurrentAlarmstatusregister lesen.
	dwCurrentAlarmState = 0; 

	if (dwCurrentAlarmState != m_dwAlarmState)
	{										  
		m_dwAlarmState = dwCurrentAlarmState;
					
		bResult = DoNotifyAlarm(dwCurrentAlarmState);
	}

	return bResult;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL DoNotifyAlarm(DWORD dwAlarmState)
{
	return PostMessage(TMM_NOTIFY_ALARM, dwAlarmState);
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
void Delay(DWORD dwEnd)
{
	DWORD dwDelay = 0;
	
	for (dwDelay= 0; dwDelay< dwEnd; dwDelay++){}
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


