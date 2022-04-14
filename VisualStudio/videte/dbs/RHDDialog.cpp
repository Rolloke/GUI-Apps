// RHDDialog.cpp : implementation file
//

#include "stdafx.h"
#include "dbs.h"
#include "RHDDialog.h"
#include "Drives.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRHDDialog dialog


CRHDDialog::CRHDDialog(BOOL bStart, CIPCDrive* pDrive,CWnd* pParent /*=NULL*/)
	: CDialog(CRHDDialog::IDD, pParent)
{
	m_bStart = bStart;
	m_pDrive = pDrive;
	m_bIsEmpty = CDrives::IsEmpty(pDrive->GetRootString());
	m_bIsRO = CDrives::IsRO(pDrive->GetRootString());
	m_bIsWrite = CDrives::IsRW(pDrive->GetRootString());
	m_uTimer = 0;
	m_iAction = 0;

	//{{AFX_DATA_INIT(CRHDDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CRHDDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRHDDialog)
	DDX_Control(pDX, IDC_PROGRESS, m_ctrlProgress);
	DDX_Control(pDX, IDC_TXT_TO_WRITE, m_txtToWrite);
	DDX_Control(pDX, IDC_BUTTON_TO_WRITE, m_ctrlToWrite);
	DDX_Control(pDX, IDC_TXT_TO_RO, m_txtToRO);
	DDX_Control(pDX, IDC_BUTTON_TO_RO, m_ctrlToRO);
	DDX_Control(pDX, IDC_TXT_DRIVE, m_ctrlDrive);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRHDDialog, CDialog)
	//{{AFX_MSG_MAP(CRHDDialog)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_TO_RO, OnButtonToRo)
	ON_BN_CLICKED(IDC_BUTTON_TO_WRITE, OnButtonToWrite)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRHDDialog message handlers

BOOL CRHDDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CString s1,s2;

	m_ctrlDrive.GetWindowText(s1);
	s2.Format(s1,m_pDrive->GetRootString(),m_pDrive->GetMB()/1024);
	m_ctrlDrive.SetWindowText(s2);

	m_uTimer = SetTimer(1,1000,NULL);

	if (m_bIsEmpty)
	{
		m_txtToRO.ShowWindow(SW_HIDE);
		m_ctrlToRO.ShowWindow(SW_HIDE);
		m_txtToWrite.ShowWindow(SW_HIDE);
		m_ctrlToWrite.ShowWindow(SW_HIDE);

		m_ctrlProgress.SetRange(0,1);
	}
	else
	{
		m_ctrlProgress.SetRange(0,20);
		m_ctrlProgress.SetStep(1);
		if (m_bIsRO)
		{
			m_txtToRO.ShowWindow(SW_HIDE);
			m_ctrlToRO.ShowWindow(SW_HIDE);
		}
		if (m_bIsWrite)
		{
			m_txtToWrite.ShowWindow(SW_HIDE);
			m_ctrlToWrite.ShowWindow(SW_HIDE);
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRHDDialog::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if (m_bIsEmpty)
	{
		EndDialog(IDOK);
	}
	else
	{
		m_ctrlProgress.StepIt();

		int nLower=0,nUpper=0,nPos = m_ctrlProgress.GetPos();

		m_ctrlProgress.GetRange(nLower,nUpper);

		TRACE(_T("RHD %d,%d\n"),nPos,nUpper);
		if (nPos == nUpper)
		{
			EndDialog(IDOK);
		}
	}
	
	CDialog::OnTimer(nIDEvent);
}

void CRHDDialog::OnDestroy() 
{
	CDialog::OnDestroy();
	
	if (m_uTimer!=0)
	{
		KillTimer(m_uTimer);
		m_uTimer = 0;
	}
	
}

void CRHDDialog::OnButtonToRo() 
{
	m_iAction = 1;
	EndDialog(IDOK);
}

void CRHDDialog::OnButtonToWrite() 
{
	m_iAction = 2;
	EndDialog(IDOK);
}
