#if !defined(AFX_YUTAPAGE_H__4D248FE5_8C0E_4806_9E44_5C0C995D3E12__INCLUDED_)
#define AFX_YUTAPAGE_H__4D248FE5_8C0E_4806_9E44_5C0C995D3E12__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// YutaPage.h : header file
//
#include "SVPage.h"
/////////////////////////////////////////////////////////////////////////////
// CYutaPage dialog

#define NO_OF_YUTA_UNITS 8

class CYutaPage : public CSVPage
{
	DECLARE_DYNAMIC(CYutaPage)

// Construction
public:
	CYutaPage(CSVView* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CYutaPage)
	enum { IDD = IDD_YUTA };
	CButton	m_cUtaUnitOne;
	//}}AFX_DATA



// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CYutaPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	virtual void SaveChanges();
	virtual void CancelChanges();
	virtual void Initialize();
	virtual BOOL IsDataValid();

	BOOL CheckActivations(int nIndex, bool bDelete);

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CYutaPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	afx_msg void OnCkYutaUnit(UINT nID);
	DECLARE_MESSAGE_MAP()
protected:
	BOOL	m_bYutaUnit[NO_OF_YUTA_UNITS];
	bool	m_bDeleteActivations[NO_OF_YUTA_UNITS];
	CInputList  *m_pInputs;
	COutputList *m_pOutputs;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_YUTAPAGE_H__4D248FE5_8C0E_4806_9E44_5C0C995D3E12__INCLUDED_)
