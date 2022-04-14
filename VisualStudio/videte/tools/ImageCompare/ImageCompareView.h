/////////////////////////////////////////////////////////////////////////////
// PROJECT:		ImageCompare
// FILE:		$Workfile: ImageCompareView.h $
// ARCHIVE:		$Archive: /Project/Tools/ImageCompare/ImageCompareView.h $
// CHECKIN:		$Date: 22.04.99 16:14 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 22.04.99 16:14 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMAGECOMPAREVIEW_H__C221097D_F324_11D2_8D05_004005A11E32__INCLUDED_)
#define AFX_IMAGECOMPAREVIEW_H__C221097D_F324_11D2_8D05_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CImageCompareView : public CView
{
protected: // create from serialization only
	CImageCompareView();
	DECLARE_DYNCREATE(CImageCompareView)

// Attributes
public:
	CImageCompareDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImageCompareView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CImageCompareView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CImageCompareView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in ImageCompareView.cpp
inline CImageCompareDoc* CImageCompareView::GetDocument()
   { return (CImageCompareDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMAGECOMPAREVIEW_H__C221097D_F324_11D2_8D05_004005A11E32__INCLUDED_)
