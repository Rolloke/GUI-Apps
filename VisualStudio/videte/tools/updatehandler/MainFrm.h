// MainFrm.h : interface of the CMainFrame class
//

#include "CoolBar.h"

/////////////////
// My Cool bar: specialized CCoolBar creates bands.
//
class CMyCoolBar : public CCoolBar 
{
protected:
	DECLARE_DYNAMIC(CMyCoolBar)
	virtual BOOL   OnCreateBands();

private:
	CCoolToolBar	m_wndToolBar;			 // toolbar
	CBitmap			m_bmBackground;		 // background bitmap
};

/////////////////////////////////////////////////////////////////////////////
class CDirectoryView;
class CFileView;
class CReportView;

class CMainFrame : public CFrameWnd
{
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	inline CDirectoryView* GetDirectoryView();
	inline CFileView* GetFileView();
	inline CReportView* GetReportView();
	inline CDialogBar* GetDialogBar();

// Operations
public:
	void ActualizeTitleBarPaneSizes(CWnd*pWnd=NULL);	
	void AddReportLine(const CString& sLine);
	void AddDrives(const CString& sDrives);
	void CountDown(int nSek, UINT nCommand = 0);
	void SetFileViewDirectory(const CString& sDir);
	void RemoveFileFromFileView(const CString& sFile);
	void ClearDirFileView();
	void SetIdleTimer(BOOL bSet);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar		m_wndStatusBar;
	CMyCoolBar		m_wndToolBar;
	CSplitterWnd	m_wndSplitter;
	CDialogBar		m_wndTitleBar;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnOptionsClearReport();
	afx_msg void OnClose();
	afx_msg long DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CDirectoryView* m_pDirectoryView;
	CFileView* m_pFileView;
	CReportView* m_pReportView;

	CProgressCtrl m_wndProgress;
	int			m_nCountUp;
	int			m_nLimit;
	UINT		m_nCommand;
	UINT		m_nTimerCountUpTimer;
	UINT		m_nIdleTimer;
};
inline CDirectoryView* CMainFrame::GetDirectoryView()
{
	return m_pDirectoryView;
}
inline CFileView* CMainFrame::GetFileView()
{
	return m_pFileView;
}
inline CReportView* CMainFrame::GetReportView()
{
	return m_pReportView;
}
inline CDialogBar* CMainFrame::GetDialogBar()
{
	return &m_wndTitleBar;
}

/////////////////////////////////////////////////////////////////////////////
