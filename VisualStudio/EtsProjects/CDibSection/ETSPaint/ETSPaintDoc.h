// ETSPaintDoc.h : Schnittstelle der Klasse CETSPaintDoc
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ETSPAINTDOC_H__A560281A_9494_11D4_B6EC_0000B458D891__INCLUDED_)
#define AFX_ETSPAINTDOC_H__A560281A_9494_11D4_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DibSection.h"

#define VIEW_SIZE 0x00000001

class CETSPaintDoc : public CDocument
{
protected: // Nur aus Serialisierung erzeugen
	CETSPaintDoc();
	DECLARE_DYNCREATE(CETSPaintDoc)

// Attribute
public:
   CDibSection m_Dibsection;

// Operationen
public:

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CETSPaintDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void DeleteContents();
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~CETSPaintDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generierte Message-Map-Funktionen
protected:
	//{{AFX_MSG(CETSPaintDoc)
	afx_msg void OnEditConvert();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_ETSPAINTDOC_H__A560281A_9494_11D4_B6EC_0000B458D891__INCLUDED_)
