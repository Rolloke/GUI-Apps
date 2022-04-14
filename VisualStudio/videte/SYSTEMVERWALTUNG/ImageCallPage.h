#if !defined(AFX_IMAGECALLPAGE_H__58F70A6B_01FE_11D3_B5E2_004005A19028__INCLUDED_)
#define AFX_IMAGECALLPAGE_H__58F70A6B_01FE_11D3_B5E2_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ImageCallPage.h : header file
//

#include "SVPage.h"

class CPermissionArray;
/////////////////////////////////////////////////////////////////////////////
// CImageCallPage dialog

class CImageCallPage : public CSVPage
{
// Construction
public:
	CImageCallPage(CSVView* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CImageCallPage)
	enum { IDD = IDD_PROZ_IMAGE_CALL };
	CComboBox	m_cbPermission;
	//}}AFX_DATA

// Overrides
public:
	virtual BOOL IsDataValid();
	virtual void SaveChanges();
	virtual void CancelChanges();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImageCallPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void InitProcess();
	void ProcessToControl(CProcess* pProcess);
	void ControlToProcess(CProcess* pProcess);
	void FillPermissions();

	// Generated message map functions
	//{{AFX_MSG(CImageCallPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboPermission();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CProcessList*	m_pProcessList;
	CProcess*		m_pImageCallProcess;
	CPermissionArray*	m_pPermissionArray;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMAGECALLPAGE_H__58F70A6B_01FE_11D3_B5E2_004005A19028__INCLUDED_)
