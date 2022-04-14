// IPCamSocket.cpp : implementation file
//

#include "stdafx.h"
#include "IPCameraUnit.h"
#include "IPCamSocket.h"

#include "DsIpCamera.h"
#include "IPCameraUnitDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CIpCamSocket
CIpCamSocket::CIpCamSocket()
{
}
/////////////////////////////////////////////////////////////////////////////
CIpCamSocket::~CIpCamSocket()
{
}
/////////////////////////////////////////////////////////////////////////////
// CIpCamSocket member functions
void CIpCamSocket::OnReceive(int nErrorCode)
{
	TRACE(_T("CIpCamSocket::OnReceive(%d)\n"), nErrorCode); 
	TCHAR buff[1024];
	int nRead;
	nRead = Receive(buff, 1024) / sizeof(TCHAR); 

	switch (nRead)
	{
		case SOCKET_ERROR:
			if (GetLastError() != WSAEWOULDBLOCK) 
			{
				AfxMessageBox(_T("Error occurred"));
				Close();
			}
			break;
		default:
			if (nRead > 0 && nRead < 4096)
			{
				buff[nRead] = 0; //terminate the string
				TRACE(_T("%s\n"), buff);
			}break;
	}

	CAsyncSocket::OnReceive(nErrorCode);
}
/////////////////////////////////////////////////////////////////////////////
void CIpCamSocket::OnClose(int nErrorCode)
{
	TRACE(_T("CIpCamSocket::OnClose(%d)\n"), nErrorCode); 

	CAsyncSocket::OnClose(nErrorCode);
}
/////////////////////////////////////////////////////////////////////////////
void CIpCamSocket::OnConnect(int nErrorCode)
{
	TRACE(_T("CIpCamSocket::OnConnect(%d)\n"), nErrorCode); 

	CAsyncSocket::OnConnect(nErrorCode);
}
/////////////////////////////////////////////////////////////////////////////
void CIpCamSocket::OnAccept(int nErrorCode)
{
	TRACE(_T("CIpCamSocket::OnAccept(%d)\n"), nErrorCode); 

	CAsyncSocket::OnAccept(nErrorCode);
}
/////////////////////////////////////////////////////////////////////////////
void CIpCamSocket::OnOutOfBandData(int nErrorCode)
{
	TRACE(_T("CIpCamSocket::OnOutOfBandData(%d)\n"), nErrorCode); 

	CAsyncSocket::OnOutOfBandData(nErrorCode);
}
/////////////////////////////////////////////////////////////////////////////
void CIpCamSocket::OnSend(int nErrorCode)
{
	TRACE(_T("CIpCamSocket::OnSend(%d)\n"), nErrorCode); 
	CAsyncSocket::OnSend(nErrorCode);
}

/////////////////////////////////////////////////////////////////////////////
// CIpCamInetSession
CIpCamInetSession::CIpCamInetSession(CDsIpCamera*pCam, LPCTSTR pstrAgent, DWORD_PTR dwContext, DWORD dwAccessType, LPCTSTR pstrProxyName, LPCTSTR pstrProxyBypass, DWORD dwFlags):
	CInternetSession(pstrAgent, dwContext, dwAccessType, pstrProxyName, pstrProxyBypass, dwFlags)
{
	m_pCam = pCam;
}
/////////////////////////////////////////////////////////////////////////////
void CIpCamInetSession::OnStatusCallback(DWORD_PTR dwContext, DWORD dwInternetStatus,
		LPVOID lpvStatusInformation, DWORD dwStatusInformationLength)
{
	if (m_pCam)
	{
		m_pCam->OnInternetEvent((DWORD)dwContext, dwInternetStatus, lpvStatusInformation, dwStatusInformationLength);
	}
	else
	{
		((CWnd*)theApp.GetDlg())->PostMessage(WM_INET_STATUS, (WPARAM)dwContext, (LPARAM)dwInternetStatus);
	}
}


/////////////////////////////////////////////////////////////////////////////
// CIpCamRTCEvents
CIpCamRTCEvents::CIpCamRTCEvents()
{
	m_dwRefCount = 0;
	m_dwCookie = 0;
	m_pCam = NULL;
}

CIpCamRTCEvents::~CIpCamRTCEvents()
{

}

//////////////////////////////////////////////////////////////////////////////
// Method: AddRef()
// Parameter: None
// Return Value: Reference count
// Purpose: Keeps track of the number of connections to the event object.
//			Called from QueryInterface() method.
//////////////////////////////////////////////////////////////////////////////
ULONG STDMETHODCALLTYPE CIpCamRTCEvents::AddRef()
{
    m_dwRefCount++;
    return m_dwRefCount;
}

//////////////////////////////////////////////////////////////////////////////
// Method: Release()
// Parameter: None
// Return Value: Reference count
// Purpose: Releases connection to the event object and decrements the ref
//			count.  See SAFE_RELEASE() macro
//////////////////////////////////////////////////////////////////////////////
ULONG STDMETHODCALLTYPE CIpCamRTCEvents::Release()
{
    m_dwRefCount--;

    if ( 0 == m_dwRefCount)
    {
        delete this;
    }

    return m_dwRefCount;
}

//////////////////////////////////////////////////////////////////////////////
// Method: QueryInterface()
// Parameter: None
// Return Value: None
// Purpose: Called by RTC to create a connection point.
//////////////////////////////////////////////////////////////////////////////
HRESULT STDMETHODCALLTYPE CIpCamRTCEvents::QueryInterface(REFIID iid, void **ppvObject)
{
    if (iid == IID_IRTCEventNotification)
    {
        *ppvObject = (void *)this;
        AddRef();
        return S_OK;
    }

    if (iid == IID_IUnknown)
    {
        *ppvObject = (void *)this;
        AddRef();
        return S_OK;
    }
	return E_NOINTERFACE;
}

//////////////////////////////////////////////////////////////////////////////
// Method: Advise()
// Parameter: pClient	Client object to associate the connection point
//			  hWnd		Owner's Window handle 
// Return Value: S_OK if successful.
// Purpose: Called during initialization to create an event connection point.
//////////////////////////////////////////////////////////////////////////////
HRESULT CIpCamRTCEvents::Advise(IRTCClient *pClient, CDsIpCamera *pCam)
{
	IConnectionPointContainer * pCPC = NULL;
	IConnectionPoint * pCP = NULL;
    HRESULT hr;

    // Find the connection point container
	hr = pClient->QueryInterface(IID_IConnectionPointContainer, (void**)&pCPC);

	if (SUCCEEDED(hr))
    {
        // Find the connection point
		hr = pCPC->FindConnectionPoint(IID_IRTCEventNotification, &pCP);

        SAFE_RELEASE(pCPC);

        if (SUCCEEDED(hr))
        {
            // Advise the connection
		    hr = pCP->Advise(this, &m_dwCookie);

            SAFE_RELEASE(pCP);
        }
    }

    // Store the window handle of the application so we
    // can post messages to it when events are fired
	m_pCam = pCam;
	
	return hr;
}

//////////////////////////////////////////////////////////////////////////////
// Method: Unadvise()
// Parameter: pClient	Client object that owns the connection point.
// Return Value: S_OK if successful.
// Purpose: Releases the connection point to the event handler.
//////////////////////////////////////////////////////////////////////////////
HRESULT CIpCamRTCEvents::Unadvise(IRTCClient *pClient)
{
	IConnectionPointContainer * pCPC = NULL;
	IConnectionPoint * pCP = NULL;
    HRESULT hr = S_OK;
	m_pCam = NULL;

    if (m_dwCookie)
    {
        // Find the connection point container
	    hr = pClient->QueryInterface(IID_IConnectionPointContainer, (void**)&pCPC);

	    if (SUCCEEDED(hr))
        {
            // Find the connection point
		    hr = pCPC->FindConnectionPoint(IID_IRTCEventNotification, &pCP);

            SAFE_RELEASE(pCPC);

            if (SUCCEEDED(hr))
            {
                // Unadvise this connection
		        hr = pCP->Unadvise(m_dwCookie);

                SAFE_RELEASE(pCP);
            }
        }
    }

	return hr;
}

//////////////////////////////////////////////////////////////////////////////
// Method: Event()
// Parameter: enEvent		Type of event from the RTC object
//			  pDisp			Dispatch handle	
// Return Value: S_OK if successful.
// Purpose: Event Message handler
//////////////////////////////////////////////////////////////////////////////
HRESULT STDMETHODCALLTYPE CIpCamRTCEvents::Event(RTC_EVENT enEvent, IDispatch *pDisp)
{
	HRESULT hr = S_OK;
	// We will post a message containing the event to the
    // application window.
    
    // Add a reference to the event so we can hold onto it while
    // the event is in the message queue.
//    pDisp->AddRef();

	if (m_pCam)
	{
		hr = m_pCam->OnRTCEvent(enEvent, pDisp);
	}
    return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CIpCamDeviceFindCB
CIpCamDeviceFindCB::CIpCamDeviceFindCB(CIpCameraUnitDlg*pDlg)
{
    m_lRefCount = 0;
	m_pDlg = pDlg;
}

/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CIpCamDeviceFindCB::QueryInterface(REFIID iid, LPVOID* ppvObject)
{
    HRESULT hr = S_OK;
    
    if(NULL == ppvObject)
    {
        hr = E_POINTER;
    }
    else
    {
        *ppvObject = NULL;
    }
    
    if(SUCCEEDED(hr))
    {
        if(IsEqualIID(iid, IID_IUnknown) || IsEqualIID(iid, IID_IUPnPDeviceFinderCallback))
        {
            *ppvObject = static_cast<IUPnPDeviceFinderCallback*>(this);
            AddRef();
        }
        else
        {
            hr = E_NOINTERFACE;
        }
    }
    
    return hr;
}
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(ULONG) CIpCamDeviceFindCB::AddRef()
{
    return ::InterlockedIncrement(&m_lRefCount);
}
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(ULONG) CIpCamDeviceFindCB::Release()
{
    LONG lRefCount = ::InterlockedDecrement(&m_lRefCount);
    if(0 == lRefCount)
    {
        delete this;
    }
    
    return lRefCount;
}
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CIpCamDeviceFindCB::DeviceAdded(LONG lFindData, IUPnPDevice* pDevice)
{
    HRESULT hr = S_OK;
    
 	CString sTxt, sLF(_T("\n"));
	// 0             1                2            3
	// FriendlyName\nPresentationURL\nDescription\nSerialNumber
	BSTR bstr;
    hr = pDevice->get_FriendlyName(&bstr);
    if (SUCCEEDED(hr))
    {
		sTxt += CString(bstr) + sLF;
        ::SysFreeString(bstr);
	}
    hr = pDevice->get_PresentationURL(&bstr);
    if (SUCCEEDED(hr))
    {
		sTxt += CString(bstr) + sLF;
        ::SysFreeString(bstr);
    }
    hr = pDevice->get_Description(&bstr);
    if (SUCCEEDED(hr))
    {
		sTxt += CString(bstr) + sLF;
        ::SysFreeString(bstr);
    }
	hr = pDevice->get_SerialNumber(&bstr);
    if (SUCCEEDED(hr))
    {
		sTxt += CString(bstr);
        ::SysFreeString(bstr);
    }
	m_pDlg->m_saUpnpDevices.Add(sTxt);

    WK_TRACE(_T("Device Added: %s\n"), sTxt);
    return hr;   
}
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CIpCamDeviceFindCB::DeviceRemoved(LONG lFindData, BSTR bstrUDN)
{
    WK_TRACE(_T("Device Removed: udn: %s"), CString(bstrUDN));
    return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CIpCamDeviceFindCB::SearchComplete(LONG lFindData)
{
    WK_TRACE(_T("Search Complete\n"));
    PostQuitMessage(0);
    return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CHttpRequestThread

CHttpResult::CHttpResult(DWORD dwID, DWORD dwResult, const CString&sAnswer)
{
	m_sAnswer = sAnswer;
	m_dwID = dwID;
	m_dwResult = dwResult;
}
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CHttpRequestThread, CWK_WinThread)
/////////////////////////////////////////////////////////////////////////////
CHttpRequestThread::CHttpRequestThread(const CString&sName, CWnd*pWnd, DWORD dwThreadID) :
	CWK_WinThread(sName)
{
	m_pWnd = pWnd;
	m_dwThreadID = dwThreadID;
	m_uState = 0;
	m_pSession = NULL;
	m_pHttpCon = NULL;
	m_pFile = NULL;
	m_bAutoDelete = TRUE;
}
/////////////////////////////////////////////////////////////////////////////
CHttpRequestThread::~CHttpRequestThread()
{
	CloseFile();
	CloseConnection();
	CloseSession();
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CHttpRequestThread, CWK_WinThread)
	//{{AFX_MSG_MAP(CHttpRequestThread)
	ON_THREAD_MESSAGE(WM_USER, OnUser)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
void CHttpRequestThread::AddRequest(const CString&sUrl, UINT uPort, const CString&sRequest, BOOL bPost, DWORD dwID, UINT uState, LPCTSTR psUser/*=NULL*/, LPCTSTR psPwd/*=NULL*/, LPCTSTR psProxy/*=NULL*/, LPCTSTR psProxyBypass/*=NULL*/)
{
	CString sParams;
	sParams.Format(_T("%s%s%s%d%s%s%s%d%s%d%s%d"),
		INI_URL, sUrl,
		INI_PORT, uPort,
		_T("\\RQ"), sRequest,
		INI_ID, dwID,
		INI_STATE, uState,
		_T("\\Post"), bPost);
	if (psUser && psPwd)
	{
		sParams += _T("\\User");
		sParams += psUser;
		sParams += _T("\\Pwd");
		sParams += psPwd;
	}
	if (psProxy && psProxyBypass)
	{
		sParams += _T("\\Proxy");
		sParams += psProxy;
		sParams += _T("\\ProxyBP");
		sParams += psProxyBypass;
	}

	m_csRequest.Lock();
	m_saRequests.Add(sParams);
	m_csRequest.Unlock();
	PostThreadMessage(WM_USER, 0, 0);
}
/////////////////////////////////////////////////////////////////////////////
void CHttpRequestThread::CancelCurrentRequest()
{
	m_csFile.Lock();
	CloseFile();
	m_csFile.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
void CHttpRequestThread::CloseFile()
{
	if (m_pFile)
	{
		try
		{
			m_pFile->Close();
			delete m_pFile;
			m_pFile = NULL;
		}
		catch (CInternetException* pEx)
		{
			theApp.ReportError(pEx);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CHttpRequestThread::CloseConnection()
{
	if (m_pHttpCon)
	{
		try
		{
			m_pHttpCon->Close();
			delete m_pHttpCon;
			m_pHttpCon = NULL;
		}
		catch (CInternetException* pEx)
		{
			theApp.ReportError(pEx);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CHttpRequestThread::CloseSession()
{
	if (m_pSession)
	{
		m_pSession->Close();
		delete m_pSession;
		m_pSession = NULL;
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CHttpRequestThread::InitInstance()
{
	CWK_WinThread::InitInstance();
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
int CHttpRequestThread::ExitInstance()
{
	CloseFile();
	CloseConnection();
	CloseSession();
	return CWK_WinThread::ExitInstance();
}
/////////////////////////////////////////////////////////////////////////////
void CHttpRequestThread::OnUser(WPARAM, LPARAM)
{
	CString sParams;
	m_csRequest.Lock();
	if (m_saRequests.GetSize())
	{
		sParams = m_saRequests[0];
		m_saRequests.RemoveAt(0);
	}
	m_csRequest.Unlock();

	if (!sParams.IsEmpty())
	{
		CString sUrl         = CWK_Profile::GetStringFromString(sParams, INI_URL, NULL);
		CString sRequest     = CWK_Profile::GetStringFromString(sParams, _T("\\RQ"), NULL);
		CString sUser        = CWK_Profile::GetStringFromString(sParams, _T("\\User"), NULL);
		CString sPwd         = CWK_Profile::GetStringFromString(sParams, _T("\\Pwd"), NULL);
		CString sProxy       = CWK_Profile::GetStringFromString(sParams, _T("\\Proxy"), NULL);
		CString sProxyBypass = CWK_Profile::GetStringFromString(sParams, _T("\\ProxyBP"), NULL);
		UINT	uPort        = CWK_Profile::GetNrFromString(sParams, INI_PORT, 0);
		DWORD	dwID         = CWK_Profile::GetNrFromString(sParams, INI_ID, 0);
		BOOL	bPost        = CWK_Profile::GetNrFromString(sParams, _T("\\Post"), 0);
		m_uState             = CWK_Profile::GetNrFromString(sParams, INI_STATE, 0);
		try
		{
			if (m_pSession == NULL)
			{
				LPCTSTR pszProxyName = NULL, pszProxyBypass = NULL;
				if (!sProxy.IsEmpty())
				{
					pszProxyName = sProxy;
				}
				if (!sProxyBypass.IsEmpty())
				{
					pszProxyBypass = sProxyBypass;
				}
				DWORD dwAccessType = INTERNET_OPEN_TYPE_DIRECT;	// Connect directly to Internet
				if (pszProxyName || pszProxyBypass)
				{
					dwAccessType = INTERNET_OPEN_TYPE_PROXY;	// Connect through a CERN proxy
				}
				// PRE_CONFIG_INTERNET_ACCESS					// Connect using preconfigured settings in the registry
				m_pSession = (CInternetSession*) new CIpCamInetSession(NULL, GetName(),
					1,											// context
					dwAccessType,								
					pszProxyName, pszProxyBypass,				// Proxy name and bypass
					INTERNET_FLAG_DONT_CACHE);					// flags
		//		m_pSession->EnableStatusCallback();
			}
			LPCTSTR pszUser     = NULL;
			LPCTSTR pszPassword = NULL;
			if (!sUser.IsEmpty())
			{
				pszUser = sUser;
			}
			if (!sPwd.IsEmpty())
			{
				pszPassword = sPwd;
			}
			m_pHttpCon = m_pSession->GetHttpConnection(sUrl, (INTERNET_PORT)uPort, pszUser, pszPassword);
			m_pFile = m_pHttpCon->OpenRequest(bPost ? CHttpConnection::HTTP_VERB_PUT : CHttpConnection::HTTP_VERB_GET, sRequest,
						NULL, // pstrReferer
						1,
						NULL, // ppstrAcceptTypes
						NULL, // pstrVersion
						INTERNET_FLAG_RELOAD|INTERNET_FLAG_DONT_CACHE);
			m_pFile->SendRequest();
			if (m_uState == HRTS_REQUEST)
			{
				DWORD dwResult;
				CString sAnswer = CIpCameraUnitDlg::ReadHTTPAnswer(m_pFile, dwResult);
				CHttpResult *pResult = new CHttpResult(dwID, dwResult, sAnswer);
				if (m_pWnd)
				{
					m_pWnd->PostMessage(WM_POST_HTTP_RESULT, (WPARAM)pResult, 0);
					pResult = NULL;
				}
				else if (m_dwThreadID)
				{
					::PostThreadMessage(m_dwThreadID, WM_POST_HTTP_RESULT, (WPARAM)pResult, 0);
					pResult = NULL;
				}
				WK_DELETE(pResult);				
			}
			else if (m_uState == HRTS_POLL)
			{
				// timer zum Pollen starten
			}
		}
		catch (CInternetException* pEx)
		{		
			theApp.ReportError(pEx);
		}
	}
}
