// EndcontrolDlg.h : header file
//

#if !defined(AFX_ENDCONTROLDLG_H__08D44057_0786_11D3_8962_004005A1D890__INCLUDED_)
#define AFX_ENDCONTROLDLG_H__08D44057_0786_11D3_8962_004005A1D890__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CEndcontrolDlg dialog

class CEndcontrolDlg : public CDialog
{
// Construction
public:
	CList<int, int>* GetListStatusNotOK();
	CList<int, int>* GetListStatusOK();
	void ShowMainDlg();
	CEndcontrolDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CEndcontrolDlg)
	enum { IDD = IDD_ENDCONTROL_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEndcontrolDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CEndcontrolDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	BOOL m_bVideoSaveOK;
	BOOL m_bLauncherDown;
	int m_nCmdShow;
	CList<int, int> m_ListStatusNotOK;
	CList<int, int> m_ListStatusOK;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ENDCONTROLDLG_H__08D44057_0786_11D3_8962_004005A1D890__INCLUDED_)
