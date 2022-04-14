// CMDConfigDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vision.h"
#include "MDConfigDlg.h"
#include "DisplayWindow.h"
#include "Server.h"
#include "CIPCOutputVision.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMDConfigDlg dialog

				   
CMDConfigDlg::CMDConfigDlg(CSecID camID, CDisplayWindow* pParent)
	: CDialog(CMDConfigDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMDConfigDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pDisplayWindow	= pParent;
	m_camID				= camID;
	m_pDCInlayWnd			= NULL;
	m_pDibImageDataInlay	= NULL;
	m_bShowAdaptive			= FALSE;
	m_bMaskLoaded			= FALSE;
	m_CIPCError				= CIPC_ERROR_OKAY;

	m_PermMask.SetType(AM_PERMANENT);
	m_AdapMask.SetType(AM_ADAPTIVE);

	Create(IDD,pParent);
}

CMDConfigDlg::~CMDConfigDlg()
{
	WK_DELETE(m_pDCInlayWnd);
}

void CMDConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMDConfigDlg)
	DDX_Control(pDX, IDC_MD_SHOW_ADAPTIVE, m_ctrlShowAdaptive);
	DDX_Control(pDX, IDC_MD_OK, m_ctrlOK);
	DDX_Control(pDX, IDC_MD_INVERT, m_ctrlInvert);
	DDX_Control(pDX, IDC_MD_DEL, m_ctrlDel);
	DDX_Control(pDX, IDC_MD_INLAY, m_ctrlInlay);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMDConfigDlg, CDialog)
	//{{AFX_MSG_MAP(CMDConfigDlg)
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_MD_DEL, ClearPermanentMask)
	ON_BN_CLICKED(IDC_MD_INVERT, InvertPermanentMask)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_MD_SHOW_ADAPTIVE, OnMdShowAdaptive)
	ON_BN_CLICKED(IDC_MD_OK, OnMdOk)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMDConfigDlg message handlers

/////////////////////////////////////////////////////////////////////////////
BOOL CMDConfigDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	

	m_ctrlInlay.CreateFrame(this);
	m_ctrlInlay.EnableShadow(TRUE);

	CenterWindow();
	ShowWindow(SW_SHOW);

	// Koordinaten des Maskfenster ermitteln
	CWnd* pWndMask	= GetDlgItem(IDC_MD_INLAY);
	GetClientRect(m_rcDlg); 
	ClientToScreen(m_rcDlg);
	pWndMask->GetWindowRect(m_rcMask);

	m_nXMaskOffset = m_rcMask.left - m_rcDlg.left;
	m_nYMaskOffset = m_rcMask.top  - m_rcDlg.top;

	if (!m_pDCInlayWnd)
	{
		m_pDCInlayWnd = new CWindowDC(GetDlgItem(IDC_MD_INLAY));
	}

	SetWindowText(m_pDisplayWindow->GetName());

	SetTimer(1, 500, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
void CMDConfigDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	OnMouseMove(nFlags, point);
	
	CDialog::OnLButtonDown(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
void CMDConfigDlg::OnRButtonDown(UINT nFlags, CPoint point) 
{
	OnMouseMove(nFlags, point);
	
	CDialog::OnRButtonDown(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
void CMDConfigDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	int		nX		= point.x;
	int		nY		= point.y;
	int		nX1		= 0;
	int		nY1		= 0;
	BOOL	bMask	= FALSE;

	nX1 = nX - m_nXMaskOffset;
	nY1 = nY - m_nYMaskOffset;

	if ((nX1 >= 0) &&
		(nX1 <= m_rcMask.Width()) &&
		(nY1 >= 0) &&
		(nY1 <= m_rcMask.Height()))
		bMask = TRUE;
	
	if (bMask)
	{
		if (nFlags & MK_LBUTTON)
		{
			if (bMask)
				SetPixelInMask(nX1 , nY1, TRUE);
		}

		if (nFlags & MK_RBUTTON)
		{
			if (bMask)
				SetPixelInMask(nX1, nY1, FALSE);
		}
	}
	
	CDialog::OnMouseMove(nFlags, point);
}

//////////////////////////////////////////////////////////////////////
void CMDConfigDlg::SetPixelInMask(int nXM, int nYM, BOOL bFlag)
{
	CRect rectClient;
	GetDlgItem(IDC_MD_INLAY)->GetClientRect(rectClient);	
	
	nXM = (nXM * m_PermMask.GetSize().cx) / rectClient.Width();
	nYM = (nYM * m_PermMask.GetSize().cy) / rectClient.Height();

	nXM = min(max(nXM, 0), m_PermMask.GetSize().cx);
	nYM = min(max(nYM, 0), m_PermMask.GetSize().cy);
	
//	TRACE(_T("nXM=%d, nYM=%d\n"), nXM, nYM);

	if (bFlag)
		m_PermMask.SetAt(nXM, nYM, AM_UPPER_BOUNDARY);
	else
		m_PermMask.SetAt(nXM, nYM, AM_LOWER_BOUNDARY);

	ShowFrame();
}

//////////////////////////////////////////////////////////////////////
void CMDConfigDlg::LoadMask()
{
	WK_TRACE(_T("LoadMask %08lx\n"),m_camID.GetID());
	CServer* pServer = m_pDisplayWindow->GetServer();

	if (pServer && pServer->IsConnected())
	{
		CIPCOutputVision* pCIPC = pServer->GetOutput();
		pCIPC->DoRequestGetMask(m_camID,AM_PERMANENT,0);
		pCIPC->DoRequestGetMask(m_camID,AM_ADAPTIVE,0);
		m_bMaskLoaded = TRUE;
/*
		if (WAIT_TIMEOUT == WaitForSingleObject(m_eventGetPermMask,2000))
		{
			WK_TRACE_WARNING(_T("TIMEOUT LoadMaskPermanent %08lx\n"),m_camID.GetID());
		}
		if (m_CIPCError == CIPC_ERROR_OKAY)
		{
			if (WAIT_TIMEOUT == WaitForSingleObject(m_eventGetAdapMask,2000))
			{
				WK_TRACE_WARNING(_T("TIMEOUT LoadMaskAdaptive %08lx\n"),m_camID.GetID());
			}
		}
		else
		{
			CString s;
			s.Format(IDP_MASK_DENIED,m_pMicoWindow->GetName());
			COemGuiApi::MessageBox(s,20);
			KillTimer(1);
			DestroyWindow();
		}
*/
	}
	else
	{
		WK_TRACE_ERROR(_T("server not connected loading mask\n"));
	}
}
//////////////////////////////////////////////////////////////////////
void CMDConfigDlg::ConfirmGetMask(CSecID camID, const CIPCActivityMask& mask)
{
	TRACE(_T("ConfirmGetMask %08lx (%d,%d)\n"),camID.GetID(),mask.GetWidth(),mask.GetHeight());
	if (mask.GetType() == AM_PERMANENT)
	{
		m_csMasks.Lock();
		m_PermMask = mask;
		m_CIPCError = CIPC_ERROR_OKAY;
		m_csMasks.Unlock();
	}
	else if (mask.GetType() == AM_ADAPTIVE)
	{
		m_csMasks.Lock();
		m_AdapMask = mask;
		m_CIPCError = CIPC_ERROR_OKAY;
		m_csMasks.Unlock();
	}
	PostMessage(WM_TIMER,1);
}
//////////////////////////////////////////////////////////////////////
BOOL CMDConfigDlg::SaveMask()
{	
	BOOL bResult = FALSE;
	
	CServer* pServer = m_pDisplayWindow->GetServer();

	WK_TRACE(_T("SaveMask %08lx\n"),m_camID.GetID());
	if (pServer && pServer->IsConnected())
	{
		CIPCOutputVision* pCIPC = pServer->GetOutput();
		pCIPC->DoRequestSetMask(m_camID,0,m_PermMask);
	}
	else
	{
		WK_TRACE_ERROR(_T("server not connected save mask\n"));
	}

	return bResult;
}
//////////////////////////////////////////////////////////////////////
void CMDConfigDlg::ConfirmSetMask(CSecID camID, MaskType type)
{
	TRACE(_T("ConfirmSetMask %08lx\n"),camID.GetID());
	if (type == AM_PERMANENT)
	{
		m_CIPCError = CIPC_ERROR_OKAY;
	}
	else if (type == AM_ADAPTIVE)
	{
		m_CIPCError = CIPC_ERROR_OKAY;
	}
	else
	{
		WK_TRACE(_T("Unknown MaskType\n"));
	}
}
//////////////////////////////////////////////////////////////////////
void CMDConfigDlg::ClearPermanentMask()
{
	// Alte Maske löschen
	m_csMasks.Lock();
	m_PermMask.Clear();
	m_csMasks.Unlock();
	ShowFrame();
}

//////////////////////////////////////////////////////////////////////
void CMDConfigDlg::InvertPermanentMask()
{
	// Permanente Maske invertieren
	m_csMasks.Lock();
	m_PermMask.Invert();
	m_csMasks.Unlock();
	ShowFrame();
}

//////////////////////////////////////////////////////////////////////
// Hier muß das Bild und die Maske gezeichnet werden.
void CMDConfigDlg::ShowFrame()
{
	CRect rectClient;
	GetDlgItem(IDC_MD_INLAY)->GetClientRect(rectClient);	
	rectClient.DeflateRect(1,1); // Rahmenbreite abziehen
	rectClient.OffsetRect(1,1);

	// Offscreensurface anlegen
	CDC memDC;
	memDC.CreateCompatibleDC(m_pDCInlayWnd);
	
	// Kompatible Bitmap mit dem Offscreensurface verbinden
	CBitmap Bitmap;
	Bitmap.CreateCompatibleBitmap(m_pDCInlayWnd, rectClient.Width(), rectClient.Height());
	CBitmap* pOldBitmap = memDC.SelectObject(&Bitmap);

	m_pDisplayWindow->DrawBitmap(&memDC,rectClient);

	double dXM = (double)rectClient.Width()  / (double)m_PermMask.GetSize().cx;
	double dYM = (double)rectClient.Height() / (double)m_PermMask.GetSize().cy;

	CBrush brush1(RGB(255,0,0));
	m_csMasks.Lock();
	memDC.SetBkMode(TRANSPARENT);
	for (int nY = 0; nY < m_PermMask.GetSize().cy; nY++)
	{
		for (int nX = 0; nX < m_PermMask.GetSize().cx; nX++)
		{
			CRect rc((int)(nX*dXM), (int)(nY*dYM), (int)((nX+1)*dXM), (int)((nY+1)*dYM));	
			rc.OffsetRect(2,2);
			if (m_bShowAdaptive && m_AdapMask.GetAt(nX, nY) > AM_LOWER_BOUNDARY)
			{
				CBrush brush2(RGB(0,m_AdapMask.GetAt(nX, nY),0));
				memDC.FillRect(rc, &brush2);
			}
			if (m_PermMask.GetAt(nX, nY) == AM_UPPER_BOUNDARY)
				memDC.FillRect(rc, &brush1);

		}
	}
	m_csMasks.Unlock();

	// Und das Offscreensurface zeichnen
	m_pDCInlayWnd->BitBlt(0, 0, rectClient.Width(), rectClient.Height(), &memDC, 0, 0, SRCCOPY);

	memDC.SelectObject(pOldBitmap);
	Bitmap.DeleteObject();
	memDC.DeleteDC();

}
	
//////////////////////////////////////////////////////////////////////
void CMDConfigDlg::OnTimer(UINT nIDEvent) 
{
	if (m_CIPCError == CIPC_ERROR_OKAY)
	{
		if (!m_bMaskLoaded)
		{
			LoadMask();
		}
		else
		{
			ShowFrame();
		}
	}
	else
	{
		CString s;
		s.Format(IDP_MASK_DENIED,m_pDisplayWindow->GetName());
		COemGuiApi::MessageBox(s,20);
		KillTimer(1);
		DestroyWindow();
	}
}
//////////////////////////////////////////////////////////////////////
void CMDConfigDlg::OnMdShowAdaptive() 
{
	m_bShowAdaptive = m_ctrlShowAdaptive.GetCheck();
	PostMessage(WM_TIMER,1);
}
//////////////////////////////////////////////////////////////////////
void CMDConfigDlg::OnCancel()
{
	DestroyWindow();
}
//////////////////////////////////////////////////////////////////////
void CMDConfigDlg::OnMdOk() 
{
	KillTimer(1);
	SaveMask();
	DestroyWindow();
}
//////////////////////////////////////////////////////////////////////
void CMDConfigDlg::PostNcDestroy() 
{
	m_pDisplayWindow->m_pMDConfigDlg = NULL;
	delete this;
}
//////////////////////////////////////////////////////////////////////
void CMDConfigDlg::OnIndicateError(DWORD dwCmd, CIPCError error, int iErrorCode)
{
	switch (dwCmd)
	{
	case CIPC_OUTPUT_REQUEST_GET_MASK:
		switch (iErrorCode)
		{
		case AM_PERMANENT:
			m_CIPCError = error;
			break;
		case AM_ADAPTIVE:
			m_CIPCError = error;
			break;
		}
		break;
	case CIPC_OUTPUT_REQUEST_SET_MASK:
		switch (iErrorCode)
		{
		case AM_PERMANENT:
			m_CIPCError = error;
			break;
		case AM_ADAPTIVE:
			m_CIPCError = error;
			break;
		}
		break;
	}
	PostMessage(WM_TIMER,1);
}
