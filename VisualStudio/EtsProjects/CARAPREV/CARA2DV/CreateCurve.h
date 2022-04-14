#ifndef AFX_CREATECURVE_H__EA0688E4_A8A6_11D2_9DB9_0000B458D891__INCLUDED_
#define AFX_CREATECURVE_H__EA0688E4_A8A6_11D2_9DB9_0000B458D891__INCLUDED_

// CreateCurve.h : Header-Datei
//
#include "CurveDialog.h"
#include "ArithmaEdit.h"
#include "EditDoubleCtrl.h"
#include "Aritmetic.h"
/////////////////////////////////////////////////////////////////////////////

class CLabelContainer;
// Dialogfeld CCreateCurve 
class CCreateCurve : public CCurveDialog
{
   struct sRunCurve
   {
      CCurveLabel *pl;
      Complex     *pdRunX;
      Complex     *pdRunY;
      Complex     *pdRunC;
   };
// Konstruktion
public:
	CCreateCurve(CWnd* pParent = NULL);   // Standardkonstruktor
	virtual ~CCreateCurve();

   // Statische Funktionen
private:
	static UINT ThreadFunction(void*);
   static void DeleteParam(void *);
	static int SetFcToTree(CTreeDlg*, int);

	void OptimizeFormat(CCurveLabel*, CCurveLabel*);
   LRESULT OnUpdateError(WPARAM, LPARAM);
	bool CheckCurveList();
	void ReportError(int, bool bUpdate=false);
	bool SetCurvesToList();
	void SetVariablesToList();

// Dialogfelddaten
	//{{AFX_DATA(CCreateCurve)
	enum { IDD = IDD_CREATECURVE };
	CArithmaEdit m_Function;
	CEditDoubleCtrl m_cVarList;
	CListCtrl   m_cCurveList;
	CString	   m_strError;
	double	   m_dFrom;
	double	   m_dTo;
	CString	   m_strFunction;
	int		   m_nNoOfValues;
	CString	   m_strRunVar;
	BOOL	      m_bLogarithmic;
	int		   m_nResultType;
	CString     m_strResultUnit;
	CString     m_strRunVarUnit;
	//}}AFX_DATA

   CArithmetic m_Equation;
   int         m_nRunVar;
   bool        m_bCurves;
   bool        m_bCurveList;
   CCurveLabel *m_p2ndCurve;
   CPtrList    m_RunList;
	CString	   m_strRunVarText;

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CCreateCurve)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CCreateCurve)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnEndlabeleditFcVarlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeFcFunction();
	afx_msg void OnDblclkFcVarlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdateCtrl();
	afx_msg void OnBtnClickedLog();
	afx_msg void OnChangeParameter();
	afx_msg void OnRclickFcVarlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginlabeleditFcVarlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnFcTest();
	afx_msg void OnDblclkFcCurvelist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetfocusFcCurvelist(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnSetFocus(CWnd* pOldWnd);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_CREATECURVE_H__EA0688E4_A8A6_11D2_9DB9_0000B458D891__INCLUDED_
