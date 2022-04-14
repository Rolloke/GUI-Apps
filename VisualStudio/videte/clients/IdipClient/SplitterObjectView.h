#pragma once

#include "SplitterWndCommon.h"

class CViewCamera;
class CViewArchive;
class CViewRelais;
class CViewAlarmList;
class CViewAudio;
class CViewDialogBar;


// CSplitterObjectView

enum eObjViewDivMode
{
	OVDM_MINIMIZED				= 0,
	OVDM_EQUAL_ARANGEMENT		= 1,
	OVDM_SHOW_ONE_VIEW			= 2,
	OVDM_VARIABLE_ARANGEMENT	= 3,
};

class CSplitterObjectView : public CSplitterWndCommon
{
	friend class CViewObjectTree;
	DECLARE_DYNAMIC(CSplitterObjectView)

public:
	CSplitterObjectView();
	virtual ~CSplitterObjectView();
	void	SetDivisionMode(eObjViewDivMode nDM);
	void	SetVisibility(eVisibilityStates eVisibility);
	void	Update();
	void	SetActiveID(int nID);
	BOOL	SetActiveView(CRuntimeClass* pRTC);
	void	ShowObjectView(BOOL bShow);
	void	ShowOEMDialog(BOOL bShow);
	void	SaveSettings();
	CViewObjectTree* GetSelectedServerAndView(WORD &wServer);
	BOOL	OnTab(BOOL bNext, BOOL bSelect);


protected:
	void    SetRowInfo(int nRow, int nIdealHeight, int nMinHeight, int&nRemainHeight);
	void	UpdateVisibility();
// Attribute
public:
	CViewCamera*	GetViewCamera()		const	{ return m_pViewCamera;};
	CViewArchive*	GetViewArchive()	const	{ return m_pViewArchive;};
	CViewRelais*	GetViewRelais()		const	{ return m_pViewRelais;};
	CViewAlarmList* GetViewAlarmList()	const	{ return m_pViewAlarmList;};
	CViewAudio*		GetViewAudio()		const	{ return m_pViewAudio;};
	CViewDialogBar*	GetViewOEMDlg()		const	{ return m_pViewOEMDlg;}
	CViewObjectTree*GetViewSelected()	const;
	int				GetMinWidth();
	int				GetBarHeight();
	BOOL			IsOEMDialogVisible()const	{ return m_bOEMDialogVisible;};
	BOOL			IsObjectViewVisible()const	{ return m_bObjectViewVisible;};
	eVisibilityStates GetVisibilityState() { return m_eVisibility;};
	eObjViewDivMode	GetDivisionMode() { return m_nDivisionMode; };

	virtual UINT	GetSubstCmdID();
	virtual CImageList*GetSubstImageList(int);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplitterObjectView)
public:
	virtual BOOL CreateView(int row, int col, CRuntimeClass* pViewClass, SIZE sizeInit, CCreateContext* pContext);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CSplitterObjectView)
protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnUserMsg(WPARAM wParam, LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnObjectviewEqual();
	afx_msg void OnUpdateObjectviewEqual(CCmdUI *pCmdUI);
	afx_msg void OnObjectviewOneview();
	afx_msg void OnUpdateObjectviewOneview(CCmdUI *pCmdUI);
	afx_msg void OnObjectviewVariable();
	afx_msg void OnUpdateObjectviewVariable(CCmdUI *pCmdUI);
	afx_msg void OnDestroy();
	afx_msg void OnUpdateCmdShowObjectPane(CCmdUI *pCmdUI);
	afx_msg void OnShowObjectPane(UINT nID);
	afx_msg BOOL OnToolTipNotify(UINT, NMHDR *, LRESULT *);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	int				m_nActiveID;
	int				m_nTrackingRow;
	CWndList		m_ViewList;
	eObjViewDivMode	m_nDivisionMode;
	SIZE			m_szUpdate;

	BOOL			m_bOEMDialogVisible;
	BOOL			m_bObjectViewVisible;
	eVisibilityStates	m_eVisibility;
	CString			m_strTipText;

	CViewCamera*	m_pViewCamera;
	CViewArchive*	m_pViewArchive;
	CViewRelais*	m_pViewRelais;
	CViewAlarmList*	m_pViewAlarmList;
	CViewAudio*		m_pViewAudio;
	CViewDialogBar*	m_pViewOEMDlg;

	CViewObjectTree*m_pViewSelected;
};


