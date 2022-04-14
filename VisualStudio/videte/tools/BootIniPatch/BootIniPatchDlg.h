// BootIniPatchDlg.h : header file
//

#if !defined(AFX_BOOTINIPATCHDLG_H__B4E60745_782E_431F_B850_569C1663C404__INCLUDED_)
#define AFX_BOOTINIPATCHDLG_H__B4E60745_782E_431F_B850_569C1663C404__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CBootIniPatchDlg dialog

class CBootIniPatchDlg : public CDialog
{
// Construction
public:
	CBootIniPatchDlg(CWnd* pParent = NULL);	// standard constructor

// Implementation
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBootIniPatchDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	// Generated message map functions
	//{{AFX_MSG(CBootIniPatchDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	afx_msg LRESULT OnUser(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()

private:
	DWORD	SearchBootIniFiles();
	BOOL	DoesFileExist(LPCTSTR szFileOrDirectory);
	void	PatchBootIniFile(CString sFile);
	void	CopySystemFiles(CString sDrive);
	void	OnWarning(CString& sText);
	void	OnError(CString& sText);
	void	PrintOutput(CString& sText);
	void	ClearData();

private:
	// Dialog Data
	//{{AFX_DATA(CBootIniPatchDlg)
	enum { IDD = IDD_BOOTINIPATCH_DIALOG };
	CListBox	m_lbTextOutput;
	//}}AFX_DATA

	HICON			m_hIcon;
	CStringArray	m_saBootIniFiles;
	UINT			m_uErrors;
	UINT			m_uWarnings;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BOOTINIPATCHDLG_H__B4E60745_782E_431F_B850_569C1663C404__INCLUDED_)
