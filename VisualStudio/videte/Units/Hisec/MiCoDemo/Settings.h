// Settings.h : header file
//
class CMiCo;
class CMegra;

/////////////////////////////////////////////////////////////////////////////
// CSettings dialog

class CSettings : public CDialog
{
// Construction
public:
	CSettings(CMiCo* pMiCo, CWnd* pParent = NULL);
	CSettings::~CSettings();
// Dialog Data
	//{{AFX_DATA(CSettings)
	enum { IDD = IDD_SETTINGS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSettings)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void Dump( CDumpContext &dc ) const;

	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSettings)
	virtual BOOL OnInitDialog();
	afx_msg void OnLuminance();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnColor();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CMiCo			*m_pMiCo;
	static  BOOL	 m_bOK;
};
