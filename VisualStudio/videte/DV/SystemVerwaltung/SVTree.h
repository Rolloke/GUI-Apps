// SVTree.h : header file
//

class CSVView;

/////////////////////////////////////////////////////////////////////////////
// CSVTree window

class CSVTree : public CTreeCtrl
{
// Construction
public:
	CSVTree(CSVView* pParent);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSVTree)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSVTree();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSVTree)
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	CSVView* m_pParent;
};

/////////////////////////////////////////////////////////////////////////////
