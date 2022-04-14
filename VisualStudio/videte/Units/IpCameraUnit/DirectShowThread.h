#pragma once

// behaviour flags
#define DSTF_RECEIVE_NOTIFICATIONS	0x00000001
#define DSTF_INIT_MEDIA_CONTROL		0x00000002
#define DSTF_PROVIDE_KEY			0x00000004
#define DSTF_PREVIEW				0x00000008
#define DSTF_INIT_FILTERS_AT_START	0x00000010
#define DSTF_NO_GRAPH_INIT			0x00000020
#define DSTF_START_AFTER_INIT	   (0x00000040|DSTF_INIT_MEDIA_CONTROL)
// state flags
#define DSTF_PARAM_DIRTY			0x00010000
#define DSTF_NO_REINIT_IF_DIRTY		0x00020000
#define DSTF_FILTER_INITIALISED		0x00040000
// user flag range start
#define DSTF_USER_MIN				0x01000000

#define DST_NOTIFICATION			0x00000815
#define	EC_MEDIA_STATE_CHANGED		(EC_USER + 1)
#define	EC_SET_MEDIA_STATE			(EC_USER + 2)


#define STOP_WHEN_READY	1

#define WM_SET_MEDIA_STATE		(WM_USER + 2)
#define WM_REQUEST_GET_VALUE	(WM_USER + 3)
#define WM_REQUEST_SET_VALUE	(WM_USER + 4)
#define WM_SET_FILTERS			(WM_USER + 5)
#define    RELEASED		FALSE
#define    INITIALISED	TRUE
#define    REGISTERED	2
#define    UNREGISTERED	3
#define    PREVIEWED	4
#define    UNPREVIEWED	5

LPCTSTR NameOfEnum(OAFilterState fs);
/////////////////////////////////////////////////////////////////////////////
// CDirectShowThread

class CDirectShowThread : public CWK_WinThread
{
	DECLARE_DYNAMIC(CDirectShowThread)

protected:
	CDirectShowThread(const CString&sName, DWORD dwFlags);           // protected constructor used by dynamic creation
public:
	virtual ~CDirectShowThread();

	// Attribute
public:
	inline OAFilterState	GetMediaState();
	inline BOOL				IsDirty();
	inline BOOL				IsInitialised();
	inline BOOL				IsRegistered();
	inline BOOL				IsPreviewed();
	inline int				GetOutStandingConfirms();

	// Implementation
public:
	BOOL SetMediaState(OAFilterState oafs, LPARAM lParam=0);
	BOOL SetFilters(int nSet, LPARAM lParam=0);
	void SetDirty(BOOL bSet);
	void DoSelfCheck();

protected:
	void ReportError(ErrorStruct*pe, BOOL bMsgBox=TRUE);
	BOOL PostToMainWnd(UINT uMsg, WPARAM wParam, LPARAM lParam);
	BOOL LoadGraph(CString sGraph);
	void RemoveAndReleaseFilter(IBaseFilter*&pFilter);

	//{{AFX_VIRTUAL(CDirectShowThread)
public:
	virtual BOOL StopThread();
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual HRESULT InitFilters();
	virtual void ReleaseFilters();
	virtual HRESULT OnEvent(LONG lCode, LONG lParam1, LONG lParam2);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CDirectShowThread)
	afx_msg void OnSetMediaState(WPARAM wParam, LPARAM lParam);
	afx_msg void OnGraphNotify(WPARAM, LPARAM);
	afx_msg void OnSetFilters(WPARAM wParam, LPARAM);
	afx_msg void OnConfirmSelfcheck(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	//{{AFX_DATA(CDirectShowThread)
protected:
	volatile DWORD			m_dwFlags;
	volatile int			m_nOutstandingConfirms;
	DWORD					m_dwRegisterROT;
	IGraphBuilder*			m_pGraph;
	ICaptureGraphBuilder2*	m_pGraphBuilder;
	IMediaControl*			m_pMediaControl;
	IMediaEventEx*			m_pMediaEvent;
	OAFilterState			m_sMediaState;
	IBaseFilter*			m_pRenderer;
	//}}AFX_DATA
public:
};

/////////////////////////////////////////////////////////////////////////////
inline OAFilterState CDirectShowThread::GetMediaState()
{
	return m_sMediaState;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CDirectShowThread::IsDirty()
{
	return m_dwFlags & DSTF_PARAM_DIRTY ? TRUE : FALSE;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CDirectShowThread::IsInitialised()
{
	return m_dwFlags & DSTF_FILTER_INITIALISED ? TRUE : FALSE;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CDirectShowThread::IsRegistered()
{
	return m_dwRegisterROT != 0 ? TRUE : FALSE;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CDirectShowThread::IsPreviewed()
{
	return m_dwFlags & DSTF_PREVIEW ? TRUE : FALSE;
}
/////////////////////////////////////////////////////////////////////////////
inline int	CDirectShowThread::GetOutStandingConfirms()
{
	return m_nOutstandingConfirms;
}
