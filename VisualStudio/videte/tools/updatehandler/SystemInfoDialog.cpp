// SystemInfoDialog.cpp : implementation file
//

#include "stdafx.h"
#include "updatehandler.h"
#include "SystemInfoDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSystemInfoDialog dialog


CSystemInfoDialog::CSystemInfoDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSystemInfoDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSystemInfoDialog)
	m_sVersion = _T("");
	m_sMemory = _T("");
	m_sProzessor = _T("");
	m_sSoftware = _T("");
	//}}AFX_DATA_INIT
}


void CSystemInfoDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSystemInfoDialog)
	DDX_Text(pDX, IDC_TXT_OSVERSION, m_sVersion);
	DDX_Text(pDX, IDC_TXT_MEMORY, m_sMemory);
	DDX_Text(pDX, IDC_TXT_PROZESSOR, m_sProzessor);
	DDX_Text(pDX, IDC_TXT_SOFTWARE, m_sSoftware);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSystemInfoDialog, CDialog)
	//{{AFX_MSG_MAP(CSystemInfoDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSystemInfoDialog message handlers

BOOL CSystemInfoDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	switch (m_SystemInfo.wProcessorArchitecture)
	{
	case PROCESSOR_ARCHITECTURE_INTEL:
		m_sProzessor = _T("Intel");
		break;
	case PROCESSOR_ARCHITECTURE_MIPS:
		m_sProzessor = _T("Mips");
		break;
	case PROCESSOR_ARCHITECTURE_ALPHA:
		m_sProzessor = _T("Alpha");
		break;
	case PROCESSOR_ARCHITECTURE_PPC:
		m_sProzessor = _T("PowerPC");
		break;
	default:
		m_sProzessor = _T("unbekannt");
	}
	
	switch (m_SystemInfo.dwProcessorType)
	{
	case PROCESSOR_INTEL_386:
		m_sProzessor += _T(" 386");
		break;
	case PROCESSOR_INTEL_486:
		m_sProzessor += _T(" 486");
		break;
	case PROCESSOR_INTEL_PENTIUM:
		m_sProzessor += _T(" Pentium");
		break;
	case PROCESSOR_MIPS_R4000:
		m_sProzessor += _T("R4000");
		break;
	case PROCESSOR_ALPHA_21064:
		m_sProzessor += _T(" 21064");
		break;
	default:
		m_sProzessor += _T(" unbekannt");
		break;
	}

	CString build;
	switch (m_OSVersionInfo.dwPlatformId)
	{
	case VER_PLATFORM_WIN32s:
		m_sVersion = _T("Win32s on Windows 3.1 ");
		build.Format(_T("%d"),m_OSVersionInfo.dwBuildNumber);
		break;
	case VER_PLATFORM_WIN32_WINDOWS:
		m_sVersion = _T("Windows 95 ");
		build.Format(_T("%d"),LOWORD(m_OSVersionInfo.dwBuildNumber));
		break;
	case VER_PLATFORM_WIN32_NT:
		m_sVersion = _T("Windows NT ");
		build.Format(_T("%d"),m_OSVersionInfo.dwBuildNumber);
		break;
	default:
		m_sVersion = _T("unbekannt ");
	}
	CString temp;
	temp.Format(_T("%d"),m_OSVersionInfo.dwMajorVersion);
	m_sVersion += temp + _T('.');
	temp.Format(_T("%d"),m_OSVersionInfo.dwMinorVersion);
	m_sVersion += temp + _T('.');
	m_sVersion += build;

	int i = m_MemoryStatus.dwTotalPhys / (1024 * 1024);
	if (1&i) i++;
	m_sMemory.Format(_T("%d MB RAM"),i);
	m_sSoftware = m_sSoftwareVersion;
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
