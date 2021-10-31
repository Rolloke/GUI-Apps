// CopyFolder.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "ETS3DGLDLG.h"
#include "CopyFolder.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CCopyFolder 

#define COPY_OPTIONS "CopyOptions"
#define SOURCE       "Source"
#define DESTINATION  "Destination"
#define EXCLUDES     "Excludes"
#define INCLUDES     "Includes%d"
#define EXCLUDE      "Exclude"
#define INCLUDE      "Include"

#define CREATEDIRECTORYS      0
#define COPY_IN_ONE_DIRECTORY 1

CCopyFolder::CCopyFolder(CWnd* pParent /*=NULL*/)
	: CDialog(CCopyFolder::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCopyFolder)
	m_bOnlyNewerFiles = TRUE;
	//}}AFX_DATA_INIT

   m_nDirOptions   = CREATEDIRECTORYS;
   m_nIncludes     = 0;
   m_ppszIncludes  = NULL;
   m_hIcon         = AfxGetApp()->LoadIcon(IDR_MAINFRAME5);
   m_nLogTextLimit = 0;
   m_hThread       = NULL;
   m_nThreadID     = 0;
   m_nSel          = CB_ERR;
   m_bChanged      = false;
}

CCopyFolder::~CCopyFolder()
{
	if (m_hIcon != INVALID_HANDLE_VALUE)
      ::DestroyIcon(m_hIcon);
   DeleteStringArrays();
   pGetFileAttributesEx = NULL;
}

void CCopyFolder::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCopyFolder)
	DDX_Control(pDX, IDC_CPF_INCLUDES, m_cIncludes);
	DDX_Control(pDX, IDC_LOG_WINDOW, m_cLogWindow);
	DDX_Text(pDX, IDC_CPF_DESTINATION_PATH, m_strDestinatonPath);
	DDX_Text(pDX, IDC_CPF_INCLUDES, m_strIncludes);
	DDX_Text(pDX, IDC_CPF_SOURCE_PATH, m_strSourcePath);
	DDX_Check(pDX, IDC_CPF_COPY_ONLY_NEWER, m_bOnlyNewerFiles);
	DDX_Radio(pDX, IDC_CPF_COPY_CREATE_DIR, m_nDirOptions);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCopyFolder, CDialog)
	//{{AFX_MSG_MAP(CCopyFolder)
	ON_BN_CLICKED(IDC_CPF_SEARCH_DESTINATION_PATH, OnCpfSearchDestinationPath)
	ON_BN_CLICKED(IDC_CPF_SEARCH_SOURCE_PATH, OnCpfSearchSourcePath)
	ON_BN_CLICKED(IDC_COPY, OnCopy)
	ON_CBN_SELCHANGE(IDC_CPF_INCLUDES, OnSelchangeCpfIncludes)
	ON_CBN_KILLFOCUS(IDC_CPF_INCLUDES, OnKillfocusCpfIncludes)
	ON_CBN_EDITCHANGE(IDC_CPF_INCLUDES, OnEditchangeCpfIncludes)
	ON_EN_KILLFOCUS(IDC_CPF_SOURCE_PATH, OnKillfocusCpfSourcePath)
	ON_EN_KILLFOCUS(IDC_CPF_DESTINATION_PATH, OnKillfocusCpfDestinationPath)
	ON_BN_CLICKED(IDC_BTN_FILES, OnBtnFiles)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CCopyFolder 

void CCopyFolder::TerminateCopyThread()
{
   if (m_hThread)
   {
      DWORD dwExitCode;
      while (GetExitCodeThread(m_hThread, &dwExitCode) && (dwExitCode == STILL_ACTIVE))
      {
         TerminateThread(m_hThread, 0xffffffff);
         Sleep(100);
      }
      CloseHandle(m_hThread);
      m_hThread = NULL;
   }
}

void CCopyFolder::OnCancel() 
{
   TerminateCopyThread();
	CDialog::OnCancel();
}

void CCopyFolder::OnOK() 
{
   if (UpdateData(true))
   {
      CETS3DGLDLGApp *pApp = (CETS3DGLDLGApp*) AfxGetApp();
      CString str, strInclude;
      int i, nCount = m_cIncludes.GetCount();
	   pApp->WriteProfileString(COPY_OPTIONS, SOURCE, m_strSourcePath);
      pApp->WriteProfileString(COPY_OPTIONS, DESTINATION, m_strDestinatonPath);

      for (i=0; i<nCount; i++)
      {
         str.Format(INCLUDES, i+1);
         m_cIncludes.GetLBText(i, strInclude);
         if (strInclude == _T("*.*")) break;
         pApp->WriteProfileString(COPY_OPTIONS, str, strInclude);
      }

      if (!m_strIncludes.IsEmpty())
         m_strIncludes.MakeLower();

      TerminateCopyThread();
   	CDialog::OnOK();
   }
}

void CCopyFolder::OnCpfSearchDestinationPath() 
{
   CETS3DGLDLGApp *pApp = (CETS3DGLDLGApp*) AfxGetApp();
   if (pApp->GetFolderPath(m_strDestinatonPath, NULL, FOLDERPATH_CONCAT_SLASH))
   {
      CDataExchange dx(this, false);
   	DDX_Text(&dx, IDC_CPF_DESTINATION_PATH, m_strDestinatonPath);
   }
}

void CCopyFolder::OnCpfSearchSourcePath() 
{
   CETS3DGLDLGApp *pApp = (CETS3DGLDLGApp*) AfxGetApp();
   if (pApp->GetFolderPath(m_strSourcePath, NULL, FOLDERPATH_CONCAT_SLASH))
   {
      CDataExchange dx(this, false);
	   DDX_Text(&dx, IDC_CPF_SOURCE_PATH, m_strSourcePath);
   }
}

BOOL CCopyFolder::OnInitDialog() 
{
   CETS3DGLDLGApp *pApp = (CETS3DGLDLGApp*) AfxGetApp();
   int i;
   CString strInclude, str;
   char szAppName[64];
   strcpy(szAppName, pApp->m_pszExeName);
   strcat(szAppName, ".exe");

   if (pApp->SetProfileName(szAppName, (char*)pApp->m_pszExeName))
   {
	   m_strSourcePath     = pApp->GetProfileString(COPY_OPTIONS, SOURCE, NULL);
      m_strDestinatonPath = pApp->GetProfileString(COPY_OPTIONS, DESTINATION, NULL);
   }
	CDialog::OnInitDialog();
   HMODULE hModule;
   hModule = GetModuleHandle("KERNEL32.DLL");
   if (hModule)
   {
      pGetFileAttributesEx = (BOOL (__stdcall*)(LPCSTR, GET_FILEEX_INFO_LEVELS, LPVOID)) GetProcAddress(hModule,"GetFileAttributesExA");
      if (pGetFileAttributesEx)
      {
         GetDlgItem(IDC_CPF_COPY_ONLY_NEWER)->ShowWindow(SW_SHOW);
      }
      else
      {
         m_bOnlyNewerFiles = false;
      }
   }

	SetIcon(m_hIcon, TRUE);		// Kleines Symbol verwenden
   m_cLogWindow.SendMessage(EM_SETLIMITTEXT, 0xFFFF);
	m_nLogTextLimit = m_cLogWindow.SendMessage(EM_GETLIMITTEXT, 0, 0); 

   for (i=1; ; i++)
   {
      str.Format(INCLUDES, i);
      strInclude = pApp->GetProfileString(COPY_OPTIONS, str, NULL);
      if (strInclude.IsEmpty())
      {
         m_cIncludes.AddString("*.*");
         break;
      }
      m_cIncludes.AddString(strInclude);
	   if (i==1) m_strIncludes = strInclude;
   }
   if (m_cIncludes.GetCount())
   {
      m_cIncludes.SetCurSel(0);
      m_nSel = 0;
   }
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CCopyFolder::OnCopy() 
{
   if (UpdateData(true))
   {
      if (m_strSourcePath.Find(".ini") != -1)
      {
         EnableControls(false);
         m_hThread = (HANDLE) _beginthreadex(NULL, 0, CCopyFolder::CopyIniControlledS, (void*)this, 0, &m_nThreadID);
      }
      else
      {
         EnableControls(false);
         m_hThread = (HANDLE) _beginthreadex(NULL, 0, CCopyFolder::CopyFilesS, (void*)this, 0, &m_nThreadID);
      }
   }
}

void CCopyFolder::EnumFiles(char *pszFilePath)
{
   WIN32_FIND_DATA   w32fd;
   bool bFirst = true;
   HANDLE hFindFile = FindFirstFile(pszFilePath, &w32fd);
   char *pstr = strstr(pszFilePath, "*.*");
   if (pstr) pstr[0] = 0;
   if (hFindFile != INVALID_HANDLE_VALUE)
   {
//      TRACE("%s\n", pszFilePath);
      do
      {
         if (w32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
         {
            if (strcmp(w32fd.cFileName, "." )==0) continue;
            if (strcmp(w32fd.cFileName, "..")==0) continue;
            strcat(pszFilePath, w32fd.cFileName);
            strcat(pszFilePath, "\\*.*");
            int nLength = 0;
            if (m_nDirOptions == CREATEDIRECTORYS)
            {
               nLength = m_strDestinatonPath.GetLength();
               m_strDestinatonPath += _T(w32fd.cFileName);
               m_strDestinatonPath += '\\';
            }
            EnumFiles(pszFilePath);
            if (m_nDirOptions == CREATEDIRECTORYS)
               m_strDestinatonPath = m_strDestinatonPath.Left(nLength);
            if (pstr) pstr[0] = 0;
         }
         else
         {
            CString strDestination;
            char szFileName[_MAX_PATH];
            for (int i=0; i<m_nIncludes; i++)
            {
               int nPos = 0, nSize = 1;
               bool bCopy = false;
               strncpy(szFileName, w32fd.cFileName, _MAX_PATH);
               strlwr(szFileName);
               char *str = strstr(m_ppszIncludes[i], "*.*");   // alle Dateien ?
               if (str) bCopy = true;                          // ja
               else
               {
                  str = strstr(m_ppszIncludes[i], "*");        // Wildcard ?
                  if (str)                                     // ja, wo ?
                  {
                     nPos = m_ppszIncludes[i] - str;
                     if (nPos == 0)                            // am Anfang
                     {
                        str = strstr(szFileName, &m_ppszIncludes[i][1]);
                        if (str && (strcmp(str, &m_ppszIncludes[i][1]) ==0)) bCopy = true;
                     }
                     else                                      // weitere ? mal sehen
                     {

                     }
                  }
                  else                                         // bestimmte Datei ?
                  {
                     str = strstr(szFileName, m_ppszIncludes[i]);
                     if (str) bCopy = true;                    // Ok
                  }
               }

//               if (((m_ppszIncludes[i][0] == '.') && (m_ppszIncludes[i][1] == '*')) ||
//                   (str && (strncmp(str, &m_ppszIncludes[i][nPos], nSize) ==0)))
               if (bCopy)
               {
                  SetLastError(0);
                  BOOL bReturn = CreateDirectoryAndSubs((char*)LPCTSTR(m_strDestinatonPath));
                  if (bReturn)
                  {
                     strDestination.Format(IDS_DIR_CREATED, m_strDestinatonPath);
                     OutputLineToLogWnd(strDestination);
                     bFirst = false;
                  }
                  else
                  {
                     DWORD dwError = GetLastError();
                     if (dwError != ERROR_ALREADY_EXISTS)
                     {
                        if (bFirst)
                        {
                           if (m_nDirOptions == COPY_IN_ONE_DIRECTORY)
                              OutputLineToLogWnd(pszFilePath);
                           else
                              OutputLineToLogWnd(m_strDestinatonPath);

                           bFirst = false;
                        }
                     }
                     else if (dwError != 0)
                     {
                        CString str(strerror(dwError));
                        OutputLineToLogWnd(str);
                     }
                  }
                  strcat(pszFilePath, w32fd.cFileName);
                  strDestination.Format("%s%s", m_strDestinatonPath, w32fd.cFileName);
 
                  if (m_bOnlyNewerFiles)
                  {
                     WIN32_FILE_ATTRIBUTE_DATA faSrc, faDest;
                     if (pGetFileAttributesEx(pszFilePath, GetFileExInfoStandard, (void*)&faSrc) &&
                         pGetFileAttributesEx(strDestination, GetFileExInfoStandard, (void*)&faDest))
                     {
                        if (CompareFileTime(&faSrc.ftLastWriteTime, &faDest.ftLastWriteTime) < 1) // Source ist älter
                        {
                           strDestination.Format(IDS_FILE_OLDER, w32fd.cFileName);
                           OutputLineToLogWnd(strDestination);
                           if (pstr) pstr[0] = 0;
                           continue;
                        }
                     }
                  }                  
                  bReturn = CopyFile(pszFilePath, strDestination, false);
                  if (bReturn)
                  {
                     strDestination.Format(IDS_FILE_COPIED, w32fd.cFileName);
                     OutputLineToLogWnd(strDestination);
                  }
                  else
                  {
                     DWORD dwError = GetLastError();
                     CString str(strerror(dwError));
                     OutputLineToLogWnd(str);
                  }
                  if (pstr) pstr[0] = 0;
                  break;
               }
            }
         }
      }
      while (FindNextFile(hFindFile, &w32fd));
      FindClose(hFindFile);
   }
   if (pstr) pstr[0] = 0;
}

void CCopyFolder::DeleteStringArrays()
{
   if (m_ppszIncludes)
   {
      for (int i=0; i<m_nIncludes; i++)
      {
         if (m_ppszIncludes[i]) free(m_ppszIncludes[i]);
      }
      free(m_ppszIncludes);
      m_ppszIncludes = NULL;
   }
   m_nIncludes = 0;
}

void CCopyFolder::MakeStringArrays()
{
   if (!m_strIncludes.IsEmpty())
   {
      m_ppszIncludes = (char**) malloc(sizeof(char*));
      if (!m_ppszIncludes)
         return;
      m_ppszIncludes[0] = NULL;
      const char*pszWildCard = LPCTSTR(m_strIncludes);
      char *pszSpace, cOld;
      for (m_nIncludes=1; pszWildCard!=NULL; m_nIncludes++)
      {
         for ( ;(*pszWildCard != 0) && (*pszWildCard == ' '); pszWildCard++);
//         pszWildCard = strstr(pszWildCard, "*.");
//         if (pszWildCard)
         if (*pszWildCard)
         {
            pszSpace = strstr(pszWildCard, ",");
            if (pszSpace)
            {
               cOld = pszSpace[0];
               pszSpace[0] = 0;
            }
            if (m_nIncludes > 1)
            {
               m_ppszIncludes = (char**) realloc(m_ppszIncludes, sizeof(char*)*(m_nIncludes));
               if (!m_ppszIncludes) return;
            }
            m_ppszIncludes[m_nIncludes-1] = _strdup(pszWildCard);
         }
         if (pszSpace)
         {
            pszSpace[0] = cOld;
            pszWildCard = &pszSpace[1];
         }
         else break;
      }
      
   }
}

BOOL CCopyFolder::CreateDirectoryAndSubs(char *pszPath)
{
   BOOL bReturn = 0;
   if (pszPath)
   {
      int  i, nLen  = strlen(pszPath);
      if (nLen            == 0   ) return 0;
      char cTemp = 0;
      if (GetFileAttributes(pszPath) != -1) return 0;

      for (i=nLen-2; i>=0; i--)
      {
         if (pszPath[i] == '\\') break;
      }
      if (i < 0) return 0;
      i++;
      cTemp = pszPath[i];
      pszPath[i] = 0;
      if (GetFileAttributes(pszPath) == -1)
      {
         bReturn = CreateDirectoryAndSubs(pszPath);
      }
      else bReturn = 1;

      if (cTemp) pszPath[i] = cTemp;

      if (bReturn)
      {
         bReturn = CreateDirectory(pszPath, NULL);
      }
   }
   return bReturn;
}

void CCopyFolder::OutputLineToLogWnd(LPCTSTR str)
{
   static const char linefeed[] = "\r\n";
   int nLen = m_cLogWindow.GetWindowTextLength();
   if (nLen > m_nLogTextLimit) m_cLogWindow.SetWindowText(NULL);
   m_cLogWindow.SendMessage(EM_REPLACESEL, 0, (LPARAM)LPCTSTR(str));
   m_cLogWindow.SendMessage(EM_REPLACESEL, 0, (LPARAM)linefeed);
}

unsigned int __stdcall CCopyFolder::CopyFilesS(void *p)
{
   CCopyFolder *pThis = (CCopyFolder*) p;
   return pThis->CopyFiles();
}

unsigned int CCopyFolder::CopyFiles()
{
   char szFiles[_MAX_PATH*2];
   MakeStringArrays();
   m_cLogWindow.SetWindowText(NULL);
   strcpy(szFiles, LPCTSTR(m_strSourcePath));
   strcat(szFiles, "*.*");
   if (m_strDestinatonPath.GetAt(m_strDestinatonPath.GetLength()-1) != '\\')
      m_strDestinatonPath += '\\';
   EnumFiles(szFiles);
   DeleteStringArrays();
   EnableControls(true);
   CloseHandle(m_hThread);
   m_hThread   = NULL;
   m_nThreadID = 0;
   return 0;
}

unsigned int __stdcall CCopyFolder::CopyIniControlledS(void *p)
{
   CCopyFolder *pThis = (CCopyFolder*) p;
   return pThis->CopyIniControlled();
}

unsigned int CCopyFolder::CopyIniControlled()
{
   CETS3DGLDLGApp *pApp = (CETS3DGLDLGApp*) AfxGetApp();
   const char *pszRegKey      = pApp->m_pszRegistryKey;
   const char *pszProfileName = pApp->m_pszProfileName;
   pApp->m_pszRegistryKey = NULL;
   pApp->m_pszProfileName = m_strSourcePath;

   CString strSrc, strDest, strEnum;
   int i, bReturn;
   for (i=1;;i++)
   {
      strEnum.Format("%ds", i);
	   strSrc  = pApp->GetProfileString("Copy", strEnum, NULL);
      strEnum.Format("%dd", i);
      strDest = pApp->GetProfileString("Copy", strEnum, NULL);
      if (strSrc.IsEmpty()) break;
      if (strDest.IsEmpty()) break;

      bReturn = CopyFile(strSrc, strDest, false);
      if (bReturn)
      {
         strDest.Format(IDS_FILE_COPIED, strSrc);
         OutputLineToLogWnd(strDest);
      }
      else
      {
         DWORD dwError = GetLastError();
         CString strDest(strerror(dwError));
         strDest += strEnum;
         OutputLineToLogWnd(strDest);
      }
   }
   pApp->m_pszRegistryKey = pszRegKey;
   pApp->m_pszProfileName = pszProfileName;

   EnableControls(true);
   CloseHandle(m_hThread);
   m_hThread   = NULL;
   m_nThreadID = 0;
   return 0;
}

void CCopyFolder::EnableControls(bool bEnable)
{
   ::EnableWindow(::GetDlgItem(m_hWnd,IDC_COPY), bEnable);
   ::EnableWindow(::GetDlgItem(m_hWnd,IDOK), bEnable);
   ::EnableWindow(::GetDlgItem(m_hWnd,IDC_CPF_SEARCH_DESTINATION_PATH), bEnable);
   ::EnableWindow(::GetDlgItem(m_hWnd,IDC_CPF_SEARCH_SOURCE_PATH), bEnable);
   ::EnableWindow(::GetDlgItem(m_hWnd,IDC_CPF_DESTINATION_PATH), bEnable);
   ::EnableWindow(::GetDlgItem(m_hWnd,IDC_CPF_INCLUDES), bEnable);
   ::EnableWindow(::GetDlgItem(m_hWnd,IDC_CPF_SOURCE_PATH), bEnable);
   ::EnableWindow(::GetDlgItem(m_hWnd,IDC_BTN_FILES), bEnable);
   ::EnableWindow(::GetDlgItem(m_hWnd,IDC_CPF_COPY_ONLY_NEWER), bEnable);
   ::EnableWindow(::GetDlgItem(m_hWnd,IDC_CPF_COPY_CREATE_DIR), bEnable);
   ::EnableWindow(::GetDlgItem(m_hWnd,IDC_CPF_COPY_IN_ONE_DIR), bEnable);
}


void CCopyFolder::OnSelchangeCpfIncludes() 
{
   m_nSel = m_cIncludes.GetCurSel();
   if (m_nSel != CB_ERR)
   {
      m_cIncludes.GetLBText(m_nSel, m_strIncludes);
      CDataExchange dx(this, false);
	   DDX_Text(&dx, IDC_CPF_INCLUDES, m_strIncludes);
   }
}

void CCopyFolder::OnKillfocusCpfIncludes() 
{
   if (m_bChanged)
   {
      CDataExchange dx(this, true);
	   DDX_Text(&dx, IDC_CPF_INCLUDES, m_strIncludes);
      if (m_nSel != CB_ERR)
      {
         m_cIncludes.DeleteString(m_nSel);
         m_cIncludes.InsertString(m_nSel, m_strIncludes);
         m_bChanged = false;
      }
   }
}

void CCopyFolder::OnEditchangeCpfIncludes() 
{
   m_bChanged = true;
}

void CCopyFolder::OnKillfocusCpfSourcePath() 
{
   CWnd *pWnd = GetDlgItem(IDC_CPF_SOURCE_PATH);
   if ((pWnd!=NULL) && pWnd->SendMessage(EM_GETMODIFY))
   {
      CDataExchange dx(this, true);
   	DDX_Text(&dx, IDC_CPF_SOURCE_PATH, m_strSourcePath);
      pWnd->SendMessage(EM_SETMODIFY);
   }
}

void CCopyFolder::OnKillfocusCpfDestinationPath() 
{
   CWnd *pWnd = GetDlgItem(IDC_CPF_DESTINATION_PATH);
   if ((pWnd!=NULL) && pWnd->SendMessage(EM_GETMODIFY))
   {
      CDataExchange dx(this, true);
   	DDX_Text(&dx, IDC_CPF_DESTINATION_PATH, m_strDestinatonPath);
      pWnd->SendMessage(EM_SETMODIFY);
   }
}

void CCopyFolder::OnBtnFiles() 
{
   CETS3DGLDLGApp *pApp = (CETS3DGLDLGApp*) AfxGetApp();
   if (pApp->GetFolderPath(m_strSourcePath, NULL, FOLDERPATH_RETURN_FILES))
   {
      CDataExchange dx(this, false);
	   DDX_Text(&dx, IDC_CPF_SOURCE_PATH, m_strSourcePath);
   }
}
