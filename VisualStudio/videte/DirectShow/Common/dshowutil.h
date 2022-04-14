//------------------------------------------------------------------------------
// File: DShowUtil.h
//
// Desc: DirectShow sample code - prototypes for utility functions
//
// Copyright (c) 2000-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------
#ifndef __DSHOWUTIL_H_INCLUDED__
#define __DSHOWUTIL_H_INCLUDED__

#ifdef _DEBUG
#define RELEASE_OBJECT(pObject)															\
{																						\
 	if (pObject) {																		\
		int nR = pObject->Release();													\
		if (nR == 0){																	\
			TRACE(_T("%s(%d):%s::Release(%d)\n"), __FILE__, __LINE__, _T(#pObject), nR);\
		}																				\
		pObject = NULL;																	\
	}																					\
}
#else
 #define RELEASE_OBJECT(pObject) { if (pObject) { pObject->Release();	pObject = NULL; } }																
#endif // _DEBUG
		
#define SAFE_COTASKMEMFREE(pBuff)	\
{									\
	if (pBuff)						\
	{								\
		CoTaskMemFree(pBuff);		\
		pBuff = NULL;				\
	}								\
}

#define SAFE_DELETEMEDIATYPE(pMT)	\
{									\
	if (pMT)						\
	{								\
		DeleteMediaType(pMT);		\
		pMT = NULL;					\
	}								\
}


//
// Function prototypes for DirectShow applications
//
AFX_EXT_CLASS HRESULT GetPin(IBaseFilter * pFilter, PIN_DIRECTION dirrequired, 
               int iNum, IPin **ppPin);
    
AFX_EXT_CLASS HRESULT FindOtherSplitterPin(IPin *pPinIn, GUID guid, 
               int nStream, IPin **ppSplitPin);
    
AFX_EXT_CLASS HRESULT SeekNextFrame(IMediaSeeking * pSeeking, double FPS, long Frame );
    
AFX_EXT_CLASS IPin * GetInPin ( IBaseFilter *pFilter, int Num );
AFX_EXT_CLASS IPin * GetOutPin( IBaseFilter *pFilter, int Num );
AFX_EXT_CLASS HRESULT GetPinCategory(IPin *pPin, GUID *pPinCategory);

AFX_EXT_CLASS HRESULT CountFilterPins(IBaseFilter *pFilter, ULONG *pulInPins, ULONG *pulOutPins);
AFX_EXT_CLASS HRESULT CountTotalFilterPins(IBaseFilter *pFilter, ULONG *pulPins);

// Find a renderer filter in an existing graph
AFX_EXT_CLASS HRESULT FindRenderer(IGraphBuilder *pGB, const GUID *mediatype, IBaseFilter **ppFilter);
AFX_EXT_CLASS HRESULT FindAudioRenderer(IGraphBuilder *pGB, IBaseFilter **ppFilter);
AFX_EXT_CLASS HRESULT FindVideoRenderer(IGraphBuilder *pGB, IBaseFilter **ppFilter);

// Finds a filter's server name
AFX_EXT_CLASS void ShowFilenameByCLSID(REFCLSID clsid, TCHAR *szFilename);

// Media file information
AFX_EXT_CLASS HRESULT GetFileDurationString(IMediaSeeking *pMS, TCHAR *szDuration);

// GUID to Strings and reverse 
AFX_EXT_CLASS GUID MakeGUID(LPCTSTR pszBuff);
AFX_EXT_CLASS CString GetString(GUID guid);

// Property pages and capabilities
AFX_EXT_CLASS BOOL SupportsPropertyPage(IBaseFilter *pFilter);
AFX_EXT_CLASS HRESULT ShowFilterPropertyPage(IBaseFilter *pFilter, HWND hwndParent);
AFX_EXT_CLASS BOOL CanFrameStep(IGraphBuilder *pGB);

// Debug helper functions
AFX_EXT_CLASS void TurnOnDebugDllDebugging( );
AFX_EXT_CLASS void DbgPrint( char * pText );
AFX_EXT_CLASS void ErrPrint( char * pText );
AFX_EXT_CLASS void GetErrorStrings(HRESULT, CString&sError, CString&sDescription);


// Adds/removes a DirectShow filter graph from the Running Object Table,
// allowing GraphEdit to "spy" on a remote filter graph if enabled.
AFX_EXT_CLASS HRESULT AddGraphToRot(IUnknown *pUnkGraph, DWORD *pdwRegister);
AFX_EXT_CLASS void RemoveGraphFromRot(DWORD pdwRegister);

// Operation of Filters with Graph
#define AFTG_MATCH_CASE      0x00000001
#define AFTG_MATCH_EXACT     0x00000002
#define AFTG_MATCH_PARTIAL   0x00000004
#define AFTG_MATCH_IN_SEARCH 0x00000008
// helpers
AFX_EXT_CLASS HRESULT EnumFilterCategory(REFCLSID guidCategory, BOOL bAddGuidString, CStringArray &sa);
AFX_EXT_CLASS HRESULT FindMonikerByName(const GUID&, CString&, DWORD, IMoniker**);

AFX_EXT_CLASS HRESULT ConnectFilters(IGraphBuilder*, IBaseFilter*, int, IBaseFilter*, int, AM_MEDIA_TYPE *pMT=NULL);
AFX_EXT_CLASS HRESULT AddFilterToGraph(IGraphBuilder*, IBaseFilter **, const GUID&, LPCWSTR);
AFX_EXT_CLASS HRESULT AddFilterToGraph(IGraphBuilder*, IBaseFilter **, LPCTSTR, LPCWSTR);
AFX_EXT_CLASS HRESULT AddFilterToGraph(IGraphBuilder*, IBaseFilter **, IMoniker*, LPCWSTR);
AFX_EXT_CLASS HRESULT AddFilterToGraph(IGraphBuilder*, IBaseFilter **, const GUID&, CString&, DWORD, LPCWSTR);


#endif // __DSHOWUTIL_H_INCLUDED__
