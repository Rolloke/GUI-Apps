// CExportDlg.cpp : implementation file
//

#include "stdafx.h"
#include "dvclient.h"
#include "CExportDlg.h"
#include "CPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExportDlg dialog


/////////////////////////////////////////////////////////////////////////////
CExportDlg::CExportDlg(CDisplay* pDisplay, CPanel* pPanel /*=NULL*/)
	: CTransparentDialog(CExportDlg::IDD, (CWnd*)pPanel)
{
	//{{AFX_DATA_INIT(CExportDlg)
	//}}AFX_DATA_INIT

	m_BaseColor		= CPanel::m_BaseColorBackGrounds;
	m_pPanel		= pPanel;
	m_pDisplay		= pDisplay;
	m_hCursor		= NULL;
	m_sInput		= _T("");
	m_sFormat		= _T("");
	m_eInputState	= InputNo;
	m_sStartTime	= _T("");
	m_sEndTime		= _T("");
	m_dwCamMask		= 0;
	m_bDDMMYY		= TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CExportDlg::DoDataExchange(CDataExchange* pDX)
{
	CTransparentDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExportDlg)    
	DDX_Control(pDX, IDC_STATIC_EXPORT, m_ctrlStaticExport);
	DDX_Control(pDX, IDC_CLOCK, m_ctrlButtonTimer);
	DDX_Control(pDX, IDC_BORDER_EXPORT, m_ctrlBorder);
	DDX_Control(pDX, IDC_DISPLAY_EXPORT_TO, m_ctrlDisplayTo);
	DDX_Control(pDX, IDC_DISPLAY_EXPORT_FROM, m_ctrlDisplayFrom);
	DDX_Control(pDX, IDOK, m_ctrlButtonOk);
	DDX_Control(pDX, IDCANCEL, m_ctrlButtonCancel);
	DDX_Control(pDX, IDC_EXPORT_CAM9, m_ctrlButtonCam9);
	DDX_Control(pDX, IDC_EXPORT_CAM8, m_ctrlButtonCam8);
	DDX_Control(pDX, IDC_EXPORT_CAM7, m_ctrlButtonCam7);
	DDX_Control(pDX, IDC_EXPORT_CAM6, m_ctrlButtonCam6);
	DDX_Control(pDX, IDC_EXPORT_CAM4, m_ctrlButtonCam4);
	DDX_Control(pDX, IDC_EXPORT_CAM5, m_ctrlButtonCam5);
	DDX_Control(pDX, IDC_EXPORT_CAM3, m_ctrlButtonCam3);
	DDX_Control(pDX, IDC_EXPORT_CAM2, m_ctrlButtonCam2);
	DDX_Control(pDX, IDC_EXPORT_CAM16, m_ctrlButtonCam16);
	DDX_Control(pDX, IDC_EXPORT_CAM15, m_ctrlButtonCam15);
	DDX_Control(pDX, IDC_EXPORT_CAM14, m_ctrlButtonCam14);
	DDX_Control(pDX, IDC_EXPORT_CAM13, m_ctrlButtonCam13);
	DDX_Control(pDX, IDC_EXPORT_CAM12, m_ctrlButtonCam12);
	DDX_Control(pDX, IDC_EXPORT_CAM11, m_ctrlButtonCam11);
	DDX_Control(pDX, IDC_EXPORT_CAM10, m_ctrlButtonCam10);
	DDX_Control(pDX, IDC_EXPORT_CAM1, m_ctrlButtonCam1);
	DDX_Control(pDX, IDC_BUTTON_NUM9, m_ctrlButtonNum9);
	DDX_Control(pDX, IDC_BUTTON_NUM8, m_ctrlButtonNum8);
	DDX_Control(pDX, IDC_BUTTON_NUM7, m_ctrlButtonNum7);
	DDX_Control(pDX, IDC_BUTTON_NUM6, m_ctrlButtonNum6);
	DDX_Control(pDX, IDC_BUTTON_NUM5, m_ctrlButtonNum5);
	DDX_Control(pDX, IDC_BUTTON_NUM4, m_ctrlButtonNum4);
	DDX_Control(pDX, IDC_BUTTON_NUM3, m_ctrlButtonNum3);
	DDX_Control(pDX, IDC_BUTTON_NUM2, m_ctrlButtonNum2);
	DDX_Control(pDX, IDC_BUTTON_NUM1, m_ctrlButtonNum1);
	DDX_Control(pDX, IDC_BUTTON_NUM0, m_ctrlButtonNum0);
	DDX_Control(pDX, IDC_BUTTON_CLEAR, m_ctrlButtonClear);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CExportDlg, CTransparentDialog)
	//{{AFX_MSG_MAP(CExportDlg)
	ON_BN_CLICKED(IDC_BUTTON_NUM0, OnButton0)
	ON_BN_CLICKED(IDC_BUTTON_NUM1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON_NUM2, OnButton2)
	ON_BN_CLICKED(IDC_BUTTON_NUM3, OnButton3)
	ON_BN_CLICKED(IDC_BUTTON_NUM4, OnButton4)
	ON_BN_CLICKED(IDC_BUTTON_NUM5, OnButton5)
	ON_BN_CLICKED(IDC_BUTTON_NUM6, OnButton6)
	ON_BN_CLICKED(IDC_BUTTON_NUM7, OnButton7)
	ON_BN_CLICKED(IDC_BUTTON_NUM8, OnButton8)
	ON_BN_CLICKED(IDC_BUTTON_NUM9, OnButton9)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, OnButtonClear)
	ON_WM_SETCURSOR()
	ON_BN_CLICKED(IDC_CLOCK, OnInputClock)
	ON_BN_CLICKED(IDC_EXPORT_CAM1, OnCheckExportCam)
	ON_BN_CLICKED(IDC_EXPORT_CAM2, OnCheckExportCam)
	ON_BN_CLICKED(IDC_EXPORT_CAM3, OnCheckExportCam)
	ON_BN_CLICKED(IDC_EXPORT_CAM4, OnCheckExportCam)
	ON_BN_CLICKED(IDC_EXPORT_CAM5, OnCheckExportCam)
	ON_BN_CLICKED(IDC_EXPORT_CAM6, OnCheckExportCam)
	ON_BN_CLICKED(IDC_EXPORT_CAM7, OnCheckExportCam)
	ON_BN_CLICKED(IDC_EXPORT_CAM8, OnCheckExportCam)
	ON_BN_CLICKED(IDC_EXPORT_CAM9, OnCheckExportCam)
	ON_BN_CLICKED(IDC_EXPORT_CAM10, OnCheckExportCam)
	ON_BN_CLICKED(IDC_EXPORT_CAM11, OnCheckExportCam)
	ON_BN_CLICKED(IDC_EXPORT_CAM12, OnCheckExportCam)
	ON_BN_CLICKED(IDC_EXPORT_CAM13, OnCheckExportCam)
	ON_BN_CLICKED(IDC_EXPORT_CAM14, OnCheckExportCam)
	ON_BN_CLICKED(IDC_EXPORT_CAM15, OnCheckExportCam)
	ON_BN_CLICKED(IDC_EXPORT_CAM16, OnCheckExportCam)
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
BOOL CExportDlg::OnInitDialog() 
{
	CTransparentDialog::OnInitDialog();
	
	CreateTransparent(StyleBackGroundColorChangeBrushed, CPanel::m_BaseColorBackGrounds);

	CString sMsg;
	sMsg.LoadString(IDC_STATIC_EXPORT);
	m_ctrlStaticExport.SetDisplayText(sMsg);

	m_NumButtons.Add(&m_ctrlButtonNum0);
	m_NumButtons.Add(&m_ctrlButtonNum1);
	m_NumButtons.Add(&m_ctrlButtonNum2);
	m_NumButtons.Add(&m_ctrlButtonNum3);
	m_NumButtons.Add(&m_ctrlButtonNum4);
	m_NumButtons.Add(&m_ctrlButtonNum5);
	m_NumButtons.Add(&m_ctrlButtonNum6);
	m_NumButtons.Add(&m_ctrlButtonNum7);
	m_NumButtons.Add(&m_ctrlButtonNum8);
	m_NumButtons.Add(&m_ctrlButtonNum9);
	m_NumButtons.Add(&m_ctrlButtonClear);
	m_NumButtons.SetShape(ShapeRound);
	m_NumButtons.SetBaseColor(m_BaseColor);

	//------------------------------------------------------------
	m_CamButtons.Add(&m_ctrlButtonCam1);
	m_CamButtons.Add(&m_ctrlButtonCam2);
	m_CamButtons.Add(&m_ctrlButtonCam3);
	m_CamButtons.Add(&m_ctrlButtonCam4);
	m_CamButtons.Add(&m_ctrlButtonCam5);
	m_CamButtons.Add(&m_ctrlButtonCam6);
	m_CamButtons.Add(&m_ctrlButtonCam7);
	m_CamButtons.Add(&m_ctrlButtonCam8);
	m_CamButtons.Add(&m_ctrlButtonCam9);
	m_CamButtons.Add(&m_ctrlButtonCam10);
	m_CamButtons.Add(&m_ctrlButtonCam11);
	m_CamButtons.Add(&m_ctrlButtonCam12);
	m_CamButtons.Add(&m_ctrlButtonCam13);
	m_CamButtons.Add(&m_ctrlButtonCam14);
	m_CamButtons.Add(&m_ctrlButtonCam15);
	m_CamButtons.Add(&m_ctrlButtonCam16);
	m_CamButtons.SetBaseColor(m_BaseColor);
	m_CamButtons.SetStyle(StyleCheck);

	//------------------------------------------------------------
	m_ctrlButtonOk.SetBaseColor(m_BaseColor);
	m_ctrlButtonCancel.SetBaseColor(m_BaseColor);
 	m_ctrlButtonTimer.SetBaseColor(m_BaseColor);

	m_ilOK.Create(IDB_CONFIRM, 16, 0, SKIN_COLOR_KEY);
	m_ctrlButtonOk.SetImageList(&m_ilOK);

	m_ilCancel.Create(IDB_DEL, 16, 0, SKIN_COLOR_KEY);
	m_ctrlButtonCancel.SetImageList(&m_ilCancel);

	m_ilTimer.Create(IDB_CLOCK, 16, 0, SKIN_COLOR_KEY);
	m_ctrlButtonTimer.SetImageList(&m_ilTimer);

	m_ctrlButtonOk.EnableWindow(FALSE);

	//------------------------------------------------------------
	m_ctrlBorder.CreateFrame(this);
	m_ctrlBorder.EnableShadow(TRUE);

	m_ctrlDisplayFrom.OpenStockDisplay(this, Display2);
	m_ctrlDisplayTo.OpenStockDisplay(this, Display2);

	m_ctrlStaticExport.OpenStockDisplay(this, StaticGrey);

	
	// Kontrast erhöhen
	DSPCOL DisplayColors;
	DisplayColors.colText = SKIN_COLOR_BLACK;
	DisplayColors.dwFlags = DSP_COL_TEXT;
	m_ctrlDisplayFrom.SetDisplayColors(DisplayColors);
	m_ctrlDisplayFrom.SetTextAllignment(DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	m_ctrlDisplayTo.SetDisplayColors(DisplayColors);
	m_ctrlDisplayTo.SetTextAllignment(DT_LEFT|DT_VCENTER|DT_SINGLELINE);

	m_hCursor = theApp.m_hArrow;

	if (m_pPanel)
	{
		for (int nCam = 0; nCam < 16; nCam++)
			m_CamButtons.GetAtFast(nCam)->EnableWindow((m_pPanel->ExistPlayCamera(nCam)!= NULL));

		CRect rcPanel;
		m_pPanel->GetWindowRect(rcPanel);

		CRect rcKeyboard;
		GetWindowRect(rcKeyboard);
		
		// Keyboard oberhalb des Panels positionieren
		int nX	= max(rcPanel.left, 0);
		int	nY	= rcPanel.top - rcKeyboard.Height();

		if (nX < 0 || nY < 0)
		{
			// Keyboard unterhalb des Panels positionieren
			nX	= rcPanel.left;
			nY	= rcPanel.bottom;
		}
		SetWindowPos(&wndTopMost, nX, nY, -1, -1, SWP_NOSIZE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
void CExportDlg::OnButton0() 
{
	OnButtonPressed(_T("0"));	
}

/////////////////////////////////////////////////////////////////////////////
void CExportDlg::OnButton1() 
{
	OnButtonPressed(_T("1"));	
}

/////////////////////////////////////////////////////////////////////////////
void CExportDlg::OnButton2() 
{
	OnButtonPressed(_T("2"));	
}

/////////////////////////////////////////////////////////////////////////////
void CExportDlg::OnButton3() 
{
	OnButtonPressed(_T("3"));	
}

/////////////////////////////////////////////////////////////////////////////
void CExportDlg::OnButton4() 
{
	OnButtonPressed(_T("4"));	
}

/////////////////////////////////////////////////////////////////////////////
void CExportDlg::OnButton5() 
{
	OnButtonPressed(_T("5"));	
}

/////////////////////////////////////////////////////////////////////////////
void CExportDlg::OnButton6() 
{
	OnButtonPressed(_T("6"));	
}

/////////////////////////////////////////////////////////////////////////////
void CExportDlg::OnButton7() 
{
	OnButtonPressed(_T("7"));	
}

/////////////////////////////////////////////////////////////////////////////
void CExportDlg::OnButton8() 
{
	OnButtonPressed(_T("8"));	
}

/////////////////////////////////////////////////////////////////////////////
void CExportDlg::OnButton9() 
{
	OnButtonPressed(_T("9"));	
}

/////////////////////////////////////////////////////////////////////////////
void CExportDlg::OnButtonPressed(const CString &sCharacter)
{
	CString sMsg;
	CString sChar = sCharacter;

	if (m_eInputState == InputNo)
		return;

	m_sInput += sChar;
	m_sInput = m_sInput.Left(12);

	// Eingabe des Startzeitpunktes
	if (m_eInputState == InputExportTimeFrom)
		m_ctrlDisplayFrom.SetFormatDisplayText(m_sInput, m_sFormat);

	// Eingabe des Endzeitpunktes
	if (m_eInputState == InputExportTimeTo)
		m_ctrlDisplayTo.SetFormatDisplayText(m_sInput, m_sFormat);
}

/////////////////////////////////////////////////////////////////////////////
void CExportDlg::OnButtonClear()
{
	m_sInput = _T("");
	if (m_eInputState == InputExportTimeFrom)
		m_ctrlDisplayFrom.SetFormatDisplayText(m_sInput, m_sFormat);
	if (m_eInputState == InputExportTimeTo)
		m_ctrlDisplayTo.SetFormatDisplayText(m_sInput, m_sFormat);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CExportDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (m_hCursor)
	{
		if (SetCursor(m_hCursor) != NULL)
			return TRUE;		
	}	
	
	return CTransparentDialog::OnSetCursor(pWnd, nHitTest, message);
}

/////////////////////////////////////////////////////////////////////////////
void CExportDlg::OnInputClock() 
{
	CString	sMsg;

	if (m_eInputState == InputNo)
	{
		m_eInputState	= InputExportTimeFrom;
		m_sFormat.LoadString(IDS_EXPORT_STARTTIME_FORMAT);
		m_sInput		= _T("");
		sMsg.LoadString(IDS_EXPORT_INPUT_STARTTIME);
		m_ctrlDisplayFrom.SetDisplayText(sMsg);
		Sleep(1000);
		m_ctrlDisplayFrom.EnableBlinking(500);
		m_ctrlDisplayFrom.SetFormatDisplayText(_T("ttmmjjhhmmss"), m_sFormat);
		m_ctrlButtonOk.EnableWindow(FALSE);
	}
	else if (m_eInputState == InputExportTimeFrom)
	{
		if (ValidateDateTime(m_sInput, m_TimeFrom))
		{
			sMsg.LoadString(IDS_EXPORT_TIME_OK);
			m_ctrlDisplayFrom.SetDisplayText(sMsg);
			Sleep(1000);
			m_ctrlDisplayFrom.SetFormatDisplayText(m_sInput, m_sFormat);

			m_sStartTime	= m_sInput;
			m_eInputState	= InputExportTimeTo;

			m_sFormat.LoadString(IDS_EXPORT_ENDTIME_FORMAT);
			m_sInput		= _T("");
			m_ctrlDisplayFrom.DisableBlinking();
			m_ctrlDisplayTo.EnableBlinking(500);
			sMsg.LoadString(IDS_EXPORT_INPUT_ENDTIME);
			m_ctrlDisplayTo.SetDisplayText(sMsg);
			Sleep(1000);
			m_ctrlDisplayTo.SetFormatDisplayText(_T("ttmmjjhhmmss"), m_sFormat);
		}
		else
		{
			sMsg.LoadString(IDS_EXPORT_TIME_BAD);
			m_ctrlDisplayFrom.SetDisplayText(sMsg);
			Sleep(1000);
			m_ctrlDisplayFrom.SetFormatDisplayText(_T("ttmmjjhhmmss"), m_sFormat);
			m_sInput		= _T("");
		}
	}
	else if (m_eInputState == InputExportTimeTo)
	{
		if (ValidateDateTime(m_sInput, m_TimeTo) && m_TimeTo >= m_TimeFrom)
		{
			sMsg.LoadString(IDS_EXPORT_TIME_OK);
			m_ctrlDisplayTo.SetDisplayText(sMsg);
			Sleep(1000);
			m_ctrlDisplayTo.SetFormatDisplayText(m_sInput, m_sFormat);

			m_sEndTime		= m_sInput;
			m_eInputState	= InputNo;
			m_sFormat		= _T("");
			m_sInput		= _T("");
			m_ctrlDisplayTo.DisableBlinking();
			OnCheckExportCam();
		}
		else
		{
			sMsg.LoadString(IDS_EXPORT_TIME_BAD);
			m_ctrlDisplayTo.SetDisplayText(sMsg);
			Sleep(1000);
			m_ctrlDisplayTo.SetFormatDisplayText(_T("ttmmjjhhmmss"), m_sFormat);
			m_sInput		= _T("");
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CExportDlg::OnCheckExportCam() 
{
	m_dwCamMask = 0;
	DWORD dwBit;
	int nCam;
	for (nCam=0,dwBit=1; nCam < 16; nCam++,dwBit<<=1)
	{
		if (m_CamButtons.GetAtFast(nCam)->GetCheck())
		{
			TRACE(_T("backup %d on\n"),nCam);
			m_dwCamMask |= dwBit;	
		}
	}

	TRACE(_T("OnCheckExportCam %08lx, %s, %s\n"),m_dwCamMask,m_sStartTime,m_sEndTime);
	m_ctrlButtonOk.EnableWindow(   m_dwCamMask != 0 
								&& !m_sStartTime.IsEmpty() 
								&& !m_sEndTime.IsEmpty());
}

/////////////////////////////////////////////////////////////////////////////
BOOL CExportDlg::ValidateDateTime(CString &sDateTime, CTime &Time)
{
	int nDay, nMonth, nYear, nSecond, nMinute, nHour;

	if (sDateTime.IsEmpty())
	{
		CSystemTime t;
		t.GetLocalTime();
		sDateTime.Format(_T("%02d%02d%02d%02d%02d%02d"), t.GetDay(), t.GetMonth(), t.GetYear() % 100, t.GetHour(), t.GetMinute(), t.GetSecond());
	}
	else if (sDateTime.GetLength() < 12)
	{
		WK_TRACE_WARNING(_T("Date too short\n"));
		return FALSE;
	}

	if (m_bDDMMYY)
	{
		nDay	= _ttoi(sDateTime.Mid(0,2));
		nMonth	= _ttoi(sDateTime.Mid(2,2));
		nYear	= _ttoi(sDateTime.Mid(4,2));
	}
	else
	{
		nMonth	= _ttoi(sDateTime.Mid(0,2));
		nDay	= _ttoi(sDateTime.Mid(2,2));
		nYear	= _ttoi(sDateTime.Mid(4,2));
	}

	if (nYear >= 99)
		nYear += 1900;
	else
		nYear += 2000;

	if ((nDay	>= 32)	|| (nDay	<= 0)	||
		(nMonth	>= 13)	|| (nMonth	<= 0)	||
		(nYear	>= 2039)|| (nYear	<= 1969))
	{
		WK_TRACE_WARNING(_T("Wrong Date\n"));
		return FALSE;
	}

	nHour	= _ttoi(sDateTime.Mid(6,2));
	nMinute	= _ttoi(sDateTime.Mid(8,2));
	nSecond	= _ttoi(sDateTime.Mid(10,2));

	if ((nHour		< 0) || (nHour > 23)	||
		(nMinute	< 0) || (nMinute > 59)	||
		(nSecond	< 0) || (nSecond > 59))
	{
		WK_TRACE_WARNING(_T("Wrong Time\n"));
		return FALSE;
	}

	CTime time(nYear, nMonth, nDay, nHour, nMinute, nSecond);
	Time = time;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CExportDlg::GetExportData(CString &sStart, CString &sEnd, DWORD &dwCamMask)
{
	sStart		= m_sStartTime;
	sEnd		= m_sEndTime;
	dwCamMask	= m_dwCamMask;
}

/////////////////////////////////////////////////////////////////////////////
void CExportDlg::GetExportData(CTime &TimeFrom, CTime &TimeTo, DWORD &dwCamMask)
{
	TimeFrom	= m_TimeFrom;
	TimeTo		= m_TimeTo;
	dwCamMask	= m_dwCamMask;
}

void CExportDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	CPoint CurPoint(0,0);

	GetCursorPos(&CurPoint);
	CWnd* pWnd = WindowFromPoint(CurPoint);
	CString s;
	if (WK_IS_WINDOW(pWnd))
	{
		int nID = pWnd->GetDlgCtrlID();
		if ((nID >= IDC_EXPORT_CAM1) && (nID <= IDC_EXPORT_CAM16))
			nID = IDC_EXPORT_CAM1;
		s.LoadString(nID);
	}
	if (m_pPanel)
		((CPanel*)m_pPanel)->SetTooltipText(s);
	
	CTransparentDialog::OnMouseMove(nFlags, point);
}
