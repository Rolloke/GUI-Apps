/*
*******************************************************************************
                       Copyright eMuzed Inc., 2001-2003.
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

#include "baseVideoEnc.h"
//#include "common.h"

#define NO_FRAME -2
#define SET_BIT_RATE 0
#define SET_PACKET_SIZE 1
#define RESET_ENCODER 2
#define SET_GOVLENGTH 3
#define SET_LEVEL 4

#define GET_BIT_RATE 10
#define GET_PACKET_SIZE 11
#define GET_VBV_SIZE 12
#define GET_MAX_FRAME_SIZE	13
#define GET_MAX_NUM_PACKETS	14

typedef struct{
    uint32  VOPHeight;
    uint32  VOPWidth;
    flt32   VOPRate;
    uint32  BitRate;
    uint32  PacketSize;
	int32   ResyncMarkerDisable; 
    int32   ReversibleVLC;       
	int32   DataPartitioning;   
    int32   UnrestrictedMV;      
    int32   Enable4MV;           
    int32   Level;     
	int32   BufferSize;
	int32   GOVLength;
    int32   MAIR;
    int32   MAPS;
    int32   RemoveImpulseNoise;
    int32   DeInterlace;
    int32   RCModel;
	int32	ShortVideoHeader;
	int32	EncoderPerformance;
	uint32	TimerResolution;
	int32	SearchRange;
	int32	AdaptiveSearchRange;
	int32	BitErrorRate;
	int32	PacketLossRate;
	int32	NumGOBHeaders;
	int32	SceneChangeDetection;
	int32	ACPrediction;
	int32	HeaderExtCode;
	int32	IntraDCVLCThr;
}tMPEG4VideoEncParam;

extern int32 gCreateMPEG4VideoEncode(tBaseVideoEncoder**, tMPEG4VideoEncParam*);

extern int32 gMP4SPEncodeFrame(tBaseVideoEncoder *base,
								 uint8 *frameInBuf,
								 uint8 *outputBuf, 
								 int32 *numBytes, 
								 uint32* PktSizBuf, 
								 uint32 timestamp);

extern int32 gDestroyMPEG4VideoEncode(tBaseVideoEncoder*);

extern int32 gSetParamMPEG4Enc(tBaseVideoEncoder *base, uint32 flag, uint32 val);

extern int32 gGetParamMPEG4Enc(tBaseVideoEncoder *base, uint32 flag, uint32 *val);

#endif