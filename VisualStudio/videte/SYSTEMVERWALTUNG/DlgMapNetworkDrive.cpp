// DlgMapNetworkDrive.cpp : implementation file
//

#include "stdafx.h"
#include "SystemVerwaltung.h"
#include "DlgMapNetworkDrive.h"
#include ".\dlgmapnetworkdrive.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgMapNetworkDrive dialog
IMPLEMENT_DYNAMIC(CDlgMapNetworkDrive, CWK_Dialog)
CDlgMapNetworkDrive::CDlgMapNetworkDrive(CWK_Profile&wkp, CWnd* pParent /*=NULL*/)
	: CWK_Dialog(CDlgMapNetworkDrive::IDD, pParent)
{
	m_sPath      = _T("");
	m_sDomain    = _T("");
	m_sUser      = _T("");
	m_sPassword  = _T("");
	m_pProfile   = &wkp;
	m_pNewDrive = NULL;
	m_nInitToolTips = TOOLTIPS_SIMPLE;
}
/////////////////////////////////////////////////////////////////////////////
CDlgMapNetworkDrive::~CDlgMapNetworkDrive()
{
	WK_DELETE(m_pNewDrive);
}
/////////////////////////////////////////////////////////////////////////////
void CDlgMapNetworkDrive::SetVolume(CString&sVolume)
{
	CStringArray sa;
	SplitString(sVolume, sa, _T(';'));
	m_sPath = sa.GetAt(NETWORK_PATH);
	if (sa.GetSize()>NETWORK_DOMAIN)
	{
		m_sDomain = sa.GetAt(NETWORK_DOMAIN);
	}
	if (sa.GetSize()>NETWORK_USER)
	{
		m_sUser = sa.GetAt(NETWORK_USER);
	}
	if (sa.GetSize()>NETWORK_PASSWORD)
	{
		m_sPassword = sa.GetAt(NETWORK_PASSWORD);
		Decode(m_sPassword);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDlgMapNetworkDrive::SetRoot(CString&sRoot)
{
	m_sRoot = sRoot;
	m_sRoot.MakeUpper();
}
/////////////////////////////////////////////////////////////////////////////
void CDlgMapNetworkDrive::DoDataExchange(CDataExchange* pDX)
{
	CWK_Dialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_MND_LETTER, m_cDriveLetters);
	DDX_Text(pDX, IDC_EDT_MND_DOMAIN, m_sDomain);
	DDX_Text(pDX, IDC_EDT_MND_USER, m_sUser);
	DDX_Text(pDX, IDC_EDT_MND_PASSWORD, m_sPassword);

	DDX_Text(pDX, IDC_EDT_MND_PATH, m_sPath);
	if (pDX->m_bSaveAndValidate)
	{
		if (m_sPath.IsEmpty())
		{
			AfxMessageBox(IDP_NOEMPTY_FIELDS, MB_OK|MB_ICONERROR);
			pDX->Fail();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDlgMapNetworkDrive, CWK_Dialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_EN_CHANGE(IDC_EDT_MND_DOMAIN, OnChange)
	ON_EN_CHANGE(IDC_EDT_MND_USER, OnChange)
	ON_EN_CHANGE(IDC_EDT_MND_PASSWORD, OnChange)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CDlgMapNetworkDrive message handlers
BOOL CDlgMapNetworkDrive::OnInitDialog()
{
	CWK_Dialog::OnInitDialog();

	DWORD dwMask=1, dwLogical = GetLogicalDrives();
	int i, nSel = 0;				// list the available disks
	CString sFmt;
	for (i=3,dwMask=8; i<26; i++,dwMask<<=1)
	{
		if (!(dwMask & dwLogical))
		{
			sFmt.Format(_T("%c:"), i+'A');
			m_cDriveLetters.AddString(sFmt);
		}
	}

	if (!m_sRoot.IsEmpty())
	{
		nSel = m_cDriveLetters.AddString(m_sRoot.Left(2));
		m_cDriveLetters.EnableWindow(FALSE);
		((CEdit*)GetDlgItem(IDC_EDT_MND_PATH))->SetReadOnly();
//		((CEdit*)GetDlgItem(IDC_EDT_MND_DOMAIN))->SetReadOnly();
//		((CEdit*)GetDlgItem(IDC_EDT_MND_USER))->SetReadOnly();
//		((CEdit*)GetDlgItem(IDC_EDT_MND_PASSWORD))->SetReadOnly();
		GetDlgItem(IDOK)->EnableWindow(FALSE);
	}

	sFmt.LoadString(IDS_DRIVE);
	SetDlgItemText(IDC_TXT_MND_LETTER, sFmt);

	if (m_cDriveLetters.GetCount())
	{
		m_cDriveLetters.SetCurSel(nSel);
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CDlgMapNetworkDrive::OnBnClickedOk()
{
	if (UpdateData())
	{
		CString sLetter;
		m_cDriveLetters.GetLBText(m_cDriveLetters.GetCurSel(), sLetter);
		if (!sLetter.IsEmpty())
		{
			if (CIPCDrive::InitNetworkApi())
			{
				sLetter.MakeLower();
				WK_DELETE(m_pNewDrive);
				m_pNewDrive = new CIPCDrive(sLetter+_T("\\"), DVR_NORMAL_DRIVE);
				CString sPath = m_sPath;
				if (!m_sPassword.IsEmpty() && !m_sUser.IsEmpty())
				{
					if (m_sDomain.IsEmpty())
					{
						sPath += _T("; ");
					}
					else
					{
						sPath += _T(";") + m_sDomain;
					}
					sPath += _T(";") + m_sUser;
					CString sPassword = m_sPassword;
					Encode(sPassword);
					sPath += _T(";") + sPassword;
				}
				m_pNewDrive->SetType(DRIVE_REMOTE);
				m_pNewDrive->SetVolume(sPath);
				if (m_pProfile)
				{
					m_pNewDrive->Save(*m_pProfile);
				}
				if (!m_sRoot.IsEmpty())
				{
					m_pNewDrive->DisconnectNetworkDrive();
				}
				if (m_pNewDrive->ConnectNetworkDrive())
				{
					EndDialog(IDOK);
				}
				else
				{
					CString sFmt, sError = GetLastErrorString();
					sFmt.LoadString(AFX_IDS_INVALID_ARG_EXCEPTION);
					sFmt += _T("\n") + sError;
					AfxMessageBox(sFmt, MB_OK|MB_ICONERROR);
					m_pNewDrive->SetType(DRIVE_DELETED);
					if (m_pProfile)
					{
						m_pNewDrive->Save(*m_pProfile);
					}
				}
			}
			else
			{
				CString sCmd, sLetter;
				sCmd.Format(_T("net use %s \"%s\""), sLetter, m_sPath);
				ExecuteProgram(sCmd, NULL, FALSE);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDlgMapNetworkDrive::OnChange()
{
	GetDlgItem(IDOK)->EnableWindow(TRUE);
}
/////////////////////////////////////////////////////////////////////////////
CIPCDrive*CDlgMapNetworkDrive::GetNewDrive(BOOL bDetach/*=FALSE*/)
{
	CIPCDrive*pDrive = m_pNewDrive;
	if (bDetach)
	{
		m_pNewDrive = NULL;
	}
	return pDrive;
}
/////////////////////////////////////////////////////////////////////////////
