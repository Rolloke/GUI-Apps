// LeftView.h : interface of the CLeftView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_LEFTVIEW_H__39F1650D_B1A5_4930_A976_C8D33FFE11DF__INCLUDED_)
#define AFX_LEFTVIEW_H__39F1650D_B1A5_4930_A976_C8D33FFE11DF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMSDNEditDoc;

class CLeftView : public CTreeView
{
protected: // create from serialization only
	CLeftView();
	DECLARE_DYNCREATE(CLeftView)

// Attributes
public:
	CMSDNEditDoc* GetDocument();

// Operations
public:
	void OnEditDelete();
   void OnEditCopy(bool bCut=false);
   void OnEditPaste();
   void OnUpdateEditPaste(CCmdUI*);
   void OnUpdateEditDelete(CCmdUI*);
   

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLeftView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnInitialUpdate(); // called first time after construct
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL


// Implementation
public:
	void ExpandRec(HTREEITEM);
	virtual ~CLeftView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CLeftView)
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEditNewFolder();
	afx_msg void OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnViewExpandTree();
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	int FindItem(HTREEITEM, LPCTSTR);
	void CopyItem(HTREEITEM, HTREEITEM);
	HTREEITEM SaveTree(HTREEITEM, CArchive&, CString&, int nOrder=1);

   HTREEITEM m_hSelected;
	CImageList m_Images;
  
   bool m_bExport;
};

#ifndef _DEBUG  // debug version in LeftView.cpp
inline CMSDNEditDoc* CLeftView::GetDocument()
   { return (CMSDNEditDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LEFTVIEW_H__39F1650D_B1A5_4930_A976_C8D33FFE11DF__INCLUDED_)
