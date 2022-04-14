// NeroBurn.cpp: implementation of the CNeroBurn class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "acdc.h"
#include "NeroBurn.h"
#include "Psapi.h"

#include <aclapi.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

HMODULE  g_hPSapi = NULL;
BOOL  (WINAPI*g_pfnEnumProcesses)(DWORD*, DWORD, DWORD*)              = NULL;
BOOL  (WINAPI*g_pfnEnumProcessModules)(HANDLE,HMODULE*,DWORD,LPDWORD) = NULL;
DWORD (WINAPI*g_pfnGetModuleBaseName)(HANDLE,HMODULE,LPTSTR,DWORD)    = NULL;
DWORD (WINAPI*g_pfnGetMappedFileName)(HANDLE,LPVOID,LPTSTR,DWORD)     = NULL;
DWORD (WINAPI*g_pfnGetModuleFileNameEx)(HANDLE,HMODULE,LPTSTR,DWORD)  = NULL;
BOOL  (WINAPI*g_pfnGetModuleInformation)(HANDLE,HMODULE,LPMODULEINFO,DWORD) = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNeroBurn::CNeroBurn(CProducer* pProducer)
{
	m_pProducer			= pProducer;
	m_bIsDVD			= FALSE;

	m_pBurnContext = NULL;
	DWORD dw = NERO_MINIMUM_VERSION;
	m_sVersionRequired.Format(_T("%d.%d.%d.%d"),
						(dw / 1000),
						(dw /  100)  % 10,
						(dw /   10)  % 10,
						dw          % 10);

	//create BurnContext. This object connects to the NeroAPI.dll
	m_pBurnContext = new CBurnContext(&m_params, this);
	m_bNeroAPIInitialized = FALSE;
	
   if (g_hPSapi == NULL)
   {
      g_hPSapi = LoadLibrary(_T("PSAPI.DLL"));                    // Dll laden
      if (g_hPSapi >= (HANDLE)32)
      {
         g_pfnEnumProcesses        = (BOOL  (WINAPI*)(DWORD*, DWORD, DWORD*))             GetProcAddress((HINSTANCE)g_hPSapi, "EnumProcesses");
         g_pfnEnumProcessModules   = (BOOL  (WINAPI*)(HANDLE,HMODULE*,DWORD,LPDWORD))     GetProcAddress((HINSTANCE)g_hPSapi, "EnumProcessModules");
#ifdef _UNICODE
         g_pfnGetModuleBaseName    = (DWORD (WINAPI*)(HANDLE,HMODULE,LPTSTR,DWORD))       GetProcAddress((HINSTANCE)g_hPSapi, "GetModuleBaseNameW");
         g_pfnGetMappedFileName    = (DWORD (WINAPI*)(HANDLE,LPVOID,LPTSTR,DWORD))        GetProcAddress((HINSTANCE)g_hPSapi, "GetMappedFileNameW");
         g_pfnGetModuleFileNameEx  = (DWORD (WINAPI*)(HANDLE,HMODULE,LPTSTR,DWORD))       GetProcAddress((HINSTANCE)g_hPSapi, "GetModuleFileNameExW");
#else
         g_pfnGetModuleBaseName    = (DWORD (WINAPI*)(HANDLE,HMODULE,LPTSTR,DWORD))       GetProcAddress((HINSTANCE)g_hPSapi, "GetModuleBaseNameA");
         g_pfnGetMappedFileName    = (DWORD (WINAPI*)(HANDLE,LPVOID,LPTSTR,DWORD))        GetProcAddress((HINSTANCE)g_hPSapi, "GetMappedFileNameA");
         g_pfnGetModuleFileNameEx  = (DWORD (WINAPI*)(HANDLE,HMODULE,LPTSTR,DWORD))       GetProcAddress((HINSTANCE)g_hPSapi, "GetModuleFileNameExA");
#endif

         g_pfnGetModuleInformation = (BOOL  (WINAPI*)(HANDLE,HMODULE,LPMODULEINFO,DWORD)) GetProcAddress((HINSTANCE)g_hPSapi, "GetModuleInformation");
      }
   }

}

CNeroBurn::~CNeroBurn()
{
	DeleteBurnContext();
}


/*********************************************************************************************
 Class		   : CNeroBurn
 Function      : MainApp 
 Description   : This function leads all NeroAPI calls and handles all errors.

 Parameters:  
  argc:	   (O):  numbers of parsed parameters  (int)
  argc:	   (O):  list of the parsed parameters  (char ** argv)

 Result type:  returns 0 if successful or an error code if not  (EXITCODE)

 Author: TKR
 created: July, 16 2002
 <TITLE MainApp >
 <GROUP CNeroBurn>
 <TOPICORDER 0>
 <KEYWORDS CNeroBurn, MainApp>
*********************************************************************************************/
EXITCODE CNeroBurn::MainApp(CStringArray& saParams)
{
	CString sCommandLine = _T("nero");

	for (int i=0;i<saParams.GetSize();i++)
	{
		sCommandLine += _T(" ") + saParams.GetAt(i);
	}
	WK_TRACE(_T("%s\n"),sCommandLine);

	EXITCODE code = EXITCODE_OK;

	//get and decode the argument list and fill the PARAMETERS object m_params
	if (false == GetParams(m_params, saParams))
	{
		WK_TRACE (_T("\n%s\n"), GetTextualExitCode (EXITCODE_BAD_USAGE));
		return EXITCODE_BAD_USAGE;
	}

	// Load the NEROAPI.DLL.
	if(!m_bNeroAPIInitialized)
	{
		TRACE(_T("*** CNeroBurn::MainApp Lade und Init. NeroAPI.dll\n"));
		code = m_pBurnContext->NeroLoad ();
		if (code != EXITCODE_OK)
		{
			CString sError;
			code = EXITCODE_WRONG_NERO_VERSION;
			sError.Format(_T("Error: %s\n"), GetTextualExitCode (code));
			m_pBurnContext->AddMessageLine(sError);
			m_pBurnContext->Exit (code);
			return code;
		}
		
		//CheckImapi NICHT unter Win95/98/ME/2000/NT aufrufen, gibt exception handler
		//CheckImapi nur unter XP aufrufen
		COsVersionInfo osinfo;

		if(osinfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) //Win95/98/ME
		{
			if(!osinfo.IsWin95())
			{
				m_pBurnContext->InitNeroAPI ();
			}
		}
		else if (osinfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
		{
			if(osinfo.IsWinXP())
			{
				if(CheckImapi())
				{
					m_pBurnContext->InitNeroAPI ();
				}
			}
			else
			{
				m_pBurnContext->InitNeroAPI ();
			}
		}
	}

	if (COMMAND_VERSION == m_params.GetCommand())
	{
		CString sVersion;

		//get version information.
		DWORD dwVersion = m_pBurnContext->CommandVersion(m_sVersionInstalled);

		//has to be at least version 5.5.8.2
		DWORD dwMinVersion = NERO_MINIMUM_VERSION;
	
		if(dwVersion >= dwMinVersion)
		{
			code = EXITCODE_OK;
		}
		else
		{
			//error codes are handled (traced) later
			code = EXITCODE_WRONG_NERO_VERSION;
		}
	}
	else
	{
		if (code == EXITCODE_OK)
		{
			// Get available drives.
			code = m_pBurnContext->GetAvailableDrives ();
			if (code == EXITCODE_OK)
			{
				if (COMMAND_LISTDRIVES == m_params.GetCommand())
				{
					// Just give a nice listing of all available drives.
					code = m_pBurnContext->CommandListDrives (m_params);
				}
				else
				{
					// Open the selected device. We didn't need this unitl now.
					code = m_pBurnContext->OpenDevice (m_params);
					if (code == EXITCODE_OK)
					{
						switch (m_params.GetCommand())
						{
							case COMMAND_ERASE:
								code = m_pBurnContext->CommandErase (m_params);
								break;

							case COMMAND_WRITE:
								code = m_pBurnContext->CommandWrite (m_params);
								break;

							case COMMAND_CDINFO:
								code = m_pBurnContext->CommandCDInfo (m_params);
								break;

							case COMMAND_READ:
								code = m_pBurnContext->CommandRead (m_params);
								break;

							case COMMAND_EJECT:
							case COMMAND_LOAD:
								code = m_pBurnContext->CommandEject (m_params);
								break;

							default:

								// This should never happen!
								_ASSERTE (FALSE);
								code = EXITCODE_INTERNAL_ERROR;
						}
					}
				}
			}
		}
	}

	m_pBurnContext->Exit (code);
	
	return code;	
}
/*********************************************************************************************
 Class		   : CNeroBurn
 Function      : CheckImapi 
 Description   : This function checks if the process Imapi.exe is currently running.
				 If it runs, this process will be terminated, because NeroApi.dll an only
				 be initialized when Imapi.exe is not running.

 Parameters: --
  
 Result type:  --

 Author: TKR
 created: October, 30 2002
 <TITLE CheckImapi >
 <GROUP CNeroBurn>
 <TOPICORDER 0>
 <KEYWORDS CNeroBurn, CheckImapi>
//*********************************************************************************************/
BOOL CNeroBurn::CheckImapi()
{

	BOOL bRet = FALSE;
	const DWORD dwSize = 1024;
	DWORD  i, nP, dwNeeded = 0;
	DWORD  dwProcess[dwSize];
	HANDLE hProcess = NULL;
	int    nlen;
	CString sImapi = _T("imapi.exe");
	_TCHAR szBaseName[256] = _T("");
	g_pfnEnumProcesses(&dwProcess[0], dwSize*sizeof(DWORD), &dwNeeded);
	nP = dwNeeded / sizeof(DWORD);
   
	//check all running processes
	for (i=0; i<nP; i++)
    {
		if (hProcess == NULL)
		{
			hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, dwProcess[i]);
		}
		 
		if (hProcess)
		{
			nlen = g_pfnGetModuleBaseName(hProcess, NULL,  szBaseName, 256);
            if (nlen)
            {
				// Name zum vergleichen
				if(sImapi.CompareNoCase(szBaseName) == 0)
				{
					bRet = TerminateProcess(hProcess, NULL);
					WaitForSingleObject(hProcess, 1000);
					WK_CLOSE_HANDLE(hProcess);
					break;
				}
            }
			WK_CLOSE_HANDLE(hProcess);
		}
		else
		{
			DWORD dwError = GetLastError();
			if (dwError == 5) // No permission
			{
				// oh well, we need to grant ourselves PROCESS_TERMINATE
				HANDLE hpWriteDAC = OpenProcess( WRITE_DAC, FALSE, dwProcess[i]);
				if (!hpWriteDAC )
				{
					 // hmm, we don't have permissions to modify the DACL...
					 // time to take ownership...
					HANDLE htok;
					TOKEN_PRIVILEGES tpOld;
					if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &htok ))
					{
						continue;
					}

					if (!EnablePrivilege(htok, SE_TAKE_OWNERSHIP_NAME, tpOld))
					{
						continue;
					}

					// SeTakeOwnershipPrivilege allows us to open objects with
					// WRITE_OWNER, but that's about it, so we'll update the owner,
					// and dupe the handle so we can get WRITE_DAC permissions.
					HANDLE hpWriteOwner = OpenProcess( WRITE_OWNER, FALSE, dwProcess[i]);
					if (!hpWriteOwner)
					{
					  continue;
					}

					BYTE buf[512]; // this should always be big enough
					DWORD cb = sizeof buf;
					if (!GetTokenInformation( htok, TokenUser, buf, cb, &cb ))
					{
					   continue;
					}

					if (SetSecurityInfo(hpWriteOwner, SE_KERNEL_OBJECT, OWNER_SECURITY_INFORMATION,
							             reinterpret_cast<TOKEN_USER*>(buf)->User.Sid, 0, 0, 0 ) != 0)
					{
					   continue;
					}
					// now that we're the owner, we've implicitly got WRITE_DAC
					// permissions, so ask the system to reevaluate our request,
					// giving us a handle with WRITE_DAC permissions
					if (!DuplicateHandle(GetCurrentProcess(), hpWriteOwner,
							 					GetCurrentProcess(), &hpWriteDAC,
													WRITE_DAC, FALSE, 0 ))
					{
					   continue;
					}
					 // not truly necessary in this app,
					 // but included for completeness
					RestorePrivilege( htok, tpOld );
				}

				if ( hpWriteDAC )
				{
					// we've now got a handle that allows us WRITE_DAC permission
					DWORD dwDesiredAccess = PROCESS_ALL_ACCESS;
					AdjustDacl(hpWriteDAC, dwDesiredAccess);
					// now that we've granted ourselves permission to terminate
					// the process, ask the system to reevaluate our request,
					// giving us a handle with PROCESS_TERMINATE permissions

					if ( !DuplicateHandle(GetCurrentProcess(), hpWriteDAC,
												 GetCurrentProcess(), &hProcess,
												dwDesiredAccess, FALSE, 0 ) )
					{

						WK_CLOSE_HANDLE(hProcess);
					   continue;
					}
					CloseHandle(hpWriteDAC);
					i--;
					continue;
				}
			}
		}

     }
	 if (i==nP)
	 {
		 bRet = TRUE;
	 }
	return bRet;;
}

//*********************************************************************************************
BOOL CNeroBurn::AdjustDacl(HANDLE h, DWORD dwDesiredAccess)
{
    // the WORLD Sid is trivial to form programmatically (S-1-1-0)
    SID world = { SID_REVISION, 1, SECURITY_WORLD_SID_AUTHORITY, 0 };

    EXPLICIT_ACCESS ea =
    {
        dwDesiredAccess,
        SET_ACCESS,
        NO_INHERITANCE,
        {
            0, NO_MULTIPLE_TRUSTEE,
            TRUSTEE_IS_SID,
            TRUSTEE_IS_USER,
            reinterpret_cast<_TCHAR*>( &world )
        }
    };
    ACL* pdacl = 0;
    DWORD err = SetEntriesInAcl( 1, &ea, 0, &pdacl );
    if ( err ) 
	{
		return FALSE;
	}

    err = SetSecurityInfo( h, SE_KERNEL_OBJECT, DACL_SECURITY_INFORMATION,
                           0, 0, pdacl, 0 );
    if ( err ) 
	{
		return FALSE;
	}

    LocalFree( pdacl );

	return TRUE;
}

//*********************************************************************************************
BOOL CNeroBurn::EnablePrivilege( HANDLE htok, LPCTSTR pszPriv,
                      TOKEN_PRIVILEGES& tpOld )
{
    TOKEN_PRIVILEGES tp;
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    if ( !LookupPrivilegeValue( 0, pszPriv, &tp.Privileges[0].Luid ) )
		 return false;

    // htok must have been opened with the following permissions:
    // TOKEN_QUERY (to get the old priv setting)
    // TOKEN_ADJUST_PRIVILEGES (to adjust the priv)
    DWORD cbOld = sizeof tpOld;
    if ( !AdjustTokenPrivileges( htok, FALSE, &tp, cbOld, &tpOld, &cbOld ) )
		 return false;

    // Note that AdjustTokenPrivileges may succeed, and yet
    // some privileges weren't actually adjusted.
    // You've got to check GetLastError() to be sure!
    return ( ERROR_NOT_ALL_ASSIGNED != GetLastError() );
}
 
//*********************************************************************************************
// Corresponding restoration helper function
BOOL CNeroBurn::RestorePrivilege( HANDLE htok, const TOKEN_PRIVILEGES& tpOld )
{
    if ( !AdjustTokenPrivileges( htok, FALSE,
                                 const_cast<TOKEN_PRIVILEGES*>(&tpOld),
                                 0, 0, 0 ) )
	{
		return FALSE;
	}
	return TRUE;
}
/*********************************************************************************************
 Class		   : CNeroBurn
 Function      : BurnThread 
 Description   : This function is the thread function which directs all processes for burning 
				 CD`s or DVD`s. It gets the current information (MainFunktion) what`s to do next. 
				 This MainFunction is set by a parsed order from the CIPC. After this MainFunction
				 finished this function "BurnThread" ends and the burn thread will be destroyed.

 Parameters: --
  
 Result type:  --

 Author: TKR
 created: July, 16 2002
 <TITLE BurnThread >
 <GROUP CNeroBurn>
 <TOPICORDER 0>
 <KEYWORDS CNeroBurn, BurnThread>
*********************************************************************************************/
void CNeroBurn::BurnThread(MainFunction mf)
{
	EXITCODE code;
	CAcdcDlg* pDlg = (CAcdcDlg*)theApp.GetMainWnd();

	if(m_pBurnContext)
	{
		//reset all parameters before executing a new MainFunction, because NeroAPI can handle
		//one MainFunction in one BurnContext object only.
		m_pBurnContext->ResetParams();

		//all MainFunctions are the main commands from CIPC
		switch(mf)
		{

		case MF_CD_DRIVES:
			code = OnCDListDrives();
			break;

		case MF_CD_INFO:		//get information about loaded medium (CD-R/CD-RW/DVD)
			{
			code = OnCDInfo();

			if(pDlg)
			{
				if(code == EXITCODE_OK) //an error occoured
				{
					//set all nessessary data in CIPCDrive 
					code = pDlg->SetVolumeProperties(m_pBurnContext);
				}

				//send exitcode to main window 
				::PostMessage(pDlg->m_hWnd,WM_BURN_CDINFO, mf, code);

			}
			else
			{
				WK_TRACE(_T("Error: OnCDInfo(), No Main Window available\n"));
			}
			break;
			}

		case MF_CD_WRITE_ISO:	//write CD in ISO format
			code = OnCDWriteIso();
			if(pDlg)
			{
				//send to main window that no medium found in drive 
				::PostMessage(pDlg->m_hWnd,WM_BURN_WRITEISO, mf, code);
			}
			else
			{
				WK_TRACE(_T("Error: OnCDWriteIso(), No Main Window available\n"));
			}
			break;

		case MF_CD_WRITE_VCD:	//write CD in VideoCD format
			code = OnCDWriteVCD();
			break;
		case MF_CD_WRITE_SVCD:	//write CD in SuperVideoCD format
			code = OnCDWriteSVCD();
			break;
		case MF_EJECT:			//eject medium
			code = OnEject();
			break;
		case MF_WAIT_IN_USE:
			break;

		default:
			WK_TRACE(_T("BurnThread: Error, no MainFunction. Quit BurnThread. \n"));
			break;
		}
	}

}

/*********************************************************************************************
 Class		   : CNeroBurn
 Function      : DeleteBurnContext 
 Description   : This function deletes the BurnContext object. In the destructor of BurnContext
				 the function "NeroAPIFree()" is called. This function ends all connections
				 to the NeroAPI.dll.
				 Deleting (ending connections) takes several minutes, so the main window will receive two
				 messages, one before deleting, one after deleting has finished.
				 In the main window this "waiting for delete" is shown.

 Parameters: -- 

 Result type:  --

 Author: TKR
 created: September, 13 2002
 <TITLE DeleteBurnContext >
 <GROUP CNeroBurn>
 <TOPICORDER 0>
 <KEYWORDS CNeroBurn, DeleteBurnContext>
*********************************************************************************************/
void CNeroBurn::DeleteBurnContext()
{
	CAcdcDlg* pDlg = (CAcdcDlg*)theApp.GetMainWnd();

	if(pDlg)
	{
		SetNewMessage(_T("please wait, closing Nero API connection... \n"));
		::PostMessage(pDlg->m_hWnd, WM_BURN_NEW_MESSAGE, NM_NERO_BURN_THREAD, NULL);
	}

	WK_DELETE(m_pBurnContext);

	if(pDlg)
	{
		SetNewMessage(_T("Nero API connection closed \n"));
		::PostMessage(pDlg->m_hWnd, WM_BURN_NEW_MESSAGE, NM_NERO_BURN_THREAD, NULL);
	}
}

/*********************************************************************************************
 Class		   : CNeroBurn
 Function      : GetApiVersion 
 Description   : This function checks the installed NeroAPI.

 Parameters: -- 

 Result type:  returns 0 if successful or an error code if not  (EXITCODE)

 Author: TKR
 created: July, 16 2002
 <TITLE GetApiVersion >
 <GROUP CNeroBurn>
 <TOPICORDER 0>
 <KEYWORDS CNeroBurn, GetApiVersion>
*********************************************************************************************/
EXITCODE CNeroBurn::GetApiVersion()
{		
	EXITCODE code;
//	CStringArray saParams;
	CStringArray saParams;
	saParams.Add(_T("--version"));
	saParams.Add(_T("--no_error_log"));
	
	WK_TRACE(_T("\n"));
	WK_TRACE(_T("get NeroAPI version\n"));

	code = MainApp(saParams);
	return code;
}


/*********************************************************************************************
 Class		   : CNeroBurn
 Function      : GetParams 
 Description   : This function decodes the argument list and fills the PARAMETERS object.

 Parameters:  
  params:  (O):  reference to the parameters  (PARAMETERS &)
  argc:	   (O):  numbers of parsed parameters  (int)
  argc:	   (O):  list of the parsed parameters  (char ** argv)

 Result type:  returns 0 if the parameterlist (params) was successfully filled  (BOOL)

 Author: TKR
 created: July, 16 2002
 <TITLE GetParams >
 <GROUP CNeroBurn>
 <TOPICORDER 0>
 <KEYWORDS CNeroBurn, GetParams>
*********************************************************************************************/
BOOL CNeroBurn::GetParams(PARAMETERS & params, CStringArray& saParams)
{
	if ((0 == saParams.GetSize()) && (params.GetProcessedParameterFile() == false))
	{
		WK_TRACE (_T("CAcdcDlg::GetParams() Bad parameters usage  \n"));
		return FALSE;
	}

	COMMAND_LINE_ERRORS ecl = CLE_NO_ERROR;

	int iSize = saParams.GetSize();
	try
	{
		for (int i=0; i<iSize; i++)
		{
			if (CLE_NO_ERROR != ecl)
			{
				throw ecl;
			}
			else if (!_tcsicmp (saParams.GetAt(i), _T("--listdrives")))
			{
				ecl = params.SetCommand(COMMAND_LISTDRIVES);
			}
			else if (!_tcsicmp (saParams.GetAt(i), _T("--version")))
			{
				ecl = params.SetCommand(COMMAND_VERSION);
			}
			else if (!_tcsicmp (saParams.GetAt(i), _T("--cdinfo")))
			{
				ecl = params.SetCommand(COMMAND_CDINFO);
			}
			else if (!_tcsicmp (saParams.GetAt(i), _T("--read")))
			{
				ecl = params.SetCommand(COMMAND_READ);
			}
			else if (!_tcsicmp (saParams.GetAt(i), _T("--write")))
			{
				ecl = params.SetCommand(COMMAND_WRITE);
			}
			else if (!_tcsicmp (saParams.GetAt(i), _T("--erase")))
			{
				ecl = params.SetCommand(COMMAND_ERASE);
			}
			else if (!_tcsicmp (saParams.GetAt(i), _T("--eject")))
			{
				ecl = params.SetCommand(COMMAND_EJECT);
			}
			else if (!_tcsicmp (saParams.GetAt(i), _T("--load")))
			{
				ecl = params.SetCommand(COMMAND_LOAD);
			}
			else if (!_tcsicmp (saParams.GetAt(i), _T("--drivename")))
			{
				ecl = params.SetDriveName(i, saParams);
			}
			else if (!_tcsicmp (saParams.GetAt(i), _T("--real")))
			{
				ecl = params.SetUseReal();
			}
			else if (!_tcsicmp (saParams.GetAt(i), _T("--tao")))
			{
				ecl = params.SetUseTAO();
			}
			else if (!_tcsicmp (saParams.GetAt(i), _T("--underrun_prot")))
			{
				ecl = params.SetUseUnderRunProt();
			}
			else if (!_tcsicmp (saParams.GetAt(i), _T("--no_error_log")))
			{
				ecl = params.SetWriteErrorLog();
			}
			else if (!_tcsicmp (saParams.GetAt(i), _T("--speedtest")))
			{
				ecl = params.SetSpeedTest();
			}
			else if (!_tcsicmp (saParams.GetAt(i), _T("--enable_abort")))
			{
				ecl = params.SetEnableAbort();
			}
			else if (!_tcsicmp (saParams.GetAt(i), _T("--close_session")))
			{
				ecl = params.SetCloseSession();
			}
			else if (!_tcsicmp (saParams.GetAt(i), _T("--detect_non_empty_cdrw")))
			{
				ecl = params.SetDetectNonEmptyCDRW();
			}
			else if (!_tcsicmp (saParams.GetAt(i), _T("--cd_text")))
			{
				ecl = params.SetUseCDText();
			}
			else if (!_tcsicmp (saParams.GetAt(i), _T("--use_rockridge")))
			{
				ecl = params.SetUseRockridge();
			}
			else if (!_tcsicmp (saParams.GetAt(i), _T("--create_iso_fs")))
			{
				ecl = params.SetCreateIsoFs();
			}
			else if (!_tcsicmp (saParams.GetAt(i), _T("--create_udf_fs")))
			{
				ecl = params.SetCreateUdfFs();
			}
			else if (!_tcsicmp (saParams.GetAt(i), _T("--import_rockridge")))
			{
				ecl = params.SetImportRockridge();
			}
			else if (!_tcsicmp (saParams.GetAt(i), _T("--import_iso_only")))
			{
				ecl = params.SetImportIsoOnly();
			}
			else if (!_tcsicmp (saParams.GetAt(i), _T("--prefer_rockridge")))
			{
				ecl = params.SetPreferRockridge();
			}
			else if (!_tcsicmp (saParams.GetAt(i), _T("--disable_eject")))
			{
				ecl = params.SetDisableEject();
			}
			else if (!_tcsicmp (saParams.GetAt(i), _T("--verify")))
			{
				ecl = params.SetVerify();
			}
			else if (!_tcsicmp (saParams.GetAt(i), _T("--error_log")))
			{
				ecl = params.SetErrorLogName(i, saParams);
			}
			else if (!_tcsicmp (saParams.GetAt(i), _T("--artist")))
			{
				ecl = params.SetArtist(i, saParams);
			}
			else if (!_tcsicmp (saParams.GetAt(i), _T("--title")))
			{
				ecl = params.SetTitle(i, saParams);
			}
			else if (!_tcsicmp (saParams.GetAt(i), _T("--speed")))
			{
				ecl = params.SetSpeed(i, saParams);
			}
			else if (!_tcsicmp (saParams.GetAt(i), _T("--import")))
			{
				ecl = params.SetSessionToImport(i, saParams);
			}
			else if (!_tcsicmp (saParams.GetAt(i), _T("--cdextra")))
			{
				params.SetCDExtra();
			}
			else if (!_tcsicmp (saParams.GetAt(i), _T("--iso")))
			{
				ecl = params.SetISOSelected(i, saParams);
			}
			else if (!_tcsicmp (saParams.GetAt(i), _T("--iso-no-joliet")))
			{
				params.SetUseJoliet();
			}
			else if (!_tcsicmp (saParams.GetAt(i), _T("--iso-mode2")))
			{
				params.SetUseMode2();
			}
			else if (!_tcsicmp (saParams.GetAt(i), _T("--audio")))
			{
				ecl = params.SetAudioSelected();
			}
			else if (!_tcsicmp (saParams.GetAt(i), _T("--videocd")))
			{
				ecl = params.SetBurnType(BURNTYPE_VIDEOCD);
			}
			else if (!_tcsicmp (saParams.GetAt(i), _T("--svideocd")))
			{
				ecl = params.SetBurnType(BURNTYPE_SVIDEOCD);
			}
			else if (!_tcsicmp (saParams.GetAt(i), _T("--image")))
			{
				params.SetImageBurnType(i, saParams);
			}
			else if (!_tcsicmp (saParams.GetAt(i), _T("--freestyle_mode1")) ||
					!_tcsicmp (saParams.GetAt(i), _T("--freestyle_mode2")) ||
					!_tcsicmp (saParams.GetAt(i), _T("--freestyle_audio")))
			{
				params.SetFreestyleBurnType(i, saParams);
			}
			else if (!_tcsicmp (saParams.GetAt(i), _T("--entire")))
			{
				ecl = params.SetEraseMode(NEROAPI_ERASE_ENTIRE);
			}
			else if (!_tcsicmp (saParams.GetAt(i), _T("--dvd")))
			{
				ecl = params.SetUseDVD();
			}
			else if (!_tcsicmp (saParams.GetAt(i), _T("--nero_log_timestamp")))
			{
				ecl = params.SetNeroLogTimestamp();
			}
			else if (!_tcsicmp (saParams.GetAt(i), _T("--force_erase_cdrw")))
			{
				ecl = params.SetForceEraseCDRW();
			}

			else if ((saParams.GetAt(i).GetAt(0)) == _T('@'))
			{
				if (params.GetProcessedParameterFile() == true)
				{
					throw CLE_PARAMETER_FILE_ALREADY_PROCESSED;
				}

				ecl = params.SetProcessedParameterFile();
				if (CLE_NO_ERROR != ecl)
				{
					throw ecl;
				}

				CWK_String str = saParams.GetAt(i).Right(saParams.GetAt(i).GetLength()-1);
				if (!ReadParameterFile (params, str))
				{
					return FALSE;
				}
			}

			else if (!_tcsncmp (saParams.GetAt(i), _T("--"), 2))
			{
				ecl = params.AddAudioReadTrack(i, saParams);
			}
			else
			{
				ecl = params.AddAudioIsoFreestyleWriteTrack(i, saParams);
			}
		}

		// Make sure we had no error during
		// the last loop execution.

		if (CLE_NO_ERROR != ecl)
		{
			throw ecl;
		}

		if (params.GetCommand() == COMMAND_WRITE && params.GetBurnType() == BURNTYPE_UNKNOWN)
		{
			throw CLE_BURN_TYPE_NOT_SPECIFIED;
		}

		if ((params.GetBurnType() == BURNTYPE_ISOAUDIO ||
			params.GetBurnType() == BURNTYPE_SVIDEOCD ||
			params.GetBurnType() == BURNTYPE_VIDEOCD) &&
			params.GetNumberOfTracks() <= 0 && params.GetFileListSize() == 0)
		{
			throw CLE_NEED_TRACKS;
		}
	}
	catch (COMMAND_LINE_ERRORS e)
	{
		WK_TRACE (_T("%s\n"), s_cle[e]);
		WK_TRACE (_T("CAcdcDlg::GetParams() Bad parameters usage  \n"));
		return FALSE;
	}

	return TRUE;	
}
/*********************************************************************************************
 Class		   : CNeroBurn
 Function      : OnCDListDrives 
 Description   : This function calls the MainApp with all neccessary parameters to list
				 all available CD-RW drives.

 Parameters: -- 

 Result type:  returns 0 if successful or an error code if not  (EXITCODE)

 Author: TKR
 created: October, 10 2002
 <TITLE OnCDListDrives >
 <GROUP CNeroBurn>
 <TOPICORDER 0>
 <KEYWORDS CNeroBurn, OnCDListDrives>
*********************************************************************************************/
EXITCODE CNeroBurn::OnCDListDrives() 
{
	EXITCODE code;
	CStringArray saParams;

	WK_TRACE(_T("\n"));
	WK_TRACE(_T("get available drives\n"));

	saParams.Add(_T("--listdrives"));
	saParams.Add(_T("--no_error_log"));

//	saParams.Add("@params.txt");
	code = MainApp(saParams);

	return code;
}
/*********************************************************************************************
 Class		   : CNeroBurn
 Function      : OnCDInfo 
 Description   : This function calls the MainApp with all neccessary parameters check
				 the inserted medium and it`s contents.

 Parameters: -- 

 Result type:  returns 0 if successful or an error code if not  (EXITCODE)

 Author: TKR
 created: July, 16 2002
 <TITLE OnCDInfo >
 <GROUP CNeroBurn>
 <TOPICORDER 0>
 <KEYWORDS CNeroBurn, OnCDInfo>
*********************************************************************************************/
EXITCODE CNeroBurn::OnCDInfo() 
{

	EXITCODE code;
	CStringArray saParams;

	WK_TRACE(_T("\n"));
	WK_TRACE(_T("get medium Info\n"));

	saParams.Add(_T("--cdinfo"));
	saParams.Add(_T("--no_error_log"));
	saParams.Add(_T("--drivename"));
	saParams.Add(m_sDriveLetter);	


//	saParams.Add("@params.txt");
	code = MainApp(saParams);

	return code;
}

/*********************************************************************************************
 Class		   : CNeroBurn
 Function      : OnCDWriteIso 
 Description   : This function calls the MainApp with all neccessary parameters to burn a 
				 CD in ISO format.

 Parameters: -- 

 Result type:  returns 0 if successful or an error code if not  (EXITCODE) 

 Author: TKR
 created: July, 16 2002
 <TITLE OnCDWriteIso >
 <GROUP CNeroBurn>
 <TOPICORDER 0>
 <KEYWORDS CNeroBurn, OnCDWriteIso>
*********************************************************************************************/
EXITCODE CNeroBurn::OnCDWriteIso()
{
	EXITCODE code;
	CStringArray saFiles, saParams;
	CString sCDName;

	CAcdcDlg* pDlg = (CAcdcDlg*)theApp.GetMainWnd();
	if(pDlg)
	{
		pDlg->GetFilesToBackup(saFiles);

		CSystemTime sysTime;
		CString sDateTime;
		sysTime.GetLocalTime();
		WORD wYear = sysTime.GetYear();
		CString sYear;
		sYear.Format(_T("%4d"), wYear);
		sYear = sYear.Right(2);

		//sDateTime maximal 11 zeichen lang
		sDateTime.Format(_T("%02d%02d%s_%02d%02d"),
				sysTime.GetDay(),
				sysTime.GetMonth(),
				//sysTime.GetYear(),
				sYear,
				sysTime.GetHour(),
				sysTime.GetMinute(),
				sysTime.GetSecond()
				);
		sCDName = sDateTime;

	}

	for(int j = 0; j < saFiles.GetSize(); j++)
	{
		WK_TRACE(_T("File: %s\n"), saFiles.GetAt(j));
	}
//TODO TKR Multi Session wenn gewünscht
	//info zu multibackups von DVD`s
	//prinzipiell ist ein multibackup auf DVD`s möglich.
	//Parameter "--tao" dazu nicht verwenden, wird bei DVD`s nicht unterstützt
	//Parameter "--import" importiert eine session
	//Parameter "00" ist die Session Nummer (bei DVD`s gibt es immer nur eine einzige Session
	//in die später dann weitere track geschrieben werden können (MultiSession), die Sessionnummer
	//bleibt aber immer bei "00", es gibt immer nur eine einzige Session, Zählung beginnt bei "00"
	//Parameter "--import_iso_only" erforderlich
	//Parameter "--close_session" erforderlich, da sonst die DVD abgeschlossen (fixiert) wird



	saParams.Add(_T("--write"));
	saParams.Add(_T("--drivename"));
	saParams.Add(m_sDriveLetter);		
	saParams.Add(_T("--iso"));
	saParams.Add(sCDName);
//	saParams.Add(_T("--import"));
//	saParams.Add(_T("00"));
//	saParams.Add(_T("--import_iso_only"));
	saParams.Add(_T("--real"));
	saParams.Add(_T("--enable_abort"));
	saParams.Add(_T("--underrun_prot"));
	saParams.Add(_T("--detect_non_empty_cdrw"));
	saParams.Add(_T("--force_erase_cdrw"));
	saParams.Add(_T("--no_error_log"));
  
	WK_TRACE(_T("\n"));
	if(m_bIsDVD)
	{
		saParams.Add(_T("--dvd"));

		//das Brennen von mind. 1 GB (Generating DVD high compatibility borders) lässt sich
		//mittels RegEintrag "BurnAtLeast30mm" setzen auf "0" in HKLM und HKCU nicht abschalten
		WK_TRACE(_T("write ISO DVD \n"));
	}
	else
	{	
		WK_TRACE(_T("write ISO medium \n"));
	}
	saParams.Append(saFiles);

	code = MainApp(saParams);
	return code;
}

/*********************************************************************************************
 Class		   : CNeroBurn
 Function      : OnCDWriteVCD 
 Description   : This function calls the MainApp with all neccessary parameters to burn a 
				 VideoCD.

 Parameters: -- 

 Result type:  returns 0 if successful or an error code if not  (EXITCODE) 

 Author: TKR
 created: July, 16 2002
 <TITLE OnCDWriteVCD >
 <GROUP CNeroBurn>
 <TOPICORDER 0>
 <KEYWORDS CNeroBurn, OnCDWriteVCD>
*********************************************************************************************/
EXITCODE CNeroBurn::OnCDWriteVCD()
{
	WK_TRACE(_T("Write VCD not yet implemented\n"));
/*
	char *c[] =  {"--write", "--drivename", "g", "--videocd", "--real", 
							 "--enable_abort", "--detect_non_empty_cdrw", "--force_erase_cdrw", 
							 "--underrun_prot","--iso", "VCDTest", "d:\\nerotest\\test1.mpg"};
	MainApp(12, c);
*/
	return EXITCODE_OK;
}

/*********************************************************************************************
 Class		   : CNeroBurn
 Function      : OnCDWriteSVCD 
 Description   : This function calls the MainApp with all neccessary parameters to burn a 
				 SuperVideoCD.

 Parameters: -- 

 Result type:  returns 0 if successful or an error code if not  (EXITCODE) 

 Author: TKR
 created: July, 16 2002
 <TITLE OnCDWriteSVCD >
 <GROUP CNeroBurn>
 <TOPICORDER 0>
 <KEYWORDS CNeroBurn, OnCDWriteSVCD>
*********************************************************************************************/
EXITCODE CNeroBurn::OnCDWriteSVCD()
{
	WK_TRACE(_T("Write S-VCD not yet implemented\n"));
/*
	char *c[] =  {"--write", "--drivename", "g", "--svideocd", "--real", 
							 "--enable_abort", "--underrun_prot", "d:\\nerotest\\svcdtest.mpg"};
	MainApp(8, c);
*/
	return EXITCODE_OK;
}

/*********************************************************************************************
 Class		   : CNeroBurn
 Function      : OnEject 
 Description   : This function calls the MainApp with all neccessary parameters to eject 
				 the medium from the CD / DVD drive.

 Parameters: -- 

 Result type:  returns 0 if successful or an error code if not  (EXITCODE) 

 Author: TKR
 created: July, 16 2002
 <TITLE OnEject >
 <GROUP CNeroBurn>
 <TOPICORDER 0>
 <KEYWORDS CNeroBurn, OnEject>
*********************************************************************************************/
EXITCODE CNeroBurn::OnEject()
{
	EXITCODE code;
	CStringArray saParams;

	saParams.Add(_T("--eject"));
	saParams.Add(_T("--drivename"));
	saParams.Add(m_sDriveLetter);		
	saParams.Add(_T("--no_error_log"));
	
	WK_TRACE(_T("\n"));
	WK_TRACE(_T("eject medium\n"));

	code = MainApp(saParams);
	return code;
}

/*********************************************************************************************
 Class		   : CNeroBurn
 Function      : OnError 
 Description   : This function handles all errors.

 Parameters: -- 

 Result type:  -- 

 Author: TKR
 created: July, 16 2002
 <TITLE OnError >
 <GROUP CNeroBurn>
 <TOPICORDER 0>
 <KEYWORDS CNeroBurn, OnError>
*********************************************************************************************/
void CNeroBurn::OnError()
{
	WK_TRACE(_T("OnError not yet implemented\n"));
}

/*********************************************************************************************
 Class		   : CNeroBurn
 Function      : ReadParameterFile 
 Description   : ReadParameterFile is responsible for reading and parsing the parameter list from a
				 disk file. Once the parameters are parsed, they are passed on to the getopt
				 function for decoding. 
				 We not use this function because we only parse the parameters, we don`t parse
				 a disk file. But it is implemented because it is possible to parse a disk file.

 Parameters:  
  params:	  (O):  reference to the parameters  (PARAMETERS &)
  psFilename: (O):  the disk file which holds the parameters  (LPCSTR)

 Result type:  returns 0 if the parameterlist (params) was successfully filled  (BOOL)

 Author: TKR
 created: July, 16 2002
 <TITLE ReadParameterFile >
 <GROUP CNeroBurn>
 <TOPICORDER 0>
 <KEYWORDS CNeroBurn, ReadParameterFile>
*********************************************************************************************/
BOOL CNeroBurn::ReadParameterFile (PARAMETERS & params, LPCSTR psFilename)
{

	FILE * fh;
	char ch;
	int iStart = -1;
	bool bStartWasQuote = false;
	bool bStartWasSemicolon = false;
	bool bLastWasBackslash = false;
	

	static CSimpleStringArray args;
	int iSize = args.GetSize();
	args.Clear();	//da mehrere Aufrufe nacheinander möglich, statisches Array erstmal wieder leeren
	iSize = args.GetSize();

	// Open the file in binary mode. Text mode retrieves a DOS newline
	// as 0x0A, though it really is 0x0D+0x0A, thus compromising
	// the command length calculation.
	fh = fopen (psFilename, "rb");
	if (NULL == fh)
	{
		WK_TRACE (_T("Could not open parameter file '%s'\n"), psFilename);
		return false;
	}

	// The following code parses the file into argc/argv type of arguments.
	do
	{
		ch = (char)getc (fh);

		// Handle the start of a command or parameter string
		if (!isspace (ch) && ch != EOF && iStart == -1)
		{
			// ftell returns the current position of the file pointer
			// Here we are at start of the string.
			iStart = ftell (fh);

			// Check if a quote character was read and remember it.
			// This facilitates usage of blanks in file names.
			bStartWasQuote = ch == _T('"');

			// Check and remember that a comment was started
			// Comments are discarded later.
			bStartWasSemicolon = ch == _T(';');

			// If the string started with a quote, we don't want that,
			// so we leave the file pointer on the current position.
			if (!bStartWasQuote)
			{
				// We did not start with a quote, so lets set the 
				// start value, which is one less than the current
				// position
				iStart--;
			}
		}

		// Handle the end of comments
		else if (iStart != -1 && true == bStartWasSemicolon && 
						(isspace(ch) || ch == EOF))
		{
			// Comments are discarded
			if (ch == _T('\n'))
			{
				iStart = -1;
			}

		}

		// Handle the end of a command or parameter string
		else if (iStart != -1 && ((isspace (ch) && !bStartWasQuote) ||
								(ch == _T('"') && bStartWasQuote) ||
								 ch == _T(';') || ch == EOF))
		{
			// Get the current file pointer position
			// and calculate the command string length
			int iCurPos = ftell (fh);
			int iLen = iCurPos - iStart;

			// Set the file pointer to the start of the command
			// by moving it back by iLen bytes.
			fseek (fh, -iLen, SEEK_CUR);

			// Normally getc reads a character and then moves the file
			// pointer to the next character. At the end of the input stream,
			// the file pointer cannot go beyond EOF,
			// thus we must not decrement the length in that case because we
			// would delete parts of a command string.
			if (ch != EOF)
			{
				iLen--;
			}

			// Allocate enough space for the command string and the 
			// termination character
			LPSTR p = new char [iLen + 1];
			if (!p)
			{
				WK_TRACE (_T("Out of memory while parsing parameter file '%s'\n"), psFilename);
				fclose (fh);
				return false;
			}

			// Copy the command string
			fread (p, 1, iLen, fh);
			p[iLen] = 0;

			// Return to the previous last position
			// by seting the file pointer to an absolute position
			fseek (fh, iCurPos, SEEK_SET);

			// Add the string to the string vector
			args.Add (p);


		    // Make sure we discard the end of the line even
		    // if there was no blank between the command
		    // line parameter and a comment character ";"
		    bStartWasSemicolon = ch == _T(';');

		    // If the command was terminated by ";" we 
		    // do not start another command yet.
		    if (false == bStartWasSemicolon)
			{
				// Set the flag to get the next command
				iStart = -1;
			}
		}
		else if (bLastWasBackslash && ch != EOF)
		{
			if (ch != _T('"') && ch != _T('\\'))
			{
				ungetc (ch, fh);
			}

			bLastWasBackslash = false;
		}
		
	} while (ch != EOF);

	fclose (fh);

	char ** argv = new char*[args.vect.size ()];
	if (!argv)
	{
		WK_TRACE (_T("Out of memory while parsing parameter file '%s'\n"), psFilename);
		return false;
	}

	for (int i = 0; i < (int)(args.vect.size()); i ++)
	{
		argv[i] = args.vect[i];
	}

	CStringArray saParamsFromFile;
	
	int j = args.vect.size();
	CString s = args.vect[0];
	for(int k=0; k < j; k++)
	{
		saParamsFromFile.Add(args.vect[k]);
	}

	CString s1 = saParamsFromFile.GetAt(0);
	CString s2 = saParamsFromFile.GetAt(1);
	CString s3 = saParamsFromFile.GetAt(2);

	BOOL bRet = GetParams(params, saParamsFromFile);

	delete [] argv;

	return bRet;

}

/*********************************************************************************************
 Class		   : CNeroBurn
 Function      : SetAbortFlag 
 Description   : This function sets the abort flag in the BurnContext Object.
				 Some Nero callback checks during Nero processes (erase, burn, ect.) this flag.
				 If it`s TRUE, the current Nero process will be terminated. 
				 This function here is called from the main application window when the
				 user want to abort the current Nero process.

 Parameters:  --

 Result type:  --

 Author: TKR
 created: August, 02 2002
 <TITLE SetAbortFlag >
 <GROUP CNeroBurn>
 <TOPICORDER 0>
 <KEYWORDS CNeroBurn, SetAbortFlag>
*********************************************************************************************/
void CNeroBurn::SetAbortFlag()
{
	if(m_pBurnContext)
	{
		m_pBurnContext->SetAbortFlag();
	}
}
/*********************************************************************************************/
void CNeroBurn::SetCDRWAbortFlag(BOOL bFlag)
{
	if(m_pBurnContext)
	{
		m_pBurnContext->SetCDRWAbortFlag(bFlag);
	}
}

/*********************************************************************************************/
void CNeroBurn::SetNewMessage(CString sNewMsg)
{
	m_saNewMessage.Add(sNewMsg);
}

/*********************************************************************************************/
CString CNeroBurn::GetNewMessage()
{
	CString sMessage;
	if(m_saNewMessage.GetSize()>0)
	{
		sMessage = m_saNewMessage.GetAt(0);
		m_saNewMessage.RemoveAt(0);
	}

	return sMessage;
}

/*********************************************************************************************/
int CNeroBurn::NeroExitCodeToDCErrorCode(EXITCODE code)
{
	int iRet = DC_ERROR_BURN_PROCESS_FAILED;
	switch(code)
	{
	case EXITCODE_OK:				
	case EXITCODE_EJECT_FAILED:	
		iRet = DC_ERROR_NO_ERROR; 
		break;

	case EXITCODE_BAD_USAGE:
	case EXITCODE_INTERNAL_ERROR:
	case EXITCODE_NO_SERIAL_NUMBER:	
	case EXITCODE_BAD_SERIAL_NUMBER:
	case EXITCODE_ERROR_OBTAINING_AVAILABLE_DRIVES:	
	case EXITCODE_MISSING_DRIVENAME:
	case EXITCODE_BURN_FAILED:	
	case EXITCODE_FUNCTION_NOT_ALLOWED:	
	case EXITCODE_TRACK_NOT_FOUND:
	case EXITCODE_UNKNOWN_FILE_TYPE:
	case EXITCODE_DAE_FAILED:
	case EXITCODE_ERROR_OPENNING_FILE:
	case EXITCODE_OUT_OF_MEMORY:	
	case EXITCODE_ERROR_DETERMINING_LENGTH_OF_FILE:
	case EXITCODE_BAD_IMPORT_SESSION_NUMBER:
	case EXITCODE_FAILED_TO_CREATE_ISO_TRACK:
	case EXITCODE_UNKNOWN:	
	case EXITCODE_UNKNOWN_MEDIUM:
		iRet = DC_ERROR_BURN_PROCESS_FAILED;
		break;

	case EXITCODE_NEROAPI_DLL_NOT_FOUND:	
	case EXITCODE_DEMOVERSION_EXPIRED:	
	case EXITCODE_WRONG_NERO_VERSION: //by TKR
	case EXITCODE_NO_BURN_SOFTWARE:	  //by TKR)	
		iRet = DC_ERROR_NO_BURN_SOFTWARE;
		break;

	case EXITCODE_NO_CD_INSERTED:	
	case EXITCODE_UNKNOWN_CD_FORMAT:
	case EXITCODE_ERROR_GETTING_CD_INFO:
		iRet = DC_ERROR_NO_MEDIUM; 
		break;
		
	case EXITCODE_NO_SUPPORT_FOR_CDRW:								
		iRet = DC_ERROR_NO_CDRW_SUPPORTED; 
		break;
		
	case EXITCODE_ERROR_ERASING_CDRW:								
		iRet = DC_ERROR_ERASE_CDRW_FAILED; 
		break;
		
	case EXITCODE_ERROR_OPENNING_DRIVE:	
	case EXITCODE_INVALID_DRIVE:	
	case EXITCODE_DRIVE_NOT_FOUND:
	case EXITCODE_DRIVE_NOT_ALLOWED:
		iRet = DC_ERROR_DRIVE_NOT_FOUND; 
		break;

	case EXITCODE_FILE_NOT_FOUND:								
		iRet = DC_ERROR_FILE_NOT_FOUND; 
		break;
		
	case EXITCODE_USER_ABORTED:								
		iRet = DC_ERROR_USER_ABORT; 
		break;

	default:
		iRet = DC_ERROR_BURN_PROCESS_FAILED;
		break;
	}
	return iRet;
}

/*********************************************************************************************/
BOOL CNeroBurn::GetCDRWEraseSuccess()
{
	BOOL bRet = TRUE;
	if(m_pBurnContext)
	{
		bRet = m_pBurnContext->GetCDRWEraseSuccess();
	}

	return bRet;
}

/*********************************************************************************************/
void CNeroBurn::SetNeroAPIInitialized(BOOL bIsInitialized)
{
	m_bNeroAPIInitialized = bIsInitialized;
}
/*********************************************************************************************/
BOOL CNeroBurn::IsNeroAPIInitialized()
{
	return m_bNeroAPIInitialized;
}

/*********************************************************************************************/
void CNeroBurn::SetFileNotFound(CString& sFileNotFound)
{
	m_sFileNotFound = sFileNotFound;
}
/*********************************************************************************************/
CString CNeroBurn::GetFileNotFound()
{
	return m_sFileNotFound;
}

/*********************************************************************************************/
void CNeroBurn::SetDriveLetter(CString sDriveLetter)
{

	CAcdcDlg* pACDC = theApp.GetAcdcDlg();
	CString sMsg;

	if(    WK_IS_WINDOW(pACDC)
		&& !sDriveLetter.IsEmpty())
	{
		if(sDriveLetter == _T('?'))
		{
			sMsg.Format(_T("no drive available"));
			pACDC->AppendMessage(sMsg);
		}
		else
		{
			sMsg.Format(_T("current drive: %s"), sDriveLetter);
			pACDC->AppendMessage(sMsg);
			pACDC->SetCurrentDrive(sDriveLetter);
			m_sDriveLetter = sDriveLetter;
		}
	}
}
/*********************************************************************************************/
CString CNeroBurn::GetDriveLetter()
{
	return m_sDriveLetter;
}
/*********************************************************************************************/
CBurnContext* CNeroBurn::GetBurnContext()
{
	return m_pBurnContext;
}
/*********************************************************************************************/
void CNeroBurn::SetIsDVD(BOOL bIsDVD)
{
	m_bIsDVD = bIsDVD;
}
/*********************************************************************************************/
BOOL CNeroBurn::IsDVD()
{
	return m_bIsDVD;
}
/*********************************************************************************************/
CProducer* CNeroBurn::GetProducer()
{
	return m_pProducer;
}
