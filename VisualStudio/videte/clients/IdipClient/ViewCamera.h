#if !defined(AFX_VIEWCAMERA_H__INCLUDED_)
#define AFX_VIEWCAMERA_H__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ViewCamera.h : header file
//
#include "ViewControllItems.h"

class CViewCamera : public CViewControllItems
{
protected:
	CViewCamera();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CViewCamera)

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewCamera)
	public:
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);	// B & C & R & I
	protected:
	//}}AFX_VIRTUAL
// Implementation
protected:
	virtual ~CViewCamera();

	virtual	HTREEITEM	GetTreeItem(WORD wServer, const CString& s);
	virtual HTREEITEM	GetTreeItem(WORD wServer, DWORD dwID=0);
	virtual void		Select(WORD wServerID, HTREEITEM hItem, CSecID id, BOOL bpOn = TRUE);

	void	InsertOutputs(CIPCOutputIdipClient* pOutput);
	void	ReactivateCamera(HTREEITEM hItem);
	void	ActivateCamera(HTREEITEM hItem);
	void	SwitchCamera(HTREEITEM hItemCamera, BOOL bOn, BOOL bForce=TRUE);
	int		GetImageIndex(const CIPCOutputRecord& rec);

public:
	void	SetFirstCamID(CSecID id) { m_idFirstCam = id;	};
	int	    GetNrOfCamerasOfSelectedServer();									// C
	void	OnHostSaveSequence(WORD wServerID);
	void	OnHostLoadSequence(WORD wServerID);
	BOOL	InsertKnownHosts(BOOL bInsert);

private:
	BOOL	FindCamInTreeAndActivate(WORD wCurrentCamID);							// C

	// Generated message map functions
protected:
	//{{AFX_MSG(CViewCamera)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnUser(WPARAM wParam, LPARAM lParam);
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnVideoStart();
	afx_msg void OnSpace();
	afx_msg void OnButtonNumBlock(UINT nID);
	afx_msg void OnUpdateButtonNumBlock(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CSecID			m_idFirstCam;					// C
	BOOL			m_bCamNrFirstDigit;				// C
	WORD			m_wCamNrFirstDigit;				// C
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWCAMERA_H__INCLUDED_)
