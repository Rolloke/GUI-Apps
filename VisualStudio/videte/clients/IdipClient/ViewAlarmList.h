#if !defined(AFX_VIEWALARMLIST_H__INCLUDED_)
#define AFX_VIEWALARMLIST_H__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ViewCamera.h : header file
//
#include "ViewControllItems.h"


class CViewAlarmList : public CViewControllItems
{
	friend class CDlgInputDeactivation;
protected:
	CViewAlarmList();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CViewAlarmList)

// Attributes
public:
	inline BOOL IsInputDeactivationOpen() const;

	// Implementation
public:
	virtual void ModifyContextMenu(CMenu*pMenu);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewAlarmList)
	public:
	protected:
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);	// B & C & R & I
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

protected:
	virtual ~CViewAlarmList();

	virtual	HTREEITEM GetTreeItem(WORD wServer, const CString& s);
	virtual HTREEITEM GetTreeItem(WORD wServer, DWORD dwID=0);
	
	void		InsertInputs(CIPCInputIdipClient* pInput);
	HTREEITEM	InsertInput(HTREEITEM hServer, const CIPCInputRecord& rec);	
	BOOL		ActualizeInput(HTREEITEM hInput, const CIPCInputRecord& rec);

	void		Actualize(WORD wServerID, CSecID id);
	void		Add(WORD wServerID, CSecID id);

	void		OnAlarmOffSpanData(WORD wServerID,CSecID id);
	void		OnAlarmOffSpan(WORD wServerID,CSecID id, BOOL bOn);

	int			GetImageIndex(const CIPCInputRecord& rec);					// I

	// Generated message map functions
protected:
	//{{AFX_MSG(CViewAlarmList)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnUser(WPARAM wParam, LPARAM lParam);	// splitten und in UpdateAllViewsAsync aufteilen
	afx_msg void OnInputDeactivate(); /*RM*/
	afx_msg void OnUpdateInputDeactivate(CCmdUI* pCmdUI); /*RM*/
	afx_msg void OnInputConfirm();													// I
	afx_msg void OnUpdateInputConfirm(CCmdUI* pCmdUI);								// I
	afx_msg void OnInputInformation();												// I
	afx_msg void OnUpdateInputInformation(CCmdUI* pCmdUI);							// I
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CDlgInputDeactivation* m_pDlgInputDeactivation;
public:
protected:
public:
};

inline BOOL CViewAlarmList::IsInputDeactivationOpen() const
{
	return m_pDlgInputDeactivation != NULL;
}
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWALARMLIST_H__INCLUDED_)
