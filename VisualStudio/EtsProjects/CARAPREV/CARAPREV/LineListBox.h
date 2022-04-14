#ifndef AFX_LINELISTBOX_H__B59D8C0A_04F7_11D2_9DB9_0000B458D891__INCLUDED_
#define AFX_LINELISTBOX_H__B59D8C0A_04F7_11D2_9DB9_0000B458D891__INCLUDED_

// LineListBox.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Fenster CLineListBox 

class AFX_EXT_CLASS CLineListBox : public CListBox
{
// Konstruktion
public:
   CLineListBox();

// Attribute
public:

// Operationen
public:

// Überschreibungen
   // Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
   //{{AFX_VIRTUAL(CLineListBox)
	public:
   virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementierung
public:
	void DrawLine(LPDRAWITEMSTRUCT lpDIS);
	void DrawSelect(LPDRAWITEMSTRUCT lpDIS);
	void DrawFocus(LPDRAWITEMSTRUCT lpDIS);
   virtual ~CLineListBox();

   // Generierte Nachrichtenzuordnungsfunktionen
protected:
   //{{AFX_MSG(CLineListBox)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.
#endif // AFX_LINELISTBOX_H__B59D8C0A_04F7_11D2_9DB9_0000B458D891__INCLUDED_
