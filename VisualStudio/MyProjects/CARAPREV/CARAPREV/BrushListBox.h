#ifndef AFX_BRUSHLISTBOX_H__B59D8C0B_04F7_11D2_9DB9_0000B458D891__INCLUDED_
#define AFX_BRUSHLISTBOX_H__B59D8C0B_04F7_11D2_9DB9_0000B458D891__INCLUDED_

// BrushListBox.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Fenster CBrushListBox 

class AFX_EXT_CLASS CBrushListBox : public CListBox
{
// Konstruktion
public:
   CBrushListBox();

// Attribute
public:

// Operationen
public:

// Überschreibungen
   // Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
   //{{AFX_VIRTUAL(CBrushListBox)
	public:
   virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementierung
public:
	void DrawSelect(LPDRAWITEMSTRUCT lpDIS);
	void DrawFocus(LPDRAWITEMSTRUCT lpDIS);
	void DrawBrush(LPDRAWITEMSTRUCT lpDIS);
   virtual ~CBrushListBox();

   // Generierte Nachrichtenzuordnungsfunktionen
protected:
   //{{AFX_MSG(CBrushListBox)
      // HINWEIS - Der Klassen-Assistent fügt hier Member-Funktionen ein und entfernt diese.
   //}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_BRUSHLISTBOX_H__B59D8C0B_04F7_11D2_9DB9_0000B458D891__INCLUDED_
