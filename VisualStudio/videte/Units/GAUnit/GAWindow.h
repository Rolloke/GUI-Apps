// GAWindow.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGAWindow

class CGAWindow : public CWnd
{
public:
	CGAWindow();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGAWindow)
	protected:
	virtual void PostNcDestroy();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CGAWindow();

	// Generated message map functions
	//{{AFX_MSG(CGAWindow)
	afx_msg void OnDestroy();
	afx_msg void OnAppExit();
	afx_msg void OnAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
