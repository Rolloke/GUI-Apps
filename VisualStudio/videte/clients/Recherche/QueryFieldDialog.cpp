// QueryFieldDialog.cpp : implementation file
//

#include "stdafx.h"
#include "recherche.h"
#include "QueryFieldDialog.h"
#include "QueryDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CQueryFieldDialog dialog


CQueryFieldDialog::CQueryFieldDialog(UINT nIDTemplate,
									 const CString& sName, 
									 CIPCField* pField, 
									 CQueryDialog* pParent /*=NULL*/)
	: CDialog(nIDTemplate, pParent)
{
	m_pQueryDialog = pParent;
	m_pField = pField;
	m_sName = sName + ":";
	//{{AFX_DATA_INIT(CQueryFieldDialog)
	//}}AFX_DATA_INIT
}


void CQueryFieldDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQueryFieldDialog)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CQueryFieldDialog, CDialog)
	//{{AFX_MSG_MAP(CQueryFieldDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CString CQueryFieldDialog::GetName()
{
	return m_sName;
}
/////////////////////////////////////////////////////////////////////////////
// CQueryFieldDialog message handlers
void CQueryFieldDialog::OnOK() 
{
}
/////////////////////////////////////////////////////////////////////////////
void CQueryFieldDialog::OnCancel() 
{
}
/////////////////////////////////////////////////////////////////////////////
void CQueryFieldDialog::PostNcDestroy() 
{
	delete this;
}
