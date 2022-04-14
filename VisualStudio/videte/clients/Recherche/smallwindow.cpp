// SmallWindow.cpp : implementation file
//

#include "stdafx.h"
#include "Recherche.h"

#include "Mainfrm.h"
#include "RechercheDoc.h"
#include "RechercheView.h"

#include "SmallWindow.h"
#include "images.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SW_COLOR_NORMAL 128
#define SW_COLOR_HIGH   176

static COLORREF s_crTitleColors[] = {	
										RGB(0,0,SW_COLOR_NORMAL),	// blau
										RGB(0,SW_COLOR_NORMAL,0),	// dgruen
										RGB(SW_COLOR_NORMAL,0,0), // lila
										RGB(0,SW_COLOR_NORMAL,SW_COLOR_NORMAL), // dorange
										RGB(SW_COLOR_NORMAL,0,SW_COLOR_NORMAL),	// drot
										RGB(SW_COLOR_NORMAL,SW_COLOR_NORMAL,0),	// dgruen
									};
static COLORREF s_crActiveTitleColors[] = {	
										RGB(0,0,SW_COLOR_HIGH),	// blau
										RGB(0,SW_COLOR_HIGH,0),	// dgruen
										RGB(SW_COLOR_HIGH,0,0), // lila
										RGB(0,SW_COLOR_HIGH,SW_COLOR_HIGH), // dorange
										RGB(SW_COLOR_HIGH,0,SW_COLOR_HIGH),	// drot
										RGB(SW_COLOR_HIGH,SW_COLOR_HIGH,0),	// dgruen
									};
static int		s_iNrOfTitleColors = sizeof(s_crTitleColors) / sizeof(s_crTitleColors[0]);

/////////////////////////////////////////////////////////////////////////////
// CSmallWindow

CSmallWindow::CSmallWindow(CServer* pServer)
{
	m_pServer = pServer;

	if (m_pServer)
	{
		m_wServerID = pServer->GetID();
	}
	else
	{
		m_wServerID = 0;
	}
	m_ToolBarSize = CSize(0,16);
	m_bIsActive = FALSE;
	m_b1to1 = FALSE;
	m_pQueryRectTracker = NULL;
}

CSmallWindow::~CSmallWindow()
{
}


BEGIN_MESSAGE_MAP(CSmallWindow, CWnd)
	//{{AFX_MSG_MAP(CSmallWindow)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_VIDEO_FULLSCREEN, OnVideoFullscreen)
	ON_UPDATE_COMMAND_UI(ID_VIDEO_FULLSCREEN, OnUpdateVideoFullscreen)
	ON_COMMAND(ID_SMALL_CLOSE, OnSmallClose)
	ON_UPDATE_COMMAND_UI(ID_SMALL_CLOSE, OnUpdateSmallClose)
	ON_WM_CREATE()
	ON_COMMAND(ID_SMALL_CONTEXT, OnSmallContext)
	ON_WM_RBUTTONDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
BOOL CSmallWindow::Create(const RECT& rect, CRechercheView* pParentWnd)
{
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSmallWindow::operator < (CSmallWindow & second)
{
	if (GetServerID()<second.GetServerID())
	{
		return TRUE;
	}
	else if (GetServerID()>second.GetServerID())
	{
		return FALSE;
	}
	else
	{
		if (IsDisplayWindow() && second.IsDibWindow())
		{
			return TRUE;
		}
		else if (IsDibWindow() && second.IsDisplayWindow())
		{
			return FALSE;
		}
		return GetName() > second.GetName();
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSmallWindow::IsImageWindow()
{
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSmallWindow::IsDisplayWindow()
{
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSmallWindow::IsDibWindow()
{
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSmallWindow::IsHtmlWindow()
{
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSmallWindow::IsAlarmListWindow()
{
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
CString CSmallWindow::GetTitleText(CDC* pDC)
{
	return _T("");
}
/////////////////////////////////////////////////////////////////////////////
void CSmallWindow::DrawTitle(CDC* pDC)
{
	CRect rect;
	CBrush brush;
	CFont font;
	COLORREF crTitle = RGB(0,0,0);

	GetClientRect(rect);

	rect.bottom = m_ToolBarSize.cy;
	rect.right -= m_ToolBarSize.cx;

	if (GetServerID()!=0)
	{
		if (IsActive())
		{
			crTitle = s_crActiveTitleColors[(GetServerID()-1)%s_iNrOfTitleColors];
		}
		else
		{
			crTitle = s_crTitleColors[(GetServerID()-1)%s_iNrOfTitleColors];
		}
	}

	brush.CreateSolidBrush(crTitle);
	pDC->FillRect(rect,&brush);
	brush.DeleteObject();

	CFont* pOldFont;
	int iOldBkMode;
	COLORREF rgbOld;
	COLORREF rgbTitle;
	CString sTitle;

	font.CreatePointFont(80,_T("MS Sans Serif"),pDC);
	pOldFont = pDC->SelectObject(&font);
	iOldBkMode = pDC->SetBkMode(TRANSPARENT);
	rgbTitle = GetSysColor(IsActive() ? COLOR_CAPTIONTEXT : COLOR_INACTIVECAPTIONTEXT);
	sTitle = GetTitleText(pDC);
	rgbOld = pDC->SetTextColor(rgbTitle);
	CSize ts = pDC->GetOutputTextExtent(sTitle);
	pDC->TextOut(VW_TEXTOFF,(rect.Height()-ts.cy)/2,sTitle);

	pDC->SetTextColor(rgbOld);
	pDC->SetBkMode(iOldBkMode);
	pDC->SelectObject(pOldFont);
	font.DeleteObject();
}
/////////////////////////////////////////////////////////////////////////////
void CSmallWindow::GetTitleRect(LPRECT lpRect)
{
	GetClientRect(lpRect);
	lpRect->bottom = m_ToolBarSize.cy;
}
/////////////////////////////////////////////////////////////////////////////
void CSmallWindow::GetFrameRect(LPRECT lpRect)
{
	GetClientRect(lpRect);
	lpRect->top = m_ToolBarSize.cy;
	lpRect->bottom -= NAVIGATION_HEIGHT;
	lpRect->left = FIELD_WIDTH;
}
/////////////////////////////////////////////////////////////////////////////
void CSmallWindow::SetActive(BOOL bActive/*=TRUE*/)
{
	if ((!m_bIsActive && bActive) || ((m_bIsActive && !bActive)))
	{
		CRect rect;
		GetTitleRect(rect);
		InvalidateRect(rect,FALSE);
	}
	m_bIsActive = bActive;
}
/////////////////////////////////////////////////////////////////////////////
void CSmallWindow::OnPaint() 
{
	CMainFrame* pMF = (CMainFrame*)m_pRechercheView->GetParentFrame();
	if (!pMF->IsFullScreenChanging())
	{
		// only draw myself if Mainframe
		// doesn't change the fullscreen mode
		CPaintDC dc(this); // device context for painting
		OnDraw(&dc);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSmallWindow::OnDraw(CDC* pDC)
{
	// implemented in derived classes
}
/////////////////////////////////////////////////////////////////////////////
void CSmallWindow::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	if (WK_IS_WINDOW(&m_wndToolBar))
	{
		m_wndToolBar.MoveWindow(cx-m_ToolBarSize.cx,
								0,
								m_ToolBarSize.cx,
								m_ToolBarSize.cy);
		CRect rect;
		m_wndToolBar.GetClientRect(rect);
		m_wndToolBar.InvalidateRect(rect);
	}
}
#define _countof(A) (sizeof(A)/sizeof(A[0]))
/////////////////////////////////////////////////////////////////////////////
BOOL CSmallWindow::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
{
	ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);

	// allow top level routing frame to handle the message
	if (GetRoutingFrame() != NULL)
		return FALSE;

	// need to handle both ANSI and UNICODE versions of the message
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
	TCHAR szFullText[256];
	CString strTipText;
	UINT nID = pNMHDR->idFrom;
	if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND) ||
		pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND))
	{
		// idFrom is actually the HWND of the tool
		// nID = _AfxGetDlgCtrlID((HWND)nID);
      nID = ::GetDlgCtrlID((HWND)nID);
	}

	if (nID != 0) // will be zero on a separator
	{
		AfxLoadString(nID, szFullText);
			// this is the command id, not the button index
		AfxExtractSubString(strTipText, szFullText, 1, _T('\n'));
	}

#ifndef _UNICODE
	if (pNMHDR->code == TTN_NEEDTEXTA)
		lstrcpyn(pTTTA->szText, strTipText, _countof(pTTTA->szText));
	else
		_mbstowcsz(pTTTW->szText, strTipText, _countof(pTTTW->szText));
#else
	if (pNMHDR->code == TTN_NEEDTEXTA)
		_wcstombsz(pTTTA->szText, strTipText, _countof(pTTTA->szText));
	else
		lstrcpyn(pTTTW->szText, strTipText, _countof(pTTTW->szText));
#endif
	*pResult = 0;

	// bring the tooltip window above other popup windows
	::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,
		SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE|SWP_NOOWNERZORDER);

	return TRUE;    // message was handled
}
/////////////////////////////////////////////////////////////////////////////
void CSmallWindow::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	m_pRechercheView->SetSmallWindowView1(this);
	
	CWnd::OnLButtonDblClk(nFlags, point);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSmallWindow::IsFullScreen()
{
	CMainFrame* pMF = (CMainFrame*)m_pRechercheView->GetParentFrame();
	return pMF->IsFullScreen();
}
/////////////////////////////////////////////////////////////////////////////
void CSmallWindow::OnVideoFullscreen() 
{
	CMainFrame* pMF = (CMainFrame*)m_pRechercheView->GetParentFrame();
	pMF->ChangeFullScreenModus();
}
/////////////////////////////////////////////////////////////////////////////
void CSmallWindow::OnUpdateVideoFullscreen(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(IsFullScreen());
}
/////////////////////////////////////////////////////////////////////////////
void CSmallWindow::OnSmallClose() 
{
	// attention delete's myself
	if (WK_IS_WINDOW(m_pRechercheView))
	{
		m_pRechercheView->DeleteSmallWindow(GetServerID(),GetID());
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSmallWindow::OnUpdateSmallClose(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!IsFullScreen() || m_pRechercheView->m_SmallWindows.GetSize()>1);
}

int CSmallWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.Create(this) ||
		!m_wndToolBar.LoadToolBar(GetToolBarID()))
	{
		TRACE(_T("Failed to create dib toolbar\n"));
		return -1;      // fail to create
	}

	DWORD dwStyle = m_wndToolBar.GetBarStyle();
	dwStyle &= ~(CBRS_BORDER_RIGHT|CBRS_BORDER_TOP|CBRS_BORDER_LEFT|CBRS_BORDER_BOTTOM);
	dwStyle |= CBRS_TOOLTIPS | CBRS_SIZE_FIXED;
	m_wndToolBar.SetBarStyle(dwStyle);
	m_wndToolBar.ModifyStyle(0, TBSTYLE_FLAT);
	CSize sizeButton(15,15);
	CSize sizeImage(12,12);
	m_wndToolBar.SendMessage(TB_SETBITMAPSIZE, 0, MAKELONG(sizeImage.cx, sizeImage.cy));
	m_wndToolBar.SendMessage(TB_SETBUTTONSIZE, 0, MAKELONG(sizeButton.cx, sizeButton.cy));
	m_ToolBarSize = m_wndToolBar.CalcFixedLayout(FALSE,TRUE);
	m_ToolBarSize.cx += m_wndToolBar.GetToolBarCtrl().GetButtonCount()*3;
	
	EnableToolTips(TRUE);
	// Alle wollen sie ToolTips haben
	if (m_ToolTip.Create(this, TTS_ALWAYSTIP|TTS_NOPREFIX) && m_ToolTip.AddTool(this))
	{
		// Lasse den ToolTip die Zeilenumbrueche beachten
		m_ToolTip.SendMessage(TTM_SETMAXTIPWIDTH, 0, SHRT_MAX);

		// Wenn man will, kann man auch noch die Zeiten veraendern
		// Verweildauer des ToolTips (Maximale Zeit=SHRT_MAX)
		m_ToolTip.SendMessage(TTM_SETDELAYTIME, TTDT_AUTOPOP, SHRT_MAX);
		// Wie schnell soll er erscheinen, wenn der Cursor ueber einem Tool steht
		m_ToolTip.SendMessage(TTM_SETDELAYTIME, TTDT_INITIAL, 100);
		// Mindest-Verzoegerungszeit bis zum naechsten Erscheinen
		m_ToolTip.SendMessage(TTM_SETDELAYTIME, TTDT_RESHOW, 200);
	}
	
	return 0;
}

BOOL CSmallWindow::PreTranslateMessage(MSG* pMsg) 
{
	if (::IsWindow(m_ToolTip.m_hWnd) && pMsg->hwnd == m_hWnd)
	{
		switch(pMsg->message)
		{
		case WM_LBUTTONDOWN:	
		case WM_MOUSEMOVE:
		case WM_LBUTTONUP:	
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:	
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
			m_ToolTip.RelayEvent(pMsg);
			break;
		}
	}
	
	return CWnd::PreTranslateMessage(pMsg);
}
///////////////////////////////////////////////////////////////////
void CSmallWindow::Set1to1(BOOL b1to1)
{
	m_b1to1 = b1to1;
	RedrawWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CSmallWindow::OnSmallContext() 
{
	CRect trect;
	CRect brect;
	CPoint pt;

	m_wndToolBar.GetItemRect(m_wndToolBar.CommandToIndex(ID_SMALL_CONTEXT),brect);
	m_wndToolBar.GetWindowRect(trect);
	ScreenToClient(trect);
	pt.x = trect.left + brect.left;
	pt.y = trect.bottom;
	ClientToScreen(&pt);

	PopupMenu(pt);
}
///////////////////////////////////////////////////////////////////
void CSmallWindow::OnRButtonDown(UINT nFlags, CPoint point) 
{
	//also on rightclick (not only on leftclick) first activate the window
	if(m_pRechercheView)
	{
		m_pRechercheView->ActivateSmallWindow(this);
	}

	//ist der Mauszeiger über dem Rechteck (falls eines da ist), dann nicht das Menü anzeigen
	//sondern das Rechteck löschen
	if(    m_pQueryRectTracker
		&& !m_pQueryRectTracker->GetRect()->IsRectNull()
		&& (m_pQueryRectTracker->HitTest(point) >= 0))
	{
		ResetTracker();
		InvalidateRect(NULL);

		//Setzt in CQueryParameter die Rechteckparameter wieder auf 0
		theApp.GetQueryParameter().SetRectToQuery(m_pQueryRectTracker->GetRectPromilleImage());

		//War der Suchdialog geöffnet, diesen bezüglich des Rechtecks aktualisieren 
		if(m_pRechercheView)
		{
			m_pRechercheView->ShowHideRectangle();
		}
						
		UpdateWindow();
	}
	else
	{
		ClientToScreen(&point);
		PopupMenu(point);
	}
	
	CWnd::OnRButtonDown(nFlags, point);
}
///////////////////////////////////////////////////////////////////
BOOL CSmallWindow::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}
///////////////////////////////////////////////////////////////////
void CSmallWindow::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_pRechercheView->ActivateSmallWindow(this);
	
	CWnd::OnLButtonDown(nFlags, point);
}
///////////////////////////////////////////////////////////////////
BOOL CSmallWindow::PreDestroyWindow(CRechercheView* pRechercheView)
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CSmallWindow::ResetTracker()
{
	WK_DELETE(m_pQueryRectTracker);

	m_pQueryRectTracker = new CQueryRectTracker(this);	
}
