// TimerImportDialog.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "TimerImportDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTimerImportDialog dialog


CTimerImportDialog::CTimerImportDialog(const CStringArray &names, CWnd* pParent /*=NULL*/)
	: CWK_Dialog(CTimerImportDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTimerImportDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_names.Append(names);
}


void CTimerImportDialog::DoDataExchange(CDataExchange* pDX)
{
	CWK_Dialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTimerImportDialog)
	DDX_Control(pDX, IDC_TIMER_IMPORT_DUPLICATES, m_ctlDuplicates);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTimerImportDialog, CWK_Dialog)
	//{{AFX_MSG_MAP(CTimerImportDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTimerImportDialog message handlers

BOOL CTimerImportDialog::OnInitDialog() 
{
	CWK_Dialog::OnInitDialog();
	if (CSkinDialog::SetChildWindowFont(m_hWnd))
	{
		if (m_ToolTip.m_hWnd)
		{
			m_ToolTip.SetFont(CSkinDialog::GetDialogItemGlobalFont(), 0);
		}
	}

	CString s;

	s.LoadString(IDS_TIMERPAGE);

	m_ctlDuplicates.InsertColumn(0,s,LVCFMT_LEFT,300);
	for (int i=0;i<m_names.GetSize();i++) 
	{
		m_ctlDuplicates.InsertItem(0,m_names[i]);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
