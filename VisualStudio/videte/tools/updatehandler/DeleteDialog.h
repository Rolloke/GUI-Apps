// DeleteDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDeleteDialog dialog

class CDeleteDialog : public CDialog
{
// Construction
public:
	CDeleteDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDeleteDialog)
	enum { IDD = IDD_DELETE };
	CString	m_sDelete;
	//}}AFX_DATA
	int	m_iType;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeleteDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDeleteDialog)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
