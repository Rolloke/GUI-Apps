// RegPriv.cpp

#include "windows.h"
#include <stdio.h>
#include <tchar.h>
#include <malloc.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const TCHAR szLogon[] = _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon");

/*------------------------------------------------------------------
| Name: RunningAsAdministrator
| Desc: checks if user has administrator privileges
| Notes: This function returns TRUE if the user identifier associated with 
|   this process is a member of the the Administrators group.
------------------------------------------------------------------*/
BOOL RunningAsAdministrator ( VOID)
{ 
	BOOL   fAdmin = FALSE; 
	HANDLE  hThread;
	TOKEN_GROUPS *ptg = NULL; 
	DWORD  cbTokenGroups; 
	DWORD  dwGroup;
	PSID   psidAdmin;
	SID_IDENTIFIER_AUTHORITY SystemSidAuthority= SECURITY_NT_AUTHORITY;
	
	// First we must open a handle to the access token for this thread.
	if ( !OpenThreadToken ( GetCurrentThread(), TOKEN_QUERY, FALSE, &hThread)) 
	{
		if ( GetLastError() == ERROR_NO_TOKEN)  
		{
			// If the thread does not have an access token, we'll examine the
			// access token associated with the process.
			if (! OpenProcessToken ( GetCurrentProcess(), TOKEN_QUERY, &hThread))   
				return ( FALSE);  
		}
		else    
			return ( FALSE); 
	}

	// Then we must query the size of the group information associated with
	// the token. Note that we expect a FALSE result from GetTokenInformation
	// because we've given it a NULL buffer. On exit cbTokenGroups will tell
	// the size of the group information.
	if ( GetTokenInformation ( hThread, TokenGroups, NULL, 0, &cbTokenGroups))
		return ( FALSE);
	
	// Here we verify that GetTokenInformation failed for lack of a large
	// enough buffer. 
	if ( GetLastError() != ERROR_INSUFFICIENT_BUFFER)
		return ( FALSE); 
	
	// Now we allocate a buffer for the group information.
	// Since _alloca allocates on the stack, we don't have
	// to explicitly deallocate it. That happens automatically
	// when we exit this function. 
	ptg = (TOKEN_GROUPS*)_alloca ( cbTokenGroups);
	if (!ptg) 
		return ( FALSE); 
	
	// Now we ask for the group information again.
	// This may fail if an administrator has added this account
	// to an additional group between our first call to
	// GetTokenInformation and this one.
	
	if ( !GetTokenInformation ( hThread, TokenGroups, ptg, cbTokenGroups,
		&cbTokenGroups) )  
		return ( FALSE);
	
	// Now we must create a System Identifier for the Admin group.
	if ( ! AllocateAndInitializeSid ( &SystemSidAuthority, 2, 
		SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS,
		0, 0, 0, 0, 0, 0, &psidAdmin) )  
		return ( FALSE);
	
	// Finally we'll iterate through the list of groups for this access
	// token looking for a match against the SID we created above. fAdmin= FALSE;
	for ( dwGroup= 0; dwGroup < ptg->GroupCount; dwGroup++) 
	{
		if ( EqualSid ( ptg->Groups[dwGroup].Sid, psidAdmin))  
		{   
			fAdmin = TRUE;
			break;  
		} 
	}
	
	// Before we exit we must explicity deallocate the SID we created.
	FreeSid ( psidAdmin); 
	return ( fAdmin);
}
/* eof - RunningAsAdministrator */

#define RTN_OK 0
#define RTN_ERROR 13

void DisplayWinError(
    LPSTR szAPI,    // pointer to Ansi function name
    DWORD dwError   // DWORD WinError
    )
{
    LPSTR MessageBuffer;
    DWORD dwBufferLength;

    //
    // TODO get this fprintf out of here!
    //
    fprintf(stderr,"%s error!\n", szAPI);

	dwBufferLength = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
									NULL,
									dwError,
									MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
									(LPSTR) &MessageBuffer,
									0,
									NULL
									);
    if(dwBufferLength)
    {
        //
        // Output message string on stderr
        //
		OutputDebugString(MessageBuffer);
        //
        // free the buffer allocated by the system
        //
        LocalFree(MessageBuffer);
    }
}

BOOL SetLogonPrivileges()
{
    SID_IDENTIFIER_AUTHORITY sia = SECURITY_NT_AUTHORITY;
    PSID pSystemSid = NULL;
    PSID pEveryUserSid = NULL;
    PSID pAdministratorsSid = NULL;
    SECURITY_DESCRIPTOR sd;
    PACL pDacl = NULL;
    DWORD dwAclSize;
    HKEY hKey;
    LONG lRetCode;
    BOOL bSuccess = FALSE; // assume this function fails

    //
    // open the performance key for WRITE_DAC access
    //
    lRetCode = RegOpenKeyEx(
        HKEY_LOCAL_MACHINE,
		szLogon,
        0,
        WRITE_DAC,
        &hKey
        );

    if(lRetCode != ERROR_SUCCESS) {
        DisplayWinError("RegOpenKeyEx", lRetCode);
        return RTN_ERROR;
    }

    //
    // prepare a Sid representing system
    //
    if(!AllocateAndInitializeSid(
        &sia,
        1,
		SECURITY_LOCAL_SYSTEM_RID,
        0, 
		0, 0, 0, 0, 0, 0,
        &pSystemSid
        )) 
	{
        DisplayWinError("AllocateAndInitializeSid", GetLastError());
        goto cleanup; /* NEEDS_GOTO */
    }

    //
    // prepare a Sid representing any user
    //
    if(!AllocateAndInitializeSid(
        &sia,
        2,
		SECURITY_BUILTIN_DOMAIN_RID,
        DOMAIN_ALIAS_RID_USERS, 
		0, 0, 0, 0, 0, 0,
        &pEveryUserSid
        )) 
	{
        DisplayWinError("AllocateAndInitializeSid", GetLastError());
        goto cleanup; /* NEEDS_GOTO */
    }

    //
    // preprate a Sid representing the well-known admin group
    //
    if(!AllocateAndInitializeSid(
        &sia,
        2,
        SECURITY_BUILTIN_DOMAIN_RID,
        DOMAIN_ALIAS_RID_ADMINS,
        0, 0, 0, 0, 0, 0,
        &pAdministratorsSid
        )) 
	{
        DisplayWinError("AllocateAndInitializeSid", GetLastError());
        goto cleanup;	/* NEEDS_GOTO */
    }

    //
    // compute size of new acl
    //
    dwAclSize = sizeof(ACL) +
        3 * ( sizeof(ACCESS_ALLOWED_ACE) - sizeof(DWORD) ) +
        GetLengthSid(pSystemSid) +
        GetLengthSid(pEveryUserSid) +
        GetLengthSid(pAdministratorsSid) ;

    //
    // allocate storage for Acl
    //
    pDacl = (PACL)HeapAlloc(GetProcessHeap(), 0, dwAclSize);
    
	if(pDacl == NULL) 
		goto cleanup;	/* NEEDS_GOTO */

    if(!InitializeAcl(pDacl, dwAclSize, ACL_REVISION)) 
	{
        DisplayWinError("InitializeAcl", GetLastError());
        goto cleanup;	/* NEEDS_GOTO */
    }

    //
    // grant the System Sid KEY_ALL_ACCESS access to the perf key
    //
    if(!AddAccessAllowedAce(
        pDacl,
        ACL_REVISION,
        KEY_ALL_ACCESS,
        pSystemSid
        )) 
	{
        DisplayWinError("AddAccessAllowedAce", GetLastError());
        goto cleanup; /* NEEDS_GOTO */
    }
    //
    // grant the EveryUser Sid KEY_ALL_ACCESS access to the perf key
    //
    if(!AddAccessAllowedAce(
        pDacl,
        ACL_REVISION,
        KEY_ALL_ACCESS,
        pEveryUserSid
        )) 
	{
        DisplayWinError("AddAccessAllowedAce", GetLastError());
        goto cleanup;	/* NEEDS_GOTO */
    }

    //
    // grant the Administrators Sid KEY_ALL_ACCESS access to the perf key
    //
    if(!AddAccessAllowedAce(
        pDacl,
        ACL_REVISION,
        KEY_ALL_ACCESS,
        pAdministratorsSid
        )) 
	{
        DisplayWinError("AddAccessAllowedAce", GetLastError());
        goto cleanup;	/* NEEDS_GOTO */
    }

    if(!InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION)) 
	{
        DisplayWinError("InitializeSecurityDescriptor", GetLastError());
        goto cleanup;	/* NEEDS_GOTO */
    }

    if(!SetSecurityDescriptorDacl(&sd, TRUE, pDacl, FALSE)) 
	{
        DisplayWinError("SetSecurityDescriptorDacl", GetLastError());
        goto cleanup;	/* NEEDS_GOTO */
    }

    //
    // apply the security descriptor to the registry key
    //
    lRetCode = RegSetKeySecurity(
        hKey,
        (SECURITY_INFORMATION)DACL_SECURITY_INFORMATION,
        &sd
        );

    if(lRetCode != ERROR_SUCCESS) 
	{
        DisplayWinError("RegSetKeySecurity", lRetCode);
        goto cleanup;	/* NEEDS_GOTO */
    }

    bSuccess = TRUE; // indicate success

cleanup:

    RegCloseKey(hKey);
    RegCloseKey(HKEY_LOCAL_MACHINE);

    //
    // free allocated resources
    //
    if(pDacl != NULL)
        HeapFree(GetProcessHeap(), 0, pDacl);

    if(pEveryUserSid != NULL)
        FreeSid(pEveryUserSid);

    if(pSystemSid != NULL)
        FreeSid(pSystemSid);

    if(pAdministratorsSid != NULL)
        FreeSid(pAdministratorsSid);

    if(bSuccess) 
	{
        printf("SUCCESS updating performance data security\n");
        return RTN_OK;
    } 
	else 
	{
        printf("ERROR updating performance data security\n");
        return RTN_ERROR;
    }
}

BOOL GetLogonPrivileges()
{
//    SID_IDENTIFIER_AUTHORITY sia = SECURITY_NT_AUTHORITY;
//    PSID pEveryUserSid = NULL;
//    PSID pAdministratorsSid = NULL;
	#define              SZ_SD_BUF 8096
	UCHAR                ucBuf       [SZ_SD_BUF] = "";
	DWORD                dwSDLength = SZ_SD_BUF;
	PSECURITY_DESCRIPTOR psdSD      = (PSECURITY_DESCRIPTOR)&ucBuf;
    //SECURITY_DESCRIPTOR sd;
    HKEY hKey;
    LONG lRetCode;
    BOOL bSuccess = FALSE; // assume this function fails

    //
    // open the performance key for WRITE_DAC access
    //
    lRetCode = RegOpenKeyEx(
        HKEY_LOCAL_MACHINE,
		szLogon,
        0,
        KEY_ALL_ACCESS,
        &hKey
        );

    if(lRetCode != ERROR_SUCCESS) {
        DisplayWinError("RegOpenKeyEx", lRetCode);
        return RTN_ERROR;
    }

    //
    // apply the security descriptor to the registry key
    //
    dwSDLength = SZ_SD_BUF;
    lRetCode = RegGetKeySecurity(
        hKey,
        (SECURITY_INFORMATION)DACL_SECURITY_INFORMATION,
        psdSD,
		&dwSDLength
        );

    if(lRetCode != ERROR_SUCCESS) {
        DisplayWinError("RegSetKeySecurity", lRetCode);
        goto cleanup;	/* NEEDS_GOTO */
    }

    bSuccess = TRUE; // indicate success

cleanup:

    RegCloseKey(hKey);
    RegCloseKey(HKEY_LOCAL_MACHINE);

	return bSuccess ? RTN_OK : RTN_ERROR;
}
/* eof - CreateSDForRegKey */
static const TCHAR szLauncher[] = _T("Software\\DVRT\\SecurityLauncher");

void WriteAdmin(BOOL bAdmin)
{
	LONG res;
	HKEY hSecKey = NULL;
	DWORD dw=0;
	TCHAR szValue[2] = "1";
	TCHAR szColors[3];

	HWND hWnd = GetDesktopWindow();
	HDC hDC = GetDC(hWnd);
	wsprintf(szColors,"%d",GetDeviceCaps(hDC,BITSPIXEL));
	ReleaseDC(hWnd,hDC);

	res = RegCreateKeyEx(HKEY_LOCAL_MACHINE, szLauncher, 0, REG_NONE,
						REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL,
						&hSecKey, &dw);
	if (res == ERROR_SUCCESS)
	{
		szValue[0] = bAdmin ? '1' : '0';
		res = RegSetValueEx(hSecKey, "SetupAsAdmin", NULL, REG_SZ,
			(LPBYTE)szValue, (lstrlen(szValue)+1)*sizeof(TCHAR));
		if (res != ERROR_SUCCESS)
		{
			DisplayWinError("cannot write value",GetLastError());
		}

		res = RegSetValueEx(hSecKey, "BitsPerPixel", NULL, REG_SZ,
			(LPBYTE)szColors, (lstrlen(szColors)+1)*sizeof(TCHAR));
		if (res != ERROR_SUCCESS)
		{
			DisplayWinError("cannot write value",GetLastError());
		}
		RegCloseKey(hSecKey);
	}
	else
	{
		DisplayWinError("cannot open section key because",GetLastError());
	}
}
/* eof - WriteAdmin */

int WINAPI WinMain(  HINSTANCE /*hInstance*/,  // handle to current instance
					  HINSTANCE /*hPrevInstance*/,  // handle to previous instance
					  LPSTR /*lpCmdLine*/,      // pointer to command line
					  int /*nCmdShow*/          // show state of window
					  )
{
	BOOL bAdmin = RunningAsAdministrator();

	if (bAdmin)
	{
//		MessageBox(NULL,"Running as Admin","ARP",MB_OK);
		if (RTN_OK==SetLogonPrivileges())
		{
			//MessageBox(NULL,"Privileges successfull adjusted","ARP",MB_OK);
		}
		else
		{
			//MessageBox(NULL,"cannot adjust Privileges","ARP",MB_OK);
		}
	}
	else
	{
		//MessageBox(NULL,"Not Running as Admin","ARP",MB_OK);
	}

	WriteAdmin(bAdmin);

	return 0;
}
