// AboutDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BootIniPatch.h"
#include "AboutDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}
/////////////////////////////////////////////////////////////////////////////
void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_TXT_COPYRIGHT, m_txtCopyright);
	DDX_Control(pDX, IDC_TXT_APPVERSION, m_txtAppVersion);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_BN_CLICKED(IDC_TXT_APPVERSION, OnTxtAppVersion)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	UpdateData();

	CString sExe;
	GetModuleFileName(AfxGetApp()->m_hInstance, sExe.GetBufferSetLength(_MAX_PATH), _MAX_PATH);
	sExe.ReleaseBuffer();
	CString sApp = AfxGetAppName();

	CString sFileVersion = GetFileVersion(sExe);
	CString sVersion = _T("Version ");
	if (sFileVersion.IsEmpty())
	{
		CString sText;
		m_txtAppVersion.GetWindowText(sText);
		int iIndex = sText.Find(sVersion);
		if (iIndex != -1)
		{
			sVersion += sText.Mid(iIndex);
		}
	}
	else
	{
		sVersion += sFileVersion;
	}
	m_txtAppVersion.SetWindowText(sApp + " " + sVersion);

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CAboutDlg::OnTxtAppVersion() 
{
	// TODO: Add your control notification handler code here
	CString sText;
	m_txtCopyright.GetWindowText(sText);

	if (sText.Find(_T("Georg Feddern")) == -1)
	{
		sText += _T(" by Georg Feddern");
		m_txtCopyright.SetWindowText(sText);
		UpdateData();
	}
}
/////////////////////////////////////////////////////////////////////////////
CString CAboutDlg::GetFileVersion(const CString& sFileName)
{
	CString sRet;
	CString sExe = sFileName;
	DWORD dwHandle=0;
	DWORD dwSize = GetFileVersionInfoSize(sExe.GetBuffer(0),&dwHandle);
	sExe.ReleaseBuffer();

	if (dwSize>0)
	{
		PBYTE pData = new BYTE[dwSize];
		if (GetFileVersionInfo(sExe.GetBuffer(0),0,dwSize,pData))
		{
			struct LANGANDCODEPAGE 
			{
				WORD wLanguage;
				WORD wCodePage;
			} *lpTranslate;
			UINT cbTranslate = 0;

			// Read the list of languages and code pages.

			VerQueryValue(pData, 
						  TEXT("\\VarFileInfo\\Translation"),
						  (LPVOID*)&lpTranslate,
						  &cbTranslate);

			UINT  dwValueLen = 0;
			void* pValueData;
			for (UINT i=0; i < (cbTranslate/sizeof(struct LANGANDCODEPAGE)); i++ )
			{
				CString sLanguage;
				sLanguage.Format(TEXT("\\StringFileInfo\\%04x%04x\\FileVersion"),
								lpTranslate[i].wLanguage,
								lpTranslate[i].wCodePage);
				
				// Retrieve file description for language and code page "i". 
				if (VerQueryValue(pData, 
								  sLanguage.GetBuffer(0), 
								  &pValueData, 
								  &dwValueLen))
				{
					sRet = LPCTSTR(pValueData);
					sLanguage.ReleaseBuffer();
					break;
				}
				else
				{
					sLanguage.ReleaseBuffer();
				}
			}
		}
		sExe.ReleaseBuffer();
		delete pData;
	}
	return sRet;
}
