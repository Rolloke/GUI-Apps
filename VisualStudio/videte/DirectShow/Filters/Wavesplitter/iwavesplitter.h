//------------------------------------------------------------------------------
// File: IWaveSplitter.h
//
// Desc: DirectShow sample code - custom interface to splitt Stereo Wave Samples
//			into Mono Wave Samples.
//       Set a threshold value to reduce the number of wave samples.
//
// Copyright (c) 1992-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------


#ifndef __IWAVESPLITTER__
#define __IWAVESPLITTER__

#define CLSID_WAVESPLITTER "DA49101B-E655-442f-BD38-D9A181DDC688"
#define GUID_IWAVESPLITTER "2E1100A0-451B-4c10-BA23-716FA7E91600"

DEFINE_GUID(CLSID_WaveSplitter, 
0xDA49101B, 0xE655, 0x442F, 0xBD, 0x38, 0xD9, 0xA1, 0x81, 0xDD, 0xC6, 0x88);


#define WAVESPLIT_IF_ACTIVE 0
#define WAVESPLIT_NEVER     1
#define WAVESPLIT_ALWAYS    2

#define EC_ACTIVATE_TRANSPORT (EC_USER + 1)

#ifdef __cplusplus
extern "C" {
#endif

typedef HRESULT  (WINAPI*MEDIASAMPLE_FUNCTION)(IMediaSample*,	// the MediaSample
															  AM_MEDIA_TYPE*, // the MediaType
															  long,				// Pin Number
															  long);				// additional Parameter

//
// IWaveSplitter's GUID
//
DEFINE_GUID(IID_IWaveSplitter, 
0x2e1100a0, 0x451b, 0x4c10, 0xba, 0x23, 0x71, 0x6f, 0xa7, 0xe9, 0x16, 0x0);


//
// IWaveSplitter: interface of CWaveInputPin
// retrieve this through the input pin
DECLARE_INTERFACE_(IWaveSplitter, IUnknown) {

	STDMETHOD(SetThreshold) (THIS_
				 float fValue              // [in] Threshold value in percent
			 ) PURE;

	STDMETHOD(SetMediaSampleCheckFnc) (THIS_
			 MEDIASAMPLE_FUNCTION pFnc,   // [in] The Check MediaSample Function
			 long                 lParam  // [in] additional Parameter
		 ) PURE;

	STDMETHOD(SetPinNumber) (THIS_
				 int nPin,						// [in] Index of the Pin
				 int nNumber					// [in] Number of the Pin (normally identical to it´s index
			 ) PURE;
	STDMETHOD(SetDeliver) (THIS_
				int nDeliver               // [in] Delivery Status
			 ) PURE;
};


#ifdef __cplusplus
}
#endif


#endif // __IWAVESPLITTER__


