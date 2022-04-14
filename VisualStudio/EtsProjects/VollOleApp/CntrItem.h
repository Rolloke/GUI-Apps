// CntrItem.h : Schnittstelle der Klasse CVollOleAppCntrItem
//

#if !defined(AFX_CNTRITEM_H__E1E8E855_3E20_11D4_B6EC_0000B458D891__INCLUDED_)
#define AFX_CNTRITEM_H__E1E8E855_3E20_11D4_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CVollOleAppDoc;
class CVollOleAppView;

class CVollOleAppCntrItem : public COleDocObjectItem
{
	DECLARE_SERIAL(CVollOleAppCntrItem)

// Konstruktoren
public:
	CVollOleAppCntrItem(CVollOleAppDoc* pContainer = NULL);
		// Hinweis: pContainer darf auch NULL sein, um IMPLEMENT_SERIALIZE zuzulassen.
		//  IMPLEMENT_SERIALIZE verlangt, dass die Klasse einen Konstruktor mit
		//  null Argumenten besitzt. Normalerweise werden OLE-Elemente mit einem 
		//  Dokumentzeiger konstruiert, der ungleich Null ist.

// Attribute
public:
	CVollOleAppDoc* GetDocument()
		{ return (CVollOleAppDoc*)COleDocObjectItem::GetDocument(); }
	CVollOleAppView* GetActiveView()
		{ return (CVollOleAppView*)COleDocObjectItem::GetActiveView(); }

	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CVollOleAppCntrItem)
	public:
	virtual void OnChange(OLE_NOTIFICATION wNotification, DWORD dwParam);
	virtual void OnActivate();
	protected:
	virtual void OnDeactivateUI(BOOL bUndoable);
	virtual BOOL OnChangeItemPosition(const CRect& rectPos);
	virtual BOOL CanActivate();
	//}}AFX_VIRTUAL

// Implementierung
public:
	~CVollOleAppCntrItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	virtual void Serialize(CArchive& ar);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_CNTRITEM_H__E1E8E855_3E20_11D4_B6EC_0000B458D891__INCLUDED_)
