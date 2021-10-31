// VollOleAppView.h : Schnittstelle der Klasse CVollOleAppView
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_VOLLOLEAPPVIEW_H__E1E8E84E_3E20_11D4_B6EC_0000B458D891__INCLUDED_)
#define AFX_VOLLOLEAPPVIEW_H__E1E8E84E_3E20_11D4_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CVollOleAppCntrItem;

class CVollOleAppView : public CView
{
protected: // Nur aus Serialisierung erzeugen
	CVollOleAppView();
	DECLARE_DYNCREATE(CVollOleAppView)

// Attribute
public:
	CVollOleAppDoc* GetDocument();
	// m_pSelection enthält die Auswahl des aktuellen CVollOleAppCntrItem-Objekts.
	// Für viele Anwendungen ist eine derartige Member-Variable nicht angemessen, um
	//  z.B. eine Mehrfachauswahl oder eine Auswahl von Objekten zu repräsentieren,
	//  die keine CVollOleAppCntrItem-Objekte sind. Dieser Auswahlmechanismus
	//  dient nur dazu, Ihnen bei den ersten Schritten zu helfen.

	// ZU ERLEDIGEN: Ersetzen Sie diesen Auswahlmechanismus durch einen für Ihre Anwendung geeigneten.
	CVollOleAppCntrItem* m_pSelection;

// Operationen
public:

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CVollOleAppView)
	public:
	virtual void OnDraw(CDC* pDC);  // überladen zum Zeichnen dieser Ansicht
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // das erste mal nach der Konstruktion aufgerufen
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual BOOL IsSelected(const CObject* pDocItem) const;// Container-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~CVollOleAppView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generierte Message-Map-Funktionen
protected:
	//{{AFX_MSG(CVollOleAppView)
		// HINWEIS - An dieser Stelle werden Member-Funktionen vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	afx_msg void OnDestroy();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnInsertObject();
	afx_msg void OnCancelEditCntr();
	afx_msg void OnCancelEditSrvr();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // Testversion in VollOleAppView.cpp
inline CVollOleAppDoc* CVollOleAppView::GetDocument()
   { return (CVollOleAppDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_VOLLOLEAPPVIEW_H__E1E8E84E_3E20_11D4_B6EC_0000B458D891__INCLUDED_)
