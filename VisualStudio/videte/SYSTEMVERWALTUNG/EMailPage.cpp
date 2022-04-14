// EMailPage.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "EMailPage.h"
#include "Ras.h"
#include <malloc.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEMailPage dialog
/////////////////////////////////////////////////////////////////////////////
static TCHAR BASED_CODE szSection[]   = _T("EmailUnit");

/////////////////////////////////////////////////////////////////////////////
CEMailPage::CEMailPage(CSVView* pParent)
: CSVPage(CEMailPage::IDD)
{
	m_fpRasEnumEntries = NULL;
	m_pParent = pParent;
	//{{AFX_DATA_INIT(CEMailPage)
	m_bEMail = FALSE;
	m_iDFUEon = 0;
	m_iPort = 25;
	m_sSMTPServer = _T("");
	m_sEmail = _T("");
	m_sDFUEPassword = _T("");
	m_sDFUEUser = _T("");
	m_bPOP = FALSE;
	m_sPOPPassword = _T("");
	m_sPOPUser = _T("");
	m_uPOPPort = 110;
	m_sPOPServer = _T("");
	m_iDFUE = -1;
	//}}AFX_DATA_INIT

	Create(IDD,m_pParent);
}
/////////////////////////////////////////////////////////////////////////////
void CEMailPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEMailPage)
	DDX_Control(pDX, IDC_CHECK_POP3, m_checkPOP);
	DDX_Control(pDX, IDC_EDIT_POPSERVER, m_editPOPServer);
	DDX_Control(pDX, IDC_EDIT_POPPORT, m_editPOPPort);
	DDX_Control(pDX, IDC_EDIT_POP_PASSWORD, m_editPOPPasswd);
	DDX_Control(pDX, IDC_EDIT_POP_USERNAME, m_editPOPUsername);
	DDX_Control(pDX, IDC_EDIT_DFUE_USER, m_editUser);
	DDX_Control(pDX, IDC_EDIT_DFUE_PASSWORD, m_editPassword);
	DDX_Control(pDX, IDC_EDIT_EMAIL, m_editEmail);
	DDX_Control(pDX, IDC_RADIO_LAN, m_radioLAN);
	DDX_Control(pDX, IDC_RADIO_DFUE, m_radioDFUE);
	DDX_Control(pDX, IDC_EDIT_SMTP, m_editSMTP);
	DDX_Control(pDX, IDC_COMBO_DFUE, m_comboDFUE);
	DDX_Control(pDX, IDC_EDIT_PORT, m_editPort);
	DDX_Check(pDX, IDC_CHECK_EMAIL, m_bEMail);
	DDX_Radio(pDX, IDC_RADIO_LAN, m_iDFUEon);
	DDX_Text(pDX, IDC_EDIT_PORT, m_iPort);
	DDX_Text(pDX, IDC_EDIT_SMTP, m_sSMTPServer);
	DDX_Text(pDX, IDC_EDIT_EMAIL, m_sEmail);
	DDX_Text(pDX, IDC_EDIT_DFUE_PASSWORD, m_sDFUEPassword);
	DDX_Text(pDX, IDC_EDIT_DFUE_USER, m_sDFUEUser);
	DDX_Check(pDX, IDC_CHECK_POP3, m_bPOP);
	DDX_Text(pDX, IDC_EDIT_POP_PASSWORD, m_sPOPPassword);
	DDX_Text(pDX, IDC_EDIT_POP_USERNAME, m_sPOPUser);
	DDX_Text(pDX, IDC_EDIT_POPPORT, m_uPOPPort);
	DDV_MinMaxUInt(pDX, m_uPOPPort, 0, 65535);
	DDX_Text(pDX, IDC_EDIT_POPSERVER, m_sPOPServer);
	DDX_CBIndex(pDX, IDC_COMBO_DFUE, m_iDFUE);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CEMailPage, CSVPage)
//{{AFX_MSG_MAP(CEMailPage)
	ON_BN_CLICKED(IDC_RADIO_LAN, OnRadioLan)
	ON_BN_CLICKED(IDC_RADIO_DFUE, OnRadioDfue)
	ON_BN_CLICKED(IDC_CHECK_EMAIL, OnCheckEmail)
	ON_EN_CHANGE(IDC_EDIT_SMTP, OnChangeEditSmtp)
	ON_EN_CHANGE(IDC_EDIT_PORT, OnChangeEditPort)
	ON_CBN_SELCHANGE(IDC_COMBO_DFUE, OnSelchangeComboDfue)
	ON_EN_CHANGE(IDC_EDIT_EMAIL, OnChangeEditEmail)
	ON_EN_CHANGE(IDC_EDIT_DFUE_PASSWORD, OnChangeEditPassword)
	ON_EN_CHANGE(IDC_EDIT_DFUE_USER, OnChangeEditUser)
	ON_BN_CLICKED(IDC_CHECK_POP3, OnCheckPop)
	ON_EN_CHANGE(IDC_EDIT_POPSERVER, OnChangeEditPopServer)
	ON_EN_CHANGE(IDC_EDIT_POPPORT, OnChangeEditPopPort)
	ON_EN_CHANGE(IDC_EDIT_POP_USERNAME, OnChangeEditPopUser)
	ON_EN_CHANGE(IDC_EDIT_POP_PASSWORD, OnChangeEditPopPassword)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
BOOL CEMailPage::IsDataValid()
{
	BOOL bReturn = TRUE;
	// check for correct DFUE
	if (m_iDFUEon && m_sDFUE.IsEmpty())
	{
		AfxMessageBox(IDP_EMAIL_NO_DFUE, MB_ICONSTOP|MB_OK);
		m_comboDFUE.SetFocus();
		bReturn = FALSE;
	}
	return bReturn;
}
////////////////////////////////////////////////////////////////////////////
void CEMailPage::SaveChanges()
{
	UpdateData();
	WK_TRACE_USER(_T("EMail_Modul-Einstellungen wurden geändert\n"));

	CString sTemp;
	GetProfile().WriteString(theApp.GetModuleSection(),WK_APPNAME_EMAILUNIT,m_bEMail ? _T("EmailUnit.exe") : _T(""));
	GetProfile().WriteInt(szSection,_T("CONTYPEDFUE"),m_iDFUEon);
	GetProfile().WriteString(szSection,_T("DFUE"),m_sDFUE);
	GetProfile().WriteString(szSection,_T("USER"),m_sDFUEUser);
	sTemp = m_sDFUEPassword;
	GetProfile().Encode(sTemp);
#ifdef _UNICODE
	GetProfile().WriteString(szSection,_T("PASSWORD"),sTemp,TRUE);
#else
	GetProfile().WriteString(szSection,_T("PASSWORD"),sTemp);
#endif

	GetProfile().WriteString(szSection,_T("SMTP"),m_sSMTPServer);
	GetProfile().WriteInt(szSection,_T("PORT"),m_iPort);
	GetProfile().WriteString(szSection,_T("EMAIL"),m_sEmail);

	GetProfile().WriteInt(szSection,_T("POPAuth"),m_bPOP);
	GetProfile().WriteString(szSection,_T("POPSERVER"),m_sPOPServer);
 	GetProfile().WriteInt(szSection,_T("POPPORT"),m_uPOPPort);
	GetProfile().WriteString(szSection,_T("POPUSER"),m_sPOPUser);
	sTemp = m_sPOPPassword;
	GetProfile().Encode(sTemp);
#ifdef _UNICODE
	GetProfile().WriteString(szSection,_T("POPPASSWORD"),sTemp,TRUE);
#else
	GetProfile().WriteString(szSection,_T("POPPASSWORD"),sTemp);
#endif
}
/////////////////////////////////////////////////////////////////////////////
void CEMailPage::CancelChanges()
{
	CString sTemp	= GetProfile().GetString(theApp.GetModuleSection(),WK_APPNAME_EMAILUNIT,_T(""));
	m_bEMail		= sTemp.CompareNoCase(_T("EmailUnit.exe"))==0; 
	m_iDFUEon		= GetProfile().GetInt(szSection,_T("CONTYPEDFUE"),m_iDFUEon=0);
	m_sDFUE			= GetProfile().GetString(szSection,_T("DFUE"),m_sDFUE);
	m_sDFUEUser		= GetProfile().GetString(szSection,_T("USER"),m_sDFUEUser);
#ifdef _UNICODE
	m_sDFUEPassword	= GetProfile().GetString(szSection, _T("PASSWORD"), m_sDFUEPassword, TRUE);
#else
	m_sDFUEPassword	= GetProfile().GetString(szSection, _T("PASSWORD"), m_sDFUEPassword);
#endif	
	GetProfile().Decode(m_sDFUEPassword);

	m_sSMTPServer	= GetProfile().GetString(szSection,_T("SMTP"),m_sSMTPServer);
	m_iPort			= GetProfile().GetInt(szSection,_T("PORT"),m_iPort=25);
	m_sEmail		= GetProfile().GetString(szSection,_T("EMAIL"),m_sEmail);

	m_bPOP			= GetProfile().GetInt(szSection,_T("POPAuth"),m_bPOP=0);
	m_sPOPServer	= GetProfile().GetString(szSection,_T("POPSERVER"),m_sPOPServer);
 	m_uPOPPort		= GetProfile().GetInt(szSection,_T("POPPORT"),m_uPOPPort=110);
	m_sPOPUser		= GetProfile().GetString(szSection,_T("POPUSER"),m_sPOPUser);
#ifdef _UNICODE
	m_sPOPPassword	= GetProfile().GetString(szSection, _T("POPPASSWORD"), m_sPOPPassword, TRUE);
#else
	m_sPOPPassword	= GetProfile().GetString(szSection, _T("POPPASSWORD"), m_sPOPPassword);
#endif	
	GetProfile().Decode(m_sPOPPassword);

	// DUN still available, maybe deleted or renamed
	if (m_sDFUE != _T(""))
	{
		m_iDFUE = m_comboDFUE.FindStringExact(0, m_sDFUE);
		if (m_iDFUE == LB_ERR)
		{
			AfxMessageBox(IDP_EMAIL_DFUE_NOT_FOUND, MB_ICONSTOP|MB_OK);
			m_sDFUE = _T("");
			m_iDFUEon = 0;
		}
		else
		{
			m_comboDFUE.SelectString(m_iDFUE, m_sDFUE);
		}
	}
	if(m_comboDFUE.GetCount() == 0)
	{
		m_iDFUEon=0;	//kein DFÜ vorhanden
	}

	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CEMailPage::CheckRasAPI()
{
	BOOL	bRasAPI = FALSE;
	HINSTANCE hModule;
	hModule = LoadLibraryEx(_T("RasApi32.dll"),NULL,0);

	if(!hModule)	//RasApi32.dll nicht gefunden oder vorhanden
	{
		WK_TRACE(_T("cannot load RasApi32.dll\n"));
		bRasAPI = FALSE;
	}
	else
	{
#ifdef _UNICODE
		m_fpRasEnumEntries = (MyRasEnumEntries)GetProcAddress(hModule, "RasEnumEntriesW");
#else
	    m_fpRasEnumEntries = (MyRasEnumEntries)GetProcAddress(hModule, "RasEnumEntriesA");
#endif
 
		if(!m_fpRasEnumEntries)	//Funktion nicht gefunden
		{
			WK_TRACE(_T("cannot load RasApi32.dll::RasEnumEntries\n"));
			bRasAPI = FALSE;
		}
		bRasAPI = TRUE;
	}
	return bRasAPI;
}
/////////////////////////////////////////////////////////////////////////////
void CEMailPage::ShowHide()
{
	BOOL bEnable = m_bEMail;
	m_radioLAN.EnableWindow(m_bEMail);

	bEnable = m_bEMail && m_comboDFUE.GetCount();
	m_radioDFUE.EnableWindow(bEnable);

	bEnable = m_bEMail && m_comboDFUE.GetCount() && m_iDFUEon;
	m_comboDFUE.EnableWindow(bEnable);
	m_editUser.EnableWindow(bEnable);
	m_editPassword.EnableWindow(bEnable);

	m_editSMTP.EnableWindow(m_bEMail);
	m_editPort.EnableWindow(m_bEMail);
	m_editEmail.EnableWindow(m_bEMail);

	m_checkPOP.EnableWindow(m_bEMail);

	bEnable = m_bEMail && m_bPOP;
	m_editPOPPasswd.EnableWindow(bEnable);
	m_editPOPUsername.EnableWindow(bEnable);
	m_editPOPServer.EnableWindow(bEnable);
	m_editPOPPort.EnableWindow(bEnable);
}
/////////////////////////////////////////////////////////////////////////////
void CEMailPage::OnChange()
{
	UpdateData();
	SetModified(TRUE,FALSE);
	ShowHide();
	m_pParent->AddInitApp(WK_APPNAME_EMAILUNIT);
}
/////////////////////////////////////////////////////////////////////////////
void CEMailPage::FillCombo()
{
	m_comboDFUE.ResetContent();

	if(CheckRasAPI())
	{
		LPRASENTRYNAME pRasentryname;
		DWORD dwBufferSize = 1, dwEntries;
		RASENTRYNAME Rasentryname[10];

		pRasentryname= Rasentryname;
		pRasentryname->dwSize = sizeof(RASENTRYNAME);

		if(m_fpRasEnumEntries(
							  NULL,			    // reserved, must be NULL
							  NULL,	            // pointer to full path and filename of 
												// phone-book file
							  pRasentryname,    // buffer to receive phone-book entries
							  &dwBufferSize,    // size in bytes of buffer
							  &dwEntries        // number of entries written to buffer
							  ))
							  
		{
			
			if(!m_fpRasEnumEntries(
									NULL,         // reserved, must be NULL
									NULL,         // pointer to full path and filename of 
												  // phone-book file
									pRasentryname,// buffer to receive phone-book entries
									&dwBufferSize,// size in bytes of buffer
									&dwEntries    // number of entries written to buffer
									))
			{
				DWORD counter;
				for(counter = 0; counter < dwEntries; counter++)
				{
					TRACE(_T("%s"),(CString)Rasentryname[counter].szEntryName);
					m_comboDFUE.AddString((CString)Rasentryname[counter].szEntryName);

				}
			}			
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CEMailPage::OnInitDialog() 
{
	CSVPage::OnInitDialog();

	FillCombo();

	CancelChanges();
	ShowHide();

	return TRUE;	// return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CEMailPage::OnRadioLan() 
{
	OnChange();
}
/////////////////////////////////////////////////////////////////////////////
void CEMailPage::OnRadioDfue() 
{
	OnChange();
}
/////////////////////////////////////////////////////////////////////////////
void CEMailPage::OnCheckEmail() 
{
	OnChange();
}
/////////////////////////////////////////////////////////////////////////////
void CEMailPage::OnChangeEditSmtp() 
{
	OnChange();
}
/////////////////////////////////////////////////////////////////////////////
void CEMailPage::OnChangeEditPort() 
{
	OnChange();	
}
/////////////////////////////////////////////////////////////////////////////
void CEMailPage::OnSelchangeComboDfue() 
{
	OnChange();
	m_comboDFUE.GetLBText(m_iDFUE, m_sDFUE);
}
/////////////////////////////////////////////////////////////////////////////
void CEMailPage::OnChangeEditEmail() 
{
	OnChange();	
}
/////////////////////////////////////////////////////////////////////////////
void CEMailPage::OnChangeEditPassword() 
{
	OnChange();
}
/////////////////////////////////////////////////////////////////////////////
void CEMailPage::OnChangeEditUser() 
{
	OnChange();	
}
/////////////////////////////////////////////////////////////////////////////
void CEMailPage::OnCheckPop() 
{
	OnChange();
}
/////////////////////////////////////////////////////////////////////////////
void CEMailPage::OnChangeEditPopServer() 
{
	OnChange();	
}
/////////////////////////////////////////////////////////////////////////////
void CEMailPage::OnChangeEditPopPort() 
{
	OnChange();
}
/////////////////////////////////////////////////////////////////////////////
void CEMailPage::OnChangeEditPopUser() 
{
	OnChange();
}
/////////////////////////////////////////////////////////////////////////////
void CEMailPage::OnChangeEditPopPassword() 
{
	OnChange();	
}
