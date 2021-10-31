#ifndef AFX_CURVECOLORPROPPAGE_H__30FA9ED5_14B6_11D2_9DB9_0000B458D891__INCLUDED_
#define AFX_CURVECOLORPROPPAGE_H__30FA9ED5_14B6_11D2_9DB9_0000B458D891__INCLUDED_

// CurveColorPropPage.h : Header-Datei
//
#include "PropertyPreview.h"
#include "PlotLabel.h"
#include "BezierLabel.h"
#include "CaraPropertyPage.h"
#include "Resource.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CCurveColorPropPage 

class AFX_EXT_CLASS CCurveColorPropPage : public CCaraPropertyPage
{
	DECLARE_DYNCREATE(CCurveColorPropPage)

// Konstruktion
public:
	CCurveColorPropPage();
	virtual ~CCurveColorPropPage();
   void OnButtoncolor(UINT);
	void SetColors(COLORREF *);

   COLORREF      m_Backgroundcolor;

// Dialogfelddaten
	//{{AFX_DATA(CCurveColorPropPage)
	enum { IDD = IDD_PROPPAGE_CURVE_COLORS };
	//}}AFX_DATA
	CPropertyPreview	m_Preview[NUM_CURVE_COLORS];


// Überschreibungen
	// Der Klassen-Assistent generiert virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CCurveColorPropPage)
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
	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CCurveColorPropPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnViewbackground();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	bool          m_bColorsChanged;
	LOGPEN        m_CurvePen[NUM_CURVE_COLORS];
   COLORREF     *m_pColors;
	CBezierLabel  m_Bezier;
   int           m_nZoom;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_CURVECOLORPROPPAGE_H__30FA9ED5_14B6_11D2_9DB9_0000B458D891__INCLUDED_
