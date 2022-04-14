// TashaUnitDlg.cpp : implementation file
//

#include "stdafx.h"
#include <direct.h>
#include "TashaUnit.h"
#include "TashaUnitDlg.h"
#include "CCodec.h"
#include "CipcInputTashaUnit.h"
#include "CipcInputTashaUnitMDAlarm.h"
#include "CipcOutputTashaUnit.h"
#include "VideoInlayWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CTashaUnitApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

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
	m_bShowDiffImage = FALSE;
	m_sMaskTreshold = _T("50");
	m_sMDTreshold = _T("50");
	m_sMaskDelay = _T("50");
	m_sMaskIncrement = _T("50");
	m_bShowPermanentMask = FALSE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32

	// Pfad innerhalb der Registry bestimmen.
	CString sIniFile	= "TashaUnit\\Device";
	_TCHAR szBuffer[256];

	m_sIniFile.Format(sIniFile + _T("%u"), theApp.GetCardRef());

	CString sJpegPath;
	sJpegPath.Format(_T("E:\\Jpeg%u\\"), theApp.GetCardRef());

	CString sMDPath;
	sMDPath.Format(_T("MDMask%u\\"), theApp.GetCardRef());

	// Defaulteinstellungen für die SaVic in die Registry schreiben, wenn nicht vorhanden
	ReadConfigurationString(_T("Debug"),		  _T("JpegPath"),	sJpegPath,	szBuffer, 128,	m_sIniFile);

	if (theApp.GetCardRef() == 1)
		WriteConfigurationInt(_T("General"),	_T("ListenToExtEvent"), 1,	m_sIniFile);
	else
		WriteConfigurationInt(_T("General"),	_T("ListenToExtEvent"), 0,	m_sIniFile);	

	//m_bStandalone = 0-> SaVicUnit arbeitet mit Security, 1-> SaVicUnit ist Standalone
	m_bStandalone = ReadConfigurationInt(_T("Debug"), _T("Mode"),  0, m_sIniFile);
	if (m_bStandalone)
		WK_TRACE(_T("Running as 'standalone'\n"));
	    
	m_bSaveInLog = ReadConfigurationInt(_T("Debug"), _T("SaveInLog"),  0, m_sIniFile);
	if (m_bSaveInLog)
		WK_TRACE(_T("Jpeg saved in log'\n"));
	
	// Pfad zum Speichern der Jpeg-dateien einlesen
	ReadConfigurationString(_T("Debug"), _T("JpegPath"), _T(""), szBuffer,  255, m_sIniFile); 
	m_sJpegPath = szBuffer;

	if (m_bStandalone)
	{
		_tmkdir(m_sJpegPath);
		_tmkdir(m_sJpegPath+_T("\\Camera1"));
		_tmkdir(m_sJpegPath+_T("\\Camera2"));
		_tmkdir(m_sJpegPath+_T("\\Camera3"));
		_tmkdir(m_sJpegPath+_T("\\Camera4"));
	}
	

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
//	m_pPerf1 = new CWK_PerfMon("CommunicationDelay (CTashaUnitDlg)");

	// Standardeinstellungen laden.
	m_bMultiView			= ReadConfigurationInt(_T("General"), _T("MultiView"),			1, m_sIniFile);
	m_bNoiseReduction		= ReadConfigurationInt(_T("General"), _T("NoiseReduction"),		0, m_sIniFile);
	m_bShowAdaptiveMask		= ReadConfigurationInt(_T("General"), _T("ShowAdaptiveMask"),	1, m_sIniFile);
	m_bShowPermanentMask	= ReadConfigurationInt(_T("General"), _T("ShowPermanentMask"),  1, m_sIniFile);
	m_bShowDiffImage		= ReadConfigurationInt(_T("General"), _T("ShowDiffImage"),		1, m_sIniFile);
	m_wSource				= ReadConfigurationInt(_T("General"), _T("DefaultCam"),			0, m_sIniFile);
	m_dwEncodedFrames		= 0;
	m_dwStartTime			= 0;
	m_dwStandbyTime			= 30000;
	m_nTimePerFrame			= 40;
	m_hSyncSemaphore		= NULL;
	m_hShutDownEvent		= NULL;
	m_bRun					= FALSE;
	m_wRelayStateMask		= 0;

	m_pOutputTashaCamera	= NULL;
	m_pOutputTashaRelay		= NULL;
	m_pInput				= NULL;
	m_pInputMDAlarm			= NULL;
	for (int nI = 0; nI < MAX_CAMERAS; nI++)
	{
		m_pCurrentEncodeJob[nI]	= NULL;
		m_dwFieldID[nI]		= 0;
	}

	m_pPollProzessThread	= NULL;
	m_pPrewiewWnd			= NULL;
}

/////////////////////////////////////////////////////////////////////////////
CTashaUnitDlg::~CTashaUnitDlg()
{
	if (m_hShutDownEvent)
	{	
		CloseHandle(m_hShutDownEvent);
		m_hShutDownEvent = NULL;
	}
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
	DDX_Check(pDX, IDC_CHECK_SHOW_DIFF_IMAGE, m_bShowDiffImage);
	DDX_Text(pDX, IDC_STATIC_MASK_TRESHOLD, m_sMaskTreshold);
	DDV_MaxChars(pDX, m_sMaskTreshold, 3);
	DDX_Text(pDX, IDC_STATIC_MD_TRESHOLD, m_sMDTreshold);
	DDV_MaxChars(pDX, m_sMDTreshold, 3);
	DDX_Text(pDX, IDC_STATIC_MASK_DELAY, m_sMaskDelay);
	DDV_MaxChars(pDX, m_sMaskDelay, 3);
	DDX_Text(pDX, IDC_STATIC_MASK_INCREMENT, m_sMaskIncrement);
	DDV_MaxChars(pDX, m_sMaskIncrement, 3);
	DDX_Check(pDX, IDC_CHECK_SHOW_PERMANENT_MASK, m_bShowPermanentMask);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTashaUnitDlg, CDialog)
	//{{AFX_MSG_MAP(CTashaUnitDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
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
	ON_BN_CLICKED(IDC_PREVIEW, OnPreview)
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
	ON_WM_CANCELMODE()
	ON_BN_CLICKED(IDC_INVERT_MASK, OnInvertPermanentMask)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_UPDATE_DIALOG,OnUpdateDialog)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
UINT PollProzessThread(LPVOID pData)
{
	CTashaUnitDlg*	pAppDlg	 = (CTashaUnitDlg*)pData;  // Pointer auf's App-Objekt

	if (!pData)
		return 0;

	// Poll bis Mainprozeß terminiert werden soll.
	while (pAppDlg->m_bRun)
	{
		pAppDlg->PollProzess();	// Prozeßmanagement
	}
	WK_TRACE("HOST: Exit PollProzessManagementThread\n");
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CTashaUnitDlg message handlers

/////////////////////////////////////////////////////////////////////////////
int CTashaUnitDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// Dieses Event veranlaßt das Beenden der einzelnen Threads
	m_hShutDownEvent = CreateEvent(0, TRUE, FALSE, NULL);
	if (!m_hShutDownEvent)
		WK_TRACE_ERROR("Create m_hShutDownEvent failed\n");

	// TODO: Add your specialized creation code here
	m_pCodec = new CCodec(this, theApp.GetCardRef(), m_sIniFile);
	if (!m_pCodec)
		return -1;

	if (!m_pCodec->Open(TASHA_FIRMWARE))
		return -1;

	// Die CIPC-Schnittstelle braucht im Standalonebetrieb nicht aufgebaut zu werde
	if (!m_bStandalone) 
	{
		CString sSMName;
		// die CIPC-Schnittstellen aufbauen.
		sSMName = SM_SAVIC_INPUT;
		if (theApp.GetCardRef() != 1)
			sSMName += ('0'+ theApp.GetCardRef());
		m_pInput = new CIPCInputTashaUnit(this, m_pCodec, sSMName);
		if (!m_pInput)
			WK_TRACE_ERROR("CTashaUnitDlg::Create\tCIPCInputTashaUnit-Objekt konnte nicht angelegt werden.\n");

		sSMName = SM_SAVIC_MD_INPUT;
		if (theApp.GetCardRef() != 1)
			sSMName += ('0'+ theApp.GetCardRef());
		m_pInputMDAlarm = new CIPCInputTashaUnitMDAlarm(this, m_pCodec, sSMName);
		if (!m_pInputMDAlarm)
			WK_TRACE_ERROR("CTashaUnitDlg::Create\tCIPCInputTashaUnitMDAlarm-Objekt konnte nicht angelegt werden.\n");
		
		sSMName = SM_SAVIC_OUTPUT_CAMERAS;
		if (theApp.GetCardRef() != 1)
			sSMName += ('0'+ theApp.GetCardRef());
		
		m_pOutputTashaCamera = new CIPCOutputTashaUnit(this, m_pCodec, OUTPUT_TYPE_CAMERA, sSMName);
		if (!m_pOutputTashaCamera)
			WK_TRACE_ERROR("CTashaUnitDlg::Create\tCIPCOutputTashaUnit-TashaCamera-Objekt konnte nicht angelegt werden.\n");
		
		sSMName = SM_SAVIC_OUTPUT_RELAYS;
		if (theApp.GetCardRef() != 1)
			sSMName += ('0'+ theApp.GetCardRef());

		m_pOutputTashaRelay = new CIPCOutputTashaUnit(this, m_pCodec, OUTPUT_TYPE_RELAY, sSMName);
		if (!m_pOutputTashaRelay)
			WK_TRACE_ERROR("CTashaUnitDlg::Create\tCIPCOutputTashaUnit-Relay-Objekt konnte nicht angelegt werden.\n");	

		
		CString sEventName = "SaVicSyncEvent";
//		if (theApp.GetCardRef() != 1)
//			sEventName += ('0'+ theApp.GetCardRef());

		m_hSyncSemaphore = CreateSemaphore(NULL, 0, 10, sEventName);
		if (m_hSyncSemaphore==NULL) 
		{
			WK_TRACE_ERROR("Failed to create sync semaphore\n");
		}
	}						    

	m_bRun	= TRUE;

	// PollProzessThread anlegen
	m_pPollProzessThread = AfxBeginThread(PollProzessThread, this);
	if (m_pPollProzessThread)
		m_pPollProzessThread->m_bAutoDelete = FALSE;

	return 0;
}

///////////////////// ////////////////////////////////////////////////////////
void CTashaUnitDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	ClearAllJobs();

	// Threads beenden.
	m_bRun = FALSE;

	// ShutDown-Event setzen
	if (m_hShutDownEvent)
		SetEvent(m_hShutDownEvent);
	
	// Warte bis 'PollProzessThread' beendet ist.
	if (m_pPollProzessThread){
		WaitForSingleObject(m_pPollProzessThread->m_hThread, 2000);
	}
	WK_DELETE(m_pPollProzessThread); //Autodelete = FALSE;

	if (m_pCodec)
		m_pCodec->Close();

	WK_DELETE(m_pCodec);


	WK_DELETE(m_pInput);
	WK_DELETE(m_pInputMDAlarm);
	WK_DELETE(m_pOutputTashaCamera);
	WK_DELETE(m_pOutputTashaRelay);

//	WK_DELETE(m_pPerf1);
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
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_ctrlBrightness.SetRange(0, 100);
	m_ctrlContrast.SetRange(0, 100);
	m_ctrlSaturation.SetRange(0, 100);
	
	m_ctrlMDTreshold.SetRange(0, 100);
	m_ctrlMaskTreshold.SetRange(0, 100);
	m_ctrlMaskIncrement.SetRange(0, 100);
	m_ctrlMaskDelay.SetRange(0, 100);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnNotifySlaveInitReady(WORD wSource)
{

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmGetBrightness(DWORD dwUserData, WORD wSource, int nValue)
{	
	if (wSource == m_wSource)
	{
		m_sBrightness.Format("%d", nValue);
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
		m_sContrast.Format("%d", nValue);
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
		m_sSaturation.Format("%d", nValue);
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
		m_sMDTreshold.Format("%d", nValue);
		UpdateDialog();
	}

	if (m_pOutputTashaCamera)
		m_pOutputTashaCamera->OnConfirmGetMDTreshold(dwUserData, wSource, nValue);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmGetMaskTreshold(DWORD dwUserData, WORD wSource, int nValue)
{
	if (wSource == m_wSource)
	{
		m_sMaskTreshold.Format("%d", nValue);
		UpdateDialog();
	}

	if (m_pOutputTashaCamera)
		m_pOutputTashaCamera->OnConfirmGetMaskTreshold(dwUserData, wSource, nValue);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmGetMaskIncrement(DWORD dwUserData, WORD wSource, int nValue)
{
	if (wSource == m_wSource)
	{
		m_sMaskIncrement.Format("%d", nValue);
		UpdateDialog();
	}

	if (m_pOutputTashaCamera)
		m_pOutputTashaCamera->OnConfirmGetMaskIncrement(dwUserData, wSource, nValue);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmGetMaskDelay(DWORD dwUserData, WORD wSource, int nValue)
{
	if (wSource == m_wSource)
	{
		m_sMaskDelay.Format("%d", nValue);
		UpdateDialog();
	}

	if (m_pOutputTashaCamera)
		m_pOutputTashaCamera->OnConfirmGetMaskDelay(dwUserData, wSource, nValue);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmChangePermanentMask(DWORD dwUserData, WORD wSource, int nX, int nY, int nValue)
{
	return TRUE;
}


//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmSetBrightness(DWORD dwUserData, WORD wSource, int nValue)
{	
//	m_pPerf1->Stop();

	if (wSource == m_wSource)
	{
		m_sBrightness.Format("%d", nValue);
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
		m_sContrast.Format("%d", nValue);
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
		m_sSaturation.Format("%d", nValue);
		UpdateDialog();
	}

	if (m_pOutputTashaCamera)
		m_pOutputTashaCamera->OnConfirmSetSaturation(dwUserData, wSource, nValue);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmSetMDTreshold(DWORD dwUserData, WORD wSource, int nValue)
{	
	if (wSource == m_wSource)
	{
		m_sMDTreshold.Format("%d", nValue);
		UpdateDialog();
	}

	if (m_pOutputTashaCamera)
		m_pOutputTashaCamera->OnConfirmSetMDTreshold(dwUserData, wSource, nValue);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmSetMaskTreshold(DWORD dwUserData, WORD wSource, int nValue)
{	
	if (wSource == m_wSource)
	{
		m_sMaskTreshold.Format("%d", nValue);
		UpdateDialog();
	}

	if (m_pOutputTashaCamera)
		m_pOutputTashaCamera->OnConfirmSetMaskTreshold(dwUserData, wSource, nValue);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmSetMaskIncrement(DWORD dwUserData, WORD wSource, int nValue)
{	
	if (wSource == m_wSource)
	{
		m_sMaskIncrement.Format("%d", nValue);
		UpdateDialog();
	}

	if (m_pOutputTashaCamera)
		m_pOutputTashaCamera->OnConfirmSetMaskIncrement(dwUserData, wSource, nValue);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmSetMaskDelay(DWORD dwUserData, WORD wSource, int nValue)
{	
	if (wSource == m_wSource)
	{
		m_sMaskDelay.Format("%d", nValue);
		UpdateDialog();
	}

	if (m_pOutputTashaCamera)
		m_pOutputTashaCamera->OnConfirmSetMaskDelay(dwUserData, wSource, nValue);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmClearPermanentMask(DWORD dwUserData, WORD wSource)
{
	if (m_pOutputTashaCamera)
		m_pOutputTashaCamera->OnConfirmClearPermanentMask(dwUserData, wSource);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmInvertPermanentMask(DWORD dwUserData, WORD wSource)
{
	if (m_pOutputTashaCamera)
		m_pOutputTashaCamera->OnConfirmInvertPermanentMask(dwUserData, wSource);

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

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTashaUnitDlg::OnPaint() 
{
	if (IsIconic())
	{
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
		dc.DrawIcon(x, y, m_hIcon);
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
	return (HCURSOR) m_hIcon;
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

//	TRACE("OnHScroll (%d, %d) SBCode=%d\n", curpos, nPos, nSBCode);
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
	// Gültiges Bild?
	if (!pPacket)
		return FALSE;

//	if (pPacket->dwFieldID - m_dwFieldID[pPacket->wSource] != 1)
//		WK_TRACE("%d fields of camera %d dropped\n", (pPacket->dwFieldID-m_dwFieldID[pPacket->wSource])-1,pPacket->wSource);  
	
	m_dwFieldID[pPacket->wSource] = pPacket->dwFieldID;

	if (m_bMultiView)
	{
		if (m_pPrewiewWnd)
			m_pPrewiewWnd->DrawImage(pPacket, m_bShowAdaptiveMask, m_bShowPermanentMask, m_bShowDiffImage);	
	}
	else
	{
		if (m_pPrewiewWnd && (pPacket->wSource == m_wSource))
			m_pPrewiewWnd->DrawImage(pPacket, m_bShowAdaptiveMask, m_bShowPermanentMask, m_bShowDiffImage);	
	}
	bResult = TRUE;

	if (!m_bStandalone)		   
	{
		// Daten an die Outputgroup übergeben.
//		m_csEncode.Lock();
		WORD wSource = pPacket->wSource;

		// Existiert ein Job ?
		if (m_pCurrentEncodeJob[wSource] != NULL) 
		{
			// Stimmt der geforderte Prozeß mit dem gelieferten Prozeß überein.?
			if (m_pCurrentEncodeJob[wSource]->m_camID.GetSubID() == pPacket->wSource)
			{
				pPacket->dwProzessID = m_pCurrentEncodeJob[wSource]->m_dwUserData;
				// Daten an die Outputgroup übergeben.
				if (m_pOutputTashaCamera)
					m_pOutputTashaCamera->IndicationDataReceived(pPacket,TRUE);

				// Bewegungsalarme melden.
				if (pPacket->bMotion)
				{
					if (m_pInputMDAlarm)
						m_pInputMDAlarm->OnReceivedMotionAlarm(pPacket, TRUE);
 					if (m_pInputMDAlarm)
						m_pInputMDAlarm->OnReceivedMotionAlarm(pPacket, FALSE);
				}

				if (m_pCurrentEncodeJob[wSource])
				{
					m_pCurrentEncodeJob[wSource]->m_iOutstandingPics--;

					if (m_pCurrentEncodeJob[wSource]->m_iOutstandingPics == 0) 
						ClearCurrentEncodeJob(wSource); // Letztes Bild erhalten -> Job beenden
				}
			}
			else
			{
	//			WK_TRACE("Data without job (ProzessIDs: (%lu, %lu)\n",	m_pCurrentEncodeJob->m_dwUserData,
	//																	pJpeg->dwProzessID);
				// ProzessID 0 gibt's nicht, wird nur von der JaCob intern verwendet
	//			if (pJpeg->dwProzessID == 0)
	//				ClearCurrentEncodeJob();
			}	
		}

//		m_csEncode.Unlock();

	}
	m_dwEncodedFrames++;
	if ((m_dwEncodedFrames % 1000) == 0)
		m_dwStartTime = GetTickCount();

	UpdateDialog();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnReceivedCameraStatus(WORD wSource, BOOL bVideoState)
{
	WK_TRACE("CameraStatus: Source=%d, Status=%d\n", wSource, bVideoState);

	// Statusänderung dem Server mitteilen
	if (m_pOutputTashaCamera)
		m_pOutputTashaCamera->IndicationVideoState(wSource, bVideoState);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnIndicationAlarm(DWORD dwAlarmMask)
{
//	WK_TRACE(_T("OnIndicationAlarm dwAlarmMask=0x%x\n"), dwAlarmMask);
	for (int nI = 0; nI < 8; nI++)
		((CButton*)GetDlgItem(IDC_CHECK_ALARM0+nI))->SetCheck(TSTBIT(dwAlarmMask, nI));

	if (m_pInput)
		m_pInput->AlarmStateChanged(dwAlarmMask);

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
			s.Format("%sJ_%06lu.jpg\0",m_sJpegPath, m_dwEncodedFrames % 1000);

			CFile wFile(s,	CFile::modeCreate | CFile::modeWrite  |
							CFile::typeBinary | CFile::shareDenyWrite);

			wFile.WriteHuge(pBuffer, pPacket->dwImageDataLen); 
			bResult = TRUE;
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
	if (m_pOutputTashaCamera)
		m_pOutputTashaCamera->OnConfirmSetRelayState(wRelayID, bOpenClose);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmGetRelayState(DWORD dwRelayStateMask)
{
	m_wRelayStateMask = dwRelayStateMask;
	m_evRelayStateConfirm.SetEvent();

	if (m_pOutputTashaCamera)
		m_pOutputTashaCamera->OnConfirmGetRelayState(dwRelayStateMask);

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
void CTashaUnitDlg::UpdateDialog()
{
	PostMessage(WM_UPDATE_DIALOG);
}

/////////////////////////////////////////////////////////////////////////////
long CTashaUnitDlg::OnUpdateDialog(WPARAM wParam, LPARAM lParam)
{
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
			WK_TRACE_WARNING("Unknown Encoderstate (%d)\n", m_pCodec->GetEncoderState());
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

	m_ctrlBrightness.SetPos(atoi(m_sBrightness));
	m_ctrlContrast.SetPos(atoi(m_sContrast));
	m_ctrlSaturation.SetPos(atoi(m_sSaturation));

	GetDlgItem(IDC_STATIC_MD_TRESHOLD)->SetWindowText(m_sMDTreshold);
	GetDlgItem(IDC_STATIC_MASK_TRESHOLD)->SetWindowText(m_sMaskTreshold);
	GetDlgItem(IDC_STATIC_MASK_INCREMENT)->SetWindowText(m_sMaskIncrement);
	GetDlgItem(IDC_STATIC_MASK_DELAY)->SetWindowText(m_sMaskDelay);

	m_ctrlMDTreshold.SetPos(atoi(m_sMDTreshold));
	m_ctrlMaskTreshold.SetPos(atoi(m_sMaskTreshold));
	m_ctrlMaskIncrement.SetPos(atoi(m_sMaskIncrement));
	m_ctrlMaskDelay.SetPos(atoi(m_sMaskDelay));

	if (m_pCodec->GetEncoderState() == eEncoderStateOn)
	{
		if ((GetTickCount() - m_dwStartTime) != 0)
		{
			float fFps = (1000.0* (float)(m_dwEncodedFrames % 1000)) / (float)(GetTickCount() - m_dwStartTime);
			CString sText;

			sText.Format("%.02f", fFps);
			GetDlgItem(IDC_STATIC_FPS)->SetWindowText(sText);	

			sText.Format("%lu", m_dwEncodedFrames);
			GetDlgItem(IDC_STATIC_FRAME)->SetWindowText(sText);	
		}
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::PollProzess()
{
	static BOOL bOnlyOnce = FALSE;
	HANDLE hEvent[MAX_CAMERAS+1];

	for (int nI = 0; nI < MAX_CAMERAS; nI++)
		hEvent[nI] = m_jobQueueEncode[nI].GetSemaphore();
	hEvent[MAX_CAMERAS] = m_hShutDownEvent;

	// Warte bis neuer Job verfügbar ist
	switch (WaitForMultipleObjects(2, hEvent, FALSE, m_dwStandbyTime))
	{
		case WAIT_OBJECT_0:
			ProzessManagement();
			bOnlyOnce = TRUE;
			break;
		case WAIT_OBJECT_0+1:
			break;
		case WAIT_TIMEOUT:
			if (bOnlyOnce)
			{
				WK_TRACE("JobQueue empty\tEncoder stop\n");
				if (m_pOutputTashaCamera)
					m_pOutputTashaCamera->EncoderStop(NULL);
				bOnlyOnce = FALSE;
//				SetTashaIcon(JACOB_ICON_BLUE);
			}
			break;
	}
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::ProzessManagement()
{
	m_csEncode.Lock();

	// Alle Jobqueues nach Jobs durchsuchen
	for (int nI = 0; nI < MAX_CAMERAS; nI++)
	{
		// Wird für diese Kamera gerade ein Job bearbeitet?
		if (m_pCurrentEncodeJob[nI] == NULL)
		{
			// Nein...dann hole dir den nächstem Job für diese Kamera
			m_pCurrentEncodeJob[nI] = m_jobQueueEncode[nI].SafeGetAndRemoveHead();
			
			// Existiert ein Job?
			if (m_pCurrentEncodeJob[nI])
			{
				CIPCOutputTashaUnit* pOutput = (CIPCOutputTashaUnit*)m_pCurrentEncodeJob[nI]->m_pCipc;
	//			SetTashaIcon(JACOB_ICON_RED);

				switch (m_pCurrentEncodeJob[nI]->m_action)
				{			  
					case VJA_ENCODE:
						if (pOutput)
				 			pOutput->EncoderStart(m_pCurrentEncodeJob[nI]);
						break;
					case VJA_STOP_ENCODE:
	//					SetTashaIcon(JACOB_ICON_BLUE);
						if (pOutput)       					    
				 			pOutput->EncoderStop(m_pCurrentEncodeJob[nI]);
						ClearCurrentEncodeJob(nI);	// job done 
						break;
					case VJA_IDLE:
						Sleep(m_nTimePerFrame);		// Ein (Halb)bild warten
						ClearCurrentEncodeJob(nI);	// job done 
						break;
					case VJA_SYNC:
						if (m_hSyncSemaphore)
							ReleaseSemaphore(m_hSyncSemaphore, 1, NULL);
						ClearCurrentEncodeJob(nI);	// job done 
						break;
					default:	
						WK_TRACE_ERROR("Invalid Encode job type\n");
						ClearCurrentEncodeJob(nI);	// job done 
				}	// end of job switch
			}	// end of job found		  
			else															 
			{
	//			SetTashaIcon(JACOB_ICON_GREEN);
			}
		}
	}
	
	m_csEncode.Unlock();
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::ClearAllJobs()
{
	ClearAllEncodeJobs();
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::ClearAllEncodeJobs()
{
	for (int nI = 0; nI < MAX_CAMERAS; nI++)
	{
		ClearCurrentEncodeJob(nI);
		m_jobQueueEncode[nI].SafeDeleteAll();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::ClearCurrentEncodeJob(WORD wSource)
{
	m_csEncode.Lock();

	if (m_pCurrentEncodeJob[wSource])
	 	m_jobQueueEncode[wSource].IncreaseSemaphore(); // Event auslösen -> Nächsten Encoderjob holen
	WK_DELETE(m_pCurrentEncodeJob[wSource]);
	m_csEncode.Unlock();
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnRelais0() 
{
	if (m_pCodec)
	{
		m_evRelayStateConfirm.ResetEvent();
		m_pCodec->DoRequestGetRelayState();
		if (WaitForSingleObject(m_evRelayStateConfirm, 1000) == WAIT_OBJECT_0)
			m_pCodec->DoRequestSetRelayState(0, !TSTBIT(m_wRelayStateMask, 0));
		else
			WK_TRACE_ERROR("DoRequestGetRelayState timeout\n");
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
			m_pCodec->DoRequestSetRelayState(1, !TSTBIT(m_wRelayStateMask, 1));
		else
			WK_TRACE_ERROR("DoRequestGetRelayState timeout\n");
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
			m_pCodec->DoRequestSetRelayState(2, !TSTBIT(m_wRelayStateMask, 2));
		else
			WK_TRACE_ERROR("DoRequestGetRelayState timeout\n");
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
			m_pCodec->DoRequestSetRelayState(3, !TSTBIT(m_wRelayStateMask, 3));
		else
			WK_TRACE_ERROR("DoRequestGetRelayState timeout\n");
	}
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnCheckFrameSync() 
{
	UpdateData();

	if (m_pCodec)
		m_pCodec->EnableFrameSync(m_bFrameSync);

	if (m_bFrameSync && m_pPrewiewWnd)
	{
		Sleep(10);
		m_pPrewiewWnd->InvalidateRect(NULL, TRUE);
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
		m_pCodec->DoRequestSetChannel(3, nCamNr);
		m_pCodec->DoRequestGetBrightness(0, nCamNr);
		m_pCodec->DoRequestGetContrast(0, nCamNr);
		m_pCodec->DoRequestGetSaturation(0, nCamNr);
		m_pCodec->DoRequestGetMDTreshold(0, nCamNr);
		m_pCodec->DoRequestGetMaskTreshold(0, nCamNr);
		m_pCodec->DoRequestGetMaskIncrement(0, nCamNr);
		m_pCodec->DoRequestGetMaskDelay(0, nCamNr);
		m_wSource = nCamNr;
	}
	UpdateDialog();

	WriteConfigurationInt(_T("General"), _T("DefaultCam"), m_wSource, m_sIniFile);
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnPreview() 
{
	if (m_pPrewiewWnd == NULL)
	{
		m_pPrewiewWnd = new CVideoInlayWindow(m_pCodec, TRUE);
		if (m_pPrewiewWnd)
		{
			OnCheckMultyView();
			OnStartCapture();
		}
	}
	else
	{
		OnStopCapture();
		WK_DELETE(m_pPrewiewWnd);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnCheckMultyView() 
{
	UpdateData();
	if (m_pPrewiewWnd)
		m_pPrewiewWnd->SetPreviewMode(m_bMultiView);
	
	WriteConfigurationInt(_T("General"), _T("MultiView"), m_bMultiView, m_sIniFile);
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnDownloadEPLD() 
{
	CString sFilename;

	if (SelectFile(sFilename, "*.rbf"))
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

	if (SelectFile(sFilename, "*.ldr"))
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

	if (SelectFile(sFilename, "*.ldr"))
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
	CString sFileName;
	GetDlgItem(IDC_RESET_BF533)->EnableWindow(FALSE);
	sFileName.Format(BOOT_IMAGE_533, m_wSource);
	if (m_pCodec)
		m_pCodec->TransferBF533BootData(m_wSource, sFileName);

	GetDlgItem(IDC_RESET_BF533)->EnableWindow(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::SelectFile(CString &sFilename, const CString& sExt)
{
	OPENFILENAME  ofn;
	static char   szDirName[256]  = {'\0'};                 // Startdirectory
	static char   szFileName[256] = {0};                    // Filename + Pfad
	char          szFileTitle[256];                         // Dateiname
	char		  szFilter[] = {"EEProm Image (*.ldr)\0*.ldr\0Altera Image (*.rbf)\0*.rbf\0\0\0"};

	strcpy(szFileName, sExt);

	/*** OPENFILENAME-Struktur ausfüllen ***/
	ofn.lStructSize       = sizeof(OPENFILENAME);
	ofn.hwndOwner         = (HWND) NULL;
	ofn.hInstance         = (HINSTANCE) AfxGetInstanceHandle();
	ofn.lpstrFilter       = szFilter;
	ofn.lpstrCustomFilter = (LPSTR) NULL;
	ofn.nMaxCustFilter    = 0L;
	ofn.nFilterIndex      = 0L;
	ofn.lpstrFile         = szFileName;                    // Filename mit Pfad
	ofn.nMaxFile          = 256;                   
	ofn.lpstrFileTitle    = szFileTitle;                   // Filename ohne Pfad
	ofn.nMaxFileTitle     = sizeof(szFileTitle);
	ofn.lpstrInitialDir   = szDirName;                     // Startverzeichnis
	ofn.lpstrTitle        = (LPSTR)"Datei auswählen";
	ofn.Flags             = OFN_HIDEREADONLY;
	ofn.nFileOffset       = 0;
	ofn.nFileExtension    = 0;
	ofn.lpstrDefExt       = (LPSTR) NULL;
	ofn.lCustData         = 0L;
	//   ofn.lpfnHook          = (FARPROC) NULL;
	ofn.lpTemplateName    = (LPSTR) NULL;
	
	/*** Dialog anzeigen ***/
	if (GetOpenFileName(&ofn) == FALSE)
	   return (FALSE);
	
	/*** Verzeichnis für nächsten Aufruf merken ***/
	strncpy(&szDirName[0], &szFileName[0], ofn.nFileOffset - 1);
	sFilename = CString(szFileName);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnCheckNoisereduction() 
{
	UpdateData();
	if (m_bNoiseReduction)
	{
		if (m_pPrewiewWnd)
			m_pPrewiewWnd->EnableNoiseReduction();
	}
	else
	{
		if (m_pPrewiewWnd)
			m_pPrewiewWnd->DisableNoiseReduction();
	}

	WriteConfigurationInt(_T("General"), _T("NoiseReduction"), m_bNoiseReduction, m_sIniFile);
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
	OnStopCapture();

	WK_DELETE(m_pPrewiewWnd);
	
	CDialog::OnOK();
}

