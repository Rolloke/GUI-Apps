// CDlgMDConfig.cpp : implementation file
//

#include "stdafx.h"
#include "IdipClient.h"
#include "DlgMDConfig.h"
#include "WndLive.h"
#include "Server.h"
#include "CIPCOutputIdipClient.h"
#include ".\dlgmdconfig.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define UPDATE_ALARM_SENSITIVITY 1
#define UPDATE_MASK_SENSITIVITY  2

/////////////////////////////////////////////////////////////////////////////
// CDlgMDConfig dialog

				   
CDlgMDConfig::CDlgMDConfig(CSecID camID, CWndLive* pParent)
	: CSkinDialog(CDlgMDConfig::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgMDConfig)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pWndLive	= pParent;
	m_camID				= camID;
	m_pDCInlayWnd			= NULL;
	m_pDibImageDataInlay	= NULL;
	m_bShowAdaptive			= FALSE;
	m_bPermMaskLoaded		= FALSE;
	m_bAdapMaskLoaded		= FALSE;
	m_CIPCError				= CIPC_ERROR_OKAY;
	m_iAlarmSensitivity		= -1;
	m_iMaskSensitivity		= -1;


	m_PermMask.SetType(AM_PERMANENT);
	m_AdapMask.SetType(AM_ADAPTIVE);

	m_nInitToolTips = TOOLTIPS_SIMPLE;
	if (UseOEMSkin())
	{
		CreateTransparent(StyleBackGroundColorChangeSimple, m_BaseColor);
		// MoveOnClientAreaClick(false);
	}
	Create(IDD,pParent);
}

CDlgMDConfig::~CDlgMDConfig()
{
	WK_DELETE(m_pDCInlayWnd);
}

void CDlgMDConfig::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgMDConfig)
	DDX_Control(pDX, IDC_MD_SHOW_ADAPTIVE, m_ctrlShowAdaptive);
	DDX_Control(pDX, IDC_MD_OK, m_ctrlOK);
	DDX_Control(pDX, IDC_MD_INVERT, m_ctrlInvert);
	DDX_Control(pDX, IDC_MD_DEL, m_ctrlDel);
	DDX_Control(pDX, IDC_MD_INLAY, m_ctrlInlay);
	DDX_Control(pDX, IDC_MD_COMBO_SENSITIVITY, m_cbMaskSensitivity);
	DDX_Control(pDX, IDC_MD_COMBO_SENSITIVITY_ALARM, m_cbAlarmSensitivity);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgMDConfig, CSkinDialog)
	//{{AFX_MSG_MAP(CDlgMDConfig)
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_BN_CLICKED(IDC_MD_DEL, ClearPermanentMask)
	ON_BN_CLICKED(IDC_MD_INVERT, InvertPermanentMask)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_MD_SHOW_ADAPTIVE, OnMdShowAdaptive)
	ON_BN_CLICKED(IDC_MD_OK, OnMdOk)
	ON_CBN_SELCHANGE(IDC_MD_COMBO_SENSITIVITY, OnCbnSelchangeMdComboSensitivity)
	ON_MESSAGE(WM_USER, OnUser)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgMDConfig message handlers

/////////////////////////////////////////////////////////////////////////////
BOOL CDlgMDConfig::OnInitDialog() 
{
	CSkinDialog::OnInitDialog();
	AutoCreateSkinButtons();
	AutoCreateSkinStatic();
	AutoCreateSkinComboBox();

	m_ctrlInlay.CreateFrame(this);
	m_ctrlInlay.EnableShadow(TRUE);

	m_ctrlInlay.EnableActionOnButtonDown();
	m_ctrlInlay.EnableActionOnButtonUp();

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

	SetWindowText(m_pWndLive->GetName());
	CString str;

	int i, nID[] = {IDS_OFF, IDS_LOW, IDS_NORMAL, IDS_HIGH, 0};
	for (i=1; nID[i] != 0; i++)
	{
		str.LoadString(nID[i]);
		m_cbAlarmSensitivity.AddString(str);
	}

	for (i=0; nID[i] != 0; i++)
	{
		str.LoadString(nID[i]);
		m_cbMaskSensitivity.AddString(str);
	}
	SetTimer(1, 500, NULL);

	CenterWindow();
	ShowWindow(SW_SHOW);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
void CDlgMDConfig::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_ptButtonDown = point;
}
/////////////////////////////////////////////////////////////////////////////
void CDlgMDConfig::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_ptButtonUp = point;
	ProcessRectangle(MK_LBUTTON);
}
/////////////////////////////////////////////////////////////////////////////
void CDlgMDConfig::ProcessRectangle(UINT nFlags)
{
	TRACE(_T("%d,%d-%d,%d\n"),m_ptButtonDown.x,m_ptButtonDown.y,m_ptButtonUp.x,m_ptButtonUp.y);

	if (m_ptButtonUp == m_ptButtonDown)
	{
		SetPixelPoint(nFlags,m_ptButtonDown,FALSE);
	}
	else
	{
		for (int i=m_ptButtonDown.x;i<m_ptButtonUp.x;i++)
		{
			for (int j=m_ptButtonDown.y;j<m_ptButtonUp.y;j++)
			{
				CPoint pt;
				pt.x = i;
				pt.y = j;
				SetPixelPoint(nFlags,pt,FALSE);
			}
		}
	}
	ShowFrame();
}
/////////////////////////////////////////////////////////////////////////////
void CDlgMDConfig::OnRButtonDown(UINT nFlags, CPoint point) 
{
	m_ptButtonDown = point;
}
/////////////////////////////////////////////////////////////////////////////
void CDlgMDConfig::OnRButtonUp(UINT nFlags, CPoint point) 
{
	m_ptButtonUp = point;
	ProcessRectangle(MK_RBUTTON);
}
/////////////////////////////////////////////////////////////////////////////
void CDlgMDConfig::SetPixelPoint(UINT nFlags, CPoint point, BOOL bRedraw)
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
				SetPixelInMask(nX1 , nY1, TRUE,bRedraw);
		}

		if (nFlags & MK_RBUTTON)
		{
			if (bMask)
				SetPixelInMask(nX1, nY1, FALSE,bRedraw);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDlgMDConfig::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (nFlags & MK_LBUTTON)
	{
		OnLButtonUp(nFlags,point);
	}
	if (nFlags & MK_RBUTTON)
	{
		OnRButtonUp(nFlags,point);
	}
	CSkinDialog::OnMouseMove(nFlags, point);
}

//////////////////////////////////////////////////////////////////////
void CDlgMDConfig::SetPixelInMask(int nXM, int nYM, BOOL bFlag, BOOL bRedraw)
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

	if (bRedraw)
	{
		ShowFrame();
	}
}

//////////////////////////////////////////////////////////////////////
void CDlgMDConfig::LoadMask()
{
	WK_TRACE(_T("LoadMask %08lx\n"),m_camID.GetID());
	CServer* pServer = m_pWndLive->GetServer();

	if (pServer && pServer->IsConnectedLive())
	{
		CIPCOutputIdipClient* pCIPC = pServer->GetOutput();
		
		if (!m_bPermMaskLoaded)
			pCIPC->DoRequestGetMask(m_camID,AM_PERMANENT,0);
		if (!m_bAdapMaskLoaded)
			pCIPC->DoRequestGetMask(m_camID,AM_ADAPTIVE,0);
	}
	else
	{
		WK_TRACE_ERROR(_T("server not connected loading mask\n"));
	}
}

//////////////////////////////////////////////////////////////////////
void CDlgMDConfig::ConfirmGetMask(CSecID camID, const CIPCActivityMask& mask)
{
	TRACE(_T("ConfirmGetMask %08lx (%d,%d)\n"),camID.GetID(),mask.GetWidth(),mask.GetHeight());
	if (mask.GetType() == AM_PERMANENT)
	{
		m_csMasks.Lock();
		m_PermMask = mask;
		m_CIPCError = CIPC_ERROR_OKAY;
		m_bPermMaskLoaded = TRUE;
		m_csMasks.Unlock();
	}
	else if (mask.GetType() == AM_ADAPTIVE)
	{
		m_csMasks.Lock();
		m_AdapMask = mask;
		m_CIPCError = CIPC_ERROR_OKAY;
		m_bAdapMaskLoaded = TRUE;
		m_csMasks.Unlock();
	}
	PostMessage(WM_TIMER,1);
}
//////////////////////////////////////////////////////////////////////
BOOL CDlgMDConfig::SaveMask()
{	
	BOOL bResult = FALSE;
	
	CServer* pServer = m_pWndLive->GetServer();

	WK_TRACE(_T("SaveMask %08lx\n"),m_camID.GetID());
	if (pServer && pServer->IsConnectedLive())
	{
		CIPCOutputIdipClient* pCIPC = pServer->GetOutput();
		pCIPC->DoRequestSetMask(m_camID,0,m_PermMask);
		CString sValue;
		LPCTSTR pszValue[] = { CSD_OFF, CSD_LOW, CSD_NORMAL, CSD_HIGH };
		int nValue = m_cbMaskSensitivity.GetCurSel();
		if (IsBetween(nValue, 0, 3))
		{
			sValue = pszValue[nValue];
		}
		else
		{
			sValue = pszValue[0];
		}
		pCIPC->DoRequestSetValue(m_camID, CSD_MD_MASK_SENSITIVITY, sValue);
		nValue = m_cbAlarmSensitivity.GetCurSel();
		if (IsBetween(nValue, 0, 2))
		{
			sValue = pszValue[nValue+1];
		}
		else
		{
			sValue = pszValue[0];
		}
		pCIPC->DoRequestSetValue(m_camID, CSD_MD_ALARM_SENSITIVITY, sValue);
	}
	else
	{
		WK_TRACE_ERROR(_T("server not connected save mask\n"));
	}

	return bResult;
}
//////////////////////////////////////////////////////////////////////
void CDlgMDConfig::ConfirmSetMask(CSecID camID, MaskType type)
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
void CDlgMDConfig::ClearPermanentMask()
{
	// Alte Maske löschen
	m_csMasks.Lock();
	m_PermMask.Clear();
	m_csMasks.Unlock();
	ShowFrame();
}

//////////////////////////////////////////////////////////////////////
void CDlgMDConfig::InvertPermanentMask()
{
	// Permanente Maske invertieren
	m_csMasks.Lock();
	m_PermMask.Invert();
	m_csMasks.Unlock();
	ShowFrame();
}

//////////////////////////////////////////////////////////////////////
// Hier muß das Bild und die Maske gezeichnet werden.
void CDlgMDConfig::ShowFrame()
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

	m_pWndLive->DrawBitmap(&memDC,rectClient);

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
void CDlgMDConfig::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == 1)
	{
		if (m_iAlarmSensitivity	== -1 || m_iMaskSensitivity	== -1)
		{
			CServer* pServer = m_pWndLive->GetServer();
			if (pServer && pServer->IsConnectedLive())
			{
				CIPCOutputIdipClient* pCIPC = pServer->GetOutput();
				pCIPC->DoRequestGetValue(m_camID, CSD_MD_MASK_SENSITIVITY);
				pCIPC->DoRequestGetValue(m_camID, CSD_MD_ALARM_SENSITIVITY);
			}
		}
		if (m_CIPCError == CIPC_ERROR_OKAY)
		{
			if (!m_bPermMaskLoaded || !m_bAdapMaskLoaded)
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
			s.Format(IDP_MASK_DENIED,m_pWndLive->GetName());
			COemGuiApi::MessageBox(s,20);
			KillTimer(1);
			DestroyWindow();
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CDlgMDConfig::OnMdShowAdaptive() 
{
	m_bShowAdaptive = m_ctrlShowAdaptive.GetCheck();
	PostMessage(WM_TIMER,1);
}
//////////////////////////////////////////////////////////////////////
void CDlgMDConfig::OnCancel()
{
	DestroyWindow();
}
//////////////////////////////////////////////////////////////////////
void CDlgMDConfig::OnMdOk() 
{
	KillTimer(1);
	SaveMask();
	DestroyWindow();
}
//////////////////////////////////////////////////////////////////////
void CDlgMDConfig::PostNcDestroy() 
{
	m_pWndLive->m_pDlgMDConfig = NULL;
	delete this;
}
//////////////////////////////////////////////////////////////////////
void CDlgMDConfig::OnIndicateError(DWORD dwCmd, CIPCError error, int iErrorCode)
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
//////////////////////////////////////////////////////////////////////
void CDlgMDConfig::OnCbnSelchangeMdComboSensitivity()
{

}
//////////////////////////////////////////////////////////////////////
void CDlgMDConfig::ConfirmMaskSensitivity(int nMask, int nSensitivity)
{
	// Achtung in CIPC Thread!
	if (nMask == 0)
	{
		m_iMaskSensitivity = nSensitivity;
		PostMessage(WM_USER, UPDATE_MASK_SENSITIVITY);
	}
	else
	{
		m_iAlarmSensitivity = nSensitivity;
		PostMessage(WM_USER, UPDATE_ALARM_SENSITIVITY);
	}
}
//////////////////////////////////////////////////////////////////////
LRESULT CDlgMDConfig::OnUser(WPARAM wParam, LPARAM lParam)
{
	if (wParam == UPDATE_ALARM_SENSITIVITY)
	{
		int iSel = 0;
		iSel = m_cbAlarmSensitivity.GetCurSel();
		if (iSel != m_iAlarmSensitivity-1)
		{
			m_cbAlarmSensitivity.SetCurSel(m_iAlarmSensitivity-1);
		}
	}
	else if (wParam == UPDATE_MASK_SENSITIVITY)
	{
		int iSel = m_cbMaskSensitivity.GetCurSel();
		if (iSel != m_iMaskSensitivity)
		{
			m_cbMaskSensitivity.SetCurSel(m_iMaskSensitivity);
		}
	}
	return 0;
}
