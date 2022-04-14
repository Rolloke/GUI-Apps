// CreateAviDlg.h : header file
//

#if !defined(AFX_CREATEAVIDLG_H__5CDA0C17_6E8D_11D4_BAE7_00400531137E__INCLUDED_)
#define AFX_CREATEAVIDLG_H__5CDA0C17_6E8D_11D4_BAE7_00400531137E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CCreateAviDlg dialog

class CCreateAviDlg : public CDialog
{
// Construction
public:
	CCreateAviDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CCreateAviDlg();

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCreateAviDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCreateAviDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnAdd();
	afx_msg void OnBtnDelete();
	afx_msg void OnBtnDown();
	afx_msg void OnBtnSave();
	afx_msg void OnBtnSort();
	afx_msg void OnBtnUp();
	afx_msg void OnChangeEditFilename();
	afx_msg void OnSelchangeListBitmaps();
	afx_msg void OnBtnClear();
	afx_msg void OnChangeEditFps();
	afx_msg void OnBtnPlay();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void	EnableControls();
	void	MoveBitmap(BOOL bUp);
	BOOL	SortList();

// Dialog Data
protected:
	//{{AFX_DATA(CCreateAviDlg)
	enum { IDD = IDD_CREATEAVI_DIALOG };
	CButton	m_btnPlay;
	CButton	m_btnClear;
	CListBox	m_listBitmaps;
	CButton	m_btnUp;
	CButton	m_btnSort;
	CButton	m_btnSave;
	CButton	m_btnDown;
	CButton	m_btnDelete;
	CButton	m_btnAdd;
	CString	m_sFilename;
	int		m_iIndex;
	int		m_iFps;
	//}}AFX_DATA

	HICON		m_hIcon;
	HBITMAP		m_hUp;
	HBITMAP		m_hDown;
	CString		m_sCurrentBitmap;
	CString		m_sAviFilePath;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CREATEAVIDLG_H__5CDA0C17_6E8D_11D4_BAE7_00400531137E__INCLUDED_)
