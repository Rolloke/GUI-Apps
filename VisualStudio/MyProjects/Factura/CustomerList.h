#ifndef AFX_CUSTOMERLIST_H__C7FAC242_AAE8_11D2_8859_A1CC9EEA3618__INCLUDED_
#define AFX_CUSTOMERLIST_H__C7FAC242_AAE8_11D2_8859_A1CC9EEA3618__INCLUDED_

// CustomerList.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Fenster CCustomerList 

class CCustomerList : public CListCtrl
{
// Konstruktion
public:
	CCustomerList();

// Attribute
public:
   int m_nValue;
// Operationen
public:

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CCustomerList)
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementierung
public:
   virtual ~CCustomerList();
//   virtual void DrawItem(LPDRAWITEMSTRUCT);

	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	//{{AFX_MSG(CCustomerList)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_CUSTOMERLIST_H__C7FAC242_AAE8_11D2_8859_A1CC9EEA3618__INCLUDED_
