#ifndef AFX_CURVEVALUEPROPPAGE_H__30FA9ED4_14B6_11D2_9DB9_0000B458D891__INCLUDED_
#define AFX_CURVEVALUEPROPPAGE_H__30FA9ED4_14B6_11D2_9DB9_0000B458D891__INCLUDED_

#include "Resource.h"
#include "CaraPropertyPage.h"
#include <afxwin.h>

class CCurveLabel;
class CPlotLabel;

// CurveValuePropPage.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CCurveValuePropPage 

class AFX_EXT_CLASS CCurveValuePropPage : public CCaraPropertyPage
{
	DECLARE_DYNCREATE(CCurveValuePropPage)

// Konstruktion
public:
   CCurveValuePropPage();
   virtual ~CCurveValuePropPage();
   void SetPlot(CPlotLabel *);
   BOOL SetCurve(int);
   void SetCurveListContent();
	virtual BOOL UpdateData(BOOL);

//   CComboBox         m_cCurveList;
// Dialogfelddaten
   //{{AFX_DATA(CCurveValuePropPage)
	enum { IDD = IDD_PROPPAGE_CURVE };
	CListCtrl	m_cValueList;
	CListCtrl	m_cCurveList;
   CSpinButtonCtrl   m_cCurveIndex;
   CString           m_sUnit_x;
   CString           m_sUnit_y;
   double            m_fXValue;
   double            m_fYValue;
   int		         m_nCurveWidth;
   BOOL              m_bEditable;
   int               m_nEditMode;
   BOOL              m_bVisible;
   double            m_dOffset;
	int               m_nCurveIndex;
	double            m_dOrigin;
	double            m_dStep;
	double            m_dEnd;
	CString           m_sUnitLocus;
	int               m_nFormat;
	//}}AFX_DATA
   BOOL              m_bEdit;
   CCurveLabel      *m_pCurve;
   CPlotLabel       *m_pPlot;
   COLORREF          m_CurveColor;
   int               m_nCursel;
   bool              m_bGradientChangeable;
	double            m_dOldOrigin;
	double            m_dOldStep;
// Überschreibungen
	// Der Klassen-Assistent generiert virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CCurveValuePropPage)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnApply();
	virtual void OnOK();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL
   static int gm_nCurveValueCtrl[];
   static int gm_nCurveUnitsCtrl[];
   static int gm_nCurveParamCtrl[];
   static int gm_nCurveValueListCtrl[];
// Implementierung
protected:
	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CCurveValuePropPage)
	afx_msg void OnChangeCurveIndex();
	virtual BOOL OnInitDialog();
	afx_msg void OnCurveColor();
	afx_msg void OnEditable();
	afx_msg void OnChangeValues();
	afx_msg void OnKillfocusCurveValueY();
	afx_msg void OnEndlabeleditCurveList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedCurveList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginlabeleditCurveList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeCurveTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangingCurveTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeCurveFormat();
	afx_msg void OnKillfocusCurveEnd();
	afx_msg void OnKillfocusCurveOrigin();
	afx_msg void OnKillfocusCurveStep();
	afx_msg void OnGetdispinfoCurveValueList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginlabeleditCurveValueList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditCurveValueList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickCurveValueList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
   int          m_nItem;
   int          m_nSubItem;
   int          m_nNoTabCtrlChange;
   POINT        m_Acc;

   static long gm_lOldEditCtrlWndProc;
   static LRESULT CALLBACK EditCtrlWndProc(HWND, UINT, WPARAM, LPARAM);
public:
   CComboBox m_ComboScatter;
   afx_msg void OnCbnSelchangeComboScatter();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_CURVEVALUEPROPPAGE_H__30FA9ED4_14B6_11D2_9DB9_0000B458D891__INCLUDED_
