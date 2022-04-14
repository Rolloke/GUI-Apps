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
File        : exp_SP_MPEG4_Enc.h
Description : base video encoder structure for MPEG-4 SP Video encoder module.

Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date            Id          Author              Comment 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Mar 12, 2001                K. Ramkishor        Initial code
*******************************************************************************
*/
#ifndef INCLUDE_EXP_SP_MPEG4_ENC
#define INCLUDE_EXP_SP_MPEG4_ENC

#include "common.h"
#include "baseVideoEnc.h"



#define GET_BIT_RATE			10
#define GET_PACKET_SIZE			11
#define GET_MAX_FRAME_SIZE		12
#define GET_INPUT_FRAME_SIZE	13
#define GET_SEQ_ENDCODE_LENGTH	14
#define GET_SEQ_ENDCODE			15
#define GET_MAX_PACKETS         16 

typedef struct{
    uint32  VOPHeight;				/* Frame Height */
    uint32  VOPWidth;				/* Frame Width */
    flt32  VOPRate;				    /* Frame rate */
    uint32  BitRate;				/* Bit rate */
	uint32  Level;					/* Level parameter */	
    uint32  DeInterlace;			/* Deinterlaceing  */
    uint32  RCModel;				/* Rate control model*/
    uint32  SVHMode;				/* Shortvideo header/h.263 mode*/
    uint32  PacketMode;				/* Packet mode*/
    uint32  PacketSize;				/* Packet Size*/
    uint32	AIR;					/* Adaptive Intra Refresh */
    uint32	GOVLength;				/* Distance between two I-frames */
    uint32  SearchRange;			/* ME search range */
	uint32  EncoderPerformance;		/* Encoder Performance level*/
	uint32	TimerResolution;		/* Resoultion if the timer used for time stamp*/	
}tMPEG4VideoEncParam;

#endif



