// CommWindow.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCommWindow
#include "DebugTraceDlg.h"
class CCommWindow : public CWnd
{
public:
	CCommWindow();

// Attributes
public:

// Operations
public:
	void SetToolTip(const CString& sComs);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCommWindow)
	protected:
	virtual void PostNcDestroy();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CCommWindow();

	// Generated message map functions
	//{{AFX_MSG(CCommWindow)
	afx_msg void OnDestroy();
	afx_msg void OnAppExit();
	afx_msg void OnAbout();
	afx_msg long DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLanguageChanged(WPARAM wParam, LPARAM lParam);
	afx_msg void OnViewDebuginfo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CDebugTraceDlg m_Dlg;
};

/////////////////////////////////////////////////////////////////////////////
