// PushSource.cpp: implementation of CPushSource class.
// 
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "resource.h"

#include <streams.h>     // DirectShow
#include <stdio.h>

#pragma warning( push )
#pragma warning( disable : 4201 )
#include <ks.h>
#include <ksmedia.h>
#pragma warning( pop )

#include "PushSource.h"
#include "PushSourceProp.h"
#include "PushSourceStream.h"
#include "DirectBufferAllocator.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define FILTER_NAME		L"PushSource Filter"
#define STREAM_NAME		_T("PushSourceStream")

#define CAPTURE_PIN					L"Capture"
#define PREVIEW_PIN					L"Preview"
#define STILL_PIN					L"Still"
#define OUTPUT_PIN					L"Out"
#define CAPTURE_PIN_UNPRIVILEGED	L"~Capture"
#define PREVIEW_PIN_UNPRIVILEGED	L"~Preview"
#define STILL_PIN_UNPRIVILEGED		L"~Still"

// Profile
#define PUSHSOURCE_PARAM	_T("PushSource")
#define PARAM_CAPTURE		_T("Capture")
#define PARAM_PREVIEW		_T("Preview")
#define PARAM_STILL			_T("Still")
#define PARAM_LIVE_SOURCE	_T("LiveSource")
#define PARAM_INTERNAL_RM	_T("InternalRateMatching")
#define PARAM_MISC_FLAGS	_T("MiscFlags")
#define PARAM_TIME_STAMPS	_T("GenerateTimeStamps")
#define PARAM_OFFSET		_T("Offset")
#define PARAM_PREF_ALLOC	_T("PreferOwnAllocator")
#define PARAM_SORT_SAMPLES	_T("SortSamples")

// setup data - allows the self-registration to work.
const AMOVIESETUP_MEDIATYPE sudPinTypes =
{ &MEDIATYPE_NULL        // clsMajorType
, &MEDIASUBTYPE_NULL };  // clsMinorType

const AMOVIESETUP_PIN psudPins[] =
{
 { L"Output"           // strName
  , FALSE               // bRendered
  , TRUE                // bOutput
  , FALSE               // bZero
  , FALSE               // bMany
  , &CLSID_NULL         // clsConnectsToFilter
  , L""                 // strConnectsToPin
  , 1                   // nTypes
  , &sudPinTypes        // lpTypes
  }
};

const AMOVIESETUP_FILTER sudPushSource =
{ &CLSID_PushSource              // clsID
, FILTER_NAME                    // strName
, MERIT_DO_NOT_USE                  // dwMerit
, 1                                 // nPins
, psudPins };                       // lpPin

// Needed for the CreateInstance mechanism
CFactoryTemplate g_Templates[]=
    {   { FILTER_NAME
        , &CLSID_PushSource
        , CPushSource::CreateInstance
        , NULL
        , &sudPushSource },
        { FILTER_NAME L" Properties"
        , &CLSID_PushSourceProp
        , CPushSourceProp::CreateInstance }
    };
int g_cTemplates = sizeof(g_Templates)/sizeof(g_Templates[0]);

///////////////////////////////////////////////////////////////////////
// CreateInstance: for COM to create a CPushSource object
///////////////////////////////////////////////////////////////////////
CUnknown * WINAPI CPushSource::CreateInstance(LPUNKNOWN punk, HRESULT *phr) {

	CPushSource *pNewObject = new CPushSource(NAME("PushSource"), punk, phr );
	if (pNewObject == NULL) 
	{
		*phr = E_OUTOFMEMORY;
	}

	return pNewObject;
}

HWND CPushSource::gm_hWndThread = NULL;
_TCHAR CPushSource::gm_sWndName[_MAX_PATH] = PUSHSOURCE_PARAM;
const _TCHAR CPushSource::gm_sModuleName[] = _T("PushSource");

///////////////////////////////////////////////////////////////////////
// CPushSource: Constructor
///////////////////////////////////////////////////////////////////////
BOOL CALLBACK CPushSource::EnumThreadWndProc(HWND hwnd, LPARAM lParam)
{
	if (GetParent(hwnd) == NULL)
	{
		*((HWND*)lParam) = hwnd;
		return FALSE;
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
CPushSource::CPushSource(TCHAR *tszName, LPUNKNOWN punk, HRESULT *phr)
	: CSource(tszName, punk, CLSID_PushSource)
	, CPersistStream(punk, phr)
	, m_pDirectAllocator(NULL)
	, m_pSampleProperties(NULL)
	, m_nBufferType(0)
	, m_pReleaseBufferFunction(NULL)
	, m_BufferList(_T("Buffers"))
	, m_SentBufferList(_T("Sent Buffers"))
	, m_MediaTypes(_T("Media Types"))
	, m_bReCommit(FALSE)
	, m_hWndPropertyPage(NULL)
	, m_pRefClock(NULL)
	, m_bIsStopping(FALSE)
{
	BOOL  bReadModuleParams = TRUE;
	HKEY  hSecKey = NULL;
	LONG  lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, REGKEY_PS_PUSHSOURCE, 0, KEY_READ, &hSecKey);
	DWORD dwValue;
	DWORD dwType;
	DWORD dwCount = sizeof(DWORD);
	if (lResult == ERROR_SUCCESS)
	{
		lResult = RegQueryValueEx(hSecKey, REGKEY_PS_SAMPLE_TOO_LATE, NULL, &dwType, (LPBYTE)&dwValue, &dwCount);
		if (lResult == ERROR_SUCCESS && dwType == REG_DWORD)
		{
			m_nSampleToLate = dwValue;
		}
		lResult = RegQueryValueEx(hSecKey, REGKEY_PS_SAMPLE_TOO_EARLY, NULL, &dwType, (LPBYTE)&dwValue, &dwCount);
		if (lResult == ERROR_SUCCESS && dwType == REG_DWORD)
		{
			m_nSampleToEarly = dwValue;
		}
		lResult = RegQueryValueEx(hSecKey, REGKEY_PS_READ_MODULE_PARAM, NULL, &dwType, (LPBYTE)&dwValue, &dwCount);
		if (lResult == ERROR_SUCCESS && dwType == REG_DWORD)
		{
			bReadModuleParams = dwValue;
		}
		RegCloseKey(hSecKey);
	}

	if (gm_hWndThread == NULL && _tcscmp(gm_sWndName, PUSHSOURCE_PARAM) == 0)
	{
		EnumThreadWindows(GetCurrentThreadId(), EnumThreadWndProc, (LPARAM)&gm_hWndThread);
		if (gm_hWndThread)
		{
			_TCHAR sWndName[_MAX_PATH] = _T("");
			::GetWindowText(gm_hWndThread, sWndName, _MAX_PATH);
			if (_tcslen(sWndName))
			{
				_tcscat(gm_sWndName, _T(":"));
				_tcscat(gm_sWndName, sWndName);
			}
			LRESULT lResult = 0;
			SendMessage(gm_hWndThread, WM_TRACE_TEXT, (WPARAM)&lResult, NULL);	// Test whether the message is handled
			if (lResult == 0)
			{
				gm_hWndThread = NULL;
			}
			else
			{
				if (bReadModuleParams && _tcslen(sWndName))
				{
					Trace(_T("Reading Parameter of:"));
					Trace(sWndName);
				}
			}
		}
	}

	readConfig();                  // read default value from registry
	ZERO_INIT(m_rtStartTime);
	m_pQSink = NULL;
	HRESULT hr = InitPins();
	if (FAILED(hr))
	{
		*phr = hr;
	}
	m_nNextSampleNumber = 0xffffffff;
	
//	m_pRefClock = new CPushSourceClock(static_cast<IBaseFilter*>(this), phr, NULL);

	m_pCurrentMediaType = NULL;
	m_nSampleToEarly = 2000;
	m_nSampleToLate  = 0;
}

HRESULT CPushSource::InitPins()
{
	QueryInterface(IID_IQualityControl, (void**)&m_pQSink);
	// Achtung !!
	// Der Destructor wird nicht aufgerufen, wenn noch eine Referenz auf ein
	// Interface der Klasse besteht. Deshalb muss in JoinFiltergraph(..) beim
	// entfernen des Graphen m_pQSink->Release() aufgerufen werden und die 
	// Referenzen aus den Pins entfernt werden, damit sie nicht in der Pins
	// auch noch m_pQSink->Release() aufgerufen wird.
	// Dies wird in ReleasePins() durchgeführt.
	
	int iOutPins = 0;
	if (m_PushSourceParams.m_bCapture) iOutPins++;
	if (m_PushSourceParams.m_bPreview) iOutPins++;
	if (m_PushSourceParams.m_bStill  ) iOutPins++;

	LONG lFlags = 0;
	if (!m_PushSourceParams.m_bLiveSource) lFlags |= AM_PUSHSOURCECAPS_NOT_LIVE;
	if ( m_PushSourceParams.m_bInternalRM) lFlags |= AM_PUSHSOURCECAPS_INTERNAL_RM;
	
//	if (iOutPins == 0)				// einfachen Pin erzeugen
	{
		iOutPins = 1;					// erst mal nur einen
	}

	m_paStreams = (CSourceStream**) new CSourceStream*[iOutPins];
	if (m_paStreams == NULL)
	{
		return E_OUTOFMEMORY;
	}

	HRESULT hr;
	iOutPins = 0;
	LPCWSTR sPinName = NULL;
	if (m_PushSourceParams.m_bCapture)
	{
		sPinName = CAPTURE_PIN;
		if (m_PushSourceParams.m_bCapture != PRIVILEGED) sPinName = CAPTURE_PIN_UNPRIVILEGED;
		m_paStreams[iOutPins] = new CPushSourceStream(STREAM_NAME, &hr, this, sPinName, &PIN_CATEGORY_CAPTURE, lFlags);
		if (FAILED(hr)) return hr;
		if (m_paStreams[iOutPins] == NULL)
		{
			return E_OUTOFMEMORY;
		}

		m_paStreams[iOutPins++]->SetSink(m_pQSink);
	}

	if (m_PushSourceParams.m_bPreview)
	{
		sPinName = PREVIEW_PIN;
		if (m_PushSourceParams.m_bPreview!= PRIVILEGED) sPinName = PREVIEW_PIN_UNPRIVILEGED;
		m_paStreams[iOutPins] = new CPushSourceStream(STREAM_NAME, &hr, this, sPinName, &PIN_CATEGORY_PREVIEW, lFlags);
		if (FAILED(hr)) return hr;
		if (m_paStreams[iOutPins] == NULL)
		{
			return E_OUTOFMEMORY;
		}
		
		m_paStreams[iOutPins++]->SetSink(m_pQSink);
	}

	if (m_PushSourceParams.m_bStill)
	{
		sPinName = STILL_PIN;
		if (m_PushSourceParams.m_bStill!= PRIVILEGED) sPinName = STILL_PIN_UNPRIVILEGED;
		m_paStreams[iOutPins] = new CPushSourceStream(STREAM_NAME, &hr, this, sPinName, &PIN_CATEGORY_STILL, lFlags);
		if (FAILED(hr)) return hr;
		if (m_paStreams[iOutPins] == NULL)
		{
			return E_OUTOFMEMORY;
		}
		
		m_paStreams[iOutPins++]->SetSink(m_pQSink);
	}

	if (iOutPins == 0)				// einfachen Pin erzeugen
	{
		m_paStreams[iOutPins] = new CPushSourceStream(STREAM_NAME, &hr, this, OUTPUT_PIN, NULL, lFlags);
		if (FAILED(hr)) return hr;
		if (m_paStreams[iOutPins] == NULL)
		{
			return E_OUTOFMEMORY;
		}
		m_paStreams[iOutPins++]->SetSink(m_pQSink);
	}

	return S_OK;
}

void CPushSource::ReleasePins()
{
	if (m_pQSink)
	{
		HRESULT hr;
		for (int i=0; i<m_iPins; i++)
		{
			hr = m_paStreams[i]->SetSink(NULL);
		}
		
		m_pQSink->Release();
		m_pQSink = NULL;
	}
}

///////////////////////////////////////////////////////////////////////
// ~CPushSource: Destructor
///////////////////////////////////////////////////////////////////////
CPushSource::~CPushSource()
{
	ReleaseAllBuffers();
	ReleaseDirectAllocator();
	if (m_pSampleProperties)
	{
		delete m_pSampleProperties;
	}
	if (m_pQSink)
	{
		m_pQSink->Release();
	}
	if (m_pRefClock)
	{
		delete m_pRefClock;
	}
	SetMediaType(NULL);
	if (m_pCurrentMediaType)
	{
		DeleteMediaType(m_pCurrentMediaType);
	}
}

void CPushSource::ReleaseAllBuffers()
{
	PushSourceProperties *pMSP;
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
///////////////////////////////////////////////////////////////////////
// saveConfig: write the parameters into the registry
///////////////////////////////////////////////////////////////////////

BOOL CPushSource::saveConfig()
{
	_TCHAR sz[STR_MAX_LENGTH];
	// data, that is set only from calling applications
	WriteProfileString(gm_sWndName, PARAM_TIME_STAMPS, sz);
	_stprintf(sz, _T("%d"), m_PushSourceParams.m_nOffsetInMS);

	if (m_PushSourceParams.m_dwFlags & PS_FLAG_SAVE_ONLY_FIRST_TIME)
	{	// save only, if it has never been set before
		if (GetProfileInt(gm_sWndName, PARAM_CAPTURE, -1) != -1)
		{
			return FALSE;
		}
	}

	// data, that may be modified in own property page
	_stprintf(sz, _T("%d"), m_PushSourceParams.m_bCapture);
	WriteProfileString(gm_sWndName, PARAM_CAPTURE, sz);
	_stprintf(sz, _T("%d"), m_PushSourceParams.m_bPreview);
	WriteProfileString(gm_sWndName, PARAM_PREVIEW, sz);
	_stprintf(sz, _T("%d"), m_PushSourceParams.m_bStill);
	WriteProfileString(gm_sWndName, PARAM_STILL, sz);
	_stprintf(sz, _T("%d"), m_PushSourceParams.m_bLiveSource);
	WriteProfileString(gm_sWndName, PARAM_LIVE_SOURCE, sz);
	_stprintf(sz, _T("%d"), m_PushSourceParams.m_bInternalRM);
	WriteProfileString(gm_sWndName, PARAM_INTERNAL_RM, sz);
	_stprintf(sz, _T("%d"), m_PushSourceParams.m_lMiscFlags);
	WriteProfileString(gm_sWndName, PARAM_MISC_FLAGS, sz);
	_stprintf(sz, _T("%d"), m_PushSourceParams.m_bGenerateTimeStamps);
	WriteProfileString(gm_sWndName, PARAM_OFFSET, sz);
	_stprintf(sz, _T("%d"), m_PushSourceParams.m_bPreferOwnAllocator);
	WriteProfileString(gm_sWndName, PARAM_PREF_ALLOC, sz);

	SetDirty(FALSE);
	return TRUE;
}

///////////////////////////////////////////////////////////////////////
// readConfig: read the parameters from the registry
///////////////////////////////////////////////////////////////////////
void CPushSource::readConfig()
{
	m_PushSourceParams.m_bCapture				= GetProfileInt(gm_sWndName, PARAM_CAPTURE, PRIVILEGED);
	m_PushSourceParams.m_bPreview				= GetProfileInt(gm_sWndName, PARAM_PREVIEW, FALSE);
	m_PushSourceParams.m_bStill					= GetProfileInt(gm_sWndName, PARAM_STILL, FALSE);
	m_PushSourceParams.m_bLiveSource			= GetProfileInt(gm_sWndName, PARAM_LIVE_SOURCE, FALSE);
	m_PushSourceParams.m_bInternalRM			= GetProfileInt(gm_sWndName, PARAM_INTERNAL_RM, FALSE);
	m_PushSourceParams.m_lMiscFlags				= GetProfileInt(gm_sWndName, PARAM_MISC_FLAGS, AM_FILTER_MISC_FLAGS_IS_SOURCE);
	m_PushSourceParams.m_nOffsetInMS			= GetProfileInt(gm_sWndName, PARAM_OFFSET, 0);
	m_PushSourceParams.m_bGenerateTimeStamps	= GetProfileInt(gm_sWndName, PARAM_TIME_STAMPS, TRUE);
	m_PushSourceParams.m_bPreferOwnAllocator	= GetProfileInt(gm_sWndName, PARAM_PREF_ALLOC, FALSE);
	m_PushSourceParams.m_bSortSamples			= GetProfileInt(gm_sWndName, PARAM_SORT_SAMPLES, FALSE);
}

///////////////////////////////////////////////////////////////////////
// DecideBufferSize: Tell the output pin's allocator what size buffers 
// we require. Can only do this when the input is connected
///////////////////////////////////////////////////////////////////////
HRESULT CPushSource::DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *pProperties)
{
	HRESULT hr = NOERROR;
	if (pAlloc == NULL) return E_POINTER;
	if (pProperties == NULL) return E_POINTER;

	if (m_nBufferType > 0)						// external buffer
	{
		*pProperties = m_allocprops;			// with internal allocator
		ALLOCATOR_PROPERTIES Actual;
//		if (pProperties->cBuffers == 0) pProperties->cBuffers = 1;
		if (pProperties->cBuffers < 8)
		{
			pProperties->cBuffers = 8;
		}
		
		hr = pAlloc->SetProperties(pProperties, &Actual);
		if (FAILED(hr)) return hr;
		if (Actual.cbBuffer < pProperties->cbBuffer)// on this error:
		{											// try to divide into smaller 
			return E_FAIL;							// buffer pieces ?
		}
		return hr;
	}

   return NOERROR;
}

///////////////////////////////////////////////////////////////////////
// GetMediaType: I support one type, namely the type of the input pin
// This type is only available if my input is connected
///////////////////////////////////////////////////////////////////////
HRESULT CPushSource::GetMediaType(int iPosition, CMediaType *pMediaType)
{
	if (iPosition < 0) 
	{
		return E_INVALIDARG;
	}

	if (iPosition >= m_MediaTypes.GetCount())
	{
		return VFW_S_NO_MORE_ITEMS;
	}
	 
	if (m_nBufferType > 0)
	{
		int nPos, nCount = m_MediaTypes.GetCount();
		POSITION pos = m_MediaTypes.GetHeadPosition();
		for (nPos=0; nPos<nCount && pos != NULL; nPos++)
		{
			AM_MEDIA_TYPE *pM = m_MediaTypes.GetNext(pos);
			if (iPosition == nPos)
			{
				CopyMediaType(pMediaType, pM);
				break;
			}
		}
	}
	else 
	{
		return E_UNEXPECTED;
	}
	return NOERROR;
}
///////////////////////////////////////////////////////////////////////
HRESULT CPushSource::CheckMediaType(const CMediaType *pMediaType)
{
	CAutoLock lock(pStateLock ());
	int nPos, nCount = m_MediaTypes.GetCount();
	POSITION pos = m_MediaTypes.GetHeadPosition();
	for (nPos=0; nPos<nCount && pos != NULL; nPos++)
	{
		AM_MEDIA_TYPE *pM = m_MediaTypes.GetNext(pos);
		if (*pMediaType == *pM)
		{
	        return S_OK;
		}
	}

    return E_FAIL;
}
///////////////////////////////////////////////////////////////////////
HRESULT	CPushSource::SetCurrentMediaType(const AM_MEDIA_TYPE*pMT)
{
	HRESULT hr = CheckMediaType((const CMediaType*)pMT);
	if (SUCCEEDED(hr))
	{
		if (m_pCurrentMediaType)
		{
			DeleteMediaType(m_pCurrentMediaType);
		}
		m_pCurrentMediaType = CreateMediaType(pMT);
	}
	return hr;
}
///////////////////////////////////////////////////////////////////////
// NonDelegatingQueryInterface:
///////////////////////////////////////////////////////////////////////
STDMETHODIMP CPushSource::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	CheckPointer(ppv,E_POINTER);

	if (riid == IID_IPushSource) 
	{
		return GetInterface((IPushSource *) this, ppv);
	}
	else if (riid == IID_IPersistStream) 
	{
		return GetInterface((IPersistStream *) this, ppv);
	}
	else if (riid == IID_ISpecifyPropertyPages) 
	{
		return GetInterface((ISpecifyPropertyPages *) this, ppv);
	}
	else if (riid == IID_IQualityControl) 
	{
		return GetInterface((IQualityControl*) this, ppv);
	}
	else if (riid == IID_IAMFilterMiscFlags) 
	{
	return GetInterface((IAMFilterMiscFlags*) this, ppv);
	}
	else if (riid == IID_IKsPropertySet) 
	{
		return GetInterface((IKsPropertySet*) this, ppv);
	}
    else if(riid == IID_IReferenceClock && m_pRefClock)
    {
        return GetInterface(static_cast<IReferenceClock*>(m_pRefClock), ppv);
	}
	else
	{
		return CSource::NonDelegatingQueryInterface(riid, ppv);
	}
}

#define WRITEOUT(var)  hr = pStream->Write(&var, sizeof(var), NULL); \
               if (FAILED(hr)) return hr;

#define READIN(var)    hr = pStream->Read(&var, sizeof(var), NULL); \
               if (FAILED(hr)) return hr;

///////////////////////////////////////////////////////////////////////
// GetClassID: This is the only method of IPersist
///////////////////////////////////////////////////////////////////////
STDMETHODIMP CPushSource::GetClassID(CLSID *pClsid)
{
    return CSource::GetClassID(pClsid);
}

///////////////////////////////////////////////////////////////////////
// WriteToStream: called when save the filter to stream
///////////////////////////////////////////////////////////////////////
HRESULT CPushSource::WriteToStream(IStream *pStream)
{
    HRESULT hr = NOERROR;
    WRITEOUT(m_PushSourceParams);
    return hr;
}

///////////////////////////////////////////////////////////////////////
// ReadFromStream: called when read from stream
///////////////////////////////////////////////////////////////////////
HRESULT CPushSource::ReadFromStream(IStream *pStream)
{
    HRESULT hr = NOERROR;
    READIN(m_PushSourceParams);
    return hr;
}

///////////////////////////////////////////////////////////////////////
// GetPages: Returns the clsid's of the property pages we support
///////////////////////////////////////////////////////////////////////
STDMETHODIMP CPushSource::GetPages(CAUUID *pPages)
{
	pPages->cElems = 1;
	pPages->pElems = (GUID *) CoTaskMemAlloc(sizeof(GUID));
	if (pPages->pElems == NULL) 
	{
		return E_OUTOFMEMORY;
	}
	*(pPages->pElems) = CLSID_PushSourceProp;
	return NOERROR;
}
//////////////////////////////////////////////////////////////////////////
HRESULT CPushSource::ReleaseMediaBuffer(BYTE *pBuffer)
{
	HRESULT hr = NOERROR;
	switch (m_nBufferType)
	{
		case BUFFER_FIXED:
		if (m_pReleaseBufferFunction)						// external function to release the buffer
		{
			hr = m_pReleaseBufferFunction(pBuffer, m_lParamRBF);
		}break;
		case BUFFER_COPY:
		case BUFFER_ATTACH:
		{													// Find and remove the properties buffer from the List
			PushSourceProperties*pMSP = FindMSP(pBuffer, true);
			if (pMSP)
			{
				if (m_pReleaseBufferFunction)				// external function to release the buffer
				{
					if (pMSP->pReference)
					{
						hr = m_pReleaseBufferFunction(pMSP->pReference, m_lParamRBF);
					}
					else
					{
						hr = m_pReleaseBufferFunction(pBuffer, m_lParamRBF);
					}
				}
				if (m_nBufferType == BUFFER_COPY)			// if the buffer is copied
				{
					CoTaskMemFree(pBuffer);					// it was allocated in SetBuffer(..)
				}
				delete pMSP;								// delete the properties buffer
			}
		}break;
	}
	return hr;
}
//////////////////////////////////////////////////////////////////////////
PushSourceProperties* CPushSource::FindMSPbyNumber(DWORD nSampleNumber, bool bRemove/*=false*/)
{
	PushSourceProperties *pMSP;
	POSITION posOld, pos;
	pos = m_BufferList.GetHeadPosition();
	while (pos)
	{
		posOld = pos;
		pMSP = m_BufferList.GetNext(pos);
		if (pMSP->nSampleNumber == nSampleNumber)
		{
			if (bRemove) m_BufferList.Remove(posOld);
			return pMSP;
		}
		RELEASE_TRACE(_T("Unsorted"));
	}
	return NULL;
}
//////////////////////////////////////////////////////////////////////////
PushSourceProperties* CPushSource::FindMSP(void *pBuffer, bool bRemove/*=false*/)
{
	PushSourceProperties *pMSP;
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
//////////////////////////////////////////////////////////////////////////
HRESULT CPushSource::CreateDirectAllocator()
{
	HRESULT hr = NOERROR;
	if (m_pDirectAllocator == NULL)
	{
		m_pDirectAllocator = new CDirectBufferAllocater(this, &hr);
		if (!m_pDirectAllocator) return E_OUTOFMEMORY;
		m_pDirectAllocator->AddRef();
		if (FAILED(hr)) return hr;
		m_bReCommit = TRUE;
	}
	
	ALLOCATOR_PROPERTIES Actual;
	m_pDirectAllocator->SetProperties(&m_allocprops, &Actual);
	return hr;
}
//////////////////////////////////////////////////////////////////////////
void CPushSource::ReleaseDirectAllocator()
{
	if (m_pDirectAllocator)
	{
		m_pDirectAllocator->Release();
		m_pDirectAllocator = NULL;
	}
}
//////////////////////////////////////////////////////////////////////////
HRESULT CPushSource::RecommitDirectAllocator()
{
	CAutoLock lock(pStateLock ());
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
	
	m_bReCommit = FALSE;
	return hr;
}
//////////////////////////////////////////////////////////////////////////
HRESULT CPushSource::GetDeliveryBuffer(IMediaSample ** ppSample, REFERENCE_TIME * pStartTime, REFERENCE_TIME * pEndTime, DWORD dwFlags)
{
	if (m_bIsStopping) return E_FAIL;

	CAutoLock lock(pStateLock ());
	HRESULT                hr   = NOERROR;
	PushSourceProperties *pMSP = NULL;
	if (!m_pDirectAllocator)								// no direct buffer allocator
	{
		if (!m_paStreams   ) return E_POINTER;
		if (!m_paStreams[0]) return E_POINTER;
		BYTE *pData = NULL;

		switch (m_nBufferType)
		{
			case BUFFER_FIXED:								// fixed buffer
			if (m_allocprops.cBuffers > 0)					// if there is a buffer
			{												// get the buffer from the allocator
				hr = m_paStreams[0]->CSourceStream::GetDeliveryBuffer(ppSample, pStartTime, pEndTime, dwFlags);
				if (SUCCEEDED(hr))
				{
					hr = (*ppSample)->GetPointer(&pData);	// get the data pointer
				}
				if (SUCCEEDED(hr))
				{
					pMSP = m_pSampleProperties;
					m_allocprops.cBuffers--;				// decrement No. of buffers
					if (pData && pMSP->pBuffer)
					{
						if (m_allocprops.cbPrefix)			// if there is a header prefix
						{
							pData -= m_allocprops.cbPrefix;	// correct the offset
						}
						CopyMemory(pData, pMSP->pBuffer, (*ppSample)->GetActualDataLength());// copy the data
					}
				}break;
			}
			else
			{
				hr = VFW_E_SAMPLE_REJECTED_EOS;
			}break;
			case BUFFER_ATTACH:								// attached buffer or
			case BUFFER_COPY:								// copied buffer
			if (m_BufferList.GetCount())					// if there is a buffer
			{												// get the buffer from the allocator
				hr = m_paStreams[0]->CSourceStream::GetDeliveryBuffer(ppSample, pStartTime, pEndTime, dwFlags);
				if (SUCCEEDED(hr))
				{
					hr = (*ppSample)->GetPointer(&pData);	// get the data pointer
				}
				
				pMSP = m_BufferList.RemoveHead();			// remove the buffer from the list
				if (!pMSP) return E_POINTER;
				m_SentBufferList.AddHead(pMSP);				// add to the sent buffers list
				if (pData && pMSP->pBuffer)
				{
					if (m_allocprops.cbPrefix)				// if there is a header prefix
					{
						pData -= m_allocprops.cbPrefix;		// correct the offset
					}
					(*ppSample)->SetActualDataLength(pMSP->nLength);
					CopyMemory(pData, pMSP->pBuffer, (*ppSample)->GetActualDataLength());// copy
				}
			}
			else
			{
				hr = VFW_E_SAMPLE_REJECTED_EOS;
			}break;
		}
	}
	else													// direct buffer allocator
	{
		if (m_bReCommit)
		{
			hr = RecommitDirectAllocator();					// recommit if neccessary
		}
		if (FAILED(hr))
		{
			return hr;
		}
		switch (m_nBufferType)
		{
			case BUFFER_FIXED:								// fixed buffer
			if (m_allocprops.cBuffers > 0)
			{												// get the buffer from the direct allocator
				hr = m_pDirectAllocator->GetBuffer(ppSample, pStartTime, pEndTime, dwFlags);
				if (FAILED(hr))
				{
					return hr;
				}
				pMSP = m_pSampleProperties;
				if (m_bBufferChanged)						// buffer changed
				{											// set new buffer
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
			{												// get the buffer from the direct allocator
				hr = m_pDirectAllocator->GetBuffer(ppSample, pStartTime, pEndTime, dwFlags);
				if (FAILED(hr))
				{
					return hr;
				}

				if (m_nNextSampleNumber != 0xffffffff)
				{
					pMSP = FindMSPbyNumber(m_nNextSampleNumber, true);
				}
				else
				{
					pMSP = m_BufferList.RemoveHead();		// remove the buffer from the list
				}
				
				if (!pMSP) return E_POINTER;
				m_SentBufferList.AddHead(pMSP);				// add to the sent buffers list
															// set sample buffer
				hr = ((CMediaSample*)*ppSample)->SetPointer(pMSP->pBuffer+m_allocprops.cbPrefix, pMSP->nLength);
				if (m_PushSourceParams.m_bSortSamples && pMSP->nSampleNumber != 0xffffffff)
				{
					m_nNextSampleNumber = pMSP->nSampleNumber + 1;
				}
			}
			else
			{
				hr = VFW_E_SAMPLE_REJECTED_EOS;
			}break;
		}
	}
	if ((m_nBufferType != 0) && (pMSP != NULL))				// update the media sample properties
	{
		if (SUCCEEDED(hr))
		{
			hr = ((CMediaSample*)*ppSample)->SetPreroll(pMSP->bIsPreroll);
		}
		if (SUCCEEDED(hr))
		{
			hr = ((CMediaSample*)*ppSample)->SetMediaType(m_pCurrentMediaType);
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
			if (pMSP->llTimeEnd < pMSP->llTimeStart)
			{
				pMSP->llTimeStart = pMSP->llTimeEnd;
			}
			hr = ((CMediaSample*)*ppSample)->SetMediaTime(&pMSP->llTimeStart, &pMSP->llTimeEnd);
		}
		if (SUCCEEDED(hr))
		{
			hr = ((CMediaSample*)*ppSample)->SetTime(&pMSP->rtTimeStart, &pMSP->rtTimeEnd);
		}

//		char szText[16];
//		wsprintf(szText, "sNo%d", pMSP->nSampleNumber);
//		DEBUG_TRACE(szText);
	}
	if (!m_pDirectAllocator && pMSP)							// no direct buffer allocator
	{
		ReleaseMediaBuffer(pMSP->pBuffer);						// release here
	}
	return hr;
}

// --- IMediaFilter Interface
STDMETHODIMP CPushSource::SetSyncSource(IReferenceClock *pClock)
{
	DEBUG_TRACE(_T("CPushSource::SetSyncSource"));
	for (int i=0; i<m_iPins; i++)
	{
		((CPushSourceStream*)m_paStreams[i])->SetSyncSource(pClock);
	}
	IReferenceClock *pOwnClock = NULL;
	if (SUCCEEDED(QueryInterface(IID_IReferenceClock, (void**)&pOwnClock)))
	{
		if (pClock == pOwnClock)
		{
			RELEASE_TRACE(_T("own clock selected"));
		}
		pOwnClock->Release();
	}
	return CSource::SetSyncSource(pClock);
}

// --- IBaseFilter Interface
STDMETHODIMP CPushSource::JoinFilterGraph(IFilterGraph * pGraph, LPCWSTR pName)
{
	DEBUG_TRACE(_T("CPushSource::JoinFilterGraph"));
	HRESULT hr = CSource::JoinFilterGraph(pGraph, pName);
	if (SUCCEEDED(hr))
	{
		for (int i=0; i<m_iPins; i++)
		{
			((CPushSourceStream*)m_paStreams[i])->SetFilterGraph(m_pSink);
		}
		
		if (pGraph == NULL)
		{
			ReleasePins();
		}
		else if (m_pRefClock)
		{
			IReferenceClock *pOwnClock = NULL;
			if (SUCCEEDED(QueryInterface(IID_IReferenceClock, (void**)&pOwnClock)))
			{
				IMediaFilter *pMediaFilter = NULL;
				if (SUCCEEDED(pGraph->QueryInterface(IID_IMediaFilter, (void**)&pMediaFilter)))
				{
					pMediaFilter->SetSyncSource(pOwnClock);
					pMediaFilter->Release();
				}
				pOwnClock->Release();
			}
		}
	}
	return hr;
}

STDMETHODIMP CPushSource::Run(REFERENCE_TIME tStart)
{
	DEBUG_TRACE(_T("CPushSource::Run"));

//	ZERO_INIT(m_rtStartTime);
//	ZERO_INIT(tStart);
//	m_nAdvices = 0;

	m_nNextSampleNumber = 0xffffffff;
	for (int i=0; i<m_iPins; i++)
	{
		((CPushSourceStream*)m_paStreams[i])->NotifyFilterState(State_Running, tStart);
	}
	
	return CSource::Run(tStart); // Call the filter base class.
}

STDMETHODIMP CPushSource::Pause()
{
	DEBUG_TRACE(_T("CPushSource::Pause()"));
	for (int i=0; i<m_iPins; i++)
	{
		((CPushSourceStream*)m_paStreams[i])->NotifyFilterState(State_Paused);
	}
	return CSource::Pause();
}

STDMETHODIMP CPushSource::Stop()
{
	m_bIsStopping = TRUE;
	DEBUG_TRACE(_T("CPushSource::Stop"));
	ZERO_INIT(m_rtStartTime);
	m_nNextSampleNumber = 0xffffffff;
	
	for (int i=0; i<m_iPins; i++)
	{
		((CPushSourceStream*)m_paStreams[i])->NotifyFilterState(State_Stopped);
	}

#ifdef _DEBUG
	CString str;
	int nCount = m_SentBufferList.GetCount();
	str.Format(_T("SentBuffers:%d"), nCount);
	DEBUG_TRACE(str);
#endif

	ReleaseAllBuffers();
	HRESULT hr = CSource::Stop();
	m_bIsStopping = FALSE;
	return hr;
}


STDMETHODIMP CPushSource::GetState(DWORD dw, FILTER_STATE *State)
{
	*State = m_State;
	if (m_State == State_Paused)
		return VFW_S_CANT_CUE;
	else
	return S_OK;
}

///////////////////////////////////////////////////////////////////////
// GetParams: Get the filter parameters to a given destination
///////////////////////////////////////////////////////////////////////
STDMETHODIMP CPushSource::GetParams(PushSourceParams *irp)
{
	CAutoLock lock(pStateLock ());
	CheckPointer(irp,E_POINTER);

	*irp = m_PushSourceParams;

	return NOERROR;
}

///////////////////////////////////////////////////////////////////////
// SetParams: Set the filter parameters
///////////////////////////////////////////////////////////////////////
STDMETHODIMP CPushSource::SetParams(PushSourceParams *irp)
{
	if (irp == NULL)
	{
		return E_POINTER;
	}
	HRESULT hr = S_OK;
	CAutoLock lock(pStateLock ());
	PushSourceParams OldParam = m_PushSourceParams;
	m_PushSourceParams = *irp;
	int nCount = 0;
	if (m_PushSourceParams.m_bCapture) nCount++;
	if (m_PushSourceParams.m_bPreview) nCount++;
	if (m_PushSourceParams.m_bStill  ) nCount++;

	if (nCount > 1)	// es kann nur einen geben, noch jedenfalls
	{
		m_PushSourceParams.m_bCapture = 0;	// bei Fehler einfachen Pin erzeugen
		m_PushSourceParams.m_bPreview = 0;
		m_PushSourceParams.m_bStill   = 0;
		
	}

	SetDirty(TRUE);
	BOOL bSavedALL = saveConfig();
	if (!bSavedALL)
	{
		SetDirty(FALSE);
		m_PushSourceParams = OldParam;
		m_PushSourceParams.m_nOffsetInMS = irp->m_nOffsetInMS;
		return S_FALSE;
	}
	m_PushSourceParams.m_dwFlags &= ~PS_FLAG_SAVE_ONLY_FIRST_TIME;

	GUID guid;
	ZERO_INIT(guid);
	if (m_PushSourceParams.m_bCapture) guid = PIN_CATEGORY_CAPTURE;
	if (m_PushSourceParams.m_bPreview) guid = PIN_CATEGORY_PREVIEW;
	if (m_PushSourceParams.m_bStill  ) guid = PIN_CATEGORY_STILL;
	((CPushSourceStream*)m_paStreams[0])->Set(AMPROPSETID_Pin, AMPROPERTY_PIN_CATEGORY, NULL, 0, (void*)&guid, sizeof(GUID));

	LONG lFlags = 0;
	if (!m_PushSourceParams.m_bLiveSource) lFlags |= AM_PUSHSOURCECAPS_NOT_LIVE;
	if ( m_PushSourceParams.m_bInternalRM) lFlags |= AM_PUSHSOURCECAPS_INTERNAL_RM;
	((CPushSourceStream*)m_paStreams[0])->SetPushSourceFlags(lFlags);

	return hr;
} 
// SetBuffer
STDMETHODIMP CPushSource::SetBuffer(PushSourceProperties *pMSP)
{
	if (!pMSP)							// Free all buffers that are not already sent
	{
		CAutoLock lock(pStateLock ());
		int nCount = m_BufferList.GetCount();
		while (m_BufferList.GetCount())
		{
			pMSP = m_BufferList.GetHead();
			ReleaseMediaBuffer(pMSP->pBuffer);
		}
		return (HRESULT)nCount;
	}

	if (!pMSP->pBuffer) return E_POINTER;
	if (!pMSP->nLength) return E_INVALIDARG;

	HRESULT hr = NOERROR;
	{ // lock this scope
		CAutoLock lock(pStateLock ());
		if (pMSP->bIsSyncPoint == 2)
		{
			TRACE(_T("Reseting times SP:%d, SR: %d\n"),
				UNITS_100NS_TO_MILLISECONDS(m_tStart),
				UNITS_100NS_TO_MILLISECONDS(m_rtStartTime));
			ZERO_INIT(m_rtStartTime);
//			ZERO_INIT(m_tStart);
		}

		if (m_nBufferType == BUFFER_ATTACH)						// attach the buffer,
		{														// copy the properties
			m_BufferList.AddTail(new PushSourceProperties(pMSP));// and add to the list
		}
		else if (m_nBufferType == BUFFER_COPY)					// copy the buffer
		{														// and the properties
			PushSourceProperties *pNew = new PushSourceProperties(pMSP);
			pNew->pBuffer = (BYTE*)CoTaskMemAlloc(pMSP->nLength);// allocate the buffer, 
			if (!pNew->pBuffer)
			{
				delete pNew;
				return E_OUTOFMEMORY;
			}
			CopyMemory(pNew->pBuffer, pMSP->pBuffer, pMSP->nLength);// copy
			m_BufferList.AddTail(pNew);							// and add to the list
		}
		else if (m_nBufferType == BUFFER_FIXED)		// fixed buffer
		{
			if (!m_pSampleProperties)							// no list, only one fixed buffer
			{													// with properties
				m_pSampleProperties = new PushSourceProperties();
			}
			if (m_pSampleProperties->pBuffer != pMSP->pBuffer)
			{													// different buffer
				m_bBufferChanged = TRUE;						// notify buffer is changed
			}
			if (m_pSampleProperties->nLength > m_allocprops.cbBuffer)
			{													// different allocator properties
				m_bReCommit = TRUE;								// Recomit is neccessary
			}
			*m_pSampleProperties = *pMSP;						// copy further properites
			m_allocprops.cBuffers = 1;							// set No. of buffers to 1
		}
	}
	return hr;
}
// End SetBuffer
// SetBufferType
STDMETHODIMP CPushSource::SetBufferType(int nBufferType, RELEASE_BUFFER_FUNCTION pFnc, long lParam, ALLOCATOR_PROPERTIES*pAP)
{
	CAutoLock lock(pStateLock ());
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

	if (nBufferType == BUFFER_ATTACH)						// attached buffers
	{
		if (pFnc == NULL) return E_POINTER;					// need a detach function !
	}

	m_pReleaseBufferFunction = pFnc;						// release or detach function
	m_lParamRBF              = lParam;						// additional parameter
	m_nBufferType            = nBufferType;					// type of the buffer
	
	if (pAP)
	{
		m_allocprops = *pAP;								// set allocator properties
	}
	return hr;
}
// End SetBufferType

// SetMediaType
STDMETHODIMP CPushSource::SetMediaType(AM_MEDIA_TYPE*pMT)
{
	CAutoLock lock(pStateLock ());
	if (pMT)
	{
		AM_MEDIA_TYPE *pM = CreateMediaType(pMT);
		m_MediaTypes.AddTail(pM);
	}
	else
	{
		while (m_MediaTypes.GetCount())
		{
			AM_MEDIA_TYPE *pM = m_MediaTypes.RemoveHead();
			DeleteMediaType(pM);
		}
	}
	return NOERROR;
}
// End SetMediaType
STDMETHODIMP CPushSource::GetBuffer(PushSourceProperties *pMSP)
{
	if (pMSP == NULL) return E_POINTER;
	CAutoLock lock(pStateLock ());
	HRESULT hr = E_FAIL;
	if (m_pSampleProperties)
	{
		*pMSP = *m_pSampleProperties;
		hr = S_OK;
	}
	else
	{
		if (m_BufferList.GetCount())
		{
			*pMSP = *m_BufferList.GetHead();
			hr = S_OK;
		}
	}
	return hr;
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CPushSource::GetBufferType(int &nBufferType, ALLOCATOR_PROPERTIES*pAP)
{
	if (pAP == NULL) return E_POINTER;
	CAutoLock lock(pStateLock ());
	*pAP        = m_allocprops;
	nBufferType = m_nBufferType;
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CPushSource::GetSourceMediaType(AM_MEDIA_TYPE **ppMT)
{
	if (ppMT == NULL) return E_POINTER;
	CAutoLock lock(pStateLock ());
	if (m_pCurrentMediaType)
	{
		*ppMT = CreateMediaType(m_pCurrentMediaType);
		return S_OK;
	}
	return E_FAIL;
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CPushSource::SetPropertyPageHWND(HWND hwnd)
{
	m_hWndPropertyPage = hwnd;
	if (m_hWndPropertyPage)
	{
		CString strInfo, sAlloctor, sBufferType;
		ALLOCATOR_PROPERTIES props;
		if (m_pDirectAllocator)
		{
			m_pDirectAllocator->GetProperties(&props);
			sAlloctor = _T("Own");
		}
		else if (m_paStreams[0] && ((CPushSourceStream*)m_paStreams[0])->GetAllocator())
		{
			((CPushSourceStream*)m_paStreams[0])->GetAllocator()->GetProperties(&props);
			sAlloctor = _T("Other");
		}
		switch (m_nBufferType)
		{
			case BUFFER_FIXED:	sBufferType = _T("fixed"); break;
			case BUFFER_COPY:	sBufferType = _T("copy"); break;
			case BUFFER_ATTACH:	sBufferType = _T("attach"); break;
			default:			sBufferType = _T("undefined"); break;
		}
		strInfo.Format(_T("Allocator: %s\r\nBuffers: %d\r\nSize: %d\r\nAlign: %d\r\nPrefix: %d\r\nBuffertype:%s"), sAlloctor, props.cBuffers, props.cbBuffer, props.cbAlign, props.cbPrefix, sBufferType);
		sAlloctor = strInfo;
		if (m_pCurrentMediaType)
		{
			strInfo.Format(_T("\r\nMajortype: %s\r\nSubtype: %s\r\nFormattype: %s"), 
				CString(GuidNames[m_pCurrentMediaType->majortype]), 
				CString(GuidNames[m_pCurrentMediaType->subtype]), 
				CString(GuidNames[m_pCurrentMediaType->formattype]));
			strInfo = sAlloctor + strInfo;
		}
		SetDlgItemText(m_hWndPropertyPage, IDC_EDT_INFO, strInfo);
	}
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CPushSource::SetStreamOffset(REFERENCE_TIME rt)
{
	if (m_paStreams)
	{
		((CPushSourceStream*)m_paStreams[0])->SetStreamOffset(rt);
		return S_OK;
	}
	return E_FAIL;
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CPushSource::GetStreamOffset(REFERENCE_TIME *prt)
{
	if (m_paStreams)
	{
		((CPushSourceStream*)m_paStreams[0])->GetStreamOffset(prt);
		return S_OK;
	}
	return E_FAIL;
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CPushSource::Notify(IBaseFilter *pSelf, Quality q)
{
	DEBUG_TRACE(_T("CPushSource::Notify"));
	// TODO! CPushSource::IQualityControl::Notify()
	return E_NOTIMPL;
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CPushSource::SetSink(IQualityControl *piqc)
{
	DEBUG_TRACE(_T("CPushSource::SetSink"));
	// TODO! CPushSource::IQualityControl::SetSink()
	
	return E_NOTIMPL;
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(ULONG) CPushSource::GetMiscFlags(void)
{
	DEBUG_TRACE(_T("CPushSource::GetMiscFlags"));
	return m_PushSourceParams.m_lMiscFlags;
}
/*
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CPushSource::GetAverageFrameSize(long *plAverageSize)
{
	DEBUG_TRACE(_T("CPushSource::GetAverageFrameSize"));
	return E_NOTIMPL;
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CPushSource::GetDroppedInfo(long lSize, long *plArray, long *plNumCopied)
{
	DEBUG_TRACE(_T("CPushSource::GetDroppedInfo"));
	return E_NOTIMPL;
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CPushSource::GetNumDropped(long *plDropped)
{
	DEBUG_TRACE(_T("CPushSource::GetNumDropped"));
	return E_NOTIMPL;
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CPushSource::GetNumNotDropped(long *plNotDropped)
{
	DEBUG_TRACE(_T("CPushSource::GetNumNotDropped"));
	return E_NOTIMPL;
}
*/
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CPushSource::Set(REFGUID guidPropSet,DWORD dwPropID,
							LPVOID pInstanceData, DWORD cbInstanceData,
							LPVOID pPropData,DWORD cbPropData)
{
	DEBUG_TRACE(_T("CPushSource::Set"));
	return E_NOTIMPL;
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CPushSource::Get(REFGUID guidPropSet, DWORD dwPropID,
							LPVOID pInstanceData, DWORD cbInstanceData,
							LPVOID pPropData, DWORD cbPropData, DWORD *pcbReturned)
{
	DEBUG_TRACE(_T("CPushSource::Get"));
	return E_NOTIMPL;
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CPushSource::QuerySupported(REFGUID guidPropSet,DWORD dwPropID,DWORD *pTypeSupport)
{
	DEBUG_TRACE(_T("CPushSource::QuerySupported:"));

	return E_NOTIMPL;
}
//////////////////////////////////////////////////////////////////////////
void CPushSource::Trace(LPCTSTR str)
{
#ifdef _DEBUG
	CString sOut = str;
	sOut += _T("\n");
	OutputDebugStr(sOut);
#else
	if (gm_hWndThread)
	{
		PostMessage(gm_hWndThread, WM_TRACE_TEXT, (WPARAM)gm_sModuleName, (LPARAM)_tcsdup(str));
	}
#endif
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
