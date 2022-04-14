// JoystickPage.cpp : implementation file
//

#include "stdafx.h"
#include "SystemVerwaltung.h"
#include "JoystickPage.h"
#include ".\joystickpage.h"

#define GAMECONTROLLER 0

/////////////////////////////////////////////////////////////////////////////////////////
// CTargetCrossControl
IMPLEMENT_DYNCREATE(CTargetCrossControl, CStatic)
CTargetCrossControl::CTargetCrossControl() :
	m_szCross(10, 10),
	m_ptHorzRange(-100, 100),
	m_ptVertRange(-100, 100),
	m_ptPos(0, 0)
{

	m_nZeroRange  = 0;
	m_cBkGndColor = RGB(255,255,255);	// white
	m_cCrossColor = RGB(0,0,0);			// black
	m_cZeroRangeColor = RGB(255,0,0);	// red
}
/////////////////////////////////////////////////////////////////////////////////////////
CTargetCrossControl::~CTargetCrossControl()
{
}
/////////////////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CTargetCrossControl, CStatic)
	//{{AFX_MSG_MAP(CTargetCrossControl)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////////////////
// CTargetCrossControl message handlers
void CTargetCrossControl::OnPaint()
{
	CPaintDC dc(this);
	CRect rc;
	CSize szOld;
	GetClientRect(&rc);

	CPoint pt = TransformPoint(rc, m_ptPos);
	CSize  szCross(m_szCross.cx>>1, m_szCross.cy>>1);
	CPen penCross(PS_SOLID, 1, m_cCrossColor);
	CRgn rgn;

	dc.SaveDC();
	rgn.CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);
	dc.SelectClipRgn(&rgn, RGN_COPY);
	rc.DeflateRect(1,1);

	dc.SelectObject(&penCross);
	dc.MoveTo(pt.x - szCross.cx, pt.y);
	dc.LineTo(pt.x + szCross.cx, pt.y);
	dc.MoveTo(pt.x, pt.y - szCross.cy);
	dc.LineTo(pt.x, pt.y + szCross.cy);

	if (m_nZeroRange)
	{
		CPen penZero(PS_SOLID, 1, m_cZeroRangeColor);
		dc.SelectObject(&penZero);
		dc.SelectStockObject(HOLLOW_BRUSH);
		pt = TransformPoint(rc, CPoint(0,0));
		int nX = MulDiv(m_nZeroRange, rc.Width(), m_ptHorzRange.y - m_ptHorzRange.x);
		int nY = MulDiv(m_nZeroRange, rc.Height(),m_ptVertRange.y - m_ptVertRange.x);
		rc.left   = pt.x - nX;
		rc.right  = pt.x + nX;
		rc.top    = pt.y - nY;
		rc.bottom = pt.y + nY;
		dc.Rectangle(&rc);
		dc.RestoreDC(-1);
	}
	else
	{
		dc.RestoreDC(-1);
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
BOOL CTargetCrossControl::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(&rc);
	pDC->FillSolidRect(&rc, m_cBkGndColor);
	return TRUE;
//	return CStatic::OnEraseBkgnd(pDC);
}
/////////////////////////////////////////////////////////////////////////////////////////
void CTargetCrossControl::SetCrossColor(COLORREF col)
{
	m_cCrossColor = col;
	InvalidateRect(NULL);
}
/////////////////////////////////////////////////////////////////////////////////////////
void CTargetCrossControl::SetZeroRangeColor(COLORREF col)
{
	m_cZeroRangeColor = col;
	InvalidateRect(NULL);
}
/////////////////////////////////////////////////////////////////////////////////////////
void CTargetCrossControl::SetBkGndColor(COLORREF col)
{
	m_cBkGndColor = col;
	InvalidateRect(NULL);
}
/////////////////////////////////////////////////////////////////////////////////////////
void CTargetCrossControl::SetHorzRange(CPoint ptHorz)
{
	SetHorzRange(ptHorz.x, ptHorz.y);
}
void CTargetCrossControl::SetHorzRange(int nX1, int nX2)
{
	m_ptHorzRange.x = nX1;
	m_ptHorzRange.y = nX2;
	InvalidateRect(NULL);
}
/////////////////////////////////////////////////////////////////////////////////////////
void CTargetCrossControl::SetVertRange(CPoint ptVert)
{
	SetVertRange(ptVert.x, ptVert.y);
}
void CTargetCrossControl::SetVertRange(int nY1, int nY2)
{
	m_ptVertRange.x = nY1;
	m_ptVertRange.y = nY2;
	InvalidateRect(NULL);
}
/////////////////////////////////////////////////////////////////////////////////////////
void CTargetCrossControl::SetZeroRange(int nRange)
{
	if (m_nZeroRange != nRange)
	{
		m_nZeroRange = nRange;
		InvalidateRect(NULL);
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
void CTargetCrossControl::SetPos(CPoint ptPos)
{
	if (m_ptPos != ptPos)
	{
		m_ptPos = ptPos;
		InvalidateRect(NULL);
	}
}
void CTargetCrossControl::SetPos(int nX, int nY)
{
	SetPos(CPoint(nX, nY));
}
/////////////////////////////////////////////////////////////////////////////////////////
CPoint CTargetCrossControl::TransformPoint(CRect& rc, CPoint pt)
{
	pt.x = (rc.Width()  >> 1) + MulDiv(pt.x, rc.Width(), m_ptHorzRange.y - m_ptHorzRange.x);
	pt.y = (rc.Height() >> 1) + MulDiv(pt.y, rc.Height(), m_ptVertRange.y - m_ptVertRange.x);
	return pt;
}

/////////////////////////////////////////////////////////////////////////////
// CJoystickPage dialog
IMPLEMENT_DYNAMIC(CJoystickPage, CSVPage)
CJoystickPage::CJoystickPage(CSVView* pParent /*=NULL*/)
	: CSVPage(CJoystickPage::IDD)
{
	m_pParent   = pParent;
	m_nJoyTimer = 0;
	m_pJoyStick = NULL;
	m_pJoyState	= NULL;
	m_nZeroRange = 2;
	m_bControlSpeed = TRUE;
	m_pGameControl = NULL;

	m_bLearnMode = -1;
	OnBnClickedJpBtnReset();

	Create(IDD,pParent);
}
/////////////////////////////////////////////////////////////////////////////
CJoystickPage::~CJoystickPage()
{
}
/////////////////////////////////////////////////////////////////////////////
void CJoystickPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLanguagePage)
	DDX_Text(pDX, IDC_JP_EDT_ZERO_RANGE, m_nZeroRange);
	DDV_MinMaxInt(pDX, m_nZeroRange, 1, 10);
	DDX_Control(pDX, IDC_JP_CTRL_JOY_XY_AXIS, m_ctrlJoyAxis);
	DDX_Check(pDX, IDC_JP_CK_CONTROL_SPEED, m_bControlSpeed);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CJoystickPage, CDialog)
	//{{AFX_MSG_MAP(CLanguagePage)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_JP_BTN_CHOOSE_JOYSTICK, OnBnClickedJpBtnChooseJoystick)
	ON_BN_CLICKED(IDC_JP_BTN_RESET, OnBnClickedJpBtnReset)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_JP_CK_BTN_1, IDC_JP_CK_BTN_32, OnCkBtn)
	ON_EN_CHANGE(IDC_JP_EDT_ZERO_RANGE, OnEnChangeJpEdtZeroRange)
	ON_BN_CLICKED(IDC_JP_CK_CONTROL_SPEED, OnBnClickedJpCkControlSpeed)
	ON_WM_DEVICECHANGE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CJoystickPage message handlers
/////////////////////////////////////////////////////////////////////////////
BOOL CJoystickPage::OnInitDialog()
{
	CSVPage::OnInitDialog();

	m_pGameControl = new CWK_CPLApplet(m_hWnd, _T("joy.cpl"));
	HICON hIcon = m_pGameControl->GetIcon(0);
	if (hIcon)
	{
		GetDlgItem(IDC_JP_BTN_CHOOSE_JOYSTICK)->SendMessage(BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIcon);
	}
	else
	{
		GetDlgItem(IDC_JP_BTN_CHOOSE_JOYSTICK)->EnableWindow(FALSE);
	}

	CString str;
	str.LoadString(IDS_IRIS_OPEN);
	GetDlgItem(IDC_JP_CK_BTN_5)->SetWindowText(str);
	str.LoadString(IDS_IRIS_CLOSE);
	GetDlgItem(IDC_JP_CK_BTN_6)->SetWindowText(str);
	str.LoadString(IDS_IRIS_AUTO);
	GetDlgItem(IDC_JP_CK_BTN_8)->SetWindowText(str);

	str.LoadString(IDS_FOCUS_FAR);
	GetDlgItem(IDC_JP_CK_BTN_3)->SetWindowText(str);
	str.LoadString(IDS_FOCUS_NEAR);
	GetDlgItem(IDC_JP_CK_BTN_4)->SetWindowText(str);
	str.LoadString(IDS_FOCUS_AUTO);
	GetDlgItem(IDC_JP_CK_BTN_7)->SetWindowText(str);

	str.LoadString(IDS_ZOOM_IN);
	GetDlgItem(IDC_JP_CK_BTN_1)->SetWindowText(str);
	str.LoadString(IDS_ZOOM_OUT);
	GetDlgItem(IDC_JP_CK_BTN_2)->SetWindowText(str);
	str.LoadString(IDS_TURN_180);
	GetDlgItem(IDC_JP_CK_BTN_9)->SetWindowText(str);

	int i, nTT=0, nSize;
	nSize = m_sTTArray.GetCount();
	str.LoadString(IDC_JP_CK_BTN_1);
	for (i=0; i<nSize; i++)
	{
		if (str == m_sTTArray.GetAt(i))
		{
			nTT = i;
			break;
		}
	}
	const CString &sTip = m_sTTArray.GetAt(nTT);
	for (i=1; i<MAX_JOYSTICK_BUTTONS; i++)
	{
		m_ToolTip.AddTool(GetDlgItem(IDC_JP_CK_BTN_1+i), sTip);
	}

	Initialize();	// lädt daten aus der registry
	InitJoystick();

	RegisterDeviceDetect(m_hWnd, GUID_DEVINTCLASS_HUDINT);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CJoystickPage::Initialize()
{
	CWK_Profile&wkp = GetProfile();
	UINT nSize = 0;
	BYTE *pData = NULL;
	if (wkp.GetBinary(SECTION_COMMON, COMMON_JOYSTICK_BUTTONS, (LPBYTE*)&pData, &nSize))
	{
		CopyMemory((void*)&m_nButtonMap[0], pData, sizeof(m_nButtonMap));
	}
	else
	{
		OnBnClickedJpBtnReset();
	}
	WK_DELETE(pData);
	m_nZeroRange = wkp.GetInt(SECTION_COMMON, COMMON_JOYSTICK_ZERO_RANGE, m_nZeroRange);
	m_bControlSpeed = wkp.GetInt(SECTION_COMMON, COMMON_JOYSTICK_CONTROLSPEED, m_bControlSpeed);
	if (m_hWnd)
	{
		UpdateData(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CJoystickPage::SaveChanges()
{
	CWK_Profile&wkp = GetProfile();
	wkp.WriteBinary(SECTION_COMMON, COMMON_JOYSTICK_BUTTONS, (LPBYTE)&m_nButtonMap[0], sizeof(m_nButtonMap));
	wkp.WriteInt(SECTION_COMMON, COMMON_JOYSTICK_ZERO_RANGE, m_nZeroRange);
	wkp.WriteInt(SECTION_COMMON, COMMON_JOYSTICK_CONTROLSPEED, m_bControlSpeed);
}
/////////////////////////////////////////////////////////////////////////////
void CJoystickPage::CancelChanges()
{
	Initialize();
}
/////////////////////////////////////////////////////////////////////////////
void CJoystickPage::OnTimer(UINT nIDEvent)
{
	if (   nIDEvent == m_nJoyTimer
	     && m_pJoyStick && m_pJoyState	// joystick available
	     && SUCCEEDED(m_pJoyStick->UpdateInputState(m_pJoyState, NULL)))
	{
		CDataExchange dx(this, FALSE);
		DWORD dwCaps = m_pJoyStick->GetCapabilities();
		if (dwCaps & JOY_CAPS_XAXIS && dwCaps & JOY_CAPS_YAXIS)
		{
			if (!m_bControlSpeed)
			{
				int nRange = 5 + m_nZeroRange;
				if (m_pJoyState->lX < -m_nZeroRange)
				{
					m_pJoyState->lX = -nRange;
				}
				if (m_pJoyState->lX > m_nZeroRange)
				{
					m_pJoyState->lX = nRange;
				}
				if (m_pJoyState->lY < -m_nZeroRange)
				{
					m_pJoyState->lY = -nRange;
				}
				if (m_pJoyState->lY > m_nZeroRange)
				{
					m_pJoyState->lY = nRange;
				}
			}
			m_ctrlJoyAxis.SetPos(m_pJoyState->lX, m_pJoyState->lY);
		}

		if (dwCaps & JOY_CAPS_RZAXIS)
		{
			BOOL bCheck;
			bCheck = m_pJoyState->lRz > m_nZeroRange;
			DDX_Check(&dx, IDC_JP_CK_BTN_1, bCheck);
			bCheck = m_pJoyState->lRz < -m_nZeroRange;
			DDX_Check(&dx, IDC_JP_CK_BTN_2, bCheck);
		}

		int i, n, nBtn = min(MAX_JOYSTICK_BUTTONS, m_pJoyStick->GetNoOfButtons());
		if (m_bLearnMode != -1)
		{
			for (i=0; i<nBtn; i++)
			{
				if (m_pJoyState->rgbButtons[i] != 0)
				{
					m_nButtonMap[i] = m_bLearnMode;
					if (m_nButtonMap[m_bLearnMode] == m_bLearnMode)
					{
						m_nButtonMap[m_bLearnMode] = i;
					}
					n = FALSE;
					DDX_Check(&dx, IDC_JP_CK_BTN_1+m_bLearnMode, n);
					m_bLearnMode = -1;
					SetModified();
					m_pParent->AddInitApp(WK_APP_NAME_IDIP_CLIENT);
					break;
				}
			}
		}
		else
		{
			for (i=0; i<nBtn; i++)
			{
				n = m_pJoyState->rgbButtons[i] != 0 ? 1 : 0;
				DDX_Check(&dx, IDC_JP_CK_BTN_1+m_nButtonMap[i], n);
			}
		}
	}
	CSVPage::OnTimer(nIDEvent);
}

/////////////////////////////////////////////////////////////////////////////
void CJoystickPage::InitJoystick()
{
	if (m_pJoyStick)
	{
		FreeJoystick();
	}
	m_pJoyStick = new CJoyStickDX;
	int nRange = 10 + m_nZeroRange;
	m_pJoyStick->SetAxisRange(-nRange, nRange);
	m_ctrlJoyAxis.SetZeroRange(m_nZeroRange);
	m_ctrlJoyAxis.SetHorzRange(-nRange, nRange);
	m_ctrlJoyAxis.SetVertRange(-nRange, nRange);
	if (S_OK == m_pJoyStick->Init(AfxGetMainWnd()->m_hWnd, DISCL_EXCLUSIVE|DISCL_FOREGROUND, 0))
	{
		CWnd *pWnd = GetDlgItem(IDC_JP_TXT_JOYSTICK_NAME);
		if (pWnd)
		{
			CString sText;
			pWnd->GetWindowText(sText);
			if (!sText.IsEmpty())
			{
				if (sText != m_pJoyStick->GetDeviceName())
				{
					SetModified();
					m_pParent->AddInitApp(WK_APP_NAME_IDIP_CLIENT);
					OnBnClickedJpBtnReset();
				}
			}
			pWnd->SetWindowText(m_pJoyStick->GetDeviceName());
		}
		m_pJoyState = new DIJOYSTATE;
		ZeroMemory(m_pJoyState, sizeof(DIJOYSTATE));
		int i;
		for (i=0; i<MAX_JOYSTICK_BUTTONS; i++)
		{
			GetDlgItem(IDC_JP_CK_BTN_1+i)->EnableWindow(TRUE);
		}
		GetDlgItem(IDC_JP_BTN_RESET)->EnableWindow(TRUE);
		m_nJoyTimer = SetTimer(0x0815, 100, NULL);
	}
	else
	{
		GetDlgItem(IDC_JP_TXT_JOYSTICK_NAME)->SetWindowText(_T(""));
		FreeJoystick();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CJoystickPage::FreeJoystick()
{
	int i;
	for (i=0; i<MAX_JOYSTICK_BUTTONS; i++)
	{
		GetDlgItem(IDC_JP_CK_BTN_1+i)->EnableWindow(FALSE);
	}
	GetDlgItem(IDC_JP_BTN_RESET)->EnableWindow(FALSE);
	if (m_nJoyTimer)
	{
		KillTimer(m_nJoyTimer);
		m_nJoyTimer = 0;
	}
	WK_DELETE(m_pJoyState);
	WK_DELETE(m_pJoyStick);
}
/////////////////////////////////////////////////////////////////////////////
void CJoystickPage::OnDestroy()
{
	WK_DELETE(m_pGameControl);
	FreeJoystick();
	CSVPage::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
void CJoystickPage::OnBnClickedJpBtnChooseJoystick()
{
	FreeJoystick();
	m_pGameControl->Invoke(AfxGetMainWnd()->m_hWnd, 0);
	InitJoystick();
}
/////////////////////////////////////////////////////////////////////////////
void CJoystickPage::OnBnClickedJpBtnReset()
{
	int i;
	BOOL bModfied = FALSE;
	for (i=0; i<MAX_JOYSTICK_BUTTONS; i++)
	{
		if (m_nButtonMap[i] != i)
		{
			bModfied = TRUE;
		}
		m_nButtonMap[i] = i;
	}
	if (m_pJoyStick)
	{
		DWORD dwCaps = m_pJoyStick->GetCapabilities();
		if (dwCaps & JOY_CAPS_RZAXIS)
		{
			int nNoOfBtn = m_pJoyStick->GetNoOfButtons();
			if (nNoOfBtn < MAX_JOYSTICK_BUTTONS-2)
			{
				m_nButtonMap[nNoOfBtn] = 0;
				m_nButtonMap[0] = nNoOfBtn;
				m_nButtonMap[nNoOfBtn+1] = 1;
				m_nButtonMap[1] = nNoOfBtn+1;
			}
		}
	}
	if (m_hWnd)
	{
		if (bModfied)
		{
			SetModified();
			m_pParent->AddInitApp(WK_APP_NAME_IDIP_CLIENT);
		}
		if (m_bLearnMode != -1)
		{
			CDataExchange dx(this, FALSE);
			i = FALSE;
			DDX_Check(&dx, IDC_JP_CK_BTN_1+m_bLearnMode, i);
			m_bLearnMode = -1;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CJoystickPage::OnCkBtn(UINT nID)
{
	if (m_pJoyStick)
	{
		int i, n;
		CDataExchange dx(this, TRUE);
		DDX_Check(&dx, nID, n);
		dx.m_bSaveAndValidate = FALSE;
		if (n)
		{
			UINT nCurr;
			n = FALSE;
			for (i=0; i<MAX_JOYSTICK_BUTTONS; i++)
			{
				nCurr = (IDC_JP_CK_BTN_1+i);
				if (nCurr != nID)
				{
					DDX_Check(&dx, nCurr, n);
				}
			}

			m_bLearnMode =  nID - IDC_JP_CK_BTN_1;
			for (i=0; i<m_pJoyStick->GetNoOfButtons(); i++)
			{
				if (m_nButtonMap[i] != i)
				{
					if (m_nButtonMap[i] == m_bLearnMode)
					{
						m_nButtonMap[i] = i;
					}
				}
			}

			for (i=0; i<m_pJoyStick->GetNoOfButtons(); i++)
			{
				n = (IDC_JP_CK_BTN_1+i) == (int)nID;
				DDX_Check(&dx, IDC_JP_CK_BTN_1+i, n);
			}
		}
		else
		{
			m_bLearnMode = -1;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CJoystickPage::OnEnChangeJpEdtZeroRange()
{
	int nOld = m_nZeroRange;
	m_nZeroRange = GetDlgItemInt(IDC_JP_EDT_ZERO_RANGE);
	if (IsBetween(m_nZeroRange, 1, 10) && nOld != m_nZeroRange)
	{
		m_ctrlJoyAxis.SetZeroRange(m_nZeroRange);
		InitJoystick();
		SetModified();
		m_pParent->AddInitApp(WK_APP_NAME_IDIP_CLIENT);
	}
	else
	{
		m_nZeroRange = nOld;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CJoystickPage::OnBnClickedJpCkControlSpeed()
{
	CDataExchange dx(this, TRUE);
	DDX_Check(&dx, IDC_JP_CK_CONTROL_SPEED, m_bControlSpeed);
	SetModified();
	m_pParent->AddInitApp(WK_APP_NAME_IDIP_CLIENT);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CJoystickPage::OnDeviceChange(UINT nEventType, DWORD dwData )
{
	DEV_BROADCAST_HDR* pHdr = (DEV_BROADCAST_HDR*)dwData;
	if (pHdr)
	{
		if (pHdr->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE_EX)
		{
			CDevIntEx dix((DEV_BROADCAST_DEVICEINTERFACE_EX*)pHdr);
			if (dix.GetGuidDevClass() == GUID_DEVCLASS_HID)
			{
				if (   nEventType == DBT_DEVICEARRIVAL
					|| nEventType == DBT_DEVICEREMOVECOMPLETE)
				{
					InitJoystick();
				}
			}
		}
	}
	return TRUE;
}
