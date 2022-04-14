// TextWindow.cpp: implementation of the CTextWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Vision.h"

#include "VisionDoc.h"
#include "VisionView.h"

#include "TextWindow.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTextWindow::CTextWindow(CServer* pServer, const CSecID id, const CString& sMessage)
	: CSmallWindow(pServer)
{
	m_ID = id;
	m_sMessage = sMessage;
	m_stTimeStamp.GetLocalTime();
	m_dwMonitor = USE_MONITOR_2;
}

CTextWindow::~CTextWindow()
{

}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CTextWindow, CSmallWindow)
	//{{AFX_MSG_MAP(CTextWindow)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
BOOL CTextWindow::IsTextWindow()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CTextWindow::OnDraw(CDC* pDC)
{
	DrawTitle(pDC);
}
/////////////////////////////////////////////////////////////////////////////
CString CTextWindow::GetTitleText(CDC* pDC)
{
	CString sTitle,sTime;

	CString sFormat;
	CSize size;
	CRect rect;

	GetClientRect(rect);

	sTitle = GetServerName();
	sTime.Format(_T("%02d.%02d. , %02d:%02d:%02d"),
				 m_stTimeStamp.GetDay(),
				 m_stTimeStamp.GetMonth(),
				 m_stTimeStamp.GetHour(),
				 m_stTimeStamp.GetMinute(),
				 m_stTimeStamp.GetSecond());

	sFormat.LoadString(IDS_NOTIFICATION_FROM);
	sTitle.Format(sFormat,GetServerName(),sTime);

	return sTitle;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTextWindow::Create(const RECT& rect, UINT nID, CVisionView* pParentWnd)
{
	BOOL bRet;
	m_pVisionView = pParentWnd;
	bRet = CWnd::Create(NULL,NULL, 
						WS_CHILD | WS_BORDER | WS_VISIBLE | WS_CLIPSIBLINGS | CS_DBLCLKS,
						rect, pParentWnd,
						nID); 

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
CSecID CTextWindow::GetID()
{
	return m_ID;
}
/////////////////////////////////////////////////////////////////////////////
int CTextWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CSmallWindow::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CRect rect;
	GetClientRect(rect);

	if (!m_RichEditCtrl.Create(WS_VISIBLE|WS_CHILD|
							   ES_READONLY|ES_MULTILINE,
							   rect,
							   this,
							   AFX_IDW_PANE_FIRST))
	{
		TRACE("Failed to create richedit ctrl\n");
		return -1;
	}
	if (!m_wndToolBar.Create(this) ||
		!m_wndToolBar.LoadToolBar(IDR_TEXT))
	{
		TRACE("Failed to create display toolbar\n");
		return -1;      // fail to create
	}

	DWORD dwStyle = m_wndToolBar.GetBarStyle();
	dwStyle &= ~(CBRS_BORDER_RIGHT|CBRS_BORDER_TOP|CBRS_BORDER_LEFT|CBRS_BORDER_BOTTOM);
	dwStyle |= CBRS_TOOLTIPS | CBRS_SIZE_FIXED;
	m_wndToolBar.SetBarStyle(dwStyle);
	m_wndToolBar.ModifyStyle(0, TBSTYLE_FLAT);

	CSize sizeButton(14,14);
	CSize sizeImage(12,12);
	m_wndToolBar.SetBorders(1,1,1,1);
	m_wndToolBar.SendMessage(TB_SETBITMAPSIZE, 0, MAKELONG(sizeImage.cx, sizeImage.cy));
	m_wndToolBar.SendMessage(TB_SETBUTTONSIZE, 0, MAKELONG(sizeButton.cx, sizeButton.cy));
	
	m_ToolBarSize.cx = m_wndToolBar.GetToolBarCtrl().GetButtonCount()*(sizeButton.cx+4)+4;
	m_ToolBarSize.cy = sizeButton.cy+6;

	m_RichEditCtrl.SetBackgroundColor(FALSE,RGB(192,192,192));
	CFont font;
	font.CreatePointFont(120,_T("Times New Roman"));
	m_RichEditCtrl.SetFont(&font);
	m_RichEditCtrl.SetWindowText(m_sMessage);
	
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CTextWindow::OnSize(UINT nType, int cx, int cy) 
{
	CSmallWindow::OnSize(nType, cx, cy);
	
	CRect rect;
	GetFrameRect(rect);
	m_RichEditCtrl.MoveWindow(rect);
	
}
/////////////////////////////////////////////////////////////////////////////
void CTextWindow::SetNotificationText(const CString& sMessage)
{
	m_sMessage = sMessage;
	m_RichEditCtrl.SetWindowText(m_sMessage);
}
