// TimerWarning.cpp : implementation file
//

#include "stdafx.h"
#include "timereditor.h"
#include "TimerWarning.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTimerWarning dialog


CTimerWarning::CTimerWarning(CWnd* pParent /*=NULL*/)
	: CDialog(CTimerWarning::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTimerWarning)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CTimerWarning::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTimerWarning)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTimerWarning, CDialog)
	//{{AFX_MSG_MAP(CTimerWarning)
	ON_BN_CLICKED(ID_TIMER_OK, OnTimerOk)
	ON_BN_CLICKED(ID_TIMER_CANCEL, OnTimerCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTimerWarning message handlers

void CTimerWarning::OnTimerOk() 
{
	EndDialog(IDOK);
}

void CTimerWarning::OnTimerCancel() 
{
	EndDialog(IDCANCEL);		
}
