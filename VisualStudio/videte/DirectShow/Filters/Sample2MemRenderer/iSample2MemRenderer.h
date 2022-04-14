// iSample2MemRenderer.h: interface for the CSample2MemRenderer class
//
//////////////////////////////////////////////////////////////////////
// Classes defined:
//
//   GUIDs of Filter CSample2MemRenderer, Interface ISample2MemRenderer and 
//       its property page CSample2MemRendererProp
//   Sample2MemRendererParams:
//       The parameters that control the CSample2MemRenderer
//   ISample2MemRenderer:
//       The interface that controls the CSample2MemRenderer.
//////////////////////////////////////////////////////////////////////
#pragma once

// The parameter control the CSample2MemRenderer.
struct Sample2MemRendererParams 
{
	ULONG m_lMiscFlags;
	BOOL  m_bScheduleSamples;
};

typedef HRESULT  (WINAPI*RECEIVE_FUNCTION)(IMediaSample*, long, long);

// {4198a5d5-30ff-4b69-8ca1-49f3833c712a}
DEFINE_GUID(CLSID_Sample2MemRenderer, 
0x4198a5d5, 0x30ff, 0x4b69, 0x8c, 0xa1, 0x49, 0xf3, 0x83, 0x3c, 0x71, 0x2a);

// {54f63727-d3f9-493b-b2e3-fbd8d8114c8b}
DEFINE_GUID(CLSID_Sample2MemRendererProp, 
0x54f63727, 0xd3f9, 0x493b, 0xb2, 0xe3, 0xfb, 0xd8, 0xd8, 0x11, 0x4c, 0x8b);

// {56bb0892-5296-46dc-9e43-27e93b8deeaf}
DEFINE_GUID(IID_ISample2MemRenderer, 
0x56bb0892, 0x5296, 0x46dc, 0x9e, 0x43, 0x27, 0xe9, 0x3b, 0x8d, 0xee, 0xaf);

#ifdef __cplusplus
extern "C" {
#endif

DECLARE_INTERFACE_(ISample2MemRenderer, IUnknown)
{
	// Get the parameters of the filter
	STDMETHOD(GetParams) (THIS_
				 Sample2MemRendererParams *irp
			 ) PURE;

	// Set the parameters of the filter
	STDMETHOD(SetParams) (THIS_
				 Sample2MemRendererParams *irp
			 ) PURE;

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

