/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: WndPT.cpp $
// ARCHIVE:		$Archive: /Project/Clients/IdipClient/WndPT.cpp $
// CHECKIN:		$Date: 4.05.06 11:34 $
// VERSION:		$Revision: 10 $
// LAST CHANGE:	$Modtime: 4.05.06 11:12 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#include "stdafx.h"

#include "IdipClient.h"
#include "IdipClientDoc.h"
#include "ViewIdipClient.h"
#include "ViewCamera.h"
#include "Server.h"

#include "WndPT.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// PT Resolutions
// RESOLUTION_QCIF 176 x 144, 11:9	
// RESOLUTION_2CIF 352 x 288, 11:9
IMPLEMENT_DYNAMIC(CWndPT, CWndLive)
/////////////////////////////////////////////////////////////////////////////
// CWndPT
CWndPT::CWndPT(CServer* pServer, const CIPCOutputRecord &or) 
	: CWndLive(pServer, or)
{
//	m_iX = 11;
//	m_iY = 9;
	m_rcZoom = CRect(0,0,0,0);
	m_pPTDecoder = new CPTDecoder(TRUE);
	m_nType = WST_PT;
}
/////////////////////////////////////////////////////////////////////////////
CWndPT::~CWndPT()
{

}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CWndPT, CWndLive)
	//{{AFX_MSG_MAP(CWndPT)
	ON_MESSAGE(WM_CHANGE_VIDEO, OnChangeVideo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
CSize CWndPT::GetPictureSize()
{
	CSize s;
	s.cx = s.cy = 0;

	m_csPictureRecord.Lock();
	if (m_pPictureRecord)
	{
		GetPTFrameSize(m_pPictureRecord->GetData(), m_pPictureRecord->GetDataLength(), s);
	}
	m_csPictureRecord.Unlock();
	return s;
}
/////////////////////////////////////////////////////////////////////////////
void CWndPT::OnDraw(CDC* pDC)
{
	if (m_bWasSomeTimesActive)
	{
		CRect rect;
		GetVideoClientRect(rect);
		m_pPTDecoder->OnDraw(pDC->m_hDC,rect, m_rcZoom);
	}
	CWndLive::OnDraw(pDC);

}
/////////////////////////////////////////////////////////////////////////////
LRESULT  CWndPT::OnChangeVideo(WPARAM /*wParam*/, LPARAM /*lParam*/) 
{
	// called after CopyToBuffer, now we are in main thread
	m_csPictureRecord.Lock();

	if (   m_pPictureRecord
		&& m_pPictureRecord->GetCompressionType() == COMPRESSION_PRESENCE)
	{
		m_pPTDecoder->Decode((BYTE*)m_pPictureRecord->GetData(),
								m_pPictureRecord->GetDataLength(),
								m_pPictureRecord->GetPictureType()!=SPT_DIFF_PICTURE,
								FALSE);
		m_bWasSomeTimesActive = TRUE;
	}
	m_csPictureRecord.Unlock();
	if (m_pViewIdipClient->SetWndLiveRequested(this))
	{
		Invalidate();
		UpdateWindow();
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CWndPT::Set1to1(BOOL b1to1)
{
	CWndSmall::Set1to1(b1to1);
}
/////////////////////////////////////////////////////////////////////////////
void CWndPT::Settings()
{
/*
	if (m_pVideoColorDialog)
	{
		m_pVideoColorDialog->DestroyWindow();
		m_pVideoColorDialog = NULL;
		m_bMarkedForShowColorDialog = FALSE;
	}
	else
	{
		// first request the actual parameters
		// then show the dialog
		CExplorerOutput* pExplorerOutput = m_pVideoView->GetExplorerOutput();
		if (pExplorerOutput)
		{
			pExplorerOutput->RequestPTCSB();
			m_bMarkedForShowColorDialog = TRUE;
		}
	}
*/
}
