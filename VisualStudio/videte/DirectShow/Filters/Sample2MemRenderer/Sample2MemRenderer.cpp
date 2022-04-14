// Sample2MemRenderer.cpp: implementation of CSample2MemRenderer class.
// 
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include <streams.h>     // DirectShow
#include <initguid.h>    // DEFINE_GUID to declare an EXTERN_C const.
#include <stdio.h>

#include "iSample2MemRenderer.h"
#include "Sample2MemRenderer.h"
#include "Sample2MemRendererProp.h"
#include "Samp2MemInputPin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define THE_FILTERS_NAME L"Sample2MemRenderer Filter"

// setup data - allows the self-registration to work.
const AMOVIESETUP_MEDIATYPE sudPinTypes =
{ &MEDIATYPE_NULL        // clsMajorType
, &MEDIASUBTYPE_NULL };  // clsMinorType

const AMOVIESETUP_PIN psudPins[] =
{ { L"Input"            // strName
  , TRUE					  // bRendered
  , FALSE               // bOutput
  , FALSE               // bZero
  , FALSE               // bMany
  , &CLSID_NULL         // clsConnectsToFilter
  , L""                 // strConnectsToPin
  , 1                   // nTypes
  , &sudPinTypes        // lpTypes
  }
};

const AMOVIESETUP_FILTER sudSample2MemRenderer =
{ &CLSID_Sample2MemRenderer              // clsID
, THE_FILTERS_NAME                    // strName
, MERIT_DO_NOT_USE                  // dwMerit
, 1                                 // nPins
, psudPins };                       // lpPin

// Needed for the CreateInstance mechanism
CFactoryTemplate g_Templates[]=
    {   { THE_FILTERS_NAME
        , &CLSID_Sample2MemRenderer
        , CSample2MemRenderer::CreateInstance
        , NULL
        , &sudSample2MemRenderer },
        { THE_FILTERS_NAME L" Properties"
        , &CLSID_Sample2MemRendererProp
        , CSample2MemRendererProp::CreateInstance }
    };
int g_cTemplates = sizeof(g_Templates)/sizeof(g_Templates[0]);

///////////////////////////////////////////////////////////////////////
// CreateInstance: for COM to create a CSample2MemRenderer object
///////////////////////////////////////////////////////////////////////
CUnknown * WINAPI CSample2MemRenderer::CreateInstance(LPUNKNOWN punk, HRESULT *phr) {

    CSample2MemRenderer *pNewObject = new CSample2MemRenderer(NAME("Sample2MemRenderer"), punk, phr );
    if (pNewObject == NULL)
	 {
       *phr = E_OUTOFMEMORY;
    }

    return pNewObject;
}

///////////////////////////////////////////////////////////////////////
// CSample2MemRenderer: Constructor
///////////////////////////////////////////////////////////////////////
CSample2MemRenderer::CSample2MemRenderer(TCHAR *tszName, LPUNKNOWN punk, HRESULT *phr)
	: CBaseRenderer (CLSID_Sample2MemRenderer, tszName, punk, phr), 
      CPersistStream(punk, phr)
{
   readConfig();                  // read default value from registry
   SetParams(&m_Sample2MemRendererParams); // Set Filter parameters
	m_nPackageNo    = 0;
	m_pReceiveFnc   = NULL;
	m_lRFParam      = 0;
	m_ppMediaTypes  = NULL;
	m_pRenderedType = NULL;
	m_nMediaTypes   = 0;
	m_bEnumerate    = FALSE;
	ZeroMemory(&m_AllocProps, sizeof(ALLOCATOR_PROPERTIES));

   m_trThrottle = 0;
	if (SUCCEEDED(*phr))
	{
		m_pInputPin = new CSamp2MemInputPin(this, phr, L"Input");
	}
}

///////////////////////////////////////////////////////////////////////
// ~CSample2MemRenderer: Destructor
///////////////////////////////////////////////////////////////////////
CSample2MemRenderer::~CSample2MemRenderer()
{
	SetAcceptedMediaTypes(NULL, 0, FALSE);
	SetMediaType(NULL);
}
#define SAMPLE2MEMRENDERER_PARAM	_T("Sample2MemRenderer")
#define PARAM_MISC_FLAGS			_T("MiscFlags")
#define PARAM_SCHEDULE			   _T("Schedule")

///////////////////////////////////////////////////////////////////////
// saveConfig: write the parameters into the registry
///////////////////////////////////////////////////////////////////////
void CSample2MemRenderer::saveConfig()
{
   char sz[STR_MAX_LENGTH];
	sprintf(sz, "%d", m_Sample2MemRendererParams.m_lMiscFlags);
	WriteProfileStringA(SAMPLE2MEMRENDERER_PARAM, PARAM_MISC_FLAGS, sz);
	sprintf(sz, "%d", m_Sample2MemRendererParams.m_bScheduleSamples);
	WriteProfileStringA(SAMPLE2MEMRENDERER_PARAM, PARAM_SCHEDULE, sz);
}

///////////////////////////////////////////////////////////////////////
// readConfig: read the parameters from the registry
///////////////////////////////////////////////////////////////////////
void CSample2MemRenderer::readConfig()
{
   m_Sample2MemRendererParams.m_lMiscFlags  = GetProfileInt(SAMPLE2MEMRENDERER_PARAM, PARAM_MISC_FLAGS, AM_FILTER_MISC_FLAGS_IS_RENDERER);
	m_Sample2MemRendererParams.m_bScheduleSamples = GetProfileInt(SAMPLE2MEMRENDERER_PARAM, PARAM_SCHEDULE, 1);
}

///////////////////////////////////////////////////////////////////////
// NonDelegatingQueryInterface:
//   Reveals ISample2MemRenderer, ISpecifyPropertyPages, IPersistStream
///////////////////////////////////////////////////////////////////////
STDMETHODIMP CSample2MemRenderer::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
    CheckPointer(ppv,E_POINTER);

    if (riid == IID_ISample2MemRenderer) 
	 {
        return GetInterface((ISample2MemRenderer *) this, ppv);
    }
	 else if (riid == IID_IPersistStream) 
	 {
        return GetInterface((IPersistStream *) this, ppv);
    }
	 else if (riid == IID_ISpecifyPropertyPages) 
	 {
        return GetInterface((ISpecifyPropertyPages *) this, ppv);
    }
	 else if (riid == IID_IAMFilterMiscFlags) 
	 {
        return GetInterface((IAMFilterMiscFlags *) this, ppv);
    }
	 else if (riid == IID_IQualityControl) 
	 {
        return GetInterface((IQualityControl *) this, ppv);
    }
	 else 
	 {
        return CBaseRenderer::NonDelegatingQueryInterface(riid, ppv);
    }
}

///////////////////////////////////////////////////////////////////////
// GetParams: Get the filter parameters to a given destination
///////////////////////////////////////////////////////////////////////
STDMETHODIMP CSample2MemRenderer::GetParams(Sample2MemRendererParams *irp)
{
    CAutoLock cAutolock (&m_Sample2MemRendererLock);
    CheckPointer(irp,E_POINTER);

    *irp = m_Sample2MemRendererParams;

    return NOERROR;
}

///////////////////////////////////////////////////////////////////////
// SetParams: Set the filter parameters
///////////////////////////////////////////////////////////////////////
STDMETHODIMP CSample2MemRenderer::SetParams(Sample2MemRendererParams *irp)
{
    CAutoLock cAutolock (&m_Sample2MemRendererLock);
    m_Sample2MemRendererParams = *irp;
    SetDirty(TRUE);

    // Save the config whenever the use changes it
    saveConfig();

    return NOERROR;
} 

#define WRITEOUT(var)  hr = pStream->Write(&var, sizeof(var), NULL); \
               if (FAILED(hr)) return hr;

#define READIN(var)    hr = pStream->Read(&var, sizeof(var), NULL); \
               if (FAILED(hr)) return hr;

///////////////////////////////////////////////////////////////////////
// GetClassID: This is the only method of IPersist
///////////////////////////////////////////////////////////////////////
STDMETHODIMP CSample2MemRenderer::GetClassID(CLSID *pClsid)
{
    return CBaseFilter::GetClassID(pClsid);
}

///////////////////////////////////////////////////////////////////////
// WriteToStream: called when save the filter to stream
///////////////////////////////////////////////////////////////////////
HRESULT CSample2MemRenderer::WriteToStream(IStream *pStream)
{
	HRESULT hr;
   WRITEOUT(m_Sample2MemRendererParams);
   return NOERROR;
}
///////////////////////////////////////////////////////////////////////
// ReadFromStream: called when read from stream
///////////////////////////////////////////////////////////////////////
HRESULT CSample2MemRenderer::ReadFromStream(IStream *pStream)
{
	HRESULT hr;
   READIN(m_Sample2MemRendererParams);
   return NOERROR;
}
///////////////////////////////////////////////////////////////////////
// GetPages: Returns the clsid's of the property pages we support
///////////////////////////////////////////////////////////////////////
STDMETHODIMP CSample2MemRenderer::GetPages(CAUUID *pPages)
{
    pPages->cElems = 1;
    pPages->pElems = (GUID *) CoTaskMemAlloc(sizeof(GUID));
    if (pPages->pElems == NULL)
	 {
        return E_OUTOFMEMORY;
    }
    *(pPages->pElems) = CLSID_Sample2MemRendererProp;
    return NOERROR;
}
//////////////////////////////////////////////////////////////////////////
HRESULT CSample2MemRenderer::DoRenderSample(IMediaSample *pMediaSample)
{
	if (m_nPackageNo == 0)
	{
		IMemAllocator *pMA = NULL;
		CBaseInputPin *pPin = (CBaseInputPin*)GetPin(0);
		HRESULT hr = pPin->GetAllocator(&pMA);
		if (SUCCEEDED(hr))
		{
			pMA->GetProperties(&m_AllocProps);
			pMA->Release();
		}
	}
#ifdef _DEBUG
	REFERENCE_TIME rtStart, rtEnd, rtCurrent;
	pMediaSample->GetTime(&rtStart, &rtEnd);
	SIMPLE_TRACE("Target:");
	SIMPLE_TRACE(CDisp(rtStart, CDISP_TIME));
	SIMPLE_TRACE(":");
	SIMPLE_TRACE(CDisp(rtEnd, CDISP_TIME));
	if (m_pClock)
	{
		m_pClock->GetTime(&rtCurrent);
		SIMPLE_TRACE(", current:");
		SIMPLE_TRACE(CDisp(rtCurrent, CDISP_TIME));
	}
	SIMPLE_TRACE("\n");
#endif

	if (m_pReceiveFnc)
	{
		pMediaSample->SetMediaType(m_pRenderedType);
		return m_pReceiveFnc(pMediaSample,  m_nPackageNo++, m_lRFParam);
	}
	return S_FALSE;
}
//////////////////////////////////////////////////////////////////////////
void CSample2MemRenderer::OnRenderStart(IMediaSample *pMediaSample)
{
	SIMPLE_TRACE("OnRenderStart\n");
}
//////////////////////////////////////////////////////////////////////////
void CSample2MemRenderer::OnRenderEnd(IMediaSample *pMediaSample)
{
	SIMPLE_TRACE("OnRenderEnd\n");
}
//////////////////////////////////////////////////////////////////////////
HRESULT CSample2MemRenderer::OnStartStreaming() 
{
	SIMPLE_TRACE("OnStartStreaming\n");
//	m_RenderEvent.Reset();
	return NOERROR; 
}
//////////////////////////////////////////////////////////////////////////
HRESULT CSample2MemRenderer::OnStopStreaming()
{
	SIMPLE_TRACE("OnStopStreaming\n");
	return NOERROR; 
}
//////////////////////////////////////////////////////////////////////////
void CSample2MemRenderer::OnWaitStart() 
{
	SIMPLE_TRACE("OnWaitStart\n");
}
//////////////////////////////////////////////////////////////////////////
void CSample2MemRenderer::OnWaitEnd()
{
	SIMPLE_TRACE("OnWaitEnd\n");
}
//////////////////////////////////////////////////////////////////////////
void CSample2MemRenderer::PrepareRender()
{
	SIMPLE_TRACE("PrepareRender\n");
}
//////////////////////////////////////////////////////////////////////////
void CSample2MemRenderer::ThrottleWait()
{
    if (m_trThrottle>0) 
	 {
        int iThrottle = m_trThrottle/10000;    // convert to mSec
        MSR_INTEGER( m_idThrottle, iThrottle);
        DbgLog((LOG_TRACE, 0, TEXT("Throttle %d ms"), iThrottle));
        Sleep(iThrottle);
    } else {
        Sleep(0);
    }
} // ThrottleWait
//////////////////////////////////////////////////////////////////////////
void CSample2MemRenderer::OnReceiveFirstSample(IMediaSample *pMediaSample)
{
	SIMPLE_TRACE("OnReceiveFirstSample\n");
	return;
}
//////////////////////////////////////////////////////////////////////////
HRESULT CSample2MemRenderer::CheckMediaType(const CMediaType *pMT)
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
			if (hr == S_OK)
			{
				break;
			}
		}
		return hr;
	}
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
HRESULT CSample2MemRenderer::SetMediaType(const CMediaType *pmt)
{
	if (m_pRenderedType)
	{
		DeleteMediaType(m_pRenderedType);
		m_pRenderedType = NULL;
	}
	if (pmt)
	{
		m_pRenderedType = CreateMediaType(pmt);
	}
   return NOERROR;
}
///
STDMETHODIMP CSample2MemRenderer::SetSink( IQualityControl * piqc)
{
	SIMPLE_TRACE(_T("CSample2MemRenderer::SetSink\n"));
   m_pQSink = piqc;
   return NOERROR;
} // SetSink


STDMETHODIMP CSample2MemRenderer::Notify( IBaseFilter * pSelf, Quality q)
{
	SIMPLE_TRACE(_T("CSample2MemRenderer::Notify\n"));
    if (q.Proportion>=1000)
	 { 
		 m_trThrottle = 0; 
	 }
    else 
	 {
        m_trThrottle = -330000 + (388880000/(q.Proportion+167));
    }
    return NOERROR;
} // Notify
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSample2MemRenderer::SetReceiveFunction(RECEIVE_FUNCTION pReceiveFnc, long lParam)
{
	m_pReceiveFnc = pReceiveFnc;
	m_lRFParam    = lParam;
   return NOERROR;
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSample2MemRenderer::SetAcceptedMediaTypes(AM_MEDIA_TYPE*pMT, int nCount, BOOL bEnumerate)
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
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSample2MemRenderer::GetAllocatorProperties(ALLOCATOR_PROPERTIES *pAP)
{
	if (pAP == NULL) return E_POINTER;
	if (m_AllocProps.cbBuffer != 0)
	{
		*pAP = m_AllocProps;
		return S_OK;
	}
	return E_NOTIMPL;
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(ULONG) CSample2MemRenderer::GetMiscFlags(void)
{
	return m_Sample2MemRendererParams.m_lMiscFlags;
}
//////////////////////////////////////////////////////////////////////////
BOOL CSample2MemRenderer::ScheduleSample(IMediaSample *pMediaSample)
{
	if (m_Sample2MemRendererParams.m_bScheduleSamples)
	{
		return CBaseRenderer::ScheduleSample(pMediaSample);
	}
	else
	{
		SetEvent((HANDLE) m_RenderEvent);
		return TRUE;
	}
}

///////////////////////////////////////////////////////////////////////
// DllRegisterServer
///////////////////////////////////////////////////////////////////////
STDAPI DllRegisterServer()
{
    return AMovieDllRegisterServer2( TRUE );
}

///////////////////////////////////////////////////////////////////////
// DllUnregisterServer
///////////////////////////////////////////////////////////////////////
STDAPI DllUnregisterServer()
{
    return AMovieDllRegisterServer2( FALSE );
}
