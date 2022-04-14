// DecodeWindow.cpp : implementation file
//

#include "stdafx.h"
#include "DecodeApp.h"
#include "DecodeWindow.h"
#include "..\PTDecoder\PTDecoderClass.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDecodeWindow

CDecodeWindow::CDecodeWindow(CPTDecoder* pDecoder)
{
	m_pDecoder = pDecoder;

	CRect rc(50,50,50+MAX_HSIZE,50+MAX_VSIZE);

	if (!Create(NULL, "PTDecoder w+k VideoCommunication", WS_OVERLAPPEDWINDOW, rc, NULL, NULL))
	{	
		TRACE("CDecodeWindow Create Window failed\n");
		return;
	}
	ShowWindow(SW_NORMAL);
}

CDecodeWindow::~CDecodeWindow()
{
}


BEGIN_MESSAGE_MAP(CDecodeWindow, CWnd)
	//{{AFX_MSG_MAP(CDecodeWindow)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDecodeWindow message handlers

void CDecodeWindow::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CRect Rc;
	GetClientRect(&Rc);
	if (m_pDecoder)
		m_pDecoder->OnDraw(dc.m_hDC, Rc);
}
