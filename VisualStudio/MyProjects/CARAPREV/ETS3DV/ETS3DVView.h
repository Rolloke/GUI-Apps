// ETS3DVView.h : Schnittstelle der Klasse CETS3DVView
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ETS3DVVIEW_H__579C69CB_3BA8_11D4_B6EC_0000B458D891__INCLUDED_)
#define AFX_ETS3DVVIEW_H__579C69CB_3BA8_11D4_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CETS3DVView : public CView
{
protected: // Nur aus Serialisierung erzeugen
	CETS3DVView();
	DECLARE_DYNCREATE(CETS3DVView)

// Attribute
public:
	CETS3DVDoc* GetDocument();

// Operationen
public:

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CETS3DVView)
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
	virtual ~CETS3DVView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generierte Message-Map-Funktionen
protected:
	//{{AFX_MSG(CETS3DVView)
		// HINWEIS - An dieser Stelle werden Member-Funktionen vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	afx_msg void OnCancelEditSrvr();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // Testversion in ETS3DVView.cpp
inline CETS3DVDoc* CETS3DVView::GetDocument()
   { return (CETS3DVDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_ETS3DVVIEW_H__579C69CB_3BA8_11D4_B6EC_0000B458D891__INCLUDED_)
