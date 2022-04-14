// VideoSNDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Vision.h"
#include "CIPCInputVision.h"
#include "CIPCOutputVision.h"

#include "DisplayWindow.h"
#include "VideoSNDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUpDownButton
CUpDownButton::CUpDownButton()
{}
/////////////////////////////////////////////////////////////////////////////
CUpDownButton::~CUpDownButton()
{}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CUpDownButton, CButton)
	//{{AFX_MSG_MAP(CUpDownButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CUpDownButton message handlers
void CUpDownButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	WPARAM wParam = MAKELONG((WORD)GetDlgCtrlID(),(WORD)BN_DOWN); // low, high
	GetParent()->PostMessage(WM_COMMAND,wParam,(LPARAM)m_hWnd);
	CButton::OnLButtonDown(nFlags, point);
}
/////////////////////////////////////////////////////////////////////////////
void CUpDownButton::OnLButtonUp(UINT nFlags, CPoint point) 
{
	WPARAM wParam = MAKELONG((WORD)GetDlgCtrlID(),(WORD)BN_UP); // low, high
	GetParent()->PostMessage(WM_COMMAND,wParam,(LPARAM)m_hWnd);
	CButton::OnLButtonUp(nFlags, point);
}
/////////////////////////////////////////////////////////////////////////////
//***************************************************************************
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CVideoSNDialog, CDialog)
// CVideoSNDialog dialog
CVideoSNDialog::CVideoSNDialog(	CIPCInputVision* pInput,CIPCOutputVision* pOutput,
								CameraControlType ccType,DWORD dwPTZFunctions,
								CWnd* pParent /*=NULL*/)
	: CWK_Dialog(CVideoSNDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVideoSNDialog)
	m_bFocusAuto = FALSE;
	m_bIrisAuto = FALSE;
	//}}AFX_DATA_INIT
	// GF todo m_SmallWindows.Lock; CAVEAT Can be deleted in the meantime!!!
	m_pDisplayWindow = (CDisplayWindow*)pParent;
	m_pInput = pInput;
	m_pOutput = pOutput;
	m_ccType = ccType;
	m_dwPTZFunctions = dwPTZFunctions;
	m_commID = pInput->GetCommID();
	m_camID = m_pDisplayWindow->GetCamID();
	m_bPreset = FALSE;

	Create(IDD,pParent);
	ShowWindow(SW_SHOW);
}
/////////////////////////////////////////////////////////////////////////////
CVideoSNDialog::~CVideoSNDialog()
{

}
/////////////////////////////////////////////////////////////////////////////
void CVideoSNDialog::DoDataExchange(CDataExchange* pDX)
{
	CWK_Dialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVideoSNDialog)
	DDX_Control(pDX, IDC_TURN_180, m_Turn180Button);
	DDX_Control(pDX, IDC_IRIS_AUTO, m_IrisAutoButton);
	DDX_Control(pDX, IDC_FOCUS_AUTO, m_FocusAutoButton);
	DDX_Control(pDX, IDC_SPEED, m_SpeedCtrl);
	DDX_Control(pDX, IDC_PRESET, m_PresetButton);
	DDX_Control(pDX, IDC_POS_9, m_Pos9Button);
	DDX_Control(pDX, IDC_POS_8, m_Pos8Button);
	DDX_Control(pDX, IDC_POS_7, m_Pos7Button);
	DDX_Control(pDX, IDC_POS_6, m_Pos6Button);
	DDX_Control(pDX, IDC_POS_5, m_Pos5Button);
	DDX_Control(pDX, IDC_POS_4, m_Pos4Button);
	DDX_Control(pDX, IDC_POS_3, m_Pos3Button);
	DDX_Control(pDX, IDC_POS_2, m_Pos2Button);
	DDX_Control(pDX, IDC_POS_1, m_Pos1Button);
	DDX_Control(pDX, IDC_CENTER, m_HomeButton);
	DDX_Control(pDX, IDC_IRISOUT, m_IrisOutButton);
	DDX_Control(pDX, IDC_IRISIN, m_IrisInButton);
	DDX_Control(pDX, IDC_ZOOMOUT, m_ZoomOutButton);
	DDX_Control(pDX, IDC_ZOOMIN, m_ZoomInButton);
	DDX_Control(pDX, IDC_FOCUSOUT, m_FocusOutButton);
	DDX_Control(pDX, IDC_FOCUSIN, m_FocusInButton);
	DDX_Control(pDX, IDC_UP, m_UpButton);
	DDX_Control(pDX, IDC_RIGHT, m_RightButton);
	DDX_Control(pDX, IDC_LEFT, m_LeftButton);
	DDX_Control(pDX, IDC_DOWN, m_DownButton);
	DDX_Check(pDX, IDC_FOCUS_AUTO, m_bFocusAuto);
	DDX_Check(pDX, IDC_IRIS_AUTO, m_bIrisAuto);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CVideoSNDialog, CWK_Dialog)
	//{{AFX_MSG_MAP(CVideoSNDialog)
	ON_WM_DESTROY()
	ON_BN_UP(IDC_UP, OnUpUp)
	ON_BN_DOWN(IDC_UP, OnUpDown)
	ON_BN_UP(IDC_DOWN, OnDownUp)
	ON_BN_DOWN(IDC_DOWN, OnDownDown)
	ON_BN_UP(IDC_LEFT, OnLeftUp)
	ON_BN_DOWN(IDC_LEFT, OnLeftDown)
	ON_BN_UP(IDC_RIGHT, OnRightUp)
	ON_BN_DOWN(IDC_RIGHT, OnRightDown)
	ON_BN_UP(IDC_ZOOMIN, OnZoominUp)
	ON_BN_DOWN(IDC_ZOOMIN, OnZoominDown)
	ON_BN_UP(IDC_ZOOMOUT, OnZoomoutUp)
	ON_BN_DOWN(IDC_ZOOMOUT, OnZoomoutDown)
	ON_BN_UP(IDC_FOCUSIN, OnFocusinUp)
	ON_BN_DOWN(IDC_FOCUSIN, OnFocusinDown)
	ON_BN_UP(IDC_FOCUSOUT, OnFocusoutUp)
	ON_BN_DOWN(IDC_FOCUSOUT, OnFocusoutDown)
	ON_BN_UP(IDC_IRISIN, OnIrisinUp)
	ON_BN_DOWN(IDC_IRISIN, OnIrisinDown)
	ON_BN_UP(IDC_IRISOUT, OnIrisoutUp)
	ON_BN_DOWN(IDC_IRISOUT, OnIrisoutDown)
	ON_BN_CLICKED(IDC_CENTER, OnCenter)
	ON_BN_CLICKED(IDC_POS_1, OnPos1)
	ON_BN_CLICKED(IDC_POS_2, OnPos2)
	ON_BN_CLICKED(IDC_POS_3, OnPos3)
	ON_BN_CLICKED(IDC_POS_4, OnPos4)
	ON_BN_CLICKED(IDC_POS_5, OnPos5)
	ON_BN_CLICKED(IDC_POS_6, OnPos6)
	ON_BN_CLICKED(IDC_POS_7, OnPos7)
	ON_BN_CLICKED(IDC_POS_8, OnPos8)
	ON_BN_CLICKED(IDC_POS_9, OnPos9)
	ON_BN_CLICKED(IDC_PRESET, OnPreset)
	ON_BN_CLICKED(IDC_FOCUS_AUTO, OnFocusAuto)
	ON_BN_CLICKED(IDC_IRIS_AUTO, OnIrisAuto)
	ON_BN_CLICKED(IDC_TURN_180, OnTurn180)
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CVideoSNDialog message handlers
BOOL CVideoSNDialog::OnInitDialog() 
{
	CWK_Dialog::OnInitDialog();
	
	m_UpButton.SetIcon(   (HICON)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_UP),     IMAGE_ICON, 16,16, LR_DEFAULTCOLOR));
	m_DownButton.SetIcon( (HICON)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_DOWN),   IMAGE_ICON, 16,16, LR_DEFAULTCOLOR));
	m_LeftButton.SetIcon( (HICON)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_LEFT),   IMAGE_ICON, 16,16, LR_DEFAULTCOLOR));
	m_RightButton.SetIcon((HICON)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_RIGHT),  IMAGE_ICON, 16,16, LR_DEFAULTCOLOR));
	m_HomeButton.SetIcon( (HICON)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_CENTER), IMAGE_ICON, 16,16, LR_DEFAULTCOLOR));
	m_Turn180Button.SetIcon( (HICON)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_TURN180), IMAGE_ICON, 16,16, LR_DEFAULTCOLOR));
	
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

	SetPTZFunctions(m_dwPTZFunctions);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CVideoSNDialog::SetPTZType(CameraControlType ccType)
{
	m_ccType = ccType;
}
/////////////////////////////////////////////////////////////////////////////
void CVideoSNDialog::SetPTZFunctions(DWORD dwFunctions)
{
	m_dwPTZFunctions = dwFunctions;
	if (m_dwPTZFunctions & PTZF_SPEED)
	{
		m_dwSpeed = m_pDisplayWindow->m_dwPTZSpeed;
		m_SpeedCtrl.SetRange(1,10);
		m_SpeedCtrl.SetPos(m_dwSpeed);
		m_SpeedCtrl.ShowWindow(SW_SHOW);
	}
	else
	{
		m_SpeedCtrl.ShowWindow(SW_HIDE);
		m_dwSpeed = 0L;
	}

	m_HomeButton.ShowWindow((dwFunctions & PTZF_POS_HOME) ? SW_SHOW : SW_HIDE);
	m_Pos1Button.ShowWindow((dwFunctions & PTZF_POS_1) ? SW_SHOW : SW_HIDE);
	m_Pos2Button.ShowWindow((dwFunctions & PTZF_POS_2) ? SW_SHOW : SW_HIDE);
	m_Pos3Button.ShowWindow((dwFunctions & PTZF_POS_3) ? SW_SHOW : SW_HIDE);
	m_Pos4Button.ShowWindow((dwFunctions & PTZF_POS_4) ? SW_SHOW : SW_HIDE);
	m_Pos5Button.ShowWindow((dwFunctions & PTZF_POS_5) ? SW_SHOW : SW_HIDE);
	m_Pos6Button.ShowWindow((dwFunctions & PTZF_POS_6) ? SW_SHOW : SW_HIDE);
	m_Pos7Button.ShowWindow((dwFunctions & PTZF_POS_7) ? SW_SHOW : SW_HIDE);
	m_Pos8Button.ShowWindow((dwFunctions & PTZF_POS_8) ? SW_SHOW : SW_HIDE);
	m_Pos9Button.ShowWindow((dwFunctions & PTZF_POS_9) ? SW_SHOW : SW_HIDE);

	if (dwFunctions & PTZF_SET_POS)
	{
		m_PresetButton.ShowWindow(SW_SHOW);
	}

	EnablePTZControls(m_dwPTZFunctions);
}
/////////////////////////////////////////////////////////////////////////////
void CVideoSNDialog::EnablePTZControls(DWORD dwFunctions)
{
	m_RightButton.EnableWindow(dwFunctions & PTZF_PAN_RIGHT);
	m_LeftButton.EnableWindow(dwFunctions & PTZF_PAN_LEFT);

	m_UpButton.EnableWindow(dwFunctions & PTZF_TILT_UP);
	m_DownButton.EnableWindow(dwFunctions & PTZF_TILT_DOWN);

	m_SpeedCtrl.EnableWindow(dwFunctions & PTZF_SPEED);

	m_FocusOutButton.EnableWindow(dwFunctions & PTZF_FOCUS_FAR);
	m_FocusInButton.EnableWindow(dwFunctions & PTZF_FOCUS_NEAR);
	m_FocusAutoButton.EnableWindow(dwFunctions & PTZF_FOCUS_AUTO);

	m_IrisOutButton.EnableWindow(dwFunctions & PTZF_IRIS_OPEN);
	m_IrisInButton.EnableWindow(dwFunctions & PTZF_IRIS_OPEN);
	m_IrisAutoButton.EnableWindow(dwFunctions & PTZF_IRIS_AUTO);

	m_ZoomOutButton.EnableWindow(dwFunctions & PTZF_ZOOM_OUT);
	m_ZoomInButton.EnableWindow(dwFunctions & PTZF_ZOOM_IN);

	m_Turn180Button.EnableWindow(dwFunctions & PTZF_TURN_180);

	m_HomeButton.EnableWindow(dwFunctions & PTZF_POS_HOME);
	m_Pos1Button.EnableWindow(dwFunctions & PTZF_POS_1);
	m_Pos2Button.EnableWindow(dwFunctions & PTZF_POS_2);
	m_Pos3Button.EnableWindow(dwFunctions & PTZF_POS_3);
	m_Pos4Button.EnableWindow(dwFunctions & PTZF_POS_4);
	m_Pos5Button.EnableWindow(dwFunctions & PTZF_POS_5);
	m_Pos6Button.EnableWindow(dwFunctions & PTZF_POS_6);
	m_Pos7Button.EnableWindow(dwFunctions & PTZF_POS_7);
	m_Pos8Button.EnableWindow(dwFunctions & PTZF_POS_8);
	m_Pos9Button.EnableWindow(dwFunctions & PTZF_POS_9);

	m_PresetButton.EnableWindow(dwFunctions & PTZF_SET_POS);
}
/////////////////////////////////////////////////////////////////////////////
void CVideoSNDialog::PostNcDestroy() 
{
	delete this;
}
/////////////////////////////////////////////////////////////////////////////
void CVideoSNDialog::OnDestroy() 
{
	m_pDisplayWindow->m_pVideoSNDialog = NULL;
	CWK_Dialog::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
void CVideoSNDialog::OnCancel() 
{
	m_pDisplayWindow->m_dwPTZSpeed = m_SpeedCtrl.GetPos();
	DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CVideoSNDialog::OnOK() 
{
	m_pDisplayWindow->m_dwPTZSpeed = m_SpeedCtrl.GetPos();
	DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CVideoSNDialog::OnUpUp() 
{
	if (m_ccType==CCT_RELAY_PTZ)
	{
		m_pOutput->RequestCameraControl(CCC_TILT_UP,FALSE);
	}
	else if (m_ccType==CCT_DIRECT_SHOW)
	{
		CString sValue;
		sValue.Format(_T("%d"), (int)CCC_TILT_STOP);
		m_pOutput->DoRequestSetValue(m_camID, CSD_CAMERA_CONTROL, sValue);
	}
	else
	{
		m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_TILT_STOP,0L);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVideoSNDialog::OnUpDown() 
{
	if (m_ccType==CCT_RELAY_PTZ)
	{
		m_pOutput->RequestCameraControl(CCC_TILT_UP,TRUE);
	}
	else if (m_ccType==CCT_DIRECT_SHOW)
	{
		CString sValue;
		sValue.Format(_T("%d:%d"), (int)CCC_TILT_UP, m_dwSpeed);
		m_pOutput->DoRequestSetValue(m_camID, CSD_CAMERA_CONTROL, sValue);
	}
	else
	{
		m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_TILT_UP,m_dwSpeed);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVideoSNDialog::OnDownUp() 
{
	if (m_ccType==CCT_RELAY_PTZ)
	{
		m_pOutput->RequestCameraControl(CCC_TILT_DOWN,FALSE);
	}
	else if (m_ccType==CCT_DIRECT_SHOW)
	{
		CString sValue;
		sValue.Format(_T("%d"), (int)CCC_TILT_STOP);
		m_pOutput->DoRequestSetValue(m_camID, CSD_CAMERA_CONTROL, sValue);
	}
	else
	{
		m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_TILT_STOP,0L);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVideoSNDialog::OnDownDown() 
{
	if (m_ccType==CCT_RELAY_PTZ)
	{
		m_pOutput->RequestCameraControl(CCC_TILT_DOWN,TRUE);
	}
	else if (m_ccType==CCT_DIRECT_SHOW)
	{
		CString sValue;
		sValue.Format(_T("%d:%d"), (int)CCC_TILT_DOWN, m_dwSpeed);
		m_pOutput->DoRequestSetValue(m_camID, CSD_CAMERA_CONTROL, sValue);
	}
	else
	{
		m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_TILT_DOWN,m_dwSpeed);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVideoSNDialog::OnLeftUp() 
{
	if (m_ccType==CCT_RELAY_PTZ)
	{
		m_pOutput->RequestCameraControl(CCC_PAN_LEFT,FALSE);
	}
	else if (m_ccType==CCT_DIRECT_SHOW)
	{
		CString sValue;
		sValue.Format(_T("%d"), (int)CCC_PAN_STOP);
		m_pOutput->DoRequestSetValue(m_camID, CSD_CAMERA_CONTROL, sValue);
	}
	else
	{
		m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_PAN_STOP,0L);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVideoSNDialog::OnLeftDown() 
{
	if (m_ccType==CCT_RELAY_PTZ)
	{
		m_pOutput->RequestCameraControl(CCC_PAN_LEFT,TRUE);
	}
	else if (m_ccType==CCT_DIRECT_SHOW)
	{
		CString sValue;
		sValue.Format(_T("%d:%d"), (int)CCC_PAN_LEFT, m_dwSpeed);
		m_pOutput->DoRequestSetValue(m_camID, CSD_CAMERA_CONTROL, sValue);
	}
	else
	{
		m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_PAN_LEFT,m_dwSpeed);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVideoSNDialog::OnRightUp() 
{
	if (m_ccType==CCT_RELAY_PTZ)
	{
		m_pOutput->RequestCameraControl(CCC_PAN_RIGHT,FALSE);
	}
	else if (m_ccType==CCT_DIRECT_SHOW)
	{
		CString sValue;
		sValue.Format(_T("%d"), (int)CCC_PAN_STOP);
		m_pOutput->DoRequestSetValue(m_camID, CSD_CAMERA_CONTROL, sValue);
	}
	else
	{
		m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_PAN_STOP,0L);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVideoSNDialog::OnRightDown() 
{
	if (m_ccType==CCT_RELAY_PTZ)
	{
		m_pOutput->RequestCameraControl(CCC_PAN_RIGHT,TRUE);
	}
	else if (m_ccType==CCT_DIRECT_SHOW)
	{
		CString sValue;
		sValue.Format(_T("%d:%d"), (int)CCC_PAN_RIGHT, m_dwSpeed);
		m_pOutput->DoRequestSetValue(m_camID, CSD_CAMERA_CONTROL, sValue);
	}
	else
	{
		m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_PAN_RIGHT,m_dwSpeed);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVideoSNDialog::OnZoominUp() 
{
	if (m_ccType==CCT_RELAY_PTZ)
	{
		m_pOutput->RequestCameraControl(CCC_ZOOM_IN,FALSE);
	}
	else if (m_ccType==CCT_DIRECT_SHOW)
	{
		CString sValue;
		sValue.Format(_T("%d"), (int)CCC_ZOOM_STOP);
		m_pOutput->DoRequestSetValue(m_camID, CSD_CAMERA_CONTROL, sValue);
	}
	else
	{
		m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_ZOOM_STOP,0L);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVideoSNDialog::OnZoominDown() 
{
	if (m_ccType==CCT_RELAY_PTZ)
	{
		m_pOutput->RequestCameraControl(CCC_ZOOM_IN,TRUE);
	}
	else if (m_ccType==CCT_DIRECT_SHOW)
	{
		CString sValue;
		sValue.Format(_T("%d:%d"), (int)CCC_ZOOM_IN, m_dwSpeed);
		m_pOutput->DoRequestSetValue(m_camID, CSD_CAMERA_CONTROL, sValue);
	}
	else
	{
		m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_ZOOM_IN,m_dwSpeed);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVideoSNDialog::OnZoomoutUp() 
{
	if (m_ccType==CCT_RELAY_PTZ)
	{
		m_pOutput->RequestCameraControl(CCC_ZOOM_OUT,FALSE);
	}
	else if (m_ccType==CCT_DIRECT_SHOW)
	{
		CString sValue;
		sValue.Format(_T("%d"), (int)CCC_ZOOM_STOP);
		m_pOutput->DoRequestSetValue(m_camID, CSD_CAMERA_CONTROL, sValue);
	}
	else
	{
		m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_ZOOM_STOP,0L);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVideoSNDialog::OnZoomoutDown() 
{
	if (m_ccType==CCT_RELAY_PTZ)
	{
		m_pOutput->RequestCameraControl(CCC_ZOOM_OUT,TRUE);
	}
	else if (m_ccType==CCT_DIRECT_SHOW)
	{
		CString sValue;
		sValue.Format(_T("%d:%d"), (int)CCC_ZOOM_OUT, m_dwSpeed);
		m_pOutput->DoRequestSetValue(m_camID, CSD_CAMERA_CONTROL, sValue);
	}
	else
	{
		m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_ZOOM_OUT,m_dwSpeed);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVideoSNDialog::OnFocusinUp() 
{
	if (m_ccType==CCT_RELAY_PTZ)
	{
		m_pOutput->RequestCameraControl(CCC_FOCUS_NEAR,FALSE);
	}
	else if (m_ccType==CCT_DIRECT_SHOW)
	{
		CString sValue;
		sValue.Format(_T("%d"), (int)CCC_FOCUS_STOP);
		m_pOutput->DoRequestSetValue(m_camID, CSD_CAMERA_CONTROL, sValue);
	}
	else
	{
		m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_FOCUS_STOP,0L);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVideoSNDialog::OnFocusinDown() 
{
	if (m_ccType==CCT_RELAY_PTZ)
	{
		m_pOutput->RequestCameraControl(CCC_FOCUS_NEAR,TRUE);
	}
	else if (m_ccType==CCT_DIRECT_SHOW)
	{
		CString sValue;
		sValue.Format(_T("%d:%d"), (int)CCC_FOCUS_NEAR, m_dwSpeed);
		m_pOutput->DoRequestSetValue(m_camID, CSD_CAMERA_CONTROL, sValue);
	}
	else
	{
		m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_FOCUS_NEAR,m_dwSpeed);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVideoSNDialog::OnFocusoutUp() 
{
	if (m_ccType==CCT_RELAY_PTZ)
	{
		m_pOutput->RequestCameraControl(CCC_FOCUS_FAR,FALSE);
	}
	else if (m_ccType==CCT_DIRECT_SHOW)
	{
		CString sValue;
		sValue.Format(_T("%d"), (int)CCC_FOCUS_STOP);
		m_pOutput->DoRequestSetValue(m_camID, CSD_CAMERA_CONTROL, sValue);
	}
	else
	{
		m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_FOCUS_STOP,0L);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVideoSNDialog::OnFocusoutDown() 
{
	if (m_ccType==CCT_RELAY_PTZ)
	{
		m_pOutput->RequestCameraControl(CCC_FOCUS_FAR,TRUE);
	}
	else if (m_ccType==CCT_DIRECT_SHOW)
	{
		CString sValue;
		sValue.Format(_T("%d:%d"), (int)CCC_FOCUS_FAR, m_dwSpeed);
		m_pOutput->DoRequestSetValue(m_camID, CSD_CAMERA_CONTROL, sValue);
	}
	else
	{
		m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_FOCUS_FAR,m_dwSpeed);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVideoSNDialog::OnIrisinUp() 
{
	if (m_ccType==CCT_RELAY_PTZ)
	{
		m_pOutput->RequestCameraControl(CCC_IRIS_OPEN,FALSE);
	}
	else if (m_ccType==CCT_DIRECT_SHOW)
	{
		CString sValue;
		sValue.Format(_T("%d"), (int)CCC_IRIS_STOP);
		m_pOutput->DoRequestSetValue(m_camID, CSD_CAMERA_CONTROL, sValue);
	}
	else
	{
		m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_IRIS_STOP,0L);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVideoSNDialog::OnIrisinDown() 
{
	if (m_ccType==CCT_RELAY_PTZ)
	{
		m_pOutput->RequestCameraControl(CCC_IRIS_OPEN,TRUE);
	}
	else if (m_ccType==CCT_DIRECT_SHOW)
	{
		CString sValue;
		sValue.Format(_T("%d:%d"), (int)CCC_IRIS_OPEN, m_dwSpeed);
		m_pOutput->DoRequestSetValue(m_camID, CSD_CAMERA_CONTROL, sValue);
	}
	else
	{
		m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_IRIS_OPEN,m_dwSpeed);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVideoSNDialog::OnIrisoutUp() 
{
	if (m_ccType==CCT_RELAY_PTZ)
	{
		m_pOutput->RequestCameraControl(CCC_IRIS_CLOSE,FALSE);
	}
	else if (m_ccType==CCT_DIRECT_SHOW)
	{
		CString sValue;
		sValue.Format(_T("%d"), (int)CCC_IRIS_STOP);
		m_pOutput->DoRequestSetValue(m_camID, CSD_CAMERA_CONTROL, sValue);
	}
	else
	{
		m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_IRIS_STOP,0L);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVideoSNDialog::OnIrisoutDown() 
{
	if (m_ccType==CCT_RELAY_PTZ)
	{
		m_pOutput->RequestCameraControl(CCC_IRIS_CLOSE,TRUE);
	}
	else if (m_ccType==CCT_DIRECT_SHOW)
	{
		CString sValue;
		sValue.Format(_T("%d:%d"), (int)CCC_IRIS_CLOSE, m_dwSpeed);
		m_pOutput->DoRequestSetValue(m_camID, CSD_CAMERA_CONTROL, sValue);
	}
	else
	{
		m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_IRIS_CLOSE,m_dwSpeed);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVideoSNDialog::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch (nChar)
	{
	case VK_UP:
		m_UpButton.SetState(0);
		OnUpUp();
		return;
	case VK_DOWN:
		m_DownButton.SetState(0);
		OnDownUp();
		return;
	case VK_LEFT:
		m_LeftButton.SetState(0);
		OnLeftUp();
		return;
	case VK_RIGHT:
		m_RightButton.SetState(0);
		OnRightUp();
		return;
	case VK_INSERT:
		m_IrisInButton.SetState(0);
		OnIrisinUp();
		return;
	case VK_DELETE:
		m_IrisOutButton.SetState(0);
		OnIrisoutUp();
		return;
	case VK_HOME:
		m_FocusInButton.SetState(0);
		OnFocusinUp();
		return;
	case VK_END:
		m_FocusOutButton.SetState(0);
		OnFocusoutUp();
		return;
	case VK_PRIOR:
		m_ZoomInButton.SetState(0);
		OnZoominUp();
		return;
	case VK_NEXT:
		m_ZoomOutButton.SetState(0);
		OnZoomoutUp();
		return;
	case VK_NUMPAD0:
		m_HomeButton.SetState(0);
		return;
	case VK_NUMPAD1:
		m_Pos1Button.SetState(0);
		return;
	case VK_NUMPAD2:
		m_Pos2Button.SetState(0);
		return;
	case VK_NUMPAD3:
		m_Pos3Button.SetState(0);
		return;
	case VK_NUMPAD4:
		m_Pos4Button.SetState(0);
		return;
	case VK_NUMPAD5:
		m_Pos5Button.SetState(0);
		return;
	case VK_NUMPAD6:
		m_Pos6Button.SetState(0);
		return;
	case VK_NUMPAD7:
		m_Pos7Button.SetState(0);
		return;
	case VK_NUMPAD8:
		m_Pos8Button.SetState(0);
		return;
	case VK_NUMPAD9:
		m_Pos9Button.SetState(0);
		return;
	}
	
	CWK_Dialog::OnKeyUp(nChar, nRepCnt, nFlags);
}
/////////////////////////////////////////////////////////////////////////////
void CVideoSNDialog::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch (nChar)
	{
	case VK_UP:
		m_UpButton.SetState(1);
		m_UpButton.SetFocus();
		OnUpDown();
		return;
	case VK_DOWN:
		m_DownButton.SetState(1);
		m_DownButton.SetFocus();
		OnDownDown();
		return;
	case VK_LEFT:
		m_LeftButton.SetState(1);
		m_LeftButton.SetFocus();
		OnLeftDown();
		return;
	case VK_RIGHT:
		m_RightButton.SetState(1);
		m_RightButton.SetFocus();
		OnRightDown();
		return;
	case VK_INSERT:
		m_IrisInButton.SetState(1);
		m_IrisInButton.SetFocus();
		OnIrisinDown();
		return;
	case VK_DELETE:
		m_IrisOutButton.SetState(1);
		m_IrisOutButton.SetFocus();
		OnIrisoutDown();
		return;
	case VK_HOME:
		m_FocusInButton.SetState(1);
		m_FocusInButton.SetFocus();
		OnFocusinDown();
		return;
	case VK_END:
		m_FocusOutButton.SetState(1);
		m_FocusOutButton.SetFocus();
		OnFocusoutDown();
		return;
	case VK_PRIOR:
		m_ZoomInButton.SetState(1);
		m_ZoomInButton.SetFocus();
		OnZoominDown();
		return;
	case VK_NEXT:
		m_ZoomOutButton.SetState(1);
		m_ZoomOutButton.SetFocus();
		OnZoomoutDown();
		return;
	case VK_NUMPAD0:
		m_HomeButton.SetState(1);
		m_HomeButton.SetFocus();
		OnCenter();
		return;
	case VK_NUMPAD1:
		m_Pos1Button.SetState(1);
		m_Pos1Button.SetFocus();
		OnPos1();
		return;
	case VK_NUMPAD2:
		m_Pos2Button.SetState(1);
		m_Pos2Button.SetFocus();
		OnPos2();
		return;
	case VK_NUMPAD3:
		m_Pos3Button.SetState(1);
		m_Pos3Button.SetFocus();
		OnPos3();
		return;
	case VK_NUMPAD4:
		m_Pos4Button.SetState(1);
		m_Pos4Button.SetFocus();
		OnPos4();
		return;
	case VK_NUMPAD5:
		m_Pos5Button.SetState(1);
		m_Pos5Button.SetFocus();
		OnPos5();
		return;
	case VK_NUMPAD6:
		m_Pos6Button.SetState(1);
		m_Pos6Button.SetFocus();
		OnPos6();
		return;
	case VK_NUMPAD7:
		m_Pos7Button.SetState(1);
		m_Pos7Button.SetFocus();
		OnPos7();
		return;
	case VK_NUMPAD8:
		m_Pos8Button.SetState(1);
		m_Pos8Button.SetFocus();
		OnPos8();
		return;
	case VK_NUMPAD9:
		m_Pos9Button.SetState(1);
		m_Pos9Button.SetFocus();
		OnPos9();
		return;
	}
	
	CWK_Dialog::OnKeyDown(nChar, nRepCnt, nFlags);
}
/////////////////////////////////////////////////////////////////////////////
void CVideoSNDialog::OnCenter() 
{
	if (m_dwPTZFunctions & PTZF_POS_HOME)
	{
		if (m_ccType==CCT_DIRECT_SHOW)
		{
			CString sValue;
			sValue.Format(_T("%d:%d"), (int)CCC_POS_HOME, m_bPreset);
			m_pOutput->DoRequestSetValue(m_camID, CSD_CAMERA_CONTROL, sValue);
		}
		else if (m_ccType!=CCT_RELAY_PTZ)
		{
			m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_POS_HOME,(DWORD)m_bPreset);
		}
		if (m_bPreset)
		{
			TogglePresetMode();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVideoSNDialog::OnPos1() 
{
	if (m_dwPTZFunctions & PTZF_POS_1)
	{
		if (m_ccType==CCT_DIRECT_SHOW)
		{
			CString sValue;
			sValue.Format(_T("%d:%d"), (int)CCC_POS_1, m_bPreset);
			m_pOutput->DoRequestSetValue(m_camID, CSD_CAMERA_CONTROL, sValue);
		}
		else if (m_ccType!=CCT_RELAY_PTZ)
		{
			m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_POS_1,(DWORD)m_bPreset);
		}
		if (m_bPreset)
		{
			TogglePresetMode();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVideoSNDialog::OnPos2() 
{
	if (m_dwPTZFunctions & PTZF_POS_2)
	{
		if (m_ccType==CCT_DIRECT_SHOW)
		{
			CString sValue;
			sValue.Format(_T("%d:%d"), (int)CCC_POS_2, m_bPreset);
			m_pOutput->DoRequestSetValue(m_camID, CSD_CAMERA_CONTROL, sValue);
		}
		else if (m_ccType!=CCT_RELAY_PTZ)
		{
			m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_POS_2,(DWORD)m_bPreset);
		}
		if (m_bPreset)
		{
			TogglePresetMode();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVideoSNDialog::OnPos3() 
{
	if (m_dwPTZFunctions & PTZF_POS_3)
	{
		if (m_ccType==CCT_DIRECT_SHOW)
		{
			CString sValue;
			sValue.Format(_T("%d:%d"), (int)CCC_POS_3, m_bPreset);
			m_pOutput->DoRequestSetValue(m_camID, CSD_CAMERA_CONTROL, sValue);
		}
		else if (m_ccType!=CCT_RELAY_PTZ)
		{
			m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_POS_3,(DWORD)m_bPreset);
		}
		if (m_bPreset)
		{
			TogglePresetMode();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVideoSNDialog::OnPos4() 
{
	if (m_dwPTZFunctions & PTZF_POS_4)
	{
		if (m_ccType==CCT_DIRECT_SHOW)
		{
			CString sValue;
			sValue.Format(_T("%d:%d"), (int)CCC_POS_4, m_bPreset);
			m_pOutput->DoRequestSetValue(m_camID, CSD_CAMERA_CONTROL, sValue);
		}
		else if (m_ccType!=CCT_RELAY_PTZ)
		{
			m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_POS_4,(DWORD)m_bPreset);
		}
		if (m_bPreset)
		{
			TogglePresetMode();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVideoSNDialog::OnPos5() 
{
	if (m_dwPTZFunctions & PTZF_POS_5)
	{
		if (m_ccType==CCT_DIRECT_SHOW)
		{
			CString sValue;
			sValue.Format(_T("%d:%d"), (int)CCC_POS_5, m_bPreset);
			m_pOutput->DoRequestSetValue(m_camID, CSD_CAMERA_CONTROL, sValue);
		}
		else if (m_ccType!=CCT_RELAY_PTZ)
		{
			m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_POS_5,(DWORD)m_bPreset);
		}
		if (m_bPreset)
		{
			TogglePresetMode();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVideoSNDialog::OnPos6() 
{
	if (m_dwPTZFunctions & PTZF_POS_6)
	{
		if (m_ccType==CCT_DIRECT_SHOW)
		{
			CString sValue;
			sValue.Format(_T("%d:%d"), (int)CCC_POS_6, m_bPreset);
			m_pOutput->DoRequestSetValue(m_camID, CSD_CAMERA_CONTROL, sValue);
		}
		else if (m_ccType!=CCT_RELAY_PTZ)
		{
			m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_POS_6,(DWORD)m_bPreset);
		}
		if (m_bPreset)
		{
			TogglePresetMode();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVideoSNDialog::OnPos7() 
{
	if (m_dwPTZFunctions & PTZF_POS_7)
	{
		if (m_ccType==CCT_DIRECT_SHOW)
		{
			CString sValue;
			sValue.Format(_T("%d:%d"), (int)CCC_POS_7, m_bPreset);
			m_pOutput->DoRequestSetValue(m_camID, CSD_CAMERA_CONTROL, sValue);
		}
		else if (m_ccType!=CCT_RELAY_PTZ)
		{
			m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_POS_7,(DWORD)m_bPreset);
		}
		if (m_bPreset)
		{
			TogglePresetMode();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVideoSNDialog::OnPos8() 
{
	if (m_dwPTZFunctions & PTZF_POS_8)
	{
		if (m_ccType==CCT_DIRECT_SHOW)
		{
			CString sValue;
			sValue.Format(_T("%d:%d"), (int)CCC_POS_8, m_bPreset);
			m_pOutput->DoRequestSetValue(m_camID, CSD_CAMERA_CONTROL, sValue);
		}
		else if (m_ccType!=CCT_RELAY_PTZ)
		{
			m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_POS_8,(DWORD)m_bPreset);
		}
		if (m_bPreset)
		{
			TogglePresetMode();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVideoSNDialog::OnPos9() 
{
	if (m_dwPTZFunctions & PTZF_POS_9)
	{
		if (m_ccType==CCT_DIRECT_SHOW)
		{
			CString sValue;
			sValue.Format(_T("%d:%d"), (int)CCC_POS_9, m_bPreset);
			m_pOutput->DoRequestSetValue(m_camID, CSD_CAMERA_CONTROL, sValue);
		}
		else if (m_ccType!=CCT_RELAY_PTZ)
		{
			m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_POS_9,(DWORD)m_bPreset);
		}
		if (m_bPreset)
		{
			TogglePresetMode();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVideoSNDialog::OnPreset() 
{
	TogglePresetMode();
}
/////////////////////////////////////////////////////////////////////////////
void CVideoSNDialog::TogglePresetMode()
{
	m_bPreset = !m_bPreset;

	if (m_bPreset)
	{
		DWORD dwPosOnly = m_dwPTZFunctions & (PTZF_POSALL | PTZF_SET_POS);
		EnablePTZControls(dwPosOnly);
	}
	else
	{
		EnablePTZControls(m_dwPTZFunctions);
	}

	CString sText;
	sText.LoadString(m_bPreset?IDS_PRESET_CANCEL:IDS_PRESET);
	m_PresetButton.SetWindowText(sText);
}
/////////////////////////////////////////////////////////////////////////////
void CVideoSNDialog::OnFocusAuto() 
{
	UpdateData();
	if (m_dwPTZFunctions & PTZF_FOCUS_AUTO)
	{
		m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_FOCUS_AUTO,(DWORD)m_bFocusAuto);
		m_FocusInButton.EnableWindow(!m_bFocusAuto);
		m_FocusOutButton.EnableWindow(!m_bFocusAuto);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVideoSNDialog::OnIrisAuto() 
{
	UpdateData();
	if (m_dwPTZFunctions & PTZF_IRIS_AUTO)
	{
		m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_IRIS_AUTO,(DWORD)m_bIrisAuto);
		m_IrisInButton.EnableWindow(!m_bIrisAuto);
		m_IrisOutButton.EnableWindow(!m_bIrisAuto);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVideoSNDialog::OnTurn180() 
{
	if (m_dwPTZFunctions & PTZF_TURN_180)
	{
		m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_TURN_180,0);
	}
}

void CVideoSNDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if ((CSliderCtrl*)pScrollBar == &m_SpeedCtrl)
	{
		TRACE(_T("Slider moved nSBCode: %u  Pos: %u\n"), nSBCode, nPos);
		if (nSBCode == TB_ENDTRACK)
		{
			m_dwSpeed = m_SpeedCtrl.GetPos();
			m_pDisplayWindow->m_dwPTZSpeed = m_dwSpeed;
		}
	}
	
	CWK_Dialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
