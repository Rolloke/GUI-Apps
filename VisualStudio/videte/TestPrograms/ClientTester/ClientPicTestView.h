// ClientPicTestView.h : interface of the CClientPicTestView class
//
/////////////////////////////////////////////////////////////////////////////

class CClientPicTestView : public CView
{
protected: // create from serialization only
	CClientPicTestView();
	DECLARE_DYNCREATE(CClientPicTestView)

// Attributes
public:
	CClientPicTestDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClientPicTestView)
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
	virtual ~CClientPicTestView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CClientPicTestView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in ClientPicTestView.cpp
inline CClientPicTestDoc* CClientPicTestView::GetDocument()
   { return (CClientPicTestDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
