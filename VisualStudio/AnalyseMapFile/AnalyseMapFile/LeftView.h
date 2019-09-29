// LeftView.h : Schnittstelle der Klasse CLeftView
//


#pragma once

class CAnalyseMapFileDoc;

class CLeftView : public CListView
{
protected: // Nur aus Serialisierung erstellen
	CLeftView();
	DECLARE_DYNCREATE(CLeftView)

// Attribute
public:
	CAnalyseMapFileDoc* GetDocument();

// Operationen
public:

// Überschreibungen
	//{{AFX_VIRTUAL(CLeftView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnDraw(CDC* pDC);
	virtual void OnInitialUpdate(); // Erster Aufruf nach Erstellung
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);

// Implementierung
public:
	virtual ~CLeftView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	//}}AFX_VIRTUAL

protected:

	//{{AFX_MSG(CLeftView)
// Generierte Funktionen für die Meldungstabellen
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // Debugversion in LeftView.cpp
inline CAnalyseMapFileDoc* CLeftView::GetDocument()
   { return reinterpret_cast<CAnalyseMapFileDoc*>(m_pDocument); }
#endif

