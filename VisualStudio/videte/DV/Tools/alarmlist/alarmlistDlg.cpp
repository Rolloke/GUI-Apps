// alarmlistDlg.cpp : implementation file
//

#include "stdafx.h"
#include "alarmlist.h"
#include "alarmlistDlg.h"
#include "AlarmObject.h"
#include "ServerAlarmlist.h"
#include "IPCDatabaseClientAlarmlist.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int CAlarmlistDlg::m_nWM_ScrollEnd = 0;

/////////////////////////////////////////////////////////////////////////////
// CAlarmlistDlg dialog

/////////////////////////////////////////////////////////////////////////////
CAlarmlistDlg::CAlarmlistDlg(const CString& sHost, const CString& sPIN, CWnd* pParent /*=NULL*/)
	: CTransparentDialog(CAlarmlistDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAlarmlistDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	for (int nI = 0; nI < 3; nI++)
		m_nAlarmFilter[nI] = 0;
	
	m_pServer = NULL;
	m_nWM_ScrollEnd = 0;
	m_sHost	= sHost;
	m_sPIN	= sPIN;
}

/////////////////////////////////////////////////////////////////////////////
CAlarmlistDlg::~CAlarmlistDlg()
{
	WK_DELETE(m_pServer);
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::DoDataExchange(CDataExchange* pDX)
{
	CTransparentDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAlarmlistDlg)
	DDX_Control(pDX, IDC_LIST, m_ctrlList);
	DDX_Control(pDX, IDC_DOWN2, m_ctrlNextDay);
	DDX_Control(pDX, IDC_UP2, m_ctrlPrevDay);
	DDX_Control(pDX, IDC_IMAGE, m_ctrlImage);
	DDX_Control(pDX, IDC_EXTERN2, m_ctrlExtern2);
	DDX_Control(pDX, IDC_EXTERN1, m_ctrlExtern1);
	DDX_Control(pDX, IDC_ACTIVITY, m_ctrlActivity);
	DDX_Control(pDX, IDC_CAM0,  m_ctrlCam[0]);
	DDX_Control(pDX, IDC_CAM1,  m_ctrlCam[1]);
	DDX_Control(pDX, IDC_CAM2,  m_ctrlCam[2]);
	DDX_Control(pDX, IDC_CAM3,  m_ctrlCam[3]);
	DDX_Control(pDX, IDC_CAM4,  m_ctrlCam[4]);
	DDX_Control(pDX, IDC_CAM5,  m_ctrlCam[5]);
	DDX_Control(pDX, IDC_CAM6,  m_ctrlCam[6]);
	DDX_Control(pDX, IDC_CAM7,  m_ctrlCam[7]);
	DDX_Control(pDX, IDC_CAM8,  m_ctrlCam[8]);
	DDX_Control(pDX, IDC_CAM9,  m_ctrlCam[9]);
	DDX_Control(pDX, IDC_CAM10, m_ctrlCam[10]);
	DDX_Control(pDX, IDC_CAM11, m_ctrlCam[11]);
	DDX_Control(pDX, IDC_CAM12, m_ctrlCam[12]);
	DDX_Control(pDX, IDC_CAM13, m_ctrlCam[13]);
	DDX_Control(pDX, IDC_CAM14, m_ctrlCam[14]);
	DDX_Control(pDX, IDC_CAM15, m_ctrlCam[15]);
	DDX_Control(pDX, IDC_PRINT, m_ctrlPrint);
	DDX_Control(pDX, IDC_UP,	m_ctrlLineUp);
	DDX_Control(pDX, IDC_DOWN, m_ctrlLineDown);
	DDX_Control(pDX, IDOK, m_ctrlOK);
	DDX_Control(pDX, IDC_DISPLAY_TITLE,  m_ctrlDisplayTitle);
	DDX_Control(pDX, IDC_DISPLAY_BORDER, m_ctrlDisplayBorder);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CAlarmlistDlg, CTransparentDialog)
	//{{AFX_MSG_MAP(CAlarmlistDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CAM0, OnCam0)
	ON_BN_CLICKED(IDC_CAM1, OnCam1)
	ON_BN_CLICKED(IDC_CAM2, OnCam2)
	ON_BN_CLICKED(IDC_CAM3, OnCam3)
	ON_BN_CLICKED(IDC_CAM4, OnCam4)
	ON_BN_CLICKED(IDC_CAM5, OnCam5)
	ON_BN_CLICKED(IDC_CAM6, OnCam6)
	ON_BN_CLICKED(IDC_CAM7, OnCam7)
	ON_BN_CLICKED(IDC_CAM8, OnCam8)
	ON_BN_CLICKED(IDC_CAM9, OnCam9)
	ON_BN_CLICKED(IDC_CAM10, OnCam10)
	ON_BN_CLICKED(IDC_CAM11, OnCam11)
	ON_BN_CLICKED(IDC_CAM12, OnCam12)
	ON_BN_CLICKED(IDC_CAM13, OnCam13)
	ON_BN_CLICKED(IDC_CAM14, OnCam14)
	ON_BN_CLICKED(IDC_CAM15, OnCam15)
	ON_BN_CLICKED(IDC_UP, OnScrollLineUp)
	ON_BN_CLICKED(IDC_UP2, OnScrollNextDay)
	ON_BN_CLICKED(IDC_DOWN, OnScrollLineDown)
	ON_BN_CLICKED(IDC_DOWN2, OnScrollPrevDay)
	ON_WM_DESTROY()
	ON_WM_CANCELMODE()
	ON_BN_CLICKED(IDC_ACTIVITY, OnActivity)
	ON_BN_CLICKED(IDC_EXTERN1, OnExtern1)
	ON_BN_CLICKED(IDC_EXTERN2, OnExtern2)
	ON_BN_CLICKED(IDC_PRINT, OnPrint)

	ON_WM_TIMER()
	ON_WM_CAPTURECHANGED()
	ON_MESSAGE(WM_NOTIFY_CONNECT, OnNotifyConnect)
	ON_MESSAGE(WM_NOTIFY_DISCONNECT, OnNotifyDisconnect)
	ON_MESSAGE(WM_NOTIFY_ALARMLIST_UPDATE, OnNotifyAlarmListUpdate)
	ON_REGISTERED_MESSAGE(m_nWM_ScrollEnd, OnNotifyMessageScrollEnd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAlarmlistDlg message handlers

/////////////////////////////////////////////////////////////////////////////
BOOL CAlarmlistDlg::OnInitDialog()
{
	CTransparentDialog::OnInitDialog();

	SetTimer(1, 250, NULL);

	CreateTransparent(StyleBackGroundColorChangeBrushed, SKIN_COLOR_GOLD_METALLIC_LIGHT);

	m_nWM_ScrollEnd = RegisterWindowMessage(SKIN_MSG_SCROLL_END);

	m_ctrlDisplayTitle.OpenStockDisplay(this, Display2);
	// Kontrast erhöhen
	DSPCOL DisplayColors;
	DisplayColors.colText = RGB(0,0,0);
	DisplayColors.dwFlags = DSP_COL_TEXT;
	m_ctrlDisplayTitle.SetDisplayColors(DisplayColors);

	
	m_ctrlDisplayBorder.CreateFrame(this);
	m_ctrlDisplayBorder.EnableShadow(TRUE);

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	m_ctrlLineUp.EnableAutoRepeat();
	m_ctrlLineDown.EnableAutoRepeat();

	m_AlarmListButtons.Add(&m_ctrlOK);
	m_AlarmListButtons.Add(&m_ctrlLineUp);
	m_AlarmListButtons.Add(&m_ctrlLineDown);
	m_AlarmListButtons.Add(&m_ctrlPrevDay);
	m_AlarmListButtons.Add(&m_ctrlNextDay);
	m_AlarmListButtons.Add(&m_ctrlPrint);
	m_AlarmListButtons.Add(&m_ctrlImage);

	for (int nI = 0; nI < 16; nI++)
	{
		m_AlarmListButtons.Add(&m_ctrlCam[nI]);
		m_ctrlCam[nI].SetStyle(StyleCheck);
	}

	m_AlarmListButtons.Add(&m_ctrlActivity);
	m_AlarmListButtons.Add(&m_ctrlExtern1);
	m_AlarmListButtons.Add(&m_ctrlExtern2);

	m_ctrlActivity.SetStyle(StyleCheck);
	m_ctrlExtern1.SetStyle(StyleCheck);
	m_ctrlExtern2.SetStyle(StyleCheck);

	m_AlarmListButtons.SetBaseColor(SKIN_COLOR_GOLD_METALLIC);

	m_ilLineUp.Create(IDB_UP, 16, 0, RGB(255,0,255));
	m_ctrlLineUp.SetImageList(&m_ilLineUp);
	
	m_ilLineDown.Create(IDB_DOWN, 16, 0, RGB(255,0,255));
	m_ctrlLineDown.SetImageList(&m_ilLineDown);
	
	m_ilPrevDay.Create(IDB_UP2, 16, 0, RGB(255,0,255));
	m_ctrlPrevDay.SetImageList(&m_ilPrevDay);
	
	m_ilNextDay.Create(IDB_DOWN2, 16, 0, RGB(255,0,255));
	m_ctrlNextDay.SetImageList(&m_ilNextDay);

	m_ilOK.Create(IDB_ENTER, 16, 0, RGB(255,0,255));
	m_ctrlOK.SetImageList(&m_ilOK);

	m_ilPlay.Create(IDB_PLAY2, 16, 0, RGB(255,0,255));
	m_ctrlImage.SetImageList(&m_ilPlay);

	m_ilActivity.Create(IDB_ACTIVITY, 16, 0, RGB(255,0,255));
	m_ctrlActivity.SetImageList(&m_ilActivity);
	m_ilExtern1.Create(IDB_EXTERN1, 16, 0, RGB(255,0,255));
	m_ctrlExtern1.SetImageList(&m_ilExtern1);
	m_ilExtern2.Create(IDB_EXTERN2, 16, 0, RGB(255,0,255));
	m_ctrlExtern2.SetImageList(&m_ilExtern2);

	m_ilPrint.Create(IDB_PRINT, 24, 0, RGB(255,0,255));
	m_ctrlPrint.SetImageList(&m_ilPrint);
	
	m_ctrlList.SetBackgroundColorNormal(RGB(134,186,133));
	m_ctrlList.SetBackgroundColorHighlighted(RGB(134/2,186/2,133/2));
	m_ctrlList.SetTextColorNormal(RGB(0,0,0));

	m_AlarmListButtons.EnableWindow(FALSE);

	m_pServer = new CServerAlarmlist(this);

	if (m_pServer && !m_pServer->IsDatabaseDisconnected())
	{
		if (m_sHost.IsEmpty())
			m_pServer->Connect(m_sHost, m_sPIN);
		else
		{
			if (m_sHost.Left(4).Find(".",0) != -1)
			{
				m_pServer->Connect("tcp:"+m_sHost, m_sPIN);
			}
			else
			{
				m_pServer->Connect(m_sHost, m_sPIN);
			}
		}
	}



	SetWindowPos(&wndTopMost, 0,0,0,0, SWP_NOSIZE |SWP_NOMOVE);

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
	// TODO: Add extra cleanup here
	
	CTransparentDialog::OnCancel();
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::OnOK() 
{
	CTransparentDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::OnCam0() 
{
	UpdateDlg();
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::OnCam1() 
{
	UpdateDlg();
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::OnCam2() 
{
	UpdateDlg();
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::OnCam3() 
{
	UpdateDlg();
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::OnCam4() 
{
	UpdateDlg();
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::OnCam5() 
{
	UpdateDlg();
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::OnCam6() 
{
	UpdateDlg();
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::OnCam7() 
{
	UpdateDlg();
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::OnCam8() 
{
	UpdateDlg();
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::OnCam9() 
{
	UpdateDlg();
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::OnCam10() 
{
	UpdateDlg();
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::OnCam11() 
{
	UpdateDlg();
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::OnCam12() 
{
	UpdateDlg();
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::OnCam13() 
{
	UpdateDlg();
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::OnCam14() 
{
	UpdateDlg();
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::OnCam15() 
{
	UpdateDlg();
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::OnExtern1() 
{
	m_nAlarmFilter[0] = m_ctrlExtern1.GetCheck();
	UpdateDlg();
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::OnExtern2() 
{
	m_nAlarmFilter[1] = m_ctrlExtern2.GetCheck();
	UpdateDlg();
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::OnActivity() 
{
	m_nAlarmFilter[2] = m_ctrlActivity.GetCheck();
	UpdateDlg();
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::OnScrollLineUp() 
{
	m_ctrlList.ScrollLineUp();
	UpdateLineUpDownButtons();
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::OnScrollLineDown() 
{
	m_ctrlList.ScrollLineDown();
	UpdateLineUpDownButtons();
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::OnScrollPrevDay() 
{
	CSystemTime DecTime(24);

	m_AlarmTime = m_AlarmTime - DecTime;
	RequestNewAlarmList();
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::OnScrollNextDay() 
{
	CSystemTime IncTime(24);

	m_AlarmTime = m_AlarmTime + IncTime;
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::OnDestroy() 
{
	CTransparentDialog::OnDestroy();
 	KillTimer(1);

	ClearListBox();
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::OnCancelMode() 
{
	CTransparentDialog::OnCancelMode();
	
	// TODO: Add your message handler code here
	
}

/////////////////////////////////////////////////////////////////////////////
BOOL CAlarmlistDlg::ClearListBox()
{
	int nCount = m_ctrlList.GetCount();
	for (int nI = 0; nI < nCount; nI++)
	{
		CAlarmObject* pAlarmObject = (CAlarmObject*)m_ctrlList.GetItemDataPtr(0);
		WK_DELETE(pAlarmObject);
		m_ctrlList.DeleteString(0);
	}
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CAlarmlistDlg::FilterAlarm(CAlarmObject* pAlarm)
{
	BOOL bResult = FALSE;
	if (pAlarm)
		bResult = m_ctrlCam[pAlarm->GetAlarmCam()].GetCheck() && m_nAlarmFilter[pAlarm->GetAlarmInput()];

	return bResult;

}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::UpdateDlg()
{
	ClearListBox();
	
	m_AlarmListButtons.EnableWindow(TRUE);

	// Erstmal alle Camerabuttons deaktivieren
	for (int nI = 0; nI < 16; nI++)
		m_ctrlCam[nI].EnableWindow(FALSE);

	
	// Stelle fest von welchen kameras Bilder gespeichert wurden. 
	if (m_pServer && m_pServer->IsDatabaseConnected())
	{
		CIPCDatabaseClientAlarmlist* pDatabase = m_pServer->GetDatabase();
		if (pDatabase)
		{
			CObList& AlarmList = pDatabase->GetAlarmObjectList();	
			for (nI = 0; nI < AlarmList.GetCount(); nI++)
			{
				CAlarmObject* pAlarmObject = (CAlarmObject*)AlarmList.GetAt(AlarmList.FindIndex(nI));	
				if (pAlarmObject)
				{
					m_ctrlCam[pAlarmObject->GetAlarmCam()].EnableWindow(TRUE);
					if (FilterAlarm(pAlarmObject))
					{
						int nIndex = m_ctrlList.AddString(pAlarmObject->GetFormatString());
						m_ctrlList.SetItemDataPtr(nIndex, new CAlarmObject(pAlarmObject));
					}
				}
			}

			// In die Zukunft blicken bringt nichts :-)
			CSystemTime time;
			time.GetLocalTime();
			m_ctrlPrevDay.EnableWindow(!(m_AlarmTime.GetDate() == time.GetDate()));

			// Ist das älteste Datum erreicht?
			time = pDatabase->GetFirstAlarmTime();
			m_ctrlNextDay.EnableWindow(!(m_AlarmTime.GetDate() == time.GetDate()));
		}
	}

	// Ist die Liste leer?
	BOOL bNotEmpty = (m_ctrlList.GetCount() > 0); 
	m_ctrlLineUp.EnableWindow(bNotEmpty);
	m_ctrlLineDown.EnableWindow(bNotEmpty);
	m_ctrlPrint.EnableWindow(bNotEmpty);
	m_ctrlImage.EnableWindow(bNotEmpty);

	m_ctrlDisplayTitle.SetDisplayText(m_AlarmTime.GetDate());
}

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
	static DWORD dwTC = GetTickCount();

	if (m_pServer)
	{
		m_pServer->OnIdle();
	}

	if (GetTimeSpan(dwTC) > 10000)
	{
		dwTC = GetTickCount();
//		RequestNewAlarmList();
	}

	CTransparentDialog::OnTimer(nIDEvent);
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
		WK_TRACE("Connecting to Database complete\n");
	}
	else
	{
		WK_TRACE("Can't connect to Database\n");
	}
	m_AlarmTime.GetLocalTime();

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
long CAlarmlistDlg::OnNotifyAlarmListUpdate(WPARAM wParam, LPARAM lParam)
{
	UpdateDlg();
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
	WK_TRACE("Disconnecting to Database complete\n");

	m_AlarmListButtons.EnableWindow(FALSE);

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
	if (m_pServer)
	{
		CIPCDatabaseClientAlarmlist* pDatabase = m_pServer->GetDatabase();
		if (pDatabase)
		{
			CString sText;
			sText.LoadString(IDS_SEARCH_START);
			m_ctrlDisplayTitle.SetDisplayText(sText);
			//m_AlarmListButtons.EnableWindow(FALSE);
			pDatabase->RequestAlarmListArchives(m_AlarmTime);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
long CAlarmlistDlg::OnNotifyMessageScrollEnd(WPARAM wParam, LPARAM lParam)
{
	if (::GetDlgCtrlID((HWND)wParam) == IDC_LIST)
	{
		m_ctrlLineDown.EnableWindow(!lParam);
		m_ctrlLineUp.EnableWindow(lParam);
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmlistDlg::OnPrint() 
{
	CString sError		= "";
	CString sTimestamp	= "";
	CString sConfirm	= "";
	CString sRteNr		= "";
	CString sMsg		= "";

	CPrintDialog dlg(FALSE);

	if (dlg.GetDefaults())
	{
		HDC hDCPrint = dlg.GetPrinterDC();
		if (hDCPrint)
		{
			CDC dcPrint;
			dcPrint.Attach(hDCPrint);
			dcPrint.m_bPrinting = TRUE;

	//		theApp.SetPrinterOriantation(dcPrint, dlg.GetDeviceName(), DMORIENT_LANDSCAPE);

			DOCINFO di;
			ZeroMemory(&di, sizeof(DOCINFO));
			di.cbSize = sizeof(DOCINFO);
			di.lpszDocName = "logevents";

			if (dcPrint.StartDoc(&di) != SP_ERROR)
			{
				if (dcPrint.StartPage() >= 0)
				{	
					CFont font;
					font.CreatePointFont(8*10, "Arial", &dcPrint);
					CFont* pOldFont = (CFont*)dcPrint.SelectObject(&font);

					CRect rcPage(0, 0, dcPrint.GetDeviceCaps(HORZRES), dcPrint.GetDeviceCaps(VERTRES));
					rcPage.DeflateRect(200,200,200, 200);
					dcPrint.DPtoLP(&rcPage);
					int nYPos = 0;
					for (int nI = 0; nI < m_ctrlList.GetCount(); nI++)
					{
						CAlarmObject* pAlarm = (CAlarmObject*)m_ctrlList.GetItemDataPtr(nI);

						sMsg.Format("%d.) %s", nI+1, pAlarm->GetFormatPrintString());

						// Text ausgeben.
						CRect rcText(rcPage.left, rcPage.top+nYPos, rcPage.right, rcPage.top+nYPos);
						dcPrint.DrawText(sMsg, rcText, DT_CALCRECT | DT_LEFT|DT_WORDBREAK);
						dcPrint.DrawText(sMsg, rcText, DT_LEFT|DT_WORDBREAK);

						// Eine Zeile weiter
						nYPos += rcText.Height(); 
						
						// Seitenende erreicht?
						if (nYPos >= rcPage.Height()-rcText.Height())
						{
							nYPos = rcText.Height(); 
							dcPrint.EndPage();
							dcPrint.StartPage();
						}
					}
					dcPrint.EndPage();
					dcPrint.SelectObject(pOldFont);
					pOldFont->DeleteObject();

				}
				dcPrint.EndDoc();
			}
			dcPrint.Detach();
			DeleteDC(hDCPrint);
		}
	}
}

