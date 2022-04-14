// Win95Reboot.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "wkclasses\wk_wincpp.h"

/////////////////////////////////////////////////////////////////////////////
BOOL AdjustPrivileges(LPCTSTR privilege, DWORD dwAccess = SE_PRIVILEGE_ENABLED)
{
	HANDLE hToken;              // handle to process token 
	TOKEN_PRIVILEGES tkp;        // ptr. to token structure 
	
	BOOL fResult;                  // system shutdown flag 
	
	// 
	// Get the current process token handle 
	// so we can get debug privilege. 
	
	
	OpenProcessToken(GetCurrentProcess(), 
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken) ;
	
	// Get the LUID for debug privilege. 
	
	LookupPrivilegeValue(NULL, privilege, 
		&tkp.Privileges[0].Luid); 
	
	tkp.PrivilegeCount = 1;  // one privilege to set    
	tkp.Privileges[0].Attributes = dwAccess; 
	
	// Get shutdown privilege for this process. 
	
	fResult = AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, 
		(PTOKEN_PRIVILEGES) NULL, 0); 
	
	
	
	return fResult;
}

int APIENTRY WinMain(HINSTANCE /*hInstance*/,
                     HINSTANCE /*hPrevInstance*/,
                     LPSTR     lpCmdLine,
                     int       /*nCmdShow*/)
{
 	COsVersionInfo os;

	if (os.IsNT())
		AdjustPrivileges(SE_SHUTDOWN_NAME);

	if (stricmp(lpCmdLine,"reboot") == 0)
		ExitWindowsEx(EWX_REBOOT, 0);
	else if (stricmp(lpCmdLine,"poweroff") == 0)
		ExitWindowsEx(EWX_POWEROFF, 0);
	else if (stricmp(lpCmdLine,"shutdown") == 0)
		ExitWindowsEx(EWX_SHUTDOWN, 0);
	return 0;
}



