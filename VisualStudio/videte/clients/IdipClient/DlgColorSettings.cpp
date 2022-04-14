// DlgColorSettings.cpp : implementation file
//

#include "stdafx.h"
#include "IdipClient.h"
#include "DlgColorSettings.h"
#include "WndLive.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgColorSettings dialog
CDlgColorSettings::CDlgColorSettings(CWnd* pParent, int iColor,
									  int iBrightness,
									  int iContrast)
	: CSkinDialog(CDlgColorSettings::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgColorSettings)
	m_sTxtCamServer = _T("");
	//}}AFX_DATA_INIT
	m_iContrast = iContrast + RANGE;
	m_iBrightness = iBrightness + RANGE;
	m_iColor = iColor + RANGE;
	m_bTracking = FALSE;
	m_pWndLive = (CWndLive*)pParent;
	Create(IDD,pParent);
	ShowWindow(SW_SHOW);
	m_nInitToolTips = FALSE;
	if (UseOEMSkin())
	{
		CreateTransparent(StyleBackGroundPlaneColor, m_BaseColor);
		MoveOnClientAreaClick(true);
	}
}


void CDlgColorSettings::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgColorSettings)
	DDX_Text(pDX,IDC_TXT_CAM_SERVER,m_sTxtCamServer);
	DDX_Control(pDX, IDC_CONTRAST, m_Contrast);
	DDX_Control(pDX, IDC_COLOR, m_Color);
	DDX_Control(pDX, IDC_BRIGHTNESS, m_Brightness);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgColorSettings, CSkinDialog)
	//{{AFX_MSG_MAP(CDlgColorSettings)
	ON_WM_DESTROY()
	ON_WM_HSCROLL()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
void CDlgColorSettings::ActualizeSettings(int iColor,int iBrightness, int iContrast)
{
	if (!m_bTracking)
	{
		m_iContrast = iContrast + RANGE;
		m_iBrightness = iBrightness + RANGE;
		m_iColor = iColor + RANGE;
		m_Contrast.SetPos(m_iContrast);
		m_Brightness.SetPos(m_iBrightness);
		m_Color.SetPos(m_iColor);
	}
}
/////////////////////////////////////////////////////////////////////////////
// CDlgColorSettings message handlers
void CDlgColorSettings::PostNcDestroy() 
{
	delete this;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDlgColorSettings::OnInitDialog() 
{
	CString sText;
	CRect vrect;
	CRect srect;
	CRect drect;
	int x,y,cx,cy;
	int l,r,t,b;

	CSkinDialog::OnInitDialog();
	AutoCreateSkinStatic();
	AutoCreateSkinSlider(0, 0, 0);

	l = 2*RANGE;
	r = l / 10;
	m_Contrast.SetRange(0,l);
	m_Brightness.SetRange(0,l);
	m_Color.SetRange(0,l);
	for (t=0; t<=l; t+=r)
	{
		m_Contrast.SetTic(t);
		m_Brightness.SetTic(t);
		m_Color.SetTic(t);
	}
	m_Contrast.SetPos(m_iContrast);
	m_Brightness.SetPos(m_iBrightness);
	m_Color.SetPos(m_iColor);


	m_pWndLive->GetWindowRect(&vrect);
	GetDesktopWindow()->GetWindowRect(&srect);
	GetClientRect(&drect);
	
	cx = drect.Width();
	cy = drect.Height();
	l = vrect.left - srect.left;
	t = vrect.top - srect.top;
	r = srect.right - vrect.right;
	b = srect.bottom - vrect.bottom;

	
	if (r>cx) // check right
	{
		x = vrect.right;
		y = vrect.top;
	}
	else if (b>cy) // check bottom
	{
		x = vrect.left;
		y = vrect.bottom;
	}
	else if (l>cx) // check left
	{
		x = vrect.left - cx;
		y = vrect.top;
	}
	else if (t>cy) // check top
	{
		x = vrect.left;
		y = vrect.top - cy;
	}
	else
	{
		// doesn't fit
		x = vrect.Width()/2;
		y = vrect.Height()/2;
	}

	SetWindowPos(NULL,x,y,0,0,SWP_NOSIZE|SWP_NOZORDER);
	m_sTxtCamServer = m_pWndLive->GetServerName() + _T(" : ") + m_pWndLive->GetName();
	UpdateData(FALSE);


/*
	// actualize our controls by requesting values from Coco/Mico
	// every 5 sec
	if (m_pWndLive->GetServer()->IsB3())
	{
		SetTimer(2,10000,NULL);
	}
	else
	{
		SetTimer(2,5000,NULL);
	}
*/
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CDlgColorSettings::OnCancel() 
{
	DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CDlgColorSettings::OnOK() 
{
	DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CDlgColorSettings::OnDestroy() 
{
	KillTimer(2);
	CSkinDialog::OnDestroy();
	m_pWndLive->m_pDlgColorSettings = NULL;
}
/////////////////////////////////////////////////////////////////////////////
void CDlgColorSettings::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CSkinDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	if (pScrollBar->m_hWnd == m_Contrast.m_hWnd)
	{
		m_pWndLive->RequestSetContrast(m_Contrast.GetPos()-RANGE);
	}
	else if (pScrollBar->m_hWnd == m_Brightness.m_hWnd)
	{
		m_pWndLive->RequestSetBrightness(m_Brightness.GetPos()-RANGE);
	}
	else if (pScrollBar->m_hWnd == m_Color.m_hWnd)
	{
		m_pWndLive->RequestSetColor(m_Color.GetPos()-RANGE);
	}
	if (nSBCode == TB_ENDTRACK)
	{
		m_pWndLive->RequestCSBUpdate();
		m_bTracking = FALSE;
	}
	else
	{
		m_bTracking = TRUE;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDlgColorSettings::OnTimer(UINT nIDEvent) 
{
	m_pWndLive->RequestCSBUpdate();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDlgColorSettings::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	NMHDR *pnmhdr = (NMHDR*)lParam;
	if (pnmhdr->code == TTN_GETDISPINFO)
	{
		NMTTDISPINFO *pDI = (NMTTDISPINFO*)lParam;
		if (   wParam == (WPARAM)m_Contrast.m_hWnd
			|| wParam == (WPARAM)m_Color.m_hWnd
			|| wParam == (WPARAM)m_Brightness.m_hWnd)
		{
			int nPos = MulDiv(_ttoi(pDI->szText), 100, 30);
			wsprintf(pDI->szText, _T("%d %%"), nPos);
		}
		*pResult = 0;
	}
	return CSkinDialog::OnNotify(wParam, lParam, pResult);
}
