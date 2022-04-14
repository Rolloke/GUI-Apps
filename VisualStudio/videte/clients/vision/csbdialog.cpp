// CSBDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Vision.h"
#include "CSBDialog.h"
#include "DisplayWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCSBDialog dialog
CCSBDialog::CCSBDialog(CWnd* pParent, int iColor,
									  int iBrightness,
									  int iContrast)
	: CDialog(CCSBDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCSBDialog)
	m_sTxtCamServer = _T("");
	//}}AFX_DATA_INIT
	m_iContrast = iContrast + RANGE;
	m_iBrightness = iBrightness + RANGE;
	m_iColor = iColor + RANGE;
	// GF todo m_SmallWindows.Lock; CAVEAT Can be deleted in the meantime!!!
	m_pDisplayWindow = (CDisplayWindow*)pParent;
	Create(IDD,pParent);
	ShowWindow(SW_SHOW);
}


void CCSBDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCSBDialog)
	DDX_Text(pDX,IDC_TXT_CAM_SERVER,m_sTxtCamServer);
	DDX_Control(pDX, IDC_CONTRAST, m_Contrast);
	DDX_Control(pDX, IDC_COLOR, m_Color);
	DDX_Control(pDX, IDC_BRIGHTNESS, m_Brightness);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCSBDialog, CDialog)
	//{{AFX_MSG_MAP(CCSBDialog)
	ON_WM_DESTROY()
	ON_WM_HSCROLL()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
void CCSBDialog::ActualizeSettings(int iColor,int iBrightness, int iContrast)
{
	m_iContrast = iContrast + RANGE;
	m_iBrightness = iBrightness + RANGE;
	m_iColor = iColor + RANGE;
	m_Contrast.SetPos(m_iContrast);
	m_Brightness.SetPos(m_iBrightness);
	m_Color.SetPos(m_iColor);
}
/////////////////////////////////////////////////////////////////////////////
// CCSBDialog message handlers
void CCSBDialog::PostNcDestroy() 
{
	delete this;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCSBDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_Contrast.SetRange(0,2*RANGE);
	m_Brightness.SetRange(0,2*RANGE);
	m_Color.SetRange(0,2*RANGE);
		
	m_Contrast.SetPos(m_iContrast);
	m_Brightness.SetPos(m_iBrightness);
	m_Color.SetPos(m_iColor);

	CString sText;
	CRect vrect;
	CRect srect;
	CRect drect;
	int x,y,cx,cy;
	int l,r,t,b;

	m_pDisplayWindow->GetWindowRect(&vrect);
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
	m_sTxtCamServer = m_pDisplayWindow->GetServerName() + _T(" : ") + m_pDisplayWindow->GetName();
	UpdateData(FALSE);


	// actualize our controls by requesting values from Coco/Mico
	// every 5 sec
	if (m_pDisplayWindow->GetServer()->IsB3())
	{
		SetTimer(2,10000,NULL);
	}
	else
	{
		SetTimer(2,5000,NULL);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CCSBDialog::OnCancel() 
{
	DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CCSBDialog::OnOK() 
{
	DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CCSBDialog::OnDestroy() 
{
	CDialog::OnDestroy();
	
	KillTimer(2);
	m_pDisplayWindow->m_pCSBDialog = NULL;
}
/////////////////////////////////////////////////////////////////////////////
void CCSBDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	if (pScrollBar->m_hWnd == m_Contrast.m_hWnd)
	{
		m_pDisplayWindow->RequestSetContrast(m_Contrast.GetPos()-RANGE);
	}
	else if (pScrollBar->m_hWnd == m_Brightness.m_hWnd)
	{
		m_pDisplayWindow->RequestSetBrightness(m_Brightness.GetPos()-RANGE);
	}
	else if (pScrollBar->m_hWnd == m_Color.m_hWnd)
	{
		m_pDisplayWindow->RequestSetColor(m_Color.GetPos()-RANGE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCSBDialog::OnTimer(UINT nIDEvent) 
{
	m_pDisplayWindow->RequestCSBUpdate();
}
