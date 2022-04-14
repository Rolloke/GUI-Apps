#if !defined(AFX_CPASSWORDCHECKDIALOG__57CDD643_260F_11D2_B8FB_00C095ECA33E__INCLUDED_)
#define AFX_CPASSWORDCHECKDIALOG__57CDD643_260F_11D2_B8FB_00C095ECA33E__INCLUDED_
// PasswordCheckDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPasswordCheckDialog dialog

class CPasswordCheckDialog : public CWK_Dialog
{
// Construction
public:
	CPasswordCheckDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPasswordCheckDialog)
	enum { IDD = IDD_PASSWORDCHECK };
	CString	m_Password;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPasswordCheckDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPasswordCheckDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
#endif // AFX_CPASSWORDCHECKDIALOG__57CDD643_260F_11D2_B8FB_00C095ECA33E__INCLUDED_