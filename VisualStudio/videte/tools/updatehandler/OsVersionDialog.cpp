// OsVersionDialog.cpp : implementation file
//

#include "stdafx.h"
#include "updatehandler.h"
#include "OsVersionDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COsVersionDialog dialog


COsVersionDialog::COsVersionDialog(CWnd* pParent /*=NULL*/)
	: CDialog(COsVersionDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(COsVersionDialog)
	m_Build = _T("");
	m_Major = _T("");
	m_Minor = _T("");
	m_Version = _T("");
	m_Platform = _T("");
	//}}AFX_DATA_INIT
}


void COsVersionDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COsVersionDialog)
	DDX_Text(pDX, IDC_TXT_BUILD, m_Build);
	DDX_Text(pDX, IDC_TXT_MAJOR, m_Major);
	DDX_Text(pDX, IDC_TXT_MINOR, m_Minor);
	DDX_Text(pDX, IDC_TXT_VERSION, m_Version);
	DDX_Text(pDX, IDC_TXT_PLATFORM, m_Platform);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COsVersionDialog, CDialog)
	//{{AFX_MSG_MAP(COsVersionDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COsVersionDialog message handlers

BOOL COsVersionDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_Version = m_OSVersionInfo.szCSDVersion;
	switch (m_OSVersionInfo.dwPlatformId)
	{
	case VER_PLATFORM_WIN32s:
		m_Platform = "Win32s on Windows 3.1.";
		break;
	case VER_PLATFORM_WIN32_WINDOWS:
		m_Platform = "Win32 on Windows 95.";
		m_Build.Format("%d",LOWORD(m_OSVersionInfo.dwBuildNumber));
		break;
	case VER_PLATFORM_WIN32_NT:
		m_Platform = "Win32 on Windows NT.";
		m_Build.Format("%d",m_OSVersionInfo.dwBuildNumber);
		break;
	default:
		m_Platform = "unbekannt";
	}
	m_Major.Format("%d",m_OSVersionInfo.dwMajorVersion);
	m_Minor.Format("%d",m_OSVersionInfo.dwMinorVersion);
	
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
