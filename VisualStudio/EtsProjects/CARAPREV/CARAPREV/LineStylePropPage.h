#ifndef AFX_LINESTYLEPROPPAGE_H__30FA9ED7_14B6_11D2_9DB9_0000B458D891__INCLUDED_
#define AFX_LINESTYLEPROPPAGE_H__30FA9ED7_14B6_11D2_9DB9_0000B458D891__INCLUDED_

// LineStylePropPage.h : Header-Datei
//
#include "LineListBox.h"
#include "PropertyPreview.h"
#include "CaraPropertyPage.h"
#include "Resource.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CLineStylePropPage 

class AFX_EXT_CLASS CLineStylePropPage : public CCaraPropertyPage
{
	DECLARE_DYNCREATE(CLineStylePropPage)

// Konstruktion
public:
   CLineStylePropPage();
   virtual ~CLineStylePropPage();
   void SetLabel(CLabel*);

   LOGPEN   m_LogPen;

// Dialogfelddaten
   //{{AFX_DATA(CLineStylePropPage)
   enum { IDD = IDD_PROPPAGE_LINE };
   CLineListBox      m_LineStyle;
   int		         m_nLineWidth;
   CPropertyPreview  m_Preview;
   //}}AFX_DATA


// Überschreibungen
	// Der Klassen-Assistent generiert virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CLineStylePropPage)
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
   //{{AFX_MSG(CLineStylePropPage)
   afx_msg void OnLinecolor();
   virtual BOOL OnInitDialog();
   afx_msg void OnSelchangeLinestyle();
	afx_msg void OnChangeFrameLinewidth();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
   CLabel  *m_pLabel;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_LINESTYLEPROPPAGE_H__30FA9ED7_14B6_11D2_9DB9_0000B458D891__INCLUDED_
