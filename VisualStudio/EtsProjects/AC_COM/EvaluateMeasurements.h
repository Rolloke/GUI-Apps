#if !defined(AFX_EVALUATEMEASUREMENTS_H__D708A721_EC59_11D4_87C5_0000B48B0FC3__INCLUDED_)
#define AFX_EVALUATEMEASUREMENTS_H__D708A721_EC59_11D4_87C5_0000B48B0FC3__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EvaluateMeasurements.h : Header-Datei
//

#define NO_OF_MEASUREMENT_COL 6
/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CEvaluateMeasurements 
class CAC_COMDlg;
class CMeasurement;
class CAudioControlData;

class CEvaluateMeasurements : public CDialog
{
// Konstruktion
public:
	CMeasurement* GetMeasurement(int, POSITION&);
	CEvaluateMeasurements(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CEvaluateMeasurements)
	enum { IDD = IDD_EVAL_MEASUREMENTS };
	CComboBox	m_cShowTestVal;
	CListCtrl	m_cMeasurements;
	int		m_nMeaningOfTestVal;
	BOOL	m_bShowCenter;
	BOOL	m_bShowLeft;
	BOOL	m_bShowRight;
	BOOL	m_bShowAverage;
	//}}AFX_DATA
   CAC_COMDlg       *m_pParent;

	bool IsMeasurementValid(CMeasurement*);
	void SetMeaningOfTestVal(CAudioControlData*);
	void SetMeasurementCount();
	LRESULT DrawSubitemImage(NMLVCUSTOMDRAW *, int);

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CEvaluateMeasurements)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
   CImageList m_ImageList;
	static int m_nColumnWidth[NO_OF_MEASUREMENT_COL];
   static char gm_cDecimal[4];
   static char gm_cList[4];
	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CEvaluateMeasurements)
	afx_msg LRESULT OnHelp(WPARAM, LPARAM);
	virtual BOOL OnInitDialog();
	afx_msg void OnGetdispinfoMeasurementList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg void OnRclickMeasurementList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeAcComboMeaningOfTv();
	afx_msg void OnClickEmMeasurementList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginlabeleditEmMeasurementList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEmBtnShowCurves();
	afx_msg void OnEndlabeleditEmMeasurementList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnItemclickEmMeasurementList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickEmMeasurementList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEmBtnDeleteMeasurement();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

   int m_nItem;
   int m_nSubItem;

   static long             gm_lOldEditCtrlWndProc;
   static LRESULT CALLBACK EditCtrlWndProc(HWND, UINT, WPARAM, LPARAM);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_EVALUATEMEASUREMENTS_H__D708A721_EC59_11D4_87C5_0000B48B0FC3__INCLUDED_
