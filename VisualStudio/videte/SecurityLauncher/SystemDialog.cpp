// SystemDialog.cpp : implementation file
//

#include "stdafx.h"
#include "securitylauncher.h"
#include "SystemDialog.h"

#ifndef _AFXEXT
	#ifdef AFX_EXT_CLASS
	#undef AFX_EXT_CLASS
	#define AFX_EXT_CLASS
	#endif
#endif
#include "OemGuiApi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CSecurityLauncherApp theApp;
/////////////////////////////////////////////////////////////////////////////
// CSystemDialog dialog


CSystemDialog::CSystemDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSystemDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSystemDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_iWhatToDo = WTD_SYS_DOWN;
	m_bShell = FALSE;
	m_bBackup = FALSE;
}


void CSystemDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSystemDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	if (pDX->m_bSaveAndValidate)
	{
		if (((CButton*)GetDlgItem(IDC_RADIO_SYS_DOWN))->GetCheck()==1)
		{
			m_iWhatToDo = WTD_SYS_DOWN;
		}
		else if (((CButton*)GetDlgItem(IDC_RADIO_SYS_REBOOT))->GetCheck()==1)
		{
			m_iWhatToDo = WTD_SYS_REBOOT;
		}
		else if (((CButton*)GetDlgItem(IDC_RADIO_WIN_EXPLORER))->GetCheck()==1)
		{
			m_iWhatToDo = WTD_WIN_EXPLORER;
		}
		else if (((CButton*)GetDlgItem(IDC_RADIO_EXPLORER))->GetCheck()==1)
		{
			m_iWhatToDo = WTD_EXPLORER;
		}
		else if (((CButton*)GetDlgItem(IDC_RADIO_WIN_LAUNCHER))->GetCheck()==1)
		{
			m_iWhatToDo = WTD_WIN_LAUNCHER;
		}
		else if (((CButton*)GetDlgItem(IDC_RADIO_DBBACKUP))->GetCheck()==1)
		{
			m_iWhatToDo = WTD_DBBACKUP;
		}
	}
	else
	{
		int id = IDC_RADIO_SYS_DOWN;
		switch (m_iWhatToDo)
		{
		case WTD_SYS_DOWN:
			id = IDC_RADIO_SYS_DOWN;
			break;
		case WTD_SYS_REBOOT:
			id = IDC_RADIO_SYS_REBOOT;
			break;
		case WTD_WIN_EXPLORER:
			id = IDC_RADIO_WIN_EXPLORER;
			break;
		case WTD_EXPLORER:
			id = IDC_RADIO_EXPLORER;
			break;
		case WTD_WIN_LAUNCHER:
			id = IDC_RADIO_WIN_LAUNCHER;
			break;
		case WTD_DBBACKUP:
			id = IDC_RADIO_DBBACKUP;
			break;
		default:
			id = IDC_RADIO_SYS_DOWN;
		}
		((CButton*)GetDlgItem(id))->SetCheck(1);
	}

}


BEGIN_MESSAGE_MAP(CSystemDialog, CDialog)
	//{{AFX_MSG_MAP(CSystemDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSystemDialog message handlers

BOOL CSystemDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CenterWindow(CWnd::GetDesktopWindow());

	if (m_bShell)
	{
		GetDlgItem(IDC_RADIO_WIN_LAUNCHER)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_RADIO_WIN_EXPLORER)->EnableWindow(FALSE);
	}

	GetDlgItem(IDC_RADIO_DBBACKUP)->EnableWindow(m_bBackup);
	
	CString s1,s2;
	GetWindowText(s1);
	s2.Format(s1,theApp.GetName());
	SetWindowText(s2);

	GetDlgItemText(IDC_RADIO_SYS_DOWN,s1);
	s2.Format(s1,COemGuiApi::GetProductName());
	SetDlgItemText(IDC_RADIO_SYS_DOWN,s2);

	GetDlgItemText(IDC_RADIO_SYS_REBOOT,s1);
	s2.Format(s1,COemGuiApi::GetProductName());
	SetDlgItemText(IDC_RADIO_SYS_REBOOT,s2);

	GetDlgItemText(IDC_RADIO_WIN_LAUNCHER,s1);
	s2.Format(s1,theApp.GetName());
	SetDlgItemText(IDC_RADIO_WIN_LAUNCHER,s2);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
