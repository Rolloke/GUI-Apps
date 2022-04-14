// TimerEditorCombi.cpp : implementation file
//

#include "stdafx.h"
#include "timereditor.h"
#include "TimerEditorCombi.h"

#include "TimerEditorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTimerEditorCombi dialog


CTimerEditorCombi::CTimerEditorCombi(CTimerEditorDlg* pParent)
	: CDialog(CTimerEditorCombi::IDD, pParent)
{
	m_pParent = pParent;
	//{{AFX_DATA_INIT(CTimerEditorCombi)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	Create(IDD,m_pParent);	// CAVEAT order is important, calls OnInitDialog
}


void CTimerEditorCombi::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTimerEditorCombi)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTimerEditorCombi, CDialog)
	//{{AFX_MSG_MAP(CTimerEditorCombi)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTimerEditorCombi message handlers

void CTimerEditorCombi::PostNcDestroy() 
{
	delete this;
}
void CTimerEditorCombi::OnOK()
{
	DestroyWindow();
}
void CTimerEditorCombi::OnCancel()
{
	DestroyWindow();
}


void CTimerEditorCombi::FillFromTimer(CSecTimer * pTimer, const CSecTimerArray & timers)
{
	// NOT YET

}
