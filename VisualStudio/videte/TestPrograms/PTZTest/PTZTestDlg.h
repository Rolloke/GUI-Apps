// PTZTestDlg.h : header file
//

#if !defined(AFX_PTZTESTDLG_H__F61E555D_56F6_47D9_A21E_0AD195F94106__INCLUDED_)
#define AFX_PTZTESTDLG_H__F61E555D_56F6_47D9_A21E_0AD195F94106__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ControlRecord.h"

/////////////////////////////////////////////////////////////////////////////
// CPTZTestDlg dialog
class CPTZTestDlg : public CDialog
{
// Construction
public:
	CPTZTestDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CPTZTestDlg();

// Implementation
	// Generated message map functions
	//{{AFX_MSG(CPTZTestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnClose();
	afx_msg void OnRadioResponse();
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void	EnableControls();
	void	FillProtocols();

// Dialog Data
protected:
	//{{AFX_DATA(CPTZTestDlg)
	enum { IDD = IDD_PTZTEST_DIALOG };
	CButton	m_groupResponse;
	CButton	m_radioResponseIgnore;
	CButton	m_radioResponseACK;
	CButton	m_radioResponseNAK;
	CButton	m_btnStartListen;
	CButton	m_btnStopListen;
	CComboBox	m_cbProtocol;
	CButton	m_btnClose;
	int		m_iResponse;
	int		m_iProtocolIndex;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPTZTestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

	HICON m_hIcon;
	CControlRecord*	m_pControlRecord;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PTZTESTDLG_H__F61E555D_56F6_47D9_A21E_0AD195F94106__INCLUDED_)
