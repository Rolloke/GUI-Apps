// PINReaderDlg.h : header file
//

#if !defined(AFX_PINREADERDLG_H__72384D28_70F5_4B1B_AC3A_3A3C08A51F85__INCLUDED_)
#define AFX_PINREADERDLG_H__72384D28_70F5_4B1B_AC3A_3A3C08A51F85__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CPINReaderDlg dialog

class CPINReaderDlg : public CDialog
{
// Construction
public:
	CPINReaderDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CPINReaderDlg)
	enum { IDD = IDD_PINREADER_DIALOG };
	CString	m_sOPIN;
	CString	m_sSPIN;
	CString	m_sSerial;
	CString	m_sStaticSerial;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPINReaderDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

	CString GetKey(CString KeyAdress, CString KeyName, DWORD Regtype);
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CPINReaderDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PINREADERDLG_H__72384D28_70F5_4B1B_AC3A_3A3C08A51F85__INCLUDED_)
