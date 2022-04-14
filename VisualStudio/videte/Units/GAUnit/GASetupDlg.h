// GAUnitSetupDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGASetupDlg dialog

class CGASetupDlg : public CDialog
{
// Construction
public:
	CGASetupDlg(CWnd* pParent = NULL);	// standard constructor

public:
	UINT	m_CommNumber;
// Dialog Data
	//{{AFX_DATA(CGASetupDlg)
	enum { IDD = IDD_SETUP };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGASetupDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGASetupDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
