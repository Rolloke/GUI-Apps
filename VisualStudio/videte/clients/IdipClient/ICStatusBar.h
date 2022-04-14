#pragma once


// CICStatusBar

class CICStatusBar : public CStatusBar
{
	DECLARE_DYNAMIC(CICStatusBar)

public:
	CICStatusBar();
	virtual ~CICStatusBar();

	virtual INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

public:
};


