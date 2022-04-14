#ifndef AFX_SCALEVIEW_H__C47BADA2_4BB1_11D2_9DB9_0000B458D891__INCLUDED_
#define AFX_SCALEVIEW_H__C47BADA2_4BB1_11D2_9DB9_0000B458D891__INCLUDED_

// ScaleView.h : Header-Datei
//
#include "stdafx.h"
#include "..\ScalePainter.h"
/////////////////////////////////////////////////////////////////////////////
// Fenster CScaleView 

class CScaleView : public CStatic
{
// Konstruktion
public:
	CScaleView();

// Attribute
public:

// Operationen
public:

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CScaleView)
	//}}AFX_VIRTUAL

// Implementierung
public:
	CSize GetMinSize();
	CScalePainter m_ScalePainter;
   bool          m_bValidSize;

	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	//{{AFX_MSG(CScaleView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_SCALEVIEW_H__C47BADA2_4BB1_11D2_9DB9_0000B458D891__INCLUDED_
