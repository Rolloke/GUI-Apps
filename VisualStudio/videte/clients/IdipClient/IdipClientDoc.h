// IdipClientDoc.h : interface of the CIdipClientDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_IdipClientDOC_H__4682526C_6AF6_11D1_93E4_00C095EC9EFA__INCLUDED_)
#define AFX_IdipClientDOC_H__4682526C_6AF6_11D1_93E4_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "CIPCDatabaseIdipClientLocal.h"
#include "Servers.h"

// Update view values
// NULL used for InitialUpdate
#define VDH_INPUT					0x0001
#define VDH_OUTPUT					0x0002
#define VDH_REMOVE_SERVER			0x0003
#define VDH_ADD_CAMWINDOW			0x0004
#define VDH_REMOVE_CAMWINDOW		0x0005
#define VDH_ACTIVATE_REQUEST_CAM	0x0006
#define VDH_INFORMATION				0x0007
#define VDH_INPUT_CONFIRM			0x0008
#define VDH_DELETE_CAMWINDOW		0x0009
#define VDH_INPUT_URL				0x000A
#define VDH_DEACTIVATE_CAMERAS		0x000B
#define VDH_CAMERA_MAP				0x000C
#define VDH_NOTIFICATION_MESSAGE	0x000D
#define VDH_SINGLE_CAM				0x000E
#define VDH_CAM_MAP					0x000F
#define VDH_HOST_MAP				0x0011
#define VDH_AUDIO 					0x0012
#define VDH_DATABASE				0x0013
#define VDH_DEL_SEQUENCE			0x0015
#define VDH_OPEN_FILE				0x0016
#define VDH_ADD_ARCHIV				0x0017
#define VDH_DEL_ARCHIV				0x0018
#define VDH_ADD_SEQUENCE			0x0019
#define VDH_OPEN_SEQUENCE			0x001A
#define VDH_BACKUP					0x001B
#define VDH_BACKUP_ADD_SEQUENCE		0x001C
#define VDH_BACKUP_DEL_SEQUENCE		0x001D
#define VDH_BACKUP_ADD_ARCHIVE		0x001E
#define VDH_BACKUP_DEL_ARCHIVE		0x001F
#define VDH_SYNC_DEL_ARCHIVE		0x0020
#define VDH_SELECT_SEQUENCE			0x0021
#define VDH_SET_CMD_ACTIVE_WND		0x0022
#define VDH_SET_AUDIO_INPUT_STATE	0x0023
#define VDH_DISCONNECT				0x0024

// append new value before and add to source code in same order please

// flags
#define VDH_FLAGS_SELECTED			0x0100
#define VDH_FLAGS_UPDATE			0x0200
#define VDH_FLAGS_DISCONNECT		0x0400

class CViewCamera;
class CViewRelais;
class CViewAlarmList;
class CViewAudio;
class CViewArchive;
class CViewIdipClient;
class CIPCDatabaseIdipClientLocal;
class CIPCServerControlIdipClient;
class CIPCOutputIdipClientDecoder;
class CIPCInputIdipClientCommData;
class CIPCAudioIdipClient;

class CStringHint : public CObject
{
	DECLARE_DYNAMIC(CStringHint)	
public:
	CStringHint(CString sMsg);
	CString GetMessage();

private:
	CStringHint();
	CString m_sMessage;
};
/////////////////////////////////////////////////////////////////////////////
class CIdipClientDoc : public CDocument
{
protected: // create from serialization only
	CIdipClientDoc();
	DECLARE_DYNCREATE(CIdipClientDoc)

// Attributes
public:
	BOOL			GetStore();
	BOOL			GetSequence();
	DWORD			GetSequenceIntervall();
	inline const CServers& GetServers() const;
	CServer*		GetServer(WORD wServerID);
	CServer*		GetServerFromIndex(int nIndex);
	CServer*		GetActiveServer();
	CServer*		GetLocalServer();
	WORD			GetServerIDbyHostID(WORD wHostID, BOOL bConnect);
/*
	CViewCamera*	GetViewCamera();
	CViewRelais*	GetViewRelais();
	CViewAlarmList*	GetViewAlarmList();
	CViewAudio*		GetViewAudio();
	CViewArchive*	GetViewArchive();
	CViewIdipClient*GetViewIdipClient();
*/
	CIPCDatabaseIdipClientLocal* GetLocalDatabase();
	CIPCInputIdipClientCommData* GetCIPCInputIdipClientCommData();
	CIPCAudioIdipClient*	     GetLocalAudio();
	CIPCDataCarrierIdipClient*	 GetDataCarrier();


	BOOL IsLocalDatabaseConnected();
	BOOL IsCommDataConnected();
	BOOL IsAudioConnected();
	BOOL IsDataCarrierConnected();

	CIPCServerControlIdipClient* GetCIPCServerControlIdipClient();

	void CloseConnectionDlg();

// Operations
public:
	void ActualizeInput(CIPCInputIdipClient* pInput);
	void ActualizeOutput(CIPCOutputIdipClient* pOutput);
	void ActualizeAudio(CIPCAudioIdipClient* pAudio);
	void ActualizeDatabase(CIPCDatabaseIdipClient* pDatabase);	// Recherche

	BOOL Connect(CSecID idHostID);
	void Disconnect(WORD wServerID, BOOL bAllowRequest=TRUE);
	BOOL DisconnectAll(BOOL bExceptAlarm, BOOL bAsk=FALSE);
	void DisconnectRemote();	// Recherche
	void Remove(WORD wServerID);
	void ResetConnectionAutoLogout();

	void AddConnectionRecord(const CConnectionRecord &c);
	void SetSequence(BOOL bOn);
	void SetSequenceTime(DWORD dwSeconds);
	DWORD UpdateMapElement(CSecID ServerID, CString &s, DWORD& dwUpdate);
	inline void SetTitle(CString sTitle);

	void PictureStored();
	BOOL InitAlarmUser();

	void OnLocalDatabaseConnected();
	void OnLocalDatabaseDisconnected();
	void OnLocalDataCarrierDisconnected();
	void OnConnectionRecord();

	void UpdateAllViewsAsync(LPARAM lHint, CObject* pHint);
	void SetAutoLogout(BOOL bSet) { m_bAutoLogout = bSet; };
	// Recherche
	void DeleteArchivFiles();
	void StartSearch();
	void StopSearch();
	void StopSearchCopy();

// Overrides
	// ClassWizard generated virtual function overrides
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void OnIdle();
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);

// Implementation
public:
	virtual ~CIdipClientDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


// Generated message map functions
	//{{AFX_MSG(CIdipClientDoc)
protected:
	afx_msg void OnConnectLocal();
	afx_msg void OnUpdateConnectLocal(CCmdUI* pCmdUI);
	afx_msg void OnConnect();
	afx_msg void OnUpdateConnect(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDisconnect(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSequenzePane(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDatabasePane(CCmdUI* pCmdUI);
	afx_msg void OnSequenze();
	afx_msg void OnSequenceX(UINT nID);
	afx_msg void OnUpdateSequenceX(CCmdUI*pCmdUI);
	afx_msg void OnVideoSaveAll();
	afx_msg void OnUpdateVideoSaveAll(CCmdUI* pCmdUI);
	afx_msg void OnVideoSaveAllStop();
	afx_msg void OnVideoSaveAllToggle();
	afx_msg void OnConnectMap();
	afx_msg void OnFileOpen();
	afx_msg void OnUpdateFileOpen(CCmdUI *pCmdUI);
	afx_msg void OnFilePrint();
	afx_msg void OnUpdateVideoSaveAllStop(CCmdUI *pCmdUI);
public:
	afx_msg void OnUpdateConnectMap(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// implementation
protected:
	static	UINT	ReconnectThreadProc(LPVOID pParam);
			void	StartReconnectThread();
			void	StopReconnectThread();
			void	ResetReconnect();
			void	Reconnect();
			void	DoReconnect();
			void	ConnectToLocalDatabase();
			void	ConnectToCommData();
			void	ConnectToAudio();
			void	ConnectToDataCarrier();
			void	AlarmConnection(const CConnectionRecord& c);
			void	ControlConnection(const CConnectionRecord& c);
			void	AlarmAnimation();

// data members
protected:
	CServers						m_Servers;	// connected servers including local one
	CIPCDatabaseIdipClientLocal*	m_pCIPCDatabaseLocal;
	CIPCServerControlIdipClient*	m_pCIPCServerControlIdipClient;
	CIPCInputIdipClientCommData*	m_pCIPCInputIdipClientCommData;
	CIPCAudioIdipClient*			m_pCIPCLocalAudio;
	CIPCDataCarrierIdipClient*		m_pDataCarrier; // the data carrier connection (backup)

	CConnectionRecords			m_aConnectionRecords;

	BOOL			m_dwLastReconnect;
	DWORD			m_dwReconnectIntervall;
	CWinThread*		m_pReconnectThread;

	CIPCFetchResult* m_pFetchResultLocalInput;
	CIPCFetchResult* m_pFetchResultLocalDatabase;
	CIPCFetchResult* m_pFetchResultLocalAudio;
	CIPCFetchResult* m_pFetchResultLocalDataCarrier;
	
	BOOL			m_bReconnectFinished;

	BOOL			m_bRun;
	BOOL			m_bAutoLogout;
	BOOL			m_bSequence;
	BOOL			m_bStore;
	DWORD			m_dwSequenceIntervall;
	DWORD			m_dwLastBeep;
	DWORD			m_dwLastStored;

	COEMConnectionDialog* m_pConnectionDlg;
public:
	// Has to be allocated with new and derived from CObject!
};
/////////////////////////////////////////////////////////////////////////////
inline const CServers& CIdipClientDoc::GetServers() const
{
	return m_Servers;
}
/////////////////////////////////////////////////////////////////////////////
inline void CIdipClientDoc::SetTitle(CString sTitle)
{
	m_strTitle = sTitle; 
};
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IdipClientDOC_H__4682526C_6AF6_11D1_93E4_00C095EC9EFA__INCLUDED_)
