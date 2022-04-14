// FileOpenDlg.h : header file
//

#if !defined(AFX_FILEOPENDLG_H__CF8B2FA8_C8BB_11D1_B8CD_0060977A76F1__INCLUDED_)
#define AFX_FILEOPENDLG_H__CF8B2FA8_C8BB_11D1_B8CD_0060977A76F1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CFileOpenDlg dialog

class CFileOpenDlg : public CDialog
{
// Construction
public:
	CFileOpenDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CFileOpenDlg)
	enum { IDD = IDD_FILEOPEN_DIALOG };
	CString	m_sFileName;
	BOOL	m_bReadOnly;
	BOOL	m_bReadWrite;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileOpenDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CFileOpenDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILEOPENDLG_H__CF8B2FA8_C8BB_11D1_B8CD_0060977A76F1__INCLUDED_)
