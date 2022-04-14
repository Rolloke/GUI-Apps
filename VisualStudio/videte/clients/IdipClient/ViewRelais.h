#if !defined(AFX_VIEWRELAIS_H__INCLUDED_)
#define AFX_VIEWRELAIS_H__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ViewRelais.h : header file
//
#include "ViewControllItems.h"

class CViewRelais : public CViewControllItems
{
protected:
	CViewRelais();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CViewRelais)

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewRelais)
	public:
	protected:
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);	// B & C & R & I
	//}}AFX_VIRTUAL
// Implementation
protected:
	virtual ~CViewRelais();

	virtual	HTREEITEM	GetTreeItem(WORD wServer, const CString& s);
	virtual HTREEITEM	GetTreeItem(WORD wServer, DWORD dwID=0);
	
	void InsertOutputs(CIPCOutputIdipClient* pOutput);
	void SwitchRelay(WORD wServerID, CSecID id, BOOL bOn);
	void SwitchRelay(HTREEITEM hItemRelay, BOOL bOn);

	// Generated message map functions
protected:
	//{{AFX_MSG(CViewRelais)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnUser(WPARAM wParam, LPARAM lParam);
	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRelayOpen();
	afx_msg void OnUpdateRelayOpen(CCmdUI* pCmdUI);
	afx_msg void OnRelayClose();
	afx_msg void OnUpdateRelayClose(CCmdUI* pCmdUI);
	afx_msg void OnRelayToggle();
	afx_msg void OnUpdateRelayToggle(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWRELAIS_H__INCLUDED_)
