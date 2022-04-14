// DlgPTZConfig.cpp : implementation file
//

#include "stdafx.h"
#include "IdipClient.h"
#include "CIPCInputIdipClient.h"
#include "CIPCOutputIdipClient.h"

#include "WndLive.h"
#include "DlgPTZConfig.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgPTZConfig dialog
#define IDC_BTN_SET3 3000
#define IDC_BTN_SET4 3001

CDlgPTZConfig::CDlgPTZConfig(CIPCInputIdipClient* pInput,CIPCOutputIdipClient* pOutput,
								CameraControlType ccType, DWORD dwPTZFunctions,	DWORD dwPTZFunctionsEx,
								CWnd* pParent /*=NULL*/)
	: CSkinDialog(CDlgPTZConfig::IDD, pParent)
{
	m_pWndLive = (CWndLive*)pParent;
	m_pInput = pInput;
	m_pOutput = pOutput;
	m_ccType = ccType;
	m_dwPTZFunctions = dwPTZFunctions;
	m_dwPTZFunctionsEx = dwPTZFunctionsEx;
	m_commID = pInput->GetCommID();
	m_camID = m_pWndLive->GetCamID();
	//{{AFX_DATA_INIT(CDlgPTZConfig)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_nInitToolTips = FALSE;
	if (UseOEMSkin())
	{
		CreateTransparent(StyleBackGroundColorChangeSimple, m_BaseColor);
		MoveOnClientAreaClick(true);
	}
	Create(IDD,pParent);
	ShowWindow(SW_SHOW);
}
/////////////////////////////////////////////////////////////////////////////
CDlgPTZConfig::~CDlgPTZConfig()
{
}


void CDlgPTZConfig::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgPTZConfig)
	DDX_Control(pDX, IDC_UP, m_btnUp);
	DDX_Control(pDX, IDC_RIGHT, m_btnRight);
	DDX_Control(pDX, IDC_LEFT, m_btnLeft);
	DDX_Control(pDX, IDC_DOWN, m_btnDown);
	DDX_Control(pDX, IDC_CONFIG_CENTER, m_btnCenter);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgPTZConfig, CSkinDialog)
	//{{AFX_MSG_MAP(CDlgPTZConfig)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CONFIG_CENTER, OnCenter)
	ON_BN_CLICKED(IDC_DOWN, OnDown)
	ON_BN_CLICKED(IDC_LEFT, OnLeft)
	ON_BN_CLICKED(IDC_RIGHT, OnRight)
	ON_BN_CLICKED(IDC_UP, OnUp)
	ON_BN_CLICKED(IDC_PROG, OnProg)
	ON_BN_CLICKED(IDC_BTN_SET3, OnSet3)
	ON_BN_CLICKED(IDC_BTN_SET4, OnSet4)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgPTZConfig message handlers

void CDlgPTZConfig::OnCancel() 
{
	DestroyWindow();
}

void CDlgPTZConfig::OnOK() 
{
	DestroyWindow();
}

BOOL CDlgPTZConfig::OnInitDialog() 
{
	CSkinDialog::OnInitDialog();
	AutoCreateSkinButtons();
	
	m_btnUp.SetImageList(GetUpButtonImages());
	m_btnDown.SetImageList(GetDownButtonImages());
	m_btnLeft.SetImageList(GetLeftButtonImages());
	m_btnRight.SetImageList(GetRightButtonImages());
	CImageList *pImageList = new CImageList;
	if (pImageList->Create(IDB_CENTER, 12, 0, SKIN_COLOR_KEY))
	{
		m_btnCenter.SetImageList(pImageList, true);
	}
	else
	{
		ASSERT(FALSE);
	}

	if (m_dwPTZFunctionsEx & PTZF_EX_CONFIG_SET_3_4)
	{
		CRect rc;
		DWORD dwStyle = GetWindowLong(::GetDlgItem(m_hWnd, IDC_CONFIG_CENTER), GWL_STYLE);
		GetDlgItem(IDC_CONFIG_CENTER)->GetWindowRect(&rc);
		ScreenToClient(&rc);
		rc.OffsetRect(rc.Width() + 10, 0);
		rc.right = rc.left + rc.Height();
		CSkinButton *pBtn = new CSkinButton();
		m_AutoCtrlList.AddTail(pBtn);
		pBtn->Create(_T(""), dwStyle, rc, this, IDC_BTN_SET3);
		pBtn->SetImageList(GetUpButtonImages());

		GetDlgItem(IDC_PROG)->GetWindowRect(&rc);
		ScreenToClient(&rc);
		rc.OffsetRect(rc.Width() + 10, 0);
		rc.right = rc.left + rc.Height();
		pBtn = new CSkinButton();
		m_AutoCtrlList.AddTail(pBtn);
		pBtn->Create(_T(""), dwStyle, rc, this, IDC_BTN_SET4);
		pBtn->SetImageList(GetDownButtonImages());
	}

	CString wText,dText;

	CRect vrect;
	CRect srect;
	CRect drect;
	int x,y,cx,cy;
	int l,r,t,b;

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
		// doesn't fit searching for best missfit
		x = vrect.Width()/2;
		y = vrect.Height()/2;
	}

	SetWindowPos(NULL,x,y,0,0,SWP_NOSIZE|SWP_NOZORDER);
	
	GetWindowText(dText);
	dText += ' ';
	dText += m_pWndLive->GetName();
	SetWindowText(dText);

	m_pInput->DoRequestCameraControl(m_commID, m_camID, CCC_CONFIG, PTZF_CONFIG_PANASONIC_ON);	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgPTZConfig::PostNcDestroy() 
{
	delete this;
}

void CDlgPTZConfig::OnDestroy() 
{
	m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_CONFIG,PTZF_CONFIG_PANASONIC_OFF);	

	CSkinDialog::OnDestroy();
	
	m_pWndLive->m_pDlgPTZConfig = NULL;
}

void CDlgPTZConfig::OnCenter() 
{
	m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_CONFIG,PTZF_CONFIG_PANASONIC_SET1);	
}

void CDlgPTZConfig::OnDown() 
{
	m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_CONFIG,PTZF_CONFIG_PANASONIC_DOWN);	
}

void CDlgPTZConfig::OnLeft() 
{
	m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_CONFIG,PTZF_CONFIG_PANASONIC_LEFT);	
}

void CDlgPTZConfig::OnRight() 
{
	m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_CONFIG,PTZF_CONFIG_PANASONIC_RIGHT);	
}

void CDlgPTZConfig::OnUp() 
{
	m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_CONFIG,PTZF_CONFIG_PANASONIC_UP);	
}

void CDlgPTZConfig::OnProg() 
{
	m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_CONFIG,PTZF_CONFIG_PANASONIC_SET2);	
}

void CDlgPTZConfig::OnSet3() 
{
	m_pInput->DoRequestCameraControl(m_commID,m_camID, CCC_CONFIG, PTZF_CONFIG_SET3);	
}

void CDlgPTZConfig::OnSet4() 
{
	m_pInput->DoRequestCameraControl(m_commID,m_camID, CCC_CONFIG, PTZF_CONFIG_SET4);	
}
