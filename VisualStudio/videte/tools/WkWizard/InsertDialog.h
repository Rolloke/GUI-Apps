// InsertDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInsertDialog dialog

class CInsertDialog : public CDialog
{
// Construction
public:
	CInsertDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInsertDialog)
	enum { IDD = IDD_DIALOG_INSERT };
	CString	m_Dest;
	CString	m_Lang;
	CString	m_Orig;
	//}}AFX_DATA
	CString m_Dir;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInsertDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInsertDialog)
	afx_msg void OnButtonBrowse();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
