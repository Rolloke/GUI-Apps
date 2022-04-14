// IPCameraUnit.h : Hauptheaderdatei für die IPCameraUnit-Anwendung
//

#pragma once

#ifndef __AFXWIN_H__
	#error 'stdafx.h' muss vor dieser Datei in PCH eingeschlossen werden.
#endif

#include "resource.h"		// Hauptsymbole

#define IPCAM_UNIT_DEVICE	_T("IPCameraUnit")

#define SETTINGS_SECTION	_T("Settings")
#define P_HTTP_VERB_SET		_T("HTTP_VERB_SET")
#define P_URL				_T("URL")
#define P_REQUEST			_T("Request")
#define P_PORT				_T("Port")
#define P_FPS				_T("Fps")
#define P_USERNAME			_T("UserName")
#define P_PASSWORD			_T("Pwd")
#define P_PROXY_NAME		_T("ProxyName")
#define P_PROXY_BYPASS		_T("ProxyBypass")
#define P_SEC_ID			_T("SecID")
//////////////////////////////////////////////////////////////////////////////////////
// CIpCameraUnitApp:
class CIpCameraUnitDlg;

CString GetTickCountEx(BOOL bStart = FALSE);

class CIpCameraUnitApp : public CWinApp
{
public:
	CIpCameraUnitApp();
	virtual~CIpCameraUnitApp();

// Überschreibungen
	//{{AFX_VIRTUAL(CIpCameraUnitApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

public:
	inline CIpCameraUnitDlg*GetDlg() const;
	inline WK_ApplicationId	GetApplicationId() const;
	inline int				GetInstanceCount() const;
	inline BOOL				IsInMainThread() const;

// Implementierung
	CInternetSession* GetInternetSession();
	CString GetAppnameFromId(WK_ApplicationId wai, int nInstance = -1);
	void	ReportError(CInternetException*pExc, BOOL bMsgBox=FALSE);

	//{{AFX_MSG(CIpCameraUnitApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


	//{{AFX_DATA(CIpCameraUnitApp)
public:
	CKeyProvider m_prov;
private:
	CIpCameraUnitDlg*	m_pDlg;
	WK_ApplicationId	m_AppID;
	int					m_nInstance;
	CInternetSession*	m_pSession;
	//}}AFX_DATA
public:
};

//////////////////////////////////////////////////////////////////////////////////////
extern CIpCameraUnitApp theApp;
extern CWK_Timer theTimer;
//////////////////////////////////////////////////////////////////////////////////////
inline CIpCameraUnitDlg* CIpCameraUnitApp::GetDlg() const
{
	ASSERT(m_pDlg != NULL);
	return m_pDlg;
}
//////////////////////////////////////////////////////////////////////////////////////
inline WK_ApplicationId	CIpCameraUnitApp::GetApplicationId() const
{
	return m_AppID;
}
//////////////////////////////////////////////////////////////////////////////////////
inline int CIpCameraUnitApp::GetInstanceCount() const
{
	return m_nInstance;
}
//////////////////////////////////////////////////////////////////////////////////////
inline BOOL	CIpCameraUnitApp::IsInMainThread() const
{
	return GetCurrentThreadId() == m_nThreadID;
}
//////////////////////////////////////////////////////////////////////////////////////


/*
/////////////////////////////////////////////////////////////////////////////
// CBSCallbackImpl

class CBSCallbackImpl : public IBindStatusCallback
{
public:
	CBSCallbackImpl(HANDLE hThread);

	// IUnknown methods
	STDMETHOD(QueryInterface)(REFIID riid, void **ppvObject);
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();

	// IBindStatusCallback methods
	STDMETHOD(OnStartBinding)(DWORD, IBinding *);
	STDMETHOD(GetPriority)(LONG *);
	STDMETHOD(OnLowResource)(DWORD);
	STDMETHOD(OnProgress)(ULONG ulProgress,
						  ULONG ulProgressMax,
						  ULONG ulStatusCode,
						  LPCWSTR szStatusText);
	STDMETHOD(OnStopBinding)(HRESULT, LPCWSTR);
	STDMETHOD(GetBindInfo)(DWORD *, BINDINFO *);
	STDMETHOD(OnDataAvailable)(DWORD, DWORD, FORMATETC *, STGMEDIUM *);
	STDMETHOD(OnObjectAvailable)(REFIID, IUnknown *);

protected:
	ULONG m_ulObjRefCount;

private:
	HANDLE m_hThread;
};
*/
	//{{AFX_DATA()
	//}}AFX_DATA
	//{{AFX_VIRTUAL()
	//}}AFX_VIRTUAL
	//{{AFX_MSG()
	//}}AFX_MSG
