// alarmlistDlg.cpp : implementation file
//

#include "stdafx.h"
#include "dvclient.h"
#include "cpanel.h"
#include "resource.h"
#include "alarmlistDlg.h"
#include "AlarmObject.h"
#include "ServerAlarmlist.h"
#include "IPCDatabaseClientAlarmlist.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int  CAlarmlistDlg::m_nWM_ScrollEnd = 0;
UINT CAlarmlistDlg::m_uWM_Scrolled = 0;

#define SELFCHECK_TIMEOUT	(DWORD)3000

static BOOL bStopWatch = FALSE;

/////////////////////////////////////////////////////////////////////////////
// CAlarmlistDlg dialog

/////////////////////////////////////////////////////////////////////////////
CAlarmlistDlg::CAlarmlistDlg(const CString& sHost, const CString& sPIN, CWnd* pParent /*=NULL*/)
	: CTransparentDialog(CAlarmlistDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAlarmlistDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32

	for (int nI = 0; nI < 4; nI++)
		m_nAlarmFilter[nI] = 0;
	
	m_hCursor		= NULL;
	m_pServer		= NULL;
	m_nWM_ScrollEnd = 0;
	m_sHost			= sHost;
	m_sPIN			= sPIN;
	m_pPanel		= (CPanel*)pParent;
	m_bRequesting   = FALSE;

	CTransparentDialog::Create(IDD, pParent);
	
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

/////////////////////////////////////////////////////////////////////////////
CAlarmlistDlg::~CAlarmlistDlg()
{
	m_pPanel->m_pAlarmlistDlg = NULL;
	WK_DELETE(m_pServer);
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::DoDataExchange(CDataExchange* pDX)
{
	CTransparentDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAlarmlistDlg)
	DDX_Control(pDX, IDC_ALARMLIST_AUDIO, m_ctrlAudio);
	DDX_Control(pDX, IDC_ALARMLIST_LIST, m_ctrlList);
	DDX_Control(pDX, IDC_ALARMLIST_UP2, m_ctrlNextDay);
	DDX_Control(pDX, IDC_ALARMLIST_DOWN2, m_ctrlPrevDay);
	DDX_Control(pDX, IDC_ALARMLIST_IMAGE, m_ctrlImage);
	DDX_Control(pDX, IDC_ALARMLIST_EXTERN2, m_ctrlExtern2);
	DDX_Control(pDX, IDC_ALARMLIST_EXTERN1, m_ctrlExtern1);
	DDX_Control(pDX, IDC_ALARMLIST_ACTIVITY, m_ctrlActivity);
	DDX_Control(pDX, IDC_ALARMLIST_PRINT, m_ctrlPrint);
	DDX_Control(pDX, IDC_ALARMLIST_UP,	m_ctrlLineUp);
	DDX_Control(pDX, IDC_ALARMLIST_DOWN, m_ctrlLineDown);
	DDX_Control(pDX, IDC_ALARMLIST_OK, m_ctrlOK);  
	DDX_Control(pDX, IDC_ALARMLIST_CAM0,  m_ctrlCam0);
	DDX_Control(pDX, IDC_ALARMLIST_CAM1,  m_ctrlCam1);
	DDX_Control(pDX, IDC_ALARMLIST_CAM2,  m_ctrlCam2);
	DDX_Control(pDX, IDC_ALARMLIST_CAM3,  m_ctrlCam3);
	DDX_Control(pDX, IDC_ALARMLIST_CAM4,  m_ctrlCam4);
	DDX_Control(pDX, IDC_ALARMLIST_CAM5,  m_ctrlCam5);
	DDX_Control(pDX, IDC_ALARMLIST_CAM6,  m_ctrlCam6);
	DDX_Control(pDX, IDC_ALARMLIST_CAM7,  m_ctrlCam7);
	DDX_Control(pDX, IDC_ALARMLIST_CAM8,  m_ctrlCam8);
	DDX_Control(pDX, IDC_ALARMLIST_CAM9,  m_ctrlCam9);
	DDX_Control(pDX, IDC_ALARMLIST_CAM10, m_ctrlCam10);
	DDX_Control(pDX, IDC_ALARMLIST_CAM11, m_ctrlCam11);
	DDX_Control(pDX, IDC_ALARMLIST_CAM12, m_ctrlCam12);
	DDX_Control(pDX, IDC_ALARMLIST_CAM13, m_ctrlCam13);
	DDX_Control(pDX, IDC_ALARMLIST_CAM14, m_ctrlCam14);
	DDX_Control(pDX, IDC_ALARMLIST_CAM15, m_ctrlCam15);
	DDX_Control(pDX, IDC_ALARMLIST_DISPLAY_BORDER, m_ctrlDisplayBorder);
	DDX_Control(pDX, IDC_ALARMLIST_DISPLAY_TITLE,  m_ctrlDisplayTitle);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CAlarmlistDlg, CTransparentDialog)
	//{{AFX_MSG_MAP(CAlarmlistDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_ALARMLIST_CAM0, OnCameraButton)
	ON_BN_CLICKED(IDC_ALARMLIST_UP, OnScrollLineUp)
	ON_BN_CLICKED(IDC_ALARMLIST_DOWN2, OnScrollPrevDay)
	ON_BN_CLICKED(IDC_ALARMLIST_DOWN, OnScrollLineDown)
	ON_BN_CLICKED(IDC_ALARMLIST_UP2, OnScrollNextDay)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_ALARMLIST_ACTIVITY, OnActivity)
	ON_BN_CLICKED(IDC_ALARMLIST_EXTERN1, OnExtern1)
	ON_BN_CLICKED(IDC_ALARMLIST_EXTERN2, OnExtern2)
	ON_BN_CLICKED(IDC_ALARMLIST_PRINT, OnPrint)
	ON_WM_TIMER()
	ON_WM_CAPTURECHANGED()
	ON_BN_CLICKED(IDC_ALARMLIST_IMAGE, OnAlarmlistImage)
	ON_LBN_DBLCLK(IDC_ALARMLIST_LIST, OnDblclkAlarmlistList)
	ON_LBN_SELCHANGE(IDC_ALARMLIST_LIST, OnSelchangeAlarmlistList)
	ON_BN_CLICKED(IDC_ALARMLIST_OK, OnAlarmlistOk)
	ON_WM_SETCURSOR()
	ON_WM_CANCELMODE()
	ON_BN_CLICKED(IDC_ALARMLIST_CAM1, OnCameraButton)
	ON_BN_CLICKED(IDC_ALARMLIST_CAM2, OnCameraButton)
	ON_BN_CLICKED(IDC_ALARMLIST_CAM3, OnCameraButton)
	ON_BN_CLICKED(IDC_ALARMLIST_CAM4, OnCameraButton)
	ON_BN_CLICKED(IDC_ALARMLIST_CAM5, OnCameraButton)
	ON_BN_CLICKED(IDC_ALARMLIST_CAM6, OnCameraButton)
	ON_BN_CLICKED(IDC_ALARMLIST_CAM7, OnCameraButton)
	ON_BN_CLICKED(IDC_ALARMLIST_CAM8, OnCameraButton)
	ON_BN_CLICKED(IDC_ALARMLIST_CAM9, OnCameraButton)
	ON_BN_CLICKED(IDC_ALARMLIST_CAM10, OnCameraButton)
	ON_BN_CLICKED(IDC_ALARMLIST_CAM11, OnCameraButton)
	ON_BN_CLICKED(IDC_ALARMLIST_CAM12, OnCameraButton)
	ON_BN_CLICKED(IDC_ALARMLIST_CAM13, OnCameraButton)
	ON_BN_CLICKED(IDC_ALARMLIST_CAM14, OnCameraButton)
	ON_BN_CLICKED(IDC_ALARMLIST_CAM15, OnCameraButton)
	ON_MESSAGE(WM_NOTIFY_CONNECT, OnNotifyConnect)
	ON_MESSAGE(WM_NOTIFY_DISCONNECT, OnNotifyDisconnect)
	ON_MESSAGE(WM_NOTIFY_ALARMLIST_UPDATE, OnNotifyAlarmListUpdate)
	ON_REGISTERED_MESSAGE(m_nWM_ScrollEnd, OnNotifyMessageScrollEnd)
	ON_REGISTERED_MESSAGE(m_uWM_Scrolled, OnNotifyMessageScrolled)
	ON_BN_CLICKED(IDC_ALARMLIST_AUDIO, OnAlarmlistAudio)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAlarmlistDlg message handlers

/////////////////////////////////////////////////////////////////////////////
BOOL CAlarmlistDlg::OnInitDialog()
{
	CTransparentDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	m_hCursor = theApp.m_hArrow;

	SetTimer(1, 250, NULL);

	CreateTransparent(StyleBackGroundColorChangeBrushed, CPanel::m_BaseColorBackGrounds);

	m_nWM_ScrollEnd = RegisterWindowMessage(SKIN_MSG_SCROLL_END);
	m_uWM_Scrolled = RegisterWindowMessage(SKIN_MSG_SCROLLED);

	m_ctrlDisplayTitle.OpenStockDisplay(this, Display2);

	DSPCOL DisplayColors;
	DisplayColors.colBackGround = CPanel::m_BaseColorDisplays;
	
//TKR kann entfallen, da der Alarmlist Dialog sich nun im Hauptmenü befindet
	// Alarmcom bekommt eine andere Displayfarbe
/*	if (m_pPanel && ((CPanel*)m_pPanel)->GetOem() == OemAlarmCom)
		DisplayColors.colBackGround	= SKIN_COLOR_BLUE; // Background Blue Color
*/
	// Farben des Displays setzen
	DisplayColors.colBorder	= DisplayColors.colBackGround;
	DisplayColors.colText	= SKIN_COLOR_BLACK;
	DisplayColors.colShadow = RGB(max(GetRValue(DisplayColors.colBackGround)-30,0),
								  max(GetGValue(DisplayColors.colBackGround)-30,0),
								  max(GetBValue(DisplayColors.colBackGround)-30,0));
	DisplayColors.dwFlags	= DSP_COL_TEXT | DSP_COL_BACKGROUNND | DSP_COL_BORDER | DSP_COL_SHADOW;
	m_ctrlDisplayTitle.SetDisplayColors(DisplayColors);
	
	m_ctrlDisplayBorder.CreateFrame(this);
	m_ctrlDisplayBorder.EnableShadow(TRUE);
	// Farben des Frames setzen
	FRAMECOL FrameColors;
	FrameColors.colBackGround	= DisplayColors.colBackGround;
	FrameColors.colBorder		= DisplayColors.colBorder;
	FrameColors.colShadow		= DisplayColors.colShadow;
	FrameColors.dwFlags			= FRAME_COL_BACKGROUNND | FRAME_COL_BORDER | FRAME_COL_SHADOW;
	m_ctrlDisplayBorder.SetFrameColors(FrameColors);

	// Farben der Listbox setzen
	m_ctrlList.SetBackgroundColorNormal(DisplayColors.colBackGround);
	m_ctrlList.SetBackgroundColorHighlighted(DisplayColors.colShadow);
	m_ctrlList.SetTextColorNormal(DisplayColors.colText);
	m_ctrlList.SetTextColorHighlighted(DisplayColors.colText);
	m_ctrlList.SetShadowColorHighlighted(DisplayColors.colShadow);

	m_ctrlList.SetShadowDistance(CPoint(2,2));	
	m_ctrlList.EnableShadow(TRUE);

	m_ctrlLineUp.EnableAutoRepeat();
	m_ctrlLineDown.EnableAutoRepeat();

	m_AlarmListButtons.Add(&m_ctrlLineUp);
	m_AlarmListButtons.Add(&m_ctrlLineDown);
	m_AlarmListButtons.Add(&m_ctrlPrevDay);
	m_AlarmListButtons.Add(&m_ctrlNextDay);
	m_AlarmListButtons.Add(&m_ctrlImage);
	m_AlarmListButtons.Add(&m_ctrlPrint);
	m_AlarmListButtons.EnableActionOnButtonDown();
	m_AlarmListButtons.SetBaseColor(CPanel::m_BaseColorButtons);
	m_AlarmListButtons.EnableWindow(FALSE);

	m_ctrlOK.SetBaseColor(CPanel::m_BaseColorButtons);

	m_CameraButtons.Add(&m_ctrlCam0);
	m_CameraButtons.Add(&m_ctrlCam1);
	m_CameraButtons.Add(&m_ctrlCam2);
	m_CameraButtons.Add(&m_ctrlCam3);
	m_CameraButtons.Add(&m_ctrlCam4);
	m_CameraButtons.Add(&m_ctrlCam5);
	m_CameraButtons.Add(&m_ctrlCam6);
	m_CameraButtons.Add(&m_ctrlCam7);
	m_CameraButtons.Add(&m_ctrlCam8);
	m_CameraButtons.Add(&m_ctrlCam9);
	m_CameraButtons.Add(&m_ctrlCam10);
	m_CameraButtons.Add(&m_ctrlCam11);
	m_CameraButtons.Add(&m_ctrlCam12);
	m_CameraButtons.Add(&m_ctrlCam13);
	m_CameraButtons.Add(&m_ctrlCam14);
	m_CameraButtons.Add(&m_ctrlCam15);
	m_CameraButtons.SetStyle(StyleCheck);
	m_CameraButtons.SetBaseColor(CPanel::m_BaseColorButtons);
	m_CameraButtons.EnableWindow(FALSE);

	m_AlarmButtons.Add(&m_ctrlActivity);
	m_AlarmButtons.Add(&m_ctrlExtern1);
	m_AlarmButtons.Add(&m_ctrlExtern2);
	m_AlarmButtons.Add(&m_ctrlAudio);
	m_AlarmButtons.SetStyle(StyleCheck);
	m_AlarmButtons.SetBaseColor(CPanel::m_BaseColorButtons);

	m_ilLineUp.Create(IDB_ALARMLIST_UP, 16, 0, SKIN_COLOR_KEY);
	m_ctrlLineUp.SetImageList(&m_ilLineUp);
	
	m_ilLineDown.Create(IDB_ALARMLIST_DOWN, 16, 0, SKIN_COLOR_KEY);
	m_ctrlLineDown.SetImageList(&m_ilLineDown);
	
	m_ilNextDay.Create(IDB_ALARMLIST_UP2, 16, 0, SKIN_COLOR_KEY);
	m_ctrlNextDay.SetImageList(&m_ilNextDay);
	
	m_ilPrevDay.Create(IDB_ALARMLIST_DOWN2, 16, 0, SKIN_COLOR_KEY);
	m_ctrlPrevDay.SetImageList(&m_ilPrevDay);

	m_ilOK.Create(IDB_ALARMLIST_ENTER, 16, 0, SKIN_COLOR_KEY);
	m_ctrlOK.SetImageList(&m_ilOK);

	m_ilPlay.Create(IDB_ALARMLIST_PLAY2, 16, 0, SKIN_COLOR_KEY);
	m_ctrlImage.SetImageList(&m_ilPlay);

	m_ilActivity.Create(IDB_ALARMLIST_ACTIVITY, 16, 0, SKIN_COLOR_KEY);
	m_ctrlActivity.SetImageList(&m_ilActivity);
	m_ilExtern1.Create(IDB_ALARMLIST_EXTERN1, 16, 0, SKIN_COLOR_KEY);
	m_ctrlExtern1.SetImageList(&m_ilExtern1);
	m_ilExtern2.Create(IDB_ALARMLIST_EXTERN2, 16, 0, SKIN_COLOR_KEY);
	m_ctrlExtern2.SetImageList(&m_ilExtern2);
	m_ilAudio.Create(IDB_ALARMLIST_AUDIO, 16, 0, SKIN_COLOR_KEY);
	m_ctrlAudio.SetImageList(&m_ilAudio);

	m_ilPrint.Create(IDB_ALARMLIST_PRINT, 24, 0, SKIN_COLOR_KEY);
	m_ctrlPrint.SetImageList(&m_ilPrint);
	
	m_pServer = new CServerAlarmlist(this);

	if (m_pServer && !m_pServer->IsDatabaseDisconnected())
	{
		CString sText;
		sText.LoadString(IDS_SEARCH_START);
		m_ctrlDisplayTitle.SetDisplayText(sText);

		if (m_sHost.IsEmpty())
			m_pServer->Connect(LOCAL_LOOP_BACK, m_sPIN);
		else
		{
			m_pServer->Connect(m_sHost, m_sPIN);
		}
	}

	CRect rcPanel;
	if (m_pPanel)
	{
		m_pPanel->GetWindowRect(rcPanel);

		CRect rcAlarmlistDlg;
		GetWindowRect(rcAlarmlistDlg);
		
		// AlarmListDialog oberhalb des Panels positionieren
		int nX	= rcPanel.right -rcAlarmlistDlg.Width();
		int	nY	= rcPanel.top - rcAlarmlistDlg.Height();

		if (nX < 0 || nY < 0)
		{
			// AlarmListDialog unterhalb des Panels positionieren
			nX	= rcPanel.left;
			nY	= rcPanel.bottom;
		}
		SetWindowPos(&wndTopMost, nX, nY, -1, -1, SWP_NOSIZE);
	}

	ShowWindow(SW_SHOW);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::OnPaint() 
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
		CTransparentDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
/////////////////////////////////////////////////////////////////////////////
HCURSOR CAlarmlistDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::OnCancel() 
{
	DestroyWindow();
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::OnOK() 
{
	DestroyWindow();
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::OnCameraButton() 
{
	UpdateDlg(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::OnExtern1() 
{
	m_nAlarmFilter[0] = m_ctrlExtern1.GetCheck();
	UpdateDlg(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::OnExtern2() 
{
	m_nAlarmFilter[1] = m_ctrlExtern2.GetCheck();
	UpdateDlg(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::OnActivity() 
{
	m_nAlarmFilter[2] = m_ctrlActivity.GetCheck();
	UpdateDlg(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::OnAlarmlistAudio() 
{
	m_nAlarmFilter[3] = m_ctrlAudio.GetCheck();
	UpdateDlg(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::OnScrollLineUp() 
{
	m_ctrlList.ScrollPageUp();
	UpdateLineUpDownButtons();
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::OnScrollLineDown() 
{
	m_ctrlList.ScrollPageDown();
	UpdateLineUpDownButtons();
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::OnScrollPrevDay() 
{
	// Ist das älteste Datum erreicht?
	if (m_pServer && m_pServer->IsDatabaseConnected())
	{
		CIPCDatabaseClientAlarmlist* pDatabase = m_pServer->GetDatabase();
		if (pDatabase)
		{
			CSystemTime time = pDatabase->GetFirstAlarmTime();
			BOOL bOK = (m_AlarmTime.GetDBDate() > time.GetDBDate());
			if (bOK)
			{
				CSystemTime DecTime(24);
				m_AlarmTime = m_AlarmTime - DecTime;
				m_ctrlDisplayTitle.SetDisplayText(m_AlarmTime.GetDate());
				RequestNewAlarmList();
			}
			m_ctrlPrevDay.EnableWindow(m_AlarmTime.GetDBDate() > time.GetDBDate());
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::OnScrollNextDay() 
{
	// In die Zukunft blicken bringt nichts :-)
	CSystemTime time;
	time.GetLocalTime();
	BOOL bOK = (m_AlarmTime.GetDBDate() < time.GetDBDate());
	if (bOK)
	{
		CSystemTime IncTime(24);
		m_AlarmTime = m_AlarmTime + IncTime;
		m_ctrlDisplayTitle.SetDisplayText(m_AlarmTime.GetDate());
		RequestNewAlarmList();
	}
	m_ctrlNextDay.EnableWindow(m_AlarmTime.GetDBDate() < time.GetDBDate());
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::OnDestroy() 
{
	CTransparentDialog::OnDestroy();
 	KillTimer(1);
	m_ctrlList.ResetContent();
	m_AlarmObjectList.SafeDeleteAll();
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::PostNcDestroy() 
{
	CTransparentDialog::PostNcDestroy();
	delete this;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CAlarmlistDlg::FilterAlarm(CAlarmObject* pAlarm)
{
	BOOL bResult = FALSE;
	if (pAlarm)
		bResult = m_CameraButtons.GetAtFast(pAlarm->GetAlarmCam())->GetCheck()
					&& m_nAlarmFilter[pAlarm->GetAlarmInput()];

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::UpdateDlg(BOOL bWithCameraButtons)
{
	CString sText;

	BOOL	bHaveDatabase = FALSE;

	if (bWithCameraButtons)
	{
		// Erstmal alle Camerabuttons deaktivieren
		m_CameraButtons.EnableWindow(FALSE);
	}	
	// Stelle fest von welchen Kameras Bilder gespeichert wurden. 
	if (m_pServer && m_pServer->IsDatabaseConnected())
	{
		CIPCDatabaseClientAlarmlist* pDatabase = m_pServer->GetDatabase();
		if (pDatabase)
		{
			bHaveDatabase = TRUE;

			// Auf jeden Fall Liste löschen
			m_ctrlList.ResetContent();

			// Überprüfen, ob überhaupt die Liste angezeigt werden muss
			BOOL bCamerasChecked = FALSE;
			BOOL bAlarmsChecked = FALSE;
			for (int i=0 ; i<m_CameraButtons.GetSize() ; i++)
			{
				bCamerasChecked |= (m_CameraButtons.GetAtFast(i))->GetCheck();
			}
			for (i=0 ; i<m_AlarmButtons.GetSize() ; i++)
			{
				bAlarmsChecked |= (m_AlarmButtons.GetAtFast(i))->GetCheck();
			}

			// Wenn keine Camera und kein Alarmtyp ausgewählt => keine Liste
			if (   bWithCameraButtons
				|| (bCamerasChecked && bAlarmsChecked)
				)
			{
				UpdateListBoxSortTime(bWithCameraButtons);
//				UpdateListBox(bWithCameraButtons);
			}

			// In die Zukunft blicken bringt nichts :-)
			CSystemTime time;
			time.GetLocalTime();
			m_ctrlNextDay.EnableWindow(m_AlarmTime.GetDBDate() < time.GetDBDate());

			// Ist das älteste Datum erreicht?
			time = pDatabase->GetFirstAlarmTime();
			m_ctrlPrevDay.EnableWindow(m_AlarmTime.GetDBDate() > time.GetDBDate());
		}
	}

	// Ist die Liste leer?
	BOOL bNotEmpty = (m_ctrlList.GetCount() > 0); 
	m_ctrlLineUp.EnableWindow(bNotEmpty);
	m_ctrlLineDown.EnableWindow(bNotEmpty);
	m_ctrlPrint.EnableWindow(bNotEmpty && m_pPanel->GetDefaultPrinter() != 0);
	m_ctrlImage.EnableWindow(bNotEmpty);
	
	m_ctrlExtern1.EnableWindow(bHaveDatabase);
	m_ctrlExtern2.EnableWindow(bHaveDatabase);
	m_ctrlActivity.EnableWindow(bHaveDatabase);	
	m_ctrlAudio.EnableWindow(bHaveDatabase);	

	m_ctrlImage.EnableWindow(m_ctrlList.GetCurSel() != CB_ERR);

	CString sTitle;
	sTitle.Format(_T("%s [%d]"), m_AlarmTime.GetDate(), m_ctrlList.GetCount());
	m_ctrlDisplayTitle.SetDisplayText(sTitle);

	UpdateLineUpDownButtons();

	Invalidate(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::UpdateLineUpDownButtons()
{
	BOOL bReturn = FALSE;
	CRect rc;
	m_ctrlList.GetClientRect(rc);
	BOOL bOutside = TRUE;
	
	CRect rectItem;
	m_ctrlList.GetItemRect(0, rectItem);
 
	if (rectItem.Height() != 0)
	{
		rc.bottom -= rectItem.Height();
	}
	int iTopIndex = m_ctrlList.GetTopIndex();
	int iLastVisibleIndex = (int)m_ctrlList.ItemFromPoint(rc.BottomRight(), bOutside);
	int iMaxIndex = m_ctrlList.GetCount() - 1;
	if (iLastVisibleIndex < iMaxIndex)
	{
		bReturn = TRUE;
	}
	m_ctrlLineDown.EnableWindow(bReturn);

	bReturn = FALSE;
	iTopIndex = m_ctrlList.GetTopIndex();
	if (iTopIndex > 0)
	{
		bReturn = TRUE;
	}
	m_ctrlLineUp.EnableWindow(bReturn);
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::OnTimer(UINT nIDEvent) 
{
	static DWORD dwTCPrinter = GetTickCount();

	if (m_pServer)
	{
		m_pServer->OnIdle();
	}

	// Im Sekundentakt nachschauen, ob ein Drucker eingestellt wurde.
	if (GetTimeSpan(dwTCPrinter) > 1000)
	{
		dwTCPrinter = GetTickCount();
		if (m_pPanel)
		{
			// Drucker vorhanden?
			if (!m_pPanel->IsPrinterAvailable())
				m_ctrlPrint.ShowWindow(SW_HIDE);
			// Drucker ausgewählt?
			m_ctrlPrint.EnableWindow((m_pPanel->GetDefaultPrinter() != 0) &&
									 (m_ctrlList.GetCount() > 0));
		}
	}

	// Test Random Alarmlist 10 min = 600000
/*	static DWORD dwTCTestRandom = GetTickCount();
	static BOOL bTestRandomDayPrev = FALSE;
	if (GetTimeSpan(dwTCTestRandom) > 60000)
	{
		dwTCTestRandom = GetTickCount();
		if (bTestRandomDayPrev)
		{
			if (m_ctrlPrevDay.IsWindowEnabled())
			{
				OnScrollPrevDay();
			}
			else
			{
				bTestRandomDayPrev = FALSE;
				// go on next time
			}
		}
		else
		{
			if (m_ctrlNextDay.IsWindowEnabled())
			{
				OnScrollNextDay();
			}
			else
			{
				bTestRandomDayPrev = TRUE;
				// go on next time
			}
		}
	}
*/	CTransparentDialog::OnTimer(nIDEvent);
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::NotifyConnect(BOOL bOK, int iErrorCode)
{
	PostMessage(WM_NOTIFY_CONNECT, bOK, iErrorCode);
}

/////////////////////////////////////////////////////////////////////////////
long CAlarmlistDlg::OnNotifyConnect(WPARAM wParam, LPARAM lParam)
{
	if (wParam)
	{
		WK_TRACE(_T("Connecting to Database complete\n"));
	}
	else
	{
		WK_TRACE(_T("Can't connect to Database\n"));
	}
	m_AlarmTime.GetLocalTime();

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
long CAlarmlistDlg::OnNotifyAlarmListUpdate(WPARAM wParam, LPARAM lParam)
{
	m_bRequesting = FALSE;
	if (m_pServer)
	{
		CIPCDatabaseClientAlarmlist* pDatabase = m_pServer->GetDatabase();
		if (pDatabase)
		{
			if (pDatabase->GetRequestedAlarmTime() == m_AlarmTime)
			{
				CopyAlarmObjectList(pDatabase->GetAlarmObjectList());
				UpdateDlg(TRUE);
			}
			else
			{
				// neue Liste anfordern
				RequestNewAlarmList();
			}
		}
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::NotifyDisconnect()
{
	PostMessage(WM_NOTIFY_DISCONNECT);
}

/////////////////////////////////////////////////////////////////////////////
long CAlarmlistDlg::OnNotifyDisconnect(WPARAM wParam, LPARAM lParam)
{
	WK_TRACE(_T("Disconnecting to Database complete\n"));

	m_AlarmListButtons.EnableWindow(FALSE);
	m_CameraButtons.EnableWindow(FALSE);

	return 0;
}

void CAlarmlistDlg::OnCaptureChanged(CWnd *pWnd) 
{
	// TODO: Add your message handler code here
	
	CTransparentDialog::OnCaptureChanged(pWnd);
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::RequestNewAlarmList()
{
	if (   m_pServer
		&& !m_bRequesting
		)
	{
		CIPCDatabaseClientAlarmlist* pDatabase = m_pServer->GetDatabase();
		if (pDatabase)
		{
			CString sText;
			sText.LoadString(IDS_SEARCH_START);
			m_ctrlList.ResetContent();
			m_ctrlList.AddString(sText);
			m_ctrlList.UpdateWindow();
			//m_AlarmListButtons.EnableWindow(FALSE);
			m_bRequesting = TRUE;
			pDatabase->RequestAlarmListArchives(m_AlarmTime);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
long CAlarmlistDlg::OnNotifyMessageScrolled(WPARAM wParam, LPARAM lParam)
{
	if (::GetDlgCtrlID((HWND)wParam) == IDC_ALARMLIST_LIST)
	{
		// Das Einfügen der Ordnungsnummern kostet wahnsinnig viel Zeit bei vielen Alarmen
		// Daher werden Abschnittsweise nur die sichtbaren angehängt
		AddListBoxOrderNumbers();
	}

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
long CAlarmlistDlg::OnNotifyMessageScrollEnd(WPARAM wParam, LPARAM lParam)
{
	if (::GetDlgCtrlID((HWND)wParam) == IDC_ALARMLIST_LIST)
	{
		m_ctrlLineDown.EnableWindow(!lParam);
		m_ctrlLineUp.EnableWindow(lParam);
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::OnPrint() 
{						    
	CString sError		 = _T("");
	CString sTimestamp = _T("");
	CString sConfirm	 = _T("");
	CString sRteNr		 = _T("");
	CString sMsg		 = _T("");
	CString	sTmp		 = _T("");

	int		nSpalten	= 2;

	CPrintDialog dlg(FALSE);
	dlg.m_pd.Flags &= ~PD_RETURNDC;
	if (dlg.GetDefaults())
	{
		DEVMODE *pDM = dlg.GetDevMode();
		pDM->dmOrientation  = DMORIENT_PORTRAIT;
		pDM->dmPrintQuality = DMRES_DRAFT;
		::GlobalUnlock(dlg.m_pd.hDevMode);

		HDC hDCPrint = dlg.CreatePrinterDC();
		if (hDCPrint)
		{
			CDC dcPrint;
			dcPrint.Attach(hDCPrint);
			dcPrint.m_bPrinting = TRUE;

			DOCINFO di;
			ZeroMemory(&di, sizeof(DOCINFO));
			di.cbSize = sizeof(DOCINFO);
			di.lpszDocName = _T("logevents");

			if (dcPrint.StartDoc(&di) != SP_ERROR)
			{
				if (dcPrint.StartPage() >= 0)
				{	
					// Geeigneten nicht proportionalen Font auswählen
					CFont font;
					font.CreatePointFont(8*10, GetFixFontFaceName(), &dcPrint);
					CFont* pOldFont = (CFont*)dcPrint.SelectObject(&font);

					// Einen Stift für die Trennlinien auswählen
					CPen pen(PS_SOLID, 4, RGB(0,0,0));
					CPen* pOldPen = dcPrint.SelectObject(&pen);

					CSize  szPrint(dcPrint.GetDeviceCaps(HORZRES), dcPrint.GetDeviceCaps(VERTRES));
					CPoint ptOffset(dcPrint.GetDeviceCaps(PHYSICALOFFSETX), dcPrint.GetDeviceCaps(PHYSICALOFFSETY)); 
					CRect  rcPage(ptOffset, szPrint);
					rcPage.DeflateRect(200,200,200, 200);
/*
					CRect rcPage(0, 0, dcPrint.GetDeviceCaps(HORZRES), dcPrint.GetDeviceCaps(VERTRES));
					rcPage.DeflateRect(200,200,200, 200);
*/
					dcPrint.DPtoLP(&rcPage);
					
					int nYPos	= 0;
					int nXPos	= 0;
					int nIndex	= 0;
					int nPage	= 0;
					do
					{
						if (nPage > 0)
						{
							dcPrint.StartPage();
						}
						// Wenn kein Hostname verfügbar, dann ist's local -> dann den lokalen Hostname ausdrucken
						CString sHost = m_pPanel->GetHostName();
						if (sHost.IsEmpty())
							sHost = m_pPanel->GetLocalHostName();

						// Datum holen....
						CSystemTime time = ((CAlarmObject*)m_ctrlList.GetItemDataPtr(0))->GetAlarmTime();
						sMsg = time.GetDate() + _T(" ") + sHost;

						// ...und als Überschrift ausgeben.
						CRect rcText(rcPage.left, rcPage.top, rcPage.right, rcPage.bottom);
						dcPrint.DrawText(sMsg, rcText, DT_CALCRECT | DT_LEFT|DT_WORDBREAK);
						dcPrint.DrawText(sMsg, rcText, DT_LEFT|DT_WORDBREAK);				
					
						int nMaxLinesPerPage = max(rcPage.Height() / rcText.Height() - 4, 0);
						int nMaxPages = m_ctrlList.GetCount() / (nMaxLinesPerPage * nSpalten) + 1;
						
						// Aktuelle Seitennummer ebenfalls ausgeben
						sMsg.Format(_T("%d/%d"), ++nPage, nMaxPages);					
						CRect rcText2(rcPage.left, rcPage.top, rcPage.right, rcPage.bottom);
						dcPrint.DrawText(sMsg, rcText2, DT_RIGHT|DT_SINGLELINE);				

						// Waagerechte Trennlinie zeichnen
						dcPrint.MoveTo(rcPage.left, rcPage.top+rcText.Height());
						dcPrint.LineTo(rcPage.right + rcPage.Width(), rcPage.top+rcText.Height());

						nXPos = 0;
						for (int nJ = 0; nJ < nSpalten; nJ++)
						{
							nYPos = rcText.Height();
							for (int nI = 0; nI < nMaxLinesPerPage; nI++)
							{
								if (nIndex < m_ctrlList.GetCount()) 
								{
									CAlarmObject* pAlarm = (CAlarmObject*)m_ctrlList.GetItemDataPtr(nIndex);
									if (pAlarm)
									{
										// Eine Zeile weiter					
										nYPos += rcText.Height(); 

										// Text ausgeben.
										sMsg.Format(_T("   %5d.) %s"), nIndex+1, pAlarm->GetFormatPrintString());
										CRect rcText(rcPage.left+nXPos, rcPage.top+nYPos, rcPage.right, rcPage.top);
										dcPrint.DrawText(sMsg, rcText, DT_CALCRECT | DT_LEFT|DT_WORDBREAK);
										dcPrint.DrawText(sMsg, rcText, DT_LEFT|DT_WORDBREAK);
									}
									nIndex++;
								}
							}
							nXPos += rcPage.Width() / nSpalten;	

							// Vertikale Trennlinie zeichnen
							dcPrint.MoveTo(rcPage.left + rcPage.Width()/2, rcPage.top+rcText.Height());
							dcPrint.LineTo(rcPage.left + rcPage.Width()/2, rcPage.bottom);
						}	
						dcPrint.EndPage();
					}
					while (nIndex < m_ctrlList.GetCount());						
					dcPrint.SelectObject(pOldFont);
					pOldFont->DeleteObject();
				
					dcPrint.SelectObject(pOldPen);
					pOldPen->DeleteObject();
				}
				dcPrint.EndDoc();
			}
			dcPrint.Detach();
			DeleteDC(hDCPrint);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::OnAlarmlistImage() 
{
	int nSel = m_ctrlList.GetCurSel();
	if (nSel != CB_ERR)
	{
		CAlarmObject* pAlarm = (CAlarmObject*)m_ctrlList.GetItemDataPtr(nSel);
		if (pAlarm)
		{
			CString sDate, sTime;
			CSystemTime time = pAlarm->GetAlarmTime();
			sDate.Format(_T("%02d%02d%02d"), time.GetDay(), time.GetMonth(), time.GetYear()-2000);
			sTime = time.GetDBTime();
			int nCam = pAlarm->GetAlarmCam();

			if (m_pPanel)
			{
				m_pPanel->SetCamera(nCam, Play);
				if (!m_pPanel->OnStartDatabaseSearch(sDate, sTime, nCam))
					WK_TRACE_WARNING(_T("CAlarmlistDlg::OnAlarmlistImage() searching failed\n"));
			}

		}
	}	
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::OnDblclkAlarmlistList() 
{
	OnAlarmlistImage();
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::OnSelchangeAlarmlistList() 
{
	m_ctrlImage.EnableWindow(m_ctrlList.GetCurSel() != CB_ERR);
	UpdateLineUpDownButtons();
	// Kann auch beim Tasten-Scrollen sein
	AddListBoxOrderNumbers();
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::OnAlarmlistOk() 
{
	OnOK();	
}
/////////////////////////////////////////////////////////////////////////////
BOOL CAlarmlistDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (m_hCursor)
	{
		if (SetCursor(m_hCursor) != NULL)
			return TRUE;		
	}	
	
	return CTransparentDialog::OnSetCursor(pWnd, nHitTest, message);
}
//////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::CopyAlarmObjectList(CAlarmObjectListCS& AlarmList)
{
	// Copy AlarmObjectList for presentation
//	CWK_StopWatch watch;
	// Da das Löschen bei vielen Alarmen recht lange dauert,
	// wird nach gewisser Zeit einmal confirmiert
// Geht doch ausreichend schnell.
//	DWORD dwStartTime = GetTickCount();
//	DWORD dwActualTime;

	AlarmList.Lock(_T(__FUNCTION__));
	m_AlarmObjectList.SafeDeleteAll();
	POSITION pos = AlarmList.GetHeadPosition();
	while (pos != NULL)
	{
		m_AlarmObjectList.AddTail(new CAlarmObject(AlarmList.GetNext(pos)));
		// Da das Löschen bei vielen Alarmen recht lange dauert,
		// wird nach timeOut einmal confirmiert.
/*		dwActualTime = GetTickCount();
		if (GetTimeSpan(dwStartTime, dwActualTime) > SELFCHECK_TIMEOUT)
		{
			dwStartTime = dwActualTime;
			m_pPanel->ForceConfirmSelfcheck();
		}
*/	}
	AlarmList.Unlock();
//	watch.StopWatch(_T("CopyAlarmObjectList"));
}
//////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::UpdateListBox(BOOL bWithCameraButtons)
{
	m_ctrlList.ResetContent();
	CString sText;
	sText.LoadString(IDS_SEARCH_START);
	m_ctrlList.AddString(sText);
	m_ctrlList.UpdateWindow();
	m_ctrlList.SetRedraw(FALSE);
	m_ctrlList.ResetContent();

	// für die korrekte Anzahl führender Leerzeichen
	// maximale Länge einer Nummer ermitteln.
	CString sNr;
	sNr.Format(_T("%d"),m_AlarmObjectList.GetCount());
	int iMaxNrLength = sNr.GetLength();

	// Da das Durchsuchen bei vielen Alarmen recht lange dauert,
	// wird nach gewisser Zeit einmal confirmiert.
	DWORD dwStartTime = GetTickCount();
	DWORD dwActualTime;

	CWK_StopWatch watch;
	CAlarmObject* pAlarmObject = NULL;
	CString sEntry;
	int nIndex;
	POSITION pos = m_AlarmObjectList.GetHeadPosition();
	while (pos != NULL)
	{
		pAlarmObject = m_AlarmObjectList.GetNext(pos);	
		if (pAlarmObject)
		{
			if (bWithCameraButtons)
			{
				m_CameraButtons.GetAtFast(pAlarmObject->GetAlarmCam())->EnableWindow(TRUE);
			}
			// entspricht das Alarmobjekt den Kriterien?
			if (FilterAlarm(pAlarmObject))
			{
				// die Nummer formatieren
				sNr.Format(_T("%d"),m_ctrlList.GetCount()+1);
				// passende Anzahl Leerzeichen erzeugen
				CString sBlanks(_T(' '),iMaxNrLength-sNr.GetLength());
				// alles aneinander ketten
				sEntry = sBlanks + sNr + _T(" ") + pAlarmObject->GetFormatString();
				// in die Liste einfügen und Pointer setzen
				nIndex = m_ctrlList.AddString(sEntry);
				m_ctrlList.SetItemDataPtr(nIndex, pAlarmObject);
			}
		}
		// Da das Durchsuchen bei vielen Alarmen recht lange dauert,
		// wird nach timeOut einmal confirmiert.
		dwActualTime = GetTickCount();
		if (GetTimeSpan(dwStartTime, dwActualTime) > SELFCHECK_TIMEOUT)
		{
			dwStartTime = dwActualTime;
			m_pPanel->ForceConfirmSelfcheck();
		}
	} // end while

	if (bStopWatch)
	{
		CString sMsg;
		sMsg.Format(_T("AlarmlistSearch for %s"), m_AlarmTime.GetDate());
		watch.StopWatch(sMsg);
	}
	m_ctrlList.SetRedraw(TRUE);
	m_ctrlList.UpdateWindow();
}
//////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::UpdateListBoxSortTime(BOOL bWithCameraButtons)
{
	m_ctrlList.ResetContent();
	CString sText;
	sText.LoadString(IDS_SEARCH_START);
	m_ctrlList.AddString(sText);
	m_ctrlList.UpdateWindow();
	m_ctrlList.SetRedraw(FALSE);
	m_ctrlList.ResetContent();

	// Da das Durchsuchen bei vielen Alarmen recht lange dauert,
	// wird nach gewisser Zeit einmal confirmiert.
	DWORD dwStartTime = GetTickCount();
	DWORD dwActualTime;

	CWK_StopWatch watch;
	CAlarmObject* pAlarmObject = NULL;
	int iIndex;
	POSITION pos = m_AlarmObjectList.GetHeadPosition();
	while (pos != NULL)
	{
		pAlarmObject = m_AlarmObjectList.GetNext(pos);	
		if (pAlarmObject)
		{
			if (bWithCameraButtons)
			{
				m_CameraButtons.GetAtFast(pAlarmObject->GetAlarmCam())->EnableWindow(TRUE);
			}
			// entspricht das Alarmobjekt den Kriterien?
			if (FilterAlarm(pAlarmObject))
			{
				// in die Liste einfügen und Pointer setzen
				iIndex = m_ctrlList.AddString(pAlarmObject->GetFormatStringTimeFirst());
				m_ctrlList.SetItemDataPtr(iIndex, pAlarmObject);
			}
		}
		// Da das Durchsuchen bei vielen Alarmen recht lange dauern kann,
		// wird nach timeOut einmal confirmiert.
		dwActualTime = GetTickCount();
		if (GetTimeSpan(dwStartTime, dwActualTime) > SELFCHECK_TIMEOUT)
		{
			dwStartTime = dwActualTime;
			m_pPanel->ForceConfirmSelfcheck();
		}
	} // end while

	// Das Einfügen der Ordnungsnummer kostet wahnsinnig viel Zeit bei vielen Alarmen
	// Daher werden Abschnittsweise nur die sichtbaren angehängt
	AddListBoxOrderNumbers();

	if (bStopWatch)
	{
		CString sMsg;
		sMsg.Format(_T("AlarmlistSearch Sort for %s"), m_AlarmTime.GetDate());
		watch.StopWatch(sMsg);
	}
	m_ctrlList.SetRedraw(TRUE);
	m_ctrlList.UpdateWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::AddListBoxOrderNumbers()
{
// Einfuegen der Ordnungsnummern in die ListBox
	m_ctrlList.SetRedraw(FALSE);
	if (m_ctrlList.GetCount() > 0)
	{
		CRect rc;
		m_ctrlList.GetClientRect(rc);
		BOOL bOutside = TRUE;
		int iTopIndex = m_ctrlList.GetTopIndex();
		int iLastVisibleIndex = (int)m_ctrlList.ItemFromPoint(rc.BottomRight(), bOutside);

		CString sEntry, sObjectText, sNr;
		CAlarmObject* pAlarmObject = NULL;
		for (int i=iTopIndex ; i<=iLastVisibleIndex ; i++)
		{
			m_ctrlList.GetText(i, sEntry);
			pAlarmObject = (CAlarmObject*)m_ctrlList.GetItemDataPtr(i);
			if (pAlarmObject)
			{
				if (pAlarmObject->GetFormatStringTimeFirst().GetLength() < sEntry.GetLength())
				{
					// Nummer bereits vorhanden!
				}
				else
				{
					m_ctrlList.DeleteString(i);
					// die Nummer formatieren
					sNr.Format(_T("% 5u"),i+1);
					// anhaengen und neuen Text einfügen
					m_ctrlList.InsertString(i, sEntry+sNr);
					m_ctrlList.SetItemDataPtr(i, pAlarmObject);
				}
			}
		} // end for
		// Beim Einfügen wird die Listbox an den Anfang gescrollt
		m_ctrlList.SetTopIndex(iTopIndex);
	}
	m_ctrlList.SetRedraw(TRUE);
}

