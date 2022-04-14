/////////////////////////////////////////////////////////////////////////////
// PROJECT:		CoCoUnit
// FILE:		$Workfile: Ccoco.cpp $
// ARCHIVE:		$Archive: /Project/Units/CoCoUnit/Ccoco.cpp $
// CHECKIN:		$Date: 29.11.02 8:59 $
// VERSION:		$Revision: 23 $
// LAST CHANGE:	$Modtime: 29.11.02 8:53 $
// BY AUTHOR:	$Author: Georg.feddern $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CipcOutput.h" // Wegen WK_TRACE
#include "wk.h"
#include "wk_file.h"
#include "mmsystem.h"
#include "message.h"
#include "dlgcolor.h"
#include "cpydata.h"
#include "ccoco.h"
#include "WK_RuntimeError.h"
#include "WK_Dongle.h"
#include "Resource.h"

// Buffersize
#define SIZE 255		

/////////////////////////////////////////////////////////////////////////////
// CCoCo::CCoCo
CCoCo::CCoCo(HWND hWnd, LPCSTR lpszAppIniName)
{
	char szBuffer[_MAX_PATH];

	// Default-Initialisierung
	m_bOK				= FALSE;
	m_hWndClient		= hWnd;
	m_hWndServer		= NULL;
	m_wLastError		= COCO_ERR_NO_ERRROR;
	m_pDlgColor			= NULL;
	m_byCoCoID			= COCO_COCOID0;
	m_wExtCard			= COCO_EXTCARD0;
	m_wSource			= COCO_SOURCE0;
	m_dwTimeOut			= 5000;
	m_pCopyData			= NULL;
	m_bIgnoreRelais4	= FALSE;
	m_hThisModule		= AfxGetInstanceHandle();

	for (WORD wID = 0; wID <= MAX_COCO_ID; wID++)
	{
		m_CoCoISA[wID].wExtCard			= COCO_EXTCARD0;
		m_CoCoISA[wID].wFeFilter		= 0;
		m_CoCoISA[wID].wBeVideoFormat	= COCO_PAL;
		m_CoCoISA[wID].wBeVideoType		= COCO_COMPOSITE;
		m_CoCoISA[wID].wFeHScale		= COCO_MIN_HSCALE;
		m_CoCoISA[wID].wFeVScale		= COCO_MIN_VSCALE;
		m_CoCoISA[wID].wFormat			= COCO_ENCODER_QCIF;
		m_CoCoISA[wID].wFramerate		= 1;
		m_CoCoISA[wID].dwDecBitrate		= 64000;
		m_CoCoISA[wID].dwEncBitrate		= 64000;

		for (WORD wCrd = COCO_EXTCARD0; wCrd <= COCO_EXTCARD0   + COCO_MAX_EXTCARD; wCrd++)
		{
 			m_CoCoISA[wID].ExtCard[wCrd].wSource = COCO_SOURCE0;
			m_CoCoISA[wID].ExtCard[wCrd].wEdge	 = 0;
			m_CoCoISA[wID].ExtCard[wCrd].wAck	 = 0;
			m_CoCoISA[wID].ExtCard[wCrd].wRelais = 0;
			
			for (WORD wSrc = COCO_SOURCE0; wSrc <= COCO_SOURCE0 + COCO_MAX_SOURCE; wSrc++)
			{
				m_CoCoISA[wID].ExtCard[wCrd].Source[wSrc].wFeVideoFormat = COCO_PAL;
				m_CoCoISA[wID].ExtCard[wCrd].Source[wSrc].wFeSourceType	 = COCO_FBAS;
				m_CoCoISA[wID].ExtCard[wCrd].Source[wSrc].wFeBrightness	 = (COCO_MAX_BRIGHTNESS - COCO_MIN_BRIGHTNESS) / 2;
				m_CoCoISA[wID].ExtCard[wCrd].Source[wSrc].wFeContrast	 = (COCO_MAX_CONTRAST - COCO_MIN_CONTRAST) / 2;
				m_CoCoISA[wID].ExtCard[wCrd].Source[wSrc].wFeSaturation	 = (COCO_MAX_SATURATION - COCO_MIN_SATURATION) / 2;
				m_CoCoISA[wID].ExtCard[wCrd].Source[wSrc].wFeHue		 = (COCO_MAX_HUE - COCO_MIN_HUE) / 2;
				m_CoCoISA[wID].ExtCard[wCrd].Source[wSrc].nAVPortY		 = wSrc - COCO_SOURCE0;
				m_CoCoISA[wID].ExtCard[wCrd].Source[wSrc].nAVPortC		 = wSrc - COCO_SOURCE0;
			}
		}
	}
	
	// Ohne INI-Datei geht gar nichts
	if (!lpszAppIniName)
		return;

	// Pfad und Name der INI-Datei sichern
	strcpy(m_szAppIniName, lpszAppIniName);

	WK_TRACE("Benutze %s als Ini-Datei\n",lpszAppIniName);

	//m_bStandalone = 0-> CoCoUnit arbeitet mit Security, 1-> CoCoUnit ist Standalone
	m_bStandalone = GetPrivateProfileInt("COCOISA", "Mode",  0, m_szAppIniName);
	if (m_bStandalone)
		WK_TRACE("Running as 'standalone'\n");

	m_bIgnoreRelais4 = GetPrivateProfileInt("COCOISA", "IgnoreRelais4",  0, m_szAppIniName);
	if (m_bIgnoreRelais4)
		WK_TRACE("Ignoriere Relais 4\n");

	// Timeout beim Laden der Serverapplikation.
	m_dwTimeOut = (1000L * GetPrivateProfileInt("COCOISA", "SrvLoadTimeOut", 5, m_szAppIniName)); 

	// Name und kompletter Pfad des Servers aus der INI-Datei holen.
	GetPrivateProfileString("COCOISA", "SrvName", "COCOSRV.EXE", szBuffer, SIZE, m_szAppIniName); 
	m_sServerPathName = szBuffer;

	// Serverapplikation starten.
	m_hWndServer = LoadServerApp();
	if (!m_hWndServer)
		return;

	WK_TRACE("%s korrekt geladen\n", (LPCSTR)m_sServerPathName);

	// Objekt zur Kommunikation mittels WM_COPYDATA
	m_pCopyData = new CWmCopyData;

	if (!m_pCopyData)
		return;

	// Dialog anlegen.
	m_pDlgColor = new CDlgColor(this, m_hWndClient);
	
	if (!m_pDlgColor)
		return;

	if (!m_pDlgColor->IsValid())
		return;

    // 32Bit Anwendung bei der Serverapplikation registrieren
	if (!(BOOL)::SendMessage(m_hWndServer, COCO_OPEN, (WPARAM)m_hWndClient, (LPARAM)0L))
	{
		CString sError;
		sError.LoadString(IDS_RTE_COCOSRV_OPEN);
	
		WK_TRACE_ERROR(sError);

		CWK_RunTimeError RTerr(WAI_COCOUNIT, REL_RESTART_ME, RTE_CONFIGURATION, sError, 0, 0);
		RTerr.Send();

		return;
	}

	// Bitweise Kodierung der vorhandenen Hardware (CoCos, ExtCards)
	m_dwHardwareState = (DWORD)SendMessage(m_hWndServer, COCO_GET_STATE, 0, 0L);

 	// Initialisiere Objekt anhand der INI-Datei
	Init();

	// Schaltet auf die in 'Init' ermittelte Defaultkamera
	SetInputSource(m_byCoCoID, m_wExtCard, m_wSource);

	// Sofern in 'SetInputSource' noch nicht gemacht:
	SetFeSourceType(GetFeSourceType());
	SetFeVideoFormat(GetFeVideoFormat());
	SetFeBrightness(GetFeBrightness());
	SetFeContrast(GetFeContrast());
	SetFeSaturation(GetFeSaturation());
	SetFeHue(GetFeHue());

	GetPrivateProfileString("COCOISA", "AVP3InitScript", "COCO263.MCS", szBuffer,  SIZE, m_szAppIniName); 
	
	if (!SendMCIScript(COCO_COCOID0, szBuffer))
	{
		CString sError;
		sError.LoadString(IDS_RTE_MCI_SCRIPT);
	
		WK_TRACE_ERROR(sError);

		CWK_RunTimeError RTerr(WAI_COCOUNIT, REL_RESTART_ME, RTE_CONFIGURATION, sError, 0, 0);
		RTerr.Send();
		return;
	}

	m_bOK = TRUE;  
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::CCoCo
CCoCo::~CCoCo()
{
	CString sServerPath;
	CString sServerName;

	SaveConfig();

	// 32Bit Anwendung bei der Serverapplikation abmelden.
	// Nicht notwendig, da CoCoSrv auf das CoCounit pollt
//	::PostMessage(m_hWndServer, COCO_CLOSE, (WPARAM)m_hWndClient, 0L);

// Warte solange bis das Fenster von CoCoSrv geschlossen wurde 
/*	DWORD dwCurrentTime = GetCurrentTime();
	while (FindWindow(NULL, sServerName))
	{				
 		if (GetCurrentTime() > dwCurrentTime + m_dwTimeOut)
		{
			WK_TRACE_ERROR("Timeout beim Beenden von %s\n", (LPCSTR)sServerName);
			break;
		}
	}
*/ 
	delete m_pDlgColor;
	m_pDlgColor = NULL;

	delete m_pCopyData;
	m_pCopyData = NULL;
}
	  
/////////////////////////////////////////////////////////////////////////////
// CCoCo::IsValid
BOOL CCoCo::IsValid()
{
	return m_bOK;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::LoadServerApp
HWND CCoCo::LoadServerApp()
{			
	HWND hWndServer = NULL;
	UINT uRet;
	CString sServerPath;
	CString sServerName;

	if (m_hWndServer)
		return m_hWndServer;

	// Serverapplikation starten.
	WK_SplitPath(m_sServerPathName, sServerPath, sServerName);
	if (!FindWindow(NULL, sServerName))
	{
		if (m_bStandalone)
			uRet = WinExec(m_sServerPathName, SW_SHOW);
		else
			uRet = WinExec(m_sServerPathName, SW_HIDE);
 
		if (uRet <= 31)
		{
			CString sError,sFormat;
			sFormat.LoadString(IDS_RTE_LOAD_SERVER);
			sError.Format(sFormat, (LPCSTR)m_sServerPathName, uRet);
			
			WK_TRACE_ERROR(sError);

			CWK_RunTimeError RTerr(WAI_COCOUNIT, REL_RESTART_ME, RTE_CONFIGURATION, sError, 0, 0);
			RTerr.Send();

			return NULL;
		}
	}

	// Warte bis Fensterhandle der Serverapplikation ermittelt ist.
	DWORD dwCurrentTime = GetCurrentTime();
	while ((hWndServer = FindWindow(NULL, sServerName)) == FALSE)
	{				
 		if (GetCurrentTime() > dwCurrentTime + m_dwTimeOut)
		{
			CString sError;
			sError.LoadString(IDS_RTE_LOAD_SERVER_TIMEOUT);
			sError.Format(sError, (LPCSTR)m_sServerPathName);
			
			WK_TRACE_ERROR(sError);

			CWK_RunTimeError RTerr(WAI_COCOUNIT, REL_RESTART_ME, RTE_CONFIGURATION, sError, 0, 0);
			RTerr.Send();

			return NULL;
		}
	}
	return hWndServer;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::SaveConfig
BOOL CCoCo::SaveConfig()
{
	if (!IsValid())
		return FALSE;

	static char szBuffer[SIZE];
	memset(szBuffer, 0, sizeof(szBuffer));

	static char szSourceType[SIZE];
	memset(szSourceType, 0, sizeof(szSourceType));

	static char szVideoFormat[SIZE];
	memset(szVideoFormat, 0, sizeof(szVideoFormat));

	static char szSource[SIZE];
	memset(szSource, 0, sizeof(szSource));

	// Aktuelle Videoquelle sichern 
	wsprintf(szBuffer, "Source%u-%u-%u",m_byCoCoID, m_wExtCard, m_wSource);
	WritePrivateProfileString("COCOISA", "VideoSource", szBuffer, m_szAppIniName); 

	// *************************************************
	// Video Front End
	// *************************************************
 	for (WORD wID = 0; wID <= MAX_COCO_ID; wID++)
	{
		if (!TSTBIT(GetState(), wID * (MAX_COCO_ID+1)))
			continue;

		// Aktuelle BackEnd Videoformat (PAL, NTSC) sichern
		wsprintf(szBuffer, "BackEnd%u",wID);
		if (m_CoCoISA[wID].wBeVideoFormat == COCO_PAL)
			WritePrivateProfileString(szBuffer, "VideoFormat", "PAL", m_szAppIniName); 
		else if (m_CoCoISA[wID].wBeVideoFormat == COCO_NTSC)
			WritePrivateProfileString(szBuffer, "VideoFormat", "NTSC", m_szAppIniName); 

		// Aktuelle BackEnd Videoausgabeart (COMPOSITE, RGB) sichern
  		if (m_CoCoISA[wID].wBeVideoType == COCO_COMPOSITE)
			WritePrivateProfileString(szBuffer, "VideoType", "COMPOSITE", m_szAppIniName); 
		else if (	m_CoCoISA[wID].wBeVideoType == COCO_RGB)
			WritePrivateProfileString(szBuffer, "VideoType", "RGB", m_szAppIniName); 

		for (WORD wCrd = COCO_EXTCARD0; wCrd <= COCO_EXTCARD0   + COCO_MAX_EXTCARD; wCrd++)
		{
	 		if (!TSTBIT(GetState(), wID * (MAX_COCO_ID+1) + wCrd))
				continue;

			for (WORD wSrc = COCO_SOURCE0; wSrc <= COCO_SOURCE0 + COCO_MAX_SOURCE; wSrc++)
			{
				wsprintf(szSource, "Source%u-%u-%u",wID, wCrd, wSrc);

				itoa(m_CoCoISA[wID].ExtCard[wCrd].Source[wSrc].wFeBrightness, szBuffer, 10);
				WritePrivateProfileString(szSource, "Brightness", szBuffer, m_szAppIniName);
	
				itoa(m_CoCoISA[wID].ExtCard[wCrd].Source[wSrc].wFeContrast, szBuffer, 10);
				WritePrivateProfileString(szSource, "Contrast", szBuffer, m_szAppIniName);
				
				itoa(m_CoCoISA[wID].ExtCard[wCrd].Source[wSrc].wFeSaturation, szBuffer, 10);
				WritePrivateProfileString(szSource, "Saturation", szBuffer, m_szAppIniName);

				itoa(m_CoCoISA[wID].ExtCard[wCrd].Source[wSrc].wFeHue, szBuffer, 10);
				WritePrivateProfileString(szSource, "Hue", szBuffer, m_szAppIniName);

				itoa(m_CoCoISA[wID].ExtCard[wCrd].Source[wSrc].nAVPortY, szBuffer, 10);
				WritePrivateProfileString(szSource, "AVPortY", szBuffer, m_szAppIniName);

				itoa(m_CoCoISA[wID].ExtCard[wCrd].Source[wSrc].nAVPortC, szBuffer, 10);
				WritePrivateProfileString(szSource, "AVPortC", szBuffer, m_szAppIniName);

				if (m_CoCoISA[wID].ExtCard[wCrd].Source[wSrc].wFeVideoFormat == COCO_PAL)
					WritePrivateProfileString(szSource, "VideoFormat", "PAL", m_szAppIniName);
				else if (m_CoCoISA[wID].ExtCard[wCrd].Source[wSrc].wFeVideoFormat == COCO_SECAM)
					WritePrivateProfileString(szSource, "VideoFormat", "SECAM", m_szAppIniName);
				else if (m_CoCoISA[wID].ExtCard[wCrd].Source[wSrc].wFeVideoFormat == COCO_NTSC)
					WritePrivateProfileString(szSource, "VideoFormat", "NTSC", m_szAppIniName);
/*	Diese Einstellungen werden vom Supervisor geschrieben.
				if (m_CoCoISA[wID].ExtCard[wCrd].Source[wSrc].wFeSourceType == COCO_FBAS)
					WritePrivateProfileString(szSource, "SourceType", "FBAS", m_szAppIniName);
				else if (m_CoCoISA[wID].ExtCard[wCrd].Source[wSrc].wFeSourceType == COCO_SVHS)
					WritePrivateProfileString(szSource, "SourceType", "SVHS", m_szAppIniName);

   				itoa(m_CoCoISA[wID].ExtCard[wCrd].Source[wSrc].nAVPortY, szBuffer, 10);
				WritePrivateProfileString(szSource, "AVPortY", szBuffer, m_szAppIniName);

   				itoa(m_CoCoISA[wID].ExtCard[wCrd].Source[wSrc].nAVPortC, szBuffer, 10);
				WritePrivateProfileString(szSource, "AVPortC", szBuffer, m_szAppIniName);
*/
			}
		}	
	}

	// Flush Cache
	WritePrivateProfileString(NULL, NULL, NULL, m_szAppIniName);

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CCoCo::Init
BOOL CCoCo::Init()
{
	static char szBuffer[SIZE];
	memset(szBuffer, 0, sizeof(szBuffer));

	static char szSourceType[SIZE];
	memset(szSourceType, 0, sizeof(szSourceType));

	static char szVideoFormat[SIZE];
	memset(szVideoFormat, 0, sizeof(szVideoFormat));

	static char szSource[SIZE];
	memset(szSource, 0, sizeof(szSource));

	// Aktuelle Videoquelle 
	GetPrivateProfileString("COCOISA", "VideoSource", "Source0-0-0", szSource,  SIZE, m_szAppIniName); 

	// *************************************************
	// Video Front End
	// *************************************************

	for (WORD wID = 0; wID <= MAX_COCO_ID; wID++)
	{
		if (!TSTBIT(GetState(), wID * (MAX_COCO_ID+1)))
			continue;

		wsprintf(szBuffer, "BackEnd%u",wID);

		// Aktuelle BackEnd Videoformat (PAL, NTSC)
		GetPrivateProfileString(szBuffer, "VideoFormat", "PAL", szVideoFormat,  SIZE, m_szAppIniName); 
		if (stricmp(szVideoFormat,"PAL") == 0)
				m_CoCoISA[wID].wBeVideoFormat = COCO_PAL;
		else if (stricmp(szVideoFormat,"NTSC") == 0)
				m_CoCoISA[wID].wBeVideoFormat = COCO_NTSC;

		// Aktuelle BackEnd Videoausgabeart (COMPOSITE, RGB)
		GetPrivateProfileString(szBuffer, "VideoType", "COMPOSITE", szSourceType,  SIZE, m_szAppIniName); 
		if (stricmp(szSourceType,"COMPOSITE") == 0)
			m_CoCoISA[wID].wBeVideoType = COCO_COMPOSITE;
		else if (stricmp(szSourceType,"RGB") == 0)
			m_CoCoISA[wID].wBeVideoType = COCO_RGB;

		for (WORD wCrd = COCO_EXTCARD0; wCrd <= COCO_EXTCARD0   + COCO_MAX_EXTCARD; wCrd++)
		{
			if (!TSTBIT(GetState(), wID * (MAX_COCO_ID+1) + wCrd))
				continue;
		
			for (WORD wSrc = COCO_SOURCE0; wSrc <= COCO_SOURCE0 + COCO_MAX_SOURCE; wSrc++)
			{
				wsprintf(szBuffer, "Source%u-%u-%u",wID, wCrd, wSrc);
				// Test ob Eintrag vorhanden ist
				if (GetPrivateProfileInt(szBuffer, "AVPortY",    -1, m_szAppIniName) != -1)
				{
					m_CoCoISA[wID].ExtCard[wCrd].Source[wSrc].wFeBrightness =	
						GetPrivateProfileInt(szBuffer, "Brightness", (COCO_MAX_BRIGHTNESS - COCO_MIN_BRIGHTNESS) / 2, m_szAppIniName);
					m_CoCoISA[wID].ExtCard[wCrd].Source[wSrc].wFeContrast	=
						GetPrivateProfileInt(szBuffer, "Contrast",   (COCO_MAX_CONTRAST - COCO_MIN_CONTRAST) / 2, m_szAppIniName); 
					m_CoCoISA[wID].ExtCard[wCrd].Source[wSrc].wFeSaturation =
						GetPrivateProfileInt(szBuffer, "Saturation", (COCO_MAX_SATURATION - COCO_MIN_SATURATION) / 2, m_szAppIniName); 
					m_CoCoISA[wID].ExtCard[wCrd].Source[wSrc].wFeHue		=
						GetPrivateProfileInt(szBuffer, "Hue",		 (COCO_MAX_HUE - COCO_MIN_HUE) / 2, m_szAppIniName);
					m_CoCoISA[wID].ExtCard[wCrd].Source[wSrc].nAVPortY		=
								GetPrivateProfileInt(szBuffer, "AVPortY", wSrc - COCO_SOURCE0, m_szAppIniName); 
 					m_CoCoISA[wID].ExtCard[wCrd].Source[wSrc].nAVPortC		=
						GetPrivateProfileInt(szBuffer, "AVPortC", wSrc - COCO_SOURCE0, m_szAppIniName); 

					GetPrivateProfileString(szBuffer, "VideoFormat", "PAL", szVideoFormat, SIZE, m_szAppIniName); 
					if (stricmp(szVideoFormat,"PAL") == 0)
						m_CoCoISA[wID].ExtCard[wCrd].Source[wSrc].wFeVideoFormat	= COCO_PAL;
					else if (stricmp(szVideoFormat,"SECAM") == 0)
						m_CoCoISA[wID].ExtCard[wCrd].Source[wSrc].wFeVideoFormat	= COCO_SECAM;
					else if (stricmp(szVideoFormat,"NTSC") == 0)
						m_CoCoISA[wID].ExtCard[wCrd].Source[wSrc].wFeVideoFormat	= COCO_NTSC;

					GetPrivateProfileString(szBuffer, "SourceType", "FBAS", szSourceType,  SIZE, m_szAppIniName); 
					if (stricmp(szSourceType,"FBAS") == 0)
						m_CoCoISA[wID].ExtCard[wCrd].Source[wSrc].wFeSourceType	= COCO_FBAS;
					else if (stricmp(szSourceType,"SVHS") == 0)
						m_CoCoISA[wID].ExtCard[wCrd].Source[wSrc].wFeSourceType	= COCO_SVHS;
				}

				if (stricmp(szBuffer, szSource) == 0)
				{
					m_wSource  = wSrc;
					m_wExtCard = wCrd;
					m_byCoCoID = (BYTE)wID;
				}
			}
		}	
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::GetVersion
WORD CCoCo::GetVersion()
{
	return (WORD) SendMessage(m_hWndServer, COCO_GET_VERSION, (WPARAM)m_hWndClient, 0L);
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::GetLastError
WORD CCoCo::GetLastError()
{
	WORD wTemp = m_wLastError;
	m_wLastError = COCO_ERR_NO_ERRROR;

	return wTemp;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::CheckHardware
BOOL CCoCo::CheckHardware(BYTE byCoCoID)
{
	if (!IsWindow(m_hWndServer))
	{
		WK_TRACE_ERROR("CCoCo::CheckHardware/tCoCoSrv-Fensterhandle ungültig\n");
		return FALSE;
	}

	if (!SendMessage(m_hWndServer, COCO_CHECK_HARDWARE, (WPARAM)byCoCoID, 0L))
	{
		WK_TRACE_ERROR("CCoCo::CheckHardware/tCheckHardware failed\n");
		return FALSE;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::Aktivate
BOOL CCoCo::Activate(WORD wActType)
{
	if (wActType & COCO_ACT_SOURCE)
		SetInputSource(COCO_COCOID_CURRENT,COCO_EXTCARD_CURRENT, COCO_SOURCE_CURRENT);
	if (wActType & COCO_ACT_BEVIDEOFORMAT)
		SetBeVideoFormat(GetBeVideoFormat());
	if (wActType & COCO_ACT_BEVIDEOTYPE)
		SetBeVideoType(GetBeVideoType());
	if (wActType & COCO_ACT_FEFILTER)
		SetFeFilter(GetFeFilter());
//	if (wActType & COCO_ACT_FEHSCALE)
//		SetFeHScale(GetFeHScale());
//	if (wActType & COCO_ACT_FEVSCALE)
//		SetFeVScale(GetFeVScale());

	if (wActType & COCO_ACT_EDGE)
		SetAlarmEdge(COCO_COCOID_CURRENT, COCO_EXTCARD_CURRENT,
		GetAlarmEdge(COCO_COCOID_CURRENT, COCO_EXTCARD_CURRENT));
	if (wActType & COCO_ACT_ACK)
		SetAlarmAck(COCO_COCOID_CURRENT, COCO_EXTCARD_CURRENT,
		GetAlarmEdge(COCO_COCOID_CURRENT, COCO_EXTCARD_CURRENT));
	if (wActType & COCO_ACT_RELAIS)
		SetRelais(COCO_COCOID_CURRENT, COCO_EXTCARD_CURRENT,
		GetRelais(COCO_COCOID_CURRENT, COCO_EXTCARD_CURRENT));
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::SetInputSource
DWORD CCoCo::SetInputSource(BYTE byCoCoID, WORD wExtCard, WORD wSource)
{
	DWORD	dwOldSource		= MAKELONG(m_wSource, m_wExtCard);
	WORD	wPortYC			= 0;
	DWORD	dwVidSel		= 0;

//	WK_TRACE("SetInputSource\tSource=%u\tExtCard=%u\n", wSource, wExtCard);

	if (!CheckCoCoID(byCoCoID))
		return COCO_ERROR;

	if (!CheckExtCard(wExtCard))
		return COCO_ERROR;

	if(!CheckSource(wSource))
		return COCO_ERROR;

	if (byCoCoID == COCO_COCOID_CURRENT)
		byCoCoID = m_byCoCoID;

 	if (wExtCard == COCO_EXTCARD_CURRENT)
		wExtCard = m_CoCoISA[byCoCoID].wExtCard;

 	if (wSource == COCO_SOURCE_CURRENT)
		wSource = m_CoCoISA[byCoCoID].ExtCard[wExtCard].wSource;

	// CoCo vorhanden ?
	if (!TSTBIT(GetState(), byCoCoID * (MAX_COCO_ID+1)))
		return COCO_ERROR;

	// Erweiterungskarte vorhanden ?
	if (wExtCard != COCO_EXTCARD_MEGRA)
	{
		if (!TSTBIT(GetState(), byCoCoID * (MAX_COCO_ID+1) + wExtCard))
			return COCO_ERROR;
	}
	
	m_byCoCoID = byCoCoID;
	m_wExtCard = wExtCard;		
	m_wSource  = wSource;

	m_CoCoISA[m_byCoCoID].wExtCard = m_wExtCard;
	m_CoCoISA[m_byCoCoID].ExtCard[m_wExtCard].wSource = m_wSource;

	// LowByte von wPortYC: Portnummer des Y-Signals (0...7)
	// HiByte  von wPortYC: Portnummer des C-Signals (0...7, ff=NC)
	// LowNibble von wPortYC: Portnummer des Y-Signals (0...7)
	// HiNibble  von wPortYC: Portnummer des C-Signals (0...7, ff=NC)

	wPortYC  = (BYTE)m_CoCoISA[m_byCoCoID].ExtCard[m_wExtCard].Source[m_wSource].nAVPortY;
	wPortYC |= (BYTE)(m_CoCoISA[m_byCoCoID].ExtCard[m_wExtCard].Source[m_wSource].nAVPortC)<<4;

	wPortYC = MAKEWORD((BYTE)wPortYC, (BYTE)m_CoCoISA[m_byCoCoID].ExtCard[m_wExtCard].Source[m_wSource].wFeSourceType);
		  
	dwVidSel = MAKELONG(wPortYC, MAKEWORD(m_wExtCard, m_wSource));
	
//	PostMessage(m_hWndServer, COCO_SET_INPUT_SOURCE, (WPARAM)m_byCoCoID, (LPARAM)dwVidSel);
	SendMessage(m_hWndServer, COCO_SET_INPUT_SOURCE, (WPARAM)m_byCoCoID, (LPARAM)dwVidSel);

	// Die Megra regelt diese Dinge selber
	if (wExtCard != COCO_EXTCARD_MEGRA)
	{
		// Die der Eingangsquelle zugeordneten Parameter ebenfalls ändern.
		// OOPS Aus Performencegründen erst mal einige Parameter abschalten

	//	SetFeSourceType(GetFeSourceType());
	//	SetFeVideoFormat(GetFeVideoFormat());
	//	SetFeBrightness(GetFeBrightness());
	//	SetFeContrast(GetFeContrast());
	//	SetFeSaturation(GetFeSaturation());
	//	SetFeHue(GetFeHue());

	  }

	return dwOldSource;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::GetInputSource
DWORD CCoCo::GetInputSource(BYTE byCoCoID)
{
	if (!CheckCoCoID(byCoCoID))
		return COCO_ERROR;

	if (byCoCoID == COCO_COCOID_CURRENT)
		byCoCoID = m_byCoCoID;

	return MAKELONG(m_wSource, m_wExtCard);
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::GetCoCoID
BYTE CCoCo::GetCoCoID()
{
	return m_byCoCoID;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::GetState
DWORD CCoCo::GetState()
{

	return m_dwHardwareState;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::SetFeVideoFormat
WORD CCoCo::SetFeSourceType(WORD wSourceType)
{
	WORD wOldSourceType = GetFeSourceType();

	if (!CheckSourceType(wSourceType))
		return COCO_ERROR;

	m_CoCoISA[m_byCoCoID].ExtCard[m_wExtCard].Source[m_wSource].wFeSourceType = wSourceType;

	PostMessage(m_hWndServer, COCO_SET_FE_SOURCE_TYPE, (WPARAM)m_byCoCoID, (LPARAM)wSourceType);

	return wOldSourceType;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::GetFeVideoFormat()
WORD CCoCo::GetFeSourceType()
{
	return  m_CoCoISA[m_byCoCoID].ExtCard[m_wExtCard].Source[m_wSource].wFeSourceType;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::SetFeVideoFormat
WORD CCoCo::SetFeVideoFormat(WORD wVideoFormat)
{
	WORD wOldVideoFormat = GetFeVideoFormat();

	if (!CheckFeVideoFormat(wVideoFormat))
		return COCO_ERROR;

	m_CoCoISA[m_byCoCoID].ExtCard[m_wExtCard].Source[m_wSource].wFeVideoFormat = wVideoFormat;
	
	PostMessage(m_hWndServer, COCO_SET_FE_VIDEO_FORMAT, (WPARAM)m_byCoCoID, (LPARAM)wVideoFormat);

	return wOldVideoFormat;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::GetFeVideoFormat
WORD CCoCo::GetFeVideoFormat()
{
	return m_CoCoISA[m_byCoCoID].ExtCard[m_wExtCard].Source[m_wSource].wFeVideoFormat;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::SetBeVideoFormat
WORD CCoCo::SetBeVideoFormat(WORD wBeVideoFormat)
{
	WORD wOldBeVideoFormat = GetBeVideoFormat();

	if(!CheckBeVideoFormat(wBeVideoFormat))
		return  COCO_ERROR;

 	m_CoCoISA[m_byCoCoID].wBeVideoFormat = wBeVideoFormat;
 
 	PostMessage(m_hWndServer, COCO_SET_BE_VIDEO_FORMAT, (WPARAM)m_byCoCoID, (LPARAM)wBeVideoFormat);

	return wOldBeVideoFormat;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::GetBeVideoFormat
WORD CCoCo::GetBeVideoFormat()
{
	return  m_CoCoISA[m_byCoCoID].wBeVideoFormat;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::SetBeVideoType
WORD CCoCo::SetBeVideoType(WORD wBeVideoType)
{
	WORD wOldBeVideoType = GetBeVideoType();

	if(!CheckBeVideoType(wBeVideoType))
		return  COCO_ERROR;

 	m_CoCoISA[m_byCoCoID].wBeVideoType = wBeVideoType;
 
 	PostMessage(m_hWndServer, COCO_SET_BE_VIDEO_TYPE, (WPARAM)m_byCoCoID, (LPARAM)wBeVideoType);

	return wOldBeVideoType;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::GetBeVideoType
WORD CCoCo::GetBeVideoType()
{
	return  m_CoCoISA[m_byCoCoID].wBeVideoType;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::DlgLuminance
BOOL CCoCo::DlgLuminance()
{
	if (m_pDlgColor)
		m_pDlgColor->ShowWindow(SW_SHOW);
	
	return TRUE; //m_pDlgColor->IsValid();
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::SetFeBrightness
WORD CCoCo::SetFeBrightness(WORD wBrightness)
{
	WORD wOldBrightness = GetFeBrightness();
	
	CheckRange(&wBrightness, COCO_MIN_BRIGHTNESS, COCO_MAX_BRIGHTNESS);

	// Da der CoCo sehr schnell Schalten muß ist es leider nicht möglich die
	// Helligkeit für jede Kamera einzeln einzustellen, da dies beim Umschalten
	// zuviel Zeit in Anspruch nehmen würde. Deshalb werden nun für alle Kameras
	// die Einstellungen der Kamera 0 übernommen
	//m_CoCoISA[m_byCoCoID].ExtCard[m_wExtCard].Source[m_wSource].wFeBrightness = wBrightness;
	m_CoCoISA[m_byCoCoID].ExtCard[0].Source[0].wFeBrightness = wBrightness;

	PostMessage(m_hWndServer, COCO_SET_FE_BRIGHTNESS, (WPARAM)m_byCoCoID, (LPARAM)wBrightness);

	if (m_pDlgColor)
		m_pDlgColor->UpdateDlg(TRUE);

	return wOldBrightness;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::GetFeBrightness
WORD CCoCo::GetFeBrightness()
{
	// Da der CoCo sehr schnell Schalten muß ist es leider nicht möglich die
	// Helligkeit für jede Kamera einzeln einzustellen, da dies beim Umschalten
	// zuviel Zeit in Anspruch nehmen würde. Deshalb werden nun für alle Kameras
	// die Einstellungen der Kamera 0 übernommen
//	return 	m_CoCoISA[m_byCoCoID].ExtCard[m_wExtCard].Source[m_wSource].wFeBrightness;
	return 	m_CoCoISA[m_byCoCoID].ExtCard[0].Source[0].wFeBrightness;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::SetFeContrast
WORD CCoCo::SetFeContrast(WORD wContrast)
{
	WORD wOldContrast = GetFeContrast();

	CheckRange(&wContrast, COCO_MIN_CONTRAST, COCO_MAX_CONTRAST);

	// Da der CoCo sehr schnell Schalten muß ist es leider nicht möglich der
	// Kontrast für jede Kamera einzeln einzustellen, da dies beim Umschalten
	// zuviel Zeit in Anspruch nehmen würde. Deshalb werden nun für alle Kameras
	// die Einstellungen der Kamera 0 übernommen
//	m_CoCoISA[m_byCoCoID].ExtCard[m_wExtCard].Source[m_wSource].wFeContrast = wContrast;
	m_CoCoISA[m_byCoCoID].ExtCard[0].Source[0].wFeContrast = wContrast;

	PostMessage(m_hWndServer, COCO_SET_FE_CONTRAST, (WPARAM)m_byCoCoID, (LPARAM)wContrast);

	if (m_pDlgColor)
		m_pDlgColor->UpdateDlg(TRUE);

	return wOldContrast;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::GetFeContrast
WORD CCoCo::GetFeContrast()
{
	// Da der CoCo sehr schnell Schalten muß ist es leider nicht möglich der
	// Kontrast für jede Kamera einzeln einzustellen, da dies beim Umschalten
	// zuviel Zeit in Anspruch nehmen würde. Deshalb werden nun für alle Kameras
	// die Einstellungen der Kamera 0 übernommen
//	return m_CoCoISA[m_byCoCoID].ExtCard[m_wExtCard].Source[m_wSource].wFeContrast;
	return m_CoCoISA[m_byCoCoID].ExtCard[0].Source[0].wFeContrast;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::SetFeSaturation
WORD CCoCo::SetFeSaturation(WORD wSaturation)
{
	WORD wOldContrast = GetFeSaturation();

	CheckRange(&wSaturation, COCO_MIN_SATURATION, COCO_MAX_SATURATION);

	// Da der CoCo sehr schnell Schalten muß ist es leider nicht möglich die
	// Farbsättigung für jede Kamera einzeln einzustellen, da dies beim Umschalten
	// zuviel Zeit in Anspruch nehmen würde. Deshalb werden nun für alle Kameras
	// die Einstellungen der Kamera 0 übernommen
//	m_CoCoISA[m_byCoCoID].ExtCard[m_wExtCard].Source[m_wSource].wFeSaturation = wSaturation;
	m_CoCoISA[m_byCoCoID].ExtCard[0].Source[0].wFeSaturation = wSaturation;

 	PostMessage(m_hWndServer, COCO_SET_FE_SATURATION, (WPARAM)m_byCoCoID, (LPARAM)wSaturation);

	if (m_pDlgColor)
		m_pDlgColor->UpdateDlg(TRUE);

	return wOldContrast;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::GetFeSaturation
WORD CCoCo::GetFeSaturation()
{
	// Da der CoCo sehr schnell Schalten muß ist es leider nicht möglich die
	// Farbsättigung für jede Kamera einzeln einzustellen, da dies beim Umschalten
	// zuviel Zeit in Anspruch nehmen würde. Deshalb werden nun für alle Kameras
	// die Einstellungen der Kamera 0 übernommen
//	return m_CoCoISA[m_byCoCoID].ExtCard[m_wExtCard].Source[m_wSource].wFeSaturation;
	return m_CoCoISA[m_byCoCoID].ExtCard[0].Source[0].wFeSaturation;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::SetFeHue
WORD CCoCo::SetFeHue(WORD wHue)
{
	WORD wOldHue = GetFeHue();

	CheckRange(&wHue, COCO_MIN_HUE, COCO_MAX_HUE);

	// Da der CoCo sehr schnell Schalten muß ist es leider nicht möglich die
	// Phase für jede Kamera einzeln einzustellen, da dies beim Umschalten
	// zuviel Zeit in Anspruch nehmen würde. Deshalb werden nun für alle Kameras
	// die Einstellungen der Kamera 0 übernommen
//	m_CoCoISA[m_byCoCoID].ExtCard[m_wExtCard].Source[m_wSource].wFeHue = wHue;
	m_CoCoISA[m_byCoCoID].ExtCard[0].Source[0].wFeHue = wHue;

 	PostMessage(m_hWndServer, COCO_SET_FE_HUE, (WPARAM)m_byCoCoID, (LPARAM)wHue);

	if (m_pDlgColor)
		m_pDlgColor->UpdateDlg(TRUE);

	return wOldHue;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::GetFeHue
WORD CCoCo::GetFeHue()
{
	// Da der CoCo sehr schnell Schalten muß ist es leider nicht möglich die
	// Phase für jede Kamera einzeln einzustellen, da dies beim Umschalten
	// zuviel Zeit in Anspruch nehmen würde. Deshalb werden nun für alle Kameras
	// die Einstellungen der Kamera 0 übernommen
//	return m_CoCoISA[m_byCoCoID].ExtCard[m_wExtCard].Source[m_wSource].wFeHue;
	return m_CoCoISA[m_byCoCoID].ExtCard[0].Source[0].wFeHue;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::SetFeHScale
WORD CCoCo::SetFeHScale(WORD wHScale)
{
	WORD wOldHScale = GetFeHScale();

	CheckRange(&wHScale, COCO_MIN_HSCALE, COCO_MAX_HSCALE);

	m_CoCoISA[m_byCoCoID].wFeHScale = wHScale;

 	PostMessage(m_hWndServer, COCO_SET_FE_HSCALE, (WPARAM)m_byCoCoID, (LPARAM)wHScale);

	if (m_pDlgColor)
		m_pDlgColor->UpdateDlg(TRUE);

	return wOldHScale;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::GetFeHScale
WORD CCoCo::GetFeHScale()
{
	return m_CoCoISA[m_byCoCoID].wFeHScale;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::SetFeVScale
WORD CCoCo::SetFeVScale(WORD wVScale)
{
	WORD wOldVScale = GetFeVScale();

	CheckRange(&wVScale, COCO_MIN_VSCALE, COCO_MAX_VSCALE);

	m_CoCoISA[m_byCoCoID].wFeVScale = wVScale;

 	PostMessage(m_hWndServer, COCO_SET_FE_VSCALE, (WPARAM)m_byCoCoID, (LPARAM)wVScale);

	if (m_pDlgColor)
		m_pDlgColor->UpdateDlg(TRUE);

	return wOldVScale;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::GetFeVScale
WORD CCoCo::GetFeVScale()
{
	return m_CoCoISA[m_byCoCoID].wFeVScale;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::SetFeFilter
WORD CCoCo::SetFeFilter(WORD wFilter)
{
	WORD wOldFilter = GetFeFilter();

	if(!CheckFeFilter(wFilter))
		return COCO_ERROR;

	m_CoCoISA[m_byCoCoID].wFeFilter = wFilter;

	PostMessage(m_hWndServer, COCO_SET_FE_FILTER, (WPARAM)m_byCoCoID, (LPARAM)wFilter);

	if (m_pDlgColor)
		m_pDlgColor->UpdateDlg(TRUE);

	return wOldFilter;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::GetFeFilter()
WORD CCoCo::GetFeFilter()
{
	return	m_CoCoISA[m_byCoCoID].wFeFilter;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::GetCurrentAlarmStatus()
WORD CCoCo::GetCurrentAlarmStatus(BYTE byCoCoID, WORD wExtCard)
{
	if (!CheckCoCoID(byCoCoID))
		return COCO_ERROR;

	if (!CheckExtCard(wExtCard))
		return COCO_ERROR;

	if (byCoCoID == COCO_COCOID_CURRENT)
		byCoCoID = m_byCoCoID;

 	if (wExtCard == COCO_EXTCARD_CURRENT)
		wExtCard = m_CoCoISA[byCoCoID].wExtCard;

 	return (WORD)SendMessage(m_hWndServer, COCO_GET_CURRENT_ALARM_STATUS,
		   (WPARAM)byCoCoID, (LPARAM)wExtCard);
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::GetAlarmStatus()
WORD CCoCo::GetAlarmStatus(BYTE byCoCoID, WORD wExtCard)
{
	if (!CheckCoCoID(byCoCoID))
		return COCO_ERROR;

	if (!CheckExtCard(wExtCard))
		return COCO_ERROR;

	if (byCoCoID == COCO_COCOID_CURRENT)
		byCoCoID = m_byCoCoID;

 	if (wExtCard == COCO_EXTCARD_CURRENT)
		wExtCard = m_CoCoISA[byCoCoID].wExtCard;

 	return (WORD)SendMessage(m_hWndServer, COCO_GET_ALARM_STATUS,
		   (WPARAM)byCoCoID, (LPARAM)wExtCard);
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::SetAlarmEdge()
WORD CCoCo::SetAlarmEdge(BYTE byCoCoID, WORD wExtCard, WORD wEdge)
{
	if (!CheckCoCoID(byCoCoID))
		return COCO_ERROR;

	if (!CheckExtCard(wExtCard))
		return COCO_ERROR;

	if (byCoCoID == COCO_COCOID_CURRENT)
		byCoCoID = m_byCoCoID;

 	if (wExtCard == COCO_EXTCARD_CURRENT)
		wExtCard = m_CoCoISA[byCoCoID].wExtCard;

	WORD wOldEdge = GetAlarmEdge(byCoCoID, wExtCard);

	m_CoCoISA[byCoCoID].ExtCard[wExtCard].wEdge = wEdge;

	PostMessage(m_hWndServer, COCO_SET_ALARM_EDGE,
		   (WPARAM)byCoCoID, (LPARAM)MAKELONG(wExtCard, wEdge));

	return wOldEdge;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::SetAlarmAck()
WORD CCoCo::SetAlarmAck(BYTE byCoCoID, WORD wExtCard, WORD wAck)
{
	if (!CheckCoCoID(byCoCoID))
		return COCO_ERROR;

	if (!CheckExtCard(wExtCard))
		return COCO_ERROR;

	if (byCoCoID == COCO_COCOID_CURRENT)
		byCoCoID = m_byCoCoID;

 	if (wExtCard == COCO_EXTCARD_CURRENT)
		wExtCard = m_CoCoISA[byCoCoID].wExtCard;

	WORD wOldAck = GetAlarmAck(byCoCoID, wExtCard); 
	m_CoCoISA[byCoCoID].ExtCard[wExtCard].wAck = wAck;

	PostMessage(m_hWndServer, COCO_SET_ALARM_ACK,
		   (WPARAM)byCoCoID, (LPARAM)MAKELONG(wExtCard, wAck));

	return wOldAck;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::GetAlarmEdge()
WORD CCoCo::GetAlarmEdge(BYTE byCoCoID, WORD wExtCard)
{
	if (!CheckCoCoID(byCoCoID))
		return COCO_ERROR;

	if (!CheckExtCard(wExtCard))
		return COCO_ERROR;

	if (byCoCoID == COCO_COCOID_CURRENT)
		byCoCoID = m_byCoCoID;

 	if (wExtCard == COCO_EXTCARD_CURRENT)
		wExtCard = m_CoCoISA[byCoCoID].wExtCard;

	return  m_CoCoISA[byCoCoID].ExtCard[wExtCard].wEdge;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::GetAlarmAck()
WORD CCoCo::GetAlarmAck(BYTE byCoCoID, WORD wExtCard)
{
	if (!CheckCoCoID(byCoCoID))
		return COCO_ERROR;

	if (!CheckExtCard(wExtCard))
		return COCO_ERROR;

	if (byCoCoID == COCO_COCOID_CURRENT)
		byCoCoID = m_byCoCoID;

 	if (wExtCard == COCO_EXTCARD_CURRENT)
		wExtCard = m_CoCoISA[byCoCoID].wExtCard;

 	return m_CoCoISA[byCoCoID].ExtCard[wExtCard].wAck;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::GetRelais()
WORD CCoCo::GetRelais(BYTE byCoCoID, WORD wExtCard)
{
	if (!CheckCoCoID(byCoCoID))
		return COCO_ERROR;

	if (!CheckExtCard(wExtCard))
		return COCO_ERROR;

	if (byCoCoID == COCO_COCOID_CURRENT)
		byCoCoID = m_byCoCoID;

 	if (wExtCard == COCO_EXTCARD_CURRENT)
		wExtCard = m_CoCoISA[byCoCoID].wExtCard;

 	return (WORD)SendMessage(m_hWndServer, COCO_GET_RELAIS,
		   (WPARAM)byCoCoID, (LPARAM)wExtCard);
	
//	return m_CoCoISA[byCoCoID].ExtCard[wExtCard].wRelais;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::SetRelais()
WORD CCoCo::SetRelais(BYTE byCoCoID, WORD wExtCard, WORD wRelais)
{
	if (!CheckCoCoID(byCoCoID))
		return COCO_ERROR;

	if (!CheckExtCard(wExtCard))
		return COCO_ERROR;

	if (byCoCoID == COCO_COCOID_CURRENT)
		byCoCoID = m_byCoCoID;

 	if (wExtCard == COCO_EXTCARD_CURRENT)
		wExtCard = m_CoCoISA[byCoCoID].wExtCard;

	WORD wOldRelais = m_CoCoISA[byCoCoID].ExtCard[wExtCard].wRelais;

	if ((byCoCoID == COCO_COCOID0) && (wExtCard == COCO_EXTCARD0) && (m_bIgnoreRelais4))
		wRelais = (wRelais & 0x07) | (wOldRelais &= 0xf8);
	
	m_CoCoISA[byCoCoID].ExtCard[wExtCard].wRelais = wRelais;

	PostMessage(m_hWndServer, COCO_SET_RELAIS,
		   (WPARAM)byCoCoID, (LPARAM)MAKELONG(wExtCard, wRelais));
	
	return wOldRelais;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::SendMCIScript
BOOL CCoCo::SendMCIScript(BYTE byCoCoID, LPCSTR lpScriptFileName)
{

	if (!CheckCoCoID(byCoCoID))
		return FALSE;

	if (byCoCoID == COCO_COCOID_CURRENT)
		byCoCoID = m_byCoCoID;

	if (!lpScriptFileName)
		return FALSE;

	ATOM atm = GlobalAddAtom(lpScriptFileName);
	if (!atm)
		return FALSE;

	BOOL bRet = (BOOL)SendMessage(m_hWndServer, COCO_EXECUTE_MCI_SCRIPT,
				(WPARAM)byCoCoID, (LPARAM)atm);

	GlobalDeleteAtom(atm);

	//return TRUE;
	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::WriteData
BOOL CCoCo::WriteData(PARAMSTRUCT &Param)
{
	if (!CheckCoCoID((BYTE)Param.wCoCoID))
		return FALSE;

	if (Param.wCoCoID == COCO_COCOID_CURRENT)
		Param.wCoCoID = (WORD)m_byCoCoID; // Wegen Bytealignment auf WORD casten

	if (!m_pCopyData)
		return FALSE;

	return m_pCopyData->WriteData(m_hWndServer, Param);
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::RequestData
BOOL CCoCo::RequestData(BYTE byCoCoID, WORD wData)
{
	if (!CheckCoCoID(byCoCoID))
		return FALSE;

	if (byCoCoID == COCO_COCOID_CURRENT)
		byCoCoID = m_byCoCoID;

	return SendMessage(m_hWndServer, COCO_REQUEST_DATA,
				(WPARAM)m_hWndClient, (LPARAM)MAKELONG(wData, (WORD)byCoCoID));
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::ReadData
BOOL CCoCo::ReadData(LPARAM lParam, PARAMSTRUCT &Param)
{
	if (!lParam)
		return FALSE;

	if (!m_pCopyData)
		return FALSE;

	return m_pCopyData->ReadData(lParam, Param);
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::EncoderStart
BOOL CCoCo::EncoderStart(BYTE byCoCoID)
{
	if (!CheckCoCoID(byCoCoID))
		return FALSE;

	if (byCoCoID == COCO_COCOID_CURRENT)
		byCoCoID = m_byCoCoID;

 	PostMessage(m_hWndServer, COCO_ENC_START, (WPARAM)byCoCoID, (LPARAM)m_hWndClient);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::EncoderStop
BOOL CCoCo::EncoderStop(BYTE byCoCoID)
{
	if (!CheckCoCoID(byCoCoID))
		return FALSE;

	if (byCoCoID == COCO_COCOID_CURRENT)
		byCoCoID = m_byCoCoID;

	PostMessage(m_hWndServer, COCO_ENC_STOP, (WPARAM)byCoCoID, (LPARAM)m_hWndClient);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::EncoderBreak
BOOL CCoCo::EncoderBreak(BYTE byCoCoID)
{
	if (!CheckCoCoID(byCoCoID))
		return FALSE;

	if (byCoCoID == COCO_COCOID_CURRENT)
		byCoCoID = m_byCoCoID;

	PostMessage(m_hWndServer, COCO_ENC_BREAK, (WPARAM)byCoCoID, (LPARAM)m_hWndClient);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::EncoderSetFormat
WORD CCoCo::EncoderSetFormat(BYTE byCoCoID, WORD wFormat)
{
	if (!CheckCoCoID(byCoCoID))
		return COCO_ERROR;

	if (!CheckEncoderFormat(wFormat))
		return COCO_ERROR;

	if (byCoCoID == COCO_COCOID_CURRENT)
		byCoCoID = m_byCoCoID;

	// Bisherige Einstellung holen.
	WORD wOldFormat = EncoderGetFormat(byCoCoID);

	PostMessage(m_hWndServer, COCO_ENC_SET_FORMAT, (WPARAM)byCoCoID, (LPARAM)wFormat);

	m_CoCoISA[byCoCoID].wFormat = wFormat;

	return wOldFormat;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::EncoderGetFormat
WORD CCoCo::EncoderGetFormat(BYTE byCoCoID)
{
	if (!CheckCoCoID(byCoCoID))
		return COCO_ERROR;

	if (byCoCoID == COCO_COCOID_CURRENT)
		byCoCoID = m_byCoCoID;

	return 	m_CoCoISA[byCoCoID].wFormat;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::EncoderSetFramerate
WORD CCoCo::EncoderSetFramerate(BYTE byCoCoID, WORD wFramerate)
{
	if (!CheckCoCoID(byCoCoID))
		return COCO_ERROR;

	if (!CheckEncoderFramerate(wFramerate))
		return COCO_ERROR;

	if (byCoCoID == COCO_COCOID_CURRENT)
		byCoCoID = m_byCoCoID;

	// Bisherige Einstellung holen.
	WORD wOldFramerate = EncoderGetFramerate(byCoCoID);

//	PostMessage(m_hWndServer, COCO_ENC_SET_FRAMERATE, (WPARAM)byCoCoID, (LPARAM)wFramerate);
	SendMessage(m_hWndServer, COCO_ENC_SET_FRAMERATE, (WPARAM)byCoCoID, (LPARAM)wFramerate);

	m_CoCoISA[byCoCoID].wFramerate = wFramerate;

	return wOldFramerate;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::EncoderGetFramerate
WORD CCoCo::EncoderGetFramerate(BYTE byCoCoID)
{
	if (!CheckCoCoID(byCoCoID))
		return COCO_ERROR;

	if (byCoCoID == COCO_COCOID_CURRENT)
		byCoCoID = m_byCoCoID;

	return m_CoCoISA[byCoCoID].wFramerate;
}
/////////////////////////////////////////////////////////////////////////////
// CCoCo::EncoderSetBitrate
DWORD CCoCo::EncoderSetBitrate(BYTE byCoCoID, DWORD dwBitrate)
{
	if (!CheckCoCoID(byCoCoID))
		return COCO_ERROR;

	if (!CheckEncoderBitrate(dwBitrate))
		return COCO_ERROR;

	if (byCoCoID == COCO_COCOID_CURRENT)
		byCoCoID = m_byCoCoID;

	// Bisherige Einstellung holen.
	DWORD dwOldBitrate = EncoderGetBitrate(byCoCoID);

//	PostMessage(m_hWndServer, COCO_ENC_SET_BITRATE, (WPARAM)byCoCoID, (LPARAM)dwBitrate);
	SendMessage(m_hWndServer, COCO_ENC_SET_BITRATE, (WPARAM)byCoCoID, (LPARAM)dwBitrate);

	m_CoCoISA[byCoCoID].dwEncBitrate = dwBitrate;

	return dwOldBitrate;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::EncoderGetBitrate
DWORD CCoCo::EncoderGetBitrate(BYTE byCoCoID)
{
	if (!CheckCoCoID(byCoCoID))
		return COCO_ERROR;

	if (byCoCoID == COCO_COCOID_CURRENT)
		byCoCoID = m_byCoCoID;

	return m_CoCoISA[byCoCoID].dwEncBitrate;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::EncoderSetMode
BOOL CCoCo::EncoderSetMode(BYTE byCoCoID, WORD wFormat, WORD wFramerate, DWORD dwBitrate)
{
	if (!CheckCoCoID(byCoCoID))
		return FALSE;

	if (!CheckEncoderFormat(wFormat))
		return COCO_ERROR;

	if (!CheckEncoderFramerate(wFramerate))
		return COCO_ERROR;

	if (!CheckEncoderBitrate(dwBitrate))
		return COCO_ERROR;
	
	if (byCoCoID == COCO_COCOID_CURRENT)
		byCoCoID = m_byCoCoID;

	m_CoCoISA[byCoCoID].wFormat = wFormat;
 	m_CoCoISA[byCoCoID].wFramerate = wFramerate;
	m_CoCoISA[byCoCoID].dwEncBitrate = dwBitrate;

	WORD wParam = wFramerate<<8 | wFormat<<4 | byCoCoID;

//	PostMessage(m_hWndServer, COCO_ENC_SET_MODE, (WPARAM)wParam, (LPARAM)dwBitrate);
	SendMessage(m_hWndServer, COCO_ENC_SET_MODE, (WPARAM)wParam, (LPARAM)dwBitrate);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::EncoderGetState
int CCoCo::EncoderGetState(BYTE byCoCoID)
{
	return 	(int)SendMessage(m_hWndServer, COCO_ENC_GET_STATE,
				 (WPARAM)byCoCoID, (LPARAM)0);
	
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::ConnectVideoOutput
BOOL CCoCo::ConnectVideoOutput(BYTE byCoCoID, WORD wConnect)
{
	if (!CheckCoCoID(byCoCoID))
		return FALSE;

	if (!CheckVideoOutput(wConnect))
		return FALSE;

	if (byCoCoID == COCO_COCOID_CURRENT)
		byCoCoID = m_byCoCoID;

	PostMessage(m_hWndServer, COCO_CONNECT_OUTPUT, (WPARAM)byCoCoID, (LPARAM)wConnect);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::DecoderStart
BOOL CCoCo::DecoderStart(BYTE byCoCoID)
{
	if (!CheckCoCoID(byCoCoID))
		return FALSE;

	if (byCoCoID == COCO_COCOID_CURRENT)
		byCoCoID = m_byCoCoID;

	PostMessage(m_hWndServer, COCO_DEC_START, (WPARAM)byCoCoID, (LPARAM)m_hWndClient);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::DecoderStop
BOOL CCoCo::DecoderStop(BYTE byCoCoID)	
{
	if (!CheckCoCoID(byCoCoID))
		return FALSE;

	if (byCoCoID == COCO_COCOID_CURRENT)
		byCoCoID = m_byCoCoID;

	PostMessage(m_hWndServer, COCO_DEC_STOP, (WPARAM)byCoCoID, (LPARAM)m_hWndClient);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::DecoderBreak
BOOL CCoCo::DecoderBreak(BYTE byCoCoID)	
{
	if (!CheckCoCoID(byCoCoID))
		return FALSE;

	if (byCoCoID == COCO_COCOID_CURRENT)
		byCoCoID = m_byCoCoID;

	PostMessage(m_hWndServer, COCO_DEC_BREAK, (WPARAM)byCoCoID, (LPARAM)m_hWndClient);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::DecoderSetBitrate
DWORD CCoCo::DecoderSetBitrate(BYTE byCoCoID, DWORD dwBitrate)
{
	if (!CheckCoCoID(byCoCoID))
		return COCO_ERROR;

	if (!CheckEncoderBitrate(dwBitrate))
		return COCO_ERROR;

	if (byCoCoID == COCO_COCOID_CURRENT)
		byCoCoID = m_byCoCoID;

	// Bisherige Einstellung holen.
	DWORD dwOldBitrate = DecoderGetBitrate(byCoCoID);

	PostMessage(m_hWndServer, COCO_DEC_SET_BITRATE, (WPARAM)byCoCoID, (LPARAM)dwBitrate);

	m_CoCoISA[byCoCoID].dwDecBitrate = dwBitrate;

	return dwOldBitrate;
}
/////////////////////////////////////////////////////////////////////////////
// CCoCo::DecoderGetBitrate
DWORD CCoCo::DecoderGetBitrate(BYTE byCoCoID)
{
	if (!CheckCoCoID(byCoCoID))
		return FALSE;

	if (byCoCoID == COCO_COCOID_CURRENT)
		byCoCoID = m_byCoCoID;

	return m_CoCoISA[byCoCoID].dwDecBitrate;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::DecoderGetState
int CCoCo::DecoderGetState(BYTE byCoCoID)
{
	return 	(int)SendMessage(m_hWndServer, COCO_DEC_GET_STATE,
				 (WPARAM)byCoCoID, (LPARAM)0);
	
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::CheckCoCoID
BOOL CCoCo::CheckCoCoID(BYTE byCoCoID)
{
	if ((byCoCoID <= MAX_COCO_ID) || (byCoCoID == COCO_COCOID_CURRENT))
		return TRUE;

	m_wLastError = COCO_ERR_WRONG_COCOID;
	return FALSE;
} 

/////////////////////////////////////////////////////////////////////////////
// CCoCo::CheckSource
BOOL CCoCo::CheckSource(WORD wSource)
{
	switch(wSource)
	{
		case COCO_SOURCE0: 
		case COCO_SOURCE1: 
		case COCO_SOURCE2: 
		case COCO_SOURCE3: 
		case COCO_SOURCE4: 
		case COCO_SOURCE5: 
		case COCO_SOURCE6: 
		case COCO_SOURCE7: 
		case COCO_SOURCE_CURRENT:
        	return TRUE;
		default:
			m_wLastError = COCO_ERR_WRONG_SOURCE;
			return FALSE;
	}		
} 

/////////////////////////////////////////////////////////////////////////////
// CCoCo::CheckExtCard
BOOL CCoCo::CheckExtCard(WORD wExtCard)
{
	switch(wExtCard)
	{
		case COCO_EXTCARD0: 
		case COCO_EXTCARD1: 
		case COCO_EXTCARD2: 
		case COCO_EXTCARD3:
		case COCO_EXTCARD_MEGRA:
		case COCO_EXTCARD_CURRENT: 
        	return (TRUE);
		default:
			m_wLastError = COCO_ERR_WRONG_EXT_CARD;
			return FALSE;
	}		
} 

/////////////////////////////////////////////////////////////////////////////
// CCoCo::CheckSourceType
BOOL CCoCo::CheckSourceType(WORD wSourceType)
{
	switch(wSourceType)
	{
		case COCO_FBAS: 
		case COCO_SVHS: 
        	return TRUE;
		default:
			m_wLastError = COCO_ERR_WRONG_SOURCE_TYPE;
			return FALSE;
	}		
} 

/////////////////////////////////////////////////////////////////////////////
// CCoCo::CheckFeVideoFormat
BOOL CCoCo::CheckFeVideoFormat(WORD wFeVideoFormat)
{
	switch(wFeVideoFormat)
	{
		case COCO_PAL: 
		case COCO_SECAM:
		case COCO_NTSC: 
        	return TRUE;
		default:
			m_wLastError = COCO_ERR_WRONG_FE_VIDEO_FORMAT;
			return FALSE;
	}		
} 

/////////////////////////////////////////////////////////////////////////////
// CCoCo::CheckBeVideoFormat
BOOL CCoCo::CheckBeVideoFormat(WORD wBeVideoFormat)
{
	switch(wBeVideoFormat)
	{
		case COCO_PAL: 
		case COCO_NTSC: 
        	return (TRUE);
		default:
			m_wLastError = COCO_ERR_WRONG_BE_VIDEO_FORMAT;
			return FALSE;
	}		
} 

/////////////////////////////////////////////////////////////////////////////
// CCoCo::CheckBeVideoType
BOOL CCoCo::CheckBeVideoType(WORD wBeVideoType)
{
	switch(wBeVideoType)
	{
		case COCO_COMPOSITE: 
		case COCO_RGB: 
        	return TRUE;
		default:
			m_wLastError = COCO_ERR_WRONG_BE_VIDEO_TYPE;
			return FALSE;
	}		
} 

/////////////////////////////////////////////////////////////////////////////
// CCoCo::CheckFeFilter
BOOL CCoCo::CheckFeFilter(WORD wFilter)
{
	if (wFilter <= (COCO_LNOTCH | COCO_LDEC))
		return TRUE;
	else
		m_wLastError = COCO_ERR_WRONG_FILTER_TYPE;
		return FALSE;
}
 
/////////////////////////////////////////////////////////////////////////////
// CCoCo::CheckEncoderFormat
BOOL CCoCo::CheckEncoderFormat(WORD wFormat)
{
	switch(wFormat)
	{
		case COCO_ENCODER_CIF: 
		case COCO_ENCODER_QCIF: 
        	return (TRUE);
		default:
			m_wLastError = COCO_ERR_WRONG_ENCODER_FORMAT;
			return FALSE;
	}		
} 

/////////////////////////////////////////////////////////////////////////////
// CCoCo::CheckEncoderFramerate
BOOL CCoCo::CheckEncoderFramerate(WORD wFramerate)
{
	if ((wFramerate >= COCO_MIN_MPI) && (wFramerate <= COCO_MAX_MPI))
		return TRUE;

	m_wLastError = COCO_ERR_WRONG_ENCODER_FRAMERATE;
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CCoCo::CheckEncoderBitrate
BOOL CCoCo::CheckEncoderBitrate(DWORD dwBitrate)
{
	if ((dwBitrate >= COCO_ENC_MIN_BITRATE) && (dwBitrate <= COCO_ENC_MAX_BITRATE))
		return (TRUE);
	else
	{
		m_wLastError = COCO_ERR_WRONG_ENCODER_BITRATE;
		return FALSE;
	}		
} 
 
/////////////////////////////////////////////////////////////////////////////
// CCoCo::CheckEncoderFormat
BOOL CCoCo::CheckVideoOutput(WORD wConnect)
{
	switch(wConnect)
	{
		case COCO_VIDIN: 
		case COCO_PREP: 
		case COCO_DECODER: 
		case COCO_ENCODER: 
			return (TRUE);
		default:
			m_wLastError = COCO_ERR_WRONG_AVP_PROZESS;
			return FALSE;
	}		
} 

/*********************************************************************************/
/* Aufruf:    BOOL CheckRange(LPWORD lpwValue, long lMin, long lMax);            */
/* Parameter: lpwValue = Pointer auf eine WORD-Variable, dessen Inhalt auf      */
/*                        Grenzen untersucht wird.                               */
/*            lMin      = Kleinstwert der zugelassen ist                         */
/*            lMax      = Größtwert der zugelassen ist                           */
/* Rückgabe:  TRUE -> Der untersuchte Wert liegt innerhalb der angegebenen       */
/*                    Grenzen, ansonsten FALSE                                   */
/* Zweck:     Testet ob der Varableninhalt auf den 'lpwValue' zeigt innerhalb    */
/*            der Grenzen 'lMin' und 'lMax' liegt. Ist er kleiner als 'lMin',    */
/*            so wird er auf 'lMin' zurückgesetzt, ist er hingegen größer als    */
/*            'lMax', so wird er auf 'lMax' gesetzt.                             */
/* Bemerkung: Liegt der Variableninhalt außerhalb der Grenzen, so wird ein       */
/*            Fehler vom Typ VDE_RANGE erzeugt.                                  */
/*********************************************************************************/ 
BOOL CCoCo::CheckRange(LPWORD lpwValue, long lMin, long lMax)
{
	if (!lpwValue)
	{
		WK_TRACE_WARNING("CCoCo::CheckRange\tNullPointer in 'CheckRange'\n");
		return FALSE;
	}

	if (*lpwValue < (WORD)lMin)
	{
		m_wLastError = COCO_ERR_PARM_OUT_OF_RANGE;
		*lpwValue = (WORD)lMin;
		return FALSE;
	}
	else if (*lpwValue > (WORD)lMax)
	{
		m_wLastError = COCO_ERR_PARM_OUT_OF_RANGE;
		*lpwValue = (WORD)lMax;
		return FALSE;
	
	}
	return TRUE;
}
