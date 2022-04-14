/******************************************************************************
|*	This file keeps all main command which are responsible for any calls of
|*	the NeroAPI. We have 7 main functions:
|*
|*	implemented main functions:
|*		-	CommandCDInfo		-	Retrieve CD information
|*		-	CommandEject		-	Eject and load implementation
|*		-	CommandErase		-	Implements erasure of CDRWs
|*		-	CommandListDrives	-	List available drives and their characteristics
|*		-	CommandRead			-	Perform DAE (digital audio extraction)
|*		-	CommandVersion		-	Obtaining Version Information
|*		-	CommandWrite		-	General Write Function
|*
|*	PROGRAM: MainCommands.cpp
|*
******************************************************************************/


#include "stdafx.h"
#include "BurnContext.h"
#include "AcdcDlg.h"
#include "ACDC.h"

/*********************************************************************************************
 Class		   : CBurnContext
 Function      : CommandCDInfo 
 Description   : This function performs execution of a CD info command.
				 Retrieve a pointer to a NERO_CD_INFO structure for the specified device.
				 NULL will be returned if an error occurred. 
				 The allocated memory for the structure has to be freed by using  NeroFreeMem().
				 This will be performed by the destructor of CBurnContext.
				 NGCDI_READ_CD_TEXT is the only constant allowed.

 Parameters:  
  params: (O):  reference to the parameters  (const PARAMETERS &)

 Result type:  returns 0 if successful or an error code if not  (EXITCODE)

 Author: TKR
 created: July, 10 2002
 <TITLE CommandCDInfo >
 <GROUP MainCommands>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, MainCommands, CommandCDInfo>
*********************************************************************************************/
EXITCODE CBurnContext::CommandCDInfo (const PARAMETERS & params)
{
	CString sMedienType;
	BOOL bNeroVersionFits = FALSE;

	m_NeroCDInfo = NeroGetCDInfo (m_NeroDeviceHandle, NGCDI_READ_CD_TEXT);
	if (NULL == m_NeroCDInfo)
	{
		GetLastErrorLogLine();
			
		return EXITCODE_ERROR_GETTING_CD_INFO;
	}
	
	//get information about mediums capacities with CIPCDrive
	//------------------------------------------------------- 
	CString sDriveLetter;
	if(m_pNeroBurn)
	{
		sDriveLetter = m_pNeroBurn->GetDriveLetter();
	}
	
	//check current Nero Version, has to be at least 5.5.9.10 for using NERO_MEDIA_TYPE
	//---------------------------------------------------------------------------------
	CString sNeroVersionAtLeast = _T("5.5.9.10");
	CString sCurrNeroVersion;
	NERO_MEDIA_TYPE MediaType = MEDIA_NONE;

	CProducer *pProducer = theApp.GetAcdcDlg()->GetProducer();
	if(pProducer)
	{
		bNeroVersionFits = pProducer->GetNeroVersion(sCurrNeroVersion, sNeroVersionAtLeast);
		if(bNeroVersionFits)
		{
			MediaType = m_NeroCDInfo->ncdiMediaType;
		}
	}

	//reset all members for checking mediums capacity and medium type
	//---------------------------------------------------------------
	InitCheckMediaType();
	sMedienType = UNKNOWN_MEDIA_TYPE;
	
	m_bCheckMediaCapacity = CheckMediaCapacities(sDriveLetter);


	if(bNeroVersionFits) //NeroAPI version is equal or higher than 5.5.9.10., use CheckNewMediaType()
	{
		sMedienType = CheckNewMediaType(MediaType);
	}
	else //NeroAPI version less than  5.5.9.10., use CheckOldMediumType()
	{
		sMedienType = CheckOldMediumType();
	}

	AddMessageLine(sMedienType);
	return EXITCODE_OK;
}

/*********************************************************************************************
 Class		   : CBurnContext
 Function      : CommandEject 
 Description   : This function executes the eject and load commands.

 Parameters:  
  params: (O):  reference to the parameters  (const PARAMETERS &)

 Result type:  returns 0 if successful or an error code if not  (EXITCODE)

 Author: TKR
 created: July, 10 2002
 <TITLE CommandEject >
 <GROUP MainCommands>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, MainCommands, CommandEject>
*********************************************************************************************/
EXITCODE CBurnContext::CommandEject (const PARAMETERS & params)
{
	int ret;

	// "false" in parameter "eject" loads a CD, "true" ejects.
	if (COMMAND_EJECT == params.GetCommand())
	{
		ret = NeroEjectLoadCD (m_NeroDeviceHandle, true);
	}
	else
	{
		ret = NeroEjectLoadCD (m_NeroDeviceHandle, false);
	}

	// Return the proper EXITCODE
	if (0 != ret)
	{
		GetLastErrorLogLine();
		return EXITCODE_EJECT_FAILED;		
	}
	return EXITCODE_OK;
}

/*********************************************************************************************
 Class		   : CBurnContext
 Function      : CommandErase 
 Description   : This function erases a CDRW either entirely or in quick mode.

 Parameters:  
  params: (O):  reference to the parameters  (const PARAMETERS &)

 Result type:  returns 0 if successful or an error code if not  (EXITCODE)

 Author: TKR
 created: July, 10 2002
 <TITLE CommandErase >
 <GROUP MainCommands>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, MainCommands, CommandErase>
*********************************************************************************************/
EXITCODE CBurnContext::CommandErase (const PARAMETERS & params)
{
	// Get the estimated time for the erase operation
	int time = NeroGetCDRWErasingTime (m_NeroDeviceHandle, params.GetEraseMode());


	// Abort on error
	switch (time)
	{
		case -1:
			GetLastErrorLogLine();
			return EXITCODE_NO_CD_INSERTED;

		case -2:
			GetLastErrorLogLine();
			return EXITCODE_NO_SUPPORT_FOR_CDRW;

		default:
			break;
	}

	WK_TRACE (_T("CBurnContext::CommandErase() Erasing CDRW. This will take approx. %d seconds.\n"), time);

	// Erase the CDRW. Mode is either NEROAPI_ERASE_ENTIRE or NEROAPI_ERASE_QUICK.
	// Quick mode is default. NeroEraseCDRW returns 0 if successful
	int ret = NeroEraseCDRW (m_NeroDeviceHandle, params.GetEraseMode());

	// Return the proper EXITCODE
	if (0 != ret)
	{
		GetLastErrorLogLine();
		return EXITCODE_ERROR_ERASING_CDRW;
	}
	return EXITCODE_OK;
}

/*********************************************************************************************
 Class		   : CBurnContext
 Function      : CommandListDrives 
 Description   : This function performs listing of all available drives with their
				 main characteristics. m_NeroDeviceInfos has been filled by a call
				 GetAvailableDrives during NeroAPI initialization in main().

 Parameters:  
  params: (O):  reference to the parameters  (const PARAMETERS &)

 Result type:  returns 0 if successful or an error code if not  (EXITCODE)

 Author: TKR
 created: July, 10 2002
 <TITLE CommandListDrives >
 <GROUP MainCommands>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, MainCommands, CommandListDrives>
*********************************************************************************************/
EXITCODE CBurnContext::CommandListDrives (const PARAMETERS & params)
{
	TRACE (_T("Drv: Adapter      Underrun Prot.  # ID  Type\n"));
	TRACE (_T("------------------------------------------------------\n"));

	CString sLetter;
	BOOL bFoundDriveLetter = FALSE;

	//reset drive letter
	m_pNeroBurn->SetDriveLetter(sLetter);
	
	for (int i = 0; i < (int)(m_NeroDeviceInfos->nsdisNumDevInfos); i ++)
	{
		// Use a helper variable for better code readability
		NERO_SCSI_DEVICE_INFO tempNSDI = m_NeroDeviceInfos->nsdisDevInfos[i];

		// Check whether the drive has a Windows drive letter,
		// use '?' if it is not available.
		char cDriveLetter;
		if (0 != tempNSDI.nsdiDriveLetter)
		{
			cDriveLetter = tempNSDI.nsdiDriveLetter;
			bFoundDriveLetter = TRUE;
		}
		else
		{
			cDriveLetter = _T('?');
		}
#ifdef _UNICODE
		char szText[512];
		sprintf(szText, "%c %-31s %-31s %2d %-7s %2d %d\n", 
			cDriveLetter,
			tempNSDI.nsdiDeviceName,
			tempNSDI.nsdiBufUnderrunProtName,
			tempNSDI.nsdiHostAdapterNo,
			tempNSDI.nsdiHostAdapterName,
			tempNSDI.nsdiDeviceID,
			tempNSDI.nsdiDevType);
		CWK_String str(szText);
		WK_TRACE(_T("%s\n"), LPCTSTR(str));
#else
		WK_TRACE("%c %-31s %-31s %2d %-7s %2d %d\n",
			cDriveLetter,
			tempNSDI.nsdiDeviceName,
			tempNSDI.nsdiBufUnderrunProtName,
			tempNSDI.nsdiHostAdapterNo,
			tempNSDI.nsdiHostAdapterName,
			tempNSDI.nsdiDeviceID,
			tempNSDI.nsdiDevType);
#endif
		// Print one line for every device
		CString sNoRecorder;
		BOOL bRecorder = FALSE;
		NEROAPI_SCSI_DEVTYPE type = tempNSDI.nsdiDevType;

		CString sImageDr = tempNSDI.nsdiHostAdapterName;

		if(sImageDr == _T("imagedr"))
		{
			//found image drive, ignore it
		}
		else
		{
			if(type == NEA_SCSI_DEVTYPE_WORM) //recorder
			{
				bRecorder = TRUE;
				if(!bFoundDriveLetter)
				{
					sLetter = cDriveLetter;
				}
				else if (cDriveLetter != _T('?'))
				{
					sLetter = tempNSDI.nsdiDriveLetter;
				}
			}
			else if(type == NEA_SCSI_DEVTYPE_CDROM) //CD-Rom, no recorder
			{
				sNoRecorder = _T("found CD-ROM drive; recorder expected; recording impossible");
				WK_TRACE(_T("CD-Rom, kein Brenner\n"));
			}
			else
			{
				//unknown or not supported device
				sNoRecorder = _T("no drive found; recorder expected; recording impossible");
				WK_TRACE(_T("CD nicht vorhanden bzw. wird nicht unterstützt\n"));
			}

			if(!bRecorder)
			{
				CWK_Dongle dongle;
				if (dongle.IsTransmitter())
				{
					CAcdcDlg* pACDC = theApp.GetAcdcDlg();
					CString sMsg;
					pACDC->AppendMessage(sNoRecorder);
					CWK_RunTimeError RTerr(WAI_AC_DC, REL_ERROR, RTE_CONFIGURATION, sNoRecorder, 0, 0);
					RTerr.Send();
				}
			}
		}


	}

	if(!sLetter.IsEmpty())
	{
		//set current CD-RW drive letter
		//the current drive is the last alphabetic cd-drive
		m_pNeroBurn->SetDriveLetter(sLetter);
	}
	
	TRACE (_T("\n"));
	return EXITCODE_OK;
}

/*********************************************************************************************
 Class		   : CBurnContext
 Function      : CommandRead 
 Description   : This function performs DAE (digital audio extraction). 
				 (we don`t use this function, because we don`t read or burn audio files
				 but it`s implemented because nero can do it.)

 Parameters:  
  params: (O):  reference to the parameters  (const PARAMETERS &)

 Result type:  returns 0 if successful or an error code if not  (EXITCODE)

 Author: TKR
 created: July, 10 2002
 <TITLE CommandRead >
 <GROUP MainCommands>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, MainCommands, CommandRead>
*********************************************************************************************/
EXITCODE CBurnContext::CommandRead (const PARAMETERS & params)
{
	// Make sure that NeroGetCDInfo has not been called before to prevent multiple 
	// allocation of memory for NERO_CD_INFO.
//	_ASSERTE (m_NeroCDInfo == NULL);

	// First, get the CD info.
	// NeroGetCDInfo retrieves a pointer to a NERO_CD_INFO structure for the specified device.
	// The allocated memory for the structure has to be freed by using  NeroFreeMem().
	// NULL will be returned if an error occurred. NGCDI_READ_CD_TEXT is the only allowed constant.
	if(m_NeroCDInfo == NULL)
	{
		m_NeroCDInfo = NeroGetCDInfo (m_NeroDeviceHandle, NGCDI_READ_CD_TEXT);
	}
	if (NULL == m_NeroCDInfo)
	{
		GetLastErrorLogLine();
		return EXITCODE_ERROR_GETTING_CD_INFO;
	}

	// Loop through the user supplied list of tracks.
	for (int i = 0; i < params.GetNumberOfTracks(); i++)
	{
		NERO_TRACK_INFO* pTrackInfo = NULL;
		NERO_CALLBACK callback;
		NERO_DATA_EXCHANGE exchange;

		// Find the track among the existing tracks on the CD.
		for (unsigned int j = 0; j < m_NeroCDInfo->ncdiNumTracks; j ++)
		{
			if (m_NeroCDInfo->ncdiTrackInfos[j].ntiTrackNumber == (DWORD)params.GetTrackNumber(i))
			{
				pTrackInfo = &m_NeroCDInfo->ncdiTrackInfos[j];
				break;
			}
		}

		// If the track could not be found pTrackInfo still contains NULL
		if (NULL == pTrackInfo)
		{
			// If the track was not found, report an error.
			CWK_String sTrack(params.GetTrackFileName(i));
			WK_TRACE(_T("CommandRead: Track %d for file '%s' was not found on medium\n"), params.GetTrackNumber(i), (LPCTSTR)sTrack);
			return EXITCODE_TRACK_NOT_FOUND;
		}
		
		// Track found, now extract the audio data.
		callback.ncCallbackFunction = ProgressCallback;
		callback.ncUserData = &s_NeroSettings;

		// Find the file extension the user supplied for the file 
		// that will contain the extracted data.
		// Supported extensions are WAV and PCM.
		// Try to find the file extension by looking for '.' from the right
		char* psExt = strrchr (params.GetTrackFileName(i), _T('.'));

		// stricmp performs a lowercase comparison and returns 0 if the strings are identical.
		if ((NULL != psExt) && (0 == stricmp (psExt, ".wav")))
		{
			// WAV files require NERO_ET_FILE as data exchange type
			exchange.ndeType = NERO_ET_FILE;

			// Copy the user supplied file name to the appropriate NERO_DATA_EXCHANGE member
			strncpy (exchange.ndeData.ndeFileName, params.GetTrackFileName(i), sizeof (exchange.ndeData.ndeFileName));
			exchange.ndeData.ndeFileName[sizeof (exchange.ndeData.ndeFileName) - 1] = 0;
		}
		else if ((NULL != psExt) && (0 == stricmp (psExt, ".pcm")))
		{
			// PCM files require NEOR_ET_IO_CALLBACK to exchange data directly
			exchange.ndeType = NERO_ET_IO_CALLBACK;

			// Set the required callbacks
			exchange.ndeData.ndeIO.nioIOCallback = WriteIOCallback;
			exchange.ndeData.ndeIO.nioEOFCallback = EOFCallback;
			exchange.ndeData.ndeIO.nioErrorCallback = ErrorCallback;
			
			// fopen with mode "wb" opens an empty file for writing in binary (untranslated) mode. If the given file exists, its contents are destroyed. 
			// Translations involving carriage-return and linefeed characters are suppressed. 
			exchange.ndeData.ndeIO.nioUserData = fopen (params.GetTrackFileName(i), "wb");

			// Make sure that the file could be openend
			if (0 == exchange.ndeData.ndeIO.nioUserData)
			{
				CWK_String sTrack(params.GetTrackFileName(i));
				WK_TRACE(_T("Cannot open target file %s\n"), (LPCTSTR)sTrack);
				return EXITCODE_ERROR_OPENNING_FILE;
			}
		}
		else
		{
			// We did not recognize the file extension.
			CWK_String sTrack(params.GetTrackFileName(i));
			WK_TRACE(_T("Unknown file type for writing of %s\n"), (LPCTSTR)sTrack);
			return EXITCODE_UNKNOWN_FILE_TYPE;
		}

		// Print track number and file name
		CWK_String sTrack(params.GetTrackFileName(i));
		WK_TRACE (_T("%02d. '%s':\n"), params.GetTrackNumber(i), (LPCTSTR)sTrack);

		// Do the actual audio extraction.
		// Aborting will not be reported by NeroGetLastError().
		// Incomplete target files are not deleted.
		// 0 is returned if the operation was successful.
		int res = NeroDAE (m_NeroDeviceHandle,
							pTrackInfo->ntiTrackStartBlk,
							pTrackInfo->ntiTrackLengthInBlks,
							&exchange,
							// Speed parameter - 0 means maximum speed
							0,
							&callback);

		// If we extracted PCM the data file needs to be closed
		if (exchange.ndeType == NERO_ET_IO_CALLBACK)
		{
			// Close the file handle if we opened it.
			if (0 != exchange.ndeData.ndeIO.nioUserData)
			{
				fclose ((FILE *) exchange.ndeData.ndeIO.nioUserData);
			}
		}

		// If NeroDAE did not return 0 the operation failed.
		if (0 != res)
		{
			return EXITCODE_DAE_FAILED;
		}

		WK_TRACE (_T("\n"));
	}

	return EXITCODE_OK;
}

/*********************************************************************************************
 Class		   : CBurnContext
 Function      : CommandVersion 
 Description   : Print out the version information.  and return d´the version number.
				 NeroGetAPIVersion retrieves NeroAPI's version number in DWORD-format.
				 E.g. a value of 5551 means that the version is 5.5.5.1

 Parameters:   --
 Result type:  the installed version of Nero (DWORD)

 Author: TKR
 created: July, 10 2002
 <TITLE CommandVersion >
 <GROUP MainCommands>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, MainCommands, CommandVersion>
*********************************************************************************************/
DWORD CBurnContext::CommandVersion (CString& sVersion)
{
	DWORD dwVersion = NeroGetAPIVersion();

	// The version number is then split into a format fit for display and printed.
	sVersion.Format(_T("%d.%d.%d.%d"),
					(dwVersion / 1000),
					(dwVersion /  100)  % 10,
					(dwVersion /   10)  % 10,
					dwVersion          % 10);
	return dwVersion;
}

/*********************************************************************************************
 Class		   : CBurnContext
 Function      : CommandWrite 
 Description   : This function distinguishes between different burn types and
				 acts accordingly.

 Parameters:  
  params: (O):  reference to the parameters  (const PARAMETERS &)

 Result type:  returns 0 if successful or an error code if not  (EXITCODE)

 Author: TKR
 created: July, 10 2002
 <TITLE CommandWrite >
 <GROUP MainCommands>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, MainCommands, CommandWrite>
*********************************************************************************************/
EXITCODE CBurnContext::CommandWrite (const PARAMETERS & params)
{
	EXITCODE code;

	switch (params.GetBurnType())
	{
	case BURNTYPE_IMAGE:

		// WriteImage burns Nero Image files (.nrg)
		code = WriteImage (params);
		break;

	case BURNTYPE_ISOAUDIO:
		
		// WriteIsoAudio handles ISO CDs and DVDs, as well as Audio and mixed mode CDs
		code = WriteIsoAudio (params);
		break;

	case BURNTYPE_SVIDEOCD:
	case BURNTYPE_VIDEOCD:

		// Both Video CD and Super Video CD are written by WriteVideoCD
		code = WriteVideoCD (params);
		break;

	case BURNTYPE_FREESTYLE:

		// Write Freestyle compilation
		code = WriteFreestyle (params);
		break;

	default:

		// No valid burn type
		code = EXITCODE_INTERNAL_ERROR;
	}
	
	if(m_NeroDeviceHandle != NULL)
	{
		//gib allozierten Speicher, der während des Brennens zusätzlich angelegt wurde,
		//wieder frei. Ist aus "NeroAPIFree" rausgenommen worden
		NeroCloseDevice (m_NeroDeviceHandle);
		WK_TRACE(_T("** Allozierter Speicher wieder freigegeben\n **"));
	}

	ResetCurrentProgress();
	return code;
}

/*********************************************************************************************/
BOOL CBurnContext::GetCDRWAbortFlag()
{
	return m_bCDRWAbortFlag;
}