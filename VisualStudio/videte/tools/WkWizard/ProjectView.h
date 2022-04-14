// ProjectView.h : interface of the CProjectView class
//
/////////////////////////////////////////////////////////////////////////////

class CProjectView : public CTreeView
{
protected: // create from serialization only
	CProjectView();
	DECLARE_DYNCREATE(CProjectView)

// Attributes
public:
	CWkWizardDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProjectView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CProjectView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CProjectView)
	afx_msg void OnEditExtractResource();
	afx_msg void OnEditScan();
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextMenu();
	afx_msg void OnEditInsert();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void ContextMenu(LPPOINT lpPoint); 
};

#ifndef _DEBUG  // debug version in ProjectView.cpp
inline CWkWizardDoc* CProjectView::GetDocument()
   { return (CWkWizardDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
