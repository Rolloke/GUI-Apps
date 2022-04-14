// EnableAllDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEnableAllDlg dialog

class CEnableAllDlg : public CDialog
{
// Construction
public:
	CEnableAllDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CEnableAllDlg)
	enum { IDD = IDD_ENABLEALL_DIALOG };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEnableAllDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CEnableAllDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnEnableAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void ClimbWindow(CWnd* pWnd);

};
