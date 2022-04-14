// MPEG4PlayView.h : interface of the CMPEG4PlayView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MPEG4PLAYVIEW_H__AFF94141_2D8F_4EB2_B55D_1D60BB06FB77__INCLUDED_)
#define AFX_MPEG4PLAYVIEW_H__AFF94141_2D8F_4EB2_B55D_1D60BB06FB77__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define ID_DECODE_VIDEO 0x7000

class CMPEG4PlayView : public CView
{
protected: // create from serialization only
	CMPEG4PlayView();
	DECLARE_DYNCREATE(CMPEG4PlayView)

// Attributes
public:
	CMPEG4PlayDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMPEG4PlayView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMPEG4PlayView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMPEG4PlayView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	afx_msg void OnDecode();
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in MPEG4PlayView.cpp
inline CMPEG4PlayDoc* CMPEG4PlayView::GetDocument()
   { return (CMPEG4PlayDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MPEG4PLAYVIEW_H__AFF94141_2D8F_4EB2_B55D_1D60BB06FB77__INCLUDED_)
