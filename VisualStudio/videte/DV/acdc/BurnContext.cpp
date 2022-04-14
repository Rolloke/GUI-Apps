// BurnContext.cpp: implementation of the BurnContext class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "acdc.h"
#include "BurnContext.h"
#include "AcdcDlg.h"

#include "FindFiles.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// Set pointers to callback functions
NERO_PROGRESS CBurnContext::s_NeroProgress =
{
	ProgressCallback,
	AbortedCallback,
	AddLogLine,
	SetPhaseCallback,
	&s_NeroSettings,
	DisableAbortCallback,
	SetMajorPhaseCallback,
};

// NERO_SETTINGS is needed when calling NeroInit()
NERO_SETTINGS CBurnContext::s_NeroSettings =
{
	NULL,
	"ahead", "Nero - Burning Rom",
	"Nero.txt",
	{IdleCallback, &s_NeroSettings},
	{UserDialog, &s_NeroSettings}
};

static CBurnContext* m_pThis;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBurnContext::CBurnContext(PARAMETERS* params, CNeroBurn* pNeroBurn)
{
	// A PARAMETERS reference is required for 
	// the UserDialog callback.

	m_pThis	= this;
	m_bAborted = FALSE;
	m_NeroDeviceHandle = NULL;
	m_NeroDeviceInfos = NULL;
	m_pCDStamp = NULL;
	m_NeroCDInfo = NULL;
	m_bNeroInitialized = false;
	m_pNeroLastError = NULL;


	m_params = params;
	m_pNeroBurn = pNeroBurn;
	m_iCurrentProgress = 0;
	m_bDVDSecondProgress = FALSE;
	m_bCDRWEraseSuccess = FALSE;

	m_bCDRWAbortFlag	= FALSE;
	m_bCDRWEraseStart	= FALSE;

	InitCheckMediaType();
}

CBurnContext::~CBurnContext ()
{
	//it takes a few seconds to complete the function "NeroAPIFree()"
	NeroAPIFree();
}

/*********************************************************************************************
 Class		   : CBurnContext
 Function      : GetAvailableDrives 
 Description   : This function gets a list of available WORM and CDROM devices.

 Parameters:  --

 Result type:  returns 0 if successful or an error code if not  (EXITCODE)

 Author: TKR
 created: July, 12 2002
 <TITLE GetAvailableDrives >
 <GROUP CBurnContext>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, GetAvailableDrives>
*********************************************************************************************/
EXITCODE CBurnContext::GetAvailableDrives (void)
{
	// Make sure that NeroGetAvailableDrives has not been called before to prevent multiple 
	// allocation of memory for NERO_SCSI_DEVICE_INFOS.
//	_ASSERTE (NULL == m_NeroDeviceInfos);

	// NeroGetAvailableDrives returns a list of available WORM and CDROM devices.
	// The memory allocated for NERO_SCSI_DEVICE_INFOS must be freed with NeroFreeMem().
	// This function returns NULL for errors 
	
	//only search for available drives once
	if(m_NeroDeviceInfos == NULL)
	{
	
		//check for CDRW drives
//		m_NeroDeviceInfos = NeroGetAvailableDrivesEx (MEDIA_CD,NULL);

		//check for DVD drives
		m_NeroDeviceInfos = NeroGetAvailableDrivesEx (MEDIA_DVD_P,NULL);
	}

	if (NULL == m_NeroDeviceInfos)
	{
		GetLastErrorLogLine();
		return EXITCODE_ERROR_OBTAINING_AVAILABLE_DRIVES;
	}
	else
	{
		return EXITCODE_OK;
	}
}
/*********************************************************************************************
 Class		   : CBurnContext
 Function      : NeroLoad 
 Description   : This function initializes the NeroAPI.

 Parameters:  --

 Result type:  returns 0 if successful or an error code if not  (EXITCODE)

 Author: TKR
 created: July, 12 2002
 <TITLE NeroLoad >
 <GROUP CBurnContext>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, NeroLoad>
*********************************************************************************************/
EXITCODE CBurnContext::NeroLoad (void)
{
	// Finally, initialize NeroAPI.
	if (!TRUE == NeroAPIGlueConnect (NULL))
	{
		return EXITCODE_NEROAPI_DLL_NOT_FOUND;
	}

	// Set the flag.
	m_bNeroInitialized = true;
	m_pNeroBurn->SetNeroAPIInitialized(TRUE);

	return EXITCODE_OK;
}

/*********************************************************************************************
 Class		   : CBurnContext
 Function      : SetExtraWriteBuffer 
 Description   : Set extra memory to allocate during burn process.

 Parameters:  --

 Result type:  returns TRUE if successful (BOOL)

 Author: TKR
 created: July, 12 2002
 <TITLE SetExtraWriteBuffer >
 <GROUP CBurnContext>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, SetExtraWriteBuffer>
*********************************************************************************************/
BOOL CBurnContext::SetExtraWriteBuffer()
{
	BOOL bRet = FALSE;

	// open registry path "SOFTWARE\\Ahead\\Nero - Burning Rom\\Recorder", 
	//find key "WriteBufferSize" and set this key to 5 MB
	HKEY namekey;
	DWORD dwValue = 5 * 1024 * 1024;;
	LONG lResult;

	//get Nero installation path from Nero`s registry values
	lResult = RegOpenKeyEx(HKEY_CURRENT_USER, 
							_T("SOFTWARE\\Ahead\\Nero - Burning Rom\\Recorder"),
							0,
							KEY_WRITE,
							&namekey);
	if(lResult == ERROR_SUCCESS)
	{

		// Set new value
		lResult = RegSetValueEx(namekey, 
								_T("WriteBufferSize"), 
								NULL, 
								REG_DWORD, 
								(const BYTE*)&dwValue, 
								sizeof(DWORD));
		
		if(lResult == ERROR_SUCCESS)
		{
			bRet = TRUE;
		}

/*
		lResult = RegQueryValueEx(namekey, 
								_T("WriteBufferSize"), 
								NULL, 
								&dwType, 
								(LPBYTE)&dwValue,
								&dwCount);
		if(lResult == ERROR_SUCCESS)
		{
			if(dwValue)
		}
*/
		RegCloseKey(namekey);


	}

	return bRet;
}
/*********************************************************************************************
 Class		   : CBurnContext
 Function      : InitNeroAPI 
 Description   : This function queries the registry for serial number and initializes
				 the NeroAPI.

 Parameters:  --

 Result type:  returns 0 if successful or an error code if not  (EXITCODE)

 Author: TKR
 created: July, 12 2002
 <TITLE InitNeroAPI >
 <GROUP CBurnContext>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, InitNeroAPI>
*********************************************************************************************/
EXITCODE CBurnContext::InitNeroAPI (void)
{
	//set extra write buffer from Nero in Reg
	if(SetExtraWriteBuffer())
	{
		WK_TRACE(_T("neuer UltraBuffer gesetzt\n"));
	}
	else
	{
		WK_TRACE(_T("kein UltraBuffer gesetzt\n"));
	}

	// Provide the this-pointer for the UserDialog callback
	memset(&s_NeroSettings, 0, sizeof(s_NeroSettings));

	s_NeroSettings.nstUserDialog.ncCallbackFunction = UserDialog;
	s_NeroSettings.nstUserDialog.ncUserData = this;
	s_NeroSettings.nstIdle.ncCallbackFunction = IdleCallback;
	s_NeroSettings.nstIdle.ncUserData = this;
	s_NeroSettings.nstNeroFilesPath = "NeroFiles";
	s_NeroSettings.nstVendor = "ahead";
	s_NeroSettings.nstIdle.ncCallbackFunction = IdleCallback;
	s_NeroSettings.nstIdle.ncUserData = this;
	s_NeroSettings.nstSoftware = "Nero - Burning Rom";
	s_NeroSettings.nstUserDialog.ncCallbackFunction = UserDialog;
	s_NeroSettings.nstLanguageFile ="Nero.txt";

	memset(&s_NeroProgress, 0, sizeof(s_NeroProgress));
	s_NeroProgress.npAbortedCallback = AbortedCallback;
	s_NeroProgress.npAddLogLineCallback = AddLogLine;
	s_NeroProgress.npDisableAbortCallback = NULL;
	s_NeroProgress.npProgressCallback = ProgressCallback;
	s_NeroProgress.npSetPhaseCallback = SetPhaseCallback;
	s_NeroProgress.npSetMajorPhaseCallback = SetMajorPhaseCallback;
	s_NeroProgress.npUserData = this;
	m_NeroDeviceInfos = NULL;

	// Do the actual initialization and map the return value
	// into our EXITCODE.
	NEROAPI_INIT_ERROR initErr;
	initErr = NeroInit (&s_NeroSettings, NULL);
	
	if(initErr == NEROAPI_INIT_OK)
	{
		WK_TRACE(_T("**** NeroInit OK\n"));
	}
	else
	{
		WK_TRACE(_T("**** NeroInit Error\n"));
	}
	switch (initErr)
	{
		case NEROAPI_INIT_OK:
			return EXITCODE_OK;
		
		case NEROAPI_INIT_INVALID_SERIAL_NUM:
			return EXITCODE_BAD_SERIAL_NUMBER;

		case NEROAPI_INIT_DEMOVERSION_EXPIRED:
			return EXITCODE_DEMOVERSION_EXPIRED;
		
		case NEROAPI_INIT_UNSPECIFIED_ERROR:
		case NEROAPI_INIT_INVALID_ARGS:
		default:
			return EXITCODE_INTERNAL_ERROR;
	}
}

/*********************************************************************************************
 Class		   : CBurnContext
 Function      : OpenDevice 
 Description   : OpenDevice checks if params.m_psDriveName has a matching
				 drive in the list of availabe drives and opens it.

 Parameters:  
  params: (O):  reference to the parameters  (const PARAMETERS &)

 Result type:  returns 0 if successful or an error code if not  (EXITCODE)

 Author: TKR
 created: July, 12 2002
 <TITLE OpenDevice >
 <GROUP CBurnContext>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, OpenDevice>
*********************************************************************************************/
EXITCODE CBurnContext::OpenDevice (const PARAMETERS & params)
{
	if (NULL == params.GetDriveName())
	{
		// To open a device we must have its name or drive letter.
		return EXITCODE_MISSING_DRIVENAME;
	}

	// Enumerate drives and find the requested drive among them.
	for (int i = 0; i < (int)(m_NeroDeviceInfos->nsdisNumDevInfos); i++) 
	{
		NERO_SCSI_DEVICE_INFO nsdiShort = m_NeroDeviceInfos->nsdisDevInfos[i];

		// Check if the full device name has been supplied.
        // stricmp performs a lowercase comparison and returns 0 if the strings are identical.
		bool bFoundDeviceName = false;
		CString sDrive = params.GetDriveName();
		CString s = nsdiShort.nsdiDeviceName;
		if (0 == stricmp (params.GetDriveName(), nsdiShort.nsdiDeviceName))
		{
			bFoundDeviceName = true;
		}

		// Check if the user supplied drive name is only one character long
		// and see if it matches the drive letter
		bool bFoundDriveName = false;
		if (1 == strlen(params.GetDriveName()))
		{
			if (toupper(params.GetDriveName()[0]) == toupper(nsdiShort.nsdiDriveLetter))
			{
				bFoundDriveName = true;
			}
		}

		// If either a device name or drive name was found try to open the device
		if (bFoundDeviceName || bFoundDriveName)
		{
			m_NeroDeviceHandle = NeroOpenDevice (&m_NeroDeviceInfos->nsdisDevInfos[i]);
			if (NULL == m_NeroDeviceHandle) 
			{
				GetLastErrorLogLine();
				return EXITCODE_ERROR_OPENNING_DRIVE;
			}

			// We successfully retrieved a handle. Break the for-loop.
			break;
		}
	}
	
	// Output a list of available drives and return
	// an error if the requested device could not be found
	if (NULL == m_NeroDeviceHandle)
	{
		const CWK_String sDriveName = params.GetDriveName();
		WK_TRACE (_T("Drive '%s' not found, available are:\n"), (LPCTSTR)sDriveName);
		CommandListDrives (params);
		
		return EXITCODE_DRIVE_NOT_FOUND;
	}
	
	return EXITCODE_OK;
}

/*********************************************************************************************
 Class		   : CBurnContext
 Function      : GetBurnFlags 
 Description   : This function sets the appropriate burn flags according to the user
				 supplied parameters.

 Parameters:  
  params: (O):  reference to the parameters  (const PARAMETERS &)

 Result type:  returns the burn flags  (DWORD)

 Author: TKR
 created: July, 23 2002
 <TITLE GetBurnFlags >
 <GROUP CBurnContext>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, GetBurnFlags>
*********************************************************************************************/
DWORD CBurnContext::GetBurnFlags (const PARAMETERS & params)
{
	DWORD dwFlags;

	// Simulation or real mode
	if (true == params.GetUseReal())
	{
		dwFlags = NBF_WRITE;
	}
	else
	{
		dwFlags = NBF_SIMULATE;
	}

	// DAO (Disc At Once) or TAO (Track At Once)
	// TAO is default
	if (false == params.GetUseTAO())
	{
		dwFlags |= NBF_DAO;
	}

	// Disable Abort
	// The disable abort callback will be called.
	if (false == params.GetEnableAbort())
	{
		dwFlags |= NBF_DISABLE_ABORT;
	}

	// Perform source speed test first
	if (true == params.GetUseSpeedTest())
	{
		dwFlags |= NBF_SPEED_TEST;
	}

	// Close session after write, not the whole disc
	if (true == params.GetCloseSession())
	{
		dwFlags |= NBF_CLOSE_SESSION;
	}

	// Buffer underrun protection for safer burning
	if (true == params.GetUseUnderRunProt())
	{
		dwFlags |= NBF_BUF_UNDERRUN_PROT;
	}

	// Detect non-empty CDRW
	// The DLG_NON_EMPTY_CDRW user callback will be called when trying
	// to burn onto a non empty CDRW
	if (true == params.GetDetectNonEmptyCDRW())
	{
		dwFlags |= NBF_DETECT_NON_EMPTY_CDRW;
	}

	// Enable CD text writing.
	// Will be ignored if the drive does not support this feature
	if (true == params.GetUseCDText())
	{
		dwFlags |= NBF_CD_TEXT;
	}

	// Do not eject CD at the end of the burn process
	if (true == params.GetDisableEject())
	{
		dwFlags |= NBF_DISABLE_EJECT;
	}

	// Verify Filesystem after writing. Works for ISO only
	if (true == params.GetVerify())
	{
		dwFlags |= NBF_VERIFY;
	}

	
		dwFlags |= NBF_DVDP_BURN_30MM_AT_LEAST;

	return dwFlags;
}

/*********************************************************************************************
 Class		   : CBurnContext
 Function      : GetLastErrorLogLine 
 Description   : Copy the first occuring NeroAPI error to a member
				 and keep it until application exits.

 Parameters:  --

 Result type: --

 Author: TKR
 created: July, 23 2002
 <TITLE GetLastErrorLogLine >
 <GROUP CBurnContext>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, GetLastErrorLogLine>
*********************************************************************************************/
void CBurnContext::GetLastErrorLogLine()
{
	// Get a pointer to the last error string
	char* pErr = NeroGetLastError();

	// Only proceed if there was an error
	// and no previous error has occured.
	if (NULL != pErr && NULL == m_pNeroLastError)
	{
		// Allocate some memory for the error string
		// and copy the error message to the CBurnContext class
		// so we can let NeroAPI free the string immediately.
		// The allocated memory in CBurnContext will
		// be freed during destruction.
		size_t iErrSize = strlen(pErr);

		// strlen does not count the terminal NULL, so we
		// need to allocate iErrSize + 1 bytes.
		m_pNeroLastError = (char*) malloc(iErrSize + 1);
		strcpy(m_pNeroLastError, pErr);
		NeroFreeMem(pErr);
	}
}

/*********************************************************************************************
 Class		   : CBurnContext
 Function      : Exit 
 Description   : This function is called to exit with a specific error code.

 Parameters:  
  code:	  (I):	Exitcode to translate (EXITCODE)
		
 Result type:  returns 0 if successful or an error code if not  (EXITCODE)

 Author: TKR
 created: July, 12 2002
 <TITLE Exit >
 <GROUP CBurnContext>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, Exit>
*********************************************************************************************/
EXITCODE CBurnContext::Exit(EXITCODE code)
{
	if (code != EXITCODE_OK)
	{
		if (NULL != m_pNeroLastError)
		{
			CString sLastError;
			CString s = m_pNeroLastError;
			sLastError.Format(_T("NeroAPI reports: %s.\n"), s);
			AddMessageLine(sLastError);
			
			//reset abort flag
			SetAbortFlag(FALSE);
		}
	}

	return code;
}

/*********************************************************************************************
 Class		   : CBurnContext
 Function      : GetParams 
 Description   : This function returns the parameters, which are nesseccary for burn process
.

 Parameters:  --
		
 Result type:   pointer to the parameters object (PARAMETERS*)
 

 Author: TKR
 created: July, 12 2002
 <TITLE GetParams >
 <GROUP CBurnContext>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, GetParams>
*********************************************************************************************/
PARAMETERS* CBurnContext::GetParams()
{
	return m_params;
}

/*********************************************************************************************
 Class		   : CBurnContext
 Function      : NeroAPIFree 
 Description   : This function disconnects ACDC.exe from the NeroAPI and frees all NeroAPI
				 related members, also frees reserved memory.

 Parameters:  --

 Result type:  --

 Author: TKR
 created: July, 12 2002
 <TITLE NeroAPIFree >
 <GROUP CBurnContext>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, NeroAPIFree>
*********************************************************************************************/
void CBurnContext::NeroAPIFree()
{
	TRACE(_T("*** entlade NeroAPI.dll ***\n"));
	// Let the NeroAPI perform the required cleanup
	if (true == m_bNeroInitialized)
	{
//		NeroCloseDevice (m_NeroDeviceHandle);	

		NeroFreeMem (m_NeroDeviceInfos);
		NeroFreeMem (m_NeroCDInfo);
		NeroFreeCDStamp (m_pCDStamp);
		NeroClearErrors ();
	}

	// Free memory allocated during retrieval of 
	// NeroAPI's last error. No checking against NULL
	// required, this will be done by free().
	free(m_pNeroLastError);

	// NeroDone needs to be called before closing the DLL.
	// This is necessary because some clean-up actions like
	// stopping threads cannot be done in the close function of the DLL

	NeroDone (); //it takes a few seconds to complete this function "NeroDone()"
	NeroAPIGlueDone();
	m_pNeroBurn->SetNeroAPIInitialized(FALSE);
	TRACE(_T("*** NeroAPI.dll entladen ***\n"));
}

/*********************************************************************************************
 Class		   : CBurnContext
 Function      : SetCurrentProgress 
 Description   : This function is called continously by  CBurnContext::ProgressCallback(..)
				 and if the new progress is different from the last this function calls
				 the SetProgress() funktion in the main dialog window.

 Parameters:  
  iProgress (I): progress of burning in percent (int)

 Result type:  --

 Author: TKR
 created: August, 02 2002
 <TITLE SetCurrentProgress >
 <GROUP CBurnContext>
 <TOPICORDER 0>
 <KEYWORDS CBurnContext, SetCurrentProgress>
*********************************************************************************************/
void CBurnContext::SetCurrentProgress(int iProgress)
{
	//start tracing progress when dwProgress == -1
	//this is set in SetMajorPhaseCallback() when the current 
	//phase is NERO_PHASE_START_WRITE


	//beim DVD brennen wird 2x von 0% bis 100% gezählt
	//einmal für brennen von Leadin und Tracks, einmal für brennen von Leadout
	//1. wird von 0 bis 100% gezählt
	//2. wird von 1 bis 100% gezählt
	
	if(iProgress == -1)
	{
		//start tracing progress
		m_iCurrentProgress = iProgress;
		m_bDVDSecondProgress = FALSE;
		TRACE(_T("Start progressing...\n"));
	}

	if(    m_iCurrentProgress != 0 
		&& iProgress > 0
		&& iProgress != m_iCurrentProgress)
	{
		if(m_iCurrentProgress >= 100)
		{
			iProgress = 100;
		}

		m_iCurrentProgress = iProgress;

		WK_TRACE(_T("progress: %d  \n"), iProgress);

		//update progress bar in applications window
		CAcdcDlg* pDlg = (CAcdcDlg*)theApp.GetMainWnd();
		if(pDlg)
		{
			pDlg->SetProgress((DWORD)iProgress);
		}
	}
}

/*********************************************************************************************
 Class		   : CBurnContext
 Function      : static AddMessageLine 
 Description   : This function is called continously by some Nero callback functions and calls
				 the main window function AppendMessage() which appends the new message into
				 the message editfield in the main window of the application.

 Parameters:  
  sLine (I)				: message line to append (CString)
  iINeroWaitCDType (I)	: NeroWaitCDType, if nesseccary (only from within Funtion "UserDialog"
						  default: -1

 Result type:  --

 Author: TKR
 created: August, 02 2002
 <TITLE AddMessageLine >
 <GROUP CBurnContext>
 <TOPICORDER 0>
 <KEYWORDS CBurnContext, AddMessageLine>
*********************************************************************************************/
void CBurnContext::AddMessageLine(CString sLine, int iNeroWaitCDType /* = -1*/)
{
	LPARAM lParam = 0;
	WK_TRACE(_T("%s\n"),sLine);
	CAcdcDlg* pDlg = (CAcdcDlg*)theApp.GetMainWnd();
	if(pDlg)
	{
		if(m_pThis->m_pNeroBurn)
		{
			m_pThis->m_pNeroBurn->SetNewMessage(sLine);

			if(iNeroWaitCDType != -1)
			{
				lParam = iNeroWaitCDType;
			}

			::PostMessage(pDlg->m_hWnd, WM_BURN_NEW_MESSAGE, NM_NERO_BURN_THREAD, lParam);

		}
	}
}

/*********************************************************************************************
 Class		   : CBurnContext
 Function      : ResetParams 
 Description   : This function resets all parameters in the parameters object, which holds 
				 the parameters for all Nero processes (erase, burn ect.)

 Parameters:  --

 Result type:  --

 Author: TKR
 created: August, 02 2002
 <TITLE ResetParams >
 <GROUP CBurnContext>
 <TOPICORDER 0>
 <KEYWORDS CBurnContext, ResetParams>
*********************************************************************************************/
void CBurnContext::ResetParams()
{
	m_params->InitParameters();
}

/*********************************************************************************************
 Class		   : CBurnContext
 Function      : SetAbortFlag 
 Description   : This function sets the abort flag. Some Nero callback checks during Nero 
				 processes (erase, burn, ect.) this flag. If it`s TRUE, the current Nero
				 process will be terminated.

 Parameters:  
  bFlag (I): abort flag to set(BOOL = TRUE)

 Result type:  --

 Author: TKR
 created: August, 02 2002
 <TITLE SetAbortFlag >
 <GROUP CBurnContext>
 <TOPICORDER 0>
 <KEYWORDS CBurnContext, SetAbortFlag>
*********************************************************************************************/
void CBurnContext::SetAbortFlag(BOOL bFlag /* = TRUE */ )
{
	m_bAborted = bFlag;
	TRACE(_T("CBurnContext::SetAbortFlag() %d\n"), bFlag);
	ResetCurrentProgress();
	TRACE(_T("CBurnContext::SetAbortFlag() reset progress\n"));
}


/*********************************************************************************************/
void CBurnContext::SetCDRWAbortFlag(BOOL bFlag)
{
	m_bCDRWAbortFlag = bFlag;
}
/*********************************************************************************************
 Class		   : CBurnContext
 Function      : GetAbortFlag 
 Description   : This function returns the current state of the abort flag. 

 Parameters:  --

 Result type:  current state of the abort flag (BOOL)

 Author: TKR
 created: August, 02 2002
 <TITLE GetAbortFlag >
 <GROUP CBurnContext>
 <TOPICORDER 0>
 <KEYWORDS CBurnContext, GetAbortFlag>
*********************************************************************************************/
BOOL CBurnContext::GetAbortFlag()
{
	return m_bAborted;
}

/*********************************************************************************************
 Class		   : CBurnContext
 Function      : GetMediumInfo 
 Description   : This function returns the current medium info which is the information about
				 the inserted medium in the drive (CD-ROM, CD-R, CD_RW, ect.)

 Parameters:  --

 Result type:  current inserted medium in drive (enum MediumInfo)

 Author: TKR
 created: August, 02 2002
 <TITLE GetMediumInfo >
 <GROUP CBurnContext>
 <TOPICORDER 0>
 <KEYWORDS CBurnContext, GetMediumInfo>
*********************************************************************************************/
MediumInfo CBurnContext::GetMediumInfo()
{
	return m_MediumInfo;
}

/*********************************************************************************************
 Class		   : CBurnContext
 Function      : GetMediumFreeCapacityInBytes 
 Description   : This function returns the free MB`s on the inserted medium.

 Parameters:  --

 Result type:  free capacity of the inserted medium in Bytes (ULONGLONG)

 Author: TKR
 created: August, 05 2002
 <TITLE GetMediumFreeCapacityInBytes >
 <GROUP CBurnContext>
 <TOPICORDER 0>
 <KEYWORDS CBurnContext, GetMediumFreeCapacityInBytes>
*********************************************************************************************/
ULONGLONG CBurnContext::GetMediumFreeCapacityInBytes()
{
	return m_ullMediumFreeCapacityInBytes;
}


/*********************************************************************************************
 Class		   : CBurnContext
 Function      : GetMediumUsedCapacityInBytes 
 Description   : This function returns the used MB`s on the inserted medium.

 Parameters:  --

 Result type:  used capacity of the inserted medium in Bytes (ULONGLONG)

 Author: TKR
 created: September, 13 2002
 <TITLE GetMediumUsedCapacityInBytes >
 <GROUP CBurnContext>
 <TOPICORDER 0>
 <KEYWORDS CBurnContext, GetMediumUsedCapacityInBytes>
*********************************************************************************************/
ULONGLONG CBurnContext::GetMediumUsedCapacityInBytes()
{
	return m_ullMediumUsedCapacityInBytes;
}

/*********************************************************************************************
 Class		   : CBurnContext
 Function      : SetCDRWErasingTime 
 Description   : This function is called from the Nero callback function UserDialog() and
				 calls the same function in the main dialog, which sets the time in seconds 
				 which is needed to erase the inserted CDRW. The seconds are calculated by 
				 the NeroAPI.

 Parameters: 
  sSeconds: (I): the approx. time for erasing the inserted cdrw (DWORD)

 Result type:  --

 Author: TKR
 created: August, 02 2002
 <TITLE SetCDRWErasingTime >
 <GROUP CBurnContext>
 <TOPICORDER 0>
 <KEYWORDS CBurnContext, SetCDRWErasingTime>
*********************************************************************************************/
void CBurnContext::SetCDRWErasingTime(DWORD sSeconds)
{
	CAcdcDlg* pDlg = (CAcdcDlg*)theApp.GetMainWnd();
	if(pDlg)
	{
		pDlg->SetCDRWErasingTime(sSeconds);
	}

}

/*********************************************************************************************
 Class		   : CBurnContext
 Function      : SetCDRWEraseSuccess 
 Description   : This function is called from the Nero callback function UserDialog() and
				 indicates that the current erasing process of the inserted CDRW has finished.
				 This function calls the function SetCDRWErasingTime().
				 
 Parameters: 
  bSuccess: (I): if TRUE the erase process has finished (BOOL)

 Result type:  --

 Author: TKR
 created: August, 02 2002
 <TITLE SetCDRWEraseSuccess >
 <GROUP CBurnContext>
 <TOPICORDER 0>
 <KEYWORDS CBurnContext, SetCDRWEraseSuccess>
*********************************************************************************************/
void CBurnContext::SetCDRWEraseSuccess(BOOL bSuccess)
{
	m_bCDRWEraseSuccess = bSuccess;
	SetCDRWErasingTime((DWORD)(-1));
}

/*********************************************************************************************
 Class		   : CBurnContext
 Function      : GetCDRWEraseSuccess 
 Description   : This function returns the current state of the CDRW erasing process. 
				 
 Parameters: --

 Result type:  if TRUE the erase process has finished (BOOL)

 Author: TKR
 created: August, 02 2002
 <TITLE GetCDRWEraseSuccess >
 <GROUP CBurnContext>
 <TOPICORDER 0>
 <KEYWORDS CBurnContext, GetCDRWEraseSuccess>
*********************************************************************************************/
BOOL CBurnContext::GetCDRWEraseSuccess()
{
	return m_bCDRWEraseSuccess;
}

/*********************************************************************************************/
void CBurnContext::ResetCurrentProgress()
{
	m_iCurrentProgress = 0;
	m_bDVDSecondProgress = FALSE;
}

/*********************************************************************************************/
BOOL CBurnContext::IsNeroInitialized()
{	
	return m_bNeroInitialized;
}

/*********************************************************************************************/
BOOL CBurnContext::IsOldNeroCmdVersion()
{
	CProducer* pProducer = theApp.GetAcdcDlg()->GetProducer();
	BOOL bAtLeast = FALSE;
	if(pProducer)
	{
		CString sInstalledVersion;
		CString sAtLeastVersion(_T("5.5.10.15"));
		
		bAtLeast = pProducer->GetNeroVersion(sInstalledVersion, sAtLeastVersion);
	}
	return bAtLeast;
}
/*********************************************************************************************/
//zählt den belegten Speicherplatz zusammen von einem Startverzeichnis ausgehend, rekursiv
//sStartDir = ""  ==> alle lokalen Festplatten werden durchsucht, oder "C:" eingeben
//sStartDir = "C:\test" ==> Verzeichnis C:\test (incl. Unterverz.) werden durchsucht
//ullTotalSize liefert die gesamte Größe aller im Verzeichnis befindlichen Dateien
BOOL CBurnContext::GetUsedCapacityRecursiv(CString sStartDir, ULONGLONG &ullTotalBytes)
{
	HANDLE hF;
	WIN32_FIND_DATA FindFileData;
	CString sSearch, sFileName;
	CFileFind FileFind;
	CString sPath;
	CString sFile;
	DWORD dwNewBytes;
	HANDLE hFile;
	sPath = sStartDir;

	if(FileFind.FindFile(sStartDir))
	{
		FileFind.FindNextFile();
		if(!FileFind.IsDirectory())
		{

			sPath = FileFind.GetRoot();
		}	
	}

	if (sPath.GetLength()==0) {
		sPath += _T(".\\");
	}
	else if (sPath[sPath.GetLength()-1]!=_T('\\')) 
	{
		sPath += _T('\\');
	}


	sSearch = sPath + _T("*.*");

	hF = FindFirstFile(LPCTSTR(sSearch), &FindFileData);
	if (hF != INVALID_HANDLE_VALUE)
	{
		
		CString s = FindFileData.cFileName;
		if ( (_tcscmp(FindFileData.cFileName,_T("."))!=0) &&
			  (_tcscmp(FindFileData.cFileName,_T(".."))!=0))
		{
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				sFile = sPath+FindFileData.cFileName;
				if (GetUsedCapacityRecursiv(sFile, ullTotalBytes))
				{
					return TRUE;
				}
			}
			else
			{
				sFileName = (sSearch.Left(sSearch.GetLength()-3))+FindFileData.cFileName;

				dwNewBytes = 0;
				hFile = NULL;
				hFile = CreateFile(sFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
				DWORD dwSize = GetFileSize(hFile, &dwNewBytes);
				CloseHandle(hFile);
				ullTotalBytes += dwSize;
			}
		} 
		while (FindNextFile(hF,&FindFileData))
		{
			if ( (_tcscmp(FindFileData.cFileName,_T("."))!=0) &&
				  (_tcscmp(FindFileData.cFileName,_T(".."))!=0))
			{
				if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					sFile = sPath+FindFileData.cFileName;
					if (GetUsedCapacityRecursiv(sFile, ullTotalBytes))
					{
						return TRUE;
					}
				}
				else
				{
					sFileName = (sSearch.Left(sSearch.GetLength()-3))+FindFileData.cFileName;

					dwNewBytes = 0;
					hFile = NULL;
					hFile = CreateFile(sFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
					DWORD dwSize = GetFileSize(hFile, &dwNewBytes);
					CloseHandle(hFile);
					ullTotalBytes += dwSize;
				}
			}
		}
		FindClose(hF);
	}

	return FALSE;
}

void CBurnContext::InitCheckMediaType()
{
	m_ullMediumFreeCapacityInBytes	= 0;
	m_ullMediumUsedCapacityInBytes	= 0;
	m_ullOneMegaByte				= 1024 * 1024;   
	m_ullBytesPerBlock				= 2048;
	m_ullMaxCapacityDVD				= MAX_DVD_CAPACITY_IN_MB * m_ullOneMegaByte;
	m_ullMaxCapacityDVDDoubleLayer	= MAX_DVD_DOUBLE_LAYER_CAPACITY_IN_MB * m_ullOneMegaByte;
	m_ullMaxCapacityCD				= MAX_CD_CAPACITY_IN_MB * m_ullOneMegaByte;
	m_dwNumOfTracks					= 0;
	m_ullMaxDVDBlocks				= m_ullMaxCapacityDVD / m_ullBytesPerBlock;
	m_bCheckMediaCapacity			= FALSE;
	m_MediumInfo					= MI_NOMEDIUM;
	ResetCurrentProgress();
}