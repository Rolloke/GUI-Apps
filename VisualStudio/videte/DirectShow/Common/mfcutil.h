//------------------------------------------------------------------------------
// File: MFCUtil.h
//
// Desc: DirectShow sample code - prototypes for utility functions
//       used by MFC applications.
//
// Copyright (c) 2000-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------
#ifndef __MFCUTIL_H_INCLUDED__
#define __MFCUTIL_H_INCLUDED__
//
// Application-defined messages
//
#define WM_GRAPHNOTIFY  WM_APP + 1

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x)  if(x) {x->Release(); x=0;}
#endif

#define JIF(x) if (FAILED(hr=(x))) \
    {RetailOutput(TEXT("FAILED(0x%x) ") TEXT(#x) TEXT("\n"), hr); goto CLEANUP;}

//
//  Function prototypes
//
AFX_EXT_CLASS HRESULT AddGraphFiltersToList (IGraphBuilder *pGB, CWnd*pWnd);
AFX_EXT_CLASS HRESULT AddFilterPinsToLists  (IGraphBuilder *pGB, LPCTSTR pszFilterName, CWnd& m_ListPinsInput, CWnd& m_ListPinsOutput);

AFX_EXT_CLASS HRESULT EnumPinsOnFilter (IBaseFilter *pFilter, PIN_DIRECTION PinDir, CWnd*pWnd);

// Enumeration functions that add extra data to the listbox
AFX_EXT_CLASS HRESULT EnumFiltersAndMonikersToList(IEnumMoniker *pEnumCat, CWnd*pWnd);
AFX_EXT_CLASS HRESULT EnumFiltersAndCLSIDsToList(IEnumMoniker *pEnumCat, CWnd*pWnd);

AFX_EXT_CLASS HRESULT EnumFiltersWithCLSIDToList(ICreateDevEnum *pSysDevEnum, const GUID *clsid, CWnd *pWnd);
AFX_EXT_CLASS HRESULT EnumFiltersWithMonikerToList(ICreateDevEnum *pSysDevEnum, const GUID *clsid, CWnd *pWnd);

// Similar to IGraphBuilder::FindFilterByName()
AFX_EXT_CLASS IBaseFilter *FindFilterFromName(IGraphBuilder *pGB, LPTSTR szNameToFind);

// Listbox add/clear functions that store extra data
AFX_EXT_CLASS HRESULT AddFilterToListWithCLSID  (const TCHAR *szFilterName,const GUID *pCatGuid, CWnd&Wnd);
AFX_EXT_CLASS HRESULT AddFilterToListWithMoniker(const TCHAR *szFilterName,IMoniker *pMoniker, CWnd&Wnd);
AFX_EXT_CLASS void ClearFilterListWithCLSID  (CWnd&Wnd);
AFX_EXT_CLASS void ClearFilterListWithMoniker(CWnd&Wnd);

AFX_EXT_CLASS HRESULT AddVideoStreamConfigCapsToList(IBaseFilter *pFilter, int, CWnd&Wnd);
AFX_EXT_CLASS void ClearVideoStreamConfigCapsList(CWnd&);
AFX_EXT_CLASS HRESULT AddAudioStreamConfigCapsToList(IBaseFilter *pFilter, int, CWnd&Wnd);
AFX_EXT_CLASS void ClearAudioStreamConfigCapsList(CWnd&);

// Listbox manipulation functions
AFX_EXT_CLASS HRESULT AddEventToList(CWnd& ListEvents, long lEventCode);
AFX_EXT_CLASS HRESULT AddFilterCategoriesToList(CWnd&Wnd);

// Debug output
AFX_EXT_CLASS void RetailOutput(TCHAR *tszErr, ...);

AFX_EXT_CLASS BOOL GetFileInfoStrings(LPTSTR szFile, TCHAR *szSize, TCHAR *szDate);

#endif // __MFCUTIL_H_INCLUDED__
