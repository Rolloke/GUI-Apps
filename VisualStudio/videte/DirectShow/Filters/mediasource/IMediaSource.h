//------------------------------------------------------------------------------
// File: IMediaSource.h
//
// Desc: DirectShow sample code - Filter to deliver Mediasamples from external
//       Buffers
//
// Copyright (c) 1992-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------


#ifndef __IMEDIA_SOURCE__
#define __IMEDIA_SOURCE__

#ifdef __cplusplus
extern "C" {
#endif

// "1A410C37-0D4E-4694-88F2-FA44E6CF03F5"
DEFINE_GUID(CLSID_MEDIA_SOURCE_FILTER, 
0x1A410C37, 0x0D4E, 0x4694, 0x88, 0xF2, 0xFA, 0x44, 0xE6, 0xCF, 0x03, 0xF5);

// "78F10056-BB75-4ae9-99E8-1CF363065BBE"
DEFINE_GUID(CLSID_MEDIA_SOURCE_PROPERTY, 
0x78F10056, 0xBB75, 0x4ae9, 0x99, 0xE8, 0x1C, 0xF3, 0x63, 0x06, 0x5B, 0xBE);

#define BUFFER_ATTACH 1
#define BUFFER_FIXED  2
#define BUFFER_COPY   3


typedef HRESULT  (WINAPI*RELEASE_BUFFER_FUNCTION)(void*, long);

struct MediaSampleProperties
{
	MediaSampleProperties()
	{
		ZeroMemory(this, sizeof(MediaSampleProperties));
	};
	MediaSampleProperties(MediaSampleProperties *pmsp)
	{
		CopyMemory(this, pmsp, sizeof(MediaSampleProperties));
	};

	BYTE          *pBuffer;				// the Buffer pointer
	long           nLength;				// actual length of the buffers data
	LONGLONG       llTimeStart;		// the beginning media time
	LONGLONG       llTimeEnd;			// the ending media time
	REFERENCE_TIME rtTimeStart;		// sets the stream times when this sample should begin
	REFERENCE_TIME rtTimeEnd;			// and finish
	BOOL           bIsPreroll;			// specifies whether this sample is a preroll sample
	BOOL           bIsDiscontinuity;	// specifies whether this sample represents a break in the data stream
	BOOL           bIsSyncPoint;		// specifies whether the beginning of this sample is a synchronization point
};

//
// IMediaSource's GUID
//
// {3731938F-429F-4d2a-A7C9-60FDDFAEB2E3}
DEFINE_GUID(IID_IMediaSource, 
0x3731938f, 0x429f, 0x4d2a, 0xa7, 0xc9, 0x60, 0xfd, 0xdf, 0xae, 0xb2, 0xe3);

enum SYNTH_OUTPUT_FORMAT
{
    SYNTH_OF_PCM,
    SYNTH_OF_MS_ADPCM
};

//
// IMediaSource
//
DECLARE_INTERFACE_(IMediaSource, IUnknown) 
{

   STDMETHOD(SetBuffer) (THIS_
                MediaSampleProperties *pMSP/* [in] */	// Databuffer
             ) PURE;

   STDMETHOD(SetBufferType) (THIS_
                int nBufferType,				/* [in] */	// Buffer Typ
					 RELEASE_BUFFER_FUNCTION pFnc,/*[in]*/	// Function to free the Attached Buffers
					 long lParam,				   /* [i] */   // additional Parameter
					 ALLOCATOR_PROPERTIES*pAP	/* [in] */	// Allocator Properties
             ) PURE;

   STDMETHOD(SetMediaType) (THIS_
                AM_MEDIA_TYPE *pMT		  /* [in] */    // MediaType info
             ) PURE;

    STDMETHOD(get_Frequency) (THIS_
                int *Frequency          /* [out] */    // the current frequency
             ) PURE;

    STDMETHOD(put_Frequency) (THIS_
                int    Frequency        /* [in] */    // Change to this frequency
             ) PURE;

    STDMETHOD(get_Waveform) (THIS_
                int *Waveform           /* [out] */    // the current Waveform
             ) PURE;

    STDMETHOD(put_Waveform) (THIS_
                int    Waveform         /* [in] */    // Change to this Waveform
             ) PURE;

    STDMETHOD(get_Channels) (THIS_
                int *Channels           /* [out] */    // the current Channels
             ) PURE;

    STDMETHOD(put_Channels) (THIS_
                int    Channels         /* [in] */    // Change to this Channels
             ) PURE;

    STDMETHOD(get_BitsPerSample) (THIS_
                int *BitsPerSample      /* [out] */    // the current BitsPerSample
             ) PURE;

    STDMETHOD(put_BitsPerSample) (THIS_
                int    BitsPerSample    /* [in] */    // Change to this BitsPerSample
             ) PURE;

    STDMETHOD(get_SamplesPerSec) (THIS_
                 int *SamplesPerSec     /* [out] */    // the current SamplesPerSec
             ) PURE;

    STDMETHOD(put_SamplesPerSec) (THIS_
                  int    SamplesPerSec  /* [in] */    // Change to this SamplesPerSec
             ) PURE;

    STDMETHOD(get_Amplitude) (THIS_
                  int *Amplitude        /* [out] */    // the current Amplitude
             ) PURE;

    STDMETHOD(put_Amplitude) (THIS_
                  int    Amplitude      /* [in] */    // Change to this Amplitude
              ) PURE;

    STDMETHOD(get_SweepRange) (THIS_
                  int *SweepStart,      /* [out] */
                  int *SweepEnd         /* [out] */
             ) PURE;

    STDMETHOD(put_SweepRange) (THIS_
                  int    SweepStart,    /* [in] */
                  int    SweepEnd       /* [in] */
             ) PURE;

    STDMETHOD(get_OutputFormat) (THIS_
                  SYNTH_OUTPUT_FORMAT *pOutputFormat /* [out] */
             ) PURE;

    STDMETHOD(put_OutputFormat) (THIS_
                  SYNTH_OUTPUT_FORMAT ofNewOutputFormat /* [out] */
             ) PURE;
};


#ifdef __cplusplus
}
#endif

#endif // __IMEDIA_SOURCE__


