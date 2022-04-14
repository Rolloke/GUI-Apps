//------------------------------------------------------------------------------
// File: IDump.h
//
// Desc: Custom Interface to receive MediaSamples via notifications, via
//       a callback function or save sampledata into a file.
//
// Copyright (c) 1992-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------


#ifndef __IDUMP__
#define __IDUMP__

DEFINE_GUID(CLSID_Dump,
0x36a5f770, 0xfe4c, 0x11ce, 0xa8, 0xed, 0x00, 0xaa, 0x00, 0x2f, 0xea, 0xb5);

#define EC_RECEIVE_MEDIA_SAMPLE (EC_USER + 2)

typedef HRESULT  (WINAPI*RECEIVE_FUNCTION)(IMediaSample*, long, long);

#ifdef __cplusplus
extern "C" {
#endif

//
// IDump's GUID
//
DEFINE_GUID(IID_IDump, 
0x85ec5623, 0x9557, 0x445f, 0x89, 0xab, 0x34, 0xcc, 0xba, 0x5f, 0xcc, 0x10);


//
// IDump interface of CDumpInputPin
// retrieve throught the input pin
DECLARE_INTERFACE_(IDump, IUnknown) {

   STDMETHOD(SetReceiveFunction) (THIS_
               RECEIVE_FUNCTION pFnc, // [in] : Pointer to function to receive IMediaSample(s)
               long lParam            // [in] : additional Parameters
             ) PURE;
   
   STDMETHOD(SetAcceptedMediaTypes) (THIS_
                AM_MEDIA_TYPE *pMT,		// [in] : MediaType info
					 int nCount,				// [in] : No. Of Types
					 BOOL bEnumerate			// [in] : Specifies that the accepted types may be enumerated
             ) PURE;                   // by EnumMediaTypes(..)
													// This may be helpfull for Negotiating Media Types
   STDMETHOD(GetAllocatorProperties) (THIS_
             ALLOCATOR_PROPERTIES *pAP // [out] : Allocator Properties
             ) PURE;
};


#ifdef __cplusplus
}
#endif


#endif // __IDUMP__


