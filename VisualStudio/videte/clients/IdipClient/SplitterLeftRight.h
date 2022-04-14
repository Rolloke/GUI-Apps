#pragma once

#include "SplitterWndCommon.h"

// CSplitterLeftRight
class CSplitterTopBottom;  // left side
class CSplitterObjectView; // right side

class CSplitterLeftRight : public CSplitterWndCommon
{
	DECLARE_DYNAMIC(CSplitterLeftRight)

public:
	CSplitterLeftRight();
	virtual ~CSplitterLeftRight();

	// Operations
	void ShowObjectView(BOOL);
	void SetObjectView(int);
	void SetTopBottom(int);
	void InitColumns();
	void UpdateSizes();
	void GetWidths(int cx, int &nWidthTopBottom, int &nWidthObjectView);
	int  GetGapAndBorderWidth();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplitterLeftRight)
public:
	virtual BOOL CreateView(int row, int col, CRuntimeClass* pViewClass, SIZE sizeInit, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Attribute
public:
	int						GetObjectView()			{ return m_nObjectView; };
	int						GetTopBottom()			{ return m_nTopBottom; };
	int						GetObjectViewWidth()	{ return m_nObjectViewWidth; };
	CSplitterTopBottom*		GetSplitterTopBottom();
	CSplitterObjectView*	GetSplitterObjectView();
	BOOL					IsObjectViewVisible() { return m_bShowObjectViews;};
	BOOL					IsOEMDialogVisible();
	BOOL					IsDialogBarVisible();

// Generated message map functions
	//{{AFX_MSG(CSplitterLeftRight)
protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	int  m_nObjectViewWidth;
	int  m_nTopBottom;
	int  m_nObjectView;
	BOOL m_bShowObjectViews;
	BOOL m_bCorrectObjectViewWidth;
};


