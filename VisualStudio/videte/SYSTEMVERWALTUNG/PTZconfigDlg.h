#pragma once
#include "afxcmn.h"


// CPTZconfigDlg dialog

class CPTZconfigDlg : public CDialog
{
	DECLARE_DYNAMIC(CPTZconfigDlg)

public:
	CPTZconfigDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPTZconfigDlg();
	CString GetFlagName(DWORD dwFlag, BOOL bExtended);

protected:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComConfigurationDialog)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CComConfigurationDialog)
	afx_msg void OnLvnBeginlabeleditPtzConfigList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndlabeleditPtzConfigList(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dialog Data
public:
	CWK_Profile*m_pWKP;
	CString     m_sSection;
	CameraControlType m_CameraType; 
	int				m_nCom;
private:
	//{{AFX_DATA(CComConfigurationDialog)
	enum { IDD = IDD_PTZ_CONFIG_DLG };
	CListCtrl m_List;
	//}}AFX_DATA
protected:
	virtual void OnOK();
public:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
