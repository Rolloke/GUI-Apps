// HealthControlDlg.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "HealthControl.h"
#include "HealthControlDlg.h"
#include "W83627.h"
#include "W83697.h"
#include "IT8712.h"
#include "PC87372.h"

#include "ConfigDlg.h"
#include ".\healthcontroldlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define VIRTUAL_ALARM_SET		-50		// -50 °C und -50 Upm wird es wohl nicht geben
#define	VOLTAGE_TRESHOLD		0.01	// 1 Prozent Schwellwert um den Alarm wieder auszuschalten

// CHealthControlDlg Dialogfeld
/////////////////////////////////////////////////////////////////////////////
CHealthControlDlg::CHealthControlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHealthControlDlg::IDD, pParent)
{

	m_pECMon = NULL;
	m_nLastToolTips = 0;

	m_bSendFanRTE = 0;
	m_nFan1RPM = 0;
	m_nFan2RPM = 0;
	m_nFan3RPM = 0;
	m_nLastFan1RPM = 0;	// 0x01
	m_nLastFan2RPM = 0;	// 0x02
	m_nLastFan3RPM = 0;	// 0x04
	m_nFan1RpmThreshold = 1000;
	m_nFan2RpmThreshold = 1000;
	m_nFan3RpmThreshold = 1000;
	m_nFan1Device = DEVICE_UNKNOWN;
	m_nFan2Device = DEVICE_UNKNOWN;
	m_nFan3Device = DEVICE_UNKNOWN;

	m_bSendTemperatureRTE = 0;
	m_nTemperature1 = 0;
	m_nTemperature2 = 0;
	m_nTemperature3 = 0;
    m_nLastTemperature1 = 0;	// 0x01
	m_nLastTemperature2 = 0;	// 0x02
	m_nLastTemperature3 = 0;	// 0x04
	m_nTemperature1Threshold = 55;
	m_nTemperature2Threshold = 55;
	m_nTemperature3Threshold = 55;
	m_nTemperature1Device = DEVICE_UNKNOWN;
	m_nTemperature2Device = DEVICE_UNKNOWN;
	m_nTemperature3Device = DEVICE_UNKNOWN;

	m_bSendVoltageRTE = 0;
	m_fVCoreA = 0;
	m_fVCoreB = 0;
	m_f12V = 0;
	m_fN5V = 0;
	m_f3_3V = 0;
	m_f5V = 0;
	m_fN12V = 0;

	m_bVCoreATresholdFailureReported	= FALSE; // 0, 0x01
	m_bVCoreBTresholdFailureReported	= FALSE; // 1, 0x02
	m_b3_3VTresholdFailureReported		= FALSE; // 2, 0x04
	m_b5VTresholdFailureReported		= FALSE; // 3, 0x08
	m_bN5VTresholdFailureReported		= FALSE; // 4, 0x10
	m_b12VTresholdFailureReported		= FALSE; // 5, 0x20
	m_bN12VTresholdFailureReported		= FALSE; // 6, 0x40

	m_fVCoreAMinThreshold	= (float)1.20;
	m_fVCoreAMaxThreshold	= (float)1.80;

	m_fVCoreBMinThreshold	= (float)1.20;
	m_fVCoreBMaxThreshold	= (float)1.80;

	m_f12VMinThreshold		= (float)11.00;
	m_f12VMaxThreshold		= (float)13.00;

	m_f5VMinThreshold		= (float)4.0;
	m_f5VMaxThreshold		= (float)6.5;		

	m_f3_3VMinThreshold		= (float)3.00;
	m_f3_3VMaxThreshold		= (float)3.60;

	m_fN12VMinThreshold		= (float)-11.00;
	m_fN12VMaxThreshold		= (float)-13.00;

	m_fN5VMinThreshold		= (float)-4.00;
	m_fN5VMaxThreshold		= (float)-6.00;	

	m_hIcon			= AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hNotHealthy	= AfxGetApp()->LoadIcon(IDR_NOT_HEALTHY);
}

/////////////////////////////////////////////////////////////////////////////
void CHealthControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_FAN1_RPM, m_nFan1RPM);
	DDX_Text(pDX, IDC_FAN2_RPM, m_nFan2RPM);
	DDX_Text(pDX, IDC_FAN3_RPM, m_nFan3RPM);
	DDX_Text(pDX, IDC_TEMP1, m_nTemperature1);
	DDX_Text(pDX, IDC_TEMP2, m_nTemperature2);
	DDX_Text(pDX, IDC_TEMP3, m_nTemperature3);
	DDX_Text(pDX, IDC_VCOREA, m_fVCoreA);
	DDX_Text(pDX, IDC_VCOREB, m_fVCoreB);
	DDX_Text(pDX, IDC_12V, m_f12V);
	DDX_Text(pDX, IDC_N5V, m_fN5V);
	DDX_Text(pDX, IDC_3_3V, m_f3_3V);
	DDX_Text(pDX, IDC_5V, m_f5V);
	DDX_Text(pDX, IDC_N12V, m_fN12V);
	BOOL bIcon = !theApp.m_bStandalone;
	DDX_Check(pDX, IDC_CK_STAND_ALONE, bIcon);
}

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CHealthControlDlg, CDialog)
	//{{AFX_MSG_MAP(CHealthControlDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_PARENTNOTIFY()
	ON_WM_CTLCOLOR()
	ON_WM_CREATE()
	ON_COMMAND(ID_SETTINGS, OnSettings)
	ON_UPDATE_COMMAND_UI(ID_SETTINGS, OnUpdateSettings)
	ON_MESSAGE(WM_TRAYCLICKED, OnTrayClicked)	
	ON_MESSAGE(WM_SELFCHECK, DoConfirmSelfcheck)
	ON_MESSAGE(WM_LANGUAGE_CHANGED, OnLanguageChanged)	
	ON_COMMAND(ID_ABOUT, OnAbout)
	ON_UPDATE_COMMAND_UI(ID_ABOUT, OnUpdateAbout)
	ON_BN_CLICKED(IDC_BTN_SAVE, OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_CK_STAND_ALONE, OnBnClickedCkStandAlone)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CHealthControlDlg Meldungshandler

/////////////////////////////////////////////////////////////////////////////
BOOL CHealthControlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// IDM_ABOUTBOX muss sich im Bereich der Systembefehle befinden.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
/*
	if (!theApp.m_bStandalone)
	{
		CString str;
		str.LoadString(AFX_IDS_HIDE);
		SetDlgItemText(IDOK, str);
		GetDlgItem(IDOK)->SetDlgCtrlID(IDCANCEL);
	}
*/
	// Symbol für dieses Dialogfeld festlegen. Wird automatisch erledigt
	//  wenn das Hauptfenster der Anwendung kein Dialogfeld ist
	SetIcon(m_hIcon, TRUE);			// Großes Symbol verwenden
	SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden

#if WKCLASSES == 1
	CWK_Profile			prof;
	m_bSendFanRTE	= GetProfInt(prof, REG_PATH_FAN, SEND_RTE, m_bSendFanRTE);
	GetProfInt(prof, REG_PATH_PC_HEALTH, POP_BALLOON, 0, TRUE);
	m_nFan1RpmThreshold = GetProfInt(prof, REG_PATH_FAN, FAN1_RPM_THRESHOLD, m_nFan1RpmThreshold);
	m_nFan2RpmThreshold = GetProfInt(prof, REG_PATH_FAN, FAN2_RPM_THRESHOLD, m_nFan2RpmThreshold);
	m_nFan3RpmThreshold = GetProfInt(prof, REG_PATH_FAN, FAN3_RPM_THRESHOLD, m_nFan3RpmThreshold);
	m_nFan1Device = GetProfInt(prof, REG_PATH_FAN, FAN1_DEVICE, m_nFan1Device);
	m_nFan2Device = GetProfInt(prof, REG_PATH_FAN, FAN2_DEVICE, m_nFan2Device);
	m_nFan3Device = GetProfInt(prof, REG_PATH_FAN, FAN3_DEVICE, m_nFan3Device);

	m_bSendTemperatureRTE	=GetProfInt(prof, REG_PATH_TEMPERATURE, SEND_RTE, m_bSendTemperatureRTE);
	m_nTemperature1Threshold = GetProfInt(prof, REG_PATH_TEMPERATURE, TEMP1_THRESHOLD, m_nTemperature1Threshold);
	m_nTemperature2Threshold = GetProfInt(prof, REG_PATH_TEMPERATURE, TEMP2_THRESHOLD, m_nTemperature2Threshold);
	m_nTemperature3Threshold = GetProfInt(prof, REG_PATH_TEMPERATURE, TEMP3_THRESHOLD, m_nTemperature3Threshold);
	m_nTemperature1Device    = GetProfInt(prof, REG_PATH_TEMPERATURE, TEMP1_DEVICE, m_nTemperature1Device);
	m_nTemperature2Device    = GetProfInt(prof, REG_PATH_TEMPERATURE, TEMP2_DEVICE, m_nTemperature2Device);
	m_nTemperature3Device    = GetProfInt(prof, REG_PATH_TEMPERATURE, TEMP3_DEVICE, m_nTemperature3Device);

	m_bSendVoltageRTE	=GetProfInt(prof, REG_PATH_VOLTAGE, SEND_RTE, m_bSendVoltageRTE);
	m_fVCoreAMinThreshold = (float)GetProfInt(prof, REG_PATH_VOLTAGE, VCOREAMINTHRESHOLD, m_fVCoreAMinThreshold*100.0)/100.0;
	m_fVCoreAMaxThreshold = (float)GetProfInt(prof, REG_PATH_VOLTAGE, VCOREAMAXTHRESHOLD, m_fVCoreAMaxThreshold*100.0)/100.0;

	m_fVCoreBMinThreshold = (float)GetProfInt(prof, REG_PATH_VOLTAGE, VCOREBMINTHRESHOLD, m_fVCoreBMinThreshold*100.0)/100.0;
	m_fVCoreBMaxThreshold = (float)GetProfInt(prof, REG_PATH_VOLTAGE, VCOREBMAXTHRESHOLD, m_fVCoreBMaxThreshold*100.0)/100.0;

	m_f12VMinThreshold = (float)GetProfInt(prof, REG_PATH_VOLTAGE, V12MINTHRESHOLD, m_f12VMinThreshold*100.0)/100.0;
	m_f12VMaxThreshold = (float)GetProfInt(prof, REG_PATH_VOLTAGE, V12MAXTHRESHOLD, m_f12VMaxThreshold*100.0)/100.0;

	m_f5VMinThreshold = (float)GetProfInt(prof, REG_PATH_VOLTAGE, V5MINTHRESHOLD, m_f5VMinThreshold*100.0)/100.0;
	m_f5VMaxThreshold = (float)GetProfInt(prof, REG_PATH_VOLTAGE, V5MAXTHRESHOLD, m_f5VMaxThreshold*100.0)/100.0;

	m_f3_3VMinThreshold = (float)GetProfInt(prof, REG_PATH_VOLTAGE, V3_3MINTHRESHOLD, m_f3_3VMinThreshold*100.0)/100.0;
	m_f3_3VMaxThreshold = (float)GetProfInt(prof, REG_PATH_VOLTAGE, V3_3MAXTHRESHOLD, m_f3_3VMaxThreshold*100.0)/100.0;

	m_fN12VMinThreshold = (float)GetProfInt(prof, REG_PATH_VOLTAGE, N12VMINTHRESHOLD, -m_fN12VMinThreshold*100.0)/-100.0;
	m_fN12VMaxThreshold = (float)GetProfInt(prof, REG_PATH_VOLTAGE, N12VMAXTHRESHOLD, -m_fN12VMaxThreshold*100.0)/-100.0;

	m_fN5VMinThreshold = (float)GetProfInt(prof, REG_PATH_VOLTAGE, N5VMINTHRESHOLD, -m_fN5VMinThreshold*100.0)/-100.0;
	m_fN5VMaxThreshold = (float)GetProfInt(prof, REG_PATH_VOLTAGE, N5VMAXTHRESHOLD, -m_fN5VMaxThreshold*100.0)/-100.0;
#endif
	m_pECMon = CreateMonitorObject();
	if (m_pECMon)
	{
		WK_TRACE(_T("Detecting %s\n"), m_pECMon->GetIdentifier());

		if (m_pECMon->EnableMonitoring())
		{
			if (m_pECMon->GetMaxFans() >= 1)
			{
				GetDlgItem(IDC_STATIC_FAN1)->EnableWindow(TRUE);
				GetDlgItem(IDC_FAN1_RPM)->EnableWindow(TRUE);
				GetDlgItem(IDC_STATIC_FANUNIT1)->EnableWindow(TRUE);
				GetDlgItem(IDC_FAN1_RPM)->ModifyStyleEx(WS_EX_NOPARENTNOTIFY, 0);
			}

			if (m_pECMon->GetMaxFans() >= 2)
			{
				GetDlgItem(IDC_STATIC_FAN2)->EnableWindow(TRUE);
				GetDlgItem(IDC_FAN2_RPM)->EnableWindow(TRUE);
				GetDlgItem(IDC_FAN2_RPM)->ModifyStyleEx(WS_EX_NOPARENTNOTIFY, 0);
				GetDlgItem(IDC_STATIC_FANUNIT2)->EnableWindow(TRUE);
			}

			if (m_pECMon->GetMaxFans() >= 3)
			{
				GetDlgItem(IDC_STATIC_FAN3)->EnableWindow(TRUE);
				GetDlgItem(IDC_FAN3_RPM)->ModifyStyleEx(WS_EX_NOPARENTNOTIFY, 0);
				GetDlgItem(IDC_FAN3_RPM)->EnableWindow(TRUE);
				GetDlgItem(IDC_STATIC_FANUNIT3)->EnableWindow(TRUE);
			}

			GetDlgItem(IDC_STATIC_IDENTIFIER)->SetWindowText(m_pECMon->GetIdentifier());

			if (m_pECMon->GetMaxTemps() >= 1)
			{
				GetDlgItem(IDC_TEMP1)->EnableWindow(TRUE);
				GetDlgItem(IDC_TEMP1)->ModifyStyleEx(WS_EX_NOPARENTNOTIFY, 0);
				GetDlgItem(IDC_STATIC_TEMP1)->EnableWindow(TRUE);
				GetDlgItem(IDC_STATIC_CELSIUS1)->EnableWindow(TRUE);
			}
			if (m_pECMon->GetMaxTemps() >= 2)
			{
				GetDlgItem(IDC_TEMP2)->EnableWindow(TRUE);
				GetDlgItem(IDC_TEMP2)->ModifyStyleEx(WS_EX_NOPARENTNOTIFY, 0);
				GetDlgItem(IDC_STATIC_TEMP2)->EnableWindow(TRUE);
				GetDlgItem(IDC_STATIC_CELSIUS2)->EnableWindow(TRUE);
			}
			if (m_pECMon->GetMaxTemps() >= 3)
			{
				GetDlgItem(IDC_TEMP3)->EnableWindow(TRUE);
				GetDlgItem(IDC_TEMP3)->ModifyStyleEx(WS_EX_NOPARENTNOTIFY, 0);
				GetDlgItem(IDC_STATIC_TEMP3)->EnableWindow(TRUE);
				GetDlgItem(IDC_STATIC_CELSIUS3)->EnableWindow(TRUE);
			}
			if (m_pECMon->HasVCoreASensor())
			{
				GetDlgItem(IDC_STATIC_VCOREA)->EnableWindow(TRUE);
				GetDlgItem(IDC_VCOREA)->EnableWindow(TRUE);
				GetDlgItem(IDC_VCOREA)->ModifyStyleEx(WS_EX_NOPARENTNOTIFY, 0);
				GetDlgItem(IDC_STATIC_V1)->EnableWindow(TRUE);
			}
			if (m_pECMon->HasVCoreBSensor())
			{
				GetDlgItem(IDC_STATIC_VCOREB)->EnableWindow(TRUE);
				GetDlgItem(IDC_VCOREB)->EnableWindow(TRUE);
				GetDlgItem(IDC_VCOREB)->ModifyStyleEx(WS_EX_NOPARENTNOTIFY, 0);
				GetDlgItem(IDC_STATIC_V2)->EnableWindow(TRUE);
			}
			if (m_pECMon->Has12VSensor())
			{
				GetDlgItem(IDC_STATIC_12V)->EnableWindow(TRUE);
				GetDlgItem(IDC_12V)->EnableWindow(TRUE);
				GetDlgItem(IDC_12V)->ModifyStyleEx(WS_EX_NOPARENTNOTIFY, 0);
				GetDlgItem(IDC_STATIC_V3)->EnableWindow(TRUE);
			}
			if (m_pECMon->Has5VSensor())
			{
				GetDlgItem(IDC_STATIC_5V)->EnableWindow(TRUE);
				GetDlgItem(IDC_5V)->EnableWindow(TRUE);
				GetDlgItem(IDC_5V)->ModifyStyleEx(WS_EX_NOPARENTNOTIFY, 0);
				GetDlgItem(IDC_STATIC_V6)->EnableWindow(TRUE);
			}
			if (m_pECMon->Has3_3VSensor())
			{
				GetDlgItem(IDC_STATIC_3_3V)->EnableWindow(TRUE);
				GetDlgItem(IDC_3_3V)->EnableWindow(TRUE);
				GetDlgItem(IDC_3_3V)->ModifyStyleEx(WS_EX_NOPARENTNOTIFY, 0);
				GetDlgItem(IDC_STATIC_V5)->EnableWindow(TRUE);
			}
			if (m_pECMon->HasN12VSensor())
			{
				GetDlgItem(IDC_STATIC_N12V)->EnableWindow(TRUE);
				GetDlgItem(IDC_N12V)->EnableWindow(TRUE);
				GetDlgItem(IDC_N12V)->ModifyStyleEx(WS_EX_NOPARENTNOTIFY, 0);
				GetDlgItem(IDC_STATIC_V7)->EnableWindow(TRUE);
			}
			if (m_pECMon->HasN5VSensor())
			{
				GetDlgItem(IDC_STATIC_N5V)->EnableWindow(TRUE);
				GetDlgItem(IDC_N5V)->EnableWindow(TRUE);
				GetDlgItem(IDC_N5V)->ModifyStyleEx(WS_EX_NOPARENTNOTIFY, 0);
				GetDlgItem(IDC_STATIC_V4)->EnableWindow(TRUE);
			}

			SetTimer(1, 1000, NULL);
		}
	}
	UpdateNames();

	return TRUE;  // Geben Sie TRUE zurück, außer ein Steuerelement soll den Fokus erhalten
}
/////////////////////////////////////////////////////////////////////////////
void CHealthControlDlg::OnDestroy()
{
	KillTimer(1);
	
	WK_DELETE(m_pECMon);
	if (!theApp.m_bStandalone)
	{
		NOTIFYICONDATA tnd;

		tnd.cbSize	= sizeof(NOTIFYICONDATA);
		tnd.hWnd		= m_hWnd;
		tnd.uID		= 1;
		tnd.uFlags	= 0;

		Shell_NotifyIcon(NIM_DELETE, &tnd);
	}

	CDialog::OnDestroy();

	PostQuitMessage(0);
}

/////////////////////////////////////////////////////////////////////////////
void CHealthControlDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
#if OEMGUI == 1
		COemGuiApi::AboutDialog(this);
#else
      theApp.About();
#endif
	}
	else if (   !theApp.m_bStandalone 
		     && (nID == SC_MINIMIZE || nID == SC_CLOSE))
	{
		ShowWindow(SW_HIDE);
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CHealthControlDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // Gerätekontext zum Zeichnen

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Symbol in Clientrechteck zentrieren
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Symbol zeichnen
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// Die System ruft diese Funktion auf, um den Cursor abzufragen, der angezeigt wird, während der Benutzer
//  das minimierte Fenster mit der Maus zieht.
/////////////////////////////////////////////////////////////////////////////
HCURSOR CHealthControlDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
/////////////////////////////////////////////////////////////////////////////
void CHealthControlDlg::OnTimer(UINT nIDEvent)
{
	if (nIDEvent = 1)
	{
		BOOL bUpdate = FALSE;

		bUpdate |= CheckFans();
		bUpdate |= CheckTemperatures();
		bUpdate |= CheckVoltages();

		if (bUpdate)
		{
			UpdateData(FALSE);
			if (theApp.m_bIconCreated && m_nLastToolTips != m_saToolTip.GetSize())
			{
				UpdateToolTip();
			}
		}
	}
	CDialog::OnTimer(nIDEvent);
}

/////////////////////////////////////////////////////////////////////////////
CECBase* CHealthControlDlg::CreateMonitorObject()
{
	CECBase* pECMon = NULL;

	if (!pECMon)
	{
		WK_TRACE(_T("Searching for W83627...\n"));
		pECMon = new CW83627(WINBOND_BASE_ADDRESS);
		if (pECMon)
		{
			if (pECMon->Open())
			{
				if (pECMon->IsValid())
				{
					pECMon->WriteFanDivisors(32, 32, 32);
					pECMon->SetFanCountLimits(254, 254, 254);
				}
				else
					WK_DELETE(pECMon);
			}
			else
				WK_DELETE(pECMon);
		}
	}

	if (!pECMon)
	{
		WK_TRACE(_T("Searching for W83697...\n"));

		pECMon = new CW83697(WINBOND_BASE_ADDRESS);
		if (pECMon)
		{
			if (pECMon->Open())
			{
				if (pECMon->IsValid())
				{
					pECMon->WriteFanDivisors(32, 32, 32);
					pECMon->SetFanCountLimits(254, 254, 254);
				}
				else
					WK_DELETE(pECMon);
			}
			else
				WK_DELETE(pECMon);
		}
	}

	if (!pECMon)
	{
		WK_TRACE(_T("Searching for IT8712...\n"));

		pECMon = new CIT8712(ITE_BASE_ADDRESS);
		if (pECMon)
		{
			if (pECMon->Open())
			{
				if (pECMon->IsValid())
				{
					pECMon->WriteFanDivisors(8, 8, 0);
					pECMon->SetFanCountLimits(254, 254, 0);
				}
				else
					WK_DELETE(pECMon);
			}
			else
				WK_DELETE(pECMon);
		}
	}

	if (!pECMon)
	{
		pECMon = new CPC87372(PC87_BASE_ADDRESS);
		if (pECMon)
		{
			if (pECMon->Open())
			{
				if (pECMon->IsValid())
				{
					pECMon->WriteFanDivisors(1, 1, 0);
					pECMon->SetFanCountLimits(0xfffe, 0xfffe, 0);
				}
				else
					WK_DELETE(pECMon);

			}
			else
				WK_DELETE(pECMon);
		}
	}

	return pECMon;
}

///////////////////////////////////////////////////////////////////////////////
BOOL CHealthControlDlg::CheckFans()
{
	BOOL bResult = FALSE;

	if ((m_pECMon) && m_pECMon->IsValid())
	{
		if (m_pECMon->ReadFanSpeeds(m_nFan1RPM, m_nFan2RPM, m_nFan3RPM))
		{
			CString str;
			if (m_pECMon->GetMaxFans() > 0)
			{
				GetDlgItemText(IDC_STATIC_FAN1, str);
				CheckFan(m_nFan1RPM, m_nFan1RpmThreshold, m_nFan1Device, str, 0x01, m_nLastFan1RPM);
			}
			if (m_pECMon->GetMaxFans() > 1)
			{
				GetDlgItemText(IDC_STATIC_FAN2, str);
				CheckFan(m_nFan2RPM, m_nFan2RpmThreshold, m_nFan2Device, str, 0x02, m_nLastFan2RPM);
			}
			if (m_pECMon->GetMaxFans() > 2)
			{
				GetDlgItemText(IDC_STATIC_FAN3, str);
				CheckFan(m_nFan3RPM, m_nFan3RpmThreshold, m_nFan3Device, str, 0x04, m_nLastFan3RPM);
			}
			bResult = TRUE;
		}
	}

	return bResult;
}
///////////////////////////////////////////////////////////////////////////////
void CHealthControlDlg::CheckFan(int nRpm, int nMinRpm, int nDevice, LPCTSTR szValue, DWORD dwFlag, int &nLastRpm)
{
	if (nRpm != nLastRpm)
	{
		if (nLastRpm == VIRTUAL_ALARM_SET)
		{
			if (nRpm > nMinRpm+100)
			{
				DWORD dwParam1 = MAKELONG(nRpm, nDevice);
#if WKCLASSES == 1
				CWK_RunTimeError rte(WAI_HEALTH_CONTROL, REL_MESSAGE, RTE_FANCONTROL, _T(""), dwParam1);
				rte.Send();
#endif
				nLastRpm = nRpm;
				RemoveToolTip(szValue);
			}
		}
		else
		{
			nLastRpm = nRpm;
			if (nRpm < nMinRpm)
			{
				CString sErrorMsg;
				sErrorMsg.Format(IDS_RPM_TOO_LOW, szValue, nRpm, nMinRpm);
				m_saToolTip.Add(sErrorMsg);
				WK_TRACE_ERROR(sErrorMsg+_T("\n"));
				if (m_bSendFanRTE&dwFlag)
				{
					DWORD dwParam1 = MAKELONG(nRpm, nDevice);
					DWORD dwParam2 = MAKELONG(1|2, nMinRpm); // alarm on, show MsgBox
					if (!theApp.IsDts())
					{
						sErrorMsg.Empty();
					}
#if WKCLASSES == 1
					CWK_RunTimeError rte(WAI_HEALTH_CONTROL, REL_ERROR, RTE_FANCONTROL, sErrorMsg, dwParam1, dwParam2);
					rte.Send();
#endif
				}
				nLastRpm = VIRTUAL_ALARM_SET;
			}
		}
	}

}
///////////////////////////////////////////////////////////////////////////////
BOOL CHealthControlDlg::CheckTemperatures()
{
	BOOL bResult = FALSE;

	if ((m_pECMon) && m_pECMon->IsValid())
	{
		if (m_pECMon->ReadTemperatures(m_nTemperature1, m_nTemperature2, m_nTemperature3))
		{
			CString str;
			GetDlgItemText(IDC_STATIC_TEMP1, str);
			CheckTemperature(m_nTemperature1, m_nTemperature1Threshold, m_nTemperature1Device, str, 0x01, m_nLastTemperature1);
			GetDlgItemText(IDC_STATIC_TEMP2, str);
			CheckTemperature(m_nTemperature2, m_nTemperature2Threshold, m_nTemperature2Device, str, 0x02, m_nLastTemperature2);
			GetDlgItemText(IDC_STATIC_TEMP3, str);
			CheckTemperature(m_nTemperature3, m_nTemperature3Threshold, m_nTemperature3Device, str, 0x04, m_nLastTemperature3);
			bResult = TRUE;
		}
	}
/*
tooltip 
			CString str, sValue, sUnit;
			CWnd *pNext = pWnd->GetNextWindow(GW_HWNDNEXT);
			if (pNext)
			{
				pNext->GetWindowText(sUnit);
			}
			CWnd *pPrev = pWnd->GetNextWindow(GW_HWNDPREV);
			if (pPrev)
			{
				pPrev->GetWindowText(sValue);
			}
			pWnd->GetWindowText(str);
			m_sToolTip += _T("\n") + sValue + _T(": ") + str + _T(" ") + sUnit;
*/

	return bResult;
}
///////////////////////////////////////////////////////////////////////////////
void CHealthControlDlg::CheckTemperature(int nTemp, int nMaxTemp, int nDevice, LPCTSTR szValue, DWORD dwFlag, int &nLastTemp)
{
	if (nTemp != nLastTemp)
	{
		if (nLastTemp == VIRTUAL_ALARM_SET)
		{
			if (nTemp < nMaxTemp-5)
			{
				nLastTemp = nTemp;
				if (m_bSendTemperatureRTE&dwFlag)
				{
					DWORD dwParam1 = MAKELONG(nTemp, nDevice);
#if WKCLASSES == 1
					CWK_RunTimeError rte(WAI_HEALTH_CONTROL, REL_MESSAGE, RTE_TEMPERATURE, _T(""), dwParam1);
					rte.Send();
#endif
					RemoveToolTip(szValue);
				}
			}
		}
		else
		{
			nLastTemp = nTemp;
			if (nTemp > nMaxTemp)
			{
				CString sErrorMsg;
				sErrorMsg.Format(IDS_TEMP_TOO_HIGH, szValue, nTemp, nMaxTemp);
				m_saToolTip.Add(sErrorMsg);
				WK_TRACE_ERROR(sErrorMsg+_T("\n"));
				if (m_bSendTemperatureRTE&dwFlag)
				{
					DWORD dwParam1 = MAKELONG(nTemp, nDevice);
					DWORD dwParam2 = MAKELONG(1|2, nMaxTemp); // alarm on, show MsgBox
					if (!theApp.IsDts())
					{
						sErrorMsg.Empty();
					}
#if WKCLASSES == 1
					CWK_RunTimeError rte(WAI_HEALTH_CONTROL, REL_ERROR, RTE_TEMPERATURE, sErrorMsg, dwParam1, dwParam2);
					rte.Send();
#endif
				}
				nLastTemp = VIRTUAL_ALARM_SET;
			}
		}
	}
}
///////////////////////////////////////////////////////////////////////////////
BOOL CHealthControlDlg::CheckVoltages()
{
	BOOL bResult = FALSE;

	if ((m_pECMon) && m_pECMon->IsValid())
	{
		if (m_pECMon->ReadVoltages(m_fVCoreA, m_fVCoreB, m_f3_3V, m_f5V, m_f12V, m_fN12V, m_fN5V))
		{
			// VCOREA out of range
			if (  m_pECMon->HasVCoreASensor()) 
			{
				CheckVoltage(m_fVCoreA, m_fVCoreAMinThreshold, m_fVCoreAMaxThreshold, _T("VCoreA"), 0x01, m_bVCoreATresholdFailureReported);
			}

			// VCOREB out of range
			if (m_pECMon->HasVCoreASensor()) 
			{
				CheckVoltage(m_fVCoreB, m_fVCoreBMinThreshold, m_fVCoreBMaxThreshold, _T("VCoreB"), 0x02, m_bVCoreBTresholdFailureReported);
			}

			// 3.3V out of range
			if (m_pECMon->Has3_3VSensor())
			{
				CheckVoltage(m_f3_3V, m_f3_3VMinThreshold, m_f3_3VMaxThreshold, _T("3.3 V"), 0x04, m_b3_3VTresholdFailureReported);
			}

			// 5V out of range
			if (m_pECMon->Has5VSensor())
			{
				CheckVoltage(m_f5V, m_f5VMinThreshold, m_f5VMaxThreshold, _T("5 V"), 0x08, m_b5VTresholdFailureReported);
			}

			// -5V out of range
			if (m_pECMon->HasN5VSensor())
			{
				CheckVoltage(-m_fN5V, -m_fN5VMinThreshold, -m_fN5VMaxThreshold, _T("- 5 V"), 0x10, m_bN5VTresholdFailureReported);
			}

			// 12V out of range
			if (m_pECMon->Has12VSensor())
			{
				CheckVoltage(m_f12V, m_f12VMinThreshold, m_f12VMaxThreshold, _T("12 V"), 0x20, m_b12VTresholdFailureReported);
			}

			// -12V out of range
			if (m_pECMon->HasN12VSensor())
			{
				CheckVoltage(-m_fN12V, -m_fN12VMinThreshold, -m_fN12VMaxThreshold, _T("-12 V"), 0x40, m_bN12VTresholdFailureReported);
			}

			bResult = TRUE;
		}
	}

	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
void CHealthControlDlg::CheckVoltage(float fV, float fVmin, float fVmax, LPCTSTR szValue, DWORD dwFlag, BOOL &bTresholdFailureReported)
{
	if (bTresholdFailureReported == FALSE)
	{
		if (!IsBetween(fV, fVmin, fVmax))
		{
			bTresholdFailureReported = TRUE;
			CString sErrorMsg;
			sErrorMsg.Format(IDS_VOLTAGE_WRONG, szValue, fVmin, fV, fVmax);
			m_saToolTip.Add(sErrorMsg);
			if (m_bSendVoltageRTE&dwFlag)
			{
				WK_TRACE_ERROR(sErrorMsg + _T("\n"));
				DWORD dwParam1 = MAKELONG((int)(fV*10), (int)(fVmin*10));
				DWORD dwParam2 = MAKELONG(1|2, (int)(fVmax*10)); // alarm on, show MsgBox
#if WKCLASSES == 1
				CWK_RunTimeError rte(WAI_HEALTH_CONTROL, REL_ERROR, RTE_VOLTAGE, sErrorMsg, dwParam1, dwParam2);
				rte.Send();
#endif
			}
		}
	}
	else
	{
		float fT = fVmax* VOLTAGE_TRESHOLD;
		if (IsBetween(fV, fVmin+fT, fVmax-fT))
		{
			if (m_bSendVoltageRTE&dwFlag)
			{
#if WKCLASSES == 1
				CWK_RunTimeError rte(WAI_HEALTH_CONTROL, REL_MESSAGE, RTE_VOLTAGE, _T(""));
				rte.Send();
#endif
			}
			RemoveToolTip(szValue);
			bTresholdFailureReported = FALSE;
		}
	}
}
///////////////////////////////////////////////////////////////////////////////
#if WKCLASSES == 1
int CHealthControlDlg::GetProfInt(CWK_Profile& wkp, LPCTSTR szSection, LPCTSTR szValue, int nDefault, BOOL bWrite/*=FALSE*/)
{
	int nValue = wkp.GetInt(szSection, szValue, -1);
	if (nValue == -1)
	{
		nValue = nDefault;
		if (bWrite)
		{
			wkp.WriteInt(szSection, szValue, nValue);
		}
	}
	return nValue;	
}
#endif
///////////////////////////////////////////////////////////////////////////////
void CHealthControlDlg::OnParentNotify(UINT message, LPARAM lParam)
{
	CDialog::OnParentNotify(message, lParam);
	CWnd *pWnd = ChildWindowFromPoint(CPoint(lParam));
	if (pWnd)
	{
		CConfigDlg dlg;
		BOOL bDoModal = TRUE;
		switch(pWnd->GetDlgCtrlID())
		{
			case IDC_TEMP1:
				dlg.m_pnDevice   = &m_nTemperature1Device;
				dlg.m_pnMaxValue = &m_nTemperature1Threshold;
				dlg.m_pbSendRTE  = &m_bSendTemperatureRTE;
				dlg.m_nDeviceNum = 0;
				break;
			case IDC_TEMP2:
				dlg.m_pnDevice   = &m_nTemperature2Device;
				dlg.m_pnMaxValue = &m_nTemperature2Threshold;
				dlg.m_pbSendRTE  = &m_bSendTemperatureRTE;
				dlg.m_nDeviceNum = 1;
				break;
			case IDC_TEMP3:
				dlg.m_pnDevice   = &m_nTemperature3Device;
				dlg.m_pnMaxValue = &m_nTemperature3Threshold;
				dlg.m_pbSendRTE  = &m_bSendTemperatureRTE;
				dlg.m_nDeviceNum = 2;
				break;
			case IDC_FAN1_RPM:
				dlg.m_pnDevice   = &m_nFan1Device;
				dlg.m_pnMinValue = &m_nFan1RpmThreshold;
				dlg.m_pbSendRTE  = &m_bSendFanRTE;
				dlg.m_nDeviceNum = 0;
				break;
			case IDC_FAN2_RPM:
				dlg.m_pnDevice   = &m_nFan2Device;
				dlg.m_pnMinValue = &m_nFan2RpmThreshold;
				dlg.m_pbSendRTE  = &m_bSendFanRTE;
				dlg.m_nDeviceNum = 1;
				break;
			case IDC_FAN3_RPM:
				dlg.m_pnDevice   = &m_nFan3Device;
				dlg.m_pnMinValue = &m_nFan3RpmThreshold;
				dlg.m_pbSendRTE  = &m_bSendFanRTE;
				dlg.m_nDeviceNum = 2;
				break;
			case IDC_VCOREA:
				dlg.m_pfMinValue = &m_fVCoreAMinThreshold;
				dlg.m_pfMaxValue = &m_fVCoreAMaxThreshold;
				dlg.m_pbSendRTE  = &m_bSendVoltageRTE;
				dlg.m_nDeviceNum = 0;
				break;
			case IDC_VCOREB:
				dlg.m_pfMinValue = &m_fVCoreBMinThreshold;
				dlg.m_pfMaxValue = &m_fVCoreBMaxThreshold;
				dlg.m_pbSendRTE  = &m_bSendVoltageRTE;
				dlg.m_nDeviceNum = 1;
				break;
			case IDC_3_3V:
				dlg.m_pfMinValue = &m_f3_3VMinThreshold;
				dlg.m_pfMaxValue = &m_f3_3VMaxThreshold;
				dlg.m_pbSendRTE  = &m_bSendVoltageRTE;
				dlg.m_nDeviceNum = 2;
				break;
			case IDC_5V:
				dlg.m_pfMinValue = &m_f5VMinThreshold;
				dlg.m_pfMaxValue = &m_f5VMaxThreshold;
				dlg.m_pbSendRTE  = &m_bSendVoltageRTE;
				dlg.m_nDeviceNum = 3;
				break;
			case IDC_N5V:
				dlg.m_pfMinValue = &m_fN5VMinThreshold;
				dlg.m_pfMaxValue = &m_fN5VMaxThreshold;
				dlg.m_pbSendRTE  = &m_bSendVoltageRTE;
				dlg.m_nDeviceNum = 4;
				break;
			case IDC_12V:
				dlg.m_pfMinValue = &m_f12VMinThreshold;
				dlg.m_pfMaxValue = &m_f12VMaxThreshold;
				dlg.m_pbSendRTE  = &m_bSendVoltageRTE;
				dlg.m_nDeviceNum = 5;
				break;
			case IDC_N12V:
				dlg.m_pfMinValue = &m_fN12VMinThreshold;
				dlg.m_pfMaxValue = &m_fN12VMaxThreshold;
				dlg.m_pbSendRTE  = &m_bSendVoltageRTE;
				dlg.m_nDeviceNum = 6;
				break;
			default:
				bDoModal = FALSE;
				break;
		}
		if (bDoModal)
		{
			CWnd *pNext = pWnd->GetNextWindow(GW_HWNDNEXT);
			if (pNext)
			{
				pNext->GetWindowText(dlg.m_sUnit);
			}
			CWnd *pPrev = pWnd->GetNextWindow(GW_HWNDPREV);
			if (pPrev)
			{
				pPrev->GetWindowText(dlg.m_sValue);
			}
			if (dlg.DoModal() == IDOK)
			{
				GetDlgItem(IDC_BTN_SAVE)->EnableWindow(TRUE);
				UpdateNames();
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CHealthControlDlg::OnBnClickedCkStandAlone()
{
	CDataExchange dx(this, TRUE);
	BOOL bIcon;
	DDX_Check(&dx, IDC_CK_STAND_ALONE, bIcon);
	theApp.m_bStandalone = !bIcon;
	GetDlgItem(IDC_BTN_SAVE)->EnableWindow(TRUE);
}
/////////////////////////////////////////////////////////////////////////////
void CHealthControlDlg::UpdateNames()
{
	CString str;

	str.Format(IDS_FAN, 1);
	if (m_nFan1Device)
		str.LoadString(IDS_DEVICE_UNKNOWN + m_nFan1Device);
	SetDlgItemText(IDC_STATIC_FAN1, str);

	str.Format(IDS_FAN, 2);
	if (m_nFan2Device)
		str.LoadString(IDS_DEVICE_UNKNOWN + m_nFan2Device);
	SetDlgItemText(IDC_STATIC_FAN2, str);

	str.Format(IDS_FAN, 3);
	if (m_nFan3Device)
		str.LoadString(IDS_DEVICE_UNKNOWN + m_nFan3Device);
	SetDlgItemText(IDC_STATIC_FAN3, str);
	

	str.Format(IDS_TEMPERATURE, 1);
	if (m_nTemperature1Device)
		str.LoadString(IDS_DEVICE_UNKNOWN + m_nTemperature1Device);
	SetDlgItemText(IDC_STATIC_TEMP1, str);

	str.Format(IDS_TEMPERATURE, 2);
	if (m_nTemperature2Device)
		str.LoadString(IDS_DEVICE_UNKNOWN + m_nTemperature2Device);
	SetDlgItemText(IDC_STATIC_TEMP2, str);

	str.Format(IDS_TEMPERATURE, 3);
	if (m_nTemperature3Device)
		str.LoadString(IDS_DEVICE_UNKNOWN + m_nTemperature3Device);
	SetDlgItemText(IDC_STATIC_TEMP3, str);
}
/////////////////////////////////////////////////////////////////////////////
void CHealthControlDlg::OnOK()
{
	CDialog::OnOK();
	PostMessage(WM_DESTROY);
}
/////////////////////////////////////////////////////////////////////////////
void CHealthControlDlg::OnCancel()
{
	CDialog::OnCancel();
	if (theApp.m_bStandalone)
	{
		PostMessage(WM_DESTROY);
	}
	else
	{
		ShowWindow(SW_HIDE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CHealthControlDlg::OnBnClickedBtnSave()
{
#if WKCLASSES == 1
	CWK_Profile			prof;
	prof.WriteInt(REG_PATH_FAN, SEND_RTE, m_bSendFanRTE);
	prof.WriteInt(REG_PATH_FAN, FAN1_RPM_THRESHOLD, m_nFan1RpmThreshold);
	prof.WriteInt(REG_PATH_FAN, FAN2_RPM_THRESHOLD, m_nFan2RpmThreshold);
	prof.WriteInt(REG_PATH_FAN, FAN3_RPM_THRESHOLD, m_nFan3RpmThreshold);
	prof.WriteInt(REG_PATH_FAN, FAN1_DEVICE, m_nFan1Device);
	prof.WriteInt(REG_PATH_FAN, FAN2_DEVICE, m_nFan2Device);
	prof.WriteInt(REG_PATH_FAN, FAN3_DEVICE, m_nFan3Device);

	prof.WriteInt(REG_PATH_TEMPERATURE, SEND_RTE, m_bSendTemperatureRTE);
	prof.WriteInt(REG_PATH_TEMPERATURE, TEMP1_THRESHOLD, m_nTemperature1Threshold);
	prof.WriteInt(REG_PATH_TEMPERATURE, TEMP2_THRESHOLD, m_nTemperature2Threshold);
	prof.WriteInt(REG_PATH_TEMPERATURE, TEMP3_THRESHOLD, m_nTemperature3Threshold);
	prof.WriteInt(REG_PATH_TEMPERATURE, TEMP1_DEVICE, m_nTemperature1Device);
	prof.WriteInt(REG_PATH_TEMPERATURE, TEMP2_DEVICE, m_nTemperature2Device);
	prof.WriteInt(REG_PATH_TEMPERATURE, TEMP3_DEVICE, m_nTemperature3Device);

	prof.WriteInt(REG_PATH_VOLTAGE, SEND_RTE, m_bSendVoltageRTE);
	prof.WriteInt(REG_PATH_VOLTAGE, VCOREAMINTHRESHOLD, m_fVCoreAMinThreshold*100.0);
	prof.WriteInt(REG_PATH_VOLTAGE, VCOREAMAXTHRESHOLD, m_fVCoreAMaxThreshold*100.0);

	prof.WriteInt(REG_PATH_VOLTAGE, VCOREBMINTHRESHOLD, m_fVCoreBMinThreshold*100.0);
	prof.WriteInt(REG_PATH_VOLTAGE, VCOREBMAXTHRESHOLD, m_fVCoreBMaxThreshold*100.0);

	prof.WriteInt(REG_PATH_VOLTAGE, V12MINTHRESHOLD, m_f12VMinThreshold*100.0);
	prof.WriteInt(REG_PATH_VOLTAGE, V12MAXTHRESHOLD, m_f12VMaxThreshold*100.0);

	prof.WriteInt(REG_PATH_VOLTAGE, V5MINTHRESHOLD, m_f5VMinThreshold*100.0);
	prof.WriteInt(REG_PATH_VOLTAGE, V5MAXTHRESHOLD, m_f5VMaxThreshold*100.0);

	prof.WriteInt(REG_PATH_VOLTAGE, V3_3MINTHRESHOLD, m_f3_3VMinThreshold*100.0);
	prof.WriteInt(REG_PATH_VOLTAGE, V3_3MAXTHRESHOLD, m_f3_3VMaxThreshold*100.0);

	prof.WriteInt(REG_PATH_VOLTAGE, N12VMINTHRESHOLD, -m_fN12VMinThreshold*100.0);
	prof.WriteInt(REG_PATH_VOLTAGE, N12VMAXTHRESHOLD, -m_fN12VMaxThreshold*100.0);

	prof.WriteInt(REG_PATH_VOLTAGE, N5VMINTHRESHOLD, -m_fN5VMinThreshold*100.0);
	prof.WriteInt(REG_PATH_VOLTAGE, N5VMAXTHRESHOLD, -m_fN5VMaxThreshold*100.0);

	prof.WriteInt(REG_PATH_PC_HEALTH, STAND_ALONE, theApp.m_bStandalone);
	if (theApp.m_bStandalone == 0)
	{
		prof.WriteString(SECTION_LAUNCHER_MODULES, WK_APP_NAME_HEALT_CONTROL, WK_APP_NAME_HEALT_CONTROL _T(".exe"));
	}
	else
	{
		prof.DeleteEntry(SECTION_LAUNCHER_MODULES, WK_APP_NAME_HEALT_CONTROL);
	}

	GetDlgItem(IDC_BTN_SAVE)->EnableWindow(FALSE);
	if	(!theApp.m_bStandalone && theApp.m_bIconCreated)
	{
		ShowWindow(SW_HIDE);
	}
	CFileDialog dialog(FALSE, _T("reg"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Registry Datei (*.reg)|*.reg||"), this);
	if (dialog.DoModal() == IDOK)
	{
		HKEY hKey = prof.GetSectionKey(REG_PATH_PC_HEALTH);
		CString sSection, sFileName = dialog.GetFileName(), sBase;
		sBase.Format(_T("%s\\%s\\%s"), HKEY_LOCAL_MACHINE_NAME, SOFTWARE_KEY, WK_PROFILE_ROOT);
		sSection = sBase + REG_PATH_PC_HEALTH;
		prof.SaveAs(sFileName, sSection, 0, hKey);
		CStdioFile file;
		if (file.Open(sFileName, CFile::modeWrite))
		{
			sBase += SECTION_LAUNCHER_MODULES;
			sSection.Format(_T("\r\n[%s]\r\n"), sBase);
			file.SeekToEnd();
			file.WriteString(sSection);
			sSection.Format(_T("\"%s\"=\"%s.exe\""), WK_APP_NAME_HEALT_CONTROL, WK_APP_NAME_HEALT_CONTROL);
			file.WriteString(sSection);
		}
	}
#endif
}
/////////////////////////////////////////////////////////////////////////////
HBRUSH CHealthControlDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	if (CTLCOLOR_STATIC == nCtlColor)
	{
		BOOL bRed = FALSE;
		switch(pWnd->GetDlgCtrlID())
		{
			case IDC_TEMP1:
				if (m_nLastTemperature1 == VIRTUAL_ALARM_SET)	bRed = TRUE; 
				break;
			case IDC_TEMP2:
				if (m_nLastTemperature2 == VIRTUAL_ALARM_SET)	bRed = TRUE; 
				break;
			case IDC_TEMP3:
				if (m_nLastTemperature3 == VIRTUAL_ALARM_SET)	bRed = TRUE; 
				break;
			case IDC_FAN1_RPM:
				if (m_nLastFan1RPM == VIRTUAL_ALARM_SET)		bRed = TRUE;
				break;
			case IDC_FAN2_RPM:
				if (m_nLastFan2RPM == VIRTUAL_ALARM_SET)		bRed = TRUE;
				break;
			case IDC_FAN3_RPM:
				if (m_nLastFan3RPM == VIRTUAL_ALARM_SET)		bRed = TRUE;
				break;
			case IDC_VCOREA:
				if (m_bVCoreATresholdFailureReported)			bRed = TRUE;
				break;
			case IDC_VCOREB:
				if (m_bVCoreBTresholdFailureReported)			bRed = TRUE;
				break;
			case IDC_3_3V:
				if (m_b3_3VTresholdFailureReported)				bRed = TRUE;
				break;
			case IDC_5V:
				if (m_b5VTresholdFailureReported)				bRed = TRUE;
				break;
			case IDC_N5V:
				if (m_bN5VTresholdFailureReported)				bRed = TRUE;
				break;
			case IDC_12V:
				if (m_b12VTresholdFailureReported)				bRed = TRUE;
				break;
			case IDC_N12V:
				if (m_bN12VTresholdFailureReported)				bRed = TRUE;
				break;
		}
		if (bRed)
		{
			pDC->SetTextColor(RGB(255, 0,0));
		}
	}
	return hbr;
}
/////////////////////////////////////////////////////////////////////////////
int CHealthControlDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (theApp.m_bStandalone)
	{	
		SetWindowPos(&CWnd::wndTop, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);
	}
	else
	{
		// Icon ins Systemtray
		NOTIFYICONDATA tnd;

		tnd.cbSize = sizeof(NOTIFYICONDATA);
		tnd.hWnd	  = m_hWnd;
		tnd.uID    = 1;
		tnd.hIcon  = m_hIcon;
		tnd.uFlags = NIF_MESSAGE|NIF_ICON;
		tnd.uCallbackMessage = WM_TRAYCLICKED;

		tnd.uFlags |= NIF_TIP;
		lstrcpyn(tnd.szTip, theApp.m_pszAppName, 128);
		theApp.m_bIconCreated = Shell_NotifyIcon(NIM_ADD, &tnd);
	}

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CHealthControlDlg::RemoveToolTip(LPCTSTR szValue)
{
	int i, n = m_saToolTip.GetSize();
	for (i=0; i<n; i++)
	{
		if (m_saToolTip.GetAt(i).Find(szValue) != -1)
		{
			m_saToolTip.RemoveAt(i);
			break;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CHealthControlDlg::UpdateToolTip()
{
	NOTIFYICONDATA tnd;
	ZERO_INIT(tnd);
	BOOL bBalloon = FALSE;

	tnd.cbSize = sizeof(NOTIFYICONDATA);
	tnd.hWnd	  = m_hWnd;
	tnd.uID    = 1;
	tnd.uFlags = NIF_ICON;
	CString str;
	int i, n = m_saToolTip.GetSize();
	if (m_saToolTip.GetSize())
	{
		tnd.hIcon  = m_hNotHealthy;
		CString sLF = _T("\n");
		for (i=0; i<n; i++)
		{
			str += m_saToolTip.GetAt(i);
			if (i<n-1)
			{
				str += sLF;
			}
		}
#if WKCLASSES == 1
		CWK_Profile wkp;
		if (   !theApp.IsShellMode()
			&& wkp.GetInt(REG_PATH_PC_HEALTH, POP_BALLOON, FALSE))
		{
			bBalloon = TRUE;
		}
#endif
	}
	else
	{
		tnd.hIcon = m_hIcon;
		str = theApp.m_pszExeName;
	}
	SetIcon(tnd.hIcon, TRUE);
	SetIcon(tnd.hIcon, FALSE);

	if (bBalloon)
	{
		tnd.uFlags |= NIF_INFO;
		int nSize = sizeof(tnd.szInfoTitle)/sizeof(tnd.szInfoTitle[0]);
		lstrcpyn(tnd.szInfoTitle, theApp.m_pszAppName, nSize);
		nSize = sizeof(tnd.szInfo)/sizeof(tnd.szInfo[0]);
		lstrcpyn(tnd.szInfo, str, nSize);
		tnd.uTimeout = 10;
		tnd.dwInfoFlags = NIIF_WARNING;
	}
	else
	{
		tnd.uFlags |= NIF_TIP;
		int nSize = sizeof(tnd.szTip)/sizeof(tnd.szTip[0]);
		lstrcpyn(tnd.szTip, str, nSize);
	}
	Shell_NotifyIcon(NIM_MODIFY, &tnd);
	m_nLastToolTips = n;
}
/////////////////////////////////////////////////////////////////////////////
void CHealthControlDlg::OnSettings()
{
   if (!theApp.m_bStandalone)
	{
		SetWindowPos(&CWnd::wndTop, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);
		SetForegroundWindow(); 
	}
}
/////////////////////////////////////////////////////////////////////////////
void CHealthControlDlg::OnUpdateSettings(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CHealthControlDlg::OnTrayClicked(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
		case WM_RBUTTONDOWN:
		{
			CMenu menu;
			CMenu* pM;
			CPoint pt;

			GetCursorPos(&pt);
			menu.LoadMenu(IDR_MAINFRAME);
			pM = menu.GetSubMenu(0);

			SetForegroundWindow();		// Siehe ID: Q135788 
			pM->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this);
			PostMessage(WM_NULL, 0, 0); // Siehe ID: Q135788
			break;
		}
	}

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
long CHealthControlDlg::DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam)
{
	HWND hWnd = (HWND)wParam;

#if WKCLASSES == 1
	if (hWnd && IsWindow(hWnd))
	{
		::PostMessage(hWnd, WM_SELFCHECK, WAI_HEALTH_CONTROL, 0);
	}
#endif
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CHealthControlDlg::OnLanguageChanged(WPARAM wParam, LPARAM lParam)
{
#if _MFC_VER >= 0x0710
	LRESULT lResult = theApp.SetLanguageParameters((UINT)wParam, (DWORD)lParam);
	UpdateNames();
	return lResult;
#else
	return 0;
#endif

}
/////////////////////////////////////////////////////////////////////////////
void CHealthControlDlg::OnAbout()
{
#if OEMGUI == 1
	COemGuiApi::AboutDialog(this);
#else
      theApp.About();
#endif
}
/////////////////////////////////////////////////////////////////////////////
void CHealthControlDlg::OnUpdateAbout(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
}
