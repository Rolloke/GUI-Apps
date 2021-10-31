#ifndef AFX_VIEWPARAMS_H__1F2B2A08_170A_11D2_9DB9_0000B458D891__INCLUDED_
#define AFX_VIEWPARAMS_H__1F2B2A08_170A_11D2_9DB9_0000B458D891__INCLUDED_

// ViewParams.h : Header-Datei
//
#include "NonModalDlg.h"
#include "ColorView.h"
#include "ScaleView.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CViewParams 
#define VIEWPARAMS_INTERPOLATE   0x00000001
#define VIEWPARAMS_ZOOM          0x00000002
#define VIEWPARAMS_PROJECTION    0x00000004
#define VIEWPARAMS_SHADECOLORS   0x00000008
#define VIEWPARAMS_BKGNDCOLOR    0x00000010
#define VIEWPARAMS_OBJECTCOLOR   0x00000020
#define VIEWPARAMS_MINMAXLEVEL   0x00000040
#define VIEWPARAMS_DETAIL        0x00000080
#define VIEWPARAMS_SHADING       0x00000100

class CViewParams : public CNonModalDlg
{
// Konstruktion
public:
   CViewParams(CWnd* pParent = NULL);   // Standardkonstruktor
   void ShowCurrentColor();

// Dialogfelddaten
   //{{AFX_DATA(CViewParams)
	enum { IDD = IDD_VIEW_DIALOG };
   CColorView	      m_BkGndColor;
   CColorView        m_ObjectColor;
   CColorView        m_ShadeColors;
   CColorView        m_Colors;
   CScaleView	      m_Scale;
   CSpinButtonCtrl   m_cDetail;
   double            m_dObjectZoom;
   double            m_dValueZoom;
   int               m_nProjection;
   BOOL              m_bShadingSmooth;
   int               m_nNumColors;
   BOOL              m_bInterpolate;
   BOOL              m_bInterpolateColor;
   CString           m_strZ_Unit;
   double            m_dMaxLevel;
   double            m_dMinLevel;
	//}}AFX_DATA
   int               m_nDetail;


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CViewParams)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	void InitSysColors();

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CViewParams)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnInterpolateColor();
	afx_msg void OnChangeDetail();
	afx_msg void OnKillfocusNumcolors();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKillfocusZValue();
	afx_msg void OnInterpolate();
	afx_msg void OnChangeZoom();
	afx_msg void OnProjection();
	afx_msg void OnChangeMinMax();
   afx_msg void OnApplyNowCmd();
	afx_msg void OnShading();
	afx_msg LRESULT OnColorViewMouseEvent(WPARAM, LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
      CColorView  m_CurrentColor;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_VIEWPARAMS_H__1F2B2A08_170A_11D2_9DB9_0000B458D891__INCLUDED_
