#include <CdefBF535.h>
#include <cdefblackfin.h>
#include <sys/exception.h>
#include "..\Include\Helper.h"
#include "..\Include\TMMessages.h"
#include "..\..\TashaUnit\TashaUnitDefs.h"
#include "TashaFirmware535.h"
#include "SPICommunication.h"

#define MESSAGE_QUEUE_SIZE 	32

SPIPacketStruct	g_SPITransmitPacketQueue[CHANNEL_COUNT][MESSAGE_QUEUE_SIZE] = {0};
WORD			g_wSendSPIReadIndex[CHANNEL_COUNT] 	= {0};
WORD			g_wSendSPIWriteIndex[CHANNEL_COUNT] = {0};

extern BOOL  g_bBF533Booting;
#define PF13 0x2000

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
		g_SPITransmitPacketQueue[wSlave][wWriteIndex].nPending 			= 100;			// 100x100ys = 10ms Timeout	
		g_SPITransmitPacketQueue[wSlave][wWriteIndex].nMessageID		= nMessageID;
		g_SPITransmitPacketQueue[wSlave][wWriteIndex].dwUserData		= dwUserData;
		g_SPITransmitPacketQueue[wSlave][wWriteIndex].wSlave			= wSlave;
		g_SPITransmitPacketQueue[wSlave][wWriteIndex].dwParam1			= dwParam1;
		g_SPITransmitPacketQueue[wSlave][wWriteIndex].dwParam2			= dwParam2;

		// Schreibindex zyklisch weiterzählen.
		wWriteIndex++;
		if (wWriteIndex == MESSAGE_QUEUE_SIZE)
			wWriteIndex = 0;
		g_wSendSPIWriteIndex[wSlave] = wWriteIndex;	
		
		bResult = TRUE;
	}
	else
		DoNotifyInformation(INFO_SEND_SPI_COMMAND_PENDING, nMessageID, wSlave); 

	return bResult;	
}

///////////////////////////////////////////////////////////////////////////////////
void HandleSPICommunication()
{ 
	BOOL bResult = FALSE;
	BOOL bErr	 = FALSE;	
	static DWORD dwCounter = 0;
	static WORD wSlave  = 0;

	// Solange einer der BF533 bootet, darf der SPI1 nicht angefaßt werden.
	if (!g_bBF533Booting)
	{	
		WaitMicro(100);

		// Alle Slaves regelmäßig anpingen
		if (dwCounter++ >= 2500)
		{
			dwCounter = 0;	
	//		for (int nSlave = 0; nSlave < CHANNEL_COUNT; nSlave++)
	//			SendSPIMessage(TMM_REQUEST_BF533_PING, 0, nSlave);
			SendSPIMessage(TMM_REQUEST_BF533_PING, 0, 3);
		}
		
		SPIPacketStruct SPIReceivePacket  = {0};
	
		WORD wReadIndex = g_wSendSPIReadIndex[wSlave];
		
		// Liegt eine Message vor?
		if (g_SPITransmitPacketQueue[wSlave][wReadIndex].nPending > 0)
		{
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
				*pSPI1_TDBR = pTransmitBuffer[nI];
				if (WaitUntilSPITransferComplete(10))
					pReceiveBuffer[nI] = *pSPI1_RDBR;
				else
					bErr = TRUE;
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
				
				WaitMicro(1);
				// Permanente Maske an den Slave schicken..
				for (int nI = 0; (nI < dwLen/sizeof(WORD)) && !bErr; nI++)
				{			
					SelectSlave(wSlave);
					*pSPI1_TDBR = pTransmitBuffer[nI];
					if (WaitUntilSPITransferComplete(10))
						byDummy = *pSPI1_RDBR;
					else
						bErr = TRUE;
					SelectSlave(-1);
				}
				
			}
			
			// Ist ein Übertragungsfehler aufgetreten?
			bResult = !bErr && (!(*pSPI1_ST & TXCOL) && !(*pSPI1_ST & RBSY) && !(*pSPI1_ST & TXE) && !(*pSPI1_ST & MODF));

			DisableSPI();
			
			// Schaue nach ob ein gültiges Confirm empfangen wure.
			if (bResult)		
				bResult = CheckIncommingData(SPIReceivePacket);
		
			if (bResult)
			{
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
																	   g_SPITransmitPacketQueue[wSlave][wReadIndex].dwParam1,
																	   g_SPITransmitPacketQueue[wSlave][wReadIndex].wSlave); 
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

		// Alle Slaves sequentiell abprüfen
		wSlave++;
		if (wSlave == CHANNEL_COUNT)
			wSlave=0;
	}
	
	// Clear IRQ
	*pTIMER1_STATUS = IRQ1 | OVF_ERR1;		
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CheckIncommingData(SPIPacketStruct& SPIReceivePacket)
{
	BOOL	bResult 		= FALSE;

	int nMessageID 	= SPIReceivePacket.nMessageID;
	DWORD dwUserData= SPIReceivePacket.dwUserData;
	WORD  wSlave	= SPIReceivePacket.wSlave;
	DWORD dwParam1	= SPIReceivePacket.dwParam1;
	DWORD dwParam2	= SPIReceivePacket.dwParam2;
	
	switch (nMessageID)
	{
		case TMM_CONFIRM_SET_BRIGHTNESS:
			bResult = DoConfirmSetBrightness(dwUserData, wSlave, dwParam1);
			break;		
		case TMM_CONFIRM_SET_CONTRAST:
			bResult = DoConfirmSetContrast(dwUserData, wSlave, dwParam1);
			break;
		case TMM_CONFIRM_SET_SATURATION:
			bResult = DoConfirmSetSaturation(dwUserData, wSlave, dwParam1);
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
		case TMM_CONFIRM_BF533_PING:
			bResult = TRUE;
			break;
		case TMM_NOTIFY_INFORMATION:
			bResult = DoNotifyInformation(dwParam1, wSlave);
			break;
		case TMM_NOTIFY_SLAVE_INIT_READY:
			bResult = DoNotifySlaveInitComplete(wSlave);
			break;
	
		}
	
	return bResult;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL InitSPI()
{
	BOOL bResult = FALSE;

	*pFIO_DIR 	 = *pFIO_DIR | PF13;			// PF13 auf Ausgang
	*pFIO_FLAG_S = PF13;						// PF13 auf '1'

	// Baudraten der beiden SPIs einstellen	
	*pSPI1_BAUD = SYSTEM_CLK/(2*SPI1_COMMUNICATION_BAUDRATE);	
	
	// DisableSPI, Master, 16Bit, Disable MISO, SlaveSelect disable, SendZero, Transmit
	*pSPI1_CTL = MSTR | SIZE | SZ | 0x01;
	
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
	// ToDo: Die Auswahl des Slaves erfolgt später nicht über die PF-Pins, sondern über
	// den Altera!
	if (nSlave == -1)
		*pFIO_FLAG_S  =  PF13;
	else
		*pFIO_FLAG_C  =  PF13;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL WaitUntilSPITransferComplete(int nTimeOut)
{
	int nCounter = 0;
	nTimeOut *= 1000;		// Es wird jeweils 1ys gewartet.
	while(!(*pSPI1_ST & SPIF) && (nCounter++ < nTimeOut))
		WaitMicro(1);

	return (*pSPI1_ST & SPIF);
}

