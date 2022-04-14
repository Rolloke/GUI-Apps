/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaUnit
// FILE:		$Workfile: TashaUnitDlg.cpp $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaUnit/TashaUnitDlg.cpp $
// CHECKIN:		$Date: 11.02.05 9:04 $
// VERSION:		$Revision: 134 $
// LAST CHANGE:	$Modtime: 9.02.05 12:26 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <direct.h>
#include "User.h"
#include "Permission.h"
#include <Skins/Skins.h> // Skins before OemGui!
#include "oemgui\oemlogindialog.h"
#include "WK_Msg.h"
#include "TashaUnit.h"
#include "TashaUnitDlg.h"
#include "TashaUnitDefs.h"
#include "CMyTasha.h"
#include "CipcInputTashaUnit.h"
#include "CipcInputTashaUnitMDAlarm.h"
#include "CipcOutputTashaUnit.h"
#include "VideoInlayWindow.h"
#include "AnalogVideoDlg.h"
#include "EncoderParameterDlg.h"
#include "oemgui\oemguiapi.h"
#include "tashaunitdlg.h"
#include "cprogressdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CTashaUnitApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About


/////////////////////////////////////////////////////////////////////////////
// CTashaUnitDlg dialog

/////////////////////////////////////////////////////////////////////////////
CTashaUnitDlg::CTashaUnitDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTashaUnitDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTashaUnitDlg)
	m_sBrightness = _T("50");
	m_sContrast = _T("50");
	m_sSaturation = _T("50");
	m_bMultiView = TRUE;
	m_bFrameSync = TRUE;
	m_bNoiseReduction = FALSE;
	m_bShowAdaptiveMask = FALSE;
	m_bShowPermanentMask = FALSE;
	m_bShowDiffImage = FALSE;
	m_sMaskTreshold = _T("50");
	m_sMDTreshold = _T("50");
	m_sMaskDelay = _T("50");
	m_sMaskIncrement = _T("50");
	m_nRegisterNumber = 0;
	m_nRegisterValue = 0;
	m_bSaveToDisk = FALSE;
	m_bEnableNR	  = FALSE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32

	m_pCodec				= NULL;
	m_pPrewiewWnd1			= NULL;
	m_pPrewiewWnd2			= NULL;
	m_pAnalogDlg			= NULL;
	m_pEncoderParameterDlg	= NULL;
	m_pProgressDlg			= NULL;
	m_bPowerLED				= FALSE;
	m_bResetLED				= FALSE;
	m_nActiveRefreshRate	= 2;
	m_nIdleRefreshRate		= 8;
	m_bShowOnlyIFrames		= TRUE;
	m_bEnableBF533Recovery	= TRUE;
	m_bEnableBF535Recovery	= TRUE;
	m_bEnableStreamRecovery = TRUE;

	// Pfad innerhalb der Registry bestimmen.
	CString sIniFile	= _T("TashaUnit\\Device");
	_TCHAR szBuffer[256];

	m_sIniFile.Format(sIniFile + _T("%u"), theApp.GetCardRef());

	CString sJpegPath;
	sJpegPath.Format(_T("E:\\Jpeg%u\\"), theApp.GetCardRef());

	CString sStreamPath;
	sStreamPath.Format(_T("E:\\Stream%u\\"), theApp.GetCardRef());

	//m_bStandalone = 0-> SaVicUnit arbeitet mit Security, 1-> SaVicUnit ist Standalone
	m_bStandalone = ReadConfigurationInt(_T("Debug"), _T("Mode"),  0, m_sIniFile);
	if (m_bStandalone)
		WK_TRACE(_T("Running as 'standalone'\n"));
	    
	m_bSaveInLog = ReadConfigurationInt(_T("Debug"), _T("SaveInLog"),  0, m_sIniFile);
	if (m_bSaveInLog)
		WK_TRACE(_T("Jpeg saved in log'\n"));
	
	// Pfad zum Speichern der Streams einlesen
	ReadConfigurationString(_T("Debug"), _T("StreamPath"), sStreamPath, szBuffer,  255, m_sIniFile); 
	m_sStreamPath = szBuffer;

	m_nActiveRefreshRate	= ReadConfigurationInt(_T("General"), _T("ActiveRefreshRate"), m_nActiveRefreshRate, m_sIniFile); 
	m_nIdleRefreshRate		= ReadConfigurationInt(_T("General"), _T("IdleRefreshRate"),	m_nIdleRefreshRate, m_sIniFile); 
	m_bShowOnlyIFrames		= ReadConfigurationInt(_T("General"), _T("ShowOnlyIFrames"),	m_bShowOnlyIFrames, m_sIniFile); 
	m_bEnableBF533Recovery	= ReadConfigurationInt(_T("Recovery"), _T("EnableBF533Recovery"), m_bEnableBF533Recovery, m_sIniFile);
	m_bEnableBF535Recovery	= ReadConfigurationInt(_T("Recovery"), _T("EnableBF535Recovery"), m_bEnableBF535Recovery, m_sIniFile);
	m_bEnableStreamRecovery = ReadConfigurationInt(_T("Recovery"), _T("EnableStreamRecovery"), m_bEnableStreamRecovery, m_sIniFile);
	
	WK_TRACE(_T("BF533Recovery is %s\n"),  m_bEnableBF533Recovery ? _T("enabled.") : _T("dispabled"));
	WK_TRACE(_T("BF535Recovery is %s\n"),  m_bEnableBF535Recovery ? _T("enabled.") : _T("dispabled"));
	WK_TRACE(_T("StreamRecovery is %s\n"), m_bEnableStreamRecovery ? _T("enabled."): _T("dispabled"));

	if (m_bStandalone)
		_tmkdir(m_sStreamPath);

	m_hIconEnabled	= AfxGetApp()->LoadIcon(IDI_ICON_ENABLED);
	m_hIconDisabled	= AfxGetApp()->LoadIcon(IDI_ICON_DISABLED);
	m_hIconActivity	= AfxGetApp()->LoadIcon(IDI_ICON_ACTIVITY);
	m_hIconFailure	= AfxGetApp()->LoadIcon(IDI_ICON_FAILURE);

	// Standardeinstellungen laden.
	m_bMultiView			= ReadConfigurationInt(_T("General"), _T("MultiView"),			1, m_sIniFile);
	m_bShowAdaptiveMask		= ReadConfigurationInt(_T("General"), _T("ShowAdaptiveMask"),	1, m_sIniFile);
	m_bShowPermanentMask	= ReadConfigurationInt(_T("General"), _T("ShowPermanentMask"),  1, m_sIniFile);
	m_bShowDiffImage		= ReadConfigurationInt(_T("General"), _T("ShowDiffImage"),		1, m_sIniFile);
	m_wSource				= ReadConfigurationInt(_T("General"), _T("DefaultCam"),			0, m_sIniFile);
	m_dwEncodedFrames		= 0;
	m_dwStartTime			= 0;
	m_dwRelayStateMask		= 0;
	m_dwAlarmMask			= 0;
	m_dwAdapterSelectMask	= 0;
	m_dwPCKeysMask			= 0;

	m_pOutputTashaCamera	= NULL;
	m_pOutputTashaRelay		= NULL;
	m_pInput				= NULL;
	m_pInputMDAlarm			= NULL;
}

/////////////////////////////////////////////////////////////////////////////
CTashaUnitDlg::~CTashaUnitDlg()
{
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::PostNcDestroy() 
{
	WK_TRACE(_T("PostNcDestroy\n"));
	delete this;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::Create()
{
	if (!CDialog::Create(IDD))
	{
		return FALSE;
	}
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTashaUnitDlg)
	DDX_Control(pDX, IDC_SCROLLBAR_MD_TRESHOLD, m_ctrlMDTreshold);
	DDX_Control(pDX, IDC_SCROLLBAR_MASK_TRESHOLD, m_ctrlMaskTreshold);
	DDX_Control(pDX, IDC_SCROLLBAR_MASK_INCREMENT, m_ctrlMaskIncrement);
	DDX_Control(pDX, IDC_SCROLLBAR_MASK_DELAY, m_ctrlMaskDelay);
	DDX_Control(pDX, IDC_SCROLLBAR_BRIGHTNESS, m_ctrlBrightness);
	DDX_Control(pDX, IDC_SCROLLBAR_CONTRAST, m_ctrlContrast);
	DDX_Control(pDX, IDC_SCROLLBAR_SATURATION, m_ctrlSaturation);
	DDX_Text(pDX, IDC_STATIC_BRIGTNESS, m_sBrightness);
	DDV_MaxChars(pDX, m_sBrightness, 3);
	DDX_Text(pDX, IDC_STATIC_CONTRAST, m_sContrast);
	DDV_MaxChars(pDX, m_sContrast, 3);
	DDX_Text(pDX, IDC_STATIC_SATURATION, m_sSaturation);
	DDV_MaxChars(pDX, m_sSaturation, 3);
	DDX_Check(pDX, IDC_CHECK_MULTY_VIEW, m_bMultiView);
	DDX_Check(pDX, IDC_CHECK_FRAME_SYNC, m_bFrameSync);
	DDX_Check(pDX, IDC_CHECK_NOISEREDUCTION, m_bNoiseReduction);
	DDX_Check(pDX, IDC_CHECK_SHOW_ADAPTIVE_MASK, m_bShowAdaptiveMask);
	DDX_Check(pDX, IDC_CHECK_SHOW_PERMANENT_MASK, m_bShowPermanentMask);
	DDX_Check(pDX, IDC_CHECK_SHOW_DIFF_IMAGE, m_bShowDiffImage);
	DDX_Text(pDX, IDC_STATIC_MASK_TRESHOLD, m_sMaskTreshold);
	DDV_MaxChars(pDX, m_sMaskTreshold, 3);
	DDX_Text(pDX, IDC_STATIC_MD_TRESHOLD, m_sMDTreshold);
	DDV_MaxChars(pDX, m_sMDTreshold, 3);
	DDX_Text(pDX, IDC_STATIC_MASK_DELAY, m_sMaskDelay);
	DDV_MaxChars(pDX, m_sMaskDelay, 3);
	DDX_Text(pDX, IDC_STATIC_MASK_INCREMENT, m_sMaskIncrement);
	DDV_MaxChars(pDX, m_sMaskIncrement, 3);
	DDX_Text(pDX, IDC_EDIT_REGISTER, m_nRegisterNumber);
	DDV_MinMaxInt(pDX, m_nRegisterNumber, 0, 255);
	DDX_Text(pDX, IDC_EDIT_REGISTER_VALUE, m_nRegisterValue);
	DDV_MinMaxInt(pDX, m_nRegisterValue, 0, 65535);
	DDX_Check(pDX, IDC_CHECK_SAVE_TO_DISK, m_bSaveToDisk);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTashaUnitDlg, CDialog)
	//{{AFX_MSG_MAP(CTashaUnitDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_PLAY, OnResumeCapture)
	ON_BN_CLICKED(IDC_PAUSE, OnPauseCapture)
	ON_BN_CLICKED(IDC_STOP, OnStopCapture)
	ON_BN_CLICKED(IDC_START, OnStartCapture)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_RELAIS0, OnRelais0)
	ON_BN_CLICKED(IDC_RELAIS1, OnRelais1)
	ON_BN_CLICKED(IDC_RELAIS2, OnRelais2)
	ON_BN_CLICKED(IDC_RELAIS3, OnRelais3)
	ON_BN_CLICKED(IDC_RADIO_CAM0, OnCheckCam0)
	ON_BN_CLICKED(IDC_RADIO_CAM1, OnCheckCam1)
	ON_BN_CLICKED(IDC_RADIO_CAM2, OnCheckCam2)
	ON_BN_CLICKED(IDC_RADIO_CAM3, OnCheckCam3)
	ON_BN_CLICKED(IDC_RADIO_CAM4, OnCheckCam4)
	ON_BN_CLICKED(IDC_RADIO_CAM5, OnCheckCam5)
	ON_BN_CLICKED(IDC_RADIO_CAM6, OnCheckCam6)
	ON_BN_CLICKED(IDC_RADIO_CAM7, OnCheckCam7)
	ON_BN_CLICKED(IDC_PREVIEW1, OnPreview1)
	ON_BN_CLICKED(IDC_PREVIEW2, OnPreview2)
	ON_BN_CLICKED(IDC_CHECK_MULTY_VIEW, OnCheckMultyView)
	ON_BN_CLICKED(IDC_DOWNLOAD_ALTERA, OnDownloadEPLD)
	ON_BN_CLICKED(IDC_CHECK_FRAME_SYNC, OnCheckFrameSync)
	ON_BN_CLICKED(IDC_CHECK_NOISEREDUCTION, OnCheckNoisereduction)
	ON_BN_CLICKED(IDC_CHECK_SHOW_ADAPTIVE_MASK, OnCheckShowAdaptiveMask)
	ON_BN_CLICKED(IDC_DOWNLOAD_EEPROM, OnDownloadEEEProm)
	ON_BN_CLICKED(IDC_CHECK_SHOW_DIFF_IMAGE, OnCheckShowDiffImage)
	ON_BN_CLICKED(IDC_SAVE_EEPROM, OnSaveEEProm)
	ON_BN_CLICKED(IDC_RESET_BF533, OnResetBF533)
	ON_BN_CLICKED(IDC_CHECK_SHOW_PERMANENT_MASK, OnCheckShowPermanentMask)
	ON_BN_CLICKED(IDC_CLEAR_MASK, OnClearPermanentMask)
	ON_BN_CLICKED(IDC_INVERT_MASK, OnInvertPermanentMask)
	ON_BN_CLICKED(IDC_CROSSPOINT, OnCrosspointDlg)
	ON_BN_CLICKED(IDC_SET_REG, OnSetRegister)
	ON_BN_CLICKED(IDC_CHECK_POWER_LED, OnSetPowerLED)
	ON_BN_CLICKED(IDC_CHECK_RESET_LED, OnSetResetLED)
	ON_BN_CLICKED(IDC_CHECK_SAVE_TO_DISK, OnCheckSaveToDisk)
	ON_WM_CANCELMODE()
	ON_BN_CLICKED(IDC_ENCODERPARAM, OnEncoderParamDlg)
	ON_MESSAGE(WM_TRAYCLICKED, OnTrayClicked)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_UPDATE_DIALOG,OnUpdateDialog)
	ON_MESSAGE(WM_ON_INDICATION_NO_BF533_RESPONSE, OnIndicationNoBF533Response)
	ON_MESSAGE(WM_ON_INDICATION_NO_BF535_RESPONSE, OnIndicationNoBF535Response)
	ON_MESSAGE(WM_ON_INDICATION_VIDEOSTREAM_ABORTED, OnIndicationVideoStreamAborted)
	ON_MESSAGE(WM_ON_INDICATION_VIDEOSTREAM_RESUMED, OnIndicationVideoStreamResumed)
	ON_MESSAGE(WM_ON_CONFIRM_OPEN_DEVICE, OnConfirmOpenDevice)
	ON_MESSAGE(WM_SELFCHECK, DoConfirmSelfcheck)
	ON_MESSAGE(WM_POWERBROADCAST, OnWmPowerBroadcast)
	ON_MESSAGE(WM_QUERYENDSESSION, OnWmQueryEndSession)
	ON_COMMAND(ID_FILE_ABOUT, OnFileAbout)
	ON_COMMAND(ID_FILE_SETTINGS, OnFileSettings)
	ON_COMMAND(ID_APP_EXIT, OnFileExit)
	ON_BN_CLICKED(IDC_SET_PERM_MASK, OnBnClickedSetPermMask)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTashaUnitDlg message handlers

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
long CTashaUnitDlg::DoConfirmSelfcheck(WPARAM wParam, LPARAM /*lParam*/)
{
	HWND hWnd = (HWND)wParam;

	if (hWnd && IsWindow(hWnd))
		::PostMessage(hWnd, WM_SELFCHECK, theApp.GetApplicationId(), 0);

	return 0;
}
	
/////////////////////////////////////////////////////////////////////////////
int CTashaUnitDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Dialog solange sperren, bis alle DSPs gebootet sind
	EnableWindow(FALSE);

	m_pProgressDlg			= new CProgressDlg(this);
	
	m_pCodec = new CMyTasha(this, theApp.GetCardRef(), m_sIniFile);

	if (!m_pCodec)
		return -1;

	m_pCodec->DoRequestOpenDevice();

	if (!m_bStandalone)
	{
		// Icon ins Systemtray
		NOTIFYICONDATA tnd;

		tnd.cbSize		= sizeof(NOTIFYICONDATA);
		tnd.hWnd		= m_hWnd;
		tnd.uID			= 1;

		tnd.uFlags		= NIF_MESSAGE|NIF_ICON|NIF_TIP;
		tnd.uCallbackMessage = WM_TRAYCLICKED;
		tnd.hIcon		= m_hIconDisabled;

		CString sTip = COemGuiApi::GetApplicationName(theApp.GetApplicationId());
		lstrcpyn(tnd.szTip, sTip, sTip.GetLength()+1);
		Shell_NotifyIcon(NIM_ADD, &tnd);
		ShowWindow(SW_HIDE);
	}

	return 0;
}

///////////////////// ////////////////////////////////////////////////////////
void CTashaUnitDlg::OnDestroy() 
{
	WK_TRACE(_T("CTashaUnitDlg::OnDestroy()\n"));

	CDialog::OnDestroy();

	OnStopCapture();

	Sleep(100);
	KillTimer(1);

	if (m_pCodec)
		m_pCodec->Close();

	// Threads beenden.
	WK_DELETE(m_pPrewiewWnd1);
	WK_DELETE(m_pPrewiewWnd2);
	WK_DELETE(m_pInput);
	WK_DELETE(m_pInputMDAlarm);
	WK_DELETE(m_pOutputTashaCamera);
	WK_DELETE(m_pOutputTashaRelay);
	WK_DELETE(m_pAnalogDlg);
	WK_DELETE(m_pEncoderParameterDlg);
	WK_DELETE(m_pProgressDlg);
	WK_DELETE(m_pCodec);

	if (!m_bStandalone)
	{
		// Nur erforderlich, wenn Icon in der Systemtaskleiste
		NOTIFYICONDATA tnd;

		tnd.cbSize		= sizeof(NOTIFYICONDATA);
		tnd.hWnd		= m_hWnd;
		tnd.uID			= 1;
		tnd.uFlags		= 0;

		Shell_NotifyIcon(NIM_DELETE, &tnd);
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIconDisabled, TRUE);			// Set big icon
	SetIcon(m_hIconDisabled, FALSE);		// Set small icon

	if (theApp.GetCardRef() == 1)
		SetWindowText(WK_APP_NAME_TASHAUNIT1);
	else if (theApp.GetCardRef() == 2)
		SetWindowText(WK_APP_NAME_TASHAUNIT2);
	else if (theApp.GetCardRef() == 3)
		SetWindowText(WK_APP_NAME_TASHAUNIT3);
	else if (theApp.GetCardRef() == 4)
		SetWindowText(WK_APP_NAME_TASHAUNIT4);
	else
		WK_TRACE_ERROR(_T("Unknown Applicationname\n"));

	m_ctrlBrightness.SetRange(0, 100);
	m_ctrlContrast.SetRange(0, 100);
	m_ctrlSaturation.SetRange(0, 100);
	
	m_ctrlMDTreshold.SetRange(0, 100);
	m_ctrlMaskTreshold.SetRange(0, 100);
	m_ctrlMaskIncrement.SetRange(0, 100);
	m_ctrlMaskDelay.SetRange(0, 100);

	// Alarm/AdapterSelect und PC-Key Status korrekt anzeigen
	OnIndicationAlarmStateChanged(m_dwAlarmMask);
	OnIndicationAdapterSelectStateChanged(m_dwAdapterSelectMask);
	OnIndicationPCKeysStateChanged(m_dwPCKeysMask);

	if (m_bStandalone)
	{
		CenterWindow();
		ShowWindow(SW_SHOW);
	}
	else
	{
		CenterWindow();
		ShowWindow(SW_SHOW);
		ShowWindow(SW_MINIMIZE);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::SetTashaIcon(WORD wColor, BOOL bType)
{
	static WORD wCurColor	= TASHA_ICON_NOCOLOR;
	static BOOL bCurType	= FALSE;

	if ((wColor != wCurColor) || (bType != bCurType))
	{
		wCurColor = wColor;
	
		if (m_bStandalone)
		{
			switch (wColor)
			{
				case TASHA_ICON_ACTIVITY:
					if (m_hIconActivity)
						::PostMessage(m_hWnd, WM_SETICON,	bType, (LPARAM)m_hIconActivity);
					break;
				case TASHA_ICON_FAILURE:
					if (m_hIconFailure)
						::PostMessage(m_hWnd, WM_SETICON,	bType, (LPARAM)m_hIconFailure);
					break;
				case TASHA_ICON_ENABLED:
					if (m_hIconEnabled)
						::PostMessage(m_hWnd, WM_SETICON,	bType, (LPARAM)m_hIconEnabled);
					break;
				case TASHA_ICON_DISABLED:
					if (m_hIconDisabled)
						::PostMessage(m_hWnd, WM_SETICON,	bType, (LPARAM)m_hIconDisabled);
					break;
				default:
					WK_TRACE(_T("Nicht definiertes Icon\n"));
			}
		}
		else
		{
			// Icon ins Systemtray
			NOTIFYICONDATA tnd;

			tnd.cbSize		= sizeof(NOTIFYICONDATA);
			tnd.hWnd		= m_hWnd;
			tnd.uID			= 1;

			tnd.uFlags		= NIF_MESSAGE|NIF_ICON|NIF_TIP;
			tnd.uCallbackMessage = WM_TRAYCLICKED;
			tnd.hIcon		= m_hIconEnabled;

			CString sTip = COemGuiApi::GetApplicationName(theApp.GetApplicationId());
			float fFps = (1000.0* (float)(m_dwEncodedFrames % 1000)) / (float)(GetTickCount() - m_dwStartTime);
			CString sText;

			sText.Format(_T(" (%.02ffps)"), fFps);
			sTip += sText;
			lstrcpyn(tnd.szTip, sTip, sTip.GetLength()+1);

			switch (wColor)
			{
				case TASHA_ICON_ACTIVITY:
					tnd.hIcon =	m_hIconActivity;
					break;
				case TASHA_ICON_FAILURE:
					tnd.hIcon =	m_hIconFailure;
					break;
				case TASHA_ICON_ENABLED:
					tnd.hIcon =	m_hIconEnabled;
					break;
				case TASHA_ICON_DISABLED:
					tnd.hIcon =	m_hIconDisabled;
					break;
				default:
					WK_TRACE(_T("Nicht definiertes Icon\n"));
			}
			Shell_NotifyIcon(NIM_MODIFY, &tnd);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnTimer(UINT nIDEvent)
{
	static DWORD dwLastTimer = WK_GetTickCount();
	if (nIDEvent == 1)
	{
		if (m_pCodec)
		{
			// Im Standalone selber den Watchdog triggern, in DTS und Idip
			// erfolgt dies vom DVStarter und Launcher.
			if (m_bStandalone)
			{
				// Alle 20 Sekunden muß der Watchdog getriggert werden.
				if (WK_GetTimeSpan(dwLastTimer) > 20000)
				{
					dwLastTimer = WK_GetTickCount();
					m_pCodec->DoRequestTriggerWatchdog(60); // Timeout auf 60 Sekunden
				}
			}
			// Testet im Sekundentakt, ob der BF535 noch arbeitet.
			m_pCodec->DoRequestBF535Ping();
		}
	}
}

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmGetEncoderParam(DWORD dwUserData, WORD wSource, EncoderParam EncParam)
{
	if (m_pEncoderParameterDlg)
		m_pEncoderParameterDlg->OnConfirmGetEncoderParam(dwUserData, wSource, EncParam);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmGetBrightness(DWORD dwUserData, WORD wSource, int nValue)
{	
	if (wSource == m_wSource)
	{
		m_sBrightness.Format(_T("%d"), nValue);
		UpdateDialog();
	}

	if (m_pOutputTashaCamera)
		m_pOutputTashaCamera->OnConfirmGetBrightness(dwUserData, wSource, nValue);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmGetContrast(DWORD dwUserData, WORD wSource, int nValue)
{
	if (wSource == m_wSource)
	{
		m_sContrast.Format(_T("%d"), nValue);
		UpdateDialog();
	}

	if (m_pOutputTashaCamera)
		m_pOutputTashaCamera->OnConfirmGetContrast(dwUserData, wSource, nValue);
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmGetSaturation(DWORD dwUserData, WORD wSource, int nValue)
{
	if (wSource == m_wSource)
	{
		m_sSaturation.Format(_T("%d"), nValue);
		UpdateDialog();
	}

	if (m_pOutputTashaCamera)
		m_pOutputTashaCamera->OnConfirmGetSaturation(dwUserData, wSource, nValue);

	return TRUE;
}


//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmGetMDTreshold(DWORD dwUserData, WORD wSource, int nValue)
{
	if (wSource == m_wSource)
	{
		m_sMDTreshold.Format(_T("%d"), nValue);
		UpdateDialog();
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmGetMaskTreshold(DWORD dwUserData, WORD wSource, int nValue)
{
	if (wSource == m_wSource)
	{
		m_sMaskTreshold.Format(_T("%d"), nValue);
		UpdateDialog();
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmGetMaskIncrement(DWORD dwUserData, WORD wSource, int nValue)
{
	if (wSource == m_wSource)
	{
		m_sMaskIncrement.Format(_T("%d"), nValue);
		UpdateDialog();
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmGetMaskDelay(DWORD dwUserData, WORD wSource, int nValue)
{
	if (wSource == m_wSource)
	{
		m_sMaskDelay.Format(_T("%d"), nValue);
		UpdateDialog();
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmChangePermanentMask(DWORD dwUserData, WORD wSource, int nX, int nY, int nValue)
{
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmSetEncoderParam(DWORD dwUserData, WORD wSource, EncoderParam EncParam)
{
	if (m_pEncoderParameterDlg)
		m_pEncoderParameterDlg->OnConfirmSetEncoderParam(dwUserData, wSource, EncParam);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmSetBrightness(DWORD dwUserData, WORD wSource, int nValue)
{	
	if (wSource == m_wSource)
	{
		m_sBrightness.Format(_T("%d"), nValue);
		UpdateDialog();
	}

	if (m_pOutputTashaCamera)
		m_pOutputTashaCamera->OnConfirmSetBrightness(dwUserData, wSource, nValue);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmSetContrast(DWORD dwUserData, WORD wSource, int nValue)
{
	if (wSource == m_wSource)
	{
		m_sContrast.Format(_T("%d"), nValue);
		UpdateDialog();
	}
	
	if (m_pOutputTashaCamera)
		m_pOutputTashaCamera->OnConfirmSetContrast(dwUserData, wSource, nValue);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmSetSaturation(DWORD dwUserData, WORD wSource, int nValue)
{
	if (wSource == m_wSource)
	{
		m_sSaturation.Format(_T("%d"), nValue);
		UpdateDialog();
	}

	if (m_pOutputTashaCamera)
		m_pOutputTashaCamera->OnConfirmSetSaturation(dwUserData, wSource, nValue);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmSetMDMaskSensitivity(DWORD dwUserData, WORD wSource, const CString &sLevel)
{
	if (m_pOutputTashaCamera)
		m_pOutputTashaCamera->OnConfirmSetMDMaskSensitivity(dwUserData, wSource, sLevel);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmGetMDMaskSensitivity(DWORD dwUserData, WORD wSource, CString &sLevel)
{
	if (m_pOutputTashaCamera)
		m_pOutputTashaCamera->OnConfirmGetMDMaskSensitivity(dwUserData, wSource, sLevel);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmSetMDAlarmSensitivity(DWORD dwUserData, WORD wSource, const CString &sLevel)
{
	if (m_pOutputTashaCamera)
		m_pOutputTashaCamera->OnConfirmSetMDAlarmSensitivity(dwUserData, wSource, sLevel);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmGetMDAlarmSensitivity(DWORD dwUserData, WORD wSource, CString &sLevel)
{
	if (m_pOutputTashaCamera)
		m_pOutputTashaCamera->OnConfirmGetMDAlarmSensitivity(dwUserData, wSource, sLevel);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmSetMDTreshold(DWORD dwUserData, WORD wSource, int nValue)
{	
	if (wSource == m_wSource)
	{
		m_sMDTreshold.Format(_T("%d"), nValue);
		UpdateDialog();
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmSetMaskTreshold(DWORD dwUserData, WORD wSource, int nValue)
{	
	if (wSource == m_wSource)
	{
		m_sMaskTreshold.Format(_T("%d"), nValue);
		UpdateDialog();
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmSetMaskIncrement(DWORD dwUserData, WORD wSource, int nValue)
{	
	if (wSource == m_wSource)
	{
		m_sMaskIncrement.Format(_T("%d"), nValue);
		UpdateDialog();
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmSetMaskDelay(DWORD dwUserData, WORD wSource, int nValue)
{	
	if (wSource == m_wSource)
	{
		m_sMaskDelay.Format(_T("%d"), nValue);
		UpdateDialog();
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmClearPermanentMask(DWORD dwUserData, WORD wSource)
{
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmInvertPermanentMask(DWORD dwUserData, WORD wSource)
{
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmResumeCapture()
{
	UpdateDialog();

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmPauseCapture() 
{
	UpdateDialog();

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmStopCapture() 
{
	UpdateDialog();

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmStartCapture() 
{
	m_dwEncodedFrames		= 0;

	m_dwStartTime = GetTickCount();
	UpdateDialog();

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmSetChannel(WORD wSource, WORD wNewSource)
{
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmSetAnalogOut(DWORD dwCrosspointMask)
{
	if (m_pAnalogDlg)
		m_pAnalogDlg->OnConfirmSetAnalogOut(dwCrosspointMask);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmGetAnalogOut(DWORD dwCrosspointMask)
{
	if (m_pAnalogDlg)
		m_pAnalogDlg->OnConfirmGetAnalogOut(dwCrosspointMask);

	return TRUE;
}	

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmSetTerminationState(BYTE byTermMask)
{
	if (m_pAnalogDlg)
		m_pAnalogDlg->OnConfirmSetTerminationState(byTermMask);
	if (m_pOutputTashaCamera)
		m_pOutputTashaCamera->OnConfirmSetTerminationState(byTermMask);

	return TRUE;
}	

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmGetTerminationState(BYTE byTermMask)
{
	if (m_pAnalogDlg)
		m_pAnalogDlg->OnConfirmGetTerminationState(byTermMask);
	if (m_pOutputTashaCamera)
		m_pOutputTashaCamera->OnConfirmGetTerminationState(byTermMask);

	return TRUE;
}	

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmSetVideoEnableState(BOOL bState)
{
	if (m_pAnalogDlg)
		m_pAnalogDlg->OnConfirmSetVideoEnableState(bState);

	return TRUE;
}	

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmGetVideoEnableState(BOOL bState)
{
	if (m_pAnalogDlg)
		m_pAnalogDlg->OnConfirmGetVideoEnableState(bState);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmSetCrosspointEnableState(BOOL bState)
{
	if (m_pAnalogDlg)
		m_pAnalogDlg->OnConfirmSetCrosspointEnableState(bState);

	return TRUE;
}	

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmGetCrosspointEnableState(BOOL bState)
{
	if (m_pAnalogDlg)
		m_pAnalogDlg->OnConfirmGetCrosspointEnableState(bState);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmSetPowerLED(BOOL bState)
{
	m_bPowerLED = bState;

	UpdateDialog();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmSetResetLED(BOOL bState)
{
	m_bResetLED = bState;

	UpdateDialog();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmSetNR(DWORD dwUserData, WORD wSource, BOOL bEnable)
{
	if (wSource == m_wSource)
	{
		m_bEnableNR = bEnable;
		UpdateDialog();
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmGetNR(DWORD dwUserData, WORD wSource, BOOL bEnable)
{
	if (wSource == m_wSource)
	{
		m_bEnableNR = bEnable;
		UpdateDialog();
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
		OnFileAbout();
	else if ((nID == SC_MINIMIZE) && (m_bStandalone == FALSE))
		ShowWindow(SW_HIDE);
	else
		CDialog::OnSysCommand(nID, lParam);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTashaUnitDlg::OnPaint() 
{
	if (IsIconic())
	{
		if (m_bStandalone == FALSE)
			ShowWindow(SW_HIDE);

		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIconEnabled);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
/////////////////////////////////////////////////////////////////////////////
HCURSOR CTashaUnitDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIconEnabled;
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnResumeCapture() 
{
	if (m_pCodec)
		m_pCodec->DoRequestResumeCapture();
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnPauseCapture() 
{
	if (m_pCodec)
		m_pCodec->DoRequestPauseCapture();
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnStopCapture() 
{
	if (m_pCodec)
		m_pCodec->DoRequestStopCapture();
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnStartCapture() 
{
	if (m_pCodec)
		m_pCodec->DoRequestStartCapture();
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{ 
	CSliderCtrl* pSlider = (CSliderCtrl*)pScrollBar;

	// Get the current position of scroll box.
	int curpos = pSlider->GetPos();

//	TRACE(_T("OnHScroll (%d, %d) SBCode=%d\n"), curpos, nPos, nSBCode);
	if (nSBCode != SB_ENDSCROLL)
	{
		if (pScrollBar->GetDlgCtrlID() == IDC_SCROLLBAR_BRIGHTNESS)
		{
			if (m_pCodec)
				m_pCodec->DoRequestSetBrightness(0, m_wSource, curpos);
		}
		else if (pScrollBar->GetDlgCtrlID() == IDC_SCROLLBAR_CONTRAST)
		{
			if (m_pCodec)
				m_pCodec->DoRequestSetContrast(0, m_wSource, curpos);
		}
		else if (pScrollBar->GetDlgCtrlID() == IDC_SCROLLBAR_SATURATION)
		{
			if (m_pCodec)
				m_pCodec->DoRequestSetSaturation(0, m_wSource, curpos);
		}
		else if (pScrollBar->GetDlgCtrlID() == IDC_SCROLLBAR_MD_TRESHOLD)
		{
			if (m_pCodec)
				m_pCodec->DoRequestSetMDTreshold(0, m_wSource, curpos);
		}
		else if (pScrollBar->GetDlgCtrlID() == IDC_SCROLLBAR_MASK_TRESHOLD)
		{									   
			if (m_pCodec)
				m_pCodec->DoRequestSetMaskTreshold(0, m_wSource, curpos);
		}
		else if (pScrollBar->GetDlgCtrlID() == IDC_SCROLLBAR_MASK_INCREMENT)
		{									   
			if (m_pCodec)
				m_pCodec->DoRequestSetMaskIncrement(0, m_wSource, curpos);
		}
		else if (pScrollBar->GetDlgCtrlID() == IDC_SCROLLBAR_MASK_DELAY)
		{									   
			if (m_pCodec)
				m_pCodec->DoRequestSetMaskDelay(0, m_wSource, curpos);
		}
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnClearPermanentMask() 
{
	if (m_pCodec)
		m_pCodec->DoRequestClearPermanentMask(0, m_wSource);
	
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnInvertPermanentMask() 
{
	if (m_pCodec)
		m_pCodec->DoRequestInvertPermanentMask(0, m_wSource);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnReceiveNewCodecData(DATA_PACKET *pPacket)
{
	BOOL bResult = FALSE;
	static DWORD dwCounter = 0;
/*
if (pPacket->dwStreamID == 1)
{	
	if (pPacket->eSubType == eSubIFrame)
		WK_TRACE(_T("OnReceiveNewCodecData Source=%d Stream=%d I-Frame\n"), pPacket->wSource, pPacket->dwStreamID);
	else
		WK_TRACE(_T("OnReceiveNewCodecData Source=%d Stream=%d P-Frame\n"), pPacket->wSource, pPacket->dwStreamID);
}
*/
	// Gültiges Bild?
	if (!pPacket)
		return FALSE;

	if (pPacket->wSource >= CHANNEL_COUNT)
		return FALSE;

	// Tray-Icon blinken lassen
	if (dwCounter++ == 100)
		SetTashaIcon(TASHA_ICON_ACTIVITY);
	else if (dwCounter++ == 200)
	{
		SetTashaIcon(TASHA_ICON_ENABLED);
		dwCounter = 0;
	}

	if (m_bMultiView)
	{
		// Um die Prozessorauslastung nicht ans Limit zu fahren, wird im MultiView-Mode
		// Nur jedes 6. Bild dekodiert und angezeigt.
		// Das aktive Fenster und bei Fenstern mit Bewegung hingegen jedes 3. Bild.
		static DWORD dwCounter[MAX_STREAMS][CHANNEL_COUNT] = {0};
		BOOL bDecode = FALSE;

		if (pPacket->wSource == m_wSource)
			bDecode = TRUE;
		else  if (pPacket->bMotion)
		{
			if (pPacket->eSubType == eSubIFrame)
			{
				dwCounter[pPacket->dwStreamID][pPacket->wSource]++;
				bDecode = ((dwCounter[pPacket->dwStreamID][pPacket->wSource] % m_nActiveRefreshRate) == 0);
			}
		}
		else
		{
			if (pPacket->eSubType == eSubIFrame)
			{
				dwCounter[pPacket->dwStreamID][pPacket->wSource]++;
				bDecode = ((dwCounter[pPacket->dwStreamID][pPacket->wSource] % m_nIdleRefreshRate) == 0);
			}
		}

		m_csMpegDecoder.Lock();
		if (pPacket->dwStreamID == 0)
		{
			if (m_pPrewiewWnd1 && bDecode)
				m_pPrewiewWnd1->DrawImage(pPacket, m_bShowAdaptiveMask, m_bShowPermanentMask, m_bShowDiffImage);	
		}
		else
		{
			if (m_pPrewiewWnd2 && bDecode)
				m_pPrewiewWnd2->DrawImage(pPacket, m_bShowAdaptiveMask, m_bShowPermanentMask, m_bShowDiffImage);	
		}
		m_csMpegDecoder.Unlock();
	}
	else
	{
		m_csMpegDecoder.Lock();
		if (pPacket->dwStreamID == 0)
		{
			if (m_pPrewiewWnd1 && (pPacket->wSource == m_wSource))
				m_pPrewiewWnd1->DrawImage(pPacket, m_bShowAdaptiveMask, m_bShowPermanentMask, m_bShowDiffImage);	
		}
		else
		{
			if (m_pPrewiewWnd2 && (pPacket->wSource == m_wSource))
				m_pPrewiewWnd2->DrawImage(pPacket, m_bShowAdaptiveMask, m_bShowPermanentMask, m_bShowDiffImage);	
		}
		m_csMpegDecoder.Unlock();
	}
	
	if (m_bSaveToDisk && pPacket->bMotion)
	{
		if (pPacket->eType == eTypeJpeg)
			SaveJpegFile(pPacket);
		else if (pPacket->eType == eTypeMpeg4)
			SaveMPEGStream(pPacket);
	}

	bResult = TRUE;

	if (!m_bStandalone)		   
	{
		// Daten an die Outputgroup übergeben.
		if (m_pOutputTashaCamera)
			m_pOutputTashaCamera->IndicationDataReceived(pPacket);

		// Bewegungsalarme melden.
		if (pPacket->bMotion)
		{
			if (m_pInputMDAlarm)
			{
				m_pInputMDAlarm->OnReceivedMotionAlarm(pPacket, TRUE);
				m_pInputMDAlarm->OnReceivedMotionAlarm(pPacket, FALSE);
			}
		}
	}

	m_dwEncodedFrames++;
	if ((m_dwEncodedFrames % 1000) == 0)
		m_dwStartTime = GetTickCount();

	if (!IsIconic())
		UpdateDialog();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnIndicationCameraStatusChanged(WORD wSource, BOOL bVideoState)
{
	BOOL bResult = FALSE;

	WK_TRACE(_T("CameraStatus: Source=%d, Status=%d\n"), wSource, bVideoState);

	if (bVideoState == FALSE)
		SetTashaIcon(TASHA_ICON_FAILURE);
	
	// Statusänderung dem Server mitteilen
	if (m_pOutputTashaCamera)
	{
		m_pOutputTashaCamera->OnIndicationVideoStateChanged(wSource, bVideoState);
		bResult = TRUE;
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnIndicationAlarmStateChanged(DWORD dwAlarmMask)
{
//	WK_TRACE(_T("OnIndicationAlarm dwAlarmMask=0x%x\n"), dwAlarmMask);
	m_dwAlarmMask = dwAlarmMask;

	// Post an update message.
	UpdateDialog();

	if (m_pInput)
		m_pInput->AlarmStateChanged(dwAlarmMask);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnIndicationAdapterSelectStateChanged(DWORD dwAdapterSelectMask)
{
//	WK_TRACE(_T("OnIndicationAdapterSelectStateChanged dwAdapterSelectMask=0x%x\n"), dwAdapterSelectMask);
	m_dwAdapterSelectMask = dwAdapterSelectMask;

	// Post an update message.
	UpdateDialog();

	if (m_pInput)
		m_pInput->AdapterSelectStateChanged(dwAdapterSelectMask);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnIndicationPCKeysStateChanged(DWORD dwPCKeysMask)
{
//	WK_TRACE(_T("OnIndicationPCKeysStateChanged dwPCKeysMask=0x%x\n"), dwPCKeysMask);
	m_dwPCKeysMask = dwPCKeysMask;

	// Post an update message.
	UpdateDialog();
	
	if (m_pInput)
		m_pInput->PCKeysStateChanged(dwPCKeysMask);

	// Ist der 'SoftPower'-Button gedrückt? dann melde es DVClient
	if (TSTBIT(dwPCKeysMask, 1))
	{
		WK_TRACE(_T("Tasha SoftPowerOff-Button pressed\n"));
		MessageBeep(0);
		CWnd* pWnd = FindWindow(_T("DVClient"), NULL);
		if (WK_IS_WINDOW(pWnd))
   			pWnd->PostMessage(WM_POWEROFF_PRESSED, 0, 0);
	}
	
	// Ist der 'SoftReset'-Button gedrückt? dann melde es DVClient
	if (TSTBIT(dwPCKeysMask, 2))
	{
	#if (0)
		WK_TRACE(_T("Reset-Button pressed\n"));
		MessageBeep(0);
		CWnd* pWnd = FindWindow(_T("DVClient"), NULL);
		if (WK_IS_WINDOW(pWnd))
   			pWnd->PostMessage(WM_RESET_PRESSED, 0, 0);
	#else
			if (m_pOutputTashaCamera)
				m_pOutputTashaCamera->OnResetButtonPressed();

	#endif
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::SaveJpegFile(DATA_PACKET* pPacket)
{
	BOOL bResult = FALSE;
	if (pPacket)
	{
		BYTE* pBuffer = (BYTE*)&pPacket->pImageData;
		if (pBuffer)
		{
			CString s;
			s.Format(_T("%sCamera%d\\J_%06lu.jpg\0"),m_sStreamPath, pPacket->wSource+1, m_dwEncodedFrames);

			CFile File(s,	CFile::modeCreate | CFile::modeWrite  |
							CFile::typeBinary | CFile::shareDenyWrite);

			File.Write(pBuffer, pPacket->dwImageDataLen); 
			bResult = TRUE;
		}
	}
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::SaveMPEGStream(DATA_PACKET* pPacket)
{
	BOOL bResult = FALSE;
	if (pPacket)
	{
		BYTE* pBuffer = (BYTE*)&pPacket->pImageData;
		if (pBuffer)
		{
			CString s;
			s.Format(_T("%sStream%d_%d.mp4\0"),m_sStreamPath, pPacket->wSource, pPacket->dwStreamID);

			CFile File;

			bResult = File.Open(s, CFile::modeWrite  |	CFile::typeBinary | CFile::shareDenyWrite);
			
			// Das File konnte nicht geöffnet werden -> es existiert existiert nicht
			if (!bResult)
			{
				if (pPacket->eSubType == eSubIFrame)
					bResult = File.Open(s, CFile::modeCreate | CFile::modeWrite  |	CFile::typeBinary | CFile::shareDenyWrite);
			}

			if (bResult)
			{	
				try
				{
					File.SeekToEnd();
					File.Write(pBuffer, pPacket->dwImageDataLen); 
				}
				catch(...)
				{
					WK_TRACE(_T("CTashaUnitDlg::SaveMPEGStream File operation throws an exception, maybe disk full \n"));
					m_bSaveToDisk = FALSE;
					UpdateDialog();		
				}
			}
		}
	}
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmScanForCameras(DWORD dwUserData, DWORD dwCameraMask)
{
	if (m_pOutputTashaCamera)
		m_pOutputTashaCamera->OnConfirmScanForCameras(dwUserData, dwCameraMask);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmSetRelayState(WORD wRelayID, BOOL bOpenClose)
{
	if (m_pOutputTashaRelay)
		m_pOutputTashaRelay->OnConfirmSetRelayState(wRelayID, bOpenClose);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmGetRelayState(DWORD dwRelayStateMask)
{
	m_dwRelayStateMask = dwRelayStateMask;
	m_evRelayStateConfirm.SetEvent();

	if (m_pOutputTashaRelay)
		m_pOutputTashaRelay->OnConfirmGetRelayState(dwRelayStateMask);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmGetAlarmState(DWORD dwAlarmStateMask)
{
	if (m_pInput)
		m_pInput->OnConfirmAlarmState(dwAlarmStateMask);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmSetAlarmEdge(DWORD dwAlarmEdge)
{
	if (m_pInput)
		m_pInput->OnConfirmSetEdge(dwAlarmEdge);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmGetAlarmEdge(DWORD dwAlarmEdge)
{
	if (m_pInput)
		m_pInput->OnConfirmGetEdge(dwAlarmEdge);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
long CTashaUnitDlg::OnIndicationVideoStreamAborted(WPARAM wParam, LPARAM lParam)
{
	WORD wSource = wParam;

	SetTashaIcon(TASHA_ICON_FAILURE);

	// Statusänderung dem Server mitteilen (11.08.04: Nicht mehr notwendig)
//	if (m_pOutputTashaCamera)
//		m_pOutputTashaCamera->OnIndicationVideoStreamStateChanged(wSource, FALSE);

	if (m_bEnableStreamRecovery)
	{
		WK_TRACE(_T("Videostream from Source %d is aborted. Autoreboot is enabled by registry....try to reboot slave %d\n"),wSource, wSource);
		
		if (m_pCodec)
			m_pCodec->TransferBF533BootData(wSource);
	}	
	else
		WK_TRACE(_T("Videostream from Source %d is aborted. Autoreboot is disabled by registry\n"), wSource);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
long CTashaUnitDlg::OnIndicationVideoStreamResumed(WPARAM wParam, LPARAM lParam)
{
//	WORD wSource = wParam;

	// Statusänderung dem Server mitteilen (11.08.04: Nicht mehr notwendig)
//	if (m_pOutputTashaCamera)
//		m_pOutputTashaCamera->OnIndicationVideoStreamStateChanged(wSource, TRUE);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
long CTashaUnitDlg::OnIndicationNoBF533Response(WPARAM wParam, LPARAM lParam)
{
	WORD wSource = wParam;

	SetTashaIcon(TASHA_ICON_FAILURE);

	if (m_bEnableBF533Recovery)
	{
		WK_TRACE(_T("Slave %d is not responding. Autoreboot is enabled by registry....try to reboot slave %d\n"),wSource, wSource);

		if (m_pCodec)
			m_pCodec->TransferBF533BootData(wSource);
	}	
	else
		WK_TRACE(_T("Slave %d is not responding. Autoreboot is disabled by registry\n"), wSource);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
long CTashaUnitDlg::OnIndicationNoBF535Response(WPARAM wParam, LPARAM lParam)
{
	SetTashaIcon(TASHA_ICON_FAILURE);

	if (m_bEnableBF535Recovery)
	{
		WK_TRACE(_T("BF535 is not responding. Autoreboot is enabled by registry....try to reboot BF535\n"));

		if (m_pCodec)
		{
			KillTimer(1);
			m_pCodec->DoRequestStopCapture();
			m_pCodec->Close();
			m_pCodec->DoRequestOpenDevice();
		}
	}	
	else
		WK_TRACE(_T("BF535 is not responding. Autoreboot is disabled by registry\n"));

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnNotifySlaveInitReady(WORD wSource)
{
	BOOL bResult = FALSE;
	static double fPercent = 0;
	static WORD   wLastSource = (WORD)-1;

	// Nur wenn wirklich ein neuer DSP gebootet wurde, die Prozentanzeige erhöhen.
	if (wSource != wLastSource)
		fPercent += 100.0/(float)CHANNEL_COUNT ;

	// Bootprocess Fortschritt anzeigen.
	if (m_pProgressDlg)
		m_pProgressDlg->SetProgress((int)(fPercent+0.5), wSource);

	if (m_pCodec)
		bResult = m_pCodec->LoadConfig(wSource);

	// Statusänderung dem Server mitteilen
	if (m_pOutputTashaCamera)
		m_pOutputTashaCamera->OnIndicationSlaveInitReady(wSource);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
long CTashaUnitDlg::OnConfirmOpenDevice(WPARAM wParam, LPARAM lParam)
{
	// Progresscontrol entfernen.
	WK_DELETE(m_pProgressDlg);

	int nBoardID= (int)wParam;
	int nErrors = (int)lParam;

	// Sind während des Bootprocesses Fehler aufgetreten?
	if (nErrors == 0)
	{
		// Dongle auslesen
		if (m_pCodec)
			m_pCodec->ReadDongleInformation();

		EnableWindow(TRUE);
		SetTashaIcon(TASHA_ICON_ENABLED);

		if (!m_pAnalogDlg)
			m_pAnalogDlg = new CAnalogVideoDlg(this, m_pCodec);
		
		if (!m_pEncoderParameterDlg)
			m_pEncoderParameterDlg	= new CEncoderParameterDlg(this, m_pCodec);

		SetTimer(1, 1000, NULL);

		// Codecs gleich starten...
		m_pCodec->DoRequestStartCapture();

		// Die CIPC-Schnittstelle braucht im Standalonebetrieb nicht aufgebaut zu werde
		if (!m_bStandalone) 
		{
			CString sSMName;
			CString sCardRefNr;
			if (nBoardID != 1)
				sCardRefNr.Format(_T("%i"), nBoardID);
			
			// die CIPC-Schnittstellen aufbauen.
			sSMName = SM_TASHA_INPUT;
			sSMName += sCardRefNr;
			WK_DELETE(m_pInput);
			m_pInput = new CIPCInputTashaUnit(this, m_pCodec, sSMName);
			if (!m_pInput)
				WK_TRACE_ERROR(_T("CTashaUnitDlg::Create\tCIPCInputTashaUnit-Objekt konnte nicht angelegt werden.\n"));

			sSMName = SM_TASHA_MD_INPUT;
			sSMName += sCardRefNr;
			WK_DELETE(m_pInputMDAlarm);
			m_pInputMDAlarm = new CIPCInputTashaUnitMDAlarm(this, m_pCodec, sSMName);
			if (!m_pInputMDAlarm)
				WK_TRACE_ERROR(_T("CTashaUnitDlg::Create\tCIPCInputTashaUnitMDAlarm-Objekt konnte nicht angelegt werden.\n"));
			
			sSMName = SM_TASHA_OUTPUT_CAMERAS;
			sSMName += sCardRefNr;
			WK_DELETE(m_pOutputTashaCamera);
			m_pOutputTashaCamera = new CIPCOutputTashaUnit(this, m_pCodec, OUTPUT_TYPE_CAMERA, sSMName);
			if (!m_pOutputTashaCamera)
				WK_TRACE_ERROR(_T("CTashaUnitDlg::Create\tCIPCOutputTashaUnit-TashaCamera-Objekt konnte nicht angelegt werden.\n"));
			
			sSMName = SM_TASHA_OUTPUT_RELAYS;
			sSMName += sCardRefNr;
			WK_DELETE(m_pOutputTashaRelay);
			m_pOutputTashaRelay = new CIPCOutputTashaUnit(this, m_pCodec, OUTPUT_TYPE_RELAY, sSMName);
			if (!m_pOutputTashaRelay)
				WK_TRACE_ERROR(_T("CTashaUnitDlg::Create\tCIPCOutputTashaUnit-Relay-Objekt konnte nicht angelegt werden.\n"));	
		}
	}
	else // Es sind Fehler aufgetreten....Programm beenden.
		OnOK();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::UpdateDialog()
{
	PostMessage(WM_UPDATE_DIALOG);
}

/////////////////////////////////////////////////////////////////////////////
long CTashaUnitDlg::OnUpdateDialog(WPARAM wParam, LPARAM lParam)
{
//	if (!IsWindowEnabled())
//		return 0;

	// Alarme anzeigen
	for (int nI = 0; nI < 8; nI++)
	{
		CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_ALARM0+nI);
		if (pButton)
			pButton->SetCheck(TSTBIT(m_dwAlarmMask, nI));
	}

	// PC-Keys und Adapterselect anzeigen
	for (nI = 0; nI < 4; nI++)
	{
		CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_ADAPTER_SELECT0+nI);
		if (pButton)
			pButton->SetCheck(TSTBIT(m_dwAdapterSelectMask, nI));

		pButton = (CButton*)GetDlgItem(IDC_CHECK_PC_KEYS0+nI);
		if (pButton)
			pButton->SetCheck(TSTBIT(m_dwPCKeysMask, nI));
	}

	CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_NOISEREDUCTION);
	if (pButton)
		pButton->SetCheck(m_bEnableNR);


	switch (m_pCodec->GetEncoderState())
	{
		case eEncoderStateUnvalid:
			GetDlgItem(IDC_START)->EnableWindow(TRUE);
			GetDlgItem(IDC_STOP)->EnableWindow(FALSE);
			GetDlgItem(IDC_PAUSE)->EnableWindow(FALSE);
			GetDlgItem(IDC_PLAY)->EnableWindow(FALSE);
			break;
		case eEncoderStateOn:
			GetDlgItem(IDC_START)->EnableWindow(FALSE);
			GetDlgItem(IDC_STOP)->EnableWindow(TRUE);
			GetDlgItem(IDC_PAUSE)->EnableWindow(TRUE);
			GetDlgItem(IDC_PLAY)->EnableWindow(FALSE);
			break;
		case eEncoderStateOff:
			GetDlgItem(IDC_START)->EnableWindow(TRUE);
			GetDlgItem(IDC_STOP)->EnableWindow(FALSE);
			GetDlgItem(IDC_PAUSE)->EnableWindow(FALSE);
			GetDlgItem(IDC_PLAY)->EnableWindow(FALSE);
			break;
		case eEncoderStatePause:
			GetDlgItem(IDC_START)->EnableWindow(FALSE);
			GetDlgItem(IDC_STOP)->EnableWindow(TRUE);
			GetDlgItem(IDC_PAUSE)->EnableWindow(FALSE);
			GetDlgItem(IDC_PLAY)->EnableWindow(TRUE);
			break;

		default:
			WK_TRACE_WARNING(_T("Unknown Encoderstate (%d)\n"), m_pCodec->GetEncoderState());
	}

	switch (m_wSource)
	{
		case VIDEO_SOURCE0:
			CheckRadioButton(IDC_RADIO_CAM0, IDC_RADIO_CAM7, IDC_RADIO_CAM0);
			break;
		case VIDEO_SOURCE1:
			CheckRadioButton(IDC_RADIO_CAM0, IDC_RADIO_CAM7, IDC_RADIO_CAM1);
			break;
		case VIDEO_SOURCE2:
			CheckRadioButton(IDC_RADIO_CAM0, IDC_RADIO_CAM7, IDC_RADIO_CAM2);
			break;
		case VIDEO_SOURCE3:
			CheckRadioButton(IDC_RADIO_CAM0, IDC_RADIO_CAM7, IDC_RADIO_CAM3);
			break;										  				   
		case VIDEO_SOURCE4:
			CheckRadioButton(IDC_RADIO_CAM0, IDC_RADIO_CAM7, IDC_RADIO_CAM4);
			break;
		case VIDEO_SOURCE5:
			CheckRadioButton(IDC_RADIO_CAM0, IDC_RADIO_CAM7, IDC_RADIO_CAM5);
			break;
		case VIDEO_SOURCE6:
			CheckRadioButton(IDC_RADIO_CAM0, IDC_RADIO_CAM7, IDC_RADIO_CAM6);
			break;
		case VIDEO_SOURCE7:
			CheckRadioButton(IDC_RADIO_CAM0, IDC_RADIO_CAM7, IDC_RADIO_CAM7);
			break;
	}

	GetDlgItem(IDC_STATIC_BRIGTNESS)->SetWindowText(m_sBrightness);
	GetDlgItem(IDC_STATIC_CONTRAST)->SetWindowText(m_sContrast);
	GetDlgItem(IDC_STATIC_SATURATION)->SetWindowText(m_sSaturation);

	m_ctrlBrightness.SetPos(_ttoi(m_sBrightness));
	m_ctrlContrast.SetPos(_ttoi(m_sContrast));
	m_ctrlSaturation.SetPos(_ttoi(m_sSaturation));

	GetDlgItem(IDC_STATIC_MD_TRESHOLD)->SetWindowText(m_sMDTreshold);
	GetDlgItem(IDC_STATIC_MASK_TRESHOLD)->SetWindowText(m_sMaskTreshold);
	GetDlgItem(IDC_STATIC_MASK_INCREMENT)->SetWindowText(m_sMaskIncrement);
	GetDlgItem(IDC_STATIC_MASK_DELAY)->SetWindowText(m_sMaskDelay);

	m_ctrlMDTreshold.SetPos(_ttoi(m_sMDTreshold));
	m_ctrlMaskTreshold.SetPos(_ttoi(m_sMaskTreshold));
	m_ctrlMaskIncrement.SetPos(_ttoi(m_sMaskIncrement));
	m_ctrlMaskDelay.SetPos(_ttoi(m_sMaskDelay));

	CheckDlgButton(IDC_CHECK_SAVE_TO_DISK, m_bSaveToDisk);

	if (m_pCodec->GetEncoderState() == eEncoderStateOn)
	{
		if ((GetTickCount() - m_dwStartTime) != 0)
		{
			float fFps = (1000.0* (float)(m_dwEncodedFrames % 1000)) / (float)(GetTickCount() - m_dwStartTime);
			CString sText;

			sText.Format(_T("%.02f"), fFps);
			GetDlgItem(IDC_STATIC_FPS)->SetWindowText(sText);	

			sText.Format(_T("%lu"), m_dwEncodedFrames);
			GetDlgItem(IDC_STATIC_FRAME)->SetWindowText(sText);	
		}
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnRelais0() 
{
	if (m_pCodec)
	{
		m_evRelayStateConfirm.ResetEvent();
		m_pCodec->DoRequestGetRelayState();
		if (WaitForSingleObject(m_evRelayStateConfirm, 1000) == WAIT_OBJECT_0)
			m_pCodec->DoRequestSetRelayState(0, !TSTBIT(m_dwRelayStateMask, 0));
		else
			WK_TRACE_ERROR(_T("DoRequestGetRelayState timeout\n"));
	}
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnRelais1() 
{
	if (m_pCodec)
	{
		m_evRelayStateConfirm.ResetEvent();
		m_pCodec->DoRequestGetRelayState();
		if (WaitForSingleObject(m_evRelayStateConfirm, 1000) == WAIT_OBJECT_0)
			m_pCodec->DoRequestSetRelayState(1, !TSTBIT(m_dwRelayStateMask, 1));
		else
			WK_TRACE_ERROR(_T("DoRequestGetRelayState timeout\n"));
	}
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnRelais2() 
{
	if (m_pCodec)
	{
		m_evRelayStateConfirm.ResetEvent();
		m_pCodec->DoRequestGetRelayState();
		if (WaitForSingleObject(m_evRelayStateConfirm, 1000) == WAIT_OBJECT_0)
			m_pCodec->DoRequestSetRelayState(2, !TSTBIT(m_dwRelayStateMask, 2));
		else
			WK_TRACE_ERROR(_T("DoRequestGetRelayState timeout\n"));
	}
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnRelais3() 
{
	if (m_pCodec)
	{
		m_evRelayStateConfirm.ResetEvent();
		m_pCodec->DoRequestGetRelayState();
		if (WaitForSingleObject(m_evRelayStateConfirm, 1000) == WAIT_OBJECT_0)
			m_pCodec->DoRequestSetRelayState(3, !TSTBIT(m_dwRelayStateMask, 3));
		else
			WK_TRACE_ERROR(_T("DoRequestGetRelayState timeout\n"));
	}
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnCheckFrameSync() 
{
	UpdateData();

	if (m_pCodec)
		m_pCodec->EnableFrameSync(m_bFrameSync);

	if (m_bFrameSync && m_pPrewiewWnd1)
	{
		Sleep(10);
		m_pPrewiewWnd1->InvalidateRect(NULL, TRUE);
	}
	if (m_bFrameSync && m_pPrewiewWnd2)
	{
		Sleep(10);
		m_pPrewiewWnd2->InvalidateRect(NULL, TRUE);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnCheckCam0() 
{
	OnCheckCam(0);	
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnCheckCam1() 
{
	OnCheckCam(1);	
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnCheckCam2() 
{
	OnCheckCam(2);
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnCheckCam3() 
{
	OnCheckCam(3);	
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnCheckCam4() 
{
	OnCheckCam(4);	
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnCheckCam5() 
{
	OnCheckCam(5);	
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnCheckCam6() 
{
	OnCheckCam(6);	
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnCheckCam7() 
{
	OnCheckCam(7);	
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnCheckCam(int nCamNr) 
{
	if (m_pCodec)
	{
		if (m_wSource != nCamNr)
		{
			m_wSource = nCamNr;
			m_pCodec->DoRequestGetBrightness(0, nCamNr);
			m_pCodec->DoRequestGetContrast(0, nCamNr);
			m_pCodec->DoRequestGetSaturation(0, nCamNr);
			m_pCodec->DoRequestGetMDTreshold(0, nCamNr);
			m_pCodec->DoRequestGetMaskTreshold(0, nCamNr);
			m_pCodec->DoRequestGetMaskIncrement(0, nCamNr);
			m_pCodec->DoRequestGetMaskDelay(0, nCamNr);
			m_pCodec->DoRequestGetEncoderParam(0, nCamNr, 0);
			m_pCodec->DoRequestGetNR(0, nCamNr);
		}
	}
	UpdateDialog();

	WriteConfigurationInt(_T("General"), _T("DefaultCam"), m_wSource, m_sIniFile);
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnPreview1() 
{
	if (m_pPrewiewWnd1 == NULL)
	{
		CRect rect(250, 20, 250+MAX_HSIZE/2, 20+MAX_VSIZE/2);

		m_pPrewiewWnd1 = new CVideoInlayWindow(m_pCodec, rect, TRUE);
		if (m_pPrewiewWnd1)
		{
			OnCheckMultyView();
			OnStartCapture();
		}
	}
	else
	{
		m_csMpegDecoder.Lock();
		WK_DELETE(m_pPrewiewWnd1);
		m_csMpegDecoder.Unlock();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnPreview2() 
{
	if (m_pPrewiewWnd2 == NULL)
	{
		CRect rect(250+MAX_HSIZE/2, 20, 250+MAX_HSIZE, 20+MAX_VSIZE/2);

		m_pPrewiewWnd2 = new CVideoInlayWindow(m_pCodec, rect, TRUE);
	
		if (m_pPrewiewWnd2)
		{
			OnCheckMultyView();
			OnStartCapture();
		}
	}
	else
	{
		m_csMpegDecoder.Lock();
		WK_DELETE(m_pPrewiewWnd2);
		m_csMpegDecoder.Unlock();
	}

}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnCheckMultyView() 
{
	UpdateData();
	if (m_pPrewiewWnd1)
		m_pPrewiewWnd1->SetPreviewMode(m_bMultiView);
	
	if (m_pPrewiewWnd2)
		m_pPrewiewWnd2->SetPreviewMode(m_bMultiView);

	WriteConfigurationInt(_T("General"), _T("MultiView"), m_bMultiView, m_sIniFile);
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnDownloadEPLD() 
{
	CString sFilename;

	if (SelectFile(sFilename, _T("*.rbf")))
	{
		GetDlgItem(IDC_DOWNLOAD_ALTERA)->EnableWindow(FALSE);
		if (m_pCodec)
			m_pCodec->TransferEPLDdata(sFilename);
		GetDlgItem(IDC_DOWNLOAD_ALTERA)->EnableWindow(TRUE);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnDownloadEEEProm() 
{
	CString sFilename;

	if (SelectFile(sFilename, _T("*.ldr")))
	{
		GetDlgItem(IDC_DOWNLOAD_EEPROM)->EnableWindow(FALSE);
		if (m_pCodec)
			m_pCodec->WriteEEPromData(sFilename);
		GetDlgItem(IDC_DOWNLOAD_EEPROM)->EnableWindow(TRUE);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnSaveEEProm() 
{
	CString sFilename;

	if (SelectFile(sFilename, _T("*.ldr")))
	{
		GetDlgItem(IDC_SAVE_EEPROM)->EnableWindow(FALSE);
		if (m_pCodec)
			m_pCodec->ReadEEPromData(sFilename);
		GetDlgItem(IDC_SAVE_EEPROM)->EnableWindow(TRUE);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnResetBF533() 
{
	GetDlgItem(IDC_RESET_BF533)->EnableWindow(FALSE);

	if (m_pCodec)
		m_pCodec->TransferBF533BootData(m_wSource);

	GetDlgItem(IDC_RESET_BF533)->EnableWindow(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::SelectFile(CString &sFilename, const CString& sExt)
{
	OPENFILENAME  ofn;
	static TCHAR  szDirName[256]  = {_T('\0')};                 // Startdirectory
	static TCHAR  szFileName[256] = {0};                    // Filename + Pfad
	TCHAR         szFileTitle[256];                         // Dateiname
	TCHAR		  szFilter[] = {_T("EEProm Image (*.ldr)\0*.ldr\0Altera Image (*.rbf)\0*.rbf\0\0\0")};

	//strcpy(szFileName, sExt);
	lstrcpyn(szFileName, sExt,sExt.GetLength()+1);

	/*** OPENFILENAME-Struktur ausfüllen ***/
	ofn.lStructSize       = sizeof(OPENFILENAME);
	ofn.hwndOwner         = (HWND) NULL;
	ofn.hInstance         = (HINSTANCE) AfxGetInstanceHandle();
	ofn.lpstrFilter       = &szFilter[0];
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter    = 0L;
	ofn.nFilterIndex      = 0L;
	ofn.lpstrFile         = szFileName;                    // Filename mit Pfad
	ofn.nMaxFile          = 256;                   
	ofn.lpstrFileTitle    = szFileTitle;                   // Filename ohne Pfad
	ofn.nMaxFileTitle     = sizeof(szFileTitle);
	ofn.lpstrInitialDir   = szDirName;                     // Startverzeichnis
	ofn.lpstrTitle        = _T("Datei auswählen");
	ofn.Flags             = OFN_HIDEREADONLY;
	ofn.nFileOffset       = 0;
	ofn.nFileExtension    = 0;
	ofn.lpstrDefExt       = NULL;
	ofn.lCustData         = 0L;
	//   ofn.lpfnHook          = (FARPROC) NULL;
	ofn.lpTemplateName    = NULL;
	
	/*** Dialog anzeigen ***/
	if (GetOpenFileName(&ofn) == FALSE)
	   return (FALSE);
	
	/*** Verzeichnis für nächsten Aufruf merken ***/
//	strncpy(&szDirName[0], &szFileName[0], ofn.nFileOffset - 1);
	lstrcpyn(szDirName, szFileName,ofn.nFileOffset - 1);

	sFilename = CString(szFileName);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnCheckNoisereduction() 
{
	UpdateData();
	if (m_pCodec)
		m_pCodec->DoRequestSetNR(0, m_wSource, m_bNoiseReduction);
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnCheckShowAdaptiveMask() 
{
	UpdateData();
	WriteConfigurationInt(_T("General"), _T("ShowAdaptiveMask"), m_bShowAdaptiveMask, m_sIniFile);
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnCheckShowPermanentMask() 
{
	UpdateData();
	WriteConfigurationInt(_T("General"), _T("ShowPermanentMask"), m_bShowPermanentMask, m_sIniFile);
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnCheckShowDiffImage() 
{
	UpdateData();
	WriteConfigurationInt(_T("General"), _T("ShowDiffImage"), m_bShowDiffImage, m_sIniFile);
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnOK() 
{
	DestroyWindow();
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CTashaUnitDlg::OnTrayClicked(WPARAM /*wParam*/, LPARAM lParam)
{
	switch (lParam)
	{
		case WM_RBUTTONDOWN:
		{
			CMenu menu;
			CMenu* pM;
			CPoint pt;

			GetCursorPos(&pt);
			menu.LoadMenu(IDR_MENU1);
			pM = menu.GetSubMenu(0);

			SetForegroundWindow();		// Siehe ID: Q135788 
			pM->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this);
			PostMessage(WM_NULL, 0, 0); // Siehe ID: Q135788
			break;
		}
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnCrosspointDlg() 
{
	if (m_pAnalogDlg)
	{
		if (!m_pAnalogDlg->IsVisible())
			m_pAnalogDlg->ShowConfigDlg(m_hWnd);
		else
			m_pAnalogDlg->HideConfigDlg();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnEncoderParamDlg() 
{
	if (m_pEncoderParameterDlg)
	{
		if (!m_pEncoderParameterDlg->IsVisible())
			m_pEncoderParameterDlg->ShowConfigDlg(m_hWnd, m_wSource);
		else
			m_pEncoderParameterDlg->HideConfigDlg();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnSetRegister() 
{
	UpdateData(TRUE);
	if (m_pCodec)
		m_pCodec->DoRequestSetRelayState(m_nRegisterNumber, m_nRegisterValue);
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnSetPowerLED() 
{
	m_bPowerLED = !m_bPowerLED;

	if (m_pCodec)
		m_pCodec->DoRequestSetPowerLED(m_bPowerLED);
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnSetResetLED() 
{
	m_bResetLED = !m_bResetLED;

	if (m_pCodec)
		m_pCodec->DoRequestSetResetLED(m_bResetLED);
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnCheckSaveToDisk() 
{
	UpdateData(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnFileAbout()
{
	COemGuiApi::AboutDialog(this);
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnFileSettings()
{
	if (m_bStandalone || Login())
		ShowWindow(SW_NORMAL);
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnFileExit()
{
	OnOK();
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnBnClickedSetPermMask()
{
	if (m_pCodec)
	{
		GetDlgItem(IDC_SET_PERM_MASK)->EnableWindow(FALSE);
		MASK_STRUCT mask;
		ZeroMemory(&mask, sizeof(MASK_STRUCT));
		mask.bValid = TRUE;
		mask.nDimH  = 45;
		mask.nDimV	= 36;
		mask.byMask[0][0] = 0xff;
		mask.byMask[0][1] = 0x00;
		mask.byMask[0][2] = 0xee;

		mask.byMask[16][22] = 0xff;
		mask.byMask[17][23] = 0xff;
		mask.byMask[18][24] = 0xff;
		mask.bHasChanged = FALSE;

		m_pCodec->DoRequestSetPermanentMask(0, m_wSource, mask, TRUE);
		GetDlgItem(IDC_SET_PERM_MASK)->EnableWindow(TRUE);
	}
}

/////////////////////////////////////////////////////////////////////////////
long CTashaUnitDlg::OnWmPowerBroadcast(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
		case PBT_APMBATTERYLOW:
			WK_TRACE(_T("Battery power is low\n"));
			break;
		case PBT_APMOEMEVENT:
			WK_TRACE(_T("OEM-defined event occurred.\n"));
			break;
		case PBT_APMPOWERSTATUSCHANGE:
			WK_TRACE(_T("Power status has changed.\n"));
			break;
		case PBT_APMQUERYSUSPEND:
			WK_TRACE(_T("Request for permission to suspend.\n"));
			break;
		case PBT_APMQUERYSUSPENDFAILED:
			WK_TRACE(_T("Suspension request denied.\n"));
			break;
		case PBT_APMRESUMEAUTOMATIC:
			WK_TRACE(_T("Operation resuming automatically after event.\n"));
			break;
		case PBT_APMRESUMECRITICAL:
			WK_TRACE(_T("Operation resuming after critical suspension.\n"));
			break;
		case PBT_APMRESUMESUSPEND:
			WK_TRACE(_T("Operation resuming after suspension.\n"));	
			break;
		case PBT_APMSUSPEND:
			WK_TRACE(_T("System is suspending operation.\n"));	
			OnOK();			
			break;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
long CTashaUnitDlg::OnWmQueryEndSession(WPARAM wParam, LPARAM lParam)
{
	WK_TRACE(_T("System is shutting down...\n"));	
	//PostMessage(WM_COMMAND, MAKELONG(BN_CLICKED,0), IDOK);
	OnOK();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::Login()
{
	BOOL bRet = FALSE;			 
	CString sUser;
	CString sPassword;
	CSecID  idUser;
	CUserArray UserArray;
	CWK_Profile prof;

	COEMLoginDialog dlg(this);
										   
	if (IDOK==dlg.DoModal())	  		 
		bRet = TRUE;		 

	return bRet;
}
