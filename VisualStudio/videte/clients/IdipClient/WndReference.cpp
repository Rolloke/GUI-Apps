// WndReference.cpp: implementation of the CWndReference class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "IdipClient.h"
#include "IdipClientDoc.h"
#include "ViewIdipClient.h"

#include "WndReference.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CWndReference, CWndImage)

CWndReference::CWndReference(CServer* pServer, CSecID id,const CString& sCameraName)
	: CWndImage(pServer)
{
	m_nType = WST_REFERENCE;
	m_idCamera = id;
	m_sCameraName = sCameraName;
	m_pJpeg = new CJpeg;
}

CWndReference::~CWndReference()
{

}
/////////////////////////////////////////////////////////////////////////////
UINT CWndReference::GetToolBarID()
{
	return IDR_TEXT;
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CWndReference, CWndImage)
	//{{AFX_MSG_MAP(CWndReference)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
void CWndReference::OnDraw(CDC* pDC)
{
	CRect fr,vr;
	GetFrameRect(fr);
	pDC->FillRect(fr,&m_pViewIdipClient->m_CinemaBrush);
	if (m_pJpeg->IsValid())
	{
		GetVideoClientRect(vr,m_pJpeg->GetImageDims(),FALSE);
		m_pJpeg->OnDraw(pDC, vr);
	}
	DrawTitle(pDC);
}
/////////////////////////////////////////////////////////////////////////////
CString CWndReference::GetTitleText(CDC* pDC)
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

	if (size.cx > (rect.Width()-m_wndToolBar.GetSize().cx))
	{
		sTitle.Format(IDS_REFERENCE_TITLE, m_sCameraName, sTime);
		size = pDC->GetOutputTextExtent(sTitle);
		if (size.cx > (rect.Width()-m_wndToolBar.GetSize().cx))
		{
			sTitle.Format(IDS_REFERENCE_TITLE_LIGHT, m_sCameraName, sTime);
			size = pDC->GetOutputTextExtent(sTitle);
			if (size.cx > (rect.Width()-m_wndToolBar.GetSize().cx))
			{
				sTitle = m_sCameraName;
			}
		}
	}
	return sTitle;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndReference::Create(const RECT& rect, UINT nID, CViewIdipClient* pParentWnd)
{
	BOOL bRet;
	m_pViewIdipClient = pParentWnd;
	bRet = CWnd::Create(NULL,NULL, 
						WS_CHILD | WS_BORDER | WS_VISIBLE | WS_CLIPSIBLINGS | CS_DBLCLKS,
						rect, pParentWnd,
						nID); 

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
CSecID CWndReference::GetID()
{
	return SECID_NO_ID;
}
/////////////////////////////////////////////////////////////////////////////
int CWndReference::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWndImage::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CRect rect;
	GetFrameRect(rect);
	Reload();

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CWndReference::OnSize(UINT nType, int cx, int cy) 
{
	CWndImage::OnSize(nType, cx, cy);
	
	// still nothing but who knows	
}
/////////////////////////////////////////////////////////////////////////////
void CWndReference::OnDestroy() 
{
	CWndImage::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
void CWndReference::Reload()
{
	CString sPath;
	sPath.Format(_T("%s\\%08lx\\%08lx.jpg"),CNotificationMessage::GetWWWRoot(),
		GetServer()->GetHostID().GetID(),m_idCamera.GetID());

	CFileStatus cfs;

	if (CFile::GetStatus(sPath,cfs))
	{
		m_stTimeStamp.FromTime(cfs.m_mtime);
	}

	m_pJpeg->DecodeJpegFromFile(sPath);
	TRACE(_T("reference image res %d,%d"),m_pJpeg->GetImageDims().cx,m_pJpeg->GetImageDims().cy);
}
