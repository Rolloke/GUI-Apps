// SystemInfoDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSystemInfoDialog dialog

class CSystemInfoDialog : public CDialog
{
// Construction
public:
	CSystemInfoDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSystemInfoDialog)
	enum { IDD = IDD_SYSTEMINFO };
	CString	m_sVersion;
	CString	m_sMemory;
	CString	m_sProzessor;
	CString	m_sSoftware;
	//}}AFX_DATA

	SYSTEM_INFO		m_SystemInfo;
	OSVERSIONINFO   m_OSVersionInfo;
	MEMORYSTATUS    m_MemoryStatus;
	CString			m_sSoftwareVersion;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSystemInfoDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSystemInfoDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
