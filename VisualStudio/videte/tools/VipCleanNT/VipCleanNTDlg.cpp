// VipCleanNTDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VipCleanNT.h"
#include "VipCleanNTDlg.h"
#include <HardwareInfo\\CBoards.h>
#include ".\vipcleanntdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SETBIT(w,b)    ((DWORD)((DWORD)(w) | (1L << (b))))
#define CLRBIT(w,b)    ((DWORD)((DWORD)(w) & ~(1L << (b))))
#define TSTBIT(w,b)    ((BOOL)((DWORD)(w) & (1L << (b)) ? TRUE : FALSE))

#define TOGGLEBIT(w,b) (TSTBIT(w,b) ? CLRBIT(w,b) : SETBIT(w,b))

#define ORGA_SIEMENS		_T("Siemens")
#define ORGA_SIEMENS_TYPE	_T("VDR")
#define ORGA_SANYO			_T("Sanyo Video Vertriebs GmbH & Co")
#define ORGA_SANYO_TYPE		_T("SDR")
#define ORGA_VIDETE			_T("videte IT")
#define ORGA_VIDETE_TYPE	_T("DTS")


/////////////////////////////////////////////////////////////////////////////
// CVipCleanNTDlg dialog
CVipCleanNTDlg::CVipCleanNTDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVipCleanNTDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVipCleanNTDlg)
	m_sOEM1 = _T("");
	m_sOEM2 = _T("");
	m_sOEM3 = _T("");
	m_shostname = _T("");
	m_del = _T("");
	m_iLoaderTimeout = 0;
	m_sScreen = _T("");
	m_sTempDir = _T("");
	m_bISDN = FALSE;
	m_bNet = FALSE;
	m_bBackup = FALSE;
	m_sHz = _T("");
	m_sOrgaPullDown = _T("");
	m_sOrgaCombo = _T("");
	m_sXPeLicense = _T("");
	m_sNero2 = _T("");
	m_sNero3 = _T("");
	m_sNero4 = _T("");
	m_sNero5 = _T("");
	m_sNero6 = _T("");
	m_sDisplay = _T("");
	m_sNero1 = _T("");
	m_bBackupDVD = FALSE;
	m_iNrOfAllowedQUnitCams = 4;
	m_sStaticNeroVersion = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bDirDeleted = FALSE;
	m_bRegAktual  = TRUE;
	m_bSaveAsSerialNumberKey = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVipCleanNTDlg)
	DDX_Control(pDX, IDC_KAM16_TASHA, m_StaticKam16_Tasha);
	DDX_Control(pDX, IDC_KAM8_TASHA, m_StaticKam8_Tasha);
	DDX_Control(pDX, IDC_CHECK_DVD, m_ctrlCheckDVD);
	DDX_Control(pDX, IDC_CHECK_BACKUP, m_ctrlCheckBackup);
	DDX_Control(pDX, IDC_STATICNERO, m_StaticNero);
	DDX_Control(pDX, IDC_NERO6, m_CtrlNero6);
	DDX_Control(pDX, IDC_NERO5, m_CtrlNero5);
	DDX_Control(pDX, IDC_NERO4, m_CtrlNero4);
	DDX_Control(pDX, IDC_NERO3, m_CtrlNero3);
	DDX_Control(pDX, IDC_NERO2, m_CtrlNero2);
	DDX_Control(pDX, IDC_NERO1, m_CtrlNero1);
	DDX_Control(pDX, IDC_STATICXPE, m_StaticXPeLicense);
	DDX_Control(pDX, IDC_EDIT_XPE, m_CtrlXPeLicense);
	DDX_Control(pDX, IDC_TEMP, m_EditTemp);
	DDX_Control(pDX, IDC_STATICTEMP, m_StaticTemp);
	DDX_Control(pDX, IDC_KAM16, m_StaticKam16);
	DDX_Control(pDX, IDC_KAM8, m_StaticKam8);
	DDX_Control(pDX, IDC_KAM4, m_StaticKam4);
	DDX_Control(pDX, IDC_STATICOEM2, m_StaticOEM2);
	DDX_Control(pDX, IDC_STATICOEM1, m_StaticOEM1);
	DDX_Control(pDX, IDC_ORGA_COMBO, m_editOrgaCombo);
	DDX_Control(pDX, IDC_PING_OK, m_CtrlPingOK);
	DDX_Control(pDX, IDC_PING_NOTYET, m_CtrlPingNotYet);
	DDX_Control(pDX, IDC_IPADDRESS1, m_IP);
	DDX_Control(pDX, IDC_LOADERTIMEOUT, m_LoaderTimeout);
	DDX_Control(pDX, IDC_HOSTNAME, m_Hostname);
	DDX_Control(pDX, IDC_WRITEREG, m_writeReg);
	DDX_Control(pDX, IDC_OEM3, m_editOEM3);
	DDX_Control(pDX, IDC_OEM2, m_editOEM2);
	DDX_Control(pDX, IDC_OEM1, m_editOEM1);
	DDX_Text(pDX, IDC_OEM1, m_sOEM1);
	DDV_MaxChars(pDX, m_sOEM1, 5);
	DDX_Text(pDX, IDC_OEM2, m_sOEM2);
	DDV_MaxChars(pDX, m_sOEM2, 7);
	DDX_Text(pDX, IDC_OEM3, m_sOEM3);
	DDV_MaxChars(pDX, m_sOEM3, 5);
	DDX_Text(pDX, IDC_HOSTNAME, m_shostname);
	DDX_Text(pDX, IDC_DEL, m_del);
	DDX_Control(pDX, IDC_RADIO4KAM, m_Radio4);
	DDX_Control(pDX, IDC_RADIO8KAM, m_Radio8);
	DDX_Control(pDX, IDC_RADIO16KAM, m_Radio16);
	DDX_Text(pDX, IDC_LOADERTIMEOUT, m_iLoaderTimeout);
	DDX_Text(pDX, IDC_STATIC_DISPLAY, m_sScreen);
	DDX_Text(pDX, IDC_TEMP, m_sTempDir);
	DDX_Check(pDX, IDC_CHECK_ISDN, m_bISDN);
	DDX_Check(pDX, IDC_CHECK_NET, m_bNet);
	DDX_Check(pDX, IDC_CHECK_BACKUP, m_bBackup);
	DDX_Text(pDX, IDC_HZ, m_sHz);
	DDX_CBString(pDX, IDC_ORGA_COMBO, m_sOrgaCombo);
	DDX_Text(pDX, IDC_EDIT_XPE, m_sXPeLicense);
	DDX_Text(pDX, IDC_NERO2, m_sNero2);
	DDV_MaxChars(pDX, m_sNero2, 4);
	DDX_Text(pDX, IDC_NERO3, m_sNero3);
	DDV_MaxChars(pDX, m_sNero3, 4);
	DDX_Text(pDX, IDC_NERO4, m_sNero4);
	DDV_MaxChars(pDX, m_sNero4, 4);
	DDX_Text(pDX, IDC_NERO5, m_sNero5);
	DDV_MaxChars(pDX, m_sNero5, 4);
	DDX_Text(pDX, IDC_NERO6, m_sNero6);
	DDV_MaxChars(pDX, m_sNero6, 4);
	DDX_Text(pDX, IDC_NERO1, m_sNero1);
	DDV_MaxChars(pDX, m_sNero1, 4);
	DDX_Check(pDX, IDC_CHECK_DVD, m_bBackupDVD);
	DDX_Text(pDX, IDC_STATICNERO_VERSION, m_sStaticNeroVersion);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_COMPORT_OK, m_ctrlComPort_OK);
	DDX_Control(pDX, IDC_COMPORT_NOTOK, m_ctrlComPort_NotOK);
	DDX_Control(pDX, IDC_KAM_QUNIT, m_Static_Kam_QUnit);
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CVipCleanNTDlg, CDialog)
	//{{AFX_MSG_MAP(CVipCleanNTDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_KILLFOCUS(IDC_OEM1, OnKillfocusOem1)
	ON_EN_KILLFOCUS(IDC_OEM2, OnKillfocusOem2)
	ON_EN_KILLFOCUS(IDC_OEM3, OnKillfocusOem3)
	ON_BN_CLICKED(IDC_WRITEREG, OnWriteReg)
	ON_EN_CHANGE(IDC_OEM1, OnChangeOem1)
	ON_EN_CHANGE(IDC_OEM2, OnChangeOem2)
	ON_EN_CHANGE(IDC_OEM3, OnChangeOem3)
	ON_EN_CHANGE(IDC_HOSTNAME, OnChangeHostname)
	ON_EN_CHANGE(IDC_ORGA, OnChangeOrga)
	ON_BN_CLICKED(IDC_DELDIR, OnDeldir)
	ON_EN_CHANGE(IDC_LOADERTIMEOUT, OnChangeLoadertimeout)
	ON_EN_KILLFOCUS(IDC_LOADERTIMEOUT, OnKillfocusLoadertimeout)
	ON_EN_SETFOCUS(IDC_OEM1, OnSetfocusOem1)
	ON_EN_SETFOCUS(IDC_OEM2, OnSetfocusOem2)
	ON_EN_SETFOCUS(IDC_OEM3, OnSetfocusOem3)
	ON_EN_KILLFOCUS(IDC_ORGA, OnKillfocusOrganisation)
	ON_BN_CLICKED(IDOK2, OnOk2)
	ON_BN_CLICKED(IDC_CHECK_ISDN, OnCheckIsdn)
	ON_BN_CLICKED(IDC_CHECK_NET, OnCheckNet)
	ON_BN_CLICKED(IDC_CHECK_BACKUP, OnCheckBackup)
	ON_BN_CLICKED(IDC_PING, OnPing)
	ON_BN_CLICKED(IDC_FREQUENCE, OnFrequence)
	ON_CBN_KILLFOCUS(IDC_ORGA_COMBO, OnKillfocusOrgaCombo)
	ON_CBN_SELCHANGE(IDC_ORGA_COMBO, OnSelchangeOrgaCombo)
	ON_EN_CHANGE(IDC_EDIT_XPE, OnChangeEditXpe)
	ON_EN_KILLFOCUS(IDC_NERO1, OnKillfocusNero1)
	ON_EN_CHANGE(IDC_NERO2, OnChangeNero2)
	ON_EN_KILLFOCUS(IDC_NERO2, OnKillfocusNero2)
	ON_EN_KILLFOCUS(IDC_NERO3, OnKillfocusNero3)
	ON_EN_CHANGE(IDC_NERO3, OnChangeNero3)
	ON_EN_CHANGE(IDC_NERO4, OnChangeNero4)
	ON_EN_KILLFOCUS(IDC_NERO4, OnKillfocusNero4)
	ON_EN_CHANGE(IDC_NERO5, OnChangeNero5)
	ON_EN_KILLFOCUS(IDC_NERO5, OnKillfocusNero5)
	ON_EN_CHANGE(IDC_NERO6, OnChangeNero6)
	ON_EN_KILLFOCUS(IDC_NERO6, OnKillfocusNero6)
	ON_EN_CHANGE(IDC_NERO1, OnChangeNero1)
	ON_BN_CLICKED(IDC_CHECK_DVD, OnCheckDvd)
	ON_BN_CLICKED(IDC_CK_AUDIO_TEST, OnCkAudioTest)
	ON_BN_CLICKED(IDC_BTN_AUDIO_VOLUMES, OnBtnAudioVolumes)
	ON_BN_CLICKED(IDC_RADIO16KAM, OnRadio16kam)
	ON_BN_CLICKED(IDC_RADIO4KAM, OnRadio4kam)
	ON_BN_CLICKED(IDC_RADIO8KAM, OnRadio8kam)
	ON_STN_CLICKED(IDC_KAM8, OnStnClickedKam8)
	ON_BN_CLICKED(IDC_BTN_CF_INIT, OnBnClickedBtnCfInit)
	ON_BN_CLICKED(IDC_BTN_CF_UNINIT, OnBnClickedBtnCfUninit)
	ON_BN_CLICKED(IDC_BTN_INIT_DISKS, OnBnClickedBtnInitDisks)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CVipCleanNTDlg message handlers
BOOL CVipCleanNTDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	//AsShell vorbelegungauf 1;
//	m_shell=_T("1");
//	int len = m_shell.GetLength() * sizeof(TCHAR);
//	m_Reg.SetKey(_T("SOFTWARE\\dvrt\\DV\\DVStarter"),_T("AsShell"),(BYTE*)LPCTSTR(m_shell),len,REG_DWORD);
/*	int len;
	CString s = _T("´™®ÆßÆ™ßíëêèéçåãäâàáÜÖÑÉÇÅÄ~}");
	len =  s.GetLength() * sizeof(TCHAR);
	m_Reg.SetKey(_T("SOFTWARE\\dvrt\\DV\\DVStarter"),_T("Oem"),(BYTE*)LPCTSTR(s),len,REG_SZ);
	m_Reg.SetInt(_T("SYSTEM\\CurrentControlSet\\Services\\Cdrom"),_T("Autorun"), 1);
*/
	m_iNrOfJacobs = m_iNrOfSavics = m_iNrOfTashas = m_iNrOfQs = 0;
	m_bIsXP = IsOSVersionXP();

	if(m_bIsXP)
	{
		CBoards boards;
		m_iNrOfJacobs = boards.GetNumbersOfJaCobs();
		m_iNrOfSavics = boards.GetNumbersOfSaVics();
		m_iNrOfTashas = boards.GetNumbersOfTashas();
		m_iNrOfQs	  = boards.GetNumbersOfQs();
		GetXPDataFromReg();
		InitXPButtons();
		m_editOEM1.ShowWindow(SW_HIDE);
		m_editOEM2.ShowWindow(SW_HIDE);
		m_editOEM3.ShowWindow(SW_HIDE);
		m_StaticOEM1.ShowWindow(SW_HIDE);
		m_StaticOEM2.ShowWindow(SW_HIDE);
	}
	else
	{
		GetDataFromReg();
		m_editOEM1.SetFocus();
	}

	//setzte standard IP Adresse f¸r LAN-Test 
	m_IP.SetAddress(192,168,0,5);

	//MsgBox mit HInweis, wie die Dongle-Einstellungen resetet werden
	m_bResetMsgBox = TRUE;

	//hole alle Monitoreinstellungen
	GetMonitorConfigurations();

	//teste die ComPorts
	CString sReport, sCom;
	LPCOMMCONFIG pConfig;
	DWORD dwLen = 2*sizeof(COMMCONFIG);
	DWORD dwCommPortMask = 0;
	DWORD dwBitMask   = 1;
	BOOL  i, bRet;

	pConfig = (LPCOMMCONFIG)malloc(dwLen);
	sReport = _T("Ports:\n");
	for (i=1; i<=32; i++,dwBitMask<<=1)
	{
		sCom.Format(_T("COM%d"),i);
		bRet = GetDefaultCommConfig(sCom,(LPCOMMCONFIG)pConfig,&dwLen);
		if (bRet)
		{
			dwCommPortMask |= dwBitMask;
			sReport += sCom;
			sCom.Format(_T(" baud=%d parity=%d data=%d stop=%d\n"), pConfig->dcb.BaudRate, pConfig->dcb.fParity, pConfig->dcb.ByteSize, pConfig->dcb.StopBits);
			sReport += sCom;
		}
	}
	free(pConfig);
	if (dwCommPortMask == 0)
	{
		sReport = _T("No ComPorts!\n");
		m_ctrlComPort_NotOK.ShowWindow(SW_SHOW);
		AfxMessageBox(sReport, MB_OK|MB_ICONSTOP);
	}
	else
	{
		m_ctrlComPort_OK.ShowWindow(SW_SHOW);
	}

	UpdateData(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}
/////////////////////////////////////////////////////////////////////////////
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void CVipCleanNTDlg::OnPaint() 
{
	if(m_bIsXP)
	{
		m_CtrlXPeLicense.SetFocus();
	}
	else
	{
		m_editOEM1.SetFocus();
	}

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
/////////////////////////////////////////////////////////////////////////////
// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CVipCleanNTDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CVipCleanNTDlg::GetDataFromReg()
{
	CString value;
	DWORD dwHeigth, dwwidth, color, dwHZ;
	m_iLoaderTimeout=m_Reg.GetNTLoaderTimeout();


	if(m_Reg.GetDisplaySettings(dwHeigth,dwwidth,color,dwHZ))
	{
		m_sScreen.Format(_T("%d x %d, %d Bit Farbtiefe"),dwwidth,dwHeigth,color);

	}
	else
	{
		m_sScreen.Format(_T("Fehler,Display Settings ¸berpr¸fen!"));

	}

	if(m_Reg.CheckTemp())
	{
		m_sTempDir=_T("c:\\Temp vorhanden");
		m_Reg.DelFiles(_T("c:\\Temp"));

	}
	else
	{
		if(!m_Reg.CreateTempDir())
		{
			m_sTempDir=_T("Fehler bei ‹berpr¸fung des Temp Verzeichnisses!");
		}
		else
		{
			m_sTempDir=_T("c:\\Temp war nicht vorhanden. Verzeichnis angelegt!");
		}
	}




	//1. OEM Nummer holen;
	value=m_Reg.GetKey(REGPATH_HKLM_NT_CURRENTVERSION, REGKEY_PRODUCTID);
	m_sOEM1 = value.Left(5);
	m_sOEM2 = value.Mid(8,7);
	m_sOEM3 = value.Mid(15,5);
	m_sOEMMid = value.Mid(5,3);


	//2. Computernamen holen;
	m_shostname = m_Reg.GetKey(REGPATH_HKLM_COMPUTERNAME, REGKEY_COMPUTERNAME);
	
/*	m_shostname = m_Reg.GetKey(REGPATH_HKLM_COMPUTERNAME, REGKEY_SERIALNUMBER);
	if (m_shostname.IsEmpty())
	{
		m_shostname = m_Reg.GetKey(REGPATH_HKLM_COMPUTERNAME, REGKEY_COMPUTERNAME);
		m_bSaveAsSerialNumberKey = FALSE;
	}
	else
	{
		m_bSaveAsSerialNumberKey = TRUE;
	}
*/
	//3. Organisation holen
	m_sOrgaCombo = m_Reg.GetKey(REGPATH_HKLM_NT_CURRENTVERSION, REGKEY_REGISTEREDORGANIZATION);
//	m_sOrganisation = m_Reg.GetKey(REGPATH_HKLM_NT_CURRENTVERSION, REGKEY_REGISTEREDORGANIZATION);

	if(m_editOrgaCombo.SelectString(-1, m_sOrgaCombo) == CB_ERR )
	{
		m_editOrgaCombo.AddString(m_sOrgaCombo);
		m_editOrgaCombo.SelectString(-1, m_sOrgaCombo);
	}



	CString kamera = m_Reg.GetKey(_T("SOFTWARE\\dvrt\\DV\\Process"),_T("NrOfCameras"),REG_DWORD);
	m_scam=kamera;
	if(m_scam==_T("4"))
		m_Radio4.SetCheck(1);
	else if(m_scam==_T("8"))
		m_Radio8.SetCheck(1);
	else if(m_scam==_T("16"))
		m_Radio16.SetCheck(1);

	/* Lˆschen der Reg Pfade

				HKEY_LOCAL_MACHINE\SOFTWARE\DVRT\DV\INPUTGROUPS
				HKEY_LOCAL_MACHINE\SOFTWARE\DVRT\DV\OUTGROUPS
				HKEY_LOCAL_MACHINE\SOFTWARE\DVRT\DV\ARCHIVE
	*/

	m_Reg.DelKey(_T("SOFTWARE\\DVRT\\DV\\InputGroups"));
	m_Reg.DelKey(_T("SOFTWARE\\DVRT\\DV\\OutputGroups"));
	m_Reg.DelKey(_T("SOFTWARE\\DVRT\\DV\\Archive"));

	m_bNet  = (m_Reg.GetInt(_T("SOFTWARE\\dvrt\\JaCobUnit\\Device1\\EEProm"), _T("NET"), 0) == 1);
	m_bISDN = (m_Reg.GetInt(_T("SOFTWARE\\dvrt\\JaCobUnit\\Device1\\EEProm"), _T("ISDN"), 0) == 1);
	m_bBackup = (m_Reg.GetInt(_T("SOFTWARE\\dvrt\\JaCobUnit\\Device1\\EEProm"), _T("Backup"), 0) == 1);
	

	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::GetXPDataFromReg()
{
	CString value;
	DWORD dwHeigth, dwwidth, color, dwHZ;
	dwHeigth = dwwidth = color = dwHZ = 0;

	m_iLoaderTimeout=m_Reg.GetNTLoaderTimeout();

	if(m_Reg.GetDisplaySettings(dwHeigth,dwwidth,color,dwHZ))
	{
		m_sScreen.Format(_T("%d x %d, %d Bit Farbtiefe"),dwwidth,dwHeigth,color);

	}
	else
	{
		m_sScreen.Format(_T("Fehler,Display Settings ¸berpr¸fen!"));

	}

	//XPe Lizenz holen
	m_sXPeLicense = m_Reg.GetKey(_T("SOFTWARE\\dvrt\\Version"),_T("XPeLicense"));

	//Nero Lizenz holen
	//pr¸fe Nero Version 5 oder 6
	CString sSerial = _T("Serial6");
	BOOL bInitSerial = FALSE;

	m_sNeroLicense = m_Reg.GetKey(_T("SOFTWARE\\Ahead\\Nero - Burning Rom\\Info"),sSerial);
	m_iNeroVersion = 6;
	if(m_sNeroLicense == _T("0"))
	{	
		sSerial = _T("Serial5");
		m_sNeroLicense = m_Reg.GetKey(_T("SOFTWARE\\Ahead\\Nero - Burning Rom\\Info"),sSerial);
		if(m_sNeroLicense != _T("0"))
		{
			m_iNeroVersion = 5;
			bInitSerial = TRUE;
		}
	}
	else
	{
		bInitSerial = TRUE;
	}
	//default Nero Serial f¸r LiteOn Brenner mit Nero Version 5.5 
	m_sNero1 = _T("1502");
	m_sNero2 = _T("4240");
	m_sNero3 = _T("1125");
	m_sNero4 = _T("0482");
	m_sNero5 = _T("1088");
	m_sNero6 = _T("9784");

	if(bInitSerial)
	{
		DWORD Regtype = REG_SZ;
		HKEY MainKey = HKEY_CURRENT_USER;
		CString sNeroVersion = _T("RegisteredTypesNeroVersion");
		m_sStaticNeroVersion = m_Reg.GetKey(_T("SOFTWARE\\Ahead\\Nero - Burning Rom\\General"),sNeroVersion, Regtype,MainKey);

		int iStep = 4;
		m_sNero1 = m_sNeroLicense.Left(iStep);
		m_sNero2 = m_sNeroLicense.Mid	(iStep+1,iStep);
		m_sNero3 = m_sNeroLicense.Mid	(iStep*2+2,iStep);
		m_sNero4 = m_sNeroLicense.Mid	(iStep*3+3,iStep);
		m_sNero5 = m_sNeroLicense.Mid	(iStep*4+4,iStep);
		m_sNero6 = m_sNeroLicense.Mid	(iStep*5+5,iStep);


	}
	else
	{
		

	}

	//Computernamen holen;
	m_shostname =m_Reg.GetKey(REGPATH_HKLM_COMPUTERNAME, REGKEY_COMPUTERNAME);
	
/*	m_shostname =m_Reg.GetKey(REGPATH_HKLM_COMPUTERNAME, REGKEY_SERIALNUMBER);
	if (m_shostname.IsEmpty())
	{
		m_shostname =m_Reg.GetKey(REGPATH_HKLM_COMPUTERNAME, REGKEY_COMPUTERNAME);
		m_bSaveAsSerialNumberKey = FALSE;
	}
	else
	{
		m_bSaveAsSerialNumberKey = TRUE;
	}
*/
	//Organisation initialisieren
	m_editOrgaCombo.ResetContent();
	m_editOrgaCombo.AddString(ORGA_SIEMENS);
	m_editOrgaCombo.AddString(ORGA_SANYO);
	m_editOrgaCombo.AddString(ORGA_VIDETE);

	//Organisation holen
	m_sOrgaCombo = m_Reg.GetKey(REGPATH_HKLM_NT_CURRENTVERSION, REGKEY_REGISTEREDORGANIZATION);
	if(m_editOrgaCombo.SelectString(-1, m_sOrgaCombo) == CB_ERR )
	{
		m_editOrgaCombo.AddString(m_sOrgaCombo);
		m_editOrgaCombo.SelectString(-1, m_sOrgaCombo);
	}
	
	CString kamera=m_Reg.GetKey(_T("SOFTWARE\\dvrt\\DV\\Process"),_T("NrOfCameras"),REG_DWORD);
	m_scam=kamera;

	m_Reg.DelKey(_T("SOFTWARE\\DVRT\\DV\\InputGroups"));
	m_Reg.DelKey(_T("SOFTWARE\\DVRT\\DV\\OutputGroups"));
	m_Reg.DelKey(_T("SOFTWARE\\DVRT\\DV\\Archive"));

	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::InitXPButtons()
{

	//enable Nero fields
	m_StaticNero.ShowWindow(SW_SHOW);
	m_CtrlNero1.ShowWindow(SW_SHOW);
	m_CtrlNero2.ShowWindow(SW_SHOW);
	m_CtrlNero3.ShowWindow(SW_SHOW);
	m_CtrlNero4.ShowWindow(SW_SHOW);
	m_CtrlNero5.ShowWindow(SW_SHOW);
	m_CtrlNero6.ShowWindow(SW_SHOW);

	//setze Dialog Name
	this->SetWindowText(_T("DTSCleanXP"));

	//enable fields
	m_StaticXPeLicense.ShowWindow(SW_SHOW);
	m_CtrlXPeLicense.ShowWindow(SW_SHOW);

	//disable buttons
	m_Radio4.EnableWindow(FALSE);
	m_Radio8.EnableWindow(FALSE);
	m_Radio16.EnableWindow(FALSE);
	m_StaticTemp.ShowWindow(SW_HIDE);
	m_EditTemp.ShowWindow(SW_HIDE);

	if (m_iNrOfSavics == 1)
	{
		m_StaticKam4.ShowWindow(SW_SHOW);
		m_Radio4.SetCheck(1);
		//setze ISDN/NET
		m_bNet   = (m_Reg.GetInt(_T("SOFTWARE\\dvrt\\SavicUnit\\Device1\\EEProm"), _T("NET"), 0) == 1);
		m_bISDN  = (m_Reg.GetInt(_T("SOFTWARE\\dvrt\\SavicUnit\\Device1\\EEProm"), _T("ISDN"), 0) == 1);
		m_bBackup  = (m_Reg.GetInt(_T("SOFTWARE\\dvrt\\SavicUnit\\Device1\\EEProm"), _T("Backup"), 0) == 1);
		m_bBackupDVD = (m_Reg.GetInt(_T("SOFTWARE\\dvrt\\SavicUnit\\Device1\\EEProm"), _T("BackUpToDVD"), 0) == 1);

	}
	else if(m_iNrOfJacobs == 1)
	{
		m_StaticKam8.ShowWindow(SW_SHOW);
		m_Radio8.SetCheck(1);
		//setze ISDN/NET
		m_bNet   = (m_Reg.GetInt(_T("SOFTWARE\\dvrt\\JaCobUnit\\Device1\\EEProm"), _T("NET"), 0) == 1);
		m_bISDN  = (m_Reg.GetInt(_T("SOFTWARE\\dvrt\\JaCobUnit\\Device1\\EEProm"), _T("ISDN"), 0) == 1);
		m_bBackup  = (m_Reg.GetInt(_T("SOFTWARE\\dvrt\\JaCobUnit\\Device1\\EEProm"), _T("Backup"), 0) == 1);
		m_bBackupDVD = (m_Reg.GetInt(_T("SOFTWARE\\dvrt\\JaCobUnit\\Device1\\EEProm"), _T("BackUpToDVD"), 0) == 1);
	}
	else if(m_iNrOfJacobs == 2)
	{
		m_StaticKam16.ShowWindow(SW_SHOW);
		m_Radio16.SetCheck(1);
		//setze ISDN/NET
		m_bNet   = (m_Reg.GetInt(_T("SOFTWARE\\dvrt\\JaCobUnit\\Device1\\EEProm"), _T("NET"), 0) == 1);
		m_bISDN  = (m_Reg.GetInt(_T("SOFTWARE\\dvrt\\JaCobUnit\\Device1\\EEProm"), _T("ISDN"), 0) == 1);
		m_bBackup  = (m_Reg.GetInt(_T("SOFTWARE\\dvrt\\JaCobUnit\\Device1\\EEProm"), _T("Backup"), 0) == 1);
		m_bBackupDVD = (m_Reg.GetInt(_T("SOFTWARE\\dvrt\\JaCobUnit\\Device1\\EEProm"), _T("BackUpToDVD"), 0) == 1);

	}
	else if(m_iNrOfTashas == 1)
	{
		m_StaticKam8_Tasha.ShowWindow(SW_SHOW);
		m_Radio8.SetCheck(1);
		//setze ISDN/NET
		m_bNet   = (m_Reg.GetInt(_T("SOFTWARE\\dvrt\\TashaUnit\\Device1\\EEProm"), _T("NET"), 0) == 1);
		m_bISDN  = (m_Reg.GetInt(_T("SOFTWARE\\dvrt\\TashaUnit\\Device1\\EEProm"), _T("ISDN"), 0) == 1);
		m_bBackup  = (m_Reg.GetInt(_T("SOFTWARE\\dvrt\\TashaUnit\\Device1\\EEProm"), _T("Backup"), 0) == 1);
		m_bBackupDVD = (m_Reg.GetInt(_T("SOFTWARE\\dvrt\\TashaUnit\\Device1\\EEProm"), _T("BackUpToDVD"), 0) == 1);

	}
	else if(m_iNrOfTashas == 2)
	{
		m_StaticKam16_Tasha.ShowWindow(SW_SHOW);
		m_Radio16.SetCheck(1);
		//setze ISDN/NET
		m_bNet   = (m_Reg.GetInt(_T("SOFTWARE\\dvrt\\TashaUnit\\Device1\\EEProm"), _T("NET"), 0) == 1);
		m_bISDN  = (m_Reg.GetInt(_T("SOFTWARE\\dvrt\\TashaUnit\\Device1\\EEProm"), _T("ISDN"), 0) == 1);
		m_bBackup  = (m_Reg.GetInt(_T("SOFTWARE\\dvrt\\TashaUnit\\Device1\\EEProm"), _T("Backup"), 0) == 1);
		m_bBackupDVD = (m_Reg.GetInt(_T("SOFTWARE\\dvrt\\TashaUnit\\Device1\\EEProm"), _T("BackUpToDVD"), 0) == 1);

	}
	else if(m_iNrOfQs == 1)
	{
		m_Static_Kam_QUnit.ShowWindow(SW_SHOW);
		m_Radio4.EnableWindow();
		m_Radio8.EnableWindow();
		m_Radio16.EnableWindow();

		m_bNet   = (m_Reg.GetInt(_T("SOFTWARE\\dvrt\\QUnit\\EEProm"), _T("NET"), 0) == 1);
		m_bISDN  = (m_Reg.GetInt(_T("SOFTWARE\\dvrt\\QUnit\\EEProm"), _T("ISDN"), 0) == 1);
		m_bBackup  = (m_Reg.GetInt(_T("SOFTWARE\\dvrt\\QUnit\\EEProm"), _T("Backup"), 0) == 1);
		m_bBackupDVD = (m_Reg.GetInt(_T("SOFTWARE\\dvrt\\QUnit\\EEProm"), _T("BackUpToDVD"), 0) == 1);
		m_iNrOfAllowedQUnitCams = (m_Reg.GetInt(_T("SOFTWARE\\dvrt\\QUnit\\EEProm"), _T("AllowedCameras"), m_iNrOfAllowedQUnitCams));
		
		if(m_iNrOfAllowedQUnitCams == 4)
		{
			m_Radio4.SetCheck(1);
		}
		else if(m_iNrOfAllowedQUnitCams == 8)
		{
			m_Radio8.SetCheck(1);
		}
		else if(m_iNrOfAllowedQUnitCams == 16)
		{
			m_Radio16.SetCheck(1);
		}
	}
	

	UpdateData(FALSE);

}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::OnKillfocusOem1() 
{
	if(m_sOEM1.GetLength() < 5)		
	{
		m_editOEM1.SetFocus();
		m_editOEM1.SetSel(0, -1, FALSE);
	}

}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::OnKillfocusOem2() 
{
	if(m_sOEM2.GetLength() < 7)		
	{
		m_editOEM2.SetFocus();
		m_editOEM2.SetSel(0, -1, FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::OnKillfocusOem3() 
{
	if(m_sOEM3.GetLength() < 5)		
	{
		m_editOEM3.SetFocus();
		m_editOEM3.SetSel(0, -1, FALSE);
	}


}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::OnWriteReg() 
{

	if(m_bIsXP)
	{
		int len = 0;

		//Nero Lizenz eintragen
		CString sNewNeroLizenz;

		if(m_sNeroLicense != _T("0"))
		{
			sNewNeroLizenz.Format(_T("%s-%s-%s-%s-%s-%s"),m_sNero1,m_sNero2,m_sNero3,
													  m_sNero4,m_sNero5,m_sNero6);
			len = sNewNeroLizenz.GetLength() * sizeof(TCHAR);
			CString sSerial = _T("Serial5");
			if(m_iNeroVersion == 5)
			{
				m_Reg.SetKey(_T("SOFTWARE\\Ahead\\Nero - Burning Rom\\Info"),sSerial, (BYTE*)LPCTSTR(sNewNeroLizenz), len);
			}
			else if(m_iNeroVersion == 6)
			{
				sSerial = _T("Serial6");
				m_Reg.SetKey(_T("SOFTWARE\\Ahead\\Nero - Burning Rom\\Info"),sSerial, (BYTE*)LPCTSTR(sNewNeroLizenz), len);
			}
		}


		//XPe Lizenz eintragen
		len = m_sXPeLicense.GetLength() * sizeof(TCHAR);
		m_Reg.SetKey(_T("SOFTWARE\\dvrt\\Version"),_T("XPeLicense"), (BYTE*)LPCTSTR(m_sXPeLicense), len);

		//in Abh‰ngigkeit der Organisation die Seriennummer (Hostname) setzen
		//z.B. bei Videte: DTS+Hostname aber nur, wenn noch kein Pr‰fix (DTS...) davor steht
		CString sDummy;
		CString sType = m_shostname.Left(3);

		//steht schon ein DTS,usw. vor der Seriennummer, dann kein Pr‰fix hinzuf¸gen
		if(    !m_shostname.IsEmpty()
			&& (sType != ORGA_SIEMENS_TYPE)
			&& (sType != ORGA_SANYO_TYPE)
			&& (sType != ORGA_VIDETE_TYPE))
		{
			if(m_sOrgaCombo == ORGA_SIEMENS)
			{
				sDummy = ORGA_SIEMENS_TYPE;
			}
			else if(m_sOrgaCombo == ORGA_SANYO)
			{
				sDummy = ORGA_SANYO_TYPE;
			}
			else if(m_sOrgaCombo == ORGA_VIDETE)
			{
				sDummy = ORGA_VIDETE_TYPE;
			}
		}

		//steht schon ein Pr‰fix davor und soll die Organisation ge‰ndert werden
		//dann nur das Pr‰fix ‰ndern
		if(     (     sType == ORGA_SIEMENS_TYPE
			       && m_sOrgaCombo != ORGA_SIEMENS)
			||  (     sType == ORGA_SANYO_TYPE
				   && m_sOrgaCombo != ORGA_SANYO)
			||  (     sType == ORGA_VIDETE_TYPE
				   && m_sOrgaCombo != ORGA_VIDETE))
		{
			if(m_sOrgaCombo == ORGA_SIEMENS)
			{
				sDummy = ORGA_SIEMENS_TYPE;
			}
			else if(m_sOrgaCombo == ORGA_SANYO)
			{
				sDummy = ORGA_SANYO_TYPE;
			}
			else if(m_sOrgaCombo == ORGA_VIDETE)
			{
				sDummy = ORGA_VIDETE_TYPE;
			}

			//den Hostnamen um das falche K¸rzel (DTS, VDR, ...) k¸rzen
			m_shostname = m_shostname.Right(m_shostname.GetLength()-sType.GetLength());
		}
		sDummy += m_shostname;
		m_shostname = sDummy;

		//Hostname aktualisieren
		len = m_shostname.GetLength() * sizeof(TCHAR);

		CString sUpperName(m_shostname);		// to change the computername
		sUpperName.MakeUpper();
		BOOL bReturn = FALSE;

#ifndef _DEBUG
		bReturn = SetComputerNameEx(ComputerNamePhysicalNetBIOS, sUpperName) &&
			SetComputerNameEx(ComputerNamePhysicalDnsHostname, m_shostname);
#endif

		
		if(!bReturn)
		{
			AfxMessageBox(_T("Computername konnte nicht gesetzt werden?"), MB_OK|MB_ICONEXCLAMATION);
		}

//		m_Reg.SetKey(REGPATH_HKLM_COMPUTERNAME, REGKEY_COMPUTERNAME,(BYTE*)LPCTSTR(m_shostname),len);
//		m_Reg.SetKey(_T("SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters"), _T("Hostname"),(BYTE*)LPCTSTR(m_shostname),len);

/*		if (m_bSaveAsSerialNumberKey)
		{
			m_Reg.SetKey(REGPATH_HKLM_COMPUTERNAME, REGKEY_SERIALNUMBER,(BYTE*)LPCTSTR(m_shostname),len);
		}
		else
		{
			m_Reg.SetKey(REGPATH_HKLM_COMPUTERNAME, REGKEY_COMPUTERNAME,(BYTE*)LPCTSTR(m_shostname),len);
			m_Reg.SetKey(_T("SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters"), _T("Hostname"),(BYTE*)LPCTSTR(m_shostname),len);
		}
*/
		m_Reg.SetKey(REGPATH_HKLM_NT_CURRENTVERSION, REGKEY_REGISTEREDOWNER,(BYTE*)LPCTSTR(m_shostname),len);

		//Organisation aktualisieren
		len = m_sOrgaCombo.GetLength() * sizeof(TCHAR);
		m_Reg.SetKey(REGPATH_HKLM_NT_CURRENTVERSION, REGKEY_REGISTEREDORGANIZATION,(BYTE*)LPCTSTR(m_sOrgaCombo),len);

/*
		len=m_scam.GetLength() * sizeof(TCHAR);
		m_Reg.SetKey(_T("SOFTWARE\\dvrt\\DV\\Process"),_T("NrOfCameras"),(BYTE*)LPCTSTR(m_scam),len,REG_DWORD);
*/
		
		DWORD dwRawBits = 0;
		
		if (m_bISDN)
			dwRawBits = SETBIT(dwRawBits, 0);
		else
			dwRawBits = CLRBIT(dwRawBits, 0);

		if (m_bNet)
			dwRawBits = SETBIT(dwRawBits, 1);
		else
			dwRawBits = CLRBIT(dwRawBits, 1);

		if (m_bBackup)
			dwRawBits = SETBIT(dwRawBits, 2);
		else
			dwRawBits = CLRBIT(dwRawBits, 2);

		if (m_bBackupDVD)
			dwRawBits = SETBIT(dwRawBits, 3);
		else
			dwRawBits = CLRBIT(dwRawBits, 3);

		//get new allowed cameras
		dwRawBits = CLRBIT(dwRawBits, 4);
		dwRawBits = CLRBIT(dwRawBits, 5);

		if(m_Radio4.GetCheck())
		{
			m_iNrOfAllowedQUnitCams = 4;
		}
		else if(m_Radio8.GetCheck())
		{
			m_iNrOfAllowedQUnitCams = 8;
		}
		else if(m_Radio16.GetCheck())
		{
			m_iNrOfAllowedQUnitCams = 16;
		}


		if (m_iNrOfAllowedQUnitCams == 8)
		{
			dwRawBits = SETBIT(dwRawBits, 4);
		}

		if (m_iNrOfAllowedQUnitCams == 16)
		{
			dwRawBits = SETBIT(dwRawBits, 5);
		}

		if (m_iNrOfJacobs>=1)
		{
			m_Reg.SetInt(_T("SOFTWARE\\dvrt\\JaCobUnit\\Device1\\EEProm"), _T("RawBits"), dwRawBits);
			m_Reg.SetInt(_T("SOFTWARE\\dvrt\\JaCobUnit\\Device1\\EEProm"), _T("NET"), m_bNet);
			m_Reg.SetInt(_T("SOFTWARE\\dvrt\\JaCobUnit\\Device1\\EEProm"), _T("ISDN"), m_bISDN);
			m_Reg.SetInt(_T("SOFTWARE\\dvrt\\JaCobUnit\\Device1\\EEProm"), _T("Backup"), m_bBackup);
			m_Reg.SetInt(_T("SOFTWARE\\dvrt\\JaCobUnit\\Device1\\EEProm"), _T("BackUpToDVD"), m_bBackupDVD);
		}
		else if(m_iNrOfSavics>=1)
		{
			m_Reg.SetInt(_T("SOFTWARE\\dvrt\\SavicUnit\\Device1\\EEProm"), _T("RawBits"), dwRawBits);
			m_Reg.SetInt(_T("SOFTWARE\\dvrt\\SavicUnit\\Device1\\EEProm"), _T("NET"), m_bNet);
			m_Reg.SetInt(_T("SOFTWARE\\dvrt\\SavicUnit\\Device1\\EEProm"), _T("ISDN"), m_bISDN);
			m_Reg.SetInt(_T("SOFTWARE\\dvrt\\SavicUnit\\Device1\\EEProm"), _T("Backup"), m_bBackup);
			m_Reg.SetInt(_T("SOFTWARE\\dvrt\\SavicUnit\\Device1\\EEProm"), _T("BackUpToDVD"), m_bBackupDVD);
		}
		else if(m_iNrOfTashas>=1)
		{
			m_Reg.SetInt(_T("SOFTWARE\\dvrt\\TashaUnit\\Device1\\EEProm"), _T("RawBits"), dwRawBits);
			m_Reg.SetInt(_T("SOFTWARE\\dvrt\\TashaUnit\\Device1\\EEProm"), _T("NET"), m_bNet);
			m_Reg.SetInt(_T("SOFTWARE\\dvrt\\TashaUnit\\Device1\\EEProm"), _T("ISDN"), m_bISDN);
			m_Reg.SetInt(_T("SOFTWARE\\dvrt\\TashaUnit\\Device1\\EEProm"), _T("Backup"), m_bBackup);
			m_Reg.SetInt(_T("SOFTWARE\\dvrt\\TashaUnit\\Device1\\EEProm"), _T("BackUpToDVD"), m_bBackupDVD);
		}
		else if(m_iNrOfQs>=1)
		{
			m_Reg.SetInt(_T("SOFTWARE\\dvrt\\QUnit\\EEProm"), _T("RawBits"), dwRawBits);
			m_Reg.SetInt(_T("SOFTWARE\\dvrt\\QUnit\\EEProm"), _T("NET"), m_bNet);
			m_Reg.SetInt(_T("SOFTWARE\\dvrt\\QUnit\\EEProm"), _T("ISDN"), m_bISDN);
			m_Reg.SetInt(_T("SOFTWARE\\dvrt\\QUnit\\EEProm"), _T("Backup"), m_bBackup);
			m_Reg.SetInt(_T("SOFTWARE\\dvrt\\QUnit\\EEProm"), _T("BackUpToDVD"), m_bBackupDVD);
			m_Reg.SetInt(_T("SOFTWARE\\dvrt\\QUnit\\EEProm"), _T("AllowedCameras"), m_iNrOfAllowedQUnitCams);
		}

		GetXPDataFromReg();
		InitXPButtons();
		m_writeReg.EnableWindow(false);

	}
	else
	{
		//OEM aktualisieren
		CString OEM=m_sOEM1 + m_sOEMMid + m_sOEM2 + m_sOEM3;
		int len = OEM.GetLength() * sizeof(TCHAR);
		m_Reg.SetKey(REGPATH_HKLM_NT_CURRENTVERSION, REGKEY_PRODUCTID,(BYTE*)LPCTSTR(OEM),len);

		//Hostname aktualisieren
		len = m_shostname.GetLength() * sizeof(TCHAR);
		m_Reg.SetKey(REGPATH_HKLM_COMPUTERNAME, REGKEY_COMPUTERNAME,(BYTE*)LPCTSTR(m_shostname),len);
		m_Reg.SetKey(_T("SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters"), _T("Hostname"),(BYTE*)LPCTSTR(m_shostname),len);

/*		if (m_bSaveAsSerialNumberKey)
		{
			m_Reg.SetKey(REGPATH_HKLM_COMPUTERNAME, REGKEY_SERIALNUMBER,(BYTE*)LPCTSTR(m_shostname),len);
		}
		else
		{
			m_Reg.SetKey(REGPATH_HKLM_COMPUTERNAME, REGKEY_COMPUTERNAME,(BYTE*)LPCTSTR(m_shostname),len);
			m_Reg.SetKey(_T("SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters"), _T("Hostname"),(BYTE*)LPCTSTR(m_shostname),len);
		}
*/
		m_Reg.SetKey(REGPATH_HKLM_NT_CURRENTVERSION, REGKEY_REGISTEREDOWNER, (BYTE*)LPCTSTR(m_shostname), len);

		//Organisation aktualisieren

	//	len = m_sOrganisation.GetLength() * sizeof(TCHAR);
	//	m_Reg.SetKey(REGPATH_HKLM_NT_CURRENTVERSION, REGKEY_REGISTEREDORGANIZATION, (BYTE*)LPCTSTR(m_sOrganisation),len);
		len = m_sOrgaCombo.GetLength() * sizeof(TCHAR);
		m_Reg.SetKey(REGPATH_HKLM_NT_CURRENTVERSION, REGKEY_REGISTEREDORGANIZATION, (BYTE*)LPCTSTR(m_sOrgaCombo),len);

		//AsShell
	//	len = m_shell.GetLength() * sizeof(TCHAR);
	//	m_Reg.SetKey(_T("SOFTWARE\\dvrt\\DV\\DVStarter"),_T("AsShell"),(BYTE*)LPCTSTR(m_shell),len,REG_DWORD);

/*
		len=m_scam.GetLength() * sizeof(TCHAR);
		m_Reg.SetKey(_T("SOFTWARE\\dvrt\\DV\\Process"),_T("NrOfCameras"),(BYTE*)LPCTSTR(m_scam),len,REG_DWORD);
*/
		
		DWORD dwRawBits = 0;
		
		if (m_bISDN)
			dwRawBits = SETBIT(dwRawBits, 0);
		else
			dwRawBits = CLRBIT(dwRawBits, 0);

		if (m_bNet)
			dwRawBits = SETBIT(dwRawBits, 1);
		else
			dwRawBits = CLRBIT(dwRawBits, 1);

		if (m_iNrOfJacobs>=1)
		{
			m_Reg.SetInt(_T("SOFTWARE\\dvrt\\JaCobUnit\\Device1\\EEProm"), _T("RawBits"), dwRawBits);
			m_Reg.SetInt(_T("SOFTWARE\\dvrt\\JaCobUnit\\Device1\\EEProm"), _T("NET"), m_bNet);
			m_Reg.SetInt(_T("SOFTWARE\\dvrt\\JaCobUnit\\Device1\\EEProm"), _T("ISDN"), m_bISDN);
		}
		else if (m_iNrOfSavics>=1)
		{
			m_Reg.SetInt(_T("SOFTWARE\\dvrt\\SavicUnit\\Device1\\EEProm"), _T("RawBits"), dwRawBits);
			m_Reg.SetInt(_T("SOFTWARE\\dvrt\\SavicUnit\\Device1\\EEProm"), _T("NET"), m_bNet);
			m_Reg.SetInt(_T("SOFTWARE\\dvrt\\SavicUnit\\Device1\\EEProm"), _T("ISDN"), m_bISDN);
		}
		else if (m_iNrOfTashas>=1)
		{
			m_Reg.SetInt(_T("SOFTWARE\\dvrt\\TashaUnit\\Device1\\EEProm"), _T("RawBits"), dwRawBits);
			m_Reg.SetInt(_T("SOFTWARE\\dvrt\\TashaUnit\\Device1\\EEProm"), _T("NET"), m_bNet);
			m_Reg.SetInt(_T("SOFTWARE\\dvrt\\TashaUnit\\Device1\\EEProm"), _T("ISDN"), m_bISDN);
		}

		GetDataFromReg();
		m_writeReg.EnableWindow(false);
		m_bRegAktual = TRUE;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::OnChangeOem1() 
{
	m_writeReg.EnableWindow();
	m_bRegAktual=FALSE;
	UpdateData(TRUE);

	if((UINT)m_sOEM1.GetLength() == m_editOEM1.GetLimitText())
	{
		m_editOEM2.SetFocus();
		m_editOEM2.SetSel(0, -1, FALSE);

	}
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::OnChangeOem2() 
{
	m_writeReg.EnableWindow();
	m_bRegAktual=FALSE;

	UpdateData(TRUE);

	if((UINT)m_sOEM2.GetLength() == m_editOEM2.GetLimitText())
	{
		m_editOEM3.SetFocus();
		m_editOEM3.SetSel(0, -1, FALSE);

	}	
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::OnChangeOem3() 
{
	m_writeReg.EnableWindow();
	m_bRegAktual=FALSE;

	UpdateData(TRUE);

	if((UINT)m_sOEM3.GetLength() == m_editOEM3.GetLimitText())
	{
		m_Hostname.SetFocus();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::OnChangeHostname() 
{
	m_bRegAktual=FALSE;
	m_writeReg.EnableWindow();
	UpdateData(TRUE);
	
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::OnChangeOrga() 
{
	m_writeReg.EnableWindow();
	m_bRegAktual=FALSE;

	UpdateData(TRUE);
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::OnDeldir() 
{
	m_Reg.DelDir();
	m_del=_T(" DIRs GEL÷SCHT");
	m_bDirDeleted = TRUE;
	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::OnRadio4kam()
{
	m_Radio4.SetCheck(1);
	m_scam=_T("4");
	m_writeReg.EnableWindow();
	m_bRegAktual=FALSE;

	UpdateData(TRUE);
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::OnRadio8kam()
{
	m_Radio8.SetCheck(1);
	m_scam=_T("8");
	m_writeReg.EnableWindow();
	m_bRegAktual=FALSE;

	UpdateData(TRUE);
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::OnRadio16kam()
{

	m_Radio16.SetCheck(1);
	m_scam=_T("16");
	m_writeReg.EnableWindow();
	m_bRegAktual=FALSE;

	UpdateData(TRUE);
	
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::OnChangeLoadertimeout() 
{
	UpdateData(TRUE);	
	m_Reg.SetNTLoaderTimeout(m_iLoaderTimeout);
	m_iLoaderTimeout=m_Reg.GetNTLoaderTimeout();
	UpdateData(TRUE);	
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::OnKillfocusLoadertimeout() 
{
	if(!m_bIsXP)
	{
		m_editOEM1.SetFocus();
	}
		
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::OnSetfocusOem1() 
{
	m_editOEM1.SetSel(0,-1);
	//m_editOEM1.EnableWindow();
	
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::OnSetfocusOem2() 
{
	// TODO: Add your control notification handler code here
	
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::OnSetfocusOem3() 
{
	// TODO: Add your control notification handler code here
	
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::OnKillfocusOrganisation() 
{
	if(m_Radio4.GetCheck())
		  m_Radio4.SetFocus();	
	if(m_Radio8.GetCheck())
		  m_Radio8.SetFocus();	
	if(m_Radio16.GetCheck())
		  m_Radio16.SetFocus();	
	
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::OnOK() 
{

	
	CDialog::OnOK();
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::OnOk2() 
{
	if(!m_bDirDeleted && !m_bRegAktual)
	{
		if(AfxMessageBox(_T("Button Directories- und Registry lˆschen wurden nicht bet‰tigt\nTrotzdem beenden?"), MB_YESNO|MB_ICONSTOP|MB_ICONEXCLAMATION)==IDYES)
			PostQuitMessage(0);
		else
			return;
	}
	if(!m_bDirDeleted)
	{
		if(AfxMessageBox(_T("Button Directories lˆschen wurde nicht bet‰tigt\nTrotzdem beenden?"), MB_YESNO|MB_ICONSTOP|MB_ICONEXCLAMATION)==IDYES)
			PostQuitMessage(0);
		else
			return;
	}

	if(!m_bRegAktual)
	{
		if(AfxMessageBox(_T("Button Registry aktualisieren wurde nicht bet‰tigt\nTrotzdem beenden?"), MB_YESNO|MB_ICONSTOP|MB_ICONEXCLAMATION)==IDYES)
			PostQuitMessage(0);
		else
			return;
	}
	PostQuitMessage(0);

	
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::OnCheckIsdn() 
{
	if(m_bISDN)
	{
		CheckResetMsgBox();
	}

 	m_bISDN = !m_bISDN;

	m_writeReg.EnableWindow();
	m_bRegAktual=FALSE;

	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::OnCheckNet() 
{
	if(m_bNet)
	{
		CheckResetMsgBox();
	}

	m_bNet = !m_bNet;

	m_writeReg.EnableWindow();
	m_bRegAktual=FALSE;

	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::OnCheckBackup() 
{
	if(m_bBackup)
	{
		CheckResetMsgBox();
	}	

	m_bBackup = !m_bBackup;
	
	if(!m_bBackup)
	{
		m_bBackupDVD = FALSE;
		m_ctrlCheckDVD.SetCheck(0);
	}
	m_writeReg.EnableWindow();
	m_bRegAktual=FALSE;

	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::OnCheckDvd() 
{
	if(m_bBackupDVD)
	{
		CheckResetMsgBox();
	}

	m_bBackup = TRUE;
	m_bBackupDVD = !m_bBackupDVD;

	m_ctrlCheckBackup.SetCheck(1);
	m_writeReg.EnableWindow();
	m_bRegAktual=FALSE;

	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::OnPing() 
{
	BYTE bIP1, bIP2, bIP3, bIP4 = 0;
	m_IP.GetAddress(bIP1, bIP2, bIP3, bIP4);
	CString sAdress;
	sAdress.Format(_T("%i.%i.%i.%i"), bIP1, bIP2, bIP3, bIP4);
	CString sCmd	= _T("Cmd.exe");
	CString sParam	= _T("/c ping ") + sAdress;// +_T("") > a.txt";

	ShellExecute(NULL, _T("open") , sCmd, sParam, NULL, SW_SHOWNORMAL);


	m_CtrlPingNotYet.ShowWindow(SW_HIDE);
	m_CtrlPingOK.ShowWindow(SW_SHOW);
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::GetMonitorConfigurations()
{
	EnumDisplaySettings(NULL,
						ENUM_CURRENT_SETTINGS,
						&m_lpDevMode
						);
	
	m_sHz.Format(_T("%d"), m_lpDevMode.dmDisplayFrequency);
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::OnFrequence() 
{
	m_lpDevMode.dmDisplayFrequency = 75;
	int iResult = 0;
	iResult = ChangeDisplaySettings(&m_lpDevMode,CDS_UPDATEREGISTRY);
	
	m_sHz.Format(_T("%d"), m_lpDevMode.dmDisplayFrequency);
	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::OnKillfocusOrgaCombo() 
{
	if(m_Radio4.GetCheck())
		  m_Radio4.SetFocus();	
	if(m_Radio8.GetCheck())
		  m_Radio8.SetFocus();	
	if(m_Radio16.GetCheck())
		  m_Radio16.SetFocus();
	
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::OnSelchangeOrgaCombo() 
{
	UpdateData();
	m_bRegAktual=FALSE;
	m_writeReg.EnableWindow();
	UpdateData(FALSE);
	
}
/////////////////////////////////////////////////////////////////////////////
BOOL CVipCleanNTDlg::IsOSVersionXP()
{
	BOOL bRet = FALSE;

//	OS_TYPE OsRet(OS_NOT);
	OSVERSIONINFO	osinfo;
	CString			sOSVersion, sBuild, sSR, sOS;

	osinfo.dwOSVersionInfoSize = sizeof(osinfo);
	GetVersionEx(&osinfo);
	
	if (osinfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{		
		CString sNr;
		CString s;
		DWORD dwVersion = osinfo.dwMajorVersion;
		if(dwVersion  == 5)
		{
			sNr = _T("WIN XP, Version: ");
			bRet = TRUE;
		}
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::OnChangeEditXpe() 
{
	m_writeReg.EnableWindow();
	m_bRegAktual=FALSE;
	UpdateData(TRUE);
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::OnKillfocusNero1() 
{
	
	if(m_sNero1.GetLength() != 0 && m_sNero1.GetLength() < 4)		
	{
		m_CtrlNero1.SetFocus();
		m_CtrlNero1.SetSel(0, -1, FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::OnChangeNero2() 
{
	m_writeReg.EnableWindow();
	m_bRegAktual=FALSE;
	UpdateData(TRUE);

	if((UINT)m_sNero2.GetLength() == m_CtrlNero2.GetLimitText())
	{
		m_CtrlNero3.SetFocus();
		m_CtrlNero3.SetSel(0, -1, FALSE);

	}
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::OnKillfocusNero2() 
{
	if(m_sNero2.GetLength() != 0 && m_sNero2.GetLength() < 4)		
	{
		m_CtrlNero2.SetFocus();
		m_CtrlNero2.SetSel(0, -1, FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::OnKillfocusNero3() 
{
	if(m_sNero3.GetLength() != 0 && m_sNero3.GetLength() < 4)		
	{
		m_CtrlNero3.SetFocus();
		m_CtrlNero3.SetSel(0, -1, FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::OnChangeNero3() 
{
	m_writeReg.EnableWindow();
	m_bRegAktual=FALSE;
	UpdateData(TRUE);

	if((UINT)m_sNero3.GetLength() == m_CtrlNero3.GetLimitText())
	{
		m_CtrlNero4.SetFocus();
		m_CtrlNero4.SetSel(0, -1, FALSE);

	}
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::OnChangeNero4() 
{
	m_writeReg.EnableWindow();
	m_bRegAktual=FALSE;
	UpdateData(TRUE);

	if((UINT)m_sNero4.GetLength() == m_CtrlNero4.GetLimitText())
	{
		m_CtrlNero5.SetFocus();
		m_CtrlNero5.SetSel(0, -1, FALSE);

	}
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::OnKillfocusNero4() 
{
	if(m_sNero4.GetLength() != 0 && m_sNero4.GetLength() < 4)		
	{
		m_CtrlNero4.SetFocus();
		m_CtrlNero4.SetSel(0, -1, FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::OnChangeNero5() 
{
	m_writeReg.EnableWindow();
	m_bRegAktual=FALSE;
	UpdateData(TRUE);

	if((UINT)m_sNero5.GetLength() == m_CtrlNero5.GetLimitText())
	{
		m_CtrlNero6.SetFocus();
		m_CtrlNero6.SetSel(0, -1, FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::OnKillfocusNero5() 
{
	if(m_sNero5.GetLength() != 0 && m_sNero5.GetLength() < 4)		
	{
		m_CtrlNero5.SetFocus();
		m_CtrlNero5.SetSel(0, -1, FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::OnChangeNero6() 
{
	m_writeReg.EnableWindow();
	m_bRegAktual=FALSE;
	UpdateData(TRUE);

	if((UINT)m_sNero6.GetLength() == m_CtrlNero6.GetLimitText())
	{
		m_Hostname.SetFocus();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::OnKillfocusNero6() 
{
	if(m_sNero6.GetLength() != 0 && m_sNero6.GetLength() < 4)		
	{
		m_CtrlNero6.SetFocus();
		m_CtrlNero6.SetSel(0, -1, FALSE);
	}
	
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::OnChangeNero1() 
{
	m_writeReg.EnableWindow();
	m_bRegAktual=FALSE;
	UpdateData(TRUE);

//	int i = m_CtrlNero1.GetLimitText();
	if((UINT)m_sNero1.GetLength() == m_CtrlNero1.GetLimitText())
	{
		m_CtrlNero2.SetFocus();
		m_CtrlNero2.SetSel(0, -1, FALSE);

	}
	
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::CheckResetMsgBox()
{
	if(m_bResetMsgBox)
	{
		m_bResetMsgBox = FALSE;

		CString sWndTitle = _T("Reset des Dongles ");	
		CString sMsgBox = _T("\r\nACHTUNG:\r\nDeaktivieren eines bereits freigeschalteten");
		sMsgBox += _T("\r\nFeatures:\r\n\r\n1. Alle Features deaktivieren");
		sMsgBox += _T("\r\n2. Jacob/Savic/Tasha oder Q Unit starten (reset ohne Features)");
		sMsgBox += _T("\r\n3. freizuschaltende Feature aktivieren");
		MessageBox(sMsgBox, sWndTitle, MB_OK | MB_ICONINFORMATION);		
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::OnCkAudioTest() 
{
	theApp.ShellExecute(_T("AudioTest.exe"));
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::OnBtnAudioVolumes() 
{
	theApp.ShellExecute(_T("SNDVOL32.EXE"));
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::OnStnClickedKam8()
{
	// TODO: Add your control notification handler code here
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::OnBnClickedBtnCfInit()
{
	CString sCmd = theApp.m_sStartDir + _T("\\VideteCheckDisk.exe /e /a");
	int nReturnCode = CVipCleanNTApp::ExecuteProgram(sCmd, TRUE);
	if (nReturnCode == 2)
	{

	}
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::OnBnClickedBtnCfUninit()
{
	CString sCmd = theApp.m_sStartDir + _T("\\VideteCheckDisk.exe /d");
	int nReturnCode = CVipCleanNTApp::ExecuteProgram(sCmd, TRUE);
	if (nReturnCode == 2)
	{

	}
}
/////////////////////////////////////////////////////////////////////////////
void CVipCleanNTDlg::OnBnClickedBtnInitDisks()
{
	CString sCmd = theApp.m_sStartDir + _T("\\VideteCheckDisk.exe /a");
	int nReturnCode = CVipCleanNTApp::ExecuteProgram(sCmd, TRUE);
	if (nReturnCode == 2)
	{

	}
}
/////////////////////////////////////////////////////////////////////////////
