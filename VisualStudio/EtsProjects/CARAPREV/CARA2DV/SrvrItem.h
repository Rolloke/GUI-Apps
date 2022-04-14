// SrvrItem.h : Schnittstelle der Klasse COleServerSrvrItem
//

#if !defined(AFX_SRVRITEM_H__A867418E_2C1D_11D4_B6EC_0000B458D891__INCLUDED_)
#define AFX_SRVRITEM_H__A867418E_2C1D_11D4_B6EC_0000B458D891__INCLUDED_
#include "stdafx.h"

#ifdef ETS_OLE_SERVER

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCARA2DVDoc;
class COleServerSrvrItem : public COleServerItem
{
	DECLARE_DYNAMIC(COleServerSrvrItem)

// Konstruktoren
public:
	COleServerSrvrItem(CCARA2DVDoc* pContainerDoc);

// Attribute
	CCARA2DVDoc* GetDocument() const
		{ return (CCARA2DVDoc*)COleServerItem::GetDocument(); }

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(COleServerSrvrItem)
	public:
	virtual BOOL OnDraw(CDC* pDC, CSize& rSize);
   virtual BOOL OnDrawEx( CDC* pDC, DVASPECT nDrawAspect, CSize& rSize);
	virtual BOOL OnGetExtent(DVASPECT dwDrawAspect, CSize& rSize);
   virtual BOOL OnQueryUpdateItems();
   virtual void OnUpdateItems();
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~COleServerSrvrItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void Serialize(CArchive& ar);   // überladen für Dokument-E/A

	static int DrawLabel(CLabel*, void*);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // ETS_OLE_SERVER

#endif // !defined(AFX_SRVRITEM_H__A867418E_2C1D_11D4_B6EC_0000B458D891__INCLUDED_)
