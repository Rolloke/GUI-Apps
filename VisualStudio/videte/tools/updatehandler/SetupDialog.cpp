// SetupDialog.cpp : implementation file
//

#include "stdafx.h"
#include "updatehandler.h"
#include "SetupDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupDialog dialog


CSetupDialog::CSetupDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSetupDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetupDialog)
	m_sFile = _T("");
	//}}AFX_DATA_INIT
	m_iRetries = 0;
}


void CSetupDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupDialog)
	DDX_Control(pDX, IDC_PROGRESS_SEND, m_ctrlProgress);
	DDX_Control(pDX, IDOK, m_ctrlButtonOK);
	DDX_Control(pDX, IDCANCEL, m_ctrlButtonCancel);
	DDX_Control(pDX, IDC_BUTTON_UPDATE, m_ctrlButtonUpdate);
	DDX_Control(pDX, IDC_BUTTON_SEND, m_ctrlButtonSend);
	DDX_Control(pDX, IDC_LIST_CONFIRMED, m_ctrlListConfirmed);
	DDX_Control(pDX, IDC_LIST_SEND, m_ctrlListSend);
	DDX_Text(pDX, IDC_TXT_FILE, m_sFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetupDialog, CDialog)
	//{{AFX_MSG_MAP(CSetupDialog)
	ON_BN_CLICKED(IDC_BUTTON_SEND, OnButtonSend)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_UPDATE, OnButtonUpdate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupDialog message handlers

BOOL CSetupDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	WK_SearchFiles(m_saSend,theApp.GetSource(),_T("setup.*"));

	for (int i=0;i<m_saSend.GetSize();i++)
	{
		m_ctrlListSend.AddString(m_saSend[i]);
		TRACE(_T("must send %s\n"),m_saSend[i]);
	}

	SetTimer(1,1000,NULL);
	OnTimer(1);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CSetupDialog::OnButtonSend() 
{
	m_iRetries = 3;
	SendCurrentFile();
}
/////////////////////////////////////////////////////////////////////////////
void CSetupDialog::NextFile()
{
	BOOL bDone = FALSE;
	if (m_saSend.GetSize()>0)
	{
		m_saSend.RemoveAt(0);
		m_ctrlListSend.DeleteString(0);
		m_ctrlListConfirmed.AddString(m_sFile);

		if (m_saSend.GetSize()>0)
		{
			OnButtonSend();
		}
		else
		{
			// fertig
			bDone = TRUE; 
		}
	}
	else
	{
		// fertig
		bDone = TRUE; 
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSetupDialog::SendCurrentFile()
{
	CString sFile;
	m_sFile = m_saSend[0];
	sFile = theApp.GetSource() + _T('\\') + m_sFile;

	UpdateData(FALSE);

	int iSek = theApp.SendFile(sFile,_T("c:\\installation\\"));

	if (iSek>0)
	{
		m_ctrlProgress.SetRange32(0,iSek*2);
	}
	else
	{
		// TODO error
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSetupDialog::OnTimer(UINT nIDEvent) 
{
	if (m_sFile.IsEmpty())
	{
        m_ctrlButtonUpdate.EnableWindow(m_ctrlListSend.GetCount() == 0);
		m_ctrlButtonSend.EnableWindow(TRUE);
	}
	else
	{
		if (m_ctrlListSend.GetCount())
			m_ctrlProgress.StepIt();
		else
			m_ctrlProgress.SetPos(0);
		m_ctrlButtonSend.EnableWindow(FALSE);
        m_ctrlButtonUpdate.EnableWindow(m_ctrlListSend.GetCount() == 0);
	}
	
//	CDialog::OnTimer(nIDEvent);
}
/////////////////////////////////////////////////////////////////////////////
void CSetupDialog::OnConfirmFile(const CString& sFile)
{
	TRACE(_T("CSetupDialog::OnConfirmFile(%s)\n"),sFile);
	CString sFile1;
	int p = sFile.ReverseFind(_T('\\'));
	if (p!=-1)
	{
		sFile1 = sFile.Mid(p+1);
		if (0 == m_sFile.CompareNoCase(sFile1))
		{
			NextFile();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSetupDialog::OnError(const CString& sError)
{
	TRACE(_T("CSetupDialog::OnError(%s)\n"),sError);
	CString sFile;
	int p = sError.ReverseFind(_T('\\'));
	if (p!=-1)
	{
		sFile = sError.Mid(p+1);
		if (0 == m_sFile.CompareNoCase(sFile))
		{
			if (m_iRetries>1)
			{
				m_iRetries--;
				SendCurrentFile();
			}
			else
			{
				// TODO
				TRACE(_T("cannot send %s\n"),m_sFile);
			}
		}
		else
		{
			// TODO
			TRACE(_T("unknown error 1\n"));
		}
	}
	else
	{
		// TODO 
		TRACE(_T("unknown error 2\n"));
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSetupDialog::OnButtonUpdate() 
{
	// TODO: Add your control notification handler code here

	if (m_ctrlListSend.GetCount() == 0)
	{
		m_sCommand = _T("Setup c:\\installation\\setup.exe -auto");
		theApp.SendUpdateCommand(m_sCommand,FALSE);
	}
}
