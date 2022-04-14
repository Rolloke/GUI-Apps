// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////
class CLauncherToolBar : public CToolBar
{
public:
	CLauncherToolBar();


// Generated message map functions
protected:
	//{{AFX_MSG(CLauncherToolBar)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
#ifdef AfxOldTOOLINFO
	afx_msg int	 OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
#endif
	DECLARE_MESSAGE_MAP()

};

class CMainFrame : public CFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:

	// Operations
public:
	void InitialShowToolBar();
	void SetPosition(int n);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CLauncherToolBar   m_wndToolBar;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSkip();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CPoint  m_Point;
	int		m_Pos;

	friend class CLauncherToolBar;
};
/////////////////////////////////////////////////////////////////////////////
