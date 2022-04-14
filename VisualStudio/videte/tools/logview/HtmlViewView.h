// HtmlViewView.h : interface of the CHtmlViewView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_HTMLVIEWVIEW_H__11FDFA71_0122_11D3_8D1C_004005A11E32__INCLUDED_)
#define AFX_HTMLVIEWVIEW_H__11FDFA71_0122_11D3_8D1C_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CHtmlViewView : public CHtmlView
{
protected: // create from serialization only
	CHtmlViewView();
	DECLARE_DYNCREATE(CHtmlViewView)

// Attributes
public:
	CHtmlViewDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHtmlViewView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnTitleChange(LPCTSTR lpszText);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CHtmlViewView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CHtmlViewView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in HtmlViewView.cpp
inline CHtmlViewDoc* CHtmlViewView::GetDocument()
   { return (CHtmlViewDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HTMLVIEWVIEW_H__11FDFA71_0122_11D3_8D1C_004005A11E32__INCLUDED_)
