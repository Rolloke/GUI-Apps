
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the MPEG4SPENC_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// MPEG4SPENC_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef MPEG4SPENC_EXPORTS
#define MPEG4SPENC_API __declspec(dllexport)
#else
#define MPEG4SPENC_API __declspec(dllimport)
#endif

#ifndef _MPEG4SPENC_LEAN

#define E_INVALID_SIZE					(-1001)
#define E_INVALID_PARAMS				(-1002)
#define E_INVALID_ARGS					(-1003)
#define E_OUTBUF_OVERFLOW				(-1004)
#define E_SUCCESS            0
#define E_FAILURE           (-1)
#define E_OUT_OF_MEMORY     (-2)
#define E_OUT_OF_RANGE      (-3)
#define E_FILE_CREATE_FAIL  (-4)
#define E_UNDEFINED_FLAG    (-5)

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

typedef unsigned int         uint32;
typedef float                flt32;
typedef int                  int32;
typedef unsigned char        uint8;

typedef struct 
{
	int32 i;
}tBaseVideoEncoder;

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

#endif

MPEG4SPENC_API int32 CreateMPEG4VideoEncode(tBaseVideoEncoder** handle, tMPEG4VideoEncParam* param);

MPEG4SPENC_API int32 MP4SPEncodeFrame(tBaseVideoEncoder *base,
								 uint8 *frameInBuf,
								 uint8 *outputBuf, 
								 int32 *numBytes, 
								 uint32* PktSizBuf, 
								 uint32 timestamp);

MPEG4SPENC_API int32 DestroyMPEG4VideoEncode(tBaseVideoEncoder* handle);

MPEG4SPENC_API int32 SetParamMPEG4Enc(tBaseVideoEncoder *base, uint32 flag, uint32 val);

MPEG4SPENC_API int32 GetParamMPEG4Enc(tBaseVideoEncoder *base, uint32 flag, uint32 *val);



