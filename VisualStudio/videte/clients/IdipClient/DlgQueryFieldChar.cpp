// QueryFieldCharDialog.cpp : implementation file
//

#include "stdafx.h"
#include "IdipClient.h"
#include "DlgQueryFieldChar.h"
#include "DlgQuery.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgQueryFieldChar dialog

CDlgQueryFieldChar::CDlgQueryFieldChar(const CString& sName, 
											 CIPCField* pField, 
											 CDlgQuery* pParent)   // standard constructor
	: CDlgQueryField(IDD_QUERY_FIELD_CHAR,sName,pField,pParent)
{
	//{{AFX_DATA_INIT(CDlgQueryFieldChar)
	//}}AFX_DATA_INIT
	m_sValue = m_pField->ToString();

	m_nInitToolTips = FALSE;
//	MoveOnClientAreaClick(true);
	Create(IDD_QUERY_FIELD_CHAR,pParent);
}


void CDlgQueryFieldChar::DoDataExchange(CDataExchange* pDX)
{
	CDlgQueryField::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgQueryFieldChar)
	DDX_Text(pDX, IDC_TXT_FIELD, m_sName);
	DDX_Text(pDX, IDC_EDIT_VALUE, m_sValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgQueryFieldChar, CDlgQueryField)
	//{{AFX_MSG_MAP(CDlgQueryFieldChar)
	ON_EN_CHANGE(IDC_EDIT_VALUE, OnChangeEditValue)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgQueryFieldChar message handlers
/////////////////////////////////////////////////////////////////////////////
BOOL CDlgQueryFieldChar::OnInitDialog() 
{
	CDlgQueryField::OnInitDialog();
	AutoCreateSkinStatic();
	AutoCreateSkinEdit();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CDlgQueryFieldChar::OnChangeEditValue() 
{
	UpdateData();
	m_pField->SetValue(m_sValue);
}
