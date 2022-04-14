// CopyReadOnlyVersion.cpp: implementation of the CCopyReadOnlyVersion class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IdipClient.h"
#include "CopyReadOnlyVersion.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
CCopyReadOnlyVersion::CCopyReadOnlyVersion(CString sDestDir, BOOL bCopyToTemp) :
	CWK_Thread(_T("CopyReadOnlyVersionThread"),NULL)
{
	m_sDestinationDir = sDestDir;
	int p = m_sDestinationDir.ReverseFind(_T('\\'));

	if(bCopyToTemp)
	{
		//überprüfe Temppfad beim CD/DVD brennen, denn dort sollen auch die Daten der
		//Lesesoftware rein
		if (p != m_sDestinationDir.GetAt(m_sDestinationDir.GetLength()-1))
		{
			m_sDestinationDir += _T('\\');
		}
	}

	//kein Temppfad, also wird direkt auf das Wechselmedium (Stick, ext.HD) kopiert
	
	m_saSourceFiles.Add(_T("dbs.exe"));
	m_saSourceFiles.Add(_T("idipclient.exe"));

	m_saSourceFiles.Add(_T("CipcU.dll"));
	m_saSourceFiles.Add(_T("VImageU.dll"));
	m_saSourceFiles.Add(_T("OemGuiU.dll"));
	m_saSourceFiles.Add(_T("ijl15.dll"));
	m_saSourceFiles.Add(_T("SkinsU.dll"));

	m_saSourceFiles.Add(_T("mp4spvd_all.dll"));

	m_saSourceFiles.Add(_T("mfc71.dll"));
	m_saSourceFiles.Add(_T("mfc71u.dll"));
	m_saSourceFiles.Add(_T("msvcr71.dll"));
	m_saSourceFiles.Add(_T("msvcp71.dll"));

	m_saSourceFiles.Add(_T("autorun.inf"));
	m_saSourceFiles.Add(_T("oem.ini"));
	m_saSourceFiles.Add(_T("logo.bmp"));
	
	//Audio Components
	m_saSourceFiles.Add(_T("audiounit.exe"));
	m_saSourceFiles.Add(_T("commondirectshowU.dll"));
	m_saSourceFiles.Add(_T("pushsource.ax"));
	m_saSourceFiles.Add(_T("DeviceDetectU.dll"));

	// Quell Verzeichnis
	GetModuleFileName(AfxGetApp()->m_hInstance,
					  m_sSourceDir.GetBuffer(_MAX_PATH),
					  _MAX_PATH);
	m_sSourceDir.ReleaseBuffer();
	p = m_sSourceDir.ReverseFind(_T('\\'));
	m_sSourceDir = m_sSourceDir.Left(p);
#ifdef _DEBUG
	m_sSourceDir = theApp.GetApplicationDir();
#endif

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

	CString sOemFile = theApp.GetApplicationDir() + _T("\\oem.ini");
	CString sTempFile = m_sWindowsDir + _T("\\oem.ini");

	CString sOEM,sWrite,sDest;
	CStdioFileU fileOem, fileTemp;
	CWK_Dongle dongle;
	sOEM.Format(_T("%d"),dongle.GetOemCode());

	fileOem.SetFileMCBS((WORD)CWK_String::GetCodePage());	// Codepage is set in InitInstance
	fileTemp.SetFileMCBS((WORD)CWK_String::GetCodePage());	// 

	CFileStatus fileStatus;
	if (CFile::GetStatus(sTempFile, fileStatus))
	{
		WK_TRACE_WARNING(_T("CopyReadOnlyVersion: File %s already exists, overwriting\n"), sTempFile);
	}
	if (fileTemp.Open(sTempFile, CFile::modeCreate|CFile::modeWrite))
	{
		TRY
		{
			if (CFile::GetStatus(sOemFile, fileStatus))
			{
				if (fileOem.Open(sOemFile, CFile::modeRead))
				{
					ULONGLONG ulSize = fileStatus.m_size;
					ASSERT(ulSize <= UINT_MAX);
					DWORD dwSize = (DWORD)ulSize;
					BYTE* pBuffer = new BYTE[dwSize];
					DWORD dwRead = fileOem.Read(pBuffer, dwSize);
					if (dwRead != dwSize)
					{
						// gf seems to be normal, so only TRACE
						TRACE(_T("CopyReadOnlyVersion: File %s only read %u of %u\n"),
														sOemFile, dwRead, dwSize);
					}

					fileTemp.Write(pBuffer, dwRead);
					fileOem.Close();
					WK_DELETE(pBuffer);
				}
			}

			fileTemp.WriteString(_T("\n")); // just for readability
			// now follows the Read-Only part
			fileTemp.WriteString(_T("[oem]\n"));
			sWrite.Format(_T("oem=%s\n"),sOEM);
			fileTemp.WriteString(sWrite);
			sWrite.Format(_T("version=%s\n"),dongle.GetVersionString());
			fileTemp.WriteString(sWrite);
			
			fileTemp.WriteString(_T("[Fields]\n"));
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
					fileTemp.WriteString(s);
				}
			}

			fileTemp.Flush();
			fileTemp.Close();
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
	CStdioFileU file;
	file.SetFileMCBS(CP_ACP);	// Codepage is set in InitInstance

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
	CString sSource,sSourcePath,sDestinationPath = m_sDestinationDir;
	BOOL bRet = TRUE;
	int i,c;
	if (m_sDestinationDir.IsEmpty())
	{
		bRet = FALSE;
	}

	c = m_saSourceFiles.GetSize();
	for (i=0;i<c;i++) 
	{
		sSource = m_saSourceFiles.GetAt(i);
		// aktuelles Verzeichnis
		// Die oem.ini nicht aus dem Source-Dir, sondern dem Windows-Dir nehmen
		if (sSource.CompareNoCase(_T("oem.ini")) == 0)
		{
			sSourcePath = m_sWindowsDir + _T('\\') + sSource;
		}
		else
		{
			sSourcePath = m_sSourceDir + _T('\\') + sSource;
		}

		if (!DoesFileExist(sSourcePath))
		{
			sSourcePath = m_sSourceDir + _T("\\dlls\\") + sSource;
			if (!DoesFileExist(sSourcePath))
			{
				sSourcePath = m_sWindowsDir + _T('\\') + sSource;
				if (!DoesFileExist(sSourcePath))
				{
					sSourcePath = m_sWindowsSystemDir + _T('\\') + sSource;
					if (!DoesFileExist(sSourcePath))
					{
						if (sSource.CompareNoCase(_T("logo.bmp")) == 0)
						{
							// RKE: Macht nix, isso
							continue;
						}
						WK_TRACE_ERROR(_T("Cannot copy %s to %s, because not found\n"), sSource, sDestinationPath);
						bRet = FALSE;
						continue;
					}
				}
			}
		}
		if (0==sSource.CompareNoCase(_T("mp4spvd_all.dll")))
		{
			sSource = _T("mp4spvd.dll");
		}

		if (m_sDestinationDir.IsEmpty())
		{
			WIN32_FILE_ATTRIBUTE_DATA fad;
			GetFileAttributesEx(sSourcePath, GetFileExInfoStandard, &fad);
			bRet += fad.nFileSizeLow;
			continue;
		}
		sDestinationPath = m_sDestinationDir + sSource;

		if (!DoesFileExist(sDestinationPath))
		{
			if (!CopyFile(sSourcePath,sDestinationPath,TRUE))
			{
				WK_TRACE_ERROR(_T("Cannot copy %s to %s, because %s"),
								sSourcePath,
								sDestinationPath,
								GetLastErrorString());
				bRet = FALSE;
			}
			else
			{
				WK_TRACE(_T("Copied %s to %s\n"),sSourcePath,sDestinationPath);
			}
		}
	} // for 

	return bRet;
}
//////////////////////////////////////////////////////////////////////
DWORD CCopyReadOnlyVersion::GetReaderSoftwareSizeMB()
{
	DWORD dwMB;
	CString sDestination = m_sDestinationDir;
	m_sDestinationDir.Empty();
	BOOL bResult = CopyFiles();
	dwMB = bResult >> 20;
	dwMB++;
	m_sDestinationDir = sDestination;
	return dwMB;
}
