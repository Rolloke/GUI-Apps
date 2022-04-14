#if !defined(AFX_CDlgComment_H__5AFF7523_0F73_11D4_BAC8_00400531137E__INCLUDED_)
#define AFX_CDlgComment_H__5AFF7523_0F73_11D4_BAC8_00400531137E__INCLUDED_
/////////////////////////////////////////////////////////////////////////////
// CDlgComment dialog

class CDlgComment : public CSkinDialog
{
// Construction
public:
	CDlgComment(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgComment)
	enum  { IDD = IDD_COMMENT };
	CString	m_sComment;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgComment)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgComment)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif // AFX_CDlgComment_H__5AFF7523_0F73_11D4_BAC8_00400531137E__INCLUDED_