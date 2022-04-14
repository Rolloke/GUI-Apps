// QueryFieldNumDialog.cpp : implementation file
//

#include "stdafx.h"
#include "recherche.h"
#include "QueryFieldNumDialog.h"
#include "QueryDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CQueryFieldNumDialog dialog


CQueryFieldNumDialog::CQueryFieldNumDialog(const CString& sName, CIPCField* pField,CQueryDialog* pParent)
	: CQueryFieldDialog(IDD_QUERY_FIELD_NUM,sName,pField,pParent)
{
	//{{AFX_DATA_INIT(CQueryFieldNumDialog)
	//}}AFX_DATA_INIT
	m_sValue = m_pField->ToString();
	Create(IDD_QUERY_FIELD_NUM,pParent);
}


void CQueryFieldNumDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQueryFieldNumDialog)
	DDX_Text(pDX, IDC_TXT_FIELD, m_sName);
	DDX_Text(pDX, IDC_EDIT_VALUE, m_sValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CQueryFieldNumDialog, CDialog)
	//{{AFX_MSG_MAP(CQueryFieldNumDialog)
	ON_EN_CHANGE(IDC_EDIT_VALUE, OnChangeEditValue)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQueryFieldNumDialog message handlers
/////////////////////////////////////////////////////////////////////////////
BOOL CQueryFieldNumDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CQueryFieldNumDialog::OnChangeEditValue() 
{
	UpdateData();
	m_pField->SetValue(m_sValue);
}
