#ifndef AFX_FITCURVE_H__EA0688E5_A8A6_11D2_9DB9_0000B458D891__INCLUDED_
#define AFX_FITCURVE_H__EA0688E5_A8A6_11D2_9DB9_0000B458D891__INCLUDED_

// FitCurve.h : Header-Datei
//
#include "CurveDialog.h"
#include "EditDoubleCtrl.h"

#define MAX_POLYORDNUNG 11
/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CFitCurve 

class CFitCurve : public CCurveDialog
{
   // Konstruktion
public:
	CFitCurve(CWnd* pParent = NULL);   // Standardkonstruktor
	virtual ~CFitCurve();                       
	void     SelectFunction();
   int      FitPolynome();
   int      FitPotFkt();
   int      FitExpFkt();
	double   CalculateDiff();
   int      Gauss_elimin(int, Complex **, Complex *, Complex *);
	void     GMdKlQuMatrix(int, int, Complex **, Complex **);

	static Complex   PolynomeValue(Complex, Complex*, int);
	static Complex   PotFktValue(  Complex, Complex*, int);
	static Complex   ExpFktValue(  Complex, Complex*, int);
   static Complex **AllocateMatrix(int, int);
	static void     DeleteMatrix(int, Complex**);

   // Statische Funktionen
private:
	Complex  m_dVar[MAX_POLYORDNUNG];
	double   m_dDiff;
	double   m_dFrom;
	double   m_dTo;
	int      m_nValues;
	int      m_nFrom;
	int      m_nTo;
	CString  m_strDiff;
	CString  m_strRange;

// Dialogfelddaten
	//{{AFX_DATA(CFitCurve)
	enum { IDD = IDD_FITCURVE };
	CEditDoubleCtrl   m_cVarList;
	CString           m_strFunction;
	int               m_nCombo;
	int               m_nPolyOrdnung;
	//}}AFX_DATA

	static UINT ThreadFunction(void *);

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CFitCurve)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	virtual void InitCurveParams();

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CFitCurve)
	afx_msg void OnSelchangeCrCombo();
	afx_msg void OnCrCalculate();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnRclickCrVarlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditCrVarlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginlabeleditCrVarlist(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_FITCURVE_H__EA0688E5_A8A6_11D2_9DB9_0000B458D891__INCLUDED_
