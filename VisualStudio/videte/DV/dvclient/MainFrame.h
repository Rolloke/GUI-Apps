#if !defined(AFX_MAINFRAME_H__6B5BBED9_9757_11D3_A870_004005A19028__INCLUDED_)
#define AFX_MAINFRAME_H__6B5BBED9_9757_11D3_A870_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MainFrame.h : header file
//

#include "DisplayWindow.h"
#include "PlayWindow.h"
#include "BackupWindow.h"
#include "DebugWindow.h"


/////////////////////////////////////////////////////////////////////////////
// CDisplayWindowThread
/////////////////////////////////////////////////////////////////////////////
class CPictureData
{
public:
	CPictureData(const CIPCPictureRecord &pict, DWORD dwX, DWORD dwY, CSecID idCam)
	{
		m_pPict = new CIPCPictureRecord(pict);
		m_dwX = dwX;
		m_dwY = dwY;
		m_idCam = idCam;
	}
	~CPictureData()
	{
		WK_DELETE(m_pPict);
	}
	CIPCPictureRecord*m_pPict;
	DWORD m_dwX, m_dwY;
	CSecID m_idCam;
};
typedef CPictureData* CPictureDataPtr;
WK_PTR_LIST_CS(CPictureDataList, CPictureDataPtr, CPictureDataListCS);

class CDisplayWindowThread : public CWK_Thread
{
public:
	CDisplayWindowThread(const CString sName, LPVOID lpContext=NULL);
	virtual ~CDisplayWindowThread();
	BOOL PostThreadMessage(UINT nMsg, WPARAM wParam, LPARAM lParam);
	void LivePictureData(const CIPCPictureRecord &pict, DWORD dwX, DWORD dwY, CSecID idCam);
	int  GetQueueLength();

protected:
	virtual	BOOL Run(LPVOID lpContext);
	LRESULT OnPictureData(WPARAM, LPARAM);

private:
	DWORD m_dwThreadID;
	CPictureDataListCS m_Pictures;
	CWK_Average<int> m_avgQueueLen;
};

/////////////////////////////////////////////////////////////////////////////
#define SHOWABLE		TRUE
#define NOT_SHOWABLE	FALSE

#define	TIMER_EVENT_ONE_SECOND	2

class CLiveWindow;
class CPlayWindow;
class CBackupWindow;
/////////////////////////////////////////////////////////////////////////////
// CMainFrame frame

class CMainFrame : public CFrameWnd
{
	friend class CDisplayWindowThread;
public:
	enum BackupStatus
	{
		//Backup with PacketCD
		BS_NOT_IN_BACKUP		= 0,
		BS_IN_BACKUP			= 1,
		BS_FORMAT_SUCCESS		= 2,
		BS_FORMAT_FAILED		= 3,
		BS_FINALIZE_SUCCESS		= 4,
		BS_FINALIZE_FAILED		= 5,
		BS_REMOUNT_SUCCESS		= 10,
		BS_REMOUNT_FAILED		= 11,
		BS_REQUEST_STATUS		= 100,

		//Backup with DirectCD
		BS_DCD_NOT_FOUND_EMPTY_CDR	= 200,
		BS_DCD_FOUND_NOT_EMPTY_CDR	= 201,
		BS_DCD_FORMATTING			= 202,
		BS_DCD_START_BACKUP			= 203,
		BS_DCD_COPYING_DATA			= 204,
		BS_DCD_BACKUP_FAILED		= 205,
		BS_DCD_DATA_COPIED			= 206,
		BS_DCD_FINALIZE_SUCCESS		= 207,
		BS_DCD_UNEXPECTED_ERROR		= 208,
		BS_DCD_ERROR				= 299,
	};

	DECLARE_DYNCREATE(CMainFrame)
public:
	CMainFrame();           // protected constructor used by dynamic creation

// Attributes
public:
	CLiveWindow*	GetLiveWindow(WORD wCameraNr);
	CPlayWindow*	GetPlayWindow(WORD wCameraNr);
	CPlayWindow*	GetNextPlayWindow(CPlayWindow* pPlayWndExcept);
	CBackupWindow*	GetBackupWindow();
	CDebugWindow*	GetDebugWindow();
	int				GetLiveWindowQueueLength();
	int				GetNrOfWindows(int nType=0, BOOL bShowable=-1);

	inline ViewMode	GetViewMode() const;
	inline CDisplayWindow* GetBigOne();
	BOOL	GetLogoPosition(CRect&);
	CString GetShortOemName();

// Operations
public:
	void ClearOEMName();
	void DoRequest();
	void DoAutoViewMode();
	void DoSetForeGround();
	void SetView(WORD wCameraNr, ViewMode vm);
	void SetMustAutoViewMode();
	void OnPINEntered(WORD wPIN);
	void SetDisplayActive(CDisplayWindow* pDW);
	void OnIndicateAlarm(BOOL bAlarm);

	void DisconnectInput();
	void DisconnectOutput();
	void DisconnectDatabase();

	BOOL IsInBackup();
	BOOL IsDeleting();
	void AddBackupWindow(CServer* pServer);
	void DeleteBackupWindow();
	CDisplayWindows* GetDisplayWindows(){return &m_Displays;}
	inline void	LockDisplayWindows(LPCTSTR sFkt);
	inline void	UnlockDisplayWindows();
	void	LivePictureData(const CIPCPictureRecord &pict, DWORD dwX, DWORD dwY, CSecID idCam);
	void	SetPanetext(int nPane, LPCTSTR sText);
	void	Request();
	void	RequestLive(int nMode=0);
	inline void	SetUnitFps(int nFps);

	//maintenance text to show in debug window
	void SetMaintenanceText(CString sValue);
	CString GetMaintenanceText();
	void	UpdateLogoFont();

//TODO TKR für Sync Abspielen
/*
	//functions for synchron play
	CDisplayWindow* FindOldestDisplayWindow(CSystemTime& st, BOOL bForward, WORD& wPlayingCam);
	CPlayWindow*	GetOldestPlayWindow();
*/
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CMainFrame();

	// Generated message map functions
	//{{AFX_MSG(CMainFrame)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg long DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnAskCbFormatName(UINT nMaxCount, LPTSTR lpszString);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	afx_msg BOOL OnDeviceChange( UINT nEventType, DWORD dwData );
	afx_msg LRESULT OnUser(WPARAM wParam, LPARAM lParam);
	afx_msg long OnBackupCheckStatus(WPARAM wParam, LPARAM lParam);
	afx_msg long OnPowerOffButtonPressed(WPARAM wParam, LPARAM lParam);
	afx_msg long OnResetButtonPressed(WPARAM wParam, LPARAM lParam);
	afx_msg void OnUpdateDummy(CCmdUI* pCmdUI);
	afx_msg BOOL OnQueryEndSession( );
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()

	// Implementation
protected:
	CString GetOemName();
	CDisplayWindow*	GetDisplayWindow(CSecID id);

	void SetWindowSize();
	void SetWindowSizeXxX();
	void SetWindowSize1pX();
	void SetWindowSizeFull();
	void SortDisplayWindows();

	void AddLiveWindow(CServer* pServer, const CIPCOutputRecord& rec);
	void AddPlayWindow(CServer* pServer, const CIPCArchivRecord& rec);
	void AddDisplayWindow(CDisplayWindow* pDW);

	void OnAddInput(CSecID id);
	void OnUpdateInput(CSecID id);
	void OnAddCamera(CSecID id);
	void OnAddRelais(CSecID id);
	void OnAddArchive(CSecID id);
	void OnAddArchives();
	void OnAddBackup();
	void OnDeleteBackup();

	void OnUpdateCamera(CSecID id);
	void OnDeleteDisplay(CSecID id);
	void OnDisconnect();
	void OnAlarmConnection();

	void DrawNoCameras(CRect rect, CDC* pDC);
	void DrawBetaWarning(CRect rect, CDC* pDC);

	void CheckActivePanelCamera();
	void AutoViewMode();
	void IndicateAlarmToUser(BOOL bAlarm);

	// data member
private:
	CJpeg					m_JpegLogo;
	CRect					m_rcLogo;
	CString					m_sOemName;
	CString					m_sShortOemName;
	BOOL					m_bLogoValid;
	CDisplayWindows			m_Displays;
	CDisplayWindow*			m_pBigOne;
	int						m_iXWin;
	int						m_nUnitFps;
	ViewMode				m_eViewMode;
	WORD					m_wCamera;
	BOOL					m_bAutoViewMode;
	BOOL					m_bDestroying;
	COLORREF				m_colBackGround;
	BOOL					m_bBackupStatus;
	BOOL					m_bMustAutoViewMode;
	UINT					m_uFlashTimer;
	CEvent					m_evBackupWindow;
	CCriticalSection		m_csDestroyWnd;
	BOOL					m_bDrawBetaWarning;
	BOOL					m_bWindowsCreationComplete;
	UINT					m_uDevKnodeChangedTimer;
	CDisplayWindowThread*	m_pLiveWindowThread;
	CStatusBar				m_wndStatusBar;

	//maintenance text to show in debug window
	CString					m_sMaintenanceText;

//TODO TKR für Sync Abspielen
/*
	CPlayWindow*			m_pSyncOldestPlayWnd; //beim synchronen Abspielen das jeweils älteste PlayWnd
*/
};
/////////////////////////////////////////////////////////////////////////////
inline ViewMode	CMainFrame::GetViewMode() const
{
	return m_eViewMode;
}
/////////////////////////////////////////////////////////////////////////////
inline CDisplayWindow* CMainFrame::GetBigOne()
{
	return m_pBigOne;
}
/////////////////////////////////////////////////////////////////////////////
inline void	CMainFrame::LockDisplayWindows(LPCTSTR sFkt)
{
	m_Displays.Lock();
}
/////////////////////////////////////////////////////////////////////////////
inline void	CMainFrame::UnlockDisplayWindows()
{
	m_Displays.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
inline void	CMainFrame::SetUnitFps(int nFps)
{
	m_nUnitFps = nFps;
}
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRAME_H__6B5BBED9_9757_11D3_A870_004005A19028__INCLUDED_)
