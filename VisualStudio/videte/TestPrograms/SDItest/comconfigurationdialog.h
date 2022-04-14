// ComConfigurationDialog.h : header file
//

#if !defined(AFX_COMCONFIGURATIONDIALOG_H__043403C4_38CB_11D2_AFCE_00C095EC2267__INCLUDED_)
#define AFX_COMCONFIGURATIONDIALOG_H__043403C4_38CB_11D2_AFCE_00C095EC2267__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CComParameters;

/////////////////////////////////////////////////////////////////////////////
// CComConfigurationDialog dialog

class CComConfigurationDialog : public CWK_Dialog
{
// Construction
public:
	CComConfigurationDialog(const CString& sCom,
							CComParameters* pComParameters,
							CWnd* pParent = NULL);   // standard constructor

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComConfigurationDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CComConfigurationDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboBaudRate();
	afx_msg void OnSelchangeComboParity();
	afx_msg void OnSelchangeComboDataBits();
	afx_msg void OnSelchangeComboStopBits();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void	FillComboboxBaudRate();
	void	FillComboboxParity();
	void	FillComboboxDataBits();
	void	FillComboboxStopBits();
	void	InitData();

// Dialog Data
protected:
	//{{AFX_DATA(CComConfigurationDialog)
	enum { IDD = IDD_COM_CONFIGURATION };
	CComboBox	m_cbBaudRate;
	CComboBox	m_cbParity;
	CComboBox	m_cbDataBits;
	CComboBox	m_cbStopBits;
	int		m_iIndexBaudRate;
	int		m_iIndexParity;
	int		m_iIndexDataBits;
	int		m_iIndexStopBits;
	//}}AFX_DATA
	CString			m_sCom;
	CComParameters*	m_pComParams;
	BOOL			m_bModified;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMCONFIGURATIONDIALOG_H__043403C4_38CB_11D2_AFCE_00C095EC2267__INCLUDED_)
