// SNConfigDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Vision.h"
#include "CIPCInputVision.h"
#include "CIPCOutputVision.h"

#include "DisplayWindow.h"
#include "SNConfigDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSNConfigDialog dialog


CSNConfigDialog::CSNConfigDialog(CIPCInputVision* pInput,CIPCOutputVision* pOutput,
								CameraControlType ccType,DWORD dwPTZFunctions,
								CWnd* pParent /*=NULL*/)
	: CDialog(CSNConfigDialog::IDD, pParent)
{
	// GF todo m_SmallWindows.Lock; CAVEAT Can be deleted in the meantime!!!
	m_pDisplayWindow = (CDisplayWindow*)pParent;
	m_pInput = pInput;
	m_pOutput = pOutput;
	m_ccType = ccType;
	m_dwPTZFunctions = dwPTZFunctions;
	m_commID = pInput->GetCommID();
	m_camID = m_pDisplayWindow->GetCamID();
	//{{AFX_DATA_INIT(CSNConfigDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	Create(IDD,pParent);
	ShowWindow(SW_SHOW);
}
/////////////////////////////////////////////////////////////////////////////
CSNConfigDialog::~CSNConfigDialog()
{
}


void CSNConfigDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSNConfigDialog)
	DDX_Control(pDX, IDC_UP, m_btnUp);
	DDX_Control(pDX, IDC_RIGHT, m_btnRight);
	DDX_Control(pDX, IDC_LEFT, m_btnLeft);
	DDX_Control(pDX, IDC_DOWN, m_btnDown);
	DDX_Control(pDX, IDC_CONFIG_CENTER, m_btnCenter);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSNConfigDialog, CDialog)
	//{{AFX_MSG_MAP(CSNConfigDialog)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CONFIG_CENTER, OnCenter)
	ON_BN_CLICKED(IDC_DOWN, OnDown)
	ON_BN_CLICKED(IDC_LEFT, OnLeft)
	ON_BN_CLICKED(IDC_RIGHT, OnRight)
	ON_BN_CLICKED(IDC_UP, OnUp)
	ON_BN_CLICKED(IDC_PROG, OnProg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSNConfigDialog message handlers

void CSNConfigDialog::OnCancel() 
{
	DestroyWindow();
}

void CSNConfigDialog::OnOK() 
{
	DestroyWindow();
}

BOOL CSNConfigDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_btnUp.SetIcon(    (HICON)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_UP),     IMAGE_ICON, 16,16, LR_DEFAULTCOLOR));
	m_btnDown.SetIcon(  (HICON)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_DOWN),   IMAGE_ICON, 16,16, LR_DEFAULTCOLOR));
	m_btnLeft.SetIcon(  (HICON)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_LEFT),   IMAGE_ICON, 16,16, LR_DEFAULTCOLOR));
	m_btnRight.SetIcon( (HICON)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_RIGHT),  IMAGE_ICON, 16,16, LR_DEFAULTCOLOR));
	m_btnCenter.SetIcon((HICON)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_CENTER), IMAGE_ICON, 16,16, LR_DEFAULTCOLOR));

	CString wText,dText;
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
		// doesn't fit searching for best missfit
		x = vrect.Width()/2;
		y = vrect.Height()/2;
	}

	SetWindowPos(NULL,x,y,0,0,SWP_NOSIZE|SWP_NOZORDER);
	
	GetWindowText(dText);
	dText += ' ';
	dText += m_pDisplayWindow->GetName();
	SetWindowText(dText);

	m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_CONFIG,PTZF_CONFIG_PANASONIC_ON);	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSNConfigDialog::PostNcDestroy() 
{
	delete this;
}

void CSNConfigDialog::OnDestroy() 
{
	m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_CONFIG,PTZF_CONFIG_PANASONIC_OFF);	

	CDialog::OnDestroy();
	
	m_pDisplayWindow->m_pSNConfigDialog = NULL;
}

void CSNConfigDialog::OnCenter() 
{
	m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_CONFIG,PTZF_CONFIG_PANASONIC_SET1);	
}

void CSNConfigDialog::OnDown() 
{
	m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_CONFIG,PTZF_CONFIG_PANASONIC_DOWN);	
}

void CSNConfigDialog::OnLeft() 
{
	m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_CONFIG,PTZF_CONFIG_PANASONIC_LEFT);	
}

void CSNConfigDialog::OnRight() 
{
	m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_CONFIG,PTZF_CONFIG_PANASONIC_RIGHT);	
}

void CSNConfigDialog::OnUp() 
{
	m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_CONFIG,PTZF_CONFIG_PANASONIC_UP);	
}

void CSNConfigDialog::OnProg() 
{
	m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_CONFIG,PTZF_CONFIG_PANASONIC_SET2);	
}
