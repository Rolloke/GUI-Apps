// DBSView.h : interface of the CDBSView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DBSVIEW_H__AB5B236D_3834_11D2_B58E_00C095EC9EFA__INCLUDED_)
#define AFX_DBSVIEW_H__AB5B236D_3834_11D2_B58E_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CDBSView : public CListView
{
protected: // create from serialization only
	CDBSView();
	DECLARE_DYNCREATE(CDBSView)

// Attributes
public:
	CDBSDoc* GetDocument();

// Operations
public:
	void Clear();
	void Add(const CString& s);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDBSView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDBSView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CDBSView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in DBSView.cpp
inline CDBSDoc* CDBSView::GetDocument()
   { return (CDBSDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DBSVIEW_H__AB5B236D_3834_11D2_B58E_00C095EC9EFA__INCLUDED_)
