// RemoteDialog.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "RemoteDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRemoteDialog dialog
CRemoteDialog::CRemoteDialog(CSecID hostID, 
							 BOOL bReceiving,
							 CWnd* pParent /*=NULL*/)
	: CWK_Dialog(CRemoteDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRemoteDialog)
	m_sState = _T("");
	m_sHost = _T("");
	//}}AFX_DATA_INIT

	m_dwCounter = 0;
	m_dwTimeout = 1200; // 120*10*100ms Timer
	CWK_Profile wkp;
	m_dwTimeout = 10 * wkp.GetInt(REG_KEY_SUPERVISOR, _T("RemoteTimeoutSeconds"), m_dwTimeout/10);
	wkp.WriteInt(REG_KEY_SUPERVISOR, _T("RemoteTimeoutSeconds"), m_dwTimeout/10);

	m_HostID = hostID;
	CHostArray ha;
	ha.Load(wkp);
	CHost* pHost = ha.GetHost(m_HostID);
	if (pHost)
	{
		m_sHost = pHost->GetName();
	}
	m_bReceiving = bReceiving;
}
/////////////////////////////////////////////////////////////////////////////
void CRemoteDialog::DoDataExchange(CDataExchange* pDX)
{
	CWK_Dialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRemoteDialog)
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDC_PROGRESS, m_ctrlProgress);
	DDX_Text(pDX, IDC_STATE, m_sState);
	DDX_Text(pDX, IDC_HOST, m_sHost);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CRemoteDialog, CWK_Dialog)
	//{{AFX_MSG_MAP(CRemoteDialog)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CRemoteDialog message handlers
/////////////////////////////////////////////////////////////////////////////
BOOL CRemoteDialog::OnInitDialog() 
{
	CWK_Dialog::OnInitDialog();
	if (CSkinDialog::SetChildWindowFont(m_hWnd))
	{
		if (m_ToolTip.m_hWnd)
		{
			m_ToolTip.SetFont(CSkinDialog::GetDialogItemGlobalFont(), 0);
		}
	}

	m_btnOK.EnableWindow(FALSE);

	m_RemoteThread.m_pRemoteDialog = this;
	m_RemoteThread.m_sRegistryFile = m_sRegistryFile;
	m_RemoteThread.m_nRemoteCodePage = m_nRemoteCodePage;

	m_RemoteThread.m_sCoCoISAini = m_sCoCoISAini;

	m_ctrlProgress.SetRange32(0, m_dwTimeout);

	if (m_bReceiving)
	{
		WK_TRACE(_T("Starting Remote SV %s Receive\n"), m_sHost);
	}
	else
	{
		WK_TRACE(_T("Starting Remote SV %s Send\n"), m_sHost);
	}
	
	SetTimer(1,100,NULL);
	m_RemoteThread.StartThread();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CRemoteDialog::OnTimer(UINT nIDEvent) 
{
	UpdateData(FALSE);

	if (m_RemoteThread.IsRunning())
	{
		m_ctrlProgress.StepIt();
		m_dwCounter++;
		
		if (m_dwCounter & 0x00000010)
		{
			TRACE(_T("IsRunning: Counter: %u \n"), m_dwCounter);
		}
		if (m_dwCounter >= m_dwTimeout)
		{
			// time out
			m_RemoteThread.Cancel();
			m_sRegistryFile.Empty();
			m_sCoCoISAini.Empty();
			KillTimer(1);
			WK_TRACE(_T("Remote SV %s TIMEOUT %u s\n"), m_sHost, m_dwTimeout/10);
			m_sState.LoadString(IDS_DISCONNECTED_ERROR);
		}
	}
	else
	{
		if (m_bReceiving)
		{
			if (m_dwTimeout & 0x00000010)
			{
				TRACE(_T("Receiving: Counter: %u \n"), m_dwTimeout);
			}
			if (m_RemoteThread.HasSettings() &&
				m_RemoteThread.HasDrives())
			{
				m_ctrlProgress.SetPos(m_dwTimeout);
				m_btnOK.EnableWindow(TRUE);
				if(GetFocus() == &m_btnCancel)
				{
					m_btnOK.SetFocus();
				}
				m_btnCancel.EnableWindow(FALSE);
				m_sRegistryFile = m_RemoteThread.m_sRegistryFile;
				m_nRemoteCodePage = m_RemoteThread.m_nRemoteCodePage;
				m_sCoCoISAini = m_RemoteThread.m_sCoCoISAini;
				KillTimer(1);
				WK_TRACE(_T("Remote SV %s Received OK\n"), m_sHost);
			}
		}
		else
		{
			if (m_dwTimeout & 0x00000010)
			{
				TRACE(_T("Sending: Counter: %u \n"), m_dwTimeout);
			}
			if (m_RemoteThread.m_bSend)
			{
				m_ctrlProgress.SetPos(m_dwTimeout);
				m_btnOK.EnableWindow(TRUE);
				if(GetFocus() == &m_btnCancel)
				{
					m_btnOK.SetFocus();
				}
				m_btnCancel.EnableWindow(FALSE);
				KillTimer(1);
				WK_TRACE(_T("Remote SV %s Sent OK\n"), m_sHost);
			}
		}
	}
	UpdateData(FALSE);

	CWK_Dialog::OnTimer(nIDEvent);
}
/////////////////////////////////////////////////////////////////////////////
void CRemoteDialog::OnCancel() 
{
	if (m_RemoteThread.IsRunning())
	{
		m_RemoteThread.Cancel();
	}
	m_RemoteThread.StopThread();
	CWK_Dialog::OnCancel();
}
/////////////////////////////////////////////////////////////////////////////
CIPCDrives& CRemoteDialog::GetDrives()
{
	return m_RemoteThread.m_Drives;
}
