// SrvrItem.h : Schnittstelle der Klasse CETS2DVSrvrItem
//

#if !defined(AFX_SRVRITEM_H__E217CE71_3215_11D4_B6EC_0000B458D891__INCLUDED_)
#define AFX_SRVRITEM_H__E217CE71_3215_11D4_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CETS2DVSrvrItem : public COleServerItem
{
	DECLARE_DYNAMIC(CETS2DVSrvrItem)

// Konstruktoren
public:
	CETS2DVSrvrItem(CETS2DVDoc* pContainerDoc);

// Attribute
	CETS2DVDoc* GetDocument() const
		{ return (CETS2DVDoc*)COleServerItem::GetDocument(); }

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CETS2DVSrvrItem)
	public:
	virtual BOOL OnDraw(CDC* pDC, CSize& rSize);
	virtual BOOL OnGetExtent(DVASPECT dwDrawAspect, CSize& rSize);
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~CETS2DVSrvrItem();
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

#endif // !defined(AFX_SRVRITEM_H__E217CE71_3215_11D4_B6EC_0000B458D891__INCLUDED_)
