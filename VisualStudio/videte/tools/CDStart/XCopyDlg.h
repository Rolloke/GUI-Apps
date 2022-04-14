#if !defined(AFX_XCOPYDLG_H__E3923D06_E5D4_11D3_BAB4_00400531137E__INCLUDED_)
#define AFX_XCOPYDLG_H__E3923D06_E5D4_11D3_BAB4_00400531137E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// XCopyDlg.h : header file
//

#include "TextProgressCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CXCopyDlg dialog
class CXCopyDlg : public CDialog
{
// Construction
public:
	CXCopyDlg(CString sSource,
				CString sDestination,
				BOOL bRecursiv,
				BOOL bEnableCancel,
				CWnd* pParent = NULL);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXCopyDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void		Run();
	void		Initializing();
	void		CountTotalBytesToCopy();
	void		CopyAllFiles();
	void		CopyFile(CString sSource, CString sDest);
	void		UpdateDlg();
	LPCTSTR	GetFileException(int iCause);
	// Generated message map functions
	//{{AFX_MSG(CXCopyDlg)
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	afx_msg LRESULT OnInitDlgReady(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
private:
// Dialog Data
protected:
	//{{AFX_DATA(CXCopyDlg)
	enum { IDD = IDD_COPY_FILES };
	CAnimateCtrl	m_Animation;
	CStatic	m_txtTo;
	CButton	m_btnCancel;
	CTextProgressCtrl	m_progressFile;
	CTextProgressCtrl	m_progressTotal;
	CStatic	m_txtTotal;
	CStatic	m_txtDestinationFile;
	CStatic	m_txtSourceFile;
	CString	m_sSourceFile;
	CString	m_sDestinationFile;
	CString	m_sTo;
	//}}AFX_DATA
	CString			m_sSource;
	CString			m_sSourcePath;
	CString			m_sDestination;
	CString			m_sDestinationPath;
	BOOL			m_bRecursiv;
	BOOL			m_bEnableCancel;
	BOOL			m_bIsCanceled;
	CStringArray	m_arrayFiles;
	ULARGE_INTEGER	m_liTotalBytes;
	ULARGE_INTEGER	m_liTotalBytesCopied;
	DWORD			m_dwFileBytes;
	DWORD			m_dwFileBytesCopied;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XCOPYDLG_H__E3923D06_E5D4_11D3_BAB4_00400531137E__INCLUDED_)
