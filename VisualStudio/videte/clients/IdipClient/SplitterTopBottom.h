#pragma once

#include "SplitterWndCommon.h"


// CSplitterTopBottom
class CViewDialogBar;
class CViewIdipClient;

class CSplitterTopBottom : public CSplitterWndCommon
{
	DECLARE_DYNAMIC(CSplitterTopBottom)

public:
	CSplitterTopBottom();
	virtual ~CSplitterTopBottom();

// Operations
public:
	void UpdateDlgControls(CCmdTarget* pTarget, BOOL bDisableIfNoHndler);

// Attribute
public:
	CViewDialogBar*		GetViewDlgBar()		{ return m_pDlgBarView;}
	CViewIdipClient*	GetViewIdipClient()	{ return m_pViewIdipClient;}
	int					GetMinWidth();
	int					GetBarHeight();
	virtual UINT		GetSubstCmdID();
	virtual CImageList*	GetSubstImageList(int);

	// Generated message map functions
	//{{AFX_MSG(CSplitterTopBottom)
protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplitterTopBottom)
protected:
public:
	virtual BOOL CreateView(int row, int col, CRuntimeClass* pViewClass, SIZE sizeInit, CCreateContext* pContext);
	//}}AFX_VIRTUAL

protected:
	CViewDialogBar*		m_pDlgBarView;
	CViewIdipClient*	m_pViewIdipClient;
	int					m_nDlgBarRow;
};


