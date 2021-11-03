#ifndef AFX_PROPERTYPREVIEW_H__30FA9EDD_14B6_11D2_9DB9_0000B458D891__INCLUDED_
#define AFX_PROPERTYPREVIEW_H__30FA9EDD_14B6_11D2_9DB9_0000B458D891__INCLUDED_

// PropertyPreview.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Fenster CPropertyPreview 
class CLabel;

class AFX_EXT_CLASS CPropertyPreview : public CStatic
{
// Konstruktion
public:
	CPropertyPreview();

// Attribute
public:
// Attribute
private:
   CLabel     *m_pLabel;
   LOGBRUSH   *m_pLogBrush;
   LOGPEN     *m_pLogPen;
   LOGFONT    *m_pLogFont;
   COLORREF   *m_pTextColor;
   UINT       *m_pTextAlign;
   int        *m_pnZoom;
   int        *m_pnExtraSpacing;
   COLORREF   *m_pBkColor;
   BOOL       *m_pbDrawBkGnd;
   COLORREF    m_BkColor;
// Operationen
public:

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CPropertyPreview)
	//}}AFX_VIRTUAL

// Implementierung
public:
   virtual ~CPropertyPreview();
   void     SetLogBrush(LOGBRUSH *plb) {m_pLogBrush = plb;};
   void     SetLogPen(LOGPEN     *plp) {m_pLogPen   = plp;};
   void     SetLabel(CLabel *pl)       {m_pLabel    = pl; };
   void     SetLogFont(LOGFONT*, COLORREF *, UINT*, int*);
   void     SetZoom(int *pnz)          {m_pnZoom    = pnz;};
   void     SetpBkColor(COLORREF *pBkColor){m_pBkColor = pBkColor;};
   void     SetBkColor(COLORREF  BkColor)  {m_BkColor = BkColor;};
   void     SetpbDrawBkGnd(BOOL *pb)       {m_pbDrawBkGnd = pb;};
	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	//{{AFX_MSG(CPropertyPreview)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_PROPERTYPREVIEW_H__30FA9EDD_14B6_11D2_9DB9_0000B458D891__INCLUDED_
