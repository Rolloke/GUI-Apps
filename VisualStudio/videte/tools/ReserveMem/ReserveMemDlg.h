// ReserveMemDlg.h : header file
//

#if !defined(AFX_RESERVEMEMDLG_H__AF7596E2_6118_4531_B615_86EE30A256D4__INCLUDED_)
#define AFX_RESERVEMEMDLG_H__AF7596E2_6118_4531_B615_86EE30A256D4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CReserveMemDlg dialog

class CReserveMemDlg : public CDialog
{
// Construction
public:
	CReserveMemDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CReserveMemDlg)
	enum { IDD = IDD_RESERVEMEM_DIALOG };
	UINT	m_dwLen;
	CString	m_sFilename;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReserveMemDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CReserveMemDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnReserve();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RESERVEMEMDLG_H__AF7596E2_6118_4531_B615_86EE30A256D4__INCLUDED_)
