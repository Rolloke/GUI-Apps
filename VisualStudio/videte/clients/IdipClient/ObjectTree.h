#ifndef _OBJECTTREE_H_INCLUDED
#define _OBJECTTREE_H_INCLUDED

class CIdipClientDoc;

#define TREE_CONTROL_CHECK_OFF	0
#define TREE_CONTROL_UNCHECKED	2
#define TREE_CONTROL_CHECKED	1

#define FLAG_MINIMIZED	0x00010000

class CViewObjectTree  : public CTreeView
{
	friend class CSplitterObjectView;
public:
	CViewObjectTree();
	DECLARE_DYNCREATE(CViewObjectTree)

// Attributes
public:
	CIdipClientDoc* GetDocument();
	CPoint      GetWindowOffset(CWnd*);
	BOOL		IsActive();
	BOOL		IsMinimized() { return m_bMinimized; };
	int			GetCaptionHeight() { return m_nCaptionHeight;};
	eVisibilityStates	GetVisibilityState() { return m_eVisibility;};
	void		CheckVisibility();
	WORD		GetSelectedServer(WORD*pwHostID=NULL, HTREEITEM hSelected=NULL);
	WORD		GetServerIDs(HTREEITEM hServer, WORD*pwHost);

	// implementation
public:
	virtual void ModifyContextMenu(CMenu*pMenu);
	void OnUpdateCmdShowObjectPane(CCmdUI *pCmdUI);
	BOOL OnShowObjectPane();
	void SetVisibility(eVisibilityStates eVisibility);
	void SetMinimized(BOOL bMin) { m_bMinimized = bMin; }
	void EnableCloseButton(BOOL bEnable);
	void SetSelected(BOOL bSelect);
	void ToggleVisibility();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewObjectTree)
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL
	
	// Generated message map functions
protected:
	//{{AFX_MSG(CViewObjectTree)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()
	//}}AFX_MSG

// Helpers

	BOOL SelectItems(HTREEITEM hItemFrom, HTREEITEM hItemTo);
	HTREEITEM GetFirstSelectedItem();
	HTREEITEM GetNextSelectedItem(HTREEITEM hItem);
	HTREEITEM GetPrevSelectedItem(HTREEITEM hItem);
	void ClearSelection();
	BOOL GetCheck(HTREEITEM hItem);
	BOOL SetCheck(HTREEITEM hItem,BOOL bFlag,BOOL bDisable=FALSE);
	void SetCheckRecurse(HTREEITEM hParent, int iState, BOOL bRecurse=TRUE);
	void MoveTitleButtons();

#ifdef _DEBUG
	// diagnostics
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
protected:
	// TreeCtrl
	HTREEITEM			m_hItemFirstSel;
	// SplitterView
	int					m_nCaptionHeight;
	int					m_nHeightInPromille;
	eVisibilityStates	m_eVisibility;
	BOOL				m_bMinimized;
	BOOL				m_bActive;
	CSkinButton			m_btnTitle;
	CSkinButton			m_btnMiniMize;
	CSkinButton			m_btnClose;
	CSkinButton			m_btnMenu;
};

#ifndef _DEBUG  // debug version in ViewIdipClient.cpp
inline CIdipClientDoc* CViewObjectTree::GetDocument()
   { return (CIdipClientDoc*)m_pDocument; }
#endif

#define TREE_CONTROL_CHECK_OFF	0
#define TREE_CONTROL_UNCHECKED	2
#define TREE_CONTROL_CHECKED	1
/////////////////////////////////////////////////////////////////////////////
// check boxes
BOOL GetCheck(CTreeCtrl& ctrl, HTREEITEM hItem);
BOOL SetCheck(CTreeCtrl& ctrl, HTREEITEM hItem,BOOL bFlag,BOOL bDisable = FALSE);
void SetCheckRecurse(CTreeCtrl& ctrl, HTREEITEM hParent, int iState, BOOL bRecurse=TRUE);

#endif // _OBJECTTREE_H_INCLUDED