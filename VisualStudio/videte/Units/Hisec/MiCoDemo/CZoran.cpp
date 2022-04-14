#include "stdafx.h"
#include "CIo.h"
#include "MiCoDefs.h"
#include "MiCoReg.h"
#include "CZoran.h"
#include "H55_Tbls.h"
#include "resource.h"
			 
// Buffersize
#define SIZE 255 		
		  
///////////////////////////////////////////////////////////////////////////////////////////////////////////
CZoran::CZoran(const CString &sAppIniName)
{
	static char szBuffer[SIZE];
	memset(szBuffer, 0, sizeof(szBuffer));

	m_sAppIniName = sAppIniName;

	// Member initialisieren...
	m_bOK								= FALSE;
	m_pzCoder							= NULL;
	m_pZIo								= NULL;
	m_nEncoderState						= MICO_ENCODER_OFF;
	m_nDecoderState						= MICO_DECODER_OFF;
	m_hCVxD								= INVALID_HANDLE_VALUE;
	m_wFormat							= MICO_ENCODER_HIGH;
	m_wFramerate						= 12;
	m_wBPF								= 20000;
	m_wMinLen							= 0;
	m_wMaxLen							= 0;
	m_wReadIndex						= 0;

	m_wYBackgnd							= 0x7ff;
	m_wUVBackgnd						= 0x7ff;
	m_wFeVideoFormat					= MICO_PAL_CCIR;
	m_pConf								= NULL;

	m_wFeHStart[MICO_NTSC_SQUARE]		= 0;
	m_wFeHEnd[MICO_NTSC_SQUARE]			= 0;
	m_wFeHTotal[MICO_NTSC_SQUARE]		= 0;
	m_wFeHSStart[MICO_NTSC_SQUARE]		= 0;
	m_wFeHSEnd[MICO_NTSC_SQUARE]		= 0;
	m_wFeBlankStart[MICO_NTSC_SQUARE]	= 0;
	m_wFeVStart[MICO_NTSC_SQUARE]		= 0;
	m_wFeVEnd[MICO_NTSC_SQUARE]			= 0;
	m_wFeVTotal[MICO_NTSC_SQUARE]		= 0;

	m_wFeHStart[MICO_PAL_SQUARE]		= 0;
	m_wFeHEnd[MICO_PAL_SQUARE]			= 0;
	m_wFeHTotal[MICO_PAL_SQUARE]		= 0;
	m_wFeHSStart[MICO_PAL_SQUARE]		= 0;
	m_wFeHSEnd[MICO_PAL_SQUARE]			= 0;
	m_wFeBlankStart[MICO_PAL_SQUARE]	= 0;
	m_wFeVStart[MICO_PAL_SQUARE]		= 0;
	m_wFeVEnd[MICO_PAL_SQUARE]			= 0;
	m_wFeVTotal[MICO_PAL_SQUARE]		= 0;
	m_wFeNMCU[MICO_PAL_SQUARE]			= 0;

	m_wFeHStart[MICO_NTSC_CCIR]			= 0;
	m_wFeHEnd[MICO_NTSC_CCIR]			= 0;
	m_wFeHTotal[MICO_NTSC_CCIR]			= 0;
	m_wFeHSStart[MICO_NTSC_CCIR]		= 0;
	m_wFeHSEnd[MICO_NTSC_CCIR]			= 0;
	m_wFeBlankStart[MICO_NTSC_CCIR]		= 0;
	m_wFeVStart[MICO_NTSC_CCIR]			= 0;
	m_wFeVEnd[MICO_NTSC_CCIR]			= 0;
	m_wFeVTotal[MICO_NTSC_CCIR]			= 0;
	m_wFeNMCU[MICO_NTSC_CCIR]			= 0;

	m_wFeHStart[MICO_PAL_CCIR]			= 0;
	m_wFeHEnd[MICO_PAL_CCIR]			= 0;
	m_wFeHTotal[MICO_PAL_CCIR]			= 0;
	m_wFeHSStart[MICO_PAL_CCIR]			= 0;
	m_wFeHSEnd[MICO_PAL_CCIR]			= 0;
	m_wFeBlankStart[MICO_PAL_CCIR]		= 0;
	m_wFeVStart[MICO_PAL_CCIR]			= 0;
	m_wFeVEnd[MICO_PAL_CCIR]			= 0;
	m_wFeVTotal[MICO_PAL_CCIR]			= 0;
	m_wFeNMCU[MICO_PAL_CCIR]			= 0;
	
	// Backend mit den gleichen Einstellungen wie das Frontend konfigurieren
	m_wBeVideoFormat= m_wFeVideoFormat;
	m_wBeHStart		= m_wFeHStart[m_wBeVideoFormat];
	m_wBeHEnd		= m_wFeHEnd[m_wBeVideoFormat];
	m_wBeHTotal		= m_wFeHTotal[m_wBeVideoFormat];
	m_wBeHSStart	= m_wFeHSStart[m_wBeVideoFormat];
	m_wBeHSEnd		= m_wFeHSEnd[m_wBeVideoFormat];
	m_wBeBlankStart	= m_wFeBlankStart[m_wBeVideoFormat];
	m_wBeVStart		= m_wFeVStart[m_wBeVideoFormat];
	m_wBeVEnd		= m_wFeVEnd[m_wBeVideoFormat];
	m_wBeVTotal		= m_wFeVTotal[m_wBeVideoFormat]; 
	m_wBeNMCU		= m_wFeNMCU[m_wBeVideoFormat];
  
	m_dwTCVNet 		= JPEG_BUFFER_SIZE*8-15;
	m_wSF			= 1*256;
	m_dwDecodedFrames = 0;

	// Minimale Jpeggröße in Prozent zur Sollgröße
	GetPrivateProfileString("MICOISA", "MinJpegSize", "0.8", szBuffer,  SIZE, m_sAppIniName); 
	m_fMinJpegLen = atof(szBuffer);
	
	// Maximale Jpeggröße in Prozent zur Sollgröße
	GetPrivateProfileString("MICOISA", "MaxJpegSize", "1.4", szBuffer,  SIZE, m_sAppIniName); 
	m_fMaxJpegLen = atof(szBuffer);

	// EncoderTimeOut 	
	m_dwEncoderTimeout = GetPrivateProfileInt("MICOISA", "TimeOut", 1000, m_sAppIniName);
	WK_TRACE("EncoderTimeout=%lums\n", m_dwEncoderTimeout);

	// Protokollieren
	WK_TRACE("MinJpegSize=%f%%\n", 100.0 * m_fMinJpegLen);
	WK_TRACE("MaxJpegSize=%f%%\n", 100.0 * m_fMaxJpegLen);
		
	m_bOK = TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
CZoran::~CZoran()
{
	if (m_hCVxD == INVALID_HANDLE_VALUE)
		return;

	// Entlade MICO.VXD
	if(!CloseHandle(m_hCVxD)){
		WK_TRACE_ERROR("cannot close mico.vxd\n");
	}

	m_hCVxD = NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CZoran::IsValid()
{
	return m_bOK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CZoran::Reset()
{
    BYTE byReset = 0;

	// I2C-Bus Controller reseten...
	if (m_pConf)
	{
		byReset = m_IO.ReadFromLatch(m_pConf->wIOBase[MICO_EXTCARD0] | RESET_OFFSET);
		byReset = (BYTE)CLRBIT(byReset, PARAM_RESET_ZR);
		m_IO.WriteToLatch(m_pConf->wIOBase[MICO_EXTCARD0] | RESET_OFFSET, byReset);
		Sleep(10);

		byReset = (BYTE)SETBIT(byReset, PARAM_RESET_ZR);

		m_IO.WriteToLatch(m_pConf->wIOBase[MICO_EXTCARD0] | RESET_OFFSET, byReset);

		Sleep(10);
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CZoran::Open(const CONFIG &Conf)
{
	DWORD       cbBytesReturned;
    DWORD       dwLastError;
	DWORD		dwVxdVersion = 0;

	// Konfigurationsstruktur anlegen
	m_pConf = (CONFIG*)GlobalAlloc(GMEM_FIXED,(DWORD)sizeof(CONFIG));

   	if (!m_pConf)
	{
		WK_TRACE_ERROR("CZoran::Open\tGlobalAlloc failed\n");
		return FALSE;
	}

	*m_pConf = Conf;

	// Mico Identifier Register auslesen
	m_byMiCoID = m_IO.Input(m_pConf->wIOBase[MICO_EXTCARD0] | MICO_ID_OFFSET);

	m_wFeHStart[MICO_NTSC_CCIR]			= GetPrivateProfileInt("Timing_NTSC_CCIR", "FeHStart",  8, m_sAppIniName);
	m_wFeHEnd[MICO_NTSC_CCIR]			= GetPrivateProfileInt("Timing_NTSC_CCIR", "FeHEnd",	712, m_sAppIniName);
	m_wFeHTotal[MICO_NTSC_CCIR]			= GetPrivateProfileInt("Timing_NTSC_CCIR", "FeHTotal",  858, m_sAppIniName);
	m_wFeHSStart[MICO_NTSC_CCIR]		= GetPrivateProfileInt("Timing_NTSC_CCIR", "FeHSStart", 735, m_sAppIniName);
	m_wFeHSEnd[MICO_NTSC_CCIR]			= GetPrivateProfileInt("Timing_NTSC_CCIR", "FeHSEnd",	783, m_sAppIniName);
	m_wFeBlankStart[MICO_NTSC_CCIR]		= GetPrivateProfileInt("Timing_NTSC_CCIR", "FeBlankStart",  720, m_sAppIniName);
	m_wFeVStart[MICO_NTSC_CCIR]			= GetPrivateProfileInt("Timing_NTSC_CCIR", "FeVStart",  14, m_sAppIniName);
	m_wFeVEnd[MICO_NTSC_CCIR]			= GetPrivateProfileInt("Timing_NTSC_CCIR", "FeVEnd",	254, m_sAppIniName);
	m_wFeVTotal[MICO_NTSC_CCIR]			= GetPrivateProfileInt("Timing_NTSC_CCIR", "FeVTotal",  525, m_sAppIniName);
	m_wFeNMCU[MICO_NTSC_CCIR]			= GetPrivateProfileInt("Timing_NTSC_CCIR", "FeNMCU",	43, m_sAppIniName);

	m_wFeHStart[MICO_PAL_CCIR]			= GetPrivateProfileInt("Timing_PAL_CCIR", "FeHStart",	8, m_sAppIniName);
	m_wFeHEnd[MICO_PAL_CCIR]			= GetPrivateProfileInt("Timing_PAL_CCIR", "FeHEnd",		712, m_sAppIniName);
	m_wFeHTotal[MICO_PAL_CCIR]			= GetPrivateProfileInt("Timing_PAL_CCIR", "FeHTotal",	864, m_sAppIniName);
	m_wFeHSStart[MICO_PAL_CCIR]			= GetPrivateProfileInt("Timing_PAL_CCIR", "FeHSStart",  731, m_sAppIniName);
	m_wFeHSEnd[MICO_PAL_CCIR]			= GetPrivateProfileInt("Timing_PAL_CCIR", "FeHSEnd",	783, m_sAppIniName);
	m_wFeBlankStart[MICO_PAL_CCIR]		= GetPrivateProfileInt("Timing_PAL_CCIR", "FeBlankStart",  720, m_sAppIniName);
	m_wFeVStart[MICO_PAL_CCIR]			= GetPrivateProfileInt("Timing_PAL_CCIR", "FeVStart",	18, m_sAppIniName);
	m_wFeVEnd[MICO_PAL_CCIR]			= GetPrivateProfileInt("Timing_PAL_CCIR", "FeVEnd",		306, m_sAppIniName);
	m_wFeVTotal[MICO_PAL_CCIR]			= GetPrivateProfileInt("Timing_PAL_CCIR", "FeVTotal",	625, m_sAppIniName);
	m_wFeNMCU[MICO_PAL_CCIR]			= GetPrivateProfileInt("Timing_PAL_CCIR", "FeNMCU",		43, m_sAppIniName);

	// 58 MHz Quarz
	if (m_byMiCoID == 0x22)
	{
		m_wFeHStart[MICO_NTSC_SQUARE]		= GetPrivateProfileInt("Timing_NTSC_SQUARE", "FeHStart",	0, m_sAppIniName);
		m_wFeHEnd[MICO_NTSC_SQUARE]			= GetPrivateProfileInt("Timing_NTSC_SQUARE", "FeHEnd",		640, m_sAppIniName);
		m_wFeHTotal[MICO_NTSC_SQUARE]		= GetPrivateProfileInt("Timing_NTSC_SQUARE", "FeHTotal",	780, m_sAppIniName);
		m_wFeHSStart[MICO_NTSC_SQUARE]		= GetPrivateProfileInt("Timing_NTSC_SQUARE", "FeHSStart",	657, m_sAppIniName);
		m_wFeHSEnd[MICO_NTSC_SQUARE]		= GetPrivateProfileInt("Timing_NTSC_SQUARE", "FeHSEnd",		697, m_sAppIniName);
		m_wFeBlankStart[MICO_NTSC_SQUARE]	= GetPrivateProfileInt("Timing_NTSC_SQUARE", "FeBlankStart",640, m_sAppIniName);
		m_wFeVStart[MICO_NTSC_SQUARE]		= GetPrivateProfileInt("Timing_NTSC_SQUARE", "FeVStart",	14, m_sAppIniName);
		m_wFeVEnd[MICO_NTSC_SQUARE]			= GetPrivateProfileInt("Timing_NTSC_SQUARE", "FeVEnd",		254, m_sAppIniName);
		m_wFeVTotal[MICO_NTSC_SQUARE]		= GetPrivateProfileInt("Timing_NTSC_SQUARE", "FeVTotal",	525, m_sAppIniName);
		m_wFeNMCU[MICO_NTSC_SQUARE]			= GetPrivateProfileInt("Timing_NTSC_SQUARE", "FeNMCU",		39, m_sAppIniName);

		m_wFeHStart[MICO_PAL_SQUARE]		= GetPrivateProfileInt("Timing_PAL_SQUARE", "FeHStart",		0, m_sAppIniName);
		m_wFeHEnd[MICO_PAL_SQUARE]			= GetPrivateProfileInt("Timing_PAL_SQUARE", "FeHEnd",		768, m_sAppIniName);
		m_wFeHTotal[MICO_PAL_SQUARE]		= GetPrivateProfileInt("Timing_PAL_SQUARE", "FeHTotal",		944, m_sAppIniName);
		m_wFeHSStart[MICO_PAL_SQUARE]		= GetPrivateProfileInt("Timing_PAL_SQUARE", "FeHSStart",	791, m_sAppIniName);
		m_wFeHSEnd[MICO_PAL_SQUARE]			= GetPrivateProfileInt("Timing_PAL_SQUARE", "FeHSEnd",		837, m_sAppIniName);
		m_wFeBlankStart[MICO_PAL_SQUARE]	= GetPrivateProfileInt("Timing_PAL_SQUARE", "FeBlankStart", 768, m_sAppIniName);
		m_wFeVStart[MICO_PAL_SQUARE]		= GetPrivateProfileInt("Timing_PAL_SQUARE", "FeVStart",		18, m_sAppIniName);
		m_wFeVEnd[MICO_PAL_SQUARE]			= GetPrivateProfileInt("Timing_PAL_SQUARE", "FeVEnd",		306, m_sAppIniName);
		m_wFeVTotal[MICO_PAL_SQUARE]		= GetPrivateProfileInt("Timing_PAL_SQUARE", "FeVTotal",		625, m_sAppIniName);
		m_wFeNMCU[MICO_PAL_SQUARE]			= GetPrivateProfileInt("Timing_PAL_SQUARE", "FeNMCU",		47, m_sAppIniName);
	}
	else if (m_byMiCoID == 0x30)	// 59MHz Quarz
	{
		m_wFeHStart[MICO_NTSC_SQUARE]		= GetPrivateProfileInt("Timing_NTSC_SQUARE_59MHZ", "FeHStart",  0, m_sAppIniName);
		m_wFeHEnd[MICO_NTSC_SQUARE]			= GetPrivateProfileInt("Timing_NTSC_SQUARE_59MHZ", "FeHEnd",	640, m_sAppIniName);
		m_wFeHTotal[MICO_NTSC_SQUARE]		= GetPrivateProfileInt("Timing_NTSC_SQUARE_59MHZ", "FeHTotal",  780, m_sAppIniName);
		m_wFeHSStart[MICO_NTSC_SQUARE]		= GetPrivateProfileInt("Timing_NTSC_SQUARE_59MHZ", "FeHSStart", 657, m_sAppIniName);
		m_wFeHSEnd[MICO_NTSC_SQUARE]		= GetPrivateProfileInt("Timing_NTSC_SQUARE_59MHZ", "FeHSEnd",	697, m_sAppIniName);
		m_wFeBlankStart[MICO_NTSC_SQUARE]	= GetPrivateProfileInt("Timing_NTSC_SQUARE_59MHZ", "FeBlankStart",  640, m_sAppIniName);
		m_wFeVStart[MICO_NTSC_SQUARE]		= GetPrivateProfileInt("Timing_NTSC_SQUARE_59MHZ", "FeVStart",  14, m_sAppIniName);
		m_wFeVEnd[MICO_NTSC_SQUARE]			= GetPrivateProfileInt("Timing_NTSC_SQUARE_59MHZ", "FeVEnd",	254, m_sAppIniName);
		m_wFeVTotal[MICO_NTSC_SQUARE]		= GetPrivateProfileInt("Timing_NTSC_SQUARE_59MHZ", "FeVTotal",  525, m_sAppIniName);

		m_wFeHStart[MICO_PAL_SQUARE]		= GetPrivateProfileInt("Timing_PAL_SQUARE_59MHZ", "FeHStart",	0, m_sAppIniName);
		m_wFeHEnd[MICO_PAL_SQUARE]			= GetPrivateProfileInt("Timing_PAL_SQUARE_59MHZ", "FeHEnd",		768, m_sAppIniName);
		m_wFeHTotal[MICO_PAL_SQUARE]		= GetPrivateProfileInt("Timing_PAL_SQUARE_59MHZ", "FeHTotal",	944, m_sAppIniName);
		m_wFeHSStart[MICO_PAL_SQUARE]		= GetPrivateProfileInt("Timing_PAL_SQUARE_59MHZ", "FeHSStart",  791, m_sAppIniName);
		m_wFeHSEnd[MICO_PAL_SQUARE]			= GetPrivateProfileInt("Timing_PAL_SQUARE_59MHZ", "FeHSEnd",	837, m_sAppIniName);
		m_wFeBlankStart[MICO_PAL_SQUARE]	= GetPrivateProfileInt("Timing_PAL_SQUARE_59MHZ", "FeBlankStart",  768, m_sAppIniName);
		m_wFeVStart[MICO_PAL_SQUARE]		= GetPrivateProfileInt("Timing_PAL_SQUARE_59MHZ", "FeVStart",	18, m_sAppIniName);
		m_wFeVEnd[MICO_PAL_SQUARE]			= GetPrivateProfileInt("Timing_PAL_SQUARE_59MHZ", "FeVEnd",		306, m_sAppIniName);
		m_wFeVTotal[MICO_PAL_SQUARE]		= GetPrivateProfileInt("Timing_PAL_SQUARE_59MHZ", "FeVTotal",	625, m_sAppIniName);
		m_wFeNMCU[MICO_PAL_SQUARE]			= GetPrivateProfileInt("Timing_PAL_SQUARE_59MHZ", "FeNMCU",		47, m_sAppIniName);
	}
	
	// Backend mit den gleichen Einstellungen wie das Frontend konfigurieren
	m_wBeVideoFormat= m_wFeVideoFormat;
	m_wBeHStart		= m_wFeHStart[m_wBeVideoFormat];
	m_wBeHEnd		= m_wFeHEnd[m_wBeVideoFormat];
	m_wBeHTotal		= m_wFeHTotal[m_wBeVideoFormat];
	m_wBeHSStart	= m_wFeHSStart[m_wBeVideoFormat];
	m_wBeHSEnd		= m_wFeHSEnd[m_wBeVideoFormat];
	m_wBeBlankStart	= m_wFeBlankStart[m_wBeVideoFormat];
	m_wBeVStart		= m_wFeVStart[m_wBeVideoFormat];
	m_wBeVEnd		= m_wFeVEnd[m_wBeVideoFormat];
	m_wBeVTotal		= m_wFeVTotal[m_wBeVideoFormat]; 
	m_wBeNMCU		= m_wFeNMCU[m_wBeVideoFormat];


	// Die Funktion OpenVxDHandle wandelt ein R3EventHandle in ein R0EventHandle
	OPENVXDH fpOvh;
	fpOvh = (OPENVXDH)GetProcAddress(GetModuleHandle("KERNEL32"), "OpenVxDHandle");
	if (!fpOvh)
	{
		WK_TRACE_ERROR("Kernel32 Adress of OpenVxDHandle unvalid\n");
		return FALSE;
	}

	// Dieses Event teilt der MiCoUnit mit, daß Bilddaten abgeholt werden können
	m_hEvent = CreateEvent(0, FALSE, FALSE, NULL);

	if (!m_hEvent)
	{
		WK_TRACE_ERROR("CreateEvent failed\n");
		return FALSE;
	}

	m_pConf->hEvent = (DWORD)(*fpOvh)(m_hEvent);

	// Zoran reset
	Reset();

	// Board-Interrupts sperren
	DisableBrdIRQ();

	// Speicherblöcke zum Transport der Jpegbilder anlegen
	for (int nI = 0; nI	< MAX_JPEG_BUFFER; nI++)
	{
		if (m_pConf)
			m_pConf->Jpeg[nI].lpBuffer = (LPSTR)GlobalAlloc(GPTR, JPEG_BUFFER_SIZE);
	}

	// Lade MICO.VXD
    m_hCVxD = CreateFile("\\\\.\\MICO.VXD", 0,0,NULL, OPEN_EXISTING, FILE_FLAG_DELETE_ON_CLOSE, 0);
   
	if (m_hCVxD == INVALID_HANDLE_VALUE)
    {
        dwLastError = GetLastError();
		CString sError;

		if (dwLastError == ERROR_NOT_SUPPORTED)
		{
			sError.LoadString(IDS_RTE_VXD_NO_IOCTL);
			WK_TRACE_ERROR(sError);
		}
		else if (dwLastError == ERROR_FILE_NOT_FOUND)
		{
			sError.LoadString(IDS_RTE_VXD_NOT_FOUND);
			WK_TRACE_ERROR(sError);
		}

		return FALSE;
	}			 

	//Versionsnummer des VXD erfragen
	if (!DeviceIoControl(m_hCVxD, MICO_VXD_GETVERSION, NULL, 0, &dwVxdVersion, sizeof(DWORD), &cbBytesReturned, NULL))
    {
		dwLastError = GetLastError();

		CString sError;
		sError.LoadString(IDS_RTE_VXD_API_FAILED);
		sError.Format(sError, dwLastError);

		WK_TRACE_ERROR(sError);

		return FALSE;
	}
/*
	if (dwVxdVersion != MICO_VXD_VERSION)
	{
		CString sError;
		sError.LoadString(IDS_RTE_VXD_WRONG_VERSION);
		sError.Format(sError, (WORD)dwVxdVersion);

		WK_TRACE_ERROR(sError);

		return FALSE;
	}
*/
	WK_TRACE("Mico.vxd Version=0x%x\n", dwVxdVersion);  
/*	
	DWORD *pAdr =  (DWORD*)dwVxdVersion;

	pAdr[0] = 0x12345678;
*/
	// Initialisiere den MICOVXD
	if (!DeviceIoControl(m_hCVxD, MICO_VXD_OPEN, m_pConf, sizeof(CONFIG), NULL, 0, &cbBytesReturned, NULL))
    {
		CString sError;
		dwLastError = GetLastError();

		switch (dwLastError)
		{
			case ERROR_IRQ_BUSY:
				sError.LoadString(IDS_RTE_IRQ_FAILED);
				break;

			case E_POINTER:
   				sError.LoadString(IDS_RTE_VIRTUALIZE);
				break;

		}
		WK_TRACE_ERROR(sError);

		return FALSE;
	}

	// Dem Mico-Board die verwendete Interruptnummer mitteilen
	SetIRQNumber(m_pConf->wIRQ);

	// Dies sind die original Zoran-Klassen
	m_pZIo		= new ZoranIo(m_pConf->wIOBase[0]);
	m_pzCoder	= new Z050_Coder(m_pZIo);

	// Interrupts freigeben
	EnableBrdIRQ();

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CZoran::Close()
{
    DWORD       cbBytesReturned;
	
	// Interrupts sperren
	DisableBrdIRQ();

	// Warte bis der letzte Interrupt beendet wurde.
	Sleep(100);

    // Schließe den MICOVXD
	if (!DeviceIoControl(m_hCVxD, MICO_VXD_CLOSE, NULL, 0, NULL, 0, &cbBytesReturned, NULL))
    {
		DWORD dwLastError = GetLastError();
        WK_TRACE_ERROR("CZoran::Close\tDevice does not support the requested API\t(Fehlercode=%lu)\n", dwLastError);
  		return FALSE;
	}

	// JPEG-Buffer freigeben
	for (int nI = 0; nI	< MAX_JPEG_BUFFER; nI++)
	{
		if (m_pConf->Jpeg[nI].lpBuffer != NULL)
			GlobalFree(m_pConf->Jpeg[nI].lpBuffer);
	}
 
	// Konfigurationsbuffer freigeben
	if (m_pConf != NULL)
		GlobalFree(m_pConf);

	WK_DELETE(m_pzCoder);
	WK_DELETE(m_pZIo);

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CZoran::SetInputSource(WORD wFrontEnd, WORD wExtCard, WORD wSource, BYTE byYPort, BYTE byCPort, DWORD dwProzessID)
{
	DWORD	dwTimeOut = GetTickCount();
	BYTE	byCSVDIN;
	BYTE	byCSVIAN;
	BOOL	bChanges = FALSE;

	// Gewünschte Videoquelle auswählen
	byCSVIAN = ChipSelectVideoInAnalogIn(wFrontEnd, wExtCard) & 0xc0;
	byCSVIAN = byCSVIAN | byYPort << 3 | byCPort;		
	ChipSelectVideoInAnalogOut(wFrontEnd, wExtCard, byCSVIAN);

	// Auf gewünschten CoVi schalten
	byCSVIAN = ChipSelectVideoInAnalogIn(wFrontEnd, MICO_EXTCARD0) & 0x3f;
	byCSVIAN = byCSVIAN | wExtCard << 6; 					
	ChipSelectVideoInAnalogOut(wFrontEnd, MICO_EXTCARD0, byCSVIAN);

	// Das Frontend wählen
	byCSVDIN = ChipSelectVideoInDigitalIn();
	if (wFrontEnd == MICO_FE0)
		byCSVDIN = CLRBIT(byCSVDIN, SELECT_VD_BIT);
	else if (wFrontEnd == MICO_FE1)
		byCSVDIN = SETBIT(byCSVDIN, SELECT_VD_BIT);

	if (GetEncoderState() == MICO_ENCODER_ON)
	{
		// Synchron auf das jeweils andere Frontend umschalten
		if (m_pConf)
		{
			m_pConf->dwReqProzessID	= dwProzessID;	// Prozeßzuordnung
			m_pConf->wReqSource		= wSource;		// Gewünschter Videoeingang
			m_pConf->wReqExtCard	= wExtCard;		// Gewünschtes Board
			m_pConf->wCSVDIN		= byCSVDIN;		// ChipSelectVideoInDigital
			m_pConf->bSwitch		= TRUE;			// Und Umschalten.

			DWORD	dwTimeOut = GetTickCount();
			// Warte bis der MiCo.vxd die Quelle umgeschaltet hat.
			while(m_pConf->bSwitch)
			{
				if (GetTickCount() > dwTimeOut + 1000)
				{
					WK_TRACE_ERROR("CZoran::SetInputSource\tTimeOut\n");
					ChipSelectVideoInDigitalOut(byCSVDIN);
					m_pConf->dwProzessID	= dwProzessID;
					m_pConf->wSource		= wSource;
					m_pConf->wExtCard		= wExtCard;
					m_pConf->bSwitch		= FALSE;
					return;
				}
				Sleep(5);
			}

			// Dieser Aufruf bewirkt, daß das DigitalVideoSelect-Register, welches
			// in der IRQ-Routine angesprochen wird, auch gelatcht wird, da dieses
			// leider nicht ReadBack-fähig ist.
			ChipSelectVideoInDigitalOut(byCSVDIN);
		}
	}  
	else
	{
		// Es wird nicht im Interrupt umgeschaltet -> Globale Parameter
		// müssen deshalb an dieser Stelle manuell gesetzt werden.
		if (m_pConf)
		{
			m_pConf->dwReqProzessID	= dwProzessID;
			m_pConf->wReqSource		= wSource;
			m_pConf->wReqExtCard	= wExtCard;
			m_pConf->dwProzessID	= dwProzessID;
			m_pConf->wSource		= wSource;
			m_pConf->wExtCard		= wExtCard;
		}
		// Dieser Aufruf bewirkt das manuelle digitale Umschalten, wenn
		// der Encoder nicht läuft.
		ChipSelectVideoInDigitalOut(byCSVDIN);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CZoran::SetFeVideoFormat(WORD wFormat)
{
	switch (wFormat)
	{
		case MICO_PAL_SQUARE:
		case MICO_NTSC_SQUARE:
		case MICO_PAL_CCIR:
		case MICO_NTSC_CCIR:
			break;
		default:
			WK_TRACE_ERROR("CZoran::SetFeVideoFormat\tUnbekanntes Videoformat:%u\n", wFormat);
			return;
	}

	m_wFeVideoFormat = wFormat;

	if (GetEncoderState() == MICO_ENCODER_ON)
	{
		// Wenn der Encoder arbeitet dann den Encoder durch Neustart uminitialisieren.
		StartEncoding();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CZoran::SetBeVideoFormat(WORD wFormat)
{
	switch (wFormat)
	{
		case MICO_PAL_SQUARE:
		case MICO_NTSC_SQUARE:
		case MICO_PAL_CCIR:
		case MICO_NTSC_CCIR:
			break;
		default:
			WK_TRACE_ERROR("CZoran::SetBeVideoFormat\tUnbekanntes Videoformat:%u\n", wFormat);
			return;
	}

	m_wBeHTotal		= m_wFeHTotal[wFormat];
	m_wBeVTotal		= m_wFeVTotal[wFormat]; 
	m_wBeBlankStart	= m_wFeBlankStart[wFormat];
	m_wBeHStart		= m_wFeHStart[wFormat];
	m_wBeHEnd		= m_wFeHEnd[wFormat];
	m_wBeHSStart	= m_wFeHSStart[wFormat];
	m_wBeHSEnd		= m_wFeHSEnd[wFormat];
	m_wBeVStart		= m_wFeVStart[wFormat];
	m_wBeVEnd		= m_wFeVEnd[wFormat];

	m_wBeVideoFormat= wFormat;
	m_wBeNMCU		= (m_wBeHEnd - m_wBeHStart) / 16 - 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CZoran::SetFormat(WORD wFormat)
{
	switch(wFormat)
	{
		case MICO_ENCODER_HIGH:
		case MICO_ENCODER_LOW:
			break;
		default:
   			WK_TRACE_ERROR("CZoran::SetFormat\tUnbekanntes Bildformat\n");
			return;
	}

	// Ändert sich das Bildformat?
	if (wFormat == m_wFormat)
		return;

	m_wFormat = wFormat;

	if (GetEncoderState() == MICO_ENCODER_ON)
	{
		// Wenn der Encoder arbeitet dann den Encoder durch Neustart uminitialisieren.
		StartEncoding();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CZoran::SetFramerate(WORD wFramerate)
{
	/* Nicht verwendet */

	m_wFramerate = wFramerate;
	if (wFramerate == 0)
	{
		WK_TRACE_ERROR("Framerate=0\n");
		return;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CZoran::SetBPF(DWORD dwBPF)
{
	// Die neuen Einstellungen werden erst nach dem Umschalten der Kamera verwendet.
	m_wMinLen		= (WORD)min(((float)dwBPF * m_fMinJpegLen), JPEG_BUFFER_SIZE);
	m_wMaxLen		= (WORD)min(((float)dwBPF * m_fMaxJpegLen), JPEG_BUFFER_SIZE);
	m_wBPF			= (WORD)dwBPF;	// BYTE PER FRAME

	if (m_pConf)
	{
		_asm cli
		m_pConf->wBPF			= m_wBPF;		// Sollgröße eines Jpegfields
		m_pConf->wMinLen		= m_wMinLen;	// Mindestgröße eines Jpegfields
		m_pConf->wMaxLen		= m_wMaxLen;	// Maximalröße eines Jpegfields
		_asm sti
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CZoran::StartEncoding()
{
	// Zoranchipsatz initialisieren
	ZR36055InitForEncoding();
	ZR36050InitForEncoding();

	m_nEncoderState = MICO_ENCODER_ON;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CZoran::StopEncoding()
{
	WORD wIOBase	= 0x200;
	BYTE byCom1		= 0x00;

	if (m_pConf)
		wIOBase = m_pConf->wIOBase[MICO_EXTCARD0];

	// Commandregister 1 initialisieren (COM1 = 0x03)
	byCom1 = m_IO.Input(wIOBase | ZR_COM1_OFFSET);
	byCom1 = SETBIT(byCom1, COM1_ACTIVE_BIT);		// Inactive			
	byCom1 = SETBIT(byCom1, COM1_CLK_EN_BIT);		// Disable ZR36050 clock
	byCom1 = SETBIT(byCom1, COM1_INT_EN_BIT);		// Disable ZR36050 interrupts
 	m_IO.Output(wIOBase | ZR_COM1_OFFSET, byCom1);

	m_nEncoderState = MICO_ENCODER_OFF;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CZoran::StartDecoding()
{
	m_dwDecodedFrames = 0;

	ZR36055InitForDecoding();
	ZR36050InitForDecoding();

	m_nDecoderState = MICO_DECODER_ON;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CZoran::StopDecoding()
{
	WORD wIOBase	= 0x200;
	BYTE byCom0		= 0x00;
	BYTE byCom1		= 0x00;

	if (m_pConf)
		wIOBase = m_pConf->wIOBase[MICO_EXTCARD0];

	// Commandregister 1 initialisieren (COM1 = 0x03)
	byCom1 = m_IO.Input(wIOBase | ZR_COM1_OFFSET);
	byCom1 = SETBIT(byCom1, COM1_ACTIVE_BIT);		// Inactive			
	byCom1 = CLRBIT(byCom1, COM1_CLK_EN_BIT);		// Disable ZR36050 clock
	byCom1 = CLRBIT(byCom1, COM1_INT_EN_BIT);		// Disable ZR36050 interrupts
 	m_IO.Output(wIOBase | ZR_COM1_OFFSET, byCom1);

	byCom0 = m_IO.Input(wIOBase | ZR_COM0_OFFSET);
	byCom0 = CLRBIT(byCom0, COM0_EXP_BIT);			// Compression
  	m_IO.Output(wIOBase | ZR_COM0_OFFSET, byCom0);

	m_nDecoderState = MICO_DECODER_OFF;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
int  CZoran::GetEncoderState()
{
 	return m_nEncoderState;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
int	 CZoran::GetDecoderState()
{
 	return m_nDecoderState;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
DWORD CZoran::WriteData(LPCSTR pData, DWORD dwDataLen)
{
	BYTE  byCom0	= 0;
	BYTE  byFCNT	= 0;
	BYTE  byLen0	= 0;
	BYTE  byLen1	= 0;
	BYTE  byStatus0	= 0;
	DWORD  dwTimeOut= 0;
	WORD  *pWord	= NULL;
	WORD wIOBase	= 0x200;
	
	if (m_pConf)
		wIOBase = m_pConf->wIOBase[MICO_EXTCARD0];

	m_dwDecodedFrames++;

	if (m_dwDecodedFrames <= 2)
	{
		pWord = (WORD*)pData;

		for (DWORD dwI = 0; dwI < dwDataLen / 2; dwI++)
			m_IO.Outputw(wIOBase | ZR_CB_DATA_OFFSET, pWord[dwI]);	

		// Setze nächste Codebuffer page number
		byCom0 = m_IO.Input(wIOBase | ZR_COM0_OFFSET);
		
		if (TSTBIT(byCom0, STATUS0_PG0_BIT))
			byCom0 = CLRBIT(byCom0, STATUS0_PG0_BIT);
		else
			byCom0 = SETBIT(byCom0, STATUS0_PG0_BIT);

		m_IO.Output(wIOBase | ZR_COM0_OFFSET, byCom0);
	}
	else
	{
		// Wait until Ready
		dwTimeOut = GetTickCount();
		while (!TSTBIT(m_IO.Input(wIOBase | ZR_STATUS0_OFFSET), STATUS0_RDY_BIT))
		{
			if (GetTickCount() > dwTimeOut + 1000)
			{
				WK_TRACE_ERROR("CZoran::WriteData\tTimeOut\n");
				return 0;
			}
			Sleep(0);
		}
		
		// Read Status
		byStatus0	= m_IO.Input(wIOBase | ZR_STATUS0_OFFSET);
		byFCNT		= m_IO.Input(wIOBase | ZR_FCNT_OFFSET);
		byLen0		= m_IO.Input(wIOBase | ZR_LEN0_OFFSET);
		byLen1		= m_IO.Input(wIOBase | ZR_LEN1_OFFSET);

		// Daten in den Globalen Hilfs-JPEG-Buffer kopieren (Siehe oben)
		WORD wLen = dwDataLen / 2;	// 16 Bit Zugriff
		WORD wCBData = wIOBase | ZR_CB_DATA_OFFSET;
		pWord = (WORD*)pData;
#if (1)
		for (WORD wI = 0; wI < wLen; wI++)
			m_IO.Outputw(wCBData, pWord[wI]);	
#else
		// Daten in den Codebuffer schreiben
		_asm{
 			  movzx     ecx, wLen
			  mov       dx,  wCBData
			  mov       edi, pWord
			  cld

			  test      cx,cx
			  jz        LAB_END
			  rep		outsw
			LAB_END:
		}
#endif

		// Setze nächste Codebuffer page number
		byCom0 = m_IO.Input(wIOBase | ZR_COM0_OFFSET);
		if (TSTBIT(byCom0, STATUS0_PG0_BIT))
			byCom0 = CLRBIT(byCom0, STATUS0_PG0_BIT);
		else
			byCom0 = SETBIT(byCom0, STATUS0_PG0_BIT);
		m_IO.Output(wIOBase | ZR_COM0_OFFSET, byCom0);

		// Read Status
		byStatus0	= m_IO.Input(wIOBase | ZR_STATUS0_OFFSET);
		byFCNT		= m_IO.Input(wIOBase | ZR_FCNT_OFFSET);
		byLen0		= m_IO.Input(wIOBase | ZR_LEN0_OFFSET);
		byLen1		= m_IO.Input(wIOBase | ZR_LEN1_OFFSET);

	}
	return dwDataLen;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
JPEG* CZoran::ReadData(int &nErrorCode)
{
	static	char byBuffer[JPEG_BUFFER_SIZE];
	static	JPEG Jpeg;
	WORD	wBufferID	= 0;

	nErrorCode			= MICO_ERR_NO_ERRROR;

	if (!m_pConf)
	{
		nErrorCode = MICO_ERROR;
		return NULL;
	}
	
	// Warte bis der MiCo.vxd ein Event auslöst
	DWORD dwRet = WaitForFrameEvent();

	switch(dwRet)
	{
		case WAIT_TIMEOUT:
			if (GetEncoderState() == MICO_ENCODER_OFF)
				nErrorCode = MICO_ERR_ENCODER_OFF;
			else
			{
				// Encoder neu anwerfen.
				WK_TRACE_WARNING("Encodertimout\n");
				StopEncoding();
				StartEncoding();
				nErrorCode = MICO_ERR_TIMEOUT;
			}
			return NULL;
		case WAIT_FAILED:
			WK_TRACE_ERROR("CZoran::ReadData\tWAIT_FAILED\n");
			nErrorCode = MICO_ERR_WAITFAILED;
			return NULL;
		case WAIT_OBJECT_0:
			break;
		default:
			nErrorCode = MICO_ERROR;
			WK_TRACE_WARNING("CZoran::ReadData\tUnknown returnvalue\n");
			return NULL;
	}

	// Gültige Bilddaten vorhanden?
	if (m_pConf && !m_pConf->Jpeg[m_wReadIndex].bValid)
	{
		nErrorCode = MICO_ERR_UNVALID_FIELD;
		return NULL;
	}

	if (m_pConf)
	{
		Jpeg.dwProzessID= m_pConf->Jpeg[m_wReadIndex].dwProzessID;
		Jpeg.wSource	= m_pConf->Jpeg[m_wReadIndex].wSource;
		Jpeg.wExtCard	= m_pConf->Jpeg[m_wReadIndex].wExtCard;
		Jpeg.dwLen		= m_pConf->Jpeg[m_wReadIndex].dwLen;
		Jpeg .dwTime	= m_pConf->Jpeg[m_wReadIndex].dwTime;
		Jpeg.dwFieldID	= m_pConf->Jpeg[m_wReadIndex].dwFieldID;
		Jpeg.wField		= m_pConf->Jpeg[m_wReadIndex].wField;	
		Jpeg.byFCNT		= m_pConf->Jpeg[m_wReadIndex].byFCNT;	// For debug only
		Jpeg.byPage		= m_pConf->Jpeg[m_wReadIndex].byPage;	// For debug only
		Jpeg.lpBuffer	= byBuffer;
		memcpy(&Jpeg.lpBuffer[0], &m_pConf->Jpeg[m_wReadIndex].lpBuffer[0], Jpeg.dwLen);
	}

	// Buffer freigeben
	if (m_pConf)
		m_pConf->Jpeg[m_wReadIndex].bValid = FALSE;

	// Leseposition weitersetzen
	m_wReadIndex++;
	if (m_wReadIndex == MAX_JPEG_BUFFER)
		m_wReadIndex = 0;

	return &Jpeg;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
DWORD CZoran::WaitForFrameEvent()
{
	if (!m_pConf)
		return WAIT_FAILED;

	if (m_pConf->Jpeg[m_wReadIndex].bValid)
		return WAIT_OBJECT_0;

	return WaitForSingleObject(m_hEvent, m_dwEncoderTimeout);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CZoran::EnableBrdIRQ()
{
	if (m_pConf)
	{
		BYTE byStatus = m_IO.Input(m_pConf->wIOBase[MICO_EXTCARD0] | MICO_IER_OFFSET);
		byStatus = CLRBIT(byStatus, VI_IE_BIT);	   // Video-IRQ sperren
		byStatus = SETBIT(byStatus, ZR_IE_BIT);	   // Zoran-IRQ freigeben
		byStatus = CLRBIT(byStatus, VID_AES_BIT);  // VSync bei steigende Flanke
		m_IO.Output(m_pConf->wIOBase[MICO_EXTCARD0] | MICO_IER_OFFSET, byStatus);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CZoran::DisableBrdIRQ()
{
	if (m_pConf)
	{
		BYTE byStatus = m_IO.Input(m_pConf->wIOBase[MICO_EXTCARD0] | MICO_IER_OFFSET);
		byStatus = CLRBIT(byStatus, VI_IE_BIT);		// Video-IRQ sperren
		byStatus = CLRBIT(byStatus, ZR_IE_BIT);		// Zoran-IRQ sperren
		m_IO.Output(m_pConf->wIOBase[MICO_EXTCARD0] | MICO_IER_OFFSET, byStatus);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CZoran::SetIRQNumber(WORD wIRQ)
{
	BYTE byTbl[16] = {0,0,0,0,0,1,0,0,0,0,2,3,4,0,5,6}; // IRQ 5, 10, 11, 12, 14, 15
	BYTE byStatus;
		
	if (wIRQ > 15)
	{
		CString sError;

		sError.LoadString(IDS_RTE_UNKNOWN_IRQ);
		sError.Format(sError, wIRQ);

		WK_TRACE_ERROR(sError);
		return;
	}

	if (byTbl[wIRQ] == 0)
	{
		CString sError;

		sError.LoadString(IDS_RTE_UNKNOWN_IRQ);
		sError.Format(sError, wIRQ);

		WK_TRACE_ERROR(sError);
		return;
	}

	if (m_pConf)
	{	
		byStatus = m_IO.Input(m_pConf->wIOBase[MICO_EXTCARD0] + MICO_IER_OFFSET);
		byStatus &= 0xf8;
		m_IO.Output(m_pConf->wIOBase[MICO_EXTCARD0] + MICO_IER_OFFSET, byStatus | byTbl[wIRQ]);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CZoran::ChipSelectVideoInAnalogOut(WORD wFrontEnd, WORD wExtCard, BYTE byData)
{
	if (wExtCard == MICO_EXTCARD0)
	{
		if (wFrontEnd == MICO_FE0)
		{
			if (m_pConf)
				m_IO.WriteToLatch(m_pConf->wIOBase[wExtCard] | CSVINAN1_OFFSET, byData);
		}
		else if (wFrontEnd == MICO_FE1)
		{
			if (m_pConf)
				m_IO.WriteToLatch(m_pConf->wIOBase[wExtCard] | CSVINAN2_OFFSET, byData);
		}
	}
	else
	{
		if (m_pConf)
			m_IO.WriteToLatch(m_pConf->wIOBase[wExtCard] | CSVINAN_COVI_OFFSET, byData);
	}
}

/////////////////////////////////////////////////////////////////////////////
BYTE CZoran::ChipSelectVideoInAnalogIn(WORD wFrontEnd, WORD wExtCard)
{
	if (!m_pConf)
		return 0;

	if (wExtCard == MICO_EXTCARD0)
	{
		if (wFrontEnd == MICO_FE0)
			return m_IO.ReadFromLatch(m_pConf->wIOBase[wExtCard] | CSVINAN1_OFFSET);
		else if (wFrontEnd == MICO_FE1)
			return m_IO.ReadFromLatch(m_pConf->wIOBase[wExtCard] | CSVINAN2_OFFSET);
		else
			return 0;
	}
	else
		return m_IO.ReadFromLatch(m_pConf->wIOBase[wExtCard] | CSVINAN_COVI_OFFSET);
}

/////////////////////////////////////////////////////////////////////////////
void CZoran::ChipSelectVideoInDigitalOut(BYTE byData)
{
	if (m_pConf)
		m_IO.WriteToLatch(m_pConf->wIOBase[MICO_EXTCARD0] | CSVINDN_OFFSET, byData);
}

/////////////////////////////////////////////////////////////////////////////
BYTE CZoran::ChipSelectVideoInDigitalIn()
{
	if (!m_pConf)
		return 0;

	return m_IO.ReadFromLatch(m_pConf->wIOBase[MICO_EXTCARD0] | CSVINDN_OFFSET);
}

/////////////////////////////////////////////////////////////////////////////
void CZoran::ZR36055InitForEncoding()
{
	WORD wIOBase = 0x200;
	BYTE byCom0	 = 0;
	BYTE byCom1	 = 0;
	BYTE byCom2	 = 0;

	if (m_pConf)
		wIOBase = m_pConf->wIOBase[MICO_EXTCARD0];

	// Commandregister 0 initialisieren (COM0 = 0xd0)
	byCom0 = m_IO.Input(wIOBase | ZR_COM0_OFFSET);
	byCom0 = CLRBIT(byCom0, COM0_PCG0_BIT);			// Codebufferpage 0
	byCom0 = CLRBIT(byCom0, COM0_PCG1_BIT);			// Codebufferpage 0
	byCom0 = CLRBIT(byCom0, COM0_STILL_BIT);		// Contininuous compression
	byCom0 = CLRBIT(byCom0, COM0_EXP_BIT);			// Compression
	byCom0 = SETBIT(byCom0, COM0_VID_422_BIT);		// Irrelevant for Compression
	byCom0 = CLRBIT(byCom0, COM0_GRESET_BIT);		// Reset ZR36055
	byCom0 = SETBIT(byCom0, COM0_FFI_BIT);			// Irrelevant for Compression
	byCom0 = SETBIT(byCom0, COM0_FAST_BIT);			// OOPS? FAST-Clock 
  	m_IO.Output(wIOBase | ZR_COM0_OFFSET, byCom0);

	// Commandregister 1 initialisieren (COM1 = 0x13)
	byCom1 = m_IO.Input(wIOBase | ZR_COM1_OFFSET);
	byCom1 = SETBIT(byCom1, COM1_ACTIVE_BIT);		// Inactive			
	byCom1 = SETBIT(byCom1, COM1_CB_SIZE_BIT);		// 256KByte VRAM
	byCom1 = CLRBIT(byCom1, COM1_VER_DEC_BIT);		// Irrelevant for Compression

	if (m_wFormat == MICO_ENCODER_HIGH)
		byCom1 = CLRBIT(byCom1, COM1_HOR_DEC_BIT);	// No horizontal decimation		
	else if (m_wFormat == MICO_ENCODER_LOW)
		byCom1 = SETBIT(byCom1, COM1_HOR_DEC_BIT);	// Horizontal decimation		

	byCom1 = SETBIT(byCom1, COM1_SNC_SRC_BIT);		// Internal Syncronization
	byCom1 = CLRBIT(byCom1, COM1_CLK_EN_BIT);		// Disable ZR36050 clock
	byCom1 = CLRBIT(byCom1, COM1_INT_EN_BIT);		// Disable ZR36050 interrupts
	
	if ((m_wFeVideoFormat == MICO_PAL_SQUARE) || (m_wFeVideoFormat == MICO_PAL_CCIR))
		byCom1 = CLRBIT(byCom1, COM1_VID_FMT_BIT);	// PAL Videoformat	
  	else if ((m_wFeVideoFormat == MICO_NTSC_SQUARE) || (m_wFeVideoFormat == MICO_NTSC_CCIR))
		byCom1 = SETBIT(byCom1, COM1_VID_FMT_BIT);	// NTSC Videoformat;	
 	m_IO.Output(wIOBase | ZR_COM1_OFFSET, byCom1);

	// Set operationg mode (COM0 = 0xd3)
	byCom0 = m_IO.Input(wIOBase | ZR_COM0_OFFSET);
	byCom0 = SETBIT(byCom0, COM0_PCG0_BIT);			// Codebufferpage 3
	byCom0 = SETBIT(byCom0, COM0_PCG1_BIT);			// Codebufferpage 3
  	m_IO.Output(wIOBase | ZR_COM0_OFFSET, byCom0);
	
	// Set operationg control (COM1 = 0x03)
	m_IO.Output(wIOBase | ZR_COM1_OFFSET, byCom1);	

	//  (COM2 = 0x80)
	byCom2 = m_IO.Input(wIOBase | ZR_COM2_OFFSET);
	byCom2 = CLRBIT(byCom2, COM2_ID0_BIT);			// ZR36055 ID
	byCom2 = CLRBIT(byCom2, COM2_ID1_BIT);			// ZR36055 ID
	byCom2 = CLRBIT(byCom2, COM2_ID2_BIT);			// ZR36055 ID
	byCom2 = CLRBIT(byCom2, COM2_ID3_BIT);			// ZR36055 ID
	byCom2 = CLRBIT(byCom2, COM2_ID4_BIT);			// ZR36055 ID
	byCom2 = CLRBIT(byCom2, COM2_ID5_BIT);			// ZR36055 ID
	byCom2 = CLRBIT(byCom2, COM2_VID_FILT_BIT);		// No Video filter
	byCom2 = SETBIT(byCom2, COM2_STRP_SIZE_BIT);	// 32 KByte SRAM
	m_IO.Output(wIOBase | ZR_COM2_OFFSET, byCom2);

	// Set HSTART, HEND, HTOTAL, VSTART, VEND, VTOTAL and NMCU
	m_IO.Outputw(wIOBase | ZR_SIZE_OFFSET, 0x0000 | m_wFeHStart[m_wFeVideoFormat]);	// Set HSTART
	m_IO.Outputw(wIOBase | ZR_SIZE_OFFSET, 0x1000 | m_wFeHEnd[m_wFeVideoFormat]-1);		// Set HEND
	m_IO.Outputw(wIOBase | ZR_SIZE_OFFSET, 0x2000 | m_wFeHTotal[m_wFeVideoFormat]);	// Set HTOTAL
	m_IO.Outputw(wIOBase | ZR_SIZE_OFFSET, 0x8000 | m_wFeVStart[m_wFeVideoFormat]);	// Set VSTART
	m_IO.Outputw(wIOBase | ZR_SIZE_OFFSET, 0x9000 | m_wFeVEnd[m_wFeVideoFormat]-1);		// Set VEND
	m_IO.Outputw(wIOBase | ZR_SIZE_OFFSET, 0xa000 | m_wFeVTotal[m_wFeVideoFormat]);	// Set VTOTAL
	m_IO.Outputw(wIOBase | ZR_SIZE_OFFSET, 0x3000 | m_wFeHSStart[m_wFeVideoFormat]);		// Set HSSTART
	m_IO.Outputw(wIOBase | ZR_SIZE_OFFSET, 0x4000 | m_wFeHSEnd[m_wFeVideoFormat]);		// Set HSEND
	m_IO.Outputw(wIOBase | ZR_SIZE_OFFSET, 0xc000 | m_wFeNMCU[m_wFeVideoFormat]);		// Set NMCU
	m_IO.Outputw(wIOBase | ZR_SIZE_OFFSET, 0x5000 | m_wFeBlankStart[m_wFeVideoFormat]);		// Set HSEND
	m_IO.Outputw(wIOBase | ZR_SIZE_OFFSET, 0xe000 | m_wYBackgnd); // Set YBackgnd
	m_IO.Outputw(wIOBase | ZR_SIZE_OFFSET, 0xe800 | m_wUVBackgnd);// Set UVBackgnd

	// Enable ZR36050 clock (COM1 = 0x23)
	byCom1 = m_IO.Input(wIOBase | ZR_COM1_OFFSET);
	byCom1 = SETBIT(byCom1, COM1_CLK_EN_BIT);		
	m_IO.Output(wIOBase | ZR_COM1_OFFSET, byCom1);		

	Sleep(1);	// Wait minimal 0.5 ms

	// Deactivate GRESET (COM0 = 0xf3)
	byCom0 = m_IO.Input(wIOBase | ZR_COM0_OFFSET);
	byCom0 = SETBIT(byCom0, COM0_GRESET_BIT);
  	m_IO.Output(wIOBase | ZR_COM0_OFFSET, byCom0);
}

/////////////////////////////////////////////////////////////////////////////
void CZoran::ZR36050InitForEncoding()
{
 	WORD	wIOBase			= 0x200;
	BYTE	byCom1			= 0x00;
	WORD	wSamplesPerLine	= 0;
	WORD	wNumberOfLines	= 0;
	WORD    wMBCV			= 0;

	char	strSpace[64];

	memset(strSpace, 0x20, 64);

	if (m_pConf)
		wIOBase = m_pConf->wIOBase[MICO_EXTCARD0];

	wSamplesPerLine = m_wFeHEnd[m_wFeVideoFormat] - m_wFeHStart[m_wFeVideoFormat]; 
	wNumberOfLines	= m_wFeVEnd[m_wFeVideoFormat] - m_wFeVStart[m_wFeVideoFormat];

	// Horizontale Dezimierung ein ?
	if (m_wFormat == MICO_ENCODER_LOW)
		wSamplesPerLine /= 2;
	
//	wMBCV	= 8L * 64L * (DWORD)m_wBPF / ((DWORD)wSamplesPerLine * (DWORD)wNumberOfLines); 
	wMBCV = 510; 

	Z050_CODER_CONFIG Cfg;
	Cfg.Width  = wSamplesPerLine;
	Cfg.Height = wNumberOfLines;
	Cfg.HSampRatio[0] = 2;
	Cfg.VSampRatio[0] = 1;
	Cfg.HSampRatio[1] = 1;
	Cfg.VSampRatio[1] = 1;
	Cfg.HSampRatio[2] = 1;
	Cfg.VSampRatio[2] = 1;
	Cfg.TotalCodeVolume = m_wBPF;
	Cfg.MaxBlockCodeVolume = wMBCV;
	Cfg.ScaleFactor = 1*0x100;
	Cfg.DRI = 1;
	Cfg.QTable = (char FAR *)&DefaultQTables[0];
	Cfg.HuffTable = (char FAR *)&DefaultHuffTables[0];
	Cfg.APPString = strSpace;
	Cfg.COMString = strSpace;
	if (m_pzCoder)
		m_pzCoder->CoderConfigureCompression((LP_Z050_CODER_CONFIG)&Cfg);
  	
	ReadFromZR36050(0);	// Hardware GO

	// Get Status
	m_IO.Input(wIOBase | ZR_FCNT_OFFSET);
	m_IO.Input(wIOBase | ZR_STATUS0_OFFSET);
	m_IO.Input(wIOBase | ZR_LEN0_OFFSET);
	m_IO.Input(wIOBase | ZR_LEN1_OFFSET);

	// Activate
	byCom1 = m_IO.Input(wIOBase | ZR_COM1_OFFSET);
	byCom1 = CLRBIT(byCom1, COM1_ACTIVE_BIT);	// Activate
	byCom1 = SETBIT(byCom1, COM1_INT_EN_BIT);	// Enable Interrupt
	m_IO.Output(wIOBase | ZR_COM1_OFFSET, byCom1); 
}

/////////////////////////////////////////////////////////////////////////////
void CZoran::ZR36055InitForDecoding()
{
	WORD wIOBase = 0x200;
	BYTE byCom0;
	BYTE byCom1;
	BYTE byCom2;

	if (m_pConf)
		wIOBase = m_pConf->wIOBase[MICO_EXTCARD0];

	// Commandregister 0 initialisieren (COM0 = 0xd0)
	byCom0 = m_IO.Input(wIOBase | ZR_COM0_OFFSET);
	byCom0 = CLRBIT(byCom0, COM0_PCG0_BIT);			// Codebufferpage 0
	byCom0 = CLRBIT(byCom0, COM0_PCG1_BIT);			// Codebufferpage 0
	byCom0 = CLRBIT(byCom0, COM0_STILL_BIT);		// Contininuous expansion
	byCom0 = CLRBIT(byCom0, COM0_EXP_BIT);			// Compression
	byCom0 = SETBIT(byCom0, COM0_VID_422_BIT);		// YUV422
	byCom0 = CLRBIT(byCom0, COM0_GRESET_BIT);		// Reset ZR36055
	byCom0 = SETBIT(byCom0, COM0_FFI_BIT);			// First expanded field is field I
	byCom0 = SETBIT(byCom0, COM0_FAST_BIT);			// OOPS? FAST-Clock 
  	m_IO.Output(wIOBase | ZR_COM0_OFFSET, byCom0);

	// Commandregister 1 initialisieren (COM1 = 0x03)
	byCom1 = m_IO.Input(wIOBase | ZR_COM1_OFFSET);
	byCom1 = SETBIT(byCom1, COM1_ACTIVE_BIT);		// Inactive			
	byCom1 = SETBIT(byCom1, COM1_CB_SIZE_BIT);		// 256KByte VRAM

	// OOPS Ich weiß nicht genau ob die Dezimierung benötigt wird
	byCom1 = CLRBIT(byCom1, COM1_VER_DEC_BIT);		// No vert. decimation
	byCom1 = CLRBIT(byCom1, COM1_HOR_DEC_BIT);		// No hor. decimation or interpolation		
	byCom1 = CLRBIT(byCom1, COM1_SNC_SRC_BIT);		// OOPS? External Syncronization
	byCom1 = CLRBIT(byCom1, COM1_CLK_EN_BIT);		// Disable ZR36050 clock
	byCom1 = CLRBIT(byCom1, COM1_INT_EN_BIT);		// Disable ZR36050 interrupts
	if ((m_wBeVideoFormat == MICO_PAL_SQUARE) || (m_wBeVideoFormat == MICO_PAL_CCIR))
		byCom1 = CLRBIT(byCom1, COM1_VID_FMT_BIT);	// PAL Videoformat	
  	else if ((m_wBeVideoFormat == MICO_NTSC_SQUARE) || (m_wBeVideoFormat == MICO_NTSC_CCIR))
		byCom1 = SETBIT(byCom1, COM1_VID_FMT_BIT);	// NTSC Videoformat;	
 	m_IO.Output(wIOBase | ZR_COM1_OFFSET, byCom1);

	// Set operationg mode (COM0 = 0xd8)
	byCom0 = m_IO.Input(wIOBase | ZR_COM0_OFFSET);
	byCom0 = SETBIT(byCom0, COM0_EXP_BIT);			// Expansion
  	m_IO.Output(wIOBase | ZR_COM0_OFFSET, byCom0);

	// Set operationg control (COM1 = 0x17) (COM2 = 0x80)
	byCom1 = m_IO.Input(wIOBase | ZR_COM1_OFFSET);
	byCom1 = SETBIT(byCom1, COM1_VER_DEC_BIT);		// Vert. decimation or interpolation		
	byCom1 = SETBIT(byCom1, COM1_SNC_SRC_BIT);		// Internal Synchronisation
	m_IO.Output(wIOBase | ZR_COM1_OFFSET, byCom1);	
	
	byCom2 = m_IO.Input(wIOBase | ZR_COM2_OFFSET);
	byCom2 = SETBIT(byCom2, COM2_ID0_BIT);			// ZR36055 ID
	byCom2 = CLRBIT(byCom2, COM2_ID1_BIT);			// ZR36055 ID
	byCom2 = SETBIT(byCom2, COM2_ID2_BIT);			// ZR36055 ID
	byCom2 = CLRBIT(byCom2, COM2_ID3_BIT);			// ZR36055 ID
	byCom2 = SETBIT(byCom2, COM2_ID4_BIT);			// ZR36055 ID
	byCom2 = CLRBIT(byCom2, COM2_ID5_BIT);			// ZR36055 ID
	byCom2 = CLRBIT(byCom2, COM2_VID_FILT_BIT);		// No Video filter
	byCom2 = SETBIT(byCom2, COM2_STRP_SIZE_BIT);	// 32 KByte SRAM
	m_IO.Output(wIOBase | ZR_COM2_OFFSET, byCom2);

	// Set HSTART, HEND, HTOTAL, VSTART, VEND, VTOTAL and NMCU
	m_IO.Outputw(wIOBase | ZR_SIZE_OFFSET, 0x0000 | m_wBeHStart);		// Set HSTART
	m_IO.Outputw(wIOBase | ZR_SIZE_OFFSET, 0x1000 | m_wBeHEnd-1);		// Set HEND
	m_IO.Outputw(wIOBase | ZR_SIZE_OFFSET, 0x2000 | m_wBeHTotal);		// Set HTOTAL
	m_IO.Outputw(wIOBase | ZR_SIZE_OFFSET, 0x3000 | m_wBeHSStart);		// Set HSSTART
	m_IO.Outputw(wIOBase | ZR_SIZE_OFFSET, 0x4000 | m_wBeHSEnd);		// Set HSEND
	
	m_IO.Outputw(wIOBase | ZR_SIZE_OFFSET, 0x8000 | m_wBeVStart);		// Set VSTART
	m_IO.Outputw(wIOBase | ZR_SIZE_OFFSET, 0x9000 | m_wBeVEnd-1);			// Set VEND
	m_IO.Outputw(wIOBase | ZR_SIZE_OFFSET, 0xa000 | m_wBeVTotal);		// Set VTOTAL
	m_IO.Outputw(wIOBase | ZR_SIZE_OFFSET, 0xc000 | m_wBeNMCU);			// Set NMCU
	m_IO.Outputw(wIOBase | ZR_SIZE_OFFSET, 0x5000 | m_wBeBlankStart);	// Set BLANKSTART

	m_IO.Outputw(wIOBase | ZR_SIZE_OFFSET, 0xe000 | m_wYBackgnd);		// Set YBackgnd
	m_IO.Outputw(wIOBase | ZR_SIZE_OFFSET, 0xe000 | m_wUVBackgnd);		// Set UVBackgnd

	// Enable ZR36050 clock (COM1 = 0x23)
	byCom1 = m_IO.Input(wIOBase | ZR_COM1_OFFSET);
	byCom1 = SETBIT(byCom1, COM1_CLK_EN_BIT);		
	m_IO.Output(wIOBase | ZR_COM1_OFFSET, byCom1);		

	Sleep(1);	// Wait minimal 0.5 ms

	// Deactivate GRESET (COM0 = 0xf3)
	byCom0 = m_IO.Input(wIOBase | ZR_COM0_OFFSET);
	byCom0 = SETBIT(byCom0, COM0_GRESET_BIT);
  	m_IO.Output(wIOBase | ZR_COM0_OFFSET, byCom0);
}

/////////////////////////////////////////////////////////////////////////////
void CZoran::ZR36050InitForDecoding()
{
 	WORD	wIOBase			= 0x200;
	BYTE	byCom1			= 0x00;
	
	if (m_pConf)
		wIOBase = m_pConf->wIOBase[MICO_EXTCARD0];

	Z050_CODER_CONFIG Cfg;
	Cfg.HuffTable = (char FAR *)&DefaultHuffTables[0];
	if (m_pzCoder)
		m_pzCoder->CoderConfigureExpansion((LP_Z050_CODER_CONFIG)&Cfg);

	ReadFromZR36050(0x0000);				// Go...(Siehe S. 22)

	// Get Status
	m_IO.Input(wIOBase | ZR_FCNT_OFFSET);
	m_IO.Input(wIOBase | ZR_LEN0_OFFSET);
	m_IO.Input(wIOBase | ZR_LEN1_OFFSET);

	// Activate
	byCom1 = m_IO.Input(wIOBase | ZR_COM1_OFFSET);
	byCom1 = CLRBIT(byCom1, COM1_ACTIVE_BIT);	// Activate
	m_IO.Output(wIOBase | ZR_COM1_OFFSET, byCom1); 
}

/////////////////////////////////////////////////////////////////////////////
inline void CZoran::WriteToZR36050(WORD wAdr, BYTE byValue)
{
	WORD wIOBase = 0x200;
	
	if (m_pConf)
		wIOBase = m_pConf->wIOBase[MICO_EXTCARD0];

	_asm cli
	m_IO.Outputw(wIOBase | ZR_ADDRESS_OFFSET, wAdr);	// Write Adress
	m_IO.Output(wIOBase  | ZR_DATA_OFFSET,	byValue);	// Write Data
	_asm sti
}

/////////////////////////////////////////////////////////////////////////////
inline BYTE CZoran::ReadFromZR36050(WORD wAdr)
{
	WORD wIOBase = 0x200;
	
	if (m_pConf)
		wIOBase = m_pConf->wIOBase[MICO_EXTCARD0];

	_asm cli
	m_IO.Outputw(wIOBase | ZR_ADDRESS_OFFSET, wAdr);	// Write Adress
	BYTE byVal = m_IO.Input(wIOBase | ZR_DATA_OFFSET);	// Read Data
	_asm sti

	return byVal;
}

/////////////////////////////////////////////////////////////////////////////
void CZoran::TraceEncoderQueue()
{
	WORD wI;
	CString s;

	s="";
	for (wI = 0; wI < m_wReadIndex; wI++)
		s=s+" ";
	s=s+"R";
	WK_TRACE("%s\n", s);

	s="";
	for (wI = 0; wI < MAX_JPEG_BUFFER; wI++)
	{
		if (m_pConf->Jpeg[wI].bValid)
		{
			s=s+"1";
		}
		else
		{
			s=s+"0";
		}
	}

	WK_TRACE("%s\n", s);
}

