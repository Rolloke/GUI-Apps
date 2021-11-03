#ifndef AFX_FONTPROPPAGE_H__30FA9ED6_14B6_11D2_9DB9_0000B458D891__INCLUDED_
#define AFX_FONTPROPPAGE_H__30FA9ED6_14B6_11D2_9DB9_0000B458D891__INCLUDED_

// FontPropPage.h : Header-Datei
//
#include "PropertyPreview.h"
#include "TextLabel.h"
#include "CaraPropertyPage.h"
#include "Resource.h"

#define TXT_TEXT_EDIT   0x0001
#define TXT_FONT        0x0002
#define TXT_HEIGHT      0x0004
#define TXT_WIDTH       0x0008
#define TXT_ALIGN       0x0010
#define TXT_EXTRA       0x0020
#define TXT_WEIGHT      0x0040
#define TXT_ITALIC      0x0080
#define TXT_UNDERLINE   0x0100
#define TXT_STRIKEOUT   0x0200
#define TXT_COLOR       0x0400
#define TXT_BKCOLOR     0x0800

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CFontPropPage 

class AFX_EXT_CLASS CFontPropPage : public CCaraPropertyPage
{
	DECLARE_DYNCREATE(CFontPropPage)

// Konstruktion
public:
   CFontPropPage();
   virtual ~CFontPropPage();
   void SetAlign(UINT);
   UINT GetAlign();
   void SetLabel(CLabel*);
   void SetMode(UINT nMode);

// Public Members
public:
   UINT     m_nAlign;
   COLORREF m_Color;
   COLORREF m_BKColor;
   LOGFONT  m_LogFont;
   CString  m_strFullName;
   int      m_nZoomFactor;
// Dialogfelddaten
	//{{AFX_DATA(CFontPropPage)
	enum { IDD = IDD_PROPPAGE_FONT };
   CComboBox        m_cFont;
   CPropertyPreview m_Preview;
   int              m_AlignH;
   int              m_AlignV;
   BOOL             m_bCursiv;
   BOOL             m_bUnderLine;
   BOOL             m_bLine;
   int              m_nExtra;
   int              m_nHeight;
   int              m_nWeight;
   int              m_nWidth;
   BOOL             m_bBkGround;
   int		      m_nZoom;
	//}}AFX_DATA

// Private Members
private:
   static int CALLBACK FindFontFamProc(ENUMLOGFONT *pelf, NEWTEXTMETRIC *pntm, int nFontType, LPARAM lParam);
   static int CALLBACK EnumFontFamProc(ENUMLOGFONT *pelf, NEWTEXTMETRIC *pntm, int nFontType, LPARAM lParam);

   CTextLabel  *m_pLabel;
   UINT         m_nMode;
// Überschreibungen
	// Der Klassen-Assistent generiert virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CFontPropPage)
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
	//{{AFX_MSG(CFontPropPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeFont();
	afx_msg void OnTextColor();
	afx_msg void OnBkgColor();
	afx_msg void OnSelchangeAlign();
	afx_msg void OnBkground();        
	afx_msg void OnCursiv();
	afx_msg void OnLine();
	afx_msg void OnUnderline();
	afx_msg void OnKillfocusWidth();
	afx_msg void OnKillfocusHeight();
	afx_msg void OnKillfocusExtra();
	afx_msg void OnSelchangeWeight();
	afx_msg void OnZoom();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_FONTPROPPAGE_H__30FA9ED6_14B6_11D2_9DB9_0000B458D891__INCLUDED_
