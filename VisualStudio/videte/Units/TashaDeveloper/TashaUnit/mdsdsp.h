/****************************************************************************
;
;	File:	mdsdsp.h
;
;***************************************************************************/

#ifndef __MDSDSP_H__
#define __MDSDSP_H__

#include "mds_error_codes.h"
#include <windows.h>

/* ------------------------------------------------------------------------ */

enum mdsdspMemorySpace
{
	MDS_DSP_MEM_PROGRAM	= 0,
	MDS_DSP_MEM_X_DATA	= 1,
	MDS_DSP_MEM_DATA	= 1,			   
	MDS_DSP_MEM_Y_DATA	= 2,
	MDS_DSP_MEM_IO      = 3,
};

enum mdsdspDspIndex
{
	MDS_DSP_INDEX_A = 0,
	MDS_DSP_INDEX_B = 1,
	MDS_DSP_INDEX_C = 2,
	MDS_DSP_INDEX_D = 3
};

enum mdsBoardID
{
	MDS_BOARD_UNKNOWN		= 0,
	MDS_BOARD_IGUANA		= 1,
	MDS_BOARD_HUMMINGBIRD	= 2,
	MDS_BOARD_ESP202		= 3,
	MDS_BOARD_HAWK89		= 4,
	MDS_BOARD_HAWK81		= 5,
	MDS_BOARD_RAPTOR		= 6,
	MDS_BOARD_FALCON		= 7,
	MDS_BOARD_OPUS			= 8,
	MDS_BOARD_GECKO			= 9,
	MDS_BOARD_HAWK35		= 10,
};

/* ------------------------------------------------------------------------ */

#if defined(__cplusplus)
extern "C"
{
#endif

/* ........................................................................ */

enum mdsErrorCode	mdsBoardOpen(
						enum mdsBoardID			boardID,
						int						*pboardInstance,
						int						reserved1,
						int						reserved2,
						HANDLE					*pboardHandle);

enum mdsErrorCode	mdsBoardClose(
						HANDLE					boardHandle);

enum mdsErrorCode	mdsBoardDownLoad(
						HANDLE					boardHandle,
						char					*DSPFileName,
						enum mdsdspDspIndex		dspIndex);

enum mdsErrorCode	mdsBoardGetDriverVersion(
						HANDLE					boardHandle,
						int						*pVersion);

enum mdsErrorCode	mdsBoardGetFirmwareVersion(
						HANDLE					boardHandle,
						int						*pVersion,
						mdsdspDspIndex		dspIndex);

enum mdsErrorCode	mdsBoardGetLibraryVersion(
						int						*pVersion);

enum mdsErrorCode	mdsBoardReadMemory(
						HANDLE					boardHandle,
						enum mdsdspMemorySpace	memSpc,
						int						length,
						int						address,
						void					*dataBuffer,
						enum mdsdspDspIndex		dspIndex);

enum mdsErrorCode	mdsBoardWriteMemory(
						HANDLE					boardHandle,
						enum mdsdspMemorySpace	memSpc,
						int						length,
						int						address,
						void					*dataBuffer,
						enum mdsdspDspIndex		dspIndex);

enum mdsErrorCode	mdsBoardInterruptDsp(
						HANDLE					boardHandle,
						enum mdsdspDspIndex		dspIndex,
						int						reserved1,
						int						reserved2);

enum mdsErrorCode	mdsBoardSetInterruptCallback(
					HANDLE					boardHandle,
					enum mdsdspDspIndex		dspIndex,
					void					(__stdcall *pFunction) (void *),
					void					*pUserData,
					int						reserved1,
					int						reserved2);

/* ........................................................................ */

#if defined(__cplusplus)
}
#endif

/* ------------------------------------------------------------------------ */

#endif /* __MDSDSP_H__ */
