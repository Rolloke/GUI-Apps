#if !defined(AFX_H263CHILD_H__5F6F9B83_EEF4_11D1_B51E_00C095EC9EFA__INCLUDED_)
#define AFX_H263CHILD_H__5F6F9B83_EEF4_11D1_B51E_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// H263Child.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CH263Child frame

class CH263Child : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CH263Child)
protected:
	CH263Child();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CH263Child)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CH263Child();

	// Generated message map functions
	//{{AFX_MSG(CH263Child)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_H263CHILD_H__5F6F9B83_EEF4_11D1_B51E_00C095EC9EFA__INCLUDED_)
