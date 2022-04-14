
#ifndef _CHiddenCapiWnd_H_
#define _CHiddenCapiWnd_H

class CCapiThread;

class CHiddenCapiWnd : public CWnd
{
// Construction
public:
	CHiddenCapiWnd(CCapiThread *pThread);

// Attributes
private:
	CCapiThread *m_pThread;
// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHiddenCapiWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual void PostNcDestroy();
	virtual ~CHiddenCapiWnd();
	// Generated message map functions
protected:
	//{{AFX_MSG(CHiddenCapiWnd)
	afx_msg long OnCapiMessage(WPARAM, LPARAM);	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif // _CHiddenCapiWnd_H_