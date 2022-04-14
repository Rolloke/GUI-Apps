
#ifndef _CSocketUnitView_H
#define _CSocketUnitView_H

class CSocketUnitView : public CView
{
protected: // create from serialization only
	CSocketUnitView();
	DECLARE_DYNCREATE(CSocketUnitView)

// Attributes
public:
	CSocketUnitDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSocketUnitView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSocketUnitView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSocketUnitView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in SocketUnitView.cpp
inline CSocketUnitDoc* CSocketUnitView::GetDocument()
   { return (CSocketUnitDoc*)m_pDocument; }
#endif


#endif
/////////////////////////////////////////////////////////////////////////////
