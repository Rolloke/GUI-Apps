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
Product     : Common module 
Module      : 
File        : common.h
Description : This is the commom header to be used by all modules.

Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date            Id          Author          Comment 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Jan 31, 2001                Satish Nayak    Initial code

*******************************************************************************
*/

#ifndef INCLUDE_COMMON
#define INCLUDE_COMMON


typedef signed char          int8;
typedef unsigned char        uint8;
typedef short int            int16;
typedef unsigned short int   uint16;
typedef int                  int32;
typedef unsigned int         uint32;

typedef float                flt32;
typedef double               flt64;

typedef unsigned char        tBool;
typedef signed int           tError;


#define E_TRUE               1
#define E_FALSE              0

#define E_ON                 1
#define E_OFF                0

#define E_DEBUG              1
#define E_RELEASE            0

#define E_SUCCESS            0
#define E_FAILURE           (-1)
#define E_OUT_OF_MEMORY     (-2)
#define E_OUT_OF_RANGE      (-3)
#define E_FILE_CREATE_FAIL  (-4)
#define E_UNDEFINED_FLAG    (-5)

#define E_USER_ERROR_BASE   (-1000)

#define mIsSuccess(code)    ((code)>=0)
#define mIsFailure(code)    ((code)<0)

/* This is the data structure for the output video frame buffer */
typedef struct BaseVideoFrame
{
    uint8  *lum;       /* Luminance pointer */
	uint8  *cb;        /* Cb pointer */
	uint8  *cr;        /* Cr pointer */
	uint16  width;     /* Width of the frame */
	uint16  height;    /* Height of the frame */
	uint32  timeStamp; /* Playback Timestamp */

} tBaseVideoFrame;

/* This is the data structure for the output audio frame buffer */
typedef struct BaseSpFrame
{
    int32    *data;         /* Raw data pointer */
    int32    SamFreq;       /* Sampling Frequency */
    uint16   noOfSamples;   /* Number of samples in the data buffer */
	uint32	 noOfChannels;	/* Number of output channels */
}tBaseSpFrame;

/* This is the data structure for the input audio frame buffer */
typedef struct ACapBaseFrame
{
	int16   *data;             /* Pointer to audio samples */
	uint16   numberOfSample;   /* Number of samples in data buffer*/

} tACapBaseFrame;

/* This is the data structure for the input video frame buffer */
typedef struct VCapBaseFrame
{
	uint8  *data;        /* Pointer to YUV planar data */
	uint32 length;       /* Length of YUV buffer       */
	uint32 timeStamp;    /* Capture time-stamp in msecs */

} tVCapBaseFrame;

#endif  /* INCLUDE_COMMON */
