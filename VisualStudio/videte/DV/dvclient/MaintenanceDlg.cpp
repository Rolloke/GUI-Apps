// MaintenanceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DVClient.h"
#include "MaintenanceDlg.h"
#include "CPanel.h"
#include <wkclasses\CHDDTApi.h>	//to get HDD informations
#include <wkclasses\csmart.h>	//to get SMART informations

// CMaintenanceDlg

IMPLEMENT_DYNAMIC(CMaintenanceDlg, CSkinDialog)
CMaintenanceDlg::CMaintenanceDlg(CPanel* pParent /*=NULL*/)
: CSkinDialog(CMaintenanceDlg::IDD, pParent)
{
	CreateTransparent(StyleBackGroundColorChangeBrushed, CPanel::m_BaseColorBackGrounds);
	m_hCursor	= NULL;
	m_pPanel	= pParent;
	m_bIntervallOnOff = FALSE;
	m_sIntervall	= _T("12");

	//init installer data with videte
	m_sInstallerCompany = _T("videte IT AG");
	m_sInstallerStreet = _T("Westring 431");
	m_sInstallerPostcode = _T("24118");
	m_sInstallerCity = _T("Kiel");
	m_sInstallerTelefon = _T("0431/57929-0");
	m_sInstallerEmail = _T("info@videte.com");
	m_sInstallerCountry = _T("Deutschland");
	m_bResetHDDTemperatures = FALSE;

	m_sReport = InitSysReport();
	AfxInitRichEdit();
}

CMaintenanceDlg::~CMaintenanceDlg()
{
}
/////////////////////////////////////////////////////////////////////////////
void CMaintenanceDlg::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_MAINT_USERNAME, m_sUserName);
	DDX_Text(pDX, IDC_MAINT_PASS, m_sUserPass);
	DDX_Text(pDX, IDC_MAINT_COMPANY, m_sInstallerCompany);
	DDX_Text(pDX, IDC_MAINT_STR, m_sInstallerStreet);
	DDX_Text(pDX, IDC_MAINT_CODE, m_sInstallerPostcode);
	DDX_Text(pDX, IDC_MAINT_CITY, m_sInstallerCity);
	DDX_Text(pDX, IDC_MAINT_TELEFON, m_sInstallerTelefon);
	DDX_Text(pDX, IDC_MAINT_EMAIL, m_sInstallerEmail);
	DDX_Text(pDX, IDC_MAINT_COUNTRY, m_sInstallerCountry);
	DDX_Text(pDX, IDC_MAINT_START_INTERVALL, m_sIntervallStartDate);
	DDX_Control(pDX, IDC_MAINT_TXT_LOGIN_ERROR, m_ctrlLoginError);
	DDX_CBString(pDX, IDC_MAINT_COMBO_INTERVALL, m_sIntervall);
	DDX_Control(pDX, IDC_MAINT_COMBO_INTERVALL, m_ctrlComboIntervall);
	DDX_Check(pDX, IDC_MAINT_CHECK_NAG, m_bIntervallOnOff);
	DDX_Control(pDX, IDC_MAINT_START_INTERVALL, m_ctrlIntervallDateTime);
	DDX_Control(pDX, IDC_MAINT_TXT_SHOW_MAINT, m_ctrlTextShowMaint);
	DDX_Control(pDX, IDC_MAINT_CHECK_NAG, m_ctrlCheckShowMaint);
	DDX_Control(pDX, IDC_MAINT_GRP_SETTINGS, m_ctrlGrpSettingsMaint);
	DDX_Control(pDX, IDC_MAINT_GRP_SYSREPORT, m_ctrlGrpSystemReport);
	DDX_Control(pDX, IDC_MAINT_GRP_INSTALLER, m_ctrlGrpSettingsInstaller);
	DDX_Control(pDX, IDC_MAINT_GRP_SYSCHECK, m_ctrlGrpSystemCheck);
	DDX_Control(pDX, IDC_MAINT_GRP_LOGIN, m_ctrlGrpLogin);
	DDX_Control(pDX, IDC_MAINT_TXT_USER, m_ctrlLoginTxtUsername);
	DDX_Control(pDX, IDC_MAINT_TXT_PASS, m_ctrlLoginTxtPassword);
	DDX_Control(pDX, IDC_MAINT_USERNAME, m_ctrlLoginUsername);
	DDX_Control(pDX, IDC_MAINT_PASS, m_ctrlLoginPassword);
	DDX_Control(pDX, IDC_MAINT_LOGIN, m_ctrlLoginButton);
	DDX_Text(pDX, IDC_MAINT_REPORT, m_sReport);
	DDX_Control(pDX, IDC_MAINT_RICHEDIT_MANUAL_MAINT_RTF, m_ctrlInstruction);
	DDX_Control(pDX, IDC_MAINT_BUTTON_OK, m_ctrlOK);
	DDX_Control(pDX, IDC_MAINT_BUTTON_SAVE, m_ctrlSave);
	
}
BOOL CMaintenanceDlg::StretchElements()
{
	return m_bResize;
}
/////////////////////////////////////////////////////////////////////////////
// CMaintenanceDlg message handlers
BOOL CMaintenanceDlg::OnInitDialog() 
{
	m_bResize = TRUE;
	CSkinDialog::OnInitDialog();
	
	//wieder auf FALSE setzten, da ansonsten beim resize auch alle Elemente
	//des Dialogs mit rezised werden
	m_bResize = FALSE;

	m_hCursor = theApp.m_hArrow;

	AutoCreateSkinStatic();
	AutoCreateSkinEdit();
	AutoCreateSkinButtons();

		
	//init ok and save button
	m_imagelistOK.Create(IDB_ENTER, 16, 0, SKIN_COLOR_KEY);
	m_ctrlOK.SetImageList(&m_imagelistOK);

	m_imagelistSave.Create(IDB_EXPORT, 21, 0, SKIN_COLOR_KEY);
	m_ctrlSave.SetImageList(&m_imagelistSave);
	
	InitDlgForLoginOnly();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

// CMaintenanceDlg message handlers
BEGIN_MESSAGE_MAP(CMaintenanceDlg, CSkinDialog)
	ON_BN_CLICKED(IDC_MAINT_BUTTON_OK, OnBnClickedMaintButtonOk)
	ON_BN_CLICKED(IDC_MAINT_BUTTON_SAVE, OnBnClickedMaintButtonSave)
	ON_BN_CLICKED(IDC_MAINT_LOGIN,	OnLogin)
	ON_EN_CHANGE(IDC_MAINT_COMPANY, OnChangeSettings)
	ON_EN_CHANGE(IDC_MAINT_STR,		OnChangeSettings)
	ON_EN_CHANGE(IDC_MAINT_CODE,	OnChangeSettings)
	ON_EN_CHANGE(IDC_MAINT_CITY,	OnChangeSettings)
	ON_EN_CHANGE(IDC_MAINT_TELEFON, OnChangeSettings)
	ON_EN_CHANGE(IDC_MAINT_EMAIL,	OnChangeSettings)
	ON_EN_CHANGE(IDC_MAINT_COUNTRY, OnChangeSettings)
	ON_CBN_SELCHANGE(IDC_MAINT_COMBO_INTERVALL, OnChangeSettings)
	ON_BN_CLICKED(IDC_MAINT_CHECK_NAG, OnBnClickedIntervallOnOff)

	ON_BN_CLICKED(IDC_MAINT_BUTTON_STARTDATE, OnBnClickedMaintButtonStartdate)
	ON_BN_CLICKED(IDC_MAINT_BUTTON_REPORT, OnBnClickedMaintButtonReport)
	ON_WM_SETCURSOR()

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
void CMaintenanceDlg::InitDlgForLoginOnly()
{
	//move the OK button near to the login section and resize (make small) the dialog
	CRect rGrpLogin;
	CRect*p;

	//get coords of OK Button (IDC_MAINT_BUTTON_OK)
	if(m_RectMap.Lookup((WORD)IDC_MAINT_BUTTON_OK, (void*&)p))
	{
		m_Rect_CtrlOK_OrigPosition = *p;
		p = NULL;
	}

	//get coords of LoginGrp (IDC_MAINT_GRP_LOGIN)
	if(m_RectMap.Lookup((WORD)IDC_MAINT_GRP_LOGIN, (void*&)p))
	{
		rGrpLogin = *p;
		p = NULL;
	}

	//move OK Button near to Login Grp
	CRect rNewPositionOKButton(rGrpLogin.right-m_Rect_CtrlOK_OrigPosition.Width()-5,
							   rGrpLogin.bottom+10,
							   rGrpLogin.right,
							   rGrpLogin.bottom+10+m_Rect_CtrlOK_OrigPosition.Height());

	m_ctrlOK.MoveWindow(rNewPositionOKButton);

	//resize maintenance dlg to login dlg
	int left =	0;
	int top =	0;
	int right = rNewPositionOKButton.right+28;
	int bottom = rNewPositionOKButton.bottom+20;

	CRect rLoginDlgSize(left, top, right, bottom);

	MoveWindow(rLoginDlgSize);
	
	//move the small MaintenanceDialog above or beneath the panel
	CRect rcPanel;
	if (m_pPanel)
	{
		m_pPanel->GetWindowRect(rcPanel);

		// MaintenanceDialog oberhalb des Panels positionieren
		int nX	= rcPanel.left;
		int	nY	= rcPanel.top - rLoginDlgSize.Height();

		if (nX < 0 || nY < 0)
		{
			// MaintenanceDialog unterhalb des Panels positionieren
			nX	= rcPanel.left;
			nY	= rcPanel.bottom;
		}
		SetWindowPos(&wndTopMost, nX, nY, -1, -1, SWP_NOSIZE);
	}
	Invalidate();
}
/////////////////////////////////////////////////////////////////////////////
void CMaintenanceDlg::ResizeDlgToOriginSize()
{
	//move OK button to original position and resize the dialog to original size
 	m_ctrlOK.MoveWindow(m_Rect_CtrlOK_OrigPosition);
	MoveWindow(0, 0, m_InitialWindowRect.Width(), m_InitialWindowRect.Height());

}
/////////////////////////////////////////////////////////////////////////////
void CMaintenanceDlg::Initialize()
{
	//fill dlg with saved settings
	GetSettings();
}
/////////////////////////////////////////////////////////////////////////////
void CMaintenanceDlg::OnBnClickedMaintButtonOk()
{
	EndDialog(IDOK);
}
/////////////////////////////////////////////////////////////////////////////
void CMaintenanceDlg::OnBnClickedMaintButtonSave()
{
	SetSettings();
	m_ctrlSave.EnableWindow(FALSE);
}
/////////////////////////////////////////////////////////////////////////
void CMaintenanceDlg::OnLogin()
{
	//hide error text wrong login
	m_ctrlLoginError.ShowWindow(SW_HIDE);
	
	UpdateData();
	//check user and password

	if(    m_sUserName == "installer"
		&& m_sUserPass == "installer40")
	{
		ResizeDlgToOriginSize();
		//empty login data
		m_sUserName.Empty();
		m_sUserPass.Empty();

		//show or hide all sections
		CWK_Profile wkp;
		BOOL bIntervallOnOff;
		bIntervallOnOff = wkp.GetInt(SECTION_MAINTENANCE, MAINT_INTERVALL_ONOFF, FALSE);
		ShowDlgItems(IDC_MAINT_GRP_INSTALLER, TRUE, bIntervallOnOff);
		ShowDlgItems(IDC_MAINT_GRP_SYSCHECK, TRUE, bIntervallOnOff);
		ShowDlgItems(IDC_MAINT_GRP_SYSREPORT, TRUE, bIntervallOnOff);
		ShowDlgItems(IDC_MAINT_GRP_SETTINGS, TRUE, bIntervallOnOff);
		m_ctrlSave.ShowWindow(SW_SHOW);

		//hide login section
		ShowLoginGrp(SW_HIDE);

		//always enable following controls
		m_ctrlGrpSettingsMaint.EnableWindow(TRUE);
		m_ctrlTextShowMaint.EnableWindow(TRUE);
		m_ctrlCheckShowMaint.EnableWindow(TRUE);
		

		//DateTime picker is always disabled
		m_ctrlIntervallDateTime.EnableWindow(FALSE);

		Initialize();
		//init maintenance repair manual
		m_ctrlInstruction.SetBackgroundColor(FALSE, RGB(190, 190, 190));
		
		CString sMaintenance;
		CString sEnu = _T("enu");
		if(m_pPanel)
		{
			sEnu = m_pPanel->GetCurrentLanguageAbbr();
		}

		sMaintenance.Format(_T("C:\\DV\\Maintenance%s.rtf"), sEnu);

		CString sFile(sMaintenance); //default english
		if(GetFileAttributes(sFile) == 0xffffffff)
		{
			sMaintenance.Format(_T("C:\\DV\\Maintenance%s.rtf"), _T("enu"));
			sFile = sMaintenance;
		}

		if (!sFile.IsEmpty())
		{
			if(m_InstructionFile.Open(sFile,CFile::modeRead))
			{
				EDITSTREAM es;
				es.dwCookie = (DWORD)this;
				es.pfnCallback = EditStreamCallback;
				es.dwError = 0;
				m_ctrlInstruction.StreamIn(SF_RTF,es);
				m_InstructionFile.Close();
			}
		}

	}
	else
	{
		m_ctrlLoginError.ShowWindow(SW_SHOW);
	}
	InvalidateRect(NULL);
}
/////////////////////////////////////////////////////////////////////////////
DWORD CALLBACK CMaintenanceDlg::EditStreamCallback( DWORD dwCookie, // application-defined value
													LPBYTE pbBuff,      // data buffer
													LONG cb,            // number of bytes to read or write
													LONG *pcb           // number of bytes transferred
													)
{
	CMaintenanceDlg* pThis = (CMaintenanceDlg*)dwCookie;
	DWORD dwRet = 0;

	TRY
	{
		*pcb = pThis->m_InstructionFile.Read(pbBuff,cb);
		TRACE(_T("read %d Bytes of %d\n"),*pcb,cb);
	}
	CATCH (CFileException, cfe)
	{
		dwRet = cfe->m_lOsError;
	}
	END_CATCH;

	return dwRet;
}
/////////////////////////////////////////////////////////////////////////
void CMaintenanceDlg::OnChangeSettings()
{
	m_ctrlSave.EnableWindow(TRUE);
}
/////////////////////////////////////////////////////////////////////////
void CMaintenanceDlg::UpdateDlgItems()
{
	//show or hide all sections
	ShowDlgItems(IDC_MAINT_GRP_INSTALLER, TRUE, m_bIntervallOnOff);
	ShowDlgItems(IDC_MAINT_GRP_SYSCHECK, TRUE, m_bIntervallOnOff);
	ShowDlgItems(IDC_MAINT_GRP_SYSREPORT, TRUE, m_bIntervallOnOff);
	ShowDlgItems(IDC_MAINT_GRP_SETTINGS, TRUE, m_bIntervallOnOff);

	//m_ctrlGrpSettingsInstaller.SetTextColorDisabled(CSkinButton::m_BaseColor);
	//m_ctrlGrpSystemCheck.SetTextColorDisabled(CSkinButton::m_BaseColor);
	//m_ctrlGrpSystemReport.SetTextColorDisabled(CSkinButton::GetTextColorNormalG());
	//m_ctrlGrpSettingsMaint.SetTextColorDisabled(CSkinButton::m_BaseColor);
	//hide login section
	ShowLoginGrp(SW_HIDE);


	//always enable IDC_MAINT_CHECK_NAGT
	m_ctrlCheckShowMaint.EnableWindow(TRUE);


	//DateTime picker is always disabled
	m_ctrlIntervallDateTime.EnableWindow(FALSE);

	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////
// bShow -> show/hide section, bEnable -> enable/disable section
void CMaintenanceDlg::ShowDlgItems(int nDlg, BOOL bShow /*=TRUE*/, BOOL bEnable /*=TRUE*/)
{
	CWnd *pWnd, *pWndGrp;
	int iShowHide;
	bShow ? iShowHide  = SW_SHOW :  iShowHide  = SW_HIDE;
	pWnd = GetDlgItem(nDlg);
	if (pWnd)
	{
		CRect rc, rcGrp;
		pWnd->GetWindowRect(&rcGrp);
		pWndGrp = pWnd;
		pWnd = GetWindow(GW_CHILD);
		while (pWnd)
		{
			pWnd->GetWindowRect(&rc);
			if (rcGrp.PtInRect(rc.TopLeft()))
			{
				if(pWndGrp != pWnd)
				{
					pWnd->EnableWindow(bEnable);
				}
				pWnd->ShowWindow(iShowHide);
			}
			pWnd = pWnd->GetWindow(GW_HWNDNEXT);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMaintenanceDlg::ShowLoginGrp(int iShow)
{
	m_ctrlGrpLogin.ShowWindow(iShow);
	m_ctrlLoginTxtUsername.ShowWindow(iShow);
	m_ctrlLoginTxtPassword.ShowWindow(iShow);
	m_ctrlLoginUsername.ShowWindow(iShow);
	m_ctrlLoginPassword.ShowWindow(iShow);
	m_ctrlLoginButton.ShowWindow(iShow);
	m_ctrlLoginError.ShowWindow(iShow);
}
/////////////////////////////////////////////////////////////////////////////
void CMaintenanceDlg::OnBnClickedIntervallOnOff()
{
	//switch intervall on/off
	m_bIntervallOnOff = !m_bIntervallOnOff;

	UpdateDlgItems();

	OnChangeSettings();
}
/////////////////////////////////////////////////////////////////////////////
//creates a system report
void CMaintenanceDlg::OnBnClickedMaintButtonReport()
{
	//OEM = g_Dongle.GetOemCode();
	//CWK_Dongle::OemCode OEM;
	CString sReport, sDateTime;
	CSystemTime st;
	st.GetLocalTime();
	sDateTime = st.GetDateTime();
	sReport.Format(_T("System Report: %s \r\n-------------------------------------\r\n\r\n"), sDateTime);

	//get serial number of unit (computername)
	CString sSerialComputer, sSerialStr(_T("Serial number: ")), sSerial;
	CWK_Profile prof(CWK_Profile::WKP_ABSOLUT,HKEY_LOCAL_MACHINE,_T(""), _T(""));
	sSerialComputer = prof.GetString(_T("System\\CurrentControlSet\\Control\\ComputerName\\ComputerName"),_T("ComputerName"),_T(""));
	sSerial.Format(_T("%s%s"), sSerialStr, sSerialComputer);
	sReport += sSerial + _T("\r\n");

	//get software type and version 
	CString sVersion, sVersionStr(_T("Version:      ")); 


	sVersion = GetOEMVersion();
	sReport = sReport + _T("\r\n") + sVersionStr + sVersion + _T("\r\n");

	//get model, serial ,firmware and SMART from HDDs
	CString sHDD = GetHDDInformations();
	sReport  = sReport + _T("\r\n") + sHDD;

	m_sReport = sReport;
	UpdateData(FALSE);
	WK_TRACE(_T("%s"), sReport);

}
/////////////////////////////////////////////////////////////////////////////
void CMaintenanceDlg::OnBnClickedMaintButtonStartdate()
{
	//set new (current) start date
	CSystemTime st;
	st.GetLocalTime();
	m_sIntervallStartDate = st.GetDBDate();
	m_ctrlIntervallDateTime.SetTime(st);

	//prepare reset max HDD temperatures in registry
	m_bResetHDDTemperatures = TRUE;

	OnChangeSettings();
}
/////////////////////////////////////////////////////////////////////////////
CString CMaintenanceDlg::GetHDDInformations()
{
	CString sHDD, sDummy;

	CHDDTApi Hdd;
	DRIVE_INFO drive_info;
	int nTemperatur = 0;

	for (UCHAR i=0;i<MAX_IDE_DRIVES;i++)
	{
		if (Hdd.GetDriveInfo(i, drive_info)!=0)  // get HDD information
		{
			sDummy.Format(_T("Hard Drive (%d)\r\n"), i); 
			sHDD += sDummy;
			sDummy.Format(_T("===========================\r\n"));
			sHDD += sDummy;
			sDummy.Format(_T("Model       : %s\r\n"), CString(drive_info.ModelNumber));
			sHDD += sDummy;
			sDummy.Format(_T("Serial      : %s\r\n"), CString(drive_info.FirmWareRev));
			sHDD += sDummy;
			sDummy.Format(_T("Firmware    : %s\r\n"), CString(drive_info.SerialNumber));
			sHDD += sDummy;

			if (Hdd.GetDriveTemperatur(i, nTemperatur))
			{
				sDummy.Format(_T("Temperature : %d °C\r\n"), nTemperatur);

				//get the highest reached HDD temperature from registry 
				CWK_Profile wkp;
				CString sKey, sTemperature, sMaxTemp;
				sKey.Format(_T("Temp%d"), i);
				sTemperature = wkp.GetString(SECTION_MAINTENANCE, sKey, _T(""));
				if(!sTemperature.IsEmpty())
				{
					sMaxTemp.Format(_T("max. Temp. : %s °C\r\n"), sTemperature);
					sDummy += sMaxTemp;
				}
			}
			else
			{			
				sDummy.Format(_T("Temperature : not available\r\n"));
			}
			sHDD += sDummy;

			//get SMART info
			CString sSmart = GetSMARTparameters(i);
			sHDD += sSmart;
			sDummy = _T("\r\n");
			sHDD += sDummy;
		}
	}

	return sHDD;
}
/////////////////////////////////////////////////////////////////////////////
CString CMaintenanceDlg::GetSMARTparameters(int nDrive)
{
	CString sSmart(_T("S.M.A.R.T status: not available \r\n\r\n"));
	GETVERSIONOUTPARAMS VersionParams;

	CSmart smart;
	if (smart.Open(nDrive))
	{
		if (smart.GetVersionInfo(VersionParams))
		{
			BOOL bState = FALSE;
			if (smart.EnableSmart())
			{
				smart.Identify();
				if (smart.CheckDrive(bState, FALSE))
				{
					if (bState)
					{
						sSmart.Format(_T("S.M.A.R.T status: OK \r\n\r\n"));
					}
					else
					{
						sSmart.Format(_T("S.M.A.R.T status: failed! \r\n\r\n"));
					}
				}
			}
		}
	}
	return sSmart;
}
/////////////////////////////////////////////////////////////////////////////
CString CMaintenanceDlg::GetOEMVersion()
{
	CString sOEMName;

	// OEM Spezifische Einstellungen
	OEM eOem = OemAlarmCom;
	int nNrOfCameras = 8;

	CWK_Profile prof;

	// which OEM?
	eOem = (OEM)prof.GetInt(_T("DV\\DVClient"), _T("Oem"),  (int)OemDTS);
	nNrOfCameras = prof.GetInt(_T("DV\\Process"), _T("NrOfCameras"), 4);

	// get version number from DVProcess
	CString sOEM, sVersion;
	GetModuleFileName(theApp.m_hInstance,sOEM.GetBuffer(_MAX_PATH),_MAX_PATH);
	sOEM.ReleaseBuffer();
	int p = sOEM.ReverseFind(_T('\\'));
	sOEM = sOEM.Left(p+1) + _T("\\DVProcess.exe");
#ifdef _DEBUG
	sOEM = _T("C:\\DV\\DVProcess.exe");
#endif 
	sVersion = WK_GetFileVersion(sOEM);

	// OEM Spezifische Einstellungen
	CString sTemp, sVersionText;
	//	sVersionText.LoadString(IDS_VERSION);
	if (eOem == OemDTS)
	{
		sTemp = _T("videte IT® DTS %d00 ") + sVersionText + _T(" %s");
		sOEMName.Format(sTemp, nNrOfCameras, sVersion);
	}
	else if (eOem == OemAlarmCom)
	{
		sOEMName = _T("Siemens VDR ") + sVersionText + _T(" ") + sVersion;
	}
	else if (eOem == OemPOne)
	{
		sOEMName = _T("Protection One VDR ")+ sVersionText + _T(" ") + sVersion;
	}
	else if (eOem == OemDResearch)
	{
		sTemp = _T("DResearch Vidia 500 - %d ") + sVersionText + _T(" %s");
		sOEMName.Format(sTemp, nNrOfCameras, sVersion);
	}
	else if (eOem == OemSantec)
	{
		sTemp = _T("Santec SDR-%d ") + sVersionText + _T(" %s");
		sOEMName.Format(sTemp, nNrOfCameras, sVersion);
	}
	return sOEMName;
}
/////////////////////////////////////////////////////////////////////////////
void CMaintenanceDlg::GetSettings()
{
	CWK_Profile wkp;

	//get installer settings
	CString sEmpty(_T(""));
	m_sInstallerCompany = wkp.GetString(SECTION_MAINTENANCE, MAINT_INST_COMPANY, m_sInstallerCompany);
	m_sInstallerStreet = wkp.GetString(SECTION_MAINTENANCE, MAINT_INST_STREET, m_sInstallerStreet);
	m_sInstallerPostcode = wkp.GetString(SECTION_MAINTENANCE, MAINT_INST_POSTCODE, m_sInstallerPostcode);
	m_sInstallerCity = wkp.GetString(SECTION_MAINTENANCE, MAINT_INST_CITY, m_sInstallerCity);
	m_sInstallerTelefon = wkp.GetString(SECTION_MAINTENANCE, MAINT_INST_TELEFON, m_sInstallerTelefon);
	m_sInstallerEmail = wkp.GetString(SECTION_MAINTENANCE, MAINT_INST_EMAIL, m_sInstallerEmail);
	m_sInstallerCountry = wkp.GetString(SECTION_MAINTENANCE, MAINT_INST_COUNTRY, m_sInstallerCountry);

	//get intervall settings
	m_sIntervall.Format(_T("%d"),wkp.GetInt(SECTION_MAINTENANCE, MAINT_INTERVALL, NULL));
	m_bIntervallOnOff = wkp.GetInt(SECTION_MAINTENANCE, MAINT_INTERVALL_ONOFF, FALSE);
	m_sIntervallStartDate = wkp.GetString(SECTION_MAINTENANCE, MAINT_INTERVALL_STARTDATE, sEmpty);
	CSystemTime st;
	if(st.SetDBDate(m_sIntervallStartDate))
	{
		m_ctrlIntervallDateTime.SetTime(st);
	}

	m_sReport = InitSysReport();
	m_bResetHDDTemperatures = FALSE;
	UpdateDlgItems();
}
/////////////////////////////////////////////////////////////////////////////
void CMaintenanceDlg::SetSettings()
{
	UpdateData();
	CWK_Profile wkp;

	//write installer settings
	wkp.WriteString(SECTION_MAINTENANCE, MAINT_INST_COMPANY, m_sInstallerCompany);
	wkp.WriteString(SECTION_MAINTENANCE, MAINT_INST_STREET, m_sInstallerStreet);
	wkp.WriteString(SECTION_MAINTENANCE, MAINT_INST_POSTCODE, m_sInstallerPostcode);
	wkp.WriteString(SECTION_MAINTENANCE, MAINT_INST_CITY, m_sInstallerCity);
	wkp.WriteString(SECTION_MAINTENANCE, MAINT_INST_TELEFON, m_sInstallerTelefon);
	wkp.WriteString(SECTION_MAINTENANCE, MAINT_INST_EMAIL, m_sInstallerEmail);
	wkp.WriteString(SECTION_MAINTENANCE, MAINT_INST_COUNTRY, m_sInstallerCountry);

	//write intervall settings
	int iIntervall = _wtoi(m_sIntervall);
	wkp.WriteInt(SECTION_MAINTENANCE, MAINT_INTERVALL, iIntervall);
	wkp.WriteInt(SECTION_MAINTENANCE, MAINT_INTERVALL_ONOFF, m_bIntervallOnOff);

	CSystemTime* pst = NULL;
	pst = new CSystemTime();
	m_ctrlIntervallDateTime.GetTime(pst);
	if(pst)
	{
		m_sIntervallStartDate = pst->GetDBDate();
		wkp.WriteString(SECTION_MAINTENANCE, MAINT_INTERVALL_STARTDATE, m_sIntervallStartDate);
	}
	WK_DELETE(pst);

	//reset HDD temperatures 
	if(m_bResetHDDTemperatures)
	{
		for (UCHAR i=0;i<MAX_IDE_DRIVES;i++)
		{
			CWK_Profile wkp;
			CString sKey, sTemperature;
			sKey.Format(_T("Temp%d"), i);
			sTemperature = wkp.GetString(SECTION_MAINTENANCE, sKey, _T(""));
			if(!sTemperature.IsEmpty())
			{		
				wkp.WriteString(SECTION_MAINTENANCE,sKey,_T(""));
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
CString CMaintenanceDlg::InitSysReport()
{
	CString sReturn;

	for(int i=0; i<MAX_SYREPORT_LINES; i++)
	{
		sReturn += _T("\r\n");
	}
	return sReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMaintenanceDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (m_hCursor)
	{
		if (SetCursor(m_hCursor) != NULL)
			return TRUE;		
	}	

	return CSkinDialog::OnSetCursor(pWnd, nHitTest, message);
}