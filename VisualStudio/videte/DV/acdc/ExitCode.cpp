/******************************************************************************
|* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
|* ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
|* THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
|* PARTICULAR PURPOSE.
|* 
|* Copyright 1995-2002 Ahead Software AG. All Rights Reserved.
|*-----------------------------------------------------------------------------
|* NeroSDK / NeroCmd
|*
|* PROGRAM: ExitCode.cpp
|*
|* PURPOSE: Translation of numeric error code
******************************************************************************/


#include "stdafx.h"
#include "ExitCode.h"


// Translate the numeric error code into a textual representation.

LPCTSTR GetTextualExitCode (EXITCODE code)
{
	static struct 
	{
		EXITCODE code;
		LPCTSTR message;
	}
	errors[] =
	{
		EXITCODE_UNKNOWN,							_T("Unknown error."),
		EXITCODE_OK,								_T("Ok."),
		EXITCODE_BAD_USAGE,							_T("Bad usage!"),
		EXITCODE_INTERNAL_ERROR,					_T("Internal error!"),
		EXITCODE_NEROAPI_DLL_NOT_FOUND,				_T("NEROAPI.DLL was not found!"),
		EXITCODE_NO_SERIAL_NUMBER,					_T("Serial number was not found!"),
		EXITCODE_BAD_SERIAL_NUMBER,					_T("Serial number is invalid!"),
		EXITCODE_NO_CD_INSERTED,					_T("No medium was inserted!"),
		EXITCODE_NO_SUPPORT_FOR_CDRW,				_T("CDRW is not supported!"),
		EXITCODE_ERROR_ERASING_CDRW,				_T("There was an error erasing medium!"),
		EXITCODE_ERROR_OBTAINING_AVAILABLE_DRIVES,	_T("There was an error obtaining a list of available drives!"),
		EXITCODE_MISSING_DRIVENAME,					_T("Drive name is missing!"),
		EXITCODE_ERROR_OPENNING_DRIVE,				_T("There was an error while trying to access the drive!"),
		EXITCODE_DRIVE_NOT_FOUND,					_T("Drive was not found!"),
		EXITCODE_UNKNOWN_CD_FORMAT,					_T("Unknown medium format!"),
		EXITCODE_INVALID_DRIVE,						_T("Invalid drive!"),
		EXITCODE_BURN_FAILED,						_T("Burn process failed!"),
		EXITCODE_FUNCTION_NOT_ALLOWED,				_T("Function was not allowed!"),
		EXITCODE_DRIVE_NOT_ALLOWED,					_T("Drive was not allowed!"),
		EXITCODE_ERROR_GETTING_CD_INFO,				_T("Medium not found! Error obtaining medium Info."),
		EXITCODE_TRACK_NOT_FOUND,					_T("Track was not found!"),
		EXITCODE_UNKNOWN_FILE_TYPE,					_T("File type is unknown!"),
		EXITCODE_DAE_FAILED,						_T("Digital audio extraction failed!"),
		EXITCODE_ERROR_OPENNING_FILE,				_T("There was an error openning the file!"),
		EXITCODE_OUT_OF_MEMORY,						_T("Out of memory!"),
		EXITCODE_ERROR_DETERMINING_LENGTH_OF_FILE,	_T("Error determining the length of input file!"),
		EXITCODE_EJECT_FAILED,						_T("Eject/Load failed!"),
		EXITCODE_BAD_IMPORT_SESSION_NUMBER,			_T("Invalid session number to import!"),
		EXITCODE_FAILED_TO_CREATE_ISO_TRACK,		_T("Failed to create ISO track!"),
		EXITCODE_FILE_NOT_FOUND,					_T("Specified file was not found!"),
		EXITCODE_USER_ABORTED,						_T("User aborted!"),
		EXITCODE_DEMOVERSION_EXPIRED,				_T("Demo version has expired!"),
		
		//new Exitcodes by TKR
		EXITCODE_WRONG_NERO_VERSION,	_T("Invalid Nero version, must be at least version 5.5.8.2."),
		EXITCODE_NO_BURN_SOFTWARE,		_T("no burn software is installed."),
		EXITCODE_UNKNOWN_MEDIUM,		_T("drive failed to get mediums capacity.")
	};

	for (int i = 0; i < sizeof (errors)/sizeof (errors[0]); i ++)
	{
		if (code == errors[i].code)
		{
			return errors[i].message;
		}
	}

	return errors[0].message;
}
