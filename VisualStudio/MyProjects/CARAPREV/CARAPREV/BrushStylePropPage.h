#ifndef AFX_BRUSHSTYLEPROPPAGE_H__30FA9ED3_14B6_11D2_9DB9_0000B458D891__INCLUDED_
#define AFX_BRUSHSTYLEPROPPAGE_H__30FA9ED3_14B6_11D2_9DB9_0000B458D891__INCLUDED_

// BrushStylePropPage.h : Header-Datei
//
#include "BrushListBox.h"
#include "PropertyPreview.h"
#include "CaraPropertyPage.h"
#include "Resource.h"
/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CBrushStylePropPage 
#define NO_OF_BRUSHES 8
class CDibSection;

class AFX_EXT_CLASS CBrushStylePropPage : public CCaraPropertyPage
{
	DECLARE_DYNCREATE(CBrushStylePropPage)

// Konstruktion
public:
	CBrushStylePropPage();
	~CBrushStylePropPage();
   void  CheckStyle(UINT);
   void  SetLabel(CLabel *);

// Public Members
public:
   LOGBRUSH          m_LogBrush;
   COLORREF          m_cBkColor;
// Dialogfelddaten
	//{{AFX_DATA(CBrushStylePropPage)
	enum { IDD = IDD_PROPPAGE_BRUSH };
   CButton           m_BkColor;
   CButton           m_BrushColor;
   CBrushListBox     m_BrushStyle;
   CPropertyPreview  m_Preview;
	//}}AFX_DATA

// Private Members
   BOOL              m_bDrawBkGnd;
private:
   CLabel           *m_pLabel;
   static LOGBRUSH   gm_LogBrushes[NO_OF_BRUSHES];
#ifdef _DEBUG
	void DeleteLogBrushBmp();
   CDibSection      *m_pDib;
#endif
// Überschreibungen
	// Der Klassen-Assistent generiert virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CBrushStylePropPage)
public:
   virtual BOOL OnSetActive();
   virtual BOOL OnApply();
   virtual void OnOK();
   virtual BOOL OnKillActive();
protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
   virtual BOOL OnInitDialog(); 
	//}}AFX_VIRTUAL

// Implementierung
protected:
	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CBrushStylePropPage)
   afx_msg void OnBrushcolor();
   afx_msg void OnSelchangeBrushstyle();
   afx_msg void OnBkcolor();
	//}}AFX_MSG
#ifdef _DEBUG
   afx_msg void OnBrushbitmap();
#endif
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_BRUSHSTYLEPROPPAGE_H__30FA9ED3_14B6_11D2_9DB9_0000B458D891__INCLUDED_
