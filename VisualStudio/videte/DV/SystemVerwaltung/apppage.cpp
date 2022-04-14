// AppPage.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "AppPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static TCHAR BASED_CODE szSection[] = _T("AutoLogout");
/////////////////////////////////////////////////////////////////////////////
// CAppPage dialog


CAppPage::CAppPage(CSVView*  pParent) : CSVPage(CAppPage::IDD)
{
	m_pParent = pParent;
	//{{AFX_DATA_INIT(CAppPage)
	m_bAutoApp = TRUE;
	m_bAutoConn = TRUE;
	m_sAutoApp = _T("10");
	m_sAutoConn = _T("5");
	m_sConfApp = _T("30");
	m_sConfConn = _T("30");
	//}}AFX_DATA_INIT
	Create(IDD,(CWnd*)m_pParent);
}


void CAppPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAppPage)
	DDX_Control(pDX, IDC_CHECK_APP, m_btnAutoApp);
	DDX_Control(pDX, IDC_CHECK_CONN, m_btnAutoConn);
	DDX_Control(pDX, IDC_EDIT_CONF_CONN, m_editConfConn);
	DDX_Control(pDX, IDC_EDIT_AUTO_CONN, m_editAutoConn);
	DDX_Control(pDX, IDC_EDIT_CONF_APP, m_editConfApp);
	DDX_Control(pDX, IDC_EDIT_AUTO_APP, m_editAutoApp);
	DDX_Control(pDX, IDC_TXT_AUTO_CONN, m_staticAutoConn);
	DDX_Control(pDX, IDC_TXT_CONF_CONN, m_staticConfConn);
	DDX_Control(pDX, IDC_TXT_CONF_APP, m_staticConfApp);
	DDX_Control(pDX, IDC_TXT_AUTO_APP, m_staticAutoApp);
	DDX_Check(pDX, IDC_CHECK_APP, m_bAutoApp);
	DDX_Check(pDX, IDC_CHECK_CONN, m_bAutoConn);
	DDX_Text(pDX, IDC_EDIT_AUTO_APP, m_sAutoApp);
	DDX_Text(pDX, IDC_EDIT_AUTO_CONN, m_sAutoConn);
	DDX_Text(pDX, IDC_EDIT_CONF_APP, m_sConfApp);
	DDX_Text(pDX, IDC_EDIT_CONF_CONN, m_sConfConn);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAppPage, CSVPage)
	//{{AFX_MSG_MAP(CAppPage)
	ON_BN_CLICKED(IDC_CHECK_APP, OnCheckApp)
	ON_BN_CLICKED(IDC_CHECK_CONN, OnCheckConn)
	ON_EN_CHANGE(IDC_EDIT_AUTO_APP, OnChangeEditAutoApp)
	ON_EN_CHANGE(IDC_EDIT_AUTO_CONN, OnChangeEditAutoConn)
	ON_EN_CHANGE(IDC_EDIT_CONF_APP, OnChangeEditConfApp)
	ON_EN_CHANGE(IDC_EDIT_CONF_CONN, OnChangeEditConfConn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAppPage message handlers

BOOL CAppPage::OnInitDialog() 
{
	CSVPage::OnInitDialog();
	
	CancelChanges();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
BOOL CAppPage::IsDataValid()
{
	UpdateData(FALSE);
	DWORD dwAutoApp = 60*_ttoi(m_sAutoApp);
	DWORD dwConfApp = _ttoi(m_sConfApp);
	DWORD dwAutoConn = 60*_ttoi(m_sAutoConn);
    DWORD dwConfConn = _ttoi(m_sConfConn);

	if (m_bAutoApp && !m_bAutoConn)
	{
		AfxMessageBox(IDP_NOAPPWITHOUTCONN);
		return FALSE;
	}

	if (m_bAutoApp && (dwAutoApp<dwConfApp))
	{
		AfxMessageBox(IDP_APP_LT_CONF);
		m_editAutoApp.SetSel(-1,-1);
		m_editAutoApp.SetFocus();
		return FALSE;
	}

	if (m_bAutoConn && (dwAutoConn<dwConfConn))
	{
		AfxMessageBox(IDP_CONN_LT_CONF);
		m_editAutoConn.SetSel(-1,-1);
		m_editAutoConn.SetFocus();
		return FALSE;
	}

	if (m_bAutoConn && m_bAutoApp && (dwAutoApp<dwAutoConn))
	{
		AfxMessageBox(IDP_APP_LT_CONN);
		m_editAutoApp.SetSel(-1,-1);
		m_editAutoApp.SetFocus();
		return FALSE;
	}

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CAppPage::SaveChanges()
{
	WK_TRACE_USER(_T("AutoLogout-Einstellungen wurden geändert\n"));
	m_AutoLogout.SetValues(m_bAutoApp,
						   m_bAutoConn,
						   _ttoi(m_sAutoApp),
						   _ttoi(m_sConfApp),
						   _ttoi(m_sAutoConn),
						   _ttoi(m_sConfConn));
	m_AutoLogout.Save(GetProfile());
}
/////////////////////////////////////////////////////////////////////////////
void CAppPage::CancelChanges()
{
	m_AutoLogout.Load(GetProfile());
	m_bAutoApp = m_AutoLogout.GetAutoApplication();
	m_bAutoConn = m_AutoLogout.GetAutoConnection();
	if (m_bAutoApp)
	{
		m_sAutoApp.Format(_T("%d"),m_AutoLogout.GetApplicationLogoutTime());
		m_sConfApp.Format(_T("%d"),m_AutoLogout.GetApplicationConfirmTime());
	}
	if (m_bAutoConn)
	{
		m_sAutoConn.Format(_T("%d"),m_AutoLogout.GetConnectionLogoutTime());
		m_sConfConn.Format(_T("%d"),m_AutoLogout.GetConnectionConfirmTime());
	}
	UpdateData(FALSE);
	EnableControls();
}
/////////////////////////////////////////////////////////////////////////////
void CAppPage::EnableControls()
{
	m_btnAutoApp.EnableWindow(TRUE);
	m_staticAutoApp.EnableWindow(m_bAutoApp);
	m_editAutoApp.EnableWindow(m_bAutoApp);
	m_staticAutoConn.EnableWindow(m_bAutoConn);
	m_editAutoConn.EnableWindow(m_bAutoConn);

	m_btnAutoConn.EnableWindow(!m_bAutoApp);
	m_staticConfApp.EnableWindow(m_bAutoApp);
	m_editConfApp.EnableWindow(m_bAutoApp);
	m_staticConfConn.EnableWindow(m_bAutoConn);
	m_editConfConn.EnableWindow(m_bAutoConn);
}
/////////////////////////////////////////////////////////////////////////////
void CAppPage::OnCheckApp() 
{
	UpdateData();
	if (m_bAutoApp && !m_bAutoConn)
	{
		m_bAutoConn = TRUE;
		UpdateData(FALSE);
	}
	SetModified();
	EnableControls();
}
/////////////////////////////////////////////////////////////////////////////
void CAppPage::OnCheckConn() 
{
	UpdateData();
	SetModified();
	EnableControls();
}
/////////////////////////////////////////////////////////////////////////////
void CAppPage::OnChangeEditAutoApp() 
{
	UpdateData();
	CString sAutoApp = m_sAutoApp;
	int iCurSel = CheckDigits(sAutoApp);
	if (iCurSel>=0)
	{
		m_sAutoApp = sAutoApp;
		UpdateData(FALSE);
		m_editAutoApp.SetSel(iCurSel,iCurSel);
	}
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CAppPage::OnChangeEditAutoConn() 
{
	UpdateData();
	CString sAutoConn = m_sAutoConn;
	int iCurSel = CheckDigits(sAutoConn);
	if (iCurSel>=0)
	{
		m_sAutoConn = sAutoConn;
		UpdateData(FALSE);
		m_editAutoConn.SetSel(iCurSel,iCurSel);
	}
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CAppPage::OnChangeEditConfApp() 
{
	UpdateData();
	CString sConfApp = m_sConfApp;
	int iCurSel = CheckDigits(sConfApp);
	if (iCurSel>=0)
	{
		m_sConfApp = sConfApp;
		UpdateData(FALSE);
		m_editConfApp.SetSel(iCurSel,iCurSel);
	}
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CAppPage::OnChangeEditConfConn() 
{
	UpdateData();
	CString sConfConn = m_sConfConn;
	int iCurSel = CheckDigits(sConfConn);
	if (iCurSel>=0)
	{
		m_sConfConn = sConfConn;
		UpdateData(FALSE);
		m_editConfConn.SetSel(iCurSel,iCurSel);
	}
	SetModified();
}
