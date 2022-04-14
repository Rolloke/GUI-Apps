// CopyReadOnlyVersion.cpp: implementation of the CCopyReadOnlyVersion class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DVClient.h"
#include "CopyReadOnlyVersion.h"
#include "MainFrame.h"
#include "BackupWindow.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static _TCHAR szStorage[] = _T("dvstorage.exe");
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCopyReadOnlyVersion::CCopyReadOnlyVersion(const CString& sPath, 
										   int iOem, 
										   int nCameras,
										   const CString& sLanguage,
										   BOOL bOverWrite)
: CWK_Thread(_T("CopyReadOnlyVersionThread"),NULL)
{
	m_sPath = sPath;
	m_iOem = iOem;
	m_nCameras = nCameras;
	m_sLanguage = sLanguage;
	m_bOverWrite = bOverWrite;
	
	//file for normal and unicode version
	m_saSourceFiles.Add(szStorage);
	m_saSourceFiles.Add(_T("dvclient.exe"));
	m_saSourceFiles.Add(_T("oem.jpg"));
	m_saSourceFiles.Add(_T("ijl15.dll"));
#if _MFC_VER < 0x0700
	m_saSourceFiles.Add(_T("mfc42.dll"));
	m_saSourceFiles.Add(_T("msvcrt.dll"));
#endif
	m_saSourceFiles.Add(_T("autorun.inf"));
	m_saSourceFiles.Add(_T("JView.exe"));
	
	//add audio files
	m_saSourceFiles.Add(_T("audiounit.exe"));
	m_saSourceFiles.Add(_T("pushsource.ax"));

	m_saSourceFiles.Add(_T("mp4spvd_all.dll"));
#ifdef _UNICODE
	m_saSourceFiles.Add(_T("cipcU.dll"));
	m_saSourceFiles.Add(_T("skinsU.dll"));
	m_saSourceFiles.Add(_T("oemguiU.dll"));
	m_saSourceFiles.Add(_T("devicedetectU.dll"));
	m_saSourceFiles.Add(_T("commondirectshowU.dll"));
	m_saSourceFiles.Add(_T("vimageU.dll"));
	m_saSourceFiles.Add(_T("mfc71u.dll"));
	m_saSourceFiles.Add(_T("msvcr71.dll"));
	m_saSourceFiles.Add(_T("msvcp71.dll"));
	m_saSourceFiles.Add(_T("mp4spvd_all.dll"));
	m_saSourceFiles.Add(_T("oemguideu.dll"));
	m_saSourceFiles.Add(_T("oemguienu.dll"));
#else
	m_saSourceFiles.Add(_T("cipc.dll"));
	m_saSourceFiles.Add(_T("skins.dll"));
	m_saSourceFiles.Add(_T("oemgui.dll"));
	m_saSourceFiles.Add(_T("devicedetect.dll"));
	m_saSourceFiles.Add(_T("commondirectshow.dll"));
	m_saSourceFiles.Add(_T("vimage.dll"));
#endif



	// Quell Verzeichnis
#ifdef _DEBUG
	GetCurrentDirectory(_MAX_PATH,m_sSourceDir.GetBuffer(_MAX_PATH));
	m_sSourceDir.ReleaseBuffer();
#else
	GetModuleFileName(theApp.m_hInstance,
					  m_sSourceDir.GetBuffer(_MAX_PATH),
					  _MAX_PATH);
	m_sSourceDir.ReleaseBuffer();
	int p = m_sSourceDir.ReverseFind(_T('\\'));
	m_sSourceDir = m_sSourceDir.Left(p);
#endif

	// add all language dll's
	WK_SearchFiles(m_saSourceFiles,m_sSourceDir,_T("dv*.dll"));
	WK_SearchFiles(m_saSourceFiles,m_sSourceDir,_T("oemgui???.dll"));

	GetSystemDirectory(m_sWindowsSystemDir.GetBuffer(_MAX_PATH),
					  _MAX_PATH);
	m_sWindowsSystemDir.ReleaseBuffer();
	
	GetWindowsDirectory(m_sWindowsDir.GetBuffer(_MAX_PATH),
					  _MAX_PATH);
	m_sWindowsDir.ReleaseBuffer();
}
//////////////////////////////////////////////////////////////////////
CCopyReadOnlyVersion::~CCopyReadOnlyVersion()
{

}
//////////////////////////////////////////////////////////////////////
BOOL CCopyReadOnlyVersion::IsAlreadyDone(CString sDestinationPath)
{
	BOOL bRet = FALSE;
	if (!m_bOverWrite)
	{
		CString s;
		s = sDestinationPath + szStorage;
		WK_TRACE(_T("*** TKR Path: %s\n"), s);
		bRet = DoesFileExist(s);
	}
	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CCopyReadOnlyVersion::Run(LPVOID lpContext)
{
	CreateAutoRunInf();
	CopyFiles();
	DeleteFile(m_sAutoRunInf);

	return FALSE; // no longer run
}
//////////////////////////////////////////////////////////////////////
BOOL CCopyReadOnlyVersion::CreateAutoRunInf()
{
	BOOL bRet = TRUE;
	CStdioFile file;

	CString sTempFile;
	CString sLine; 

	sTempFile = m_sWindowsDir + _T("\\autorun.inf");

	if (file.Open(sTempFile,CFile::modeCreate|CFile::modeWrite))
	{
		TRY
		{
			file.WriteString(_T("[autorun]\n"));
			sLine = CString(_T("open=")) + szStorage + _T("\n");
			file.WriteString(sLine);
			sLine = CString(_T("icon=")) + szStorage + _T("\n");
			file.WriteString(sLine);
			file.WriteString(_T("[settings]\n"));
			sLine.Format(_T("OEM=%d\n"),m_iOem);
			file.WriteString(sLine);
			sLine.Format(_T("NrOfCam=%d\n"),m_nCameras);
			file.WriteString(sLine);
			sLine.Format(_T("Language=%s\n"),m_sLanguage);
			file.WriteString(sLine);
			file.Flush();
			file.Close();
		}
		CATCH(CFileException , e)
		{
			WK_TRACE_ERROR(_T("cannot write autorun.inf %s\n"),GetLastErrorString(e->m_lOsError));
			bRet = FALSE;
		}
		END_CATCH
	}

	m_sAutoRunInf = sTempFile;

	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CCopyReadOnlyVersion::CopyFiles()
{
	CString sSource,sSourcePath,sDestinationPath;
	BOOL bRet = TRUE;
	int i,c;

	c = m_saSourceFiles.GetSize();
	for (i=0;i<c;i++) 
	{
		sSource = m_saSourceFiles.GetAt(i);

#ifdef _DEBUG
		// aktuelles Verzeichnis
		if(sSource == _T("dvclient.exe"))
		{
			sSourcePath = m_sSourceDir + _T("\\Debug\\") + sSource;
		}
		else
		{
//#ifdef _UNICODE
//			sSourcePath = _T("C:\\DVX\\") + sSource;
//#else
			sSourcePath = _T("C:\\DV\\") + sSource;
//#endif
		}
#else
		// aktuelles Verzeichnis
		sSourcePath = m_sSourceDir + _T('\\') + sSource;
#endif

		if (!DoesFileExist(sSourcePath))
		{
			sSourcePath = m_sSourceDir + _T("\\dlls\\") + sSource;
		}
		if (!DoesFileExist(sSourcePath))
		{
			sSourcePath = m_sWindowsDir + _T('\\') + sSource;
		}
		if (!DoesFileExist(sSourcePath))
		{
			sSourcePath = m_sWindowsSystemDir + _T('\\') + sSource;
		}
		if (!DoesFileExist(sSourcePath))
		{
			continue;
		}
		if (0==sSource.CompareNoCase(_T("mp4spvd_all.dll")))
		{
			sSource = _T("mp4spvd.dll");
		}

		if (!DoesFileExist(m_sPath))
		{
			if (!CopyFile(sSourcePath,m_sPath+sSource,!m_bOverWrite))
			{
				WK_TRACE(_T("cannot copy %s to %s, because %s"),
					sSourcePath,
					m_sPath,
					GetLastErrorString());
				bRet = FALSE;
			}
			else
			{
				WK_TRACE(_T("copy %s to %s\n"),sSourcePath,sDestinationPath);
			}
		}
	} // for 

	return bRet;
}

//////////////////////////////////////////////////////////////////////
DWORD CCopyReadOnlyVersion::GetBytesReadOnlySoftware()
{
	DWORD dwSize = 0;

	CString sSource,sSourcePath,sDestinationPath;

	for(int i = 0; i < m_saSourceFiles.GetSize(); i++)
	{
		sSource = m_saSourceFiles.GetAt(i);
		// aktuelles Verzeichnis
		sSourcePath = m_sSourceDir + _T('\\') + sSource;

		if (!DoesFileExist(sSourcePath))
		{
			sSourcePath = m_sSourceDir + _T("\\dlls\\") + sSource;
		}
		if (!DoesFileExist(sSourcePath))
		{
			sSourcePath = m_sWindowsDir + _T('\\') + sSource;
		}
		if (!DoesFileExist(sSourcePath))
		{
			sSourcePath = m_sWindowsSystemDir + _T('\\') + sSource;
		}
		if (!DoesFileExist(sSourcePath))
		{
			continue;
		}

		if(DoesFileExist(sSourcePath))
		{
			TRACE(_T("Source: %s\n"), sSourcePath);
			CFile File(sSourcePath, CFile::modeRead);
			dwSize += (DWORD)File.GetLength();
		}
	}

	if(DoesFileExist(m_sAutoRunInf))
	{
		CFile File(m_sAutoRunInf, CFile::modeRead);
		dwSize += (DWORD)File.GetLength();
	}

	return dwSize;
}

//////////////////////////////////////////////////////////////////////
void CCopyReadOnlyVersion::GetReadOnlyFiles(CStringArray& saFiles)
{
	for(int i=0; i < m_saSourceFiles.GetSize(); i++)
	{
		saFiles.Add(m_saSourceFiles.GetAt(i));
	}
}
