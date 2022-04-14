#if !defined(AFX_JPEGVIEW_H__23C294C6_EE27_11D1_B51E_00C095EC9EFA__INCLUDED_)
#define AFX_JPEGVIEW_H__23C294C6_EE27_11D1_B51E_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// JpegView.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CJpegView view

class CJpegView : public CView
{
protected:
	CJpegView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CJpegView)

// Attributes
public:
	CJpegDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJpegView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CJpegView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CJpegView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	long			m_BufferID;
	long			m_ViewID;
};
#ifndef _DEBUG  // debug version in H263PlayView.cpp
inline CJpegDoc* CJpegView::GetDocument()
   { return (CJpegDoc*)m_pDocument; }
#endif
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JPEGVIEW_H__23C294C6_EE27_11D1_B51E_00C095EC9EFA__INCLUDED_)
