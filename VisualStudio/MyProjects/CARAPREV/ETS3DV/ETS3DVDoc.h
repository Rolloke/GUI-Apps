// ETS3DVDoc.h : Schnittstelle der Klasse CETS3DVDoc
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ETS3DVDOC_H__579C69C9_3BA8_11D4_B6EC_0000B458D891__INCLUDED_)
#define AFX_ETS3DVDOC_H__579C69C9_3BA8_11D4_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CETS3DVSrvrItem;

class CETS3DVDoc : public COleServerDoc
{
protected: // Nur aus Serialisierung erzeugen
	CETS3DVDoc();
	DECLARE_DYNCREATE(CETS3DVDoc)

// Attribute
public:
	CETS3DVSrvrItem* GetEmbeddedItem()
		{ return (CETS3DVSrvrItem*)COleServerDoc::GetEmbeddedItem(); }

// Operationen
public:

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CETS3DVDoc)
	protected:
	virtual COleServerItem* OnGetEmbeddedItem();
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~CETS3DVDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual CDocObjectServer* GetDocObjectServer(LPOLEDOCUMENTSITE pDocSite);

// Generierte Message-Map-Funktionen
protected:
	//{{AFX_MSG(CETS3DVDoc)
		// HINWEIS - An dieser Stelle werden Member-Funktionen vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_ETS3DVDOC_H__579C69C9_3BA8_11D4_B6EC_0000B458D891__INCLUDED_)
