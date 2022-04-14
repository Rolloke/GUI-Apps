// QueryFieldNumDialog.cpp : implementation file
//

#include "stdafx.h"
#include "IdipClient.h"
#include "DlgQueryFieldNum.h"
#include "DlgQuery.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgQueryFieldNum dialog


CDlgQueryFieldNum::CDlgQueryFieldNum(const CString& sName, CIPCField* pField,CDlgQuery* pParent)
	: CDlgQueryField(IDD_QUERY_FIELD_NUM,sName,pField,pParent)
{
	//{{AFX_DATA_INIT(CDlgQueryFieldNum)
	//}}AFX_DATA_INIT
	m_sValue = m_pField->ToString();
	m_nInitToolTips = FALSE;
//	MoveOnClientAreaClick(true);
	Create(IDD_QUERY_FIELD_NUM,pParent);
}


void CDlgQueryFieldNum::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgQueryFieldNum)
	DDX_Text(pDX, IDC_TXT_FIELD, m_sName);
	DDX_Text(pDX, IDC_EDIT_VALUE, m_sValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgQueryFieldNum, CSkinDialog)
	//{{AFX_MSG_MAP(CDlgQueryFieldNum)
	ON_EN_CHANGE(IDC_EDIT_VALUE, OnChangeEditValue)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgQueryFieldNum message handlers
/////////////////////////////////////////////////////////////////////////////
BOOL CDlgQueryFieldNum::OnInitDialog() 
{
	CSkinDialog::OnInitDialog();
	AutoCreateSkinStatic();
	AutoCreateSkinEdit();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CDlgQueryFieldNum::OnChangeEditValue() 
{
	UpdateData();
	m_pField->SetValue(m_sValue);
}
