// test_openCDDlg.h : header file
//

#if !defined(AFX_TEST_OPENCDDLG_H__17C5BEF7_62EB_11D4_ADD6_004005A1D890__INCLUDED_)
#define AFX_TEST_OPENCDDLG_H__17C5BEF7_62EB_11D4_ADD6_004005A1D890__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CTest_openCDDlg dialog
#include "afxmt.h"
#include "CIPCDrive.h"
#include "wk_trace.h"
#include "Winioctl.h"
class CTest_openCDDlg : public CDialog
{
// Construction
public:
	CTest_openCDDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CTest_openCDDlg)
	enum { IDD = IDD_TEST_OPENCD_DIALOG };
	CString	m_OpenStatus;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTest_openCDDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTest_openCDDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButton1();
	afx_msg void OnButton2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
		
	void LockCD();
	void UnlockCD();
	BOOL GetDriveGeometry(DISK_GEOMETRY *pdg);
	BOOL LockUnlockDrive(DISK_GEOMETRY *pdg, BOOL bLock, CString sDriveLetter);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEST_OPENCDDLG_H__17C5BEF7_62EB_11D4_ADD6_004005A1D890__INCLUDED_)
