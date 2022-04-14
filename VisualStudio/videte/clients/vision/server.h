// Server.h: interface for the CServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVER_H__BC7F5CC2_6BAE_11D1_93E4_00C095EC9EFA__INCLUDED_)
#define AFX_SERVER_H__BC7F5CC2_6BAE_11D1_93E4_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "CIPCInputVision.h"
#include "CIPCOutputVision.h"
#include "IPCControl.h"

class CVisionDoc;
class CServers;
class CConnectingDialog;
class CIPCServerControlVision;

class CIPCAudioVision;

#define SERVERID_NOID 0xFFFF
//////////////////////////////////////////////////////////////////////
class CServer  
{
	// construction / destruction
public:
	CServer(CVisionDoc* pDoc, CServers* pServers, WORD wID);
	virtual ~CServer();

	// attributes
public:
	inline WORD		GetID() const;
	inline CSecID	GetHostID() const;
	inline CString	GetName() const;
		   CString	GetFullName() const;
	inline CString	GetKind() const;
	inline BOOL		IsLocal() const; 
	inline BOOL		IsAlarm() const;
	inline BOOL		IsControl() const;
	inline BOOL		IsControlSave() const;
	inline BOOL		IsControlExclusive() const;
	inline BOOL		IsDTS() const;
	inline BOOL		GetAlarmSwitch() const;
	inline BOOL		ShowErrorMessage() const;
		   BOOL		IsConnected() const;
		   BOOL		IsDisconnected() const;
		   BOOL		IsControlConnected() const;
		   BOOL		IsControlDisconnected() const;
		   BOOL		IsConnectionFailed();
		   BOOL		IsFetching() const;
	inline CString	GetError() const;
	inline CSecID	GetAlarmID() const;
	inline CString	GetNotificationURL() const;
	inline CString	GetNotificationMessage() const;
	inline CString	GetVersion() const;
	inline DWORD	GetConfirmedOutputOptions() const;

	inline CIPCInputVision*	 GetInput() const;
	inline CIPCOutputVision* GetOutput() const;
	inline CIPCControl*		 GetControl() const;
	CIPCAudioVision*	 GetAudio() const;
	inline CVisionDoc*		 GetDocument() const;

		   BOOL IsISDN() const;
		   BOOL IsTCPIP() const;
		   BOOL IsB3() const;
		   BOOL IsB6() const;
		   BOOL IsDun() const;
		   BOOL CanActivityMask() const;
		   BOOL Can_DV_H263_QCIF() const;
		   BOOL IsMultiCamera() const;
		   BOOL CanAudio() const;

	// operations
public:
	inline void SetDTS(BOOL bIsDTS);
	void	CheckAutoLogout();
	BOOL	Connect(const CString& sName,
					const CString& sNumber,
					CSecID idHost,
					DWORD dwTimeOutInMS);
	BOOL	AlarmConnection(const CConnectionRecord& c,CIPCServerControlVision* pControl);
	BOOL	ControlConnection(const CConnectionRecord& c);
	BOOL    FindOutAlarmHost();
	BOOL	Disconnect(BOOL bAsk);
	void	ResetConnectionAutoLogout();
	void	SetAlarmSwitch(BOOL bOn);
	void	SetAlarmID(CSecID id);
	void	CancelConnecting();

	void	PictureData(const CIPCPictureRecord &pict, CSecID id);
	void	SetVersion(const CString& sVersion);
	void  SetCIPCAudio(CIPCAudioVision*pAudio) {m_pAudio = pAudio;}
	
	void	Lock();
	void	Unlock();

	void OnInputConnected();
	void OnInputDisconnected();
	void OnOutputConnected();
	void OnOutputDisconnected();
	void OnControlConnected();
	void OnControlDisconnected();
	void OnAudioConnected();
	void OnAudioDisconnected();

	void OnInputFetchResult();
	void OnOutputFetchResult();
	void OnAudioFetchResult();
	void OnConnectThreadEnd();

	// implementation
protected:
	static UINT		ConnectThreadProc(LPVOID pParam);
	static CString	GetErrorMessage(const CIPCFetchResult& fr);

	void ConnectInput();
	void ConnectOutput();
	void ConnectAudio();
	void ConfirmControlConnection();

	void AddAlarmID(CSecID id);
	void DeleteRecentAlarm(CSecID id);

	// data member
protected:
	WORD				m_wID;		// unique id in Vision
	CSecID				m_idHost;	// unique Host id System wide
	CString				m_sName;	// Name of Host
	CString				m_sNumber;	// Number of Host
	CString				m_sKind;	// ISDN / Net / B3
	BOOL				m_bLocal;	// local or remote
	BOOL				m_bAlarm;	// alarm or normal
	BOOL				m_bAlarmSwitch;	// automatic alarm cam switch
	BOOL				m_bIsDTS;
	BOOL				m_bControlConfirmed;
	BOOL				m_bShowErrorMessage;
	CSecID				m_AlarmID;	// alarm cam or melder
	CSecIDArray			m_AlarmIDs;	// array for alarm ids history for 1plus switching

	CString				m_sNotificationURL;
	CString				m_sNotificationMessage;
	DWORD				m_dwConfirmedOutputOptions;

	CVisionDoc*			m_pDoc;			// application single doc ref
	CServers*			m_pServers;		// server array ref
	CAutoLogout			m_AutoLogout;	// my auto disconnect

	CIPCInputVision*	m_pInput;		// the input connection
	CIPCOutputVision*	m_pOutput;		// the output connection
	CIPCControl*		m_pControl;		// the gemos connection
	CIPCAudioVision	*m_pAudio;	// the audio connection

	BOOL			    m_bControlSave;
	BOOL				m_bControlExclusive;

	CCriticalSection	m_csSection;
	CString				m_sLastError;
	CString				m_sVersion;

	CConnectingDialog*	m_pConnectDialog;		// to cancel connect thread
	CIPCFetch			m_Fetch;
	CConnectionRecord*  m_pConnectionRecord;	// for connect thread
	CIPCFetchResult*	m_pInputResult;
	CIPCFetchResult*	m_pOutputResult;
	CIPCFetchResult*	m_pAudioResult;
	CWinThread*			m_pConnectThread;
	BOOL				m_bConnectingCancelled;
	static int	m_iSerialCounter;

	friend class CConnectingDialog;
};
#include "server.inl"

#endif // !defined(AFX_SERVER_H__BC7F5CC2_6BAE_11D1_93E4_00C095EC9EFA__INCLUDED_)
