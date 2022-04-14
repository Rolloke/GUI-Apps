// GraphCreatorView.h : interface of the CGraphCreatorView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRAPHCREATORVIEW_H__93BCFC88_8E0C_4E70_88BC_4B9B04BE8E6C__INCLUDED_)
#define AFX_GRAPHCREATORVIEW_H__93BCFC88_8E0C_4E70_88BC_4B9B04BE8E6C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FilterButton.h"
#include "LineTracker.h"


#define MK_FILTERPIN          0x1000

#define FIRST_FILTER_ID		1000
#define FIRST_CONNECTION_ID	2000
#define LAST_CONNECTION_ID	3000

class CGraphCreatorDoc;
class CFilterCtrl;

// Declare and implement a key provider class derived from IServiceProvider.

struct SFindFilterCtrl
{
	SFindFilterCtrl(IBaseFilter*pF) 
	{
		pFilter = pF;
		nID = 0;
	}
	UINT nID;
	IBaseFilter*pFilter;
};

class CFindFilterDlg;

class CGraphCreatorView : public CFormView
{
	friend class CFilterCtrl;
protected: // create from serialization only
	CGraphCreatorView();
	DECLARE_DYNCREATE(CGraphCreatorView)

public:
	//{{AFX_DATA(CGraphCreatorView)
	enum { IDD = IDD_GRAPHCREATOR_FORM };
	//}}AFX_DATA

// Attributes
public:
	CGraphCreatorDoc*	GetDocument();
	IGraphBuilder*		GetGraph();
	UINT				GetFreeFilterID();
	UINT				GetFreeConnectionID();
	BOOL				GetCtrlRect(UINT nID, CRect *prcCtrl);
   BOOL           IsPrinting() { return m_PrintPen.m_hObject != NULL ? TRUE : FALSE; }
   CPen*          GetPrintPen() { return &m_PrintPen; }
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGraphCreatorView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnDraw(CDC* pDC);
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGraphCreatorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

private:
	void UpdateGraphFilters();
	void ReArrangeFilters();
	CRect ReArrangeFilters(CBaseCtrl*pCtrl, CRect *prcPrev);
	void ReportError(ErrorStruct*, bool bMsgBox=true);

	static BOOL CALLBACK FindFilterCtrls(HWND, LPARAM);
	static BOOL CALLBACK FindPinConnections(HWND, LPARAM);
	static BOOL CALLBACK DrawCtrls(HWND, LPARAM);
	static BOOL CALLBACK RemoveCtrlID(HWND, LPARAM);
	static BOOL CALLBACK EnumSourceFilters(HWND, LPARAM);

protected:


// Generated message map functions
protected:
	//{{AFX_MSG(CGraphCreatorView)
	afx_msg void OnUpdateInsertFilter(CCmdUI* pCmdUI);
	afx_msg void OnInsertFilter();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnEditClear();
	afx_msg void OnUpdateEditClear(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg LRESULT OnHresultError(WPARAM, LPARAM);
	afx_msg LRESULT OnUser(WPARAM, LPARAM);
	afx_msg LRESULT OnGraphNotify(WPARAM, LPARAM);
	afx_msg void OnGraphStart();
	afx_msg void OnUpdateGraphStart(CCmdUI *pCmdUI);
	afx_msg void OnGraphStop();
	afx_msg void OnUpdateGraphStop(CCmdUI *pCmdUI);
	afx_msg void OnGraphPause();
	afx_msg void OnUpdateGraphPause(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFileRenderMedia(CCmdUI *pCmdUI);
	afx_msg void OnViewRearrangeFilters();
	afx_msg void OnUpdateViewRearrangeFilters(CCmdUI *pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CFindFilterDlg *m_pDlgFilters;
	CFont			m_Font;
	CRect			m_rcBounding;
   CPen        m_PrintPen;
};

#ifndef _DEBUG  // debug version in GraphCreatorView.cpp
inline CGraphCreatorDoc* CGraphCreatorView::GetDocument()
   { return (CGraphCreatorDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRAPHCREATORVIEW_H__93BCFC88_8E0C_4E70_88BC_4B9B04BE8E6C__INCLUDED_)
