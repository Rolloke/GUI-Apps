// DirectoryView.h : interface of the CDirectoryView class
//

class CDirContent;
/////////////////////////////////////////////////////////////////////////////

class CDirectoryView : public CTreeView
{
protected: // create from serialization only
	CDirectoryView();
	DECLARE_DYNCREATE(CDirectoryView)

// Attributes
public:
	CUpdateDoc* GetDocument();

// Operations
public:
	HTREEITEM AddDirectory(const CString& sDir, int ico);
	void    DeleteDirectory(const CString& sDir);
	void    Clear();
	void	InsertDirInfo(CDirContent* pDirContent);
	
private:
	CString		GetDir(HTREEITEM hItem);
	HTREEITEM	GetTreeItem(const CString& sDir);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDirectoryView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual void OnDragLeave();
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDirectoryView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CDirectoryView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
    COleDropTarget m_oleDropTarget;
};

#ifndef _DEBUG  // debug version in DirectoryView.cpp
inline CUpdateDoc* CDirectoryView::GetDocument()
   { return (CUpdateDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
