// DVClient.h : main header file for the DVCLIENT application
//

#if !defined(AFX_DVCLIENT_H__6B5BBEC5_9757_11D3_A870_004005A19028__INCLUDED_)
#define AFX_DVCLIENT_H__6B5BBEC5_9757_11D3_A870_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "..\LangDll\resource.h"

class CServer;
class CMainFrame;
class CPanel;
class CWK_Dongle;

#include "DVUIThread.h"
#include "RequestThread.h"
/////////////////////////////////////////////////////////////////////////////
#ifdef _UNICODE
static _TCHAR BASED_CODE szLangPrefix[] = _T("DVU");
#else
static _TCHAR BASED_CODE szLangPrefix[] = _T("DV");
#endif
static _TCHAR BASED_CODE szLangSuffix[] = _T(".dll");
static _TCHAR BASED_CODE szLangDebug[]  = _T("Debug");
////////////////////////////////////////////////////////////////////////////
#define WPARAM_ADD_INPUT		0x0001
#define WPARAM_UPD_INPUT		0x0002
#define WPARAM_DEL_INPUT		0x0003
#define WPARAM_IND_ALARM		0x0004
////////////////////////////////////////////////////////////////////////////
#define WPARAM_ADD_CAMERA		0x0101
#define WPARAM_UPD_CAMERA		0x0102
#define WPARAM_DEL_CAMERA		0x0103
#define WPARAM_CAMERA_CT		0x0104
#define WPARAM_CAMERA_CF		0x0105
#define WPARAM_CAMERA_CF_EX		0x0106
////////////////////////////////////////////////////////////////////////////
#define WPARAM_ADD_RELAIS		0x0201
#define WPARAM_UPD_RELAIS		0x0202
#define WPARAM_DEL_RELAIS		0x0203
////////////////////////////////////////////////////////////////////////////
#define WPARAM_ADD_ARCHIV		0x0301
#define WPARAM_UPD_ARCHIV		0x0302
#define WPARAM_DEL_ARCHIV		0x0303
#define WPARAM_ADD_ARCHIVS		0x0304
////////////////////////////////////////////////////////////////////////////
#define WPARAM_ADD_SEQUENCE		0x0401
#define WPARAM_UPD_SEQUENCE		0x0402
#define WPARAM_DEL_SEQUENCE		0x0403
////////////////////////////////////////////////////////////////////////////
#define WPARAM_SET_VIEW			0x0500
#define WPARAM_PIN_ENTERED		0x0501
#define WPARAM_DISCONNECT		0x0502
#define WPARAM_ALARM_CONNECTION	0x0503
////////////////////////////////////////////////////////////////////////////
#define WPARAM_AUTOVIEWMODE		0x0600
#define WPARAM_REQUEST			0x0601
  #define RM_NORMAL				0
  #define RM_PERFORMANCE		1
#define WPARAM_SET_FOREGROUND	0x0602
#define WPARAM_SET_WINDOW_SIZE	0x0603
////////////////////////////////////////////////////////////////////////////
#define WPARAM_ADD_BACKUP		0x0700
#define WPARAM_DEL_BACKUP		0x0701
#define WPARAM_ADD_DEBUG		0x0702
#define WPARAM_DEL_DEBUG		0x0703
////////////////////////////////////////////////////////////////////////////
#define WPARAM_UPDATE_MENU		0x0800
#define WPARAM_CONFIG_MENU		0x0801
#define WPARAM_PANE_INT			0x0802

#define REGKEY_DVCLIENT				_T("DV\\DVClient")
#define REGKEY_DVSTARTER			_T("DV\\DVStarter")

#define REGKEY_ENABLE_PRINTER		_T("EnablePrinter")
#define REGKEY_DEFAULTCAM			_T("DefaultCam")
#define REGKEY_OWNNUMBER			_T("OwnNumber")
#define REGKEY_ISDNUNIT				_T("ISDNUnit")
#define REGKEY_ISDNUNIT2			_T("ISDNUnit2")
#define REGKEY_OEM					_T("Oem")
#define REGKEY_SETUP				_T("Setup")
#define REGKEY_SETTINGS				_T("settings")

#define REGKEY_MAX_SENT_SAMPLES      _T("MaxSentSamples")
#define REGKEY_MAX_REQUESTED_RECORDS _T("MaxRequestedRecords")
#define REGKEY_TIME_OUT_PLAY		 _T("TimeOutPlay")

#define REGKEY_BRIGHTNESS			_T("Brightness")
#define REGKEY_CONTRAST				_T("Contrast")
#define REGKEY_SATURATION			_T("Saturation")
#define REGKEY_CAMERANAME			_T("CameraName")
#define REGKEY_MDALARMSENSITIVITY	_T("MDAlarmSensitivity")
#define REGKEY_MDMASKSENSITIVITY	_T("MDMaskSensitivity")
#define REGKEY_RECORDINGMODE		_T("RecordingMode")
#define REGKEY_ALARMCALLEVENT		_T("AlarmCallEvent")
#define REGKEY_SEQUENZDWELLTIME		_T("SequencerDwellTime")
#define REGKEY_TIMERONOFF			_T("TimerOnOff")
#define REGKEY_ALARMOUTPUTONOFF		_T("AlarmOutputOnOff")
#define REGKEY_ALARMDIALINGNUMBER	_T("AlarmDialingNumber%d")
#define REGKEY_VIDEOOUTPORT			_T("VideoOutPort%d")
#define REGKEY_LOCALIPADDRESS		_T("IPAddress")
#define REGKEY_LOCALSUBNETMASK		_T("Subnetmask")	
#define REGKEY_LOCALDEFAULTGATEWAY	_T("DefaultGateWay")
#define REGKEY_LOCALMSN				_T("MSN")
#define REGKEY_LOCALHOSTNAME		_T("LocalHostName")
#define REGKEY_TIMEZONEINDEX		_T("TimeZoneIndex")
#define REGKEY_DEFAULTLANGUAGE		_T("DefaultLanguage")
#define REGKEY_TARGETCROSS			_T("TargetCross")
#define REGKEY_AUDIOALARM			_T("AudioAlarm")
#define REGKEY_CAMERARESOLUTION		_T("CameraResolution")
#define REGKEY_TIMER				_T("Timer%d")
#define REGKEY_CAMERAXX				_T("Camera%d")
#define REGKEY_OTHERSETTINGS		_T("OtherSettings")
#define REGKEY_OEM					_T("Oem")
#define REGKEY_BUILDNUMBER			_T("BuildNumber")
#define REGKEY_XPELICENSE			_T("XPeLicense")
#define REGKEY_NROFCAMERAS			_T("NumberOfCameras")
#define REGKEY_DONGLENET			_T("EEPromDongleBitNet")
#define REGKEY_DONGLEISDN			_T("EEPromDongleBitISDN")
#define REGKEY_DONGLEBACKUP			_T("EEPromDongleBitBackUp")
#define REGKEY_DONGLEBACKUPTODVD	_T("EEPromDongleBitBackUpToDVD")
#define REGKEY_CAMERACOMPRESSION	_T("CameraCompression")
#define REGKEY_CAMERAFPS			_T("CameraFPS")
#define REGKEY_CAMERATYPE			_T("CameraType")
#define REGKEY_CAMERAID				_T("CameraID")
#define REGKEY_CAMERACOMPORT		_T("CameraComport")
#define REGKEY_REALTIME				_T("Realtime")
#define REGKEY_AUDIOSETTINGS		_T("AudioSettings")
#define REGKEY_CAMERANUMBER			_T("CameraNumber")
#define REGKEY_INPUTSENSITIVITY		_T("InputSensitivity")
#define REGKEY_INPUTTHRESHOLD		_T("InputThreshold")
#define REGKEY_OUTPUTVOLUME			_T("OutputVolume")
#define REGKEY_RECORDINGMODE		_T("RecordingMode")
#define REGKEY_INPUTON				_T("InputOn")
#define REGKEY_OUTPUTON				_T("OutputOn")


#define CAN_SET_COMPUTERNAME		_T("CanSetComputerName")

// Performance Management
typedef LONG (WINAPI *PROCNTQSI)(UINT,PVOID,ULONG,PULONG);
#define MAX_PERFORMANCE_LEVEL 10

// CDVClientApp:
// See DVClient.cpp for the implementation of this class
//
class CIPCServerControlDVClient;
class CIPCAudioDVClient;

#define DEFAULT_BEEP (UINT)-1

class CDVClientApp : public CWinApp,
	public COsVersionInfo
{
	// construction / destruction
public:
	CDVClientApp();
	virtual ~CDVClientApp();

	// attributes
public:
	inline BOOL		   IsReadOnly() const;
	inline BOOL		   IsAudioAllowed() const;
	inline BOOL		   ShowMilliseconds() const;
	inline BOOL		   ShowSequenceInfo() const;
	inline BOOL		   ShowFPS() const;
	inline BOOL		   ShowCPUusage() const;
	inline BOOL		   TKR_Trace() const;
	inline BOOL		   TKR_TraceBackup() const;
	inline BOOL		   AudibleAlarm() const;
	inline BOOL		   ChangeCodePage() const;
	inline CString	   GetComputerName();
	inline BOOL		   IsComputerNameValid();

	CMainFrame* GetMainFrame();
	CPanel*     GetPanel();	
	CServer*    GetServer();
	
	BOOL IsReceiver();
	BOOL IsTransmitter();
	
	const CString& GetHomeDir() const;
	const CString& GetWindowsDirectory() const;

	BOOL SetPrinterOriantation(CDC &dcPrint, const CString &sDeviceName, short dmOrientation);
	BOOL GetPrinterStatus(DWORD& dwPrinterStatus, CString& sPrinterStatus);

	BOOL CopyFile(const CString &sFrom, const CString &sTo);

	inline int GetMajorVersion() const;
	inline int GetMinorVersion() const;
	inline int GetSubVersion() const;
	inline int GetBuildNr() const;
	inline BOOL CanRectangleQuery() const;
	inline BOOL CanAlarmList() const;
	inline BOOL CanActivityMask() const;
	inline BOOL CanAlarmOutput() const;
	inline BOOL CanAlarmCall() const;
	inline BOOL CanMultiCDExport() const;
	inline BOOL HasExpanionCode() const;
	
	DWORD GetCPUClockFrequency(){return m_dwCPUClock;};
	void  SetBackupWithACDC(BOOL bCanBackup);
	BOOL  CanBackupWithACDC();
	void  SetACDCBackupDrive(BOOL bDriveAvailable);	//setzt die member variable 
													//m_bIsACDCBackupDrive
	inline BOOL  IsACDCBackupDrive();	//liefert m_bIsACDCBackupDrive zurück
	
	BOOL SetRealtime(BOOL bRealtime);
	BOOL CanRealtime();
	
	inline DWORD		GetCPUusage() const;	// Performance Management
	inline DWORD		GetAvgCPUusage();
	inline int			GetPerformanceLevel() const;

	// operations
public:
	void ForceOnIdle();
	void Connect(const CString& sHostNumber, const CString& sHostName, const CString& sPin);
	void Disconnect();
	void DoDisconnect();
	void SetAudibleAlarm(BOOL bAlarm);
	void DoAlarmConnection(const CConnectionRecord& c);
	void OnAlarmConnection();
	bool SetTimeOutValue(int nValue=-1);
	BOOL MessageBeep(UINT);
	void CalcCPUusage();						// Performance Management
	void CheckPerformanceLevel();
	void IncreasePerformanceLevel();
	void DecreasePerformanceLevel();

	//media (audio) implementations
	CIPCAudioDVClient* GetAudio();
	CIPCAudioDVClient* GetLocalAudio();
	BOOL IsLocalAudioConnected();
	void ConnectToLocalAudio();
	static int GetIntValue(LPCTSTR sKey, int nDefault=0, BOOL bCreate=TRUE);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDVClientApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation
protected:
	void RegisterWindowClass();
	void InitReadOnlyModus();
	void LoadConfiguration();
	void SaveConfiguration();

public:
	//{{AFX_MSG(CDVClientApp)
	afx_msg void OnAppExit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CEvent		m_IdleEvent;
	HCURSOR		m_hArrow;
	HCURSOR		m_hNoCursor;
	HCURSOR		m_hGlass;
	HCURSOR		m_hRect;
	HCURSOR		m_hPanLeft;
	HCURSOR		m_hPanRight;
	HCURSOR		m_hTiltUp;
	HCURSOR		m_hTiltDown;
	HCURSOR		m_hZoomIn;
	HCURSOR		m_hZoomOut;
	HCURSOR		m_hFocusFar;
	HCURSOR		m_hFocusNear;
	HCURSOR		m_hFocusAuto;
	HCURSOR		m_hLeftTop;
	HCURSOR		m_hRightTop;
	HCURSOR		m_hRightBottom;
	HCURSOR		m_hLeftBottom;

	int			m_nMaxSentSamples;
	int			m_nMaxRequestedRecords;
	int			m_nTimeOutPlay;
	BOOL		m_bUseMMtimer;

		
private:
	CServer*		m_pServer;
	CDVUIThread 	m_DVUIThread;
	CIPCServerControlDVClient* m_pCIPCServerControlDVClient;
	CRequestThread  m_RequestThread;
	BOOL			m_bReadOnly;
	BOOL			m_bReceiver;
	BOOL			m_bTransmitter;
	BOOL			m_bShowMilliseconds;
	BOOL			m_bShowFPS;
	BOOL			m_bShowCPUusage;
	BOOL			m_bShowSequenceInfo;
	BOOL			m_bTKR_Trace;
	BOOL			m_bTKR_TraceBackup;
	BOOL			m_bAudibleAlarm;
	BOOL			m_bChangeCodePage;
	BOOL            m_bUseSpeaker;
	CString			m_sHomePath;
	CString			m_sWindowsDir;
	CString			m_sComputerName;
	CEvent			m_LockEvent;
	DWORD			m_dwCPUClock;
	BOOL			m_bCanBackupWithACDC;	//TRUE, wenn ACDC eine Brennsoftwre gefunden hat
	BOOL			m_bIsACDCBackupDrive;	//TRUE, wenn ein Backup Laufwerk (CD-ROM drive)
											//vorhanden ist.
	BOOL			m_bRealtime;
	BOOL			m_bAudioAllowed;
public:
	BOOL			m_bComputerNameValid;
private:

	CIPCAudioDVClient *m_pCIPCLocalAudio;

	int m_iMajorVersion;
	int m_iMinorVersion;
	int m_iSubVersion;
	int m_iBuildNr;
	// Performance Management
	DWORD			m_dwCPUusage;
	int				m_nNoOfProcessors;
    LARGE_INTEGER	m_liOldIdleTime;
    LARGE_INTEGER	m_liOldSystemTime;
	PROCNTQSI		NtQuerySystemInformation;
	CWK_Average<DWORD>	m_AvgCPUusage;
	WORD			m_wUpperThreshold;
	WORD			m_wLowerThreshold;
	signed short	m_nPerformanceLevel;
	signed short	m_nMinPerfLevel;
};
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CDVClientApp::IsReadOnly() const
{
	return m_bReadOnly;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CDVClientApp::IsAudioAllowed() const
{
	return m_bAudioAllowed;
}
/////////////////////////////////////////////////////////////////////////////
inline const CString& CDVClientApp::GetHomeDir() const
{
	return m_sHomePath;
}
/////////////////////////////////////////////////////////////////////////////
inline const CString& CDVClientApp::GetWindowsDirectory() const
{
	return m_sWindowsDir;
}

/////////////////////////////////////////////////////////////////////////////
inline BOOL	CDVClientApp::ShowMilliseconds() const
{
	return m_bShowMilliseconds;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CDVClientApp::ShowFPS() const
{
	return m_bShowFPS;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CDVClientApp::ShowCPUusage() const
{
	return m_bShowCPUusage;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CDVClientApp::ShowSequenceInfo() const
{
	return m_bShowSequenceInfo;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CDVClientApp::AudibleAlarm() const
{
	return m_bAudibleAlarm;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CDVClientApp::ChangeCodePage() const
{
	return m_bChangeCodePage;
}
/////////////////////////////////////////////////////
inline int CDVClientApp::GetMajorVersion() const
{
	return m_iMajorVersion;
}
/////////////////////////////////////////////////////
inline int CDVClientApp::GetMinorVersion() const
{
	return m_iMinorVersion;
}
/////////////////////////////////////////////////////
inline int CDVClientApp::GetSubVersion() const
{
	return m_iSubVersion;
}
/////////////////////////////////////////////////////
inline int CDVClientApp::GetBuildNr() const
{
	return m_iBuildNr;
}
/////////////////////////////////////////////////////
inline BOOL CDVClientApp::CanRectangleQuery() const
{
	//return TRUE wenn Version  größer 1.4.1

	BOOL b = (    m_iMajorVersion == 1 
				&& ((m_iMinorVersion==4 && m_iSubVersion > 1) || m_iMinorVersion > 4)
				|| m_iMajorVersion>1);
	if(TKR_Trace())
	{
		TRACE(_T("TKR ##### CDVClientApp bool: %d\n"), b);
	}
	
	return b;
}
/////////////////////////////////////////////////////
inline BOOL CDVClientApp::CanAlarmList() const
{
	return    (m_iMajorVersion == 1 && m_iMinorVersion>=4)
		   || m_iMajorVersion>1;
}
/////////////////////////////////////////////////////
inline BOOL CDVClientApp::CanActivityMask() const
{
	return    (m_iMajorVersion == 1 && m_iMinorVersion>=5)
		   || m_iMajorVersion>1;
}
/////////////////////////////////////////////////////
inline BOOL CDVClientApp::CanAlarmOutput() const
{
#ifdef _DEBUG
	return TRUE;
#else
	return    (m_iMajorVersion == 1 && m_iMinorVersion>=5)
		   || m_iMajorVersion>1;
#endif
}
/////////////////////////////////////////////////////
inline BOOL CDVClientApp::CanAlarmCall() const
{
#ifdef _DEBUG
	return TRUE;
#else
	return    (m_iMajorVersion == 1 && m_iMinorVersion>=5)
		   || m_iMajorVersion>1;
#endif
}
/////////////////////////////////////////////////////
inline BOOL CDVClientApp::CanMultiCDExport() const
{
	return    (m_iMajorVersion == 1 && m_iMinorVersion>=5)
		   || m_iMajorVersion>1;
}
/////////////////////////////////////////////////////
inline BOOL CDVClientApp::HasExpanionCode() const
{
#ifdef _DEBUG
	return TRUE;
#else
	return    (m_iMajorVersion == 1 && m_iMinorVersion>=5)
		   || m_iMajorVersion>1;
#endif
}
/////////////////////////////////////////////////////////////////////////////
inline CString CDVClientApp::GetComputerName()
{
	return m_sComputerName;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CDVClientApp::IsComputerNameValid()
{
	return m_bComputerNameValid;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CDVClientApp::IsACDCBackupDrive()
{
	return m_bIsACDCBackupDrive;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CDVClientApp::TKR_Trace() const
{
	return m_bTKR_Trace;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CDVClientApp::TKR_TraceBackup() const
{
	return m_bTKR_TraceBackup;
}
/////////////////////////////////////////////////////////////////////////////
// Performance Management
inline DWORD CDVClientApp::GetCPUusage() const
{
	return m_dwCPUusage;
}
inline DWORD CDVClientApp::GetAvgCPUusage()
{
	return (DWORD)(m_AvgCPUusage.GetAverage()+0.5);
}
inline int CDVClientApp::GetPerformanceLevel() const
{
	return m_nPerformanceLevel;
}
/////////////////////////////////////////////////////////////////////////////
extern CDVClientApp theApp;
extern CWK_Timer theTimer;
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


#endif // !defined(AFX_DVCLIENT_H__6B5BBEC5_9757_11D3_A870_004005A19028__INCLUDED_)
