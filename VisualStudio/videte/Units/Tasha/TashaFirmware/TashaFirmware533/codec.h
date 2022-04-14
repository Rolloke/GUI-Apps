/********************************************************************/
/* Autor: Christian Lohmeier										*/
/* Datum: 26.08.2004												*/
/* Firma: Videte IT	AG												*/
/********************************************************************/
#ifndef  __CODEC_DEFINED
	#define __CODEC_DEFINED
	
#include "export_hdr\exp_SP_MPEG4_Enc.h"
///////////////////////////////////////////////////////////////////////////////	
// Definitionen 
///////////////////////////////////////////////////////////////////////////////	

 
///////////////////////////////////////////////////////////////////////////////	
// Prototypes
///////////////////////////////////////////////////////////////////////////////
extern "C" int32 gEzdMP4SPEnc_Create(tBaseVideoEncoder**, tMPEG4VideoEncParam*);
extern "C" int32 gEzdMP4SPEnc_EncodeVideoFrame(tBaseVideoEncoder* base , uint8* FrameData ,
						uint8 *OutputData,uint32 *NumBytes,uint32 *vopType,
						uint32	*PacketSizes,uint32 timestamp);
extern "C" int32 gEzdMP4SPEnc_Delete(tBaseVideoEncoder*);
extern "C" int32 gEzdMP4SPEnc_GetParam(tBaseVideoEncoder*, uint32, uint32*);

bool makePlanar(ImageRes eImageRes1, ImageRes eImageRes2, BYTE* pSorce);
void codec1();
void codec2();
bool DoCoding();
bool DoFiltering(BUFFER* pBuffer, int nStreamID);
void IniCodecBuffer(BYTE byNum);
void IniCodec(BYTE byNum);
bool DeleteCodec(BYTE byNr);
void CopyCif();
bool RequestEnableNR(BOOL bEnable);

///////////////////////////////////////////////////////////////////////////////

#endif //__CODEC_DEFINED
