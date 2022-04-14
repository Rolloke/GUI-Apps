//------------------------------------------------------------------------------
// File: DShowUtil.cpp
//
// Desc: DirectShow sample code - utility functions.
//
// Copyright (c) 2000-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

#include "stdafx.h"
#include <atlbase.h>
#include <dshow.h>
#include <mtype.h>
#include <wxdebug.h>
#include <reftime.h>
#include <Dxerr8.h>

#include "dshowutil.h"


GUID MakeGUID(LPCTSTR pszBuff)
{
	unsigned long  lData1;
	unsigned long  sData2, sData3;
	unsigned long  cData4_0, cData4_1, cData4_2, cData4_3, cData4_4, cData4_5, cData4_6, cData4_7;
	_TCHAR *psz = _tcsstr(pszBuff, _T("{"));
	GUID guid;
	ZeroMemory(&guid, sizeof(GUID));
	if (psz) pszBuff = &psz[1];

	int nCount = _stscanf(pszBuff, _T("%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x"),
			&lData1, &sData2, &sData3,
			&cData4_0, &cData4_1, &cData4_2, &cData4_3,
			&cData4_4, &cData4_5, &cData4_6, &cData4_7);

	if (nCount == 11)
	{
		guid.Data1    = lData1;
		guid.Data2    = (unsigned short)sData2;
		guid.Data3    = (unsigned short)sData3;
		guid.Data4[0] = (unsigned char)cData4_0;
		guid.Data4[1] = (unsigned char)cData4_1;
		guid.Data4[2] = (unsigned char)cData4_2;
		guid.Data4[3] = (unsigned char)cData4_3;
		guid.Data4[4] = (unsigned char)cData4_4;
		guid.Data4[5] = (unsigned char)cData4_5;
		guid.Data4[6] = (unsigned char)cData4_6;
		guid.Data4[7] = (unsigned char)cData4_7;
		return guid;
	}
	return guid;
}

CString GetString(GUID guid)
{
	CString str;
	str.Format(_T("{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}"),
		guid.Data1,	guid.Data2,	guid.Data3,
		guid.Data4[0],	guid.Data4[1],	guid.Data4[2],	guid.Data4[3],
		guid.Data4[4],	guid.Data4[5],	guid.Data4[6],	guid.Data4[7]);
	return str;
}

HRESULT FindRenderer(IGraphBuilder *pGB, const GUID *mediatype, IBaseFilter **ppFilter)
{
    HRESULT hr;
    IEnumFilters *pEnum = NULL;
    IBaseFilter *pFilter = NULL;
    IPin *pPin;
    ULONG ulFetched, ulInPins, ulOutPins;
    BOOL bFound=FALSE;

    // Verify graph builder interface
    if (!pGB)
        return E_NOINTERFACE;

    // Verify that a media type was passed
    if (!mediatype)
        return E_POINTER;

    // Clear the filter pointer in case there is no match
    if (ppFilter)
        *ppFilter = NULL;

    // Get filter enumerator
    hr = pGB->EnumFilters(&pEnum);
    if (FAILED(hr))
        return hr;

    pEnum->Reset();

    // Enumerate all filters in the graph
    while(!bFound && (pEnum->Next(1, &pFilter, &ulFetched) == S_OK))
    {
#ifdef DEBUG
        // Read filter name for debugging purposes
        FILTER_INFO FilterInfo;
        TCHAR szName[256];
    
        hr = pFilter->QueryFilterInfo(&FilterInfo);
        if (SUCCEEDED(hr))
        {
            // Show filter name in debugger
#ifdef UNICODE
            lstrcpy(szName, FilterInfo.achName);
#else
            WideCharToMultiByte(CP_ACP, 0, FilterInfo.achName, -1, szName, 256, 0, 0);
#endif
            FilterInfo.pGraph->Release();
        }       
#endif

        // Find a filter with one input and no output pins
        hr = CountFilterPins(pFilter, &ulInPins, &ulOutPins);
        if (FAILED(hr))
            break;

        if ((ulInPins == 1) && (ulOutPins == 0))
        {
            // Get the first pin on the filter
            pPin=0;
            pPin = GetInPin(pFilter, 0);

            // Read this pin's major media type
            AM_MEDIA_TYPE type={0};
            hr = pPin->ConnectionMediaType(&type);
            if (FAILED(hr))
                break;

            // Is this pin's media type the requested type?
            // If so, then this is the renderer for which we are searching.
            // Copy the interface pointer and return.
            if (type.majortype == *mediatype)
            {
                // Found our filter
                *ppFilter = pFilter;
                bFound = TRUE;;
            }
            // This is not the renderer, so release the interface.
            else
                pFilter->Release();

            // Delete memory allocated by ConnectionMediaType()
            FreeMediaType(type);
        }
        else
        {
            // No match, so release the interface
            pFilter->Release();
        }
    }

    pEnum->Release();
    return hr;
}

HRESULT FindAudioRenderer(IGraphBuilder *pGB, IBaseFilter **ppFilter)
{
    return FindRenderer(pGB, &MEDIATYPE_Audio, ppFilter);
}

HRESULT FindVideoRenderer(IGraphBuilder *pGB, IBaseFilter **ppFilter)
{
    return FindRenderer(pGB, &MEDIATYPE_Video, ppFilter);
}

HRESULT CountFilterPins(IBaseFilter *pFilter, ULONG *pulInPins, ULONG *pulOutPins)
{
    HRESULT hr=S_OK;
    IEnumPins *pEnum=0;
    ULONG ulFound;
    IPin *pPin;

    // Verify input
    if (!pFilter || !pulInPins || !pulOutPins)
        return E_POINTER;

    // Clear number of pins found
    *pulInPins = 0;
    *pulOutPins = 0;

    // Get pin enumerator
    hr = pFilter->EnumPins(&pEnum);
    if(FAILED(hr)) 
        return hr;

    pEnum->Reset();

    // Count every pin on the filter
    while(S_OK == pEnum->Next(1, &pPin, &ulFound))
    {
        PIN_DIRECTION pindir = (PIN_DIRECTION)3;

        hr = pPin->QueryDirection(&pindir);

        if(pindir == PINDIR_INPUT)
            (*pulInPins)++;
        else
            (*pulOutPins)++;

        pPin->Release();
    } 

    pEnum->Release();
    return hr;
}


HRESULT CountTotalFilterPins(IBaseFilter *pFilter, ULONG *pulPins)
{
    HRESULT hr;
    IEnumPins *pEnum=0;
    ULONG ulFound;
    IPin *pPin;

    // Verify input
    if (!pFilter || !pulPins)
        return E_POINTER;

    // Clear number of pins found
    *pulPins = 0;

    // Get pin enumerator
    hr = pFilter->EnumPins(&pEnum);
    if(FAILED(hr)) 
        return hr;

    // Count every pin on the filter, ignoring direction
    while(S_OK == pEnum->Next(1, &pPin, &ulFound))
    {
        (*pulPins)++;
        pPin->Release();
    } 

    pEnum->Release();
    return hr;
}


HRESULT GetPin( IBaseFilter * pFilter, PIN_DIRECTION dirrequired, int iNum, IPin **ppPin)
{
    CComPtr< IEnumPins > pEnum;
    *ppPin = NULL;
    HRESULT hr = pFilter->EnumPins(&pEnum);
    if(FAILED(hr)) 
        return hr;

    ULONG ulFound;
    IPin *pPin;
    hr = E_FAIL;

    while(S_OK == pEnum->Next(1, &pPin, &ulFound))
    {
        PIN_DIRECTION pindir = (PIN_DIRECTION)3;
        pPin->QueryDirection(&pindir);
        if(pindir == dirrequired)
        {
            if(iNum == 0)
            {
                *ppPin = pPin;
                // Found requested pin, so clear error
                hr = S_OK;
                break;
            }
            iNum--;
        } 

        pPin->Release();
    } 

    return hr;
}


IPin * GetInPin( IBaseFilter * pFilter, int Num )
{
    CComPtr< IPin > pComPin;
    GetPin(pFilter, PINDIR_INPUT, Num, &pComPin);
    return pComPin;
}


IPin * GetOutPin( IBaseFilter * pFilter, int Num )
{
    CComPtr< IPin > pComPin;
    GetPin(pFilter, PINDIR_OUTPUT, Num, &pComPin);
    return pComPin;
}

HRESULT GetPinCategory(IPin *pPin, GUID *pPinCategory)
{
    HRESULT hr;
    IKsPropertySet *pKs;
    hr = pPin->QueryInterface(IID_IKsPropertySet, (void **)&pKs);
	if (SUCCEEDED(hr))
    {
		DWORD cbReturned;
		hr = pKs->Get(AMPROPSETID_Pin, AMPROPERTY_PIN_CATEGORY, NULL, 0, 
			pPinCategory, sizeof(GUID), &cbReturned);
		pKs->Release();
	}

    return hr;
}

HRESULT FindOtherSplitterPin(IPin *pPinIn, GUID guid, int nStream, IPin **ppSplitPin)
{
	if (pPinIn == NULL)
		return E_POINTER;

    if (!ppSplitPin)
        return E_POINTER;

    CComPtr< IPin > pPinOut;
    pPinOut = pPinIn;

    while(pPinOut)
    {
        PIN_INFO ThisPinInfo;
        pPinOut->QueryPinInfo(&ThisPinInfo);
        if(ThisPinInfo.pFilter) ThisPinInfo.pFilter->Release();

        pPinOut = NULL;
        CComPtr< IEnumPins > pEnumPins;
        ThisPinInfo.pFilter->EnumPins(&pEnumPins);
        if(!pEnumPins)
        {
            return NULL;
        }

        // look at every pin on the current filter...
        //
        ULONG Fetched = 0;
        while(1)
        {
            CComPtr< IPin > pPin;
            Fetched = 0;
            ASSERT(!pPin); // is it out of scope?
            pEnumPins->Next(1, &pPin, &Fetched);
            if(!Fetched)
            {
                break;
            }

            PIN_INFO pi;
            pPin->QueryPinInfo(&pi);
            if(pi.pFilter) pi.pFilter->Release();

            // if it's an input pin...
            //
            if(pi.dir == PINDIR_INPUT)
            {
                // continue searching upstream from this pin
                //
                pPin->ConnectedTo(&pPinOut);

                // a pin that supports the required media type is the
                // splitter pin we are looking for!  We are done
                //
            }
            else
            {
                CComPtr< IEnumMediaTypes > pMediaEnum;
                pPin->EnumMediaTypes(&pMediaEnum);
                if(pMediaEnum)
                {
                    Fetched = 0;
                    AM_MEDIA_TYPE *pMediaType;
                    pMediaEnum->Next(1, &pMediaType, &Fetched);
                    if(Fetched)
                    {
                        if(pMediaType->majortype == guid)
                        {
                            if(nStream-- == 0)
                            {
                                DeleteMediaType(pMediaType);
                                *ppSplitPin = pPin;
                                (*ppSplitPin)->AddRef();
                                return S_OK;
                            }
                        }
                        DeleteMediaType(pMediaType);
                    }
                }
            }

            // go try the next pin

        } // while
    }
    ASSERT(FALSE);
    return E_FAIL;
}


HRESULT SeekNextFrame( IMediaSeeking * pSeeking, double FPS, long Frame )
{
    // try seeking by frames first
    //
    HRESULT hr = pSeeking->SetTimeFormat(&TIME_FORMAT_FRAME);
    REFERENCE_TIME Pos = 0;
    if(!FAILED(hr))
    {
        pSeeking->GetCurrentPosition(&Pos);
        Pos++;
    }
    else
    {
        // couldn't seek by frames, use Frame and FPS to calculate time
        //
        Pos = REFERENCE_TIME(double( Frame * UNITS ) / FPS);

        // add a half-frame to seek to middle of the frame
        //
        Pos += REFERENCE_TIME(double( UNITS ) * 0.5 / FPS);
    }

    hr = pSeeking->SetPositions(&Pos, AM_SEEKING_AbsolutePositioning, 
                                NULL, AM_SEEKING_NoPositioning);
    return hr;

}

#ifdef DEBUG
    // for debugging purposes
    const INT iMAXLEVELS = 5;                // Maximum debug categories
    extern DWORD m_Levels[iMAXLEVELS];       // Debug level per category
#endif


void TurnOnDebugDllDebugging( )
{
#ifdef DEBUG
    for(int i = 0 ; i < iMAXLEVELS ; i++)
    {
        m_Levels[i] = 1;
    }
#endif
}

void DbgPrint( char * pText )
{
    DbgLog(( LOG_TRACE, 1, "%s", pText ));
}

void ErrPrint( char * pText )
{
    printf(pText);
    return;
}


// Adds a DirectShow filter graph to the Running Object Table,
// allowing GraphEdit to "spy" on a remote filter graph.
HRESULT AddGraphToRot(IUnknown *pUnkGraph, DWORD *pdwRegister) 
{
    IMoniker * pMoniker;
    IRunningObjectTable *pROT;
    WCHAR wsz[128];
    HRESULT hr;

    if (FAILED(GetRunningObjectTable(0, &pROT)))
        return E_FAIL;

    wsprintfW(wsz, L"FilterGraph %08x pid %08x", (DWORD_PTR)pUnkGraph, 
              GetCurrentProcessId());

    hr = CreateItemMoniker(L"!", wsz, &pMoniker);
    if (SUCCEEDED(hr)) 
    {
        hr = pROT->Register(0, pUnkGraph, pMoniker, pdwRegister);
        pMoniker->Release();
    }
    pROT->Release();
    return hr;
}

// Removes a filter graph from the Running Object Table
void RemoveGraphFromRot(DWORD pdwRegister)
{
    IRunningObjectTable *pROT;

    if (SUCCEEDED(GetRunningObjectTable(0, &pROT))) 
    {
        pROT->Revoke(pdwRegister);
        pROT->Release();
    }
}


void ShowFilenameByCLSID(REFCLSID clsid, TCHAR *szFilename)
{
    HRESULT hr;
    LPOLESTR strCLSID;

    // Convert binary CLSID to a readable version
    hr = StringFromCLSID(clsid, &strCLSID);
    if(SUCCEEDED(hr))
    {
        TCHAR szKey[512];
        CString strQuery(strCLSID);

        // Create key name for reading filename registry
        wsprintf(szKey, TEXT("Software\\Classes\\CLSID\\%s\\InprocServer32\0"),
                 strQuery);

        // Free memory associated with strCLSID (allocated in StringFromCLSID)
        CoTaskMemFree(strCLSID);

        HKEY hkeyFilter=0;
        DWORD dwSize=MAX_PATH;
        BYTE szFile[MAX_PATH];
        int rc=0;

        // Open the CLSID key that contains information about the filter
        rc = RegOpenKey(HKEY_LOCAL_MACHINE, szKey, &hkeyFilter);
        if (rc == ERROR_SUCCESS)
        {
            rc = RegQueryValueEx(hkeyFilter, NULL,  // Read (Default) value
                                 NULL, NULL, szFile, &dwSize);

            if (rc == ERROR_SUCCESS)
                wsprintf(szFilename, TEXT("%s"), szFile);
            else
                wsprintf(szFilename, TEXT("<Unknown>\0"));

            rc = RegCloseKey(hkeyFilter);
        }
    }
}


HRESULT GetFileDurationString(IMediaSeeking *pMS, TCHAR *szDuration)
{
    HRESULT hr;

    if (!pMS)
        return E_NOINTERFACE;
    if (!szDuration)
        return E_POINTER;

    // Initialize the display in case we can't read the duration
    wsprintf(szDuration, TEXT("<00:00.000>"));

    // Is media time supported for this file?
    if (S_OK != pMS->IsFormatSupported(&TIME_FORMAT_MEDIA_TIME))
        return E_NOINTERFACE;

    // Read the time format to restore later
    GUID guidOriginalFormat;
    hr = pMS->GetTimeFormat(&guidOriginalFormat);
    if (FAILED(hr))
        return hr;

    // Ensure media time format for easy display
    hr = pMS->SetTimeFormat(&TIME_FORMAT_MEDIA_TIME);
    if (FAILED(hr))
        return hr;

    // Read the file's duration
    LONGLONG llDuration;
    hr = pMS->GetDuration(&llDuration);
    if (FAILED(hr))
        return hr;

    // Return to the original format
    if (guidOriginalFormat != TIME_FORMAT_MEDIA_TIME)
    {
        hr = pMS->SetTimeFormat(&guidOriginalFormat);
        if (FAILED(hr))
            return hr;
    }

    // Convert the LONGLONG duration into human-readable format
    unsigned long nTotalMS = (unsigned long) llDuration / 10000; // 100ns -> ms
    int nMS = nTotalMS % 1000;
    int nSeconds = nTotalMS / 1000;
    int nMinutes = nSeconds / 60;
    nSeconds %= 60;

    // Update the string
    wsprintf(szDuration, _T("%02dm:%02d.%03ds\0"), nMinutes, nSeconds, nMS);

    return hr;
}


BOOL SupportsPropertyPage(IBaseFilter *pFilter) 
{
    HRESULT hr;
    ISpecifyPropertyPages *pSpecify;

    // Discover if this filter contains a property page
    hr = pFilter->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pSpecify);
    if (SUCCEEDED(hr)) 
    {
        pSpecify->Release();
        return TRUE;
    }
    else
        return FALSE;
}


HRESULT ShowFilterPropertyPage(IBaseFilter *pFilter, HWND hwndParent)
{
    HRESULT hr;
    ISpecifyPropertyPages *pSpecify=0;

    if (!pFilter)
        return E_NOINTERFACE;

    // Discover if this filter contains a property page
    hr = pFilter->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pSpecify);
    if (SUCCEEDED(hr)) 
    {
        do 
        {
            FILTER_INFO FilterInfo;
            hr = pFilter->QueryFilterInfo(&FilterInfo);
            if (FAILED(hr))
                break;

            CAUUID caGUID;
            hr = pSpecify->GetPages(&caGUID);
            if (FAILED(hr))
                break;

            pSpecify->Release();
        
            // Display the filter's property page
            OleCreatePropertyFrame(
                hwndParent,             // Parent window
                0,                      // x (Reserved)
                0,                      // y (Reserved)
                FilterInfo.achName,     // Caption for the dialog box
                1,                      // Number of filters
                (IUnknown **)&pFilter,  // Pointer to the filter 
                caGUID.cElems,          // Number of property pages
                caGUID.pElems,          // Pointer to property page CLSIDs
                0,                      // Locale identifier
                0,                      // Reserved
                NULL                    // Reserved
            );
            CoTaskMemFree(caGUID.pElems);
            FilterInfo.pGraph->Release(); 

        } while(0);
    }

//    pFilter->Release();
    return hr;
}


//
// Some hardware decoders and video renderers support stepping media
// frame by frame with the IVideoFrameStep interface.  See the interface
// documentation for more details on frame stepping.
//
BOOL CanFrameStep(IGraphBuilder *pGB)
{
    HRESULT hr;
    IVideoFrameStep* pFS;

    hr = pGB->QueryInterface(__uuidof(IVideoFrameStep), (PVOID *)&pFS);
    if (FAILED(hr))
        return FALSE;

    // Check if this decoder can step
    hr = pFS->CanStep(0L, NULL); 

    pFS->Release();

    if (hr == S_OK)
        return TRUE;
    else
        return FALSE;
}

HRESULT ConnectFilters(IGraphBuilder* pGB, IBaseFilter *pF1, int nPinF1, IBaseFilter *pF2, int nPinF2, AM_MEDIA_TYPE *pMT)
{
	HRESULT hr;
	IPin *pPinIn  = NULL, 
		  *pPinOut = NULL;

	if (pGB == NULL) return E_POINTER;

	hr = GetPin(pF1, PINDIR_OUTPUT, nPinF1, &pPinOut);
	if (SUCCEEDED(hr))
	{
		hr = GetPin(pF2, PINDIR_INPUT , nPinF2, &pPinIn);
		if (SUCCEEDED(hr))
		{
			if (pPinOut && pPinIn)
			{
				if (pMT)
				{
					hr = pPinOut->Connect(pPinIn, pMT);
				}
				else
				{
					hr = pGB->Connect(pPinOut, pPinIn);						// Connect the splitter device to the encoder device
				}
			}
		}
	}
	RELEASE_OBJECT(pPinOut);
	RELEASE_OBJECT(pPinIn);
	return hr;
}


HRESULT AddFilterToGraph(IGraphBuilder* pGB, IBaseFilter **ppFilter, LPCTSTR strGUID, LPCWSTR strFilterName)
{
	return AddFilterToGraph(pGB, ppFilter, MakeGUID(strGUID), strFilterName);
}

HRESULT AddFilterToGraph(IGraphBuilder* pGB, IBaseFilter **ppFilter, const GUID& guid, LPCWSTR strFilterName)
{
	if (!pGB)      return E_POINTER;
	if (!ppFilter) return E_POINTER;
	ASSERT(*ppFilter == NULL);

	HRESULT hr = CoCreateInstance(guid, NULL, CLSCTX_INPROC, IID_IBaseFilter, (void **)ppFilter);
	if (!*ppFilter) return (hr);
	hr = pGB->AddFilter(*ppFilter, strFilterName);
	return hr;
}

/*********************************************************************************************
 Class      : 
 Function   : AddFilterToGraph
 Description: Adds a filter of a specific category to the graph by name

 Parameters:   
  pGB          : (E): The graph builder inteface  (<LINK IGraphBuilder, IGraphBuilder*>)
  ppFilter     : (A): The inserted filter  (<LINK IBaseFilter, IBaseFilter**>)
  guidCategory : (E): The filter category to search in  (GUID)
  sSearchName  : (E): The name of the filter to be found  (CString)
  dwFlags      : (E): Search options  (DWORD)
  - AFTG_MATCH_CASE
  - AFTG_MATCH_EXACT
  - AFTG_MATCH_PARTIAL
  strFilterName: (E): The name of the filter inserted to the graph  (LPCWSTR)

 Result type: Error value, if failed to find the filter (HRESULT)
 created: September, 17 2003
 <TITLE AddFilterToGraph>
*********************************************************************************************/
HRESULT FindMonikerByName(const GUID&guidCategory, CString&sSearchName, DWORD dwFlags, IMoniker**ppMoniker)
{
	if (!ppMoniker) return E_POINTER;
	ASSERT(*ppMoniker == NULL);
	HRESULT hr=S_OK;

	ICreateDevEnum *pSysDevEnum = NULL;
	IEnumMoniker *pEnumCat = NULL;

	IMoniker *pMoniker = NULL;
	ULONG cFetched;
	VARIANT varName={0};
	BOOL bMatch = FALSE;
	int nPartialLen = 0;

	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC, IID_ICreateDevEnum, (void **)&pSysDevEnum);
	if FAILED(hr) return hr;

	// Enumerate all filters of the selected category  
	hr = pSysDevEnum->CreateClassEnumerator(guidCategory, &pEnumCat, 0);
	if (hr == S_OK)
	{	// Enumerate all items associated with the moniker
		if (!(dwFlags & AFTG_MATCH_CASE))
		{
			sSearchName.MakeUpper();
		}
		while(pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK)
		{
			IPropertyBag *pPropBag;
			ASSERT(pMoniker);

			// Associate moniker with a file
			hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pPropBag);
			if (FAILED(hr)) continue;

			// Read filter name from property bag
			varName.vt = VT_BSTR;
			hr = pPropBag->Read(L"FriendlyName", &varName, 0);
			if (FAILED(hr))
			{
				RELEASE_OBJECT(pMoniker);
				continue;
			}

			CString sName(varName.bstrVal);
			SysFreeString(varName.bstrVal);

			if (!(dwFlags & AFTG_MATCH_CASE))
			{
				sName.MakeUpper();
			}
			if (dwFlags & AFTG_MATCH_EXACT)
			{
				bMatch = (sName == sSearchName);
			}
			else if (dwFlags & AFTG_MATCH_IN_SEARCH)
			{
				bMatch = (sSearchName.Find(sName, 0) != -1);
			}
			else
			{
				bMatch = (sName.Find(sSearchName, 0) != -1);
				if (!bMatch && dwFlags & AFTG_MATCH_PARTIAL)
				{
					int nLen = sSearchName.GetLength()-1;
					while (!bMatch && nLen >= nPartialLen)
					{
						bMatch = (sName.Find(sSearchName.Left(nLen), 0) != -1);
						if (bMatch)
						{
							nPartialLen = nLen;
							break;
						}
						else
						{
							nLen--;
						}
					}
				}
			}

			RELEASE_OBJECT(pPropBag);
			
			if (bMatch) break;
			RELEASE_OBJECT(pMoniker);
		}

		RELEASE_OBJECT(pEnumCat);
	}

	pSysDevEnum->Release();
	if (pMoniker)
	{
		hr = nPartialLen;
		*ppMoniker = pMoniker;
	}
	return hr;
}

HRESULT AddFilterToGraph(IGraphBuilder* pGB, IBaseFilter **ppFilter, const GUID& guidCategory, CString &sSearchName, DWORD dwFlags, LPCWSTR strFilterName)
{
	if (!pGB)      return E_POINTER;
	if (!ppFilter) return E_POINTER;
	ASSERT(*ppFilter == NULL);
	HRESULT hr=S_OK;

	IMoniker *pMoniker = NULL;
	hr = FindMonikerByName(guidCategory, sSearchName, dwFlags, &pMoniker);

	if (pMoniker)
	{
		HRESULT hrAdd = AddFilterToGraph(pGB, ppFilter, pMoniker, strFilterName);
 		RELEASE_OBJECT(pMoniker);
		if (SUCCEEDED(hrAdd))
		{
			return hr;
		}
		else
		{
			return hrAdd;
		}
	}
	return VFW_E_NOT_FOUND;
}

HRESULT AddFilterToGraph(IGraphBuilder *pGB, IBaseFilter **ppFilter, IMoniker *pMoniker, LPCWSTR strFilterName)
{
	if (!pGB)      return E_POINTER;
	if (!pMoniker) return E_POINTER;
	if (!ppFilter) return E_POINTER;
	ASSERT(*ppFilter == NULL);
	VARIANT var;
	VariantInit(&var);
	HRESULT hr = pMoniker->BindToObject(0,0, IID_IBaseFilter, (void **)ppFilter);
	if (!*ppFilter)	return (hr);
	if (strFilterName == NULL)
	{
		IPropertyBag *pBag = NULL;
		pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)&pBag);
		if (pBag)
		{
			pBag->Read(L"FriendlyName", &var, 0);
			strFilterName = var.bstrVal;
			pBag->Release();
		}
	}
	hr = pGB->AddFilter(*ppFilter, strFilterName);
	if (var.vt == VT_BSTR) SysFreeString(var.bstrVal);
	return hr;
}

void GetErrorStrings(HRESULT hr, CString&sError, CString&sDescription)
{
	const TCHAR *pstrError = DXGetErrorString8(hr);
	sError = pstrError;
	sDescription = DXGetErrorDescription8(hr);
	sDescription.Replace(_T("&"), _T("\n"));
}

HRESULT EnumFilterCategory(REFCLSID guidCategory, BOOL bAddGuidString, CStringArray &sa)
{
	HRESULT hr=S_OK;
	ICreateDevEnum *pSysDevEnum = NULL;
	IEnumMoniker *pEnumCat = NULL;

	IMoniker *pMoniker = NULL;
	ULONG cFetched;
	VARIANT varName={0};

	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC, IID_ICreateDevEnum, (void **)&pSysDevEnum);
	if FAILED(hr) return hr;

	// Enumerate all filters of the selected category  
	hr = pSysDevEnum->CreateClassEnumerator(guidCategory, &pEnumCat, 0);
	if (hr == S_OK)
	{	// Enumerate all items associated with the moniker
		while(pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK)
		{
			IPropertyBag *pPropBag;
			ASSERT(pMoniker);

			// Associate moniker with a file
			hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pPropBag);
			if (FAILED(hr)) continue;

			// Read filter name from property bag
			varName.vt = VT_BSTR;
			hr = pPropBag->Read(L"FriendlyName", &varName, 0);
			if (FAILED(hr))
			{
				RELEASE_OBJECT(pMoniker);
				continue;
			}

			CString sName(varName.bstrVal);
			SysFreeString(varName.bstrVal);
			if (bAddGuidString)
			{
				hr = pPropBag->Read(L"CLSID", &varName, 0);
				if (SUCCEEDED(hr))
				{
					CString sGUID(varName.bstrVal);
					SysFreeString(varName.bstrVal);
					sName += _T(":") + sGUID;
				}
			}
			sa.Add(sName);
			RELEASE_OBJECT(pPropBag);
			RELEASE_OBJECT(pMoniker);
		}

		RELEASE_OBJECT(pEnumCat);
	}

	pSysDevEnum->Release();
	return hr;
}
