// ProductKeyDlg.h : header file
//

#if !defined(AFX_PRODUCTKEYDLG_H__95A55B87_0A83_11D2_B533_00C095EC9EFA__INCLUDED_)
#define AFX_PRODUCTKEYDLG_H__95A55B87_0A83_11D2_B533_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CProductKeyDlg dialog

class CProductKeyDlg : public CDialog
{
// Construction
public:
	CProductKeyDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CProductKeyDlg)
	enum { IDD = IDD_PRODUCTKEY_DIALOG };
	CString	m_sSerialNr;
	CString	m_sRSAn;
	CString	m_sRSAe;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProductKeyDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CProductKeyDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnEncode();
	afx_msg void OnDecode();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CRSA	m_RSA;
	CString m_sPath;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRODUCTKEYDLG_H__95A55B87_0A83_11D2_B533_00C095EC9EFA__INCLUDED_)
