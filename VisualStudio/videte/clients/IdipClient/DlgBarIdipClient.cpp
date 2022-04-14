// DlgBarIdipClient.cpp : implementation file
//

#include "stdafx.h"
#include "IdipClient.h"
#include "DlgBarIdipClient.h"
#include "MainFrm.h"

#include "ViewDialogBar.h"

// PTZ
#include "WndLive.h"
#include "CIPCInputIdipClient.h"
#include "CIPCOutputIdipClient.h"
#include ".\dlgbaridipclient.h"

class CSkinProgressEx : public CSkinProgress  
{
public:
	CSkinProgressEx() {};
    virtual ~CSkinProgressEx() {};

	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};

const UINT CDlgBarIdipClient::gm_nPTZF_POS[] = 
{
	PTZF_POS_1,		// 0
	PTZF_POS_2,		// 1
	PTZF_POS_3,		// 2
	PTZF_POS_4,		// 3
	PTZF_POS_5,		// 4
	PTZF_POS_6,		// 5
	PTZF_POS_7,		// 6
	PTZF_POS_8,		// 7
	PTZF_POS_9,		// 8
	PTZF_PAN_LEFT,	// 9
	PTZF_TILT_UP,	// 10
	PTZF_PAN_RIGHT,	// 11
	PTZF_TILT_DOWN,	// 12
	PTZF_FOCUS_NEAR,// 13
	PTZF_FOCUS_FAR,	// 14
	PTZF_IRIS_OPEN,	// 15
	PTZF_IRIS_CLOSE,// 16
	PTZF_ZOOM_IN,	// 17
	PTZF_ZOOM_OUT,	// 18
	PTZF_POS_HOME,	// 19
	PTZF_IRIS_AUTO,	// 20
	PTZF_FOCUS_AUTO,// 21
	PTZF_TURN_180	// 22
};

const CameraControlCmd CDlgBarIdipClient::gm_nCamCtrlCmd[] = 
{
	CCC_POS_1,		// 0
	CCC_POS_2,		// 1
	CCC_POS_3,		// 2
	CCC_POS_4,		// 3
	CCC_POS_5,		// 4
	CCC_POS_6,		// 5
	CCC_POS_7,		// 6
	CCC_POS_8,		// 7
	CCC_POS_9,		// 8
	CCC_PAN_LEFT,	// 9
	CCC_TILT_UP,	// 10
	CCC_PAN_RIGHT,	// 11
	CCC_TILT_DOWN,	// 12
	CCC_FOCUS_NEAR,	// 13
	CCC_FOCUS_FAR,	// 14
	CCC_IRIS_OPEN,	// 15
	CCC_IRIS_CLOSE,	// 16
	CCC_ZOOM_IN,	// 17
	CCC_ZOOM_OUT,	// 18
	CCC_POS_HOME,	// 19
	CCC_IRIS_AUTO,	// 20
	CCC_FOCUS_AUTO,	// 21
	CCC_TURN_180	// 22
};

CSkinButton	CDlgBarIdipClient::gm_DummySkinButton;

#define FLAG_KEY_IS_PRESSED 0x00004000

#define IDC_CPU_USAGE			100
#define IDC_CPU_AVG_USAGE		101
#define IDC_PERFORMANCE_LEVEL	102

LRESULT CSkinProgressEx::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_LBUTTONDOWN && GetDlgCtrlID() == IDC_PERFORMANCE_LEVEL)
	{
		int nY = HIWORD(lParam);
		CRect rc;
		GetClientRect(&rc);
		if (nY < rc.bottom >> 1)
		{
			theApp.SetPerformanceLevel((short)theApp.GetPerformanceLevel()+1);
		}
		else
		{
			theApp.SetPerformanceLevel((short)theApp.GetPerformanceLevel()-1);
		}
		SendMessage(PBM_SETPOS, theApp.GetPerformanceLevel());
	}
	return CSkinProgress::WindowProc(message, wParam, lParam);
}

/////////////////////////////////////////////////////////////////////////////
// CDlgBarIdipClient dialog
IMPLEMENT_DYNAMIC(CDlgBarIdipClient, CDlgInView)
/////////////////////////////////////////////////////////////////////////////
CDlgBarIdipClient::CDlgBarIdipClient(CWnd* pParent /*=NULL*/)
	: CDlgInView(CDlgBarIdipClient::IDD, pParent)
{
	MoveOnClientAreaClick(false);
	m_nInitToolTips = TOOLTIPS_SIMPLE;

	//{{AFX_DATA_INIT(CDlgBarIdipClient)
	m_bFocusAuto = FALSE;
	m_bIrisAuto = FALSE;
	//}}AFX_DATA_INIT

	SetPTZWindow(NULL);
	Create(CDlgBarIdipClient::IDD, pParent);
}
/////////////////////////////////////////////////////////////////////////////
CDlgBarIdipClient::~CDlgBarIdipClient()
{
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBarIdipClient::DoDataExchange(CDataExchange* pDX)
{
	CDlgInView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBarIdipClient)
	DDX_Control(pDX, IDC_SPEED, m_SpeedCtrl);
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
BEGIN_MESSAGE_MAP(CDlgBarIdipClient, CDlgInView)
	//{{AFX_MSG_MAP(CDlgBarIdipClient)
	ON_WM_LBUTTONDOWN()
	ON_WM_GETMINMAXINFO()
	ON_MESSAGE(WM_USER, OnUserMsg)
	ON_COMMAND_RANGE(0x8000, 0xDFFF, OnCommandRange)
	ON_BN_CLICKED(IDC_CLOSE_DLG_BAR, OnBnClickedCloseDlgBar)
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_CENTER, OnCenter)
	ON_BN_CLICKED(IDC_PRESET, OnPreset)
	ON_BN_CLICKED(IDC_FOCUS_AUTO, OnFocusAuto)
	ON_BN_CLICKED(IDC_IRIS_AUTO, OnIrisAuto)
	ON_BN_CLICKED(IDC_TURN_180, OnTurn180)
	ON_CONTROL_RANGE(BN_UP, IDC_LEFT, IDC_CENTER, OnButtonUp)
	ON_CONTROL_RANGE(BN_UP, IDC_ZOOMIN, IDC_ZOOMOUT, OnButtonUp)
	ON_CONTROL_RANGE(BN_DOWN, IDC_LEFT, IDC_CENTER, OnButtonDown)
	ON_CONTROL_RANGE(BN_DOWN, IDC_ZOOMIN, IDC_ZOOMOUT, OnButtonDown)
	ON_UPDATE_COMMAND_UI_RANGE(IDC_LEFT, IDC_CENTER, OnUpdateCmdPTZ)
	ON_UPDATE_COMMAND_UI_RANGE(IDC_ZOOMIN, IDC_ZOOMOUT, OnUpdateCmdPTZ)
	ON_UPDATE_COMMAND_UI_RANGE(IDC_IRIS_AUTO, IDC_TURN_180, OnUpdateCmdPTZ)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_DOME, OnUpdateButtonDome)
	ON_UPDATE_COMMAND_UI(IDC_PRESET, OnUpdateButtonPreset)
	ON_UPDATE_COMMAND_UI(IDC_BTN_KB_LANG, OnUpdateKbLang)
	ON_COMMAND(IDC_BTN_KB_LANG, OnKbLang)
	ON_WM_TIMER()
	ON_UPDATE_COMMAND_UI(IDC_BTN_ISDN_CHANNELS, OnUpdateBtnISDN)
	ON_COMMAND(IDC_BTN_ISDN_CHANNELS, OnBtnISDN)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgBarIdipClient message handlers
/////////////////////////////////////////////////////////////////////////////
BOOL CDlgBarIdipClient::OnInitDialog()
{
	CDlgInView::OnInitDialog();

	AutoCreateSkinButtons();

	CString sTitle;
	int i;
	sTitle.LoadString(IDS_CONTROL_BAR);
	SetWindowText(sTitle);

	m_SpeedCtrl.SetRange(1,10);
	for (i=1; i<=10; i++)
	{
		m_SpeedCtrl.SetTic(i);
	}
	m_SpeedCtrl.ModifyFlags(0, SKINSLIDER_USE_CUSTOM_DRAW);	// no CustomDraw ´cause it´s not transparent

	m_LeftButton.SetImageList(GetLeftButtonImages());
	m_UpButton.SetImageList(GetUpButtonImages());
	m_RightButton.SetImageList(GetRightButtonImages());
	m_DownButton.SetImageList(GetDownButtonImages());

	GetSkinButton(IDC_CENTER).SetImageList(CreateImageList(IDB_CENTER, 12), true);
	GetSkinButton(IDC_TURN_180).SetImageList(CreateImageList(IDB_TURN180, 13), true);
	GetSkinButton(ID_BUTTON_DOME).SetImageList(CreateImageList(IDB_DOME, 16), true);

	m_imgPTZ.Create(IDB_PTZ_IMAGES, 16, 0, SKIN_COLOR_KEY);
	m_IrisInButton.SetImageList(&m_imgPTZ, false, 0);
	m_IrisOutButton.SetImageList(&m_imgPTZ, false, 1);
	m_FocusInButton.SetImageList(&m_imgPTZ, false, 2);
	m_FocusOutButton.SetImageList(&m_imgPTZ, false, 3);
	m_ZoomInButton.SetImageList(&m_imgPTZ, false, 4);
	m_ZoomOutButton.SetImageList(&m_imgPTZ, false, 5);
	GetSkinButton(IDC_IRIS_AUTO).SetImageList(&m_imgPTZ, false, 6);
	GetSkinButton(IDC_FOCUS_AUTO).SetImageList(&m_imgPTZ, false, 6);

	CSkinButton &btnISDN = GetSkinButton(IDC_BTN_ISDN_CHANNELS);
	btnISDN.SetTextColorDisabled(CSkinButton::GetTextColorNormalG());
	btnISDN.SetSurface(SurfacePlaneColor);
	btnISDN.SetStyle(StyleStaticSuncen);

	CSkinButton &btnSaveState= GetSkinButton(IDC_BTN_SAVE_STATE);
	CImageList *pList =new CImageList;
	pList->Create(IDB_STOP, 16, 0, SKIN_COLOR_KEY);
	btnSaveState.SetImageList(pList, true, 0);
	btnSaveState.ShowWindow(SW_HIDE);
	btnSaveState.SetSurface(SurfacePlaneColor);
	btnSaveState.SetStyle(StyleStaticSuncen);

	// TODO! RKE: new Sequencer modus to be implemented
	CSkinButton &btnSequence = GetSkinButton(IDC_BTN_SEQUENCE);
	btnSequence.ShowWindow(SW_HIDE);
	btnSequence.SetTextColorDisabled(CSkinButton::GetTextColorNormalG());
	btnSequence.SetSurface(SurfacePlaneColor);
	btnSequence.SetStyle(StyleStaticSuncen);

	//keep proportion of all buttons in dlg
	CWnd *pWnd = GetWindow(GW_CHILD);
	while (pWnd && IsChild(pWnd))
	{
		int iCtrlID = pWnd->GetDlgCtrlID();
		switch (iCtrlID)
		{
			case IDC_CLOSE_DLG_BAR:
			case IDC_BTN_KB_LANG:
			case IDC_BTN_ISDN_CHANNELS:
			case IDC_BTN_SAVE_STATE:
			case IDC_BTN_SEQUENCE:
			//TODO! RKE: Right align verbessern in SkinDialog
				KeepProportion(iCtrlID, PROPORTION_KEEP_SIZE|PROPORTION_RIGHT_ALIGN);
			break;
			default:
				KeepProportion(iCtrlID);
			break;
		}
		pWnd = pWnd->GetNextWindow();
	}

	SetPTZFunctions(0, 0);

	CKBInput &kbi = COemGuiApi::GetKeyboardInput(0);
	if (kbi.IsValid())
	{
		CSkinButton &btnKBlang = GetSkinButton(IDC_BTN_KB_LANG);
		if (btnKBlang.m_hWnd)
		{
			btnKBlang.SetWindowText(kbi.GetShortName());
			btnKBlang.ShowWindow(SW_SHOW);
		}
	}

	if (theApp.ShowCPUusage() & CPU_USAGE_SHOW)
	{
		CRect rc;
		m_SpeedCtrl.GetWindowRect(&rc);
		ScreenToClient(&rc);
		rc.left += rc.Width()/2;
		int nWidth  = rc.Width();

		rc.OffsetRect(nWidth+10, 0);
		CSkinProgressEx*pProgress = new CSkinProgressEx;
		m_AutoCtrlList.AddHead(pProgress);
		pProgress->Create(PBS_VERTICAL|WS_VISIBLE|WS_CHILD, rc, this, IDC_CPU_USAGE);
		pProgress->SetRange(0, 100);

		rc.OffsetRect(nWidth, 0);
		pProgress = new CSkinProgressEx;
		m_AutoCtrlList.AddHead(pProgress);
		pProgress->Create(PBS_VERTICAL|WS_VISIBLE|WS_CHILD, rc, this, IDC_CPU_AVG_USAGE);
		pProgress->SetRange(0, 100);

		rc.OffsetRect(nWidth, 0);
		pProgress = new CSkinProgressEx;
		m_AutoCtrlList.AddHead(pProgress);
		pProgress->Create(PBS_VERTICAL|WS_VISIBLE|WS_CHILD, rc, this, IDC_PERFORMANCE_LEVEL);
		int nMax = MAX_PERFORMANCE_LEVEL + -theApp.GetMinPerformanceLevel();
		pProgress->SetRange(0, (short)nMax);
	}

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
CImageList * CDlgBarIdipClient::CreateImageList(UINT nBmpId, int nWidth)
{
	CImageList *pImageList = new CImageList;
	if (!pImageList->Create(nBmpId, nWidth, 0, SKIN_COLOR_KEY)) ASSERT(FALSE);
	return pImageList;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDlgBarIdipClient::StretchElements()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CDlgBarIdipClient::OnUserMsg(WPARAM wParam, LPARAM lParam)
{
	WORD wLow = LOWORD(wParam);
	if (wLow == WM_INITDIALOG && HIWORD(wParam) == 0)
	{
		int nImage, nID;
		bool bAutoDelete;
		CImageList*pIL = NULL;
		CWnd*pBtn = (CWnd*) GetDlgItem(RUNTIME_CLASS(CWnd), NULL);
		while (pBtn)
		{
			nID = pBtn->GetDlgCtrlID();
			bAutoDelete = false;
			if (nID == IDC_CLOSE_DLG_BAR)
			{
				if (theApp.m_bDialogBarTop)
				{
					pIL = GetUpButtonImages();
				}
				else
				{
					pIL = GetDownButtonImages();
				}
				nImage = -1;
			}
			else
			{
				pIL = theApp.GetMainFrame()->GetToolBarImageListFromID(nID, nImage);
			}
			if (pIL == NULL)
			{
				int nSize = 16;
				CImageList il;
				switch (nID)
				{
					//anpassen der Größe der Buttoninhalte
					case ID_NAVIGATE_SKIP_FORWARD :
						nSize = 17; break;
					case ID_NAVIGATE_FAST_BACK: 
					case ID_NAVIGATE_FAST_FORWARD: 
					case ID_NAVIGATE_POS1:
						nSize = 20; break;
					case ID_FILE_HTML:
					case ID_NAVIGATE_END :
						nSize = 21; break;
					case ID_FILE_PRINT:    
						nSize = 24; break;
					case ID_CONNECT_MAP:
					case ID_HOST_CAMERA_MAP:
						nSize = 28; break;
				}
                if (il.Create((UINT)nID, (int)nSize, (int)0, SKIN_COLOR_KEY))
				{
					pIL = new CImageList;
					pIL->Create(&il);
					nImage = -1;
				}
				bAutoDelete = true;
			}
			if (pIL)
			{
				if (!UseOEMSkin() && theApp.IsWinXP() == TRUE && theApp.IsAppThemed())
				{
					pBtn = &GetSkinButton(nID);
				}
				if (pBtn->IsKindOf(RUNTIME_CLASS(CSkinButton)))
				{
					((CSkinButton*)pBtn)->SetImageList(pIL, bAutoDelete, nImage);
				}
				else
				{
					if (nImage == -1)
					{
						nImage = 0;
					}
					HICON hIcon = pIL->ExtractIcon(nImage);
					pBtn->ModifyStyle(0, BS_ICON);
					pBtn->SendMessage(BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIcon);
					if (bAutoDelete)
					{
						delete pIL;
					}
				}
			}
			pBtn =(CWnd*) GetDlgItem(RUNTIME_CLASS(CWnd), pBtn);
		}
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
CWnd * CDlgBarIdipClient::GetDlgItem(CRuntimeClass*pRC, CWnd*pWndPrevious)
{
	CWnd *pWnd = (pWndPrevious == NULL) ? GetWindow(GW_CHILD) : pWndPrevious->GetNextWindow();
	while (pWnd && IsChild(pWnd))
	{
		if (pWnd->IsKindOf(pRC))
		{
			return pWnd;
		}
		pWnd = pWnd->GetNextWindow();
	};
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBarIdipClient::OnCommandRange(UINT nID)
{
	if (IsBetween(nID, ID_BUTTON_NUM1, ID_BUTTON_NUM9))
	{
		OnNum(nID);
	}
	else if (nID == ID_BUTTON_DOME)
	{
		CDataExchange dx(this, TRUE);
		DDX_Check(&dx, ID_BUTTON_DOME, m_bDome);
		EnablePTZControls(m_dwPTZFunctions);
	}
	else
	{
		PostCommandID(nID);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CDlgBarIdipClient::OnBnClickedCloseDlgBar()
{
	theApp.GetMainFrame()->PostMessage(WM_USER, WM_CLOSE, (LPARAM)this);
}
/////////////////////////////////////////////////////////////////////////////
// PTZ Controls
void CDlgBarIdipClient::SetPTZType(CameraControlType ccType)
{
	m_ccType = ccType;
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBarIdipClient::SetPTZFunctions(DWORD dwFunctions, DWORD dwPTZFunctionsEx)
{
	m_dwPTZFunctions = dwFunctions;
	m_dwPTZFunctionsEx = dwPTZFunctionsEx;
	if (m_dwPTZFunctions & PTZF_SPEED && m_pWndLive)
	{
		m_dwSpeed = m_pWndLive->GetPTZSpeed();
		m_SpeedCtrl.SetPos(11-m_dwSpeed);
	}
	else
	{
		m_dwSpeed = 0L;
	}
	if (m_hWnd)
	{
		EnablePTZControls(m_dwPTZFunctions);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBarIdipClient::EnablePTZControls(DWORD dwFunctions)
{
	int i, nID;

	m_SpeedCtrl.EnableWindow(dwFunctions & PTZF_SPEED ? TRUE : FALSE);

	for (i=0, nID=ID_BUTTON_NUM1; nID<=ID_BUTTON_NUM9; i++, nID++)
	{
		CSkinButton &Btn = GetSkinButton(nID);
		if (m_bDome && dwFunctions & gm_nPTZF_POS[i])
		{
			Btn.SetTextColorNormal(SKIN_COLOR_DARK_RED);
			Btn.SetTextColorHighlighted(SKIN_COLOR_RED);
		}
		else
		{
			Btn.SetTextColorNormal(CSkinButton::GetTextColorNormalG());
			Btn.SetTextColorHighlighted(CSkinButton::GetTextColorHighlightedG());
		}
	}
	GetSkinButton(ID_BUTTON_DOME).SetCheck(m_bDome);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDlgBarIdipClient::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (m_bDome)
	{
		TRACE(_T("OnKeyDown(%d, %d, %x)\n"), nChar, nRepCnt, nFlags);
		switch (nChar)
		{
		case VK_UP:
			return OnButtonDown(IDC_UP, nRepCnt, nFlags);
		case VK_DOWN:
			return OnButtonDown(IDC_DOWN, nRepCnt, nFlags);
		case VK_LEFT:
			return OnButtonDown(IDC_LEFT, nRepCnt, nFlags);
		case VK_RIGHT:
			return OnButtonDown(IDC_RIGHT, nRepCnt, nFlags);
		case VK_INSERT:
			return OnButtonDown(IDC_IRISIN, nRepCnt, nFlags);
		case VK_DELETE:
			return OnButtonDown(IDC_IRISOUT, nRepCnt, nFlags);
		case VK_HOME:
			return OnButtonDown(IDC_FOCUSOUT, nRepCnt, nFlags);
		case VK_END:
			return OnButtonDown(IDC_FOCUSIN, nRepCnt, nFlags);
		case VK_PRIOR:
			return OnButtonDown(IDC_ZOOMIN, nRepCnt, nFlags);
		case VK_NEXT:
			return OnButtonDown(IDC_ZOOMOUT, nRepCnt, nFlags);
		case VK_ADD:
			if (m_dwPTZFunctions & PTZF_SPEED)
			{
				m_SpeedCtrl.SetPos(m_SpeedCtrl.GetPos()-1);
				OnVScroll(TB_ENDTRACK, m_SpeedCtrl.GetPos(), (CScrollBar*)&m_SpeedCtrl);
				return TRUE;
			}break;
		case VK_SUBTRACT:
			if (m_dwPTZFunctions & PTZF_SPEED)
			{
				m_SpeedCtrl.SetPos(m_SpeedCtrl.GetPos()+1);
				OnVScroll(TB_ENDTRACK, m_SpeedCtrl.GetPos(), (CScrollBar*)&m_SpeedCtrl);
				return TRUE;
			}break;
		case VK_NUMPAD0:
			if (!(nFlags & FLAG_KEY_IS_PRESSED))
			{
				GetSkinButton(IDC_CENTER).CButton::SetCheck(BST_CHECKED);
				OnCenter();
			}
			return TRUE;
		case VK_RETURN:
			if (!(nFlags & FLAG_KEY_IS_PRESSED))
			{
				GetSkinButton(IDC_PRESET).CButton::SetCheck(BST_CHECKED);
				OnPreset();
			}
			return TRUE;
		case VK_MULTIPLY:
			if (!(nFlags & FLAG_KEY_IS_PRESSED))
			{
				m_bDome = FALSE;
				GetSkinButton(ID_BUTTON_DOME).CButton::SetCheck(BST_UNCHECKED);
				EnablePTZControls(m_dwPTZFunctions);
			}
			return TRUE;
		}
	}
	switch (nChar)
	{
	case VK_NUMPAD0:
		return OnNum(ID_BUTTON_NUM0, nRepCnt, nFlags);
	case VK_NUMPAD1:
		return OnNum(ID_BUTTON_NUM1, nRepCnt, nFlags);
	case VK_NUMPAD2:
		return OnNum(ID_BUTTON_NUM2, nRepCnt, nFlags);
	case VK_NUMPAD3:
		return OnNum(ID_BUTTON_NUM3, nRepCnt, nFlags);
	case VK_NUMPAD4:
		return OnNum(ID_BUTTON_NUM4, nRepCnt, nFlags);
	case VK_NUMPAD5:
		return OnNum(ID_BUTTON_NUM5, nRepCnt, nFlags);
	case VK_NUMPAD6:
		return OnNum(ID_BUTTON_NUM6, nRepCnt, nFlags);
	case VK_NUMPAD7:
		return OnNum(ID_BUTTON_NUM7, nRepCnt, nFlags);
	case VK_NUMPAD8:
		return OnNum(ID_BUTTON_NUM8, nRepCnt, nFlags);
	case VK_NUMPAD9:
		return OnNum(ID_BUTTON_NUM9, nRepCnt, nFlags);
	case VK_DIVIDE:
		if (!(nFlags & FLAG_KEY_IS_PRESSED))
		{
			PostCommandID(ID_BUTTON_NUMPLUS);
			GetSkinButton(ID_BUTTON_NUMPLUS).CButton::SetCheck(BST_UNCHECKED);
		}
		return TRUE;
	case VK_MULTIPLY:
		if (!(nFlags & FLAG_KEY_IS_PRESSED) && GetSkinButton(ID_BUTTON_DOME).IsWindowEnabled())
		{
			m_bDome = TRUE;
			GetSkinButton(ID_BUTTON_DOME).CButton::SetCheck(BST_CHECKED);
			EnablePTZControls(m_dwPTZFunctions);
		}
		return TRUE;
//	case VK_TAB:
//      TODO! RKE: Change the Focus of the views or windows
//		return TRUE;

	}
	
	CSkinDialog::OnKeyDown(nChar, nRepCnt, nFlags);
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDlgBarIdipClient::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (m_bDome)
	{
		switch (nChar)
		{
		case VK_UP:
			return OnButtonUp(IDC_UP, nRepCnt, nFlags);
		case VK_DOWN:
			return OnButtonUp(IDC_DOWN, nRepCnt, nFlags);
		case VK_LEFT:
			return OnButtonUp(IDC_LEFT, nRepCnt, nFlags);
		case VK_RIGHT:
			return OnButtonUp(IDC_RIGHT, nRepCnt, nFlags);
		case VK_INSERT:
			return OnButtonUp(IDC_IRISIN, nRepCnt, nFlags);
		case VK_DELETE:
			return OnButtonUp(IDC_IRISOUT, nRepCnt, nFlags);
		case VK_HOME:
			return OnButtonUp(IDC_FOCUSOUT, nRepCnt, nFlags);
		case VK_END:
			return OnButtonUp(IDC_FOCUSIN, nRepCnt, nFlags);
		case VK_PRIOR:
			return OnButtonUp(IDC_ZOOMIN, nRepCnt, nFlags);
		case VK_NEXT:
			return OnButtonUp(IDC_ZOOMOUT, nRepCnt, nFlags);
		case VK_NUMPAD0:
			GetSkinButton(IDC_CENTER).CButton::SetCheck(BST_UNCHECKED);
			return TRUE;
		case VK_RETURN:
			GetSkinButton(IDC_PRESET).CButton::SetCheck(BST_UNCHECKED);
			return TRUE;
		}
	}

	switch (nChar)
	{
	case VK_NUMPAD0:
		GetSkinButton(ID_BUTTON_NUM0).CButton::SetCheck(BST_UNCHECKED);
		return TRUE;
	case VK_NUMPAD1:
		GetSkinButton(ID_BUTTON_NUM1).CButton::SetCheck(BST_UNCHECKED);
		return TRUE;
	case VK_NUMPAD2:
		GetSkinButton(ID_BUTTON_NUM2).CButton::SetCheck(BST_UNCHECKED);
		return TRUE;
	case VK_NUMPAD3:
		GetSkinButton(ID_BUTTON_NUM3).CButton::SetCheck(BST_UNCHECKED);
		return TRUE;
	case VK_NUMPAD4:
		GetSkinButton(ID_BUTTON_NUM4).CButton::SetCheck(BST_UNCHECKED);
		return TRUE;
	case VK_NUMPAD5:
		GetSkinButton(ID_BUTTON_NUM5).CButton::SetCheck(BST_UNCHECKED);
		return TRUE;
	case VK_NUMPAD6:
		GetSkinButton(ID_BUTTON_NUM6).CButton::SetCheck(BST_UNCHECKED);
		return TRUE;
	case VK_NUMPAD7:
		GetSkinButton(ID_BUTTON_NUM7).CButton::SetCheck(BST_UNCHECKED);
		return TRUE;
	case VK_NUMPAD8:
		GetSkinButton(ID_BUTTON_NUM8).CButton::SetCheck(BST_UNCHECKED);
		return TRUE;
	case VK_NUMPAD9:
		GetSkinButton(ID_BUTTON_NUM9).CButton::SetCheck(BST_UNCHECKED);
		return TRUE;
	case VK_DIVIDE:
		GetSkinButton(ID_BUTTON_NUMPLUS).CButton::SetCheck(BST_UNCHECKED);
		return TRUE;
	}
	
	CSkinDialog::OnKeyUp(nChar, nRepCnt, nFlags);
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
CSkinButton& CDlgBarIdipClient::GetSkinButton(int nID)
{
	CWnd *pWnd = CWnd::GetDlgItem(nID);
	if (pWnd)
	{
		if (!pWnd->IsKindOf(RUNTIME_CLASS(CSkinButton)))
		{
			CSkinButton *pBtn = new CSkinButton();
			m_AutoCtrlList.AddHead(pBtn);
			CDataExchange dx(this, FALSE);
			DDX_Control(&dx, nID, *pBtn);
			return *pBtn;
		}
		return *((CSkinButton*)pWnd);
	}
	else
	{
		return gm_DummySkinButton;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBarIdipClient::TogglePresetMode()
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
	GetSkinButton(IDC_PRESET).SetWindowText(sText);
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBarIdipClient::OnCenter() 
{
	if (m_dwPTZFunctions & PTZF_POS_HOME)
	{
		if (m_ccType!=CCT_RELAY_PTZ && m_pInput)
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
int  CDlgBarIdipClient::GetIndexFromID(UINT nID)
{
	switch (nID)
	{
		case ID_BUTTON_NUM1:	nID =  0; break;
		case ID_BUTTON_NUM2:	nID =  1; break;
		case ID_BUTTON_NUM3:	nID =  2; break;
		case ID_BUTTON_NUM4:	nID =  3; break;
		case ID_BUTTON_NUM5:	nID =  4; break;
		case ID_BUTTON_NUM6:	nID =  5; break;
		case ID_BUTTON_NUM7:	nID =  6; break;
		case ID_BUTTON_NUM8:	nID =  7; break;
		case ID_BUTTON_NUM9:	nID =  8; break;
		case IDC_LEFT:			nID =  9; break;
		case IDC_UP:			nID = 10; break;
		case IDC_RIGHT:			nID = 11; break;
		case IDC_DOWN:			nID = 12; break;
		case IDC_FOCUSIN:		nID = 13; break;
		case IDC_FOCUSOUT:		nID = 14; break;
		case IDC_IRISIN:		nID = 15; break;
		case IDC_IRISOUT:		nID = 16; break;
		case IDC_ZOOMIN:		nID = 17; break;
		case IDC_ZOOMOUT:		nID = 18; break;
		case IDC_CENTER:		nID = 19; break;
		case IDC_IRIS_AUTO:		nID = 20; break;
		case IDC_FOCUS_AUTO:	nID = 21; break;
		case IDC_TURN_180:		nID = 22; break;
		default:
			nID = (UINT)-1; 
			ASSERT(FALSE);
			break;
	}
	return (int)nID;
}
/////////////////////////////////////////////////////////////////////////////
CameraControlCmd CDlgBarIdipClient::GetStopCmd(CameraControlCmd nCmd)
{
	switch (nCmd)
	{
		case CCC_PAN_LEFT:		case CCC_PAN_RIGHT:		return CCC_PAN_STOP;
		case CCC_TILT_UP:		case CCC_TILT_DOWN:		return CCC_TILT_STOP;
		case CCC_FOCUS_NEAR:	case CCC_FOCUS_FAR:		return CCC_FOCUS_STOP;
		case CCC_IRIS_OPEN:		case CCC_IRIS_CLOSE:	return CCC_IRIS_STOP;
		case CCC_ZOOM_IN:		case CCC_ZOOM_OUT:		return CCC_ZOOM_STOP;
	}
	ASSERT(FALSE);
	return CCC_INVALID;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDlgBarIdipClient::OnNum(UINT nID, UINT nRepCnt, UINT nFlags)
{
	if (!(nFlags & FLAG_KEY_IS_PRESSED))
	{
		GetSkinButton(nID).CButton::SetCheck(BST_CHECKED);
		OnNum(nID);
	}
	return GetSkinButton(nID).IsWindowEnabled();
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBarIdipClient::OnNum(UINT nID)
{
	int nIndex = nID - ID_BUTTON_NUM1;
	if (m_bDome && m_dwPTZFunctions & gm_nPTZF_POS[nIndex])
	{
		if (m_ccType!=CCT_RELAY_PTZ && m_pInput)
		{
			m_pInput->DoRequestCameraControl(m_commID, m_camID, gm_nCamCtrlCmd[nIndex], (DWORD)m_bPreset);
		}
		if (m_bPreset)
		{
			TogglePresetMode();
		}
	}
	else
	{
		PostCommandID(nID);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDlgBarIdipClient::OnButtonDown(UINT nID, UINT nRepCnt, UINT nFlags)
{
	CSkinButton& btn = GetSkinButton(nID);
	if (btn.IsWindowEnabled())
	{
		if (!(nFlags & FLAG_KEY_IS_PRESSED))
		{
			btn.CButton::SetCheck(BST_CHECKED);
			OnButtonDown(nID);
		}
		return	TRUE;
	}
	return FALSE;
}
void CDlgBarIdipClient::OnButtonDown(UINT nID)
{
	int nIndex = GetIndexFromID(nID);
	if (m_ccType==CCT_RELAY_PTZ)
	{
		m_pOutput->RequestCameraControl(gm_nCamCtrlCmd[nIndex], TRUE);
	}
	if (m_pInput)
	{
		m_pInput->DoRequestCameraControl(m_commID, m_camID, gm_nCamCtrlCmd[nIndex], m_dwSpeed);
	}
	switch (nID)
	{
		case IDC_IRISIN: case IDC_IRISOUT:
			GetSkinButton(IDC_IRIS_AUTO).CButton::SetCheck(BST_UNCHECKED);
			m_bIrisAuto = FALSE;
			break;
		case IDC_FOCUSIN: case IDC_FOCUSOUT:
			GetSkinButton(IDC_FOCUS_AUTO).CButton::SetCheck(BST_UNCHECKED);
			m_bFocusAuto = FALSE;
			break;
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDlgBarIdipClient::OnButtonUp(UINT nID, UINT nRepCnt, UINT nFlags)
{
	CSkinButton& btn = GetSkinButton(nID);
	if (btn.IsWindowEnabled())
	{
		btn.CButton::SetCheck(BST_UNCHECKED);
		OnButtonUp(nID);
		return TRUE;
	}
	return FALSE;
}
void CDlgBarIdipClient::OnButtonUp(UINT nID)//, NMHDR*, LRESULT*) 
{
	if (m_pWndLive)
	{
		int nIndex = GetIndexFromID(nID);
		if (m_ccType==CCT_RELAY_PTZ)
		{
			m_pOutput->RequestCameraControl(gm_nCamCtrlCmd[nIndex], FALSE);
		}
		if (m_pInput)
		{
			m_pInput->DoRequestCameraControl(m_commID, m_camID, GetStopCmd(gm_nCamCtrlCmd[nIndex]), 0L);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBarIdipClient::OnTurn180() 
{
	if (m_dwPTZFunctions & PTZF_TURN_180 &&  m_pInput)
	{
		m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_TURN_180,0);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBarIdipClient::OnIrisAuto() 
{
	if (m_dwPTZFunctions & PTZF_IRIS_AUTO && m_pInput)
	{
		CDataExchange dx(this, FALSE);
		m_bIrisAuto = TRUE;
		DDX_Check(&dx, IDC_IRIS_AUTO, m_bIrisAuto);
		m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_IRIS_AUTO,(DWORD)m_bIrisAuto);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBarIdipClient::OnFocusAuto() 
{
	if (m_dwPTZFunctions & PTZF_FOCUS_AUTO && m_pInput)
	{
		CDataExchange dx(this, FALSE);
		m_bFocusAuto = TRUE;
		DDX_Check(&dx, IDC_FOCUS_AUTO, m_bFocusAuto);
		m_pInput->DoRequestCameraControl(m_commID,m_camID,CCC_FOCUS_AUTO,(DWORD)m_bFocusAuto);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBarIdipClient::OnPreset() 
{
	TogglePresetMode();
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBarIdipClient::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if ((CSliderCtrl*)pScrollBar == &m_SpeedCtrl)
	{
		if (nSBCode == TB_ENDTRACK && m_pWndLive)
		{
			TRACE(_T("Slider moved nSBCode: %u  Pos: %u\n"), nSBCode, nPos);
			m_dwSpeed = 11-m_SpeedCtrl.GetPos();
			m_pWndLive->SetPTZSpeed(m_dwSpeed);
		}
	}
	
	CSkinDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBarIdipClient::SetPTZWindow(CWndLive*pWndLive)
{
	m_pWndLive = pWndLive;
	if (pWndLive)
	{
		if (pWndLive->GetCameraControlType() == CCT_UNKNOWN)
		{
			SetPTZWindow(NULL);
			return;
		}
		CServer *pServer = pWndLive->GetServer();
		if (pServer)
		{
			m_pInput = pServer->GetInput();
			m_pOutput = pServer->GetOutput();
			m_commID = m_pInput->GetCommID();
		}
		m_camID = m_pWndLive->GetCamID();
		SetPTZType(pWndLive->GetCameraControlType());
		m_bDome		= TRUE;
		SetPTZFunctions(pWndLive->GetPTZFunctions(), pWndLive->GetPTZFunctionsEx());
	}
	else
	{
		m_pWndLive	= NULL;
		m_pInput	= NULL;
		m_pOutput	= NULL;
		m_commID	= SECID_NO_ID;
		m_camID		= SECID_NO_ID;
		m_bDome		= FALSE;
		SetPTZType(CCT_UNKNOWN);
		SetPTZFunctions(0, 0);
	}
	m_bPreset = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBarIdipClient::OnUpdateButtonDome(CCmdUI*pCmdUI)
{
	pCmdUI->Enable(m_dwPTZFunctions != 0);
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBarIdipClient::OnUpdateButtonPreset(CCmdUI*pCmdUI)
{
	pCmdUI->Enable(m_dwPTZFunctions & PTZF_SET_POS);
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBarIdipClient::OnUpdateCmdPTZ(CCmdUI*pCmdUI)
{
	int nIndex = GetIndexFromID(pCmdUI->m_nID);
	pCmdUI->Enable(m_dwPTZFunctions & gm_nPTZF_POS[nIndex] ? TRUE : FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBarIdipClient::PostCommandID(UINT nID)
{
	if (theApp.m_pMainWnd)
	{
		theApp.m_pMainWnd->PostMessage(WM_COMMAND, nID, (LPARAM)m_hWnd);
	}
}
#define TRACE_MESSAGE(x)  case x: TRACE(_T("%s\n"), _T(#x)); break;
#define NTRACE_MESSAGE(x)  case x: break;
/////////////////////////////////////////////////////////////////////////////
LRESULT CDlgBarIdipClient::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		NTRACE_MESSAGE(WM_CHAR);
		NTRACE_MESSAGE(WM_USER);
		NTRACE_MESSAGE(WM_NOTIFY);
		NTRACE_MESSAGE(WM_NOTIFYFORMAT);
		NTRACE_MESSAGE(WM_DRAWITEM);
		NTRACE_MESSAGE(WM_CTLCOLORBTN);
		NTRACE_MESSAGE(WM_NCHITTEST);
		NTRACE_MESSAGE(WM_ERASEBKGND);
		NTRACE_MESSAGE(0x0363);
		NTRACE_MESSAGE(0x0019);
		NTRACE_MESSAGE(WM_SETCURSOR);
		NTRACE_MESSAGE(WM_PAINT);
		NTRACE_MESSAGE(WM_NCPAINT);
		NTRACE_MESSAGE(WM_COMMAND);
		NTRACE_MESSAGE(WM_MOUSEMOVE);
		NTRACE_MESSAGE(WM_WINDOWPOSCHANGING);
		NTRACE_MESSAGE(WM_WINDOWPOSCHANGED);
		NTRACE_MESSAGE(WM_NCCALCSIZE);
		NTRACE_MESSAGE(WM_CHILDACTIVATE);
		NTRACE_MESSAGE(WM_SIZE);
		NTRACE_MESSAGE(WM_GETOBJECT);
		NTRACE_MESSAGE(WM_VSCROLL);
		NTRACE_MESSAGE(WM_MOUSEACTIVATE);
//		default:TRACE(_T("%x\n"), message); break;
	}

	return CDlgInView::WindowProc(message, wParam, lParam);
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBarIdipClient::OnUpdateKbLang(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBarIdipClient::OnBtnISDN()
{
	// TODO! RKE: change ISDN connection
	// disconnect one channel
	// connect additional channel
	// disconnect all
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBarIdipClient::OnUpdateBtnISDN(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBarIdipClient::OnKbLang()
{
	COemGuiApi::ToggleKeyboardInput();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDlgBarIdipClient::OnKeyboardChanged(CKBInput*pActive)
{
	CSkinButton &btnKBlang = GetSkinButton(IDC_BTN_KB_LANG);
	if (btnKBlang.m_hWnd)
	{
		if (pActive->IsValid())
		{
			btnKBlang.SetWindowText(pActive->GetShortName());
			if (!btnKBlang.IsWindowVisible())
			{
				btnKBlang.ShowWindow(SW_SHOW);
			}
		}
		else
		{
			btnKBlang.ShowWindow(SW_HIDE);
		}
		return TRUE;
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBarIdipClient::OnISDNBchannels(WORD wChannels, LPCTSTR sText)
{
	SetDlgItemText(IDC_BTN_ISDN_CHANNELS, sText);
	CWnd *pWnd = CWnd::GetDlgItem(IDC_BTN_ISDN_CHANNELS);
	if (pWnd)
	{
		pWnd->ShowWindow(wChannels ? SW_SHOW : SW_HIDE);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDlgBarIdipClient::GetToolTip(UINT nID, CString&sText)
{
	switch (nID)
	{
		case IDC_BTN_SAVE_STATE:
		{
			sText.LoadString(ID_DATABASE_TEXT);
			CString sWndTxt;
			CDialog::GetDlgItem(nID)->GetWindowText(sWndTxt);
			sText += _T(" ") + sWndTxt;
			return TRUE;
		}
		case IDC_BTN_SEQUENCE:
		if (sText.LoadString(ID_SEQUENCE_TEXT))
		{
			sText.Replace(_T(":"), _T(""));
			return TRUE;
		}break;
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBarIdipClient::OnTimer(UINT nIDEvent)
{
	if (   nIDEvent == theApp.GetMainFrame()->GetOneSecTimerID()
		&& IsWindowVisible())
	{
		CWnd *pWnd =  CDialog::GetDlgItem(IDC_CPU_USAGE);
		if (pWnd)
		{
			CRect rc;
			CDialog::GetDlgItem(IDC_CLOSE_DLG_BAR)->GetWindowRect(&rc);
			int nLeft = rc.left;
			pWnd->GetWindowRect(&rc);
			pWnd->ShowWindow(rc.right > nLeft ? SW_HIDE : SW_SHOW);
			pWnd->SendMessage(PBM_SETPOS, 	theApp.GetCPUusage());

			pWnd =  CDialog::GetDlgItem(IDC_CPU_AVG_USAGE);
			if (pWnd)
			{
				pWnd->GetWindowRect(&rc);
				pWnd->ShowWindow(rc.right > nLeft ? SW_HIDE : SW_SHOW);
				pWnd->SendMessage(PBM_SETPOS, 	theApp.GetAvgCPUusage());
			}
			pWnd =  CDialog::GetDlgItem(IDC_PERFORMANCE_LEVEL);
			if (pWnd)
			{
				pWnd->GetWindowRect(&rc);
				pWnd->ShowWindow(rc.right > nLeft ? SW_HIDE : SW_SHOW);
				pWnd->SendMessage(PBM_SETPOS, theApp.GetPerformanceLevel()+ -theApp.GetMinPerformanceLevel());
			}
		}
	}
	CDlgInView::OnTimer(nIDEvent);
}
