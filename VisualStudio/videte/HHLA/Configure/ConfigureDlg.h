// ConfigureDlg.h : header file
//

#if !defined(AFX_CONFIGUREDLG_H__EF9FDD59_C244_11D2_8CCE_004005A11E32__INCLUDED_)
#define AFX_CONFIGUREDLG_H__EF9FDD59_C244_11D2_8CCE_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CConfigureDlg dialog

class CConfigureDlg : public CDialog
{
// Construction
public:
	virtual  ~CConfigureDlg();
	CConfigureDlg(CWnd* pParent = NULL);	// standard constructor

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CConfigureDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnOkComboMico();
	afx_msg void OnOkComboCovi1();
	afx_msg void OnOkComboCovi2();
	afx_msg void OnOkComboCovi3();
	virtual void OnOK();
	afx_msg void OnOkComboMicoIrq();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


// Dialog Data
	//{{AFX_DATA(CConfigureDlg)
	enum { IDD = IDD_CONFIGURE_DIALOG };
	CComboBox	m_comboMiCoIRQ;
	CComboBox	m_comboCoVi3IOBase;
	CComboBox	m_comboCoVi2IOBase;
	CComboBox	m_comboCoVi1IOBase;
	CComboBox	m_comboMiCoIOBase;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfigureDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

private:
	int m_nMiCoIOBase;
	int m_nCoVi1IOBase;
	int m_nCoVi2IOBase;
	int m_nCoVi3IOBase;
	int m_nMiCoIRQ;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGUREDLG_H__EF9FDD59_C244_11D2_8CCE_004005A11E32__INCLUDED_)
