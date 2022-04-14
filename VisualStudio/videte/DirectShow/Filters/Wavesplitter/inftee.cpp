//------------------------------------------------------------------------------
// File: WaveSplitter.cpp
//
// Desc: DirectShow sample code - pass through filter that splits a
//       data stream into many output channels.
//
// Copyright (c) 1992-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------


// Summary
//
// This is a sample DirectShow pass-through filter. We have a single input
// pin and can have many output pins. We start with one output pin and each
// time we connect an output pin we spawn another, although we could keep
// doing this ad infinitum we have a top level maximum of WAVESPLITTER_MAX_PINS.
// Any data samples our input pin receives will be sent down each output
// pin in turn. Each output pin has a separate thread if necessary (see
// the output queue class in the SDK) to avoid delivery blocking our thread
//
// Demonstration instructions
//
// Start GraphEdit, which is available in the SDK DXUtils folder. Drag and drop
// an MPEG, AVI or MOV file into the tool and it will be rendered. Then go to
// the filters in the graph and find the filter (box) titled "Video Renderer"
// Then click on the box and hit DELETE. After that go to the Graph menu and
// select "Insert Filters", from the dialog box find and select the "Wave
// Splitter Filter" and then dismiss the dialog. Back in the graph layout find the
// output pin of the filter that was connected to the input of the video
// renderer you just deleted, right click and select "Render". You should
// see it being connected to the input pin of the filter you just inserted
//
// The Wave Splitter Filter will have one output pin connected and will have
// spawned another, right click on this and select Render. A new renderer
// will pop up fo the stream. Do this once or twice more and then click on
// the Pause and Run on the GraphEdit frame and you will see the video...
//      .. many times over in different windows
//
// Files
//
// inftee.cpp           Main implementation of the Wave Splitter
// inftee.def           What APIs the DLL will import and export
// inftee.h             Class definition of the infinite tee
// inftee.rc            Not much, just our version information
// makefile             How to build it...
//
//
// Base classes used
//
// CBaseInputPin        Basic IMemInputPin based input pin
// CBaseOutputPin       Used for basic connection stuff
// CBaseFilter          Well we need a filter don't we
// CCritSec             Controls access to output pin list
// COutputQueue         Delivers data on a separate thread
//
//


#include <streams.h>
#include <initguid.h>
#include "inftee.h"
#include <tchar.h>
#include <stdio.h>
#define WAVESPLITTER_MAX_PINS 10

#include <MMREG.H>

#include "iwavesplitter.h"
#include "WK_Msg.h"

// Using this pointer in constructor
#pragma warning(disable:4355 4127)

#define INPUT_PIN  L"Input"
#define OUTPUT_PIN L"Output"
// Setup data

const AMOVIESETUP_MEDIATYPE sudPinTypes =
{
    &MEDIATYPE_Audio,       // Major CLSID
    &MEDIASUBTYPE_PCM		// Minor type
};

const AMOVIESETUP_PIN psudPins[] =
{
    { INPUT_PIN,            // Pin's string name
      FALSE,                // Is it rendered
      FALSE,                // Is it an output
      FALSE,                // Allowed none
      FALSE,                // Allowed many
      &CLSID_NULL,          // Connects to filter
      OUTPUT_PIN,            // Connects to pin
      1,                    // Number of types
      &sudPinTypes },       // Pin information
    { OUTPUT_PIN,            // Pin's string name
      FALSE,                // Is it rendered
      TRUE,                 // Is it an output
      FALSE,                // Allowed none
      FALSE,                // Allowed many
      &CLSID_NULL,          // Connects to filter
      INPUT_PIN,             // Connects to pin
      1,                    // Number of types
      &sudPinTypes }        // Pin information
};

const AMOVIESETUP_FILTER sudWaveSplitter =
{
    &CLSID_WaveSplitter,   // CLSID of filter
    L"Wave Splitter",		// Filter's name
    MERIT_DO_NOT_USE,      // Filter merit
    2,                     // Number of pins
    psudPins               // Pin information
};


CFactoryTemplate g_Templates [1] =
{
    { L"Wave Splitter"
    , &CLSID_WaveSplitter
    , CWaveSplitter::CreateInstance
    , NULL
    , &sudWaveSplitter }
};
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);


//
// CreateInstance
//
// Creator function for the class ID
//
CUnknown * WINAPI CWaveSplitter::CreateInstance(LPUNKNOWN pUnk, HRESULT *phr)
{
    return new CWaveSplitter(NAME("Wave Splitter Filter"), pUnk, phr);
}


HWND CWaveSplitter::gm_hWndThread = NULL;
const _TCHAR CWaveSplitter::gm_sModuleName[] = _T("WaveSplitter");
//
// Constructor
//
BOOL CALLBACK CWaveSplitter::EnumThreadWndProc(HWND hwnd, LPARAM lParam)
{
	if (GetParent(hwnd) == NULL)
	{
		*((HWND*)lParam) = hwnd;
		return FALSE;
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
CWaveSplitter::CWaveSplitter(TCHAR *pName, LPUNKNOWN pUnk, HRESULT *phr) :
    m_OutputPinsList(NAME("Wave Output Pins list")),
    m_lCanSeek(TRUE),
    m_pAllocator(NULL),
    m_NumOutputPins(0),
    m_Input(NAME("Wave Input Pin"), this, phr, INPUT_PIN),
	 m_bActivated(true),
	 m_nThresholdLevel(0),
	 m_nDeliver(0),
	 m_pCheckFnc(NULL),
	 m_lParamCheckFnc(0),
    CBaseFilter(NAME("Splitter filter"), pUnk, this, CLSID_WaveSplitter)
{
   ASSERT(phr);

   // Create a single output pin at this time
   InitOutputPinsList(false);

	CWaveOutputPin *pOutputPin = CreateNextOutputPin(this);

	if (pOutputPin != NULL )
	{
		m_NumOutputPins++;
		m_OutputPinsList.AddTail(pOutputPin);
	}
	if (gm_hWndThread == NULL)
	{
		EnumThreadWindows(GetCurrentThreadId(), EnumThreadWndProc, (LPARAM)&gm_hWndThread);
		if (gm_hWndThread)
		{
			LRESULT lResult = 0;
			SendMessage(gm_hWndThread, WM_TRACE_TEXT, (WPARAM)&lResult, NULL);	// Test whether the message is handled
			if (lResult == 0)
			{
				gm_hWndThread = NULL;
			}
		}
	}
}


//
// Destructor
//
CWaveSplitter::~CWaveSplitter()
{
    InitOutputPinsList(true);
}


//
// GetPinCount
//
int CWaveSplitter::GetPinCount()
{
    return (1 + m_NumOutputPins);
}


//
// GetPin
//
CBasePin *CWaveSplitter::GetPin(int n)
{
    if (n < 0)
        return NULL ;
 
    // Pin zero is the one and only input pin
    if (n == 0)
        return &m_Input;

    // return the output pin at position(n - 1) (zero based)
    return GetPinNFromList(n-1);
}


//
// InitOutputPinsList
//
void CWaveSplitter::InitOutputPinsList(bool bDelete)
{
    POSITION pos = m_OutputPinsList.GetHeadPosition();
    while(pos)
    {
        CWaveOutputPin *pOutputPin = m_OutputPinsList.GetNext(pos);
        ASSERT(pOutputPin->m_pOutputQueue == NULL);
		  if (bDelete) DeleteOutputPin(pOutputPin);
        else         pOutputPin->Release();
    }
    m_NumOutputPins = 0;
    m_OutputPinsList.RemoveAll();

} // InitOutputPinsList


//
// CreateNextOutputPin
//
CWaveOutputPin *CWaveSplitter::CreateNextOutputPin(CWaveSplitter *pSplit)
{
	WCHAR szbuf[32];
	HRESULT          hr = NOERROR;
	int              nPinNumber = m_NumOutputPins + 1;

	if      (nPinNumber == 1) swprintf(szbuf, L"Left");
	else if (nPinNumber == 2) swprintf(szbuf, L"Right");
	else							  swprintf(szbuf, L"Output %d", nPinNumber);
/*
	switch (nPinNumber)
	{
		case 1: swprintf(szbuf, L"Left"); break;
		case 2: swprintf(szbuf, L"Right"); break;
		case 3: swprintf(szbuf, L"Center Front"); break;
		case 4: swprintf(szbuf, L"Left Effect"); break;
		case 5: swprintf(szbuf, L"Right Effect"); break;
		case 6: swprintf(szbuf, L"Center Rear"); break;
		case 7: swprintf(szbuf, L"Sub"); break;
		case 8: swprintf(szbuf, L"Left Rear"); break;
		case 9: swprintf(szbuf, L"Right Rear"); break;
	}
*/
	CWaveOutputPin *pPin = new CWaveOutputPin(NAME("Wave Output"), pSplit,
					 &hr, szbuf,
					 nPinNumber);

	if (FAILED(hr) || pPin == NULL)
	{
		delete pPin;
		return NULL;
	}

	pPin->AddRef();

	return pPin;

} // CreateNextOutputPin


//
// DeleteOutputPin
//
void CWaveSplitter::DeleteOutputPin(CWaveOutputPin *pPin)
{
    POSITION pos = m_OutputPinsList.GetHeadPosition();
    while(pos)
	 {
        POSITION posold = pos;         // Remember this position
        CWaveOutputPin *pOutputPin = m_OutputPinsList.GetNext(pos);
        if (pOutputPin == pPin)
		  {
            // If this pin holds the seek interface release it
            if (pPin->m_bHoldsSeek)
				{
                InterlockedExchange(&m_lCanSeek, FALSE);
                pPin->m_bHoldsSeek = FALSE;
                pPin->m_pPosition->Release();
            }

            m_OutputPinsList.Remove(posold);
            ASSERT(pOutputPin->m_pOutputQueue == NULL);
            delete pPin;
            m_NumOutputPins--;
				ASSERT(m_NumOutputPins >= 0);
				IncrementPinVersion();
            break;
        }
    }

} // DeleteOutputPin


//
// GetNumFreePins
//
int CWaveSplitter::GetNumFreePins()
{
    int n = 0;
    POSITION pos = m_OutputPinsList.GetHeadPosition();
    while(pos)
	 {
        CWaveOutputPin *pOutputPin = m_OutputPinsList.GetNext(pos);
        if (pOutputPin->m_Connected == NULL)
            n++;
    }
    return n;

} // GetNumFreePins


//
// GetPinNFromList
//
CWaveOutputPin *CWaveSplitter::GetPinNFromList(int n)
{
    // Validate the position being asked for
	if (n >= m_NumOutputPins)
	{
      return NULL;
	}
   // Get the head of the list
   POSITION pos = m_OutputPinsList.GetHeadPosition();

   n++;       // Make the number 1 based

   CWaveOutputPin *pOutputPin=0;
   while(n)
	{
		pOutputPin = m_OutputPinsList.GetNext(pos);
      n--;
   }

   return pOutputPin;

} // GetPinNFromList


//
// Stop
//
// Overriden to handle no input connections
//
STDMETHODIMP CWaveSplitter::Stop()
{
    CBaseFilter::Stop();
    m_State = State_Stopped;

	 CWaveOutputPin *pPin;
	 for (int i=0; i<m_NumOutputPins; i++)
	 {
		 pPin = GetPinNFromList(i);
		 if (pPin)
		 {
			 pPin->m_bSynchronized = false;
		 }
	 }

    return NOERROR;
}


//
// Pause
//
// Overriden to handle no input connections
//
STDMETHODIMP CWaveSplitter::Pause()
{
    CAutoLock cObjectLock (m_pLock);
    HRESULT hr = CBaseFilter::Pause();
    if (m_Input.IsConnected() == FALSE)
	 {
        m_Input.EndOfStream();
    }
    return hr;
}


//
// Run
//
// Overriden to handle no input connections
//
STDMETHODIMP CWaveSplitter::Run(REFERENCE_TIME tStart)
{
    CAutoLock cObjectLock (m_pLock);
    HRESULT hr = CBaseFilter::Run(tStart);

    if (m_Input.IsConnected() == FALSE)
	 {
       m_Input.EndOfStream();
    }
    return hr;
}

STDMETHODIMP CWaveSplitter::SetSyncSource(IReferenceClock *pCl)
{
	return CBaseFilter::SetSyncSource(pCl);
}

//////////////////////////////////////////////////////////////////////////
#define MAX_TRACE_STRINGLEN 1024
void CWaveSplitter::Trace(LPCTSTR sFormat, ...)
{
	_TCHAR szString[MAX_TRACE_STRINGLEN];
	szString[0]=0;
	szString[MAX_TRACE_STRINGLEN-1]=0;	// safer if _vsnprintf fails

	va_list args;
	va_start(args, sFormat);

	int iReturn = _vsntprintf(szString, MAX_TRACE_STRINGLEN-1, sFormat, args);

	va_end(args);
	if (gm_hWndThread)
	{
#ifdef _DEBUG
		_tcscat(szString, _T("\n"));
		OutputDebugStr(szString);
#else
		PostMessage(gm_hWndThread, WM_TRACE_TEXT, (WPARAM)gm_sModuleName, (LPARAM)_tcsdup(szString));
#endif
	}
}
//
// CWaveInputPin constructor
//
CWaveInputPin::CWaveInputPin(TCHAR *pName,
                           CWaveSplitter *pSplit,
                           HRESULT *phr,
                           LPCWSTR pPinName) :
    CBaseInputPin(pName, pSplit, pSplit, phr, pPinName),
    m_pSplit(pSplit),
    m_bInsideCheckMediaType(FALSE)
{
    ASSERT(pSplit);
}


#ifdef DEBUG
//
// CWaveInputPin destructor
//
CWaveInputPin::~CWaveInputPin()
{
    DbgLog((LOG_TRACE,2,TEXT("CWaveInputPin destructor")));
    ASSERT(m_pSplit->m_pAllocator == NULL);
}
#endif


#ifdef DEBUG
//
// DisplayMediaType -- (DEBUG ONLY)
//
void DisplayMediaType(TCHAR *pDescription,const CMediaType *pmt)
{

    // Dump the GUID types and a short description

    DbgLog((LOG_TRACE,2,TEXT("")));
    DbgLog((LOG_TRACE,2,TEXT("%s"),pDescription));
    DbgLog((LOG_TRACE,2,TEXT("")));
    DbgLog((LOG_TRACE,2,TEXT("Media Type Description")));
    DbgLog((LOG_TRACE,2,TEXT("Major type %s"),GuidNames[*pmt->Type()]));
    DbgLog((LOG_TRACE,2,TEXT("Subtype %s"),GuidNames[*pmt->Subtype()]));
    DbgLog((LOG_TRACE,2,TEXT("Subtype description %s"),GetSubtypeName(pmt->Subtype())));
    DbgLog((LOG_TRACE,2,TEXT("Format size %d"),pmt->cbFormat));

    // Dump the generic media types */

    DbgLog((LOG_TRACE,2,TEXT("Fixed size sample %d"),pmt->IsFixedSize()));
    DbgLog((LOG_TRACE,2,TEXT("Temporal compression %d"),pmt->IsTemporalCompressed()));
    DbgLog((LOG_TRACE,2,TEXT("Sample size %d"),pmt->GetSampleSize()));


} // DisplayMediaType
#endif


//
// CheckMediaType
//
HRESULT CWaveInputPin::CheckMediaType(const CMediaType *pmt)
{
	CAutoLock lock_it(m_pLock);
/*
	if(pmt->majortype != MEDIATYPE_Audio || pmt->subtype != MEDIASUBTYPE_PCM)
	{
		return VFW_E_TYPE_NOT_ACCEPTED;
	}
*/
	// If we are already inside checkmedia type for this pin, return NOERROR
	// It is possble to hookup two of the splitter filters and some other filter
	// like the video effects sample to get into this situation. If we don't
	// detect this situation, we will carry on looping till we blow the stack

	if (m_bInsideCheckMediaType == TRUE)
	  return NOERROR;

	m_bInsideCheckMediaType = TRUE;
	HRESULT hr = NOERROR;

#ifdef DEBUG
   // Display the type of the media for debugging perposes
   DisplayMediaType(TEXT("Input Pin Checking"), pmt);
#endif

	// The media types that we can support are entirely dependent on the
	// downstream connections. If we have downstream connections, we should
	// check with them - walk through the list calling each output pin

	if (m_Connected)
	{
		hr = m_Connected->QueryAccept(pmt);
		if (hr != NOERROR)
		{
			m_bInsideCheckMediaType = FALSE;
			return VFW_E_TYPE_NOT_ACCEPTED;
		}
	}


	int n = m_pSplit->m_NumOutputPins;
	POSITION pos = m_pSplit->m_OutputPinsList.GetHeadPosition();

	while(n) 
	{
		CWaveOutputPin *pOutputPin = m_pSplit->m_OutputPinsList.GetNext(pos);
		if (pOutputPin != NULL)
		{
			if (pOutputPin->m_Connected != NULL)
			{
				 // The pin is connected, check its peer

				hr = pOutputPin->m_Connected->QueryAccept(pmt);
				if (hr != NOERROR)
				{
					m_bInsideCheckMediaType = FALSE;
					return VFW_E_TYPE_NOT_ACCEPTED;
				}

				WAVEFORMATEX *pWF = (WAVEFORMATEX *)pmt->Format();
				if (pWF && pOutputPin->m_mt.pbFormat)
				{
					WAVEFORMATEX *pWFout = (WAVEFORMATEX *)pOutputPin->m_mt.pbFormat;
					if ((pWF->nBlockAlign / pWF->nChannels) != (pWFout->nBlockAlign / pWFout->nChannels))
					{
						ASSERT(false);
						return VFW_E_TYPE_NOT_ACCEPTED;
					}
				}
			}
		}
		else
		{
			// We should have as many pins as the count says we have
			ASSERT(FALSE);
		}
		n--;
	}

	// Either all the downstream pins have accepted or there are none.
	m_bInsideCheckMediaType = FALSE;
	return NOERROR;

} // CheckMediaType


//
// SetMediaType
//
HRESULT CWaveInputPin::SetMediaType(const CMediaType *pmt)
{
	CAutoLock lock_it(m_pLock);
	HRESULT hr = NOERROR;

	// Make sure that the base class likes it
	hr = CBaseInputPin::SetMediaType(pmt);
	if (FAILED(hr))
	   return hr;

	// Force any output pins to use our type
	if (m_mt.formattype != FORMAT_WaveFormatEx)
		return VFW_E_INVALIDSUBTYPE;

	WAVEFORMATEX *pWF = (WAVEFORMATEX *) m_mt.pbFormat;
	if (pWF->nChannels > WAVESPLITTER_MAX_PINS)
		return VFW_E_INVALIDSUBTYPE;

	if (pWF->nChannels > m_pSplit->m_NumOutputPins)
	{
		for (int n=m_pSplit->m_NumOutputPins; n<pWF->nChannels; n++)
		{
			CWaveOutputPin *pOutputPin = m_pSplit->CreateNextOutputPin(m_pSplit);

			if (pOutputPin != NULL )
			{
				m_pSplit->m_NumOutputPins++;
				m_pSplit->m_OutputPinsList.AddTail(pOutputPin);
			}
		}
	}
	else if (pWF->nChannels < m_pSplit->m_NumOutputPins)
	{
		while (pWF->nChannels < m_pSplit->m_NumOutputPins)
		{
			POSITION pos = m_pSplit->m_OutputPinsList.GetTailPosition();
			CWaveOutputPin *pOutputPin = m_pSplit->m_OutputPinsList.Get(pos);
			if (pOutputPin != NULL )
			{
				m_pSplit->DeleteOutputPin(pOutputPin);
			}
		}
	}

	ASSERT(m_Connected != NULL);
	return NOERROR;

} // SetMediaType


//
// BreakConnect
//
HRESULT CWaveInputPin::BreakConnect()
{
    // Release any allocator that we are holding
    if (m_pSplit->m_pAllocator)
    {
        m_pSplit->m_pAllocator->Release();
        m_pSplit->m_pAllocator = NULL;
    }
    return NOERROR;

} // BreakConnect

STDMETHODIMP CWaveInputPin::SetThreshold(float fValue)
{
   CAutoLock lock_it(m_pLock);
	m_pSplit->m_nThresholdLevel =(short) (fValue * 100.0f);
	if (m_pSplit->m_nThresholdLevel > 0)			// Durchgang nur bei Schwellwertüberschreitung
	{
		m_pSplit->m_bActivated = false;
	}
	else if (m_pSplit->m_nThresholdLevel == 0)	// Immer auf Durchgang geschaltet
	{
		m_pSplit->m_bActivated = true;
	}
	else if (m_pSplit->m_nThresholdLevel < 0)		// Keine Schwellwerterkennung
	{
		m_pSplit->m_bActivated = false;
	}
	return S_OK;
}

STDMETHODIMP CWaveInputPin::SetMediaSampleCheckFnc(MEDIASAMPLE_FUNCTION pFnc, long lParam)
{
   CAutoLock lock_it(m_pLock);
	m_pSplit->m_pCheckFnc = pFnc;
	m_pSplit->m_lParamCheckFnc = lParam;
	return S_OK;
}

STDMETHODIMP CWaveInputPin::SetPinNumber(int nPin, int nNumber)
{
   CAutoLock lock_it(m_pLock);
	if (nNumber <= 0                     ) return E_INVALIDARG;
	if (nNumber > m_pSplit->GetPinCount()) return E_INVALIDARG;
	if (nPin >= m_pSplit->GetPinCount())   return E_INVALIDARG;
	CWaveOutputPin *pPin = m_pSplit->GetPinNFromList(nPin);
	if (pPin == NULL)                      return E_NOINTERFACE;
	pPin->m_nPinNumber = nNumber;
	return S_OK;
}

STDMETHODIMP CWaveInputPin::SetDeliver(int nDeliver)
{
   CAutoLock lock_it(m_pLock);
	m_pSplit->m_nDeliver = nDeliver;
	return S_OK;
}

STDMETHODIMP CWaveInputPin::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
   if (riid == IID_IWaveSplitter)
	{
       return GetInterface((IWaveSplitter*) this, ppv);
   }
	else
	{
       return CBaseInputPin::NonDelegatingQueryInterface(riid, ppv);
   }
}



//
// NotifyAllocator
//
STDMETHODIMP
CWaveInputPin::NotifyAllocator(IMemAllocator *pAllocator, BOOL bReadOnly)
{
    CAutoLock lock_it(m_pLock);
    if (pAllocator == NULL)
        return E_FAIL;

    // Free the old allocator if any
    if (m_pSplit->m_pAllocator)
        m_pSplit->m_pAllocator->Release();

    // Store away the new allocator
    pAllocator->AddRef();
    m_pSplit->m_pAllocator = pAllocator;

    // Notify the base class about the allocator
    return CBaseInputPin::NotifyAllocator(pAllocator,bReadOnly);

} // NotifyAllocator


//
// EndOfStream
//
HRESULT CWaveInputPin::EndOfStream()
{
    CAutoLock lock_it(m_pLock);
    ASSERT(m_pSplit->m_NumOutputPins);
    HRESULT hr = NOERROR;

    // Walk through the output pins list, sending the message downstream

    int n = m_pSplit->m_NumOutputPins;
    POSITION pos = m_pSplit->m_OutputPinsList.GetHeadPosition();
    while(n)
	 {
        CWaveOutputPin *pOutputPin = m_pSplit->m_OutputPinsList.GetNext(pos);
        if (pOutputPin != NULL)
		  {
            hr = pOutputPin->DeliverEndOfStream();
            if (FAILED(hr))
                return hr;
        }
		  else
		  {
            // We should have as many pins as the count says we have
            ASSERT(FALSE);
        }
        n--;
    }
    return(NOERROR);

} // EndOfStream

//
// BeginFlush
//
HRESULT CWaveInputPin::BeginFlush()
{
    CAutoLock lock_it(m_pLock);
    ASSERT(m_pSplit->m_NumOutputPins);
    HRESULT hr = NOERROR;

    // Walk through the output pins list, sending the message downstream

    int n = m_pSplit->m_NumOutputPins;
    POSITION pos = m_pSplit->m_OutputPinsList.GetHeadPosition();
    while(n)
	 {
        CWaveOutputPin *pOutputPin = m_pSplit->m_OutputPinsList.GetNext(pos);
        if (pOutputPin != NULL)
		  {
            hr = pOutputPin->DeliverBeginFlush();
            if (FAILED(hr))
                return hr;
        }
		  else
		  {
            // We should have as many pins as the count says we have
            ASSERT(FALSE);
        }
        n--;
    }
    return CBaseInputPin::BeginFlush();

} // BeginFlush


//
// EndFlush
//
HRESULT CWaveInputPin::EndFlush()
{
    CAutoLock lock_it(m_pLock);
    ASSERT(m_pSplit->m_NumOutputPins);
    HRESULT hr = NOERROR;

    // Walk through the output pins list, sending the message downstream

    int n = m_pSplit->m_NumOutputPins;
    POSITION pos = m_pSplit->m_OutputPinsList.GetHeadPosition();
    while(n)
	 {
        CWaveOutputPin *pOutputPin = m_pSplit->m_OutputPinsList.GetNext(pos);
        if (pOutputPin != NULL)
		  {
            hr = pOutputPin->DeliverEndFlush();
            if (FAILED(hr))
                return hr;
        }
		  else
		  {
            // We should have as many pins as the count says we have
            ASSERT(FALSE);
        }
        n--;
    }
    return CBaseInputPin::EndFlush();

} // EndFlush

//
// NewSegment
//
                    
HRESULT CWaveInputPin::NewSegment(REFERENCE_TIME tStart,
                                 REFERENCE_TIME tStop,
                                 double dRate)
{
    CAutoLock lock_it(m_pLock);
    ASSERT(m_pSplit->m_NumOutputPins);
    HRESULT hr = NOERROR;

    // Walk through the output pins list, sending the message downstream

    int n = m_pSplit->m_NumOutputPins;
    POSITION pos = m_pSplit->m_OutputPinsList.GetHeadPosition();
    while(n)
	 {
        CWaveOutputPin *pOutputPin = m_pSplit->m_OutputPinsList.GetNext(pos);
        if (pOutputPin != NULL)
		  {
            hr = pOutputPin->DeliverNewSegment(tStart, tStop, dRate);
            if (FAILED(hr))
                return hr;
        }
		  else
		  {
            // We should have as many pins as the count says we have
            ASSERT(FALSE);
        }
        n--;
    }
    return CBaseInputPin::NewSegment(tStart, tStop, dRate);

} // NewSegment


//
// Receive
//
HRESULT CWaveInputPin::Receive(IMediaSample *pSample)
{
    CAutoLock lock_it(m_pLock);

    // Check that all is well with the base class
    HRESULT hr = NOERROR;
    hr = CBaseInputPin::Receive(pSample);
    if (hr != NOERROR)
        return hr;

    // Walk through the output pins list, delivering to each in turn

    int n = m_pSplit->m_NumOutputPins;
    POSITION pos = m_pSplit->m_OutputPinsList.GetHeadPosition();
    while(n)
	 {
        CWaveOutputPin *pOutputPin = m_pSplit->m_OutputPinsList.GetNext(pos);
        if (pOutputPin != NULL)
		  {
            hr = pOutputPin->Deliver(pSample);
            if (hr != NOERROR)
                return hr;
        }
		  else
		  {
            // We should have as many pins as the count says we have
            ASSERT(FALSE);
        }
        n--;
    }
    return NOERROR;

} // Receive


//
// Completed a connection to a pin
//
HRESULT CWaveInputPin::CompleteConnect(IPin *pReceivePin)
{
	int n;
	HRESULT hr = CBaseInputPin::CompleteConnect(pReceivePin);
	if (FAILED(hr)) 
	{
		return hr;
	}

	n = m_pSplit->m_NumOutputPins;
	POSITION pos = m_pSplit->m_OutputPinsList.GetHeadPosition();

	while(n)
	{
		CWaveOutputPin *pOutputPin = m_pSplit->m_OutputPinsList.GetNext(pos);
		if (pOutputPin != NULL)
		{
			// Check with downstream pin
			if (pOutputPin->m_Connected != NULL)
			{
				if (m_mt != pOutputPin->m_mt)
				{
					WAVEFORMATEX *pWFin  = (WAVEFORMATEX *)m_mt.pbFormat;
					WAVEFORMATEX *pWFout = (WAVEFORMATEX *)pOutputPin->m_mt.pbFormat;

					if ((pWFin->nBlockAlign / pWFin->nChannels) != (pWFout->nBlockAlign / pWFout->nChannels))
					{
						m_pSplit->ReconnectPin(pOutputPin, &m_mt);
					}
				}
			}
		}
		else
		{
			// We should have as many pins as the count says we have
			ASSERT(FALSE);
		}
		n--;
	}

	return S_OK;
}


//
// CWaveOutputPin constructor
//
CWaveOutputPin::CWaveOutputPin(TCHAR *pName,
                             CWaveSplitter *pSplit,
                             HRESULT *phr,
                             LPCWSTR pPinName,
                             int PinNumber) :
    CBaseOutputPin(pName, pSplit, pSplit, phr, pPinName) ,
    m_pOutputQueue(NULL),
    m_bHoldsSeek(FALSE),
    m_pPosition(NULL),
    m_pSplit(pSplit),
    m_bInsideCheckMediaType(FALSE),
	 m_nPinNumber(PinNumber),
	 m_pAllocator(NULL),
	 m_pIMemAllocator(NULL)
{
   ASSERT(pSplit);
	m_bSynchronized = false;
}



//
// CWaveOutputPin destructor
//
CWaveOutputPin::~CWaveOutputPin()
{
    ASSERT(m_pOutputQueue == NULL);
	 if (m_pIMemAllocator)
	 {
		 m_pIMemAllocator->Release();
	 }
}

//
// NonDelegatingQueryInterface
//
// This function is overwritten to expose IMediaPosition and IMediaSelection
// Note that only one output stream can be allowed to expose this to avoid
// conflicts, the other pins will just return E_NOINTERFACE and therefore
// appear as non seekable streams. We have a LONG value that if exchanged to
// produce a TRUE means that we have the honor. If it exchanges to FALSE then
// someone is already in. If we do get it and error occurs then we reset it
// to TRUE so someone else can get it.
//
STDMETHODIMP CWaveOutputPin::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
    CheckPointer(ppv,E_POINTER);
    ASSERT(ppv);
    *ppv = NULL;
    HRESULT hr = NOERROR;

    // See what interface the caller is interested in.
    if (riid == IID_IMediaPosition || riid == IID_IMediaSeeking)
	 {
        if (m_pPosition)
		  {
            if (m_bHoldsSeek == FALSE)
                return E_NOINTERFACE;
            return m_pPosition->QueryInterface(riid, ppv);
        }
    }
	 else
	 {
        return CBaseOutputPin::NonDelegatingQueryInterface(riid, ppv);
	 }
    CAutoLock lock_it(m_pLock);
    ASSERT(m_pPosition == NULL);
    IUnknown *pMediaPosition = NULL;

    // Try to create a seeking implementation
    if (InterlockedExchange(&m_pSplit->m_lCanSeek, FALSE) == FALSE)
        return E_NOINTERFACE;

    // Create implementation of this dynamically as sometimes we may never
    // try and seek. The helper object implements IMediaPosition and also
    // the IMediaSelection control interface and simply takes the calls
    // normally from the downstream filter and passes them upstream


    hr = CreatePosPassThru(GetOwner(), FALSE, (IPin *)&m_pSplit->m_Input, &pMediaPosition);

    if (pMediaPosition == NULL)
	 {
        InterlockedExchange(&m_pSplit->m_lCanSeek, TRUE);
        return E_OUTOFMEMORY;
    }

    if (FAILED(hr))
	 {
        InterlockedExchange(&m_pSplit->m_lCanSeek, TRUE);
        pMediaPosition->Release ();
        return hr;
    }

    m_pPosition = pMediaPosition;
	 
    m_bHoldsSeek = TRUE;
    return NonDelegatingQueryInterface(riid, ppv);

} // NonDelegatingQueryInterface


//
// NonDelegatingAddRef
//
// We need override this method so that we can do proper reference counting
// on our output pin. The base class CBasePin does not do any reference
// counting on the pin in RETAIL.
//
// Please refer to the comments for the NonDelegatingRelease method for more
// info on why we need to do this.
//
STDMETHODIMP_(ULONG) CWaveOutputPin::NonDelegatingAddRef()
{
/*
    CAutoLock lock_it(m_pLock);

#ifdef DEBUG
    // Update the debug only variable maintained by the base class
    m_cRef++;
    ASSERT(m_cRef > 0);
#endif

    // Now update our reference count
    m_cOurRef++;
    ASSERT(m_cOurRef > 0);
    return m_cOurRef;
*/
	return 1;
} // NonDelegatingAddRef


//
// NonDelegatingRelease
//
// CWaveOutputPin overrides this class so that we can take the pin out of our
// output pins list and delete it when its reference count drops to 1 and there
// is atleast two free pins.
//
// Note that CreateNextOutputPin holds a reference count on the pin so that
// when the count drops to 1, we know that no one else has the pin.
//
// Moreover, the pin that we are about to delete must be a free pin(or else
// the reference would not have dropped to 1, and we must have atleast one
// other free pin(as the filter always wants to have one more free pin)
//
// Also, since CBasePin::NonDelegatingAddRef passes the call to the owning
// filter, we will have to call Release on the owning filter as well.
//
// Also, note that we maintain our own reference count m_cOurRef as the m_cRef
// variable maintained by CBasePin is debug only.
//
STDMETHODIMP_(ULONG) CWaveOutputPin::NonDelegatingRelease()
{
	/*
    CAutoLock lock_it(m_pLock);

#ifdef DEBUG
    // Update the debug only variable in CBasePin
    m_cRef--;
    ASSERT(m_cRef >= 0);
#endif

    // Now update our reference count
    m_cOurRef--;
    ASSERT(m_cOurRef >= 0);

    // if the reference count on the object has gone to one, remove
    // the pin from our output pins list and physically delete it
    // provided there are atealst two free pins in the list(including
    // this one)

    // Also, when the ref count drops to 0, it really means that our
    // filter that is holding one ref count has released it so we
    // should delete the pin as well.

    if (m_cOurRef <= 1)
	 {
        int n = 2;                     // default forces pin deletion
        if (m_cOurRef == 1)
		  {
            // Walk the list of pins, looking for count of free pins
            n = m_pSplit->GetNumFreePins();
        }

        // If there are two free pins, delete this one.
        // NOTE: normall

        if ((n >= 2 ) && (m_pSplit->m_NumOutputPins == m_nPinNumber))
		  {
            m_cOurRef = 0;
#ifdef DEBUG
            m_cRef = 0;
#endif
				
					m_pSplit->DeleteOutputPin(this);
            return(ULONG) 0;
        }

    }
    return(ULONG) m_cOurRef;
	 */
	return 1;

} // NonDelegatingRelease


//
// DecideBufferSize
//
// This has to be present to override the PURE virtual class base function
//
HRESULT CWaveOutputPin::DecideBufferSize(IMemAllocator *pMemAllocator,
                                        ALLOCATOR_PROPERTIES * ppropInputRequest)
{
    return NOERROR;

} // DecideBufferSize


//
// DecideAllocator
//
HRESULT CWaveOutputPin::DecideAllocator(IMemInputPin *pPin, IMemAllocator **ppAlloc)
{
    ASSERT(m_pSplit->m_pAllocator != NULL);
    *ppAlloc = NULL;

    // Tell the pin about our allocator, set by the input pin.
    HRESULT hr = NOERROR;
    hr = pPin->NotifyAllocator(m_pSplit->m_pAllocator,TRUE);
    if (FAILED(hr))
        return hr;

    // Return the allocator
    *ppAlloc = m_pSplit->m_pAllocator;
    m_pSplit->m_pAllocator->AddRef();
    return NOERROR;

} // DecideAllocator


//
// CheckMediaType
//
HRESULT CWaveOutputPin::CheckMediaType(const CMediaType *pmt)
{
	CAutoLock lock_it(m_pLock);

	// If we are already inside checkmedia type for this pin, return NOERROR
	// It is possble to hookup two of the splitter filters and some other filter
	// like the video effects sample to get into this situation. If we
	// do not detect this, we will loop till we blow the stack

	if (m_bInsideCheckMediaType == TRUE)
		return NOERROR;

	m_bInsideCheckMediaType = TRUE;
	HRESULT hr = NOERROR;

#ifdef DEBUG
    // Display the type of the media for debugging purposes
//    DisplayMediaType(TEXT("Output Pin Checking"), pmt);
#endif

	// The input needs to have been conneced first
	if (m_pSplit->m_Input.m_Connected == NULL)
	{
		m_bInsideCheckMediaType = FALSE;
		return VFW_E_NOT_CONNECTED;
	}

	// Make sure that our input pin peer is happy with this
	hr = m_pSplit->m_Input.m_Connected->QueryAccept(pmt);
	if (hr != NOERROR)
	{
		m_bInsideCheckMediaType = FALSE;
		return VFW_E_TYPE_NOT_ACCEPTED;
	}

	WAVEFORMATEX *pWFout = (WAVEFORMATEX *)pmt->Format();
	if (pWFout && m_pSplit->m_Input.m_mt.pbFormat)
	{
		WAVEFORMATEX *pWFin = (WAVEFORMATEX *)m_pSplit->m_Input.m_mt.pbFormat;
		if ((pWFin->nBlockAlign / pWFin->nChannels) != (pWFout->nBlockAlign / pWFout->nChannels))
		{
			ASSERT(FALSE);
			return VFW_E_TYPE_NOT_ACCEPTED;
		}
	}

	// Check the format with the other outpin pins

	int n = m_pSplit->m_NumOutputPins;
	POSITION pos = m_pSplit->m_OutputPinsList.GetHeadPosition();

	while(n)
	{
		CWaveOutputPin *pOutputPin = m_pSplit->m_OutputPinsList.GetNext(pos);
		if (pOutputPin != NULL && pOutputPin != this)
		{
			if (pOutputPin->m_Connected != NULL)
			{
				 // The pin is connected, check its peer
				 hr = pOutputPin->m_Connected->QueryAccept(pmt);
				 if (hr != NOERROR)
				 {
					  m_bInsideCheckMediaType = FALSE;
					  return VFW_E_TYPE_NOT_ACCEPTED;
				 }
			}
		}
		n--;
	}

	m_bInsideCheckMediaType = FALSE;
	return NOERROR;

} // CheckMediaType


//
// EnumMediaTypes
//
STDMETHODIMP CWaveOutputPin::EnumMediaTypes(IEnumMediaTypes **ppEnum)
{
   CAutoLock lock_it(m_pLock);
   ASSERT(ppEnum);

    // Make sure that we are connected
	if (m_pSplit->m_Input.m_Connected == NULL)
		return VFW_E_NOT_CONNECTED;

   // We will simply return the enumerator of our input pin's peer
   HRESULT hr = m_pSplit->m_Input.m_Connected->EnumMediaTypes(ppEnum);
	return hr;
} // EnumMediaTypes


//
// SetMediaType
//
HRESULT CWaveOutputPin::SetMediaType(const CMediaType *pmt)
{
	CAutoLock lock_it(m_pLock);

#ifdef DEBUG
	// Display the format of the media for debugging purposes
	DisplayMediaType(TEXT("Output pin type agreed"), pmt);
#endif
	// Make sure that we have an input connected
	if (m_pSplit->m_Input.m_Connected == NULL)
	   return VFW_E_NOT_CONNECTED;
	
	WAVEFORMATEX *pWFout = NULL;
	if (*pmt->FormatType() == FORMAT_WaveFormatEx)
	{
		pWFout = (WAVEFORMATEX *) pmt->Format();
		if (pWFout && pWFout->nChannels > 1)
		{
			pWFout->nBlockAlign     /= pWFout->nChannels;
			pWFout->nAvgBytesPerSec /= pWFout->nChannels;
			pWFout->nChannels       /= pWFout->nChannels;
		}
	}

	HRESULT hr = NOERROR;
	hr = CBaseOutputPin::SetMediaType(pmt);

	if (FAILED(hr))
	  return hr;

	return NOERROR;

} // SetMediaType


//
// CompleteConnect
//
HRESULT CWaveOutputPin::CompleteConnect(IPin *pReceivePin)
{
	CAutoLock lock_it(m_pLock);
	ASSERT(m_Connected == pReceivePin);
	HRESULT hr = NOERROR;

	hr = CBaseOutputPin::CompleteConnect(pReceivePin);
	if (FAILED(hr))
		return hr;

	// If the type is not the same as that stored for the input
	// pin then force the input pins peer to be reconnected

	if (m_mt != m_pSplit->m_Input.m_mt)
	{
		WAVEFORMATEX *pWFin  = (WAVEFORMATEX *)m_pSplit->m_Input.m_mt.pbFormat;
		WAVEFORMATEX *pWFout = (WAVEFORMATEX *)m_mt.pbFormat;

		if ((pWFin->nBlockAlign / pWFin->nChannels) != (pWFout->nBlockAlign / pWFout->nChannels))
		{
			hr = m_pSplit->ReconnectPin(m_pSplit->m_Input.m_Connected, &m_pSplit->m_Input.m_mt);
			if(FAILED(hr))
			{
				return hr;
			}
		}
	}

	// Since this pin has been connected up, create another output pin. We
	// will do this only if there are no unconnected pins on us. However
	// CompleteConnect will get called for the same pin during reconnection
/*
	int n = m_pSplit->GetNumFreePins();
	ASSERT(n <= 1);
	if (n == 1 || m_pSplit->m_NumOutputPins == WAVESPLITTER_MAX_PINS)
		return NOERROR;
*/
	WAVEFORMATEX *pWF = (WAVEFORMATEX *) m_pSplit->m_Input.m_mt.pbFormat;
	if (!pWF)
	{
		return NOERROR;
	}
	if (m_pSplit->m_NumOutputPins >= pWF->nChannels)
	{
		return NOERROR;
	}
	// No unconnected pins left so spawn a new one

	CWaveOutputPin *pOutputPin = m_pSplit->CreateNextOutputPin(m_pSplit);
	if (pOutputPin != NULL )
	{
		m_pSplit->m_NumOutputPins++;
		m_pSplit->m_OutputPinsList.AddTail(pOutputPin);
		m_pSplit->IncrementPinVersion();
	}

	// At this point we should be able to send some
	// notification that we have sprung a new pin

	return NOERROR;

} // CompleteConnect


//
// Active
//
// This is called when we start running or go paused. We create the
// output queue object to send data to our associated peer pin
//
HRESULT CWaveOutputPin::Active()
{
	CAutoLock lock_it(m_pLock);
	HRESULT hr = NOERROR;

	// Make sure that the pin is connected
	if (m_Connected == NULL)
	   return NOERROR;

	// Create the output queue if we have to
	if (m_pOutputQueue == NULL)
	{
		m_pOutputQueue = new COutputQueue(m_Connected, &hr, TRUE, FALSE);
		if (m_pOutputQueue == NULL)
			return E_OUTOFMEMORY;

		// Make sure that the constructor did not return any error
		if (FAILED(hr))
		{
			delete m_pOutputQueue;
			m_pOutputQueue = NULL;
			return hr;
		}
	}

	// Pass the call on to the base class
	CBaseOutputPin::Active();
	return NOERROR;

} // Active


//
// Inactive
//
// This is called when we stop streaming
// We delete the output queue at this time
//
HRESULT CWaveOutputPin::Inactive()
{
	CAutoLock lock_it(m_pLock);

	// Delete the output queus associated with the pin.
	if (m_pOutputQueue)
	{
	  delete m_pOutputQueue;
	  m_pOutputQueue = NULL;
	}

	if (m_pAllocator)
	{
		delete m_pAllocator;
		m_pAllocator = NULL;
	}
	CBaseOutputPin::Inactive();
	return NOERROR;

} // Inactive


//
// Deliver
//
HRESULT CWaveOutputPin::Deliver(IMediaSample *pMediaSample)
{
	HRESULT hr = NULL;
	// Make sure that we have an output queue
	if (m_pOutputQueue == NULL)
	  return NOERROR;
	
	BYTE *pDest, *pSource;
	long i, j, lLength;
	bool bDeliver = false;
//	lLength = pMediaSample->GetActualDataLength();
	lLength = pMediaSample->GetSize();
	REFERENCE_TIME rtStart = 0, rtEnd;
	if (m_pSplit->m_pCheckFnc)					// external check function for the sample data
	{
		m_pSplit->m_pCheckFnc(pMediaSample, &m_pSplit->m_Input.m_mt, m_nPinNumber, m_pSplit->m_lParamCheckFnc);
	}

	WAVEFORMATEX *pWFin  = (WAVEFORMATEX *) m_pSplit->m_Input.m_mt.pbFormat;
	WAVEFORMATEX *pWFout = (WAVEFORMATEX *) m_mt.pbFormat;

	pMediaSample->GetPointer(&pSource);
	if (m_pSplit->m_nThresholdLevel > 0)		// determine the threshold level
	{
		if (!m_pSplit->m_bActivated)				// only if it is not active
		{
			if (pWFin->wFormatTag == WAVE_FORMAT_PCM) // WAVE PCM
			{
				int nType = pWFin->nBlockAlign / pWFin->nChannels;
				if (nType == 2)						// 16 bit samples
				{
					short nThreshold = MulDiv(32767, m_pSplit->m_nThresholdLevel, 10000);
					for (i= (m_nPinNumber-1) * nType; i<lLength; i+=pWFin->nBlockAlign)
					{
						short nLevel = *((short*)&pSource[i]);
						if (( nLevel > nThreshold) ||
							 (-nLevel > nThreshold))
						{
							pMediaSample->GetTime(&rtStart, &rtEnd);
							m_pSplit->NotifyEvent(EC_ACTIVATE_TRANSPORT, (long)&rtStart, MulDiv(nLevel, 100, 32767));
							m_pSplit->m_bActivated = true;
							pMediaSample->SetSyncPoint(TRUE);
							CWaveSplitter::Trace(_T("SynchPoint"));
							break;
						}
					}
				}
				else if (nType == 1)					// 8 bit samples
				{
					short nThreshold = MulDiv(127, m_pSplit->m_nThresholdLevel, 10000);
					for (i= (m_nPinNumber-1); i<lLength; i+=pWFin->nBlockAlign)
					{
						signed char nLevel = (signed char)(pSource[i]-127);
						if (( nLevel > nThreshold) ||
							 (-nLevel > nThreshold))
						{
							pMediaSample->GetTime(&rtStart, &rtEnd);
							m_pSplit->NotifyEvent(EC_ACTIVATE_TRANSPORT, (long)&rtStart, MulDiv(nLevel, 100, 127));
							m_pSplit->m_bActivated = true;
							pMediaSample->SetSyncPoint(TRUE);
							CWaveSplitter::Trace(_T("SynchPoint"));
							break;
						}
					}
				}
			}
		}
	}

	if (m_pSplit->m_nDeliver == WAVESPLIT_NEVER)			// deliver never
	{
		return NOERROR;
	}
	else if (m_pSplit->m_nDeliver == WAVESPLIT_ALWAYS)	// deliver all samples
	{
		bDeliver = true;
		if (!m_bSynchronized)									// not synchronized
		{
			pMediaSample->SetSyncPoint(TRUE);				// set synch point
			m_bSynchronized = true;								// set synch flag
			CWaveSplitter::Trace(_T("SynchPoint DA"));
		}
	}
	else if (m_pSplit->m_bActivated)							// deliver when active
	{
		bDeliver = true;
	}

	if (pWFin->nChannels != pWFout->nChannels)
	{
		if (pWFout->nChannels != 1)				// only for one output channel
		{
			return VFW_E_TYPE_NOT_ACCEPTED;
		}
		if (m_nPinNumber > pWFin->nChannels)	// pin number must not be higher
		{
			return VFW_E_TYPE_NOT_ACCEPTED;
		}

		IMediaSample  *pNewSample = NULL;
		ALLOCATOR_PROPERTIES ap, apr;
		if ((m_pAllocator == NULL) && bDeliver)
		{
			if (m_pIMemAllocator == NULL)			// create the new allocator for the output pin
			{
				CreateMemoryAllocator(&m_pIMemAllocator);
			}
			m_pAllocator = new CMemAllocator("Output", m_pIMemAllocator, &hr);
			if (FAILED(hr))
			{
				delete m_pAllocator;
				m_pAllocator = NULL;
				return hr;
			}
															// determine the allocator properties
			int nSamples = lLength / pWFin->nBlockAlign;
			if (pWFin->wFormatTag == WAVE_FORMAT_MPEGLAYER3)
			{
				ap.cbAlign  = 1;
				ap.cbBuffer = nSamples / pWFin->nChannels;
				ap.cBuffers = 1;
				ap.cbPrefix = 0;
			}
			else if (pWFin->wFormatTag == WAVE_FORMAT_PCM)
			{
				ap.cbAlign  = pWFout->nBlockAlign;
				ap.cbBuffer = pWFout->nBlockAlign * nSamples;
				ap.cBuffers = pWFin->nChannels;
				ap.cbPrefix = 0;
			}
			m_pAllocator->SetProperties(&ap, &apr);// set properties
			m_pAllocator->Commit();						// and commit
		}
		
		if (m_pAllocator && bDeliver)// get the sample buffer
		{
			hr = m_pAllocator->GetBuffer(&pNewSample, NULL, NULL, 0);
		}

		if (pNewSample)
		{
			pNewSample->GetPointer(&pDest);		// get the data buffer
			if (pDest && pSource)
			{
				if (pWFin->nChannels > pWFout->nChannels)
				{
					if (pWFin->wFormatTag == WAVE_FORMAT_MPEGLAYER3)
					{
						i= (m_nPinNumber-1);
/*
						MP3header *pMp3 = (MP3header*)pSource;

						MPEGLAYER3WAVEFORMAT *pWFmp3 = (MPEGLAYER3WAVEFORMAT*)pWFin;

						for (j=0; j<lLength-4; j++)
						{
							pMp3 = (MP3header*)&pSource[j];
							if ((pMp3->framesync         == 0x07ff) && 
								 (pMp3->MpegAudioVesionID !=    0x1) &&
								 (pMp3->LayerDescription  ==    0x1) &&
								 (pMp3->SampleRateIndex   !=    0x3) &&
								 (pMp3->Emphasis          !=    0x2) &&
								 (pMp3->BitrateIndex      !=    0xf))
							{
								break;
							}
						}
						for (j=0; i<lLength; i+=pWFin->nChannels, j+=pWFout->nChannels)
						{
							pDest[j] = pSource[i];
						}
*/						
						return VFW_E_TYPE_NOT_ACCEPTED;
					}
					else if (pWFin->wFormatTag == WAVE_FORMAT_PCM) // WAVE PCM
					{
						int nType = pWFin->nBlockAlign / pWFin->nChannels;
						if (nType == 2)				// and split it into single channels
						{
							i= (m_nPinNumber-1) * nType;
							for (j=0; i<lLength; i+=pWFin->nBlockAlign, j+=pWFout->nBlockAlign)
							{
								*((short*)&pDest[j]) = *((short*)&pSource[i]);
							}
						}
						else if (nType == 1)
						{
							i= (m_nPinNumber-1);
							for (j=0; i<lLength; i+=pWFin->nBlockAlign, j+=pWFout->nBlockAlign)
							{
								pDest[j] = pSource[i];
							}
						}
					}
					else
					{
						pNewSample->Release();
						return VFW_E_TYPE_NOT_ACCEPTED;
					}
				}
				else
				{
					pNewSample->Release();
					return VFW_E_TYPE_NOT_ACCEPTED;
				}
				REFERENCE_TIME rtStart, rtEnd;
				pMediaSample->GetTime(&rtStart, &rtEnd);
				pNewSample->SetTime(&rtStart, &rtEnd);
				pNewSample->SetPreroll(pMediaSample->IsPreroll() == S_OK);
				pNewSample->SetDiscontinuity(pMediaSample->IsDiscontinuity() == S_OK);
				pNewSample->SetSyncPoint(pMediaSample->IsSyncPoint() == S_OK);
				return m_pOutputQueue->Receive(pNewSample);
			}
			else
			{
				pNewSample->Release();
			}
		}
	}
	else if (bDeliver)
	{
		pMediaSample->AddRef();
		return m_pOutputQueue->Receive(pMediaSample);
	}

	return NOERROR;
} // Deliver


//
// DeliverEndOfStream
//
HRESULT CWaveOutputPin::DeliverEndOfStream()
{
    // Make sure that we have an output queue
    if (m_pOutputQueue == NULL)
        return NOERROR;

    m_pOutputQueue->EOS();
    return NOERROR;

} // DeliverEndOfStream


//
// DeliverBeginFlush
//
HRESULT CWaveOutputPin::DeliverBeginFlush()
{
    // Make sure that we have an output queue
    if (m_pOutputQueue == NULL)
        return NOERROR;

    m_pOutputQueue->BeginFlush();
    return NOERROR;

} // DeliverBeginFlush


//
// DeliverEndFlush
//
HRESULT CWaveOutputPin::DeliverEndFlush()
{
    // Make sure that we have an output queue
    if (m_pOutputQueue == NULL)
        return NOERROR;

    m_pOutputQueue->EndFlush();
    return NOERROR;

} // DeliverEndFlish

//
// DeliverNewSegment
//
HRESULT CWaveOutputPin::DeliverNewSegment(REFERENCE_TIME tStart, 
                                         REFERENCE_TIME tStop,  
                                         double dRate)          
{
    // Make sure that we have an output queue
    if (m_pOutputQueue == NULL)
        return NOERROR;

    m_pOutputQueue->NewSegment(tStart, tStop, dRate);
    return NOERROR;

} // DeliverNewSegment


//
// Notify
//
STDMETHODIMP CWaveOutputPin::Notify(IBaseFilter *pSender, Quality q)
{
	// We pass the message on, which means that we find the quality sink
	// for our input pin and send it there

	POSITION pos = m_pSplit->m_OutputPinsList.GetHeadPosition();
	CWaveOutputPin *pFirstOutput = m_pSplit->m_OutputPinsList.GetNext(pos);

	if (this == pFirstOutput)
	{
		if (m_pSplit->m_Input.m_pQSink!=NULL)
		{
			return m_pSplit->m_Input.m_pQSink->Notify(m_pSplit, q);
		}
		else
		{
			// No sink set, so pass it upstream
			HRESULT hr;
			IQualityControl * pIQC;

			hr = VFW_E_NOT_FOUND;
			if (m_pSplit->m_Input.m_Connected)
			{
				m_pSplit->m_Input.m_Connected->QueryInterface(IID_IQualityControl,(void**)&pIQC);

				if (pIQC!=NULL)
				{
					hr = pIQC->Notify(m_pSplit, q);
					pIQC->Release();
				}
			}
			return hr;
		}
	}

	// Quality management is too hard to do
	return NOERROR;

} // Notify


//
// DllRegisterServer
//
STDAPI DllRegisterServer()
{
    return AMovieDllRegisterServer2( TRUE );
}


//
// DllUnregisterServer
//
STDAPI
DllUnregisterServer()
{
    return AMovieDllRegisterServer2( FALSE );
}
