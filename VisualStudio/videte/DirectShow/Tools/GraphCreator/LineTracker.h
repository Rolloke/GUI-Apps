#if !defined(AFX_LINETRACKER_H__AEFBD93D_8129_4001_9D50_30B4574C9C14__INCLUDED_)
#define AFX_LINETRACKER_H__AEFBD93D_8129_4001_9D50_30B4574C9C14__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LineTracker.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLineTracker window

class CLineTracker : public CRectTracker
{
// Construction
public:
	CLineTracker();
	CLineTracker(LPCRECT lpSrcRect, UINT nStyle);

// Attributes
public:
	CPoint	GetEndPoint();

// Operations
public:
	virtual void DrawTrackerRect(LPCRECT lpRect, CWnd* pWndClipTo,
		CDC* pDC, CWnd* pWnd);
	virtual void AdjustRect(int nHandle, LPRECT lpRect);
// Implementation
public:
	virtual ~CLineTracker();


	// Generated message map functions
protected:
	CPoint m_ptEnd;
	CRect m_rcOld;
	bool  m_bDrawn;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LINETRACKER_H__AEFBD93D_8129_4001_9D50_30B4574C9C14__INCLUDED_)
