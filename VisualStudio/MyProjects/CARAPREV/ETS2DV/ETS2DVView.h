// ETS2DVView.h : Schnittstelle der Klasse CETS2DVView
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ETS2DVVIEW_H__E217CE6E_3215_11D4_B6EC_0000B458D891__INCLUDED_)
#define AFX_ETS2DVVIEW_H__E217CE6E_3215_11D4_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CETS2DVView : public CView
{
protected: // Nur aus Serialisierung erzeugen
	CETS2DVView();
	DECLARE_DYNCREATE(CETS2DVView)

// Attribute
public:
	CETS2DVDoc* GetDocument();

// Operationen
public:

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CETS2DVView)
	public:
	virtual void OnDraw(CDC* pDC);  // überladen zum Zeichnen dieser Ansicht
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~CETS2DVView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generierte Message-Map-Funktionen
protected:
	//{{AFX_MSG(CETS2DVView)
		// HINWEIS - An dieser Stelle werden Member-Funktionen vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	afx_msg void OnCancelEditSrvr();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // Testversion in ETS2DVView.cpp
inline CETS2DVDoc* CETS2DVView::GetDocument()
   { return (CETS2DVDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_ETS2DVVIEW_H__E217CE6E_3215_11D4_B6EC_0000B458D891__INCLUDED_)
