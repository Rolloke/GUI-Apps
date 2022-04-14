#if !defined(AFX_INFOTOOLBAR_H__FBEE3D44_0EAA_11D3_B6EC_0000B458D891__INCLUDED_)
#define AFX_INFOTOOLBAR_H__FBEE3D44_0EAA_11D3_B6EC_0000B458D891__INCLUDED_

#include "ColorView.h"	// Hinzugefügt von der Klassenansicht
#include "ScaleView.h"	// Hinzugefügt von der Klassenansicht
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InfoToolBar.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Fenster CColorBar 

class CColorBar : public CControlBar
{
// Konstruktion
public:
	CColorBar();
	virtual ~CColorBar();
	virtual int OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
   virtual LRESULT DefWindowProc(UINT nMessage, WPARAM wParam, LPARAM lParam) 
   {
      return CWnd::DefWindowProc(nMessage, wParam, lParam);
   };
   BOOL Create(  CWnd*,	DWORD dwStyle = WS_CHILD|WS_VISIBLE|CBRS_TOP, UINT nID = AFX_IDW_TOOLBAR);
   BOOL CreateEx(CWnd*, DWORD ,DWORD , CRect, UINT);

// Attribute
public:

// Operationen
public:
	virtual void  DrawItem(LPDRAWITEMSTRUCT);
	virtual void  MeasureItem(LPMEASUREITEMSTRUCT);
	virtual CSize CalcFixedLayout(BOOL, BOOL);
	virtual CSize CalcDynamicLayout(int, DWORD);
	virtual void  OnUpdateCmdUI(CFrameWnd*, BOOL);
	virtual void  DoPaint(CDC* pDC);

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CColorBar)
	//}}AFX_VIRTUAL

// Implementierung
public:
	bool GetFloating();
	void SetFloating(bool);
	void VisiblebyUser(bool);
	bool IsVisiblebyUserCmd();
   void           SetButtonSize(CSize &);
	bool           AttachColorArray(CColorArray *);
	CColorArray *  DetachColorArray();
	void           SetMinMaxLevel(double, double);
	void           SetUnit(char*);
	void           SetNumRepresentation(int, int);

private:
	CSize CommitFloating();
	CSize CalcFloatingLayout(int, bool);
	CSize CommitDocked(bool);
	void  CommitWindowPositions(CRect &, CRect&);
	void  CalcVertWindowPos(CRect &, CRect &, CSize &);
	void  CalcHorzWindowPos(CRect &, CRect &, CSize &);
	void  InitSize();

   // Generierte Nachrichtenzuordnungsfunktionen
protected:
	void InitSysColors();
	//{{AFX_MSG(CColorBar)
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSysColorChange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	CSize       m_szFloating;     // Aktuelle Größe des Clientbereichs bei Floating Bar abzüglich der Ränder
	CSize       m_szMinSize;      // Minimale Größe der Toolbar abzüglich der Ränder
   CSize       m_szScale;        // Breite bzw. Höhe der Scala
   CSize       m_szButton;       // Breite bzw. Höhe eines ToolbarButtons
	CColorView  m_wndColors;      // FarbView
	CScaleView  m_wndScale;       // Scalierung
   DWORD       m_dwStates;       // Zustände für das Updaten
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_INFOTOOLBAR_H__FBEE3D44_0EAA_11D3_B6EC_0000B458D891__INCLUDED_
