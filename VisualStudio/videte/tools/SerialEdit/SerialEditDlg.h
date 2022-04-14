// SerialEditDlg.h : header file
//

#if !defined(AFX_SERIALEDITDLG_H__F78A2548_E41F_11D1_B8CD_0060977A76F1__INCLUDED_)
#define AFX_SERIALEDITDLG_H__F78A2548_E41F_11D1_B8CD_0060977A76F1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CSerialEditDlg dialog

class CSerialEditDlg : public CDialog
{
// Construction
public:
	CSerialEditDlg(CWnd* pParent = NULL);	// standard constructor

	void WriteToFile();
// Dialog Data
	//{{AFX_DATA(CSerialEditDlg)
	enum { IDD = IDD_SERIALEDIT_DIALOG };
	CString	m_sSerialText;
	CString	m_sFullPath;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSerialEditDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CSerialEditDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void TryDirectory(const CString &sDir);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERIALEDITDLG_H__F78A2548_E41F_11D1_B8CD_0060977A76F1__INCLUDED_)
