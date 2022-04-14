// BackGroundWnd.cpp : implementation file
//

#include "stdafx.h"
#include "SecurityLauncher.h"
#include "BackGroundWnd.h"
#include "MainFrame.h"
#include ".\backgroundwnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define OFFSET_START 20
/////////////////////////////////////////////////////////////////////////////
// CBackGroundWnd
CBackGroundWnd::CBackGroundWnd(int nMon)
{
	m_iStatus  = 0;
	m_nMonitor = nMon;
	m_nOffset = OFFSET_START;

	int x,y;
	x = GetSystemMetrics(SM_CXSCREEN);
	y = GetSystemMetrics(SM_CYSCREEN);
	m_BigFont.CreateFont(30,0,0,0,FW_BOLD,0,0,0,0,0,0,0,0,_T("Times New Roman"));
	m_SmallFont.CreateFont(20,0,0,0,FW_BOLD,0,0,0,0,0,0,0,0,_T("Times New Roman"));
	m_MiniFont.CreateFont(14,0,0,0,FW_NORMAL,0,0,0,0,0,0,0,0,_T("Times New Roman"));

	CreateEx(0, BACKGROUND_WND_CLASS, theApp.GetName(), WS_POPUP/*|WS_DISABLED*/, 0,1,x,y,NULL,NULL,NULL);

	SetWindowPos(&wndBottom,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW|SWP_NOACTIVATE);

}
/////////////////////////////////////////////////////////////////////////////
CBackGroundWnd::~CBackGroundWnd()
{
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CBackGroundWnd, CWnd)
	//{{AFX_MSG_MAP(CBackGroundWnd)
	ON_WM_SYSCOMMAND()
	ON_WM_CREATE()
	ON_WM_RBUTTONDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_COMMAND_RANGE(WAI_LAUNCHER, WAI_IDIP_CLIENT, OnWaiClicked)
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
	ON_COMMAND(ID_DATETIME, OnDatetime)
	ON_COMMAND(ID_ERROR, OnError)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
void CBackGroundWnd::SetStatus(int iStatus)
{
	m_iStatus = iStatus;
	if (m_hWnd && IsWindow(m_hWnd))
	{
		Invalidate();
	}
}

void CBackGroundWnd::DrawLOGO(CDC*pDC)
{
	HBITMAP hBitmap = theApp.m_videteLogo;
	if (pDC && hBitmap)
	{
		int xStart, yStart;
		BITMAP bm ;
		HDC    hMemDC ;
		CRect  rc;
		GetClientRect(&rc);

		hMemDC = CreateCompatibleDC (pDC->m_hDC) ;
		SelectObject (hMemDC, hBitmap) ;
		GetObject (hBitmap, sizeof (BITMAP), &bm) ;
		xStart = rc.right - bm.bmWidth;
		yStart = rc.bottom - bm.bmHeight;

		BitBlt (pDC->m_hDC, xStart, yStart, bm.bmWidth, bm.bmHeight, hMemDC, 0, 0, SRCCOPY);

		DeleteDC (hMemDC) ;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CBackGroundWnd message handlers
/////////////////////////////////////////////////////////////////////////////
void CBackGroundWnd::PostNcDestroy() 
{
	delete this;
}
/////////////////////////////////////////////////////////////////////////////
void CBackGroundWnd::OnSysCommand(UINT nID, LPARAM lParam) 
{
	if ((nID & 0xFFF0)==SC_CLOSE)
	{
		return;
	}
	
	CWnd::OnSysCommand(nID, lParam);
}
/////////////////////////////////////////////////////////////////////////////
int CBackGroundWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	SetMonitorRect();

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CBackGroundWnd::SetMonitorRect()
{
	CMonitorInfo MonitorInfo;
	CRect rc;
	MonitorInfo.GetMonitorRect(m_nMonitor, rc, true);
	::SetWindowPos(m_hWnd, NULL, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOOWNERZORDER);
}
/////////////////////////////////////////////////////////////////////////////
void CBackGroundWnd::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (WK_IS_WINDOW(theApp.m_pMainWnd))
	{
		((CMainFrame*)theApp.m_pMainWnd)->ContextMenu();
	}

	CWnd::OnRButtonDown(nFlags, point);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CBackGroundWnd::OnEraseBkgnd(CDC* pDC) 
{
	CRect rect;
	GetClientRect(rect);
	pDC->SaveDC();

	pDC->FillRect(rect,&theApp.m_BkGndBrush);

#ifdef _DEBUG
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(GetSysColor(COLOR_CAPTIONTEXT));
	CString strText;
	strText.Format(_T("Monitor %d"), m_nMonitor+1);
	pDC->SelectObject(&m_BigFont);
	pDC->TextOut(100, 100, strText, strText.GetLength()); 
#endif

	DrawLOGO(pDC);
	pDC->RestoreDC(-1);
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CBackGroundWnd::AddApplicationButton(CApplication*pApp)
{
	CWnd *pWnd = GetDlgItem((UINT)pApp->GetId());
	if (pWnd)
	{
		pWnd->ShowWindow(SW_SHOW);
	}
	else
	{
		CSkinButton *pBtn;
		pBtn = new CSkinButton();
		
		CRect rc;
		LOGBRUSH lb;
		theApp.m_BkGndBrush.GetLogBrush(&lb);
		GetClientRect(&rc);
		CRect rcButton(0, 0, 300, 40);
		rcButton.OffsetRect(20, m_nOffset);
		m_nOffset += rcButton.Height()+4;
		CString sName = pApp->GetExePathName();
		sName = sName.Left(sName.GetLength()-4) + COemGuiApi::GetLanguageAbbreviation() + _T(".dll");
		HINSTANCE  hInstance = ::LoadLibrary(sName);
		sName.Empty();
		if (hInstance)
		{
			sName.LoadString(hInstance, AFX_IDS_APP_TITLE);
			if (!sName.IsEmpty())
			{
				sName = COemGuiApi::GetBrandName() + _T(" ") + sName;
			}
			::FreeLibrary(hInstance);
		}

		if (sName.IsEmpty())
		{
			sName = COemGuiApi::GetApplicationNameOem(pApp->GetId());
		}
		pBtn->Create(sName, WS_VISIBLE, rcButton, this, (UINT)pApp->GetId());
		pBtn->SetBaseColor(lb.lbColor);
		pBtn->SetTextColorNormal(theApp.m_cTextColor);
		pBtn->SetTextColorHighlighted(CSkinButton::ChangeBrightness(theApp.m_cTextColor, 32, TRUE));
		
		HICON hLargeIcon = NULL;
		ExtractIconEx(pApp->GetExePathName(), 0, &hLargeIcon, NULL, 1);
		if (hLargeIcon)
		{
			pBtn->EnableImageAndText(DT_IMAGE_AND_TEXT|DT_RIGHT|DT_VCENTER);
			pBtn->SetIcon(hLargeIcon);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CBackGroundWnd::AddResourceButton(UINT nID)
{
	CString sName;
	sName.LoadString(nID);
	if (!sName.IsEmpty())
	{
		CSkinButton *pBtn;
		int nIndex;
		pBtn = new CSkinButton();
		nIndex = sName.ReverseFind(_T('\n'));
		if (nIndex != -1)
		{
			sName = sName.Mid(nIndex+1);
		}
		
		CRect rc;
		LOGBRUSH lb;
		theApp.m_BkGndBrush.GetLogBrush(&lb);
		GetClientRect(&rc);
		CRect rcButton(0, 0, 300, 40);
		rcButton.OffsetRect(20, m_nOffset);
		m_nOffset += rcButton.Height()+4;

		pBtn->Create(sName, WS_VISIBLE, rcButton, this, (UINT)nID);
		pBtn->SetBaseColor(lb.lbColor);
		pBtn->SetTextColorNormal(theApp.m_cTextColor);
		pBtn->SetTextColorHighlighted(CSkinButton::ChangeBrightness(theApp.m_cTextColor, 32, TRUE));
		
		CToolBar tb;
		if (tb.Create(this, WS_CHILD, AFX_IDW_TOOLBAR) && tb.LoadToolBar(IDR_MENU))
		{
			nIndex = tb.CommandToIndex(nID);
			if (nIndex != -1)
			{
				HICON hLargeIcon = tb.GetToolBarCtrl().GetImageList()->ExtractIcon(nIndex);
				if (hLargeIcon)
				{
					pBtn->EnableImageAndText(DT_IMAGE_AND_TEXT|DT_RIGHT|DT_VCENTER);
					pBtn->SetIcon(hLargeIcon);
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CBackGroundWnd::DeleteApplicationButtons()
{
	CWnd *pWnd = GetWindow(GW_CHILD);
	CSkinButton*pBtn = NULL;
	while (pWnd)
	{
		if (pWnd->IsKindOf(RUNTIME_CLASS(CSkinButton)))
		{
			pBtn = (CSkinButton*)pWnd;
		}
		pWnd = pWnd->GetWindow(GW_HWNDNEXT);
		WK_DELETE(pBtn);
	}
	m_nOffset = OFFSET_START;
}
/////////////////////////////////////////////////////////////////////////////
void CBackGroundWnd::OnDestroy()
{
	DeleteApplicationButtons();
	CWnd::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
void CBackGroundWnd::OnWaiClicked(UINT nID)
{
	theApp.StartClientApp((WK_ApplicationId)nID);
}
/////////////////////////////////////////////////////////////////////////////
void CBackGroundWnd::OnAppExit()
{
	AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_APP_EXIT);
}
/////////////////////////////////////////////////////////////////////////////
void CBackGroundWnd::OnDatetime()
{
	AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_DATETIME);
}
/////////////////////////////////////////////////////////////////////////////
void CBackGroundWnd::OnError()
{
	AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_ERROR);
}
