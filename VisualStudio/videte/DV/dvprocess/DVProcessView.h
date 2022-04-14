// DVProcessView.h : interface of the CDVProcessView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DVPROCESSVIEW_H__43C9E497_8B84_11D3_99EB_004005A19028__INCLUDED_)
#define AFX_DVPROCESSVIEW_H__43C9E497_8B84_11D3_99EB_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CDVProcessView : public CView
{
protected: // create from serialization only
	CDVProcessView();
	DECLARE_DYNCREATE(CDVProcessView)

// Attributes
public:
	CDVProcessDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDVProcessView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDVProcessView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CDVProcessView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CSystemTime m_stStart;
};

#ifndef _DEBUG  // debug version in DVProcessView.cpp
inline CDVProcessDoc* CDVProcessView::GetDocument()
   { return (CDVProcessDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DVPROCESSVIEW_H__43C9E497_8B84_11D3_99EB_004005A19028__INCLUDED_)
