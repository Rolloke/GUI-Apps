
#pragma once
// CViewDialogBar view
class CDlgInView;
class CDlgSubstitute;

class CViewDialogBar : public CView
{
	DECLARE_DYNCREATE(CViewDialogBar)

protected:
	CViewDialogBar();           // protected constructor used by dynamic creation
	virtual ~CViewDialogBar();
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext);
// Operations
public:
	void UpdateDlgControls(CCmdTarget* pTarget, BOOL bDisableIfNoHndler);
	void ShowDialog(BOOL bShow);

// Attributs
	CDlgInView *GetDlgInView() {return m_pDlgBar;};
	int GetBarWidth(void);
	int GetBarHeight(void);
	bool IsDlgVisible();
	bool IsDlgSubstituteVisible();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewDialogBar)
public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CViewDialogBar)
protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CDlgInView*m_pDlgBar;
	CDlgSubstitute*m_pDlgSubstitute;
private:
	BOOL m_bAvertRecursion;
};


