/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ptwindow.cpp $
// ARCHIVE:		$Archive: /Project/Clients/Vision/ptwindow.cpp $
// CHECKIN:		$Date: 3.10.02 10:47 $
// VERSION:		$Revision: 15 $
// LAST CHANGE:	$Modtime: 3.10.02 10:46 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"

#include "Vision.h"
#include "VisionDoc.h"
#include "VisionView.h"
#include "ObjectView.h"
#include "Server.h"

#include "PTWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// PT Resolutions
// RESOLUTION_LOW 176 x 144, 11:9	
// RESOLUTION_HIGH 352 x 288, 11:9
/////////////////////////////////////////////////////////////////////////////
// CPTWindow
CPTWindow::CPTWindow(CServer* pServer, const CIPCOutputRecord &or) 
	: CDisplayWindow(pServer, or)
{
	m_iX = 11;
	m_iY = 9;
	m_rcZoom = CRect(0,0,0,0);
	m_pDecoder = new CPTDecoder(TRUE);
}
/////////////////////////////////////////////////////////////////////////////
CPTWindow::~CPTWindow()
{
	delete m_pDecoder;
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CPTWindow, CDisplayWindow)
	//{{AFX_MSG_MAP(CPTWindow)
	ON_COMMAND(ID_CHANGE_VIDEO, OnChangeVideo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
BOOL CPTWindow::IsPresence()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
CSize CPTWindow::GetPictureSize()
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
void CPTWindow::OnDraw(CDC* pDC)
{
	CDisplayWindow::OnDraw(pDC);

	if (m_bWasSomeTimesActive)
	{
		CRect rect;
		GetVideoClientRect(rect);
		m_pDecoder->OnDraw(pDC->m_hDC,rect, m_rcZoom);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPTWindow::OnChangeVideo() 
{
	// called after CopyToBuffer, now we are in main thread
	m_csPictureRecord.Lock();

	if (   m_pPictureRecord
		&& m_pPictureRecord->GetCompressionType() == COMPRESSION_PRESENCE)
	{
		m_pDecoder->Decode((BYTE*)m_pPictureRecord->GetData(),
								m_pPictureRecord->GetDataLength(),
								m_pPictureRecord->GetPictureType()!=SPT_DIFF_PICTURE,
								FALSE);
		m_bWasSomeTimesActive = TRUE;
	}
	m_csPictureRecord.Unlock();
	if (m_pVisionView->SetRequestedDisplayWindow(this))
	{
		CRect rect;
		GetClientRect(rect);
		InvalidateRect(rect,FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPTWindow::Set1to1(BOOL b1to1)
{
	m_b1to1 = b1to1;
	RedrawWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CPTWindow::Settings()
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
