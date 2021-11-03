#if !defined(AFX_COLSCALEPROPPAGE_H__ECB0D203_1E7E_11D3_B6EC_0000B458D891__INCLUDED_)
#define AFX_COLSCALEPROPPAGE_H__ECB0D203_1E7E_11D3_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColScalePropPage.h : Header-Datei
//
#include "Resource.h"
#include "PropertyPreview.h"
#include "CaraPropertyPage.h"

class CColorLabel;
/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CColScalePropPage 

class CColScalePropPage : public CCaraPropertyPage
{
	DECLARE_DYNCREATE(CColScalePropPage)

// Konstruktion
public:
	void Updatepreview();
	CColScalePropPage();
	virtual ~CColScalePropPage();
	void SetLabel(CLabel *pl);

// Dialogfelddaten
	//{{AFX_DATA(CColScalePropPage)
	enum { IDD = IDD_PROPPAGE_COLORSCALE };
	CPropertyPreview  m_Preview;
	int               m_nRound;
	BOOL              m_bShowText;
	int               m_nNumMode;
	int               m_nStepWidth;
	//}}AFX_DATA
   LOGFONT           m_Font;
   COLORREF          m_TextColor;
   double            m_dValue;
// Überschreibungen
	// Der Klassen-Assistent generiert virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CColScalePropPage)
	public:
	virtual BOOL OnApply();
	virtual BOOL OnKillActive();
	virtual BOOL OnSetActive();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CColScalePropPage)
	afx_msg void OnCscaleFont();
	afx_msg void OnChangeValues();
	virtual BOOL OnInitDialog();
	afx_msg void OnCscaleNumRep();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
   CColorLabel *m_pLabel;
   CTextLabel  *m_pTextLabel;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_COLSCALEPROPPAGE_H__ECB0D203_1E7E_11D3_B6EC_0000B458D891__INCLUDED_
