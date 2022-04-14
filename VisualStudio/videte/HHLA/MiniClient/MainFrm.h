// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__AEE94399_8A89_11D2_8CA3_004005A11E32__INCLUDED_)
#define AFX_MAINFRM_H__AEE94399_8A89_11D2_8CA3_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CJpeg;					    
class CUser;
class CPermission;
class CIPCOutputMiniClient;
class CMainFrame : public CFrameWnd
{
	
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
//	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
	HWND GetRemoteControlWnd(){return m_hWndRemoteControl;}

protected:
	BOOL FetchOutputConnection();
	BOOL Login();

	void ZoomDisplayWindow(const CPoint &point, BOOL bIn);
	BOOL ScrollDisplayWindow();

protected:  // control bar embedded members
//	CStatusBar				m_wndStatusBar;

private:
	CString					m_sSection;
	CIPCOutputMiniClient	*m_pOutput;
	CIPCPictureRecord		*m_pPictRecord;
	CUser*					m_pUser;
	CPermission*			m_pPermission;
	BOOL					m_bActive;
	HWND					m_hWndRemoteControl;
	WORD					m_wClientNr;
	CString					m_sCamName;
	int						m_nHRes;
	int						m_nVRes;
	int						m_nRefreshType;
	CJpeg*					m_pJpeg;	 
	CRect					m_rcZoom;

	// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg long OnShowImage(WPARAM wParam, LPARAM lParam);
	afx_msg long OnRequestSetInputSource(WPARAM wParam, LPARAM lParam);
	afx_msg long OnRequestSetWindowPosition(WPARAM wParam, LPARAM lParam);
	afx_msg long OnRequestSavePicture(WPARAM wParam, LPARAM lParam);
	afx_msg long OnRequestSetClientNumber(WPARAM wParam, LPARAM lParam);
	afx_msg long OnRequestSetCameraParameter(WPARAM wParam, LPARAM lParam);
	afx_msg long OnRequestCameraParameter(WPARAM wParam, LPARAM lParam);
	afx_msg long OnRequestCheckCamera(WPARAM wParam, LPARAM lParam);
	
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__AEE94399_8A89_11D2_8CA3_004005A11E32__INCLUDED_)
