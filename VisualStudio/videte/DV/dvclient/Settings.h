// Settings.h: interface for the CSettings class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SETTINGS_H__7A3A76D2_EE90_4E4A_805C_B19DAF7D0C5F__INCLUDED_)
#define AFX_SETTINGS_H__7A3A76D2_EE90_4E4A_805C_B19DAF7D0C5F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


typedef enum {
		eStatusOk,
		eErrorGetBrightnessFailed,
		eErrorGetContrastFailed,
		eErrorGetSaturationFailed,
		eErrorGetSequencerDwellTimeFailed,
		eErrorGetTimerOnOffStateFailed,
		eErrorGetAlarmOutputOnOffStateFailed,
		eErrorGetAlarmDialingNumberFailed,
		eErrorGetVideoOutputModeFailed,
		eErrorGetIPAddressFailed,
		eErrorGetSubnetMaskAddressFailed,
		eErrorGetGatewayAddressFailed,
		eErrorGetTimeZoneFailed,
		eErrorGetCurrentLanguageFailed,
		eErrorSetBrightnessFailed,
		eErrorSetContrastFailed,
		eErrorSetSaturationFailed,
		eErrorSetCameraNameFailed,
		eErrorSetCameraCompressionFailed,
		eErrorSetCameraFPSFailed,
		eErrorSetCameraTypeFailed,
		eErrorSetCameraIDFailed,
		eErrorSetCameraComportFailed,
		eErrorSetMDAlarmSensitivityFailed,
		eErrorSetMDMaskSensitivityFailed,
		eErrorSetTimerFailed,
		eErrorUnknownRecordingmode,
		eErrorUnknownTimerState,
		eErrorUnknownAlarmOutputOnOffState,
		eErrorUnknownAlarmDialingNumber,
		eErrorUnknownVideoOutputMode,
		eErrorUnknownTimeZoneIndex,
		eErrorUnknownDefaultLanguage,
		eErrorEnableMDFailed,
		eErrorEnableUVVFailed,
		eErrorSetAlarmCallEventFailed,
		eErrorSetSequencerDwellTimeEventFailed,
		eErrorSetTimerOnOffStateFailed,
		eErrorSetAlarmOutputOnOffStateFailed,
		eErrorSetAlarmDialingNumberFailed,
		eErrorSetVideoOutputModeFailed,
		eErrorSetIPAddressFailed,
		eErrorSetSubnetMaskFailed,
		eErrorSetDefaultGatewayFailed,
		eErrorSetMSNFailed,
		eErrorSetLocalHostnameFailed,
		eErrorSetTimeZoneFailed,
		eErrorLoadDefaultLanguageDllFailed,
		eErrorWritePrivateProfileStringFailed,
		eErrorGetPrivateProfileStringFailed,
		eErrorNoServerObject,
		eErrorNoValidEntry,
		eErrorCopyIniFileToMediumFailed,
		eErrorGetIniFileFromMediumFailed,
		eErrorNoPanelObject,
		eErrorSetMaintenanceData
}eExportErrorCode;

class CLiveWindow;
class CMainFrame;
class CSettings  
{
public:
	CSettings(const CString& sExportPath);
	virtual ~CSettings();

	BOOL Export();
	BOOL Import();
	BOOL GetErrorCode(){return m_eErrorCode;}

protected:
	BOOL CheckComputerName();
	BOOL CheckBuildNr();
	BOOL CheckOem();

	BOOL CopyIniFileToMedium();
	BOOL GetIniFileFromMedium();
	BOOL Encode(BYTE* pBuffer, DWORD dwLen);
	BOOL Decode(BYTE* pBuffer, DWORD dwLen);
	BOOL RemoveIniFile(const CString& sFileName);
	CLiveWindow*  CreateTempLiveWindow(WORD wCamNr);

	BOOL ExportBrightness(CLiveWindow* pLiveWnd);
	BOOL ExportContrast(CLiveWindow* pLiveWnd);
	BOOL ExportSaturation(CLiveWindow* pLiveWnd);
	BOOL ExportCameraName(CLiveWindow* pLiveWnd);
	BOOL ExportMDAlarmSensitivity(CLiveWindow* pLiveWnd);
	BOOL ExportMDMaskSensitivity(CLiveWindow* pLiveWnd);
	BOOL ExportTimer(CLiveWindow* pLiveWnd);
	BOOL ExportRecordingMode(CLiveWindow* pLiveWnd);
	BOOL ExportAlarmCallEvent(CLiveWindow* pLiveWnd);
	BOOL ExportSequencerDwellTime(CLiveWindow* pLiveWnd);
	BOOL ExportTimerOnOffState();
	BOOL ExportAlarmDialingNumbers();
	BOOL ExportAlarmOutputOnOffState();
	BOOL ExportVideoOutputMode();
	BOOL ExportIPAddress();
	BOOL ExportSubnetmask();
	BOOL ExportGateway();
	BOOL ExportMSN();
	BOOL ExportLocalHostName();
	BOOL ExportTimeZoneIndex();
	BOOL ExportDefaultLanguage();
	BOOL ExportTargetCrossOnOff();
 	BOOL ExportAudioAlarmOnOff();
 	BOOL ExportCameraResolutionHighLow();
	BOOL ExportComputerName();
	BOOL ExportOem();
	BOOL ExportBuildNr();
	BOOL ExportXpeLicense();
	BOOL ExportDongleInformation();
	BOOL ExportNumberOfCameras();
	
	//ab Version 2.5.0.612
	BOOL ExportCameraCompression(CLiveWindow* pLiveWnd);
	BOOL ExportCameraType(CLiveWindow* pLiveWnd);
	BOOL ExportCameraID(CLiveWindow* pLiveWnd);
	BOOL ExportCameraComport(CLiveWindow* pLiveWnd);
	BOOL ExportRealtime();
	//ab Version 2.5....
	BOOL ExportAudio(CIPCAudioDVClient*);

	//ab Version 3.0.0.809
	BOOL ExportCameraFPS(CLiveWindow* pLiveWnd);

	//ab Version 3.1.0.840
	BOOL ExportMaintenance();

	BOOL ImportBrightness(CLiveWindow* pLiveWnd);
	BOOL ImportContrast(CLiveWindow* pLiveWnd);
	BOOL ImportSaturation(CLiveWindow* pLiveWnd);
	BOOL ImportCameraName(CLiveWindow* pLiveWnd);
	BOOL ImportMDAlarmSensitivity(CLiveWindow* pLiveWnd);
	BOOL ImportMDMaskSensitivity(CLiveWindow* pLiveWnd);
	BOOL ImportTimer(CLiveWindow* pLiveWnd);
	BOOL ImportRecordingMode(CLiveWindow* pLiveWnd);
	BOOL ImportSequencerDwellTime(CLiveWindow* pLiveWnd);	
	BOOL ImportTimerOnOffState();
	BOOL ImportAlarmDialingNumbers();
	BOOL ImportAlarmOutputOnOffState();
	BOOL ImportAlarmCallEvent(CLiveWindow* pLiveWnd);
	BOOL ImportVideoOutputMode();
	BOOL ImportIPAddress();
	BOOL ImportSubnetmask();
	BOOL ImportGateway();
	BOOL ImportMSN();
	BOOL ImportLocalHostName();
	BOOL ImportTimeZoneIndex();
	BOOL ImportDefaultLanguage();
	BOOL ImportTargetCrossOnOff();
 	BOOL ImportAudioAlarmOnOff();
	BOOL ImportCameraResolutionHighLow();
	BOOL ImportNumberOfCameras(int& nNrOfCameras);

	//ab Version 2.5.0.612
	BOOL ImportCameraCompression(CLiveWindow* pLiveWnd);
	BOOL ImportCameraType(CLiveWindow* pLiveWnd);
	BOOL ImportCameraID(CLiveWindow* pLiveWnd);
	BOOL ImportCameraComport(CLiveWindow* pLiveWnd);
	BOOL ImportRealtime();

	//ab Version 3.0.0.809
	BOOL ImportCameraFPS(CLiveWindow* pLiveWnd);

	//ab Version 3.1.0.840
	BOOL ImportMaintenance();

	BOOL ImportAudio(CIPCAudioDVClient*);
private:
	int GetPrivateProfileInt(LPCTSTR szAppName, LPCTSTR szKeyName);

private:
	CString				m_sIniFile;
	CString				m_sExportDir;
	CString				m_sHomeDir;
	CString				m_sOem;
	CString				m_sBuildNr;

	CMainFrame*			m_pMainWnd;
	int					m_nNrOfCameras;
	BOOL				m_bImportAllow;
	BOOL				m_bDecode;
	eExportErrorCode	m_eErrorCode;
};

#endif // !defined(AFX_SETTINGS_H__7A3A76D2_EE90_4E4A_805C_B19DAF7D0C5F__INCLUDED_)
