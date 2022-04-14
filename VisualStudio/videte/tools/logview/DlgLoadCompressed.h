#pragma once
//#include <afxcmn.h>


// CDlgLoadCompressed dialog

class CDlgLoadCompressed : public CDialog
{
	DECLARE_DYNAMIC(CDlgLoadCompressed)

public:
	CDlgLoadCompressed(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgLoadCompressed();
// Operations

// Dialog Data
protected:
	enum { IDD = IDD_LOAD_COMPRESSED };
	CListCtrl m_List;
public:
	CStringArray *m_pFiles;
	CString m_sHeadLine;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogViewView)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CLogViewView)
	afx_msg void OnGetDispInfoList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedEditSelectAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};
