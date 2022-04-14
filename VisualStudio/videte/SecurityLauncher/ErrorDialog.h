// ErrorDialog.h : header file
//
class CWK_RunTimeErrors;
class CWK_RunTimeError;
/////////////////////////////////////////////////////////////////////////////
// CErrorDialog dialog

class CErrorDialog : public CSkinDialog
{
// Construction
public:
	CErrorDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CErrorDialog)
	enum eID{ IDD = IDD_ERROR };
	CSkinButton	m_btnDelete;
	CSkinListCtrlX	m_lErrors;
	//}}AFX_DATA

public:
	// Overrides
	virtual BOOL StretchElements();
public:
	void	InsertErrors(CWK_RunTimeErrors* pErrors);
	void	InsertError(CWK_RunTimeError* pError);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CErrorDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CErrorDialog)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnDelete();
	afx_msg void OnStmoff();
	afx_msg void OnDblclkErrors(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnStmon();
	afx_msg void OnItemchangedErrors(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CImageList	m_Images;
};
