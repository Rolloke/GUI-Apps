// SystemDialog.h : header file
//

#define WTD_SYS_DOWN              1
#define WTD_SYS_REBOOT            2
#define WTD_WIN_EXPLORER          3
#define WTD_WIN_LAUNCHER		  4
#define WTD_EXPLORER              5
#define WTD_DBBACKUP              6

/////////////////////////////////////////////////////////////////////////////
// CSystemDialog dialog

class CSystemDialog : public CDialog
{
// Construction
public:
	CSystemDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSystemDialog)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	enum { IDD = IDD_SYSTEM };

	int	m_iWhatToDo;
	BOOL m_bShell;
	BOOL m_bBackup;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSystemDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSystemDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
