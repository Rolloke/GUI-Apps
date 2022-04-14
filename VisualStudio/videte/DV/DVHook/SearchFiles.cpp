// SearchFiles.cpp : implementation file
//

#include "stdafx.h"
#include "dvhook.h"
#include "SearchFiles.h"
#include "..\LangDll\resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSearchFiles dialog


CSearchFiles::CSearchFiles(CWnd* pParent /*=NULL*/)
	: CDialog(CSearchFiles::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSearchFiles)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pTempMain = NULL;
}


void CSearchFiles::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSearchFiles)
	DDX_Control(pDX, IDC_LIST_FOUND_FILES, m_ListFiles);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSearchFiles, CDialog)
	//{{AFX_MSG_MAP(CSearchFiles)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSearchFiles message handlers

void CSearchFiles::OnOK() 
{
	int nSel = m_ListFiles.GetSelectionMark();

	if (nSel == -1) nSel = 0;	// nichts ausgewählt ?, dann den ersten nehmen

	m_sSearch = m_ListFiles.GetItemText(nSel, 0);
	int nPos = m_sSearch.ReverseFind('\\');
	if (nPos != -1)
	{
		m_sSearch = m_sSearch.Left(nPos+1);
	}
	CDialog::OnOK();
}

BOOL CSearchFiles::OnInitDialog() 
{
	CString sText;
	CDialog::OnInitDialog();
	CDVHookApp *pApp = ((CDVHookApp*)AfxGetApp());
	if (m_pTempMain)
	{
		pApp->m_pMainWnd = m_pTempMain;
	}

	pApp->GetLanguageDll();

	if (sText.LoadString(IDS_OK))
		SetDlgItemText(IDOK, sText);
	if (sText.LoadString(IDS_CANCEL))
		SetDlgItemText(IDCANCEL, sText);
	CRect rc;

   m_ListFiles.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,
      LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT,
      LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_ListFiles.GetClientRect(&rc);
	CString sHeadline = _T("Dateien");
	LV_COLUMN lvc = {LVCF_TEXT|LVCF_WIDTH, 0, rc.right, (_TCHAR*)LPCTSTR(sHeadline), sHeadline.GetLength(), 0, 0, 0};
	m_ListFiles.InsertColumn(0, &lvc);

	CWK_Profile wkp(CWK_Profile::WKP_REGPATH,HKEY_LOCAL_MACHINE, _T("Microsoft\\Windows\\CurrentVersion\\"), _T(""));
	CStringArray sArr;
	CString      str;

	BOOL bResult = wkp.GetMultiString(_T("Setup"), _T("Installation Sources"), sArr);
	bResult = bResult; // to prevent waring 4189

	for (int i=0; i<sArr.GetSize(); i++)
	{
		str = sArr.GetAt(i);
		if (str.GetAt(str.GetLength()-1) != '\\')
		{
			str += _T("\\");
		}
		str +=  m_sSearch;
		FindFile(str);
	}
	int nCount = m_ListFiles.GetItemCount();
	if (nCount)
	{
		if (nCount == 1)
		{
			OnOK();
			return TRUE;
		}
		else
		{
			GetDlgItem(IDOK)->EnableWindow();
		}
	}
	else
	{
		OnCancel();
		return TRUE;
	}
	SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSearchFiles::FindFile(CString sSearch)
{
	HANDLE          hF = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA FindFileData;
	CString         sFile;
	int             nFilePos = sSearch.ReverseFind('\\');
	if (nFilePos == -1) return;

	sFile = sSearch.Right(sSearch.GetLength()-nFilePos);

	for (int i=0; i<2; i++)
	{
		if (i==0)		// Datei suchen
		{
			hF = FindFirstFile(LPCTSTR(sSearch), &FindFileData);
		}
		else				// Unterverzeichnisse
		{
			int nBSlash = sSearch.ReverseFind('\\');
			if (nBSlash != -1)
			{
				CString sSubDirs = sSearch.Left(nBSlash+1) + _T("*.*");
				hF = FindFirstFile(LPCTSTR(sSubDirs), &FindFileData);
			}
		}
		while (hF != INVALID_HANDLE_VALUE)
		{
			if ((_tcscmp(FindFileData.cFileName,_T("."))!=0) &&
				 (_tcscmp(FindFileData.cFileName,_T(".."))!=0))
			{
				if (FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
				{
					CString sSubDir = sSearch.Left(nFilePos+1) + FindFileData.cFileName + sFile;
					FindFile(sSubDir);
				}
				else if (i == 0)
				{
					int nItem = m_ListFiles.GetItemCount();
					CString sFilePath = sSearch.Left(nFilePos+1) + FindFileData.cFileName;
					m_ListFiles.InsertItem(nItem, sFilePath);
				}
			} 
			if (!FindNextFile(hF,&FindFileData))
			{
				break;
			}
		}
		if (hF != INVALID_HANDLE_VALUE)
		{
			FindClose(hF);
			hF = INVALID_HANDLE_VALUE;
		}
	}
}
