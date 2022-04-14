// PinDialog.cpp : implementation file
//

#include "stdafx.h"
#include "productview.h"
#include "PinDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPinDialog dialog


CPinDialog::CPinDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CPinDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPinDialog)
	m_sPin = _T("");
	//}}AFX_DATA_INIT
}


void CPinDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPinDialog)
	DDX_Text(pDX, IDC_PIN, m_sPin);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPinDialog, CDialog)
	//{{AFX_MSG_MAP(CPinDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPinDialog message handlers
