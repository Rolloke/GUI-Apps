#pragma once


class CAnalyseMapFileDoc;
// CCallStackView view

class CCallStackView : public CListView
{
	DECLARE_DYNCREATE(CCallStackView)

protected:
	CCallStackView();           // protected constructor used by dynamic creation
	virtual ~CCallStackView();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

public:
	void	InsertEntry(CString &sAddr, CString& sFncName);
	inline CAnalyseMapFileDoc* GetDocument();
	void	SwapItems(int n1, int n2);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	//{{AFX_MSG(CCallStackView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI *pCmdUI);
};

inline CAnalyseMapFileDoc* CCallStackView::GetDocument()
{
//	ASSERT_KINDOF(CAnalyseMapFileDoc, m_pDocument);
	return (CAnalyseMapFileDoc*)m_pDocument;
}

