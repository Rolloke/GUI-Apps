// CloneNT.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CloneNT.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

BOOL IsWinNT();
CString GetWindowsDirectory();

/////////////////////////////////////////////////////////////////////////////
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;
	CString sSourceDir;
	CString sTargetDir;
	CString sParam;
	BOOL	bStartRDisk = FALSE;

	if (argc >= 2)
		sTargetDir = argv[1];
	if (argc >= 3)
		sParam = argv[2];

	sParam.MakeLower();

	if (!IsWinNT())
	{
		printf("Error: CloneNT only runs on WinNT4 Systems\n");
		Sleep(1000);
		return 0;
	}
	sSourceDir = GetWindowsDirectory();

	if (sTargetDir.IsEmpty())
		sTargetDir = "C:\\CloneNT\\";

	// Eventuell noch den fehlenden Backslash anhängen
	if (sTargetDir.Right(1) != "\\")
		sTargetDir += "\\";
	
	if (sTargetDir == sSourceDir)
	{
		printf("Error: Target and source directories must be different\n");
		Sleep(1000);
		return 0;
	}
	
	if (sParam.Find("-rdisk") != -1)
		bStartRDisk = TRUE;
	else
		bStartRDisk = FALSE;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		nRetCode = 1;
	}
	else							  
	{
		CloneNT Clone(sSourceDir, sTargetDir, bStartRDisk);

		printf("Cloning current NT to %s...\n", sTargetDir);
		if (Clone.CloneWinNT())
			printf ("\nWinNT cloned successfully\n");
		else
			printf ("\nError: WinNT cloned NOT successfully!\n");
	
		Sleep(1000);
	}

	return nRetCode;
}

/////////////////////////////////////////////////////////////////////////////
CloneNT::CloneNT(const CString& sSourceDir, const CString& sTargetDir, BOOL bStartRDisk)
{
	m_sSourceDir  = sSourceDir;
	m_sTargetDir  = sTargetDir;
	m_bStartRDisk = bStartRDisk;
}

/////////////////////////////////////////////////////////////////////////////
CloneNT::~CloneNT()
{

}

/////////////////////////////////////////////////////////////////////////////
BOOL CloneNT::CloneWinNT()
{
	BOOL bResult1 = FALSE;
	BOOL bResult2 = FALSE;
	if (CreateDirStructure())
	{
		// Auch wenn nicht alle Dateien kopiert werden konnten,
		// soll der Prozess fortgesetzt werden
		bResult1 = CopyFileList(); 
		if (ExpandLockedFiles())
		{
			if (ModifyBootIni())
			{
				if (m_bStartRDisk)
					WinExec("rdisk.exe /S-", SW_NORMAL);
				bResult2 = TRUE;
			}
		}
	}

	return bResult1 && bResult2;
}		

/////////////////////////////////////////////////////////////////////////////
BOOL  CloneNT::CreateDirStructure()
{
	BOOL bErr = FALSE;

	bErr |= (mkdir(m_sTargetDir) == ENOENT);
	bErr |= (mkdir(m_sTargetDir+"Temp") == ENOENT);
	bErr |= (mkdir(m_sTargetDir+"System32") == ENOENT);
	bErr |= (mkdir(m_sTargetDir+"System32\\Config") == ENOENT);
	bErr |= (mkdir(m_sTargetDir+"System32\\Drivers") == ENOENT);
	bErr |= (mkdir(m_sTargetDir+"Fonts") == ENOENT);
	bErr |= (mkdir(m_sTargetDir+"Profiles") == ENOENT);
	bErr |= (mkdir(m_sTargetDir+"Profiles\\Administrator") == ENOENT);

	if (bErr)
		printf("Can't create directory struktur\n");

	return (bErr == FALSE);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CloneNT::CopyFileList()
{
	CStdioFile FileList;
	BOOL bErr	 = FALSE;
	CString	sLine;
	CString	sFileNameSource;
	CString	sFileNameTarget;
	char*	pResource		= NULL;
	DWORD	dwResourceLen	= 0;

	// Extract FileList
	if (ExtractResource(IDR_FILELIST_FILE, pResource, dwResourceLen))
	{
		CFile file("CloneNTFileList.ini", CFile::modeCreate | CFile::modeWrite);
		file.Write(pResource, dwResourceLen);
		file.Close();

		if (FileList.Open("CloneNTFileList.ini", CFile::modeRead, NULL))
		{
			while (FileList.ReadString(sLine))
			{
				// Leerzeilen und Kommentarzeilen ignorieren
				if (!sLine.IsEmpty() && (sLine.Left(1) != "#"))
				{				
					// Alles rechts vom Kommentarzeichen ignorieren.
					int nIndex = sLine.Find("#");
					if (nIndex != -1)
						sLine = sLine.Left(nIndex);

					// Komma gibt Target an
					nIndex = sLine.Find(",");
					if (nIndex != -1)
					{
						sFileNameSource = sLine.Left(nIndex);
						sFileNameTarget	= sLine.Mid(nIndex+1);
						sFileNameSource.TrimLeft();
						sFileNameSource.TrimRight();
						sFileNameTarget.TrimLeft();
						sFileNameTarget.TrimRight();
					}
					else
					{
						sFileNameSource  = sLine;
						sFileNameTarget  = sLine;
					}

					// Doppelpunkt signalisiert absolute Pfadangaben.
					nIndex = sFileNameSource.Find(":");
					if (nIndex == -1)
						sFileNameSource = m_sSourceDir+sFileNameSource;						
					nIndex = sFileNameTarget.Find(":");
					if (nIndex == -1)
						sFileNameTarget = m_sTargetDir+sFileNameTarget;						

					if (!CopyFile(sFileNameSource, sFileNameTarget))
					{
						printf("Can't copy %s to %s\n", sFileNameSource, sFileNameTarget);
						bErr = TRUE;
					}
				}
			}
			FileList.Close();
			CFile::Remove("CloneNTFileList.ini");
		}
	}
	return (bErr == FALSE);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CloneNT::CopyFile(const CString &sFrom, const CString &sTo)
{
	BOOL bResult = FALSE;
	DWORD dwAttribute = GetFileAttributes(sTo);
	if (dwAttribute != -1)
		SetFileAttributes(sTo, FILE_ATTRIBUTE_NORMAL);
	bResult = ::CopyFile(sFrom, sTo, FALSE);
	if (dwAttribute != -1)
		SetFileAttributes(sTo, dwAttribute);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CloneNT::ExpandLockedFiles()
{
	char*	pResource		= NULL;
	DWORD	dwResourceLen	= 0;
	BOOL	bErr	= FALSE;

	TRY
	{
		// Extract Software hive
		if (ExtractResource(IDR_SOFTWARE_FILE, pResource, dwResourceLen))
		{
			CFile file(m_sTargetDir+"system32\\config\\software._", CFile::modeCreate | CFile::modeWrite);
			file.Write(pResource, dwResourceLen);
			file.Close();
			if (ExpandAndCopy(m_sTargetDir+"system32\\config\\software._", m_sTargetDir+"system32\\config\\software"))
				CFile::Remove(m_sTargetDir+"system32\\config\\software._");
			else
				bErr = TRUE;
		}
		
		// Extract System hive
		if (ExtractResource(IDR_SYSTEM_FILE, pResource, dwResourceLen))
		{
			CFile file(m_sTargetDir+"system32\\config\\system._", CFile::modeCreate | CFile::modeWrite);
			file.Write(pResource, dwResourceLen);
			file.Close();
			if (ExpandAndCopy(m_sTargetDir+"system32\\config\\system._", m_sTargetDir+"system32\\config\\system"))
				CFile::Remove(m_sTargetDir+"system32\\config\\system._");
			else
				bErr = TRUE;
		}

		// Extract Default hive
		if (ExtractResource(IDR_DEFAULT_FILE, pResource, dwResourceLen))
		{
			CFile file(m_sTargetDir+"system32\\config\\default._", CFile::modeCreate | CFile::modeWrite);
			file.Write(pResource, dwResourceLen);
			file.Close();

			if (ExpandAndCopy(m_sTargetDir+"system32\\config\\default._", m_sTargetDir+"system32\\config\\default"))
				CFile::Remove(m_sTargetDir+"system32\\config\\default._");
			else
				bErr = TRUE;
		}

		// Extract Sam hive
		if (ExtractResource(IDR_SAM_FILE, pResource, dwResourceLen))
		{
			CFile file(m_sTargetDir+"system32\\config\\sam._", CFile::modeCreate | CFile::modeWrite);
			file.Write(pResource, dwResourceLen);
			file.Close();

			if (ExpandAndCopy(m_sTargetDir+"system32\\config\\sam._", m_sTargetDir+"system32\\config\\sam"))
				CFile::Remove(m_sTargetDir+"system32\\config\\sam._");
			else
				bErr = TRUE;
		}

		// Extract Security hive
		if (ExtractResource(IDR_SECURITY_FILE, pResource, dwResourceLen))
		{
			CFile file(m_sTargetDir+"system32\\config\\security._", CFile::modeCreate | CFile::modeWrite);
			file.Write(pResource, dwResourceLen);
			file.Close();

			if (ExpandAndCopy(m_sTargetDir+"system32\\config\\security._", m_sTargetDir+"system32\\config\\security"))
				CFile::Remove(m_sTargetDir+"system32\\config\\security._");
			else
				bErr = TRUE;
		}
		
		// Extract NTUSER.DAT hive
		if (ExtractResource(IDR_NTUSER_DAT_FILE, pResource, dwResourceLen))
		{
			CFile file(m_sTargetDir+"profiles\\Administrator\\ntuser.da_", CFile::modeCreate | CFile::modeWrite);
			file.Write(pResource, dwResourceLen);
			file.Close();

			if (ExpandAndCopy(m_sTargetDir+"profiles\\Administrator\\ntuser.da_", m_sTargetDir+"profiles\\Administrator\\ntuser.dat"))
				CFile::Remove(m_sTargetDir+"profiles\\Administrator\\ntuser.da_");
			else
				bErr = TRUE;
		}

		// Copy RepairRegistry.bat
		if (ExtractResource(IDR_REPAIR_REGISTRY_FILE, pResource, dwResourceLen))
		{
			CFile file(m_sTargetDir+"System32\\RepairRegistry.bat", CFile::modeCreate | CFile::modeWrite);
			file.Write(pResource, dwResourceLen);
			file.Close();
		}
		
		// Extract Shutdown.exe
		if (ExtractResource(IDR_SHUTDOWN_FILE, pResource, dwResourceLen))
		{
			CFile file(m_sTargetDir+"System32\\shutdown._", CFile::modeCreate | CFile::modeWrite);
			file.Write(pResource, dwResourceLen);
			file.Close();

			if (ExpandAndCopy(m_sTargetDir+"system32\\shutdown._", m_sTargetDir+"system32\\shutdown.exe"))
				CFile::Remove(m_sTargetDir+"system32\\shutdown._");
			else
				bErr = TRUE;
		}

		// Extract kbdgr.dll
		if (ExtractResource(IDR_KBDGR_FILE, pResource, dwResourceLen))
		{
			CFile file(m_sTargetDir+"System32\\kbdgr.dl_", CFile::modeCreate | CFile::modeWrite);
			file.Write(pResource, dwResourceLen);
			file.Close();

			if (ExpandAndCopy(m_sTargetDir+"system32\\kbdgr.dl_", m_sTargetDir+"system32\\kbdgr.dll"))
				CFile::Remove(m_sTargetDir+"system32\\kbdgr.dl_");
			else
				bErr = TRUE;
		}

	}
	CATCH( CFileException, e )
	{
		bErr = TRUE;
	}
	END_CATCH

	return (bErr == FALSE);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CloneNT::ExtractResource(int nResourceID, char*& pResource, DWORD& dwResourceLen)
{
	BOOL bResult = FALSE;
	HRSRC hRc = FindResource(NULL, MAKEINTRESOURCE(nResourceID), "BINRES");
	if (hRc)
	{
		HGLOBAL hResource	= LoadResource(NULL, hRc);
		if (hResource)
		{
			dwResourceLen	= SizeofResource(NULL, hRc);
			pResource 		= (char*)LockResource(hResource);
			bResult			= TRUE;
		}
		else
			printf("Hive not found\n");
	}
	else
		printf("No Bin Resource\n");

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CloneNT::ExpandAndCopy(const CString& sSource, const CString& sTarget)
{
	BOOL	bResult = FALSE;
	OFSTRUCT ofstruct1;
	OFSTRUCT ofstruct2;
	int hSrcFile  = LZOpenFile((char*)(const char*)sSource, &ofstruct1, OF_READ);
	if (hSrcFile >= 0)
	{
		int hDestFile = LZOpenFile((char*)(const char*)sTarget, &ofstruct2, OF_CREATE | OF_WRITE);
		if (hDestFile >= 0)
		{
			if (LZCopy(hSrcFile, hDestFile) >= 0)
				bResult = TRUE;
			LZClose(hDestFile);
		}
		LZClose(hSrcFile);
	}
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CloneNT::ModifyBootIni()
{
	CString sBootIni = "C:\\Boot.ini";
	char cReturn[256];

	if (GetPrivateProfileString("boot loader", "default", "", cReturn, 255, sBootIni) > 0)
	{
		CString sDefault = cReturn;
		int nIndex = sDefault.Find("\\");
		if (nIndex != -1)
		{
			CString sLine = sDefault.Left(nIndex);
			CString sRoot;
			sRoot = "\\"+ m_sTargetDir.Mid(3);		// Laufwerksbuchstaben (z.B. C:) entfernen
			sRoot = sRoot.Left(sRoot.GetLength()-1);// letzten Backslash entfernen.
			sLine += sRoot;

			// Schreibschutz entfernen
			SetFileAttributes(sBootIni, FILE_ATTRIBUTE_NORMAL);
			
			// Alle WindowsNT Booteinträge erstmal entfernen
			while (GetPrivateProfileString("operating systems", sDefault, "", cReturn, 255, sBootIni) > 0)
				WritePrivateProfileString("operating systems", sDefault, NULL, sBootIni);

			// Die Recovery-Booteinträge erstmal entfernen
			while (GetPrivateProfileString("operating systems", sLine, "", cReturn, 255, sBootIni) > 0)
				WritePrivateProfileString("operating systems", sLine, NULL, sBootIni);

			// 'Video mode' Bootmenü Eintrag schreiben
			WritePrivateProfileString("operating systems", sDefault, "\"Video mode\"", sBootIni);
			
			// 'Recovery mode' Bootmenü Eintrag schreiben
			WritePrivateProfileString("operating systems", sLine, "\"Recovery mode\"", sBootIni);
			
			// 'Timeout' auf 2 Sekunden setzen
			WritePrivateProfileString("boot loader", "timeout", "2", sBootIni);

			// Boot.ini muß schreibgeschützt sein, damit die Bootmenü einträge auswählbar sind!!?
			SetFileAttributes(sBootIni, FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY);
		}
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL IsWinNT()
{
	OSVERSIONINFO osinfo;

	ZeroMemory(&osinfo, sizeof(OSVERSIONINFO));
	osinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	GetVersionEx(&osinfo);

	return (osinfo.dwPlatformId==VER_PLATFORM_WIN32_NT) &&
			(osinfo.dwMajorVersion == 4) &&
			(osinfo.dwMinorVersion  == 0);
}

/////////////////////////////////////////////////////////////////////////////
CString GetWindowsDirectory()
{
	char szPath[MAX_PATH] = {0};

	::GetWindowsDirectory(szPath,_MAX_PATH);
	return (CString)szPath+"\\";
}
