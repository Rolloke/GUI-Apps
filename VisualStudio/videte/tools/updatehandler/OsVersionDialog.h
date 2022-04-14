// OsVersionDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COsVersionDialog dialog

class COsVersionDialog : public CDialog
{
// Construction
public:
	COsVersionDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COsVersionDialog)
	enum { IDD = IDD_OSVERSION };
	CString	m_Build;
	CString	m_Major;
	CString	m_Minor;
	CString	m_Version;
	CString	m_Platform;
	//}}AFX_DATA

	OSVERSIONINFO   m_OSVersionInfo;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COsVersionDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COsVersionDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
