// DVSoftAlarmDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DVSoftAlarm.h"
#include "DVSoftAlarmDlg.h"
#include ".\dvsoftalarmdlg.h"
#include "wkclasses\wk_RuntimeError.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CDVSoftAlarmApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CDVSoftAlarmDlg dialog

CDVSoftAlarmDlg::CDVSoftAlarmDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDVSoftAlarmDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDVSoftAlarmDlg)
	m_x = 0;
	m_y = 0;
	m_nWAI = WAI_INVALID;
	m_nRTE = RTE_INVALID;
	m_nREL = REL_INVALID;
	m_nParam1 = 0;
	m_nParam2 = 0;
	m_sErrorText = _T("Fehler Text");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_b2Left = TRUE;
	m_iCounter = 0;
	m_nTimer   = 0;
	m_nMDBtnID = IDC_BUTTON_MD1;
}

void CDVSoftAlarmDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDVSoftAlarmDlg)
	DDX_Control(pDX, IDC_MYICON, m_Icon);
	DDX_Text(pDX, IDC_EDIT_X, m_x);
	DDX_Text(pDX, IDC_EDIT_Y, m_y);
	DDX_Text(pDX, IDC_EDT_WAI, m_nWAI);
	DDV_MinMaxInt(pDX, m_nWAI, WAI_INVALID, 250);
	DDX_Text(pDX, IDC_EDT_RTE, m_nRTE);
	DDV_MinMaxInt(pDX, m_nRTE, RTE_INVALID, 250);
	DDX_Text(pDX, IDC_EDT_REL, m_nREL);
	DDV_MinMaxInt(pDX, m_nREL, REL_INVALID, 250);
	DDX_Text(pDX, IDC_EDT_PARAM1, m_nParam1);
	DDX_Text(pDX, IDC_EDT_PARAM2, m_nParam2);
	DDX_Text(pDX, IDC_EDT_TXT, m_sErrorText);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDVSoftAlarmDlg, CDialog)
	//{{AFX_MSG_MAP(CDVSoftAlarmDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CHECK_S1, OnCheckS1)
	ON_BN_CLICKED(IDC_CHECK_S10, OnCheckS10)
	ON_BN_CLICKED(IDC_CHECK_S11, OnCheckS11)
	ON_BN_CLICKED(IDC_CHECK_S12, OnCheckS12)
	ON_BN_CLICKED(IDC_CHECK_S13, OnCheckS13)
	ON_BN_CLICKED(IDC_CHECK_S14, OnCheckS14)
	ON_BN_CLICKED(IDC_CHECK_S15, OnCheckS15)
	ON_BN_CLICKED(IDC_CHECK_S16, OnCheckS16)
	ON_BN_CLICKED(IDC_CHECK_S2, OnCheckS2)
	ON_BN_CLICKED(IDC_CHECK_S3, OnCheckS3)
	ON_BN_CLICKED(IDC_CHECK_S4, OnCheckS4)
	ON_BN_CLICKED(IDC_CHECK_S5, OnCheckS5)
	ON_BN_CLICKED(IDC_CHECK_S6, OnCheckS6)
	ON_BN_CLICKED(IDC_CHECK_S7, OnCheckS7)
	ON_BN_CLICKED(IDC_CHECK_S8, OnCheckS8)
	ON_BN_CLICKED(IDC_CHECK_S9, OnCheckS9)
	ON_BN_CLICKED(IDC_CHECK_A1, OnCheckA1)
	ON_BN_CLICKED(IDC_CHECK_A10, OnCheckA10)
	ON_BN_CLICKED(IDC_CHECK_A11, OnCheckA11)
	ON_BN_CLICKED(IDC_CHECK_A12, OnCheckA12)
	ON_BN_CLICKED(IDC_CHECK_A13, OnCheckA13)
	ON_BN_CLICKED(IDC_CHECK_A14, OnCheckA14)
	ON_BN_CLICKED(IDC_CHECK_A15, OnCheckA15)
	ON_BN_CLICKED(IDC_CHECK_A16, OnCheckA16)
	ON_BN_CLICKED(IDC_CHECK_A2, OnCheckA2)
	ON_BN_CLICKED(IDC_CHECK_A3, OnCheckA3)
	ON_BN_CLICKED(IDC_CHECK_A4, OnCheckA4)
	ON_BN_CLICKED(IDC_CHECK_A5, OnCheckA5)
	ON_BN_CLICKED(IDC_CHECK_A6, OnCheckA6)
	ON_BN_CLICKED(IDC_CHECK_A7, OnCheckA7)
	ON_BN_CLICKED(IDC_CHECK_A8, OnCheckA8)
	ON_BN_CLICKED(IDC_CHECK_A9, OnCheckA9)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BUTTON_MD1, IDC_BUTTON_MD16, OnButtonMd)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_ICON_NERVES, OnIconNervesAndPeriodicMD)
	ON_BN_CLICKED(IDC_CHECK_ALL_EXTERN, OnBnClickedCheckAllExtern)
	ON_BN_CLICKED(IDC_BUTTON_ALL_MD, OnBnClickedButtonAllMd)
	ON_WM_DESTROY()
	ON_EN_KILLFOCUS(IDC_EDIT_X, OnEnKillfocusEdit)
	ON_EN_KILLFOCUS(IDC_EDIT_Y, OnEnKillfocusEdit)
	ON_BN_CLICKED(IDC_BUTTON_RTE, OnBnClickedButtonRte)
	ON_WM_PARENTNOTIFY()
	ON_CBN_SELCHANGE(AFX_IDW_PANE_FIRST, OnCbnSelchangeCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDVSoftAlarmDlg message handlers

BOOL CDVSoftAlarmDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	GetDlgItem(IDC_EDT_WAI)->ModifyStyleEx(WS_EX_NOPARENTNOTIFY, 0);
	GetDlgItem(IDC_EDT_RTE)->ModifyStyleEx(WS_EX_NOPARENTNOTIFY, 0);
	GetDlgItem(IDC_EDT_REL)->ModifyStyleEx(WS_EX_NOPARENTNOTIFY, 0);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDVSoftAlarmDlg::OnPaint() 
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
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDVSoftAlarmDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}
/////////////////////////////////////////////////////////////////////////////
void CDVSoftAlarmDlg::DoMDAlarm(DWORD id)
{
	UpdateData();
	WORD wMD = MAKEWORD((BYTE)((m_x*256)/1000),(BYTE)((m_y*256)/1000));
//	DWORD wParam = MAKELONG(0x01,wMD);
	DoAlarm(id,TRUE,wMD);
	Sleep(100);
	DoAlarm(id,FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CDVSoftAlarmDlg::DoAlarm(DWORD id, BOOL bAlarm,WORD wMD/*=0*/)
{
	CWnd* pWnd = FindWindow(_T("SecurityServer"),NULL);
	CString s;
	if (pWnd)
	{
		pWnd->GetWindowText(s);
		if (s == _T("Process"))
		{
			if (wMD == 0)
			{
				pWnd->PostMessage(WM_USER,bAlarm ? 1 : 2, id);
			}
			else
			{
				pWnd->PostMessage(WM_USER,MAKELONG(1,wMD),id);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDVSoftAlarmDlg::OnCheckS1() 
{
	DoAlarm(0x10040008,((CButton*)GetDlgItem(IDC_CHECK_S1))->GetCheck());
	
}
/////////////////////////////////////////////////////////////////////////////
void CDVSoftAlarmDlg::OnCheckS10() 
{
	DoAlarm(0x10050009,((CButton*)GetDlgItem(IDC_CHECK_S10))->GetCheck());
}
/////////////////////////////////////////////////////////////////////////////
void CDVSoftAlarmDlg::OnCheckS11() 
{
	DoAlarm(0x1005000A,((CButton*)GetDlgItem(IDC_CHECK_S11))->GetCheck());
}
/////////////////////////////////////////////////////////////////////////////
void CDVSoftAlarmDlg::OnCheckS12() 
{
	DoAlarm(0x1005000B,((CButton*)GetDlgItem(IDC_CHECK_S12))->GetCheck());
}

void CDVSoftAlarmDlg::OnCheckS13() 
{
	DoAlarm(0x1005000C,((CButton*)GetDlgItem(IDC_CHECK_S13))->GetCheck());
}

void CDVSoftAlarmDlg::OnCheckS14() 
{
	DoAlarm(0x1005000D,((CButton*)GetDlgItem(IDC_CHECK_S14))->GetCheck());
}

void CDVSoftAlarmDlg::OnCheckS15() 
{
	DoAlarm(0x1005000E,((CButton*)GetDlgItem(IDC_CHECK_S15))->GetCheck());
}

void CDVSoftAlarmDlg::OnCheckS16() 
{
	DoAlarm(0x1005000F,((CButton*)GetDlgItem(IDC_CHECK_S16))->GetCheck());
}

void CDVSoftAlarmDlg::OnCheckS2() 
{
	DoAlarm(0x10040009,((CButton*)GetDlgItem(IDC_CHECK_S2))->GetCheck());
}

void CDVSoftAlarmDlg::OnCheckS3() 
{
	DoAlarm(0x1004000A,((CButton*)GetDlgItem(IDC_CHECK_S3))->GetCheck());
	
}

void CDVSoftAlarmDlg::OnCheckS4() 
{
	DoAlarm(0x1004000B,((CButton*)GetDlgItem(IDC_CHECK_S4))->GetCheck());
	
}

void CDVSoftAlarmDlg::OnCheckS5() 
{
	DoAlarm(0x1004000C,((CButton*)GetDlgItem(IDC_CHECK_S5))->GetCheck());
	
}

void CDVSoftAlarmDlg::OnCheckS6() 
{
	DoAlarm(0x1004000D,((CButton*)GetDlgItem(IDC_CHECK_S6))->GetCheck());
	
}

void CDVSoftAlarmDlg::OnCheckS7() 
{
	DoAlarm(0x1004000E,((CButton*)GetDlgItem(IDC_CHECK_S7))->GetCheck());
	
}

void CDVSoftAlarmDlg::OnCheckS8() 
{
	DoAlarm(0x1004000F,((CButton*)GetDlgItem(IDC_CHECK_S8))->GetCheck());
}

void CDVSoftAlarmDlg::OnCheckS9() 
{
	DoAlarm(0x10050008,((CButton*)GetDlgItem(IDC_CHECK_S9))->GetCheck());
}

void CDVSoftAlarmDlg::OnCheckA1() 
{
	DoAlarm(0x10040000,((CButton*)GetDlgItem(IDC_CHECK_A1))->GetCheck());
}

void CDVSoftAlarmDlg::OnCheckA10() 
{
	DoAlarm(0x10050001,((CButton*)GetDlgItem(IDC_CHECK_A10))->GetCheck());
}

void CDVSoftAlarmDlg::OnCheckA11() 
{
	DoAlarm(0x10050002,((CButton*)GetDlgItem(IDC_CHECK_A11))->GetCheck());
}

void CDVSoftAlarmDlg::OnCheckA12() 
{
	DoAlarm(0x10050003,((CButton*)GetDlgItem(IDC_CHECK_A12))->GetCheck());
}

void CDVSoftAlarmDlg::OnCheckA13() 
{
	DoAlarm(0x10050004,((CButton*)GetDlgItem(IDC_CHECK_A13))->GetCheck());
}

void CDVSoftAlarmDlg::OnCheckA14() 
{
	DoAlarm(0x10050005,((CButton*)GetDlgItem(IDC_CHECK_A14))->GetCheck());
}

void CDVSoftAlarmDlg::OnCheckA15() 
{
	DoAlarm(0x10050006,((CButton*)GetDlgItem(IDC_CHECK_A15))->GetCheck());
}

void CDVSoftAlarmDlg::OnCheckA16() 
{
	DoAlarm(0x10050007,((CButton*)GetDlgItem(IDC_CHECK_A16))->GetCheck());
}

void CDVSoftAlarmDlg::OnCheckA2() 
{
	DoAlarm(0x10040001,((CButton*)GetDlgItem(IDC_CHECK_A2))->GetCheck());
}

void CDVSoftAlarmDlg::OnCheckA3() 
{
	DoAlarm(0x10040002,((CButton*)GetDlgItem(IDC_CHECK_A3))->GetCheck());
}

void CDVSoftAlarmDlg::OnCheckA4() 
{
	DoAlarm(0x10040003,((CButton*)GetDlgItem(IDC_CHECK_A4))->GetCheck());
}

void CDVSoftAlarmDlg::OnCheckA5() 
{
	DoAlarm(0x10040004,((CButton*)GetDlgItem(IDC_CHECK_A5))->GetCheck());
}

void CDVSoftAlarmDlg::OnCheckA6() 
{
	DoAlarm(0x10040005,((CButton*)GetDlgItem(IDC_CHECK_A6))->GetCheck());
}

void CDVSoftAlarmDlg::OnCheckA7() 
{
	DoAlarm(0x10040006,((CButton*)GetDlgItem(IDC_CHECK_A7))->GetCheck());
}

void CDVSoftAlarmDlg::OnCheckA8() 
{
	DoAlarm(0x10040007,((CButton*)GetDlgItem(IDC_CHECK_A8))->GetCheck());
}

void CDVSoftAlarmDlg::OnCheckA9() 
{
	DoAlarm(0x10050000,((CButton*)GetDlgItem(IDC_CHECK_A9))->GetCheck());
}

void CDVSoftAlarmDlg::OnButtonMd(UINT nID) 
{
	int n = nID - IDC_BUTTON_MD1;
	if (n<8)
	{
		DoMDAlarm(0x10000000 + n);
	}
	else
	{
		n -= 8;
		DoMDAlarm(0x10010000 + n);
	}
}

void CDVSoftAlarmDlg::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == m_nTimer)
	{
		CRect rect;

		m_Icon.GetWindowRect(rect);
		ScreenToClient(rect);
		if (m_b2Left)
		{
			rect.left++;
			rect.right++;
			m_iCounter++;
			if (m_iCounter == 4)
			{
				m_iCounter = 0;
			}
		}
		else
		{
			rect.left--;
			rect.right--;
			m_iCounter--;
			if (m_iCounter == -1)
			{
				m_iCounter = 3;
			}
		}
		m_Icon.MoveWindow(rect);
		
		if (rect.left == 300)
		{
			m_b2Left = FALSE;
		}
		if (rect.left == 20)
		{
			m_b2Left = TRUE;
		}

		m_Icon.SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME+m_iCounter));

		m_x = MulDiv(rand(), 1000, RAND_MAX);
		m_y = MulDiv(rand(), 1000, RAND_MAX);

		if (m_nMDBtnID > IDC_BUTTON_MD16)
		{
			m_nMDBtnID = IDC_BUTTON_MD1;
		}

		CButton *pBtn = (CButton*)GetDlgItem(m_nMDBtnID);
		pBtn->SetState(TRUE);
		OnButtonMd(m_nMDBtnID++);
		pBtn->SetState(FALSE);
	}	
	CDialog::OnTimer(nIDEvent);
}

void CDVSoftAlarmDlg::OnIconNervesAndPeriodicMD() 
{
	if (m_nTimer)
	{
		KillTimer(m_nTimer);
		m_nTimer = 0;
		m_Icon.ShowWindow(SW_HIDE);
	}
	else
	{
		CTime time = CTime::GetCurrentTime();
		srand((UINT)time.GetTime());
		m_nTimer = SetTimer(1,120,NULL);
		m_Icon.ShowWindow(SW_SHOW);
	}
}

void CDVSoftAlarmDlg::OnBnClickedCheckAllExtern()
{
	// TODO: Add your control notification handler code here
	BOOL bChecked;
		
	bChecked = ((CButton*)GetDlgItem(IDC_CHECK_A1))->GetCheck();
	((CButton*)GetDlgItem(IDC_CHECK_A1))->SetCheck(!bChecked);
	OnCheckA1();
	bChecked = ((CButton*)GetDlgItem(IDC_CHECK_A2))->GetCheck();
	((CButton*)GetDlgItem(IDC_CHECK_A2))->SetCheck(!bChecked);
	OnCheckA2();
	bChecked = ((CButton*)GetDlgItem(IDC_CHECK_A3))->GetCheck();
	((CButton*)GetDlgItem(IDC_CHECK_A3))->SetCheck(!bChecked);
	OnCheckA3();
	bChecked = ((CButton*)GetDlgItem(IDC_CHECK_A4))->GetCheck();
	((CButton*)GetDlgItem(IDC_CHECK_A4))->SetCheck(!bChecked);
	OnCheckA4();
	bChecked = ((CButton*)GetDlgItem(IDC_CHECK_A5))->GetCheck();
	((CButton*)GetDlgItem(IDC_CHECK_A5))->SetCheck(!bChecked);
	OnCheckA5();
	bChecked = ((CButton*)GetDlgItem(IDC_CHECK_A6))->GetCheck();
	((CButton*)GetDlgItem(IDC_CHECK_A6))->SetCheck(!bChecked);
	OnCheckA6();
	bChecked = ((CButton*)GetDlgItem(IDC_CHECK_A7))->GetCheck();
	((CButton*)GetDlgItem(IDC_CHECK_A7))->SetCheck(!bChecked);
	OnCheckA7();
	bChecked = ((CButton*)GetDlgItem(IDC_CHECK_A8))->GetCheck();
	((CButton*)GetDlgItem(IDC_CHECK_A8))->SetCheck(!bChecked);
	OnCheckA8();
	bChecked = ((CButton*)GetDlgItem(IDC_CHECK_A9))->GetCheck();
	((CButton*)GetDlgItem(IDC_CHECK_A9))->SetCheck(!bChecked);
	OnCheckA9();

	bChecked = ((CButton*)GetDlgItem(IDC_CHECK_A10))->GetCheck();
	((CButton*)GetDlgItem(IDC_CHECK_A10))->SetCheck(!bChecked);
	OnCheckA10();
	bChecked = ((CButton*)GetDlgItem(IDC_CHECK_A11))->GetCheck();
	((CButton*)GetDlgItem(IDC_CHECK_A11))->SetCheck(!bChecked);
	OnCheckA11();
	bChecked = ((CButton*)GetDlgItem(IDC_CHECK_A12))->GetCheck();
	((CButton*)GetDlgItem(IDC_CHECK_A12))->SetCheck(!bChecked);
	OnCheckA12();
	bChecked = ((CButton*)GetDlgItem(IDC_CHECK_A13))->GetCheck();
	((CButton*)GetDlgItem(IDC_CHECK_A13))->SetCheck(!bChecked);
	OnCheckA13();
	bChecked = ((CButton*)GetDlgItem(IDC_CHECK_A14))->GetCheck();
	((CButton*)GetDlgItem(IDC_CHECK_A14))->SetCheck(!bChecked);
	OnCheckA14();
	bChecked = ((CButton*)GetDlgItem(IDC_CHECK_A15))->GetCheck();
	((CButton*)GetDlgItem(IDC_CHECK_A15))->SetCheck(!bChecked);
	OnCheckA15();
	bChecked = ((CButton*)GetDlgItem(IDC_CHECK_A16))->GetCheck();
	((CButton*)GetDlgItem(IDC_CHECK_A16))->SetCheck(!bChecked);
	OnCheckA16();
}

void CDVSoftAlarmDlg::OnBnClickedButtonAllMd()
{
	for (UINT i=IDC_BUTTON_MD1; i<=IDC_BUTTON_MD16; i++)
	{
		CButton *pBtn = (CButton*)GetDlgItem(i);
		pBtn->SetState(TRUE);
		OnButtonMd(i);
		pBtn->SetState(FALSE);
	}
}

void CDVSoftAlarmDlg::OnDestroy()
{
	if (m_nTimer)
	{
		KillTimer(m_nTimer);
	}
	CWnd *pWnd = GetDlgItem(AFX_IDW_PANE_FIRST);
	WK_DELETE(pWnd);
	CDialog::OnDestroy();
}

void CDVSoftAlarmDlg::OnEnKillfocusEdit()
{
	UpdateData();
}

void CDVSoftAlarmDlg::OnBnClickedButtonRte()
{
	if (UpdateData())
	{
		CWK_RunTimeError rte( 
				(WK_ApplicationId)m_nWAI,
				(WK_RunTimeErrorLevel)m_nREL,
				(WK_RunTimeError)m_nRTE,
				m_sErrorText,
				m_nParam1,
				m_nParam2,
				RTES_INVALID);
		rte.Send();
	}
}

void CDVSoftAlarmDlg::OnParentNotify(UINT message, LPARAM lParam)
{
	if (message == WM_RBUTTONDOWN)
	{
		CPoint pt(lParam);
		CWnd *pWnd = ChildWindowFromPoint(pt);
		if (pWnd)
		{
			CRect rc;
			m_nEdtID = pWnd->GetDlgCtrlID();
			if (   m_nEdtID == IDC_EDT_WAI
				|| m_nEdtID == IDC_EDT_RTE
				|| m_nEdtID == IDC_EDT_REL)
			{
				pWnd->GetWindowRect(&rc);
				ScreenToClient(&rc);
				rc.right += rc.Width()*6;
				rc.OffsetRect(0, rc.Height());

				pWnd = GetDlgItem(AFX_IDW_PANE_FIRST);
				WK_DELETE(pWnd);

				CComboBox *pBox = new CComboBox;
				CString sName;
				pBox->Create(WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST|WS_VSCROLL, rc, this, AFX_IDW_PANE_FIRST);
				pBox->SetFont(GetFont(), FALSE);
				if (m_nEdtID == IDC_EDT_WAI)
				{
					WK_ApplicationId wai = WAI_INVALID;
					int i;
					for ( ; ; (*((int*)&wai))++)
					{
						sName = GetAppnameFromId(wai);
						if (sName == _T("UNKNOWN"))
						{
							break;
						}
						i = pBox->AddString(sName);
						pBox->SetItemData(i, wai);
					}
				}
				else if (m_nEdtID == IDC_EDT_RTE)
				{
					WK_RunTimeError rte;
					int i, *pi = (int*) &rte;
					for ((*pi)=1; (*pi) < 100; (*pi)++)
					{
						sName = NameOfEnum(rte);
						if (sName.Find(_T("NOT YET")) != -1)
						{
							continue;
						}
						i = pBox->AddString(sName);
						pBox->SetItemData(i, rte);
					}
				}
				else if (m_nEdtID == IDC_EDT_REL)
				{
					int i, j;
					for (i=1; i<7; i++)
					{
						switch (i)
						{
							case REL_REBOOT_ERROR:	sName = _T("Reboot"); break;
							case REL_ERROR:			sName = _T("Error"); break;
							case REL_WARNING:		sName = _T("Warning"); break;
							case REL_MESSAGE:		sName = _T("Message"); break;
							case REL_RESTART_ME:	sName = _T("Restart Me"); break;
							case REL_CANNOT_RUN:	sName = _T("Cannot Run"); break;
						}
						j = pBox->AddString(sName);
						pBox->SetItemData(j, i);
					}
				}

				COMBOBOXINFO cbi;
				CRect	rc;
				int nOldHeight = -1;
				cbi.cbSize = sizeof(COMBOBOXINFO);
				pBox->GetComboBoxInfo(&cbi);
				::GetWindowRect(cbi.hwndList, &rc);
				nOldHeight = rc.Height();
				::SetWindowPos(cbi.hwndList, NULL, 0, 0, rc.Width(), 150, SWP_NOMOVE|SWP_NOZORDER);

				pBox->ShowDropDown();
				SetCursor(theApp.LoadStandardCursor(IDC_ARROW));
			}
		}

	}

	CDialog::OnParentNotify(message, lParam);
}

void CDVSoftAlarmDlg::OnCbnSelchangeCombo()
{
	CWnd *pWnd = GetDlgItem(AFX_IDW_PANE_FIRST);
	if (pWnd)
	{
		ASSERT_KINDOF(CComboBox, pWnd);
		int nSel = ((CComboBox*)pWnd)->GetCurSel();
		if (nSel != CB_ERR)
		{
			nSel = ((CComboBox*)pWnd)->GetItemData(nSel);
		}
		SetDlgItemInt(m_nEdtID, nSel);
		pWnd->ShowWindow(SW_HIDE);
	}
}
