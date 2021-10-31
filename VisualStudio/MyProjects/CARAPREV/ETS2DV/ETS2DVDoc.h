// ETS2DVDoc.h : Schnittstelle der Klasse CETS2DVDoc
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ETS2DVDOC_H__E217CE6C_3215_11D4_B6EC_0000B458D891__INCLUDED_)
#define AFX_ETS2DVDOC_H__E217CE6C_3215_11D4_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CETS2DVSrvrItem;

class CETS2DVDoc : public COleServerDoc
{
protected: // Nur aus Serialisierung erzeugen
	CETS2DVDoc();
	DECLARE_DYNCREATE(CETS2DVDoc)

// Attribute
public:
	CETS2DVSrvrItem* GetEmbeddedItem()
		{ return (CETS2DVSrvrItem*)COleServerDoc::GetEmbeddedItem(); }

// Operationen
public:

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CETS2DVDoc)
	protected:
	virtual COleServerItem* OnGetEmbeddedItem();
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~CETS2DVDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generierte Message-Map-Funktionen
protected:
	//{{AFX_MSG(CETS2DVDoc)
		// HINWEIS - An dieser Stelle werden Member-Funktionen vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// Generierte OLE-Dispatch-Map-Funktionen
	//{{AFX_DISPATCH(CETS2DVDoc)
		// HINWEIS - An dieser Stelle werden Member-Funktionen vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_ETS2DVDOC_H__E217CE6C_3215_11D4_B6EC_0000B458D891__INCLUDED_)
