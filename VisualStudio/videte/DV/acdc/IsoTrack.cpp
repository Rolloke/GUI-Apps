/******************************************************************************
|*	This file keeps all ISO tree handling relevant functions.
|*  We have 3 ISO functions:
|*
|*	implemented main functions:
|*		-	GetIsoTrack			-	creates a CNeroIsoTrack from the user supplied parameters
|*		-	DeleteIsoItemTree	-	deletes the iso tree recursively
|*		-	CreateIsoTree		-	searches for a specified path and recursively adds
|*									all files and directories that our found.
|*
|*	PROGRAM: IsoTrack.cpp
|*
******************************************************************************/

#include "stdafx.h"
#include "BurnContext.h"
#include "FindFiles.h"

/*********************************************************************************************
 Class      : CBurnContext
 Function   : GetIsoTrack 
 Description: This function creates a CNeroIsoTrack from the user supplied parameters.
			  It imports previous session (if any) and builds the file and directory tree.

 Parameters:   
  params	: (I):  reference to the parameters  (const PARAMETERS &)
  ppIsoTrack: (I):  Nero iso track, hold all track informations for burn progress (CNeroIsoTrack **)
  ppItem    : (I):  Nero iso item, holds all iso informations for burn progress (NERO_ISO_ITEM **)

 Result type: returns 0 if successful or an error code if not  (EXITCODE)

 Author: TKR
 created: July, 12 2002
 <TITLE GetIsoTrack >
 <GROUP IsoTrack>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, IsoTrack, GetIsoTrack>
*********************************************************************************************/
EXITCODE CBurnContext::GetIsoTrack (const PARAMETERS & params, CNeroIsoTrack** ppIsoTrack, NERO_ISO_ITEM** ppItem)
{
	*ppIsoTrack = NULL;

	// Only import a session if a session number has been provided
	if (-1 != params.GetSessionToImport())
	{

		// Make sure that NeroGetCDInfo has not been called before to prevent multiple 
		// allocation of memory for NERO_CD_INFO.
//		_ASSERTE (m_NeroCDInfo == NULL);

		// First get the CD info to obtain information about all available sessions
		if(m_NeroCDInfo == NULL)
		{
			m_NeroCDInfo = NeroGetCDInfo (m_NeroDeviceHandle, 0);
		}

		if (NULL == m_NeroCDInfo)
		{
			return EXITCODE_ERROR_GETTING_CD_INFO;
		}

		// Check the numbers and make sure that the requested session exists.
		if (params.GetSessionToImport() > (int)(m_NeroCDInfo->ncdiNumTracks))
		{
			return EXITCODE_BAD_IMPORT_SESSION_NUMBER;
		}

		DWORD dwFlags = 0;

		// Determine the format to be imported
		if (true == params.GetImportRockridge())
		{
			dwFlags |= NIITEF_IMPORT_ROCKRIDGE;
		}
		if (true == params.GetImportIsoOnly())
		{
			dwFlags |= NIITEF_IMPORT_ISO_ONLY;
		}
		if (true == params.GetPreferRockRidge())
		{
			dwFlags |= NIITEF_PREFER_ROCKRIDGE;
		}

		// Now, import the session. If the function fails, it's probably
		// due to no CD in drive.
		// NeroImportIsoTrackEx creates a NERO_ISO_ITEM tree from an already
		// existing ISO track in order to create a new session with reference
		// to files from older sessions.
		// m_pCDStamp will be filled with a pointer to a CDStamp object
		// which will have to be freed later by the CBurnContext destructor.

		// ppItem is a reference to a pointer to a NERO_ISO_ITEM
		// So to get the required pointer to a NERO_ISO_ITEM we have to
		// de-reference ppItem once.
		*ppItem = NeroImportIsoTrackEx (m_NeroDeviceHandle,
										params.GetSessionToImport(),
										&m_pCDStamp,
										dwFlags);

		// The NERO_ISO_ITEM pointer must not be NULL
		if (NULL == *ppItem)
		{
			GetLastErrorLogLine();
			return EXITCODE_NO_CD_INSERTED;
		}

	}  // if (-1 != params.m_iSessionToImport)

	// Iterate through the file list and it each one to the tree.
	// If directory is stumbled upon, recurse it and it all of
	// its contents.
	for (int i = 0; i < params.GetFileListSize(); i ++)
	{
		NERO_ISO_ITEM* pItem;
		EXITCODE code;

		// Create a tree from the filename supplied.

//		int iFileListSize = params.GetFileListSize();
		CSimpleStringArray ssa = params.GetFileList();
		code = CreateIsoTree (params.GetFileList().vect[i], &pItem);

		// If there was a problem creating the tree then delete 
		// the whole tree that has been created so far and
		// return the error code.
		if (code != EXITCODE_OK)
		{
			WK_TRACE(_T("DeleteIsoItemTree %08lx at Failure\n"), ppItem);
			DeleteIsoItemTree (*ppItem);
			return code;
		}

		// Add the file to the beginning of the root directory.
		if (NULL != *ppItem)
		{
			pItem->nextItem = *ppItem;
		}
		*ppItem = pItem;
	}

	if (NULL != *ppItem)
	{
		DWORD dwFlags = 0;

		if (true == params.GetUseJoliet())
		{
			dwFlags |= NCITEF_USE_JOLIET;
		}
		if (true == params.GetUseMode2())
		{
			dwFlags |= NCITEF_USE_MODE2;
		}
		if (true == params.GetUseRockridge())
		{
			dwFlags |= NCITEF_USE_ROCKRIDGE;
		}
		if (true == params.GetCreateIsoFs())
		{
			dwFlags |= NCITEF_CREATE_ISO_FS;
		}
		if (true == params.GetCreateUdfFS())
		{
			dwFlags |= NCITEF_CREATE_UDF_FS;
		}
		
		// Finally, create the ISO track.
		*ppIsoTrack = NeroCreateIsoTrackEx (*ppItem,
											params.GetVolumeName(),
											dwFlags);
		
		// If the ISO track could not be created then delete the 
		// ISO item tree and return with an error
		if (NULL == *ppIsoTrack)
		{
			GetLastErrorLogLine();
			DeleteIsoItemTree (*ppItem);
			return EXITCODE_FAILED_TO_CREATE_ISO_TRACK;
		}
	}

	return EXITCODE_OK;
}
/*********************************************************************************************
 Class      : CBurnContext
 Function   : DeleteIsoItemTree 
 Description: This function deletes the iso tree recursively.

 Parameters:   
  pItem	: (I):  ISO tree item to delete  (NERO_ISO_ITEM *)

 Result type: --

 Author: TKR
 created: July, 12 2002
 <TITLE DeleteIsoItemTree >
 <GROUP IsoTrack>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, IsoTrack, DeleteIsoItemTree>
*********************************************************************************************/
void CBurnContext::DeleteIsoItemTree (NERO_ISO_ITEM * pItem)
{
	// Step through the tree until the 
	// ISO item tree pointer becomes NULL
	while (NULL != pItem)
	{
		NERO_ISO_ITEM* pNextItem = pItem->nextItem;

		if (TRUE == pItem->isDirectory)
		{
			// We have encountered another ISO item tree;
			// recurse another level.
			DeleteIsoItemTree (pItem->subDirFirstItem);
		}

		if (TRUE == pItem->isReference)
		{
			NeroFreeMem (pItem);
		}
		else
		{
			NeroFreeIsoItem (pItem);
		}

		pItem = pNextItem;
	}
}
/*********************************************************************************************
 Class      : CBurnContext
 Function   : CreateIsoTree 
 Description: This function searches for a specified path and recursively adds
			  all files and directories that our found.


 Parameters:   
  psFilename: (I):    List of files or directories to burn (LPCSTR)
  ppItem    : (I):    Nero iso item, holds all iso informations for burn progress (NERO_ISO_ITEM **)
  iLevel    : (I):    Level of recursion (int)

 Result type: returns 0 if successful or an error code if not  (EXITCODE)

 Author: TKR
 created: July, 12 2002
 <TITLE CreateIsoTree >
 <GROUP IsoTrack>
 <TOPICORDER 0>
 <KEYWORDS Nero, NeroAPI, IsoTrack, CreateIsoTree>
*********************************************************************************************/
EXITCODE CBurnContext::CreateIsoTree (LPCSTR psFilename, NERO_ISO_ITEM ** ppItem, int iLevel)
{
	// CFindFiles is a helper class for file and subdirectory handling 
	CFindFiles ff (psFilename);

	*ppItem = NULL;

	if (false == ff.IsValidEntry())
	{
		if (0 == iLevel)
		{
			// If we haven't found any entries and we are on the
			// first level of recursion then this should be
			// reported as an error.
			CWK_String sFilename(psFilename);
			WK_TRACE(_T("File specification '%s' resulted in no matches!\n"), (LPCTSTR)sFilename);
			m_pNeroBurn->SetFileNotFound((CString&)psFilename);
			return EXITCODE_FILE_NOT_FOUND;
		}
		else
		{
			// If we are on a level other than first, it is ok
			// not to find any entries. This simply means we
			// stumbled upon an empty directory.
			return EXITCODE_OK;
		}
	}

	char sPath[MAX_PATH];

	// Make sure that we have no relative path names, but only absolute paths
	if (NULL == _fullpath (sPath, psFilename, sizeof (sPath)))
	{
		// Our path buffer is too small. Bail out!
		return EXITCODE_INTERNAL_ERROR;
	}

	// Find the last blackslash and remove it if found.
	// This will leave us with a root directory.
	LPSTR psBackslash = strrchr (sPath, _T('\\'));
	if (NULL != psBackslash)
	{
		*psBackslash = 0;
	}

	do
	{
		char sNewPath[MAX_PATH];

		strcpy (sNewPath, sPath);

		// We need to calculate whether the string will fit.
		if (strlen (ff.m_fd.name) + strlen (sNewPath) + 1 >= sizeof (sNewPath))
		{
			DeleteIsoItemTree (*ppItem);
			return EXITCODE_INTERNAL_ERROR;
		}

		strcat (sNewPath, "\\");
		strcat (sNewPath, ff.m_fd.name);

		if (true == ff.IsSubDir())
		{
			// Here we handle subdirectories

			// strcmp returns 0 on equal strings.
			// Proceed if name contains none of _T(".") or ".."
			if ((0 != strcmp (ff.m_fd.name, ".")) && (0 != strcmp (ff.m_fd.name, "..")))
			{
				// We need to calculate whether the string will fit.
				if (strlen (sNewPath) + 2 >= sizeof (sNewPath))
				{
					DeleteIsoItemTree (*ppItem);
					return EXITCODE_INTERNAL_ERROR;
				}

				// Append a wildcard to the path and do a recursive search.
				strcat (sNewPath, "\\*");

				NERO_ISO_ITEM * pNewItem = NeroCreateIsoItem ();
				if (NULL == pNewItem)
				{
					DeleteIsoItemTree (*ppItem);
					return EXITCODE_OUT_OF_MEMORY;
				}

				memset (pNewItem, 0, sizeof (*pNewItem));

				// Attach this item to the beginning of the list.
				if (*ppItem)
				{
					pNewItem->nextItem = *ppItem;
				}
				*ppItem = pNewItem;

				pNewItem->isDirectory = TRUE;
				pNewItem->entryTime = *localtime (&ff.m_fd.time_create);
				strcpy (pNewItem->fileName, ff.m_fd.name);

				// Create an ISO item tree at a deeper level
				EXITCODE code = CreateIsoTree (sNewPath, &pNewItem->subDirFirstItem, iLevel + 1);
				if (EXITCODE_OK != code)
				{
					DeleteIsoItemTree (*ppItem);
					return code;
				}
			}
		}
		else
		{
			// Here we handle regular files
			NERO_ISO_ITEM * pNewItem = NeroCreateIsoItem ();
			if (NULL == pNewItem)
			{
				DeleteIsoItemTree (*ppItem);
				return EXITCODE_OUT_OF_MEMORY;
			}
			memset (pNewItem, 0, sizeof (*pNewItem));

			strcpy (pNewItem->sourceFilePath, sNewPath);
			pNewItem->isDirectory = FALSE;
			pNewItem->entryTime = *localtime (&ff.m_fd.time_create);
			strcpy (pNewItem->fileName, ff.m_fd.name);

			// Attach this item to the beginning of the list.
			if (*ppItem)
			{
				pNewItem->nextItem = *ppItem;
			}
			*ppItem = pNewItem;
		}

		ff.FindNext ();
	}
	while (ff.IsValidEntry ());

	return EXITCODE_OK;
}
