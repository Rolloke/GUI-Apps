#include "stdafx.h"
#include "SystemVerwaltung.h"
#include "MaintenancePage.h"
#include ".\maintenancepage.h"
#include "oemgui\AutoRun.h"		//to get idip system informations
#include <wkclasses\CHDDTApi.h>	//to get HDD informations
#include <wkclasses\csmart.h>	//to get SMART informations

#define SECTION_MAINTENANCE			_T("Maintenance")	
#define MAINT_INST_COMPANY			_T("Company")	
#define MAINT_INST_STREET			_T("Street")	
#define MAINT_INST_POSTCODE			_T("Postcode")	
#define MAINT_INST_CITY				_T("City")	
#define MAINT_INST_TELEFON			_T("Telefon")
#define MAINT_INST_EMAIL			_T("Email")	
#define MAINT_INST_COUNTRY			_T("Country")	
#define MAINT_INTERVALL				_T("Intervall")	
#define MAINT_INTERVALL_ONOFF		_T("IntervallOnOff")
#define MAINT_INTERVALL_STARTDATE	_T("IntervallStartDate")	
// CMaintenancePage dialog

IMPLEMENT_DYNAMIC(CMaintenancePage, CSVPage)
CMaintenancePage::CMaintenancePage(CSVView* pParent /*=NULL*/)
: CSVPage(CMaintenancePage::IDD)
, m_sUserName(_T(""))
, m_sUserPass(_T(""))
, m_sIntervallStartDate(_T(""))
, m_sIntervall(_T("12"))
, m_bIntervallOnOff(FALSE)
, m_sReport(_T(""))
{
	m_pParent = pParent;

	//init installer data with videte
	m_sInstallerCompany = COemGuiApi::GetProducerName();
	m_sInstallerStreet = COemGuiApi::GetStreet();
	m_sInstallerPostcode = _T("24118");
	m_sInstallerCity = _T("Kiel");
	m_sInstallerTelefon = _T("0431/57929-0");
	m_sInstallerEmail = COemGuiApi::GetEmail();
	m_sInstallerCountry = _T("Deutschland");
	m_bResetHDDTemperatures = FALSE;
	
	Create(IDD,pParent);
}
/////////////////////////////////////////////////////////////////////////////
CMaintenancePage::~CMaintenancePage()
{
}

void CMaintenancePage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
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
	DDX_Check(pDX, IDC_MAINT_CHECK_NAG, m_bIntervallOnOff);
	DDX_Control(pDX, IDC_MAINT_START_INTERVALL, m_ctrlIntervallDateTime);
	DDX_Control(pDX, IDC_MAINT_TXT_SHOW_MAINT, m_ctrlTextShowMaint);
	DDX_Control(pDX, IDC_MAINT_CHECK_NAG, m_ctrlCheckShowMaint);
	DDX_Control(pDX, IDC_MAINT_GRP_SETTINGS, m_ctrlGrpSettingsMaint);
	DDX_Control(pDX, IDC_MAINT_GRP_LOGIN, m_ctrlGrpLogin);
	DDX_Control(pDX, IDC_MAINT_TXT_USER, m_ctrlLoginTxtUsername);
	DDX_Control(pDX, IDC_MAINT_TXT_PASS, m_ctrlLoginTxtPassword);
	DDX_Control(pDX, IDC_MAINT_USERNAME, m_ctrlLoginUsername);
	DDX_Control(pDX, IDC_MAINT_PASS, m_ctrlLoginPassword);
	DDX_Control(pDX, IDC_MAINT_LOGIN, m_ctrlLoginButton);
	DDX_Text(pDX, IDC_MAINT_REPORT, m_sReport);
	DDX_Control(pDX, IDC_MAINT_BUTTON_REPORT_EXPORT, m_ctrlSaveReport);
	DDX_Control(pDX, IDC_MAINT_RICHEDIT_MANUAL_MAINT_RTF, m_ctrlInstruction);
	DDX_Control(pDX, IDC_MAINT_TXT_SYSTEMCHECK_DESCRIPTION, m_ctrlSystemTestText);
	DDX_Control(pDX, IDC_MAINT_BUTTON_SYSTEST, m_ctrlSystemTestButton);
}


BEGIN_MESSAGE_MAP(CMaintenancePage, CSVPage)
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
	ON_BN_CLICKED(IDC_MAINT_BUTTON_REPORT_EXPORT, OnBnClickedMaintButtonReportExport)
	ON_BN_CLICKED(IDC_MAINT_BUTTON_SYSTEST, OnBnClickedMaintButtonSystest)
END_MESSAGE_MAP()


// CMaintenancePage message handlers
/////////////////////////////////////////////////////////////////////////
BOOL CMaintenancePage::OnInitDialog()
{
	CSVPage::OnInitDialog();

	CWnd *pWnd = GetDlgItem(IDC_MAINT_USERNAME);
	if (pWnd)
	{
		GotoDlgCtrl(pWnd);
        return FALSE; 
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
DWORD CALLBACK CMaintenancePage::EditStreamCallback( DWORD dwCookie, // application-defined value
													 LPBYTE pbBuff,      // data buffer
													 LONG cb,            // number of bytes to read or write
													 LONG *pcb           // number of bytes transferred
													)
{
	CMaintenancePage* pThis = (CMaintenancePage*)dwCookie;
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
void CMaintenancePage::UpdateDlgItems()
{
	//show or hide all sections
	//CWK_Profile wkp;
	//BOOL bIntervallOnOff;
	//bIntervallOnOff = wkp.GetInt(SECTION_MAINTENANCE, MAINT_INTERVALL_ONOFF, FALSE);
	ShowDlgItems(IDC_MAINT_GRP_INSTALLER, TRUE, m_bIntervallOnOff);
	ShowDlgItems(IDC_MAINT_GRP_SYSCHECK, TRUE, m_bIntervallOnOff);
	ShowDlgItems(IDC_MAINT_GRP_SYSREPORT, TRUE, m_bIntervallOnOff);
	ShowDlgItems(IDC_MAINT_GRP_SETTINGS, TRUE, m_bIntervallOnOff);
	
	//hide login section
	ShowLoginGrp(SW_HIDE);

	//always enable IDC_MAINT_GRP_SETTINGS, IDC_MAINT_CHECK_NAG and IDC_MAINT_TXT_SHOW_MAINT
	m_ctrlGrpSettingsMaint.EnableWindow(TRUE);
	m_ctrlTextShowMaint.EnableWindow(TRUE);
	m_ctrlCheckShowMaint.EnableWindow(TRUE);

	//DateTime picker is always disabled
	m_ctrlIntervallDateTime.EnableWindow(FALSE);

	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////
// bShow -> show/hide section, bEnable -> enable/disable section
void CMaintenancePage::ShowDlgItems(int nDlg, BOOL bShow /*=TRUE*/, BOOL bEnable /*=TRUE*/)
{
	CWnd *pWnd;
	int iShowHide;
	bShow ? iShowHide  = SW_SHOW :  iShowHide  = SW_HIDE;
	pWnd = GetDlgItem(nDlg);
	if (pWnd)
	{
		CRect rc, rcGrp;
		pWnd->GetWindowRect(&rcGrp);
		pWnd = GetWindow(GW_CHILD);
		while (pWnd)
		{
			pWnd->GetWindowRect(&rc);
			if (rcGrp.PtInRect(rc.TopLeft()))
			{
				pWnd->EnableWindow(bEnable);
				pWnd->ShowWindow(iShowHide);
			}
			pWnd = pWnd->GetWindow(GW_HWNDNEXT);
		}
	}

	//TODO TKR verstecken, bis Zeit ist, das Speichern des Reports einzubauen
	m_ctrlSaveReport.ShowWindow(SW_HIDE);
	//TODO TKR verstecken, bis Zeit ist, den SystemTest einzubauen
	m_ctrlSystemTestText.ShowWindow(SW_HIDE);
	m_ctrlSystemTestButton.ShowWindow(SW_HIDE);
}
/////////////////////////////////////////////////////////////////////////
void CMaintenancePage::OnLogin()
{
	//hide error text wrong login
	m_ctrlLoginError.ShowWindow(SW_HIDE);

	UpdateData();
	//check user and password

	if(    m_sUserName == "installer"
		&& m_sUserPass == "installer40")
	{
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

		//hide login section
		ShowLoginGrp(SW_HIDE);

		//always enable IDC_MAINT_GRP_SETTINGS, IDC_MAINT_CHECK_NAG and IDC_MAINT_TXT_SHOW_MAINT
		m_ctrlGrpSettingsMaint.EnableWindow(TRUE);
		m_ctrlTextShowMaint.EnableWindow(TRUE);
		m_ctrlCheckShowMaint.EnableWindow(TRUE);
		
		//DateTime picker is always disabled
		m_ctrlIntervallDateTime.EnableWindow(FALSE);

		Initialize();
		//init maintenance repair manual
		m_ctrlInstruction.SetBackgroundColor(FALSE, RGB(190, 190, 190));
		
		CString sEnu = COemGuiApi::GetLanguageAbbreviation();
		CString sMaintenance;
		sMaintenance.Format(_T("C:\\Security\\Maintenance%s.rtf"), sEnu);

		CString sFile(sMaintenance); //default english
		if(GetFileAttributes(sFile) == 0xffffffff)
		{
			sMaintenance.Format(_T("C:\\Security\\Maintenance%s.rtf"), _T("enu"));
			sFile = sMaintenance;
		}

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
	else
	{
		m_ctrlLoginError.ShowWindow(SW_SHOW);
	}
}
/////////////////////////////////////////////////////////////////////////
void CMaintenancePage::OnChangeSettings()
{
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CMaintenancePage::CancelChanges()
{	
	//reset installer data
	GetSettings();
}
/////////////////////////////////////////////////////////////////////////////
void CMaintenancePage::SaveChanges()
{
	//save new settings
	SetSettings();
}
/////////////////////////////////////////////////////////////////////////////
void CMaintenancePage::Initialize()
{
	//fill dlg with saved settings
	GetSettings();
}
/////////////////////////////////////////////////////////////////////////////
void CMaintenancePage::GetSettings()
{
	CWK_Profile&wkp = GetProfile();

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

	m_sReport.Empty();
	m_bResetHDDTemperatures = FALSE;
	UpdateDlgItems();
}
/////////////////////////////////////////////////////////////////////////////
void CMaintenancePage::SetSettings()
{
	UpdateData();
	CWK_Profile&wkp = GetProfile();

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
void CMaintenancePage::OnBnClickedIntervallOnOff()
{
	//switch intervall on/off
	m_bIntervallOnOff = !m_bIntervallOnOff;

	UpdateDlgItems();

	OnChangeSettings();
}
/////////////////////////////////////////////////////////////////////////////
void CMaintenancePage::OnBnClickedMaintButtonStartdate()
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
void CMaintenancePage::ShowLoginGrp(int iShow)
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
//creates a system report
void CMaintenancePage::OnBnClickedMaintButtonReport()
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

	sVersion = COemGuiApi::GetSoftwareVersion();
	sReport = sReport + _T("\r\n") + sVersionStr + sVersion + _T("\r\n");

	//get model, serial ,firmware and SMART from HDDs
	CString sHDD = GetHDDInformations();
	sReport  = sReport + _T("\r\n") + sHDD;

	m_sReport = sReport;
	UpdateData(FALSE);
	WK_TRACE(_T("%s"), sReport);
}
/////////////////////////////////////////////////////////////////////////////
CString CMaintenancePage::GetHDDInformations()
{
	CString sHDD, sDummy;
	CHDDTApi Hdd;
	DRIVE_INFO drive_info;
	int nTemperatur = 0, nMaxDrives;
	CWK_Profile wkp;
	nMaxDrives = wkp.GetInt(SECTION_LAUNCHER, MAX_CHECKED_DRIVES, 7);

	for (UCHAR i=0;i<nMaxDrives;i++)
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
CString CMaintenancePage::GetSMARTparameters(int nDrive)
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
			smart.Close();
		}

	}
	return sSmart;
}
/////////////////////////////////////////////////////////////////////////////
//save report as HTML file
void CMaintenancePage::OnBnClickedMaintButtonReportExport()
{
	//TODO TKR noch als HTML Datei auf Stick oder Diskette speichern
}

void CMaintenancePage::OnBnClickedMaintButtonSystest()
{
	//restart idip software, delete sequence.dbf
}
