// PushSource.cpp: implementation of CPushSource class.
// 
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include <streams.h>     // DirectShow

#include "resource.h"
#include "PushSource.h"
#include "PushSourceStream.h"
#include "DirectBufferAllocator.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// *
// * --- CPushSourceStream ----
// *

//
// Set Id to point to a CoTaskMemAlloc'd
STDMETHODIMP CPushSourceStream::QueryId(LPWSTR *Id)
{
    CheckPointer(Id,E_POINTER);
    ValidateReadWritePtr(Id,sizeof(LPWSTR));

    // We give the pins id's which are 1,2,...
    // FindPinNumber returns -1 for an invalid pin
    int i = 1+ m_pFilter->FindPinNumber(this);
    if (i<1) return VFW_E_NOT_FOUND;
    *Id = (LPWSTR)CoTaskMemAlloc(8);
    if (*Id==NULL) 
	 {
       return E_OUTOFMEMORY;
    }
    IntToWstr(i, *Id);
    return NOERROR;
}



//
// CPushSourceStream::Constructor
//
// increments the number of pins present on the filter
CPushSourceStream::CPushSourceStream(TCHAR *pObjectName, HRESULT *phr, CPushSource*ps, LPCWSTR pPinName, const GUID*pPinCat, long lFlags)
    : CSourceStream(pObjectName, phr, (CSource*)ps, pPinName),
      m_pFilter(ps)
//     , m_fReconnectOutputPin(false)
{
	m_lPushSourceFlags  = lFlags;
	m_lDeliveredSamples = 0;
	ZERO_INIT(m_rtStreamOffset);
	ZERO_INIT(m_rtMaxStreamOffset);
	ZERO_INIT(m_rtLatency);
	ZERO_INIT(m_guidPIN_CATEGORY);
	if (pPinCat)
	{
		m_guidPIN_CATEGORY = *pPinCat;
	}
//	m_rtLatency = MILLISECONDS_TO_100NS_UNITS(66);
	m_rtMaxStreamOffset = 1000000; // 100 ms;
}

//
// CPushSourceStream::Destructor
//
// Decrements the number of pins on this filter
CPushSourceStream::~CPushSourceStream(void) 
{
	
}


//
// CheckMediaType
//
// Do we support this type? Provides the default support for 1 type.
HRESULT CPushSourceStream::CheckMediaType(const CMediaType *pMediaType) 
{
    CAutoLock lock(m_pFilter->pStateLock ());
	return m_pFilter->CheckMediaType(pMediaType);
}
//////////////////////////////////////////////////////////////////////////
HRESULT CPushSourceStream::GetMediaType(CMediaType *pmt)
{
	return GetMediaType(0, pmt);
}
// GetMediaType ask the filter
HRESULT CPushSourceStream::GetMediaType(int iPosition, CMediaType *pMediaType) 
{
    CAutoLock lock(m_pFilter->pStateLock ());
	return m_pFilter->GetMediaType(iPosition, pMediaType);
}
HRESULT CPushSourceStream::SetMediaType(const CMediaType *pM)
{
    CAutoLock lock(m_pFilter->pStateLock ());
	HRESULT hr = m_pFilter->SetCurrentMediaType(pM);
	if (SUCCEEDED(hr))
	{
		return CBasePin::SetMediaType(pM);
	}
	return hr;
}
// Active
// The pin is active - start up the worker thread
/*
HRESULT CPushSourceStream::Active(void) 
{
    return CSourceStream::Active();
}

HRESULT CPushSourceStream::Inactive(void) 
{
    return CSourceStream::Inactive();
}
*/
HRESULT CPushSourceStream::BreakConnect(void)
{
    CAutoLock lock(m_pFilter->pStateLock ());
    HRESULT hr = CSourceStream::BreakConnect();
	 // TODO! CPushSourceStream::BreakConnect
    if (FAILED(hr)) 
	 {
        return hr;
    }
//    m_fReconnectOutputPin = false;
    return S_OK;
}

HRESULT CPushSourceStream::DestroySourceThread(void)
{
    // The pin's thread cannot be destroyed if the thread does not exist.
    ASSERT( ThreadExists() );

    HRESULT hr = Stop();
    if (FAILED(hr)) 
	 {
        return hr;
    }

    hr = Exit();
    if (FAILED(hr)) 
	 {
        return hr;
    }

    Close();    // Wait for the thread to exit, then tidy up.
    
    return NOERROR;
}

///////////////////////////////////////////////////////////////////////
// NonDelegatingQueryInterface:
///////////////////////////////////////////////////////////////////////
STDMETHODIMP CPushSourceStream::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	CheckPointer(ppv,E_POINTER);

	if (riid == IID_IAMPushSource) 
	{
		return GetInterface((IAMPushSource *) this, ppv);
	}
	else if (riid == IID_IKsPropertySet) 
	{
		return GetInterface((IKsPropertySet*) this, ppv);
	}
	else if (riid == IID_IAMStreamControl) 
	{
		return GetInterface((IAMStreamControl*) this, ppv);
	}
	else if (riid == IID_IAMStreamConfig) 
	{
		return GetInterface((IAMStreamConfig*) this, ppv);
	}
	else 
	{
		return CSourceStream::NonDelegatingQueryInterface(riid, ppv);
	}
}

//
// ThreadProc
//
// When this returns the thread exits
// Return codes > 0 indicate an error occured
DWORD CPushSourceStream::ThreadProc(void) 
{
    HRESULT hr;  // the return code from calls
    Command com;

    do 
	 {
        com = GetRequest();
        if (com != CMD_INIT) 
		 {
			RELEASE_TRACE(_T("Thread expected init command"));
//            DbgLog((LOG_ERROR, 1, TEXT("Thread expected init command")));
            Reply((DWORD) E_UNEXPECTED);
        }
    } while (com != CMD_INIT);

    DbgLog((LOG_TRACE, 1, TEXT("CPushSourceStream worker thread initializing")));

    hr = OnThreadCreate(); // perform set up tasks
    if (FAILED(hr)) 
	{
		RELEASE_TRACE(_T("CPushSourceStream::OnThreadCreate failed. Aborting thread."));
//        DbgLog((LOG_ERROR, 1, TEXT("CPushSourceStream::OnThreadCreate failed. Aborting thread.")));
        OnThreadDestroy();
        Reply(hr);  // send failed return code from OnThreadCreate
        return 1;
    }

    // Initialisation suceeded
    Reply(NOERROR);

    Command cmd;
    do 
	 {
        cmd = GetRequest();

        switch (cmd) 
		  {

        case CMD_EXIT:
            Reply(NOERROR);
            break;

        case CMD_RUN:
			RELEASE_TRACE(_T("CMD_RUN received before a CMD_PAUSE???"));
//            DbgLog((LOG_ERROR, 1, TEXT("CMD_RUN received before a CMD_PAUSE???")));
            // !!! FALLTHROUGH ???
        
        case CMD_PAUSE:
            Reply(NOERROR);
            DoBufferProcessingLoop();
            break;

        case CMD_STOP:
            Reply(NOERROR);
            break;

        default:
			RELEASE_TRACE(_T("Unknown command %d received!"));
//            DbgLog((LOG_ERROR, 1, TEXT("Unknown command %d received!"), cmd));
            Reply((DWORD) E_NOTIMPL);
            break;
        }
    } while (cmd != CMD_EXIT);

    hr = OnThreadDestroy(); // tidy up.
    if (FAILED(hr)) 
	{
		CString str;
		str.Format(_T("CPushSourceStream::OnThreadDestroy failed (%08x). Exiting thread."), hr);
		RELEASE_TRACE(str);
//        DbgLog((LOG_ERROR, 1, TEXT("CPushSourceStream::OnThreadDestroy failed. Exiting thread.")));
        return 1;
    }

    DbgLog((LOG_TRACE, 1, TEXT("CPushSourceStream worker thread exiting")));
    return 0;
}
//////////////////////////////////////////////////////////////////////////
// DoBufferProcessingLoop
//
// Grabs a buffer and calls the users processing function.
// Overridable, so that different delivery styles can be catered for.
HRESULT CPushSourceStream::DoBufferProcessingLoop(void) 
{
    Command com;
    bool fOutputFormatChanged = false;

	// ASSERT(FALSE);
    OnThreadStartPlay();

    do 
	 {
        while (!CheckRequest(&com)) 
		  {
            HRESULT hr = S_OK;
        
            IMediaSample *pSample;
            hr = GetDeliveryBuffer(&pSample,NULL,NULL,0);
            if (FAILED(hr)) 
				{
                Sleep(1);
					 
                continue;   // go round again. Perhaps the error will go away
                            // or the allocator is decommited & we will be asked to
                            // exit soon.
            }

            if( fOutputFormatChanged ) 
				{
                pSample->SetDiscontinuity(TRUE);
                fOutputFormatChanged = false;
            }

            // Virtual function user will override.
            hr = FillBuffer(pSample);

            if (hr == S_OK) 
			{
                hr = Deliver(pSample);
                pSample->Release();

                // downstream filter returns S_FALSE if it wants us to
                // stop or an error if it's reporting an error.
                if(hr != S_OK)
                {
					CString str;
					str.Format(_T("Deliver() returned %08x; stopping!\n"), hr);
					RELEASE_TRACE(str);
//                    DbgLog((LOG_TRACE, 2, TEXT("Deliver() returned %08x; stopping"), hr));
                    return S_OK;
                }

            }
			else if (hr == S_FALSE) 
			{
                // derived class wants us to stop pushing data
                pSample->Release();
                DeliverEndOfStream();
                return S_OK;
            }
			else 
			{
                // derived class encountered an error
                pSample->Release();
				CString str;
				str.Format(_T("Error %08lX from FillBuffer!!!"), hr);
				RELEASE_TRACE(str);

                FatalError(hr);

                return hr;
            }
                // all paths release the sample
        }

        // For all commands sent to us there must be a Reply call!
        if (com == CMD_RUN || com == CMD_PAUSE) 
		  {
            Reply(NOERROR);
        } 
		  else if (com != CMD_STOP) 
		  {
            Reply((DWORD) E_UNEXPECTED);
			RELEASE_TRACE(_T("Unexpected command!!!"));
//            DbgLog((LOG_ERROR, 1, TEXT("Unexpected command!!!")));
        }
    } while (com != CMD_STOP);

    return S_FALSE;
}
//////////////////////////////////////////////////////////////////////////
void CPushSourceStream::FatalError(HRESULT hr)
{
    // Make sure the user is reporting a failure.
    ASSERT(FAILED(hr));

    m_bRunTimeError = TRUE;
    DeliverEndOfStream();
    m_pFilter->NotifyEvent(EC_ERRORABORT, hr, 0);
}
/*
void CPushSourceStream::OutputPinNeedsToBeReconnected(void)
{
    m_fReconnectOutputPin = true;
}
*/
//////////////////////////////////////////////////////////////////////////
HRESULT CPushSourceStream::DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *pProperties)
{
	ASSERT(CritCheckIn(m_pFilter->pStateLock ()));
	m_pFilter->DecideBufferSize(pAlloc, pProperties);

	return NOERROR;
}
//////////////////////////////////////////////////////////////////////////
HRESULT CPushSourceStream::CompleteConnect(IPin *pReceivePin)
{
    CAutoLock lock(m_pFilter->pStateLock ());
	 HRESULT hr = CSourceStream::CompleteConnect(pReceivePin);
	 if (SUCCEEDED(hr))
	 {
		 // TODO! CPushSourceStream::CompleteConnect
	 }
	 return hr;
}
//////////////////////////////////////////////////////////////////////////
HRESULT CPushSourceStream::FillBuffer(IMediaSample *pms) 
{
   CAutoLock lock(m_pFilter->pStateLock ());
	if (m_pFilter->m_nBufferType)		// external buffers
	{
		REFERENCE_TIME rtStart, rtEnd, rtSampleDuration;
		CRefTime       rtStreamTime, rtScheduledTime;
		bool  bTimePresent = false;
		if (m_guidPIN_CATEGORY == PIN_CATEGORY_PREVIEW)
		{
			pms->SetTime(NULL, NULL);
		}
		else if (m_pFilter->GetClock())
		{
			if (m_pFilter->DoGenerateTimeStamps())
			{
				HRESULT hr;

				AM_MEDIA_TYPE *pmt;
				hr = pms->GetMediaType(&pmt);
				if (SUCCEEDED(hr))								// determine SampleDuration
				{
					if (pmt->formattype == FORMAT_WaveFormatEx)
					{
						WAVEFORMATEX *pWfEx = (WAVEFORMATEX *) pmt->pbFormat;
						if (pWfEx->wFormatTag == WAVE_FORMAT_PCM)// Calculate it
						{
							rtSampleDuration = (REFERENCE_TIME)(UNITS * pms->GetActualDataLength()) / (REFERENCE_TIME)pWfEx->nAvgBytesPerSec;
						}
						else									// get from Sample
						{
							pms->GetTime(&rtStart, &rtEnd);
							rtSampleDuration = rtEnd - rtStart;
						}
					}
					else if (   (pmt->formattype == FORMAT_VideoInfo )
								|| (pmt->formattype == FORMAT_VideoInfo2)
								|| (pmt->formattype == FORMAT_MPEGVideo )
								|| (pmt->formattype == FORMAT_MPEG2Video ))
					{
						VIDEOINFOHEADER *pVIH = (VIDEOINFOHEADER*) pmt->pbFormat;
						rtSampleDuration = pVIH->AvgTimePerFrame;// Videoframes do contain this information
					}
				}
				
				//////////////////////////////////////////////////////////////////////////
				// Calculation Part
				// calculate the Sample Times
				if (m_pFilter->m_tStart == 0)					// initialize Reference time if nesessary
				{
					m_pFilter->GetClock()->GetTime((LONGLONG*)&m_pFilter->m_tStart);
					m_lDeliveredSamples = 0;
				}

				m_pFilter->StreamTime(rtStreamTime);			// get the StreamTime

				if (m_pFilter->m_rtStartTime == 0)				// initialize the Start time if nessesary
				{
					m_pFilter->m_rtStartTime = rtStreamTime;
				}
				
				if (m_pFilter->IsLiveSource())					// Live source:
				{
					rtStart = rtStreamTime + m_rtStreamOffset;	// SampleTime = now + StreamOffset
				}
				else if (m_pFilter->DoInternalRM())				// internal Ratematching: ?
				{
					rtStart = (rtSampleDuration * m_lDeliveredSamples) + m_rtStreamOffset;
				}
				else											// Scheduled Time:
				{
					if (pms->IsSyncPoint() == S_OK)				// Sync point:
					{
						CString str;							// Trace out
						str.Format(_T("SyncPoint:%d"), rtStreamTime.Millisecs());
						RELEASE_TRACE(str);
						m_pFilter->m_rtStartTime = rtStreamTime;// StreamTime is now
						rtScheduledTime = rtStreamTime;			// ScheduledTime is now
					}
					else										// no sync point:
					{
						rtScheduledTime  = m_pFilter->m_rtStartTime;// ScheduledTime is the continued StartTime
					}
					
					rtStart = rtScheduledTime + m_rtStreamOffset;// SampleTime = ScheduledTime + StreamOffset
				}
				if (m_pFilter->GetOffsetInMS())					// additional offset:
				{
					rtStart += MILLISECONDS_TO_100NS_UNITS(m_pFilter->GetOffsetInMS());
				}
				
				rtEnd = rtStart + rtSampleDuration;				// EndTime = SampleTime + SampleDuration

				pms->SetTime(&rtStart, &rtEnd);
				bTimePresent = true;
				
#ifdef _DEBUG
				CString str;									// Trace out
				CRefTime rt = m_pFilter->m_tStart + rtStart;
				str.Format(_T("Start:%d, End:%d, Dur:%d, Streamtime %d"),
					UNITS_100NS_TO_MILLISECONDS(rtStart),
					UNITS_100NS_TO_MILLISECONDS(rtEnd),
					UNITS_100NS_TO_MILLISECONDS(rtSampleDuration),
					UNITS_100NS_TO_MILLISECONDS(rtStreamTime)
					);
				DEBUG_TRACE(str);
#endif
				if (m_pFilter->m_pRefClock)						// control scheduling with the own clock
				{
					DWORD dwAdviceCount = m_pFilter->m_pRefClock->GetSchedule()->GetAdviseCount( );
					if (dwAdviceCount)
					{
						m_pFilter->m_nAdvices++;
						if (m_pFilter->m_nAdvices > 2)
						{
							RELEASE_TRACE(_T("TriggerThread\n"));
							m_pFilter->m_pRefClock->TriggerThread();
						}
					}
					else
					{
						m_pFilter->m_nAdvices = 0;
					}
				}

				m_pFilter->m_rtStartTime += rtSampleDuration;	// continue StartTime

				//////////////////////////////////////////////////////////////////////////
				// Control Part
				// verify the times and correct if nesessary
				rtScheduledTime = rtStart - rtStreamTime;
				int nDifference = rtScheduledTime.Millisecs();
				if (   (nDifference <    0)					    // too late
					|| (nDifference > m_pFilter->m_nSampleToEarly)) // or much too early
				{
					CString str;								// Trace out
					str.Format(_T("Streamtime:%d, Diff:%d, StreamOffset:%d, FilterOffset:%d [ms]"), rtStreamTime.Millisecs(), nDifference, UNITS_100NS_TO_MILLISECONDS(m_rtStreamOffset), m_pFilter->GetOffsetInMS());
					RELEASE_TRACE(str);
					ZERO_INIT(rtStart);
					bTimePresent = false;

					if (nDifference < 0 && pms->IsSyncPoint() == S_OK)// correct if negative and synchpoint
					{
						RELEASE_TRACE(_T("negative and syncpoint, setting StreamOffset"));
						rtStart = rtStreamTime + MILLISECONDS_TO_100NS_UNITS(m_pFilter->GetOffsetInMS()) + m_rtStreamOffset;
						m_pFilter->SetStreamOffset(MILLISECONDS_TO_100NS_UNITS(-nDifference));
					}
					else if (nDifference < -m_pFilter->m_nSampleToLate)// if too late
					{											// calculate 
						RELEASE_TRACE(_T("too late"));
						rtStart = rtStreamTime + MILLISECONDS_TO_100NS_UNITS(m_pFilter->GetOffsetInMS()) + m_rtStreamOffset;
					}
					else if (nDifference > 0) 					// or calculate if positive
					{
						CString str;// Trace out
						CRefTime told = m_pFilter->m_tStart;
						m_pFilter->GetClock()->GetTime((LONGLONG*)&m_pFilter->m_tStart);
						m_pFilter->StreamTime(rtStreamTime);			// get the StreamTime
						rtStart = rtStreamTime + MILLISECONDS_TO_100NS_UNITS(m_pFilter->GetOffsetInMS()) + m_rtStreamOffset;

						str.Format(_T("too early, %s, (%d, %d):%d"), pms->IsSyncPoint() == S_OK ? _T("Sync"): _T("No Sync"), told.Millisecs(), m_pFilter->m_tStart.Millisecs(), rtStreamTime.Millisecs());
						RELEASE_TRACE(str);
					}
					if (rtStart != 0)
					{
						rtEnd = rtStart + rtSampleDuration;		// EndTime = SampleTime + SampleDuration
						pms->SetTime(&rtStart, &rtEnd);
						
						str.Format(_T("set new times:Start:%d, End:%d, Dur:%d"),
									UNITS_100NS_TO_MILLISECONDS(rtStart),
									UNITS_100NS_TO_MILLISECONDS(rtEnd),
									UNITS_100NS_TO_MILLISECONDS(rtSampleDuration));
						RELEASE_TRACE(str);
						bTimePresent = true;

						// new start time is end time of last sample - Offsets, because offsets are added in (Calculation part)
						m_pFilter->m_rtStartTime = rtEnd - (MILLISECONDS_TO_100NS_UNITS(m_pFilter->GetOffsetInMS()) + m_rtStreamOffset);
					}
				}

			}
			m_lDeliveredSamples++;								// increment delivered samples
		}


		if (m_pFilter->GetPropertyPageHWND())
		{
			if (!bTimePresent)
			{
				pms->GetTime((REFERENCE_TIME*)&rtStreamTime, &rtEnd);
			}
			else
			{
				rtStreamTime = rtStart;
			}
			SetDlgItemInt(m_pFilter->GetPropertyPageHWND(), IDC_EDT_MEDIA_TIME, rtStreamTime.Millisecs(), FALSE);
		}
	}
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
HRESULT CPushSourceStream::InitAllocator(IMemAllocator **ppAlloc)
{
	return CSourceStream::InitAllocator(ppAlloc);
}
//////////////////////////////////////////////////////////////////////////
HRESULT CPushSourceStream::GetDeliveryBuffer(IMediaSample ** ppSample, REFERENCE_TIME * pStartTime, REFERENCE_TIME * pEndTime, DWORD dwFlags)
{
	if (m_pFilter->m_nBufferType > 0)							// external buffer
	{															// are delivered by the filter
		return m_pFilter->GetDeliveryBuffer(ppSample, pStartTime, pEndTime, dwFlags);
	}
	return CSourceStream::GetDeliveryBuffer(ppSample,pStartTime,pEndTime,dwFlags);
}
//////////////////////////////////////////////////////////////////////////
HRESULT CPushSourceStream::DecideAllocator(IMemInputPin * pPin, IMemAllocator ** ppAlloc)
{
	HRESULT hr = NOERROR;
	if (m_pFilter->m_nBufferType > 0)							// external buffer
	{
		ALLOCATOR_PROPERTIES prop;
		ZeroMemory(&prop, sizeof(prop));
		BOOL bPreferOwnAllocator = m_pFilter->PreferOwnAllocator();
		if (!bPreferOwnAllocator)
		{									// Prefer own for Attached or copied buffers
			bPreferOwnAllocator = TRUE;		// to avoid copying data too often
			if (m_pFilter->m_nBufferType == BUFFER_FIXED)
			{
				bPreferOwnAllocator = FALSE;// Prefer InputPins allocator for fixed buffers
			}								// to release the as soon as possible
		}

		if (bPreferOwnAllocator)
		{
			hr = m_pFilter->CreateDirectAllocator();			//	create the direct allocator and set its properties
			if (SUCCEEDED(hr))
			{													// notify this allocator to the receiving pin
				hr = pPin->NotifyAllocator(m_pFilter->m_pDirectAllocator, FALSE);
			}
			if (SUCCEEDED(hr))									// accepted
			{														
				*ppAlloc = m_pFilter->m_pDirectAllocator;		// take this allocator
				(*ppAlloc)->AddRef();							// and save the reference
				return hr;										// return 
			}
			else												// not accepted
			{
				m_pFilter->ReleaseDirectAllocator();			// release it
			}
		}
	}

	// default allocator first tries to select the allocator of the input pin
	hr = CSourceStream::DecideAllocator(pPin, ppAlloc);			// take the default allocater

	return hr;
}
//////////////////////////////////////////////////////////////////////////
HRESULT CPushSourceStream::OnThreadCreate(void)
{
	return NOERROR;
}
//////////////////////////////////////////////////////////////////////////
HRESULT CPushSourceStream::OnThreadDestroy(void)
{
	return NOERROR;
}
//////////////////////////////////////////////////////////////////////////
HRESULT CPushSourceStream::OnThreadStartPlay(void) 
{
	return NOERROR;
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CPushSourceStream::GetPushSourceFlags(ULONG *plFlags)
{
	DEBUG_TRACE(_T("CPushSourceStream::GetPushSourceFlags"));
	if (plFlags == NULL) return E_POINTER;
	*plFlags = m_lPushSourceFlags;
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CPushSourceStream::SetPushSourceFlags(ULONG lFlags)
{
	DEBUG_TRACE(_T("CPushSourceStream::SetPushSourceFlags"));

//	AM_PUSHSOURCECAPS_INTERNAL_RM:
//	The filter uses its own rate-matching mechanism;
//	the renderer should therefore not attempt to match rates with this filter.

//	AM_PUSHSOURCECAPS_NOT_LIVE:
//	The filter is not live. Do not treat it as a live source,
//	even though it exposes the IAMPushSource interface.

//	AM_PUSHSOURCECAPS_PRIVATE_CLOCK
//	Reserved; do not use.

//	AM_PUSHSOURCEREQS_USE_STREAM_CLOCK
//	Reserved; do not use.

	// Reserved; do not use.
	lFlags &= ~(AM_PUSHSOURCECAPS_PRIVATE_CLOCK|AM_PUSHSOURCEREQS_USE_STREAM_CLOCK);

	m_lPushSourceFlags = lFlags;
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CPushSourceStream::SetStreamOffset(REFERENCE_TIME rtOffset)
{
	DEBUG_TRACE(_T("CPushSourceStream::SetStreamOffset:"));
	m_rtStreamOffset = rtOffset;
/*
#ifdef _DEBUG
	CString str;
	double dMs = 1e-7 * m_rtStreamOffset;
	str.Format("%f", dMs);
	DEBUG_TRACE(str);
#endif
*/
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CPushSourceStream::GetStreamOffset(REFERENCE_TIME *prtOffset)
{
	DEBUG_TRACE(_T("CPushSourceStream::GetStreamOffset"));
	if (prtOffset == NULL) return E_POINTER;
	*prtOffset = m_rtStreamOffset;
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CPushSourceStream::GetMaxStreamOffset(REFERENCE_TIME *prtMaxOffset)
{
	DEBUG_TRACE(_T("CPushSourceStream::GetMaxStreamOffset"));
	if (prtMaxOffset == NULL) return E_POINTER;
	*prtMaxOffset = m_rtMaxStreamOffset;
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CPushSourceStream::SetMaxStreamOffset(REFERENCE_TIME rtMaxOffset)
{
	DEBUG_TRACE(_T("CPushSourceStream::SetMaxStreamOffset"));
	m_rtMaxStreamOffset = rtMaxOffset;
/*	
#ifdef _DEBUG
	CString str;
	double dMs = 1e-7 * m_rtMaxStreamOffset;
	str.Format("%f", dMs);
	DEBUG_TRACE(str);
#endif
*/
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CPushSourceStream::GetLatency(REFERENCE_TIME *prtLatency)
{
	DEBUG_TRACE(_T("CPushSourceStream::GetLatency"));
	if (prtLatency == NULL) return E_POINTER;
	*prtLatency = m_rtLatency;
	return S_OK;
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
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CPushSourceStream::Set(REFGUID guidPropSet,DWORD dwPropID,
												LPVOID pInstanceData, DWORD cbInstanceData,
												LPVOID pPropData,DWORD cbPropData)
{
	if (guidPropSet != AMPROPSETID_Pin)
	{
		RELEASE_TRACE(_T("CPushSourceStream::Set"));
		RELEASE_TRACE(GetString(guidPropSet));
		return E_PROP_SET_UNSUPPORTED;
	}
	if (pPropData == NULL)					return E_POINTER;

	if (dwPropID == AMPROPERTY_PIN_CATEGORY)
	{
		if (cbPropData != sizeof(GUID))		return E_UNEXPECTED;
		m_guidPIN_CATEGORY = *(GUID *)pPropData;
	}
	else
	{
		return E_PROP_ID_UNSUPPORTED;
	}

	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CPushSourceStream::Get(REFGUID guidPropSet, DWORD dwPropID, LPVOID pInstanceData, DWORD cbInstanceData, LPVOID pPropData, DWORD cbPropData, DWORD *pcbReturned)
{
	if (guidPropSet != AMPROPSETID_Pin)
	{
		DEBUG_TRACE(_T("CPushSourceStream::Get"));
		DEBUG_TRACE(GetString(guidPropSet));
		return E_PROP_SET_UNSUPPORTED;
	}

	if (pPropData == NULL && pcbReturned == NULL)	return E_POINTER;

	if (dwPropID == AMPROPERTY_PIN_CATEGORY)
	{
		if (pcbReturned) *pcbReturned = sizeof(GUID);
		if (pPropData == NULL)						return S_OK;
		if (cbPropData < sizeof(GUID))				return E_UNEXPECTED;

		*(GUID *)pPropData = m_guidPIN_CATEGORY;
	}
	else
	{
		return E_PROP_ID_UNSUPPORTED;
	}


	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CPushSourceStream::QuerySupported(REFGUID guidPropSet,DWORD dwPropID,DWORD *pTypeSupport)
{
	if (guidPropSet != AMPROPSETID_Pin)
	{
		RELEASE_TRACE(_T("CPushSourceStream::QuerySupported"));
		RELEASE_TRACE(GetString(guidPropSet));
		return E_PROP_SET_UNSUPPORTED;
	}

	if (dwPropID == AMPROPERTY_PIN_CATEGORY)
	{
		if (pTypeSupport)	*pTypeSupport = KSPROPERTY_SUPPORT_GET|KSPROPERTY_SUPPORT_SET; 
	}
	else
	{
		return E_PROP_ID_UNSUPPORTED;
	}

	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
HRESULT CPushSourceStream::GetFormat(AM_MEDIA_TYPE **pmt)
{
	return m_pFilter->GetSourceMediaType(pmt);
}
//////////////////////////////////////////////////////////////////////////
HRESULT CPushSourceStream::GetNumberOfCapabilities(int *piCount, int *piSize)
{
	if (piCount == NULL) return E_POINTER;
	if (piSize == NULL) return E_POINTER;
	*piCount = 0;
	*piSize = 0;
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
HRESULT CPushSourceStream::GetStreamCaps(int iIndex, AM_MEDIA_TYPE **pmt, BYTE *pSCC)
{
	return S_FALSE;
}
//////////////////////////////////////////////////////////////////////////
HRESULT CPushSourceStream::SetFormat(AM_MEDIA_TYPE *pmt)
{
	return m_pFilter->SetCurrentMediaType(pmt);
}


