/*
*******************************************************************************
                       Copyright eMuzed Inc., 2002-2003.
All rights Reserved, Licensed Software Confidential and Proprietary Information 
    of eMuzed Incorporation Made available under Non-Disclosure Agreement OR
                            License as applicable.
*******************************************************************************
*/

/*
*******************************************************************************
Product     : MPEG-4 Video Codec
Module      : Decoder Interface.
File        : exp_common_VD.h
Description : This file contains common data types and functions used by
			  MPEG-4 Simple Profile Video decoder and H263 baseline Video decoder 

Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date            Id          Author            Comment 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
May 12, 2003              Jayaprakash Pai    Initial code

*******************************************************************************
*/

#ifndef INCLUDE_EXP_COMMON_VD
#define	INCLUDE_EXP_COMMON_VD

#include "common.h"

#ifndef STATIC_LIB
#ifdef MP4SPVD_EXPORTS
#define MP4SPVDAPI __declspec(dllexport)
#else
#define MP4SPVDAPI __declspec(dllimport)
#endif
#else
#define MP4SPVDAPI 
#endif

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/* Stream types that can be handled by the Video Decoder */
enum {VD_H263_BASELINE_STREAM, VD_MPEG4_SP_STREAM, VD_NOT_SUPPORTED_STREAM};


/* Stream information data structure */
typedef struct streamInfo
{
	int32	streamType;	/* Compression type	*/
	uint16	width;		/* Actual width of VOP */
	uint16	height;		/* Actual height of VOP */
	uint16	extWidth;	/* Extended width of VOP */
	uint16	extHeight;	/* Extended height of VOP */

} tStreamInfo;


/* Post-processing types supported by the Video Decoder */
enum {VD_NO_POSTPROCESSING, VD_SIMPLE_DEBLOCKING, VD_COMPLEX_DEBLOCKING};


/* Flags for get/set parameters */
enum
{
	VD_POSTPROC_LEVEL,
	VD_CURRENT_YUV_FRAME,
	VD_COMPLETE_FRAME,
	VD_CONTENT_INFO
};

typedef struct BaseVideoDecoder tBaseVideoDecoder;
/* Base video decoder object */
struct BaseVideoDecoder
{
	/* Function pointer to decode a frame */
	int32   (__fastcall *vDecodeFrame) (tBaseVideoDecoder *base, uint8 *stream,
							 int32 *streamLength, tBaseVideoFrame *outBuffer);

	/* Function pointer to decode a packet */
	int32   (__fastcall *vDecodePacket) (tBaseVideoDecoder *base, uint8 *stream,
							  int32 *streamLength);

	/* Function pointer to set parameters */
	int32   (__fastcall *vSetParam) (tBaseVideoDecoder *base, uint32 flag, uint32 val);

	/* Function pointer to get a specified parameter value */
	int32   (__fastcall *vGetParam) (tBaseVideoDecoder *base, uint32 flag, uint32 *val);

	/* Function pointer to get a specified parameter value */
	int32   (__fastcall *vReset) (tBaseVideoDecoder *base);

	/* Function pointer to delete decoder instance */
	int32   (__fastcall *vDelete) (tBaseVideoDecoder *base);

};

/* Structure for display window related parameters */
typedef struct RGBWindowParam
{
	int32 wndWidth;		/* Display window width */
	int32 wndHeight;	/* Display window height */
	int32 xOffset;		/* x-offset of the displayed frame in display window */
	int32 yOffset;		/* y-offset of the displayed frame in display window */
	int32 rotateFlag;	/* Takes values from 0-3 for 0, 90, 180, 270 degrees */

} tRGBWindowParam;


/* This function parses the input stream and returns stream type, actual width
   and height of video object. For MPEG-4 SP video stream the input stream is 
   configuration information and for H263 streams the input stream is the 
   frame header of the first frame */
MP4SPVDAPI
int32 __fastcall gGetStreamInfo(uint8 *configStream, int32 configLength, 
					 tStreamInfo *strmInfo);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif  /* INCLUDE_EXP_COMMON_VD */

