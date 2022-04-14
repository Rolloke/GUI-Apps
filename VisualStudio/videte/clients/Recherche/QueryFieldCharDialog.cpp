// QueryFieldCharDialog.cpp : implementation file
//

#include "stdafx.h"
#include "recherche.h"
#include "QueryFieldCharDialog.h"
#include "QueryDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CQueryFieldCharDialog dialog

CQueryFieldCharDialog::CQueryFieldCharDialog(const CString& sName, 
											 CIPCField* pField, 
											 CQueryDialog* pParent)   // standard constructor
	: CQueryFieldDialog(IDD_QUERY_FIELD_CHAR,sName,pField,pParent)
{
	//{{AFX_DATA_INIT(CQueryFieldCharDialog)
	//}}AFX_DATA_INIT
	m_sValue = m_pField->ToString();
	Create(IDD_QUERY_FIELD_CHAR,pParent);
}


void CQueryFieldCharDialog::DoDataExchange(CDataExchange* pDX)
{
	CQueryFieldDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQueryFieldCharDialog)
	DDX_Text(pDX, IDC_TXT_FIELD, m_sName);
	DDX_Text(pDX, IDC_EDIT_VALUE, m_sValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CQueryFieldCharDialog, CQueryFieldDialog)
	//{{AFX_MSG_MAP(CQueryFieldCharDialog)
	ON_EN_CHANGE(IDC_EDIT_VALUE, OnChangeEditValue)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQueryFieldCharDialog message handlers
/////////////////////////////////////////////////////////////////////////////
BOOL CQueryFieldCharDialog::OnInitDialog() 
{
	CQueryFieldDialog::OnInitDialog();

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CQueryFieldCharDialog::OnChangeEditValue() 
{
	UpdateData();
	m_pField->SetValue(m_sValue);
}
