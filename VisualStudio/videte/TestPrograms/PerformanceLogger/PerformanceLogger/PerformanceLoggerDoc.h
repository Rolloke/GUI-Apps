// PerformanceLoggerDoc.h : Schnittstelle der Klasse CPerformanceLoggerDoc
//


#pragma once

class CPerformanceLoggerDoc : public CDocument
{
protected: // Nur aus Serialisierung erstellen
	CPerformanceLoggerDoc();
	DECLARE_DYNCREATE(CPerformanceLoggerDoc)

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
	virtual ~CPerformanceLoggerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generierte Funktionen für die Meldungstabellen
protected:
	DECLARE_MESSAGE_MAP()
};


