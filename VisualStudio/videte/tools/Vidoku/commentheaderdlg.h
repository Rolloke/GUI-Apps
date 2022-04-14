#if !defined(AFX_COMMENTHEADERDLG_H__68F37033_8CB5_437C_8B2A_4C584F936F5F__INCLUDED_)
#define AFX_COMMENTHEADERDLG_H__68F37033_8CB5_437C_8B2A_4C584F936F5F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CommentHeaderDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCommentHeaderDlg dialog

class CCommentHeaderDlg : public CDialog
{
// Construction
public:
	CCommentHeaderDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCommentHeaderDlg)
	enum { IDD = IDD_COMMENT };
	CComboBox	m_cGroup;
	CComboBox	m_cAuthors;
	CString	m_strClass;
	CString	m_strFunction;
	CString	m_strResultType;
	CListCtrl	m_cParams;
	CString	m_strFunctionDscr;
	CString	m_strResultTypeDscr;
	BOOL	m_bGerman;
	CString	m_strAuthor;
	BOOL	m_bInsertDate;
	BOOL	m_bTitle;
	BOOL	m_bAutoLinks;
	BOOL	m_bSourceCode;
	CString	m_strKeyWords;
	CString	m_strTopicOrder;
	CString	m_strGroup;
	//}}AFX_DATA
   CString m_strText;
	CString m_strOldComment;
   int m_nParamLength;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCommentHeaderDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
// Implementation
protected:
   int m_nSelection;
	// Generated message map functions
	//{{AFX_MSG(CCommentHeaderDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickParamList(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	afx_msg void OnEndlabeleditParamList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusComboAuthors();
	afx_msg void OnBtnSaveSettings();
	afx_msg void OnEditchangeComboAuthors();
	afx_msg void OnSelchangeComboAuthors();
	afx_msg void OnBtnDelAuthor();
	afx_msg void OnCkGerman();
	afx_msg void OnDblclkParamList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCkInsertDate();
	afx_msg void OnCkInsertTitle();
	afx_msg void OnKillfocusComboGroup();
	afx_msg void OnSelchangeComboGroup();
	afx_msg void OnEditchangeComboGroup();
	afx_msg void OnBtnDelGroup();
	afx_msg void OnBeginlabeleditParamList(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg LRESULT OnEditCtrlKeyUp(WPARAM, LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
/*
	afx_msg void OnKillfocusEdtClass();
	afx_msg void OnKillfocusEdtFunction();
	afx_msg void OnKillfocusEdtFunctionDescription();
	afx_msg void OnKillfocusEdtResultType();
	afx_msg void OnKillfocusEdtResultTypeDescription();
*/
private:
	int  m_nCurrAuthor;
	bool m_bAuthorEdited;
	int  m_nCurrGroup;
	bool m_bGroupEdited;
   CToolTipCtrl m_ToolTip;
   CStringArray m_sTTArray;

   static LRESULT CALLBACK EditCtrlWndProc(HWND, UINT, WPARAM, LPARAM);
   void InsertToList(CString &strParam, CString &strType, CString &strComment, int&);
   void InitToolTips();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMMENTHEADERDLG_H__68F37033_8CB5_437C_8B2A_4C584F936F5F__INCLUDED_)
