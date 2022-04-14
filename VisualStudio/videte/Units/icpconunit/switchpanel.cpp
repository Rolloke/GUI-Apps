/////////////////////////////////////////////////////////////////////////////
// PROJECT:		ICPCONUnit
// FILE:		$Workfile: switchpanel.cpp $
// ARCHIVE:		$Archive: /Project/Units/ICPCONUnit/switchpanel.cpp $
// CHECKIN:		$Date: 6.03.06 11:45 $
// VERSION:		$Revision: 13 $
// LAST CHANGE:	$Modtime: 6.03.06 11:42 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ICPCONUnit.h"
#include "SwitchPanel.h"
#include "MinimizedDlg.h"
#include "ICPCONDll\ICP7000Module.h"
#include "CIPCInputICPCONUnit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define POLL_TIMER       0x0815
#define MINIMIZE_TIMER   0x0816
#define PUSHBUTTON_TIMER 0x0817

#define IDC_RELAIS_LAST IDC_RELAIS +32
/////////////////////////////////////////////////////////////////////////////
// CSwitchPanel dialog
extern CICPCONUnitApp theApp;

CSwitchPanel::CSwitchPanel(CWnd* pParent /*=NULL*/)
	: CSkinDialog(CSwitchPanel::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSwitchPanel)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pMiniDlg         = NULL;


	m_nMinimizeTimer   = 0;
	m_nMinimizeTime    = 10;
	m_nMinimizeCounter = 0;
	
	m_nButtonShape     = -1;
	m_nSkinColor       = -1;
	m_dwPushButton     = 0;
	m_dwPushButtonState= 0;
	WK_TRACE(_T("CSwitchPanel\n"));
}
/////////////////////////////////////////////////////////////////////////////
CSwitchPanel::~CSwitchPanel()
{
	WK_DELETE(m_pMiniDlg);
	WK_DELETE(m_pInput);
	CICPI7000Thread::DeletePerformanceMonitor();
}
/////////////////////////////////////////////////////////////////////////////
void CSwitchPanel::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSwitchPanel)
	DDX_Control(pDX, IDC_RELAIS, m_cFirstButton);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CSwitchPanel, CSkinDialog)
	//{{AFX_MSG_MAP(CSwitchPanel)
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_USER, OnUserMessage)
	ON_COMMAND_RANGE(IDC_RELAIS, IDC_RELAIS_LAST, OnCheck)
//	ON_MESSAGE(WM_CHECK_SKINBUTTON, OnCheckSkinBtn)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CSwitchPanel message handlers
BOOL CSwitchPanel::OnInitDialog() 
{
	CSkinDialog::OnInitDialog();
	CWK_Profile wkp1(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, theApp.m_sRegistryLocation, _T(""));
	m_nSkinColor       = wkp1.GetInt(SETTINGS_SECTION, ICP_SKINCOLOR        , m_nSkinColor);
	m_nButtonShape     = wkp1.GetInt(SETTINGS_SECTION, ICP_BUTTON_SHAPE     , m_nButtonShape);

	CRect rcButton;
	GetWindowRect(&m_rcDlg);
	m_cFirstButton.GetWindowRect(&rcButton);
	m_nEdgeX = m_rcDlg.right  - rcButton.right;
	m_nEdgeY = m_rcDlg.bottom - rcButton.bottom;
	m_ilRelaisBtn.Create(IDB_RELAIS_BTN, 16, 0, SKIN_COLOR_KEY);

	ShowWindow(SW_HIDE);
	m_pInput = new CIPCInputICPCONUnit(this, SM_ICPCONUnitInput);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CSwitchPanel::OnCheck(UINT nID) 
{
#ifdef _DEBUG
	CThreadDebug td("OnCheck");
#endif

	CWnd *pWnd = GetDlgItem(nID);
	if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CSkinButton)))
	{
		CSkinButton *pSkinBtn = (CSkinButton*)pWnd;
		int nBtn = ::GetWindowLong(pSkinBtn->m_hWnd, GWL_USERDATA);
		bool bSet = false;
		DWORD dwPushButton = 1 << nBtn ;
		if (m_dwPushButton & dwPushButton)
		{
			bSet = true;
			m_dwPushButtonState |= dwPushButton;
			SAFE_KILL_TIMER(m_nPushButtonTimer);
			m_nPushButtonTimer = SetTimer(PUSHBUTTON_TIMER, 500, NULL);
		}
		else
		{
			bSet = pSkinBtn->GetCheck()?true:false;
		}
		m_pInput->SetAlarm(nBtn, bSet);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSwitchPanel::OnDestroy() 
{
	DeleteAllButtons();
	SAFE_KILL_TIMER(m_nMinimizeTimer);
	SAFE_KILL_TIMER(m_nPushButtonTimer);
	
	CSkinDialog::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
void CSwitchPanel::DeleteAllButtons()
{
	int nID;
	HWND hwnd;
	for (nID=IDC_RELAIS+1; nID<=IDC_RELAIS_LAST; nID++)
	{
		hwnd = ::GetDlgItem(m_hWnd, nID);
		if (hwnd)
		{
			delete CWnd::FromHandle(hwnd);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
int CSwitchPanel::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CSkinDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	ShowWindow(SW_SHOW);

	WK_TRACE(_T("CSwitchPanel::OnCreate\n"));

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CSwitchPanel::OnCancel() 
{
#ifdef _DEBUG
	CDialog::OnCancel();
	DestroyWindow();
#endif
}
/////////////////////////////////////////////////////////////////////////////
void CSwitchPanel::OnOK() 
{
#ifdef _DEBUG
	CDialog::OnOK();
	DestroyWindow();
#endif
}
/////////////////////////////////////////////////////////////////////////////
void CSwitchPanel::CreateAllButtons()
{
	CString strText, strSection;

	DWORD dwStyle;
	CRect rcButton, rcDlg;
	UINT  nIDButton;
	BOOL  bPushButton = FALSE;
	CFont *pFont = NULL;
	CWK_Profile wkpBase(CWK_Profile::WKP_DEFAULT, HKEY_LOCAL_MACHINE, _T(""), _T(""));
	CWK_Profile wkp1(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, theApp.m_sRegistryLocation, _T(""));
	int nSizeX   = wkp1.GetInt(SETTINGS_SECTION, ICP_BUTTON_DX, 0),
	    nSizeY   = wkp1.GetInt(SETTINGS_SECTION, ICP_BUTTON_DY, 0),
         nX       = 0,
		 nRight   = 0,
		 nButtons, nMaxX, nLeft, i;

	m_dwPushButton      = 0;
	m_dwPushButtonState = 0;
	m_nMinimizeTime = wkp1.GetInt(SETTINGS_SECTION, ICP_MINIMIZE_TIME, m_nMinimizeTime);

	nButtons = m_pInput->GetNoOfInputs();
	if (nButtons == 0)
	{
		ShowWindow(SW_HIDE);
		if (m_pMiniDlg)
		{
			m_pMiniDlg->ShowWindow(SW_HIDE);
		}
		return;
	}
	if (nButtons > 24)
	{
		nMaxX = 7;
	}
	else if (nButtons > 15)
	{
		nMaxX = 6;
	}
	else if (nButtons > 12)
	{
		nMaxX = 5;
	}
	else
	{
		nMaxX = 4;
	}

	if (m_nSkinColor == 1)
	{
		CreateTransparent(StyleBackGroundColorChangeBrushed, SKIN_COLOR_GREEN);
		m_cFirstButton.SetBaseColor(SKIN_COLOR_GREEN_SHADOW);
	}
	else if (m_nSkinColor == 2)
	{
		CreateTransparent(StyleBackGroundColorChangeBrushed, SKIN_COLOR_BLUE);
		m_cFirstButton.SetBaseColor(SKIN_COLOR_BLUE_SHADOW);
	}
	else if (m_nSkinColor == 3)
	{
		CreateTransparent(StyleBackGroundColorChangeBrushed, SKIN_COLOR_GREY_MEDIUM_DARK);
		m_cFirstButton.SetBaseColor(SKIN_COLOR_GREY_DARK);
	}
	else if (m_nSkinColor == 4)
	{
		CreateTransparent(StyleBackGroundColorChangeBrushed, SKIN_COLOR_GREY_LIGHT);
		m_cFirstButton.SetBaseColor(SKIN_COLOR_GREY_MEDIUM_LIGHT);
	}
	else if (m_nSkinColor == 5)
	{
		CreateTransparent(StyleBackGroundColorChangeBrushed, SKIN_COLOR_GOLD_METALLIC_LIGHT);
		m_cFirstButton.SetBaseColor(SKIN_COLOR_GOLD_METALLIC);
	}
	if (m_nButtonShape == 0)		m_cFirstButton.SetShape(ShapeRect);
	else if (m_nButtonShape == 1)   m_cFirstButton.SetShape(ShapeRound);
	
	m_cFirstButton.SetStyle(StyleCheck);
//	m_cFirstButton.SetSurface(SurfaceColorChangeBrushed);
	m_cFirstButton.SetImageList(&m_ilRelaisBtn);
	m_cFirstButton.EnableImageAndText(DT_IMAGE_AND_TEXT|DT_RIGHT|DT_TOP);

//	m_cFirstButton.SetTextColorHighlighted(SKIN_COLOR_DARK_RED);
	m_cFirstButton.SetTextAllignment(DT_WORDBREAK|DT_CENTER|DT_VCENTER);
	
	GetWindowRect(&rcDlg);
	rcDlg.right  = rcDlg.left + m_rcDlg.Width();
	rcDlg.bottom = rcDlg.top  + m_rcDlg.Height();
	m_cFirstButton.GetWindowRect(&rcButton);

	ScreenToClient(&rcButton);
	if (nSizeX)
	{
		rcButton.right  = rcButton.left + nSizeX;
		rcButton.bottom = rcButton.top  + nSizeY;
		m_cFirstButton.MoveWindow(&rcButton);
	}
	else
	{
		nSizeX = rcButton.Width();
		wkp1.WriteInt(SETTINGS_SECTION, ICP_BUTTON_DX, nSizeX);
		nSizeY = rcButton.Height();
		wkp1.WriteInt(SETTINGS_SECTION, ICP_BUTTON_DY, nSizeY);
	}
	nLeft = rcButton.left;
	pFont = m_cFirstButton.GetFont();
	nIDButton = m_cFirstButton.GetDlgCtrlID();
	dwStyle   = ::GetWindowLong(m_cFirstButton.m_hWnd, GWL_STYLE);
	
	strSection = SEC_NAME_INPUTGROUPS;
	strText.Format(ICP_GROUP_FMT, m_pInput->GetGroupID());
	strSection = strSection + _T("\\") + strText;
			
	for (i=0; i<32; i++)
	{
		if (m_pInput->IsInputActive(i))
		{
			strText     = wkpBase.GetString(strSection, i+1, NULL);
			bPushButton = wkpBase.GetNrFromString(strText, INI_PUSHBUTTON, 0);
			strText     = wkpBase.GetStringFromString(strText, INI_COMMENT, NULL);
			if (nIDButton > IDC_RELAIS)
			{
				CSkinButton *pButton = new CSkinButton;
				if (++nX >= nMaxX)
				{
					nRight = rcButton.right;
					rcButton.left  = nLeft;
					rcButton.right = nLeft + nSizeX;
					rcButton.OffsetRect(0, nSizeY+5);
					nX = 0;
				}
				else
				{
					rcButton.OffsetRect(nSizeX+5, 0);
				}
				pButton->Create(strText, dwStyle, rcButton, this, nIDButton);
				pButton->SetImageList(&m_ilRelaisBtn);
				pButton->EnableImageAndText(DT_IMAGE_AND_TEXT|DT_RIGHT|DT_TOP);
				SetWindowLong(pButton->m_hWnd, GWL_USERDATA, (long)i);
				pButton->CopyProperties(m_cFirstButton);
				pButton->SetWindowText(strText);
				pButton->SetFont(pFont);
				pButton->SetCheck(m_pInput->GetState(i));
			}
			else
			{
				m_cFirstButton.SetWindowText(strText);
				m_cFirstButton.SetCheck(m_pInput->GetState(i));
			}
			if (bPushButton)
			{
				m_dwPushButton |= 1 << i;
			}
			nIDButton++;
		}
	}

	if (!nRight)																// rechte Seite noch nicht ermittelt ?
	{
		nRight = rcButton.right;
	}
	if (pFont)																	// Font darf nicht gelöscht werden
	{
		pFont->Detach();
	}

	rcButton.right   = nRight + m_nEdgeX;								// Dialoggröße berechnen
	rcButton.bottom += m_nEdgeY;
	ClientToScreen(&rcButton);
	rcDlg.right  = rcButton.right;
	rcDlg.bottom = rcButton.bottom;
	MoveWindow(&rcDlg);
	::InvalidateRect(m_hWnd, NULL, TRUE);
	PostMessage(WM_USER, USER_MSG_RESTORE, 0);

}
/////////////////////////////////////////////////////////////////////////////
void CSwitchPanel::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == m_nMinimizeTimer)
	{
		CPoint pt;
		CRect  rc;
		GetWindowRect(&rc);
		GetCursorPos(&pt);
		if (rc.PtInRect(pt))
		{
			m_nMinimizeCounter = 0;
		}
		else if (m_nMinimizeCounter++ > m_nMinimizeTime)
		{
			PostMessage(WM_USER, USER_MSG_MINIMIZE);
		}

		if (	!theApp.m_bIsInMenuLoop
			&&	!(GetWindowLong(m_hWnd, GWL_EXSTYLE) & WS_EX_TOPMOST))
		{
			::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
		}
	}
	else if (nIDEvent == m_nPushButtonTimer)
	{
		DWORD dwPushButton = 1;
		int   i;
		for (i=0; i<32; i++, dwPushButton<<=1)
		{
			if (m_dwPushButtonState & dwPushButton)
			{
				m_pInput->SetAlarm(i, false);
				m_dwPushButtonState &= ~dwPushButton;
				CWnd *pWnd = GetDlgItem(IDC_RELAIS+i);
				ASSERT_KINDOF(CSkinButton, pWnd);
				((CSkinButton*)pWnd)->SetCheck(FALSE);
			}
		}
		SAFE_KILL_TIMER(m_nPushButtonTimer);
	}

	CSkinDialog::OnTimer(nIDEvent);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CSwitchPanel::OnUserMessage(WPARAM wParam,  LPARAM lParam)
{
	if (wParam == USER_MSG_RESTORE)
	{
		if (m_pMiniDlg)
		{
			m_pMiniDlg->ShowWindow(SW_HIDE);
		}
		ShowWindow(SW_SHOW);
	}
	else if (wParam == USER_MSG_MINIMIZE)
	{
		if (!m_pMiniDlg)
		{
			m_pMiniDlg = new CMinimizedDlg();
			m_pMiniDlg->Create(IDD_MINIMIZED_DLG, this);
		}
		if (!m_pMiniDlg->IsWindowVisible())
		{
			m_pMiniDlg->ShowWindow(SW_SHOW);
		}
		ShowWindow(SW_HIDE);
	}
	else if (wParam == USER_MSG_REQUEST_RESET)
	{
		DeleteAllButtons();
		CreateAllButtons();
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CSwitchPanel::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CSkinDialog::OnShowWindow(bShow, nStatus);
	if (bShow)
	{
		SetFocus();
		CRect rc;
		DWORD dwFlags = SWP_NOSIZE | SWP_NOMOVE;
		GetWindowRect(&rc);
		if ((rc.left < 50) && (rc.top < 50))
		{
			HMONITOR hMon = CMonitorInfo::GetMonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
			if (hMon)
			{
				MONITORINFO mi;
				mi.cbSize = sizeof(MONITORINFO);
				if (CMonitorInfo::GetMonitorInfo(hMon, &mi))
				{
					rc.left = 150;
					rc.top  = 150;
					dwFlags &= ~SWP_NOMOVE;
				}
			}
		}
		::SetWindowPos(m_hWnd, HWND_TOPMOST, rc.left, rc.top, 0, 0, dwFlags);
		m_nMinimizeTimer   = SetTimer(MINIMIZE_TIMER, 1000, NULL);
		m_nMinimizeCounter = 0;
	}
	else
	{
		SAFE_KILL_TIMER(m_nMinimizeTimer);
	}
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CSwitchPanel::OnCheckSkinBtn(WPARAM wParam,  LPARAM lParam)
{
	CWnd *pWnd = GetDlgItem(wParam);
	if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CSkinButton)))
	{
		CSkinButton *pSkinBtn = (CSkinButton*)pWnd;
		if (lParam == -1)
		{
			pSkinBtn->EnableWindow(FALSE);
		}
		else
		{
			pSkinBtn->EnableWindow(TRUE);
			pSkinBtn->SetCheck(lParam);
		}
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
