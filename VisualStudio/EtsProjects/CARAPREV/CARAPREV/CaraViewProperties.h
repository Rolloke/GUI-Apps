#ifndef AFX_CARAVIEWPROPERTIES_H__30FA9EDA_14B6_11D2_9DB9_0000B458D891__INCLUDED_
#define AFX_CARAVIEWPROPERTIES_H__30FA9EDA_14B6_11D2_9DB9_0000B458D891__INCLUDED_

// CaraViewProperties.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// CCaraViewProperties

/////////////////////////////////////////////////////////////////////////////
// CCaraViewProperties
class CLabel;
class CLineStylePropPage;
class CBrushStylePropPage;
class CFontPropPage;
class CPlotRangePropPage;
class CCurveValuePropPage;
class CCurveColorPropPage;
class CPlotViewPropPage;
class CPicturePropPage;
class CColScalePropPage;

class AFX_EXT_CLASS CCaraViewProperties : public CPropertySheet
{
	DECLARE_DYNAMIC(CCaraViewProperties)

// Konstruktion
public:
	CCaraViewProperties(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CCaraViewProperties(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attribute
public:
   CLineStylePropPage  *m_pLineStyle;
   CBrushStylePropPage *m_pBrushStyle;
   CFontPropPage       *m_pFontProps;
   CPlotRangePropPage  *m_pPlotRange;
   CCurveValuePropPage *m_pCurveValues;
   CCurveColorPropPage *m_pCurveColors;
   CPlotViewPropPage   *m_pPlotViewProps;
   CPicturePropPage    *m_pPictureProps;
   CColScalePropPage   *m_pColScaleProps;
   CView               *m_pView;
   COLORREF             m_BkColor;

// Operationen
public:

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CCaraViewProperties)
	virtual int DoModal();
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL
	virtual void BuildPropPageArray();

// Implementierung
public:
	bool RequestSaveChanges();
   virtual ~CCaraViewProperties();
   void SetTitle(UINT);
   BOOL InitDialog(int, CLabel *);
   void RemoveAllPages();
   BOOL IsModified();
   BOOL IsApplyButtonVisible();
   void CleanUpLabel();
   void SetApplyNowButton(bool bApply);

	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	//{{AFX_MSG(CCaraViewProperties)
	afx_msg LRESULT OnHelp(WPARAM, LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void Init();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_CARAVIEWPROPERTIES_H__30FA9EDA_14B6_11D2_9DB9_0000B458D891__INCLUDED_
