
// LeftView.h: Schnittstelle der Klasse CLeftView
//


#pragma once

class CAnyFileViewerDoc;

class CLeftView : public CTreeView
{
protected: // Nur aus Serialisierung erstellen
	CLeftView();
	DECLARE_DYNCREATE(CLeftView)

// Attribute
public:
	CAnyFileViewerDoc* GetDocument();

// Vorgänge
public:

// Überschreibungen
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnDraw(CDC* pDC);
	virtual void OnInitialUpdate(); // Erster Aufruf nach Erstellung

// Implementierung
public:
	virtual ~CLeftView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generierte Funktionen für die Meldungstabellen
protected:
	afx_msg void OnViewUpdate();
	DECLARE_MESSAGE_MAP()
public:
};

#ifndef _DEBUG  // Debugversion in LeftView.cpp
inline CAnyFileViewerDoc* CLeftView::GetDocument()
   { return reinterpret_cast<CAnyFileViewerDoc*>(m_pDocument); }
#endif

