//------------------------------------------------------------------------------
// File: IInfTee.h
//
// Desc: Custom interface to split one input pin into several output pins with
//       the ability to switch on or off each single output pin.
//
// Copyright (c) 1992-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------


#ifndef __IINF_TEE__
#define __IINF_TEE__


#define CLSID_INF_TEE "022B8142-0946-11cf-BCB1-444553540000"
#define GUID_IINF_TEE "998BCC0D-5948-4ce9-B34C-0E4720CBBBED"

DEFINE_GUID(CLSID_InfiniteTee,
0x22b8142, 0x946, 0x11cf, 0xbc, 0xb1, 0x44, 0x45, 0x53, 0x54, 0x0, 0x0);

#ifdef __cplusplus
extern "C" {
#endif

typedef HRESULT (WINAPI*DATACHECK_FUNCTION)(IMediaSample*, long);

//
// IInfTee's GUID
//
DEFINE_GUID(IID_IInfiniteTee, 
0x998bcc0d, 0x5948, 0x4ce9, 0xb3, 0x4c, 0xe, 0x47, 0x20, 0xcb, 0xbb, 0xed);

//
// IInfTee interface of CTeeInputPin
// retrieve through the input Pin
DECLARE_INTERFACE_(IInfTee, IUnknown) {

	STDMETHOD(EnableOutPin) (THIS_
				 int iPin,              /* [in] */  // the OutPin number
				 BOOL bEnable				/* [in] */  // Enable (TRUE, FALSE)
			 ) PURE;

	STDMETHOD(IsOutPinEnabled) (THIS_
				 int iPin,              /* [in] */  // the OutPin number
				 BOOL *bEnable				/* [out] */ // Enable (TRUE, FALSE)
			 ) PURE;

    STDMETHOD(SetDataCheckFunction) (THIS_
				 int iPin,               /* [in] */ // the OutPin number
             DATACHECK_FUNCTION pFnc,/* [in] */ // Pointer to function to check IMediaSample(s)
             long lParam             /* [in] */ // additional Parameters
             ) PURE;
};


#ifdef __cplusplus
}
#endif


#endif // __IINF_TEE__


