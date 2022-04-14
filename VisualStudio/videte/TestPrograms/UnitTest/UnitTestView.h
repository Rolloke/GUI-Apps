// UnitTestView.h : interface of the CUnitTestView class
//
/////////////////////////////////////////////////////////////////////////////

class CUnitTestView : public CView
{
protected: // create from serialization only
	CUnitTestView();
	DECLARE_DYNCREATE(CUnitTestView)

// Attributes
public:
	CUnitTestDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUnitTestView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CUnitTestView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CUnitTestView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in UnitTestView.cpp
inline CUnitTestDoc* CUnitTestView::GetDocument()
   { return (CUnitTestDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
