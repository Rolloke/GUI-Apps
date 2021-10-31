// SrvrItem.h : Schnittstelle der Klasse CVollOleAppSrvrItem
//

#if !defined(AFX_SRVRITEM_H__E1E8E851_3E20_11D4_B6EC_0000B458D891__INCLUDED_)
#define AFX_SRVRITEM_H__E1E8E851_3E20_11D4_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CVollOleAppSrvrItem : public CDocObjectServerItem
{
	DECLARE_DYNAMIC(CVollOleAppSrvrItem)

// Konstruktoren
public:
	CVollOleAppSrvrItem(CVollOleAppDoc* pContainerDoc);

// Attribute
	CVollOleAppDoc* GetDocument() const
		{ return (CVollOleAppDoc*)CDocObjectServerItem::GetDocument(); }

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CVollOleAppSrvrItem)
	public:
	virtual BOOL OnDraw(CDC* pDC, CSize& rSize);
	virtual BOOL OnGetExtent(DVASPECT dwDrawAspect, CSize& rSize);
	//}}AFX_VIRTUAL

// Implementierung
public:
	~CVollOleAppSrvrItem();
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

#endif // !defined(AFX_SRVRITEM_H__E1E8E851_3E20_11D4_B6EC_0000B458D891__INCLUDED_)
