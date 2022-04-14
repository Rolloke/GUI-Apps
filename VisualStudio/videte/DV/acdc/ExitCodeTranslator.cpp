// This function does simple translation from NeroAPI's burn error
// into our own EXITCODE.

#include "stdafx.h"
#include "BurnContext.h"

EXITCODE CBurnContext::TranslateNeroToExitCode (NEROAPI_BURN_ERROR err)
{
	switch (err)
	{
		case NEROAPI_BURN_OK:
			return EXITCODE_OK;

		case NEROAPI_BURN_UNKNOWN_CD_FORMAT:
			return EXITCODE_UNKNOWN_CD_FORMAT;

		case NEROAPI_BURN_INVALID_DRIVE:
			return EXITCODE_INVALID_DRIVE;

		case NEROAPI_BURN_FAILED:
			{
				if(GetAbortFlag())
				{
					//if deleting non empty RW-medium and user canceled during deleting
					//returncode is NEROAPI_BURN_FAILED because Nero recognizes canceling
					//during deleting after RW medium is deleted. 
					//in Nero (original) deleting RW mediums is not cancelble
					return EXITCODE_USER_ABORTED;
				}
				else
				{
					return EXITCODE_BURN_FAILED;
				}
			}

		case NEROAPI_BURN_FUNCTION_NOT_ALLOWED:
			return EXITCODE_FUNCTION_NOT_ALLOWED;

		case NEROAPI_BURN_DRIVE_NOT_ALLOWED:
			return EXITCODE_DRIVE_NOT_ALLOWED;

		case NEROAPI_BURN_USER_ABORT:
			return EXITCODE_USER_ABORTED;

		default:
			return EXITCODE_INTERNAL_ERROR;
	}
}