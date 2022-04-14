//------------------------------------------------------------------------------
// File: Dump.cpp
//
// Desc: DirectShow sample code - implementation of a renderer that dumps
//       the samples it receives into a text file.
//
// Copyright (c) 1992-2001  Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------


// Summary
//
// We are a generic renderer that can be attached to any data stream that
// uses IMemInputPin data transport. For each sample we receive we write
// its contents including its properties into a dump file. The file we
// will write into is specified when the dump filter is created. GraphEdit
// creates a file open dialog automatically when it sees a filter being
// created that supports the IFileSinkFilter interface.
//
//
// Implementation
//
// Pretty straightforward really, we have our own input pin class so that
// we can override Receive, all that does is to write the properties and
// data into a raw data file (using the Write function). We don't keep
// the file open when we are stopped so the flags to the open function
// ensure that we open a file if already there otherwise we create it.
//
//
// Demonstration instructions
//
// Start GraphEdit, which is available in the SDK DXUtils folder. Drag and drop
// an MPEG, AVI or MOV file into the tool and it will be rendered. Then go to
// the filters in the graph and find the filter (box) titled "Video Renderer"
// This is the filter we will be replacing with the dump renderer. Then click
// on the box and hit DELETE. After that go to the Graph menu and select the
// "Insert Filters", from the dialog box find and select the "Dump Filter".
//
// You will be asked to supply a filename where you would like to have the
// data dumped, the data we receive in this filter is dumped in text form.
// Then dismiss the dialog. Back in the graph layout find the output pin of
// the filter that used to be connected to the input of the video renderer
// you just deleted, right click and do "Render". You should see it being
// connected to the input pin of the dump filter you just inserted.
//
// Click Pause and Run and then a little later stop on the GraphEdit frame and
// the data being passed to the renderer will be dumped into a file. Stop the
// graph and dump the filename that you entered when inserting the filter into
// the graph, the data supplied to the renderer will be displayed as raw data
//
//
// Files
//
// dump.cpp             Main implementation of the dump renderer
// dump.def             What APIs the DLL will import and export
// dump.h               Class definition of the derived renderer
// dump.rc              Version information for the sample DLL
// dumpuids.h           CLSID for the dump filter
// makefile             How to build it...
//
//
// Base classes used
//
// CBaseFilter          Base filter class supporting IMediaFilter
// CRenderedInputPin    An input pin attached to a renderer
// CUnknown             Handle IUnknown for our IFileSinkFilter
// CPosPassThru         Passes seeking interfaces upstream
// CCritSec             Helper class that wraps a critical section
//
//

#include <windows.h>
#include <commdlg.h>
#include <streams.h>
#include <Dvdmedia.h>
#include <initguid.h>
#include "dumpuids.h"
#include "dump.h"
#include "idump.h"

#define SAVE_DELETE(p) {if (p) {delete p; p=NULL; }}
// Setup data

const AMOVIESETUP_MEDIATYPE sudPinTypes =
{
	&MEDIATYPE_NULL,            // Major type
	&MEDIASUBTYPE_NULL          // Minor type
};

const AMOVIESETUP_PIN sudPins =
{
	L"Input",                   // Pin string name
	FALSE,                      // Is it rendered
	FALSE,                      // Is it an output
	FALSE,                      // Allowed none
	FALSE,                      // Likewise many
	&CLSID_NULL,                // Connects to filter
	L"Output",                  // Connects to pin
	1,                          // Number of types
	&sudPinTypes                // Pin information
};

const AMOVIESETUP_FILTER sudDump =
{
	&CLSID_Dump,                // Filter CLSID
	L"Dump",                    // String name
	MERIT_DO_NOT_USE,           // Filter merit
	1,                          // Number pins
	&sudPins                    // Pin details
};


//
//  Object creation stuff
//
CFactoryTemplate g_Templates[]= {
    L"Dump", &CLSID_Dump, CDump::CreateInstance, NULL, &sudDump
};
int g_cTemplates = 1;


// Constructor

CDumpFilter::CDumpFilter(CDump *pDump,
                         LPUNKNOWN pUnk,
                         CCritSec *pLock,
                         HRESULT *phr) :
    CBaseFilter(NAME("CDumpFilter"), pUnk, pLock, CLSID_Dump),
    m_pDump(pDump)
{
}


//
// GetPin
//
CBasePin * CDumpFilter::GetPin(int n)
{
	if (n == 0)
	{
		return m_pDump->m_pPin;
	}
	else
	{
		return NULL;
	}
}


//
// GetPinCount
//
int CDumpFilter::GetPinCount()
{
    return 1;
}


//
// Stop
//
// Overriden to close the dump file
//
STDMETHODIMP CDumpFilter::Stop()
{
	CAutoLock cObjectLock (m_pLock);
	m_pDump->CloseFile();
	return CBaseFilter::Stop();
}


//
// Pause
//
// Overriden to open the dump file
//
STDMETHODIMP CDumpFilter::Pause()
{
	CAutoLock cObjectLock (m_pLock);
//	m_pDump->OpenFile();
	return CBaseFilter::Pause();
}


//
// Run
//
// Overriden to open the dump file
//
STDMETHODIMP CDumpFilter::Run(REFERENCE_TIME tStart)
{
	CAutoLock cObjectLock (m_pLock);
//	m_pDump->m_pPin->m_nPackageNo = 0;
	m_pDump->OpenFile();
	return CBaseFilter::Run(tStart);
}


//
//  Definition of CDumpInputPin
//
CDumpInputPin::CDumpInputPin(CDump *pDump,
                             LPUNKNOWN pUnk,
                             CBaseFilter *pFilter,
                             CCritSec *pLock,
                             CCritSec *pReceiveLock,
                             HRESULT *phr) :

   CRenderedInputPin(NAME("CDumpInputPin"),
                 pFilter,                   // Filter
                 pLock,                     // Locking
                 phr,                       // Return code
                 L"Input"),                 // Pin name
   m_pReceiveLock (pReceiveLock),
   m_pDump(pDump),
   m_tLast(0),
	m_nPackageNo(0),
	m_pReceiveFnc(NULL),
	m_lRFParam(0),
	m_ppMediaTypes(NULL),
	m_nMediaTypes(0),
	m_bEnumerate(FALSE)
{
	m_AllocProps.cbAlign  = 0;
	m_AllocProps.cbBuffer = 0;
	m_AllocProps.cbPrefix = 0;
	m_AllocProps.cBuffers = 0;
}
	
CDumpInputPin::~CDumpInputPin()
{
	SetAcceptedMediaTypes(NULL, 0, FALSE);
}


//
// CheckMediaType
//
// Check if the pin can support this specific proposed type and format
//
HRESULT CDumpInputPin::CheckMediaType(const CMediaType *pMT)
{
	if (m_ppMediaTypes)
	{
		int i;
		HRESULT hr = E_FAIL;
		for (i=0; i<m_nMediaTypes; i++)
		{
			hr = S_OK;
			if (m_ppMediaTypes[i]->majortype != pMT->majortype)
			{
				hr = VFW_E_INVALIDMEDIATYPE;
				continue;
			}
			if (   (m_ppMediaTypes[i]->subtype != GUID_NULL)
				 && (m_ppMediaTypes[i]->subtype != pMT->subtype))
			{
				hr = VFW_E_INVALIDSUBTYPE;
				continue;
			}
			if (   (m_ppMediaTypes[i]->formattype != GUID_NULL)
				 && (m_ppMediaTypes[i]->formattype != pMT->formattype))
			{
				hr = VFW_E_INVALID_MEDIA_TYPE;
				continue;
			}
			if (m_ppMediaTypes[i]->pbFormat) 
			{
				if (m_ppMediaTypes[i]->formattype == FORMAT_VideoInfo)
				{
					VIDEOINFOHEADER *pVI1 = (VIDEOINFOHEADER*)m_ppMediaTypes[i]->pbFormat;
					VIDEOINFOHEADER *pVI2 = (VIDEOINFOHEADER*)pMT->pbFormat;
					if (   pVI1->bmiHeader.biWidth  != pVI2->bmiHeader.biWidth 
						|| pVI1->bmiHeader.biHeight != pVI2->bmiHeader.biHeight)
					{
						hr = VFW_E_INVALID_MEDIA_TYPE;
						continue;
					}
				}
				else if (m_ppMediaTypes[i]->formattype == FORMAT_VideoInfo2)
				{
					VIDEOINFOHEADER2 *pVI1 = (VIDEOINFOHEADER2*)m_ppMediaTypes[i]->pbFormat;
					VIDEOINFOHEADER2 *pVI2 = (VIDEOINFOHEADER2*)pMT->pbFormat;
					if (   pVI1->bmiHeader.biWidth  != pVI2->bmiHeader.biWidth 
						|| pVI1->bmiHeader.biHeight != pVI2->bmiHeader.biHeight)
					{
						hr = VFW_E_INVALID_MEDIA_TYPE;
						continue;
					}
				}
				else if (m_ppMediaTypes[i]->formattype == FORMAT_WaveFormatEx)
				{
					WAVEFORMATEX *pWF1 = (WAVEFORMATEX*)m_ppMediaTypes[i]->pbFormat;
					WAVEFORMATEX *pWF2 = (WAVEFORMATEX*)pMT->pbFormat;
					if (   (pWF1->nChannels && pWF1->nChannels != pWF2->nChannels)
						|| (pWF1->nSamplesPerSec && pWF1->nSamplesPerSec != pWF2->nSamplesPerSec)
						|| (pWF1->wBitsPerSample && pWF1->wBitsPerSample != pWF2->wBitsPerSample))
					{
						hr = VFW_E_INVALID_MEDIA_TYPE;
						continue;
					}
				}
			}
			if (hr == S_OK)
			{
				break;
			}
		}
		return hr;
	}
	return S_OK;
}

HRESULT CDumpInputPin::CheckStreaming()
{
	return CBaseInputPin::CheckStreaming();
}

HRESULT CDumpInputPin::GetMediaType(CMediaType *pmt) 
{
	if (m_ppMediaTypes)
	{
		CopyMediaType(pmt, m_ppMediaTypes[0]);
	}
	return  S_OK;
}

HRESULT CDumpInputPin::GetMediaType(int iPosition,CMediaType *pMediaType)
{
	if (m_ppMediaTypes && m_bEnumerate)
	{
		if (iPosition < m_nMediaTypes)
		{
			CopyMediaType(pMediaType, m_ppMediaTypes[iPosition]);
			return S_OK;
		}
		return VFW_S_NO_MORE_ITEMS;
	}
	return CBasePin::GetMediaType(iPosition, pMediaType);
}

//
// BreakConnect
//
// Break a connection
//
HRESULT CDumpInputPin::BreakConnect()
{
	if (m_pDump->m_pPosition != NULL)
	{
		m_pDump->m_pPosition->ForceRefresh();
	}
	return CRenderedInputPin::BreakConnect();
}


//
// ReceiveCanBlock
//
// We don't hold up source threads on Receive
//
STDMETHODIMP CDumpInputPin::ReceiveCanBlock()
{
   return S_FALSE;
}

//
// Receive
//
// Do something with this media sample
//
STDMETHODIMP CDumpInputPin::Receive(IMediaSample *pSample)
{
   CAutoLock lock(m_pReceiveLock);

   REFERENCE_TIME tStart, tStop;
   pSample->GetTime(&tStart, &tStop);
   DbgLog((LOG_TRACE, 1, TEXT("tStart(%s), tStop(%s), Diff(%d ms), Bytes(%d)"),
          (LPCTSTR) CDisp(tStart),
          (LPCTSTR) CDisp(tStop),
          (LONG)((tStart - m_tLast) / 10000),
          pSample->GetActualDataLength()));

#ifdef _DEBUG
	DbgOutString(CDisp(tStart, CDISP_DEC));
	DbgOutString(":");
	DbgOutString(CDisp(tStop, CDISP_DEC));
	DbgOutString("\n");
#endif

	m_tLast = tStart;
	if (m_pAllocator && (m_nPackageNo == 0))
	{
		m_pAllocator->GetProperties(&m_AllocProps);
	}
	if (m_pReceiveFnc)
	{
		pSample->SetMediaType(&m_mt);
		return m_pReceiveFnc(pSample,  m_nPackageNo++, m_lRFParam);
	}
	else if (m_pDump->m_pFileName == NULL)
	{
		pSample->AddRef();
		pSample->SetMediaType(&m_mt);
		return m_pFilter->NotifyEvent(EC_RECEIVE_MEDIA_SAMPLE, (long)pSample, m_nPackageNo++);
	}
	else
	{
		PBYTE pbData;
		HRESULT hr = pSample->GetPointer(&pbData);
		if (FAILED(hr))
		{
			return hr;
		}
		m_nPackageNo++;
		return m_pDump->Write(pbData, pSample->GetActualDataLength());
	}
}

STDMETHODIMP CDumpInputPin::SetReceiveFunction(RECEIVE_FUNCTION pReceiveFnc, long lParam)
{
	m_pReceiveFnc = pReceiveFnc;
	m_lRFParam    = lParam;
	return NOERROR;
}

STDMETHODIMP CDumpInputPin::SetAcceptedMediaTypes(AM_MEDIA_TYPE*pMT, int nCount, BOOL bEnumerate)
{
	int i;
	if (m_ppMediaTypes)
	{
		for (i=0; i<m_nMediaTypes; i++)
		{
			DeleteMediaType(m_ppMediaTypes[i]);
		}
	}
	if (nCount)
	{
		if (pMT == NULL) return E_POINTER;
		if (m_ppMediaTypes)
		{
			m_ppMediaTypes = (AM_MEDIA_TYPE**)CoTaskMemRealloc(m_ppMediaTypes, nCount * sizeof(AM_MEDIA_TYPE*));
		}
		else
		{
			m_ppMediaTypes = (AM_MEDIA_TYPE**)CoTaskMemAlloc(nCount * sizeof(AM_MEDIA_TYPE*));
		}
		for (i=0; i<nCount; i++)
		{
			m_ppMediaTypes[i] = CreateMediaType(&pMT[i]);
		}
		m_nMediaTypes = nCount;
	}
	else
	{
		if (m_ppMediaTypes)
		{
			CoTaskMemFree(m_ppMediaTypes);
			m_ppMediaTypes = NULL;
		}
		m_nMediaTypes = 0;
	}
	m_bEnumerate = bEnumerate;
   return NOERROR;
}

STDMETHODIMP CDumpInputPin::GetAllocatorProperties(ALLOCATOR_PROPERTIES *pAP)
{
	if (pAP == NULL) return E_POINTER;
	if (m_AllocProps.cbBuffer != 0)
	{
		*pAP = m_AllocProps;
		return S_OK;
	}
	return E_NOTIMPL;
}

STDMETHODIMP CDumpInputPin::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
    if (riid == IID_IDump)
	{
		return GetInterface((IDump*) this, ppv);
	}
	else
	{
		return CRenderedInputPin::NonDelegatingQueryInterface(riid, ppv);
	}
}

//
// DumpStringInfo
//
// Write to the file as text form
//
HRESULT CDumpInputPin::WriteStringInfo(IMediaSample *pSample)
{
	if (m_pDump->m_pFileName == NULL)
	{
		return NOERROR;
	}
   TCHAR TempString[256],FileString[256];
   PBYTE pbData;

   // Retrieve the time stamps from this sample

   REFERENCE_TIME tStart, tStop;
   pSample->GetTime(&tStart, &tStop);
   m_tLast = tStart;

    // Write the sample time stamps out

   wsprintf(FileString,TEXT("\r\nRenderer received sample (%dms)"),timeGetTime());
   m_pDump->WriteString(FileString);
   wsprintf(FileString,TEXT("   Start time (%s)"),CDisp(tStart));
   m_pDump->WriteString(FileString);
   wsprintf(FileString,TEXT("   End time (%s)"),CDisp(tStop));
   m_pDump->WriteString(FileString);

   // Display the media times for this sample

   HRESULT hr = pSample->GetMediaTime(&tStart, &tStop);
   if (hr == NOERROR)
   {
		wsprintf(FileString,TEXT("   Start media time (%s)"),CDisp(tStart));
		m_pDump->WriteString(FileString);
		wsprintf(FileString,TEXT("   End media time (%s)"),CDisp(tStop));
		m_pDump->WriteString(FileString);
   }

   // Is this a sync point sample

   hr = pSample->IsSyncPoint();
   wsprintf(FileString,TEXT("   Sync point (%d)"),(hr == S_OK));
   m_pDump->WriteString(FileString);

   // Is this a preroll sample

   hr = pSample->IsPreroll();
   wsprintf(FileString,TEXT("   Preroll (%d)"),(hr == S_OK));
   m_pDump->WriteString(FileString);

   // Is this a discontinuity sample

   hr = pSample->IsDiscontinuity();
   wsprintf(FileString,TEXT("   Discontinuity (%d)"),(hr == S_OK));
   m_pDump->WriteString(FileString);

   // Write the actual data length

   LONG DataLength = pSample->GetActualDataLength();
   wsprintf(FileString,TEXT("   Actual data length (%d)"),DataLength);
   m_pDump->WriteString(FileString);

   // Does the sample have a type change aboard

   AM_MEDIA_TYPE *pMediaType;
   pSample->GetMediaType(&pMediaType);
   wsprintf(FileString,TEXT("   Type changed (%d)"),
       (pMediaType ? TRUE : FALSE));
   m_pDump->WriteString(FileString);
   DeleteMediaType(pMediaType);

   // Copy the data to the file

	hr = pSample->GetPointer(&pbData);
	if (FAILED(hr))
	{
		return hr;
	}

	// Write each complete line out in BYTES_PER_LINES groups

	for (int Loop = 0;Loop < (DataLength / BYTES_PER_LINE);Loop++)
	{
		wsprintf(FileString,FIRST_HALF_LINE,pbData[0],pbData[1],pbData[2],
				pbData[3],pbData[4],pbData[5],pbData[6],
				pbData[7],pbData[8],pbData[9]);
		wsprintf(TempString,SECOND_HALF_LINE,pbData[10],pbData[11],pbData[12],
				pbData[13],pbData[14],pbData[15],pbData[16],
				pbData[17],pbData[18],pbData[19]);
		lstrcat(FileString,TempString);
		m_pDump->WriteString(FileString);
		pbData += BYTES_PER_LINE;
	}

	// Write the last few bytes out afterwards
	wsprintf(FileString,TEXT("   "));
	for (Loop = 0;Loop < (DataLength % BYTES_PER_LINE);Loop++)
	{
		wsprintf(TempString,TEXT("%x "),pbData[Loop]);
		lstrcat(FileString,TempString);
	}
	m_pDump->WriteString(FileString);
	return NOERROR;
}


//
// EndOfStream
//
STDMETHODIMP CDumpInputPin::EndOfStream(void)
{
	CAutoLock lock(m_pReceiveLock);
	return CRenderedInputPin::EndOfStream();
} // EndOfStream


//
// NewSegment
//
// Called when we are seeked
//
STDMETHODIMP CDumpInputPin::NewSegment(REFERENCE_TIME tStart,
                                       REFERENCE_TIME tStop,
                                       double dRate)
{
	m_tLast = 0;
	m_nPackageNo = 0;
	return S_OK;
} // NewSegment


//
//  CDump class
//
CDump::CDump(LPUNKNOWN pUnk, HRESULT *phr) :
    CUnknown(NAME("CDump"), pUnk),
    m_pFilter(NULL),
    m_pPin(NULL),
    m_pPosition(NULL),
    m_hFile(INVALID_HANDLE_VALUE),
    m_pFileName(0),
	 m_pMediaType(0)
{
   m_pFilter = new CDumpFilter(this, GetOwner(), &m_Lock, phr);
   if (m_pFilter == NULL)
	{
      *phr = E_OUTOFMEMORY;
      return;
   }

   m_pPin = new CDumpInputPin(this, GetOwner(), m_pFilter,
                              &m_Lock, &m_ReceiveLock,
                              phr);
   if (m_pPin == NULL)
	{
      *phr = E_OUTOFMEMORY;
      return;
   }
}


//
// SetFileName
//
// Implemented for IFileSinkFilter support
//
STDMETHODIMP CDump::SetFileName(LPCOLESTR pszFileName, const AM_MEDIA_TYPE *pmt)
{
   // Is this a valid filename supplied
   SAVE_DELETE(m_pFileName);
	if (pszFileName == NULL)
	{
		return S_OK;
	}

   CheckPointer(pszFileName,E_POINTER);
   if(wcslen(pszFileName) > MAX_PATH)
      return ERROR_FILENAME_EXCED_RANGE;

   // Take a copy of the filename

   m_pFileName = new WCHAR[1+lstrlenW(pszFileName)];
   if (m_pFileName == 0)
      return E_OUTOFMEMORY;
   lstrcpyW(m_pFileName,pszFileName);

	if (pmt)
	{
		m_pMediaType  = new AM_MEDIA_TYPE;
		*m_pMediaType = *pmt;
	}
   // Create the file then close it

   HRESULT hr = OpenFile();
   CloseFile();
   return hr;

} // SetFileName


//
// GetCurFile
//
// Implemented for IFileSinkFilter support
//
STDMETHODIMP CDump::GetCurFile(LPOLESTR * ppszFileName,AM_MEDIA_TYPE *pmt)
{
   CheckPointer(ppszFileName, E_POINTER);
   *ppszFileName = NULL;
   if (m_pFileName != NULL)
	{
      *ppszFileName = (LPOLESTR)
      QzTaskMemAlloc(sizeof(WCHAR) * (1+lstrlenW(m_pFileName)));
      if (*ppszFileName != NULL)
		{
         lstrcpyW(*ppszFileName, m_pFileName);
      }
   }

   if(pmt)
	{
		if (m_pMediaType)
		{
			*pmt = *m_pMediaType;
		}
		else
		{
			ZeroMemory(pmt, sizeof(*pmt));
			pmt->majortype = MEDIATYPE_NULL;
			pmt->subtype = MEDIASUBTYPE_NULL;
		}
   }
   return S_OK;

} // GetCurFile


// Destructor

CDump::~CDump()
{
		CloseFile();
		SAVE_DELETE(m_pPin);
		SAVE_DELETE(m_pFilter);
		SAVE_DELETE(m_pPosition);
		SAVE_DELETE(m_pFileName);
		SAVE_DELETE(m_pMediaType);
}


//
// CreateInstance
//
// Provide the way for COM to create a dump filter
//
CUnknown * WINAPI CDump::CreateInstance(LPUNKNOWN punk, HRESULT *phr)
{
	CDump *pNewObject = new CDump(punk, phr);
	if (pNewObject == NULL)
	{
		*phr = E_OUTOFMEMORY;
	}
	return pNewObject;
} // CreateInstance


//
// NonDelegatingQueryInterface
//
// Override this to say what interfaces we support where
//
STDMETHODIMP CDump::NonDelegatingQueryInterface(REFIID riid, void ** ppv)
{
	CheckPointer(ppv,E_POINTER);
	CAutoLock lock(&m_Lock);

	// Do we have this interface

	if (riid == IID_IFileSinkFilter)
	{
		return GetInterface((IFileSinkFilter *) this, ppv);
	} 
	else if (riid == IID_IDump)
	{
		return GetInterface((IDump *) this, ppv);
	} 
	else if (riid == IID_IBaseFilter || riid == IID_IMediaFilter || riid == IID_IPersist)
	{
		return m_pFilter->NonDelegatingQueryInterface(riid, ppv);
	} 
	else if (riid == IID_IMediaPosition || riid == IID_IMediaSeeking)
	{
		if (m_pPosition == NULL)
		{
			HRESULT hr = S_OK;
			m_pPosition = new CPosPassThru(NAME("Dump Pass Through"),
													 (IUnknown *) GetOwner(),
													 (HRESULT *) &hr, m_pPin);
			if (m_pPosition == NULL)
			{
				return E_OUTOFMEMORY;
			}

			if (FAILED(hr))
			{
				delete m_pPosition;
				m_pPosition = NULL;
				return hr;
			}
		}

		return m_pPosition->NonDelegatingQueryInterface(riid, ppv);
	}

	return CUnknown::NonDelegatingQueryInterface(riid, ppv);

} // NonDelegatingQueryInterface


//
// OpenFile
//
// Opens the file ready for dumping
//
HRESULT CDump::OpenFile()
{
   TCHAR *pFileName = NULL;

   // Is the file already opened
   if (m_hFile != INVALID_HANDLE_VALUE)
	{
      return NOERROR;
   }

   // Has a filename been set yet
   if (m_pFileName == NULL)
	{
      return ERROR_INVALID_NAME;
   }

   // Convert the UNICODE filename if necessary

#if defined(WIN32) && !defined(UNICODE)
   char convert[MAX_PATH];
   if(!WideCharToMultiByte(CP_ACP,0,m_pFileName,-1,convert,MAX_PATH,0,0))
      return ERROR_INVALID_NAME;
   pFileName = convert;
#else
   pFileName = m_pFileName;
#endif

   // Try to open the file

   m_hFile = CreateFile((LPCTSTR) pFileName,   // The filename
                        GENERIC_WRITE,         // File access
                        (DWORD) 0,             // Share access
                        NULL,                  // Security
                        CREATE_ALWAYS,         // Open flags
                        (DWORD) 0,             // More flags
                        NULL);                 // Template

   if (m_hFile == INVALID_HANDLE_VALUE)
	{
      DWORD dwErr = GetLastError();
      return HRESULT_FROM_WIN32(dwErr);
   }
   return S_OK;

} // Open


//
// CloseFile
//
// Closes any dump file we have opened
//
HRESULT CDump::CloseFile()
{
   if (m_hFile == INVALID_HANDLE_VALUE)
	{
      return NOERROR;
   }

   CloseHandle(m_hFile);
   m_hFile = INVALID_HANDLE_VALUE;
   return NOERROR;

} // Open


//
// Write
//
// Write stuff to the file
//
HRESULT CDump::Write(PBYTE pbData,LONG lData)
{
   DWORD dwWritten;

   if (!WriteFile(m_hFile,(PVOID)pbData,(DWORD)lData,&dwWritten,NULL))
	{
      DWORD dwErr = GetLastError();
      return HRESULT_FROM_WIN32(dwErr);
   }
   return S_OK;
}


//
// WriteString
//
// Writes the given string into the file
//
void CDump::WriteString(TCHAR *pString)
{
   DWORD dwWritten = lstrlen(pString);
   const TCHAR *pEndOfLine = TEXT("\r\n");

   WriteFile((HANDLE) m_hFile,
             (PVOID) pString,
             (DWORD) dwWritten,
             &dwWritten, NULL);

   dwWritten = lstrlen(pEndOfLine);
   WriteFile((HANDLE) m_hFile,
             (PVOID) pEndOfLine,
             (DWORD) dwWritten,
             &dwWritten, NULL);

} // WriteString

STDMETHODIMP CDump::SetAcceptedMediaTypes(AM_MEDIA_TYPE*pMT, int nCount, BOOL bEnumerate)
{
	if (m_pPin == NULL) return E_NOINTERFACE;
	return m_pPin->SetAcceptedMediaTypes(pMT, nCount, bEnumerate);
}
STDMETHODIMP CDump::GetAllocatorProperties(ALLOCATOR_PROPERTIES *pAP)
{
	if (m_pPin == NULL) return E_NOINTERFACE;
	return m_pPin->GetAllocatorProperties(pAP);
}

STDMETHODIMP CDump::SetReceiveFunction(RECEIVE_FUNCTION pReceiveFnc, long lParam)
{
	if (m_pPin == NULL) return E_NOINTERFACE;
	return m_pPin->SetReceiveFunction(pReceiveFnc, lParam);
 }

//
// DllRegisterSever
//
// Handle the registration of this filter
//
STDAPI DllRegisterServer()
{
   return AMovieDllRegisterServer2( TRUE );
} // DllRegisterServer


//
// DllUnregisterServer
//
STDAPI DllUnregisterServer()
{
   return AMovieDllRegisterServer2( FALSE );
} // DllUnregisterServer

