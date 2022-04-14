// SecAnalyzerView.h : interface of the CSecAnalyzerView class
//
/////////////////////////////////////////////////////////////////////////////

class CSecAnalyzerView : public CScrollView
{
protected: // create from serialization only
	CSecAnalyzerView();
	DECLARE_DYNCREATE(CSecAnalyzerView)

// Attributes
public:
	CSecAnalyzerDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSecAnalyzerView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSecAnalyzerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSecAnalyzerView)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnStartPoint();
	afx_msg void OnEndPoint();
	afx_msg void OnNop();
	afx_msg void OnPopupExclude();
	afx_msg void OnPopupInclude();
	afx_msg void OnColoredBackground();
	afx_msg void OnUpdateColoredBackground(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void UpdateWidthHeightMembers();
	void MapToScreen(int x,int y, int &xOut, int &yOut) const;
	void DrawButtons(CDC* pDC);
	BOOL CalcTimeFromPoint(const CPoint point,CTime &result);
	BOOL CalcRecordIndexFromPoint(const CPoint point, int &result);
	void DrawBackground(CDC* pDC);
	//
	double m_dViewScale;
	double m_dWidth;
	double m_dHeight;

	double m_dScaleX;
	double m_dScaleY;

	DWORD m_dwWidth;

	CTime m_startTime;
	CTime m_endTime;

	CPoint m_lastClickedPoint;
	CString m_sClickedName;

	BOOL m_bDoColoredBackground;
};

#ifndef _DEBUG  // debug version in SecAnalyzerView.cpp
inline CSecAnalyzerDoc* CSecAnalyzerView::GetDocument()
   { return (CSecAnalyzerDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
