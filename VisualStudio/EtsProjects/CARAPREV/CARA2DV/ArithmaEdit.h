#if !defined(AFX_ARITHMAEDIT_H__359B0DC4_C0EA_11D2_9E4E_0000B458D891__INCLUDED_)
#define AFX_ARITHMAEDIT_H__359B0DC4_C0EA_11D2_9E4E_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ArithmaEdit.h : Header-Datei
//

class CTreeDlg;
/////////////////////////////////////////////////////////////////////////////
// Fenster CArithmaEdit 
class CArithmaEdit : public CEdit
{
// Konstruktion
public:
	CArithmaEdit();

// Attribute
public:

// Operationen
public:

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CArithmaEdit)
	//}}AFX_VIRTUAL

// Implementierung
public:
	static int SetArithmaStrings(CTreeDlg*, int);
	virtual ~CArithmaEdit();

	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	//{{AFX_MSG(CArithmaEdit)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.
#endif // AFX_ARITHMAEDIT_H__359B0DC4_C0EA_11D2_9E4E_0000B458D891__INCLUDED_
