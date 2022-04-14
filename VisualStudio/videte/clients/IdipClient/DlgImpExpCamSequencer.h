// DlgImpExpCamSequencer.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgImpExpCamSequencer dialog

class CDlgImpExpCamSequencer : public CSkinDialog
{
// Construction
public:
	CDlgImpExpCamSequencer(BOOL bSave, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgImpExpCamSequencer)
	enum  { IDD = IDD_SAVE_SEQUENZER };
	CSkinListBox	m_List;
	CSkinButton	m_OKButton;
	CString	m_SequenceName;
	//}}AFX_DATA


	CString m_sHost;
public:
	CString GetSequenceName();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgImpExpCamSequencer)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void LoadSequenceNames();

	// Generated message map functions
	//{{AFX_MSG(CDlgImpExpCamSequencer)
	afx_msg void OnDblclkListSequence();
	afx_msg void OnSelchangeListSequence();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDelete();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL	m_bSave;
};
