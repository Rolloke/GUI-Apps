// PerformanceLoggerView.h : Schnittstelle der Klasse CPerformanceLoggerView
//


#pragma once


class CPerformanceLoggerView : public CView
{
protected: // Nur aus Serialisierung erstellen
	CPerformanceLoggerView();
	DECLARE_DYNCREATE(CPerformanceLoggerView)

// Attribute
public:
	CPerformanceLoggerDoc* GetDocument() const;

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
	virtual ~CPerformanceLoggerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generierte Funktionen für die Meldungstabellen
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // Debugversion in PerformanceLoggerView.cpp
inline CPerformanceLoggerDoc* CPerformanceLoggerView::GetDocument() const
   { return reinterpret_cast<CPerformanceLoggerDoc*>(m_pDocument); }
#endif

