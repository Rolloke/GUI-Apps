
#ifndef _CConnectionThread_H
#define _CConnectionThread_H

#define VIDETE_CIPC_PORT    1927
#define VIDETE_OLD_PORT		888

#define WM_KICKIDLE       0x036A

#define TM_DUMMY		  WM_USER+3003

#include <afxwin.h>

#include "CIPCPipeSocket.h"
#include "CIPCServerControl.h"
#include "SocketCmdRecord.h"
#include "IPBook.h"

class CDataSocket;
class CSocketUnitDoc;
class CIPCFetchResult;

enum ConnectionThreadState {
	CTS_CREATED=0,
	//
	CTS_ACTIVATED_ACTIVE=1,
	CTS_ACTIVATED_PASSIVE=2,
	//
	CTS_CONNECTED_ACTIVE=3,
	CTS_CONNECTED_ACTIVE_UNCONFIRMED=4,
	CTS_CONNECTED_ACTIVE_CONFIRMED=5,
	//
	CTS_CONNECTED_PASSIVE=6,
	CTS_CONNECTED_PASSIVE_CONFIRMED=7,
	//
	CTS_CONNECTED=8,
	//
	CTS_CLOSING=9,
	//
	CTS_TERMINATING=10,
	//
	CTS_REJECTED=11
};

enum ConnectionState
{
	CONNECTION_OK,
	CONNECTION_ERROR,
	CONNECTION_CHECK,
	CONNECTION_INIT
};

enum SyncSocketState
{
	INIT,
	CONNECTING,
	CONNECTED,
	ERROR_CONNECTING
};

#define SYNC_RETRIES 1

class CConnectionThread : public CWinThread
{
public:
	// construction / destruction
	CConnectionThread(CSocketUnitDoc *pDoc,
						CIPCType ctype,
						LPCTSTR hostName,
						CString sIPAdapter,
						const CConnectionRecord &c
						);
	CConnectionThread(CSocketUnitDoc *pDoc,SOCKET hSocket);
	virtual ~CConnectionThread();

	// overrides
public:
	virtual BOOL InitInstance();
	virtual int  ExitInstance();
	
	// attributes
public:
	inline CSocketUnitDoc *GetDoc() const;
	inline CIPCType GetCIPCType() const;
	inline ConnectionThreadState GetState() const;
	inline CString GetHostName() const;

	CString Format();

	// implementation
public:
	void Sync();
	void HandleCmd(WORD wId1, WORD wId2,
				DWORD dwDataLen, const void *pData,
				DWORD dwCmd,DWORD dwParam1, DWORD dwParam2, 
				DWORD dwParam3, DWORD dwParam4);
	//
	void AddToSendQueue(CSocketCmdRecord *pSendRecord, BOOL bAddTail);
	void OnRequestDisconnect();
	void OnSend();
	void OnClose(int iError);  
	void ResetSync();
	void TRACE_SYNC(CString sMessage,DWORD dwValue=0);
	void CheckSyncRunningTime();
	void DoTerminate();
	void ResetSyncTimer();

	BOOL ThreadsInitDone();

	DWORD m_dwLastSyncCheck;

	CIPCPipeSocket	*m_pPipe, *m_pPipe2;
	CDataSocket		*m_pSocket;
	CSocketCmdRecords m_sendQueue;
	int m_iMaxSendQueue;

	BOOL	m_bInitDone;

	static DWORD m_dwTheSyncRetryTime;
	static int m_iTheVersion;

	// implementation
protected:
	void Send();
	void Init();
	void SendConnectionRecord();
	void HandleIncomingConnectionRecord(const CConnectionRecord &c,WORD wId1,WORD wId2);
	void HandleRemoteFetchResult(CIPCFetchResult &remoteFetchResult,WORD wId1, WORD wId2);
	void SendErrorIndicationToClient(CIPCError errorCode, DWORD dwUnitCode,const CString &sErrorMsg);
	void CloseConnection();

	BOOL HandleVirtualAlarm(BOOL bConnect);
	BOOL CheckIncomingVersion(const CString &sVersion);
	BOOL CheckPermission(CConnectionRecord &c, CIPCFetchResult &fetchResult);

	// Generated message map functions
	//{{AFX_MSG(CMainFrame)
protected:
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


private:
	CString m_sAdapterIP;
	CString m_sThreadName;
	DWORD   m_dwThreadID;
	BOOL m_bDoSendRequested;
	BOOL m_bFirstCMDReceived;

	static WORD m_wTheIdCounter;
	static WORD m_wThePipeIdCounter;
	// init data:
	SOCKET			m_hSocket;

	CConnectionRecord m_connectionRecord;
	CString	m_sHostName;
	WORD	m_wId;
	//	runtime data:
	CSocketUnitDoc	*m_pDoc;
	
	CIPCType		m_pipeType;

	ConnectionThreadState m_state;
	BOOL	m_bActivePeer;
	//
	WORD	m_wPipeId1;	// for all 'normal' connections
	WORD	m_wPipeId2;	// for dual piped AlarmConnections
	//

	SyncSocketState m_iSyncSocketState;
	int   m_iSyncRetryCounter;
	DWORD m_dwDynamicSyncTimeout;
	unsigned long m_lRemoteIP;
	DWORD m_dwLastSyncRunningTime;
	DWORD m_dwSyncSendTime;
	CCriticalSection m_CritTerminate;

	static int m_nOutgoingCall;
	static int m_nIncomingCall;
	static CPtrList m_ConnectedHosts;

	ConnectionState m_ConnectionState;


	friend class CConnectionThreadWnd;
};

inline ConnectionThreadState CConnectionThread::GetState() const
{
	return m_state;
}


typedef CConnectionThread *CConnectionThreadPtr;
WK_PTR_ARRAY_CS(CConnectionThreadArrayPlain, CConnectionThreadPtr, CConnectionThreads)

inline CSocketUnitDoc *CConnectionThread::GetDoc() const
{
	return m_pDoc;
}

inline CIPCType CConnectionThread::GetCIPCType() const
{
	return m_pipeType;
}

inline CString CConnectionThread::GetHostName() const
{
	return m_sHostName;
}



#endif