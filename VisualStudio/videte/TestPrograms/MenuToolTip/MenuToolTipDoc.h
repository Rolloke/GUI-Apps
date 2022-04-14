// MenuToolTipDoc.h : Schnittstelle der Klasse CMenuToolTipDoc
//


#pragma once

class CMenuToolTipDoc : public CDocument
{
protected: // Nur aus Serialisierung erstellen
	CMenuToolTipDoc();
	DECLARE_DYNCREATE(CMenuToolTipDoc)

// Attribute
public:

// Operationen
public:

// Überschreibungen
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementierung
public:
	virtual ~CMenuToolTipDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generierte Funktionen für die Meldungstabellen
protected:
	DECLARE_MESSAGE_MAP()
};


