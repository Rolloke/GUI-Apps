// DlgBaridipClient.h: interface for the CDlgBarIdipClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CBUTTONUPDOWN_H__)
#define AFX_CBUTTONUPDOWN_H__

/////////////////////////////////////////////////////////////////////////////
// CButtonUpDown window
#define BN_UP          8
#define BN_DOWN        9

#define ON_BN_UP(id, memberFxn) \
	ON_CONTROL(BN_UP, id, memberFxn)
#define ON_BN_DOWN(id, memberFxn) \
	ON_CONTROL(BN_DOWN, id, memberFxn)

class CButtonUpDown : public CSkinButton
{
// Construction
public:
	CButtonUpDown();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CButtonUpDown)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CButtonUpDown();

	// Generated message map functions
protected:
	//{{AFX_MSG(CButtonUpDown)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif // AFX_CBUTTONUPDOWN_H__