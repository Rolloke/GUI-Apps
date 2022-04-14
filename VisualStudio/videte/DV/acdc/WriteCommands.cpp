/******************************************************************************
|*	This file keeps all Nero write functions. These functions are responsible 
|*	for all write processes. Each function gets all neccessary data from 
|*	the PARAMETER object and starts the burn process.
|*
|*	implemented write functions:
|*		-	WriteFreestyle		-	burning a freestyle cd
|*		-	WriteImage			-	burning an ISO image
|*		-	WriteIsoAudio		-	burning ISO CDs and DVDs (Audio and mixed mode CDs)
|*		-	WriteNeroErrorLog	-	writes the standard nero error log out to a file
|*		-	WriteVideoCD		-	burning Video or Super Video CDs
|*
|*	PROGRAM: WriteCommands.cpp
|*
******************************************************************************/


#include "stdafx.h"
#include "BurnContext.h"


/*********************************************************************************************
 Class		   : CBurnContext
 Function      : WriteFreestyle 
 Description   : This function is responsible for burning a freestyle cd.

 Parameters:  
  params: (O):  reference to the parameters  (const PARAMETERS &)

 Result type:  returns 0 if successful or an error code if not  (EXITCODE)

 Author: TKR
 created: July, 10 2002
 <TITLE WriteFreestyle >
 <GROUP WriteCommands>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, MainCommands, WriteFreestyle>
*********************************************************************************************/
EXITCODE CBurnContext::WriteFreestyle (const PARAMETERS & params)
{
	int iSize;
	NERO_WRITE_FREESTYLE_CD * pWriteCD;
	EXITCODE code;
	NERO_ISO_ITEM* pItem = NULL;

	// Calculate the size required for NERO_WRITE_FREESTYLE_CD plus the given number of tracks
	iSize = sizeof (NERO_WRITE_FREESTYLE_CD) + (params.GetNumberOfTracks() - 1) * sizeof (NERO_FREESTYLE_TRACK);

	// Allocate the required memory and assign it to the NERO_WRITE_FREESTYLE_CD pointer
	pWriteCD = (NERO_WRITE_FREESTYLE_CD *) new char[iSize];

	// Make sure the system could allocate enough memory
	if (NULL == pWriteCD)
	{
		return EXITCODE_OUT_OF_MEMORY;
	}

	// Fill the allocated memory with null bytes
	memset (pWriteCD, 0, iSize);

	// Fill in the basic information
	pWriteCD->nwfcdStructureSize = sizeof (NERO_FREESTYLE_CD);
	pWriteCD->nwfcdNumTracks = params.GetNumberOfTracks();
	pWriteCD->nwfcdArtist = params.GetArtist();
	pWriteCD->nwfcdTitle = params.GetTitle();
	pWriteCD->nwfcdCDExtra = params.GetUseCDExtra();

	try
	{
		// Get the ISO track from the information in the parameters.
		code = GetIsoTrack (params, &pWriteCD->nwfcdIsoTrack, &pItem);
		if (code != EXITCODE_OK)
		{
			throw code;
		}

		// Fill in the CD stamp information as it only became available after
		// GetIsoTrack().
		pWriteCD->nwfcdpCDStamp = m_pCDStamp;

		for (int i = 0; i < params.GetNumberOfTracks(); i ++)
		{

			// Helper variables to promote readability
			NERO_DATA_EXCHANGE* ndeShort = &(pWriteCD->nwfcdTracks [i].nftSourceDataExchg);
			int iNameSize = sizeof (ndeShort->ndeData.ndeFileName);
			char* pFileName = (char*)&(pWriteCD->nwfcdTracks [i].nftSourceDataExchg.ndeData.ndeFileName);
			LPCSTR sFileName = params.GetTrackFileName(i);

			// Fill in type information, file name and mode
			ndeShort->ndeType = NERO_ET_FILE_RAW;
			strncpy (pFileName, sFileName, iNameSize);
			pFileName[iNameSize - 1] = 0;
			pWriteCD->nwfcdTracks[i].nftTracktype = params.GetTrackMode(i);
		}
		
		NEROAPI_BURN_ERROR err;

		// Perform the actual burn process
		err = NeroBurn (m_NeroDeviceHandle,
						NERO_FREESTYLE_CD,
						pWriteCD,
						GetBurnFlags (params),
						params.GetSpeed(),
						&s_NeroProgress);

		if (NEROAPI_BURN_OK != err)
		{
			GetLastErrorLogLine();
		}

		code = TranslateNeroToExitCode (err);
	}
	catch (EXITCODE e)
	{
		code = e;
	}

	// Free allocated memory
	NeroFreeIsoTrack (pWriteCD->nwfcdIsoTrack);
	DeleteIsoItemTree (pItem);
	delete [] (char *)pWriteCD;

	return code;
}

/*********************************************************************************************
 Class		   : CBurnContext
 Function      : WriteImage 
 Description   : This function is responsible for burning an ISO image.

 Parameters:  
  params: (O):  reference to the parameters  (const PARAMETERS &)

 Result type:  returns 0 if successful or an error code if not  (EXITCODE)

 Author: TKR
 created: July, 10 2002
 <TITLE WriteImage >
 <GROUP WriteCommands>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, MainCommands, WriteImage>
*********************************************************************************************/
EXITCODE CBurnContext::WriteImage (const PARAMETERS & params)
{
	// Make sure that an image file name has been provided by the user
	_ASSERTE (NULL != params.GetImageFileName());

	NERO_WRITE_IMAGE writeCD;

	// Fill in the image file name
	strncpy (writeCD.nwiImageFileName, params.GetImageFileName(), sizeof (writeCD.nwiImageFileName));
	writeCD.nwiImageFileName[sizeof (writeCD.nwiImageFileName)-1] = 0;

	NEROAPI_BURN_ERROR err;

	// Burn the image
	err = NeroBurn (m_NeroDeviceHandle,
					NERO_BURN_IMAGE_MEDIA,
					&writeCD,
					GetBurnFlags (params),
					params.GetSpeed(),
					&s_NeroProgress);

	if (NEROAPI_BURN_OK != err)
	{
		GetLastErrorLogLine();
	}

	return TranslateNeroToExitCode (err);
}

/*********************************************************************************************
 Class		   : CBurnContext
 Function      : WriteIsoAudio 
 Description   : This function is responsible for burning ISO CDs and DVDs,
				 as well as Audio and mixed mode CDs.

 Parameters:  
  params: (O):  reference to the parameters  (const PARAMETERS &)

 Result type:  returns 0 if successful or an error code if not  (EXITCODE)

 Author: TKR
 created: July, 10 2002
 <TITLE WriteIsoAudio >
 <GROUP WriteCommands>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, MainCommands, WriteIsoAudio>
*********************************************************************************************/
EXITCODE CBurnContext::WriteIsoAudio (const PARAMETERS & params)
{
	int iSize;
	NERO_WRITE_CD * pWriteCD;
	EXITCODE code;
	NERO_ISO_ITEM * pItem = NULL;

	// Calculate the size required for NERO_WRITE_CD plus the given number of tracks
	iSize = sizeof (NERO_WRITE_CD) + (params.GetNumberOfTracks() - 1) * sizeof (NERO_AUDIO_TRACK);

	// Allocate the required memory and assign it to the NERO_WRITE_CD pointer
	pWriteCD = (NERO_WRITE_CD *) new char[iSize];

	// Make sure the system could allocate enough memory
	if (NULL == pWriteCD)
	{
		return EXITCODE_OUT_OF_MEMORY;
	}

	// Fill the allocated memory with null bytes
	memset (pWriteCD, 0, iSize);

	// Fill in the basic information
	pWriteCD->nwcdNumTracks = params.GetNumberOfTracks();
	pWriteCD->nwcdArtist = params.GetArtist();
	pWriteCD->nwcdTitle = params.GetTitle();
	pWriteCD->nwcdCDExtra = params.GetUseCDExtra();

	// Set DVD as media type if the user requested so
	// DVD is only allowed with ISO; will not work with Audio
	// or mixed mode.
	if (true == params.GetUseDVD())
	{
		pWriteCD->nwcdMediaType = MEDIA_DVD_ANY;
	}

	try
	{
		// Get the ISO track from the information in the parameters.
		code = GetIsoTrack (params, &pWriteCD->nwcdIsoTrack, &pItem);
		if (code != EXITCODE_OK)
		{
			throw code;
		}

		// Fill in the CD stamp information as it only became available after
		// GetIsoTrack.
		pWriteCD->nwcdpCDStamp = m_pCDStamp;

		for (int i = 0; i < params.GetNumberOfTracks(); i ++)
		{
			// Write continously; first track requires minimal pause of 150, though.
			if (0 == i)
			{
				pWriteCD->nwcdTracks[i].natPauseInBlksBeforeThisTrack = 150;
			}
			else
			{
				pWriteCD->nwcdTracks[i].natPauseInBlksBeforeThisTrack = 0;
			}

			// Find the file name extension
			LPCSTR psExt = strrchr (params.GetTrackFileName(i), '.');

			// Helper variables to promote readability
			NERO_DATA_EXCHANGE* ndeShort = &(pWriteCD->nwcdTracks[i].natSourceDataExchg);
			int iNameSize = sizeof (ndeShort->ndeData.ndeFileName);
			char* pFileName = (char*)&(pWriteCD->nwcdTracks [i].natSourceDataExchg.ndeData.ndeFileName);
			LPCSTR sFileName = params.GetTrackFileName(i);


			// WAV, MP3, WMA and PCM are allowed types
			// stricmp performs a lowercase comparison and returns 0 if the strings are identical.
			// We fill in type information and file name.
			if ((NULL != psExt) && (0 == stricmp (psExt, ".wav")))
			{
				ndeShort->ndeType = NERO_ET_FILE;
				strncpy (ndeShort->ndeData.ndeFileName, sFileName, iNameSize);
				pFileName[iNameSize - 1] = 0;
			}
			else if ((NULL != psExt) && (0 == stricmp (psExt, ".mp3")))
			{
				ndeShort->ndeType = NERO_ET_FILE_MP3;
				strncpy (pFileName, sFileName, iNameSize);
				pFileName[iNameSize - 1] = 0;
			}
			else if ((NULL != psExt) && (0 == stricmp (psExt, ".wma")))
			{
				ndeShort->ndeType = NERO_ET_FILE_WMA;
				strncpy (pFileName, sFileName, iNameSize);
				pFileName[iNameSize - 1] = 0;
			}
			else if ((NULL != psExt) && (0 == stricmp (psExt, ".pcm")))
			{
				// PCM format will be handled by callbacks
				ndeShort->ndeType = NERO_ET_IO_CALLBACK;
				ndeShort->ndeData.ndeIO.nioIOCallback = ReadIOCallback;
				ndeShort->ndeData.ndeIO.nioEOFCallback = EOFCallback;
				ndeShort->ndeData.ndeIO.nioErrorCallback = ErrorCallback;

				struct _stat buf;

				if (0 != _stat (sFileName, &buf))
				{
#ifdef _UNICODE
					//Klammer muss sein, da sonst str nicht zerstört wird, denn nach dem
					//throw wird der Scope nicht verlassen
					{
						CWK_String str(sFileName);
						WK_TRACE(_T("Cannot determine length of file %s\n"), str);
					}

#else
					WK_TRACE(_T("Cannot determine length of file %s\n"), sFileName);
#endif

					throw EXITCODE_ERROR_DETERMINING_LENGTH_OF_FILE;
				}

				// Calculate the length in blocks. Block size for Audio compilations is 2352 bytes.
				pWriteCD->nwcdTracks [i].natLengthInBlocks = buf.st_size / 2352;

				// Open the file for reading in binary mode
				ndeShort->ndeData.ndeIO.nioUserData = fopen (sFileName, "rb");
				if (NULL == ndeShort->ndeData.ndeIO.nioUserData)
				{
#ifdef _UNICODE
					//Klammer muss sein, da sonst str nicht zerstört wird, denn nach dem
					//throw wird der Scope nicht verlassen
					{
						CWK_String str(params.GetTrackFileName(i));
						WK_TRACE(_T("Cannot open source file %s\n"), str);
					}

#else
					WK_TRACE(_T("Cannot open source file %s\n"), params.GetTrackFileName(i));
#endif
					throw EXITCODE_ERROR_OPENNING_FILE;
				}
			}
			else
			{
#ifdef _UNICODE
				//Klammer muss sein, da sonst str nicht zerstört wird, denn nach dem
				//throw wird der Scope nicht verlassen
				{
					CWK_String str(sFileName);
					WK_TRACE(_T("Unknown file type '%s'\n"), str);
				}
#else
				WK_TRACE(_T("Unknown file type '%s'\n"), sFileName);
#endif
				throw EXITCODE_UNKNOWN_FILE_TYPE;
			}
		}
		
		NEROAPI_BURN_ERROR err;

		
		// Perform the actual burn process
		err = NeroBurn (m_NeroDeviceHandle,
						NERO_ISO_AUDIO_MEDIA,
						pWriteCD,
						GetBurnFlags (params),
						params.GetSpeed(),
						&s_NeroProgress);

		if (NEROAPI_BURN_OK != err)
		{
			GetLastErrorLogLine();
		}

		code = TranslateNeroToExitCode (err);
	}
	catch (EXITCODE e)
	{
		code = e;
	}

	// Free allocated memory
	NeroFreeIsoTrack (pWriteCD->nwcdIsoTrack);

//TODO tkr wieder rein, oder komplett umbauen, also NeroAPI nur zu 
	//Programmstart verknüpfen und nur am Ende wieder lösen
//	if(code == EXITCODE_OK)
	{
//#ifndef _DEBUG
//		WK_TRACE(_T("DeleteIsoItemTree  %08lx always\n"), pItem);
//		DeleteIsoItemTree (pItem);
//#endif
	}


	delete [] (char *)pWriteCD;

	// Close the files that have been opened for
	// handling of PCM format if any.
	for (int i = 0; i < params.GetNumberOfTracks(); i ++)
	{
		if (pWriteCD->nwcdTracks [i].natSourceDataExchg.ndeType == NERO_ET_IO_CALLBACK &&
			pWriteCD->nwcdTracks [i].natSourceDataExchg.ndeData.ndeIO.nioUserData)
		{
			fclose ((FILE *)pWriteCD->nwcdTracks [i].natSourceDataExchg.ndeData.ndeIO.nioUserData);
		}
	}

	return code;
}

/*********************************************************************************************
 Class		   : CBurnContext
 Function      : WriteNeroErrorLog 
 Description   : This function simply writes the standard nero error log out
				 to a file.

 Parameters:  
  params: (O):  reference to the parameters  (const PARAMETERS &)

 Result type:  --

 Author: TKR
 created: July, 10 2002
 <TITLE WriteNeroErrorLog >
 <GROUP WriteCommands>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, MainCommands, WriteNeroErrorLog>
*********************************************************************************************/
void CBurnContext::WriteNeroErrorLog (const PARAMETERS & params)
{
	// If --no_error_log was set, GetWriteErrorLog will return false.
	if (params.GetWriteErrorLog() == true)
	{
		char filename[255];

		// If the --nero_log_timestamp command line parameter
		// was provided we will add year,month,day,hour,minute and
		// seconds information to the file name prefix.
		if (true == params.GetNeroLogTimestamp())
		{
			char tmpbuf[128];
			struct tm *today;
			time_t ltime;
			time( &ltime );
			today = localtime( &ltime );

			// Use strftime to build a customized time string
			strftime( tmpbuf, 128, "%Y%m%d%H%M%S%", today );
			strcpy(filename, "neroerr");
			strcat(filename, tmpbuf);
			strcat(filename, ".txt");
		}
		else
		{
			// standard name otherwise
			strcpy(filename, "neroerr.txt");
		}

		FILE * fh = fopen (filename, "w");
		if (NULL != fh)
		{
			//a call of this function (NeroGetErrorLog()) produces 
			//more than 30 first-chance exceptions in acdc.exe (KERNEL32.dll).
			char * psLog = NeroGetErrorLog ();
			if (NULL != psLog)
			{
				fputs (psLog, fh);
			}

			fclose (fh);
			NeroFreeMem (psLog);
		}
	}
}

/*********************************************************************************************
 Class		   : CBurnContext
 Function      : WriteVideoCD 
 Description   : This function performs burning Video or Super Video CDs..

 Parameters:  
  params: (O):  reference to the parameters  (const PARAMETERS &)

 Result type:  returns 0 if successful or an error code if not  (EXITCODE)

 Author: TKR
 created: July, 10 2002
 <TITLE WriteVideoCD >
 <GROUP WriteCommands>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, MainCommands, WriteVideoCD>
*********************************************************************************************/
EXITCODE CBurnContext::WriteVideoCD (const PARAMETERS & params)
{
	int iSize;
	NERO_WRITE_VIDEO_CD * pWriteCD;
	EXITCODE code;
	NERO_ISO_ITEM * pItem = NULL;

	// Calculate the size required for NERO_WRITE_CD plus the given number of tracks
	iSize = sizeof (NERO_WRITE_VIDEO_CD) + (params.GetNumberOfTracks() - 1) * sizeof (NERO_VIDEO_ITEM);

	// Allocate the required memory and assign it to the NERO_WRITE_CD pointer
	pWriteCD = (NERO_WRITE_VIDEO_CD *) new char[iSize];
	if (!pWriteCD)
	{
		return EXITCODE_OUT_OF_MEMORY;
	}

	// Fill the allocated memory with null bytes
	memset (pWriteCD, 0, iSize);

	// Fill in the basic information
	if (BURNTYPE_SVIDEOCD == params.GetBurnType())
	{
		pWriteCD->nwvcdSVCD = true;
	}
	else
	{
		pWriteCD->nwvcdSVCD = false;
	}

	pWriteCD->nwvcdNumItems = params.GetNumberOfTracks();

	pItem = NULL;

	try
	{
		// Get the ISO track from the information in the parameters.
		code = GetIsoTrack (params, &pWriteCD->nwvcdIsoTrack, &pItem);
		if (code != EXITCODE_OK)
		{
			throw code;
		}

		for (int i = 0; i < params.GetNumberOfTracks(); i ++)
		{
			// Helper variables to promote readability
			int iNameSize = sizeof (pWriteCD->nwvcdItems[i].nviSourceFileName);
			LPCSTR sFileName = params.GetTrackFileName(i);
			NERO_VIDEO_ITEM* item = &pWriteCD->nwvcdItems[i];

			strncpy (item->nviSourceFileName, sFileName, iNameSize);
			item->nviSourceFileName[iNameSize - 1] = 0;

			// MPEG and JPEG are allowed types
			// stricmp performs a lowercase comparison and returns 0 if the strings are identical.
			// We fill in type information and file name.
			LPCSTR psExt = strrchr (sFileName, '.');

			if (psExt && (!stricmp (psExt, ".mpg") || !stricmp (psExt, ".mpeg")))
			{
				item->nviItemType = NERO_MPEG_ITEM;

				// no pause after mpeg items
				item->nviPauseAfterItem=0;
			}
			else if (psExt && (!stricmp (psExt, ".avi")))
			{
				item->nviItemType = NERO_NONENCODED_VIDEO_ITEM;
				
				// no pause after avi items
				item->nviPauseAfterItem=0;
			}
			else if (psExt && (!stricmp (psExt, ".jpg") || !stricmp (psExt, ".jpeg")))
			{
				item->nviItemType = NERO_JPEG_ITEM;

				// infinite pause after jpeg items
				item->nviPauseAfterItem=(DWORD)(-1);
			}
			else
			{
#ifdef _UNICODE
				//Klammer muss sein, da sonst str nicht zerstört wird, denn nach dem
				//throw wird der Scope nicht verlassen
				{
					CWK_String str(params.GetTrackFileName(i));
					WK_TRACE(_T("Unknown file type '%s'\n"), str);
				}
#else
				WK_TRACE(_T("Unknown file type '%s'\n"), params.GetTrackFileName(i));
#endif
				throw EXITCODE_UNKNOWN_FILE_TYPE;
			}
		}

		NEROAPI_BURN_ERROR err;

		// Perform the actual burn process
		err = NeroBurn (m_NeroDeviceHandle,
						NERO_VIDEO_CD,
						pWriteCD,
						GetBurnFlags (params),
						params.GetSpeed(),
						&s_NeroProgress);

		if (NEROAPI_BURN_OK != err)
		{
			GetLastErrorLogLine();
		}

		code = TranslateNeroToExitCode (err);
	}
	catch (EXITCODE e)
	{
		code = e;
	}

	// Free allocated memory
	NeroFreeIsoTrack (pWriteCD->nwvcdIsoTrack);
	DeleteIsoItemTree (pItem);
	delete [] (char *)pWriteCD;

	return code;
}

