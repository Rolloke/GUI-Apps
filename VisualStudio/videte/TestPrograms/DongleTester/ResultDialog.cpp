// ResultDialog.cpp : implementation file
//

#include "stdafx.h"
#include "DongleTester.h"
#include "ResultDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ResultDialog dialog


ResultDialog::ResultDialog(CWnd* pParent /*=NULL*/)
	: CDialog(ResultDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(ResultDialog)
	m_sElapsedTime = _T("");
	m_sNumTests = _T("");
	//}}AFX_DATA_INIT
}


void ResultDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ResultDialog)
	DDX_Text(pDX, IDC_MSG_ELAPSED_TIME, m_sElapsedTime);
	DDX_Text(pDX, IDC_MSG_NUM_OUT, m_sNumTests);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ResultDialog, CDialog)
	//{{AFX_MSG_MAP(ResultDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ResultDialog message handlers
