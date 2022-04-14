// CopyReadOnlyVersion.cpp: implementation of the CCopyReadOnlyVersion class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "recherche.h"
#include "CopyReadOnlyVersion.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCopyReadOnlyVersion::CCopyReadOnlyVersion(CString sDestDir) :
	CWK_Thread(_T("CopyReadOnlyVersionThread"),NULL)
{
	m_sDestinationDir = sDestDir;
	int p = m_sDestinationDir.ReverseFind(_T('\\'));
	if (p != m_sDestinationDir.GetAt(m_sDestinationDir.GetLength()-1))
	{
		m_sDestinationDir += _T('\\');
	}
	
	m_saSourceFiles.Add(_T("dbs.exe"));
	m_saSourceFiles.Add(_T("recherche.exe"));
	m_saSourceFiles.Add(_T("cipc.dll"));
	m_saSourceFiles.Add(_T("vimage.dll"));
	m_saSourceFiles.Add(_T("oemgui.dll"));
	m_saSourceFiles.Add(_T("ijl15.dll"));
	m_saSourceFiles.Add(_T("mp4spvd.dll"));
#if _MFC_VER < 0x700
	m_saSourceFiles.Add(_T("mfc42.dll"));
	m_saSourceFiles.Add(_T("msvcrt.dll"));
#else
	m_saSourceFiles.Add(_T("mfc71.dll"));
	m_saSourceFiles.Add(_T("mfc71u.dll"));
	m_saSourceFiles.Add(_T("msvcrt.dll"));
#endif
	m_saSourceFiles.Add(_T("autorun.inf"));
	m_saSourceFiles.Add(_T("oem.ini"));
	
	//Audio Components
	m_saSourceFiles.Add(_T("audiounit.exe"));
	m_saSourceFiles.Add(_T("commondirectshow.dll"));
	m_saSourceFiles.Add(_T("pushsource.ax"));
	m_saSourceFiles.Add(_T("DeviceDetect.dll"));

	// Quell Verzeichnis
	GetModuleFileName(AfxGetApp()->m_hInstance,
					  m_sSourceDir.GetBuffer(_MAX_PATH),
					  _MAX_PATH);
	m_sSourceDir.ReleaseBuffer();
	p = m_sSourceDir.ReverseFind(_T('\\'));
	m_sSourceDir = m_sSourceDir.Left(p);

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
BOOL CCopyReadOnlyVersion::Run(LPVOID lpContext)
{
	CreateOemIni();
	CreateAutoRunInf();

	CopyFiles();

	DeleteFile(m_sOemIni);
	DeleteFile(m_sAutoRunInf);

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
			const CMapStringToString& map = theApp.GetFieldMap();
			POSITION pos;
			for( pos = map.GetStartPosition(); pos != NULL; )   
			{
				rKey.Empty();
				rValue.Empty();
				map.GetNextAssoc(pos,rKey,rValue);
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
