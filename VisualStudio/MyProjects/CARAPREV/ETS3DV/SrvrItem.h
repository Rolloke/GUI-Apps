// SrvrItem.h : Schnittstelle der Klasse CETS3DVSrvrItem
//

#if !defined(AFX_SRVRITEM_H__579C69CE_3BA8_11D4_B6EC_0000B458D891__INCLUDED_)
#define AFX_SRVRITEM_H__579C69CE_3BA8_11D4_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CETS3DVSrvrItem : public CDocObjectServerItem
{
	DECLARE_DYNAMIC(CETS3DVSrvrItem)

// Konstruktoren
public:
	CETS3DVSrvrItem(CETS3DVDoc* pContainerDoc);

// Attribute
	CETS3DVDoc* GetDocument() const
		{ return (CETS3DVDoc*)CDocObjectServerItem::GetDocument(); }

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CETS3DVSrvrItem)
	public:
	virtual BOOL OnDraw(CDC* pDC, CSize& rSize);
	virtual BOOL OnGetExtent(DVASPECT dwDrawAspect, CSize& rSize);
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~CETS3DVSrvrItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void Serialize(CArchive& ar);   // überladen für Dokument-E/A
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_SRVRITEM_H__579C69CE_3BA8_11D4_B6EC_0000B458D891__INCLUDED_)
