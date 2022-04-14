#if !defined(AFX_ACTUALIMAGEPAGE_H__58F70A6C_01FE_11D3_B5E2_004005A19028__INCLUDED_)
#define AFX_ACTUALIMAGEPAGE_H__58F70A6C_01FE_11D3_B5E2_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ActualImagePage.h : header file
//

#include "SVPage.h"
/////////////////////////////////////////////////////////////////////////////
// CActualImagePage dialog

class CActualImagePage : public CSVPage
{
// Construction
public:
	CActualImagePage(CSVView* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CActualImagePage)
	enum { IDD = IDD_PROZ_ACTUAL_IMAGE };
	CComboBox	m_cbCompression;
	CComboBox	m_cbResolution;
	//}}AFX_DATA

// Overrides
public:
	virtual BOOL IsDataValid();
	virtual void SaveChanges();
	virtual void CancelChanges();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CActualImagePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void InitProcess();
	void ProcessToControl(CProcess* pProcess);
	void ControlToProcess(CProcess* pProcess);

	// Generated message map functions
	//{{AFX_MSG(CActualImagePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboRes();
	afx_msg void OnSelchangeComboComp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CProcessList*	m_pProcessList;
	CProcess*		m_pActualImageProcess;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACTUALIMAGEPAGE_H__58F70A6C_01FE_11D3_B5E2_004005A19028__INCLUDED_)
