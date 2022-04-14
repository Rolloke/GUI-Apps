// CMDConfigDlg.cpp : implementation file
//

#include "stdafx.h"
#include "dvclient.h"
#include "CMDConfigDlg.h"
#include "CPanel.h"
#include "Server.h"
#include "Mainframe.h"
#include "LiveWindow.h"
#include "IPCOutputDVClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMDConfigDlg dialog

				   
CMDConfigDlg::CMDConfigDlg(CSecID camID, CWnd* pParent /*=NULL*/)
	: CTransparentDialog(CMDConfigDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMDConfigDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_hCursor		= NULL;
	m_pPanel			= (CPanel*)pParent;
	m_camID				= camID;
	m_pDCInlayWnd			= NULL;
	m_pDibImageDataInlay	= NULL;
	m_pJpeg					= new CJpeg();
	m_pJpeg->SetProperties();
	
	CByteArray  Array;
	m_pJpeg->SetOSDText1(0,0,_T(""));
	m_pJpeg->SetImageList(0, 0, NULL, Array);
	m_bShowAdaptive			= FALSE;

	m_PermMask.SetType(AM_PERMANENT);
	m_AdapMask.SetType(AM_ADAPTIVE);

	// Für die Mpeg4-Bilder muß ein Mpeg4Decoder-Objekt angelegt werden
//	m_pMpeg	= new CMPEG4Decoder();
	if (theApp.GetIntValue(_T("UseAVCodec"), 0))
	{
		try
		{
			// Versuch den AVCodec zu öffnen. Falls dies fehlschlägt, wird der emuzed decoder verwendet.
			m_pMpeg = new CAVCodec();
			WK_TRACE(_T("Using AVCodec to decoded mpeg4\n"));
		}
		catch(...)
		{
			m_pMpeg = new CMPEG4Decoder;
			WK_TRACE(_T("Using emuzed to decoded mpeg4\n"));
		}
	}
	else
	{
		m_pMpeg = new CMPEG4Decoder;
		WK_TRACE(_T("Using emuzed to decoded mpeg4\n"));
	}
	m_pMpeg->Init(NULL,0,0,0);
	m_pMpeg->SetOSDText1(0,0,_T(""));
}

CMDConfigDlg::~CMDConfigDlg()
{
	WK_DELETE(m_pDCInlayWnd);
	WK_DELETE(m_pJpeg);
	WK_DELETE(m_pMpeg);
}

void CMDConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CTransparentDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMDConfigDlg)
	DDX_Control(pDX, IDC_MD_SHOW_ADAPTIVE, m_ctrlShowAdaptiveMask);
	DDX_Control(pDX, ID_MD_OK, m_ctrlOK);
	DDX_Control(pDX, IDC_MD_INVERT, m_ctrlInvert);
	DDX_Control(pDX, IDC_MD_DEL, m_ctrlDel);
	DDX_Control(pDX, IDC_MD_INLAY, m_ctrlInlay);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMDConfigDlg, CTransparentDialog)
	//{{AFX_MSG_MAP(CMDConfigDlg)
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_MD_DEL, ClearPermanentMask)
	ON_BN_CLICKED(IDC_MD_INVERT, InvertPermanentMask)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_MD_SHOW_ADAPTIVE, OnMdShowAdaptive)
	ON_BN_CLICKED(ID_MD_OK, OnMdOk)
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMDConfigDlg message handlers

/////////////////////////////////////////////////////////////////////////////
BOOL CMDConfigDlg::OnInitDialog() 
{
	CTransparentDialog::OnInitDialog();
	
	m_hCursor = theApp.m_hArrow;

	CreateTransparent(StyleBackGroundColorChangeBrushed, CPanel::m_BaseColorBackGrounds);

	m_ctrlInlay.CreateFrame(this);
	m_ctrlInlay.EnableShadow(TRUE);

	m_ilDel.Create(IDB_DEL, 16, 0, SKIN_COLOR_KEY);
	m_ctrlDel.SetImageList(&m_ilDel);
	
	m_ilInvert.Create(IDB_INVERT, 16, 0, SKIN_COLOR_KEY);
	m_ctrlInvert.SetImageList(&m_ilInvert);

	m_ilOK.Create(IDB_ENTER, 16, 0, SKIN_COLOR_KEY);
	m_ctrlOK.SetImageList(&m_ilOK);

	m_ilAdaptiveMask.Create(IDB_SHOW_ADAPTIVE, 16, 0, SKIN_COLOR_KEY);
	m_ctrlShowAdaptiveMask.SetImageList(&m_ilAdaptiveMask);

	m_ctrlInvert.SetBaseColor(CPanel::m_BaseColorButtons);
	m_ctrlDel.SetBaseColor(CPanel::m_BaseColorButtons);
	m_ctrlOK.SetBaseColor(CPanel::m_BaseColorButtons);
	m_ctrlShowAdaptiveMask.SetBaseColor(CPanel::m_BaseColorButtons);
	m_ctrlShowAdaptiveMask.SetStyle(StyleCheck);

	CRect rcPanel;
	if (m_pPanel)
	{
		m_pPanel->GetWindowRect(rcPanel);

		CRect rcAlarmlistDlg;
		GetWindowRect(rcAlarmlistDlg);
		
		// StatisticDialog oberhalb des Panels positionieren
		int nX	= rcPanel.right -rcAlarmlistDlg.Width();
		int	nY	= rcPanel.top - rcAlarmlistDlg.Height();

		if (nX < 0 || nY < 0)
		{
			// RTEDialog unterhalb des Panels positionieren
			nX	= rcPanel.left;
			nY	= rcPanel.bottom;
		}
		SetWindowPos(&wndTopMost, nX, nY, -1, -1, SWP_NOSIZE);
	}

	ShowWindow(SW_SHOW);

	// Koordinaten des Maskfenster ermitteln
	CWnd* pWndMask	= GetDlgItem(IDC_MD_INLAY);
	GetClientRect(m_rcDlg); 
	ClientToScreen(m_rcDlg);
	pWndMask->GetWindowRect(m_rcMask);

	m_nXMaskOffset = m_rcMask.left - m_rcDlg.left;
	m_nYMaskOffset = m_rcMask.top  - m_rcDlg.top;

	if (!m_pDCInlayWnd)
		m_pDCInlayWnd = new CWindowDC(GetDlgItem(IDC_MD_INLAY));

	SetTimer(1, 500, NULL);

	LoadMask();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
void CMDConfigDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	OnMouseMove(nFlags, point);
	
	CTransparentDialog::OnLButtonDown(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
void CMDConfigDlg::OnRButtonDown(UINT nFlags, CPoint point) 
{
	OnMouseMove(nFlags, point);
	
	CTransparentDialog::OnRButtonDown(nFlags, point);
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

	CTransparentDialog::OnMouseMove(nFlags, point);
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
BOOL CMDConfigDlg::LoadMask()
{
	BOOL bResult = FALSE;

	WK_TRACE(_T("LoadMask %08lx\n"),m_camID.GetID());
	CServer* pServer = theApp.GetServer();

	if (pServer && pServer->IsOutputConnected())
	{
		CIPCOutputDVClient* pCIPC = pServer->GetOutput();
		pCIPC->DoRequestGetMask(m_camID,AM_PERMANENT,0);
		if (WAIT_TIMEOUT == WaitForSingleObject(m_eventGetPermMask,2000))
			WK_TRACE_WARNING(_T("TIMEOUT LoadMaskPermanent %08lx\n"),m_camID.GetID());
		pCIPC->DoRequestGetMask(m_camID,AM_ADAPTIVE,0);
		if (WAIT_TIMEOUT == WaitForSingleObject(m_eventGetAdapMask,2000))
			WK_TRACE_WARNING(_T("TIMEOUT LoadMaskAdaptive %08lx\n"),m_camID.GetID());
	}
	else
	{
		WK_TRACE_ERROR(_T("server not connected loading mask\n"));
	}

	return bResult;
}
//////////////////////////////////////////////////////////////////////
void CMDConfigDlg::ConfirmGetMask(CSecID camID, const CIPCActivityMask& mask)
{
	TRACE(_T("ConfirmGetMask %08lx (%d,%d)\n"),camID.GetID(),mask.GetWidth(),mask.GetHeight());
	if (mask.GetType() == AM_PERMANENT)
	{
		m_csMasks.Lock();
		m_PermMask = mask;
		m_csMasks.Unlock();
		m_eventGetPermMask.SetEvent();
	}
	else if (mask.GetType() == AM_ADAPTIVE)
	{
		m_csMasks.Lock();
		m_AdapMask = mask;
		m_csMasks.Unlock();
		m_eventGetAdapMask.SetEvent();
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CMDConfigDlg::SaveMask()
{	
	BOOL bResult = FALSE;
	
	CServer* pServer = theApp.GetServer();

	WK_TRACE(_T("SaveMask %08lx\n"),m_camID.GetID());
	if (pServer && pServer->IsOutputConnected())
	{
		CIPCOutputDVClient* pCIPC = pServer->GetOutput();
		pCIPC->DoRequestSetMask(m_camID,0,m_PermMask);

		if (WAIT_TIMEOUT == WaitForSingleObject(m_eventSetPermMask,2000))
		{
			WK_TRACE_WARNING(_T("TIMEOUT SaveMask %08lx\n"),m_camID.GetID());
		}
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
		m_eventSetPermMask.SetEvent();
	else if (type == AM_ADAPTIVE)
		m_eventSetAdapMask.SetEvent();
	else
		WK_TRACE(_T("Unknown MaskType\n"));
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
	CLiveWindow* pLW = theApp.GetMainFrame()->GetLiveWindow(m_camID.GetSubID());
	CIPCPictureRecord* pPict = NULL;

	if (WK_IS_WINDOW(pLW))
	{
		pPict = pLW->GetPictureCopy();

		if (pPict)
		{
			CompressionType ct = pPict->GetCompressionType();

			CBitmap Bitmap;
			Bitmap.CreateCompatibleBitmap(m_pDCInlayWnd, rectClient.Width(), rectClient.Height());
			CBitmap* pOldBitmap = memDC.SelectObject(&Bitmap);

			if (ct == COMPRESSION_JPEG || ct == COMPRESSION_RGB_24 || ct == COMPRESSION_YUV_422)
				pLW->GetJpegDecoder()->OnDraw(&memDC, rectClient);
			else if (ct == COMPRESSION_MPEG4)
				pLW->GetMpeg4Decoder()->OnDraw(&memDC, rectClient);
			else
				WK_TRACE_ERROR(_T("CMDConfigDlg::ShowFrame Unknow Compressiontype (%d)\n"), ct);

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
					if (m_PermMask.GetAt(nX, nY) > AM_UPPER_BOUNDARY/2)
						memDC.FillRect(rc, &brush1);
				}
			}
			m_csMasks.Unlock();

			// Und das Offscreensurface zeichnen
			m_pDCInlayWnd->BitBlt(0, 0, rectClient.Width(), rectClient.Height(), &memDC, 0, 0, SRCCOPY);

			memDC.SelectObject(pOldBitmap);
			Bitmap.DeleteObject();
			memDC.DeleteDC();

			WK_DELETE(pPict);
		}
	}
}

//////////////////////////////////////////////////////////////////////
void CMDConfigDlg::OnTimer(UINT nIDEvent) 
{
	ShowFrame();

	CTransparentDialog::OnTimer(nIDEvent);
}
//////////////////////////////////////////////////////////////////////
void CMDConfigDlg::OnMdShowAdaptive() 
{
	m_bShowAdaptive = m_ctrlShowAdaptiveMask.GetCheck();
}

//////////////////////////////////////////////////////////////////////
void CMDConfigDlg::OnCancel()
{
	// do nothing
}
//////////////////////////////////////////////////////////////////////
void CMDConfigDlg::OnMdOk() 
{
	KillTimer(1);
	SaveMask();
	EndDialog(IDOK);
}
//////////////////////////////////////////////////////////////////////
BOOL CMDConfigDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (m_hCursor)
	{
		if (SetCursor(m_hCursor) != NULL)
			return TRUE;		
	}	
	
	return CTransparentDialog::OnSetCursor(pWnd, nHitTest, message);
}

