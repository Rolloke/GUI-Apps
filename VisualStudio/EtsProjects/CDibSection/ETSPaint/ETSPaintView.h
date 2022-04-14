// ETSPaintView.h : Schnittstelle der Klasse CETSPaintView
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ETSPAINTVIEW_H__A560281C_9494_11D4_B6EC_0000B458D891__INCLUDED_)
#define AFX_ETSPAINTVIEW_H__A560281C_9494_11D4_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CETSPaintView : public CScrollView
{
protected: // Nur aus Serialisierung erzeugen
	CETSPaintView();
	DECLARE_DYNCREATE(CETSPaintView)

// Attribute
public:
	CETSPaintDoc* GetDocument();
   int   m_nViewFactor;
	CSize m_sizeTotal;
   DWORD m_dwDrawFlags;
   COLORREF m_cLeftColor;
   COLORREF m_cRightColor;
   CPoint   m_StartPoint;
// Operationen
public:

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CETSPaintView)
	public:
	virtual void OnDraw(CDC* pDC);  // überladen zum Zeichnen dieser Ansicht
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // das erste mal nach der Konstruktion aufgerufen
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementierung
public:
	CPoint PixelCoord(CPoint *, const BITMAPINFO *);
	void InvalidatePart(CPoint);
	virtual ~CETSPaintView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generierte Message-Map-Funktionen
protected:
	//{{AFX_MSG(CETSPaintView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnViewNormal();
	afx_msg void OnUpdateViewNormal(CCmdUI* pCmdUI);
	afx_msg void OnView400Percent();
	afx_msg void OnView200Percent();
	afx_msg void OnViewStretched();
	afx_msg void OnViewZoom();
	afx_msg void OnUpdateViewZoom(CCmdUI* pCmdUI);
	afx_msg void OnUpdateView200Percent(CCmdUI* pCmdUI);
	afx_msg void OnUpdateView400Percent(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewStretched(CCmdUI* pCmdUI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnViewBw();
	afx_msg void OnUpdateViewBw(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // Testversion in ETSPaintView.cpp
inline CETSPaintDoc* CETSPaintView::GetDocument()
   { return (CETSPaintDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_ETSPAINTVIEW_H__A560281C_9494_11D4_B6EC_0000B458D891__INCLUDED_)
