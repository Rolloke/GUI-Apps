/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ViewArchive.h $
// ARCHIVE:		$Archive: /Project/Clients/IdipClient/ViewArchive.h $
// CHECKIN:		$Date: 14.10.05 11:25 $
// VERSION:		$Revision: 21 $
// LAST CHANGE:	$Modtime: 14.10.05 9:02 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#if !defined(AFX_VIEW_ARCHIVE_H__517E1F0B_9D66_11D1_8C1B_00C095EC9EFA__INCLUDED_)
#define AFX_VIEW_ARCHIVE_H__517E1F0B_9D66_11D1_8C1B_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ObjectTree.h"
#include "DlgDatabaseInfo.h"
#include "DlgQuery.h"
#include "DlgSearchResult.h"

class CIdipClientDoc;
class CDlgSearchResult;
class CDlgQuery;

class CIPCDatabaseIdipClient;
class CServer;

/////////////////////////////////////////////////////////////////////////////
// CViewArchive view

class CViewArchive : public CViewObjectTree
{
protected:
	CViewArchive();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CViewArchive)

// Attributes
public:
	inline CDlgSearchResult* GetDlgSearchResult();
	inline CDlgQuery* GetDlgQuery();
	inline BOOL	IsBackupMode();

// Operations
public:
	void	Redraw();
	void	AddAllBackupSequencesExcept(WORD wServerID, CIPCSequenceRecord* pSequence);
	BOOL	ShowSyncCheckBoxes(BOOL bShow = TRUE);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewArchive)
	public:
	virtual void OnInitialUpdate();
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CViewArchive();

// Implementation
protected:
	void		InsertDatabase(CIPCDatabaseIdipClient* pDatabase);
	HTREEITEM	InsertArchiv(CTreeCtrl& ctrl, HTREEITEM hParent, CIPCArchivRecord* pRecord);
	void		RemoveArchiv(WORD wServerID, WORD wArchivNr);

	HTREEITEM	InsertServer(const CServer* pServer);
	void		Actualize(WORD wServerID);
	BOOL		DeleteServer(WORD wID);
	
	void		InsertSequences(CServer* pServer);
	void		InsertSequence(WORD wServerID, CIPCSequenceRecord* pSequence, HTREEITEM &hArchiv, HTREEITEM &hSplitItem, CIPCArchivRecord* pArchiveReord=NULL);
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

	void				ShowDlgSearchResult();
	CIPCArchivRecord*	HittestArchivToolTip(CPoint pt);

	CIPCSequenceRecord* GetSequenceRecord(HTREEITEM hSequence);
	CIPCArchivRecord*   GetArchiveRecord(HTREEITEM hArchiv);
	CServer*			GetServer(HTREEITEM hServer);

	void		DisconnectDialogs(WORD wServerID);
	void		OnArchivBackup();

	// Generated message map functions
protected:
	//{{AFX_MSG(CViewArchive)
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
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
	afx_msg void OnUpdateDatabaseInfo(CCmdUI *pCmdUI);
	afx_msg LRESULT OnUser(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnToolTipNeedText(UINT id, NMHDR * pNMHDR, LRESULT * pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL				m_bBackupMode;
	BOOL				m_bInsertion;
	BOOL				m_bNeedsRedraw;
	CToolTipCtrl		m_ToolTip;
	CString				m_sTipText;
	CIPCArchivRecord*	m_pArchivToolTip;
	CImageList			m_Images;
	CImageList			m_States;
	HTREEITEM			m_hItemFirstSel;

	CDlgQuery*			m_pDlgQuery;
	CDlgSearchResult*	m_pDlgSearchResult;

	friend class CMainFrame;
	friend class CDlgQuery;
	friend class CDlgSearchResult;
};
/////////////////////////////////////////////////////////////////////////////
inline CDlgSearchResult* CViewArchive::GetDlgSearchResult()
{
	return m_pDlgSearchResult;
}
/////////////////////////////////////////////////////////////////////////////
inline CDlgQuery* CViewArchive::GetDlgQuery()
{
	return m_pDlgQuery;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CViewArchive::IsBackupMode()
{
	return m_bBackupMode;
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEW_ARCHIVE_H__517E1F0B_9D66_11D1_8C1B_00C095EC9EFA__INCLUDED_)
