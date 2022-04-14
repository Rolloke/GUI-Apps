// DebugWindow.cpp : implementation file
//

#include "stdafx.h"
#include "DVClient.h"
#include "DebugWindow.h"
#include "CPanel.h"
#include "mainframe.h"
#include ".\debugwindow.h"
#include "wkclasses\CLogFile.h"


/////////////////////////////////////////////////////////////////////////////
// CDebugWindow

IMPLEMENT_DYNAMIC(CDebugWindow, CDisplayWindow)
CDebugWindow::CDebugWindow(CMainFrame* pParent, CServer* pServer, BOOL bUseAsReport /* = FALSE */):
	CDisplayWindow(pParent, pServer)
{
	m_bUseAsReport = bUseAsReport;
}
/////////////////////////////////////////////////////////////////////////////
CDebugWindow::~CDebugWindow()
{
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDebugWindow, CDisplayWindow)
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_COMMAND(IDCANCEL, OnCancel)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CDebugWindow message handlers
int CDebugWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDisplayWindow::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	CRect rect, rc;
	GetClientRect(rect);
	rc = rect;

	CWindowDC dc(this);
	CSize sz = dc.GetTextExtent(_T("X"), 1);
	rc.bottom = rc.top + sz.cy;
	rc.left = rc.right - sz.cy;

	m_BtnCancel.SetBaseColor(CPanel::m_BaseColorButtons);
	m_BtnCancel.CreateEx(0, _T("Button"), _T("X"), WS_VISIBLE|WS_CHILD|WS_TABSTOP, rc, this, IDCANCEL, NULL);
	SetWindowPos(&m_BtnCancel, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
	
	
	rect.top = sz.cy;
	DWORD dwStyle = WS_VISIBLE|WS_CHILD|ES_READONLY|ES_AUTOHSCROLL|ES_AUTOVSCROLL|ES_MULTILINE;
	if(m_bUseAsReport)
	{
		dwStyle &= ~ES_AUTOHSCROLL;
	}
	if (!m_ctrlEdit.Create(dwStyle, rect, this, AFX_IDW_PANE_FIRST))
	{
		return -1;
	}

	if(m_bUseAsReport)
	{
		SetTextLine();
	}
	else
	{
		rc.OffsetRect(-sz.cy, 0);
		m_BtnClear.SetBaseColor(SKIN_COLOR_RED);
		m_BtnClear.CreateEx(0, _T("Button"), _T("E"), WS_VISIBLE|WS_CHILD|WS_TABSTOP, rc, this, ID_EDIT_CLEAR, NULL);
		SetWindowPos(&m_BtnClear, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);

		rc.OffsetRect(-sz.cy, 0);
		m_BtnCopy.SetBaseColor(CPanel::m_BaseColorButtons);
		m_BtnCopy.CreateEx(0, _T("Button"), _T("C"), WS_VISIBLE|WS_CHILD|WS_TABSTOP, rc, this, ID_EDIT_COPY, NULL);
		SetWindowPos(&m_BtnCopy, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
		SetDebuggerWindowHandle(m_ctrlEdit.m_hWnd);
	}

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CDebugWindow::OnDestroy()
{
	SetDebuggerWindowHandle(NULL);
	CDisplayWindow::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
void CDebugWindow::OnSize(UINT nType, int cx, int cy)
{
	CDisplayWindow::OnSize(nType, cx, cy);
	CRect rc;
	m_BtnCancel.GetClientRect(&rc);
	m_BtnCancel.MoveWindow(cx-rc.bottom, 0, rc.bottom, rc.bottom);
	if(!m_bUseAsReport)
	{
		m_BtnClear.MoveWindow(cx-rc.bottom*2, 0, rc.bottom, rc.bottom);
		m_BtnCopy.MoveWindow(cx-rc.bottom*3, 0, rc.bottom, rc.bottom);
	}

	m_ctrlEdit.MoveWindow(0, rc.bottom, cx, cy);
}
void CDebugWindow::OnDraw(CDC* pDC)
{
	CDisplayWindow::OnDraw(pDC);
	m_BtnCancel.InvalidateRect(NULL);
	if(!m_bUseAsReport)
	{
		m_BtnCopy.InvalidateRect(NULL);
		m_BtnClear.InvalidateRect(NULL);
	}
}
/////////////////////////////////////////////////////////////////////////////
CSecID CDebugWindow::GetID() const
{
	return SECID_NO_ID;
}
/////////////////////////////////////////////////////////////////////////////
CString CDebugWindow::GetName() const
{
	CString sTitle(_T("Debugger Window"));
	if(m_bUseAsReport)
	{
		sTitle.Empty();
	}
	return sTitle;
}
CString CDebugWindow::GetDefaultText()
{
	return GetName();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDebugWindow::IsShowable()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CDebugWindow::OnEditCopy()
{
	m_ctrlEdit.SendMessage(WM_COPY);
}
/////////////////////////////////////////////////////////////////////////////
void CDebugWindow::OnEditClear()
{
	m_ctrlEdit.SetWindowText(_T(""));
}
/////////////////////////////////////////////////////////////////////////////
void CDebugWindow::OnCancel()
{
	m_pMainFrame->PostMessage(WM_USER, MAKELONG(WPARAM_DEL_DEBUG,0), 0);
}

void CDebugWindow::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_pMainFrame->SetDisplayActive(this);
//	CDisplayWindow::OnLButtonDown(nFlags, point);
}
/////////////////////////////////////////////////////////////////////////////
void CDebugWindow::SetTextLine()
{
	CString sLine = m_pMainFrame->GetMaintenanceText();
	m_ctrlEdit.SetWindowText(sLine);
}
/////////////////////////////////////////////////////////////////////////////
CString CDebugWindow::GetTextLine()
{
	CString sWndText;
	m_ctrlEdit.GetWindowText(sWndText); 
	return  sWndText;
}
