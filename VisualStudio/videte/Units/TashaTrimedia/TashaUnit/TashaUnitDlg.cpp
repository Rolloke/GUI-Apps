// TashaUnitDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TashaUnit.h"
#include "TashaUnitDlg.h"
#include "CCodec.h"
#include "CipcInputTashaUnit.h"
#include "CipcInputTashaUnitMDAlarm.h"
#include "CipcOutputTashaUnit.h"

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
	m_sBrightness = _T("127");
	m_sContrast = _T("63");
	m_sSaturation = _T("63");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
//	m_pPerf1 = new CWK_PerfMon("CommunicationDelay");

	m_dwEncodedFrames		= 0;
	m_sJpegPath				= "D:\\Jpeg1\\";
	m_dwStartTime			= 0;
	m_dwStandbyTime			= 30000;
	m_nTimePerFrame			= 40;
	m_hSyncSemaphore		= NULL;
	m_hShutDownEvent		= NULL;
	m_bStandalone			= FALSE;
	m_bRun					= FALSE;
	m_wSource				= VIDEO_SOURCE0;
	m_wRelayStateMask		= 0;

	m_pOutputTashaCamera	= NULL;
	m_pOutputTashaRelay		= NULL;
	m_pInput				= NULL;
	m_pInputMDAlarm			= NULL;

	m_pCurrentEncodeJob		= NULL;

	m_pPollProzessThread	= NULL;
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
	DDX_Control(pDX, IDC_SCROLLBAR_BRIGHTNESS, m_ctrlBrightness);
	DDX_Control(pDX, IDC_SCROLLBAR_CONTRAST, m_ctrlContrast);
	DDX_Control(pDX, IDC_SCROLLBAR_SATURATION, m_ctrlSaturation);
	DDX_Text(pDX, IDC_STATIC_BRIGTNESS, m_sBrightness);
	DDX_Text(pDX, IDC_STATIC_CONTRAST, m_sContrast);
	DDX_Text(pDX, IDC_STATIC_SATURATION, m_sSaturation);
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
	ON_BN_CLICKED(IDC_RADIO_CAM1, OnSelectCamera1)
	ON_BN_CLICKED(IDC_RADIO_CAM2, OnSelectCamera2)
	ON_BN_CLICKED(IDC_RADIO_CAM3, OnSelectCamera3)
	ON_BN_CLICKED(IDC_RADIO_CAM4, OnSelectCamera4)
	ON_BN_CLICKED(IDC_RELAIS0, OnRelais0)
	ON_BN_CLICKED(IDC_RELAIS1, OnRelais1)
	ON_BN_CLICKED(IDC_RELAIS2, OnRelais2)
	ON_BN_CLICKED(IDC_RELAIS3, OnRelais3)
	ON_WM_CANCELMODE()
	ON_BN_CLICKED(IDC_TEST, OnTest)
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
	m_pCodec = new CCodec(this, 0);
	if (!m_pCodec)
		return -1;

	if (!m_pCodec->Open(TASHA_FIRMWARE))
		return -1;

	// Die CIPC-Schnittstelle braucht im Standalonebetrieb nicht aufgebaut zu werde
	if (!m_bStandalone) 
	{
		CString sSMName;

		// die CIPC-Schnittstellen aufbauen.
		sSMName = SM_MICO_INPUT;
		if (theApp.GetCardRef() != 1)
			sSMName += ('0'+ theApp.GetCardRef());
		m_pInput = new CIPCInputTashaUnit(this, m_pCodec, sSMName);
		if (!m_pInput)
			WK_TRACE_ERROR("CTashaUnitDlg::Create\tCIPCInputTashaUnit-Objekt konnte nicht angelegt werden.\n");

		sSMName = SM_MICO_MD_INPUT;
		if (theApp.GetCardRef() != 1)
			sSMName += ('0'+ theApp.GetCardRef());
		m_pInputMDAlarm = new CIPCInputTashaUnitMDAlarm(this, m_pCodec, sSMName);
		if (!m_pInputMDAlarm)
			WK_TRACE_ERROR("CTashaUnitDlg::Create\tCIPCInputTashaUnitMDAlarm-Objekt konnte nicht angelegt werden.\n");
		
		sSMName = SM_MICO_OUTPUT_CAMERAS;
		if (theApp.GetCardRef() != 1)
			sSMName += ('0'+ theApp.GetCardRef());
		
		m_pOutputTashaCamera = new CIPCOutputTashaUnit(this, m_pCodec, OUTPUT_TYPE_CAMERA, sSMName);
		if (!m_pOutputTashaCamera)
			WK_TRACE_ERROR("CTashaUnitDlg::Create\tCIPCOutputTashaUnit-TashaCamera-Objekt konnte nicht angelegt werden.\n");
		
		sSMName = SM_MICO_OUTPUT_RELAYS;
		if (theApp.GetCardRef() != 1)
			sSMName += ('0'+ theApp.GetCardRef());

		m_pOutputTashaRelay = new CIPCOutputTashaUnit(this, m_pCodec, OUTPUT_TYPE_RELAY, sSMName);
		if (!m_pOutputTashaRelay)
			WK_TRACE_ERROR("CTashaUnitDlg::Create\tCIPCOutputTashaUnit-Relay-Objekt konnte nicht angelegt werden.\n");	

		
		CString sEventName = "MiCoSyncEvent";
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

	// TODO: Add extra initialization here
	m_ctrlBrightness.SetScrollRange(0, 255);
	m_ctrlContrast.SetScrollRange(0, 127);
	m_ctrlSaturation.SetScrollRange(0, 127);
	
	m_ctrlBrightness.SetScrollPos(127);
	m_ctrlContrast.SetScrollPos(63);
	m_ctrlSaturation.SetScrollPos(63);



	return TRUE;  // return TRUE  unless you set the focus to a control
}

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmGetBrightness(DWORD dwUserData, WORD wSource, int nValue)
{	
	m_sBrightness.Format("%d", nValue);
	UpdateDialog();
	
	if (m_pOutputTashaCamera)
		m_pOutputTashaCamera->OnConfirmGetBrightness(dwUserData, wSource, nValue);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmGetContrast(DWORD dwUserData, WORD wSource, int nValue)
{
	m_sContrast.Format("%d", nValue);
	UpdateDialog();

	if (m_pOutputTashaCamera)
		m_pOutputTashaCamera->OnConfirmGetContrast(dwUserData, wSource, nValue);
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmGetSaturation(DWORD dwUserData, WORD wSource, int nValue)
{
	m_sSaturation.Format("%d", nValue);
	UpdateDialog();

	if (m_pOutputTashaCamera)
		m_pOutputTashaCamera->OnConfirmGetSaturation(dwUserData, wSource, nValue);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmSetBrightness(DWORD dwUserData, WORD wSource, int nValue)
{	
	m_sBrightness.Format("%d", nValue);
	UpdateDialog();

	if (m_pOutputTashaCamera)
		m_pOutputTashaCamera->OnConfirmSetBrightness(dwUserData, wSource, nValue);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmSetContrast(DWORD dwUserData, WORD wSource, int nValue)
{
	m_sContrast.Format("%d", nValue);
	UpdateDialog();
	
	if (m_pOutputTashaCamera)
		m_pOutputTashaCamera->OnConfirmSetContrast(dwUserData, wSource, nValue);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmSetSaturation(DWORD dwUserData, WORD wSource, int nValue)
{
	m_sSaturation.Format("%d", nValue);
	UpdateDialog();

	if (m_pOutputTashaCamera)
		m_pOutputTashaCamera->OnConfirmSetSaturation(dwUserData, wSource, nValue);

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
	m_dwStartTime = GetTickCount();
	UpdateDialog();

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
	// Get the minimum and maximum scroll-bar positions.
	int minpos;
	int maxpos;
	pScrollBar->GetScrollRange(&minpos, &maxpos); 
	maxpos = pScrollBar->GetScrollLimit();

	// Get the current position of scroll box.
	int curpos = pScrollBar->GetScrollPos();

	// Determine the new position of scroll box.
	switch (nSBCode)
	{
		case SB_LEFT:    // Scroll to far left.
			curpos = minpos;
			break;
		case SB_RIGHT:    // Scroll to far right.
			curpos = maxpos;
			break;
		case SB_ENDSCROLL:  // End scroll.
			break;
		case SB_LINELEFT:    // Scroll left.
			if (curpos > minpos)
			  curpos--;
			break;
		case SB_LINERIGHT:  // Scroll right.
			if (curpos < maxpos)
			  curpos++;
			break;
		case SB_PAGELEFT:   // Scroll one page left.
			{
				// Get the page size. 
				SCROLLINFO  info;
				pScrollBar->GetScrollInfo(&info, SIF_ALL);

				if (curpos > minpos)
				curpos = max(minpos, curpos - (int) info.nPage);
			}
			break;
		case SB_PAGERIGHT:    // Scroll one page right.
			{
				// Get the page size. 
				SCROLLINFO  info;
				pScrollBar->GetScrollInfo(&info, SIF_ALL);

				if (curpos < maxpos)
				  curpos = min(maxpos, curpos + (int) info.nPage);
			}
			break;
		case SB_THUMBPOSITION: // Scroll to absolute position. nPos is the position
			curpos = nPos;      // of the scroll box at the end of the drag operation.
			break;
		case SB_THUMBTRACK:   // Drag scroll box to specified position. nPos is the
			curpos = nPos;     // position that the scroll box has been dragged to.
			break;
	}

	if (pScrollBar->GetDlgCtrlID() == IDC_SCROLLBAR_BRIGHTNESS)
	{
		if (m_pCodec)
			m_pCodec->DoRequestSetBrightness(0, m_wSource, curpos);
//		m_pPerf1->Start();
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

	// Set the new position of the thumb (scroll box).
	pScrollBar->SetScrollPos(curpos);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnReceiveNewCodecData(DataPacket *pPacket)
{
	BOOL bResult = FALSE;
#if (0)
	WK_TRACE("HOST Valid=%d\nType=%d\nSubType=%d\nUserID=0x%x\nSource=%u\nDate=%u.%u.%u\nTime=%u:%u:%u,%u\nLen=%lu\nMD(%d,%d)=%d\nMD(%d,%d)=%d\nMD(%d,%d)=%d\nMD(%d,%d)=%d\nMD(%d,%d)=%d\n###############\n\n",
											pPacket->bValid,
											pPacket->eType,
											pPacket->eSubType,
											pPacket->dwUserID,
											pPacket->wSource,
											pPacket->TimeStamp.wDay,
											pPacket->TimeStamp.wMonth,
											pPacket->TimeStamp.wYear,
											pPacket->TimeStamp.wHour,
											pPacket->TimeStamp.wMinute,
											pPacket->TimeStamp.wSecond,
											pPacket->TimeStamp.wMSecond,
											pPacket->dwDataLen,
											pPacket->Point[0].cx,
											pPacket->Point[0].cy,
											pPacket->Point[0].nValue,
											pPacket->Point[1].cx,
											pPacket->Point[1].cy,
											pPacket->Point[1].nValue,
											pPacket->Point[2].cx,
											pPacket->Point[2].cy,
											pPacket->Point[2].nValue,
											pPacket->Point[3].cx,
											pPacket->Point[3].cy,
											pPacket->Point[3].nValue,
											pPacket->Point[4].cx,
											pPacket->Point[4].cy,
											pPacket->Point[4].nValue);

#endif
	// Gültiges Bild?
	if (!pPacket)
		return FALSE;

	if (m_bStandalone)		   
		bResult = SaveJpegFile(pPacket);
	else
	{
		// Daten an die Outputgroup übergeben.
//		m_csEncode.Lock();
		
		// Existiert ein Job ?
		if (m_pCurrentEncodeJob != NULL) 
		{
			// Stimmt der geforderte Prozeß mit dem gelieferten Prozeß überein.?
			if (m_pCurrentEncodeJob->m_dwUserData == pPacket->dwProzessID) 
			{
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

				if (m_pCurrentEncodeJob)
				{
					m_pCurrentEncodeJob->m_iOutstandingPics--;

					if (m_pCurrentEncodeJob->m_iOutstandingPics == 0) 
						ClearCurrentEncodeJob(); // Letztes Bild erhalten -> Job beenden
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
BOOL CTashaUnitDlg::SaveJpegFile(DataPacket* pPacket)
{
	BOOL bResult = FALSE;
	if (pPacket)
	{
		BYTE* pBuffer = (BYTE*)&pPacket->dwData;
		if (pBuffer)
		{
			CString s;
			s.Format("%sJ_%06lu.jpg\0",m_sJpegPath, m_dwEncodedFrames % 1000);

			CFile wFile(s,	CFile::modeCreate | CFile::modeWrite  |
							CFile::typeBinary | CFile::shareDenyWrite);

			wFile.WriteHuge(pBuffer, pPacket->dwDataLen); 
			bResult = TRUE;
		}
	}
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmSetInputSource(WORD wSource, DWORD dwProcessID)
{
	m_wSource = wSource;

	UpdateDialog();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CTashaUnitDlg::OnConfirmGetInputSource(WORD wSource, DWORD dwProcessID)
{
	m_wSource = wSource;
	UpdateDialog();

	if (!m_bStandalone && m_pOutputTashaCamera)
		m_pOutputTashaCamera->OnConfirmGetInputSource(wSource);

	return TRUE;
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
void CTashaUnitDlg::OnSelectCamera1() 
{
	if (m_pCodec)
	{
		SOURCE_SELECTION SrcSel1;
		SrcSel1.dwPics		= -1;
		SrcSel1.dwProzessID	= 0x0815;
		SrcSel1.wSource		= VIDEO_SOURCE0;
		SrcSel1.wBrightness	= -1; //m_ctrlBrightness.GetScrollPos();
		SrcSel1.wContrast	= -1; //m_ctrlBrightness.GetScrollPos();
		SrcSel1.wSaturation	= -1; //m_ctrlBrightness.GetScrollPos();
		SrcSel1.bValid		= TRUE;
		
		SOURCE_SELECTION SrcSel2;
		SrcSel1.dwPics		= -1;
		SrcSel1.dwProzessID	= 0x0815;
		SrcSel1.wSource		= VIDEO_SOURCE0;
		SrcSel1.wBrightness	= -1; //m_ctrlBrightness.GetScrollPos();
		SrcSel1.wContrast	= -1; //m_ctrlBrightness.GetScrollPos();
		SrcSel1.wSaturation	= -1; //m_ctrlBrightness.GetScrollPos();
		SrcSel1.bValid		= TRUE;
		
		m_pCodec->DoRequestSetInputSource(SrcSel1, SrcSel2);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnSelectCamera2() 
{
	if (m_pCodec)
	{
		SOURCE_SELECTION SrcSel1;
		SrcSel1.dwPics		= -1;
		SrcSel1.dwProzessID	= 0x0815;
		SrcSel1.wSource		= VIDEO_SOURCE1;
		SrcSel1.wBrightness	= -1; //m_ctrlBrightness.GetScrollPos();
		SrcSel1.wContrast	= -1; //m_ctrlBrightness.GetScrollPos();
		SrcSel1.wSaturation	= -1; //m_ctrlBrightness.GetScrollPos();
		SrcSel1.bValid		= TRUE;
		
		SOURCE_SELECTION SrcSel2;
		SrcSel1.dwPics		= -1;
		SrcSel1.dwProzessID	= 0x0815;
		SrcSel1.wSource		= VIDEO_SOURCE1;
		SrcSel1.wBrightness	= -1; //m_ctrlBrightness.GetScrollPos();
		SrcSel1.wContrast	= -1; //m_ctrlBrightness.GetScrollPos();
		SrcSel1.wSaturation	= -1; //m_ctrlBrightness.GetScrollPos();
		SrcSel1.bValid		= TRUE;
		
		m_pCodec->DoRequestSetInputSource(SrcSel1, SrcSel2);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnSelectCamera3() 
{
	if (m_pCodec)
	{
		SOURCE_SELECTION SrcSel1;
		SrcSel1.dwPics		= -1;
		SrcSel1.dwProzessID	= 0x0815;
		SrcSel1.wSource		= VIDEO_SOURCE2;
		SrcSel1.wBrightness	= -1; //m_ctrlBrightness.GetScrollPos();
		SrcSel1.wContrast	= -1; //m_ctrlBrightness.GetScrollPos();
		SrcSel1.wSaturation	= -1; //m_ctrlBrightness.GetScrollPos();
		SrcSel1.bValid		= TRUE;
		
		SOURCE_SELECTION SrcSel2;
		SrcSel1.dwPics		= -1;
		SrcSel1.dwProzessID	= 0x0815;
		SrcSel1.wSource		= VIDEO_SOURCE2;
		SrcSel1.wBrightness	= -1; //m_ctrlBrightness.GetScrollPos();
		SrcSel1.wContrast	= -1; //m_ctrlBrightness.GetScrollPos();
		SrcSel1.wSaturation	= -1; //m_ctrlBrightness.GetScrollPos();
		SrcSel1.bValid		= TRUE;
		
		m_pCodec->DoRequestSetInputSource(SrcSel1, SrcSel2);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::OnSelectCamera4() 
{
	if (m_pCodec)
	{
		SOURCE_SELECTION SrcSel1;
		SrcSel1.dwPics		= -1;
		SrcSel1.dwProzessID	= 0x0815;
		SrcSel1.wSource		= VIDEO_SOURCE3;
		SrcSel1.wBrightness	= -1; //m_ctrlBrightness.GetScrollPos();
		SrcSel1.wContrast	= -1; //m_ctrlBrightness.GetScrollPos();
		SrcSel1.wSaturation	= -1; //m_ctrlBrightness.GetScrollPos();
		SrcSel1.bValid		= TRUE;
		
		SOURCE_SELECTION SrcSel2;
		SrcSel1.dwPics		= -1;
		SrcSel1.dwProzessID	= 0x0815;
		SrcSel1.wSource		= VIDEO_SOURCE3;
		SrcSel1.wBrightness	= -1; //m_ctrlBrightness.GetScrollPos();
		SrcSel1.wContrast	= -1; //m_ctrlBrightness.GetScrollPos();
		SrcSel1.wSaturation	= -1; //m_ctrlBrightness.GetScrollPos();
		SrcSel1.bValid		= TRUE;
		
		m_pCodec->DoRequestSetInputSource(SrcSel1, SrcSel2);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::UpdateDialog()
{
	PostMessage(WM_UPDATE_DIALOG);
}

/////////////////////////////////////////////////////////////////////////////
long CTashaUnitDlg::OnUpdateDialog(WPARAM wParam, LPARAM lParam)
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
			CheckRadioButton(IDC_RADIO_CAM1, IDC_RADIO_CAM4, IDC_RADIO_CAM1);
			break;
		case VIDEO_SOURCE1:
			CheckRadioButton(IDC_RADIO_CAM1, IDC_RADIO_CAM4, IDC_RADIO_CAM2);
			break;
		case VIDEO_SOURCE2:
			CheckRadioButton(IDC_RADIO_CAM1, IDC_RADIO_CAM4, IDC_RADIO_CAM3);
			break;
		case VIDEO_SOURCE3:
			CheckRadioButton(IDC_RADIO_CAM1, IDC_RADIO_CAM4, IDC_RADIO_CAM4);
			break;

	}

	GetDlgItem(IDC_STATIC_BRIGTNESS)->SetWindowText(m_sBrightness);
	GetDlgItem(IDC_STATIC_CONTRAST)->SetWindowText(m_sContrast);
	GetDlgItem(IDC_STATIC_SATURATION)->SetWindowText(m_sSaturation);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::PollProzess()
{
	static BOOL bOnlyOnce = FALSE;
	HANDLE hEvent[2];

	hEvent[0] = m_jobQueueEncode.GetSemaphore();
	hEvent[1] = m_hShutDownEvent;

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

	if (m_pCurrentEncodeJob == NULL)
	{
		// Aktuellen und nächsten Job holen
		m_pCurrentEncodeJob = m_jobQueueEncode.SafeGetAndRemoveHead();
		
		if (m_pCurrentEncodeJob)
		{
			CIPCOutputTashaUnit* pOutput = (CIPCOutputTashaUnit*)m_pCurrentEncodeJob->m_pCipc;
//			SetTashaIcon(JACOB_ICON_RED);

			switch (m_pCurrentEncodeJob->m_action)
			{			  
				case VJA_ENCODE:
					if (pOutput)
				 		pOutput->EncoderStart(m_pCurrentEncodeJob);
					break;
				case VJA_STOP_ENCODE:
//					SetTashaIcon(JACOB_ICON_BLUE);
					if (pOutput)       					    
						pOutput->EncoderStop(m_pCurrentEncodeJob);
					ClearCurrentEncodeJob();	// job done 
					break;
				case VJA_IDLE:
					Sleep(m_nTimePerFrame);		// Ein (Halb)bild warten
					ClearCurrentEncodeJob();	// job done 
					break;
				case VJA_SYNC:
					if (m_hSyncSemaphore)
						ReleaseSemaphore(m_hSyncSemaphore, 1, NULL);
					ClearCurrentEncodeJob();	// job done 
					break;
				default:	
					WK_TRACE_ERROR("Invalid Encode job type\n");
					ClearCurrentEncodeJob();	// job done 
			}	// end of job switch
		}	// end of job found		  
		else															 
		{
//			SetTashaIcon(JACOB_ICON_GREEN);
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
	ClearCurrentEncodeJob();
	m_jobQueueEncode.SafeDeleteAll();
}

/////////////////////////////////////////////////////////////////////////////
void CTashaUnitDlg::ClearCurrentEncodeJob()
{
	m_csEncode.Lock();

	if (m_pCurrentEncodeJob)
	 	m_jobQueueEncode.IncreaseSemaphore(); // Event auslösen -> Nächsten Encoderjob holen
	WK_DELETE(m_pCurrentEncodeJob);
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
void CTashaUnitDlg::OnTest() 
{
	if (m_pCodec)
	{
		m_pCodec->DoRequestGetAlarmState();
		m_pCodec->DoRequestSetAlarmEdge(0x1234);
		m_pCodec->DoRequestGetAlarmEdge();
	}
}


