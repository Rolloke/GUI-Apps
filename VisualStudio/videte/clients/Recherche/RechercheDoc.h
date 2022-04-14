// RechercheDoc.h : interface of the CRechercheDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_RECHERCHEDOC_H__517E1F00_9D66_11D1_8C1B_00C095EC9EFA__INCLUDED_)
#define AFX_RECHERCHEDOC_H__517E1F00_9D66_11D1_8C1B_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#define VDH_DATABASE					0x01
#define VDH_ADD_FILE					0x02
#define VDH_REMOVE_SERVER			0x03
#define VDH_REMOVE_FILE				0x04
#define VDH_OPEN_FILE				0x05
#define VDH_ADD_ARCHIV				0x06
#define VDH_DEL_ARCHIV				0x07
#define VDH_ADD_SEQUENCE			0x08
#define VDH_OPEN_SEQUENCE			0x09

#define VDH_BACKUP					0x0A
#define VDH_BACKUP_ADD_SEQUENCE	0x0B
#define VDH_BACKUP_DEL_SEQUENCE	0x0C
#define VDH_BACKUP_ADD_ARCHIVE	0x0D
#define VDH_BACKUP_DEL_ARCHIVE	0x0F

#define VDH_SYNC_DEL_ARCHIVE		0x10
#define VDH_SELECT_SEQUENCE		0x11

#include "Servers.h"

class CObjectView;
class CRechercheView;
class CIPCDatabaseRecherche;
class CIPCAudioRecherche;

class CRechercheDoc : public CDocument
{
protected: // create from serialization only
	CRechercheDoc();
	DECLARE_DYNCREATE(CRechercheDoc)

// Attributes
public:
	inline const CServers& GetServers() const;
	CServer*	 GetServer(WORD wServerID);
	CServer*	 GetLocalServer();
	CObjectView* GetObjectView();
	CRechercheView* GetRechercheView();

// Operations
public:
	void Connect(CSecID idHostID);
	void Disconnect(WORD wServerID);
	void DisconnectAll();
	void DisconnectRemote();
	void Remove(WORD wServerID);
	void ActualizeDatabase(CIPCDatabaseRecherche* pDatabase);
	void DeleteArchivFiles();
	void StartSearch();
	void StopSearch();
	void StopSearchCopy();
	void NoLocalConnection();
	void ResetConnectionAutoLogout();
	void UpdateMyViews(CView* pSender, LPARAM lHint, CObject* pHint);
	void ConnectToAudio();

	//media (audio) implementations
	CIPCAudioRecherche* GetLocalAudio();
	BOOL IsAudioConnected();

	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRechercheDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void OnIdle();
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRechercheDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	void AutoLogout();

// Generated message map functions
protected:
	//{{AFX_MSG(CRechercheDoc)
	afx_msg void OnConnect();
	afx_msg void OnConnectLocal();
	afx_msg void OnUpdateConnectLocal(CCmdUI* pCmdUI);
	afx_msg void OnUpdateConnect(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDisconnect(CCmdUI* pCmdUI);
	afx_msg void OnConnectMap();
	afx_msg void OnUpdateConnectMap(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

		// data members
private:		
	CServers				m_Servers;	// connected servers including local one
	BOOL					m_bLocalConnection;
	DWORD					m_dwLastLocalTry;
	CIPCAudioRecherche*	m_pCIPCLocalAudio;
	DWORD					m_dwLastLocalTryAudio;
};

/////////////////////////////////////////////////////////////////////////////
inline const CServers& CRechercheDoc::GetServers() const
{
	return m_Servers;
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RECHERCHEDOC_H__517E1F00_9D66_11D1_8C1B_00C095EC9EFA__INCLUDED_)
