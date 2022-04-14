#if !defined(AFX_CPANEL_H__40CB1163_82C2_11D3_8D9D_004305A11E32__INCLUDED_)
#define AFX_CPANEL_H__40CB1163_82C2_11D3_8D9D_004305A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CPanel.h : header file
//
//***************************************************************************
// Beim Hinzufügen eines Menüeintrages bearbeiten:
//	-	Eventuell OnMenuDown und OnMenuUp
//	-	Eventuell SetDefaultSubMenu
//	- OnItemDown und OnItemUp
//	- OnCrossSet
//	- OnUpdateMenu
//	- ChangeDisplayColor
//***************************************************************************

#define MAX_CAM 16

#include "stdafx.h"

#include "COSMenu.h"

#include "resource.h"
#include "winspool.h"
#include "LangDllInfo.h"
#include "CUpdateSystem.h"


#define NETWORKCARD_NT	_T("Software\\Microsoft\\Windows NT\\CurrentVersion\\NetworkCards\\")
#define SERVICES		_T("System\\CurrentControlSet\\Services")
#define SERVICENAME		_T("ServiceName")
#define IPADDRESS		_T("IPAddress")
#define SUBNETMASK		_T("SubnetMask")
#define DEFAULTGATEWAY	_T("DefaultGateway")
#define	ENABLEDHCP		_T("EnableDHCP")

#define WM_UPDATE_BUTTONS			WM_USER + 1
#define WM_SET_VIEWMODE				WM_USER + 2
#define WM_SET_CAMERA				WM_USER + 3
#define WM_NOTIFY_CONNECT			WM_USER + 4
#define WM_EXIT						WM_USER + 5
#define WM_NOTIFY_DISCONNECT		WM_USER + 6
#define WM_UPDATE_MENU				WM_USER + 7
#define WM_TOGGLE_VIEWMODE			WM_USER + 8
#define WM_SHUTDOWN					WM_USER + 9
#define WM_REBOOT					WM_USER + 10
#define WM_ACTIVATE_PANEL			WM_USER + 11
#define WM_NOTIFY_CIPC_CONNECT		WM_USER + 12
#define WM_NOTIFY_ALARMLIST_UPDATE	WM_USER + 13
#define WM_NOTIFY_CALCULATE_END		WM_USER + 14
#define WM_NOTIFY_FOUND_MEDIUM		WM_USER + 15
#define WM_NOTIFY_DRIVE				WM_USER + 16

#define WM_PICTUREDATA				WM_USER + 20

#define MD_SENSITIVITY_UNKNOWN	-1
#define MD_SENSITIVITY_OFF		0
#define MD_SENSITIVITY_LOW		1
#define MD_SENSITIVITY_NORMAL	2
#define MD_SENSITIVITY_HIGH		3

#define ALARMCALL_EVENT_UNKNOWN	-1
#define ALARMCALL_EVENT_OFF		0
#define ALARMCALL_EVENT_A		1
#define ALARMCALL_EVENT_A1		2
#define ALARMCALL_EVENT_A2		3
#define ALARMCALL_EVENT_A12		4
#define ALARMCALL_EVENT_1		5
#define ALARMCALL_EVENT_2		6
#define ALARMCALL_EVENT_12		7

#define AUDIO_RECORDING_UNKNOWN	-1
#define AUDIO_RECORDING_OFF		0
#define AUDIO_RECORDING_A		1
#define AUDIO_RECORDING_L		2
#define AUDIO_RECORDING_E		3
#define AUDIO_RECORDING_AL		4
#define AUDIO_RECORDING_AE		5
#define AUDIO_RECORDING_LE		6
#define AUDIO_RECORDING_ALE	7

#define CARD_TYPE_UNKNOWN		-1
#define CARD_TYPE_JACOB			1
#define CARD_TYPE_SAVIC			2
#define CARD_TYPE_TASHA			3
#define CARD_TYPE_Q				4

#define MAX_PRINTERS				16
#define MAX_SEQUENCER_DWELL_TIME	30

#define SPECIAL_HOSTIP 	_T("012.345.678.901")

#define PANEL_LAST_RECORD		98

#define VIDEO_AGC_ON   0	//Value are the same like in UDP API in Q Unit
#define VIDEO_AGC_OFF  3	//Value are the same like in UDP API in Q Unit
#define VIDEO_FORMAT_JPEG	0
#define VIDEO_FORMAT_MPEG4	1

#include "PlayWindow.h"
class CDVUIThread;
class CMainFrame;
class CRTEDlg;
class CKeyboardDlg;
class CExportDlg;
class CStatisticDlg;
class CAlarmlistDlg;
class CMinimizedDlg;
class CMDConfigDlg;
class CHostsDlg;
class CInfoDlg;
class CMaintenanceDlg;
class CCameraTypeDlg;

//const char* NameOfEnum(OEM oem);
//OEM StringToOEM(const CString& sOem);
typedef enum 
{
	Live=0,
	Play=1, 
	LivePlay=2, 
	PlayLive=3,
	Backup=4,
	Debug=5,
}WindowType;

typedef enum
{
	MT_NOMEDIUM			= 0,
	MT_CD_WRITABLE		= 1,
	MT_CDRW_ERASE		= 2,
	MT_MEDIUM_ARRIVED	= 3,
	MT_DVD_NOT_ALLOWED	= 4,

}MEDIUMTYPE;

class CLiveWindow;
class CPlayWindow;
class CBackupWindow;
class CDebugWindow;
class CPanel : public CTransparentDialog
{
	DECLARE_DYNAMIC(CPanel)
private:
	typedef enum
	{				
		InputNo,						
		InputCamFirstDigit,			
		InputPanelPIN,					
		InputOldPIN, 
		InputNewPIN,
		InputConfirmPIN,
		InputConfigPIN,
		InputSearchDate, 
		InputSearchTime, 
		InputTimer,
		InputClockDate,
		InputClockTime,
		InputIP,
		InputSubNetMask,
		InputMSN,
		InputGateway,
		InputChangeCamName,
		InputExportDlgOpen,
 		InputBackupStartDateTime,
		InputBackupWaitCalculatedEnd,
		InputBackupEndDateTime,
		InputBackupCameras,
		InputBackupAskForMedium,
		InputBackupWaitForMedium,
		InputBackupFoundMedium,
		InputBackupFoundNoEmptyCD,
		InputBackupDVDNotAllowed,
		InputBackupEraseMedium,
		InputBackupEraseMediumConfirmingYes,
		InputBackupEraseMediumConfirmingNo,
		InputUpdateSystem,
		InputChangeNetName,
		InputSelectHost,
		InputEditHostList,
		InputEditAlarmDialingNumber,
		InputSetExpansionCode,
		InputShutdownPIN,
		InputResetPIN,
		InputPrinterPIN,
		InputCameraPTZID,
		InputCameraPTZComPort,
	} 
	INP_STATE;
	
	enum 
	{
		Su=0,
		Mo=1, 
		Tu=2, 
		We=3, 
		Th=4, 
		Fr=5, 
		Sa=6, 
	} WeekDays;

	typedef enum
	{
		UM_INVALID = 0,
		UM_SUPERVISOR = 1,
		UM_OPERATOR = 2,
	} USER_MODE;

public:

	typedef enum
	{
		DisplayAlarm = 1,
	}
	Display;

	//typedef für die Bestätigung der Eingabe beim Wechsel von MD- und Alarmarchiven
	typedef enum 
	{
		AlarmModeUnknown			=-1,
		UVVKassen					= 1,
		MotionDetection				= 2, 
		UVVKassenConfirmingYes		= 3,
		UVVKassenConfirmingNo		= 4,
		MotionDetectionConfirmingYes= 5,
		MotionDetectionConfirmingNo	= 6,
	} AlarmMode;
	
	//typedef für die Bestätigung des Löschens von Alarmarchiven 
	//und für die Bestätigung des Abbruchs eines Backups
	typedef enum
	{
		ConfirmingModeUnknown	=-1,
		ConfirmingYes			= 1,
		ConfirmingNo			= 2,

//		AlarmDeleteModeUnknown		=-1,
//		AlarmDeleteConfirmingYes	= 1,
//		AlarmDeleteConfirmingNo		= 2,
	} ConfirmingYesNoMode;


	// construction / destruction
public:
	CPanel();   // standard constructor
	virtual ~CPanel();

	// attributes
public:
	inline OEM		GetOem() const;
	inline CString	GetCurrentLanguage() const;
	inline int		GetNrOfCameras() const;
	inline BOOL		IsSupervisor() const;
	inline BOOL		IsOperator() const;
	inline BOOL		IsMinimized() const;
	inline CMDConfigDlg* GetMDConfigDialog();
	inline CMinimizedDlg* GetMinimizedDlg();
	inline CLangDllInfo* GetLangDllInfo();
	CString	GetCurrentLanguageAbbr() const;
	CLiveWindow*	ExistLiveCamera(int nCam);
	CPlayWindow*	ExistPlayCamera(int nCam);
	CDebugWindow*	ExistDebugWindow();
	CBackupWindow*	ExistBackupCamera();
	static CString	NameOfEnumInputState(INP_STATE Inp);

	int			GetActiveCamera();
	BOOL		IsPrinterAvailable();
	int			GetDefaultPrinter();
	CString		GetHostName();
	CString		GetHostNumber();
	CString		GetLocalHostName();
	BOOL		SetLocalHostName(const CString& sLocalHostName, BOOL bImportSettings = FALSE);

	// operations	
public:						  
	CString GetDriveName(LPCTSTR);
	void SetTooltipText(const CString& sMsg, bool bForce=false);
	BOOL Create();

	void SetCamera(int nCamNr, WindowType eType);
	void SetViewMode(ViewMode vm, WORD wCam);
	void ToggleViewMode(WORD wCam);
	void SetDisplayActive(int nCamNr);
	void NotifyConnect(BOOL bOK, int iErrorCode);
	void NotifyCIPCConnect(ServerControlGroup control);
	void NotifyDisconnect();
	void UpdateButtons();
	void UpdateMenu();
	void ActivatePanel();
	void SetPanelPosition(BOOL bCenter);
	BOOL OnStartDatabaseSearch(const CString &sDate, const CString &sTime, int nCamNr);
	void ForceConfirmSelfcheck();

	// Diese Dinge sind derzeit über das EEProm verdongelt
	BOOL IsNetAllowedByDongle();
	BOOL IsISDNAllowedByDongle();
  	BOOL IsBackUpAllowedByDongle();
  	BOOL IsDVDBackUpAllowedByDongle();

	//wird gesetzt, wenn ältestes DB-Bild gefunden wurde, 
	//bzw. der benutzerdefinierte Endzeitpunkte auf die CD passt
	void SetCalculateEndComplete(); 

	//wird gesetzt, wenn MB`s berechnet, wenn User nicht den berechneten Endzeitpunkt gewählt hat, 
	//sondern einen anderen (der davor liegt) benutzt. Dann müssen die MB`s zum Backup neu 
	//berechnet werden. Wird benötigt für die korrekte Prozentanzeige beim backup sowie für 
	//den Fall, wenn keine MB`s zwischen Start- und benutzerspezifischen Endzeitpunkt gefunden werden.
	//wurden MB`s gefunden, bOK = TRUE, keine MB`s gefunden, bOK = FALSE
	void SetEventCalculateMBUserBackupEnd(BOOL bOK);
		
	void SetCanRectSearch(BOOL bRectSearch);
	void SetImagesToBackup(BOOL bImages);
	void SetTooManyImages(BOOL bToManyImages);
	void SetMultiBackupMembers(CSystemTime stBackupStartLast,  //Speichert nötigen members für Multi Backup
							   CSystemTime stBackupEndLast,
							   WORD wCamBackupEnd,
							   CString sBackupCamsSeperated,
							   WORD wLastSequence,
							   DWORD dwLastRecord);

	void SetEventLastBackupRecord(BOOL bOK);//zum Überprüfen, ob Menueintrag "Auslagerung fortsetzen"
										    //angezeigt werden soll (in: CanMultiBackup())
	void SetACDCBackupDrive(BOOL bDriveAvailable); //setzte member m_bACDCBackupDrive, ist FALSE
												   //wenn kein Brenner vorhanden ist

	BOOL OnSetMSN(const CString &sMSN);
	BOOL GetIP(CString &sIP, CString sParam, CStringArray* psKeyList = NULL);
	BOOL OnSetIP(const CString &sIP, CString sParam, BOOL bForceReboot=TRUE);
	BOOL DHCPParameters(BOOL bSet=FALSE, BOOL bDHCP=-1);
	BOOL SendNetShellCmd();
	static CString FormatIP(DWORD dwIP);
	static CString ConvertIP(const CString& sIP, BOOL bWantPoints);

	CString GetMSN();

	int  OnGetTimeZones();			// liefert den Index der aktuellen Zeitzone
	BOOL OnGetTimeZone(CTimeZoneInformation& TimeZone);
	BOOL OnSetTimeZone(int nIndex); // Setzt die 'nIndex'-Zeitzone des Zeitzonenarrays

	BOOL LoadDefaultLanguageDll(CString sLanguage);
	BOOL IsDVDBackupAllowedByDongle();		//Ist DVD Backup freigeschaltet
	
//TODO TKR für Sync Abspielen
/*
	BOOL CanSyncPlay();		//sollen PlayWindows synchron abgespielt werden

	//prüfe ob ein anderes sync playwnd abgespielt werden muss
	BOOL CheckNextSyncPlayWnd(CPlayWindow::PlayStatus& playstatus, 
						      CPlayWindow::PlayStatus& currentPlayStatus, 
						      CPlayWindow* pPlayWnd);
*/
protected:
	void InitMembers();
	void InitCodePageByteArrays();
	void InitConfiguration();
	void LoadLanguageDll();
	BOOL UnloadLanguageDll();
	void LoadMenus();
	void LoadNormalMenu();
	void LoadConfigMenu();
	BOOL ExtractAndValidateTime(const CString sTime, int &nHour, int &nMinute, int &nSecond);
	BOOL ExtractAndValidateDate(const CString& sDate, int &nDay, int &nMonth, int &nYear);
	BOOL IsDateTimeInFuture(int iDay,int iMonth,int iYear,
							int iHour,int iMinute,int iSeconds);
	BOOL IsDateTimeBeforeFirstImage(int iDay,int iMonth,int iYear,
									int iHour,int iMinute,int iSeconds);
	BOOL ValidateIP(const CString &sIP);
	BOOL ValidateSubNetMask(const CString &sMask);
	
	BOOL ValidateMSN(const CString &sMSN);
	BOOL ValidateTimer(const CString &sTimer);

	void LoadConfiguration();
	void SaveConfiguration();
	BOOL OnSetSubNetMask(const CString &sMask);
	BOOL OnSetDefaultGateway(const CString &sGateway);

	BOOL OnSetTimer(const CString &sTimer);
	BOOL IsMDActive(int nCamNr);
	BOOL IsActiveCamUVVActive();
	BOOL IsAnyUVVActive();

	BOOL OnSetClock(const CString &sDate, const CString &sTime);
	BOOL OnSetPlayStatus(CPlayWindow::PlayStatus playstatus, int nCamera=-1);

	BOOL GetSubNetMask(CString &sSubNetMask);
	BOOL GetDefaultGateway(CString &sGateway);

	void OnSwitchToNextLiveCamera();
	void OnSwitchToPrevLiveCamera();
	
	void OnSetViewMode(int nViewMode);
	void OnChangeAlarmMode();
	void OnChangeConfirmingYesNoMode();

	BOOL SetBrightness(int nBrightness);
	BOOL SetContrast(int nContrast);
	BOOL SetSaturation(int nSaturation);
	BOOL GetBrightness(int &nBrightness);
	BOOL GetContrast(int &nContrast);
	BOOL GetSaturation(int &nSaturation);
	BOOL SetResolution(int nResolution);
	BOOL GetResolution(int &nResolution);
	BOOL SetCompression(int nCompression);
	BOOL GetCompression(int &nCompression);
	BOOL SetCameraFPS(int nFPS);
	BOOL GetCameraFPS(int &nFPS);
	BOOL SetCameraPTZType(int nCameraPTZType);
	BOOL GetCameraPTZType(int &nCameraPTZType);
	BOOL SetCameraPTZID(DWORD dwCameraPTZID);
	BOOL GetCameraPTZID(DWORD &dwCameraPTZID);
	BOOL SetCameraPTZComPort(int nCameraPTZComPort);
	BOOL GetCameraPTZComPort(int &nCameraPTZComPort);

	BOOL OnIncrementSaturation();
	BOOL OnIncrementContrast();
	BOOL OnIncrementBrightness();
	BOOL OnDecrementSaturation();
	BOOL OnDecrementContrast();
	BOOL OnDecrementBrightness();

	void OnIncrementMaskSensitivity();
	void OnIncrementAlarmSensitivity();
	void OnDecrementMaskSensitivity();
	void OnDecrementAlarmSensitivity();
	
	void OnIncrementAlarmCallEvent();
	void OnDecrementAlarmCallEvent();

	//Audio Settings
	void OnChangeAudio(int nAudioParam, int iValue);
	void OnChangeAudioCamNr(int iValue);
	void OnChangeAudioRecordMode(int iValue);

	void OnChangePIN(BOOL bSupervisor);
	BOOL OnChangeSupervisorPIN(const CString &sPIN);
	BOOL OnChangeOperatorPIN(const CString &sPIN);
	void OnPINEntered(const CString &sPIN);
	void OnShutdownResetPINEntered(const CString &sPIN);

	void OnBeginConfig();
	void OnEndConfig();
	void SetBaseColor(COLORREF cr);
	void SetDisplayColor(COLORREF cr);
	BOOL OnChangeCameraName();
//	BOOL OnOpenExportDlg();			// not used
	void OnChangeTargetDisplay();
	void OnToggleTimerOnOffState();
	BOOL SetTimerOnOffState();

	BOOL OnSetExpansionCode();

	BOOL OnIncrementTimeZone();	
	BOOL OnDecrementTimeZone();	
	BOOL GetTimerOnOffState(BOOL& bTimerOnOffState);

	void OnToggleAlarmOutputOnOffState();
	BOOL SetAlarmOutputOnOffState();
	BOOL GetAlarmOutputOnOffState(BOOL& bAlarmOutputOnOffState);
	BOOL SetAlarmDialingNumber(int nCallingNumber, const CString& sCalingNumber);
	BOOL GetAlarmDialingNumber(int nCallingNumber, CString& sCalingNumber);

	void OnUpdateMenuSequencerDwellTime();	
	void OnIncrementSequencerDwellTime();	
	void OnDecrementSequencerDwellTime();	
	BOOL OnGetSequencerDwellTime(int& iSeconds);
	BOOL OnSetSequencerDwellTime();

	void OnUpdateMenuVideoOutputMode(int iIndex);
	void IncrementVideoOutputMode();	
	void DecrementVideoOutputMode();	
	BOOL GetVideoOutputMode(int iIndex, enumVideoOutputMode& eVideoOutputMode);
	BOOL SetVideoOutputMode(int iIndex);
	CString GetVideoOutputModeName(enumVideoOutputMode eVideoOutputMode);
	
	void OnUpdateMenuAudibleAlarm();
	void OnChangeAudibleAlarm();
	void OnUpdateMenuRealtime();
	void OnChangeRealtime();
	void OnUpdateMenuCameraResolution();
	void OnUpdateMenuCameraCompression();
	void OnUpdateMenuCameraFPS();
	void OnChangeCameraResolution(BOOL bIncreaseItem);
	void OnChangeCameraCompression(BOOL bIncreaseItem);
	void OnChangeCameraFPS(BOOL bIncreaseItem);
	void OnUpdateMenuCameraPTZType();
	void OnUpdateMenuCameraPTZID();
	void OnUpdateMenuCameraPTZComPort();
	void OnDecrementCameraPTZType();
	void OnIncrementCameraPTZType();
	void OnDecrementCameraPTZComPort();
	void OnIncrementCameraPTZComPort();

	void OnUpdateCameraTermination();
	void OnChangeCameraTermination();
	BOOL SetCameraTermination();

	//new menu for Q Unit only
	void OnChangeSystemAGC();
	void OnUpdateMenuSystemAGC();
	BOOL SetSystemAGC();
	void OnChangeSystemVideoformat();
	void OnUpdateMenuSystemVideoformat();
	BOOL SetSystemVideoformat();
	void OnChangeSystemVideonorm();
	void OnUpdateMenuSystemVideonorm();
	BOOL SetSystemVideonorm();

	void ShowOnlineHelp();

	void OnMenuUp();
	void OnMenuDown();
	void SetDefaultSubMenu();
	void OnItemUp();
	void OnItemDown();


	BOOL OnChangeLocalHostName();
	BOOL LoadHostList();
	BOOL OnSelectHost();
	BOOL OnIncrementHost();
	BOOL OnDecrementHost();

	void CreateBackupWindow();
	void OnDeleteBackup();
	BOOL CanMultiBackup();		//ist das Fortsetzen der letzen Auslagerung möglich

// Dialog Data
	//{{AFX_DATA(CPanel)
	enum { IDD = IDD_PANEL };
	CSkinButton	m_ctrlButtonPrint;
	CSkinButton	m_ctrlButtonExport;
	CSkinButton	m_ctrlButtonSkipForward;
	CSkinButton	m_ctrlButtonSkipBack;
	CSkinButton	m_ctrlButtonSingleForward;
	CSkinButton	m_ctrlButtonSingleBack;
	CSkinButton	m_ctrlButtonPlayBack;
	CSkinButton	m_ctrlButtonMinimize;
	CSkinButton	m_ctrlCrossSet;
	CSkinButton	m_ctrlCrossUp;
	CSkinButton	m_ctrlCrossRight;
	CSkinButton	m_ctrlCrossLeft;
	CSkinButton	m_ctrlCrossDown;
	CSkinButton	m_ctrlButtonEject;
	CSkinButton	m_ctrlButtonFastBack;
	CSkinButton	m_ctrlButtonPlayForward;
	CSkinButton	m_ctrlButtonFastForward;
	CSkinButton	m_ctrlButtonSearch;
	CSkinButton	m_ctrlButtonOk;
	CSkinButton	m_ctrlButtonNum9;
	CSkinButton	m_ctrlButtonNum8;
	CSkinButton	m_ctrlButtonNum7;
	CSkinButton	m_ctrlButtonNum6;
	CSkinButton	m_ctrlButtonNum5;
	CSkinButton	m_ctrlButtonNum4;
	CSkinButton	m_ctrlButtonNum3;
	CSkinButton	m_ctrlButtonNum2;
	CSkinButton	m_ctrlButtonNum1;
	CSkinButton	m_ctrlButtonNum0;
	CSkinButton	m_ctrlButtonPlus;
	CSkinButton	m_ctrlButtonClear;
	CDisplay	m_ctrlDisplayMenu1;
	CDisplay	m_ctrlDisplayMenu2;
	CDisplay	m_ctrlDisplayCamera;
	CDisplay	m_ctrlDisplayTooltip;
	CDisplay	m_ctrlDisplayDateTime;
	CFrame		m_ctrlBorder1;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPanel)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL
protected:
	virtual BOOL	StretchElements();

// Implementation
protected:
	BOOL OnImportSettings();
	BOOL OnExportSettings();
	BOOL OnMaintenance();
	void WaitToReadErrorMessage();
	void WaitToReadInfoMessage();
	COLORREF ChangeDisplayColor();
	BOOL SetDefaultPrinter(int nIndex);
	BOOL EnumeratePrinters();
	void SetImageList(CSkinButton&btn, int cx, UINT nID);

	void OnAlarmDelete();
	void OnAlarmBackup();
	BOOL OnActivityBackup();
	BOOL OnStopActivityBackup();
	BOOL OnMultyActivityBackup();
	BOOL OnBackup(BOOL bMultiBackup15 = FALSE, BOOL bAlarmBackup = FALSE);
	BOOL OnBackup14();
	BOOL OnBackup15();
	void OnBackup15_CheckBackupCameras();
	void OnBackup15_CheckStartDateTime();
	void OnBackup15_WaitForCalculatedEnd();
	void OnBackup15_CheckEndDateTime();
	BOOL OnBackup15Multi();
	BOOL OnBackupAlarm();
	BOOL OnStartBackup();
	BOOL OnBackupACDC();
	BOOL OnBackupACDCAlarm();
	BOOL OnBackupACDCMulti();	
	BOOL OnBackupNotEmptyExternMedium();
	void OnBackup_FoundExternMedium();
	void OnBackup_EraseExternMedium();
	void OnBackupACDC_AskForMedium();
	void OnBackupACDC_WaitForMedium();
	void OnBackupACDC_FoundMedium();
	void OnBackupACDC_FoundNoEmptyCD();
	void OnBackupACDC_DVDNotAllowed();
	void OnBackupACDC_EraseCDRW();
	void OnBackupACDC_CheckBackupCameras();
	void OnBackupACDC_CheckStartDateTime();
	void OnBackupACDC_WaitForCalculatedEnd();
	void OnBackupACDC_CheckEndDateTime();

	void OnSuspectSearch();
	void OnSuspectDelete();
	void OnShowRTErrors();
	void OnShowStatistic();
	void OnShowAlarmlist();
	void OnShowMDConfigDlg();
	BOOL OnShowHostListDlg();

	BOOL IsDriveAvailable(BOOL bCheckDisk, CString &sDrivePath, CString &sDriveName, UINT nUnitMask=0);
	BOOL IsDiskAvailable(const CString& sDrive);
	void DisableInputMode(BOOL bForceDisable = FALSE);
	void EnableInputMode();
	void ToggleInputMode();
	void DoShutdown();
	void DoReboot();
	void OnDisconnect();
	int  CheckExistingCameras();
	void EnablePanel(BOOL bEnable=TRUE);
	void MinimizeDlg();
	void MaximizeDlg();
	BOOL EnumerateLanguages();
	CLangDllInfo* GetDllLanguageInfo(const CString& sDll);
	BOOL SetDefaultLanguage(int nIndex);
	int  GetDefaultLanguageIndex();
	BOOL SetTargetDisplayState();
	void SetDisplayText(CDisplay& display, CString sText, int iType=0, int iTime=0);
	int SetFormatDisplayText(CDisplay& display, CString sFormat, CString sText);
	void AdjustCodePageDifferences(CString& sText);

	void StartUpdateThread();
	static UINT DoUpdateThread(LPVOID pData);
	DWORD GetCRC(CString& sSource);
	bool GetPrinterStatus(PRINTER_INFO_2 *, CString*);
	int  FindPrinter(PRINTER_INFO_2*, int, CString);
	void CheckCapacityOfExternDrive(DWORD &dwFreeMB, DWORD &dwTotalMB);
	static unsigned int CALLBACK PrintSetupHookProc(HWND, UINT, WPARAM, LPARAM);

	void OnSystemInfo();

	inline BOOL IsConfigMenu();	//is the config menu actuel shown in panel
	inline BOOL IsNormalMenu();	//is the normal menu actuel shown in panel

	// Generated message map functions
	//{{AFX_MSG(CPanel)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnButtonOk();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnButtonNum1();
	afx_msg void OnButtonNum2();
	afx_msg void OnButtonNum3();
	afx_msg void OnButtonNum4();
	afx_msg void OnButtonNum5();
	afx_msg void OnButtonNum6();
	afx_msg void OnButtonNum7();
	afx_msg void OnButtonNum8();
	afx_msg void OnButtonNum9();
	afx_msg void OnButtonFF();
	afx_msg void OnButtonPlay();
	afx_msg void OnButtonRewind();
	afx_msg void OnButtonStop();
	afx_msg void OnButtonEject();
	afx_msg void OnButtonNum0();
	afx_msg void OnButtonClear();
	afx_msg void OnButtonSearch();
	afx_msg void OnButtonPlus();
	afx_msg void OnCrossUp();			  
	afx_msg void OnCrossDown();
	afx_msg void OnCrossLeft();
	afx_msg void OnCrossRight();
	afx_msg void OnCrossSet();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnButtonClose();
	afx_msg void OnButtonSingleForward();
	afx_msg void OnButtonSingleBack();
	afx_msg void OnButtonPlayBack();
	afx_msg void OnButtonSkipBack();
	afx_msg void OnButtonSkipForward();
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnButtonExport();
	afx_msg void OnRclickPrint();
	afx_msg void OnButtonPrint();
	afx_msg void OnClose();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg LRESULT OnUpdateButtons(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateMenu(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetCamera(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnToggleViewMode(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetViewMode(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnNotifyConnect(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnNotifyCIPCConnect(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnNotifyDisconnect(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnExit(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnShutDown(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnReboot(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPowerOffButtonPressed(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnResetButtonPressed(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnActivatePanel(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnNotifyCalculateEnd(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnNotifyFoundMedium(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnNotifyDrive(WPARAM wParam, LPARAM lParam);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	void OnButtonNum(const CString &s);
	static int CALLBACK EnumFontFamExProc(CONST ENUMLOGFONTEX *, CONST NEWTEXTMETRICEX *, DWORD, LPARAM);


public:
	static COLORREF		m_BaseColorButtons;
	static COLORREF		m_BaseColorBackGrounds;
	static COLORREF		m_BaseColorDisplays;

private:
	HINSTANCE		m_hLanguageDll;
	CRTEDlg*		m_pRTEDlg;
	CKeyboardDlg*	m_pKeyboardDlg;
//	CExportDlg*		m_pExportDlg;		// not used
	CStatisticDlg*	m_pStatisticDlg;
	CAlarmlistDlg*	m_pAlarmlistDlg;
	CInfoDlg*		m_pInfoDlg;
	CMaintenanceDlg* m_pMaintenanceDlg;
	CMDConfigDlg*	m_pMDConfigDlg;
	CHostsDlg*		m_pHostsDlg;

	CSkinButtons	m_Buttons;
	COSMenu*		m_pOSD;
	COSMenu			m_OSDNormal;
	COSMenu			m_OSDConfig;

	HCURSOR			m_hCursor;

	BOOL		m_bShowPower;
	BOOL		m_bShowTargetCross;
	BOOL		m_bAudibleAlarm;
	int			m_nSystemAGC;
	int			m_nSystemVideoformat;
	CString		m_sSystemVideonorm;
	BOOL		m_bRealtimePanel;
	int			m_nCamNr;
	int			m_nDayOfWeek;
	int			m_nMDAlarmLevel;
	int			m_nMDMaskLevel;
	ViewMode	m_eViewMode;
	AlarmMode	m_eAlarmMode;
	ConfirmingYesNoMode  m_eConfirmingYesNoMode;
	int			m_iSequencerDwellTime;
	enumVideoOutputMode	m_eVideoOutputMode;

	DWORD		m_dwLastUserAction;
	DWORD		m_dwMinimizeTimespan;

	CString		m_sInput;
	CString		m_sNewPIN;
	CString		m_sSearchDate;
	CString		m_sSearchTime;
	CSystemTime	m_stBackupStart;
	CSystemTime	m_stBackupEnd;
	CString		m_sBackupCameras;
	CString		m_sIP;
	CString		m_sSubNetMask;
	CString		m_sMSN;
	CString		m_sHostAddress;
	CString		m_sHostName;
	int			m_nHostIndex;
	BOOL		m_bDHCPenabled;
	BOOL		m_bNetShellOK;
	DWORD		m_dwInterfaceIndex;
	CWKControlledProcess *m_pControlProcess;
	CString		m_sNetworkAdapterName;
	CString		m_sNetworkAdapterKey;
	CString		m_sNetShellCmd;

	CString		m_sExportPath;
	CString		m_sExportDriveName;
	CString		m_sPrevMsg;
	CString		m_sCurrentLanguage;
	CString		m_sOEMName;

	INP_STATE	m_eInputState;
	BOOL		m_bSetValue;
	BOOL		m_bDDMMYY;
	BOOL		m_bNetAllowedByDongle;		
	BOOL		m_bISDNAllowedByDongle;
	BOOL		m_bBackUpAllowedByDongle;		// BackUp auf CD, Memorystick, etc...
	BOOL		m_bBackUpToDVDAllowedByDongle;	// BackUp auf DVD
	BOOL		m_bTimerOnOffState;
	BOOL		m_bAlarmOutputOnOffState;
	BOOL		m_bCameraTermination;
	int			m_bDateTime;

	PRINTER_INFO_2*	m_pPrinterInfo;		// PrinterInfo
	int					m_nPrinterInfoSize;	// Arraygröße der PRINTER_INFO_2 Struktur
	int					m_nPrinterCnt;			// Anzahl der Drucker, die bereit sind (nicht offline)
	int               m_nNewDefaultPrt;		// neuer Default Printer
	int*					m_pnPrinterSort;		// Sortierarray, das auf die "OK"-Printer zeigt
		
	CLangDllInfoArray	m_arrayLanguages;
	CLangDllInfo*		m_pDefaultLanguage;
	int					m_iLanguageCnt;

	int					m_nExportDrive;			// Export Laufwerk
	int					m_nExternBackupDrive;	// Extern Backup Drive (MemoryStick, HDD)
	BOOL				m_bBackupToExternDrive;
	BOOL				m_bIsExportDrive;		// Export Laufwerk vorhanden (Diskette oder MemoryStick)

	CWinThread*		m_pUpdateThread;
	CUpdateSystem	m_Update;
	OEM				m_eOem;
	
	int				m_nCameras;

	int				m_nOldBrightess;
	int				m_nOldContrast;
	int				m_nOldSaturation;
	int				m_nResolution;
	int				m_nCurrentResolution;
	int				m_nCompression;
	int				m_nCameraFPS;
	int				m_nCurrentCompression;
	int				m_nCurrentCameraFPS;
	int				m_nCameraPTZType;
	int				m_nCurrentCameraPTZType;
	DWORD			m_dwCameraPTZID;
	int				m_nCameraPTZComPort;
	int				m_nCurrentCameraPTZComPort;
	int				m_nOldAudioValue;

	CTimeZoneInformations m_TziArray;
	int				m_nTimeZone;

	CPtrArray		m_IpBook;
	CHostArray		m_HostsAll;

	USER_MODE		m_eUserMode;
	BOOL			m_bFloppyExportOnlySupervisor;

	CMinimizedDlg*	m_pMinimizedDlg;

	CString			m_sSystemCodePage;
//	BYTE			m_byCP1252To1250[256];
	BYTE			m_byCP1250To1252[256];

	int				m_nAlarmCallEvent;
	int				m_nAlarmCallEventByCardType;
	friend class	CDVUIThread;
	friend class	CMinimizedDlg;
	friend class	CAlarmlistDlg;

	int      m_nAudioRecordingMode;
	int      m_nAudioCameraNumber;
	BOOL     m_bValueOn;
	
	BOOL		m_bImagesToBackup;  //Bytes zum Auslagern vorhanden
	BOOL		m_bTooManyImages;	//schlug Berechnung Endzeitpunkt fehl (timeout) und anschliessende
									//benutzereingabe Endzeitpunkt ergab zuviele Images -> TRUE
	BOOL		m_bCalculateError;	//True, wenn es beim CalculateBackupEnd ein timeout gab
	CSystemTime m_stFirstDBImage;
	CString		m_sFirstDBImage;	//Datum/Zeit erstes Bild beim backup auf CD der ausgewählten Kameras
	CString		m_sLastCalcDBImage;	//Datum/Zeit letztes Bild der Endzeitpunkt Berechnung
	CString		m_sMultiBackupStart;//Datum/Zeit des Endzeitpunktes einer vorangegangenen Auslagerung
									//wird als Startzeitpunkt bei "Auslagerung fortsetzen" benutzt
	CEvent m_evCalculateEnd;		//wird gesetzt, wenn der Endzeitpunkt der Auslagerung berechnet wurde
	BOOL   m_bCalculateEndComplete;	//wird gesetzt, wenn der Endzeitpunkt der Auslagerung berechnet wurde
	DWORD  m_dwTimeoutCalculateEnd; //timeout für das Warten auf Endzeitberechnung
	CEvent m_evCalculateMBUserBackupEndOK;	 //wird gesetzt, wenn die MB`s zwischen Start und UserEnd 
											 //berechnet wurden
	CEvent m_evCalculateMBUserBackupEndFALSE;	//wird gesetzt, wenn keine MB`s zwischen Start und 
												//UserEnd gefunden wurden
	BOOL   m_bCanRectSearch;		//In Abhängigkeit der jeweilige installierten Server-Version 
									//wird die Rechtecksuche vom Konstruktor des DisplayWindows aus
									//freigeschaltet, ab Version 1.4.1

	//Members für Multibackup
	CSystemTime m_stMultiBackupStartLast;	//Begin des letzten Backups 
	CSystemTime m_stMultiBackupEndLast;		//Ende des letzten Backups == Start des nächsten 
	CSystemTime m_stMultiBackupStartNew;	//Start des neuen (multi)  Backups
	CString		m_sMultiBackupCamsSeperated;//alle BackupKameras, durch ',' getrennt
	WORD		m_wMultiBackupCamLast;		//KameraNr des Endzeitpunktes des letzten Backups
	WORD		m_wMultiBackupLastSequence;	//SequenceNr des Endzeitpunktes des letzten Backups
	DWORD		m_dwMultiBackupLastRecord;	//RecordNr des Endzeitpunktes des letzten Backups
	CEvent		m_evLastBackupRecordOK;		//wird aus CIPCDatabaseDVClient::OnConfirmNewQueryResult()
											//gesetzt, wenn nächstes Image hinter 
											//letztem Backup-Ende in DB vorhanden ist
	CEvent		m_evLastBackupRecordFALSE;	//wird aus CIPCDatabaseDVClient::OnConfirmQuery() gesetzt
											//wenn kein Image gefunden wurde
	BOOL		m_bMultiBackup;				//TRUE, wenn Auslagerung fortgesetzt werden soll
	BOOL		m_bAlarmBackup;				//TRUE, wenn ein Alarm Archiv ausgelagert werden soll

	MEDIUMTYPE	m_MedienType;				//welches Medium beim Backup mit ACDC wurde
											//im Laufwerk gefunden
	BOOL	m_bIsExternDriveEmpty;
	DWORD 	m_dwFreeMBExternalDrive;
	DWORD	m_dwTotalMBExternalDrive;
	
	WORD	m_wNextSyncPlayWnd;				//beim synchronen Abspielen die nächste zu spielende Kamera

	int				m_nTimerCounter;		// Performance Management
public:
};
/////////////////////////////////////////////////////////////////////////////////
inline OEM CPanel::GetOem() const
{
	return m_eOem;
}
//////////////////////////////////////////////////////////////////////////////////
inline CString CPanel::GetCurrentLanguage() const
{
	return m_sCurrentLanguage;
}
//////////////////////////////////////////////////////////////////////////////////
inline int CPanel::GetNrOfCameras() const
{
	return m_nCameras;
}
//////////////////////////////////////////////////////////////////////////////////
inline BOOL	CPanel::IsSupervisor() const
{
	return m_eUserMode == UM_SUPERVISOR;
}
//////////////////////////////////////////////////////////////////////////////////
inline BOOL	CPanel::IsOperator() const
{
	return m_eUserMode == UM_OPERATOR;
}
//////////////////////////////////////////////////////////////////////////////////
inline BOOL	CPanel::IsMinimized() const
{
	return m_pMinimizedDlg != NULL;
}
/////////////////////////////////////////////////////////////////////////////
inline CString CPanel::GetHostName()
{
	return m_sHostName;
}
/////////////////////////////////////////////////////////////////////////////
inline CString CPanel::GetHostNumber()
{
	return m_sHostAddress;
}
/////////////////////////////////////////////////////////////////////////////
inline CMDConfigDlg* CPanel::GetMDConfigDialog()
{
	return m_pMDConfigDlg;
}
//////////////////////////////////////////////////////////////////////////////////
inline CMinimizedDlg* CPanel::GetMinimizedDlg()
{
	return m_pMinimizedDlg;
}
//////////////////////////////////////////////////////////////////////////////////
inline CLangDllInfo* CPanel::GetLangDllInfo()
{
	return m_pDefaultLanguage;
}
//////////////////////////////////////////////////////////////////////////////////
inline BOOL CPanel::IsConfigMenu()
{
	BOOL bRet = (m_pOSD == &m_OSDConfig);
	return bRet;
}
//////////////////////////////////////////////////////////////////////////////////
inline BOOL CPanel::IsNormalMenu()
{
	BOOL bRet = (m_pOSD == &m_OSDNormal);
	return bRet;
}


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CPANEL_H__40CB1163_82C2_11D3_8D9D_004305A11E32__INCLUDED_)
