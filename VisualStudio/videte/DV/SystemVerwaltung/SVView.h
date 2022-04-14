// SVView.h : interface of the CSVView class
//
#ifndef	_SV_VIEW_H
#define _SV_VIEW_H
/////////////////////////////////////////////////////////////////////////////
class CSVTree;
class CSVPage;

class CSVView : public CView
{
protected: // create from serialization only
	CSVView();
	DECLARE_DYNCREATE(CSVView)

// Attributes
public:
	CSVDoc* GetDocument();
	BOOL	CanCloseFrame(BOOL bInit = TRUE);

// Operations
public:
	void InitTree();
	void ClearHTREEITEMs();
	void Clear();

	void TreeSelchanged(HTREEITEM hNewSelItem);
	void AddInitApp(const CString& sAppname);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSVView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSVView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	void Resize();

	void InitSystem();
	void InitHost();
	void InitDatabase();

// Generated message map functions
protected:
	//{{AFX_MSG(CSVView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnFileSave();
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnEditNew();
	afx_msg void OnUpdateEditNew(CCmdUI* pCmdUI);
	afx_msg void OnEditCancel();
	afx_msg void OnUpdateEditCancel(CCmdUI* pCmdUI);
	afx_msg void OnEditDelete();
	afx_msg void OnUpdateEditDelete(CCmdUI* pCmdUI);
	afx_msg void OnEditServerinit();
	afx_msg void OnUpdateEditServerinit(CCmdUI* pCmdUI);
	afx_msg void OnViewTooltips();
	afx_msg void OnUpdateViewTooltips(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CSVTree*	m_pSVTree;
	CSVPage*	m_pSVPage;
	CRect		m_PageRect;
	BOOL		m_bResizing;
	BOOL		m_bClearing;
	long		m_lTreeWidth;

	HTREEITEM   m_SelectedItem;
	HTREEITEM   m_DeletedItem;

	// my tree items
	HTREEITEM	m_hISDNPage;
	HTREEITEM	m_hSocketPage;
	HTREEITEM	m_hAutoLogoutPage;
	HTREEITEM	m_hHostPage;
	HTREEITEM	m_hDrives;
	HTREEITEM	m_hArchivs;
public:
	BOOL			m_bServerInit;
	BOOL			m_bInitApps;
	BOOL		    m_bShowToolTips;
};

#ifndef _DEBUG  // debug version in SVView.cpp
inline CSVDoc* CSVView::GetDocument()
   { return (CSVDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
#endif