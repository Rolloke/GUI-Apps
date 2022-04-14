// QueryFieldDialog.cpp : implementation file
//

#include "stdafx.h"
#include "IdipClient.h"
#include "DlgQueryField.h"
#include "DlgQuery.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgQueryField dialog


CDlgQueryField::CDlgQueryField(UINT nIDTemplate,
									 const CString& sName, 
									 CIPCField* pField, 
									 CDlgQuery* pParent /*=NULL*/)
	: CSkinDialog(nIDTemplate, pParent)
{
	m_pDlgQuery = pParent;
	m_pField = pField;
	m_sName = sName + ":";
	//{{AFX_DATA_INIT(CDlgQueryField)
	//}}AFX_DATA_INIT

	m_nInitToolTips = FALSE;
	if (UseOEMSkin())
	{
		CreateTransparent(StyleBackGroundPlaneColor, m_BaseColor);
		MoveOnClientAreaClick(false);
	}
}


void CDlgQueryField::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgQueryField)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgQueryField, CSkinDialog)
	//{{AFX_MSG_MAP(CDlgQueryField)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CString CDlgQueryField::GetName()
{
	return m_sName;
}
/////////////////////////////////////////////////////////////////////////////
// CDlgQueryField message handlers
void CDlgQueryField::OnOK() 
{
}
/////////////////////////////////////////////////////////////////////////////
void CDlgQueryField::OnCancel() 
{
}
/////////////////////////////////////////////////////////////////////////////
void CDlgQueryField::PostNcDestroy() 
{
	delete this;
}
