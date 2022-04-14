// SimUnitDlg.cpp : implementation file
// w+k Sample Application
// Author : Uwe Freiwald
//
// CSimUnitDlg is the application main window class, which performs
// all UI operations
// The dialog contains all controls for Input and Output to
// Security ,two Edit Controls of IO history, 4 alarm state check
// buttons and 4 relay controls, which should illustrate relays.
//
// It's implemented as a modeless dialog box, for further information
// see Win SDK documentation.
//
// This file further contains AboutBox's code.
//

#include "stdafx.h"
#include "SimUnit.h"
#include "SimUnitDlg.h"

#include "CIPCInputSimUnit.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CSimUnitApp theApp;

#define MAX_HISTORY_LINES	50

#define FAR_FUTURE CSystemTime(1,1,2038,0,0,0,0)

/////////////////////////////////////////////////////////////////////////////
// CSimUnitDlg dialog
/////////////////////////////////////////////////////////////////////////////
// Constructor
CSimUnitDlg::CSimUnitDlg(CSimUnitApp* pApp)
	: CSkinDialog(CSimUnitDlg::IDD, NULL)
{
	//{{AFX_DATA_INIT(CSimUnitDlg)
	m_iRadioInterval = INTERVAL_REGULAR;
	m_iRadioHold = HOLD_REGULAR;
	m_iRadioAlarm = ALARM_RANDOM;
	m_dwIntervalRegular = 1000;
	m_dwIntervalRandomMin = 1000;
	m_dwIntervalRandomMax = 3000;
	m_dwHoldRegular = 500;
	m_dwHoldRandomMin = 500;
	m_dwHoldRandomMax = 2000;
	//}}AFX_DATA_INIT

	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	
	// pointer to main CWinApp derived object
	m_pApp    = pApp;
	
	// load icons and bitmaps from resource
	m_hIcon   = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bmOpen  = LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_OPEN));
	m_bmClose = LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_CLOSE));

	m_dwRelayMask = 0;
	m_dwEnableMask = (1 << NO_OF_ALARM_BTN) - 1;
	m_dwEdgeMask = (1 << NO_OF_ALARM_BTN) - 1;

	m_iBigWidth = 0;
	m_iSmallWidth = 0;
	m_iRandomAlarm = 0;
	int i;
	for (i=0; i<NO_OF_ALARM_BTN; i++)
	{
		m_bAlarm[i] =0;
		m_stStartTime[i] = FAR_FUTURE;
		m_stEndTime[i].GetLocalTime();
		m_dwAlarmCounter[i] = 0;
	}
	m_nInitToolTips = TOOLTIPS_SIMPLE;

	// create dialog window
	Create(IDD);
	WK_TRACE(_T("End:CSimUnitDlg::CSimUnitDlg()\n"));
}
/////////////////////////////////////////////////////////////////////////////
// Destructor
CSimUnitDlg::~CSimUnitDlg()
{
	// Enable all bitmap memory
	DeleteObject(m_bmOpen);
	DeleteObject(m_bmClose);

	// terminate application
	PostQuitMessage(0);
}
/////////////////////////////////////////////////////////////////////////////
// last window message destroys MFC object
void CSimUnitDlg::PostNcDestroy() 
{
	CloseDebugger();
	delete this;
}
/////////////////////////////////////////////////////////////////////////////
void CSimUnitDlg::DDX_TextMS(CDataExchange* pDX, UINT nID, DWORD &dwValue)
{
	CString s;
	if (pDX->m_bSaveAndValidate)
	{
		float fValue;
		DDX_Text(pDX, nID, fValue);
		dwValue = (DWORD)((fValue * 1000.0f) + 0.5f);
	}
	else
	{
		float fValue = dwValue / 1000.0f;
		s.Format(_T("%.3f"), fValue);
		int n = s.GetLength();
		if (s.GetAt(n-1) == _T('0'))
		{
			s.SetAt(n-1, 0);
			if (s.GetAt(n-2) == _T('0'))
			{
				s.SetAt(n-2, 0);
			}
		}
		DDX_Text(pDX, nID, s);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSimUnitDlg::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSimUnitDlg)
	DDX_Control(pDX, IDC_LINE, m_staticLine);
	DDX_Control(pDX, IDC_EDIT_INTERVAL_RANDOM_MAX, m_editIntervalRandomMax);
	DDX_Control(pDX, IDC_EDIT_HOLD_RANDOM_MAX, m_editHoldRandomMax);
	DDX_Control(pDX, IDC_RADIO_ALARM_RANDOM, m_btnAlarmRandom);
	DDX_Control(pDX, IDC_RADIO_ALARM_REGULAR, m_btnAlarmRegular);
	DDX_Control(pDX, IDC_RADIO_ALARM_ALL, m_btnAlarmAll);
	DDX_Control(pDX, IDC_RADIO_INTERVAL_REGULAR, m_btnIntervalRegular);
	DDX_Control(pDX, IDC_RADIO_INTERVAL_RANDOM, m_btnIntervalRandom);
	DDX_Control(pDX, IDC_RADIO_HOLD_REGULAR, m_btnHoldRegular);
	DDX_Control(pDX, IDC_RADIO_HOLD_RANDOM, m_btnHoldRandom);
	DDX_Control(pDX, IDC_EDIT_INTERVAL_REGULAR, m_editIntervalRegular);
	DDX_Control(pDX, IDC_EDIT_INTERVAL_RANDOM_MIN, m_editIntervalRandomMin);
	DDX_Control(pDX, IDC_EDIT_HOLD_REGULAR, m_editHoldRegular);
	DDX_Control(pDX, IDC_EDIT_HOLD_RANDOM_MIN, m_editHoldRandomMin);
	DDX_Control(pDX, IDC_CHECK_RANDOM_ALARM, m_btnRandomAlarm);

	DDX_TextMS(pDX, IDC_EDIT_HOLD_REGULAR, m_dwHoldRegular);
	DDX_TextMS(pDX, IDC_EDIT_HOLD_RANDOM_MIN, m_dwHoldRandomMin);
	DDX_TextMS(pDX, IDC_EDIT_INTERVAL_RANDOM_MIN, m_dwIntervalRandomMin);
	DDX_TextMS(pDX, IDC_EDIT_INTERVAL_REGULAR, m_dwIntervalRegular);
	DDX_TextMS(pDX, IDC_EDIT_HOLD_RANDOM_MAX, m_dwHoldRandomMax);
	DDX_TextMS(pDX, IDC_EDIT_INTERVAL_RANDOM_MAX, m_dwIntervalRandomMax);

	DDX_Radio(pDX, IDC_RADIO_HOLD_REGULAR, m_iRadioHold);
	DDX_Radio(pDX, IDC_RADIO_INTERVAL_REGULAR, m_iRadioInterval);
	DDX_Radio(pDX, IDC_RADIO_ALARM_RANDOM, m_iRadioAlarm);
	//}}AFX_DATA_MAP
	int i;
	for (i=0; i<NO_OF_ALARM_BTN; i++)
	{
		DDX_Control(pDX, IDC_ALERT1+i, m_btnAlarm[i]);
		DDX_Check(pDX, IDC_ALERT1+i, m_bAlarm[i]);
	}
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CSimUnitDlg, CSkinDialog)
	//{{AFX_MSG_MAP(CSimUnitDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_ABOUT, OnAbout)
	ON_BN_CLICKED(IDC_CHECK_RANDOM_ALARM, OnCheckRandomAlarm)
	ON_BN_CLICKED(IDC_RADIO_ALARM_ALL, OnRadioAlarm)
	ON_BN_CLICKED(IDC_RADIO_HOLD_RANDOM, OnRadioHold)
	ON_BN_CLICKED(IDC_RADIO_INTERVAL_RANDOM, OnRadioInterval)
	ON_COMMAND(ID_ABOUT,	OnAbout)
	ON_BN_CLICKED(IDC_RADIO_ALARM_RANDOM, OnRadioAlarm)
	ON_BN_CLICKED(IDC_RADIO_ALARM_REGULAR, OnRadioAlarm)
	ON_BN_CLICKED(IDC_RADIO_HOLD_REGULAR, OnRadioHold)
	ON_BN_CLICKED(IDC_RADIO_INTERVAL_REGULAR, OnRadioInterval)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_ALERT1, IDC_ALERT16, OnAlarm)
	ON_MESSAGE(WM_SELFCHECK, DoConfirmSelfcheck)
	ON_MESSAGE(WM_LANGUAGE_CHANGED, OnLanguageChanged)	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
void CSimUnitDlg::LoadSettings() 
{
	CWK_Profile wkp;
	CString sSection = _T("SimUnit");
	m_iRandomAlarm = wkp.GetInt(sSection, _T("RandomAlarm"), m_iRandomAlarm);
	CString sText;
	switch (m_iRandomAlarm)
	{
	case 0:
		sText.LoadString(IDS_RANDOM_ALARMS);
		break;
	case 1:
		sText.LoadString(IDS_STOP_ALARMS);
		break;
	case 2:
		sText.LoadString(IDS_START_ALARMS);
		break;
	}
	m_btnRandomAlarm.SetWindowText(sText);
	m_iRadioInterval = wkp.GetInt(sSection, _T("RadioInterval"), m_iRadioInterval);
	m_iRadioHold = wkp.GetInt(sSection, _T("RadioHold"), m_iRadioHold);
	m_iRadioAlarm = wkp.GetInt(sSection, _T("RadioAlarm"), m_iRadioAlarm);
	m_dwIntervalRegular = wkp.GetInt(sSection, _T("IntervalRegular"), m_dwIntervalRegular);
	m_dwIntervalRandomMin = wkp.GetInt(sSection, _T("IntervalRandomMin"), m_dwIntervalRandomMin);
	m_dwIntervalRandomMax = wkp.GetInt(sSection, _T("IntervalRandomMax"), m_dwIntervalRandomMax);
	m_dwHoldRegular = wkp.GetInt(sSection, _T("HoldRegular"), m_dwHoldRegular);
	m_dwHoldRandomMin = wkp.GetInt(sSection, _T("HoldRandomMin"), m_dwHoldRandomMin);
	m_dwHoldRandomMax = wkp.GetInt(sSection, _T("HoldRandomMax"), m_dwHoldRandomMax);
	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CSimUnitDlg::SaveSettings() 
{
	UpdateData();
	CWK_Profile wkp;
	CString sSection = _T("SimUnit");
	wkp.WriteInt(sSection, _T("RandomAlarm"), m_iRandomAlarm);
	wkp.WriteInt(sSection, _T("RadioInterval"), m_iRadioInterval);
	wkp.WriteInt(sSection, _T("RadioHold"), m_iRadioHold);
	wkp.WriteInt(sSection, _T("RadioAlarm"), m_iRadioAlarm);
	wkp.WriteInt(sSection, _T("IntervalRegular"), m_dwIntervalRegular);
	wkp.WriteInt(sSection, _T("IntervalRandomMin"), m_dwIntervalRandomMin);
	wkp.WriteInt(sSection, _T("IntervalRandomMax"), m_dwIntervalRandomMax);
	wkp.WriteInt(sSection, _T("HoldRegular"), m_dwHoldRegular);
	wkp.WriteInt(sSection, _T("HoldRandomMin"), m_dwHoldRandomMin);
	wkp.WriteInt(sSection, _T("HoldRandomMax"), m_dwHoldRandomMax);
}
/////////////////////////////////////////////////////////////////////////////
void CSimUnitDlg::ShowHide() 
{
	UpdateData();
	CRect rc;
	GetWindowRect(rc);
	int nEnabledAlarms;
	if (m_iRandomAlarm == 0)
	{
		SetWindowPos(&wndTop, rc.left, rc.top, m_iSmallWidth, rc.Height(), SWP_NOMOVE|SWP_NOZORDER);
		nEnabledAlarms = EnableAlarmButtons(TRUE);
		// additional elements are hidden, nothing else to do
	}
	else
	{
		SetWindowPos(&wndTop, rc.left, rc.top, m_iBigWidth, rc.Height(), SWP_NOMOVE|SWP_NOZORDER);
		BOOL bEnable = (m_iRandomAlarm == 2);
		nEnabledAlarms = EnableAlarmButtons(TRUE);
		m_btnIntervalRegular.EnableWindow(bEnable);
		m_btnIntervalRandom.EnableWindow(bEnable);
		m_editIntervalRegular.EnableWindow(bEnable && (m_iRadioInterval==0));
		m_editIntervalRandomMin.EnableWindow(bEnable && (m_iRadioInterval==1));
		m_editIntervalRandomMax.EnableWindow(bEnable && (m_iRadioInterval==1));
		m_btnHoldRegular.EnableWindow(bEnable);
		m_btnHoldRandom.EnableWindow(bEnable);
		m_editHoldRegular.EnableWindow(bEnable && (m_iRadioHold==0));
		m_editHoldRandomMin.EnableWindow(bEnable && (m_iRadioHold==1));
		m_editHoldRandomMax.EnableWindow(bEnable && (m_iRadioHold==1));
		m_btnAlarmRandom.EnableWindow(bEnable && nEnabledAlarms > 1);
		m_btnAlarmRegular.EnableWindow(bEnable);
		m_btnAlarmAll.EnableWindow(bEnable);
	}

	if (nEnabledAlarms < 2)
	{
		m_btnAlarmRandom.EnableWindow(FALSE);
		if (m_iRadioAlarm == ALARM_RANDOM)
		{
			m_iRadioAlarm = ALARM_REGULAR;
			CDataExchange dx(this, FALSE);
			DDX_Radio(&dx, IDC_RADIO_ALARM_RANDOM, m_iRadioAlarm);
		}
	}

}
/////////////////////////////////////////////////////////////////////////////
int CSimUnitDlg::EnableAlarmButtons(BOOL bEnable) 
{
	int i, nEnabled = 0;
	BOOL bEnabled;
	DWORD dwBit;
	for (i=0, dwBit=1; i<NO_OF_ALARM_BTN; i++, dwBit<<=1)
	{
		bEnabled = dwBit & m_dwEnableMask ? TRUE : FALSE;
		if (bEnabled)
		{
			nEnabled++;
		}
		m_btnAlarm[i].EnableWindow(bEnable && bEnabled);
	}
	return nEnabled;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSimUnitDlg::IsDataValid() 
{
	UpdateData();
	BOOL bOK = TRUE;
	if (   (m_iRadioInterval == INTERVAL_RANDOM)
		&& (m_dwIntervalRandomMax < m_dwIntervalRandomMin)
		)
	{
		bOK = FALSE;
		AfxMessageBox(IDP_INTERVAL_ERROR, MB_OK|MB_ICONSTOP);
		m_editIntervalRandomMin.SetFocus();
	}
	else if (   (m_iRadioHold == HOLD_RANDOM)
			 && (m_dwHoldRandomMax < m_dwHoldRandomMin)
			)
	{
		bOK = FALSE;
		AfxMessageBox(IDP_HOLD_ERROR, MB_OK|MB_ICONSTOP);
		m_editHoldRandomMin.SetFocus();
	}
	return bOK;
}
/////////////////////////////////////////////////////////////////////////////
void CSimUnitDlg::InitNextAlarmStart(int iPreviousAlarm) 
{
	DWORD dwIntervalMS = 0;
	switch (m_iRadioInterval)
	{
	case INTERVAL_REGULAR:
		dwIntervalMS = m_dwIntervalRegular;
		break;
	case INTERVAL_RANDOM:
		dwIntervalMS =	m_dwIntervalRandomMin +
						(DWORD)((float)(m_dwIntervalRandomMax - m_dwIntervalRandomMin + 1) * ((float)rand()/((float)RAND_MAX+1.0)));
		TRACE(_T("dwIntervalMS random %u\n"), dwIntervalMS);
		break;
	}

	if (m_iRadioAlarm == ALARM_ALL)
	{
		m_stStartTime[0] = m_stLastStartTime;
		m_stStartTime[0].IncrementTime(0,0,0,0,dwIntervalMS);
		m_stLastStartTime = m_stStartTime[0];
		TRACE(_T("All alarms start %s\n"), m_stStartTime[0].ToString());
	}
	else
	{
		int iAlarm = iPreviousAlarm;
		if (m_iRadioAlarm == ALARM_REGULAR)
		{
			iAlarm++;
			// if alarm not enabled force next one
			while ((m_dwEnableMask & (1<<iAlarm)) == 0)
			{
				(iAlarm >= NO_OF_ALARM_BTN) ? iAlarm=0 : iAlarm++;
			}
			TRACE(_T("iAlarm next %i %i\n"), iAlarm, iPreviousAlarm);
		}
		else
		{	// random, but other than previous
			
			while (iAlarm == iPreviousAlarm)
			{
				iAlarm = MulDiv(rand(), NO_OF_ALARM_BTN, RAND_MAX);
				// if alarm not enabled force another one
				if ((m_dwEnableMask & (1<<(iAlarm))) == 0)
				{
					iAlarm = iPreviousAlarm;
				}
				TRACE(_T("iAlarm random %d %d\n"), iAlarm, iPreviousAlarm);
			}
		}
		if (IsBetween(iAlarm, 0, NO_OF_ALARM_BTN-1))
		{
			m_stStartTime[iAlarm] = m_stLastStartTime;
			m_stStartTime[iAlarm].IncrementTime(0,0,0,0,dwIntervalMS);
			m_stLastStartTime = m_stStartTime[iAlarm];
			TRACE(_T("Alarms%d start %s\n"), iAlarm+1, m_stStartTime[iAlarm].ToString());
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSimUnitDlg::InitNextAlarmEnd(int iAlarm) 
{
	DWORD dwHoldMS = 0;
	switch (m_iRadioHold)
	{
	case HOLD_REGULAR:
		dwHoldMS = m_dwHoldRegular;
		break;
	case HOLD_RANDOM:
		dwHoldMS =	m_dwHoldRandomMin +	(DWORD)((float)(m_dwHoldRandomMax - m_dwHoldRandomMin + 1) * ((float)rand()/((float)RAND_MAX + 1.0)));
		TRACE(_T("dwHoldMS random %u\n"), dwHoldMS);
		break;
	}

	if (m_iRadioAlarm == ALARM_ALL)
	{
		m_stEndTime[0] = m_stStartTime[0];
		m_stEndTime[0].IncrementTime(0,0,0,0,dwHoldMS);
		TRACE(_T("All alarms end %s\n"), m_stEndTime[0].ToString());
	}
	else
	{
		if (IsBetween(iAlarm, 0, NO_OF_ALARM_BTN-1))
		{
			m_stEndTime[iAlarm] = m_stStartTime[iAlarm];
			m_stEndTime[iAlarm].IncrementTime(0,0,0,0,dwHoldMS);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSimUnitDlg::CheckAlarms() 
{
//	TRACE(_T("CheckAlarms\n"));
	CSystemTime stCurrent;
	stCurrent.GetLocalTime();

	if (m_iRadioAlarm == ALARM_ALL)
	{
		if (m_bAlarm[0])
		{
			if (stCurrent >= m_stEndTime[0])
			{
				DWORD dwBit = 1;
				int i;
				for (i=0; i<NO_OF_ALARM_BTN; i++, dwBit <<= 1)
				{
					if (m_dwEnableMask & dwBit)
					{
						OnAlarm(IDC_ALERT1+i);
					}
				}
			}
		}
		else
		{
			if (stCurrent >= m_stStartTime[0])
			{
				DWORD dwBit = 1;
				int i;
				for (i=0; i<NO_OF_ALARM_BTN; i++, dwBit <<= 1)
				{
					if (m_dwEnableMask & dwBit)
					{
						OnAlarm(IDC_ALERT1+i);
					}
				}
				InitNextAlarmEnd(0);
				m_stStartTime[0] = FAR_FUTURE;
				InitNextAlarmStart(0);
			}
		}
	}
	else // (m_iRadioAlarm != ALARM_ALL)
	{
		DWORD dwBit;
		int i;
		for (i=0, dwBit=1; i<NO_OF_ALARM_BTN; i++, dwBit <<= 1)
		{
			if (m_dwEnableMask & dwBit)
			{
				if (m_bAlarm[i])
				{
					if (stCurrent >= m_stEndTime[i])
					{
						OnAlarm(IDC_ALERT1+i);
					}
				}
				else
				{
					if (stCurrent >= m_stStartTime[i])
					{
						OnAlarm(IDC_ALERT1+i);
						InitNextAlarmEnd(i);
						m_stStartTime[i] = FAR_FUTURE;
						InitNextAlarmStart(i);
					}
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSimUnitDlg::StopRandomAlarms() 
{
	int i;
	TRACE(_T("StopRandomAlarms\n"));
	for (i=0; i< NO_OF_ALARM_BTN; i++)
	{
		m_stStartTime[i] = FAR_FUTURE;
	}

	m_iRandomAlarm = 0;
	CString sText;
	sText.LoadString(IDS_RANDOM_ALARMS);
	m_btnRandomAlarm.SetWindowText(sText);
	ShowHide();

	for (i=0; i< NO_OF_ALARM_BTN; i++)
	{
		if (m_bAlarm[i])
		{
			OnAlarm(IDC_ALERT1+i);
		}
		WK_TRACE(_T("Random Alarms  %2d %08x\n"), i+1, m_dwAlarmCounter[i]);
		m_dwAlarmCounter[i] = 0;
	}
}
/////////////////////////////////////////////////////////////////////////////
// CSimUnitDlg message handlers
BOOL CSimUnitDlg::OnInitDialog()
{
	CSkinDialog::OnInitDialog();

//	WK_TRACE(_T("-2\n"));
	AutoCreateSkinButtons();
//	WK_TRACE(_T("-1\n"));
	AutoCreateSkinStatic();
//	WK_TRACE(_T("0\n"));
	AutoCreateSkinEdit();
//	WK_TRACE(_T("1\n"));

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	CString strAboutMenu;
	strAboutMenu.LoadString(IDS_ABOUTBOX);
	if (!strAboutMenu.IsEmpty())
	{
		pSysMenu->AppendMenu(MF_SEPARATOR);
		pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
	}
//	WK_TRACE(_T("2\n"));

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	SetWindowText(COemGuiApi::GetApplicationName(WAI_SIMUNIT));
//	WK_TRACE(_T("3\n"));

	CRect rcDlg, rcLine;
	GetWindowRect(rcDlg);
	m_iBigWidth = rcDlg.Width();
	m_staticLine.GetWindowRect(rcLine);
	m_iSmallWidth = rcLine.left - rcDlg.left;

//	WK_TRACE(_T("4\n"));
	LoadSettings();
//	WK_TRACE(_T("5\n"));
	
	// initialize both history edit controls
	InputHistory(IDS_UNDEFINED);
//	WK_TRACE(_T("6\n"));
	OutputHistory(IDS_UNDEFINED);
//	WK_TRACE(_T("7\n"));

	ShowHide();
//	WK_TRACE(_T("8\n"));

#ifndef _DEBUG
	ShowWindow(SW_MINIMIZE);
#endif
//	WK_TRACE(_T("9\n"));

	// start alarms if necessary
	if (m_iRandomAlarm == 1)
	{
//		WK_TRACE(_T("10\n"));
		srand(GetCurrentTime());
		m_stLastStartTime.GetLocalTime();
		InitNextAlarmStart(0);
//		WK_TRACE(_T("11\n"));
	}
//	WK_TRACE(_T("End InitDialog\n"));

	return TRUE;  // return TRUE  unless you set the focus to a control
}
/////////////////////////////////////////////////////////////////////////////
void CSimUnitDlg::OnOK() 
{
	if (IsDataValid())
	{
		SaveSettings();
		StopRandomAlarms();
		DestroyWindow();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSimUnitDlg::OnCancel() 
{
	StopRandomAlarms();
	DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CSimUnitDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		COemGuiApi::AboutDialog(this);
	}
	else
	{
		CSkinDialog::OnSysCommand(nID, lParam);
	}

}
/////////////////////////////////////////////////////////////////////////////
void CSimUnitDlg::OnDestroy()
{
	WinHelp(0L, HELP_QUIT);
}
/////////////////////////////////////////////////////////////////////////////
// If you add a minimize button to your dialog, you will need the code below
// to draw the icon.  For MFC applications using the document/view model,
// this is automatically done for you by the framework.
void CSimUnitDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CSkinDialog::OnPaint();
	}
}
/////////////////////////////////////////////////////////////////////////////
// The system calls this to obtain the cursor to display while the user drags
// the minimized window.
HCURSOR CSimUnitDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}
/////////////////////////////////////////////////////////////////////////////
// loads string with nID from resource and adds it to the history edit control
void CSimUnitDlg::InputHistory(UINT nID)
{
	CString sHelp;
	
	sHelp.LoadString(nID);
	InputHistory(sHelp);
}
////////////////////////////////////////////////////////////////////////////
// adds lpszMessage as a new line to the history edit control
// deletes first line if there are more than 50 lines
// so we have allways less than 50 lines in the history control
void CSimUnitDlg::InputHistory(LPCTSTR lpszMessage)
{
	CString sHelp = lpszMessage;
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_INPUT_HISTORY);
	int c,i;

	c = pEdit->GetLineCount();
	
	if (c>MAX_HISTORY_LINES)
	{
		pEdit->SetSel(0,pEdit->LineIndex(1),TRUE); // select first line, no scrolling
		pEdit->Clear(); // delete the selection
		i = pEdit->LineIndex(c-2) + pEdit->LineLength(c-2) + 3;
		pEdit->SetSel(i,i,TRUE); // unselect
	}

	sHelp +=_T("\r\n");
	pEdit->ReplaceSel(sHelp);
}
////////////////////////////////////////////////////////////////////////////
// loads string with nID from resource and adds it to the history edit control
void CSimUnitDlg::OutputHistory(UINT nID)
{
	CString sHelp;
	
	sHelp.LoadString(nID);
	OutputHistory(sHelp);
}
/////////////////////////////////////////////////////////////////////////////
// adds lpszMessage as a new line to the history edit control
// deletes first line if there are more than 50 lines
// so we have allways less than 50 lines in the history control
void CSimUnitDlg::OutputHistory(LPCTSTR lpszMessage)
{
	CString sHelp = lpszMessage;
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_OUTPUT_HISTORY);
	int c,i;
	
	c = pEdit->GetLineCount();
	
	if (c>MAX_HISTORY_LINES)
	{
		pEdit->SetSel(0,pEdit->LineIndex(1),TRUE); // select first line, no scrolling
		pEdit->Clear(); // delete the selection
		i = pEdit->LineIndex(c-2) + pEdit->LineLength(c-2) + 3;
		pEdit->SetSel(i,i,TRUE); // unselect
	}

	sHelp +=_T("\r\n");
	pEdit->ReplaceSel(sHelp);
}
/////////////////////////////////////////////////////////////////////////////
// actualizes the display of 4 build in relays
// illustrating output features of Security
void CSimUnitDlg::ActualizeRelay(int iNr, BOOL bState)
{
	// iNr 0..3      bState = TRUE means that relay is closed
	if (bState)
	{
		m_dwRelayMask |= 1<<(iNr);
	}
	else
	{
		m_dwRelayMask &= ~(1<<(iNr));
	}


	CString sMessage;
	TCHAR szBuf[2];

	// we have two bitmaps for open and closed status
	((CStatic*)GetDlgItem(IDC_RELAY1+iNr))->SetBitmap(bState ? m_bmClose : m_bmOpen);
	
	// add relay action to history
	sMessage.LoadString(bState ? IDS_RELAY_CLOSE : IDS_RELAY_OPEN);
	sMessage += _itot(iNr+1, szBuf, 10);
	OutputHistory(sMessage);
}
/////////////////////////////////////////////////////////////////////////////
// resets all alarm buttons
void CSimUnitDlg::ResetAlarms(DWORD dwAlarmMask, DWORD dwEnableMask)
{
	// bit set means ALARM
	BOOL bEnable=0;
	DWORD dwBit=1;

	BOOL bNewState = FALSE;

	// to reset the alarms
	// first take the opposite and then call OnAlarmX to toggle the state
	// this will call the appropriate DoIndicateAlarm and checks the button also
	// if not enabled always set TRUE, this results in no alarm
	CWK_Profile wkp;
	CString sSection, sText;
	sSection = SEC_NAME_INPUTGROUPS;
	sText.Format(_T("Group%04x"), theApp.GetInput()->GetGroupID());
	sSection = sSection + _T("\\") + sText;
	m_saAlarmBtnTexts.RemoveAll();
	int i;
	for (i=0; i<NO_OF_ALARM_BTN; i++)
	{
		bEnable = (dwEnableMask & dwBit)!=0;
		sText = wkp.GetString(sSection, i+1, NULL);
		sText = wkp.GetStringFromString(sText, INI_COMMENT, NULL);
		m_saAlarmBtnTexts.Add(sText);
		m_btnAlarm[i].EnableWindow(bEnable);
		bNewState = bEnable && (dwAlarmMask & dwBit);
		if (bNewState != m_bAlarm[i])
		{
			m_bAlarm[i] = !bNewState;
			OnAlarm(IDC_ALERT1+i);
		}
		dwBit <<= 1;
	}

	if (dwEnableMask == 0)
	{
		StopRandomAlarms();
		m_btnRandomAlarm.EnableWindow(FALSE);
	}
	else
	{
		m_btnRandomAlarm.EnableWindow(TRUE);
	}
}
/////////////////////////////////////////////////////////////////////////////
// OnAlarmx methods perform alert messaging to security
void CSimUnitDlg::OnAlarm(UINT nID) 
{
	UINT nIndex = nID - IDC_ALERT1;
//	TRACE(_T("OnAlarm1\n"));
	m_bAlarm[nIndex] = !m_bAlarm[nIndex];
	if (m_bAlarm[nIndex])
	{
		m_dwAlarmCounter[nIndex]++;
		m_pApp->DoAlert(nIndex+1);
	}
	else
	{
		m_pApp->UndoAlert(nIndex+1);
	}
	m_btnAlarm[nIndex].CButton::SetCheck(m_bAlarm[nIndex] ? BST_CHECKED : BST_UNCHECKED);
}
/////////////////////////////////////////////////////////////////////////////
void CSimUnitDlg::OnAbout() 
{
	COemGuiApi::AboutDialog(this);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSimUnitDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	switch(wParam)
	{
	// INPUT related
		case ID_CONNECTION_INPUT:
			InputHistory(IDS_CONNECTION);
			return TRUE;
			break;
		case ID_HARDWARE_INPUT:
			InputHistory(IDS_INITHARDWARE);
			return TRUE;
			break;
		case ID_EDGE_INPUT:
			m_dwEnableMask = LOWORD(lParam);
			m_dwEdgeMask = HIWORD(lParam);
			InputHistory(IDS_EDGE_INPUT);
			return TRUE;
			break;
		case ID_ENABLE_INPUT:
			m_dwEnableMask = LOWORD(lParam);
			m_dwEdgeMask = HIWORD(lParam);
			InputHistory(IDS_ENABLE_INPUT);
			return TRUE;
			break;
		case ID_RESET_INPUT:
		{
			DWORD dwEnableMask = LOWORD(lParam);
			DWORD dwAlarmMask = HIWORD(lParam);
			ResetAlarms(dwAlarmMask, dwEnableMask);
			InputHistory(IDS_RESET);
			return TRUE;
			break;
		}
		case ID_DISCONNECT_INPUT:
			InputHistory(IDS_DISCONNECT);
			return TRUE;
			break;
	// OUTPUT related
		case ID_CONNECTION_OUTPUT:
			OutputHistory(IDS_CONNECTION);
			return TRUE;
			break;
		case ID_HARDWARE_OUTPUT:
			ActualizeRelay(0,0);
			ActualizeRelay(1,0);
			ActualizeRelay(2,0);
			ActualizeRelay(3,0);
			OutputHistory(IDS_INITHARDWARE);
			return TRUE;
			break;
		case ID_RESET_OUTPUT:
		{
			m_dwRelayMask = 0;

			ActualizeRelay(0,0);
			ActualizeRelay(1,0);
			ActualizeRelay(2,0);
			ActualizeRelay(3,0);
			OutputHistory(IDS_RESET);
			return TRUE;
			break;
		}
		case ID_CLOSE_RELAY:
			{
			int iNr = (int)lParam;
			ActualizeRelay(iNr, TRUE); // calls OutputHistory too
			}
			return TRUE;
			break;
		case ID_OPEN_RELAY:
		{
			int iNr = (int)lParam;
			ActualizeRelay(iNr, FALSE); // calls OutputHistory too
			return TRUE;
			break;
		}
		case ID_DISCONNECT_OUTPUT:
			OutputHistory(IDS_DISCONNECT);
			return TRUE;
			break;
	}
	
	return CSkinDialog::OnCommand(wParam, lParam);
}
/////////////////////////////////////////////////////////////////////////////
long CSimUnitDlg::DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam)
{
	HWND hWnd = (HWND)wParam;

	if (hWnd && IsWindow(hWnd))
		::PostMessage(hWnd, WM_SELFCHECK, WAI_SIMUNIT, 0);

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CSimUnitDlg::OnCheckRandomAlarm() 
{
	if (IsDataValid())
	{
		CString sText;
		switch (m_iRandomAlarm)
		{
		case 0:
			m_iRandomAlarm = 2;
			sText.LoadString(IDS_START_ALARMS);
			break;
		case 1:
			m_iRandomAlarm = 0;
			sText.LoadString(IDS_RANDOM_ALARMS);
			break;
		case 2:
			m_iRandomAlarm = 1;
			sText.LoadString(IDS_STOP_ALARMS);
			break;
		}
		m_btnRandomAlarm.SetWindowText(sText);

		ShowHide();
		SaveSettings();
		if (m_iRandomAlarm == 1)
		{
			// start alarms
			srand(GetCurrentTime());
			m_stLastStartTime.GetLocalTime();
			int nStart = 0;
			if (m_iRadioAlarm == ALARM_REGULAR) nStart = -1;
			InitNextAlarmStart(nStart);
		}
		else if (m_iRandomAlarm == 0)
		{
			StopRandomAlarms();
		}
	}
//	TRACE(_T("Random Alarms %i\n"), m_iRandomAlarm);
}
/////////////////////////////////////////////////////////////////////////////
void CSimUnitDlg::OnRadioAlarm() 
{
	ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
void CSimUnitDlg::OnRadioHold() 
{
	ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
void CSimUnitDlg::OnRadioInterval() 
{
	ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CSimUnitDlg::OnLanguageChanged(WPARAM wParam, LPARAM lParam)
{
#if _MFC_VER >= 0x0710
	LRESULT lResult = theApp.SetLanguageParameters((UINT)wParam, (DWORD)lParam);
	SetChildWindowFont(m_hWnd);
	return lResult;
#else
	return 0;
#endif
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSimUnitDlg::GetToolTip(UINT nID, CString&sText)
{
	if (IsBetween(nID, IDC_ALERT1, IDC_ALERT1+NO_OF_ALARM_BTN) && m_saAlarmBtnTexts.GetCount())
	{
		sText = m_saAlarmBtnTexts.GetAt(nID - IDC_ALERT1);
		return TRUE;
	}
	return FALSE;
}
