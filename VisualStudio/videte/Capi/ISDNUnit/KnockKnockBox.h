#ifndef __CKNOCKKNOCKBOX_H_INCLUDED
#define __CKNOCKKNOCKBOX_H_INCLUDED
// KnockKnockBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CKnockKnockBox dialog

class CISDNConnection;

class CKnockKnockBox : public CDialog
{
// Construction
public:
	CKnockKnockBox(CISDNConnection *pConnection, CWnd* pParent = NULL);   // standard constructor
	void SetCallInfo(const CAbstractCapiCallInfo &callInfo, BOOL bCanClose);

// Dialog Data
	//{{AFX_DATA(CKnockKnockBox)
	enum { IDD = IDD_KNOCK_KNOCK_BOX };
	CString	m_sName;
	CString	m_sNumber;
	CString	m_sTime;
	CString	m_sCurrentConnection;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKnockKnockBox)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CKnockKnockBox)
	afx_msg void OnCloseActiveConnection();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnBnClickedCloseOneChannel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CISDNConnection *m_pConnection;
};


#endif // __CKNOCKKNOCKBOX_H_INCLUDED