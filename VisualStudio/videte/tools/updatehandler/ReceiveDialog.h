// ReceiveDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CReceiveDialog dialog

class CReceiveDialog : public CDialog
{
// Construction
public:
	CReceiveDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CReceiveDialog)
	enum { IDD = IDD_RECEIVE };
	CString	m_SourceFileName;
	CString	m_Destination;
	//}}AFX_DATA
	int	m_iType;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReceiveDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CReceiveDialog)
	afx_msg void OnSave();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
