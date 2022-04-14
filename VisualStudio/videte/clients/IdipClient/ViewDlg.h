#pragma once

// CDlgBarIdipClient dialog

class CViewDialogBar;

class CDlgInView : public CSkinDialog
{
	DECLARE_DYNAMIC(CDlgInView)
public:
	CDlgInView(UINT nID, CWnd* pParent = NULL);
	virtual ~CDlgInView();

public:
	// Attribute
//	int GetWidth()  {return m_szMin.cx;};
//	int GetHeight() {return m_szMin.cy;};
	int GetWidth()  {return m_InitialClientRect.Width();};
	int GetHeight() {return m_InitialClientRect.Height()+1;};
	eVisibilityStates	GetVisibilityState() { return m_eVisibility;};

	// Implementation
	void SetVisibility(eVisibilityStates eVisibility);

protected:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgInView)
	virtual BOOL StretchElements() { return FALSE;};
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CDlgInView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CViewDialogBar *m_pView;
	int				m_nHeight;
	CSize			m_szMin;
public:
	eVisibilityStates m_eVisibility;
};
