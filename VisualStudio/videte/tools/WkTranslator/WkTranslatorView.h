// WkTranslatorView.h : interface of the CWkTranslatorView class
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

class CTokenArray;
class CToken;
class CTSArray;
class CTSArrayArray;
class CTranslateString;

class CWkTranslatorView : public CListView
{
protected: // create from serialization only
	CWkTranslatorView();
	DECLARE_DYNCREATE(CWkTranslatorView)

// Attributes
public:
	CWkTranslatorDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWkTranslatorView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWkTranslatorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CWkTranslatorView)
	afx_msg void OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnEdit();
	afx_msg void OnNextTodo();
	afx_msg void OnEditFind();
	afx_msg void OnEditFindNext();
	afx_msg void OnUpdateEditFindNext(CCmdUI* pCmdUI);
	afx_msg void OnCheckTranslation();
	afx_msg void OnCheckTranslationAll();
	afx_msg void OnEditReplace();
	afx_msg void OnViewShowOnlyTodo();
	afx_msg void OnUpdateViewShowOnlyTodo(CCmdUI* pCmdUI);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewShowOnlySuggestions();
	afx_msg void OnUpdateViewShowOnlySuggestions(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void OnTSArrayArray(const CTSArrayArray* pTSArrayArray);
	void InsertTranslateString(CTranslateString* pTranslateString,int i, int j);
	void DeleteItemsAndColumns();
	void FindText();
	void EditReplaceText();
	void CheckTranslation();

private:
	CString	m_sSearchFor;
	CString	m_sEdit;
	CString	m_sReplace;
	int		m_iCurSel;
	int		m_dwCurData;
	int		m_iLastFound;

	int		m_iI;
	int		m_iJ;
	BOOL	m_bCaseSensitive;
	BOOL	m_bShowOnlyTODOs;
	BOOL  m_bShowOnlySuggestions;
public:
	afx_msg void OnEditRemoveAllTodos();
	afx_msg void OnUpdateEditRemoveAllTodos(CCmdUI *pCmdUI);
};

#ifndef _DEBUG  // debug version in WkTranslatorView.cpp
inline CWkTranslatorDoc* CWkTranslatorView::GetDocument()
   { return (CWkTranslatorDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
