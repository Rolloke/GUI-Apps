#ifndef AFX_PLOTRANGEPROPPAGE_H__30FA9ED8_14B6_11D2_9DB9_0000B458D891__INCLUDED_
#define AFX_PLOTRANGEPROPPAGE_H__30FA9ED8_14B6_11D2_9DB9_0000B458D891__INCLUDED_

// PlotRangePropPage.h : Header-Datei
//
#include "PlotLabel.h"
#include "CaraPropertyPage.h"
#include "Resource.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CPlotRangePropPage 

class AFX_EXT_CLASS CPlotRangePropPage : public CCaraPropertyPage
{
	DECLARE_DYNCREATE(CPlotRangePropPage)

// Konstruktion
public:
	CPlotRangePropPage();
	virtual ~CPlotRangePropPage();
   void SetPlot(CPlotLabel*);
	void CheckDivision();


// Dialogfelddaten
   //{{AFX_DATA(CPlotRangePropPage)
	enum { IDD = IDD_PROPPAGE_PLOTRANGE };
   double	m_dRangex2;
   double	m_dRangex1;
   double	m_dRangey1;
   double	m_dRangey2;
   CString	m_sUnit_x;
   CString	m_sUnit_y;
   CString	m_sPlotHeading;
   int		m_nDivision_x;
   int		m_nDivision_y;
	int		m_nDiagramType;
	//}}AFX_DATA
   BOOL        m_bEdit;
   CPlotLabel *m_pPlot;
   int         m_nAllowedCurves;
   POINT       m_Acc;

private:
   void DDX_Division(CDataExchange*, int, double &);
   void MinMaxDouble(CDataExchange*, int,  double, double, double &);

// Überschreibungen
	// Der Klassen-Assistent generiert virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CPlotRangePropPage)
	public:
	virtual void OnOK();
	virtual BOOL OnApply();
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CPlotRangePropPage)
	afx_msg void OnSetModified();
	afx_msg void OnCheckRange();
	afx_msg void OnChangeValues();
	afx_msg void OnSetDefaultrange();
	afx_msg void OnDeterminRange();
	virtual BOOL OnInitDialog();
	afx_msg void OnDiagramType();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_PLOTRANGEPROPPAGE_H__30FA9ED8_14B6_11D2_9DB9_0000B458D891__INCLUDED_
