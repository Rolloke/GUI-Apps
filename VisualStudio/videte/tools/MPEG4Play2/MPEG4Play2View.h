// MPEG4Play2View.h : Schnittstelle der Klasse CMPEG4Play2View
//


#pragma once


class CMPEG4Play2View : public CView
{
// Konstruktor / Destruktor/////////////////////////////////////////////////////////
protected: // Nur aus Serialisierung erstellen
	CMPEG4Play2View();
	DECLARE_DYNCREATE(CMPEG4Play2View)
public:
	virtual ~CMPEG4Play2View();

// Attribute ///////////////////////////////////////////////////////////////////////
public:
	CMPEG4Play2Doc* GetDocument() const;

// Methoden ////////////////////////////////////////////////////////////////////////

// Überschreibungen /////////////////////////////////////////////////////////////////
public:
	virtual void OnDraw(CDC* pDC);  // Überladen, um diese Ansicht darzustellen
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate(void);

protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementierung
public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generierte Funktionen für die Meldungstabellen
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // Debugversion in MPEG4Play2View.cpp
inline CMPEG4Play2Doc* CMPEG4Play2View::GetDocument() const
   { return reinterpret_cast<CMPEG4Play2Doc*>(m_pDocument); }
#endif

