// ChangeOEMDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ChangeOEM.h"
#include "ChangeOEMDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChangeOEMDlg dialog

CChangeOEMDlg::CChangeOEMDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChangeOEMDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChangeOEMDlg)
	m_sOEM1 = _T("");
	m_sOEM2 = _T("");
	m_sOEM3 = _T("");
	m_sCOMPUTERNAME = _T("");
	m_sRegOrga = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bIsNT = false;
	m_bIsWin95 = false;
	m_bIsWin98 = false;
	m_bCheckCompName = true;
	m_sCheckCompNameFailed = "Fehler beim Registry-Lesezugriff:\r\n";
}

void CChangeOEMDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChangeOEMDlg)
	DDX_Control(pDX, IDC_REG_ORGA, m_editRegOrga);
	DDX_Control(pDX, IDC_COMPUTERNAME, m_editCOMPUTERNAME);
	DDX_Control(pDX, IDOK, m_editOK);
	DDX_Control(pDX, IDC_WrongOS, m_staticWrongOS);
	DDX_Control(pDX, IDC_W98, m_staticW98);
	DDX_Control(pDX, IDC_WNT, m_staticWNT);
	DDX_Control(pDX, IDC_W95, m_staticW95);
	DDX_Control(pDX, IDC_OEM3, m_editOEM3);
	DDX_Control(pDX, IDC_OEM2, m_editOEM2);
	DDX_Control(pDX, IDC_OEM1, m_editOEM1);
	DDX_Text(pDX, IDC_OEM1, m_sOEM1);
	DDV_MaxChars(pDX, m_sOEM1, 5);
	DDX_Text(pDX, IDC_OEM2, m_sOEM2);
	DDV_MaxChars(pDX, m_sOEM2, 7);
	DDX_Text(pDX, IDC_OEM3, m_sOEM3);
	DDV_MaxChars(pDX, m_sOEM3, 5);
	DDX_Text(pDX, IDC_COMPUTERNAME, m_sCOMPUTERNAME);
	DDX_Text(pDX, IDC_REG_ORGA, m_sRegOrga);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CChangeOEMDlg, CDialog)
	//{{AFX_MSG_MAP(CChangeOEMDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_MAXTEXT(IDC_OEM1, OnMaxtextOem1)
	ON_EN_MAXTEXT(IDC_OEM2, OnMaxtextOem2)
	ON_EN_MAXTEXT(IDC_OEM3, OnMaxtextOem3)
	ON_EN_SETFOCUS(IDC_OEM2, OnSetfocusOem2)
	ON_EN_SETFOCUS(IDC_OEM3, OnSetfocusOem3)
	ON_EN_KILLFOCUS(IDC_OEM1, OnKillfocusOem1)
	ON_EN_KILLFOCUS(IDC_OEM2, OnKillfocusOem2)
	ON_EN_KILLFOCUS(IDC_OEM3, OnKillfocusOem3)
	ON_EN_MAXTEXT(IDC_COMPUTERNAME, OnMaxtextComputername)
	ON_EN_SETFOCUS(IDC_COMPUTERNAME, OnSetfocusComputername)
	ON_EN_SETFOCUS(IDC_REG_ORGA, OnSetfocusRegOrga)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChangeOEMDlg message handlers

BOOL CChangeOEMDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	

	// get OS version and OEM number
	COsVersionInfo version;			// make instance of class "version"
	if (version.IsNT())				// OS is NT ?
	{
		m_bIsNT = true;
		GetDlgItem(IDC_WNT)->ShowWindow(SW_SHOW);
		//GetDlgItem(IDC_COMPUTERNAME)->ShowWindow(SW_HIDE);
		//GetDlgItem(IDC_CompName)->ShowWindow(SW_HIDE);
		//read OEM version number if OS is NT
		CWK_Profile wkp(CWK_Profile::WKP_REGPATH, 
			HKEY_LOCAL_MACHINE, 
			"Microsoft\\Windows NT", 
			"");
		m_sOEM = wkp.GetString("CurrentVersion", "ProductId", "");
		//delete string into 3 OEM groups and OEM number
		CString OEMleft, OEMmiddle, OEMright, OEMNr;
											// example:
											// m_sOEM    = 51581000111111121240
		OEMleft = m_sOEM.Left(5);			// OEMleft   = 51581
		m_sOEM.Delete(0,5);					// m_sOEM    = 000111111121240
		OEMNr = m_sOEM.Left(3);				// OEMNr	 = 000
		m_sOEM.Delete(0,3);					// m_sOEM	 = 111111121240
		OEMmiddle = m_sOEM.Left(7);			// OEMmiddle = 1111111
		OEMright = m_sOEM.Right(5);			// OEMright  = 21240

		m_sOEM1 = OEMleft;
		m_sOEM2 = OEMmiddle;
		m_sOEM3 = OEMright;
		m_sOEMNr = OEMNr;

		CWK_Profile wkpRegOrga(CWK_Profile::WKP_REGPATH,
			HKEY_LOCAL_MACHINE, 
			"Microsoft\\Windows NT", 
			"");
		m_sRegOrga = wkpRegOrga.GetString("CurrentVersion", "RegisteredOrganization", "");
/////////////////////////////////////////////////////////////////////////////////
		// get value of key "ComputerName"

		HKEY testkey;
		DWORD dwType, dwCount;
		CString strValue;
		CString RegOrgaValue;
		LONG lResult;
			if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
					 "SYSTEM\\CurrentControlSet\\Control\\ComputerName\\ComputerName",
					 0,
					 KEY_QUERY_VALUE,
					 &testkey) != 0)
		{
			m_sCheckCompNameFailed += "Öffnen:\r\nHKEY_LOCAL_MACHINE\\SYSTEM\\"
									  "CurrentControlSet\\"
									  "Control\\ComputerName\\"
								      "ComputerName\\ComputerName\r\n";
			m_bCheckCompName = false;
		}


		lResult = RegQueryValueEx(testkey, 
						"ComputerName", 
						NULL, 
						&dwType, 
						NULL,
						&dwCount);
		if(lResult == ERROR_SUCCESS)
		{
			lResult = RegQueryValueEx(testkey, 
									  "ComputerName", 
									  NULL, 
									  &dwType,
									  (LPBYTE)strValue.GetBuffer(dwCount/sizeof(TCHAR)), 
									  &dwCount);
			strValue.ReleaseBuffer();
		}
		else
		{
			m_sCheckCompNameFailed += "Schreiben:\r\nHKEY_LOCAL_MACHINE\\SYSTEM\\"
									  "CurrentControlSet\\"
									  "Control\\ComputerName\\"
								  	  "ComputerName\\ComputerName\r\n";
			m_bCheckCompName = false;
		}
		RegCloseKey(testkey);
		m_sCOMPUTERNAME = strValue;
		TRACE("inhalt: %s\n", strValue);

	}				

	else if(version.IsWin95())				// is OS Windows95 ?
	{

		m_bIsWin95 = true;
		GetDlgItem(IDC_W95)->ShowWindow(SW_SHOW);

		//read OEM version number if OS is Windows95
		CWK_Profile wkp(CWK_Profile::WKP_REGPATH, 
			HKEY_LOCAL_MACHINE, 
			"Microsoft\\Windows", 
			"");
		m_sOEM = wkp.GetString("CurrentVersion", "ProductId", "");

		
		// get ComputerName from Registry
		HKEY testkey;
		DWORD dwType, dwCount;
		CString strValue;
		CString RegOrgaValue;
		LONG lResult;

		// open registry ComputerName
		if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
					 "SYSTEM\\CurrentControlSet\\Control\\ComputerName\\ComputerName",
					 0,
					 KEY_QUERY_VALUE,
					 &testkey) != 0)
		{
			m_sCheckCompNameFailed += "Öffnen:\r\nHKEY_LOCAL_MACHINE\\SYSTEM\\"
									  "CurrentControlSet\\"
									  "Control\\ComputerName\\"
								      "ComputerName\\ComputerName\r\n";
			m_bCheckCompName = false;
		}

		// get value of key "ComputerName"
		lResult = RegQueryValueEx(testkey, 
						"ComputerName", 
						NULL, 
						&dwType, 
						NULL,
						&dwCount);
		if(lResult == ERROR_SUCCESS)
		{
			lResult = RegQueryValueEx(testkey, 
									  "ComputerName", 
									  NULL, 
									  &dwType,
									  (LPBYTE)strValue.GetBuffer(dwCount/sizeof(TCHAR)), 
									  &dwCount);
			strValue.ReleaseBuffer();
		}
		else
		{
			m_sCheckCompNameFailed += "Schreiben:\r\nHKEY_LOCAL_MACHINE\\SYSTEM\\"
									  "CurrentControlSet\\"
									  "Control\\ComputerName\\"
								  	  "ComputerName\\ComputerName\r\n";
			m_bCheckCompName = false;
		}
		RegCloseKey(testkey);
		m_sCOMPUTERNAME = strValue;
		TRACE("inhalt: %s\n", strValue);

		// open registry Registered Organization
		HKEY RegOrgKey;
		RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
					 "SOFTWARE\\Microsoft\\Windows\\CurrentVersion",
					 0,
					 KEY_QUERY_VALUE,
					 &RegOrgKey);
				
		// get value of key "Registered Organisation"
		lResult = RegQueryValueEx(RegOrgKey, 
						"RegisteredOrganization", 
						NULL, 
						&dwType, 
						NULL,
						&dwCount);
		if(lResult == ERROR_SUCCESS)
		{
			lResult = RegQueryValueEx(RegOrgKey, 
									  "RegisteredOrganization", 
									  NULL, 
									  &dwType,
									  (LPBYTE)RegOrgaValue.GetBuffer(dwCount/sizeof(TCHAR)), 
									  &dwCount);
			RegOrgaValue.ReleaseBuffer();
		}
		RegCloseKey(RegOrgKey);
		m_sRegOrga = RegOrgaValue;

	}

	else if(version.IsWin98())				// OS is Windows 98
	{
		m_bIsWin98 = true;
		GetDlgItem(IDC_W98)->ShowWindow(SW_SHOW);

		//read OEM version number if OS is Windows98
		CWK_Profile wkp(CWK_Profile::WKP_REGPATH, 
			HKEY_LOCAL_MACHINE, 
			"Microsoft\\Windows", 
			"");
		m_sOEM = wkp.GetString("CurrentVersion", "ProductId", "");

		
		// get ComputerName from Registry
		HKEY testkey;
		DWORD dwType, dwCount;
		CString strValue;
		LONG lResult;

		// open registry ComputerName
		if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
					 "SYSTEM\\CurrentControlSet\\Control\\ComputerName\\ComputerName",
					 0,
					 KEY_QUERY_VALUE,
					 &testkey) != 0)
		{
			m_sCheckCompNameFailed += "Öffnen:\r\nHKEY_LOCAL_MACHINE\\SYSTEM\\"
									  "CurrentControlSet\\"
									  "Control\\ComputerName\\"
								  	  "ComputerName\\ComputerName\r\n";
			m_bCheckCompName = false;
		}

		// get value of key "ComputerName"
		lResult = RegQueryValueEx(testkey, 
						"ComputerName", 
						NULL, 
						&dwType, 
						NULL,
						&dwCount);
		if(lResult == ERROR_SUCCESS)
		{
			lResult = RegQueryValueEx(testkey, "ComputerName", NULL, &dwType,
				(LPBYTE)strValue.GetBuffer(dwCount/sizeof(TCHAR)), &dwCount);
			strValue.ReleaseBuffer();
		}
		else
		{
			m_sCheckCompNameFailed += "Schreiben:\r\nHKEY_LOCAL_MACHINE\\SYSTEM\\"
									  "CurrentControlSet\\"
									  "Control\\ComputerName\\"
								  	  "ComputerName\\ComputerName\r\n";
			m_bCheckCompName = false;
		}
		RegCloseKey(testkey);
		m_sCOMPUTERNAME = strValue;

	}
	else
	{	
		GetDlgItem(IDC_WrongOS)->ShowWindow(SW_SHOW);
		CDialog::OnOK();
		// can`t change OEM number, incompatible OS
		MessageBox("Änderung der OEM-Nummer nur bei Windows95/98/NT möglich.",
				   "Falsche Betriebssystem !",
				   MB_OK|MB_ICONSTOP);
		return false;
	}

	//Error Message if editing registry failed
	if (!m_bCheckCompName)
	{
		MessageBox(m_sCheckCompNameFailed,
				   "Registry Zugriffsfehler !",
				   MB_OK|MB_ICONSTOP);
		return false;
	}


	if(m_bIsWin95 | m_bIsWin98)
	{
	//delete string into 3 OEM groups
	CString OEMleft, OEMmiddle, OEMright;
										//example:
										// m_sOEM    = 12345-OEM-1234567-67890
	OEMleft = m_sOEM.Left(5);			// OEMleft   = 12345
	m_sOEM.Delete(0,10);				// m_sOEM    = 1234567-67890
	OEMmiddle = m_sOEM.Left(7);			// OEMmiddle = 1234567
	OEMright = m_sOEM.Right(5);			// OEMright  = 67890

	m_sOEM1 = OEMleft;
	m_sOEM2 = OEMmiddle;
	m_sOEM3 = OEMright;
	}

	UpdateData(FALSE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}
///////////////////////////////////////////////////////////////////////////////////////
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CChangeOEMDlg::OnPaint() 
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
///////////////////////////////////////////////////////////////////////////////////////
// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CChangeOEMDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}
///////////////////////////////////////////////////////////////////////////////////////
void CChangeOEMDlg::OnMaxtextOem1() 
{
	//set focus to second OEM field
	m_editOEM2.SetFocus();
}

///////////////////////////////////////////////////////////////////////////////////////
void CChangeOEMDlg::OnMaxtextOem2() 
{
	//set focus to last OEM field
	m_editOEM3.SetFocus();
}
///////////////////////////////////////////////////////////////////////////////////////
void CChangeOEMDlg::OnMaxtextOem3() 
{
	//set focus to field COMPUTERNAME
	m_editCOMPUTERNAME.SetFocus();
}
///////////////////////////////////////////////////////////////////////////////////////
void CChangeOEMDlg::OnSetfocusOem2() 
{	
	m_editOEM2.SetSel(0, -1, FALSE);
}
///////////////////////////////////////////////////////////////////////////////////////
void CChangeOEMDlg::OnSetfocusOem3() 
{
	m_editOEM3.SetSel(0, -1, FALSE);
}
///////////////////////////////////////////////////////////////////////////////////////
void CChangeOEMDlg::OnMaxtextComputername() 
{
	//set focus to button "übernehmen"
	m_editRegOrga.SetFocus();

}
///////////////////////////////////////////////////////////////////////////////////////
void CChangeOEMDlg::OnSetfocusComputername() 
{
	m_editCOMPUTERNAME.SetSel(0, -1, FALSE);
	
}
///////////////////////////////////////////////////////////////////////////////////////
void CChangeOEMDlg::OnKillfocusOem1() 
{
	// check new value, if lenght of new value < 5 show old value again
	CString sOEM1old = m_sOEM1;			// save old value of m_sOEM1 
	UpdateData(true);
	if(m_sOEM1.GetLength() < 5)		
	{
		m_sOEM1 = sOEM1old;
		UpdateData(FALSE);
		m_editOEM1.SetFocus();
		UpdateData(true);
		m_editOEM1.SetSel(0, -1, FALSE);
	}
}
///////////////////////////////////////////////////////////////////////////////////////
void CChangeOEMDlg::OnKillfocusOem2() 
{
	// check new value, if lenght of new value < 7 show old value again
	CString sOEM2old = m_sOEM2;			// save old value of m_sOEM2 
	UpdateData(true);
	if(m_sOEM2.GetLength() < 7)			 
	{
		m_sOEM2 = sOEM2old;
		UpdateData(FALSE);
		m_editOEM2.SetFocus();
		UpdateData(true);
		m_editOEM2.SetSel(0, -1, FALSE);
	}
}
///////////////////////////////////////////////////////////////////////////////////////
void CChangeOEMDlg::OnKillfocusOem3() 
{
	// check new value, if lenght of new value < 5 show old value again
	CString sOEM3old = m_sOEM3;			// save old value of m_sOEM3 
	UpdateData(true);
	if(m_sOEM3.GetLength() < 5)			 
	{
		m_sOEM3 = sOEM3old;
		UpdateData(FALSE);
		m_editOEM3.SetFocus();
		UpdateData(true);
		m_editOEM3.SetSel(0, -1, FALSE);
	}
}	

///////////////////////////////////////////////////////////////////////////////////////
void CChangeOEMDlg::OnSetfocusRegOrga() 
{
	m_editRegOrga.SetSel(0, -1, FALSE);
	
}

///////////////////////////////////////////////////////////////////////////////////////
void CChangeOEMDlg::OnOK() 
{
	UpdateData(true);
	// TODO: Add your control notification handler code here
	CString sNewOEM, sNewOEMzeile, sNewCompName, sNewRegOrga;
	m_sCheckCompNameFailed = "Fehler beim Registry-Zugriff:\r\n";
	m_bCheckCompName = true;

	if(m_bIsNT)										// save new OEM if OS = WinNT
	{		
		// get new OEM number
		sNewOEM = m_sOEM1+m_sOEMNr+m_sOEM2+m_sOEM3;
		sNewOEMzeile = m_sOEM1+" - OEM - "+m_sOEM2+" - "+m_sOEM3;
		int ireturn = MessageBox("Ist die eingetragene OEM-Nummer korrekt?\n\n"
					              +sNewOEMzeile,
							      NULL,
							   	  MB_OKCANCEL | MB_ICONQUESTION);	
		if (ireturn == IDOK)
		{
			// save new OEM number
			CWK_Profile wknewOEM(CWK_Profile::WKP_REGPATH, 
						         HKEY_LOCAL_MACHINE, 
								 "Microsoft\\Windows NT", 
								 "");
			// saving possible ?
			if(wknewOEM.WriteString("CurrentVersion", "ProductId", sNewOEM))
			{
				CDialog::OnOK();
				MessageBox("OEM-Nummer wurde erfolgreich in die Registry eingetragen.",
						   NULL,
						   MB_OK|MB_ICONINFORMATION);
			}
			else
			{
				MessageBox("OEM-Nummer konnte nicht in die Registry eingetragen werden.",
						   NULL,
						   MB_OK|MB_ICONSTOP);
			}
		}
	}

	else if(m_bIsWin95 | m_bIsWin98)	// save new OEM and Computername
										//if OS = Win95 or Win98 
	{
		// get new OEM number and new Computername
		sNewOEM = m_sOEM1+"-OEM-"+m_sOEM2+"-"+m_sOEM3;
		sNewOEMzeile = m_sOEM1+" - OEM - "+m_sOEM2+" - "+m_sOEM3;
		sNewCompName = m_sCOMPUTERNAME;
		sNewRegOrga = m_sRegOrga;
		
		// save new OEM number
		int ireturn = MessageBox("Sind OEM-Nummer, Computername (Username) und Organisation"
								" korrekt?\n\n\nOEM:              "+sNewOEMzeile+
								"\n\nName:             "+sNewCompName+
								"\n\nOrganisation:   "+sNewRegOrga,
								NULL,
								MB_OKCANCEL | MB_ICONQUESTION);
		
		if(ireturn == IDOK)
		{
			// open key ProductId	
			CWK_Profile wknewOEM(CWK_Profile::WKP_REGPATH, 
				HKEY_LOCAL_MACHINE, 
				"Microsoft\\Windows", 
				"");
			
			// save key ProductId	
			if(!wknewOEM.WriteString("CurrentVersion", "ProductId", sNewOEM))
			{
				m_sCheckCompNameFailed += "Schreiben:\r\nHKEY_LOCAL_MACHINE\\SOFTWARE\\"
					"MicroSoft\\Windows\\CurrentVersion\\ProductId\r\n";
				m_bCheckCompName = false;
			}
			
			// open key ComputerName\Computername			
			HKEY namekey;
			CString strValue;
			LONG lResult;

			if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
				"SYSTEM\\CurrentControlSet\\Control\\ComputerName\\ComputerName",
				0,
				KEY_SET_VALUE,
				&namekey) != 0)
			{
				m_sCheckCompNameFailed += "Öffnen:\r\nHKEY_LOCAL_MACHINE\\SYSTEM\\"
					"CurrentControlSet\\Control\\ComputerName\\ComputerName\\"
					"ComputerName\r\n";
				m_bCheckCompName = false;
			}
			
			// save key Computername\Computername
			LPCSTR lpsComputerName_ComputerName = sNewCompName;
			lResult = RegSetValueEx(namekey, 
				"ComputerName",
				NULL, 
				REG_SZ,
				(LPBYTE)lpsComputerName_ComputerName, 
				(lstrlen(sNewCompName)+1)*sizeof(TCHAR));
			
			if (lResult != ERROR_SUCCESS)
			{
				m_sCheckCompNameFailed += "Schreiben:\r\nHKEY_LOCAL_MACHINE\\SYSTEM\\"
					"CurrentControlSet\\"
					"Control\\ComputerName\\ComputerName\\"
					"ComputerName\r\n";
				m_bCheckCompName = false;
			}
			RegCloseKey(namekey);

			// open key RegisteredOrganization			
			HKEY RegOrgaKeySet;
			CString RegOrgaSetValue;
			LONG lResultRegOrga;

			RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
						"SOFTWARE\\Microsoft\\Windows\\CurrentVersion",
						0,
						KEY_SET_VALUE,
						&RegOrgaKeySet);
			
			// save key RegisteredOrganization
			LPCSTR lpsRegOrga = sNewRegOrga;
			lResultRegOrga = RegSetValueEx(RegOrgaKeySet, 
											"RegisteredOrganization",
											NULL, 
											REG_SZ,
											(LPBYTE)lpsRegOrga, 
											(lstrlen(sNewRegOrga)+1)*sizeof(TCHAR));

			RegCloseKey(RegOrgaKeySet);



			
			//open key HKEY_LOCAL_MACHINE\Enum\Root\*PNP0C01\0000
			if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
				"Enum\\Root\\*PNP0C01\\0000",
				0,
				KEY_SET_VALUE,
				&namekey) != 0)
			{
				m_sCheckCompNameFailed += "Öffnen:\r\nHKEY_LOCAL_MACHINE\\Enum\\"
									 	  "Root\\*PNP0C01\\0000\r\n";
				m_bCheckCompName = false;
			}
			
			// save key HKEY_LOCAL_MACHINE\Enum\Root\*PNP0C01\0000\ComputerName
			LPCSTR lpsRoot_ComputerName = sNewCompName;
			
			lResult = RegSetValueEx(namekey, 
				"ComputerName",
				NULL, 
				REG_SZ,
				(LPBYTE)lpsRoot_ComputerName, 
				(lstrlen(sNewCompName)+1)*sizeof(TCHAR));

			if (lResult != ERROR_SUCCESS)
			{
				m_sCheckCompNameFailed += "Schreiben:\r\nHKEY_LOCAL_MACHINE\\Enum\\"
									 	  "Root\\*PNP0C01\\0000\r\n";
				m_bCheckCompName = false;
			}

			RegCloseKey(namekey);


			// open key VNETSUP\Computername

			if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
				"SYSTEM\\CurrentControlSet\\Services\\VxD\\VNETSUP",
				0,
				KEY_SET_VALUE,
				&namekey) != 0)
			{
				m_sCheckCompNameFailed += "Öffnen:\r\nHKEY_LOCAL_MACHINE\\SYSTEM\\"
									 	  "CurrentControlSet\\Services\\VxD\\VNETSUP\r\n";
				m_bCheckCompName = false;
			}
			
			// save key VNETSUP\Computername
			LPCSTR lpsVNETSUP_ComputerName = sNewCompName;
			
			lResult = RegSetValueEx(namekey, 
				"ComputerName",
				NULL, 
				REG_SZ,
				(LPBYTE)lpsVNETSUP_ComputerName, 
				(lstrlen(sNewCompName)+1)*sizeof(TCHAR));

			if (lResult != ERROR_SUCCESS)
			{
				m_sCheckCompNameFailed += "Schreiben:\r\nHKEY_LOCAL_MACHINE\\SYSTEM\\"
									 	  "CurrentControlSet\\Services\\VxD\\VNETSUP\r\n";
				m_bCheckCompName = false;
			}

			RegCloseKey(namekey);


			// open key RegisteredOwner
			LPCSTR lpszKey = "RegisteredOwner";

			if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
				"SOFTWARE\\MicroSoft\\Windows\\CurrentVersion",
				0,
				KEY_SET_VALUE,
				&namekey) != 0)
			{
				m_sCheckCompNameFailed += "Öffnen:\r\nHKEY_LOCAL_MACHINE\\SOFTWARE\\"
					"MicroSoft\\Windows\\CurrentVersion\\"
					"RegisteredOwner\r\n";
					m_bCheckCompName = false;
			}
			
			// save key RegisteredOwner
			LPCSTR lpsRegeisteredOwner = sNewCompName;
			
			lResult = RegSetValueEx(namekey, 
				"RegisteredOwner",
				NULL, 
				REG_SZ,
				(LPBYTE)lpsRegeisteredOwner, 
				(lstrlen(sNewCompName)+1)*sizeof(TCHAR));

			if (lResult != ERROR_SUCCESS)
			{
				m_sCheckCompNameFailed += "Schreiben:\r\nHKEY_LOCAL_MACHINE\\SOFTWARE\\"
					"MicroSoft\\Windows\\CurrentVersion\\"
					"RegisteredOwner\r\n";
					m_bCheckCompName = false;
			}

			RegCloseKey(namekey);

			// open key OwnId
			if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
				"SOFTWARE\\DVRT\\Hosts",
				0,
				KEY_SET_VALUE,
				&namekey) != 0)
			{
				m_sCheckCompNameFailed += "Öffnen:\r\nHKEY_LOCAL_MACHINE\\SOFTWARE\\"
										  "DVRT\\Hosts\\OwnId\r\n";
				m_bCheckCompName = false;
			}
			
			// save key OwnId
			LPCSTR lpsOwnId = sNewCompName;
			
			lResult = RegSetValueEx(namekey, 
				"OwnId", 
				NULL, 
				REG_SZ,
				(LPBYTE)lpsOwnId, 
				(lstrlen(sNewCompName)+1)*sizeof(TCHAR));

			if (lResult != ERROR_SUCCESS)
			{
				m_sCheckCompNameFailed += "Schreiben:\r\nHKEY_LOCAL_MACHINE\\SOFTWARE\\"
										  "DVRT\\Hosts\\OwnId\r\n";
				m_bCheckCompName = false;
			}

			RegCloseKey(namekey);
			if(!m_bCheckCompName)
			{
				MessageBox(m_sCheckCompNameFailed,"Fehler bei Registry-Zugriff",
					   MB_OK|MB_ICONSTOP);
			}
			else
			{
				CDialog::OnOK();
				MessageBox("OEM-Nummer, Computername (Username) und Organisation wurden erfolgreich "
							"in die Registry eingetragen.", "Registryeinträge geändert",
							MB_OK|MB_ICONINFORMATION);
			}

		}




	}
	else									// don`t save if OS != Win95/98/NT
	{
		MessageBox("Computername und OEM-Nummer können nicht geändert werden.",
			"Fehler beim Speichern !",
			MB_OK|MB_ICONSTOP);
	}

}


