#if !defined(AFX_CCOMMENTDLG_H__2DE1C721_3865_11D3_A9C5_004005A11E32__INCLUDED_)
#define AFX_CCOMMENTDLG_H__2DE1C721_3865_11D3_A9C5_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CCommentDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CComment dialog

class CComment : public CDialog
{
// Construction
public:
	virtual CString GetComment();
	CComment(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CComment)
	enum { IDD = IDD_DIALOG_COMMENT };
	CEdit	m_ctrlComment;
	CString	m_sComment;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComment)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CComment)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CCOMMENTDLG_H__2DE1C721_3865_11D3_A9C5_004005A11E32__INCLUDED_)
