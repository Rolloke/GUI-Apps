// VollOleAppDoc.h : Schnittstelle der Klasse CVollOleAppDoc
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_VOLLOLEAPPDOC_H__E1E8E84C_3E20_11D4_B6EC_0000B458D891__INCLUDED_)
#define AFX_VOLLOLEAPPDOC_H__E1E8E84C_3E20_11D4_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CVollOleAppSrvrItem;

class CVollOleAppDoc : public COleServerDoc
{
protected: // Nur aus Serialisierung erzeugen
	CVollOleAppDoc();
	DECLARE_DYNCREATE(CVollOleAppDoc)

// Attribute
public:
	CVollOleAppSrvrItem* GetEmbeddedItem()
		{ return (CVollOleAppSrvrItem*)COleServerDoc::GetEmbeddedItem(); }

// Operationen
public:

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CVollOleAppDoc)
	protected:
	virtual COleServerItem* OnGetEmbeddedItem();
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~CVollOleAppDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual CDocObjectServer* GetDocObjectServer(LPOLEDOCUMENTSITE pDocSite);

// Generierte Message-Map-Funktionen
protected:
	//{{AFX_MSG(CVollOleAppDoc)
		// HINWEIS - An dieser Stelle werden Member-Funktionen vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// Generierte OLE-Dispatch-Map-Funktionen
	//{{AFX_DISPATCH(CVollOleAppDoc)
		// HINWEIS - An dieser Stelle werden Member-Funktionen vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_VOLLOLEAPPDOC_H__E1E8E84C_3E20_11D4_B6EC_0000B458D891__INCLUDED_)
