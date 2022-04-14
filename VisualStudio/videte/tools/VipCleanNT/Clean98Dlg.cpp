// Clean98Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "vipcleannt.h"
#include "Clean98Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClean98Dlg dialog


CClean98Dlg::CClean98Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CClean98Dlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CClean98Dlg)
	m_sOEM = _T("");
	//}}AFX_DATA_INIT
}


void CClean98Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CClean98Dlg)
	DDX_Text(pDX, IDC_OEM, m_sOEM);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CClean98Dlg, CDialog)
	//{{AFX_MSG_MAP(CClean98Dlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClean98Dlg message handlers

void CClean98Dlg::SetOEM(CString sOEM)
{
	m_sOEM = sOEM;
}
