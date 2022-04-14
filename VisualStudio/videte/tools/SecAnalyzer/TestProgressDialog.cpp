// TestProgressDialog.cpp : implementation file
//

#include "stdafx.h"
#include "secanalyzer.h"
#include "TestProgressDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// TestProgressDialog dialog


TestProgressDialog::TestProgressDialog(CWnd* pParent /*=NULL*/)
	: CDialog(TestProgressDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(TestProgressDialog)
	m_sTestTime = _T("");
	//}}AFX_DATA_INIT
}


void TestProgressDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TestProgressDialog)
	DDX_Control(pDX, IDC_TEST_PROGRESS, m_ctlProgress);
	DDX_Text(pDX, IDC_LABEL_TEST_TIME, m_sTestTime);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(TestProgressDialog, CDialog)
	//{{AFX_MSG_MAP(TestProgressDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TestProgressDialog message handlers
