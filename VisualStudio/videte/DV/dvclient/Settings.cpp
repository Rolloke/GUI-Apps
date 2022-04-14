// Settings.cpp: implementation of the CSettings class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DVClient.h"
#include "Settings.h"
#include "CPanel.h"
#include "MainFrame.h"
#include "LiveWindow.h"
#include "Server.h"
#include "IPCOutputDVClient.h"
#include "CIPCAudioDVClient.h"
#include "MaintenanceDlg.h"

extern CDVClientApp theApp;



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
CSettings::CSettings(const CString& sExportDir)
{
	m_sExportDir	= sExportDir;
	m_sHomeDir		= theApp.GetHomeDir() + _T("\\");
	m_sIniFile		= _T("Settings.ini");
	m_pMainWnd		= theApp.GetMainFrame();
	m_nNrOfCameras	= 0;
	m_sOem			= _T("");
	m_sBuildNr		= _T("");

	// Soll später anhand der Seriennummer gesetzt werden. D.h. auf dem Exportrechner
	// sollen alle gesicherten Einstellungen auch wieder importiert werden. Auf allen
	// anderen jeweils nur eine noch zu definierende Teilmenge der Einstellungen
	m_bImportAllow	= TRUE;
	m_bDecode		= TRUE;
	
	if (m_bDecode)
 		m_sIniFile		= _T("Settings.dat");
	else
		m_sIniFile		= _T("Settings.ini");

	CPanel* pPanel = theApp.GetPanel();
	if (pPanel)
	{
		m_nNrOfCameras	= pPanel->GetNrOfCameras();
		m_sOem.Format(_T("%d"), pPanel->GetOem());
	}
	
	// BuildNr holen.
	CServer* pServer = theApp.GetServer();
	if (pServer)
		m_sBuildNr.Format(_T("%d"), pServer->GetBuildNr());
}

/////////////////////////////////////////////////////////////////////////////
CSettings::~CSettings()
{

}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::Export()
{
	BOOL bErr			= FALSE;
	BOOL bTempLiveWnd	= FALSE;
	m_eErrorCode		= eStatusOk;
	CDisplayWindows*pDisplays = m_pMainWnd->GetDisplayWindows();

	// Kameraspezifische Einstellungen sichern
	for (int nI = 0; nI < m_nNrOfCameras; nI++)
	{
		CLiveWindow* pLiveWnd = m_pMainWnd->GetLiveWindow((WORD)nI);

		// Existiert zu dieser Kamera ein Livewindow?
		if (!pLiveWnd)
		{	
			// Nein, dann temporär ein Livewindow anlegen und in die Liste hängen 
			pLiveWnd = CreateTempLiveWindow((WORD)nI);

			if (pDisplays)
			{
				pDisplays->Add(pLiveWnd);
			}	
			bTempLiveWnd = TRUE;
		}
		else
			bTempLiveWnd = FALSE;
		
		if (pLiveWnd)
		{
			if (!bErr && !ExportBrightness(pLiveWnd))
				bErr = TRUE;
			if (!bErr && !ExportContrast(pLiveWnd))
				bErr = TRUE;
			if (!bErr && !ExportSaturation(pLiveWnd))
				bErr = TRUE;
			if (!bErr && !ExportCameraName(pLiveWnd))
				bErr = TRUE;
			if (!bErr && !ExportMDAlarmSensitivity(pLiveWnd))
				bErr = TRUE;
			if (!bErr && !ExportMDMaskSensitivity(pLiveWnd))
				bErr = TRUE;
			if (!bErr && !ExportTimer(pLiveWnd))
				bErr = TRUE;
			if (!bErr && !ExportRecordingMode(pLiveWnd))
				bErr = TRUE;
			if (!bErr && !ExportAlarmCallEvent(pLiveWnd))
				bErr = TRUE;
			if (!bErr && !ExportSequencerDwellTime(pLiveWnd))
				bErr = TRUE;

			//ab Version 2.5.0.612
			if (!bErr && !ExportCameraCompression(pLiveWnd))
				bErr = TRUE;
			if (!bErr && !ExportCameraType(pLiveWnd))
				bErr = TRUE;
			if (!bErr && !ExportCameraID(pLiveWnd))
				bErr = TRUE;
			if (!bErr && !ExportCameraComport(pLiveWnd))
				bErr = TRUE;

			//ab Version 2.xx
			if (!bErr && !ExportAudio(theApp.GetLocalAudio()))
			{
//				bErr = TRUE;
			}

			//TODO TKR testen
			if(!bErr && !ExportCameraFPS(pLiveWnd))
				bErr = TRUE;
			

		}
		if (bTempLiveWnd)
		{
			// Temporäres LiveWindow wieder aus der Liste entfernen
			// und zerstören.
			CAutoCritSec acs(&pDisplays->m_cs);
			pDisplays->Remove(pLiveWnd);
			pLiveWnd->DestroyWindow();
			WK_DELETE(pLiveWnd);
		}
	}

	// Diese Einstellungen sind NICHT pro Kamera
	if (!bErr && !ExportTimerOnOffState())
		bErr = TRUE;
	if (!bErr && !ExportAlarmOutputOnOffState())
		bErr = TRUE;
	if (!bErr && !ExportAlarmDialingNumbers())
		bErr = TRUE;
	if (!bErr && !ExportVideoOutputMode())
		bErr = TRUE;
	if (!bErr && !ExportIPAddress())
		bErr = TRUE;
	if (!bErr && !ExportSubnetmask())
		bErr = TRUE;
	if (!bErr && !ExportGateway())
		bErr = TRUE;
	if (!bErr && !ExportMSN())
		bErr = TRUE;
	if (!bErr && !ExportLocalHostName())
		bErr = TRUE;
	if (!bErr && !ExportTimeZoneIndex())
		bErr = TRUE;
	if (!bErr && !ExportDefaultLanguage())
		bErr = TRUE;
	if (!bErr && !ExportTargetCrossOnOff())
		bErr = TRUE;
	if (!bErr && !ExportAudioAlarmOnOff())
		bErr = TRUE;
	if (!bErr && !ExportCameraResolutionHighLow())
		bErr = TRUE;
	if (!bErr && !ExportComputerName())
		bErr = TRUE;
	if (!bErr && !ExportOem())
		bErr = TRUE;
	if (!bErr && !ExportBuildNr())
		bErr = TRUE; 
	if (!bErr && !ExportXpeLicense())
		bErr = TRUE;
	if (!bErr && !ExportDongleInformation())
		bErr = TRUE;
	if (!bErr && !ExportNumberOfCameras())
		bErr = TRUE;

	//ab Version 2.5.0.612
	if (!bErr && !ExportRealtime())
		bErr = TRUE;
	
	//ab Version 3.1.0.840
	if (!bErr && !ExportMaintenance())
		bErr = TRUE;

	if (!bErr)
	{
		if (!CopyIniFileToMedium())
			bErr = TRUE;
	}

	return !bErr;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::Import()
{
	BOOL bErr = FALSE;
	BOOL bTempLiveWnd	= FALSE;
	CDisplayWindows*pDisplays = m_pMainWnd->GetDisplayWindows();

	if (GetIniFileFromMedium())
	{
		if (CheckOem() && CheckBuildNr())
		{
			// Stammt das IniFile von diesem Rechner?
			m_bImportAllow = CheckComputerName();

			// Anzahl der in der Settings.ini eingetragenen Kameras. holen
			// Es wird nur der kleinste gemeinsame Anzahl der Kameras importiert.
			int nNrOfCameras = 0;
			if (ImportNumberOfCameras(nNrOfCameras))
			{
				// Kameraspezifische Einstellungen laden
				for (int nI = 0; nI < min(m_nNrOfCameras, nNrOfCameras); nI++)
				{
					CLiveWindow* pLiveWnd = m_pMainWnd->GetLiveWindow((WORD)nI);

					// Existiert zu dieser Kamera ein Livewindow?
					if (!pLiveWnd)
					{	
						// Nein, dann temporär ein Livewindow anlegen und in die Liste hängen 
						pLiveWnd = CreateTempLiveWindow((WORD)nI);

						if (pDisplays)
						{
							pDisplays->Add(pLiveWnd);
						}	
						bTempLiveWnd = TRUE;
					}
					else
						bTempLiveWnd = FALSE;
					
					if (pLiveWnd)
					{
						if (!bErr && !ImportBrightness(pLiveWnd))
							bErr = TRUE;
						if (!bErr && !ImportContrast(pLiveWnd))
							bErr = TRUE;
						if (!bErr && !ImportSaturation(pLiveWnd))
							bErr = TRUE;
						if (!bErr && !ImportCameraName(pLiveWnd))
							bErr = TRUE;
						if (!bErr && !ImportMDAlarmSensitivity(pLiveWnd))
							bErr = TRUE;
						if (!bErr && !ImportMDMaskSensitivity(pLiveWnd))
							bErr = TRUE;
						if (!bErr && !ImportTimer(pLiveWnd))
							bErr = TRUE;
	//					if (!bErr && !ImportRecordingMode(pLiveWnd))
	//						bErr = TRUE;
						if (!bErr && !ImportAlarmCallEvent(pLiveWnd))
							bErr = TRUE;
						if (!bErr && !ImportSequencerDwellTime(pLiveWnd))
							bErr = TRUE;

						//ab Version 2.5.0.612
						if (!bErr && !ImportCameraCompression(pLiveWnd))
							bErr = TRUE;
						if (!bErr && !ImportCameraType(pLiveWnd))
							bErr = TRUE;
						if (!bErr && !ImportCameraID(pLiveWnd))
							bErr = TRUE;
						if (!bErr && !ImportCameraComport(pLiveWnd))
							bErr = TRUE;

						//ab Version 3.0.0.809
						//TODO TKR testen
						if (!bErr && !ImportCameraFPS(pLiveWnd))
							bErr = TRUE;

//						if (!bErr && !ImportAudio(theApp.GetLocalAudio()))
						{
//							bErr = TRUE;
						}

					}
				
					if (bTempLiveWnd)
					{
						// Temporäres LiveWindow wieder aus der Liste entfernen
						// und zerstören.
						CAutoCritSec acs(&pDisplays->m_cs);
						pDisplays->Remove(pLiveWnd);
						pLiveWnd->DestroyWindow();
						WK_DELETE(pLiveWnd);
					}		
				}
			}

			// Diese Einstellungen sind NICHT pro Kamera
			if (!bErr && !ImportTimerOnOffState())
				bErr = TRUE;
			if (!bErr && !ImportAlarmOutputOnOffState())
				bErr = TRUE;
			if (!bErr && !ImportAlarmDialingNumbers())
				bErr = TRUE;
			if (!bErr && !ImportVideoOutputMode())
				bErr = TRUE;
			if (!bErr && !ImportIPAddress())
				bErr = TRUE;
			if (!bErr && !ImportSubnetmask())
				bErr = TRUE;
			if (!bErr && !ImportGateway())
				bErr = TRUE;
			if (!bErr && !ImportMSN())
				bErr = TRUE;
			if (!bErr && !ImportLocalHostName())
				bErr = TRUE;
			if (!bErr && !ImportTimeZoneIndex())
				bErr = TRUE;
			if (!bErr && !ImportDefaultLanguage())
				bErr = TRUE;
			if (!bErr && !ImportTargetCrossOnOff())
				bErr = TRUE;
			if (!bErr && !ImportAudioAlarmOnOff())
				bErr = TRUE;
			if (!ImportCameraResolutionHighLow())
				bErr = TRUE;
				
			//ab Version 2.5.0.612
			if (!bErr && !ImportRealtime())
				bErr = TRUE;

			//ab Version 3.1.0.840
			if (!bErr && !ImportMaintenance())
				bErr = TRUE;
		}
		else
			bErr = TRUE;
	
		// Die Kopie der zu importierenden Settings.ini löschen
		RemoveIniFile(m_sHomeDir + m_sIniFile);
	}
	else
		bErr = TRUE;

	return !bErr;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ExportBrightness(CLiveWindow* pLiveWnd)
{
	BOOL bResult = FALSE;
	CString sSection;
	int nVal = 0;
	int nCameraNr = pLiveWnd->GetID().GetSubID();

	sSection.Format(REGKEY_CAMERAXX, nCameraNr);
	if (pLiveWnd->GetBrightness(nVal))
	{
		CString sVal;
		sVal.Format(_T("%d"), nVal);
		if (WritePrivateProfileString(sSection, REGKEY_BRIGHTNESS, sVal, m_sHomeDir + m_sIniFile))
		{
			bResult = TRUE;
		}
		else
		{
			m_eErrorCode = eErrorWritePrivateProfileStringFailed;
			WK_TRACE_ERROR(_T("ExportBrightness: WritePrivateProfileString failed\n"));
		}
	}
	else
	{
		m_eErrorCode = eErrorGetBrightnessFailed;
		WK_TRACE_ERROR(_T("ExportBrightness: GetBrightness failed\n"));
	}

	return bResult;
}


/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ExportContrast(CLiveWindow* pLiveWnd)
{
	BOOL bResult = FALSE;
	CString sSection;
	int nVal = 0;
	int nCameraNr = pLiveWnd->GetID().GetSubID();

	sSection.Format(REGKEY_CAMERAXX, nCameraNr);
	if (pLiveWnd->GetContrast(nVal))
	{
		CString sVal;
		sVal.Format(_T("%d"), nVal);
		if (WritePrivateProfileString(sSection, REGKEY_CONTRAST, sVal, m_sHomeDir + m_sIniFile))
		{
			bResult = TRUE;
		}
		else
		{
			m_eErrorCode = eErrorWritePrivateProfileStringFailed;
			WK_TRACE_ERROR(_T("ExportContrast: WritePrivateProfileString failed\n"));
		}
	}
	else
	{
		m_eErrorCode = eErrorGetContrastFailed;
		WK_TRACE_ERROR(_T("ExportContrast: GetContrast failed\n"));
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ExportSaturation(CLiveWindow* pLiveWnd)
{
	BOOL bResult = FALSE;
	CString sSection;
	int nVal = 0;
	int nCameraNr = pLiveWnd->GetID().GetSubID();

	sSection.Format(REGKEY_CAMERAXX, nCameraNr);

	if (pLiveWnd->GetSaturation(nVal))
	{
		CString sVal;
		sVal.Format(_T("%d"), nVal);
		if (WritePrivateProfileString(sSection, REGKEY_SATURATION, sVal, m_sHomeDir + m_sIniFile))
		{
			bResult = TRUE;
		}
		else
		{
			m_eErrorCode = eErrorWritePrivateProfileStringFailed;
			WK_TRACE_ERROR(_T("ExportSaturation: WritePrivateProfileString failed\n"));
		}
	}
	else
	{
		m_eErrorCode = eErrorGetSaturationFailed;
		WK_TRACE_ERROR(_T("ExportSaturation: GetSaturation failed\n"));
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ExportCameraName(CLiveWindow* pLiveWnd)
{
	CString sSection;
	BOOL bResult = FALSE;
	int nCameraNr = pLiveWnd->GetID().GetSubID();

	sSection.Format(REGKEY_CAMERAXX, nCameraNr);
	CString sVal = pLiveWnd->GetName();
	if (WritePrivateProfileString(sSection, REGKEY_CAMERANAME, sVal, m_sHomeDir + m_sIniFile))
		bResult = TRUE;
	else
	{
		m_eErrorCode = eErrorWritePrivateProfileStringFailed;
		WK_TRACE_ERROR(_T("ExportCameraName: WritePrivateProfileString failed\n"));
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ExportMDAlarmSensitivity(CLiveWindow* pLiveWnd)
{
	CString sSection;
	BOOL bResult = FALSE;
	int nCameraNr = pLiveWnd->GetID().GetSubID();

	sSection.Format(REGKEY_CAMERAXX, nCameraNr);
	CString sVal;
	sVal.Format(_T("%d"), pLiveWnd->GetMDAlarmSensitivity());
	if (WritePrivateProfileString(sSection, REGKEY_MDALARMSENSITIVITY, sVal, m_sHomeDir + m_sIniFile))
		bResult = TRUE;
	else
	{
		m_eErrorCode = eErrorWritePrivateProfileStringFailed;
		WK_TRACE_ERROR(_T("ExportMDAlarmSensitivity: WritePrivateProfileString failed\n"));
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ExportMDMaskSensitivity(CLiveWindow* pLiveWnd)
{
	CString sSection;
	BOOL bResult = FALSE;
	int nCameraNr = pLiveWnd->GetID().GetSubID();

	sSection.Format(REGKEY_CAMERAXX, nCameraNr);
	CString sVal;
	sVal.Format(_T("%d"), pLiveWnd->GetMDMaskSensitivity());
	if (WritePrivateProfileString(sSection, REGKEY_MDMASKSENSITIVITY, sVal, m_sHomeDir + m_sIniFile))
		bResult = TRUE;
	else
	{
		m_eErrorCode = eErrorWritePrivateProfileStringFailed;
		WK_TRACE_ERROR(_T("ExportMDMaskSensitivity: WritePrivateProfileString failed\n"));
	}
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ExportTimer(CLiveWindow* pLiveWnd)
{
	BOOL bErr = FALSE;
	CString sSection;
	CString sVal;
	CString sKey;

	int nCameraNr = pLiveWnd->GetID().GetSubID();
	sSection.Format(REGKEY_CAMERAXX, nCameraNr);

	for (int nDayOfWeek = 0; nDayOfWeek < 7; nDayOfWeek++)
	{
		sVal = pLiveWnd->GetTimer(nDayOfWeek);
		sKey.Format(REGKEY_TIMER, nDayOfWeek);

		if (!WritePrivateProfileString(sSection, sKey, sVal, m_sHomeDir + m_sIniFile))
		{
			m_eErrorCode = eErrorWritePrivateProfileStringFailed;
			WK_TRACE_ERROR(_T("ExportTimer (DOW=%d): WritePrivateProfileString failed\n"), nDayOfWeek);
			bErr = TRUE;
		}
	}

	return !bErr;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ExportRecordingMode(CLiveWindow* pLiveWnd)
{
	CString sSection;
	BOOL bResult = FALSE;
	int nCameraNr = pLiveWnd->GetID().GetSubID();

	sSection.Format(REGKEY_CAMERAXX, nCameraNr);
	CString sVal = (pLiveWnd->IsMD() ? _T("MD") : _T("UVV"));
		
	if (WritePrivateProfileString(sSection, REGKEY_RECORDINGMODE, sVal, m_sHomeDir + m_sIniFile))
		bResult = TRUE;
	else
	{
		m_eErrorCode = eErrorWritePrivateProfileStringFailed;
		WK_TRACE_ERROR(_T("ExportRecordingMode: WritePrivateProfileString failed\n"));
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ExportAlarmCallEvent(CLiveWindow* pLiveWnd)
{
 	CString sSection;
	CString sVal;
	BOOL bResult		= FALSE;
	int nCameraNr		= pLiveWnd->GetID().GetSubID();
	int nAlarmCallEvent = pLiveWnd->GetAlarmCallEvent();

	sSection.Format(REGKEY_CAMERAXX, nCameraNr);

	sVal.Format(_T("%d"), nAlarmCallEvent);

	if (WritePrivateProfileString(sSection, REGKEY_ALARMCALLEVENT, sVal, m_sHomeDir + m_sIniFile))
		bResult = TRUE;
 	else
	{
		m_eErrorCode = eErrorWritePrivateProfileStringFailed;
		WK_TRACE_ERROR(_T("ExportAlarmCallEvent: WritePrivateProfileString failed\n"));
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ExportSequencerDwellTime(CLiveWindow* pLiveWnd)
{
 	CString sSection;
	CString sVal;
	BOOL bResult	= FALSE;
	int nCameraNr	= pLiveWnd->GetID().GetSubID();
	int nVal		= pLiveWnd->GetAlarmCallEvent();

	sSection.Format(REGKEY_CAMERAXX, nCameraNr);

	if (pLiveWnd->GetSequencerDwellTime(nVal))
	{
		sVal.Format(_T("%d"), nVal);

		if (WritePrivateProfileString(sSection, REGKEY_SEQUENZDWELLTIME, sVal, m_sHomeDir + m_sIniFile))
			bResult = TRUE;
 		else
		{
			m_eErrorCode = eErrorWritePrivateProfileStringFailed;
			WK_TRACE_ERROR(_T("ExportSequencerDwellTime: WritePrivateProfileString failed\n"));
		}
	}
 	else
	{
		WK_TRACE_ERROR(_T("GetSequencerDwellTime failed\n"));
		m_eErrorCode = eErrorGetSequencerDwellTimeFailed;
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ExportComputerName()
{
	BOOL bResult = FALSE;

	if (WritePrivateProfileString(REGKEY_OTHERSETTINGS, REGKEY_COMPUTERNAME, theApp.GetComputerName(), m_sHomeDir + m_sIniFile))
		bResult = TRUE;
 	else
	{
		m_eErrorCode = eErrorWritePrivateProfileStringFailed;
		WK_TRACE_ERROR(_T("ExportComputerName: WritePrivateProfileString failed\n"));
	}

	return bResult;
}


/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ExportOem()
{
	BOOL bResult = FALSE;

	if (WritePrivateProfileString(REGKEY_OTHERSETTINGS, REGKEY_OEM, m_sOem, m_sHomeDir + m_sIniFile))
		bResult = TRUE;
  	else
	{
		m_eErrorCode = eErrorWritePrivateProfileStringFailed;
		WK_TRACE_ERROR(_T("ExportOem: WritePrivateProfileString failed\n"));
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ExportBuildNr()
{
	BOOL bResult = FALSE;

	if (WritePrivateProfileString(REGKEY_OTHERSETTINGS, REGKEY_BUILDNUMBER, m_sBuildNr, m_sHomeDir + m_sIniFile))
		bResult = TRUE;
   	else
	{
		m_eErrorCode = eErrorWritePrivateProfileStringFailed;
		WK_TRACE_ERROR(_T("ExportBuildNr: WritePrivateProfileString failed\n"));
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ExportTimerOnOffState()
{
	BOOL bResult = FALSE;

	CServer* pServer = theApp.GetServer();
	if (pServer)
	{
		BOOL bTimerOnOffState = FALSE;
		if (pServer->GetTimerOnOffState(bTimerOnOffState))
		{
			CString sVal = (bTimerOnOffState ? _T("On") : _T("Off"));
			if (WritePrivateProfileString(REGKEY_OTHERSETTINGS, REGKEY_TIMERONOFF, sVal, m_sHomeDir + m_sIniFile))
				bResult = TRUE;
   			else
			{
				m_eErrorCode = eErrorWritePrivateProfileStringFailed;
				WK_TRACE_ERROR(_T("ExportTimerOnOffState: WritePrivateProfileString failed\n"));
			}
		}
 		else
		{
			m_eErrorCode = eErrorGetTimerOnOffStateFailed;
			WK_TRACE_ERROR(_T("GetTimerOnOffState failed\n"));
		}
	}
	else
	{
		m_eErrorCode = eErrorNoServerObject;
		WK_TRACE_ERROR(_T("ExportTimerOnOffState: No Server object\n"));
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ExportAlarmOutputOnOffState()
{
	BOOL bResult = FALSE;
	BOOL bAlarmOutputOnOffState = FALSE;

	CServer* pServer = theApp.GetServer();
	if (pServer)
	{
		if (pServer->GetAlarmOutputOnOffState(bAlarmOutputOnOffState))
		{
			CString sVal = (bAlarmOutputOnOffState ? _T("On") : _T("Off"));
			if (WritePrivateProfileString(REGKEY_OTHERSETTINGS, REGKEY_ALARMOUTPUTONOFF, sVal, m_sHomeDir + m_sIniFile))
				bResult = TRUE;
   			else
			{
				m_eErrorCode = eErrorWritePrivateProfileStringFailed;
				WK_TRACE_ERROR(_T("ExportAlarmOutputOnOffState: WritePrivateProfileString failed\n"));
			}
		}	
 		else
		{
			m_eErrorCode = eErrorGetAlarmOutputOnOffStateFailed;
			WK_TRACE_ERROR(_T("GetAlarmOutputOnOffState failed\n"));
		}
	}
	else
	{
		m_eErrorCode = eErrorNoServerObject;
		WK_TRACE_ERROR(_T("ExportAlarmOutputOnOffState: No Server object\n"));
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ExportAlarmDialingNumbers()
{
	BOOL bErr = FALSE;

	CServer* pServer = theApp.GetServer();
	if (pServer)
	{
		CString sKey;
		CString sVal;
		for (int nCallingNumber = 0; nCallingNumber < 3; nCallingNumber++)
		{
			if (pServer->GetAlarmDialingNumber(nCallingNumber, sVal))
			{
				sKey.Format(REGKEY_ALARMDIALINGNUMBER, nCallingNumber);

				if (!WritePrivateProfileString(REGKEY_OTHERSETTINGS, sKey, sVal, m_sHomeDir + m_sIniFile))
				{
					m_eErrorCode = eErrorWritePrivateProfileStringFailed;
					WK_TRACE_ERROR(_T("ExportAlarmDialingNumbers (Nr.%d): WritePrivateProfileString failed\n"), nCallingNumber);
					bErr = TRUE;
				}
			}
			else
			{
				m_eErrorCode = eErrorGetAlarmDialingNumberFailed;
				WK_TRACE_ERROR(_T("GetAlarmDialingNumber failed\n"));
				bErr = TRUE;
			}
		}
	}
	else
	{
		m_eErrorCode = eErrorNoServerObject;
		WK_TRACE_ERROR(_T("ExportAlarmDialingNumbers: No Server object\n"));
		bErr = TRUE;
	}

	return !bErr;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ExportVideoOutputMode()
{
	BOOL bErr = FALSE;

	CServer* pServer = theApp.GetServer();
	if (pServer)
	{
		CString sCSDPort;
		CString sKey;
		CString sVal;
		enumVideoOutputMode eVideoOutputMode;

		for (int nPort = 0; nPort <= 3; nPort++)
		{
			switch (nPort)
			{
				case 0:	sCSDPort = CSD_PORT0;	break;
				case 1:	sCSDPort = CSD_PORT1;	break;
				case 2:	sCSDPort = CSD_PORT2;	break;
				case 3:	sCSDPort = CSD_PORT3;	break;
			}

			if (pServer->GetVideoOutputMode(sCSDPort, eVideoOutputMode))
			{
				sKey.Format(REGKEY_VIDEOOUTPORT, nPort);
				sVal.Format(_T("%d"), eVideoOutputMode);

				if (!WritePrivateProfileString(REGKEY_OTHERSETTINGS, sKey, sVal, m_sHomeDir + m_sIniFile))
				{
					m_eErrorCode = eErrorWritePrivateProfileStringFailed;
					WK_TRACE_ERROR(_T("ExportVideoOutputMode (Port.%d): WritePrivateProfileString failed\n"), nPort);
					bErr = TRUE;
				}
			}
			else
			{
				m_eErrorCode = eErrorGetVideoOutputModeFailed;
				WK_TRACE_ERROR(_T("GetVideoOutputMode failed\n"));
				bErr = TRUE;
			}
		}
	}
	else
	{
		m_eErrorCode = eErrorNoServerObject;
		WK_TRACE_ERROR(_T("ExportVideoOutputMode: No Server object\n"));
		bErr = TRUE;
	}

	return !bErr;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ExportIPAddress()
{
	BOOL bResult = FALSE;

	CPanel* pPanel = theApp.GetPanel();
	if (pPanel)
	{
		CString sIP;
		if (pPanel->GetIP(sIP, IPADDRESS))
		{
			if (WritePrivateProfileString(REGKEY_OTHERSETTINGS, REGKEY_LOCALIPADDRESS, sIP, m_sHomeDir + m_sIniFile))
				bResult = TRUE;
			else
			{
				m_eErrorCode = eErrorWritePrivateProfileStringFailed;
				WK_TRACE_ERROR(_T("ExportIPAddress: WritePrivateProfileString failed\n"));
			}
		}
		else
		{
			m_eErrorCode = eErrorGetIPAddressFailed;
			WK_TRACE_ERROR(_T("GetIP(IPADDRESS) failed\n"));
		}
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ExportSubnetmask()
{
	BOOL bResult = FALSE;

	CPanel* pPanel = theApp.GetPanel();
	if (pPanel)
	{
		CString sIP;
		if (pPanel->GetIP(sIP, SUBNETMASK))
		{
			if (WritePrivateProfileString(REGKEY_OTHERSETTINGS, REGKEY_LOCALSUBNETMASK, sIP, m_sHomeDir + m_sIniFile))
				bResult = TRUE;
			else
			{
				m_eErrorCode = eErrorWritePrivateProfileStringFailed;
				WK_TRACE_ERROR(_T("ExportSubnetmask: WritePrivateProfileString failed\n"));
			}
		}
		else
		{
			m_eErrorCode = eErrorGetSubnetMaskAddressFailed;
			WK_TRACE_ERROR(_T("GetIP(SUBNETMASK) failed\n"));
		}
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ExportGateway()
{
	BOOL bResult = FALSE;

	CPanel* pPanel = theApp.GetPanel();
	if (pPanel)
	{
		CString sIP;
		if (pPanel->GetIP(sIP, DEFAULTGATEWAY))
		{
			if (WritePrivateProfileString(REGKEY_OTHERSETTINGS, REGKEY_LOCALDEFAULTGATEWAY, sIP, m_sHomeDir + m_sIniFile))
				bResult = TRUE;
			else
			{
				m_eErrorCode = eErrorWritePrivateProfileStringFailed;
				WK_TRACE_ERROR(_T("ExportGateway: WritePrivateProfileString failed\n"));
			}
		}
		else
		{
			m_eErrorCode = eErrorGetGatewayAddressFailed;
			WK_TRACE_ERROR(_T("GetIP(DEFAULTGATEWAY) failed\n"));
		}
	}

	return bResult;
	
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ExportMSN()
{
	BOOL bResult = FALSE;

	CPanel* pPanel = theApp.GetPanel();
	if (pPanel)
	{
		if (WritePrivateProfileString(REGKEY_OTHERSETTINGS, REGKEY_LOCALMSN, pPanel->GetMSN(), m_sHomeDir + m_sIniFile))
			bResult = TRUE;
		else
		{
			m_eErrorCode = eErrorWritePrivateProfileStringFailed;
			WK_TRACE_ERROR(_T("ExportMSN: WritePrivateProfileString failed\n"));
		}
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ExportLocalHostName()
{
	BOOL bResult = FALSE;

	CPanel* pPanel = theApp.GetPanel();
	if (pPanel)
	{
		if (WritePrivateProfileString(REGKEY_OTHERSETTINGS, REGKEY_LOCALHOSTNAME, pPanel->GetLocalHostName(), m_sHomeDir + m_sIniFile))
			bResult = TRUE;
		else
		{
			m_eErrorCode = eErrorWritePrivateProfileStringFailed;
			WK_TRACE_ERROR(_T("ExportLocalHostName: WritePrivateProfileString failed\n"));
		}
	}
	
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ExportTimeZoneIndex()
{
	BOOL bResult = FALSE;

	CPanel* pPanel = theApp.GetPanel();
	if (pPanel)
	{
		CString sVal;
		CTimeZoneInformation CTzi;

		if (pPanel->OnGetTimeZone(CTzi))
		{
			sVal.Format(_T("%d"), pPanel->OnGetTimeZones());
		
			if (WritePrivateProfileString(REGKEY_OTHERSETTINGS, REGKEY_TIMEZONEINDEX, sVal, m_sHomeDir + m_sIniFile))
				bResult = TRUE;
			else
			{
				m_eErrorCode = eErrorWritePrivateProfileStringFailed;
				WK_TRACE_ERROR(_T("ExportTimeZoneIndex: WritePrivateProfileString failed\n"));
			}
		}
		else
		{
			m_eErrorCode = eErrorGetTimeZoneFailed;
			WK_TRACE_ERROR(_T("OnGetTimeZone failed\n"));
		}
	}
	
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ExportDefaultLanguage()
{
	BOOL bResult = FALSE;

	CPanel* pPanel = theApp.GetPanel();
	if (pPanel)
	{
		CString sVal = pPanel->GetCurrentLanguage();
		if (!sVal.IsEmpty())
		{
			if (WritePrivateProfileString(REGKEY_OTHERSETTINGS, REGKEY_DEFAULTLANGUAGE, sVal, m_sHomeDir + m_sIniFile))
				bResult = TRUE;
			else
			{
				m_eErrorCode = eErrorWritePrivateProfileStringFailed;
				WK_TRACE_ERROR(_T("ExportDefaultLanguage: WritePrivateProfileString failed\n"));
			}
		}
		else
		{
			m_eErrorCode = eErrorGetCurrentLanguageFailed;
			WK_TRACE_ERROR(_T("GetCurrentLanguage failed\n"));
		}
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ExportTargetCrossOnOff()
{
	BOOL bResult = FALSE;

	BOOL bShowTargetCrossActual = CDisplayWindow::GetShowTargetCross();
	CString sVal;
	sVal.Format(_T("%d"), bShowTargetCrossActual);

	if (WritePrivateProfileString(REGKEY_OTHERSETTINGS, REGKEY_TARGETCROSS, sVal, m_sHomeDir + m_sIniFile))
		bResult = TRUE;
	else
	{
		m_eErrorCode = eErrorWritePrivateProfileStringFailed;
		WK_TRACE_ERROR(_T("ExportTargetCrossOnOff: WritePrivateProfileString failed\n"));
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ExportAudioAlarmOnOff()
{
	BOOL bResult = FALSE;
	BOOL bAudioAlarm = theApp.AudibleAlarm();

	CString sVal;
	sVal.Format(_T("%d"), bAudioAlarm);

	if (WritePrivateProfileString(REGKEY_OTHERSETTINGS, REGKEY_AUDIOALARM, sVal, m_sHomeDir + m_sIniFile))
		bResult = TRUE;
	else
	{
		m_eErrorCode = eErrorWritePrivateProfileStringFailed;
		WK_TRACE_ERROR(_T("ExportAudioAlarmOnOff: WritePrivateProfileString failed\n"));
	}

	return bResult;
}
//////////////////////////////////////////////////////////////////////////
BOOL CSettings::ExportAudio(CIPCAudioDVClient *pAudio)
{
	BOOL bRet = FALSE;
	try
	{
		CString sVal, sFileName = m_sHomeDir + m_sIniFile;
		int nValue;
		if (pAudio == NULL) throw eErrorNoValidEntry;

		nValue = pAudio->GetCameraNumber();
		if (nValue == -1) throw eErrorNoValidEntry;
		sVal.Format(_T("%d"), nValue);
		if (!WritePrivateProfileString(REGKEY_AUDIOSETTINGS, REGKEY_CAMERANUMBER, sVal, sFileName))
			throw eErrorWritePrivateProfileStringFailed;

		nValue = pAudio->GetInputThreshold();
		if (nValue == -1) throw eErrorNoValidEntry;
		sVal.Format(_T("%d"), nValue);
		if (!WritePrivateProfileString(REGKEY_AUDIOSETTINGS, REGKEY_INPUTTHRESHOLD, sVal, sFileName))
			throw eErrorWritePrivateProfileStringFailed;

		nValue = pAudio->GetOutputVolume();
		if (nValue == -1) throw eErrorNoValidEntry;
		sVal.Format(_T("%d"), nValue);
		if (!WritePrivateProfileString(REGKEY_AUDIOSETTINGS, REGKEY_OUTPUTVOLUME, sVal, sFileName))
			throw eErrorWritePrivateProfileStringFailed;

		nValue = pAudio->GetRecordingMode();
		if (nValue == -1) throw eErrorNoValidEntry;
		sVal.Format(_T("%d"), nValue);
		if (!WritePrivateProfileString(REGKEY_AUDIOSETTINGS, REGKEY_RECORDINGMODE, sVal, sFileName))
			throw eErrorWritePrivateProfileStringFailed;

		nValue = pAudio->GetInputSensitivity();
		sVal.Format(_T("%d"), nValue);
		if (!WritePrivateProfileString(REGKEY_AUDIOSETTINGS, REGKEY_INPUTSENSITIVITY, sVal, sFileName))
			throw eErrorWritePrivateProfileStringFailed;
		
		nValue = pAudio->IsInputOn();
		sVal.Format(_T("%d"), nValue);
		if (!WritePrivateProfileString(REGKEY_AUDIOSETTINGS, REGKEY_INPUTON, sVal, sFileName))
			throw eErrorWritePrivateProfileStringFailed;
		
		nValue = pAudio->IsOutputOn();
		sVal.Format(_T("%d"), nValue);
		if (!WritePrivateProfileString(REGKEY_AUDIOSETTINGS, REGKEY_OUTPUTON, sVal, sFileName))
			throw eErrorWritePrivateProfileStringFailed;

		bRet = TRUE;
	}
	catch (eExportErrorCode eErrorCode)
	{
		m_eErrorCode = eErrorCode;
		WK_TRACE_ERROR(_T("ExportAudio: Error %d failed\n"), m_eErrorCode);
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ExportCameraResolutionHighLow()
{
	BOOL bRet = FALSE;
	
	CServer* pServer = theApp.GetServer();
	if(pServer)
	{
		int nCurrentResolution = 0;
		BOOL bResult = pServer->GetResolution(nCurrentResolution);
		if(bResult)
		{
			CString sVal;
			sVal.Format(_T("%d"), nCurrentResolution);

			if (WritePrivateProfileString(REGKEY_OTHERSETTINGS, REGKEY_CAMERARESOLUTION, sVal, m_sHomeDir + m_sIniFile))
				bRet = TRUE;
			else
			{
				m_eErrorCode = eErrorWritePrivateProfileStringFailed;
				WK_TRACE_ERROR(_T("ExportCameraResolutionHighLow: WritePrivateProfileString failed\n"));
			}
		}
	}

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ExportXpeLicense()
{
	BOOL bResult = FALSE;
	CWK_Profile prof;
	CString sXPeLicense = prof.GetString(_T("Version"), _T("XPeLicense"), _T(""));

	if (WritePrivateProfileString(REGKEY_OTHERSETTINGS, REGKEY_XPELICENSE, sXPeLicense, m_sHomeDir + m_sIniFile))
		bResult = TRUE;
  	else
	{
		m_eErrorCode = eErrorWritePrivateProfileStringFailed;
		WK_TRACE_ERROR(_T("ExportXpeLicense: WritePrivateProfileString failed\n"));
	}

	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ExportDongleInformation()
{
	BOOL bErr = FALSE;
	CWK_Profile prof;

	CPanel* pPanel = theApp.GetPanel();
	if (pPanel)
	{
		CString sVal;

		// Ist Netzwerk vorhanden und im Dongle freigeschaltet?
		sVal.Format(_T("%d"), pPanel->IsNetAllowedByDongle());
		if (!WritePrivateProfileString(REGKEY_OTHERSETTINGS, REGKEY_DONGLENET, sVal, m_sHomeDir + m_sIniFile))
			bErr = TRUE;

		// Ist ISDN vorhanden und im Dongle freigeschaltet?
		sVal.Format(_T("%d"), pPanel->IsISDNAllowedByDongle());
		if (!WritePrivateProfileString(REGKEY_OTHERSETTINGS, REGKEY_DONGLEISDN, sVal, m_sHomeDir + m_sIniFile))
			bErr = TRUE;

		// Ist Backup aufCD, Memorystick, etc... im Dongle freigeschaltet?
		sVal.Format(_T("%d"), pPanel->IsBackUpAllowedByDongle());
		if (!WritePrivateProfileString(REGKEY_OTHERSETTINGS, REGKEY_DONGLEBACKUP, sVal, m_sHomeDir + m_sIniFile))
			bErr = TRUE;

		// Ist Backup auf DVD im Dongle freigeschaltet?
		sVal.Format(_T("%d"), pPanel->IsDVDBackupAllowedByDongle());
		if (!WritePrivateProfileString(REGKEY_OTHERSETTINGS, REGKEY_DONGLEBACKUPTODVD, sVal, m_sHomeDir + m_sIniFile))
			bErr = TRUE;
		
  		if (bErr)
		{
			m_eErrorCode = eErrorWritePrivateProfileStringFailed;
			WK_TRACE_ERROR(_T("ExportDongleInformation: WritePrivateProfileString failed\n"));
		}
	}
	else
	{
		m_eErrorCode = eErrorNoPanelObject;
		WK_TRACE_ERROR(_T("ExportDongleInformation: No panel object\n"));
		bErr = TRUE;
	}

	return !bErr;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ExportNumberOfCameras()
{
	BOOL bResult = FALSE;
	CWK_Profile prof;
	CString sNrOfCameras;
	sNrOfCameras.Format(_T("%d"), m_nNrOfCameras); 

	if (WritePrivateProfileString(REGKEY_OTHERSETTINGS, REGKEY_NROFCAMERAS, sNrOfCameras, m_sHomeDir + m_sIniFile))
		bResult = TRUE;
  	else
	{
		m_eErrorCode = eErrorWritePrivateProfileStringFailed;
		WK_TRACE_ERROR(_T("ExportNumberOfCameras: WritePrivateProfileString failed\n"));
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ExportCameraCompression(CLiveWindow* pLiveWnd)
{
	CString sSection;
	BOOL bResult = FALSE;
	int nCameraNr = pLiveWnd->GetID().GetSubID();

	sSection.Format(REGKEY_CAMERAXX, nCameraNr);

	int iCompression = 0;
	if(pLiveWnd->GetCompression(iCompression))
	{
		CString sVal;
		sVal.Format(_T("%d"), iCompression);
		if (WritePrivateProfileString(sSection, REGKEY_CAMERACOMPRESSION, sVal, m_sHomeDir + m_sIniFile))
			bResult = TRUE;
		else
		{
			m_eErrorCode = eErrorWritePrivateProfileStringFailed;
			WK_TRACE_ERROR(_T("ExportCameraCompression: WritePrivateProfileString failed\n"));
		}
	}
	else
	{
		m_eErrorCode = eErrorWritePrivateProfileStringFailed;
		WK_TRACE_ERROR(_T("ExportCameraCompression: WritePrivateProfileString failed\n"));
	}


	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ExportCameraFPS(CLiveWindow* pLiveWnd)
{
	CString sSection;
	BOOL bResult = FALSE;
	int nCameraNr = pLiveWnd->GetID().GetSubID();

	sSection.Format(REGKEY_CAMERAXX, nCameraNr);

	int iCameraFPS = 0;
	if(pLiveWnd->GetCameraFPS(iCameraFPS))
	{
		CString sVal;
		sVal.Format(_T("%d"), iCameraFPS);
		if (WritePrivateProfileString(sSection, REGKEY_CAMERAFPS, sVal, m_sHomeDir + m_sIniFile))
			bResult = TRUE;
		else
		{
			m_eErrorCode = eErrorWritePrivateProfileStringFailed;
			WK_TRACE_ERROR(_T("ExportCameraFPS: WritePrivateProfileString failed\n"));
		}
	}
	else
	{
		m_eErrorCode = eErrorWritePrivateProfileStringFailed;
		WK_TRACE_ERROR(_T("ExportCameraFPS: WritePrivateProfileString failed\n"));
	}
	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ExportCameraType(CLiveWindow* pLiveWnd)
{
	CString sSection;
	BOOL bResult = FALSE;
	int nCameraNr = pLiveWnd->GetID().GetSubID();
	sSection.Format(REGKEY_CAMERAXX, nCameraNr);

	int iPTZType= 0;
	if(pLiveWnd->GetCameraPTZType(iPTZType))
	{
		CString sVal;
		sVal.Format(_T("%d"), iPTZType);
		if (WritePrivateProfileString(sSection, REGKEY_CAMERATYPE, sVal, m_sHomeDir + m_sIniFile))
			bResult = TRUE;
		else
		{
			m_eErrorCode = eErrorWritePrivateProfileStringFailed;
			WK_TRACE_ERROR(_T("ExportCameraType: WritePrivateProfileString failed\n"));
		}
	}
	else
	{
		m_eErrorCode = eErrorWritePrivateProfileStringFailed;
		WK_TRACE_ERROR(_T("ExportCameraType: WritePrivateProfileString failed\n"));
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ExportCameraID(CLiveWindow* pLiveWnd)
{
	CString sSection;
	BOOL bResult = FALSE;
	int nCameraNr = pLiveWnd->GetID().GetSubID();
	sSection.Format(REGKEY_CAMERAXX, nCameraNr);

	DWORD dwPTZID= 0;
	if(pLiveWnd->GetCameraPTZID(dwPTZID))
	{
		CString sVal;
		sVal.Format(_T("%d"), dwPTZID);
		if (WritePrivateProfileString(sSection, REGKEY_CAMERAID, sVal, m_sHomeDir + m_sIniFile))
			bResult = TRUE;
		else
		{
			m_eErrorCode = eErrorWritePrivateProfileStringFailed;
			WK_TRACE_ERROR(_T("ExportCameraID: WritePrivateProfileString failed\n"));
		}
	}
	else
	{
		m_eErrorCode = eErrorWritePrivateProfileStringFailed;
		WK_TRACE_ERROR(_T("ExportCameraID: WritePrivateProfileString failed\n"));
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ExportCameraComport(CLiveWindow* pLiveWnd)
{
	CString sSection;
	BOOL bResult = FALSE;
	int nCameraNr = pLiveWnd->GetID().GetSubID();
	sSection.Format(REGKEY_CAMERAXX, nCameraNr);

	int iPTZComport= 0;
	if(pLiveWnd->GetCameraPTZComPort(iPTZComport))
	{
		CString sVal;
		sVal.Format(_T("%d"), iPTZComport);
		if (WritePrivateProfileString(sSection, REGKEY_CAMERACOMPORT, sVal, m_sHomeDir + m_sIniFile))
			bResult = TRUE;
		else
		{
			m_eErrorCode = eErrorWritePrivateProfileStringFailed;
			WK_TRACE_ERROR(_T("ExportCameraComport: WritePrivateProfileString failed\n"));
		}
	}
	else
	{
		m_eErrorCode = eErrorWritePrivateProfileStringFailed;
		WK_TRACE_ERROR(_T("ExportCameraComport: WritePrivateProfileString failed\n"));
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ExportRealtime()
{
	BOOL bResult = FALSE;
	BOOL bRealtime = theApp.CanRealtime();

	CString sVal;
	sVal.Format(_T("%d"), bRealtime);

	if (WritePrivateProfileString(REGKEY_OTHERSETTINGS, REGKEY_REALTIME, sVal, m_sHomeDir + m_sIniFile))
		bResult = TRUE;
	else
	{
		m_eErrorCode = eErrorWritePrivateProfileStringFailed;
		WK_TRACE_ERROR(_T("ExportRealtime: WritePrivateProfileString failed\n"));
	}

	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ImportBrightness(CLiveWindow* pLiveWnd)
{
	BOOL bResult = FALSE;
	CString sSection;
	int nCameraNr = pLiveWnd->GetID().GetSubID();

	// Einstellung soll nicht aus einem anderen System importierbar sein. Es soll allerdings kein
	// Fehler gemeldet werden.
	if (!m_bImportAllow)
		return TRUE;

	sSection.Format(REGKEY_CAMERAXX, nCameraNr);
	_TCHAR Buffer[255] = {0};
	if (GetPrivateProfileString(sSection, REGKEY_BRIGHTNESS, _T("###"), Buffer, 255, m_sHomeDir + m_sIniFile) != 0)
	{
		CString sVal(Buffer);

		if (sVal != _T("###"))
		{
			int nVal = _ttoi(sVal);
			if (pLiveWnd->SetBrightness(nVal))
				bResult = TRUE;
			else
			{
				m_eErrorCode = eErrorSetBrightnessFailed;
				WK_TRACE_ERROR(_T("SetBrightness failed\n"));
			}
		}
		else
		{
			m_eErrorCode = eErrorNoValidEntry;
			WK_TRACE_ERROR(_T("ImportBrightness: NoValidEntry\n"));
		}
	}
	else
	{
		m_eErrorCode = eErrorGetPrivateProfileStringFailed;
		WK_TRACE_ERROR(_T("ImportBrightness: GetPrivateProfileString failed\n"));
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ImportContrast(CLiveWindow* pLiveWnd)
{
	BOOL bResult = FALSE;
	CString sSection;
	int nCameraNr = pLiveWnd->GetID().GetSubID();

	// Einstellung soll nicht aus einem anderen System importierbar sein. Es soll allerdings kein
	// Fehler gemeldet werden.
	if (!m_bImportAllow)
		return TRUE;

	sSection.Format(REGKEY_CAMERAXX, nCameraNr);
	_TCHAR Buffer[255] = {0};
	if (GetPrivateProfileString(sSection, REGKEY_CONTRAST, _T("###"), Buffer, 255, m_sHomeDir + m_sIniFile) != 0)
	{
		CString sVal(Buffer);

		if (sVal != _T("###"))
		{
			int nVal = _ttoi(sVal);
			if (pLiveWnd->SetContrast(nVal))
				bResult = TRUE;
			else
			{
				m_eErrorCode = eErrorSetContrastFailed;
				WK_TRACE_ERROR(_T("SetContrast failed\n"));
			}
		}
		else
		{
			m_eErrorCode = eErrorNoValidEntry;
			WK_TRACE_ERROR(_T("ImportContrast: NoValidEntry\n"));
		}
	}	
	else
	{
		m_eErrorCode = eErrorGetPrivateProfileStringFailed;
		WK_TRACE_ERROR(_T("ImportContrast: GetPrivateProfileString failed\n"));
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ImportSaturation(CLiveWindow* pLiveWnd)
{
	BOOL bResult = FALSE;
	CString sSection;
	int nCameraNr = pLiveWnd->GetID().GetSubID();

	// Einstellung soll nicht aus einem anderen System importierbar sein. Es soll allerdings kein
	// Fehler gemeldet werden.
	if (!m_bImportAllow)
		return TRUE;

	sSection.Format(REGKEY_CAMERAXX, nCameraNr);
	_TCHAR Buffer[255] = {0};
	if (GetPrivateProfileString(sSection, REGKEY_SATURATION, _T("###"), Buffer, 255, m_sHomeDir + m_sIniFile) != 0)
	{
		CString sVal(Buffer);

		if (sVal != _T("###"))
		{
			int nVal = _ttoi(sVal);
			if (pLiveWnd->SetSaturation(nVal))
				bResult = TRUE;
			else
			{
				m_eErrorCode = eErrorSetSaturationFailed;
				WK_TRACE_ERROR(_T("SetSaturation failed\n"));
			}
		}
		else
		{
			m_eErrorCode = eErrorNoValidEntry;
			WK_TRACE_ERROR(_T("ImportSaturation: NoValidEntry\n"));
		}
	}	
	else
	{
		m_eErrorCode = eErrorGetPrivateProfileStringFailed;
		WK_TRACE_ERROR(_T("ImportSaturation: GetPrivateProfileString failed\n"));
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ImportCameraName(CLiveWindow* pLiveWnd)
{

	BOOL bResult = FALSE;
	CString sSection;

	// Einstellung soll nicht aus einem anderen System importierbar sein. Es soll allerdings kein
	// Fehler gemeldet werden.
	if (!m_bImportAllow)
	return TRUE;
	
	int nCameraNr = pLiveWnd->GetID().GetSubID();


	sSection.Format(REGKEY_CAMERAXX, nCameraNr);
	_TCHAR Buffer[255] = {0};
	if (GetPrivateProfileString(sSection, REGKEY_CAMERANAME, _T("###"), Buffer, 255, m_sHomeDir + m_sIniFile))
	{
		CString sVal(Buffer);

		if (sVal != _T("###"))
		{
			if (pLiveWnd->SetName(sVal))
				bResult = TRUE;
			else
			{
				m_eErrorCode = eErrorSetCameraNameFailed;
				WK_TRACE_ERROR(_T("SetCameraNameFailed failed\n"));
			}
		}
		else
		{
			m_eErrorCode = eErrorNoValidEntry;
			WK_TRACE_ERROR(_T("ImportCameraName: NoValidEntry\n"));
		}
	}
	else
	{
		m_eErrorCode = eErrorGetPrivateProfileStringFailed;
		WK_TRACE_ERROR(_T("ImportCameraName: GetPrivateProfileString failed\n"));
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ImportMDAlarmSensitivity(CLiveWindow* pLiveWnd)
{
	BOOL bResult = FALSE;
	CString sSection;
	int nCameraNr = pLiveWnd->GetID().GetSubID();

	sSection.Format(REGKEY_CAMERAXX, nCameraNr);
	_TCHAR Buffer[255] = {0};
	if (GetPrivateProfileString(sSection, REGKEY_MDALARMSENSITIVITY, _T("###"), Buffer, 255, m_sHomeDir + m_sIniFile) != 0)
	{
		CString sVal(Buffer);

		if (sVal != _T("###"))
		{
			int nVal = _ttoi(sVal);
			if (pLiveWnd->SetMDAlarmSensitivity(nVal))
				bResult = TRUE;
			else
			{
				m_eErrorCode = eErrorSetMDAlarmSensitivityFailed;
				WK_TRACE_ERROR(_T("SetMDAlarmSensitivity failed\n"));
			}
		}
		else
		{
			m_eErrorCode = eErrorNoValidEntry;
			WK_TRACE_ERROR(_T("ImportMDAlarmSensitivity: NoValidEntry\n"));
		}
	}	
	else
	{
		m_eErrorCode = eErrorGetPrivateProfileStringFailed;
		WK_TRACE_ERROR(_T("ImportMDAlarmSensitivity: GetPrivateProfileString failed\n"));
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ImportMDMaskSensitivity(CLiveWindow* pLiveWnd)
{
	BOOL bResult = FALSE;
	CString sSection;
	int nCameraNr = pLiveWnd->GetID().GetSubID();

	sSection.Format(REGKEY_CAMERAXX, nCameraNr);
	_TCHAR Buffer[255] = {0};
	if (GetPrivateProfileString(sSection, REGKEY_MDMASKSENSITIVITY, _T("###"), Buffer, 255, m_sHomeDir + m_sIniFile) != 0)
	{
		CString sVal(Buffer);

		if (sVal != _T("###"))
		{
			int nVal = _ttoi(sVal);
			if (pLiveWnd->SetMDMaskSensitivity(nVal))
				bResult = TRUE;
			else
			{
				m_eErrorCode = eErrorSetMDMaskSensitivityFailed;
				WK_TRACE_ERROR(_T("SetMDMaskSensitivity failed\n"));
			}
		}
		else
		{
			m_eErrorCode = eErrorNoValidEntry;
			WK_TRACE_ERROR(_T("ImportMDMaskSensitivity: NoValidEntry\n"));
		}
	}	
	else
	{
		m_eErrorCode = eErrorGetPrivateProfileStringFailed;
		WK_TRACE_ERROR(_T("ImportMDMaskSensitivity: GetPrivateProfileString failed\n"));
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ImportTimer(CLiveWindow* pLiveWnd)
{
	BOOL bErr = FALSE;
	CString sSection;
	CString sKey;
	int nCameraNr = pLiveWnd->GetID().GetSubID();

	sSection.Format(REGKEY_CAMERAXX, nCameraNr);
	_TCHAR Buffer[255] = {0};
	for (int nDayOfWeek = 0; nDayOfWeek < 7; nDayOfWeek++)
	{
		sKey.Format(REGKEY_TIMER, nDayOfWeek);
		if (GetPrivateProfileString(sSection, sKey, _T("###"), Buffer, 255, m_sHomeDir + m_sIniFile) != 0)
		{
			CString sVal(Buffer);

			if (sVal != _T("###"))
			{
				if (!pLiveWnd->SetTimer(nDayOfWeek, sVal))
				{
					m_eErrorCode = eErrorSetTimerFailed;
					WK_TRACE_ERROR(_T("SetTimer failed (DOW=%d)\n"), nDayOfWeek);
					bErr = TRUE;
				}
			}
			else
			{
				m_eErrorCode = eErrorNoValidEntry;
				WK_TRACE_ERROR(_T("ImportTimer (DOW=%d): NoValidEntry\n"), nDayOfWeek);
			}
		}		
		else
		{
			m_eErrorCode = eErrorGetPrivateProfileStringFailed;
			WK_TRACE_ERROR(_T("ImportTimer (DOW=%d): GetPrivateProfileString failed\n"), nDayOfWeek);
		}
	}

	return !bErr;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ImportRecordingMode(CLiveWindow* pLiveWnd)
{
	BOOL bResult = FALSE;
	CString sSection;
	int nCameraNr = pLiveWnd->GetID().GetSubID();

	sSection.Format(REGKEY_CAMERAXX, nCameraNr);
	_TCHAR Buffer[255] = {0};
	if (GetPrivateProfileString(sSection, REGKEY_RECORDINGMODE, _T("###"), Buffer, 255, m_sHomeDir + m_sIniFile) != 0)
	{
		CString sVal(Buffer);

		if (sVal != _T("###"))
		{
			if (sVal == _T("MD"))
			{
				if (pLiveWnd->EnableMD())
					bResult = TRUE;
				else
				{
					m_eErrorCode = eErrorEnableMDFailed;
					WK_TRACE_ERROR(_T("EnableMD failed\n"));
				}
			}
			else if (sVal == _T("UVV"))
			{
				if (pLiveWnd->EnableUVV())
					bResult = TRUE;
				else
				{
					m_eErrorCode = eErrorEnableUVVFailed;
					WK_TRACE_ERROR(_T("EnableUVV failed\n"));
				}
			}
			else
			{ 
				m_eErrorCode = eErrorUnknownRecordingmode;
				WK_TRACE_ERROR(_T("ImportRecordingMode: Unknown Recordingmode (%s)\n"), sVal);
			}
		}
		else
		{
			m_eErrorCode = eErrorNoValidEntry;
			WK_TRACE_ERROR(_T("ImportRecordingMode: NoValidEntry\n"));
		}
	}	
	else
	{
		m_eErrorCode = eErrorGetPrivateProfileStringFailed;
		WK_TRACE_ERROR(_T("ImportRecordingMode: GetPrivateProfileString failed\n"));
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ImportAlarmCallEvent(CLiveWindow* pLiveWnd)
{
	BOOL bResult = FALSE;
	CString sSection;
	int nCameraNr = pLiveWnd->GetID().GetSubID();

	sSection.Format(REGKEY_CAMERAXX, nCameraNr);
	_TCHAR Buffer[255] = {0};
	if (GetPrivateProfileString(sSection, REGKEY_ALARMCALLEVENT, _T("###"), Buffer, 255, m_sHomeDir + m_sIniFile) != 0)
	{
		CString sVal(Buffer);

		if (sVal != _T("###"))
		{
			if (pLiveWnd->SetAlarmCallEvent(_ttoi(sVal)))
				bResult = TRUE;
			else
			{
				m_eErrorCode = eErrorSetAlarmCallEventFailed;
				WK_TRACE_ERROR(_T("SetAlarmCallEvent failed\n"));
			}
		}
		else
		{
			m_eErrorCode = eErrorNoValidEntry;
			WK_TRACE_ERROR(_T("ImportAlarmCallEvent: NoValidEntry\n"));
		}
	}	
	else
	{
		m_eErrorCode = eErrorGetPrivateProfileStringFailed;
		WK_TRACE_ERROR(_T("ImportAlarmCallEvent: GetPrivateProfileString failed\n"));
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ImportSequencerDwellTime(CLiveWindow* pLiveWnd)
{
 	CString sSection;
	CString sVal;
	BOOL bResult		= FALSE;
	int nCameraNr		= pLiveWnd->GetID().GetSubID();

	sSection.Format(REGKEY_CAMERAXX, nCameraNr);
	_TCHAR Buffer[255] = {0};
	if (GetPrivateProfileString(sSection, REGKEY_SEQUENZDWELLTIME, _T("###"), Buffer, 255, m_sHomeDir + m_sIniFile) != 0)
	{
		CString sVal(Buffer);

		if (sVal != _T("###"))
		{
			if (pLiveWnd->SetSequencerDwellTime(_ttoi(sVal)))
				bResult = TRUE;
			else
			{
				m_eErrorCode = eErrorSetSequencerDwellTimeEventFailed;
				WK_TRACE_ERROR(_T("SetSequencerDwellTime failed\n"));
			}
		}
		else
		{
			m_eErrorCode = eErrorNoValidEntry;
			WK_TRACE_ERROR(_T("ImportSequencerDwellTime: NoValidEntry\n"));
		}
	}	
	else
	{
		m_eErrorCode = eErrorGetPrivateProfileStringFailed;
		WK_TRACE_ERROR(_T("ImportSequencerDwellTime: GetPrivateProfileString failed\n"));
	}

	return bResult;

}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ImportTimerOnOffState()
{
	BOOL bResult = FALSE;

	CServer* pServer = theApp.GetServer();
	if (pServer)
	{
		_TCHAR Buffer[255] = {0};
		if (GetPrivateProfileString(REGKEY_OTHERSETTINGS, REGKEY_TIMERONOFF , _T("###"), Buffer, 255, m_sHomeDir + m_sIniFile) != 0)
		{
			CString sVal(Buffer);

			if (sVal != _T("###"))
			{
				if (sVal == _T("On"))
				{
					if (pServer->SetTimerOnOffState(TRUE))
						bResult = TRUE;
					else
					{
						m_eErrorCode = eErrorSetTimerOnOffStateFailed;
						WK_TRACE_ERROR(_T("SetTimerOnOffState failed\n"));
					}
				}
				else if (sVal == _T("Off"))
				{
					if (pServer->SetTimerOnOffState(FALSE))
						bResult = TRUE;
					else
					{
						m_eErrorCode = eErrorSetTimerOnOffStateFailed;
						WK_TRACE_ERROR(_T("SetTimerOnOffState failed\n"));
					}
				}
				else
				{
					m_eErrorCode = eErrorUnknownTimerState;
					WK_TRACE_ERROR(_T("ImportTimerOnOffState: Unknown Timer On/Off-State (%s)\n"), sVal);
				}
			}
			else
			{
				m_eErrorCode = eErrorNoValidEntry;
				WK_TRACE_ERROR(_T("ImportTimerOnOffState: NoValidEntry\n"));
			}
		}	
		else
		{
			m_eErrorCode = eErrorGetPrivateProfileStringFailed;
			WK_TRACE_ERROR(_T("ImportTimerOnOffState: GetPrivateProfileString failed\n"));
		}
	}
	else
	{
		m_eErrorCode = eErrorNoServerObject;
		WK_TRACE_ERROR(_T("ImportTimerOnOffState: No Server Object\n"));
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ImportAlarmOutputOnOffState()
{
	BOOL bResult = FALSE;

	CServer* pServer = theApp.GetServer();
	if (pServer)
	{
		_TCHAR Buffer[255] = {0};
		if (GetPrivateProfileString(REGKEY_OTHERSETTINGS, REGKEY_ALARMOUTPUTONOFF , _T("###"), Buffer, 255, m_sHomeDir + m_sIniFile) != 0)
		{
			CString sVal(Buffer);

			if (sVal != _T("###"))
			{
				if (sVal == _T("On"))
				{
					if (pServer->SetAlarmOutputOnOffState(TRUE))
						bResult = TRUE;
					else
					{
						m_eErrorCode = eErrorSetAlarmOutputOnOffStateFailed;
						WK_TRACE_ERROR(_T("SetAlarmOutputOnOffState failed\n"));
					}
				}
				else if (sVal == _T("Off"))
				{
					if (pServer->SetAlarmOutputOnOffState(FALSE))
						bResult = TRUE;
					else
					{
						m_eErrorCode = eErrorSetAlarmOutputOnOffStateFailed;
						WK_TRACE_ERROR(_T("SetAlarmOutputOnOffState failed\n"));
					}
				}
				else
				{
					m_eErrorCode = eErrorUnknownAlarmOutputOnOffState;
					WK_TRACE_ERROR(_T("ImportAlarmOutputOnOffState: Unknown AlarmOutput On/Off-State (%s)\n"), sVal);
				}
			}
			else
			{
				m_eErrorCode = eErrorNoValidEntry;
				WK_TRACE_ERROR(_T("ImportAlarmOutputOnOffState: NoValidEntry\n"));
			}
		}	
		else
		{
			m_eErrorCode = eErrorGetPrivateProfileStringFailed;
			WK_TRACE_ERROR(_T("ImportAlarmOutputOnOffState: GetPrivateProfileString failed\n"));
		}
	}
	else
	{
		m_eErrorCode = eErrorNoServerObject;
		WK_TRACE_ERROR(_T("ImportAlarmOutputOnOffState: No Server Object\n"));
	}
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ImportAlarmDialingNumbers()
{
	BOOL bErr = FALSE;

	CServer* pServer = theApp.GetServer();
	if (pServer)
	{
		CString sKey;
		CString sVal;
		for (int nCallingNumber = 0; nCallingNumber < 3; nCallingNumber++)
		{
			_TCHAR Buffer[255] = {0};
			sKey.Format(REGKEY_ALARMDIALINGNUMBER, nCallingNumber);
			GetPrivateProfileString(REGKEY_OTHERSETTINGS, sKey , _T("###"), Buffer, 255, m_sHomeDir + m_sIniFile);
			CString sVal(Buffer);

			if (sVal != _T("###"))
			{
				if (!pServer->SetAlarmDialingNumber(nCallingNumber, sVal))
				{
					m_eErrorCode = eErrorSetAlarmDialingNumberFailed;
					WK_TRACE_ERROR(_T("SetAlarmDialingNumber (Nr.:%d) failed\n"), nCallingNumber);
					bErr = TRUE;
				}
			}
			else
			{
				m_eErrorCode = eErrorUnknownAlarmDialingNumber;
				WK_TRACE_ERROR(_T("ImportAlarmDialingNumbers: Unknown Alarmdialingnumber (%s)\n"), sVal);
			}
		}
	}
	else
	{
		m_eErrorCode = eErrorNoServerObject;
		WK_TRACE_ERROR(_T("ImportAlarmDialingNumbers: No Server Object\n"));
		bErr = TRUE;
	}

	return !bErr;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ImportVideoOutputMode()
{
	BOOL bErr = FALSE;

	CServer* pServer = theApp.GetServer();
	if (pServer)
	{
		CString sVal;
		CString sKey;
		CString sCSDPort;
		_TCHAR Buffer[255] = {0};

		for (int nPort = 0; nPort <= 3; nPort++)
		{
			switch (nPort)
			{
				case 0:	sCSDPort = CSD_PORT0;	break;
				case 1:	sCSDPort = CSD_PORT1;	break;
				case 2:	sCSDPort = CSD_PORT2;	break;
				case 3:	sCSDPort = CSD_PORT3;	break;
			}

			sKey.Format(REGKEY_VIDEOOUTPORT, nPort);
			if (GetPrivateProfileString(REGKEY_OTHERSETTINGS, sKey , _T("###"), Buffer, 255, m_sHomeDir + m_sIniFile) != 0)
			{
				CString sVal(Buffer);

				if (sVal != _T("###"))
				{
					enumVideoOutputMode eVideoMode = (enumVideoOutputMode)_ttoi(sVal);

					if (!pServer->SetVideoOutputMode(sCSDPort, eVideoMode))
					{
						m_eErrorCode = eErrorSetVideoOutputModeFailed;
						WK_TRACE_ERROR(_T("SetVideoOutputMode (Port.:%s VideoMode=%d) failed\n"), sCSDPort, eVideoMode);
						bErr = TRUE;
					}
				}
				else
				{
					m_eErrorCode = eErrorUnknownVideoOutputMode;
					WK_TRACE_ERROR(_T("ImportVideoOutputMode: Unknown VideoOutputMode (%s)\n"), sVal);
				}
			}
			else
			{
				m_eErrorCode = eErrorGetPrivateProfileStringFailed;
				WK_TRACE_ERROR(_T("ImportVideoOutputMode: GetPrivateProfileString failed\n"));
				bErr = TRUE;
			}
		}
	}
	else
	{
		m_eErrorCode = eErrorNoServerObject;
		WK_TRACE_ERROR(_T("ImportVideoOutputMode: No Server Object\n"));
		bErr = TRUE;
	}

	return !bErr;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ImportIPAddress()
{
	BOOL bResult = FALSE;

	// Einstellung soll nicht aus einem anderen System importierbar sein. Es soll allerdings kein
	// Fehler gemeldet werden.
	if (!m_bImportAllow)
		return TRUE;

	_TCHAR Buffer[255] = {0};
	GetPrivateProfileString(REGKEY_OTHERSETTINGS, REGKEY_LOCALIPADDRESS , _T(""), Buffer, 255, m_sHomeDir + m_sIniFile);
	CString sVal(Buffer);

	if (!sVal.IsEmpty())
	{
		CPanel* pPanel = theApp.GetPanel();
		if (pPanel)
		{
			if (pPanel->OnSetIP(sVal, IPADDRESS, FALSE))
				bResult = TRUE;
			else
			{
				m_eErrorCode = eErrorSetIPAddressFailed;
				WK_TRACE_ERROR(_T("SetIPAddressFailed (IP=%s) failed\n"), sVal);
			}
		}
	}
	else
		bResult = TRUE; // Leeren Eintrag ignorieren

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ImportSubnetmask()
{
	BOOL bResult = FALSE;

	// Einstellung soll nicht aus einem anderen System importierbar sein. Es soll allerdings kein
	// Fehler gemeldet werden.
	if (!m_bImportAllow)
		return TRUE;

	_TCHAR Buffer[255] = {0};
	GetPrivateProfileString(REGKEY_OTHERSETTINGS, REGKEY_LOCALSUBNETMASK , _T(""), Buffer, 255, m_sHomeDir + m_sIniFile);
	CString sVal(Buffer);
	if (!sVal.IsEmpty())
	{
		CPanel* pPanel = theApp.GetPanel();
		if (pPanel)
		{
			if (pPanel->OnSetIP(sVal, SUBNETMASK, FALSE))
				bResult = TRUE;
			else
			{
				m_eErrorCode = eErrorSetSubnetMaskFailed;
				WK_TRACE_ERROR(_T("SetSubnetMask (Mask=%s) failed\n"), sVal);
			}
		}
	}
	else
		bResult = TRUE; // Leeren Eintrag ignorieren

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ImportGateway()
{
	BOOL bResult = FALSE;

	_TCHAR Buffer[255] = {0};
	GetPrivateProfileString(REGKEY_OTHERSETTINGS, REGKEY_LOCALDEFAULTGATEWAY , _T(""), Buffer, 255, m_sHomeDir + m_sIniFile);
	CString sVal(Buffer);

	if (!sVal.IsEmpty())
	{
		CPanel* pPanel = theApp.GetPanel();
		if (pPanel)
		{
			if (pPanel->OnSetIP(sVal, DEFAULTGATEWAY, FALSE))
				bResult = TRUE;
			else
			{
				m_eErrorCode = eErrorSetDefaultGatewayFailed;
				WK_TRACE_ERROR(_T("SetDefaultGateway (Gateway=%s) failed\n"), sVal);
			}
		}
	}
	else
		bResult = TRUE; // Leeren Eintrag ignorieren

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ImportMSN()
{
	BOOL bResult = FALSE;

	// Einstellung soll nicht aus einem anderen System importierbar sein. Es soll allerdings kein
	// Fehler gemeldet werden.
	if (!m_bImportAllow)
		return TRUE;

	_TCHAR Buffer[255] = {0};
	GetPrivateProfileString(REGKEY_OTHERSETTINGS, REGKEY_LOCALMSN , _T(""), Buffer, 255, m_sHomeDir + m_sIniFile);
	CString sVal(Buffer);

	if (!sVal.IsEmpty())
	{
		CPanel* pPanel = theApp.GetPanel();
		if (pPanel)
		{
			if (pPanel->OnSetMSN(sVal))
				bResult = TRUE;
			else
			{
				m_eErrorCode = eErrorSetMSNFailed;
				WK_TRACE_ERROR(_T("SetMSN (MSN=%s) failed\n"), sVal);
			}
		}
	}
	else
		bResult = TRUE; // Leeren Eintrag ignorieren

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ImportLocalHostName()
{
	BOOL bResult = FALSE;

	// Einstellung soll nicht aus einem anderen System importierbar sein. Es soll allerdings kein
	// Fehler gemeldet werden.
	if (!m_bImportAllow)
		return TRUE;

	_TCHAR Buffer[255] = {0};
	GetPrivateProfileString(REGKEY_OTHERSETTINGS, REGKEY_LOCALHOSTNAME , _T(""), Buffer, 255, m_sHomeDir + m_sIniFile);
	CString sVal(Buffer);

	CPanel* pPanel = theApp.GetPanel();
	if (pPanel)
	{
		BOOL bImportSettings = TRUE;
		if (pPanel->SetLocalHostName(sVal, bImportSettings))
			bResult = TRUE;
		else
		{
			m_eErrorCode = eErrorSetLocalHostnameFailed;
			WK_TRACE_ERROR(_T("SetLocalHostName (Hostname=%s) failed\n"), sVal);
		}
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ImportTimeZoneIndex()
{
	BOOL bResult = FALSE;

	_TCHAR Buffer[255] = {0};
	if (GetPrivateProfileString(REGKEY_OTHERSETTINGS, REGKEY_TIMEZONEINDEX , _T("###"), Buffer, 255, m_sHomeDir + m_sIniFile) != 0)
	{
		CString sVal(Buffer);

		if (sVal != _T("###"))
		{
			CPanel* pPanel = theApp.GetPanel();
			if (pPanel)
			{
				pPanel->OnGetTimeZones();
				if (pPanel->OnSetTimeZone(_ttoi(sVal)))
					bResult = TRUE;
				else
				{
					m_eErrorCode = eErrorSetTimeZoneFailed;
					WK_TRACE_ERROR(_T("SetTimeZone failed\n"));
				}
			}
		}
		else
		{
			m_eErrorCode = eErrorUnknownTimeZoneIndex;
			WK_TRACE_ERROR(_T("ImportTimeZoneIndex: Unknown TimeZoneIndex (%s)\n"), sVal);
		}
	}
 	else
	{
		m_eErrorCode = eErrorGetPrivateProfileStringFailed;
		WK_TRACE_ERROR(_T("ImportTimeZoneIndex: GetPrivateProfileString failed\n"));
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ImportDefaultLanguage()
{
	BOOL bResult = FALSE;

	_TCHAR Buffer[255] = {0};
	if (GetPrivateProfileString(REGKEY_OTHERSETTINGS, REGKEY_DEFAULTLANGUAGE , _T("###"), Buffer, 255, m_sHomeDir + m_sIniFile) != 0)
	{
		CString sVal(Buffer);

		if (sVal != _T("###"))
		{
			CPanel* pPanel = theApp.GetPanel();
			if (pPanel)
			{
				if (pPanel->LoadDefaultLanguageDll(sVal))
					bResult = TRUE;
				else
				{
					m_eErrorCode = eErrorLoadDefaultLanguageDllFailed;
					WK_TRACE_ERROR(_T("LoadDefaultLanguageDll failed\n"));
				}
			}
		}
		else
		{
			m_eErrorCode = eErrorUnknownDefaultLanguage;
			WK_TRACE_ERROR(_T("ImportDefaultLanguage: Unknown DefaultLanguage (%s)\n"), sVal);
		}
	}
  	else
	{
		m_eErrorCode = eErrorGetPrivateProfileStringFailed;
		WK_TRACE_ERROR(_T("ImportDefaultLanguage: GetPrivateProfileString failed\n"));
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ImportTargetCrossOnOff()
{
	BOOL bResult = FALSE;

	_TCHAR Buffer[255] = {0};
	if (GetPrivateProfileString(REGKEY_OTHERSETTINGS, REGKEY_TARGETCROSS , _T("###"), Buffer, 255, m_sHomeDir + m_sIniFile) != 0)
	{
		CString sVal(Buffer);

		if (sVal != _T("###"))
			CDisplayWindow::SetShowTargetCross(_ttoi(sVal));
		
		bResult = TRUE;
	}
  	else
	{
		m_eErrorCode = eErrorGetPrivateProfileStringFailed;
		WK_TRACE_ERROR(_T("ImportTargetCrossOnOff: GetPrivateProfileString failed\n"));
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ImportAudioAlarmOnOff()
{
	BOOL bResult = FALSE;

	_TCHAR Buffer[255] = {0};
	if (GetPrivateProfileString(REGKEY_OTHERSETTINGS, REGKEY_AUDIOALARM , _T("###"), Buffer, 255, m_sHomeDir + m_sIniFile) != 0)
	{
		CString sVal(Buffer);

		if (sVal != _T("###"))
			theApp.SetAudibleAlarm(_ttoi(sVal));
		bResult = TRUE;
	}
   	else
	{
		m_eErrorCode = eErrorGetPrivateProfileStringFailed;
		WK_TRACE_ERROR(_T("ImportAudioAlarmOnOff: GetPrivateProfileString failed\n"));
	}

	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ImportCameraResolutionHighLow()
{
	BOOL bResult = FALSE;

	_TCHAR Buffer[255] = {0};
	if (GetPrivateProfileString(REGKEY_OTHERSETTINGS, REGKEY_CAMERARESOLUTION , _T("###"), Buffer, 255, m_sHomeDir + m_sIniFile) != 0)
	{
		CString sVal(Buffer);

		if (sVal != _T("###"))
		{
			CServer* pServer = theApp.GetServer();
			if(pServer)
			{
				bResult = pServer->SetResolution(_ttoi(sVal));
			}
		}
	}
   	else
	{
		m_eErrorCode = eErrorGetPrivateProfileStringFailed;
		WK_TRACE_ERROR(_T("ImportCameraResolutionHighLow: GetPrivateProfileString failed\n"));
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ImportCameraCompression(CLiveWindow *pLiveWnd)
{
	BOOL bResult = FALSE;
	CString sSection;

	// Einstellung soll nicht aus einem anderen System importierbar sein. Es soll allerdings kein
	// Fehler gemeldet werden.
	if (!m_bImportAllow)
	return TRUE;
	
	int nCameraNr = pLiveWnd->GetID().GetSubID();


	sSection.Format(REGKEY_CAMERAXX, nCameraNr);
	_TCHAR Buffer[255] = {0};
	if (GetPrivateProfileString(sSection, REGKEY_CAMERACOMPRESSION, _T("###"), Buffer, 255, m_sHomeDir + m_sIniFile))
	{
		CString sVal(Buffer);

		if (sVal != _T("###"))
		{
			if (pLiveWnd->SetCompression(_ttoi(sVal)))
			{
				bResult = TRUE;
			}
			else
			{
				m_eErrorCode = eErrorSetCameraCompressionFailed;
				WK_TRACE_ERROR(_T("SetCameraCompression failed\n"));
			}
		}
		else
		{
			m_eErrorCode = eErrorNoValidEntry;
			WK_TRACE_ERROR(_T("ImportCameraCompression: NoValidEntry\n"));
		}
	}
	else
	{
		m_eErrorCode = eErrorGetPrivateProfileStringFailed;
		WK_TRACE_ERROR(_T("ImportCameraCompression: GetPrivateProfileString failed\n"));
	}

	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ImportCameraFPS(CLiveWindow *pLiveWnd)
{
	BOOL bResult = FALSE;
	CString sSection;

	// Einstellung soll nicht aus einem anderen System importierbar sein. Es soll allerdings kein
	// Fehler gemeldet werden.
	if (!m_bImportAllow)
		return TRUE;

	int nCameraNr = pLiveWnd->GetID().GetSubID();


	sSection.Format(REGKEY_CAMERAXX, nCameraNr);
	_TCHAR Buffer[255] = {0};
	if (GetPrivateProfileString(sSection, REGKEY_CAMERAFPS, _T("###"), Buffer, 255, m_sHomeDir + m_sIniFile))
	{
		CString sVal(Buffer);

		if (sVal != _T("###"))
		{
			if (pLiveWnd->SetCameraFPS(_ttoi(sVal)))
			{
				bResult = TRUE;
			}
			else
			{
				m_eErrorCode = eErrorSetCameraFPSFailed;
				WK_TRACE_ERROR(_T("SetCameraFPS failed\n"));
			}

		}
		else
		{
			m_eErrorCode = eErrorNoValidEntry;
			WK_TRACE_ERROR(_T("ImportCameraFPS: NoValidEntry\n"));
		}
	}
	else
	{
		m_eErrorCode = eErrorGetPrivateProfileStringFailed;
		WK_TRACE_ERROR(_T("ImportCameraFPS: GetPrivateProfileString failed\n"));
	}

	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ImportCameraType(CLiveWindow *pLiveWnd)
{
	BOOL bResult = FALSE;
	CString sSection;

	// Einstellung soll nicht aus einem anderen System importierbar sein. Es soll allerdings kein
	// Fehler gemeldet werden.
	if (!m_bImportAllow)
	return TRUE;
	
	int nCameraNr = pLiveWnd->GetID().GetSubID();

	sSection.Format(REGKEY_CAMERAXX, nCameraNr);
	_TCHAR Buffer[255] = {0};
	if (GetPrivateProfileString(sSection, REGKEY_CAMERATYPE, _T("###"), Buffer, 255, m_sHomeDir + m_sIniFile))
	{
		CString sVal(Buffer);

		if (sVal != _T("###"))
		{
			if (pLiveWnd->SetCameraPTZType(_ttoi(sVal)))
			{
				bResult = TRUE;
			}
			else
			{
				m_eErrorCode = eErrorSetCameraTypeFailed;
				WK_TRACE_ERROR(_T("SetCameraPTZType failed\n"));
			}
		}
		else
		{
			m_eErrorCode = eErrorNoValidEntry;
			WK_TRACE_ERROR(_T("ImportCameraPTZType: NoValidEntry\n"));
		}
	}
	else
	{
		m_eErrorCode = eErrorGetPrivateProfileStringFailed;
		WK_TRACE_ERROR(_T("ImportCameraPTZType: GetPrivateProfileString failed\n"));
	}

	return bResult;
}		

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ImportCameraID(CLiveWindow *pLiveWnd)
{
	BOOL bResult = FALSE;
	CString sSection;

	// Einstellung soll nicht aus einem anderen System importierbar sein. Es soll allerdings kein
	// Fehler gemeldet werden.
	if (!m_bImportAllow)
	return TRUE;
	
	int nCameraNr = pLiveWnd->GetID().GetSubID();


	sSection.Format(REGKEY_CAMERAXX, nCameraNr);
	_TCHAR Buffer[255] = {0};
	if (GetPrivateProfileString(sSection, REGKEY_CAMERAID, _T("###"), Buffer, 255, m_sHomeDir + m_sIniFile))
	{
		CString sVal(Buffer);

		if (sVal != _T("###"))
		{
			if (pLiveWnd->SetCameraPTZID(_ttoi(sVal)))
			{
				bResult = TRUE;
			}
			else
			{
				m_eErrorCode = eErrorSetCameraIDFailed;
				WK_TRACE_ERROR(_T("SetCameraPTZID failed\n"));
			}
		}
		else
		{
			m_eErrorCode = eErrorNoValidEntry;
			WK_TRACE_ERROR(_T("ImportCameraPTZID: NoValidEntry\n"));
		}
	}
	else
	{
		m_eErrorCode = eErrorGetPrivateProfileStringFailed;
		WK_TRACE_ERROR(_T("ImportCameraPTZID: GetPrivateProfileString failed\n"));
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ImportCameraComport(CLiveWindow *pLiveWnd)
{
	BOOL bResult = FALSE;
	CString sSection;

	// Einstellung soll nicht aus einem anderen System importierbar sein. Es soll allerdings kein
	// Fehler gemeldet werden.
	if (!m_bImportAllow)
	return TRUE;
	
	int nCameraNr = pLiveWnd->GetID().GetSubID();


	sSection.Format(REGKEY_CAMERAXX, nCameraNr);
	_TCHAR Buffer[255] = {0};
	if (GetPrivateProfileString(sSection, REGKEY_CAMERACOMPORT, _T("###"), Buffer, 255, m_sHomeDir + m_sIniFile))
	{
		CString sVal(Buffer);

		if (sVal != _T("###"))
		{
			if (pLiveWnd->SetCameraPTZComPort(_ttoi(sVal)))
			{
				bResult = TRUE;
			}
			else
			{
				m_eErrorCode = eErrorSetCameraComportFailed;
				WK_TRACE_ERROR(_T("SetCameraPTZComport failed\n"));
			}
		}
		else
		{
			m_eErrorCode = eErrorNoValidEntry;
			WK_TRACE_ERROR(_T("ImportCameraPTZComport: NoValidEntry\n"));
		}
	}
	else
	{
		m_eErrorCode = eErrorGetPrivateProfileStringFailed;
		WK_TRACE_ERROR(_T("ImportCameraPTZComport: GetPrivateProfileString failed\n"));
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ImportRealtime()
{
	BOOL bResult = FALSE;

	_TCHAR Buffer[255] = {0};
	if (GetPrivateProfileString(REGKEY_OTHERSETTINGS, REGKEY_REALTIME , _T("###"), Buffer, 255, m_sHomeDir + m_sIniFile) != 0)
	{
		CString sVal(Buffer);

		if (sVal != _T("###"))
		{
			bResult = theApp.SetRealtime(_ttoi(sVal));
		}
	}
   	else
	{
		m_eErrorCode = eErrorGetPrivateProfileStringFailed;
		WK_TRACE_ERROR(_T("ImportRealtime: GetPrivateProfileString failed\n"));
	}

	return bResult;
}
//////////////////////////////////////////////////////////////////////////
BOOL CSettings::ImportAudio(CIPCAudioDVClient*pAudio)
{
	BOOL bResult = FALSE;
	
	try
	{
		if (pAudio == NULL) throw eErrorNoValidEntry;
		int nCameraNumber     = GetPrivateProfileInt(REGKEY_AUDIOSETTINGS, REGKEY_CAMERANUMBER); 
		int nInputSensitivity = GetPrivateProfileInt(REGKEY_AUDIOSETTINGS, REGKEY_INPUTSENSITIVITY); 
		int nInputThreshold   = GetPrivateProfileInt(REGKEY_AUDIOSETTINGS, REGKEY_INPUTTHRESHOLD); 
		int nOutputVolume     = GetPrivateProfileInt(REGKEY_AUDIOSETTINGS, REGKEY_OUTPUTVOLUME); 
		int nRecordingMode    = GetPrivateProfileInt(REGKEY_AUDIOSETTINGS, REGKEY_RECORDINGMODE); 
		int nInputOn          = GetPrivateProfileInt(REGKEY_AUDIOSETTINGS, REGKEY_INPUTON); 
		int nOutputOn         = GetPrivateProfileInt(REGKEY_AUDIOSETTINGS, REGKEY_OUTPUTON); 

		BOOL bRecording   = nCameraNumber > 0;
		BOOL bDetectLevel = FALSE;
		switch (nRecordingMode)
		{
			case AUDIO_RECORDING_L:		// alle Modi, die Schwellwerterkennung auswerten
			case AUDIO_RECORDING_LE:
			case AUDIO_RECORDING_AL:
			case AUDIO_RECORDING_ALE:
				bDetectLevel = TRUE;
				break;
		}
		
		pAudio->SwitchInput(nInputOn ? CSD_ON : CSD_OFF);
		pAudio->SwitchOutput(nOutputOn ? CSD_ON : CSD_OFF);
		if (bRecording)
		{
			pAudio->SetRecordingMode(nRecordingMode);
		}

		pAudio->SetCameraNumber(nCameraNumber);

		if (bRecording || nInputOn)
		{
			pAudio->SetInputSensitivity(nInputSensitivity);
		}
		
		if (bRecording && bDetectLevel)
		{
			pAudio->SetInputThreshold(nInputThreshold);
		}
		
		pAudio->SetOutputVolume(nOutputVolume);
		bResult = TRUE;
	}
	catch (eExportErrorCode eErrorCode)
	{
		m_eErrorCode = eErrorCode;
		WK_TRACE_ERROR(_T("ImportAudio: Error %d failed\n"), m_eErrorCode);
	}

	return bResult;

}
int CSettings::GetPrivateProfileInt(LPCTSTR szAppName, LPCTSTR szKeyName)
{
	_TCHAR Buffer[32] = {0};
	if (GetPrivateProfileString(szAppName, szKeyName, _T("###"), Buffer, 32, m_sHomeDir + m_sIniFile) != 0)
	{
		CString sVal(Buffer);

		if (sVal == _T("###"))
		{
			throw eErrorNoValidEntry;
		}
		else
		{
			return _ttoi(sVal);
		}
	}
   	else
	{
		throw eErrorGetPrivateProfileStringFailed;
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::CheckComputerName()
{
	BOOL bResult = FALSE;
	m_bImportAllow = FALSE;
	
	_TCHAR Buffer[255] = {0};
	if (GetPrivateProfileString(REGKEY_OTHERSETTINGS, REGKEY_COMPUTERNAME , _T("###"), Buffer, 255, m_sHomeDir + m_sIniFile) != 0)
	{
		CString sVal(Buffer);

		if (sVal != _T("###"))
		{
			if (sVal == theApp.GetComputerName())
				bResult = TRUE;
		}
		else
		{
			m_eErrorCode = eErrorNoValidEntry;
			WK_TRACE_ERROR(_T("CheckComputerName: NoValidEntry\n"));
		}
	}
	else
	{
		m_eErrorCode = eErrorGetPrivateProfileStringFailed;
		WK_TRACE_ERROR(_T("CheckComputerName: GetPrivateProfileString failed\n"));
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::CheckOem()
{
	BOOL bResult = FALSE;
	m_bImportAllow = FALSE;
	
	_TCHAR Buffer[255] = {0};
	if (GetPrivateProfileString(REGKEY_OTHERSETTINGS, REGKEY_OEM , _T("###"), Buffer, 255, m_sHomeDir + m_sIniFile) != 0)
	{
		CString sVal(Buffer);

		if (sVal == m_sOem)
			bResult = TRUE;
	}
 	else
	{
		m_eErrorCode = eErrorGetPrivateProfileStringFailed;
		WK_TRACE_ERROR(_T("CheckOem: GetPrivateProfileString failed\n"));
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::CheckBuildNr()
{
	BOOL bResult = FALSE;
	m_bImportAllow = FALSE;
	
	_TCHAR Buffer[255] = {0};
	if (GetPrivateProfileString(REGKEY_OTHERSETTINGS, REGKEY_BUILDNUMBER , _T("###"), Buffer, 255, m_sHomeDir + m_sIniFile) != 0)
	{
		CString sVal(Buffer);

		if (sVal == m_sBuildNr)
			bResult = TRUE;
	}
 	else
	{
		m_eErrorCode = eErrorGetPrivateProfileStringFailed;
		WK_TRACE_ERROR(_T("CheckBuildNr: GetPrivateProfileString failed\n"));
	}
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ImportNumberOfCameras(int& nNrOfCameras)
{
	BOOL bResult = FALSE;
	
	_TCHAR Buffer[255] = {0};
	if (GetPrivateProfileString(REGKEY_OTHERSETTINGS, REGKEY_NROFCAMERAS , _T("###"), Buffer, 255, m_sHomeDir + m_sIniFile) != 0)
	{
		CString sVal(Buffer);
		if (sVal != _T("###"))
		{
			nNrOfCameras = _ttoi(sVal);
			bResult = TRUE;
		}
		else
		{
			m_eErrorCode = eErrorNoValidEntry;
			WK_TRACE_ERROR(_T("ImportNumberOfCameras: NoValidEntry\n"));
		}
	}
 	else
	{
		m_eErrorCode = eErrorGetPrivateProfileStringFailed;
		WK_TRACE_ERROR(_T("ImportNumberOfCameras: GetPrivateProfileString failed\n"));
	}
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::CopyIniFileToMedium()
{
	BOOL bResult	= FALSE;
	BYTE* pBuffer	= NULL;

	TRY
	{
		CFile fileSrc(m_sHomeDir + m_sIniFile, CFile::modeRead);
		CFile fileDst(m_sExportDir + m_sIniFile, CFile::modeCreate | CFile::modeWrite);
		
		DWORD dwLen = (DWORD)fileSrc.GetLength();
		if (dwLen > 0)
		{
			pBuffer = new BYTE[dwLen];
			if (fileSrc.Read(pBuffer, dwLen) == dwLen)
			{
				if (Encode(pBuffer, dwLen))
				{
					fileDst.Write(pBuffer, dwLen);
					fileSrc.Close();
					CFile::Remove(m_sHomeDir + m_sIniFile);
					bResult = TRUE;
				}
			}
		}
	}
	CATCH(CFileException, pEx)
	{
		m_eErrorCode = eErrorCopyIniFileToMediumFailed;
	}
	END_CATCH

	WK_DELETE(pBuffer);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::GetIniFileFromMedium()
{
	BOOL bResult	= FALSE;
	BYTE* pBuffer	= NULL;

	TRY
	{
		CFile fileSrc(m_sExportDir + m_sIniFile, CFile::modeRead);
		CFile fileDst(m_sHomeDir + m_sIniFile, CFile::modeCreate | CFile::modeWrite);
		
		DWORD dwLen = (DWORD)fileSrc.GetLength();
		if (dwLen > 0)
		{
			pBuffer = new BYTE[dwLen];
			if (fileSrc.Read(pBuffer, dwLen) == dwLen)
			{
				if (Decode(pBuffer, dwLen))
				{
					fileDst.Write(pBuffer, dwLen);
					bResult = TRUE;
				}
			}
		}
	}
	CATCH(CFileException, pEx)
	{
		m_eErrorCode = eErrorGetIniFileFromMediumFailed;

	}
	END_CATCH

	WK_DELETE(pBuffer);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::Encode(BYTE* pBuffer, DWORD dwLen)
{
	BOOL bResult = FALSE;

	// Soll verschlüsselt werden?
	if (!m_bDecode)
		return TRUE;

	if ((pBuffer != NULL) && (dwLen > 0))
	{
		for (DWORD dwI = 0; dwI < dwLen; dwI++)
			pBuffer[dwI] =(BYTE)~pBuffer[dwI];
		bResult = TRUE;
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::Decode(BYTE* pBuffer, DWORD dwLen)
{
	BOOL bResult = FALSE;

	// Soll entschlüsselt werden?
	if (!m_bDecode)
		return TRUE;

	if ((pBuffer != NULL) && (dwLen > 0))
	{
		for (DWORD dwI = 0; dwI < dwLen; dwI++)
			pBuffer[dwI] =(BYTE)~pBuffer[dwI];
		bResult = TRUE;
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::RemoveIniFile(const CString& sFileName)
{
	BOOL bResult = FALSE;

	TRY
	{
		CFile::Remove(sFileName);
		bResult = TRUE;
	}
	CATCH(CFileException, pEx)
	{
	}
	END_CATCH

	return bResult;
}

/////////////////////////////////////////////////////////////
// Legt ein Livefenster an, um auch auf die Kamera-Einstellungen zugreifen zu können
// die nicht angeschlossen sind, und somot folglich kein Fenster besitzen.
CLiveWindow*  CSettings::CreateTempLiveWindow(WORD wCamNr)
{
	CLiveWindow* pLW = NULL;
	CServer* pServer = theApp.GetServer();
	if (pServer && m_pMainWnd)
	{
		CIPCOutputDVClient* pOutput = pServer->GetOutput();
		if (pOutput)
		{
			CSecID id(SECID_GROUP_OUTPUT, wCamNr);
			CIPCOutputRecord rec = pOutput->GetOutputRecordFromSecID(id);
			pLW = new CLiveWindow(m_pMainWnd, pServer, rec);
			if (pLW)
			{
				if (!pLW->Create(CRect(0,0,0,0), m_pMainWnd))
					WK_DELETE(pLW);
			}
		}
	}

	return pLW;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ExportMaintenance()
{
	BOOL bErr = FALSE;
	CWK_Profile wkp;


	CString sVal;
	BOOL	bVal;


	sVal = wkp.GetString(SECTION_MAINTENANCE, MAINT_INST_COMPANY, _T(""));
	if (!WritePrivateProfileString(REGKEY_OTHERSETTINGS, MAINT_INST_COMPANY, sVal, m_sHomeDir + m_sIniFile))
		bErr = TRUE;

	sVal = wkp.GetString(SECTION_MAINTENANCE, MAINT_INST_STREET, _T(""));
	if (!bErr && !WritePrivateProfileString(REGKEY_OTHERSETTINGS, MAINT_INST_STREET, sVal, m_sHomeDir + m_sIniFile))
		bErr = TRUE;

	sVal = wkp.GetString(SECTION_MAINTENANCE, MAINT_INST_POSTCODE, _T(""));
	if (!bErr && !WritePrivateProfileString(REGKEY_OTHERSETTINGS, MAINT_INST_POSTCODE, sVal, m_sHomeDir + m_sIniFile))
		bErr = TRUE;

	sVal = wkp.GetString(SECTION_MAINTENANCE, MAINT_INST_CITY, _T(""));
	if (!bErr && !WritePrivateProfileString(REGKEY_OTHERSETTINGS, MAINT_INST_CITY, sVal, m_sHomeDir + m_sIniFile))
		bErr = TRUE;

	sVal = wkp.GetString(SECTION_MAINTENANCE, MAINT_INST_TELEFON, _T(""));
	if (!bErr && !WritePrivateProfileString(REGKEY_OTHERSETTINGS, MAINT_INST_TELEFON, sVal, m_sHomeDir + m_sIniFile))
		bErr = TRUE;

	sVal = wkp.GetString(SECTION_MAINTENANCE, MAINT_INST_EMAIL, _T(""));
	if (!bErr && !WritePrivateProfileString(REGKEY_OTHERSETTINGS, MAINT_INST_EMAIL, sVal, m_sHomeDir + m_sIniFile))
		bErr = TRUE;

	sVal = wkp.GetString(SECTION_MAINTENANCE, MAINT_INST_COUNTRY, _T(""));
	if (!bErr && !WritePrivateProfileString(REGKEY_OTHERSETTINGS, MAINT_INST_COUNTRY, sVal, m_sHomeDir + m_sIniFile))
		bErr = TRUE;

	bVal = wkp.GetInt(SECTION_MAINTENANCE, MAINT_INTERVALL, FALSE);
	sVal.Format(_T("%d"), bVal);
	if (!bErr && !WritePrivateProfileString(REGKEY_OTHERSETTINGS, MAINT_INTERVALL, sVal, m_sHomeDir + m_sIniFile))
		bErr = TRUE;

	sVal = wkp.GetString(SECTION_MAINTENANCE, MAINT_INTERVALL_STARTDATE, _T(""));
	if (!bErr && !WritePrivateProfileString(REGKEY_OTHERSETTINGS, MAINT_INTERVALL_STARTDATE, sVal, m_sHomeDir + m_sIniFile))
		bErr = TRUE;

	bVal = wkp.GetInt(SECTION_MAINTENANCE, MAINT_INTERVALL_ONOFF, FALSE);
	sVal.Format(_T("%d"), bVal);
	if (!WritePrivateProfileString(REGKEY_OTHERSETTINGS, MAINT_INTERVALL_ONOFF, sVal, m_sHomeDir + m_sIniFile))
		bErr = TRUE;

	if (bErr)
	{
		m_eErrorCode = eErrorWritePrivateProfileStringFailed;
		WK_TRACE_ERROR(_T("ExportMaintenance: WritePrivateProfileString < %s > failed\n"), bVal);
	}

	return !bErr;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::ImportMaintenance()
{
	BOOL bErr = FALSE;

	CWK_Profile wkp;
	_TCHAR Buffer[255] = {0};
	CString sVal;
	BOOL	bVal = 0;

	GetPrivateProfileString(REGKEY_OTHERSETTINGS, MAINT_INST_COMPANY , _T(""), Buffer, 255, m_sHomeDir + m_sIniFile);
	sVal = Buffer;
	if(!wkp.WriteString(SECTION_MAINTENANCE, MAINT_INST_COMPANY, sVal))
		bErr = TRUE;

	GetPrivateProfileString(REGKEY_OTHERSETTINGS, MAINT_INST_STREET , _T(""), Buffer, 255, m_sHomeDir + m_sIniFile);
	sVal = Buffer;
	if(!bErr && !wkp.WriteString(SECTION_MAINTENANCE, MAINT_INST_STREET, sVal))
		bErr = TRUE;

	GetPrivateProfileString(REGKEY_OTHERSETTINGS, MAINT_INST_POSTCODE , _T(""), Buffer, 255, m_sHomeDir + m_sIniFile);
	sVal = Buffer;
	if(!bErr && !wkp.WriteString(SECTION_MAINTENANCE, MAINT_INST_POSTCODE, sVal))
		bErr = TRUE;

	GetPrivateProfileString(REGKEY_OTHERSETTINGS, MAINT_INST_CITY , _T(""), Buffer, 255, m_sHomeDir + m_sIniFile);
	sVal = Buffer;
	if(!bErr && !wkp.WriteString(SECTION_MAINTENANCE, MAINT_INST_CITY, sVal))
		bErr = TRUE;

	GetPrivateProfileString(REGKEY_OTHERSETTINGS, MAINT_INST_TELEFON , _T(""), Buffer, 255, m_sHomeDir + m_sIniFile);
	sVal = Buffer;
	if(!bErr && !wkp.WriteString(SECTION_MAINTENANCE, MAINT_INST_TELEFON, sVal))
		bErr = TRUE;

	GetPrivateProfileString(REGKEY_OTHERSETTINGS, MAINT_INST_EMAIL , _T(""), Buffer, 255, m_sHomeDir + m_sIniFile);
	sVal = Buffer;
	if(!bErr && !wkp.WriteString(SECTION_MAINTENANCE, MAINT_INST_EMAIL, sVal))
		bErr = TRUE;

	GetPrivateProfileString(REGKEY_OTHERSETTINGS, MAINT_INST_COUNTRY , _T(""), Buffer, 255, m_sHomeDir + m_sIniFile);
	sVal = Buffer;
	if(!bErr && !wkp.WriteString(SECTION_MAINTENANCE, MAINT_INST_COUNTRY, sVal))
		bErr = TRUE;

	GetPrivateProfileString(REGKEY_OTHERSETTINGS, MAINT_INTERVALL , _T(""), Buffer, 255, m_sHomeDir + m_sIniFile);
	sVal = Buffer;
	bVal = _wtoi(sVal);
	if(!bErr && !wkp.WriteInt(SECTION_MAINTENANCE, MAINT_INTERVALL, bVal))
		bErr = TRUE;

	GetPrivateProfileString(REGKEY_OTHERSETTINGS, MAINT_INTERVALL_ONOFF , _T(""), Buffer, 255, m_sHomeDir + m_sIniFile);
	sVal = Buffer;
	bVal = _wtoi(sVal);
	if(!bErr && !wkp.WriteInt(SECTION_MAINTENANCE, MAINT_INTERVALL_ONOFF, bVal))
		bErr = TRUE;

	GetPrivateProfileString(REGKEY_OTHERSETTINGS, MAINT_INTERVALL_STARTDATE , _T(""), Buffer, 255, m_sHomeDir + m_sIniFile);
	sVal = Buffer;
	if(!bErr && !wkp.WriteString(SECTION_MAINTENANCE, MAINT_INTERVALL_STARTDATE, sVal))
		bErr = TRUE;

	if(bErr)
	{
		m_eErrorCode = eErrorSetMaintenanceData;
		WK_TRACE_ERROR(_T("Import Maintenance: Write < %s > to Registry failed\n"), sVal);
	}

	return !bErr;
}