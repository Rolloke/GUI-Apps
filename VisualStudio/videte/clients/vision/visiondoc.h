// VisionDoc.h : interface of the CVisionDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_VISIONDOC_H__4682526C_6AF6_11D1_93E4_00C095EC9EFA__INCLUDED_)
#define AFX_VISIONDOC_H__4682526C_6AF6_11D1_93E4_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Servers.h"

#define VDH_INPUT					   0x0001
#define VDH_OUTPUT					0x0002
#define VDH_REMOVE_SERVER			0x0003

#define VDH_ADD_CAMWINDOW			0x0004
#define VDH_REMOVE_CAMWINDOW		0x0005
#define VDH_ACTIVATE_REQUEST_CAM	0x06

#define VDH_INFORMATION				0x0007
#define VDH_INPUT_CONFIRM			0x0008
#define VDH_DELETE_CAMWINDOW		0x0009
#define VDH_INPUT_URL				0x000A
#define VDH_DEACTIVATE_CAMERAS	0x000B
#define VDH_CAMERA_MAP				0x000C
#define VDH_NOTIFICATION_MESSAGE	0x000D
#define VDH_SINGLE_CAM				0x000E
#define VDH_CAM_MAP					0x000F
#define VDH_HOST_MAP					0x0011
#define VDH_AUDIO 					0x0012

#define VDH_FLAGS_SELECTED			0x0100
#define VDH_FLAGS_UPDATE         0x0200
#define VDH_FLAGS_DISCONNECT     0x0400

class CObjectView;
class CVisionView;
class CIPCDataBaseVision;
class CIPCServerControlVision;
class CIPCOutputVisionDecoder;
class CIPCInputVisionCommData;
class CIPCAudioVision;

/////////////////////////////////////////////////////////////////////////////
class CVisionDoc : public CDocument
{
protected: // create from serialization only
	CVisionDoc();
	DECLARE_DYNCREATE(CVisionDoc)

// Attributes
public:
	BOOL		 GetStore();
	BOOL		 GetSequence();
	DWORD		 GetSequenceIntervall();
	CServer*	 GetServer(WORD wServerID);
	CServer*  GetServerFromIndex(int nIndex);
	CServer*	 GetActiveServer();
	CServer*	 GetLocalServer();
	WORD      GetServerIDbyHostID(WORD wHostID, BOOL bConnect);
	CObjectView* GetObjectView();
	CVisionView* GetVisionView();

	CIPCDataBaseVision*	GetDatabase();
	BOOL IsDatabaseConnected();
	
	CIPCOutputVisionDecoder* GetCIPCOutputVisionDecoder();
	BOOL IsDecoderConnected();

	CIPCInputVisionCommData* GetCIPCInputVisionCommData();
	BOOL IsCommDataConnected();

	CIPCAudioVision * GetLocalAudio();
	BOOL IsAudioConnected();

	CIPCServerControlVision* GetCIPCServerControlVision();

	inline COEMConnectionDialog* GetConnectionDlg();

// Operations
public:
	void ActualizeInput(CIPCInputVision* pInput);
	void ActualizeOutput(CIPCOutputVision* pOutput);
	void ActualizeAudio(CIPCAudioVision* pAudio);

	BOOL Connect(CSecID idHostID);
	void Disconnect(WORD wServerID);
	void DisconnectAll();
	void Remove(WORD wServerID);
	void ResetConnectionAutoLogout();

	void PictureData(WORD wServerID, const CIPCPictureRecord &pict, CSecID id);
	void AddConnectionRecord(const CConnectionRecord &c);
	void SetSequence(BOOL bOn);
	void SetSequenceTime(DWORD dwSeconds);
	void UpdateViewRects(BOOL bActive);

	void PictureStored();

	void OnDatabaseConnected();
	void OnDatabaseDisconnected();
	void OnConnectionRecord();

	void CSConnectionDlgLockFunction(BOOL bLock);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVisionDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void OnIdle();
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CVisionDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


// Generated message map functions
protected:
	//{{AFX_MSG(CVisionDoc)
	afx_msg void OnConnectLocal();
	afx_msg void OnUpdateConnectLocal(CCmdUI* pCmdUI);
	afx_msg void OnConnect();
	afx_msg void OnUpdateConnect(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDisconnect(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSequenzePane(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDatabasePane(CCmdUI* pCmdUI);
	afx_msg void OnSequenze();
	afx_msg void OnSequence1();
	afx_msg void OnUpdateSequence1(CCmdUI* pCmdUI);
	afx_msg void OnSequence10();
	afx_msg void OnUpdateSequence10(CCmdUI* pCmdUI);
	afx_msg void OnSequence15();
	afx_msg void OnUpdateSequence15(CCmdUI* pCmdUI);
	afx_msg void OnSequence2();
	afx_msg void OnUpdateSequence2(CCmdUI* pCmdUI);
	afx_msg void OnSequence20();
	afx_msg void OnUpdateSequence20(CCmdUI* pCmdUI);
	afx_msg void OnSequence3();
	afx_msg void OnUpdateSequence3(CCmdUI* pCmdUI);
	afx_msg void OnSequence30();
	afx_msg void OnUpdateSequence30(CCmdUI* pCmdUI);
	afx_msg void OnSequence4();
	afx_msg void OnUpdateSequence4(CCmdUI* pCmdUI);
	afx_msg void OnSequence5();
	afx_msg void OnUpdateSequence5(CCmdUI* pCmdUI);
	afx_msg void OnVideoSaveAll();
	afx_msg void OnUpdateVideoSaveAll(CCmdUI* pCmdUI);
	afx_msg void OnVideoSaveAllStop();
	afx_msg void OnVideoSaveAllToggle();
	afx_msg void OnConnectMap();
	//}}AFX_MSG
public:
	afx_msg void OnUpdateConnectMap(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()


	// implementation
protected:
	static	UINT	ReconnectThreadProc(LPVOID pParam);

			void	StartReconnectThread();
			void	StopReconnectThread();
			void	ResetReconnect();
			void	Reconnect();
			void	ConnectToDatabase();
			void	ConnectToDecoder();
			void	ConnectToCommData();
			void	ConnectToAudio();
			void	AutoLogout();
			void	AlarmConnection(const CConnectionRecord& c);
			void	ControlConnection(const CConnectionRecord& c);
			void	AlarmAnimation();

		// data members
protected:
	CServers					m_Servers;	// connected servers including local one
	CIPCDataBaseVision*			m_pCIPCDataBaseVision;
	CIPCServerControlVision*	m_pCIPCServerControlVision;
	CIPCOutputVisionDecoder*	m_pCIPCOutputVisionDecoder;
	CIPCInputVisionCommData*	m_pCIPCInputVisionCommData;
	CIPCAudioVision   *	m_pCIPCLocalAudio;

	CConnectionRecords			m_aConnectionRecords;
	CWinThread*		m_pReconnectThread;
	CEvent			m_evClose;
	BOOL			m_bRun;
	BOOL			m_bReconnect;
	BOOL			m_bAutoLogout;
	BOOL			m_bSequence;
	BOOL			m_bStore;
	DWORD			m_dwSequenceIntervall;
	DWORD			m_dwLastBeep;
	DWORD			m_dwLastStored;

	DWORD		m_dwLastLocalTryInput;
	DWORD		m_dwLastLocalTryOutput;
	DWORD		m_dwLastLocalTryDatabase;
	DWORD		m_dwLastLocalTryAudio;

	COEMConnectionDialog*	m_pConnectionDlg;
	CCriticalSection		m_csConnectionDlg;
};
/////////////////////////////////////////////////////////////////////////////
COEMConnectionDialog* CVisionDoc::GetConnectionDlg()
{
	return m_pConnectionDlg;
}
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VISIONDOC_H__4682526C_6AF6_11D1_93E4_00C095EC9EFA__INCLUDED_)
