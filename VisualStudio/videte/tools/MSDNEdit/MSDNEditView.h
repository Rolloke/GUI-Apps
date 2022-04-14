// MSDNEditView.h : interface of the CMSDNEditView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MSDNEDITVIEW_H__90B6F324_BD82_4419_9681_8F7D7643FBEF__INCLUDED_)
#define AFX_MSDNEDITVIEW_H__90B6F324_BD82_4419_9681_8F7D7643FBEF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define LIST_COLUMNS 12

class CMSDNEditView : public CListView
{
protected: // create from serialization only
	CMSDNEditView();
	DECLARE_DYNCREATE(CMSDNEditView)

// Attributes
public:
	CMSDNEditDoc* GetDocument();
   void OnUpdateEditDelete(CCmdUI*);
	void OnEditDelete();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMSDNEditView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMSDNEditView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
   int m_nListColumnWidths[LIST_COLUMNS];
   int m_nListColumnOrder[LIST_COLUMNS];
// Generated message map functions
protected:
	//{{AFX_MSG(CMSDNEditView)
	afx_msg void OnDestroy();
	afx_msg void OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	afx_msg void OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct);
	DECLARE_MESSAGE_MAP()
private:
	void SaveListItem(int, CArchive&);

   static int gm_nIDs[];
};

#ifndef _DEBUG  // debug version in MSDNEditView.cpp
inline CMSDNEditDoc* CMSDNEditView::GetDocument()
   { return (CMSDNEditDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MSDNEDITVIEW_H__90B6F324_BD82_4419_9681_8F7D7643FBEF__INCLUDED_)
