// EULADlg.h : header file
//

#if !defined(AFX_EULADLG_H__CD0CAF29_0C8E_11D5_99B0_004005A19028__INCLUDED_)
#define AFX_EULADLG_H__CD0CAF29_0C8E_11D5_99B0_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define LINES_VISIBLE	 17

/////////////////////////////////////////////////////////////////////////////
// CEULADlg dialog

class CEULADlg : public CDialog
{
// Construction
public:
	CEULADlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CEULADlg)
	enum { IDD = IDD_EULA_DIALOG };
	CButton	m_ctrlCancel;
	CButton	m_ctrlOK;
	CStatic	m_ctrlTextTopLicense;
	CStatic	m_ctrlTextBottomLicense;
	CRichEditCtrl	m_EULA;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEULADlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

	static DWORD CALLBACK EditStreamCallback( DWORD dwCookie, // application-defined value
											  LPBYTE pbBuff,      // data buffer
											  LONG cb,            // number of bytes to read or write
											  LONG *pcb           // number of bytes transferred
											);

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CEULADlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnUpdateRicheditEula();
	virtual void OnOK();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void Init();

public:
	CFile m_EULAFile;
private:
	BOOL m_bLineFeedRichEdit;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EULADLG_H__CD0CAF29_0C8E_11D5_99B0_004005A19028__INCLUDED_)
