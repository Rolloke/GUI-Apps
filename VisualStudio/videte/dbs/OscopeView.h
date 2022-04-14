#pragma once

#include "DBSDoc.h"

// COscopeView view
class COScopeCtrl;

class COscopeView : public CView
{
protected:
	COscopeView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(COscopeView)

// Implementation
public:
	virtual ~COscopeView();
	CDBSDoc* GetDocument();

// Operations
public:
	void AppendValue(int nType, double dVal, BOOL bUpdate);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDBSView)
protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Generated message map functions
	//{{AFX_MSG(CDBSView)
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDestroy();
	//}}AFX_MSG
private:
	COScopeCtrl*m_pScope1;
	COScopeCtrl*m_pScope2;
public:
	void InitScopeCtrls(void);
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
};

inline CDBSDoc* COscopeView::GetDocument()
   { return (CDBSDoc*)m_pDocument; }


