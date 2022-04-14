// SendDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSendDialog dialog

class CSendDialog : public CDialog
{
// Construction
public:
	CSendDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSendDialog)
	enum { IDD = IDD_SEND };
	CString	m_DestinationFileName;
	CString	m_Source;
	BOOL	m_bReboot;
	//}}AFX_DATA
	int	m_iType;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSendDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnOpen();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
