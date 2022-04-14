/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ObjectView.h $
// ARCHIVE:		$Archive: /Project/Clients/Recherche/ObjectView.h $
// CHECKIN:		$Date: 26.05.03 13:52 $
// VERSION:		$Revision: 40 $
// LAST CHANGE:	$Modtime: 26.05.03 11:46 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#if !defined(AFX_OBJECTVIEW_H__517E1F0B_9D66_11D1_8C1B_00C095EC9EFA__INCLUDED_)
#define AFX_OBJECTVIEW_H__517E1F0B_9D66_11D1_8C1B_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ObjectView.h : header file
//

#include "DataBaseInfoDlg.h"
#include "QueryDialog.h"
#include "SearchResultDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CObjectView view

class CObjectView : public CTreeView
{
protected:
	CObjectView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CObjectView)

// Attributes
public:
	CRechercheDoc* GetDocument();
	inline CSearchResultDialog* GetSearchResultDialog();
	inline CQueryDialog* GetQueryDialog();
	inline BOOL	IsBackupMode();

// Operations
public:
	void Redraw();
	void PopupDisconnectMenu(CPoint pt);
    void DisconnectID(UINT nID);
	void AddAllBackupSequencesExcept(WORD wServerID, CIPCSequenceRecord* pSequence);
	BOOL ShowSyncCheckBoxes(BOOL bShow = TRUE);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CObjectView)
	public:
	virtual void OnInitialUpdate();
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CObjectView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Implementation
protected:
	void		InsertDatabase(CIPCDatabaseRecherche* pDatabase);
	HTREEITEM	InsertArchiv(CTreeCtrl& ctrl, HTREEITEM hParent, CIPCArchivRecord* pRecord);
	void		RemoveArchiv(WORD wServerID, WORD wArchivNr);

	HTREEITEM	InsertServer(const CServer* pServer);
	BOOL		DeleteServer(WORD wID);
	
	void		InsertArchivFile(WORD wServerID, CIPCArchivFileRecord* pFile);
	void		InsertSequence(WORD wServerID, CIPCSequenceRecord* pSequence);
	void		RemoveSequence(WORD wServerID, WORD wArchivNr, WORD wSequenceNr);
	
	BOOL		IsServerItem(HTREEITEM hItem);
	BOOL		IsArchivItem(HTREEITEM hItem);
	BOOL		IsDateItem(HTREEITEM hItem);
	BOOL		IsSequenceItem(HTREEITEM hItem);
	HTREEITEM	GetServerItem(HTREEITEM hItem);
	HTREEITEM	GetServerItem(WORD wServer);
	HTREEITEM	GetArchivItem(WORD wServer, WORD wArchivNr);
	HTREEITEM FindItemByID(WORD wArchiveID, HTREEITEM hServer);
	HTREEITEM FindServerItem(WORD wServerID, HTREEITEM hSelected);

	void		ContextMenu(const CPoint& pt, HTREEITEM hItem);

	// multiple selection
	void		ClearSelection();
	BOOL		SelectItems(HTREEITEM hItemFrom, HTREEITEM hItemTo);
	HTREEITEM	GetPrevSelectedItem( HTREEITEM hItem );
	HTREEITEM	GetNextSelectedItem( HTREEITEM hItem );
	HTREEITEM	GetFirstSelectedItem();

	void				ShowSearchResultDialog();
	CIPCArchivRecord*	HittestArchivToolTip(CPoint pt);

	CIPCSequenceRecord* GetSequenceRecord(HTREEITEM hSequence);
	CIPCArchivRecord*   GetArchiveRecord(HTREEITEM hArchiv);
	CServer*			GetServer(HTREEITEM hServer);

	void	DisconnectDialogs(WORD wServerID);
	void	OnArchivBackup();

	// Generated message map functions
protected:
	//{{AFX_MSG(CObjectView)
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSequenceOpen();
	afx_msg void OnDelete();
	afx_msg void OnDatabaseInfo();
	afx_msg void OnSearch();
	afx_msg void OnUpdateSearch(CCmdUI* pCmdUI);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnUpdateSequenceOpen(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDelete(CCmdUI* pCmdUI);
	afx_msg void OnArchivEmpty();
	afx_msg void OnUpdateArchivEmpty(CCmdUI* pCmdUI);
	afx_msg void OnUpdateArchivBackup(CCmdUI* pCmdUI);
	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnArchivDelete();
	afx_msg void OnUpdateArchivDelete(CCmdUI* pCmdUI);
	afx_msg void OnSequenceOpenSingle();
	afx_msg void OnDateSequencesDelete();
	afx_msg void OnUpdateDateSequencesDelete(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSequenceOpenSingle(CCmdUI* pCmdUI);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	afx_msg void OnDisconnect();
	afx_msg LRESULT OnUser(WPARAM wParam, LPARAM lParam);
	BOOL OnToolTipNeedText(UINT id, NMHDR * pNMHDR, LRESULT * pResult);
	DECLARE_MESSAGE_MAP()

private:
	BOOL				m_bBackupMode;
	CToolTipCtrl		m_ToolTip;
	CIPCArchivRecord*	m_pArchivToolTip;
	CImageList			m_Images;
	CImageList			m_States;
	HTREEITEM			m_hItemFirstSel;

	CQueryDialog*			m_pQueryDialog;
	CSearchResultDialog*	m_pSearchResultDialog;

	DWORD m_dwLastRedraw;
	BOOL  m_bNeedsRedraw;

	friend class CMainFrame;
	friend class CQueryDialog;
	friend class CSearchResultDialog;
};
#ifndef _DEBUG  // debug version in RechercheView.cpp
inline CRechercheDoc* CObjectView::GetDocument()
   { return (CRechercheDoc*)m_pDocument; }
#endif
/////////////////////////////////////////////////////////////////////////////
inline CSearchResultDialog* CObjectView::GetSearchResultDialog()
{
	return m_pSearchResultDialog;
}
/////////////////////////////////////////////////////////////////////////////
inline CQueryDialog* CObjectView::GetQueryDialog()
{
	return m_pQueryDialog;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CObjectView::IsBackupMode()
{
	return m_bBackupMode;
}


#define TREE_CONTROL_CHECK_OFF	0
#define TREE_CONTROL_UNCHECKED	2
#define TREE_CONTROL_CHECKED	1
/////////////////////////////////////////////////////////////////////////////
// check boxes
BOOL GetCheck(CTreeCtrl& ctrl, HTREEITEM hItem);
BOOL SetCheck(CTreeCtrl& ctrl, HTREEITEM hItem,BOOL bFlag,BOOL bDisable = FALSE);
void SetCheckRecurse(CTreeCtrl& ctrl, HTREEITEM hParent, int iState, BOOL bRecurse=TRUE);
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJECTVIEW_H__517E1F0B_9D66_11D1_8C1B_00C095EC9EFA__INCLUDED_)
