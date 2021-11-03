// FacturaView.h : Schnittstelle der Klasse CFacturaView
//
/////////////////////////////////////////////////////////////////////////////
#include "CaraView.h"

#if !defined(AFX_FACTURAVIEW_H__FA828EEC_A94A_11D2_9DB9_0000B458D891__INCLUDED_)
#define AFX_FACTURAVIEW_H__FA828EEC_A94A_11D2_9DB9_0000B458D891__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define FORM_SIGN        "§"
#define FORMAT_TEXT_SIGN "$"
class CFacturaDoc;

class CFacturaView : public CCaraView
{
protected: // Nur aus Serialisierung erzeugen
	CFacturaView();
	DECLARE_DYNCREATE(CFacturaView)

// Attribute
public:
	CFacturaDoc* GetDocument();

// Operationen
public:

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CFacturaView)
	public:
	virtual void OnDraw(CDC* pDC);  // überladen zum Zeichnen dieser Ansicht
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~CFacturaView();
	afx_msg void OnExtraPrintForm();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	static int InsertDummyTextLabels(CLabel*, void*);
	static int RemoveDummyTextLabels(CLabel*, void*);
	static int FillDummyTextLabels(  CLabel*, void*);
	static int InsertVisibleLabels(  CLabel*, void*);

// Generierte Message-Map-Funktionen
protected:
	//{{AFX_MSG(CFacturaView)
	afx_msg void OnEditNewcustomer();
	afx_msg void OnEditNewinvoice();
	afx_msg void OnEditNewproducts();
	afx_msg void OnExtraPrintAdr();
	afx_msg void OnExtraPrintBank();
	afx_msg void OnEditStates();
	afx_msg void OnFilePrintLogfile();
	afx_msg void OnExtraEditForm();
	afx_msg void OnEditPaymodes();
	afx_msg void OnEditCurrencyunits();
	afx_msg void OnEditBaseData();
	afx_msg void OnExtraPrintFormDirect();
	afx_msg void OnExtraPrintStandardLetter();
	afx_msg void OnDataStatistic();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnExtraPrintProducts();
	afx_msg void OnExtraSendMail();
	//}}AFX_MSG
public:
   bool    m_bPrintDirect;
	int     m_nPrepareFormLabels;

protected:
   CString  m_strFormLabelName;
	int      m_nPrintForm;
   DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // Testversion in FacturaView.cpp
inline CFacturaDoc* CFacturaView::GetDocument()
   { return (CFacturaDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // !defined(AFX_FACTURAVIEW_H__FA828EEC_A94A_11D2_9DB9_0000B458D891__INCLUDED_)
