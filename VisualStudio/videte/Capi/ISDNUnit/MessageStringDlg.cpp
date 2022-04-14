/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: MessageStringDlg.cpp $
// ARCHIVE:		$Archive: /Project/Capi/ISDNUnit/MessageStringDlg.cpp $
// CHECKIN:		$Date: 6.05.98 12:05 $
// VERSION:		$Revision: 5 $
// LAST CHANGE:	$Modtime: 6.05.98 12:03 $
// BY AUTHOR:	$Author: Hedu $
// $Nokeywords:$

#include "stdafx.h"
#include "IsdnUnit.h"
#include "MessageStringDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMessageStringDlg dialog

// OOPS no more used, maybe back in the future
/////////////////////////////////////////////////////////////////////////////
CMessageStringDlg::CMessageStringDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMessageStringDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMessageStringDlg)
	m_sMessage = _T("");
	//}}AFX_DATA_INIT

	if (pParent == NULL) 
	{
		pParent = AfxGetMainWnd();
	}

	m_pWndParent = pParent;
	Create(CMessageStringDlg::IDD, pParent);
	// only create modeless dialog box, show in connection with a message
}
/////////////////////////////////////////////////////////////////////////////
CMessageStringDlg::~CMessageStringDlg()
{
}
/////////////////////////////////////////////////////////////////////////////
void CMessageStringDlg::PostNcDestroy()
{
	delete this;
}
/////////////////////////////////////////////////////////////////////////////
void CMessageStringDlg::Show(LPCSTR pStringMsg /*=NULL*/)
{
	if (pStringMsg) 
	{
		m_sMessage = pStringMsg;
		UpdateData(FALSE);
	}
	ShowWindow(SW_SHOW);
	BringWindowToTop();
	InstallHideTimer();
}
/////////////////////////////////////////////////////////////////////////////
void CMessageStringDlg::Show(UINT nMsg)
{
	if (nMsg) 
	{
		m_sMessage.Empty();
		m_sMessage.LoadString(nMsg);
		UpdateData(FALSE);
	}
	ShowWindow(SW_SHOW);
	BringWindowToTop();
	InstallHideTimer();
}
/////////////////////////////////////////////////////////////////////////////
void CMessageStringDlg::Hide()
{
	ShowWindow(SW_HIDE);
}
/////////////////////////////////////////////////////////////////////////////
void CMessageStringDlg::SetText(CString strMsg)
{
	m_sMessage = strMsg;
	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CMessageStringDlg::SetText(UINT nMsg)
{
	m_sMessage.Empty();
	m_sMessage.LoadString(nMsg);
	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CMessageStringDlg::InstallHideTimer()
{
	SetTimer(1,5000,NULL);
}
/////////////////////////////////////////////////////////////////////////////
void CMessageStringDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMessageStringDlg)
	DDX_Text(pDX, IDC_MESSAGE, m_sMessage);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CMessageStringDlg, CDialog)
	//{{AFX_MSG_MAP(CMessageStringDlg)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
BOOL CMessageStringDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	SetWindowText(AfxGetAppName());
	if (m_pWndParent)
	{
		CenterWindow(m_pWndParent);
	}
	else
	{
		CenterWindow(GetDesktopWindow());
	}
	return TRUE;  // return TRUE  unless you set the focus to a control
}
/////////////////////////////////////////////////////////////////////////////
void CMessageStringDlg::OnOK()
{
	ShowWindow(SW_HIDE);
}
/////////////////////////////////////////////////////////////////////////////
void CMessageStringDlg::OnCancel()
{
	ShowWindow(SW_HIDE);
}
/////////////////////////////////////////////////////////////////////////////
void CMessageStringDlg::OnTimer(UINT nIDEvent) 
{
	ShowWindow(SW_HIDE);
	KillTimer(1);
}
