/*
*******************************************************************************
                       Copyright eMuzed Inc., 2001-2002.
All rights Reserved, Licensed Software Confidential and Proprietary Information 
    of eMuzed Incorporation Made available under Non-Disclosure Agreement OR
                            License as applicable.
*******************************************************************************
*/

/*
*******************************************************************************
Product     : Streaming Solutions
Module      : MPEG-4 Video Encoder
File        : baseMPEG4Enc.h
Description : base video encoder structure for MPEG-4 SP Video encoder module.

Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date            Id          Author              Comment 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Mar 14, 2001                K. Ramkishor        Initial code
*******************************************************************************
*/
#ifndef INCLUDE_BASEVIDEOENC
#define INCLUDE_BASEVIDEOENC


/* Error codes specific to encoder */
#define E_INVALID_SIZE					(-1001)
#define E_INVALID_PARAMS				(-1002)
#define E_INVALID_ARGS					(-1003)
#define E_OUTBUF_OVERFLOW				(-1004)


/* Enumerated data type for Encoder performance (complexity) parameter */
enum {VENC_LOW, VENC_HIGH};
/* Enumerated data type for Encoder rate control mode */
enum {VENC_CBR, VENC_VBR, VENC_UVBR};

/* Enumerated data type for VOP coded type */
enum {VENC_IVOP, VENC_PVOP};


typedef int32 tBaseVideoEncoder;


#endif

