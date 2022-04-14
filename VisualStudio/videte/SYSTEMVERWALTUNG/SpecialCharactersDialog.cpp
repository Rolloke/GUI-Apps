// SpecialCharactersDialog.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "SpecialCharactersDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSpecialCharactersDialog dialog
CSpecialCharactersDialog::CSpecialCharactersDialog(CWnd* pParent /*=NULL*/)
	: CWK_Dialog(CSpecialCharactersDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSpecialCharactersDialog)
	m_sSpecialCharacter = _T("");
	//}}AFX_DATA_INIT
}
/////////////////////////////////////////////////////////////////////////////
CString CSpecialCharactersDialog::GetSpecialCharacter()
{
	return m_sSpecialCharacter.Left(4);
}
/////////////////////////////////////////////////////////////////////////////
void CSpecialCharactersDialog::FillListbox()
{
	m_lbSpecialCharacters.AddString(_T("\\001   SOH"));
	m_lbSpecialCharacters.AddString(_T("\\002   STX"));
	m_lbSpecialCharacters.AddString(_T("\\003   ETX"));
	m_lbSpecialCharacters.AddString(_T("\\004   EOT"));
	m_lbSpecialCharacters.AddString(_T("\\005   ENQ"));
	m_lbSpecialCharacters.AddString(_T("\\006   ACK"));
	m_lbSpecialCharacters.AddString(_T("\\007   BEL"));
	m_lbSpecialCharacters.AddString(_T("\\008   BS"));
	m_lbSpecialCharacters.AddString(_T("\\009   HT"));
	m_lbSpecialCharacters.AddString(_T("\\010   LF"));
	m_lbSpecialCharacters.AddString(_T("\\011   VT"));
	m_lbSpecialCharacters.AddString(_T("\\012   FF"));
	m_lbSpecialCharacters.AddString(_T("\\013   CR"));
	m_lbSpecialCharacters.AddString(_T("\\014   SO"));
	m_lbSpecialCharacters.AddString(_T("\\015   SI"));
	m_lbSpecialCharacters.AddString(_T("\\016   SLE"));
	m_lbSpecialCharacters.AddString(_T("\\017   CS1"));
	m_lbSpecialCharacters.AddString(_T("\\018   DC2"));
	m_lbSpecialCharacters.AddString(_T("\\019   DC3"));
	m_lbSpecialCharacters.AddString(_T("\\020   DC4"));
	m_lbSpecialCharacters.AddString(_T("\\021   NAK"));
	m_lbSpecialCharacters.AddString(_T("\\022   SYN"));
	m_lbSpecialCharacters.AddString(_T("\\023   ETB"));
	m_lbSpecialCharacters.AddString(_T("\\024   CAN"));
	m_lbSpecialCharacters.AddString(_T("\\025   EM"));
	m_lbSpecialCharacters.AddString(_T("\\026   SIB"));
	m_lbSpecialCharacters.AddString(_T("\\027   ESC"));
	m_lbSpecialCharacters.AddString(_T("\\028   FS"));
	m_lbSpecialCharacters.AddString(_T("\\029   GS"));
	m_lbSpecialCharacters.AddString(_T("\\030   RS"));
	m_lbSpecialCharacters.AddString(_T("\\031   US"));
}
/////////////////////////////////////////////////////////////////////////////
void CSpecialCharactersDialog::DoDataExchange(CDataExchange* pDX)
{
	CWK_Dialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpecialCharactersDialog)
	DDX_Control(pDX, IDC_LIST_SPECIAL_CHARACTERS, m_lbSpecialCharacters);
	DDX_LBString(pDX, IDC_LIST_SPECIAL_CHARACTERS, m_sSpecialCharacter);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CSpecialCharactersDialog, CWK_Dialog)
	//{{AFX_MSG_MAP(CSpecialCharactersDialog)
	ON_LBN_SELCHANGE(IDC_LIST_SPECIAL_CHARACTERS, OnSelchangeListSpecialCharacters)
	ON_LBN_DBLCLK(IDC_LIST_SPECIAL_CHARACTERS, OnDblclkListSpecialCharacters)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CSpecialCharactersDialog message handlers
/////////////////////////////////////////////////////////////////////////////
BOOL CSpecialCharactersDialog::OnInitDialog() 
{
	CWK_Dialog::OnInitDialog();
	if (CSkinDialog::SetChildWindowFont(m_hWnd))
	{
		if (m_ToolTip.m_hWnd)
		{
			m_ToolTip.SetFont(CSkinDialog::GetDialogItemGlobalFont(), 0);
		}
	}
	
	FillListbox();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CSpecialCharactersDialog::OnSelchangeListSpecialCharacters() 
{
	UpdateData();
}
/////////////////////////////////////////////////////////////////////////////
void CSpecialCharactersDialog::OnDblclkListSpecialCharacters() 
{
	UpdateData();
	OnOK();	
}
/////////////////////////////////////////////////////////////////////////////
void CSpecialCharactersDialog::OnOK() 
{
	CWK_Dialog::OnOK();
}
