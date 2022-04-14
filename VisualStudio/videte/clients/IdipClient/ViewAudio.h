#if !defined(AFX_VIEWAUDIO_H__INCLUDED_)
#define AFX_VIEWAUDIO_H__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ViewCamera.h : header file
//
#include "ViewControllItems.h"

class CViewAudio : public CViewControllItems
{
protected:
	CViewAudio();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CViewAudio)

// Operations
public:
	void SetSelectMediaID(CSecID id) { m_idSelectMedia = id; };
	void Select(WORD wServerID, HTREEITEM hItem, CSecID id, BOOL bpOn = TRUE);		// C?, B?

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewAudio)
	public:
	protected:
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL
// Implementation
protected:
	virtual ~CViewAudio();
	
	virtual	HTREEITEM GetTreeItem(WORD wServer, const CString& s);
	virtual HTREEITEM GetTreeItem(WORD wServer, DWORD dwID=0);

	void		Actualize(WORD wServerID, CSecID id);
	void		Add(WORD wServerID, CSecID id);
	void		InsertAudios(CIPCAudioIdipClient* pAudio);
	HTREEITEM   InsertAudio(HTREEITEM hServer, CIPCMediaRecord&rec);				
	BOOL		ActualizeAudio(HTREEITEM hAudio, const CIPCMediaRecord& rec);
	int			GetImageIndex(const CIPCMediaRecord& rec);

	CIPCMediaRecord* GetDefaultInputRecord(CIPCAudioIdipClient*);
	CIPCMediaRecord* GetDefaultOutputRecord(CIPCAudioIdipClient*);
	CIPCMediaRecord* GetActiveRecord(CIPCAudioIdipClient*, DWORD dwFlags=MEDIA_INPUT|MEDIA_OUTPUT);
//	HTREEITEM   GetAudioTreeItem(HTREEITEM hServer, bool bCreate=false);
	void		SwitchAudio(HTREEITEM hItemAudio, BOOL bOn);
	void		OnAuStartPlay(HTREEITEM hItemAudio);
	void		OnAuStopPlay(HTREEITEM hItemAudio);
	void		OnAuStartRec(HTREEITEM hItemAudio);
	void		OnAuStopRec(HTREEITEM hItemAudio);

	// Generated message map functions
protected:
	//{{AFX_MSG(CViewAudio)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnNotifyWindow(WPARAM, LPARAM);
	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnUser(WPARAM wParam, LPARAM lParam);
	afx_msg void OnAuStartDuplexPlay();
	afx_msg void OnUpdateAuStartDuplexPlay(CCmdUI* pCmdUI);
	afx_msg void OnAuStopAny();
	afx_msg void OnUpdateAuStopAny(CCmdUI* pCmdUI);
	afx_msg void OnAuStartPlay();
	afx_msg void OnUpdateAuStartPlay(CCmdUI* pCmdUI);
	afx_msg void OnAuStopPlay();
	afx_msg void OnUpdateAuStopPlay(CCmdUI* pCmdUI);
	afx_msg void OnAuStartRec();
	afx_msg void OnUpdateAuStartRec(CCmdUI* pCmdUI);
	afx_msg void OnAuStopRec();
	afx_msg void OnUpdateAuStopRec(CCmdUI* pCmdUI);
	afx_msg void OnAuProperties();
	afx_msg void OnUpdateAuProperties(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CSecID			m_idSelectMedia;
	BOOL			m_bDuplex;						// C
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWAUDIO_H__INCLUDED_)
