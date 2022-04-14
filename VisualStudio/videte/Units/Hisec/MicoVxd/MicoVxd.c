/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright 1993-95  Microsoft Corporation.  All Rights Reserved.           *
*                                                                           *
****************************************************************************/

#define WANTVXDWRAPS
#include <basedef.h>		    
#include <vmm.h>
#include <vwin32.h>
#include <debug.h>
#include <vxdwraps.h>
#include <winerror.h>
#include <shell.h>
#include <vpicd.h>
#include <vtd.h>
#include "micovxd.h"  
#include "micoreg.h"
			  
#pragma VxD_LOCKED_CODE_SEG
#pragma VxD_LOCKED_DATA_SEG
#pragma warning( disable : 4761 )

// Einige globale Variablen
BOOL	bGlobalSwitchAnalogVideo	= FALSE;
WORD	wGlobalSkipFieldCnt			= 0;
DWORD	dwGlobalFieldID				= 0;
HIRQ	hGlobalIrq					= 0;
VID		vidDescriptor				= {0};

// Virtuelle Adressen des Konfigurationsstruktur und der Datenbuffer
CONFIG*	pVirtualConf;
char*	pVirtualBuffer[MAX_JPEG_BUFFER] = {0};


DWORD ( _stdcall *Mico_VxD_W32_Proc[] )(DWORD, DWORD, LPDIOC) = {
		MICO_VXD_W32_Open,
		MICO_VXD_W32_Close,
		MICO_VXD_W32_GetVersion};

#define MAX_MICO_VXD_W32_API (sizeof(Mico_VxD_W32_Proc)/sizeof(DWORD))

/////////////////////////////////////////////////////////////////////////////////////
DWORD _stdcall MICO_VXD_W32_DeviceIOControl(DWORD  dwService, DWORD  dwDDB, DWORD  hDevice, LPDIOC lpDIOCParms)
{
    DWORD dwRetVal = 0;

    // DIOC_OPEN is sent when VxD is loaded w/ CreateFile (this happens just after SYS_DYNAMIC_INIT) 
    if ( dwService == DIOC_OPEN )
    {
		_Debug_Printf_Service("MICO_VXD: Open Driver Version 0x%x\n\r", MICO_VXD_VERSION);
        
        dwRetVal = 0; // Must return 0 to tell WIN32 that this VxD supports DEVIOCTL
    }
    // DIOC_CLOSEHANDLE is sent when VxD is unloaded w/ CloseHandle	(this happens just before SYS_DYNAMIC_EXIT)
    else if ( dwService == DIOC_CLOSEHANDLE )
    {
        Out_Debug_String("MICO_VXD: Close Driver\n\r");

        // Dispatch to cleanup proc
        dwRetVal = MICO_VXD_CleanUp();
    }
    else if ( dwService > MAX_MICO_VXD_W32_API )
    {
        // Returning a positive value will cause the WIN32 DeviceIOControl
        // call to return FALSE, the error code can then be retrieved
        // via the WIN32 GetLastError
        Out_Debug_String("MICO_VXD: WARNING: Unbekannter Service!\n\r");
		dwRetVal = ERROR_NOT_SUPPORTED;
    }
    else
    {
        // CALL requested service
        dwRetVal = (Mico_VxD_W32_Proc[dwService-1])(dwDDB, hDevice, lpDIOCParms);
    }

    return(dwRetVal);
}

/////////////////////////////////////////////////////////////////////////////////////
DWORD _stdcall MICO_VXD_W32_Open(DWORD dwDDB, DWORD hDevice, LPDIOC lpDIOCParms)
{
	int nI;

	Out_Debug_String("MICO_VXD: Initialisation...\n\r");

	if (!lpDIOCParms)
	{
		Out_Debug_String("MICO_VXD: MICO_VXD_W32_Open WARNING: lpDIOCParms = NULL!\n\r");
		return E_POINTER;
	}

	// Adresse der Config-Struktur virtualisieren
	pVirtualConf = (CONFIG*)VirtualizeAdress((void*)lpDIOCParms->lpvInBuffer, sizeof(CONFIG));
	if (!pVirtualConf)
	{
        Out_Debug_String("MICO_VXD: WARNING: VirtualizeAdress failed!\n\r");
		return E_POINTER;
	}

	// Adresse der JPEG-Buffer virtualisieren
	for (nI = 0; nI < MAX_JPEG_BUFFER; nI++)
	{	 
		pVirtualBuffer[nI] = (char*)VirtualizeAdress((void*)&pVirtualConf->Jpeg[nI].lpBuffer[0], JPEG_BUFFER_SIZE);
		if (!pVirtualBuffer[nI])
		{
			Out_Debug_String("MICO_VXD: WARNING: VirtualizeAdress failed!\n\r");
			return E_POINTER;
		}					   
	}

	if (!InstallISR(pVirtualConf->wIRQ))
	{
        Out_Debug_String("MICO_VXD: WARNING: ISR konnte nicht installiert werden!\n\r");
		return ERROR_IRQ_BUSY;
	}

	return NO_ERROR;
}

/////////////////////////////////////////////////////////////////////////////////////
DWORD _stdcall MICO_VXD_W32_Close(DWORD dwDDB, DWORD hDevice, LPDIOC lpDIOCParms)
{
	MICO_VXD_CleanUp();

	return(NO_ERROR);
}

/////////////////////////////////////////////////////////////////////////////////////
DWORD _stdcall MICO_VXD_W32_GetVersion(DWORD dwDDB, DWORD hDevice, LPDIOC lpDIOCParms)
{
	DWORD	*pPhysAddress	= 0;
	DWORD	pLinAdr			= 0;
	
    Out_Debug_String("MICO_VXD_W32_GetVersion\n\r");

	if (!lpDIOCParms)
	{
		Out_Debug_String("MICO_VXD: MICO_VXD_W32_GetVersion WARNING: lpDIOCParms = NULL!\n\r");
		return E_POINTER;
	}
	
	*(DWORD*)lpDIOCParms->lpvOutBuffer = MICO_VXD_VERSION;
	*(DWORD*)lpDIOCParms->lpcbBytesReturned =  sizeof(DWORD);	

	/*
	// Alloziere 20*4096 Bytes innerhalb der physikalischen Adressraums 0x0...0x1000000
	pLinAdr = _PageAllocate(20,
						  PG_SYS,
						  0,
						  0x00000000,
						  0x0000000 / 4096,
						  0x1000000 / 4096,
						  &pPhysAddress,
						  PAGECONTIG | PAGEFIXED | PAGEUSEALIGN);
	if (pLinAdr)
	{
		*(DWORD*)lpDIOCParms->lpvOutBuffer = pLinAdr;
	}
*/

	return(NO_ERROR);
}

/////////////////////////////////////////////////////////////////////////////////////
DWORD _stdcall MICO_VXD_Dynamic_Exit(void)
{
    Out_Debug_String("MICO_VXD: Dynamic Exit\n\r");

    return(VXD_SUCCESS);
}

/////////////////////////////////////////////////////////////////////////////////////
DWORD _stdcall MICO_VXD_CleanUp(void)
{
    DWORD hEvent;
	int nI;

	Out_Debug_String("MICO_VXD: Cleaning Up\n\r");

	// Interrupt Service Routine deinstallieren
	RemoveISR();

	if (pVirtualConf && pVirtualConf->hEvent)
	{
		CloseVxDHandle(pVirtualConf->hEvent);
		pVirtualConf->hEvent = 0;
	}

	// Adresse der JPEG-Buffer Unvirtualisieren (Sind z.Z nicht virtualisiert)
	for (nI = 0; nI < MAX_JPEG_BUFFER; nI++)
	{
		if (pVirtualBuffer[nI])
		{
			UnvirtualizeAdress((void*)pVirtualBuffer[nI], JPEG_BUFFER_SIZE);
			pVirtualBuffer[nI] = 0;
		}
	}

	if (pVirtualConf)
	{
		// Adresse der Config-Struktur Unvirtualisieren
		UnvirtualizeAdress(pVirtualConf, sizeof(CONFIG));
		pVirtualConf = 0;
	}

	return(VXD_SUCCESS);
}

/////////////////////////////////////////////////////////////////////////////////////
void* VirtualizeAdress(void* pAdr, DWORD dwLen)
{
	void*  pVC		= 0;
	DWORD  dwPages	= 0;

	// Anzahl der Pages
	dwPages = (dwLen / 4096) + 1;

	if (!pAdr)
		return NULL;

	pVC	= (void*)_LinPageLock(((DWORD)pAdr >> 12), dwPages, PAGEMAPGLOBAL);

	if (!pVC)
		return NULL;

    return (void*)(((DWORD)pVC & ~0xfffL) | ((DWORD)pAdr & 0xfffL));
}

/////////////////////////////////////////////////////////////////////////////////////
void UnvirtualizeAdress(void* pAdr, DWORD dwLen)
{
	DWORD  dwPages	= 0;

	// Anzahl der Pages
  	dwPages = (dwLen / 4096) + 1;

	if (!pAdr)
		return;

	_LinPageUnlock(((DWORD)pAdr >> 12), dwPages, PAGEMAPGLOBAL);
}

/////////////////////////////////////////////////////////////////////////////////////
BOOL PostMessage(HWND hW, DWORD uM, DWORD wP, DWORD lP)
{
	hW = hW & 0x0000ffff;
	uM = uM & 0x0000ffff;

	if (hW == 0)
	{
		Out_Debug_String("MICO_VXD: WARNING: PostMessage Ungültiges Fensterhandle\n\r");

		return FALSE;
	}
	
	_SHELL_PostMessage(hW, uM, wP, lP, NULL, 0);
 }

////////////////////////////////////////////////////////////////////////////////////
BOOL InstallISR(WORD wIRQ)
{
	_Debug_Printf_Service("MICO_VXD: Install IRQ %u\n\r", wIRQ);

	vidDescriptor.VID_IRQ_Number		= wIRQ;						// IRQ
    vidDescriptor.VID_Options			= VPICD_OPT_CAN_SHARE;		// INIT<0>
    vidDescriptor.VID_Hw_Int_Proc		= (ULONG)ServiceMICOBdInt;	// ISR
    vidDescriptor.VID_Virt_Int_Proc		= 0;						// INIT<0>
    vidDescriptor.VID_EOI_Proc			= 0;						// INIT<0>
    vidDescriptor.VID_Mask_Change_Proc	= 0;						// INIT<0>
    vidDescriptor.VID_IRET_Proc			= 0;						// INIT<0>
    vidDescriptor.VID_IRET_Time_Out		= 500;						// INIT<500>
    vidDescriptor.VID_Hw_Int_Ref		= 0;						// new for 4.0

	hGlobalIrq = VPICD_Virtualize_IRQ(&vidDescriptor);
	if (!hGlobalIrq)
	{
		Out_Debug_String("MICO_VXD: VPICD_Virtualize_IRQ failed\n\r");
		return FALSE;
	}
	
	if (!VPICD_Auto_Mask_At_Inst_Swap(hGlobalIrq))
	{
		Out_Debug_String("MICO_VXD: VPICD_Auto_Mask_At_Inst_Swap failed\n\r");
		return FALSE;
	}

	VPICD_Physically_Unmask(hGlobalIrq);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////
void RemoveISR()
{
	if (hGlobalIrq)
	{
		Out_Debug_String("MICO_VXD: RemoveISR\n\r");

		VPICD_Force_Default_Behavior(hGlobalIrq);
		hGlobalIrq = 0;
	}
}

/////////////////////////////////////////////////////////////////////////////////////
//void Interrupt Service Routine (ISR)
void _stdcall ServiceMICOBdInt(void)
{
	BYTE	byIRR;
	BYTE	byIER;
	WORD	wIOBase;
	WORD	wField	= EVEN_FIELD;
	BOOL	bCont	= FALSE;
	
	static	BOOL bFlock = FALSE;

	// Sicher ist sicher
	if (!pVirtualConf)
		return;

	// Basisadresse des MiCoBoards holen
	wIOBase = pVirtualConf->wIOBase[MICO_EXTCARD0];

	do{
		// Interrupt Request lesen
		byIRR = Input(wIOBase | MICO_IRR_OFFSET);
		byIER = Input(wIOBase | MICO_IER_OFFSET); 

		// IRQ Ersteintritt:-> aktuelle Fieldinformation benutzen.
		if (bCont == FALSE)
		{
			// Fieldinformation holen.
			if (TSTBIT(byIRR, VSYNC_BIT))
				wField = (TSTBIT(byIRR, FIELD_BIT)) ? ODD_FIELD : EVEN_FIELD;
			else
				wField = (TSTBIT(byIRR, FIELD_BIT)) ? EVEN_FIELD : ODD_FIELD;
		}
		else // Schleifendurchlauf:-> vorherige Fieldinformation umkehren.
		{
			wField = (wField == EVEN_FIELD)	? ODD_FIELD : EVEN_FIELD;	
		}

		bCont = FALSE;

		// Zoran Interrupt? 
		if ((TSTBIT(byIER, ZR_IE_BIT)) && (TSTBIT(byIRR, ZR_IR_BIT)))
		{ 
			bCont = TRUE;

			// Rekursiver IRQ-Aufruf des Zoran?
			if (bFlock)
			{						 
				// Interrupt quittieren
				Input(wIOBase | ZR_STATUS0_OFFSET);
				Input(wIOBase | ZR_FCNT_OFFSET);
				Input(wIOBase | ZR_LEN0_OFFSET);
				Input(wIOBase | ZR_LEN1_OFFSET);

				// MiCoUnit über den rekursiven Aufruf informieren.
				Schedule_Global_Event(PostMessageEventCallback, WM_MICO_VXD_REKURSION); 
				return;
			}

			// Verhindert rekursiven Aufruf
			bFlock = TRUE;

			if (ServiceZoranInt(wField))
			{
				// Soll die Videoquelle gewechselt werden ?
				if (pVirtualConf->bSwitch)
					ServiceSourceSelection();
			}
		
			bFlock = FALSE;
		}
	}
	while(bCont);	

	VPICD_Phys_EOI(hGlobalIrq);
}

/////////////////////////////////////////////////////////////////////////////////////
// Behandlung Videoquellenumschaltung
void ServiceSourceSelection()
{
	WORD	wI;
	BYTE	byCSVDIN;
	int		nCamNr;

	// Sicher ist sicher
	if (!pVirtualConf)
		return;

	if (bGlobalSwitchAnalogVideo == FALSE)
	{
		// Analoge Umschaltung ist erfolgt.
		bGlobalSwitchAnalogVideo = TRUE;

		if (pVirtualConf->bHardwareSkip)
		{
			// Setzen des Bits 'SKPNVS'	-> Nächstes VSYNC sperren
			byCSVDIN = (BYTE)SETBIT(pVirtualConf->wCSVDIN, SELECT_SKPNVS_BIT);
			if (TSTBIT(byCSVDIN, SELECT_VD_BIT))
				byCSVDIN = (BYTE)CLRBIT(byCSVDIN, SELECT_VD_BIT);
			else
				byCSVDIN = (BYTE)SETBIT(byCSVDIN, SELECT_VD_BIT);
			Output(pVirtualConf->wIOBase[MICO_EXTCARD0] | CSVINDN_OFFSET, byCSVDIN);
		}

		// Soll ein Interrupt gewartet werden, bevor digital umgeschaltet wird?
		if (pVirtualConf->bSwitchDelay)
			return;
	}

	byCSVDIN = (BYTE)pVirtualConf->wCSVDIN;

	// Sollbildgröße setzen
	SetBPF((WORD)pVirtualConf->wBPF);	

	// Auf neue Videoquelle schalten und Vsyncs wieder freigeben.			
	Output(pVirtualConf->wIOBase[MICO_EXTCARD0] | CSVINDN_OFFSET, byCSVDIN);

	// Digitale Umschaltung ist erfolgt
	bGlobalSwitchAnalogVideo	= FALSE;

	// Zahl der Halbbilder, die nach einem Umschaltvorgang verworfen werden
	wGlobalSkipFieldCnt = pVirtualConf->wSkipFields;

	// Neue Videoquelle merken.
	pVirtualConf->wSource		= pVirtualConf->wReqSource;
	pVirtualConf->wExtCard		= pVirtualConf->wReqExtCard;

	// Aktuelle ProzeßID merken
	pVirtualConf->dwProzessID	= pVirtualConf->dwReqProzessID;

	// Umschaltung komplett
	pVirtualConf->bSwitch		= FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////
// Behandlung der Zoran-Interrupts
BOOL ServiceZoranInt(WORD wField)
{
	static BYTE byPrevFCNT  = 0;
	static WORD wWriteIndex = 0;
	BYTE  byStatus0;
	BYTE  byFCNT;
	BYTE  byLen0;
	BYTE  byLen1;
	BYTE  byCom0;
	BYTE  byBufferPage;
	WORD  wLen;
	WORD  wI;
	WORD  wIOBase;
	WORD  wCBData;
	WORD  wMinLen;
	WORD  wMaxLen;
	char  *lpBuffer;

	// Sicher ist sicher
	if (!pVirtualConf)
		return FALSE;

	// Basisadresse des MiCo-Boards
	wIOBase = pVirtualConf->wIOBase[MICO_EXTCARD0];
	byStatus0= Input(wIOBase | ZR_STATUS0_OFFSET);
	byFCNT	 = Input(wIOBase | ZR_FCNT_OFFSET);
	byLen0	 = Input(wIOBase | ZR_LEN0_OFFSET);
	byLen1	 = Input(wIOBase | ZR_LEN1_OFFSET);
	wLen	 = MAKEWORD(byLen0, byLen1);

	// Alle Interrupts freigeben
	_asm sti

	// Pagenummer aus dem Status0-Register selektieren
	byBufferPage = byStatus0 & 0x03;

	// Eine neue Pagenummer in COM0 selektieren.
	byCom0 = Input(wIOBase | ZR_COM0_OFFSET) & 0xfc;
	byCom0 |= byBufferPage;
	Output(wIOBase | ZR_COM0_OFFSET, byCom0);
	 
	// Lost Field ?
	if ((BYTE)(byPrevFCNT + 1) != byFCNT)
	{
		Schedule_Global_Event(PostMessageEventCallback, WM_MICO_VXD_LOST_FIELD); 
//		Out_Debug_String("MICO_VXD: WM_MICO_VXD_LOST_FIELD\n\r");
	}

	// Für den nächsten Durchgang sichern
	byPrevFCNT = byFCNT;

	// Soll die Videoquelle gewechselt werden? dann wollen wir keine alten
	// Bilder mehr haben.
	if (pVirtualConf->bSwitch)
		return TRUE;

	// Nach einer Umschaltung sollen Bilder verworfen werden.
	if (wGlobalSkipFieldCnt != 0)
	{
		wGlobalSkipFieldCnt--;
		return FALSE;
	}	  

	// Es wird nur ein Halbbild verwertet
	if ((pVirtualConf->wField == ODD_FIELD) && (wField == EVEN_FIELD))
		return TRUE;
	if ((pVirtualConf->wField == EVEN_FIELD) && (wField == ODD_FIELD))
		return TRUE;

	// Invalid Data
	if (TSTBIT(byStatus0, STATUS0_FNV_BIT))
	{
		Schedule_Global_Event(PostMessageEventCallback, WM_MICO_VXD_UNVALID_FIELD); 
//		Out_Debug_String("MICO_VXD: WM_MICO_VXD_UNVALID_FIELD\n\r");
 		return TRUE;
	}

	// Ist der JPegbuffer frei ?
	if (pVirtualConf->Jpeg[wWriteIndex].bValid)
	{
		Schedule_Global_Event(PostMessageEventCallback, WM_MICO_VXD_UNVALID_BUFFER); 
//		Out_Debug_String("MICO_VXD: WM_MICO_VXD_UNVALID_BUFFER\n\r");
		return TRUE;
	}
	
	// Länge soll gerade sein
	wLen = ((wLen + 1) & 0xfffe);	

    // This section prevents reads of lengthes ending with 511 or 512 chip bug.
	if (((wLen & 0xff) == 0xfe) || ((wLen & 0xff) == 0xfd))
		wLen += 8;
								   
	// Filtert zu kleine und zu große Bilder herraus.
	if ((pVirtualConf->wMinLen != 0) && (wLen < pVirtualConf->wMinLen))
	{
		Schedule_Global_Event(PostMessageEventCallback, WM_MICO_VXD_UNVALID_FRAMELEN); 
		return TRUE;
	}
	if ((pVirtualConf->wMaxLen != 0) && (wLen > pVirtualConf->wMaxLen))
	{
		Schedule_Global_Event(PostMessageEventCallback, WM_MICO_VXD_UNVALID_FRAMELEN); 
		return TRUE;
	}

	// Videoquelle, Jpeglänge, Field und ProzeßID, die zu diesen Daten gehört merken 
	pVirtualConf->Jpeg[wWriteIndex].wSource		= pVirtualConf->wSource;
	pVirtualConf->Jpeg[wWriteIndex].wExtCard		= pVirtualConf->wExtCard;
	pVirtualConf->Jpeg[wWriteIndex].dwProzessID	= pVirtualConf->dwProzessID;
	pVirtualConf->Jpeg[wWriteIndex].dwLen			= wLen;
	pVirtualConf->Jpeg[wWriteIndex].wField		= wField;
	pVirtualConf->Jpeg[wWriteIndex].dwTime		= GetCurrentTime();
	
	// Bilddaten in den Jpeg-Buffer kopieren
	lpBuffer = pVirtualBuffer[wWriteIndex];
	wCBData = wIOBase | ZR_CB_DATA_OFFSET;
	wLen >>= 1;	// 16 Bit Zugriff
			  
	_asm{
 		  movzx     ecx, wLen
		  mov       dx, wCBData
		  mov       edi,lpBuffer
		  cld			 

		  test      cx,cx
		  jz        LAB_END
		  rep		insw
		LAB_END:
	}

	// Jedes Bild bekommt eine eindeutige ID
	pVirtualConf->Jpeg[wWriteIndex].dwFieldID = dwGlobalFieldID;
	dwGlobalFieldID++;
	
	// Debuging Informations
	pVirtualConf->Jpeg[wWriteIndex].byFCNT = byFCNT;
	pVirtualConf->Jpeg[wWriteIndex].byPage = byBufferPage; 

	// Bilddaten als gültig markieren
	pVirtualConf->Jpeg[wWriteIndex].bValid	= TRUE;

	// MiCoUnit benachrichtigen
	Schedule_Global_Event(FieldReadyEventCallback, 0); 

	// Jpegbuffernummer zyklisch weiterzählen.
	wWriteIndex++;
	if (wWriteIndex == MAX_JPEG_BUFFER)
		wWriteIndex = 0;
	
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
HEVENT FieldReadyEventCallback()
{
	DWORD dwRef = 0;

	// Sicher ist sicher
	if (!pVirtualConf)
		return NULL;

	_asm mov [dwRef], edx
	SetEvent(pVirtualConf->hEvent);
	
	return pVirtualConf->hEvent;
}

////////////////////////////////////////////////////////////////////////////////
HEVENT PostMessageEventCallback()
{
	DWORD dwRef = 0;

	// Sicher ist sicher
	if (!pVirtualConf)
		return NULL;

	_asm mov [dwRef], edx

	PostMessage(pVirtualConf->hWnd, dwRef, 0, 0);

	return pVirtualConf->hEvent;
}

////////////////////////////////////////////////////////////////////////////////////
BOOL SetEvent(DWORD hEvent)
{
	DWORD dwRet = 0;

	if (!hEvent)    
	{
		Out_Debug_String("MICO_VXD: WARNING: SetEvent Ungültiges Eventhandle\n\r");

		return FALSE;
	}

	_asm mov eax, hEvent
	VxDCall (_VWIN32_SetWin32Event)
    _asm mov [dwRet], eax
	return (dwRet != 0) ? TRUE : FALSE;
}

////////////////////////////////////////////////////////////////////////////////
BOOL CloseVxDHandle(DWORD hEvent)
{
	DWORD dwRet = 0;

	if (!hEvent)    
	{
		Out_Debug_String("MICO_VXD: WARNING: SetEvent Ungültiges Eventhandle\n\r");

		return FALSE;
	}

	_asm mov eax, hEvent
	VxDCall (_VWIN32_CloseVxDHandle)
	_asm mov [dwRet], eax

	return (dwRet != 0) ? TRUE : FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// Liefert die aktuelle Urzeit und das Datum im folgendem Format:
//		YYYYYYMMMMDDDDDhhhhhmmmmmmssssss
// Die Jahre werden in Relation zu 1980 zurückgeliefert.
DWORD GetCurrentTime()
{
	static DWORD dwLastDate	= 0;
	static DWORD dwYear		= 0;
	static DWORD dwMonth	= 0;
	static DWORD dwDay		= 0;
	DWORD  dwDate			= 0;
	DWORD  dwTime			= 0;
    DWORD  dwRet			= 0;
	DWORD  dwDayCount		= 0;
	DWORD  dwDelta			= 0;
	DWORD  dwHour			= 0;
	DWORD  dwMinute			= 0;
	DWORD  dwSecond			= 0;
	WORD dwDaysPerYear[]	=	{366, 365, 365, 365};
	WORD dwDaysPerMonth[4][13]=		{ 
										{0,31,29,31,30,31,30,31,31,30,31,30,31},
										{0,31,28,31,30,31,30,31,31,30,31,30,31},
										{0,31,28,31,30,31,30,31,31,30,31,30,31},
										{0,31,28,31,30,31,30,31,31,30,31,30,31}
									};
	// Aktuelle Zeit/Datum holen.
    Touch_Register(eax)
    Touch_Register(edx)
	VxDCall (VTD_Get_Date_And_Time)
	_asm mov dword ptr [dwTime], eax
	_asm mov dword ptr [dwDate], edx

	dwDate++;	// 0 = 1.1.1980
	
	// hat sich die Anzahl der Tage seit dem letzten Durchlauf geändert?
	if (dwDate != dwLastDate)
	{
		// Letztes Berechnete Datum merken
		dwLastDate = dwDate;
		
		dwYear = 1980;
		do
		{
			dwDelta = dwDaysPerYear[dwYear % 4];

			if (dwDayCount + dwDelta >= dwDate)
				break;

			dwDayCount += dwDelta;
			dwYear++;
		}
		while (TRUE);

		dwDay = dwDate - dwDayCount;
		dwDayCount = 0;

		dwMonth = 1;
		do
		{
			dwDelta = dwDaysPerMonth[dwYear % 4][dwMonth];

			if (dwDayCount + dwDelta >= dwDay)
				break;

			dwDayCount += dwDelta;
			dwMonth++;
		}
		while (TRUE);

		dwDay = dwDay - dwDayCount;
	}

	// Uhrzeit ermitteln... 
	dwTime /= 1000;
	dwHour	 = dwTime / 3600;
	dwMinute = (dwTime - (dwHour * 3600)) / 60;
	dwSecond = (dwTime - (dwHour * 3600)) - dwMinute * 60;

	// Zahlenbereich überprüfen
	dwYear		= min(dwYear, 2043);
	dwMonth		= min(dwMonth, 12);
	dwDay		= min(dwDay, 31);
	dwHour		= min(dwHour, 23);
	dwMinute	= min(dwMinute, 59);
	dwSecond	= min(dwSecond, 59);

	// und ins Binärformat YYYYYYMMMMDDDDDhhhhhmmmmmmssssss bringen	
	dwRet = (dwRet | dwYear-1980)	<< 4;
	dwRet = (dwRet | dwMonth)		<< 5;
	dwRet = (dwRet | dwDay)			<< 5;
	dwRet = (dwRet | dwHour)		<< 6;
	dwRet = (dwRet | dwMinute)		<< 6;
	dwRet = (dwRet | dwSecond);  

	return dwRet;
}

/////////////////////////////////////////////////////////////////////////////////////
void  SetBPF(WORD wBPF)
{
	WORD  wIOBase  = pVirtualConf->wIOBase[MICO_EXTCARD0];
	DWORD dwTemp = 8L * (DWORD)wBPF;


	Outputw(wIOBase | ZR_ADDRESS_OFFSET, 0x09);						// Write Adress
	Output(wIOBase  | ZR_DATA_OFFSET,	(dwTemp & 0xff000000)>>24);	// Write Data
	Outputw(wIOBase | ZR_ADDRESS_OFFSET, 0x0a);						// Write Adress
	Output(wIOBase  | ZR_DATA_OFFSET,	(dwTemp & 0x00ff0000)>>16);	// Write Data
	Outputw(wIOBase | ZR_ADDRESS_OFFSET, 0x0b);						// Write Adress
	Output(wIOBase  | ZR_DATA_OFFSET,	(dwTemp & 0x0000ff00)>>8);	// Write Data
	Outputw(wIOBase | ZR_ADDRESS_OFFSET, 0x0c);						// Write Adress
	Output(wIOBase  | ZR_DATA_OFFSET,	(dwTemp & 0x000000ff));		// Write Data
	Outputw(wIOBase | ZR_ADDRESS_OFFSET, 0x00);						// Go		

	dwTemp -= (dwTemp / 128L);		              // minus STUFFING (SHMUEL)
	dwTemp -= ((dwTemp * 5L) / 64L);                // minus EOB (SHMUEL)

	Outputw(wIOBase | ZR_ADDRESS_OFFSET, 0x0d);						// Write Adress
	Output(wIOBase  | ZR_DATA_OFFSET,	(dwTemp & 0xff000000)>>24);	// Write Data
	Outputw(wIOBase | ZR_ADDRESS_OFFSET, 0x0e);						// Write Adress
	Output(wIOBase  | ZR_DATA_OFFSET,	(dwTemp & 0x00ff0000)>>16);	// Write Data
	Outputw(wIOBase | ZR_ADDRESS_OFFSET, 0x0f);						// Write Adress
	Output(wIOBase  | ZR_DATA_OFFSET,	(dwTemp & 0x0000ff00)>>8);	// Write Data
	Outputw(wIOBase | ZR_ADDRESS_OFFSET, 0x10);						// Write Adress
	Output(wIOBase  | ZR_DATA_OFFSET,	(dwTemp & 0x000000ff));		// Write Data
	Outputw(wIOBase | ZR_ADDRESS_OFFSET, 0x00);						// Go		
}

/////////////////////////////////////////////////////////////////////////////////////
void Output(WORD wAdr, BYTE byVal)
{
	_outp(wAdr,	byVal);
}

/////////////////////////////////////////////////////////////////////////////////////
void Outputw(WORD wAdr, WORD wVal)
{
	_outpw(wAdr, wVal);
}

/////////////////////////////////////////////////////////////////////////////////////
BYTE Input(WORD wAdr)
{
	return _inp(wAdr);
}

/////////////////////////////////////////////////////////////////////////////////////
WORD Inputw(WORD wAdr)
{
	return _inpw(wAdr);
}

/////////////////////////////////////////////////////////////////////////////////////
#pragma VxD_ICODE_SEG
#pragma VxD_IDATA_SEG

/////////////////////////////////////////////////////////////////////////////////////
DWORD _stdcall MICO_VXD_Dynamic_Init(void)
{
    Out_Debug_String("MICO_VXD: Dynamic Init\n\r");

    return(VXD_SUCCESS);
}

