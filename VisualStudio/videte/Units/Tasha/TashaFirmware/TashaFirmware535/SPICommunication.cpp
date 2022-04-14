/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaFirmware535
// FILE:		$Workfile: SPICommunication.cpp $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaFirmware/TashaFirmware535/SPICommunication.cpp $
// CHECKIN:		$Date: 31.01.05 11:53 $
// VERSION:		$Revision: 72 $
// LAST CHANGE:	$Modtime: 31.01.05 10:52 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////
 
#include <CdefBF535.h>
#include <cdefblackfin.h>
#include <sys/exception.h>
#include "..\Include\Helper.h"
#include "..\Include\TMMessages.h"
#include "..\..\TashaUnit\TashaUnitDefs.h"
#include "TashaFirmware535.h"
#include "SPICommunication.h"
#include "Altera.h"
#include "LED.h"

#define MESSAGE_QUEUE_SIZE 	100

SPIPacketStruct	g_SPITransmitPacketQueue[CHANNEL_COUNT][MESSAGE_QUEUE_SIZE] = {0};
WORD			g_wSendSPIReadIndex[CHANNEL_COUNT] 	= {0};
WORD			g_wSendSPIWriteIndex[CHANNEL_COUNT] = {0};

extern BOOL 	g_bBF533Booting;
extern BOOL		g_bEpldDataTransfer;
extern BOOL		g_bEEPromDataTransfer;
extern CInt64	g_i64BF533LastResponseTime[CHANNEL_COUNT];
extern CInt64	g_i64BF533ResponseTimeOut;

///////////////////////////////////////////////////////////////////////////////////
BOOL SendSPIMessage(int nMessageID, DWORD dwUserData, WORD wSlave, DWORD dwParam1/*=0*/, DWORD dwParam2/*=0*/)
{	
	BOOL bResult = FALSE;
	
	// Aktuellen Schreibindex für diesen Channel holen.	
	WORD wWriteIndex = g_wSendSPIWriteIndex[wSlave];
	
	// Noch Platz in der Messagequeue?
	if (g_SPITransmitPacketQueue[wSlave][wWriteIndex].nPending == 0)
	{
		// Message in die SendMessagequeue hängen
		g_SPITransmitPacketQueue[wSlave][wWriteIndex].nPending 	= 100;			// 100*5ms = 500ms Timeout	
		g_SPITransmitPacketQueue[wSlave][wWriteIndex].nMessageID= nMessageID;
		g_SPITransmitPacketQueue[wSlave][wWriteIndex].dwUserData= dwUserData;
		g_SPITransmitPacketQueue[wSlave][wWriteIndex].wSlave	= wSlave;
		g_SPITransmitPacketQueue[wSlave][wWriteIndex].dwParam1	= dwParam1;
		g_SPITransmitPacketQueue[wSlave][wWriteIndex].dwParam2	= dwParam2;
		g_SPITransmitPacketQueue[wSlave][wWriteIndex].dwCheckSum= GetCheckSum(g_SPITransmitPacketQueue[wSlave][wWriteIndex]);
		// Schreibindex zyklisch weiterzählen.
		wWriteIndex++;
		if (wWriteIndex == MESSAGE_QUEUE_SIZE)
			wWriteIndex = 0;
		g_wSendSPIWriteIndex[wSlave] = wWriteIndex;	
		
		bResult = TRUE;
	}
	else
		DoNotifyInformation(INFO_SEND_SPI_COMMAND_PENDING, nMessageID, wSlave, g_SPITransmitPacketQueue[wSlave][wWriteIndex].nPending); 
	
	return bResult;	
}
///////////////////////////////////////////////////////////////////////////////////
void HandleSPICommunication()
{ 
	BOOL bResult = FALSE;
	BOOL bErr	 = FALSE;	
	static WORD 	wSlave  = 0;
	static CInt64	i64LastPingTime[CHANNEL_COUNT] 			= {GetCycleCounter()};
	static CInt64	i64LastCommunicationTime[CHANNEL_COUNT] = {GetCycleCounter()};
	 
	// Solange einer der BF533 bootet, darf der SPI1 nicht angefaßt werden.
	if (!g_bBF533Booting && !g_bEpldDataTransfer && !g_bEEPromDataTransfer && IsAlteraValid())
	{		
		// Zwischen zwei SPI Kommunikationen soll mindestens 5000ys verstreichen.
		if ((GetCycleCounter() - i64LastCommunicationTime[wSlave]) < (CInt64)(CORE_CLK/1000000*5000))
			return;
		
		i64LastCommunicationTime[wSlave] = GetCycleCounter();
			
		
		SPIPacketStruct SPIReceivePacket  = {0};
	
		WORD wReadIndex = g_wSendSPIReadIndex[wSlave];
		
		// Liegt eine Message vor?
		if (g_SPITransmitPacketQueue[wSlave][wReadIndex].nPending > 0)
		{
			// Zeitpunkt der letzten SPI-Kommandos merken.
			i64LastPingTime[wSlave] = GetCycleCounter();

			// Notwendige Initialisierungen durchführen.
			InitSPI();
	
			// SPI1 aktivieren.
			EnableSPI();

			WORD* pTransmitBuffer 	= (WORD*)&g_SPITransmitPacketQueue[wSlave][wReadIndex];
			WORD* pReceiveBuffer  	= (WORD*)&SPIReceivePacket;
			*pSPI1_ST 				= 0xff;
		
			// Message an den Slave verschicken und Antwort auswerten..
			for (int nI = 0; (nI < sizeof(SPIPacketStruct)/sizeof(WORD)) && !bErr; nI++)
			{					
				SelectSlave(wSlave);
				WaitMicro(1);
				
				*pSPI1_TDBR = pTransmitBuffer[nI];
				if (WaitUntilSPITransferComplete(10))
					pReceiveBuffer[nI] = *pSPI1_RDBR;
				else
					bErr = TRUE;
				
				WaitMicro(1);	// Veringert Prüfsummenfehler beim Ping!?
				SelectSlave(-1);
			}

			// Wenn es sich um die Message 'TMM_REQUEST_SET_PERMANENT_MASK' handelt, folgen nun die
			// eigentlichen Daten der permanenten Maske
			if (!bErr && g_SPITransmitPacketQueue[wSlave][wReadIndex].nMessageID == TMM_REQUEST_SET_PERMANENT_MASK)
			{
				WORD* pTransmitBuffer 	= (WORD*)g_SPITransmitPacketQueue[wSlave][wReadIndex].dwParam1;
				DWORD dwLen				= g_SPITransmitPacketQueue[wSlave][wReadIndex].dwParam2;
				BYTE  byDummy			= 0;		
				*pSPI1_ST 				= 0xff;
				
				WaitMicro(100);
				
				SelectSlave(wSlave);
				// Permanente Maske an den Slave schicken..
				for (int nI = 0; (nI < dwLen/sizeof(WORD)) && !bErr; nI++)
				{							
					*pSPI1_TDBR = pTransmitBuffer[nI];
					if (WaitUntilSPITransferComplete(10))
						byDummy = *pSPI1_RDBR;
					else
						bErr = TRUE;
				}			
				SelectSlave(-1);
			}
			
			// Ist ein Übertragungsfehler aufgetreten?
			bResult = !bErr && (!(*pSPI1_ST & TXCOL) && !(*pSPI1_ST & RBSY) && !(*pSPI1_ST & TXE) && !(*pSPI1_ST & MODF));

			DisableSPI();
			
			// Schaue nach ob ein gültiges Confirm empfangen wure.
			if (bResult)		
				bResult = CheckIncommingData(SPIReceivePacket, wSlave);
		
			if (bResult)
			{
				// Slave antwortet noch.
				g_i64BF533LastResponseTime[wSlave] = GetCycleCounter();
				
				// Ok, Confirm erhalten...Request ist nun abgeschlossen.
				g_SPITransmitPacketQueue[wSlave][wReadIndex].nPending = 0;		
			}
			else		
			{
				// Pendingcounter herunterzählen
				g_SPITransmitPacketQueue[wSlave][wReadIndex].nPending--;
				
				// Lange genug auf Antwort gewartet?
				if (g_SPITransmitPacketQueue[wSlave][wReadIndex].nPending == 0)
				{				
					DoNotifyInformation(INFO_SEND_SPI_COMMAND_TIMEOUT, g_SPITransmitPacketQueue[wSlave][wReadIndex].nMessageID,
																	   g_SPITransmitPacketQueue[wSlave][wReadIndex].wSlave); 
					// Slave hat mehrfach kein Confirm gesendet. Dies muß der TashaUnit mitgeteilt werden, damit
					// diese Maßnahmen ergreifen kann.
					if ((GetCycleCounter() - g_i64BF533LastResponseTime[wSlave]) > CInt64(CORE_CLK/1000000*g_i64BF533ResponseTimeOut))
					{
						g_i64BF533LastResponseTime[wSlave] = GetCycleCounter();
						TRACE("#### NO BF533 RESPONSE ####\n");
						DoNotifyInformation(INFO_NO_BF533_RESPONSE, wSlave); 					
					}
				}
			}
			 
			// Die Message ist abgehandelt
			if (g_SPITransmitPacketQueue[wSlave][wReadIndex].nPending == 0)
			{
				// Leseposition weitersetzen
				wReadIndex++;
				if (wReadIndex == MESSAGE_QUEUE_SIZE)
					wReadIndex = 0;
			
				// Leseindex sichern
				g_wSendSPIReadIndex[wSlave] = wReadIndex;
			}
		
		}
		else // Nein, es liegt keine Message vor, dann...
		{
			// ... sollen alle Slaves regelmäßig angepingt werden (alle 100ms)
			if ((GetCycleCounter() - i64LastPingTime[wSlave]) > CInt64(CORE_CLK/1000000*100*1000))
				SendSPIMessage(TMM_REQUEST_BF533_PING, 0, wSlave);
		}
		
		// Alle Slaves sequentiell abprüfen
		wSlave++;
		if (wSlave == CHANNEL_COUNT)
			wSlave=0;
	}
	else
	{
		// Erst wenn die SPI-Kommunikation aktiviert ist, sollen die Slaves überwacht werden.
		for (int nI = 0; nI < CHANNEL_COUNT; nI++)
			g_i64BF533LastResponseTime[nI] = GetCycleCounter();		
	}
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CheckIncommingData(SPIPacketStruct& SPIReceivePacket, WORD wSlave2)
{
	BOOL	bResult 		= FALSE;
	int nMessageID 	= SPIReceivePacket.nMessageID;
	DWORD dwUserData= SPIReceivePacket.dwUserData;
	WORD  wSlave	= SPIReceivePacket.wSlave;
	DWORD dwParam1	= SPIReceivePacket.dwParam1;
	DWORD dwParam2	= SPIReceivePacket.dwParam2;
	BOOL 	bValidCheckSum 	= (GetCheckSum(SPIReceivePacket) == SPIReceivePacket.dwCheckSum);
	static DWORD dwCounter = 0;
	
	if (bValidCheckSum)
	{
		switch (nMessageID)
		{
			case TMM_CONFIRM_SET_ENCODER_PARAMETER:
				bResult = DoConfirmSetEncoderParam(dwUserData, wSlave, dwParam1);
				break;		
			case TMM_CONFIRM_SET_BRIGHTNESS:
				bResult = DoConfirmSetBrightness(dwUserData, wSlave, dwParam1);
				break;		
			case TMM_CONFIRM_SET_CONTRAST:
				bResult = DoConfirmSetContrast(dwUserData, wSlave, dwParam1);
				break;
			case TMM_CONFIRM_SET_SATURATION:
				bResult = DoConfirmSetSaturation(dwUserData, wSlave, dwParam1);
				break;
			case TMM_CONFIRM_NEW_FRAME:
				bResult = DoConfirmNewFrame(dwUserData, wSlave);
				break;
			case TMM_CONFIRM_GET_ENCODER_PARAMETER:
				bResult = DoConfirmGetEncoderParam(dwUserData, wSlave, dwParam1);
				break;		
			case TMM_CONFIRM_GET_BRIGHTNESS:
				bResult = DoConfirmGetBrightness(dwUserData, wSlave, dwParam1);
				break;
			case TMM_CONFIRM_GET_CONTRAST:
				bResult = DoConfirmGetContrast(dwUserData, wSlave, dwParam1);
				break;
			case TMM_CONFIRM_GET_SATURATION:
				bResult = DoConfirmGetSaturation(dwUserData, wSlave, dwParam1);
				break;
			case TMM_CONFIRM_SET_THRESHOLD_MD:
				bResult = DoConfirmSetMDTreshold(dwUserData, wSlave, dwParam1);
				break;		
			case TMM_CONFIRM_SET_THRESHOLD_MASK:
				bResult = DoConfirmSetMaskTreshold(dwUserData, wSlave, dwParam1);
				break;		
			case TMM_CONFIRM_SET_INCREMENT_MASK:
				bResult = DoConfirmSetMaskIncrement(dwUserData, wSlave, dwParam1);
				break;		
			case TMM_CONFIRM_SET_DELAY_MASK:
				bResult = DoConfirmSetMaskDelay(dwUserData, wSlave, dwParam1);
				break;		
			case TMM_CONFIRM_GET_THRESHOLD_MD:
				bResult = DoConfirmGetMDTreshold(dwUserData, wSlave, dwParam1);
				break;		
			case TMM_CONFIRM_GET_THRESHOLD_MASK:
				bResult = DoConfirmGetMaskTreshold(dwUserData, wSlave, dwParam1);
				break;		
			case TMM_CONFIRM_GET_INCREMENT_MASK:
				bResult = DoConfirmGetMaskIncrement(dwUserData, wSlave, dwParam1);
				break;		
			case TMM_CONFIRM_GET_DELAY_MASK:
				bResult = DoConfirmGetMaskDelay(dwUserData, wSlave, dwParam1);
				break;							
			case TMM_CONFIRM_SET_PERMANENT_MASK:
				bResult = DoConfirmSetPermanentMask(dwUserData, wSlave, dwParam1, dwParam2);
				break;
			case TMM_CONFIRM_CHANGE_PERMANENT_MASK:
				bResult = DoConfirmChangePermanentMask(dwUserData, wSlave, dwParam1, dwParam2);
				break;
			case TMM_CONFIRM_CLEAR_PERMANENT_MASK:
				bResult = DoConfirmClearPermanentMask(dwUserData, wSlave);
				break;							
			case TMM_CONFIRM_INVERT_PERMANENT_MASK:
				bResult = DoConfirmInvertPermanentMask(dwUserData, wSlave);
				break;							
			case TMM_CONFIRM_SET_CHANNEL:
				bResult = DoConfirmSetChannel(wSlave, dwParam1);
				break;
			case TMM_CONFIRM_SET_NR:
				bResult = DoConfirmSetNR(dwUserData, wSlave, dwParam1);
				break;
			case TMM_CONFIRM_GET_NR:
				bResult = DoConfirmGetNR(dwUserData, wSlave, dwParam1);
				break;
			case TMM_CONFIRM_BF533_PING:
				bResult = TRUE;
				break;
			case TMM_NOTIFY_INFORMATION:
				bResult = DoNotifyInformation(dwParam1, wSlave);
				break;
			case TMM_NOTIFY_SLAVE_INIT_READY:
			{
				dwCounter++;
				bResult = DoNotifySlaveInitComplete(wSlave2, wSlave, dwCounter);
//				bResult = DoNotifySlaveInitComplete(wSlave, wSlave2, dwCounter);
				break;
			}
		}
	}
	else
		DoNotifyInformation(INFO_SPI_RECEIVE_CHECKSUM_FAILED, wSlave, GetCheckSum(SPIReceivePacket), SPIReceivePacket.dwCheckSum);
		
	return bResult;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL InitSPI()
{
	BOOL bResult = FALSE;

	// Baudraten der beiden SPIs einstellen	
	*pSPI1_BAUD = SYSTEM_CLK/(2*SPI1_COMMUNICATION_BAUDRATE);	
	
	// DisableSPI, Master, 16Bit, Disable MISO, SlaveSelect disable, SendZero, Transmit
	*pSPI1_CTL = EMISO | MSTR | SIZE | SZ | 0x01;
	
	return TRUE;	
}

///////////////////////////////////////////////////////////////////////////////////
void EnableSPI()
{
	*pSPI1_CTL |= SPE;	
}

///////////////////////////////////////////////////////////////////////////////////
void DisableSPI()
{
	*pSPI1_CTL &= ~SPE;	
}

///////////////////////////////////////////////////////////////////////////////////
void SelectSlave(int nSlave)
{
	if (nSlave == -1)
		writeAlteraReg(ALT_SLAVE_SELECT, 0);
	else
		writeAlteraReg(ALT_SLAVE_SELECT, SETBIT(0, nSlave));
}

///////////////////////////////////////////////////////////////////////////////////
BOOL WaitUntilSPITransferComplete(int nTimeOut)
{
	int nCounter = 0;
	nTimeOut *= 1000;		// Es wird jeweils 1ys gewartet.
//	while(!(*pSPI1_ST & SPIF) && (nCounter++ < nTimeOut))
	while(!(*pSPI1_ST & RXS) && (nCounter++ < nTimeOut))
		WaitMicro(1);

//	return (*pSPI1_ST & SPIF);
	return (*pSPI1_ST & RXS);
}

///////////////////////////////////////////////////////////////////////////////////
DWORD GetCheckSum(SPIPacketStruct SPIPacket)
{
	// Checksumme und Pendingcoounter dürfen nicht mit in die Prüfsumme einfließen
	SPIPacket.dwCheckSum = 0;
	SPIPacket.nPending	 = 0;
	DWORD dwCheckSum = 0;
	BYTE* pData  	= (BYTE*)&SPIPacket;

	// Message an den Slave verschicken und Antwort auswerten..
	for (int nI = 0; nI < sizeof(SPIPacketStruct)/sizeof(BYTE); nI++)
		dwCheckSum += nI*pData[nI];			
	
	return dwCheckSum;
}

