/****************************************************************************

	File:	mds_error_codes.h

	Error codes used by all MDS board interfaces.

;***************************************************************************/

#ifndef __MDS_ERROR_CODES_H__
#define __MDS_ERROR_CODES_H__

/*........................................................................*/
// Define possible error codes as well as error code data type.

enum mdsErrorCode
{ 	MDS_ERROR_NO_ERROR						= 0,
	MDS_ERROR_GENERAL						= 1,
	MDS_ERROR_BAD_ARG						= 2,
	MDS_ERROR_FILE_ACCESS					= 3,
	MDS_ERROR_BOARD_IN_USE					= 4,
	MDS_ERROR_BOARD_NOT_PRESENT				= 5,
	MDS_ERROR_BOARD_OPEN_FAIL				= 6,
	MDS_ERROR_BOARD_CLOSE_FAIL				= 7,
	MDS_ERROR_BOARD_RESET_ERROR				= 8,
	MDS_ERROR_BOARD_BOOT_LOAD_FAIL			= 9,
	MDS_ERROR_BOARD_LOAD_ERROR				= 10,
	MDS_ERROR_BOARD_MEMORY_ACCESS			= 11,
	MDS_ERROR_BOARD_MEMORY_WRITE			= 12, 
	MDS_ERROR_BOARD_MEMORY_READ				= 13,
	MDS_ERROR_BOARD_INVALID_DSP_INDEX		= 14,
	MDS_ERROR_BOARD_UNSUPPORTED_OS			= 15,
	MDS_ERROR_BOARD_UNSUPPORTED_FUNCTION	= 16,
	MDS_ERROR_BOARD_INVALID_MEM_SPACE		= 17,
	MDS_ERROR_MEMORY_ALLOCATION				= 18,
	MDS_ERROR_BOARD_TIMEOUT					= 19
};

/*........................................................................*/
// Legacy error codes to maintain backward compatibility
//		note: first two are quivalent to kOK, kError in ac3e_api.dll
enum { 	kGood = 0,
		kBad,
		boardInUse,
		boardNotPresent,
		bootLoadError,
		memoryError,
		fileError,
		dspLoadError,
		dspMemWriteError, 
		dspMemReadError,
		invalidDspIndex};

/*........................................................................*/

#endif	/* __MDS_ERROR_CODES_H__ */