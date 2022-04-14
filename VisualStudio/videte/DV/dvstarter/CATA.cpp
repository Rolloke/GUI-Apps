//----------------------------------------------------------------------------
// WinATA.cpp
// Befehle an ATA-Geräte schicken
// für Windows NT4(ab SP5)/2000/XP
// 2002 c't/Matthias Withopf

//#include <stdio.h>
//#include <stdlib.h>
#define WIN32_LEAN_AND_MEAN
#include <stdafx.h>
#include "CATA.h"

//----------------------------------------------------------------------------
// Der Konstruktor prüft das Betriebssystem und öffnet den Gerätetreiber

//////////////////////////////////////////////////////////////////////////////////////////////
CWinAta::CWinAta(UINT uDeviceNum)
{
	m_bIsNT4  = FALSE;
	m_hDevice = NULL;
	
	DWORD dwVersion = GetVersion();
	UCHAR MajorVersion = (UCHAR)dwVersion;

	if (dwVersion & 0x80000000)
	{
		// Windows 9x wird nicht unterstützt
		m_WinAtaErrorCode = aec_OSNotSupported;     
	}
	else
	{
		if (MajorVersion < 4)
		{
			// Windows NT vor Version 4 wird nicht unterstützt
			m_WinAtaErrorCode = aec_OSNotSupported; 
		}
		else if (MajorVersion == 4)
		{
			m_bIsNT4 = TRUE;
			m_WinAtaErrorCode = aec_Ok;           // Windows NT 4
		}
		else
		{
			m_WinAtaErrorCode = aec_Ok;          // Windows 2000 oder neuer
		}
	}

	if (!m_WinAtaErrorCode)
	{
		if (uDeviceNum < 8)
		{
			// Gerätetreiber als Datei öffnen
			CString sDeviceName;
			sDeviceName.Format("\\\\.\\PhysicalDrive%u", uDeviceNum);

			m_hDevice = CreateFile(sDeviceName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
			if (m_hDevice == INVALID_HANDLE_VALUE)
				m_WinAtaErrorCode = aec_InvalidDevice;
			else
				m_WinAtaErrorCode = aec_Ok;
		}
		else
			m_WinAtaErrorCode = aec_InvalidDevice;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Destruktor schließt Gerätetreiber
CWinAta::~CWinAta()
{
	if (m_hDevice)
		CloseHandle(m_hDevice);
}

//////////////////////////////////////////////////////////////////////////////////////////////
// SendCommand prüft Parameter
BOOL CWinAta::SendCommand(PATARegs pATARegs, void *pBuf, UINT uBufSize, UINT *pResultSize)
{
	BOOL bResult = FALSE;

	// Vorbeugend Größe der Antwort auf 0 setzten
	if (pResultSize)
		*pResultSize = 0;
  
	//  Parameterprüfung 
	if (!pATARegs)
		m_WinAtaErrorCode = aec_InvalidParameter;
	else if ((pBuf && !uBufSize) || (!pBuf && uBufSize))
		m_WinAtaErrorCode = aec_InvalidParameter;
	else
	{ 
		// Parameter o.k.
		if (m_bIsNT4)
			bResult = SendCommand_WinNT(pATARegs, pBuf, uBufSize, pResultSize);
		else
			bResult = SendCommand_Win2000(pATARegs, pBuf, uBufSize, pResultSize);
	}

	return bResult;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// ATA-Befehle unter Windows NT 4 versenden
// für Windows 2000 und Nachfolger nicht geeignet
BOOL CWinAta::SendCommand_WinNT(PATARegs pATARegs, void *pBuf, UINT uBufSize, UINT *pResultSize)
{
	BOOL bResult = FALSE;
	
	// Datenstruktur für SCSI-Befehle
	typedef struct
	{
		USHORT Length;
		UCHAR  ScsiStatus;
		UCHAR  PathId;
		UCHAR  TargetId;
		UCHAR  Lun;
		UCHAR  CdbLength;
		UCHAR  SenseInfoLength;
		UCHAR  DataIn;
		UCHAR  _pad[3];
		ULONG  DataTransferLength;
		ULONG  TimeOutValue;
#if (_WIN32_WINNT >= 0x0500)
		ULONG_PTR DataBufferOffset;
#else
		ULONG  DataBufferOffset;
#endif
		ULONG  SenseInfoOffset;
		UCHAR  Cdb[16];
	} SCSI_PASS_THROUGH;

	// Datenstruktur für SCSI-Befehl mit Daten
	typedef struct
	{
		SCSI_PASS_THROUGH spt;
		ULONG             Filler;
		UCHAR             ucSenseBuf[32];
		UCHAR             ucDataBuf[2 * 1024];
	} SCSI_PASS_THROUGH_WITH_BUFFERS, *PSCSI_PASS_THROUGH_WITH_BUFFERS;
	//---------------------------------------------------
  
   
	// Speicherplatz für Befehle und Daten anlegen
	SCSI_PASS_THROUGH_WITH_BUFFERS PT;
	ZeroMemory(&PT, sizeof(PT));

	// für ATA eigentlich unnötige SCSI-Parameter angeben
	PT.spt.Length           = sizeof(SCSI_PASS_THROUGH);
	PT.spt.PathId           = 0;
	PT.spt.TargetId         = 1;
	PT.spt.Lun              = 0;
	PT.spt.CdbLength        = CDB10GENERIC_LENGTH;
	PT.spt.SenseInfoLength  = 24;
	PT.spt.TimeOutValue     = 2;
	PT.spt.SenseInfoOffset  = FIELD_OFFSET(SCSI_PASS_THROUGH_WITH_BUFFERS, ucSenseBuf);

	// Position der eigentlichen Daten
	PT.spt.DataBufferOffset = FIELD_OFFSET(SCSI_PASS_THROUGH_WITH_BUFFERS, ucDataBuf);
	ZeroMemory(PT.ucDataBuf, sizeof(PT.ucDataBuf));
  
	if (pBuf)
	{
		UINT uTransferSize = uBufSize;

		if (uTransferSize > sizeof(PT.ucDataBuf))
			uTransferSize = sizeof(PT.ucDataBuf);

		PT.spt.DataIn             = SCSI_IOCTL_DATA_IN;
		PT.spt.DataTransferLength = uTransferSize;
	}
	else
	{
		PT.spt.DataIn             = SCSI_IOCTL_DATA_UNSPECIFIED;
		PT.spt.DataTransferLength = 0;
	}
  
	// Pseudo-SCSI-Befehl zum Übermitteln eine ATA-Befehls
	PT.spt.Cdb[0]          = SCSIOP_ATA_PASSTHROUGH;

	// ATA-Register folgen in PT.spt.Cdb
	PIDEREGS pRegs = (PIDEREGS)&PT.spt.Cdb[2];
	pRegs->bFeaturesReg     = pATARegs->Reg[0];
	pRegs->bSectorCountReg  = pATARegs->Reg[1];
	pRegs->bSectorNumberReg = pATARegs->Reg[2];
	pRegs->bCylLowReg       = pATARegs->Reg[3];
	pRegs->bCylHighReg      = pATARegs->Reg[4];
	pRegs->bDriveHeadReg    = pATARegs->Reg[5];
	pRegs->bCommandReg      = pATARegs->Reg[6];

	// Gesamtgröße von Befehlen und Datenpuffer berechnen
	UINT uDataBufOfs = FIELD_OFFSET(SCSI_PASS_THROUGH_WITH_BUFFERS, ucDataBuf);
	DWORD dwLen = uDataBufOfs + PT.spt.DataTransferLength;

	// Gerätetreiber meldet Größe der zurückgegebenen Daten
	DWORD dwBytesReturned = 0;

	// Gerätetreiber aufrufen 
	BOOL bStatus = DeviceIoControl(m_hDevice, IOCTL_SCSI_PASS_THROUGH, 
						&PT, sizeof(SCSI_PASS_THROUGH), &PT, dwLen, &dwBytesReturned, FALSE);
                            
	// bei Erfolg liefert DeviceIoControl einen Wert ungleich 0      
	if (!bStatus)
		m_WinAtaErrorCode = aec_CommandFailed;
	else
	{
		// DeviceIOControl erfolgreich	
		bResult = TRUE;
		if (pBuf)
		{
			if (uBufSize) 
				ZeroMemory(pBuf, uBufSize);

			UINT uReturnedSize = dwBytesReturned;
			if (uReturnedSize >= uDataBufOfs)
			{
				uReturnedSize -= uDataBufOfs;

				if (pResultSize) 
					*pResultSize = uReturnedSize;

				if (uReturnedSize > uBufSize)
					uReturnedSize = uBufSize;

				// Daten kopieren  
				memcpy(pBuf, PT.ucDataBuf, uReturnedSize);
			}
		}
	}
	return bResult;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// ATA-Befehle unter Windows 2000 oder XP versenden
BOOL CWinAta::SendCommand_Win2000(PATARegs pATARegs, void *pBuf, UINT uBufSize, UINT *pResultSize)
{
	BOOL bResult = FALSE;

	typedef struct
	{
		IDEREGS IdeReg;
		ULONG   DataBufferSize;
		UCHAR   DataBuffer[1];
	} ATA_PASS_THROUGH;
	//---------------------------------------------------

	UINT uSize = sizeof(ATA_PASS_THROUGH) + uBufSize;

	// Datenpuffer mit VirtualAlloc anfordern, damit der Gerätetreiber den Speicher ansprechen kann
	ATA_PASS_THROUGH *pAPT = (ATA_PASS_THROUGH *)VirtualAlloc(NULL, uSize, MEM_COMMIT, PAGE_READWRITE);
  
	if (pAPT)
	{
		ZeroMemory(pAPT, uSize);
		// Größe des Datenpuffers setzen
		pAPT->DataBufferSize          = uBufSize;
		// ATA-Register setzen
		pAPT->IdeReg.bFeaturesReg     = pATARegs->Reg[0];
		pAPT->IdeReg.bSectorCountReg  = pATARegs->Reg[1];
		pAPT->IdeReg.bSectorNumberReg = pATARegs->Reg[2];
		pAPT->IdeReg.bCylLowReg       = pATARegs->Reg[3];
		pAPT->IdeReg.bCylHighReg      = pATARegs->Reg[4];
		pAPT->IdeReg.bDriveHeadReg    = pATARegs->Reg[5];
		pAPT->IdeReg.bCommandReg      = pATARegs->Reg[6];

		if (pAPT->IdeReg.bCommandReg == 0xEC)  // ist es ATA IDENTIFY?
		{
			// Windows XP merkt sich den IDE-Konfigurationssektor 
			// daher vorher mit IOCTL_SCSI_RESCAN_BUS für ungültig erklären
			DWORD dwBytesReturned = 0;
			DeviceIoControl(m_hDevice, IOCTL_SCSI_RESCAN_BUS, NULL, 0, NULL, 0, &dwBytesReturned, FALSE);
			// eine halbe Sekunde warten
			Sleep(1500);    
		}

		DWORD dwBytesReturned = 0;

		BOOL bStatus = DeviceIoControl(m_hDevice, IOCTL_IDE_PASS_THROUGH, 
						pAPT, uSize, pAPT, uSize, &dwBytesReturned, FALSE);
		if (!bStatus)
			m_WinAtaErrorCode = aec_CommandFailed;
		else
		{
			if (pBuf)
			{
				if (uBufSize) 
					ZeroMemory(pBuf, uBufSize);
				if (dwBytesReturned > sizeof(ATA_PASS_THROUGH))
				{
					UINT uReturnedSize = dwBytesReturned - sizeof(ATA_PASS_THROUGH);
					if (pResultSize) 
						*pResultSize = uReturnedSize;
					if (uReturnedSize > uBufSize)
						uReturnedSize = uBufSize;

					memcpy(pBuf, pAPT->DataBuffer, uReturnedSize);
				}
			}
			bResult = TRUE;
		}
		VirtualFree(pAPT, uSize, MEM_RELEASE);
	}
	else
		m_WinAtaErrorCode = aec_OutOfMemory;
 
	return bResult;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// GetIdentifyStr kopiert Strings aus dem IDE-Konfigurationssektor
static void GetIdentifyStr(char *s, PWORD pIdentifyBuf, UINT uIdentifyBufSize)
{
	PWORD p  = pIdentifyBuf;
	char *p1 = s;

	// wandelt Big Endian in Little Endian
	for (UINT i = 0; i < uIdentifyBufSize; ++i, ++p)
	{
		*p1++ = (char)(*p >> 8);
		*p1++ = (char)*p;
	}
	*p1 = '\0';

	// führende Leerzeichen entfernen
	while (s[0] == ' ')
		memmove(s, s + 1, strlen(s));

	// abschließende Leerzeichen entfernen
	unsigned int l = strlen(s);
	while ((l > 0) && (s[l - 1] == ' '))
		s[--l] = '\0';
}

//////////////////////////////////////////////////////////////////////////////////////////////
BOOL CWinAta::EnableWriteCache()
{
	WORD IdentifyBuf[256];
	ZeroMemory(IdentifyBuf, sizeof(IdentifyBuf));

	BOOL bResult = FALSE;
	UINT ResultSize = 0;
	TATARegs ATARegs;
	ZeroMemory(&ATARegs, sizeof(ATARegs));

	// ATA-Befehl IDENTIFY
	ATARegs.Reg[6] = 0xEC;        
	ZeroMemory(IdentifyBuf, sizeof(IdentifyBuf));

	// SendCommand der Klasse CWinAta aufrufen, liefert bei Erfolg TRUE
	if (SendCommand(&ATARegs, IdentifyBuf, sizeof(IdentifyBuf), &ResultSize))
	{
		// Wird Write Cache unterstützt?
		if ((IdentifyBuf[82] & 0x0020))
		{
			// Ist der Cache bereits eingeschaltet?
			if (IdentifyBuf[85] & 0x0020)
				bResult = TRUE;
			else
			{
				ATARegs.Reg[6] = 0xEF;    // ATA-Befehl SET FEATURES
				ATARegs.Reg[0] = 0x02;    // Subcommand Enable Write Cache
				if (SendCommand(&ATARegs, NULL, 0, &ResultSize))
				{
					// IDE-Konfigurationssektor zum Prüfen der Einstellung erneut lesen
					ZeroMemory(&ATARegs, sizeof(ATARegs));

					// ATA-Befehl IDENTIFY
					ATARegs.Reg[6] = 0xEC;        
					ZeroMemory(IdentifyBuf, sizeof(IdentifyBuf));

					// SendCommand der Klasse CWinAta aufrufen, liefert bei Erfolg TRUE
					if (SendCommand(&ATARegs, IdentifyBuf, sizeof(IdentifyBuf), &ResultSize))
					{
						if (IdentifyBuf[85] & 0x0020)
							bResult = TRUE;
					}
				}
			}
		}
	}

	return bResult;
}

//////////////////////////////////////////////////////////////////////////////////////////////
BOOL CWinAta::DisableWriteCache()
{
	WORD IdentifyBuf[256];
	ZeroMemory(IdentifyBuf, sizeof(IdentifyBuf));

	BOOL bResult = FALSE;
	UINT ResultSize = 0;
	TATARegs ATARegs;

	// IDE-Konfigurationssektor zum Prüfen der Einstellung erneut lesen
	ZeroMemory(&ATARegs, sizeof(ATARegs));

	// ATA-Befehl IDENTIFY
	ATARegs.Reg[6] = 0xEC;        
	ZeroMemory(IdentifyBuf, sizeof(IdentifyBuf));

	// SendCommand der Klasse CWinAta aufrufen, liefert bei Erfolg TRUE
	if (SendCommand(&ATARegs, IdentifyBuf, sizeof(IdentifyBuf), &ResultSize))
	{
		// Wird Write Cache unterstützt?
		if ((IdentifyBuf[82] & 0x0020))
		{
			// Ist der Cache bereits ausgeschaltet?
			if (!(IdentifyBuf[85] & 0x0020))
				bResult = TRUE;
			else
			{
				ATARegs.Reg[6] = 0xEF;    // ATA-Befehl SET FEATURES
				ATARegs.Reg[0] = 0x82;    // Subcommand Disable Write Cache
				if (SendCommand(&ATARegs, NULL, 0, &ResultSize))
				{
					// IDE-Konfigurationssektor zum Prüfen der Einstellung erneut lesen
					ZeroMemory(&ATARegs, sizeof(ATARegs));

					// ATA-Befehl IDENTIFY
					ATARegs.Reg[6] = 0xEC;        
					ZeroMemory(IdentifyBuf, sizeof(IdentifyBuf));

					// SendCommand der Klasse CWinAta aufrufen, liefert bei Erfolg TRUE
					if (SendCommand(&ATARegs, IdentifyBuf, sizeof(IdentifyBuf), &ResultSize))
					{
						if (!(IdentifyBuf[85] & 0x0020))
							bResult = TRUE;
					}
				}
			}
		}
	}
	return bResult;
}
