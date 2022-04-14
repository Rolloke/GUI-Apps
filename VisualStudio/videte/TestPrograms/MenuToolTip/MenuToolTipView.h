// MenuToolTipView.h : Schnittstelle der Klasse CMenuToolTipView
//


#pragma once


class CMenuToolTipView : public CView
{
protected: // Nur aus Serialisierung erstellen
	CMenuToolTipView();
	DECLARE_DYNCREATE(CMenuToolTipView)

// Attribute
public:
	CMenuToolTipDoc* GetDocument() const;

// Operationen
public:

// Überschreibungen
	public:
	virtual void OnDraw(CDC* pDC);  // Überladen, um diese Ansicht darzustellen
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementierung
public:
	virtual ~CMenuToolTipView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generierte Funktionen für die Meldungstabellen
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // Debugversion in MenuToolTipView.cpp
inline CMenuToolTipDoc* CMenuToolTipView::GetDocument() const
   { return reinterpret_cast<CMenuToolTipDoc*>(m_pDocument); }
#endif

