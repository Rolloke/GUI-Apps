// DVStorageView.h : interface of the CDVStorageView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DVSTORAGEVIEW_H__F4B60C13_8ACE_11D3_99EA_004005A19028__INCLUDED_)
#define AFX_DVSTORAGEVIEW_H__F4B60C13_8ACE_11D3_99EA_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CDVStorageView : public CView
{
protected: // create from serialization only
	CDVStorageView();
	DECLARE_DYNCREATE(CDVStorageView)

// Attributes
public:
	CDVStorageDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDVStorageView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDVStorageView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CDVStorageView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CSystemTime m_stStart;
};

#ifndef _DEBUG  // debug version in DVStorageView.cpp
inline CDVStorageDoc* CDVStorageView::GetDocument()
   { return (CDVStorageDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DVSTORAGEVIEW_H__F4B60C13_8ACE_11D3_99EA_004005A19028__INCLUDED_)
