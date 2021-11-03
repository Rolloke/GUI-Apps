#ifndef AFX_CURVEDIALOG_H__3796B0C0_BB86_11D2_8859_9C2D38A40003__INCLUDED_
#define AFX_CURVEDIALOG_H__3796B0C0_BB86_11D2_8859_9C2D38A40003__INCLUDED_

// CurveDialog.h : Header-Datei
//
#include "..\CurveLabel.h"
#include "..\PlotLabel.h"
/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CCurveDialog 
// wParam
#define NEWCURVE_IS_NOTOK      0
#define NEWCURVE_IS_OK         1
// lParam
#define DELETE_TEMPCURVE       0x01
#define TERMINATE_CALCULATION  0x02
#define DESTROY_WINDOW         0x04

class CCurveDialog : public CDialog
{
// Konstruktion
public:
	CCurveDialog(UINT nID, CWnd* pParent);   // Standardkonstruktor
	virtual ~CCurveDialog();
	bool SetCurve(CCurveLabel *pcl);
	bool IsActive();
	void DeleteCurves();

// Dialogfelddaten
	//{{AFX_DATA(CCurveDialog)
	CString	m_strNewName;
	//}}AFX_DATA
protected:
	CCurveLabel *m_pNewCurve;
	CCurveLabel *m_pCurve;
   CWinThread  *m_pThread;
   BSPlotDefaults m_PlotSettings;
   double*      m_pdMarker;
   void        *m_pOldCurvePointer;
   bool         m_bNoSetCurve;
   bool         m_bAvoidRecursion;

public:
   CLabelContainer *m_pCurves;
// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CCurveDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementierung
protected:
   virtual void InitCurveParams() {};
	CWnd* GetDlgItem( int );

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CCurveDialog)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg LRESULT OnHelp(WPARAM, LPARAM);
   afx_msg LRESULT OnEndThread(WPARAM, LPARAM);
	afx_msg void OnHide();
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void OnCalculate();
	afx_msg void OnStop();
	//}}AFX_MSG
	afx_msg LRESULT OnSysCommand(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_CURVEDIALOG_H__3796B0C0_BB86_11D2_8859_9C2D38A40003__INCLUDED_
