#pragma once


class CDsIpCamera;
// CIpCamSocket command target

//////////////////////////////////////////////////////////////////////
// class CIpCamSocket
// RKE: test fuer direkte Sockekommunikation
// wird noch nicht benutzt
class CIpCamSocket : public CAsyncSocket
{
public:
	CIpCamSocket();
	virtual ~CIpCamSocket();

protected:
	//{{AFX_VIRTUAL(CIpCamSocket)
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
	virtual void OnAccept(int nErrorCode);
	virtual void OnOutOfBandData(int nErrorCode);
	virtual void OnSend(int nErrorCode);
	//}}AFX_VIRTUAL

	//{{AFX_DATA(CIpCamSocket)
	//}}AFX_DATA
};

//////////////////////////////////////////////////////////////////////
// class CIpCamInetSession
// RKE: fuer HTTP Kommunikation mit StatusCallback Informationen
class CIpCamInetSession : public CInternetSession
{
public:
	CIpCamInetSession(CDsIpCamera *pCam, LPCTSTR pstrAgent = NULL,
		DWORD_PTR dwContext = 1,
		DWORD dwAccessType = PRE_CONFIG_INTERNET_ACCESS,
		LPCTSTR pstrProxyName = NULL,
		LPCTSTR pstrProxyBypass = NULL,
		DWORD dwFlags = 0);

protected:
	virtual void OnStatusCallback(DWORD_PTR dwContext, DWORD dwInternetStatus,
		LPVOID lpvStatusInformation, DWORD dwStatusInformationLength);

	CDsIpCamera*m_pCam;
};

//////////////////////////////////////////////////////////////////////
// class CIpCamRTCEvents
// RKE: Fuer MPEG4 Protokoll ueber RTC
// Implementation ist kompliziert, Sample ist vorhanden, doku mangelhaft.
// Kommunikation ist auch ueber HTTP möglich, erst testen
class CIpCamRTCEvents : public IRTCEventNotification  
{
public:
	CIpCamRTCEvents();
	virtual ~CIpCamRTCEvents();

	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();

	HRESULT STDMETHODCALLTYPE Event(RTC_EVENT enEvent, IDispatch *pDisp);
	HRESULT Unadvise(IRTCClient *pClient);
	HRESULT Advise(IRTCClient *pClient, CDsIpCamera *pCam);
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void **ppvObject);

private:
	CDsIpCamera*m_pCam;
	DWORD m_dwCookie;
	DWORD m_dwRefCount;
};


#define UPNPDEV_FRIENDLYNAME	0
#define UPNPDEV_PRESENTATIONURL	1
#define UPNPDEV_DESCRIPTION		2
#define UPNPDEV_SERIALNUMBER	3

//////////////////////////////////////////////////////////////////////
// CIpCamDeviceFindCB
// RKE: UPnP devicefinder
class CIpCamDeviceFindCB : public IUPnPDeviceFinderCallback
{
public:
    CIpCamDeviceFindCB(CIpCameraUnitDlg*pDlg);
    
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void **ppvObject);
	ULONG	STDMETHODCALLTYPE AddRef();
	ULONG	STDMETHODCALLTYPE Release();
	HRESULT STDMETHODCALLTYPE DeviceAdded(LONG lFindData, IUPnPDevice* pDevice);
	HRESULT STDMETHODCALLTYPE DeviceRemoved(LONG lFindData, BSTR bstrUDN);
	HRESULT STDMETHODCALLTYPE SearchComplete(LONG lFindData);   
    
private:
    LONG m_lRefCount;
	CIpCameraUnitDlg *m_pDlg;
};

//////////////////////////////////////////////////////////////////////
// CHttpRequestThread
#define HRTS_REQUEST 1
#define HRTS_POLL    2
#define HRTS_POLLING 3
//////////////////////////////////////////////////////////////////////
class CHttpResult
{
public:
	CHttpResult(DWORD dwID, DWORD dwResult, const CString&sAnswer);
// members
	DWORD				m_dwID;
	DWORD				m_dwResult;
	CString				m_sAnswer;
};
//////////////////////////////////////////////////////////////////////
class CHttpRequestThread : public CWK_WinThread
{
	DECLARE_DYNAMIC(CHttpRequestThread)

//protected:
	CHttpRequestThread(const CString&sName, CWnd*pWnd, DWORD dwThreadID);
public:
	virtual ~CHttpRequestThread();

	// Attribute
public:

	// Implementation
public:
	void AddRequest(const CString&sUrl, UINT uPort, const CString&sRequest, BOOL bPost, DWORD dwID,	UINT uState, LPCTSTR psUser=NULL, LPCTSTR psPwd=NULL, LPCTSTR psProxy=NULL, LPCTSTR psProxyBypass=NULL);
	void CancelCurrentRequest();
	void CloseFile();
	void CloseConnection();
	void CloseSession();

protected:

	//{{AFX_VIRTUAL(CDirectShowThread)
	virtual BOOL InitInstance();
	virtual int ExitInstance();
public:

	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CHttpRequestThread)
	afx_msg void OnUser(WPARAM, LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	//{{AFX_DATA(CHttpRequestThread)
protected:
	//}}AFX_DATA
public:


protected:
	// Reply destinations
	CWnd*				m_pWnd;
	DWORD				m_dwThreadID; 
	// Request queue
	CCriticalSection	m_csRequest;
	CStringArray		m_saRequests;
	// Internet connection
	CCriticalSection	m_csFile;
	UINT				m_uState;
	DWORD				m_dwResult;
	CInternetSession*	m_pSession;
	CHttpConnection*	m_pHttpCon;
	CHttpFile*			m_pFile;
};
