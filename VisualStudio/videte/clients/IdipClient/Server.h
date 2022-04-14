// Server.h: interface for the CServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVER_H__BC7F5CC2_6BAE_11D1_93E4_00C095EC9EFA__INCLUDED_)
#define AFX_SERVER_H__BC7F5CC2_6BAE_11D1_93E4_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "CIPCInputIdipClient.h"
#include "CIPCOutputIdipClient.h"
#include "CIPCDatabaseIdipClient.h"
#include "CIPCDataCarrierIdipClient.h"
#include "CIPCControl.h"
#include "QueryParameter.h"
#include "QueryResult.h"

class CIdipClientDoc;
class CServers;
class CDlgConnecting;
class CIPCServerControlIdipClient;
class CIPCDataCarrierRecherche;
class CIPCDatabaseIdipClient;
class CIPCAudioIdipClient;
class CQueryResult;
class CDlgDatabaseInfo;

// request states for remote server
#define REQUEST_STATE_NORMAL	0	// unlimited request for high bandwidth
#define REQUEST_STATE_RECOVER	1	// recovery state to reduce data accumulation
#define REQUEST_STATE_LIMITED	2	// limited request for reduced bandwidth

#define RQP_TIMEOUT				5000	// ms, give 5 seconds time to arrive
#define RQP_TIME_OFFSET_NORMAL	4		// s, older than  4 seconds, switch to recovery state
#define RQP_TIME_OFFSET_RECOVER	15		// s, older than 10 seconds, ?
#define RQP_TIME_OFFSET_LIMITED	3		// s, older than  3 seconds, decrease requested fps
#define RQP_TIME_OFFSET_LIM_X	6		// s, older than  6 seconds, switch to recovery state

#define RECOVERY_TIME_COUNT		10		// s, time to recover is 10 seconds
#define INCREASE_TIME_COUNT		30		// s, after 30 seconds try to increase the requested fps

#define MAX_REQ_FPS_LIMITED		200		// do not increase unlimited

#define SERVERID_NOID 0xFFFF
//////////////////////////////////////////////////////////////////////
class CServer  
{
	friend class CDlgConnecting;
	friend class CDlgDatabaseInfo;
	// construction / destruction
public:
	CServer(CIdipClientDoc* pDoc, CServers* pServers, WORD wID);
	virtual ~CServer();

	// attributes
public:
	// Allgemein
	inline	CIdipClientDoc*	GetDocument() const;
	inline	WORD	GetID() const;
	inline	CSecID	GetHostID() const;
	inline	CString	GetName() const;
		   CString	GetFullName() const;
	inline	CString	GetKind() const;
	inline	BOOL	IsLocal() const; 
	inline	BOOL	IsDTS() const;
			BOOL	IsConnected() const;
			BOOL	IsConnectedLive() const;
			BOOL	IsConnectedPlay() const;
			BOOL	IsConnectedInput() const;
			BOOL	IsConnectedOutput() const;
			BOOL	IsConnectedDatabase() const;
			BOOL	IsConnectedAudio() const;
			BOOL	IsConnectingInput() const;
			BOOL	IsConnectingOutput() const;
			BOOL	IsConnectingDatabase() const;
			BOOL	IsConnectingAudio() const;
			BOOL	IsDisconnected() const;
	inline	CString	GetVersion() const;
	inline	int		GetMajorVersion() const;
	inline	int		GetMinorVersion() const;
	inline	int		GetSubVersion() const;
	inline	int		GetBuildNr() const;
	inline	CString	GetError() const;
			BOOL	IsISDN() const;
			BOOL	IsTCPIP() const;
			BOOL	IsB3() const;
			BOOL	IsB6() const;
			BOOL	IsDun() const;
			BOOL	IsLowBandwidth() const;
			BOOL	CanGOP() const;
	inline	BOOL	DoReconnectDataBase() const;
			void	Reset();

	CIPCArchivRecord*	  GetCIPCArchivRecord(WORD wArchiv);

	CIPCSequenceRecord*   GetCIPCSequenceRecord(WORD wArchiv,WORD wSequenceNr);
	CIPCSequenceRecord*   GetNextCIPCSequenceRecord(const CIPCSequenceRecord& s);
	CIPCSequenceRecord*   GetPrevCIPCSequenceRecord(const CIPCSequenceRecord& s);
	CIPCSequenceRecord*   GetFirstSequenceRecord(WORD wArchiveNr);
	CIPCSequenceRecord*   GetLastSequenceRecord(WORD wArchiveNr);

	CIPCSequenceRecords&  GetNewSequences();

	// Vision
	inline	BOOL	IsAlarm() const;
	inline	BOOL	IsControl() const;
	inline	BOOL	IsControlSave() const;
	inline	BOOL	IsControlExclusive() const;
	inline	BOOL	GetAlarmSwitch() const;
	inline	BOOL	ShowErrorMessage() const;
			BOOL	IsControlConnected() const;
			BOOL	IsControlDisconnected() const;
			BOOL	IsConnectionFailed();
			BOOL	IsFetching() const;
	inline	CSecID	GetAlarmID() const;
	inline	CString	GetNotificationURL() const;
	inline	CString	GetNotificationMessage() const;
	inline	DWORD	GetConfirmedOutputOptions() const;

	inline	CIPCInputIdipClient*	GetInput() const;
	inline	CIPCOutputIdipClient*	GetOutput() const;
	inline	CIPCControl*			GetControl() const;
			CIPCAudioIdipClient*	GetAudio() const;

			BOOL	CanActivityMask() const;
			BOOL	Can_DV_H263_QCIF() const;
			BOOL	IsMultiCamera() const;
			BOOL	CanAudio() const;
	int		GetMaxFPS();

	// Recherche only
	inline	CIPCDatabaseIdipClient*		GetDatabase() const;
	inline	CDlgDatabaseInfo*			GetInfoDialog() const;
	inline	BOOL CanRectangleQuery() const;
	inline	BOOL CanSplittingBackup() const;
	inline	BOOL	IsSearching() const;
			CString	GetArchiveName(WORD wArchiv);
			CString GetFieldName(const CString& sField);
	inline	const CIPCFields& GetFields() const;
	inline	CQueryArchives& GetQueryArchives();
	inline	CIPCDrives& GetDrives();
	inline	BOOL IsInBackupMode() const;

	// implementation
public:
	// Allgemein
	void	SetVersion(const CString& sVersion);
	inline	void SetDTS(BOOL bIsDTS);
	void	Connect(const CString& sName, const CString& sNumber, CSecID idHost, DWORD dwTimeOutInMS, LPCTSTR pszPin=NULL);
	BOOL	Disconnect();
	void	ResetConnectionAutoLogout();
	void	CancelConnecting();
	void	Lock(LPCTSTR sFkt=NULL);
	void	Unlock();
	void	OnIdle();
	void	CloseConnectingDlg();
	void	DeleteDataBaseObject();
	// Vision
	BOOL	AlarmConnection(const CConnectionRecord& c,CIPCServerControlIdipClient* pControl);
	BOOL	ControlConnection(const CConnectionRecord& c);
	BOOL    FindOutAlarmHost();
	void	SetAlarmSwitch(BOOL bOn);
	void	SetAlarmID(CSecID id);
	void	SetCIPCAudio(CIPCAudioIdipClient*pAudio) {m_pAudio = pAudio;}
	void	OnInputConnected();
	void	OnInputDisconnected();
	void	OnOutputConnected();
	void	OnOutputDisconnected();
	void	OnControlConnected();
	void	OnControlDisconnected();
	void	OnAudioConnected();
	void	OnAudioDisconnected();
	void	OnInputFetchResult();
	void	OnOutputFetchResult();
	void	OnAudioFetchResult();
	void	OnDatabaseFetchResult();
	void	OnConnectThreadEnd();
	// Recherche
	void    OnConfirmFieldInfo(int iNumRecords, const CIPCField data[]);

	void	AddNewArchiv(const CIPCArchivRecord& data);
	void	UpdateArchiv(const CIPCArchivRecord& data);

	void	AddNewSequences(int iNumRecords, const CIPCSequenceRecord data[]);
	BOOL	AddNewSequence(const CIPCSequenceRecord& seq,WORD  wPrevSequenceNr, DWORD dwNrOfRecords);
	void	RemoveSequence(WORD wArchivNr, WORD wSequenceNr);
	void	RemoveArchiveSequences(WORD wArchivNr);
	void	AddDeleteArchivFile(WORD wArchiv, WORD wSequence);
	void	OpenSequence(WORD wArchiv, WORD wSequence, DWORD dwRecordNr = 1);
	void	DeleteArchivFiles();
	void    InitFields3x();
	void	RequestRecord(WORD wArchiv,  WORD wSequence,  DWORD dwCurrentRecord,
							DWORD dwNewRecord, DWORD dwCamID);
	void	StartSearch();
	void	StopSearch();
	void    ClearSearch();
	void	StopSearchCopy();
	void	ShowInfoDialog();
	void	CheckDatabaseConnection();
	void	UpdateTimeOffset();

	void OnConfirmDrives(int iNumDrives, const CIPCDrive drives[]);
	void AddErrorMessage(const CString& sMessage);
	void AddQueryResult(DWORD dwUserID, WORD wArchivNr,WORD wSequenceNr, DWORD dwRecordNr,
		int iNumFields, const CIPCField fields[]);
	BOOL IsQueryResult(WORD wArchivNr,WORD wSequenceNr, DWORD dwRecordNr);
	BOOL CopyFirstQueryResult();
	BOOL CopyNextQueryResult(WORD wArchivNr,WORD wSequenceNr, DWORD dwRecordNr);
	void SetBackupMode(BOOL bBackup);

protected:
	// Allgemein
	static UINT		ConnectThreadProc(LPVOID pParam);
	UINT			ConnectThread();
	static CString	GetErrorMessage(const CIPCFetchResult& fr);
	// Vision
	void ConnectInput();
	void ConnectOutput();
	void ConnectAudio();
	void ConfirmControlConnection();
	void AddAlarmID(CSecID id);
	void DeleteRecentAlarm(CSecID id);
	// Recherche
	void ConnectDatabase();
	void	DoRequestQueryResult(CQueryResult* pQueryResult,WORD wArchivNr,WORD wSequenceNr, DWORD dwRecordNr);
	void DoGetWWWDir();

	// data member
protected:
	// allgemein
	CIdipClientDoc*		m_pDoc;			// application single doc ref
	CServers*			m_pServers;		// server array ref
	WORD				m_wID;			// unique id in IdipClient
	bool				m_bLocal;		// local or remote
	bool				m_bIsDTS;
	CSecID				m_idHost;		// unique Host id System wide
	CString				m_sName;		// Name of Host
	CString				m_sKind;		// ISDN / Net / B3
	CAutoLogout			m_AutoLogout;	// my auto disconnect
	CCriticalSection	m_csSection;
	CString				m_sLastError;
	CString				m_sVersion;
	short				m_iMajorVersion;
	short				m_iMinorVersion;
	short				m_iSubVersion;
	short				m_iBuildNr;
	HWND				m_hDlgConnecting;		// to cancel connect thread
	CIPCFetch			m_Fetch;
	CConnectionRecord*  m_pConnectionRecord;	// for connect thread
	CConnectionRecord   m_cAlarmConnectionRecord;
	CWinThread*			m_pConnectThread;
	BOOL				m_bConnectingCancelled;
	static int			m_iSerialCounter;

	// Vision
	CString				m_sNumber;	// Number of Host
	CString				m_sNotificationURL;
	CString				m_sNotificationMessage;
	DWORD				m_dwConfirmedOutputOptions;
	bool				m_bAlarm;	// alarm or normal
	bool				m_bAlarmSwitch;	// automatic alarm cam switch
	bool				m_bControlConfirmed;
	bool				m_bShowErrorMessage;
	CSecID				m_AlarmID;	// alarm cam or melder
	CSecIDArray			m_AlarmIDs;	// array for alarm ids history for 1plus switching
	CIPCInputIdipClient*	m_pInput;		// the input connection
	CIPCOutputIdipClient*	m_pOutput;		// the output connection
	CIPCControl*		m_pControl;		// the gemos connection
	CIPCAudioIdipClient	*m_pAudio;	// the audio connection
	CIPCFetchResult*	m_pInputResult;
	CIPCFetchResult*	m_pOutputResult;
	CIPCFetchResult*	m_pAudioResult;

	// Recherche
	CIPCDatabaseIdipClient*	m_pDatabase;	// the database connection
	CIPCFetchResult*		m_pDatabaseResult;
	DWORD					m_dwQueryID;
	CIPCFields				m_Fields;
	CDlgDatabaseInfo*		m_pDlgDatabaseInfo;
	CIPCSequenceRecords		m_srSequences;
	CIPCSequenceRecords		m_srNewSequences;

	CDWordArray				m_arToDelete;
	CIPCArchivRecordArray	m_arNewArchivs;
	CIPCArchivRecordArray	m_arUpdatedArchivs;

	CQueryArchives			m_QueryArchives;
	CQueryResultArrayCS		m_QueryResultsToCopy;
	CIPCDrives				m_Drives;
	CStringArray			m_saErrorMessages;
	int						m_iQueryResultsCopied;
	bool					m_bSearching;
	bool					m_bFirstQueryResult;
	bool					m_bStopSearchCopy;
	bool					m_bBackupMode;
	bool					m_bIsPINDlgOpen;
	bool					m_bDoReconnectDataBase;
	bool					m_bControlSave;
	bool					m_bControlExclusive;
	DWORD					m_dwQueryRecordNr;
	WORD					m_wQueryArchiveNr;
	WORD					m_wQuerySequenceNr;
public:					//	network performance
	// the network performance is bound to a server object
	// different objects need to have access, so they are public
	INT16					m_nRemoteTimeOffset;
	INT16					m_nMaxPictureTimeOffset;
	short					m_nRequestState;
	short					m_nStateCounter;
	short					m_nMaxRequestedFps;
	short					m_nMaxWith25Fps;
	short					m_nMaxWith5Fps;
	short					m_nMaxWith1Fps;

#ifdef _DEBUG
private:
	CString m_sCurrentThread;
	UINT	m_nCurrentThread;
	HANDLE	m_hCurrentThread;
#endif
};
#include "server.inl"

#endif // !defined(AFX_SERVER_H__BC7F5CC2_6BAE_11D1_93E4_00C095EC9EFA__INCLUDED_)
