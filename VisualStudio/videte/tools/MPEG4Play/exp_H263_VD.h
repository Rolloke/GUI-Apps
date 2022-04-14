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
File        : exp_H263_VD.h
Description : This file contains API to create H263 baseline Video decoder 

Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date            Id          Author            Comment 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
May 12, 2003              Jayaprakash Pai    Initial code

*******************************************************************************
*/

#ifndef INCLUDE_EXP_H263_VD
#define	INCLUDE_EXP_H263_VD

#include "exp_common_VD.h"

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

/* This function creates an instance of H263 baseline video decoder */
MP4SPVDAPI
int32 __fastcall gCreateH263Decoder(tBaseVideoDecoder **base, uint16 width, uint16 height, 
                                    int32 postProcType, tRGBWindowParam *wndParam);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif  /* INCLUDE_EXP_H263_VD */

