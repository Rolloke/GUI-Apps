// iPushSource.h: interface for the CPushSource class
//
//////////////////////////////////////////////////////////////////////
// Classes defined:
//
//   GUIDs of Filter CPushSource, Interface IPushSource and 
//       its property page CPushSourceProp
//   PushSourceParams:
//       The parameters that control the CPushSource
//   IPushSource:
//       The interface that controls the CPushSource.
//////////////////////////////////////////////////////////////////////
#ifndef __IPUSH_SOURCE__
#define __IPUSH_SOURCE__
#ifdef __cplusplus
extern "C" {
#endif

#define PRIVILEGED	2
#define PS_FLAG_SAVE_ONLY_FIRST_TIME	0x00000001

// The parameter control the CPushSource.
struct PushSourceParams 
{
	BOOL  m_bCapture;				// FALSE, TRUE, PRIVILEGED
	BOOL  m_bPreview;				// FALSE, TRUE, PRIVILEGED
	BOOL  m_bStill;					// FALSE, TRUE, PRIVILEGED
	BOOL  m_bInternalRM;			// FALSE, TRUE
	BOOL  m_bLiveSource;			// FALSE, TRUE
	BOOL  m_bGenerateTimeStamps;	// FALSE, TRUE
	BOOL  m_bSortSamples;			// FALSE, TRUE
	LONG  m_nOffsetInMS;			// Offset in Milliseconds
	ULONG m_lMiscFlags;				// 0, AM_FILTER_MISC_FLAGS_IS_SOURCE
	DWORD m_dwFlags;
	BOOL  m_bPreferOwnAllocator;	// FALSE, TRUE
};

#define REGKEY_PS_PUSHSOURCE		_T("SOFTWARE\\DVRT\\PushSource")
#define REGKEY_PS_SAMPLE_TOO_LATE	_T("SampleTooLate")
#define REGKEY_PS_SAMPLE_TOO_EARLY	_T("SampleTooEarly")
#define REGKEY_PS_READ_MODULE_PARAM _T("ReadModuleParam")

// {6dbb875f-21af-4eb5-bdff-06b215fd10c3}
DEFINE_GUID(CLSID_PushSource, 
0x6dbb875f, 0x21af, 0x4eb5, 0xbd, 0xff, 0x6, 0xb2, 0x15, 0xfd, 0x10, 0xc3);

// {255e6090-6faa-4dd8-9ccf-1734613f76f3}
DEFINE_GUID(CLSID_PushSourceProp, 
0x255e6090, 0x6faa, 0x4dd8, 0x9c, 0xcf, 0x17, 0x34, 0x61, 0x3f, 0x76, 0xf3);


#define BUFFER_ATTACH 1
#define BUFFER_FIXED  2
#define BUFFER_COPY   3

typedef HRESULT  (WINAPI*RELEASE_BUFFER_FUNCTION)(void*, long);

struct PushSourceProperties
{
	PushSourceProperties()
	{
		ZeroMemory(this, sizeof(PushSourceProperties));
		nSampleNumber = 0xffffffff;
	};
	PushSourceProperties(PushSourceProperties *pmsp)
	{
		CopyMemory(this, pmsp, sizeof(PushSourceProperties)); 
	};

	BYTE          *pBuffer;				// the Buffer pointer
	long           nLength;				// actual length of the buffers data
	LONGLONG       llTimeStart;			// the beginning media time
	LONGLONG       llTimeEnd;			// the ending media time
	REFERENCE_TIME rtTimeStart;			// sets the stream times when this sample should begin
	REFERENCE_TIME rtTimeEnd;			// and finish
	BOOL           bIsPreroll;			// specifies whether this sample is a preroll sample
	BOOL           bIsDiscontinuity;	// specifies whether this sample represents a break in the data stream
	BOOL           bIsSyncPoint;		// specifies whether the beginning of this sample is a synchronization point
	DWORD          nSampleNumber;		// the number of the sample for sorting
	BYTE          *pReference;			// a reference pointer for delete or release purpose
};


// {f02dfc76-4d81-4649-bc9e-30395a9c0e0b}
DEFINE_GUID(IID_IPushSource, 
0xf02dfc76, 0x4d81, 0x4649, 0xbc, 0x9e, 0x30, 0x39, 0x5a, 0x9c, 0xe, 0xb);

DECLARE_INTERFACE_(IPushSource, IUnknown)
{
	// Get the parameters of the filter
	STDMETHOD(GetParams) (THIS_
				 PushSourceParams *irp
			 ) PURE;

	// Set the parameters of the filter
	STDMETHOD(SetParams) (THIS_
				 PushSourceParams *irp
			 ) PURE;

   STDMETHOD(SetBuffer) (THIS_
                PushSourceProperties *pMSP	// [in] Databuffer
             ) PURE;

   STDMETHOD(SetBufferType) (THIS_
                int nBufferType,					// [in] Buffer Typ
					 RELEASE_BUFFER_FUNCTION pFnc,// [in] Function to free the Attached Buffers
					 long lParam,						// [in] additional Parameter
					 ALLOCATOR_PROPERTIES*pAP		// [in] Allocator Properties
             ) PURE;

   STDMETHOD(SetMediaType) (THIS_
                AM_MEDIA_TYPE *pMT				// [in] MediaType info
             ) PURE;

   STDMETHOD(GetBuffer) (THIS_
                PushSourceProperties *pMSP	// [out] Databuffer
             ) PURE;

   STDMETHOD(GetBufferType) (THIS_
                int &nBufferType,				// [out] Buffer Typ
					 ALLOCATOR_PROPERTIES*pAP		// [out] Allocator Properties
             ) PURE;

   STDMETHOD(GetSourceMediaType) (THIS_
                AM_MEDIA_TYPE **   				// [out] MediaType info
             ) PURE;

   STDMETHOD(SetPropertyPageHWND) (THIS_
                HWND hwnd			  				// [in]  Window Handle of the PropertyPage 
             ) PURE;

   STDMETHOD(SetStreamOffset) (THIS_
                REFERENCE_TIME rt 				// [in]  Stream Offset
             ) PURE;

   STDMETHOD(GetStreamOffset) (THIS_
                REFERENCE_TIME *prt 			// [out]  Stream Offset
             ) PURE;
};

#ifdef __cplusplus
}
#endif

#endif // __IPUSH_SOURCE__
