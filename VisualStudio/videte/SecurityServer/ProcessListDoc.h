// ProcessListDoc.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProcessListDoc document

class CProcessListView;

class CProcessListDoc : public CDocument
{
protected:
	CProcessListDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CProcessListDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProcessListDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CProcessListDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	CProcessListView* GetView();

	// Generated message map functions
protected:
	//{{AFX_MSG(CProcessListDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
