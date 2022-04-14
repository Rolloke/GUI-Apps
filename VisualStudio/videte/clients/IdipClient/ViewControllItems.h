#if !defined(AFX_VIEW_CONTROLLITEMS_H__INCLUDED_)
#define AFX_VIEW_CONTROLLITEMS_H__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ViewCamera.h : header file
//
#include "IdipClientDoc.h"

class CIdipClientDoc;
class CServer;
class CIPCInputIdipClient;
class CIPCOutputIdipClient;
class CIPCAudioIdipClient;
class CIPCMediaRecord;
class CDlgInputDeactivation;

#define WPARAM_ACTUALIZE              0x0001
#define WPARAM_DELETE                 0x0002
#define WPARAM_ADD                    0x0003
#define WPARAM_SELECT                 0x0004
#define WPARAM_INFORMATION            0x0005
#define WPARAM_URL                    0x0006
#define WPARAM_CAMERA_MAP             0x0007
#define WPARAM_MESSAGE                0x0008
#define WPARAM_UNSELECT               0x0009

#define WPARAM_CONTROL_SELECT_CAM     0x000a
#define WPARAM_CONTROL_UNSELECT_CAM   0x000b
#define WPARAM_CONTROL_SELECT_RELAY   0x000c
#define WPARAM_CONTROL_UNSELECT_RELAY 0x000d
#define WPARAM_ALARM_OFF_SPAN         0x000e
#define WPARAM_ALARM_OFF_SPAN_ON      0x000f
#define WPARAM_ALARM_OFF_SPAN_OFF     0x0010

#define WPARAM_DATABASE_CONNECTED     0x0011
#define WPARAM_DATABASE_DISCONNECTED  0x0012

#define WPARAM_INPUT_CONNECTED        0x0013
#define WPARAM_INPUT_DISCONNECTED     0x0014
#define WPARAM_OUTPUT_CONNECTED       0x0015
#define WPARAM_OUTPUT_DISCONNECTED    0x0016
#define WPARAM_CONTROL_CONNECTED      0x0017
#define WPARAM_CONTROL_DISCONNECTED   0x0018
#define WPARAM_AUDIO_CONNECTED        0x0019
#define WPARAM_AUDIO_DISCONNECTED     0x001A

#define WPARAM_INPUT_FETCHRESULT      0x001B
#define WPARAM_OUTPUT_FETCHRESULT     0x001C
#define WPARAM_AUDIO_FETCHRESULT      0x001D
#define WPARAM_CONNECT_THREAD_END     0x001E
#define WPARAM_DATABASE_FETCHRESULT   0x001F


#define WPARAM_CONNECTION_RECORD      0x0020
#define WPARAM_ERROR_MESSAGE          0x0021

#define WPARAM_AUDIO_STOP_AUDIORECORD 0x0030
#define WPARAM_AUDIO_SET_INPUT_STATE  0x0031

#define WPARAM_UPDATE_SYNCDLGS_IN_WNDPLAY 0x0032
#define WPARAM_INSERT_DEBUGGER_WINDOW	0x0033

#define WPARAM_ADD_SEQUENCE			  0x0033
#define WPARAM_DELETE_SEQUENCE		  0x0034


#define SEARCH_IN_INPUT				   0x00000001
#define SEARCH_IN_CAMERAS			   0x00000002
#define SEARCH_IN_RELAYS			   0x00000004
#define SEARCH_IN_MEDIA				   0x00000008
#define SEARCH_IN_HOSTS             0x00000010
#define SEARCH_IN_ALL				   (SEARCH_IN_INPUT|SEARCH_IN_CAMERAS|SEARCH_IN_RELAYS|SEARCH_IN_MEDIA|SEARCH_IN_HOSTS)
#define SEARCH_BY_ID	               0x00000020
#define SEARCH_BY_SERVER            0x00000040

#define SUBID_MEDIA_AUDIO 1
#define SUBID_MEDIA_VIDEO 2
#define MEDIA_INPUT  1
#define MEDIA_OUTPUT 2

#define SETBIT(w,b)    ((__int64)((DWORD)(w) | (1L << (b))))
#define CLRBIT(w,b)    ((__int64)((DWORD)(w) & ~(1L << (b))))
#define TSTBIT(w,b)    ((BOOL)((DWORD)(w) & (1L << (b)) ? TRUE : FALSE))


#define MAPPING_SERVER 0
#define MAPPING_EL_ID  1
/////////////////////////////////////////////////////////////////////////////
// CViewControllItems

#define STATIC_DEFAULT_DEV 1

/////////////////////////////////////////////////////////////////////////////
// CViewControllItems view

#include "ObjectTree.h"

class CViewControllItems : public CViewObjectTree
{
protected:
	CViewControllItems();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CViewControllItems)

// Attributes
public:
	// ServerItem
	BOOL		IsServerItem(HTREEITEM hItem);
	CServer*	GetServer(HTREEITEM hServer);
	CServer*	GetServer(WORD);

// Operations
public:
	void Select(WORD wServerID, const CString& s);
	void Select(WORD wServerID, CSecID id, BOOL bOn = TRUE);
	virtual	void Select(WORD wServerID, HTREEITEM hItem, CSecID id, BOOL bpOn = TRUE);


protected:
	HTREEITEM	InsertServer(const CServer* pServer);
	BOOL		DeleteServer(DWORD dwID);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewControllItems)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CViewControllItems();
	
	// HTML
	void  UpdateCameraPage(HTREEITEM, WORD wFlags=VDH_FLAGS_UPDATE);
	void  UpdateHostPage(HTREEITEM, BOOL);
public:
	DWORD UpdateMapElement(CSecID, CString&, DWORD& dwUpdate);
	static WORD	GetLocalServerID() { return gm_wLocalServerID; };
	static void	AddLocalHTMLmapping(DWORD dwID, CSecID elementID);
	static BOOL	IsOnLocalHTMLpage(WORD wHostID, CSecID elementID);
	static void	DeleteLocalHTMLmappings();

	virtual	HTREEITEM	GetTreeItem(WORD wServer, const CString& s) {return NULL;};
	virtual HTREEITEM	GetTreeItem(WORD wServer, DWORD dwID=0)		{return NULL;};

protected:
	LRESULT		OnUser(WPARAM wParam, LPARAM lParam);

	// CIPCOutputRecord and CSecID::IsOutputID() identify cameras and relays.
	// So these function do nearly the same for both Views and stay in the base class
	void Actualize(WORD wServerID, CSecID id);
	void Add(WORD wServerID, CSecID id);
	HTREEITEM	InsertOutput(HTREEITEM hServer, const CIPCOutputRecord& rec);		
	BOOL		ActualizeOutput(HTREEITEM hInput, const CIPCOutputRecord& rec);
	int			GetImageIndex(const CIPCOutputRecord& rec);

	void Delete(WORD wServerID, CSecID id);
	CIPCMediaRecord* GetSelectedItemsMediaRecord(WORD&wHost, WORD&wServer, HTREEITEM hSelected=NULL);

	BOOL		SetCheck(HTREEITEM hItem,BOOL bFlag);
	void		ShowInformation(HTREEITEM hItem);
	HTREEITEM	CheckParent(HTREEITEM hItem);

	void	ContextMenu(const CPoint& pt, HTREEITEM hItem);
	void	EndDragg();

	// Generated message map functions
protected:
	//{{AFX_MSG(CViewControllItems)
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnVideoAlarmSwitch();
	afx_msg void OnUpdateVideoAlarmSwitch(CCmdUI* pCmdUI);
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	static CImageList	gm_Images;					// B
	static CImageList	gm_States;					// B
	// HTML
	static CPtrList	gm_LocalHTMLmappings;			// B
	static WORD		gm_wLocalServerID;				// B

private:
	CImageList*		m_pDragImage;					// B
	BOOL			m_bDraggedInto;					// B

	friend class CMainFrame;
public:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEW_CONTROLLITEMS_H__INCLUDED_)
