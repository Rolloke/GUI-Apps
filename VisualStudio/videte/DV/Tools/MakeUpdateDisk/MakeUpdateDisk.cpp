// MakeUpdateDisk.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <direct.h>
#include "MakeUpdateDisk.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;
BOOL  DoesFileExist(const char* szFileOrDirectory);
BOOL  LoadFile(const CString& sSource);
BOOL  GetCRC(DWORD& dwCRC);
BOOL  Decode();
BOOL  SaveFile(const CString& sTarget);

BYTE* g_pBuffer = NULL;
DWORD g_dwLen	= 0;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = 1;
	}
	else
	{
		CString sSetupFileName			= "";
		CString sIniFileName			= "";
		CString sPrevIniFileName		= "";
		CString	sTargetFileName			= "";
		CString	sTargetDir				= "";
		CString sCRC					= "";
		CString sTC						= "";
		CString sTmp					= "";
		int		nDiskCounter			= 1;
		BOOL	bFondSomeThing			= FALSE;
		DWORD	dwCRC					= 0;
		char	szBuffer[_MAX_PATH]		= {0};
		char	szSourceDir[_MAX_PATH]	= {0};;
		BOOL	bResult					= FALSE;
		DWORD	dwTimeStamp				= GetTickCount();
		sTC.Format("0x%lx", dwTimeStamp);

		if (argc != 4)
		{
			printf("MakeUpdateDisk <PathToSetup OemNumber BuildNumber>\n");
			printf("z.b: MakeUpdateDisk d:\\Temp 1 283\n");
			return 0;
		}

		strcpy(szSourceDir, argv[argc-3]);
		sTmp.Format("%s\\Update", szSourceDir);
		mkdir(sTmp);

		int nOem	= atoi(argv[argc-2]);
		int nBuild	= atoi(argv[argc-1]);

		sTargetDir.Format("%s\\Update\\Disk1", szSourceDir); 
		sSetupFileName.Format("%s\\Setup.exe", szSourceDir);		
		sIniFileName.Format("%s\\Disk", sTargetDir);
		sTargetFileName.Format("%s\\Update", sTargetDir);
		mkdir(sTargetDir);
		while (DoesFileExist(sSetupFileName))
		{
			if (LoadFile(sSetupFileName))
			{
				if (GetCRC(dwCRC))
				{
					if (Decode())
					{
						if (SaveFile(sTargetFileName))
						{
							sCRC.Format("0x%lx", dwCRC);

							WritePrivateProfileString("Setup", "Oem",		itoa(nOem,   szBuffer, 10), sIniFileName);
							WritePrivateProfileString("Setup", "ThisDisk",	itoa(nDiskCounter,   szBuffer, 10), sIniFileName);
							WritePrivateProfileString("Setup", "NextDisk",	itoa(nDiskCounter+1, szBuffer, 10), sIniFileName);
							WritePrivateProfileString("Setup", "CRC",		sCRC, sIniFileName);
							WritePrivateProfileString("Setup", "Build",		itoa(nBuild,   szBuffer, 10), sIniFileName);		
							WritePrivateProfileString("Setup", "TimeStamp",	sTC, sIniFileName);
							
							bFondSomeThing = TRUE;
							nDiskCounter++;

							sPrevIniFileName = sIniFileName;
							sTargetDir.Format("%s\\Update\\Disk%d", szSourceDir, nDiskCounter); 
							sTargetFileName.Format("%s\\Update", sTargetDir);
							sIniFileName.Format("%s\\Disk", sTargetDir);

							sSetupFileName.Format("%s\\Setup.w%02d", szSourceDir, nDiskCounter);
							if (DoesFileExist(sSetupFileName))
								mkdir(sTargetDir);
							bResult = TRUE;
						}
					}					
				}	
			}
			if (!bResult)
				break;
		}
		if (bFondSomeThing)
			WritePrivateProfileString("Setup", "NextDisk", "0", sPrevIniFileName);
	}

	return nRetCode;
}

/////////////////////////////////////////////////////////////////////////////
BOOL LoadFile(const CString& sSource)
{
	BOOL bResult = FALSE;
	CFile file(sSource, CFile::modeRead | CFile::shareDenyNone);
	
	DWORD dwLen = file.GetLength();
	if (dwLen > 0)
	{
		if (g_pBuffer)
		{
			delete []g_pBuffer;
			g_pBuffer = NULL;
		}

		g_pBuffer = new BYTE[dwLen];
		if (file.Read(g_pBuffer, dwLen) == dwLen)
		{
			g_dwLen = dwLen;
			bResult = TRUE;
		}
	}
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL GetCRC(DWORD& dwCRC)
{
	BOOL bResult = FALSE;
	
	if (g_pBuffer && (g_dwLen > 0))
	{
		dwCRC = 0;
		for (DWORD dwI = 0; dwI < g_dwLen; dwI++)
			dwCRC += g_pBuffer[dwI]*dwI;
		bResult = TRUE;
	}
	
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL Decode()
{
	BOOL bResult = FALSE;
	
	if (g_pBuffer && (g_dwLen > 0))
	{
		for (DWORD dwI = 0; dwI < g_dwLen; dwI++)
			g_pBuffer[dwI] ^= 0x85;
		bResult = TRUE;
	}
	
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL SaveFile(const CString& sTarget)
{
	BOOL bResult = FALSE;
	CFile file(sTarget, CFile::modeWrite | CFile::modeCreate);
	
	DWORD dwLen = file.GetLength();
	if (g_pBuffer && (g_dwLen > 0))
	{
		file.Write(g_pBuffer, g_dwLen);
		bResult = TRUE;
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL DoesFileExist(const char* szFileOrDirectory)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE h = FindFirstFile(szFileOrDirectory, &FindFileData);
	if (h == INVALID_HANDLE_VALUE){
		return FALSE;
	}
	FindClose(h);
	return TRUE;
}
