#if !defined(AFX_JPEGCHILD_H__5F6F9B84_EEF4_11D1_B51E_00C095EC9EFA__INCLUDED_)
#define AFX_JPEGCHILD_H__5F6F9B84_EEF4_11D1_B51E_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// JpegChild.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CJpegChild frame

class CJpegChild : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CJpegChild)
protected:
	CJpegChild();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJpegChild)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CJpegChild();

	// Generated message map functions
	//{{AFX_MSG(CJpegChild)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JPEGCHILD_H__5F6F9B84_EEF4_11D1_B51E_00C095EC9EFA__INCLUDED_)
