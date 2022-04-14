// ExecuteDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CExecuteDialog dialog

class CExecuteDialog : public CDialog
{
// Construction
public:
	CExecuteDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CExecuteDialog)
	enum { IDD = IDD_EXECUTE };
	CString	m_sExecute;
	//}}AFX_DATA
	int	m_iType;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExecuteDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CExecuteDialog)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
