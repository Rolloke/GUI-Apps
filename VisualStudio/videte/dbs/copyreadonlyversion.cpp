// CopyReadOnlyVersion.cpp: implementation of the CCopyReadOnlyVersion class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dbs.h"
#include "CopyReadOnlyVersion.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCopyReadOnlyVersion::CCopyReadOnlyVersion(CString sDestDir)
{
	m_sDestinationDir = sDestDir;
	int p = m_sDestinationDir.ReverseFind(_T('\\'));
	if (p != m_sDestinationDir.GetAt(m_sDestinationDir.GetLength()-1))
	{
		m_sDestinationDir += _T('\\');
	}
	
	m_saSourceFiles.Add(_T("dbs.exe"));
#if _MFC_VER < 0x700
	m_saSourceFiles.Add(_T("recherche.exe"));
#else
	m_saSourceFiles.Add(_T("idipclient.exe"));
#endif

#ifdef _UNICODE
	m_saSourceFiles.Add(_T("cipcu.dll"));
	m_saSourceFiles.Add(_T("vimageu.dll"));
	m_saSourceFiles.Add(_T("oemguiu.dll"));
	m_saSourceFiles.Add(_T("skinsu.dll"));
#else
	m_saSourceFiles.Add(_T("cipc.dll"));
	m_saSourceFiles.Add(_T("vimage.dll"));
	m_saSourceFiles.Add(_T("oemgui.dll"));
	m_saSourceFiles.Add(_T("skins.dll"));
#endif
	
	m_saSourceFiles.Add(_T("mp4spvd_all.dll"));
	m_saSourceFiles.Add(_T("ijl15.dll"));

#if _MFC_VER < 0x700
	m_saSourceFiles.Add(_T("mfc42.dll"));
	m_saSourceFiles.Add(_T("msvcrt.dll"));
#else
	m_saSourceFiles.Add(_T("mfc71.dll"));
	m_saSourceFiles.Add(_T("mfc71u.dll"));
	m_saSourceFiles.Add(_T("msvcr71.dll"));
	m_saSourceFiles.Add(_T("msvcp71.dll"));
#endif
	
	m_saSourceFiles.Add(_T("autorun.inf"));
	m_saSourceFiles.Add(_T("oem.ini"));
	m_saSourceFiles.Add(_T("logo.bmp"));
	
	//Audio Components
	m_saSourceFiles.Add(_T("audiounit.exe"));
	m_saSourceFiles.Add(_T("pushsource.ax"));
#ifdef _UNICODE
	m_saSourceFiles.Add(_T("commondirectshowu.dll"));
	m_saSourceFiles.Add(_T("devicedetectu.dll"));
#else
	m_saSourceFiles.Add(_T("commondirectshow.dll"));
	m_saSourceFiles.Add(_T("DeviceDetect.dll"));
#endif


	// Quell Verzeichnis
	GetModuleFileName(AfxGetApp()->m_hInstance,
					  m_sSourceDir.GetBuffer(_MAX_PATH),
					  _MAX_PATH);
	m_sSourceDir.ReleaseBuffer();
	p = m_sSourceDir.ReverseFind(_T('\\'));
	m_sSourceDir = m_sSourceDir.Left(p);

	// add all language dll's
	WK_SearchFiles(m_saSourceFiles,m_sSourceDir,_T("oemgui???.dll"));
	WK_SearchFiles(m_saSourceFiles,m_sSourceDir,_T("idipclient???.dll"));

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
BOOL CCopyReadOnlyVersion::IsAlreadyDone()
{
	CString s;
	s = m_sDestinationDir + _T("dbs.exe");
	return DoesFileExist(s);
}
//////////////////////////////////////////////////////////////////////
BOOL CCopyReadOnlyVersion::Run(BOOL bCopy)
{
	if (bCopy)
	{
		CreateOemIni();
		CreateAutoRunInf();

		CopyFiles();

		DeleteFile(m_sOemIni);
		DeleteFile(m_sAutoRunInf);
	}
	else
	{
		DeleteFiles();
	}

	return FALSE; // no longer run
}
//////////////////////////////////////////////////////////////////////
BOOL CCopyReadOnlyVersion::CreateOemIni()
{
	BOOL bRet = TRUE;

	CString sTempFile;

	sTempFile = m_sWindowsDir + _T("\\oem.ini");

	CString sOEM,sWrite,sDest;
	CStdioFile file;
	CWK_Dongle dongle;
	sOEM.Format(_T("%d"),dongle.GetOemCode());

	if (file.Open(sTempFile,CFile::modeCreate|CFile::modeWrite))
	{
		TRY
		{
			file.WriteString(_T("[oem]\n"));
			sWrite.Format(_T("oem=%s\n"),sOEM);
			file.WriteString(sWrite);
			sWrite.Format(_T("version=%s\n"),dongle.GetVersionString());
			file.WriteString(sWrite);
			
			file.WriteString(_T("[Fields]\n"));
			CString rKey,rValue,s;
			CIPCFields& fields = theApp.m_Fields;

			for( int i=0;i<fields.GetSize(); i++)   
			{
				rKey = fields.GetAt(i)->GetName();
				rValue  = fields.GetAt(i)->GetValue();
				if (!rKey.IsEmpty() && !rValue.IsEmpty())
				{
					s.Format(_T("%s=%s\n"),rKey,rValue);
					file.WriteString(s);
				}
			}

			file.Flush();
			file.Close();
		}
		CATCH(CFileException , e)
		{
			bRet = FALSE;
		}
		END_CATCH
	}

	m_sOemIni = sTempFile;

	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CCopyReadOnlyVersion::CreateAutoRunInf()
{
	BOOL bRet = TRUE;
	CStdioFile file;

	CString sTempFile;

	sTempFile = m_sWindowsDir + _T("\\autorun.inf");

	if (file.Open(sTempFile,CFile::modeCreate|CFile::modeWrite))
	{
		TRY
		{
			file.WriteString(_T("[autorun]\n"));
			file.WriteString(_T("open=dbs.exe\n"));
			file.WriteString(_T("icon=dbs.exe\n"));
			file.Flush();
			file.Close();
		}
		CATCH(CFileException , e)
		{
			bRet = FALSE;
		}
		END_CATCH
	}

	m_sAutoRunInf = sTempFile;

	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CCopyReadOnlyVersion::DeleteFiles()
{
	CString sSource,sDestinationPath;
	BOOL bRet = TRUE;
	int i,c;

	c = m_saSourceFiles.GetSize();
	for (i=0;i<c;i++) 
	{
		sSource = m_saSourceFiles.GetAt(i);
		if (0==sSource.CompareNoCase(_T("mp4spvd_all.dll")))
		{
			sSource = _T("mp4spvd.dll");
		}
		sDestinationPath = m_sDestinationDir + sSource;

		if (DoesFileExist(sDestinationPath))
		{
			if (!DeleteFile(sDestinationPath))
			{
				WK_TRACE_ERROR(_T("cannot delete %s , because %s"),
								sDestinationPath,
								GetLastErrorString());
				bRet = FALSE;
			}
		}
	} // for 

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
			WK_TRACE_ERROR(_T("cannot copy %s to %s, because not found\n"),
							sSource,
							sDestinationPath);
			bRet = FALSE;
			continue;
		}

		if (0==sSource.CompareNoCase(_T("mp4spvd_all.dll")))
		{
			sSource = _T("mp4spvd.dll");
		}
		sDestinationPath = m_sDestinationDir + sSource;
		if (!DoesFileExist(sDestinationPath))
		{
			if (!CopyFile(sSourcePath,sDestinationPath,TRUE))
			{
				WK_TRACE_ERROR(_T("cannot copy %s to %s, because %s"),
								sSourcePath,
								sDestinationPath,
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
