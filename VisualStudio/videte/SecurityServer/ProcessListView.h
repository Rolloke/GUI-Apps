// ProcessListView.h : header file
//

class CProcess;

// used to inform the mainThread of process changes
// via PostMessage
class CUpdateProcessRecord
{
public:
	CUpdateProcessRecord() 
	{
		m_pProcess = NULL;
		m_bActive = FALSE;
	}
public:
	const CProcess *m_pProcess;
	CString m_sText;
	BOOL m_bActive;
};
WK_PTR_ARRAY_CS(CUpdateProcessRecordsPlain,CUpdateProcessRecord*,CUpdateProcessRecords)

/////////////////////////////////////////////////////////////////////////////
// CProcessListView view

class CProcessListView : public CListView
{
protected:
	CProcessListView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CProcessListView)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProcessListView)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CProcessListView();
#ifdef _DEBUG
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CProcessListView)
	afx_msg LRESULT OnUser(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void InsertUpdateProcessRecord(CUpdateProcessRecord* pRec);

public:
	CUpdateProcessRecords m_UpdateProcessRecords;
};

/////////////////////////////////////////////////////////////////////////////

extern void UpdateProcessListView(BOOL bActive, CProcess *pProcess);
extern void UpdateProcessListView(const CString & sMsg);
