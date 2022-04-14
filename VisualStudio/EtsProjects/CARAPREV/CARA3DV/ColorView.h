#ifndef AFX_COLORVIEW_H__1F2B2A09_170A_11D2_9DB9_0000B458D891__INCLUDED_
#define AFX_COLORVIEW_H__1F2B2A09_170A_11D2_9DB9_0000B458D891__INCLUDED_

// ColorView.h : Header-Datei
//
#include "Ets3DGL.h"
/////////////////////////////////////////////////////////////////////////////
// Fenster CColorView 
#define  CV_LBUTTONDOWN    1
#define  CV_LBUTTONUP      2
#define  CV_RBUTTONDOWN    3
#define  CV_RBUTTONUP      4
#define  CV_MOUSEMOVE      5

#define  FIRST_COLOR       0
#define  LAST_COLOR       -1
#define  CURRENT_DOWN     -2
#define  CURRENT_UP       -3

class CColorArray;
class CLevelColor;

class CColorView : public CStatic
{
// Konstruktion
public:
	CColorView();
	bool Attach(CColorArray*);
   CColorArray * Detach();

// Attribute
public:

// Operationen
public:

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CColorView)
	//}}AFX_VIRTUAL

// Implementierung
public:
	CColorArray & GetColorArray();
   virtual   ~CColorView();
   bool       CreateColorArray();
   int        GetCurrentIndexUp()    {return m_nCurrentIndexUp;};
   int        GetCurrentIndexDown()  {return m_nCurrentIndexDown;};
   void       ResetMouseEvent()      {m_nCurrentIndexDown = -1;};
   int        GetColorIndex(int nC, float fCol){return GetIndexFromColor(GetColor(nC), fCol);};
   COLORREF   GetColor(int nIndex = -1);
   void       SetColor(COLORREF, int nIndex = -1);
   int        SetCurrentFromPoint(CPoint, int nCurrent = 0);
   void       SetColorMode(UINT nMode);
	bool       SetArraySize(int);
	int        GetNumColor();
	bool       SetColors(COLORREF *pColors, int nColors);
	bool       IsChanged();
   void       operator =(CColorView  &);
   void       operator =(CColorArray &);
   void       operator =(CLevelColor &);

	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	//{{AFX_MSG(CColorView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	int         m_nCurrentIndexUp;
	int         m_nCurrentIndexDown;
	COLORREF    m_FirstColor;
	COLORREF    m_LastColor;
   CColorArray*m_pColors;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_COLORVIEW_H__1F2B2A09_170A_11D2_9DB9_0000B458D891__INCLUDED_
