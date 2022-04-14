// ChangeOEMDlg.h : header file
//

#if !defined(AFX_CHANGEOEMDLG_H__0BFB6917_012F_11D3_895D_004005A1D890__INCLUDED_)
#define AFX_CHANGEOEMDLG_H__0BFB6917_012F_11D3_895D_004005A1D890__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CChangeOEMDlg dialog

class CChangeOEMDlg : public CDialog
{
// Construction
public:
	CChangeOEMApp* m_pOEMApp;
	BOOL m_bIsWin98;
	BOOL m_bIsWin95;
	BOOL m_bIsNT;
	CChangeOEMDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CChangeOEMDlg)
	enum { IDD = IDD_CHANGEOEM_DIALOG };
	CEdit	m_editRegOrga;
	CEdit	m_editCOMPUTERNAME;
	CButton	m_editOK;
	CStatic	m_staticWrongOS;
	CStatic	m_staticW98;
	CStatic	m_staticWNT;
	CStatic	m_staticW95;
	CEdit	m_editOEM3;
	CEdit	m_editOEM2;
	CEdit	m_editOEM1;
	CString	m_sOEM1;
	CString	m_sOEM2;
	CString	m_sOEM3;
	CString	m_sCOMPUTERNAME;
	CString	m_sRegOrga;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChangeOEMDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CChangeOEMDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnMaxtextOem1();
	afx_msg void OnMaxtextOem2();
	afx_msg void OnMaxtextOem3();
	afx_msg void OnSetfocusOem2();
	afx_msg void OnSetfocusOem3();
	afx_msg void OnKillfocusOem1();
	afx_msg void OnKillfocusOem2();
	afx_msg void OnKillfocusOem3();
	virtual void OnOK();
	afx_msg void OnMaxtextComputername();
	afx_msg void OnSetfocusComputername();
	afx_msg void OnSetfocusRegOrga();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CString m_sCheckCompNameFailed;
	BOOL m_bCheckCompName;
	CString m_sCompName;
	CString m_sOEMNr;
	CString m_sOEM;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHANGEOEMDLG_H__0BFB6917_012F_11D3_895D_004005A1D890__INCLUDED_)
