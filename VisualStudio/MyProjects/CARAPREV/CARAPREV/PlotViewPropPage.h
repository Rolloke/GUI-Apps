#ifndef AFX_PLOTVIEWPROPPAGE_H__30FA9ED9_14B6_11D2_9DB9_0000B458D891__INCLUDED_
#define AFX_PLOTVIEWPROPPAGE_H__30FA9ED9_14B6_11D2_9DB9_0000B458D891__INCLUDED_

// PlotViewPropPage.h : Header-Datei
//
#include "PlotLabel.h"	// Hinzugefügt von ClassView
#include "CaraPropertyPage.h"
#include "Resource.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CPlotViewPropPage 

class AFX_EXT_CLASS CPlotViewPropPage : public CCaraPropertyPage
{
	DECLARE_DYNCREATE(CPlotViewPropPage)

// Konstruktion
public:
   CPlotViewPropPage();
   virtual ~CPlotViewPropPage();
   void CopyValues();
   void SetPlot(CPlotLabel*);

// Dialogfelddaten
   //{{AFX_DATA(CPlotViewPropPage)
	enum { IDD = IDD_PROPPAGE_PLOTVIEW };
   int   m_nXNumMode;
   int   m_nYNumMode;
   int   m_nXRound;
   int   m_nYRound;
   float m_fXGridStep;
   float m_fYGridStep;
	BOOL  m_bDrawCurveFrame;
	BOOL  m_bShowGridX;
	BOOL  m_bShowGridY;
	BOOL  m_bShowTextX;
	BOOL  m_bShowTextY;
	//}}AFX_DATA
   CPlotLabel *m_pPlot;
   COLORREF    m_GridLineColor;
   COLORREF    m_HeadingColor;
   LOGFONT     m_HeadingLogFont;
   COLORREF    m_GridTextColor;
   LOGFONT     m_GridTextLogFont;
   LOGFONT     m_CurveLogFont;
   bool        m_bPolar;

// Überschreibungen
	// Der Klassen-Assistent generiert virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CPlotViewPropPage)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnApply();
	virtual void OnOK();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	//{{AFX_MSG(CPlotViewPropPage)
	afx_msg void OnCurveFont();
	afx_msg void OnGridFont();
	afx_msg void OnHeadingFont();
	afx_msg void OnGridColor();
	afx_msg void OnChangeValues();
	afx_msg void OnShowgridX();
	afx_msg void OnShowgridY();
	afx_msg void OnShowtextX();
	afx_msg void OnShowtextY();
	virtual BOOL OnInitDialog();
	afx_msg void OnPltNumRepX();
	afx_msg void OnPltNumRepY();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_PLOTVIEWPROPPAGE_H__30FA9ED9_14B6_11D2_9DB9_0000B458D891__INCLUDED_
