// DirectShowThread.cpp : implementation file
//

#include "stdafx.h"
#include "IPCameraUnit.h"
#include "DirectShowThread.h"
#include "IPCameraUnitDlg.h"

LPCTSTR NameOfEnum(OAFilterState fs)
{
	switch(fs)
	{
		NAME_OF_ENUM(State_Stopped);
		NAME_OF_ENUM(State_Paused);
		NAME_OF_ENUM(State_Running);
	}
	return _T("");
}

/////////////////////////////////////////////////////////////////////////////
// CDirectShowThread
IMPLEMENT_DYNAMIC(CDirectShowThread, CWK_WinThread)

/////////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : CDirectShowThread
 Function   : CDirectShowThread
 Description: Constructor for a DirectShow thread object

 Parameters:   
  CStringsName: (E): Name of the Thread  (const)
  dwFlags     : (E): Flags for behaviour  (DWORD)
	- DSTF_RECEIVE_NOTIFICATIONS	Thread receives notifications from the graph
	- DSTF_INIT_MEDIA_CONTROL		Enables to control the graph
	- DSTF_PROVIDE_KEY				Provide a security key for ASF files
	- DSTF_PREVIEW					Initialise audio or video preview
	- DSTF_INIT_FILTERS_AT_START	Call InitFilters at Thread initialisation
	- DSTF_NO_GRAPH_INIT			Do not create a DirectShow graph
	- DSTF_START_AFTER_INIT			Start graph immediately after initialisation
	- DSTF_PARAM_DIRTY				Causes reinitialisation of the graph
	- DSTF_NO_REINIT_IF_DIRTY		Prevents from reinitialisation
	- DSTF_FILTER_INITIALISED		Indicates that the filters are initialised
	- DSTF_USER_MIN					Start posititon for User defined flags

  Author: Rolf Kary-Ehlers
 created: June, 28 2006
 <TITLE CDirectShowThread>
 <GROUP DirectShow>
 <TOPICORDER 0>
*********************************************************************************************/
CDirectShowThread::CDirectShowThread(const CString&sName, DWORD dwFlags) :
	CWK_WinThread(sName)
{
	m_dwFlags = dwFlags;
	m_dwRegisterROT = 0;
	m_nOutstandingConfirms = 0;
	m_pGraph = NULL;
	m_pGraphBuilder = NULL;
	m_pMediaControl = NULL;
	m_pMediaEvent = NULL;
	m_sMediaState = State_Stopped;
	m_pRenderer = 0;
	m_dwStopTimeout = 5000;
	WK_TRACE(_T("creating CDirectShowThread(%s)\n"), sName);
}
/////////////////////////////////////////////////////////////////////////////
CDirectShowThread::~CDirectShowThread()
{

}
/////////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : CDirectShowThread
 Function   : InitInstance, virtual
 Description: Initialises the filter graph, Calling the base class function is necessary

 Parameters: None 

 Author: Rolf Kary-Ehlers
 Result type: TRUE, FALSE (BOOL)
 created: June, 28 2006
 <TITLE InitInstance>
 <GROUP DirectShow>
*********************************************************************************************/
BOOL CDirectShowThread::InitInstance()
{
	CWinThread::InitInstance();
	BOOL bReturn = TRUE;
	HRESULT hr;
	if (!(m_dwFlags & DSTF_NO_GRAPH_INIT))
	{
		try
		{	
			hr = CoInitialize(0);
			HRESULT_EXCEPTION(hr);
			hr = CoCreateInstance(CLSID_FilterGraphNoThread, NULL, CLSCTX_INPROC, IID_IGraphBuilder, (void **)&m_pGraph);
			HRESULT_EXCEPTION(hr);
			// Create the Capture Graph Builder.
			hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC, IID_ICaptureGraphBuilder2, (void **)&m_pGraphBuilder);
			HRESULT_EXCEPTION(hr);
			// Associate the graph with the builder.
			hr = m_pGraphBuilder->SetFiltergraph(m_pGraph);
			HRESULT_EXCEPTION(hr);

			if (m_dwFlags & DSTF_INIT_MEDIA_CONTROL)
			{
				hr = m_pGraph->QueryInterface(IID_IMediaControl, (void **)&m_pMediaControl);
				HRESULT_EXCEPTION(hr);
			}

			if (m_dwFlags & DSTF_RECEIVE_NOTIFICATIONS)
			{
				hr = m_pGraph->QueryInterface(IID_IMediaEventEx, (void **)&m_pMediaEvent);
				HRESULT_EXCEPTION(hr);
				hr = m_pMediaEvent->SetNotifyWindow((OAHWND)theApp.GetDlg()->m_hWnd, WM_GRAPHNOTIFY, (LONG_PTR)this);
				HRESULT_EXCEPTION(hr);
			}

			if (m_dwFlags & DSTF_PROVIDE_KEY)
			{
				// Give the graph an IObjectWithSite pointer for callbacks and QueryService.
				IObjectWithSite* pObjectWithSite = NULL;
				hr = m_pGraph->QueryInterface(IID_IObjectWithSite, (void**)&pObjectWithSite);
				if (SUCCEEDED(hr))
				{
					// Use the IObjectWithSite pointer to specify our key provider object.
					// The filter graph manager will use this pointer to call
					// QueryService to do the unlocking.
					// If the unlocking succeeds, then we can build our graph.
					pObjectWithSite->SetSite((IUnknown *) (IServiceProvider *) &theApp.m_prov);
					pObjectWithSite->Release();
				}
			}
		}
		catch(ErrorStruct *ps)
		{
			ReportError(ps);
			bReturn = FALSE;
		}
	}
	if (   bReturn 
		&& m_dwFlags & DSTF_INIT_FILTERS_AT_START)
	{
		SetDirty(FALSE);
		InitFilters();
	}
	if (IsInitialised() && m_dwFlags & DSTF_START_AFTER_INIT)
	{
		m_dwFlags &= ~DSTF_START_AFTER_INIT;
		OnSetMediaState(State_Running, 0);
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : CDirectShowThread
 Function   : StopThread, virtual
 Description: Stops the thread with PostQuitMessage(0), Calling the base class function is 
              necessary

 Parameters: None 

 Result type: Thread stopped (BOOL)
 Author: Rolf Kary-Ehlers
 created: June, 28 2006
 <TITLE StopThread>
 <GROUP DirectShow>
 <TOPICORDER 0>
*********************************************************************************************/
BOOL CDirectShowThread::StopThread()
{
	SetMediaState(State_Stopped);
	SetFilters(RELEASED);
	return CWK_WinThread::StopThread();
}
/////////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : CDirectShowThread
 Function   : ExitInstance, virtual
 Description: Stops the graph and cleans up the graph, Calling the base class function is 
              necessary

 Parameters: None 

 Result type: Exit code (int)
 Author: Rolf Kary-Ehlers
 created: June, 28 2006
 <TITLE ExitInstance, virtual>
 <GROUP DirectShow>
 <TOPICORDER 0>
*********************************************************************************************/
int CDirectShowThread::ExitInstance()
{
	OnSetMediaState((WPARAM)State_Stopped, 0);
	ReleaseFilters();

	if ((m_dwFlags & DSTF_NO_GRAPH_INIT) == 0)
	{
		if (m_dwRegisterROT)
		{
			RemoveGraphFromRot(m_dwRegisterROT);
			m_dwRegisterROT = 0;
		}
		RELEASE_OBJECT(m_pMediaControl);
		if (m_pMediaEvent)
		{
			m_pMediaEvent->SetNotifyWindow(NULL, 0, 0);
		}
		RELEASE_OBJECT(m_pMediaEvent);
		RELEASE_OBJECT(m_pGraph);
		RELEASE_OBJECT(m_pGraphBuilder);
		CoUninitialize();
	}

	return CWinThread::ExitInstance();
}
/////////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : CDirectShowThread
 Function   : InitFilters, virtual
 Description: Initialises the graph filters. Calling the base class function is necessary

 Parameters: None 

 Result type: Error code at failure (HRESULT)
 Author: Rolf Kary-Ehlers
 created: June, 28 2006
 <TITLE InitFilters, virtual>
 <GROUP DirectShow>
*********************************************************************************************/
HRESULT CDirectShowThread::InitFilters()
{
	ASSERT(GetCurrentThreadId() == m_nThreadID);
	// Baseclass sets initialised flag and resets dirty flag
	m_dwFlags |= DSTF_FILTER_INITIALISED;
	SetDirty(FALSE);
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : CDirectShowThread
 Function   : ReleaseFilters, virtual
 Description: Cleans up the Graph filters

 Parameters: None 

 Result type:  (void)
 Author: Rolf Kary-Ehlers
 created: June, 28 2006
 <TITLE ReleaseFilters>
 <GROUP DirectShow>
 <TOPICORDER 0>
*********************************************************************************************/
void CDirectShowThread::ReleaseFilters()
{
	ASSERT(GetCurrentThreadId() == m_nThreadID);
	// Baseclass removes all filters left in the graph
	// so call it at the end of the virtual function of the derived class
	RemoveAndReleaseFilter(m_pRenderer);
	if (m_pGraph)
	{
		IEnumFilters *pEnum = NULL;
		IBaseFilter  *pFilter = NULL;
		ULONG cFetched;
		CPtrList filters;
		HRESULT hr = m_pGraph->EnumFilters(&pEnum);
		if (SUCCEEDED(hr))
		{
			while(pEnum->Next(1, &pFilter, &cFetched) == S_OK)
			{
				filters.AddTail(pFilter);
			}
			POSITION pos = filters.GetHeadPosition();
			while (pos)
			{
				pFilter = (IBaseFilter*) filters.GetNext(pos);
				RemoveAndReleaseFilter(pFilter);
			}
		}
		RELEASE_OBJECT(pEnum);
	}

	// Baseclass resets initialised flag
	m_dwFlags &= ~DSTF_FILTER_INITIALISED;
}
/////////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : CDirectShowThread
 Function   : RemoveAndReleaseFilter
 Description: Removes a filter from the Graph and releases it.

 Parameters:   
 pFilter : (EA): Filter object  (IBaseFilter*&)

 Result type:  (void)
 Author: Rolf Kary-Ehlers
 created: June, 28 2006
 <TITLE RemoveAndReleaseFilter>
 <GROUP DirectShow>
 <TOPICORDER 0>
*********************************************************************************************/
void CDirectShowThread::RemoveAndReleaseFilter(IBaseFilter*&pFilter)
{
	if (m_pGraph && pFilter)
	{
		m_pGraph->RemoveFilter(pFilter);
		pFilter->Release();
		pFilter = NULL;
	}
}
/////////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : CDirectShowThread
 Function   : OnEvent, virtual
 Description: Notifies, when an event occures

 Parameters:   
  lCode  : (E): Event code  (LONG)
  lParam1: (E): Parameter 1  (LONG)
  lParam2: (E): Parameter 2  (LONG)

 Result type: return S_OK to free parameters (HRESULT)
 Author: Rolf Kary-Ehlers
 created: June, 28 2006
 <TITLE OnEvent, virtual>
 <GROUP DirectShow>
 <TOPICORDER 0>
*********************************************************************************************/
HRESULT CDirectShowThread::OnEvent(LONG lCode, LONG lParam1, LONG lParam2)
{
	CString str;
	switch (lCode)
	{
		case EC_DEVICE_LOST:
			StopThread();
			break;
		case EC_COMPLETE:
			if (m_pMediaControl)
			{
				m_pMediaControl->GetState(0, &m_sMediaState);
			}
			break;
		default:
			str.Format(_T("Event(%d, %x), %x, %x\r\n"), lCode, lCode, lParam1, lParam1);
			break;
	}
	// Free event parameters to prevent memory leaks
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : CDirectShowThread
 Function   : PostToMainWnd
 Description: Posts a message to the main window

 Parameters:   
  uMsg  : (E): Message Id  (UINT)
  wParam: (E): Parameter 1  (WPARAM)
  lParam: (E): Parameter 2  (LPARAM)

 Result type: Message was posted (BOOL)
 Author: Rolf Kary-Ehlers
 created: June, 28 2006
 <TITLE PostToMainWnd>
 <GROUP DirectShow>
 <TOPICORDER 0>
*********************************************************************************************/
BOOL CDirectShowThread::PostToMainWnd(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return theApp.GetDlg()->PostMessage(uMsg, wParam, lParam);
}
/////////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : CDirectShowThread
 Function   : ReportError
 Description: Posts a WM_HRESULT_ERROR message to the main window

 Parameters:   
  e        : (E): Pointer in WPARAM, must be deleted MainWindowFunction (ErrorStruct*)
  [bMsgBox]: (E): Value in LPARAM, Default=TRUE  (BOOL)

 Result type:  (void)
 Author: Rolf Kary-Ehlers
 created: June, 28 2006
 <TITLE ReportError>
 <GROUP DirectShow>
 <TOPICORDER 0>
*********************************************************************************************/
void CDirectShowThread::ReportError(ErrorStruct*pe, BOOL bMsgBox/*=TRUE*/)
{
	PostToMainWnd(WM_HRESULT_ERROR, (WPARAM)pe, bMsgBox);
}
/////////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : CDirectShowThread
 Function   : SetMediaState
 Description: Sets the new mediastate

 Parameters:   
  oafs    : (E): mediastate of the graph  (OAFilterState)
				 - State_Stopped
				 - State_Started
				 - State_Paused
  [lParam]: (E): Default=0  (LPARAM)
				 - STOP_WHEN_READY (State_Stopped)

 Result type:  (BOOL)
 Author: Rolf Kary-Ehlers
 created: June, 28 2006
 <TITLE SetMediaState>
 <GROUP DirectShow>
 <TOPICORDER 0>
*********************************************************************************************/
BOOL CDirectShowThread::SetMediaState(OAFilterState oafs, LPARAM lParam/*=0*/)
{
	BOOL b = FALSE;
	if (m_hThread)
	{
		b = PostThreadMessage(WM_SET_MEDIA_STATE, (WPARAM)oafs, lParam);
	}
	if (!b)
	{
		TRACE(_T("SetMediaState failed %s(%s, %d)\n"), GetName(), NameOfEnum(oafs), lParam, GetTickCountEx());
	}
	return b;
}
/////////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : CDirectShowThread
 Function   : SetFilters
 Description: Posts a message to the thread to change the filter graph

 Parameters:   
  nSet  : (E): Change Option  (int)
				- RELEASED		Releases all filters
				- INITIALISED	Initiailises all filters
				- REGISTERED	Registers the graph to ROT
				- UNREGISTERED	Unregisters graph from ROT
				- PREVIEWED		Starts Preview of the graph
				- UNPREVIEWED	Stops preview of the graph
  lParam: (E): Additional Parameter  (LPARAM)

 Result type:  (BOOL)
 Author: Rolf Kary-Ehlers
 created: June, 28 2006
 <TITLE SetFilters>
 <GROUP DirectShow>
 <TOPICORDER 0>
*********************************************************************************************/
BOOL CDirectShowThread::SetFilters(int nSet, LPARAM lParam)
{
	if (m_hThread)
	{
		return PostThreadMessage(WM_SET_FILTERS, (WPARAM)nSet, lParam);
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : CDirectShowThread
 Function   : SetDirty
 Description: Sets the graphs parameter dirty to reinitialise the filters

 Parameters:   
  bSet: (E): Dirtyflag (TRUE, FALSE)  (BOOL)

 Result type:  (void)
 Author: Rolf Kary-Ehlers
 created: June, 28 2006
 <TITLE SetDirty>
 <GROUP DirectShow>
 <TOPICORDER 0>
*********************************************************************************************/
void CDirectShowThread::SetDirty(BOOL bSet)
{
	if (bSet)
	{
		m_dwFlags |= DSTF_PARAM_DIRTY;
	}
	else
	{
		m_dwFlags &= ~DSTF_PARAM_DIRTY;
	}
}
/////////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : CDirectShowThread
 Function   : LoadGraph
 Description: Loads a graph from a file (GraphEdit)

 Parameters:   
  sGraph: (E): Path of the file  (CString)

 Result type:  (BOOL)
 Author: Rolf Kary-Ehlers
 created: June, 28 2006
 <TITLE LoadGraph>
 <GROUP DirectShow>
 <TOPICORDER 0>
*********************************************************************************************/
BOOL CDirectShowThread::LoadGraph(CString sGraph)
{
	ASSERT(GetCurrentThreadId() == m_nThreadID);
	HRESULT hr;
	CString sStreamName = _T("ActiveMovieGraph");
	LPCOLESTR sFile = sGraph.AllocSysString();
	IStorage *pStorage = NULL;
	IPersistStream *pPersistStream = NULL;
	IStream *pStream = NULL;
	IEnumSTATSTG *pEnumS = NULL;
	try
	{
		hr = StgOpenStorage(sFile, 0, STGM_TRANSACTED|STGM_READ|STGM_SHARE_DENY_WRITE, 0, 0, &pStorage);
		HRESULT_EXCEPTION(hr);
		hr = m_pGraph->QueryInterface(IID_IPersistStream, reinterpret_cast<void**>(&pPersistStream));
		HRESULT_EXCEPTION(hr);
		hr = pStorage->EnumElements(0, NULL, 0, &pEnumS);
		if (SUCCEEDED(hr))
		{
			STATSTG s;
			ULONG uRet = 1;
			if (SUCCEEDED(pEnumS->Next(1, &s, &uRet)) && uRet)
			{
				sStreamName = CString(s.pwcsName);
			}
		}
		LPCOLESTR sStream = sStreamName.AllocSysString();
		hr = pStorage->OpenStream(sStream, 0, STGM_READ|STGM_SHARE_EXCLUSIVE, 0, &pStream);
		SysFreeString((BSTR)sStream);
		HRESULT_EXCEPTION(hr);
		hr = pPersistStream->Load(pStream);
		HRESULT_EXCEPTION(hr);
	}
	catch(ErrorStruct *ps)
	{
		ReportError(ps);
	}
	RELEASE_OBJECT(pStorage);
	RELEASE_OBJECT(pStream);
	RELEASE_OBJECT(pPersistStream);
	RELEASE_OBJECT(pEnumS);
	SysFreeString((BSTR)sFile);
	return SUCCEEDED(hr);
}
/////////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : CDirectShowThread
 Function   : DoSelfCheck
 Description: Performs a self check for the threads message loop

 Parameters: None 

 Result type:  (void)
 Author: Rolf Kary-Ehlers
 created: June, 28 2006
 <TITLE DoSelfCheck>
 <GROUP DirectShow>
 <TOPICORDER 0>
*********************************************************************************************/
void CDirectShowThread::DoSelfCheck()
{
	if (m_hThread)
	{
		m_nOutstandingConfirms++;
		PostThreadMessage(WM_SELFCHECK, 0, 0);
	}
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDirectShowThread, CWK_WinThread)
	//{{AFX_MSG_MAP(CDirectShowThread)
	ON_THREAD_MESSAGE(WM_SET_MEDIA_STATE, OnSetMediaState)
	ON_THREAD_MESSAGE(WM_GRAPHNOTIFY, OnGraphNotify)
	ON_THREAD_MESSAGE(WM_SET_FILTERS, OnSetFilters)
	ON_THREAD_MESSAGE(WM_SELFCHECK, OnConfirmSelfcheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDirectShowThread message handlers
/*********************************************************************************************
 Class      : CDirectShowThread
 Function   : OnGraphNotify
 Description: Receives WM_GRAPHNOTIFIY message from the applications main window

 Parameters:   
 Empty : (unused):    (WPARAM, LPARAM)

 Result type:  (void)
 Author: Rolf Kary-Ehlers
 created: June, 28 2006
 <TITLE OnGraphNotify>
 <GROUP DirectShow>
 <TOPICORDER 0>
*********************************************************************************************/
void CDirectShowThread::OnGraphNotify(WPARAM, LPARAM)
{
	LONG evCode, evParam1, evParam2;
	HRESULT hr=S_OK;
	if (m_pMediaEvent)
	{
		while(SUCCEEDED(m_pMediaEvent->GetEvent(&evCode, (LONG_PTR *) &evParam1, (LONG_PTR *) &evParam2, 0)))
		{
			hr = OnEvent(evCode, evParam1, evParam2);
			if (hr == S_OK)
			{
				hr = m_pMediaEvent->FreeEventParams(evCode, evParam1, evParam2);
				if (FAILED(hr))
				{
					ReportError(HRESULT_ERROR_AT_POS(hr), FALSE);
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : CDirectShowThread
 Function   : OnSetMediaState
 Description: Receives the message to change the media state

 Parameters:   
  wParam: (E): new media state  (WPARAM)
  lParam: (E): addiational parameter  (LPARAM)

 Result type:  (void)
 Author: Rolf Kary-Ehlers
 created: June, 28 2006
 <TITLE OnSetMediaState>
 <GROUP DirectShow>
 <TOPICORDER 0>
*********************************************************************************************/
void CDirectShowThread::OnSetMediaState(WPARAM wParam, LPARAM lParam)
{
	OAFilterState oafs = (OAFilterState) wParam;
	TRACE(_T("OnSetMediaState %s(%s, %d): %s\n"), GetName(), NameOfEnum(oafs), lParam, GetTickCountEx());
	if (m_pMediaControl)
	{
		m_pMediaControl->GetState(0, &m_sMediaState);
	}
	HRESULT hr = S_OK;
	if (oafs != m_sMediaState || IsDirty())
	{
		hr = E_FAIL;
		switch (oafs)
		{
			case State_Running:
				if (IsDirty())
				{
					if (m_dwFlags & DSTF_NO_REINIT_IF_DIRTY)
					{
						InitFilters();
					}
					else
					{
						ReleaseFilters();
					}
				}
				if (!IsInitialised())
				{
					InitFilters();
				}
				if (IsInitialised())
				{
					if (m_pMediaControl)
					{
						if (lParam)
						{
							hr = m_pMediaControl->Run();
						}
						else
						{
							hr = m_pMediaControl->Run();
						}
					}
					else
					{
						hr = OnEvent(EC_SET_MEDIA_STATE, oafs, 0);
					}
				} 
				break;
			case State_Paused:
				if (m_pMediaControl)
				{
					hr = m_pMediaControl->Pause();
				}
				else
				{
					hr = OnEvent(EC_SET_MEDIA_STATE, oafs, 0);
				}
				break;
			case State_Stopped:
				if (m_pMediaControl)
				{
					if (lParam == STOP_WHEN_READY)
					{
						hr = m_pMediaControl->StopWhenReady();
					}
					else
					{
						hr = m_pMediaControl->Stop();
					}
				}
				else
				{
					hr = OnEvent(EC_SET_MEDIA_STATE, oafs, 0);
				}
				break;
		} // switch (oafs)
	} // if (oafs != m_sMediaState)
	if (SUCCEEDED(hr))
	{
		m_sMediaState = oafs;
		PostToMainWnd(WM_GRAPHNOTIFY, (WPARAM)MAKELONG(DST_NOTIFICATION, EC_MEDIA_STATE_CHANGED), (LPARAM)this);
	}
}
/////////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : CDirectShowThread
 Function   : OnSetFilters
 Description: Receives the message to change the filter graph

 Parameters:   
  wParam: (E): Change option  (WPARAM)
  lParam: (E): additional paramter  (LPARAM)

 Result type:  (void)
 Author: Rolf Kary-Ehlers
 created: June, 28 2006
 <TITLE OnSetFilters>
 <GROUP DirectShow>
 <TOPICORDER 0>
*********************************************************************************************/
void CDirectShowThread::OnSetFilters(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
		case  INITIALISED:
			if (!IsInitialised())
			{
				InitFilters();
			}break;
		case  RELEASED:
			ReleaseFilters();
			break;
		case REGISTERED:
			if (m_dwRegisterROT == 0)
			{
				AddGraphToRot(m_pGraph, &m_dwRegisterROT);
			}break;
		case UNREGISTERED:
			if (m_dwRegisterROT)
			{
				RemoveGraphFromRot(m_dwRegisterROT);
				m_dwRegisterROT = 0;
			}break;
		case PREVIEWED:
			{
				m_dwFlags |= DSTF_PREVIEW;
			}break;
		case UNPREVIEWED:
			if (m_pMediaControl)
			{
				OnSetMediaState(State_Stopped, 0);
				ReleaseFilters();
				m_pActiveWnd = NULL;
				InitFilters();
				OnSetMediaState(State_Running, 0);
			}
			m_dwFlags &= ~DSTF_PREVIEW;
			break;
	}
}
/////////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : CDirectShowThread
 Function   : OnConfirmSelfcheck
 Description: Receive the Message to confirm self check

 Parameters:   
 Empty : (unused):    (WPARAM, LPARAM)

 Result type:  (void)
 Author: Rolf Kary-Ehlers
 created: June, 28 2006
 <TITLE OnConfirmSelfcheck>
 <GROUP DirectShow>
 <TOPICORDER 0>
*********************************************************************************************/
void CDirectShowThread::OnConfirmSelfcheck(WPARAM, LPARAM)
{
	m_nOutstandingConfirms--;
}

