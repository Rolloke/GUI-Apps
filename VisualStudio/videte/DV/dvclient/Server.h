// Server.h: interface for the CServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVER_H__6B5BBEDA_9757_11D3_A870_004005A19028__INCLUDED_)
#define AFX_SERVER_H__6B5BBEDA_9757_11D3_A870_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CIPCDatabaseDVClient;
class CIPCOutputDVClient;
class CIPCInputDVClient;
class CIPCDataCarrierClient;
class CIPCAudioDVClient;
class CServer  
{
	// construction / destruction
public:
	CServer();
	CServer(const CConnectionRecord& c);
	void Init();
	virtual ~CServer();

	// attributes
public:
	inline CIPCDatabaseDVClient*	GetDatabase();
	inline CIPCOutputDVClient*		GetOutput();	
	inline CIPCInputDVClient*		GetInput();
	inline CIPCDataCarrierClient*	GetDataCarrier();
	inline CIPCAudioDVClient*     GetAudio();

	inline const CString&			GetHostNumber();
	inline const CString&			GetHostName();
	
	inline int GetMajorVersion() const;
	inline int GetMinorVersion() const;
	inline int GetSubVersion() const;
	inline int GetBuildNr() const;
	
	inline BOOL CanRectangleQuery() const;
	inline BOOL CanMilliSeconds() const;
	inline BOOL CanAlarmList() const;
	inline BOOL CanActivityMask() const;
	inline BOOL CanAlarmOutput() const;
	inline BOOL CanAlarmCall() const;
	inline BOOL CanMultiCDExport() const;
	inline BOOL HasExpanionCode() const;
	inline BOOL	IsAlarm() const;
	inline WORD GetAlarmCameraNr() const;
	inline BOOL CanSystemInfo() const;

	BOOL IsConnected();
	BOOL IsOutputConnected();
	BOOL IsInputConnected();
	BOOL IsDatabaseConnected();
	BOOL IsDataCarrierConnected();
	BOOL IsAudioConnected();

	BOOL IsOutputConnecting();
	BOOL IsInputConnecting();
	BOOL IsDatabaseConnecting();
	BOOL IsDataCarrierConnecting();
	BOOL IsAudioConnecting();

	BOOL IsInputDisconnected();
	BOOL IsOutputDisconnected();
	BOOL IsDatabaseDisconnected();
	BOOL IsDataCarrierDisconnected();

	BOOL IsLowBandwidth();
	CString GetSupervisorPIN();
	CString GetOperatorPIN();
	CString GetEnteredPIN();
	CString GetInfo();

	// operations
public:
	BOOL IsLocal();
	void Connect(const CString& sHost, const CString& sHostName, const CString& sPIN);
	void AlarmConnection();
	void Disconnect();
	void OnIdle();
	
	void Lock();
	void Unlock();

	void SetVersion(const CString& sVersion);
	BOOL SetSupervisorPIN(const CString& sPIN);
	BOOL SetOperatorPIN(const CString& sPIN);

	BOOL SetExpansionCode(const CString& sExpCode);

	BOOL SetTimerOnOffState(BOOL bState);
	BOOL GetTimerOnOffState(BOOL &bState);

	BOOL SetAlarmOutputOnOffState(BOOL bState);
	BOOL GetAlarmOutputOnOffState(BOOL &bState);

	BOOL SetAlarmDialingNumber(int nCallingNumber, const CString& sCalingNumber);
	BOOL GetAlarmDialingNumber(int nCallingNumber, CString& sCalingNumber);

	void ConfirmGetValue(const CString &sKey,const CString &sValue);
	void ConfirmSetValue(const CString &sKey,const CString &sValue);

	void ConfirmGetSystemTime(const CSystemTime& st);
	void ConfirmSetSystemTime(BOOL bConfirmSetClock);

	BOOL SetClock(const CSystemTime& time);
	BOOL GetClock(CSystemTime& time);

	BOOL GetTimeZoneInformation(CTimeZoneInformation& TimeZone);
	BOOL SetTimeZoneInformation(const CTimeZoneInformation &tzi);
	BOOL GetTimeZoneInformations(CTimeZoneInformations &tzis);

	void ConfirmGetTimeZones(const CTimeZoneInformations& TimeZones);
	void ConfirmSetTimeZone();
	void ConfirmGetTimeZone(const CTimeZoneInformation& TimeZone);

	BOOL GetVideoOutputMode(LPCTSTR sCSDPort, enumVideoOutputMode& eVideoOutputMode);
	BOOL SetVideoOutputMode(LPCTSTR sCSDPort, enumVideoOutputMode& eVideoOutputMode);
	int  GetNrOfVideoOut();

	void ConfirmGetVideoOutputMode(const CString& sKey,const CString& sValue);
	void ConfirmSetVideoOutputMode(const CString& sKey,const CString& sValue);

	BOOL SetResolution(int nResolution);
	BOOL GetResolution(int &nResolution);

	BOOL SetSystemVideoformat(int nVideoformat, int nCamNr);
	BOOL GetSystemVideoformat(int& nVideoformat, int nCamNr);
	BOOL SetSystemVideonorm(CString sVideonorm, int nCamNr);
	BOOL GetSystemVideonorm(CString& sVideonorm, int nCamNr);
	BOOL SetSystemAGC(BOOL bAGC, int nCamNr);
	BOOL GetSystemAGC(BOOL& bAGC, int nCamNr);



	BOOL GetCardType(int nCamNr, int &nCardType);

	BOOL OnBeginConfig();
	BOOL OnEndConfig();
#ifdef _UNICODE
	WORD GetInputCodePage();
	WORD GetOutputCodePage();
	WORD GetDataBaseCodePage();
	WORD GetDataCarrierCodePage();
	WORD GetAudioCodePage();
#endif
// implementation
protected:
	static UINT ConnectThreadProc(LPVOID pParam);

	void Reconnect();
	void ConnectInput(const CString& shmName, CIPCError error);
	void ConnectOutput(const CString& shmName, CIPCError error);
	void ConnectDatabase(const CIPCFetchResult fr);
	void ConnectDataCarrier(const CString& shmName, CIPCError error);
	void ConnectAudio(const CString& shmName, CIPCError error);

	void DisconnectInput();
	void DisconnectOutput();
	void DisconnectDatabase();
	void DisconnectDataCarrier();

	// data member
private:
	// sync objects
	CCriticalSection m_cs;

	// cipc objects
	CConnectionRecord		   m_ConnectionRecord;
	CIPCDatabaseDVClient*	m_pDatabase;
	CIPCOutputDVClient*		m_pOutput;	
	CIPCInputDVClient*		m_pInput;
	CIPCDataCarrierClient*	m_pDataCarrier;
	CIPCAudioDVClient*      m_pAudio;

	// for connection
	CString	m_sHostName;
	CString	m_sHostNumber;
	CString m_sEnteredPIN;
	CString	m_sSupervisorPIN;
	CString	m_sOperatorPIN;
	CString m_sInfo;
	CString	m_sAlarmDialingNumber;
	int		m_nResolution;
	int		m_nSystemAGC;
	int		m_nSystemVideoformat;
	CString	m_sSystemVideonorm;
	int		m_nCardType;

	BOOL	m_bTimerOnOffState;
	BOOL	m_bAlarmOutputOnOffState;
	BOOL	m_bAlarm;	// alarm or normal
	CSystemTime m_SystemTime;
	CTimeZoneInformations m_TimeZones;
	CTimeZoneInformation  m_TimeZone;
	enumVideoOutputMode		m_eVideoOutputMode;
	int	m_iNrOfVideoOut;
	int m_iMajorVersion;
	int m_iMinorVersion;
	int m_iSubVersion;
	int m_iBuildNr;
	
	CEvent	m_evConfirmSetPIN;
	CEvent	m_evConfirmGetPIN;
	CEvent	m_evConfirmGetSystemInfo;
	CEvent	m_evConfirmSetTimerOnOff;
	CEvent	m_evConfirmGetTimerOnOff;
	CEvent	m_evConfirmSetClock;
	CEvent	m_evConfirmGetClock;
	CEvent  m_evConfirmSetTimeZone;
	CEvent  m_evConfirmGetTimeZone;
	CEvent  m_evConfirmGetTimeZones;
	CEvent	m_evConfirmGetVideoOutputMode;
	CEvent	m_evConfirmSetVideoOutputMode;
	CEvent  m_evConfirmSetAlarmOutputOnOff;
	CEvent  m_evConfirmGetAlarmOutputOnOff;
	CEvent	m_evConfirmSetAlarmDialingNumber[3];
	CEvent	m_evConfirmGetAlarmDialingNumber[3];
	CEvent  m_evConfirmSetExpansionCode;
	CEvent	m_evConfirmGetResolution;
	CEvent	m_evConfirmSetResolution;
	CEvent	m_evConfirmGetCardType;
	CEvent  m_evConfirmSetSystemAGC;
	CEvent  m_evConfirmGetSystemAGC;
	CEvent  m_evConfirmSetSystemVideoformat;
	CEvent  m_evConfirmGetSystemVideoformat;
	CEvent  m_evConfirmSetSystemVideonorm;
	CEvent  m_evConfirmGetSystemVideonorm;


	BOOL	m_bConfirmSetClock;

	BOOL				m_bCancelConnect;
	BOOL				m_bConnectThreadRunning;
	DWORD				m_dwReconnectTick;
	CConnectionRecord*  m_pConnectionRecord;	// for connect thread
	CIPCFetch			m_Fetch;
	CIPCFetchResult*	m_pInputResult;
	CIPCFetchResult*	m_pOutputResult;
	CIPCFetchResult*	m_pDatabaseResult;
	CIPCFetchResult*	m_pDataCarrierResult;
	CIPCFetchResult*	m_pAudioResult;
	CWinThread*			m_pConnectThread;

	BOOL				m_bUserForcedConnect;
};
#include "Server.inl"

#endif // !defined(AFX_SERVER_H__6B5BBEDA_9757_11D3_A870_004005A19028__INCLUDED_)
