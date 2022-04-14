#if !defined(AFX_OBJECTVIEW_H__46825278_6AF6_11D1_93E4_00C095EC9EFA__INCLUDED_)
#define AFX_OBJECTVIEW_H__46825278_6AF6_11D1_93E4_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ObjectView.h : header file
//
#include "InputDeactivationDialog.h"	/*RM*/
#include "VisionDoc.h"

class CVisionDoc;
class CServer;
class CIPCInputVision;
class CIPCOutputVision;
class CIPCAudioVision;
class CIPCMediaRecord;

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


#define WPARAM_CONNECTION_RECORD      0x0020
#define WPARAM_ERROR_MESSAGE          0x0021

#define WPARAM_AUDIO_STOP_AUDIORECORD 0x0030
#define WPARAM_AUDIO_SET_INPUT_STATE  0x0031

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

/////////////////////////////////////////////////////////////////////////////
// CObjectView view

class CObjectView : public CTreeView
{
protected:
	CObjectView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CObjectView)

// Attributes
public:
	CVisionDoc* GetDocument();
	inline BOOL IsInputDeactivationOpen() const;
	CPoint      GetWindowOffset(CWnd*);
	WORD        GetLocalServerID() { return m_wLocalServerID; };
	WORD		   GetSelectedServer(WORD*pwHostID=NULL, HTREEITEM hSelected=NULL);

// Operations
public:
	void PopupDisconnectMenu(CPoint pt);
   void DisconnectID(UINT nID);
	void Select(WORD wServerID, const CString& s);
	void Select(WORD wServerID, CSecID id, BOOL bOn = TRUE);
	void SetFirstCamID(CSecID id) { m_idFirstCam = id;	};
	void SetSelectMediaID(CSecID id) { m_idSelectMedia = id;	};
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CObjectView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CObjectView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	
#ifdef _HTML_ELEMENTS
	void  UpdateCameraPage(HTREEITEM, WORD wFlags=VDH_FLAGS_UPDATE);
	void  UpdateHostPage(HTREEITEM, BOOL);
public:
	DWORD UpdateMapElement(CSecID, CString&, DWORD& dwUpdate);
	void  AddLocalHTMLmapping(DWORD dwID, CSecID elementID);
	BOOL  IsOnLocalHTMLpage(WORD wHostID, CSecID elementID);
	void  DeleteLocalHTMLmappings();
#endif


// Implementation
protected:
	void InsertInputs(CIPCInputVision* pInput);
	void InsertOutputs(CIPCOutputVision* pOutput);
	void InsertAudios(CIPCAudioVision* pAudio);

	void Actualize(WORD wServerID, CSecID id);
	void Delete(WORD wServerID, CSecID id);
	void Add(WORD wServerID, CSecID id);
	void Select(WORD wServerID, HTREEITEM hItem, CSecID id, BOOL bpOn = TRUE);
	
//ML 24.9.99 Begin Insertation{
	void SwitchRelay(WORD wServerID, CSecID id, BOOL bOn);
//ML 24.9.99 End Insertation}

	HTREEITEM	InsertServer(const CServer* pServer);
	BOOL		DeleteServer(DWORD dwID);

	HTREEITEM	InsertInput(HTREEITEM hServer, const CIPCInputRecord& rec);
	BOOL		ActualizeInput(HTREEITEM hInput, const CIPCInputRecord& rec);
	HTREEITEM	InsertOutput(HTREEITEM hServer, const CIPCOutputRecord& rec);
	BOOL		ActualizeOutput(HTREEITEM hInput, const CIPCOutputRecord& rec);
	HTREEITEM   InsertAudio(HTREEITEM hServer, CIPCMediaRecord&rec);
	BOOL		ActualizeAudio(HTREEITEM hAudio, const CIPCMediaRecord& rec);

	int			GetImageIndex(const CIPCInputRecord& rec);
	int			GetImageIndex(const CIPCOutputRecord& rec);
	int			GetImageIndex(const CIPCMediaRecord& rec);

	BOOL		SetCheck(HTREEITEM hItem,BOOL bFlag);
	BOOL		GetCheck(HTREEITEM hItem);
	HTREEITEM	GetTreeItem(WORD wServer, DWORD dwID=0, DWORD dwSearch=SEARCH_IN_ALL); // returns the server treeitem if dwID==0
	HTREEITEM	GetTreeItem(WORD wServer, const CString& s, DWORD dwSearch=SEARCH_IN_ALL);

	BOOL		IsServerItem(HTREEITEM hItem);
	CServer*    GetServer(HTREEITEM hServer);
	CServer*    GetServer(WORD);
	WORD GetServerIDs(HTREEITEM hServer, WORD*pwHost);
	CIPCMediaRecord* GetSelectedItemsMediaRecord(WORD&wHost, WORD&wServer, HTREEITEM hSelected=NULL);
	CIPCMediaRecord* GetDefaultInputRecord(CIPCAudioVision*);
	CIPCMediaRecord* GetDefaultOutputRecord(CIPCAudioVision*);
	CIPCMediaRecord* GetActiveRecord(CIPCAudioVision*, DWORD dwFlags=MEDIA_INPUT|MEDIA_OUTPUT);
	
	HTREEITEM	GetInputTreeItem(HTREEITEM hServer);
	HTREEITEM	GetCameraTreeItem(HTREEITEM hServer);
	HTREEITEM	GetRelayTreeItem(HTREEITEM hServer);
	HTREEITEM   GetAudioTreeItem(HTREEITEM hServer, bool bCreate=false);
	void		ShowInformation(HTREEITEM hItem);

	void SwitchCamera(HTREEITEM hItemCamera, BOOL bOn);
	void SwitchRelay(HTREEITEM hItemRelais, BOOL bOn);
	void SwitchAudio(HTREEITEM hItemAudio, BOOL bOn);
	void OnAuStartPlay(HTREEITEM hItemAudio);
	void OnAuStopPlay(HTREEITEM hItemAudio);
	void OnAuStartRec(HTREEITEM hItemAudio);
	void OnAuStopRec(HTREEITEM hItemAudio);
	void ReactivateCamera(HTREEITEM hItem);
	void ActivateCamera(HTREEITEM hItem);

	void OnAlarmOffSpanData(WORD wServerID,CSecID id);
	void OnAlarmOffSpan(WORD wServerID,CSecID id, BOOL bOn);

	void	ContextMenu(const CPoint& pt, HTREEITEM hItem);
	// Generated message map functions
protected:
	//{{AFX_MSG(CObjectView)
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDisconnect();
	afx_msg void OnRelaisOpen();
	afx_msg void OnUpdateRelaisOpen(CCmdUI* pCmdUI);
	afx_msg void OnRelaisClose();
	afx_msg void OnUpdateRelaisClose(CCmdUI* pCmdUI);
	afx_msg void OnVideoStart();
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnVideoAlarmSwitch();
	afx_msg void OnUpdateVideoAlarmSwitch(CCmdUI* pCmdUI);
	afx_msg void OnHostSaveSequence();
	afx_msg void OnHostLoadSequence();
	afx_msg void OnInputConfirm();
	afx_msg void OnUpdateInputConfirm(CCmdUI* pCmdUI);
	afx_msg void OnHostDatetime();
	afx_msg void OnUpdateHostDatetime(CCmdUI* pCmdUI);
	afx_msg void OnHostCameraMap();
	afx_msg void OnInputInformation();
	afx_msg void OnUpdateInputInformation(CCmdUI* pCmdUI);
	afx_msg void OnInputDeactivate(); /*RM*/
	afx_msg void OnUpdateInputDeactivate(CCmdUI* pCmdUI); /*RM*/
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnAuStartDuplexPlay();
	afx_msg void OnUpdateAuStartDuplexPlay(CCmdUI* pCmdUI);
	afx_msg void OnAuStopAny();
	afx_msg void OnUpdateAuStopAny(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg void OnSpace();
	afx_msg LRESULT OnNotifyWindow(WPARAM, LPARAM);
	afx_msg LRESULT OnUser(WPARAM wParam, LPARAM lParam);
	afx_msg void OnUpdateAuStartPlay(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAuStopPlay(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAuStartRec(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAuStopRec(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAuProperties(CCmdUI* pCmdUI);
	afx_msg void OnAuStartPlay();
	afx_msg void OnAuStopPlay();
	afx_msg void OnAuStartRec();
	afx_msg void OnAuStopRec();
	afx_msg void OnAuProperties();
public:
	afx_msg void OnUpdateHostCameraMap(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()

private:
	CImageList		m_Images;
	CImageList		m_States;
	CImageList*    m_pDragImage;
	BOOL           m_bDraggedInto;
	WORD           m_wLocalServerID;
	CSecID         m_idFirstCam;
	CSecID         m_idSelectMedia;
	CPtrList       m_LocalHTMLmappings;
	
	BOOL           m_bDuplex;

	CInputDeactivationDialog* m_pInputDeactivationDialog;
	friend class CMainFrame;
	friend class CInputDeactivationDialog;
};

#ifndef _DEBUG  // debug version in VisionView.cpp
inline CVisionDoc* CObjectView::GetDocument()
   { return (CVisionDoc*)m_pDocument; }
#endif
/////////////////////////////////////////////////////////////////////////////
inline BOOL CObjectView::IsInputDeactivationOpen() const
{
	return m_pInputDeactivationDialog != NULL;
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJECTVIEW_H__46825278_6AF6_11D1_93E4_00C095EC9EFA__INCLUDED_)
