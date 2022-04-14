
#ifndef _CHiddenCapiWnd_H_
#define _CHiddenCapiWnd_H

class CCapiSMS;

class CHiddenCapiWnd : public CWnd
{
// Construction
public:
	CHiddenCapiWnd(CCapiSMS *pConnection);

// Attributes
private:
	CCapiSMS* m_pConnection;
// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHiddenCapiWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CHiddenCapiWnd();
	// Generated message map functions
protected:
	//{{AFX_MSG(CHiddenCapiWnd)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	afx_msg long OnCapiMessage(WPARAM, LPARAM);	
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif // _CHiddenCapiWnd_H_