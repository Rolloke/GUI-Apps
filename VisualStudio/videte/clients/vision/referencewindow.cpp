// ReferenceWindow.cpp: implementation of the CReferenceWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Vision.h"
#include "VisionDoc.h"
#include "VisionView.h"

#include "ReferenceWindow.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CReferenceWindow::CReferenceWindow(CServer* pServer, CSecID id,const CString& sCameraName)
	: CSmallWindow(pServer)
{
	m_idCamera = id;
	m_sCameraName = sCameraName;
}

CReferenceWindow::~CReferenceWindow()
{

}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CReferenceWindow, CSmallWindow)
	//{{AFX_MSG_MAP(CReferenceWindow)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
BOOL CReferenceWindow::IsReferenceWindow()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CReferenceWindow::OnDraw(CDC* pDC)
{
	DrawTitle(pDC);
	CRect fr,vr;
	GetFrameRect(fr);
	pDC->FillRect(fr,&m_pVisionView->m_CinemaBrush);
	if (m_Jpeg.IsValid())
	{
		GetVideoClientRect(vr,m_Jpeg.GetImageDims(),FALSE);
		m_Jpeg.OnDraw(pDC, vr);
	}
}
/////////////////////////////////////////////////////////////////////////////
CString CReferenceWindow::GetTitleText(CDC* pDC)
{
	CString sTitle,sTime;

	CString sFormat;
	CSize size;
	CRect rect;

	GetClientRect(rect);

	sTitle = GetServerName();
	sTime.Format(_T("%02d.%02d.%04d, %02d:%02d"),
				 m_stTimeStamp.GetDay(),
				 m_stTimeStamp.GetMonth(),
				 m_stTimeStamp.GetYear(),
				 m_stTimeStamp.GetHour(),
				 m_stTimeStamp.GetMinute());

	sTitle.Format(IDS_REFERENCE_TITLE_FULL,GetServerName(), m_sCameraName, sTime);

	size = pDC->GetOutputTextExtent(sTitle);
	if (size.cx > (rect.Width()-m_ToolBarSize.cx))
	{
		sTitle.Format(IDS_REFERENCE_TITLE, m_sCameraName, sTime);
		size = pDC->GetOutputTextExtent(sTitle);
		if (size.cx > (rect.Width()-m_ToolBarSize.cx))
		{
			sTitle.Format(IDS_REFERENCE_TITLE_LIGHT, m_sCameraName, sTime);
			size = pDC->GetOutputTextExtent(sTitle);
			if (size.cx > (rect.Width()-m_ToolBarSize.cx))
			{
				sTitle = m_sCameraName;
			}
		}
	}
	return sTitle;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CReferenceWindow::Create(const RECT& rect, UINT nID, CVisionView* pParentWnd)
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
CSecID CReferenceWindow::GetID()
{
	return SECID_NO_ID;
}
/////////////////////////////////////////////////////////////////////////////
int CReferenceWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CSmallWindow::OnCreate(lpCreateStruct) == -1)
		return -1;
	

	CRect rect;
	GetFrameRect(rect);
	Reload();

	GetClientRect(rect);

	if (!m_wndToolBar.Create(this) ||
		!m_wndToolBar.LoadToolBar(IDR_TEXT))
	{
		TRACE(_T("Failed to create display toolbar\n"));
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
	
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CReferenceWindow::OnSize(UINT nType, int cx, int cy) 
{
	CSmallWindow::OnSize(nType, cx, cy);
	
	// still nothing but who knows	
}
/////////////////////////////////////////////////////////////////////////////
void CReferenceWindow::OnDestroy() 
{
	CSmallWindow::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
void CReferenceWindow::Reload()
{
	CString sPath;
	sPath.Format(_T("%s\\%08lx\\%08lx.jpg"),CNotificationMessage::GetWWWRoot(),
		GetServer()->GetHostID().GetID(),m_idCamera.GetID());

	CFileStatus cfs;

	if (CFile::GetStatus(sPath,cfs))
	{
		m_stTimeStamp.FromTime(cfs.m_mtime);
	}

	m_Jpeg.DecodeJpegFromFile(sPath);
	TRACE(_T("reference image res %d,%d"),m_Jpeg.GetImageDims().cx,m_Jpeg.GetImageDims().cy);
}
