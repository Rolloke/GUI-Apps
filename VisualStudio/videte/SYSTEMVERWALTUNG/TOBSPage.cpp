// TOBSPage.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "TOBSPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static TCHAR BASED_CODE szSection[]   = _T("TOBSUnit");

/////////////////////////////////////////////////////////////////////////////
// CTOBSPage dialog

CTOBSPage::CTOBSPage(CSVView* pParent) : CSVPage(CTOBSPage::IDD)
{
	m_pParent = pParent;
	//{{AFX_DATA_INIT(CTOBSPage)
	m_bTOBS = FALSE;
	m_sPrefix = _T("");
	m_iMinimumPrefix = 3;
	//}}AFX_DATA_INIT
	Create(IDD,pParent);
}


void CTOBSPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTOBSPage)
	DDX_Control(pDX, IDC_EDIT_PREFIX_MINIMUM, m_editPrefixMinimum);
	DDX_Control(pDX, IDC_EDIT_PREFIX, m_editPrefix);
	DDX_Check(pDX, IDC_CHECK_TOBS, m_bTOBS);
	DDX_Text(pDX, IDC_EDIT_PREFIX, m_sPrefix);
	DDX_Text(pDX, IDC_EDIT_PREFIX_MINIMUM, m_iMinimumPrefix);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTOBSPage, CSVPage)
	//{{AFX_MSG_MAP(CTOBSPage)
	ON_BN_CLICKED(IDC_CHECK_TOBS, OnCheckTobs)
	ON_EN_CHANGE(IDC_EDIT_PREFIX, OnChangeEditPrefix)
	ON_EN_CHANGE(IDC_EDIT_PREFIX_MINIMUM, OnChangeEditPrefixMinimum)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTOBSPage message handlers
BOOL CTOBSPage::IsDataValid()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CTOBSPage::SaveChanges()
{
	WK_TRACE_USER(_T("TOBS_Modul-Einstellungen wurden geändert\n"));
	UpdateData();
	GetProfile().WriteString(theApp.GetModuleSection(),WK_APPNAME_TOBSUNIT,m_bTOBS ? _T("TOBSUnit.exe") : _T(""));
	GetProfile().WriteString(szSection,_T("Prefix"),m_sPrefix);
	GetProfile().WriteInt(szSection,_T("MinimumPrefix"),m_iMinimumPrefix);
}
/////////////////////////////////////////////////////////////////////////////
void CTOBSPage::CancelChanges()
{
	CString sTemp;
	sTemp = GetProfile().GetString(theApp.GetModuleSection(),WK_APPNAME_TOBSUNIT,_T(""));
	m_bTOBS = sTemp.CompareNoCase(_T("TOBSUnit.exe"))==0; 
	m_sPrefix = GetProfile().GetString(szSection,_T("Prefix"),m_sPrefix);
	m_iMinimumPrefix = GetProfile().GetInt(szSection,_T("MinimumPrefix"),m_iMinimumPrefix);
	UpdateData(FALSE);
	ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTOBSPage::OnInitDialog() 
{
	CSVPage::OnInitDialog();
	
	CancelChanges();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CTOBSPage::OnCheckTobs() 
{
	OnChange();
	ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
void CTOBSPage::OnChangeEditPrefix() 
{
	OnChange();
}
/////////////////////////////////////////////////////////////////////////////
void CTOBSPage::OnChangeEditPrefixMinimum() 
{
	OnChange();
}
/////////////////////////////////////////////////////////////////////////////
void CTOBSPage::OnChange()
{
	UpdateData();
	SetModified();
	m_pParent->AddInitApp(WK_APPNAME_TOBSUNIT);
}
/////////////////////////////////////////////////////////////////////////////
void CTOBSPage::ShowHide()
{
	m_editPrefix.EnableWindow(m_bTOBS);
	m_editPrefixMinimum.EnableWindow(m_bTOBS);
}
