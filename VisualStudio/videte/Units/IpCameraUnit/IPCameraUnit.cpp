// IPCameraUnit.cpp : Definiert das Klassenverhalten für die Anwendung.
//

#include "stdafx.h"
#include "IPCameraUnit.h"
#include "IPCameraUnitDlg.h"

#include "IPCamSocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// TODO! RKE: delete all these test defines
// defines to test this unit in tasha behaviour
#ifdef _TEST_TASHA
#define WAI_IP_CAMERA_UNIT	WAI_TASHAUNIT_1
#endif

/////////////////////////////////////////////////////////////////////////////
// CIpCameraUnitApp
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CIpCameraUnitApp, CWinApp)
	//{{AFX_MSG_MAP(CIpCameraUnitApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Das einzige CIpCameraUnitApp-Objekt
CIpCameraUnitApp theApp;
CWK_Timer theTimer;
LARGE_INTEGER theStart;

CString GetTickCountEx(BOOL bStart)
{
	if (bStart)
	{
		theStart = theTimer.GetMicroTicks();
	}
	else
	{
		LARGE_INTEGER liDiff, liCurrent = theTimer.GetMicroTicks();
		liDiff.QuadPart = liCurrent.QuadPart - theStart.QuadPart;
		CString s;
		s.Format(_T("%d.%d"), liDiff.LowPart/1000, liDiff.LowPart%1000);
		theStart = liCurrent;
		return s;
	}
	return _T("");
}

/////////////////////////////////////////////////////////////////////////////
// CIpCameraUnitApp-Erstellung
CIpCameraUnitApp::CIpCameraUnitApp()
{
	InitVideteLocaleLanguage();
	m_prov.AddRef();  // Don't let COM try to free our static object.
	m_pSession = NULL;
	m_pDlg = NULL;
}
/////////////////////////////////////////////////////////////////////////////
CIpCameraUnitApp::~CIpCameraUnitApp()
{
	m_pDlg = NULL;
	if (m_pSession)
	{
		m_pSession->Close();
	}
	WK_DELETE(m_pSession);
}

/////////////////////////////////////////////////////////////////////////////
CString CIpCameraUnitApp::GetAppnameFromId(WK_ApplicationId wai, int nInstance/*=-1*/)
{
	CString s = ::GetAppnameFromId(wai);
	if (nInstance == -1)
	{
		nInstance = m_nInstance;
	}
	if (nInstance > 1)
	{
		CString sNo;
		sNo.Format(_T("%d"), nInstance);
		s += sNo;
	}
	return s;
}
/////////////////////////////////////////////////////////////////////////////
// CIpCameraUnitApp Initialisierung
BOOL CIpCameraUnitApp::InitInstance()
{
	GetTickCountEx(TRUE);
	if     (!WK_IS_RUNNING(GetAppnameFromId(WAI_IP_CAMERA_UNIT, 1)))
	{
		m_nInstance = 1;
		m_AppID = WAI_IP_CAMERA_UNIT;
	}
	else if (!WK_IS_RUNNING(GetAppnameFromId(WAI_IP_CAMERA_UNIT, 2)))
	{
		m_nInstance = 2;
		m_AppID = WAI_IP_CAMERA_UNIT;
	}
	else if (!WK_IS_RUNNING(GetAppnameFromId(WAI_IP_CAMERA_UNIT, 3)))
	{
		m_nInstance = 3;
		m_AppID = WAI_IP_CAMERA_UNIT;
	}
	else if (!WK_IS_RUNNING(GetAppnameFromId(WAI_IP_CAMERA_UNIT, 4)))
	{
		m_nInstance = 4;
		m_AppID = WAI_IP_CAMERA_UNIT;
	}
	else
	{
		return FALSE;
	}

	InitCommonControls();

	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	SetRegistryKey(_T("Videte"));

	CString sAppName = GetAppnameFromId(m_AppID);
	WK_ALONE(sAppName);
	CWK_RunTimeError::SetDefaultID(m_AppID);

	// Logfilename erzeugen
	CString sLogFile = sAppName+_T(".log");

	// Debugger öffnen
	InitDebugger(sLogFile, m_AppID, NULL);

	WK_TRACE(_T("Starting IP Camera Unit..."));
	WK_PRINT_VERSION(7, 4, 16, 31, _T("$Date: 7/07/06 5:11p $"), // @AUTO_UPDATE
		0);	// DO NOT TOUCH THIS LINE

	free((void*)m_pszAppName);
	m_pszAppName = _tcsdup(COemGuiApi::GetApplicationName(m_AppID));
	AFX_MODULE_STATE*pState = AfxGetModuleState();
	if (pState)
	{
		if (pState->m_lpszCurrentAppName)
		{
			pState->m_lpszCurrentAppName = m_pszAppName;
		}
	}

	m_pDlg = new CIpCameraUnitDlg();
	if (m_pDlg && m_pDlg->Create(m_pDlg->IDD))
	{
		m_pDlg->CheckHidden();
		m_pMainWnd = m_pDlg;
		return TRUE;
	}

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
CInternetSession* CIpCameraUnitApp::GetInternetSession()
{
	if (m_pSession == NULL)
	{
		LPCTSTR pszProxyName = NULL, pszProxyBypass = NULL;
		if (!m_pDlg->m_sProxyName.IsEmpty())
		{
			pszProxyName = m_pDlg->m_sProxyName;
		}
		if (!m_pDlg->m_sProxyBypass.IsEmpty())
		{
			pszProxyBypass = m_pDlg->m_sProxyBypass;
		}
		DWORD dwAccessType = INTERNET_OPEN_TYPE_DIRECT;	// Connect directly to Internet
		if (pszProxyName || pszProxyBypass)
		{
			dwAccessType = INTERNET_OPEN_TYPE_PROXY;	// Connect through a CERN proxy
		}
		// PRE_CONFIG_INTERNET_ACCESS					// Connect using preconfigured settings in the registry
		m_pSession = (CInternetSession*) new CIpCamInetSession(NULL, _T("MY_SESSION"),
			1,											// context
			dwAccessType,								
			pszProxyName, pszProxyBypass,				// Proxy name and bypass
			INTERNET_FLAG_DONT_CACHE);					// flags
//		m_pSession->EnableStatusCallback();
	}
	return m_pSession;
}
/////////////////////////////////////////////////////////////////////////////
void CIpCameraUnitApp::ReportError(CInternetException*pEx, BOOL bMsgBox/*=FALSE*/)
{
	ASSERT(pEx!=NULL);
	TCHAR errorMessage[1024];
	pEx->GetErrorMessage(errorMessage, 1024);
	CString s;
	s.Format(_T("The session failed, error = %u \n%s\n"), pEx->m_dwError, errorMessage);
	if (bMsgBox)
	{
		AfxMessageBox(s, MB_ICONERROR|MB_OK);
	}
	else
	{
		WK_TRACE(s);
	}
	pEx->Delete();
}
/////////////////////////////////////////////////////////////////////////////
int CIpCameraUnitApp::ExitInstance()
{
	WK_TRACE(_T("ExitInstance\n"));
	CloseDebugger();
	return CWinApp::ExitInstance();
}


/*
/////////////////////////////////////////////////////////////////////////////
// CBSCallbackImpl implementation
CBSCallbackImpl::CBSCallbackImpl(HANDLE hThread)
{
	m_hThread = hThread;  // the window handle to display status

	m_ulObjRefCount = 1;
}
/////////////////////////////////////////////////////////////////////////////
// IUnknown
STDMETHODIMP CBSCallbackImpl::QueryInterface(REFIID riid, void **ppvObject)
{
	TRACE(_T("IUnknown::QueryInterface\n"));

	*ppvObject = NULL;
	
	// IUnknown
	if (::IsEqualIID(riid, __uuidof(IUnknown)))
	{
		TRACE(_T("IUnknown::QueryInterface(IUnknown)\n"));

		*ppvObject = this;
	}
	// IBindStatusCallback
	else if (::IsEqualIID(riid, __uuidof(IBindStatusCallback)))
	{
		TRACE(_T("IUnknown::QueryInterface(IBindStatusCallback)\n"));

		*ppvObject = static_cast<IBindStatusCallback *>(this);
	}

	if (*ppvObject)
	{
		(*reinterpret_cast<LPUNKNOWN *>(ppvObject))->AddRef();

		return S_OK;
	}
	
	return E_NOINTERFACE;
}                                             
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(ULONG) CBSCallbackImpl::AddRef()
{
	TRACE(_T("IUnknown::AddRef\n"));

	return ++m_ulObjRefCount;
}
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(ULONG) CBSCallbackImpl::Release()
{
	TRACE(_T("IUnknown::Release\n"));

	return --m_ulObjRefCount;
}
/////////////////////////////////////////////////////////////////////////////
// IBindStatusCallback
STDMETHODIMP CBSCallbackImpl::OnStartBinding(DWORD, IBinding *)
{
	TRACE(_T("IBindStatusCallback::OnStartBinding\n"));

	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CBSCallbackImpl::GetPriority(LONG *)
{
	TRACE(_T("IBindStatusCallback::GetPriority\n"));

	return E_NOTIMPL;
}
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CBSCallbackImpl::OnLowResource(DWORD)
{
	TRACE(_T("IBindStatusCallback::OnLowResource\n"));

	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CBSCallbackImpl::OnProgress(ULONG ulProgress,
										 ULONG ulProgressMax,
										 ULONG ulStatusCode,
										 LPCWSTR szStatusText)
{
	TRACE(_T("IBindStatusCallback::OnProgress\n"));

	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CBSCallbackImpl::OnStopBinding(HRESULT, LPCWSTR)
{
	TRACE(_T("IBindStatusCallback::OnStopBinding\n"));

	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CBSCallbackImpl::GetBindInfo(DWORD *pdwFlags, BINDINFO *pBI)
{
	TRACE(_T("IBindStatusCallback::GetBindInfo\n"));

	*pdwFlags = BINDF_ASYNCHRONOUS|BINDF_GETCLASSOBJECT|BINDF_PRAGMA_NO_CACHE|BINDF_PULLDATA|BINDF_NOWRITECACHE|BINDF_FROMURLMON;
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CBSCallbackImpl::OnDataAvailable(DWORD dw1, DWORD dw2,
											  FORMATETC *pF, STGMEDIUM *pS)
{
	TRACE(_T("IBindStatusCallback::OnDataAvailable\n"));

	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CBSCallbackImpl::OnObjectAvailable(REFIID, IUnknown *)
{
	TRACE(_T("IBindStatusCallback::OnObjectAvailable\n"));

	return S_OK;
}
*/