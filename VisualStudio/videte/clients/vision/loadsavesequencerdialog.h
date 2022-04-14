// LoadSaveSequencerDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLoadSaveSequencerDialog dialog

class CLoadSaveSequencerDialog : public CDialog
{
// Construction
public:
	CLoadSaveSequencerDialog(BOOL bSave,const CString& sHost, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLoadSaveSequencerDialog)
	enum { IDD = IDD_SAVE_SEQUENZER };
	CListBox	m_List;
	CButton	m_OKButton;
	CString	m_SequenceName;
	//}}AFX_DATA


public:
	CString GetSequenceName();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLoadSaveSequencerDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void LoadSequenceNames();

	// Generated message map functions
	//{{AFX_MSG(CLoadSaveSequencerDialog)
	afx_msg void OnDblclkListSequence();
	afx_msg void OnSelchangeListSequence();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDelete();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL	m_bSave;
	CString m_sHost;
};
