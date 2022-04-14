//------------------------------------------------------------------------------
// File: Synth.cpp
//
// Desc: DirectShow sample code - implements an audio signal generator
//       source filter.
//
// Copyright (c) 1992-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------


#include <windows.h>
#include <streams.h>
#include <math.h>
#include <mmreg.h>
#include <msacm.h>

#include <initguid.h>
#if (1100 > _MSC_VER)
#include <olectlid.h>
#else
#include <olectl.h>
#endif


#define _AUDIOSYNTH_IMPLEMENTATION_

#include "DynSrc.h"
#include "IMediaSource.h"
#include "synth.h"
#include "synthprp.h"

// setup data

const AMOVIESETUP_MEDIATYPE sudOpPinTypes =
{ &MEDIATYPE_NULL      // clsMajorType
, &MEDIASUBTYPE_NULL }; // clsMinorType

const AMOVIESETUP_PIN sudOpPin =
{ L"Output"          // strName
, FALSE              // bRendered
, TRUE               // bOutput
, FALSE              // bZero
, FALSE              // bMany
, &CLSID_NULL        // clsConnectsToFilter
, L"Input"           // strConnectsToPin
, 1                  // nTypes
, &sudOpPinTypes };  // lpTypes

const AMOVIESETUP_FILTER sudSynth =
{ &CLSID_MediaSourceFilter // clsID
, L"Media Source"		// strName
, MERIT_UNLIKELY     // dwMerit
, 1                  // nPins
, &sudOpPin };       // lpPin

// -------------------------------------------------------------------------
// g_Templates
// -------------------------------------------------------------------------
// COM global table of objects in this dll

CFactoryTemplate g_Templates[] =
{
	{
		L"Media Source"
		, &CLSID_MediaSourceFilter
		, CMediaSourceFilter::CreateInstance
		, NULL
		, &sudSynth 
	},
	{
		L"Audio Synthesizer Property Page"
		, &CLSID_SynthPropertyPage
		, CSynthProperties::CreateInstance 
	}

};
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);

// There are 8 bits in a byte.
const DWORD BITS_PER_BYTE = 8;

// -------------------------------------------------------------------------
// CMediaSourceFilter, the main filter object
// -------------------------------------------------------------------------
//
// CreateInstance
//
// The only allowed way to create Synthesizers

CUnknown * WINAPI CMediaSourceFilter::CreateInstance(LPUNKNOWN lpunk, HRESULT *phr) {

    CUnknown *punk = new CMediaSourceFilter(lpunk, phr);
    if (punk == NULL) 
	 {
        *phr = E_OUTOFMEMORY;
    }

    return punk;
}

//
// CMediaSourceFilter::Constructor
//
// initialise a CMediaSourceStream object so that we have a pin.

CMediaSourceFilter::CMediaSourceFilter(LPUNKNOWN lpunk, HRESULT *phr)
    : CDynamicSource(NAME("Audio Synthesizer Filter"),lpunk, CLSID_MediaSourceFilter, phr)
    , CPersistStream(lpunk, phr)
	 , m_pDirectAllocator(NULL)
	 , m_pSampleProperties(NULL)
	 , m_nBufferType(0)
	 , m_pReleaseBufferFunction(NULL)
	 , m_BufferList("Buffers")
	 , m_SentBufferList("Sent Buffers")
	 , m_bReCommit(false)
{
    m_paStreams    = (CDynamicSourceStream **) new CMediaSourceStream*[1];
    if (m_paStreams == NULL)
	 {
        *phr = E_OUTOFMEMORY;
        return;
    }

    m_paStreams[0] = new CMediaSourceStream(phr, this, L"Media Source Stream");
    if (m_paStreams[0] == NULL)
	 {
        *phr = E_OUTOFMEMORY;
        return;
    }

    if (SUCCEEDED(*phr))
	 {
        ASSERT(m_Synth);
        m_Synth->put_SynthFormat(1,     // Channels
                                 16,    // Bits Per Sample
                                 44100  // Samples Per Sececond
                                 );
    }
//	 ZeroMemory(&m_MediaType, sizeof(m_MediaType));
}

MediaSampleProperties* CMediaSourceFilter::FindMSP(void *pBuffer, bool bRemove)
{
	MediaSampleProperties *pMSP;
	POSITION posOld, pos = m_SentBufferList.GetHeadPosition();
	while (pos)
	{
		posOld = pos;
		pMSP = m_SentBufferList.GetNext(pos);
		if (pMSP->pBuffer == pBuffer)
		{
			if (bRemove) m_SentBufferList.Remove(posOld);
			return pMSP;
		}
	}
	pos = m_BufferList.GetHeadPosition();
	while (pos)
	{
		posOld = pos;
		pMSP = m_BufferList.GetNext(pos);
		if (pMSP->pBuffer == pBuffer)
		{
			if (bRemove) m_BufferList.Remove(posOld);
			return pMSP;
		}
	}
	return NULL;
}

//
// CMediaSourceFilter::Destructor
//
CMediaSourceFilter::~CMediaSourceFilter(void)
{
	ReleaseDirectAllocator();
	ReleaseAllBuffers();
	if (m_pSampleProperties)
	{
		delete m_pSampleProperties;
	}
}

void CMediaSourceFilter::ReleaseAllBuffers()
{
	MediaSampleProperties *pMSP;
	while (m_SentBufferList.GetCount())
	{
		pMSP = m_SentBufferList.GetHead();
		ReleaseMediaBuffer(pMSP->pBuffer);
	}
	while (m_BufferList.GetCount())
	{
		pMSP = m_BufferList.GetHead();
		ReleaseMediaBuffer(pMSP->pBuffer);
	}
}
//
// NonDelegatingQueryInterface
//
// Reveal our property page, persistance, and control interfaces
STDMETHODIMP CMediaSourceFilter::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
    if (riid == IID_IMediaSource)
	 {
        return GetInterface((IMediaSource *) this, ppv);
    }
    else if (riid == IID_IPersistStream)
	 {
        return GetInterface((IPersistStream *) this, ppv);
    }
    else if (riid == IID_ISpecifyPropertyPages)
	 {
        return GetInterface((ISpecifyPropertyPages *) this, ppv);
    }
	 else
	 {
        return CDynamicSource::NonDelegatingQueryInterface(riid, ppv);
    }
}

// --- IMediaFilter Interface
STDMETHODIMP CMediaSourceFilter::SetSyncSource(IReferenceClock *pClock)
{
	for (int i=0; i<m_iPins; i++)
	{
		((CMediaSourceStream*)m_paStreams[i])->SetSyncSource(pClock);
	}
   return CDynamicSource::SetSyncSource(pClock);
}

// --- IBaseFilter Interface
STDMETHODIMP CMediaSourceFilter::JoinFilterGraph(IFilterGraph * pGraph, LPCWSTR pName)
{
	HRESULT hr = CDynamicSource::JoinFilterGraph(pGraph, pName);
	if (SUCCEEDED(hr))
	{
		for (int i=0; i<m_iPins; i++)
		{
			((CMediaSourceStream*)m_paStreams[i])->SetFilterGraph(m_pSink);
		}
		OutputDebugStr("SetFilterGraph\n");
	}
	return hr;
}

STDMETHODIMP CMediaSourceFilter::Run(REFERENCE_TIME tStart)
{
   /* Do other things needed by the filter ... */
	for (int i=0; i<m_iPins; i++)
	{
		((CMediaSourceStream*)m_paStreams[i])->NotifyFilterState(State_Running, tStart);
	}
   return CDynamicSource::Run(tStart); // Call the filter base class.
}

STDMETHODIMP CMediaSourceFilter::Pause()
{
   /* Do other things needed by the filter ... */
	for (int i=0; i<m_iPins; i++)
	{
		((CMediaSourceStream*)m_paStreams[i])->NotifyFilterState(State_Paused);
	}
   return CDynamicSource::Pause();
}

STDMETHODIMP CMediaSourceFilter::Stop()
{
   /* Do other things needed by the filter ... */
 	for (int i=0; i<m_iPins; i++)
	{
		((CMediaSourceStream*)m_paStreams[i])->NotifyFilterState(State_Stopped);
	}
   return CDynamicSource::Stop();
}


STDMETHODIMP CMediaSourceFilter::GetState(DWORD dw, FILTER_STATE *State)
{
    *State = m_State;
    if (m_State == State_Paused)
        return VFW_S_CANT_CUE;
    else
        return S_OK;
}

//
// GetPages
//
STDMETHODIMP CMediaSourceFilter::GetPages(CAUUID * pPages)
{

    pPages->cElems = 1;
    pPages->pElems = (GUID *) CoTaskMemAlloc(sizeof(GUID));
    if (pPages->pElems == NULL)
	 {
        return E_OUTOFMEMORY;
    }
    *(pPages->pElems) = CLSID_SynthPropertyPage;

    return NOERROR;

}

// -------------------------------------------------------------------------
// --- IPersistStream ---
// -------------------------------------------------------------------------

#define WRITEOUT(var)   hr = pStream->Write(&var, sizeof(var), NULL); \
                        if (FAILED(hr)) return hr;

#define READIN(var)     hr = pStream->Read(&var, sizeof(var), NULL); \
                        if (FAILED(hr)) return hr;

STDMETHODIMP CMediaSourceFilter::GetClassID(CLSID *pClsid)
{
    return CBaseFilter::GetClassID(pClsid);
}

int CMediaSourceFilter::SizeMax ()
{
    return sizeof (int) * 8;
}

HRESULT CMediaSourceFilter::WriteToStream(IStream *pStream)
{
    HRESULT hr;
    int i, k;

    get_Frequency (&i);  
    WRITEOUT(i);
    get_Waveform (&i);
    WRITEOUT(i);
    get_Channels (&i);
    WRITEOUT(i);
    get_BitsPerSample (&i);
    WRITEOUT(i);
    get_SamplesPerSec (&i);
    WRITEOUT(i);
    get_Amplitude (&i);
    WRITEOUT(i);
    get_SweepRange (&i, &k);
    WRITEOUT(i);
    WRITEOUT(k);
    get_OutputFormat((SYNTH_OUTPUT_FORMAT*)&i);
    WRITEOUT(i);

    return hr;
}


HRESULT CMediaSourceFilter::ReadFromStream(IStream *pStream)
{
    if (GetSoftwareVersion() != mPS_dwFileVersion)
	 {
        return E_FAIL;
    }

    HRESULT hr;
    int i, k;

    READIN(i);
    put_Frequency(i);
    READIN(i);
    put_Waveform (i);
    READIN(i);
    put_Channels (i);
    READIN(i);
    put_BitsPerSample (i);
    READIN(i);
    put_SamplesPerSec (i);
    READIN(i);
    put_Amplitude (i);
    READIN(i);
    READIN(k);
    put_SweepRange (i, k);
    READIN(i);
    put_OutputFormat((SYNTH_OUTPUT_FORMAT)i);

    return hr;
}

DWORD CMediaSourceFilter::GetSoftwareVersion(void)
{
    return 1;
}

HRESULT CMediaSourceFilter::CreateDirectAllocator()
{
	HRESULT hr = NOERROR;
	if (m_pDirectAllocator == NULL)
	{
		m_pDirectAllocator = new CDirectBufferAllocater(this, &hr);
		if (FAILED(hr)) return hr;
		m_pDirectAllocator->AddRef();
		if (!m_pDirectAllocator) return E_OUTOFMEMORY;
		m_bReCommit = true;
	}
	
	ALLOCATOR_PROPERTIES Actual;
	m_pDirectAllocator->SetProperties(&m_allocprops, &Actual);
/*
	if (m_bReCommit)
	{
		hr = RecommitDirectAllocator();
	}
*/
	return hr;
}

void CMediaSourceFilter::ReleaseDirectAllocator()
{
	if (m_pDirectAllocator)
	{
		m_pDirectAllocator->Release();
		m_pDirectAllocator = NULL;
	}
}

HRESULT CMediaSourceFilter::RecommitDirectAllocator()
{
	HRESULT hr = NOERROR;
	if (!m_pDirectAllocator) return E_POINTER;
	if (m_pDirectAllocator->IsCommitted())
	{
		m_pDirectAllocator->Decommit();
	}
	
	ALLOCATOR_PROPERTIES Actual;
	hr = m_pDirectAllocator->SetProperties(&m_allocprops, &Actual);
	if (FAILED(hr)) return hr;

	if (Actual.cbBuffer < m_allocprops.cbBuffer) return E_FAIL;

	hr = m_pDirectAllocator->Commit();
	if (FAILED(hr)) return hr;
	
	m_bReCommit = false;
	return hr;
}


HRESULT CMediaSourceFilter::GetDeliveryBuffer(IMediaSample ** ppSample, REFERENCE_TIME * pStartTime, REFERENCE_TIME * pEndTime, DWORD dwFlags)
{
	HRESULT                hr   = NOERROR;
	MediaSampleProperties *pMSP = NULL;
	if (!m_pDirectAllocator)								// no direct buffer allocator
	{
		if (!m_paStreams   ) return E_POINTER;
		if (!m_paStreams[0]) return E_POINTER;
		BYTE *pData = NULL;

		switch (m_nBufferType)
		{
			case BUFFER_FIXED:								// fixed buffer
			if (m_allocprops.cBuffers > 0)				// if there is a buffer
			{														// get the buffer from the allocator
				hr = m_paStreams[0]->CDynamicOutputPin::GetDeliveryBuffer(ppSample, pStartTime, pEndTime, dwFlags);
				if (SUCCEEDED(hr))
				{
					hr = (*ppSample)->GetPointer(&pData);// get the data pointer
				}
				if (SUCCEEDED(hr))
				{
					pMSP = m_pSampleProperties;
					m_allocprops.cBuffers--;				// decrement No. of buffers
					if (pData && pMSP->pBuffer)
					{
						if (m_allocprops.cbPrefix)			// if there is a header prefix
						{												
							pData -= m_allocprops.cbPrefix;// correct the offset
						}
						CopyMemory(pData, pMSP->pBuffer, (*ppSample)->GetSize());// copy the data
					}
				}break;
			}
			else
			{
				hr = VFW_E_SAMPLE_REJECTED_EOS;
			}break;
			case BUFFER_ATTACH:								// attached buffer or
			case BUFFER_COPY:									// copied buffer
			if (m_BufferList.GetCount())					// if there is a buffer
			{														// get the buffer from the allocator
				hr = m_paStreams[0]->CDynamicOutputPin::GetDeliveryBuffer(ppSample, pStartTime, pEndTime, dwFlags);
				if (SUCCEEDED(hr))
				{
					hr = (*ppSample)->GetPointer(&pData);// get the data pointer
				}
				pMSP = m_BufferList.RemoveTail();		// remove the buffer from the list
				if (!pMSP) return E_POINTER;
				m_SentBufferList.AddHead(pMSP);			// add to the sent buffers list
				if (pData && pMSP->pBuffer)
				{
					if (m_allocprops.cbPrefix)				// if there is a header prefix
					{
						pData -= m_allocprops.cbPrefix;	// correct the offset
					}
					CopyMemory(pData, pMSP->pBuffer, (*ppSample)->GetSize());// copy
				}
			}
			else
			{
				hr = VFW_E_SAMPLE_REJECTED_EOS;
			}break;
		}
	}
	else															// direct buffer allocator
	{
		if (m_bReCommit)
		{
			hr = RecommitDirectAllocator();				// recommit if neccessary
		}
		if (FAILED(hr))
		{
			return hr;
		}
		switch (m_nBufferType)
		{
			case BUFFER_FIXED:								// fixed buffer
			if (m_allocprops.cBuffers > 0)
			{														// get the buffer from the direct allocator
				hr = m_pDirectAllocator->GetBuffer(ppSample, pStartTime, pEndTime, dwFlags);
				if (FAILED(hr))
				{
					return hr;
				}
				pMSP = m_pSampleProperties;
				if (m_bBufferChanged)						// buffer changed
				{													// set new buffer
					((CMediaSample*)*ppSample)->SetPointer(pMSP->pBuffer+m_allocprops.cbPrefix, pMSP->nLength);
				}
				m_allocprops.cBuffers--;					// decrement No. of buffers
			}
			else
			{
				hr = VFW_E_SAMPLE_REJECTED_EOS;
			}break;
			case BUFFER_ATTACH:
			case BUFFER_COPY:
			if (m_BufferList.GetCount())
			{														// get the buffer from the direct allocator
				hr = m_pDirectAllocator->GetBuffer(ppSample, pStartTime, pEndTime, dwFlags);
				if (FAILED(hr))
				{
					return hr;
				}
				pMSP = m_BufferList.RemoveTail();		// remove the buffer from the list
				if (!pMSP) return E_POINTER;
				m_SentBufferList.AddHead(pMSP);			// add to the sent buffers list
																	// set sample buffer
				hr = ((CMediaSample*)*ppSample)->SetPointer(pMSP->pBuffer+m_allocprops.cbPrefix, pMSP->nLength);
			}
			else
			{
				hr = VFW_E_SAMPLE_REJECTED_EOS;
			}break;
		}
	}
	if ((m_nBufferType != 0) && (pMSP != NULL))		// update the media sample properties
	{
		if (SUCCEEDED(hr))
		{
			hr = ((CMediaSample*)*ppSample)->SetPreroll(pMSP->bIsPreroll);
		}
		if (SUCCEEDED(hr))
		{
			hr = ((CMediaSample*)*ppSample)->SetMediaType(&m_MediaType);
		}
		if (SUCCEEDED(hr))
		{
			hr = ((CMediaSample*)*ppSample)->SetDiscontinuity(pMSP->bIsDiscontinuity);
		}
		if (SUCCEEDED(hr))
		{
			hr = ((CMediaSample*)*ppSample)->SetSyncPoint(pMSP->bIsSyncPoint);
		}
		if (SUCCEEDED(hr))
		{
			hr = ((CMediaSample*)*ppSample)->SetMediaTime(&pMSP->llTimeStart, &pMSP->llTimeEnd);
		}
		if (SUCCEEDED(hr))
		{
			hr = ((CMediaSample*)*ppSample)->SetTime(&pMSP->rtTimeStart, &pMSP->rtTimeEnd);
		}
	}
	if (!m_pDirectAllocator && pMSP)					// no direct buffer allocator
	{
		ReleaseMediaBuffer(pMSP->pBuffer);			// release here
	}
	return hr;
}

HRESULT CMediaSourceFilter::ReleaseMediaBuffer(BYTE *pBuffer)
{
	HRESULT hr = NOERROR;
	switch (m_nBufferType)
	{
		case BUFFER_FIXED:
		if (m_pReleaseBufferFunction)					// external function to release the buffer
		{
			hr = m_pReleaseBufferFunction(pBuffer, m_lParamRBF);
		}break;
		case BUFFER_COPY:
		case BUFFER_ATTACH:
		{														// Find and remove the properties buffer from the List
			MediaSampleProperties*pMSP = FindMSP(pBuffer, true);
			if (pMSP)
			{
				if (m_pReleaseBufferFunction)			// external function to release the buffer
				{
					hr = m_pReleaseBufferFunction(pBuffer, m_lParamRBF);
				}
				if (m_nBufferType == BUFFER_COPY)	// if the buffer is copied
				{
					CoTaskMemFree(pBuffer);				// it was allocated in SetBuffer(..)
				}
				delete pMSP;								// delete the properties buffer
			}
		}break;
	}
	return hr;
}

// -------------------------------------------------------------------------
// IMediaSource, the control interface for the synthesizer
// -------------------------------------------------------------------------

//
// get_Frequency
//
STDMETHODIMP CMediaSourceFilter::get_Frequency(int *Frequency)
{

    m_Synth->get_Frequency(Frequency);

    DbgLog((LOG_TRACE, 3, TEXT("get_Frequency: %d"), *Frequency));

    return NOERROR;
}


//
// put_Frequency
//
STDMETHODIMP CMediaSourceFilter::put_Frequency(int Frequency) 
{
    m_Synth->put_Frequency (Frequency);

    DbgLog((LOG_TRACE, 3, TEXT("put_Frequency: %d"), Frequency));

    return NOERROR;
}

//
// get_Waveform
//
STDMETHODIMP CMediaSourceFilter::get_Waveform(int *Waveform) 
{
    m_Synth->get_Waveform (Waveform);

    DbgLog((LOG_TRACE, 3, TEXT("get_Waveform: %d"), *Waveform));

    return NOERROR;
}


//
// put_Waveform
//
STDMETHODIMP CMediaSourceFilter::put_Waveform(int Waveform) 
{
    m_Synth->put_Waveform (Waveform);

    DbgLog((LOG_TRACE, 3, TEXT("put_Waveform: %d"), Waveform));

    return NOERROR;
}

//
// get_Channels
//
STDMETHODIMP CMediaSourceFilter::get_Channels(int *Channels) 
{

    HRESULT hr = m_Synth->get_Channels( Channels );

    DbgLog((LOG_TRACE, 3, TEXT("get_Channels: %d"), *Channels));

    return hr;
}

//
// If the format changes, we need to reconnect
//
void CMediaSourceFilter::ReconnectWithNewFormat(void) 
{

    // The caller must hold the state lock because this
    // function calls IsConnected().
    ASSERT(CritCheckIn(pStateLock ()));

    // The synth filter's only has one pin.  The pin is an output pin.
    CDynamicSourceStream* pOutputPin = (CDynamicSourceStream*)GetPin(0);

    if( pOutputPin->IsConnected() ) 
	 {
        pOutputPin->OutputPinNeedsToBeReconnected();
    }
}

//
// put_Channels
//
// SetBuffer
STDMETHODIMP CMediaSourceFilter::SetBuffer(MediaSampleProperties *pMSP)
{
	HRESULT hr = NOERROR;
	if (!pMSP)          return E_POINTER;
	if (!pMSP->pBuffer) return E_POINTER;
	if (!pMSP->nLength) return E_INVALIDARG;

	if (m_nBufferType == BUFFER_ATTACH)				// attach the buffer,
	{															// copy the properties
		m_BufferList.AddHead(new MediaSampleProperties(pMSP));// and add to the list
	}
	else if (m_nBufferType == BUFFER_COPY)			// copy the buffer
	{															// and the properties
		MediaSampleProperties *pNew = new MediaSampleProperties(pMSP);
		pNew->pBuffer = (BYTE*)CoTaskMemAlloc(pMSP->nLength);// allocate the buffer, 
		if (!pNew->pBuffer)
		{
			delete pNew;
			return E_OUTOFMEMORY;
		}
		CopyMemory(pNew->pBuffer, pMSP->pBuffer, pMSP->nLength);// copy
		m_BufferList.AddHead(pNew);					// and add to the list
	}
	else if (m_nBufferType == BUFFER_FIXED)		// fixed buffer
	{
		if (!m_pSampleProperties)						// no list, only one fixed buffer
		{														// with properties
			m_pSampleProperties = new MediaSampleProperties();
		}
		if (m_pSampleProperties->pBuffer != pMSP->pBuffer)
		{														// different buffer
			m_bBufferChanged = true;					// notify buffer is changed
		}
		if (m_pSampleProperties->nLength > m_allocprops.cbBuffer)
		{														// different allocator properties
			m_bReCommit = true;							// Recomit is neccessary
		}
		*m_pSampleProperties = *pMSP;					// copy further properites
		m_allocprops.cBuffers = 1;						// set No. of buffers to 1
	}

   return hr;
}
// End SetBuffer

// SetBufferType
STDMETHODIMP CMediaSourceFilter::SetBufferType(int nBufferType, RELEASE_BUFFER_FUNCTION pFnc, long lParam, ALLOCATOR_PROPERTIES*pAP)
{
	HRESULT hr = NOERROR;
	if (m_nBufferType < 0)
	{
		return E_INVALIDARG;
	}
	if (m_nBufferType > BUFFER_COPY)
	{
		return E_INVALIDARG;
	}

	if (m_BufferList.GetCount())
	{
		return VFW_E_BUFFERS_OUTSTANDING;
	}

	if (nBufferType == BUFFER_ATTACH)				// attached buffers
	{
		if (pFnc == NULL) return E_POINTER;			// need a detach function !
	}

	m_pReleaseBufferFunction = pFnc;					// release or detach function
	m_lParamRBF              = lParam;				// additional parameter
	m_nBufferType            = nBufferType;		// type of the buffer
	
	if (pAP)
	{
		m_allocprops = *pAP;								// set allocator properties
	}
   return hr;
}
// End SetBufferType

// SetMediaType
STDMETHODIMP CMediaSourceFilter::SetMediaType(AM_MEDIA_TYPE*pMT)
{
	if (m_MediaType.pbFormat != NULL)
	{
		FreeMediaType(m_MediaType);
	}
	CopyMediaType((AM_MEDIA_TYPE*)&m_MediaType, pMT);// set the desired MediaType
   return NOERROR;
}
// End SetMediaType

STDMETHODIMP CMediaSourceFilter::put_Channels(int Channels) 
{

    // This function holds the state lock because it does not want
    // the filter's format type state or its' connection state
    // to change between the call to put_Channels() and the call to
    // ReconnectWithNewFormat().
    CAutoLock lStateLock (pStateLock ());

    HRESULT hr = m_Synth->put_Channels(Channels);
    if( FAILED( hr ) ) 
	 {
        return hr;
    }

    ReconnectWithNewFormat ();

    DbgLog((LOG_TRACE, 3, TEXT("put_Channels: %d"), Channels));

    return NOERROR;
}

//
// get_BitsPerSample
//
STDMETHODIMP CMediaSourceFilter::get_BitsPerSample(int *BitsPerSample) 
{

    HRESULT hr = m_Synth->get_BitsPerSample(BitsPerSample);

    DbgLog((LOG_TRACE, 3, TEXT("get_BitsPerSample: %d"), *BitsPerSample));

    return hr;
}


//
// put_BitsPerSample
//
STDMETHODIMP CMediaSourceFilter::put_BitsPerSample(int BitsPerSample) 
{

    // This function holds the state lock because it does not want
    // the filter's format type state or its' connection state
    // to change between the call to put_BitsPerSample() and the call to
    // ReconnectWithNewFormat().
    CAutoLock lStateLock (pStateLock ());

    HRESULT hr = m_Synth->put_BitsPerSample(BitsPerSample);
    if( FAILED( hr ) ) 
	 {
        return hr;
    }

    ReconnectWithNewFormat ();

    DbgLog((LOG_TRACE, 3, TEXT("put_BitsPerSample: %d"), BitsPerSample));

    return NOERROR;
}

//
// get_SamplesPerSec
//
STDMETHODIMP CMediaSourceFilter::get_SamplesPerSec(int *SamplesPerSec)
{

    HRESULT hr = m_Synth->get_SamplesPerSec(SamplesPerSec);
    
    DbgLog((LOG_TRACE, 3, TEXT("get_SamplesPerSec: %d"), *SamplesPerSec));

    return hr;
}


//
// put_SamplesPerSec
//
STDMETHODIMP CMediaSourceFilter::put_SamplesPerSec(int SamplesPerSec) 
{

    // This function holds the state lock because it does not want
    // the filter's format type state or its' connection state
    // to change between the call to put_SamplesPerSec() and the call to
    // ReconnectWithNewFormat().
    CAutoLock lStateLock (pStateLock ());

    HRESULT hr = m_Synth->put_SamplesPerSec(SamplesPerSec);
    if( FAILED( hr ) )
	 {
        return hr;
    }

    ReconnectWithNewFormat ();

    DbgLog((LOG_TRACE, 3, TEXT("put_SamplesPerSec: %d"), SamplesPerSec));

    return NOERROR;
}

//
// get_Amplitude
//
STDMETHODIMP CMediaSourceFilter::get_Amplitude(int *Amplitude) 
{

    m_Synth->get_Amplitude (Amplitude);

    DbgLog((LOG_TRACE, 3, TEXT("get_Amplitude: %d"), *Amplitude));

    return NOERROR;
}


//
// put_Amplitude
//
STDMETHODIMP CMediaSourceFilter::put_Amplitude(int Amplitude) 
{
    m_Synth->put_Amplitude (Amplitude);

    DbgLog((LOG_TRACE, 3, TEXT("put_Amplitude: %d"), Amplitude));

    return NOERROR;
}


//
// get_SweepRange
//
STDMETHODIMP CMediaSourceFilter::get_SweepRange(int *SweepStart, int *SweepEnd) 
{
    m_Synth->get_SweepRange (SweepStart, SweepEnd);

    DbgLog((LOG_TRACE, 3, TEXT("get_SweepStart: %d %d"), *SweepStart, *SweepEnd));

    return NOERROR;
}


//
// put_SweepRange
//
STDMETHODIMP CMediaSourceFilter::put_SweepRange(int SweepStart, int SweepEnd) 
{
    m_Synth->put_SweepRange (SweepStart, SweepEnd);

    DbgLog((LOG_TRACE, 3, TEXT("put_SweepRange: %d %d"), SweepStart, SweepEnd));

    return NOERROR;
}

STDMETHODIMP CMediaSourceFilter::get_OutputFormat(SYNTH_OUTPUT_FORMAT* pOutputFormat) 
{
    HRESULT hr = m_Synth->get_OutputFormat(pOutputFormat);
    if (FAILED(hr)) 
	 {
        return hr;
    }

    return S_OK;
}

STDMETHODIMP CMediaSourceFilter::put_OutputFormat(SYNTH_OUTPUT_FORMAT ofOutputFormat)
{

    // This function holds the state lock because it does not want
    // the filter's format type state or its' connection state
    // to change between the call to put_OutputFormat() and the call to
    // ReconnectWithNewFormat().
    CAutoLock lStateLock (pStateLock ());    

    HRESULT hr = m_Synth->put_OutputFormat(ofOutputFormat);
    if (FAILED(hr)) 
	 {
        return hr;
    }

    ReconnectWithNewFormat();

    return S_OK;
}


// -------------------------------------------------------------------------
// CDirectBufferAllocater, the direct allocator
// -------------------------------------------------------------------------

CDirectBufferAllocater::~CDirectBufferAllocater( )
{
	ReallyFree();
	m_pBuffer = NULL;
}

// Alloc
HRESULT CDirectBufferAllocater::Alloc( )
{
   CAutoLock lck(this);										// look at the base class code to see where this came from!

   HRESULT hr = CBaseAllocator::Alloc();				// Check he has called SetProperties
   if (FAILED(hr))
	{
      return hr;
   }

   if (hr == S_FALSE)										// If the requirements haven't changed then don't reallocate
	{
      return NOERROR;
   }
   ASSERT(hr == S_OK);										// we use this fact in the loop below

	if (m_pBuffer)												// Free the old resources
	{
		ReallyFree();
	}

   LONG lAlignedSize = m_lSize + m_lPrefix;			// Compute the aligned size
   if (m_lAlignment > 1)
	{
      LONG lRemainder = lAlignedSize % m_lAlignment;
      if (lRemainder != 0)
		{
         lAlignedSize += (m_lAlignment - lRemainder);
      }
   }

   /* Create the contiguous memory block for the samples
      making sure it's properly aligned (64K should be enough!)
   */
   ASSERT(lAlignedSize % m_lAlignment == 0);

	if (m_pFilter->m_nBufferType == BUFFER_FIXED)	// Buffer is fixed ?
	{																// don't create the buffer - use what was passed to us
		m_pBuffer = (BYTE*)m_pFilter->m_pSampleProperties->pBuffer;
		if (m_pBuffer == NULL)								// set the external buffer
		{
			 return E_OUTOFMEMORY;
		}

		LPBYTE pNext = m_pBuffer;
		CMediaSample *pSample;

		ASSERT(m_lAllocated == 0);

		// Create the new samples with the external buffer
		for (; m_lAllocated < m_lCount; m_lAllocated++, pNext += lAlignedSize)
		{
			pSample = new CMediaSample(
								NAME("external buffer media sample"),
								this,
								&hr,
								pNext + m_lPrefix,      // GetPointer() value
								m_lSize);               // not including prefix

			ASSERT(SUCCEEDED(hr));
			if (pSample == NULL)
			{
				return E_OUTOFMEMORY;
			}

			m_lFree.Add(pSample);
		}
	}
	else if ((m_pFilter->m_nBufferType == BUFFER_ATTACH) || // Buffer is attached ?
		      (m_pFilter->m_nBufferType == BUFFER_COPY  ))   // Buffer is copied   ?
	{																		  // don't create the buffer 
		CMediaSample *pSample;

		// Create the new samples with empty buffer
		// the external buffers are applied later
		for (; m_lAllocated < m_lCount; m_lAllocated++)
		{
			pSample = new CMediaSample(
								NAME("external buffers media sample"),
								this,
								&hr,
								NULL, 
								m_lSize);

			ASSERT(SUCCEEDED(hr));
			if (pSample == NULL)
			{
				return E_OUTOFMEMORY;
			}

			m_lFree.Add(pSample);
		}
	}

   m_bChanged = FALSE;
   return NOERROR;
}
// End Alloc

// ReallyFree
void CDirectBufferAllocater::ReallyFree()
{
	// look at the base class code to see where this came from!
	ASSERT(m_lAllocated == m_lFree.GetCount());

	/* Free up all the CMediaSamples */
	CMediaSample *pSample;
	for (;;)											// remove all samples
	{
		pSample = m_lFree.RemoveHead();
		if (pSample != NULL)
		{
			BYTE *pBuffer = NULL;				// get data pointer
			HRESULT hr = pSample->GetPointer(&pBuffer);
			if (SUCCEEDED(hr))
			{											// check for prefix
				if (m_pFilter->m_allocprops.cbPrefix)
				{										// correct the offset
					pBuffer -= m_pFilter->m_allocprops.cbPrefix;
				}										// release the buffer
				m_pFilter->ReleaseMediaBuffer(pBuffer);
			}
		   delete pSample;
		}
		else
		{
			break;
		}
   }

   m_lAllocated = 0;
}
// End ReallyFree

// Free
void CDirectBufferAllocater::Free(void)
{
	CMemAllocator::Free();
}
// End Free

STDMETHODIMP CDirectBufferAllocater::SetProperties(ALLOCATOR_PROPERTIES* pRequest,ALLOCATOR_PROPERTIES* pActual)
{
	return CMemAllocator::SetProperties(pRequest, pActual);
}

STDMETHODIMP CDirectBufferAllocater::GetBuffer(IMediaSample **ppBuffer, REFERENCE_TIME *pT1, REFERENCE_TIME *pT2, DWORD dwFlags)
{
	return CMemAllocator::GetBuffer(ppBuffer, pT1, pT2, dwFlags);
}

STDMETHODIMP CDirectBufferAllocater::ReleaseBuffer(IMediaSample *pBuffer)
{
	HRESULT hr = E_NOTIMPL;
	
	if (m_pFilter->m_nBufferType)						// if there are external buffers
	{
		BYTE *pData = NULL;
		hr = pBuffer->GetPointer((BYTE**)&pData);	// get the data pointer to identify the properties
		if (SUCCEEDED(hr))
		{
			if (m_pFilter->m_allocprops.cbPrefix)	// correct the offset
			{
				pBuffer -= m_pFilter->m_allocprops.cbPrefix;
			}
			m_pFilter->ReleaseMediaBuffer(pData);	// release the external data buffer
		}
	}
	hr = CMemAllocator::ReleaseBuffer(pBuffer);	// release the sample buffer
	return hr;
}

// -------------------------------------------------------------------------
// CMediaSourceStream, the output pin
// -------------------------------------------------------------------------

//
// CMediaSourceStream::Constructor
//

CMediaSourceStream::CMediaSourceStream(HRESULT *phr, CMediaSourceFilter *pParent, LPCWSTR pName)
    : CDynamicSourceStream(NAME("Media Source output pin"),phr, pParent, pName)
    , m_hPCMToMSADPCMConversionStream(NULL)
    , m_dwTempPCMBufferSize(0)
    , m_fFirstSampleDelivered(FALSE)
    , m_llSampleMediaTimeStart(0)
	 , m_guidPinCategory(GUID_NULL)
	 , m_fLastSampleDiscarded(FALSE)
{

    {
        m_Synth = new CAudioSynth(pParent->pStateLock ());
        pParent->m_Synth = m_Synth;
        if (m_Synth == NULL) 
		  {
            *phr = E_OUTOFMEMORY;
            return;
        }
        m_pParent = pParent;
    }
}


//
// CMediaSourceStream::Destructor
//
CMediaSourceStream::~CMediaSourceStream(void) 
{
   delete m_Synth;
}


//
// FillBuffer
//
// Stuffs the buffer with data
HRESULT CMediaSourceStream::FillBuffer(IMediaSample *pms) 
{
	if (m_pParent->m_nBufferType)		// external buffers
	{
		HRESULT hr;
		REFERENCE_TIME rtTimeStart, rtTimeEnd, rtCurrent;
		
		pms->GetTime(&rtTimeStart, &rtTimeEnd);
		if ((rtTimeStart == 0) && (rtTimeEnd == 0) && m_pFilter && m_pFilter->GetClock())
		{
			m_pFilter->GetClock()->GetTime(&rtCurrent);
			rtTimeStart = rtCurrent;
			rtTimeStart = rtCurrent + 1000; // 1 ms
			hr = pms->SetTime(&rtTimeStart, &rtTimeEnd);
			if (FAILED(hr)) 
			{
			  return hr;
			}
		}
		return NOERROR;					// are not handled here
	}

	BYTE *pData;
	HRESULT hr = pms->GetPointer(&pData);
	if (FAILED(hr)) 
	{
	  return hr;
	}

	// This function must hold the state lock because it calls
	// FillPCMAudioBuffer().
	CAutoLock lStateLock (m_pParent->pStateLock ());
	// This lock must be held because this function uses
	// m_dwTempPCMBufferSize, m_hPCMToMSADPCMConversionStream,
	// m_rtSampleTime, m_fFirstSampleDelivered and
	// m_llSampleMediaTimeStart.
	CAutoLock lShared(&m_cSharedState);

	WAVEFORMATEX* pwfexCurrent = (WAVEFORMATEX*)m_mt.Format();

	if (WAVE_FORMAT_PCM == pwfexCurrent->wFormatTag) 
	{
	  m_Synth->FillPCMAudioBuffer(*pwfexCurrent, pData, pms->GetSize());

	  hr = pms->SetActualDataLength(pms->GetSize());
	  if (FAILED(hr)) 
	  {
			return hr;
	  }

	}
	else 
	{
	  // This filter only supports two audio formats: PCM and ADPCM. 
	  ASSERT(WAVE_FORMAT_ADPCM == pwfexCurrent->wFormatTag);

	  // Create PCM audio samples and then compress them.  We use the
	  // Audio Compression Manager (ACM) API to convert the samples to 
	  // the ADPCM format. 

	  ACMSTREAMHEADER ACMStreamHeader;

	  ACMStreamHeader.cbStruct = sizeof(ACMStreamHeader);
	  ACMStreamHeader.fdwStatus = 0;
	  ACMStreamHeader.dwUser = 0;
	  ACMStreamHeader.cbSrcLength = m_dwTempPCMBufferSize;
	  ACMStreamHeader.cbSrcLengthUsed = 0;
	  ACMStreamHeader.dwSrcUser = 0; 
	  ACMStreamHeader.pbDst = pData; 
	  ACMStreamHeader.cbDstLength = pms->GetSize(); 
	  ACMStreamHeader.cbDstLengthUsed = 0; 
	  ACMStreamHeader.dwDstUser = 0; 
	  ACMStreamHeader.pbSrc = new BYTE[m_dwTempPCMBufferSize];
	  if (NULL == ACMStreamHeader.pbSrc) 
	  {
			return E_OUTOFMEMORY;
	  }

	  WAVEFORMATEX wfexPCMAudio;

	  DerivePCMFormatFromADPCMFormatStructure(*pwfexCurrent, &wfexPCMAudio);

	  m_Synth->FillPCMAudioBuffer(wfexPCMAudio,
											ACMStreamHeader.pbSrc,
											ACMStreamHeader.cbSrcLength);

	  MMRESULT mmr = acmStreamPrepareHeader(m_hPCMToMSADPCMConversionStream,   
														 &ACMStreamHeader,
														 0);

	  // acmStreamPrepareHeader() returns 0 if no errors occur.
	  if (mmr != 0) 
	  {
			delete [] ACMStreamHeader.pbSrc;
			return E_FAIL;
	  }

	  mmr = acmStreamConvert(m_hPCMToMSADPCMConversionStream,
									 &ACMStreamHeader,
									 ACM_STREAMCONVERTF_BLOCKALIGN);

	  MMRESULT mmrUnprepare = acmStreamUnprepareHeader(m_hPCMToMSADPCMConversionStream,   
																		&ACMStreamHeader,
																		0);

	  delete [] ACMStreamHeader.pbSrc;

	  // acmStreamConvert() andacmStreamUnprepareHeader() returns 0 if no errors occur.
	  if ((mmr != 0) || (mmrUnprepare != 0)) 
	  {
			return E_FAIL;
	  }

	  hr = pms->SetActualDataLength(ACMStreamHeader.cbDstLengthUsed);
	  if (FAILED(hr)) 
	  {
			return hr;
	  }
	}

	// Set the sample's time stamps.  
	CRefTime rtStart = m_rtSampleTime;

	m_rtSampleTime = rtStart + (REFERENCE_TIME)(UNITS * pms->GetActualDataLength()) / (REFERENCE_TIME)pwfexCurrent->nAvgBytesPerSec;

	hr = pms->SetTime((REFERENCE_TIME*)&rtStart, (REFERENCE_TIME*)&m_rtSampleTime);

	m_rtSampleTime = m_rtSampleTime + 1;

	if (FAILED(hr))
	{
	  return hr;
	}

	// Set the sample's properties.
	hr = pms->SetPreroll(FALSE);
	if (FAILED(hr)) 
	{
	  return hr;
	}

	hr = pms->SetMediaType(NULL);
	if (FAILED(hr)) 
	{
	  return hr;
	}

	hr = pms->SetDiscontinuity(!m_fFirstSampleDelivered);
	if (FAILED(hr)) 
	{
	  return hr;
	}

	hr = pms->SetSyncPoint(!m_fFirstSampleDelivered);
	if (FAILED(hr)) 
	{
	  return hr;
	}

	LONGLONG llMediaTimeStart = m_llSampleMediaTimeStart;

	DWORD dwNumAudioSamplesInPacket = (pms->GetActualDataLength() * BITS_PER_BYTE) /
											  (pwfexCurrent->nChannels * pwfexCurrent->wBitsPerSample);

	LONGLONG llMediaTimeStop = m_llSampleMediaTimeStart + dwNumAudioSamplesInPacket;

	hr = pms->SetMediaTime(&llMediaTimeStart, &llMediaTimeStop);
	if (FAILED(hr)) 
	{
	  return hr;
	}

	m_llSampleMediaTimeStart = llMediaTimeStop + 1;
	m_fFirstSampleDelivered = TRUE;

    return NOERROR;
}



//
// Format Support
//

//
// GetMediaType
//
HRESULT CMediaSourceStream::GetMediaType(CMediaType *pmt) 
{

    // The caller must hold the state lock because this function
    // calls get_OutputFormat() and GetPCMFormatStructure().
    // The function assumes that the state of the m_Synth
    // object does not change between the two calls.  The
    // m_Synth object's state will not change if the 
    // state lock is held.
    ASSERT(CritCheckIn(m_pParent->pStateLock ()));

    WAVEFORMATEX *pwfex;
    SYNTH_OUTPUT_FORMAT ofCurrent;

    HRESULT hr = m_Synth->get_OutputFormat( &ofCurrent );
    if (FAILED(hr)) 
	 {
        return hr;
    }
	 if (m_pParent->m_nBufferType > 0)		// if the is an external buffer
	 {													// there is an external media type
		 CopyMediaType(pmt, &m_pParent->m_MediaType);
		 return NOERROR;
    }
    else if (SYNTH_OF_PCM == ofCurrent) 
	 {
        pwfex = (WAVEFORMATEX *) pmt->AllocFormatBuffer(sizeof(WAVEFORMATEX));
        if (NULL == pwfex)
		  {
            return E_OUTOFMEMORY;
        }

        m_Synth->GetPCMFormatStructure(pwfex);

    }
	 else if (SYNTH_OF_MS_ADPCM == ofCurrent) 
	 {
        DWORD dwMaxWAVEFORMATEXSize;
        
        MMRESULT mmr = acmMetrics(NULL, ACM_METRIC_MAX_SIZE_FORMAT, (void*)&dwMaxWAVEFORMATEXSize);

        // acmMetrics() returns 0 if no errors occur.
        if( 0 != mmr )
		  {
            return E_FAIL;
        }

        pwfex = (WAVEFORMATEX *) pmt->AllocFormatBuffer(dwMaxWAVEFORMATEXSize);
        if (NULL == pwfex)
		  {
            return E_OUTOFMEMORY;
        }

        WAVEFORMATEX wfexSourceFormat;
        m_Synth->GetPCMFormatStructure(&wfexSourceFormat);

        ZeroMemory(pwfex, dwMaxWAVEFORMATEXSize);
        pwfex->wFormatTag = WAVE_FORMAT_ADPCM;
        pwfex->cbSize = (USHORT)(dwMaxWAVEFORMATEXSize - sizeof(WAVEFORMATEX));
        pwfex->nChannels = wfexSourceFormat.nChannels;
        pwfex->nSamplesPerSec = wfexSourceFormat.nSamplesPerSec;

        mmr = acmFormatSuggest(NULL,
                               &wfexSourceFormat,
                               pwfex,
                               dwMaxWAVEFORMATEXSize,
                               ACM_FORMATSUGGESTF_WFORMATTAG | ACM_FORMATSUGGESTF_NSAMPLESPERSEC | ACM_FORMATSUGGESTF_NCHANNELS);
        // acmFormatSuggest() returns 0 if no errors occur.
        if( 0 != mmr ) 
		  {
            return E_FAIL;
        }
       
    }
	 else 
	 {
        return E_UNEXPECTED;
    }

    return CreateAudioMediaType(pwfex, pmt, FALSE);
}

HRESULT CMediaSourceStream::CompleteConnect(IPin *pReceivePin)
{
    // This lock must be held because this function uses
    // m_hPCMToMSADPCMConversionStream, m_fFirstSampleDelivered 
    // and m_llSampleMediaTimeStart.
    CAutoLock lShared(&m_cSharedState);

    HRESULT hr;
    WAVEFORMATEX *pwfexCurrent = (WAVEFORMATEX*)m_mt.Format();

    if (WAVE_FORMAT_PCM == pwfexCurrent->wFormatTag)
	 {
        hr = m_Synth->AllocWaveCache(*pwfexCurrent);
        if (FAILED(hr))
		  {
            return hr;
        }

    } 
	 else if (WAVE_FORMAT_ADPCM == pwfexCurrent->wFormatTag) 
	 {
        WAVEFORMATEX wfexSourceFormat;

        DerivePCMFormatFromADPCMFormatStructure(*pwfexCurrent, &wfexSourceFormat);

        hr = m_Synth->AllocWaveCache(wfexSourceFormat);
        if (FAILED(hr))
		  {
            return hr;
        }

        MMRESULT mmr = acmStreamOpen( &m_hPCMToMSADPCMConversionStream,
                                      NULL,
                                      &wfexSourceFormat,
                                      pwfexCurrent,
                                      NULL,
                                      0,
                                      0,
                                      ACM_STREAMOPENF_NONREALTIME );
        // acmStreamOpen() returns 0 if an no errors occur.                              
        if (mmr != 0) 
		  {
            return E_FAIL;
        }

    }
	 else
	 {
        
        ASSERT(NULL == m_hPCMToMSADPCMConversionStream);

    }
       
    hr = CDynamicSourceStream::CompleteConnect(pReceivePin);
    if (FAILED(hr))
	 {
        if (WAVE_FORMAT_ADPCM == pwfexCurrent->wFormatTag)
		  {
            // acmStreamClose() should never fail because m_hPCMToMSADPCMConversionStream
            // holds a valid ACM stream handle and all operations using the handle are 
            // synchronous.
            EXECUTE_ASSERT(0 == acmStreamClose(m_hPCMToMSADPCMConversionStream, 0));
            m_hPCMToMSADPCMConversionStream = NULL;
        }

        return hr;
    }

    m_fFirstSampleDelivered = FALSE;
    m_llSampleMediaTimeStart = 0;

    return S_OK;
}

void CMediaSourceStream::DerivePCMFormatFromADPCMFormatStructure(const WAVEFORMATEX& wfexADPCM, WAVEFORMATEX* pwfexPCM)
{
    pwfexPCM->wFormatTag = WAVE_FORMAT_PCM; 
    pwfexPCM->wBitsPerSample = 16;
    pwfexPCM->cbSize = 0;

    pwfexPCM->nChannels = wfexADPCM.nChannels;
    pwfexPCM->nSamplesPerSec = wfexADPCM.nSamplesPerSec;

    pwfexPCM->nBlockAlign = (WORD)((pwfexPCM->nChannels * pwfexPCM->wBitsPerSample) / BITS_PER_BYTE);
    pwfexPCM->nAvgBytesPerSec = pwfexPCM->nBlockAlign * pwfexPCM->nSamplesPerSec;
}

HRESULT CMediaSourceStream::BreakConnect(void)
{
    // This lock must be held because this function uses
    // m_hPCMToMSADPCMConversionStream and m_dwTempPCMBufferSize.
    CAutoLock lShared(&m_cSharedState);

    HRESULT hr = CDynamicSourceStream::BreakConnect();
    if (FAILED(hr))
	 {
        return hr;
    }

    if (NULL != m_hPCMToMSADPCMConversionStream)
	 {
        // acmStreamClose() should never fail because m_hPCMToMSADPCMConversionStream
        // holds a valid ACM stream handle and all operations using the handle are 
        // synchronous.
        EXECUTE_ASSERT(0 == acmStreamClose(m_hPCMToMSADPCMConversionStream, 0));
        m_hPCMToMSADPCMConversionStream = NULL;
        m_dwTempPCMBufferSize = 0;
    }

    return S_OK;
}


HRESULT CMediaSourceStream::InitAllocator(IMemAllocator **ppAlloc)
{
	return CBaseOutputPin::InitAllocator(ppAlloc);
}

STDMETHODIMP CMediaSourceStream::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
    if (riid == IID_IAMStreamControl )
	 {
        return GetInterface((IAMStreamControl *) this, ppv);
    }
    else if (riid == IID_IKsPropertySet)
	 {
        return GetInterface((IKsPropertySet *) this, ppv);
    }
	 else
	 {
        return CDynamicSourceStream::NonDelegatingQueryInterface(riid, ppv);
    }
}

// IKsPropertySet interface
STDMETHODIMP CMediaSourceStream::Get(REFGUID rguidPropSet, ULONG ulId, LPVOID pInstanceData, ULONG ulInstanceLength, LPVOID pPropertyData, ULONG ulDataLength,PULONG pulBytesReturned)
{
	if (rguidPropSet != AMPROPSETID_Pin) return E_PROP_SET_UNSUPPORTED;
	if (ulId != AMPROPERTY_PIN_CATEGORY) return E_PROP_ID_UNSUPPORTED;
	if (pPropertyData == NULL)           return E_POINTER;
	if (pulBytesReturned == NULL)        return E_POINTER;
	if (ulDataLength  <  sizeof(GUID))   return E_OUTOFMEMORY;

	*pulBytesReturned = sizeof(GUID);
	*((GUID*)pPropertyData) = m_guidPinCategory;
	return S_OK;
}

STDMETHODIMP CMediaSourceStream::Set(REFGUID rguidPropSet,ULONG ulId,LPVOID pInstanceData,ULONG ulInstanceLength,LPVOID pPropertyData,ULONG ulDataLength)
{
	if (rguidPropSet != AMPROPSETID_Pin) return E_PROP_SET_UNSUPPORTED;
	if (ulId != AMPROPERTY_PIN_CATEGORY) return E_PROP_ID_UNSUPPORTED;
	if (pPropertyData == NULL)           return E_POINTER;
	if (ulDataLength  != sizeof(GUID))   return E_INVALIDARG;
	m_guidPinCategory = *((GUID*)pPropertyData);
	return S_OK;
}

STDMETHODIMP CMediaSourceStream::QuerySupported(REFGUID guidPropSet,DWORD dwPropID, DWORD *pTypeSupport)
{
	if (guidPropSet == AMPROPSETID_Pin)      return E_PROP_SET_UNSUPPORTED;
	if (dwPropID == AMPROPERTY_PIN_CATEGORY) return E_PROP_ID_UNSUPPORTED;
	if (pTypeSupport == NULL)                return E_POINTER;

	*pTypeSupport = KSPROPERTY_SUPPORT_GET|KSPROPERTY_SUPPORT_SET;
	return S_OK;
}

// IPin interface
STDMETHODIMP CMediaSourceStream::BeginFlush(void)
{
	CBaseStreamControl::Flushing(TRUE);
	return CDynamicSourceStream::BeginFlush();
}

STDMETHODIMP CMediaSourceStream::EndFlush(void)
{
	CBaseStreamControl::Flushing(FALSE);
	return CDynamicSourceStream::EndFlush();
}

STDMETHODIMP CMediaSourceStream::Receive(IMediaSample *pSample)
{
    int iStreamState = CheckStreamState(pSample);
    if (iStreamState == STREAM_FLOWING) 
    {
        if (m_fLastSampleDiscarded)
            pSample->SetDiscontinuity(TRUE);
        m_fLastSampleDiscarded = FALSE;
        /* Deliver the sample. */
    } 
    else 
    {
        m_fLastSampleDiscarded = TRUE;  
       // Discard this sample. Do not deliver it.
    }
	 return S_OK;
}
//
// DecideBufferSize
//
// This will always be called after the format has been sucessfully
// negotiated. So we have a look at m_mt to see what format we agreed to.
// Then we can ask for buffers of the correct size to contain them.
HRESULT CMediaSourceStream::DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *pProperties)
{
    // The caller should always hold the shared state lock 
    // before calling this function.  This function must hold 
    // the shared state lock because it uses m_hPCMToMSADPCMConversionStream
    // m_dwTempPCMBufferSize.
    ASSERT(CritCheckIn(&m_cSharedState));

    ASSERT(pAlloc);
    ASSERT(pProperties);

    WAVEFORMATEX *pwfexCurrent = (WAVEFORMATEX*)m_mt.Format();

	 if (m_pParent->m_nBufferType > 0)						// external buffer
	 {
		 *pProperties = m_pParent->m_allocprops;			// with internal allocator
		 ALLOCATOR_PROPERTIES Actual;
		 HRESULT hr = pAlloc->SetProperties(pProperties,&Actual);
		 if (FAILED(hr)) return hr;
		 if (Actual.cbBuffer < pProperties->cbBuffer)	// on this error:
		 {																// try to divide into smaller
			  return E_FAIL;										// buffer pieces
		 }
		 return hr;
	 }
    else if (WAVE_FORMAT_PCM == pwfexCurrent->wFormatTag)
	 {
        pProperties->cbBuffer = WaveBufferSize;
    }
	 else
	 {

        // This filter only supports two formats: PCM and ADPCM. 
        ASSERT(WAVE_FORMAT_ADPCM == pwfexCurrent->wFormatTag);

        pProperties->cbBuffer = pwfexCurrent->nBlockAlign;

        MMRESULT mmr = acmStreamSize(m_hPCMToMSADPCMConversionStream,
                                     pwfexCurrent->nBlockAlign,
                                     &m_dwTempPCMBufferSize,
                                     ACM_STREAMSIZEF_DESTINATION);

        // acmStreamSize() returns 0 if no error occurs.
        if (0 != mmr)
		  {
            return E_FAIL;
        }
    }

    int nBitsPerSample = pwfexCurrent->wBitsPerSample;
    int nSamplesPerSec = pwfexCurrent->nSamplesPerSec;
    int nChannels = pwfexCurrent->nChannels;

    pProperties->cBuffers = (nChannels * nSamplesPerSec * nBitsPerSample) / (pProperties->cbBuffer * BITS_PER_BYTE);
    // Get 1/2 second worth of buffers
    pProperties->cBuffers /= 2;
    if (pProperties->cBuffers < 1)
        pProperties->cBuffers = 1 ;

    // Ask the allocator to reserve us the memory

    ALLOCATOR_PROPERTIES Actual;
    HRESULT hr = pAlloc->SetProperties(pProperties,&Actual);
    if (FAILED(hr))
	 {
        return hr;
    }

    // Is this allocator unsuitable

    if (Actual.cbBuffer < pProperties->cbBuffer) 
	 {
        return E_FAIL;
    }
    return NOERROR;
}

HRESULT CMediaSourceStream::DecideAllocator(IMemInputPin * pPin, IMemAllocator ** ppAlloc)
{
	HRESULT hr = NOERROR;
	if (m_pParent->m_nBufferType > 0)					// external buffer
	{
		ALLOCATOR_PROPERTIES prop;
		ZeroMemory(&prop, sizeof(prop));
		hr = pPin->GetAllocatorRequirements(&prop);	// whatever he returns, we assume prop is either all zeros

		if (hr == E_NOTIMPL)
		{
			hr = m_pParent->CreateDirectAllocator();	//	create the direct allocator and set its properties
			if (SUCCEEDED(hr))
			{														// notify this allocator to the receiving pin
				hr = pPin->NotifyAllocator(m_pParent->m_pDirectAllocator, FALSE);
//				hr = E_FAIL; // nur test
			}
			if (SUCCEEDED(hr))								// accepted
			{														
				*ppAlloc = m_pParent->m_pDirectAllocator;// take this allocator
				(*ppAlloc)->AddRef();						// and save the reference
				return hr;										// return 
			}
			else													// not accepted
			{
				m_pParent->ReleaseDirectAllocator();	// release it
			}
		}
	}

	hr = CBaseOutputPin::DecideAllocator(pPin, ppAlloc);// take the default allocater

	return hr;
}

HRESULT CMediaSourceStream::GetDeliveryBuffer(IMediaSample ** ppSample, REFERENCE_TIME * pStartTime, REFERENCE_TIME * pEndTime, DWORD dwFlags)
{
	if (m_pParent->m_nBufferType > 0)					// external buffer
	{																// are delivered by the filter
		return m_pParent->GetDeliveryBuffer(ppSample, pStartTime, pEndTime, dwFlags);
	}

	return CDynamicOutputPin::GetDeliveryBuffer(ppSample,pStartTime,pEndTime,dwFlags);
}


//
// Active
//
HRESULT CMediaSourceStream::Active(void)
{

    // This lock must be held because the function
    // uses m_rtSampleTime, m_fFirstSampleDelivered
    // and m_llSampleMediaTimeStart.
    CAutoLock lShared(&m_cSharedState);
    
    HRESULT hr = CDynamicSourceStream::Active();
    if (FAILED(hr))
	 {
        return hr;
    }

    m_rtSampleTime = 0;
    m_fFirstSampleDelivered = FALSE;
    m_llSampleMediaTimeStart = 0;

    return NOERROR;
}


// -------------------------------------------------------------------------
// CAudioSynth
// -------------------------------------------------------------------------
// Object that knows nothing about DirectShow, but just synthesizes waveforms

CAudioSynth::CAudioSynth(
                CCritSec* pStateLock,
                int Frequency,
                int Waveform,
                int iBitsPerSample,
                int iChannels,
                int iSamplesPerSec,
                int iAmplitude
                )
    : m_bWaveCache(NULL)
    , m_wWaveCache(NULL)
    , m_pStateLock (pStateLock)
{

    ASSERT(Waveform >= WAVE_SINE);
    ASSERT(Waveform <  WAVE_LAST);

    m_iFrequency = Frequency;
    m_iWaveform = Waveform;
    m_iAmplitude = iAmplitude;
    m_iSweepStart = DefaultSweepStart;
    m_iSweepEnd = DefaultSweepEnd;

    m_wFormatTag = WAVE_FORMAT_PCM;
    m_wBitsPerSample = (WORD) iBitsPerSample;
    m_wChannels = (WORD) iChannels;
    m_dwSamplesPerSec = iSamplesPerSec;
}

CAudioSynth::~CAudioSynth()
{
    if (m_bWaveCache)
	 {
        delete[] m_bWaveCache;
    }

    if (m_wWaveCache)
	 {
        delete[] m_wWaveCache;
    }
}

//
// AllocWaveCache
//
//
HRESULT CAudioSynth::AllocWaveCache (const WAVEFORMATEX& wfex) 
{

    // The caller should hold the state lock because this
    // function uses m_iWaveCacheCycles, m_iWaveCacheSize
    // m_iFrequency, m_bWaveCache and m_wWaveCache.  The
    // function should also hold the state lock because
    // it calls CalcCache().
    ASSERT(CritCheckIn(m_pStateLock));

    m_iWaveCacheCycles = m_iFrequency;
    m_iWaveCacheSize = (int) wfex.nSamplesPerSec;

    if (m_bWaveCache) 
	 {
        delete[] m_bWaveCache;
        m_bWaveCache = NULL;
    }
    if (m_wWaveCache) 
	 {
        delete[] m_wWaveCache;
        m_wWaveCache = NULL;
    }

    // The wave cache always stores PCM audio data.
    if (wfex.wBitsPerSample == 8)
	 {
        m_bWaveCache = new BYTE [m_iWaveCacheSize];
        if (NULL == m_bWaveCache)
		  {
            return E_OUTOFMEMORY;
        }

    }
	 else
	 {
        m_wWaveCache = new WORD [m_iWaveCacheSize];
        if (NULL == m_wWaveCache)
		  {
            return E_OUTOFMEMORY;
        }
    }

    CalcCache(wfex);

    return S_OK;
}

void CAudioSynth::GetPCMFormatStructure(WAVEFORMATEX* pwfex) {

    // The caller must hold the state lock because this function uses
    // m_wChannels, m_wBitsPerSample and m_dwSamplesPerSec.
    ASSERT(CritCheckIn(m_pStateLock));

    // Check for valid input parametes.
    ASSERT((1 == m_wChannels) || (2 == m_wChannels));
    ASSERT((8 == m_wBitsPerSample) || (16 == m_wBitsPerSample));
    ASSERT((8000 == m_dwSamplesPerSec) || (11025 == m_dwSamplesPerSec) ||
           (22050 == m_dwSamplesPerSec) || (44100 == m_dwSamplesPerSec));
            
    pwfex->wFormatTag = WAVE_FORMAT_PCM;
    pwfex->nChannels = m_wChannels;
    pwfex->nSamplesPerSec = m_dwSamplesPerSec;
    pwfex->wBitsPerSample = m_wBitsPerSample;        
    pwfex->nBlockAlign = (WORD)((pwfex->wBitsPerSample * pwfex->nChannels) / BITS_PER_BYTE);
    pwfex->nAvgBytesPerSec = pwfex->nBlockAlign * pwfex->nSamplesPerSec;
    pwfex->cbSize = 0;
}

//
// FillAudioBuffer
//
//
//
void CAudioSynth::FillPCMAudioBuffer (const WAVEFORMATEX& wfex, BYTE pBuf[], int iSize) {
    BOOL fCalcCache = FALSE;

    // The caller should always hold the state lock because this
    // function uses m_iFrequency,  m_iFrequencyLast, m_iWaveform
    // m_iWaveformLast, m_iAmplitude, m_iAmplitudeLast, m_iWaveCacheIndex
    // m_iWaveCacheSize, m_bWaveCache and m_wWaveCache.  The caller should
    // also hold the state lock because this function calls CalcCache().
    ASSERT(CritCheckIn(m_pStateLock));

    // Only realloc the cache if the format has changed !
    if (m_iFrequency != m_iFrequencyLast) 
	 {
        fCalcCache = TRUE;
        m_iFrequencyLast = m_iFrequency;
    }
    if (m_iWaveform != m_iWaveformLast)
	 {
        fCalcCache = TRUE;
        m_iWaveformLast = m_iWaveform;
    }
    if (m_iAmplitude != m_iAmplitudeLast)
	 {
        fCalcCache = TRUE;
        m_iAmplitudeLast = m_iAmplitude;
    }

    if (fCalcCache)
	 {
        CalcCache(wfex);
    }

    // Copy cache to output buffers
    if (wfex.wBitsPerSample == 8 && wfex.nChannels == 1)
	 {
        while (iSize--)
		  {
            *pBuf++ = m_bWaveCache[m_iWaveCacheIndex++];
            if (m_iWaveCacheIndex >= m_iWaveCacheSize)
                m_iWaveCacheIndex = 0;
        }
    }
    else if (wfex.wBitsPerSample == 8 && wfex.nChannels == 2)
	 {
        iSize /= 2;
        while (iSize--)
		  {
            *pBuf++ = m_bWaveCache[m_iWaveCacheIndex];
            *pBuf++ = m_bWaveCache[m_iWaveCacheIndex++];
            if (m_iWaveCacheIndex >= m_iWaveCacheSize)
                m_iWaveCacheIndex = 0;
        }
    }
    else if (wfex.wBitsPerSample == 16 && wfex.nChannels == 1)
	 {
        WORD * pW = (WORD *) pBuf;
        iSize /= 2;
        while (iSize--)
		  {
            *pW++ = m_wWaveCache[m_iWaveCacheIndex++];
            if (m_iWaveCacheIndex >= m_iWaveCacheSize)
                m_iWaveCacheIndex = 0;
        }
    }
    else if (wfex.wBitsPerSample == 16 && wfex.nChannels == 2)
	 {
        WORD * pW = (WORD *) pBuf;
        iSize /= 4;
        while (iSize--) 
		  {
            *pW++ = m_wWaveCache[m_iWaveCacheIndex];
            *pW++ = m_wWaveCache[m_iWaveCacheIndex++];
            if (m_iWaveCacheIndex >= m_iWaveCacheSize)
                m_iWaveCacheIndex = 0;
        }
    }
}

void CAudioSynth::CalcCache(const WAVEFORMATEX& wfex) {

    switch (m_iWaveform)
	 {

    case WAVE_SINE:
        CalcCacheSine (wfex);
        break;

    case WAVE_SQUARE:
        CalcCacheSquare (wfex);
        break;

    case WAVE_SAWTOOTH:
        CalcCacheSawtooth (wfex);
        break;

    case WAVE_SINESWEEP:
        CalcCacheSweep (wfex);
        break;

    }
}

//
// CalcCacheSine
//
//
void CAudioSynth::CalcCacheSine (const WAVEFORMATEX& wfex)
{

    int i;
    double d;
    double amplitude;
    double FTwoPIDivSpS;

    amplitude = ((wfex.wBitsPerSample == 8) ? 127 : 32767 )
                    * m_iAmplitude / 100;

    FTwoPIDivSpS = m_iFrequency * TWOPI / wfex.nSamplesPerSec;

    m_iWaveCacheIndex = 0;
    m_iCurrentSample = 0;

    if (wfex.wBitsPerSample == 8)
	 {
        BYTE * pB = m_bWaveCache;

        for (i = 0; i < m_iWaveCacheSize; i++)
		  {
            d = FTwoPIDivSpS * i;
            *pB++ = (BYTE) ((sin (d) * amplitude) + 128);
        }
    }
    else 
	 {
        PWORD pW = (PWORD) m_wWaveCache;

        for (i = 0; i < m_iWaveCacheSize; i++)
		  {
            d = FTwoPIDivSpS * i;
            *pW++ = (WORD) (sin (d) * amplitude);
        }
    }

}

//
// CalcCacheSquare
//
//
void CAudioSynth::CalcCacheSquare (const WAVEFORMATEX& wfex)
{

    int i;
    double d;
    double FTwoPIDivSpS;
    BYTE b0, b1;
    WORD w0, w1;

    b0 = (BYTE) (128 - (127 * m_iAmplitude / 100));
    b1 = (BYTE) (128 + (127 * m_iAmplitude / 100));
    w0 = (WORD) (32767. * m_iAmplitude / 100);
    w1 = (WORD) - (32767. * m_iAmplitude / 100);

    FTwoPIDivSpS = m_iFrequency * TWOPI / wfex.nSamplesPerSec;

    m_iWaveCacheIndex = 0;
    m_iCurrentSample = 0;

    if (wfex.wBitsPerSample == 8) 
	 {
        BYTE * pB = m_bWaveCache;

        for (i = 0; i < m_iWaveCacheSize; i++) 
		  {
            d = FTwoPIDivSpS * i;
            *pB++ = (BYTE) ((sin (d) >= 0) ? b1 : b0);
        }
    }
    else 
	 {
        PWORD pW = (PWORD) m_wWaveCache;

        for (i = 0; i < m_iWaveCacheSize; i++)
		  {
            d = FTwoPIDivSpS * i;
            *pW++ = (WORD) ((sin (d) >= 0) ? w1 : w0);
        }
    }
}

//
// CalcCacheSawtooth
//
void CAudioSynth::CalcCacheSawtooth (const WAVEFORMATEX& wfex)
{

    int i;
    double d;
    double amplitude;
    double FTwoPIDivSpS;
    double step;
    double curstep=0;
    BOOL fLastWasNeg = TRUE;
    BOOL fPositive;

    amplitude = ((wfex.wBitsPerSample == 8) ? 255 : 65535 )
                    * m_iAmplitude / 100;

    FTwoPIDivSpS = m_iFrequency * TWOPI / wfex.nSamplesPerSec;
    step = amplitude * m_iFrequency / wfex.nSamplesPerSec;

    m_iWaveCacheIndex = 0;
    m_iCurrentSample = 0;

    BYTE * pB = m_bWaveCache;
    PWORD pW = (PWORD) m_wWaveCache;

    for (i = 0; i < m_iWaveCacheSize; i++)
	 {
        d = FTwoPIDivSpS * i;

        // OneShot triggered on positive zero crossing
        fPositive = (sin (d) >= 0);

        if (fLastWasNeg && fPositive)
		  {
            if (wfex.wBitsPerSample == 8)
                curstep = 128 - amplitude / 2;
            else
                curstep = 32768 - amplitude / 2;
        }
        fLastWasNeg = !fPositive;

        if (wfex.wBitsPerSample == 8)
            *pB++ = (BYTE) curstep;
        else
            *pW++ = (WORD) (-32767 + curstep);

        curstep += step;
    }
}

//
// CalcCacheSweep
//
void CAudioSynth::CalcCacheSweep (const WAVEFORMATEX& wfex) 
{

    int i;
    double d;
    double amplitude;
    double FTwoPIDivSpS;
    double CurrentFreq;
    double DeltaFreq;

    amplitude = ((wfex.wBitsPerSample == 8) ? 127 : 32767 )
                    * m_iAmplitude / 100;

    DeltaFreq = ((double) m_iSweepEnd - m_iSweepStart) / m_iWaveCacheSize;
    CurrentFreq = m_iSweepStart;

    m_iWaveCacheIndex = 0;
    m_iCurrentSample = 0;

    if (wfex.wBitsPerSample == 8) 
	 {
        BYTE * pB = m_bWaveCache;
        d = 0.0;

        for (i = 0; i < m_iWaveCacheSize; i++) 
		  {
            FTwoPIDivSpS = (int) CurrentFreq * TWOPI / wfex.nSamplesPerSec;
            CurrentFreq += DeltaFreq;
            d += FTwoPIDivSpS;
            *pB++ = (BYTE) ((sin (d) * amplitude) + 128);
        }
    }
    else
	 {
        PWORD pW = (PWORD) m_wWaveCache;
        d = 0.0;

        for (i = 0; i < m_iWaveCacheSize; i++)
		  {
            FTwoPIDivSpS = (int) CurrentFreq * TWOPI / wfex.nSamplesPerSec;
            CurrentFreq += DeltaFreq;
            d += FTwoPIDivSpS;
            *pW++ = (WORD) (sin (d) * amplitude);
        }
    }
}

//
// get_Frequency
//
STDMETHODIMP CAudioSynth::get_Frequency(int *Frequency) 
{

    *Frequency = m_iFrequency;

    DbgLog((LOG_TRACE, 3, TEXT("get_Frequency: %d"), *Frequency));

    return NOERROR;
}


//
// put_Frequency
//
STDMETHODIMP CAudioSynth::put_Frequency(int Frequency) 
{

    CAutoLock l(m_pStateLock);

    m_iFrequency = Frequency;

    DbgLog((LOG_TRACE, 3, TEXT("put_Frequency: %d"), Frequency));

    return NOERROR;
}

//
// get_Waveform
//
STDMETHODIMP CAudioSynth::get_Waveform(int *Waveform) 
{

    *Waveform = m_iWaveform;

    DbgLog((LOG_TRACE, 3, TEXT("get_Waveform: %d"), *Waveform));

    return NOERROR;
}


//
// put_Waveform
//
STDMETHODIMP CAudioSynth::put_Waveform(int Waveform) 
{

    CAutoLock l(m_pStateLock);

    m_iWaveform = Waveform;

    DbgLog((LOG_TRACE, 3, TEXT("put_Waveform: %d"), Waveform));

    return NOERROR;
}

//
// get_Channels
//
STDMETHODIMP CAudioSynth::get_Channels(int *Channels) 
{

    *Channels = m_wChannels;

    DbgLog((LOG_TRACE, 3, TEXT("get_Channels: %d"), *Channels));

    return NOERROR;
}

//
// put_Channels
//
STDMETHODIMP CAudioSynth::put_Channels(int Channels) 
{

    CAutoLock l(m_pStateLock);

    m_wChannels = (WORD) Channels;
    return NOERROR;
}

//
// get_BitsPerSample
//
STDMETHODIMP CAudioSynth::get_BitsPerSample(int *BitsPerSample)
{

    *BitsPerSample = m_wBitsPerSample;

    DbgLog((LOG_TRACE, 3, TEXT("get_BitsPerSample: %d"), *BitsPerSample));

    return NOERROR;
}

//
// put_BitsPerSample
//
STDMETHODIMP CAudioSynth::put_BitsPerSample(int BitsPerSample) 
{

    CAutoLock l(m_pStateLock);

    m_wBitsPerSample = (WORD) BitsPerSample;
    return NOERROR;
}

//
// get_SamplesPerSec
//
STDMETHODIMP CAudioSynth::get_SamplesPerSec(int *SamplesPerSec) 
{

    *SamplesPerSec = m_dwSamplesPerSec;

    DbgLog((LOG_TRACE, 3, TEXT("get_SamplesPerSec: %d"), *SamplesPerSec));

    return NOERROR;
}

//
// put_SamplesPerSec
//
STDMETHODIMP CAudioSynth::put_SamplesPerSec(int SamplesPerSec) 
{

    CAutoLock l(m_pStateLock);

    m_dwSamplesPerSec = SamplesPerSec;
    return NOERROR;
}

//
// put_SynthFormat
//
STDMETHODIMP CAudioSynth::put_SynthFormat(int Channels, int BitsPerSample,
                      int SamplesPerSec) 
{

    CAutoLock l(m_pStateLock);

    m_wChannels = (WORD) Channels;
    m_wBitsPerSample = (WORD) BitsPerSample;
    m_dwSamplesPerSec = SamplesPerSec;

    DbgLog((LOG_TRACE, 1, TEXT("put_SynthFormat: %d-bit %d-channel %dHz"),
        BitsPerSample, Channels, SamplesPerSec));

    return NOERROR;
}


//
// get_Amplitude
//
STDMETHODIMP CAudioSynth::get_Amplitude(int *Amplitude) 
{
    *Amplitude =  m_iAmplitude;

    DbgLog((LOG_TRACE, 3, TEXT("get_Amplitude: %d"), *Amplitude));

    return NOERROR;
}


//
// put_Amplitude
//
STDMETHODIMP CAudioSynth::put_Amplitude(int Amplitude) 
{
    CAutoLock l(m_pStateLock);

    if (Amplitude > MaxAmplitude || Amplitude < MinAmplitude)
        return E_INVALIDARG;

    m_iAmplitude = Amplitude;

    DbgLog((LOG_TRACE, 3, TEXT("put_Amplitude: %d"), Amplitude));

    return NOERROR;
}


//
// get_SweepRange
//
STDMETHODIMP CAudioSynth::get_SweepRange(int *SweepStart, int *SweepEnd) 
{
    *SweepStart = m_iSweepStart;
    *SweepEnd = m_iSweepEnd;

    DbgLog((LOG_TRACE, 3, TEXT("get_SweepStart: %d %d"), *SweepStart, *SweepEnd));

    return NOERROR;
}


//
// put_SweepRange
//
STDMETHODIMP CAudioSynth::put_SweepRange(int SweepStart, int SweepEnd) 
{
    CAutoLock l(m_pStateLock);

    m_iSweepStart = SweepStart;
    m_iSweepEnd = SweepEnd;

    DbgLog((LOG_TRACE, 3, TEXT("put_SweepRange: %d %d"), SweepStart, SweepEnd));

    return NOERROR;
}


//
// get_OutputFormat
//
STDMETHODIMP CAudioSynth::get_OutputFormat(SYNTH_OUTPUT_FORMAT *pOutputFormat) 
{
    CheckPointer(pOutputFormat, E_POINTER);

    ValidateReadWritePtr(pOutputFormat, sizeof(SYNTH_OUTPUT_FORMAT));

    switch(m_wFormatTag) 
	 {
    
    case WAVE_FORMAT_PCM:
        *pOutputFormat = SYNTH_OF_PCM;
        break;

    case WAVE_FORMAT_ADPCM:
        *pOutputFormat = SYNTH_OF_MS_ADPCM;
        break;

    default:
        return E_UNEXPECTED;
    }

    return S_OK;
}


//
// put_OutputFormat
//
STDMETHODIMP CAudioSynth::put_OutputFormat(SYNTH_OUTPUT_FORMAT ofOutputFormat) {

    CAutoLock l(m_pStateLock);    

    switch(ofOutputFormat)
	 {
    
    case SYNTH_OF_PCM:
        m_wFormatTag = WAVE_FORMAT_PCM;
        break;

    case SYNTH_OF_MS_ADPCM:
        m_wFormatTag = WAVE_FORMAT_ADPCM;
        break;

    default:
        return E_INVALIDARG;
    }

    return S_OK;
}

/******************************Public*Routine******************************\
* exported entry points for registration and
* unregistration (in this case they only call
* through to default implmentations).
*
*
*
* History:
*
\**************************************************************************/
STDAPI
DllRegisterServer()
{
  return AMovieDllRegisterServer2( TRUE );
}

STDAPI
DllUnregisterServer()
{
  return AMovieDllRegisterServer2( FALSE );
}
