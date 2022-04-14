// H263PlayView.h : interface of the CH263PlayView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_H263PLAYVIEW_H__265171F0_5DC4_11D1_905F_444553540000__INCLUDED_)
#define AFX_H263PLAYVIEW_H__265171F0_5DC4_11D1_905F_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define ID_DECODE_VIDEO 0x7000

class CH263PlayView : public CView
{
protected: // create from serialization only
	CH263PlayView();
	DECLARE_DYNCREATE(CH263PlayView)

// Attributes
public:
	CH263PlayDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CH263PlayView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CH263PlayView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CH263Decoder*	m_pDecoder;

// Generated message map functions
protected:
	//{{AFX_MSG(CH263PlayView)
	afx_msg void OnPlay();
	afx_msg void OnDestroy();
	afx_msg void OnDrawframe();
	afx_msg void OnFileSaveAs();
	afx_msg void OnFileSave();
	afx_msg void OnH263Fit();
	afx_msg void OnUpdateH263Fit(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg void OnDecode();
	DECLARE_MESSAGE_MAP()

private:
	CH263	m_H263;
};

#ifndef _DEBUG  // debug version in H263PlayView.cpp
inline CH263PlayDoc* CH263PlayView::GetDocument()
   { return (CH263PlayDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_H263PLAYVIEW_H__265171F0_5DC4_11D1_905F_444553540000__INCLUDED_)
