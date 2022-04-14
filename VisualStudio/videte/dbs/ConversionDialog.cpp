// ConversionDialog.cpp : implementation file
//

#include "stdafx.h"
#include "dbs.h"
#include "ConversionDialog.h"

#include "Conversion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CConversionDialog dialog


CConversionDialog::CConversionDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CConversionDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConversionDialog)
	m_sCurrentFile = _T("");
	m_sTime = _T("");
	//}}AFX_DATA_INIT
	m_dwStart = 0;
}


void CConversionDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConversionDialog)
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDC_PROGRESS, m_Progress);
	DDX_Text(pDX, IDC_FILE, m_sCurrentFile);
	DDX_Text(pDX, IDC_TIME, m_sTime);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConversionDialog, CDialog)
	//{{AFX_MSG_MAP(CConversionDialog)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConversionDialog message handlers

BOOL CConversionDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_dwCount = theApp.m_Conversions.GetSize();
	m_Progress.SetRange(0,(WORD)m_dwCount);
	m_Progress.SetStep(1);
	m_Progress.SetPos(0);

	m_OK.EnableWindow(FALSE);

	m_dwStart = GetTickCount();
	SetTimer(1,1000,NULL);
	m_ConvertThread.StartThread();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CConversionDialog::Convert()
{
	UpdateWindow();

	DWORD d = GetTickCount()-m_dwStart;
	DWORD s,m,h;
	int c;

	d /= 1000;
	s = d % 60;
	d /= 60;
	m = d % 60;
	h = d / 60;

	m_sTime.Format(_T("%02d:%02d:%02d"),h,m,s);
	UpdateData(FALSE);

	CAutoCritSec acs(&theApp.m_Conversions.m_cs);
	c = theApp.m_Conversions.GetSize();
	if (c>0)
	{
		CConversion* pConversion = theApp.m_Conversions.GetAtFast(0);
		if (pConversion)
		{
			m_sCurrentFile = pConversion->GetDbfPath();
			UpdateData(FALSE);
			m_Progress.SetPos(m_dwCount-c);
		}
	}
	else
	{
		// finished
		m_OK.EnableWindow();
		KillTimer(1);
		WK_TRACE(_T("CONVERSION in %s\n"),m_sTime);
		EndDialog(IDOK);
	}
	acs.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
void CConversionDialog::OnTimer(UINT nIDEvent) 
{
	Convert();
	
	CDialog::OnTimer(nIDEvent);
}
/////////////////////////////////////////////////////////////////////////////
void CConversionDialog::OnCancel() 
{
	m_ConvertThread.StopThread();
	KillTimer(1);
	
	CDialog::OnCancel();
}
