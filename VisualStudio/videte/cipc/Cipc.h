/////////////////////////////////////////////////////////////////////
// FILE: $Workfile: Cipc.h $ from $Archive: /Project/CIPC/Cipc.h $
// VERSION: $Date: 2/09/06 3:27p $ $Revision: 134 $
// LAST:	$Modtime: 2/09/06 3:25p $ by $Author: Rolf.kary-ehlers $
//$Nokeywords:$

#ifndef __CIPC_H__
#define __CIPC_H__

#include "SecID.h"
#include "TimedMessage.h"

// 

// dword
#define CIPC_VERSION	2

//  CIPC master
#define CIPC_MASTER	TRUE
//  CIPC slave
#define CIPC_SLAVE	FALSE

#ifdef _UNICODE
#define CODEPAGE_UNICODE (WORD)0xffff
#endif

/////////////////////////////////////////////////////////////////////
//  state of a <c CIPC>  connection.
//  <mf CIPC.GetIPCState>
enum CIPCState 
{
	CIPC_STATE_INVALID,			// initial state
	CIPC_STATE_WRITE_CREATED,	// write channel created
	CIPC_STATE_READ_FOUND,		// read channel found
	CIPC_STATE_CONNECTED,		// the 'normal' action state
	CIPC_STATE_DISCONNECTED		// between OnRequestDisconnect and reconnect
};
AFX_EXT_CLASS const _TCHAR *NameOfEnum(CIPCState s);
/////////////////////////////////////////////////////////////////////
//  Enumeration of CIPC error codes, used in Do/OnIndicateError
//  <mf CIPC.DoIndicateError> <mf CIPC.OnIndicateError>
enum CIPCError 
{
	CIPC_ERROR_OKAY				=  0, // okay value
	CIPC_ERROR_USER_DEFINED		=  1, // depends on the context. Used if no other value fits.
	CIPC_ERROR_INVALID_GROUP	=  2, // the group id is not valid
	CIPC_ERROR_INVALID_SUBID	=  3, // the sub id is not valid
	CIPC_ERROR_UNHANDLED_CMD	=  4, // reserved for CIPC cmds
	CIPC_ERROR_INVALID_RANGE	=  5, // value ot of range
	CIPC_ERROR_INVALID_VALUE	=  6, // set or get value, unknown key
	CIPC_ERROR_UNKNOWN			=  7, // an invalid error
	CIPC_ERROR_INVALID_USER		=  8, // the user is unknown or invalid
	CIPC_ERROR_INVALID_PERMISSIONS=9, // the permission is unknown or invalid
									  // errorCode 0: unknown code, should be used as standard value // gf 20020828
									  //           1: invalid permission or password wrong 
									  //           2: calling host unknown
	CIPC_ERROR_INVALID_PASSWORD	= 10, // the password is wrong
	CIPC_ERROR_INVALID_HOST		= 11, // the host is unknown
	CIPC_ERROR_UNABLE_TO_CONNECT= 12, // cannot connect
									  // errorCode 0: unknown code, should be used as standard value // gf 20020828
									  //           1: link-modul not found,  
									  //           2: server not found,
									  //		   3: server performs reset 
									  //           4: max clients reached
									  //		   5: writeChannel busy
									  //           6: // gf 20020828 sometimes already used, meaning ambiguous:
														// SocketUnit
															// Wrong version ( atoi(ConnectionRecord::GetOutputShm()) == 0 )
															// No Routing allowed
														// ISDNUnit
															// No Routing allowed
	CIPC_ERROR_INVALID_ARCHIVE	= 13, // invalid archive nr
	CIPC_ERROR_ARCHIVE_FULL		= 14, // archive is full
	CIPC_ERROR_ARCHIVE			= 15, // common archiving error
	CIPC_ERROR_ARCHIVE_SEARCH	= 16, // error during a query
	CIPC_ERROR_CANT_DECODE		= 17, // unable to decode (not supported by hardware)
	CIPC_ERROR_CAMERA_NOT_PRESENT=18, // a camera signal is present errorCode 1 
									  // or not present error code 0
	CIPC_ERROR_FILE_UPDATE		= 19, // error updating of a file
	CIPC_ERROR_GET_FILE			= 20, // error getting a file

	CIPC_ERROR_ARCHIVE_LOCAL_SEARCH=21,// error local database query

	CIPC_ERROR_ACCESS_DENIED	= 22, // access denied
	CIPC_ERROR_BACKUP			= 23, // error during backup
									  // errorCode 1 delete sequence
	
	CIPC_ERROR_BUSY				= 24, // no connection line is busy
	CIPC_ERROR_CANCELLED		= 25, // connection was cancelled by user
	CIPC_ERROR_MEMORY			= 26, // internal memory error
	CIPC_ERROR_INVALID_SERIAL	= 27, // invalid serial number
	CIPC_ERROR_ALIVE_UNCONFIRMED= 28,  // alive command was not confirmed
	CIPC_ERROR_DATA_CARRIER		= 29, //errorcodes defined in DataCarrier
	CIPC_ERROR_TIMEOUT			= 30, //
	CIPC_ERROR_NO_HARDWARE      = 31,
	
	CIPC_ERROR_VIDEO_STREAM_ABORTED = 32, // No more Data comming from a DSP
										  // errorCode 1 means the Video stream resumed.
	
	// NOTE: adding a value here should also be done
	// NOTE: in TOPIC{CIPCError} and in NameOfEnum{CIPCError e) both in Cipc.cpp
};
AFX_EXT_CLASS const _TCHAR *NameOfEnum(CIPCError e);
/////////////////////////////////////////////////////////////////////
// common protocol for all subclasses
// CONNECTION here means that two CIPCs exchanged REQ-->CONF
// receiving a REQ or CONF sets the IPCState to CIPC_STATE_CONNECTED
#define CIPC_BASE_REQ_CONNECTION		11001
#define CIPC_BASE_CONF_CONNECTION		11002
#define CIPC_BASE_INDICATE_ERROR		11004
#define CIPC_BASE_REQUEST_SET_GROUP_ID	11005
#define CIPC_BASE_CONFIRM_SET_GROUP_ID	11006
#define CIPC_BASE_REQUEST_GET_VALUE		11007
#define CIPC_BASE_CONFIRM_GET_VALUE		11008
#define CIPC_BASE_REQUEST_SET_VALUE		11009
#define CIPC_BASE_CONFIRM_SET_VALUE		11014
#define CIPC_BASE_REQUEST_BITRATE		110010
#define CIPC_BASE_CONFIRM_BITRATE		110011
#define CIPC_BASE_REQUEST_VERSION		110012
#define CIPC_BASE_CONFIRM_VERSION		110013

#define CIPC_BASE_REQUEST_ALIVE			11015
#define CIPC_BASE_CONFIRM_ALIVE			11016
/////////////////////////////////////////////////////////////////////
class CIPC;				// forward declaration
class CIPCChannel;		// private member, the structure is hidden
class CIPCExtraMemory;
typedef CIPCExtraMemory * CIPCExtraMemoryPtr;
/////////////////////////////////////////////////////////////////////
//  CIPCCmdRecord | encapsulates the for DWORD parameter of one CIPC command
// and optional shared memory for larger commands.
class AFX_EXT_CLASS CIPCCmdRecord 
{
	//  construction / destruction:
public:
	//!ic! constructor
	CIPCCmdRecord(const CIPCExtraMemory *pExtraMemory,
				 DWORD cmd, 
				 DWORD param1=0, 
				 DWORD param2=0,
				 DWORD param3=0, 
				 DWORD param4=0);
	//!ic! copy constructor
	CIPCCmdRecord(const CIPCCmdRecord &src);
	//!ic! destructor
	virtual ~CIPCCmdRecord();
	
	//  public data:
	//!ic! memory bubble can be null
	CIPCExtraMemory* m_pExtraMemory;
	// command number
	DWORD			 m_dwCmd;
	// parameter 1
	DWORD			 m_dwParam1;
	// parameter 2
	DWORD			 m_dwParam2;
	// parameter 3
	DWORD			 m_dwParam3;
	// parameter 4
	DWORD			 m_dwParam4;
};
/////////////////////////////////////////////////////////////////////
typedef CIPCCmdRecord * CIPCCmdRecordPtr;
WK_PTR_LIST_CS(CIPCCmdRecordListPlain,CIPCCmdRecordPtr,CIPCCmdRecords)
/////////////////////////////////////////////////////////////////////
//  CIPC | Base communication class. To communicate
// you will always need to CIPC derived class objects.
// The first one must be master, the second slave, both
// must have the same shared memory name.
class AFX_EXT_CLASS CIPC
{
	//  construction / destruction:
public:
	//!ic! constructor
	CIPC(LPCTSTR lpSMName, BOOL bMaster);
	//!ic! destructor
	virtual ~CIPC();
	//!ic! stops all communication use this instead of 'delete this' in OnXXX methods.
	void DelayedDelete();	

	//  attributes:
public:
	// returns shared memory name
	inline LPCTSTR GetShmName() const;
	// returns currents state
	inline CIPCState   GetIPCState() const;
	// timeout in ms for command reception
	inline DWORD GetTimeoutCmdReceive() const;
	// timeout in ms for sending commands
	inline DWORD GetTimeoutWrite() const;
	// returns time to wait between retries in ms
	inline DWORD GetConnectRetryTime() const;
	// returns how many timeouts in reception sequence
	inline DWORD GetNumTimeoutsCmdReceive() const;	
	// returns how many timeouts in send sequence
	inline DWORD GetNumTimeoutsWrite() const;	
	// returns TRUE if is DelayedDelete was called
	inline BOOL	 GetIsMarkedForDelete() const;
	// returns TRUE for master object, otherwise FALSE
	inline BOOL	 GetIsMaster() const;
	// returns the reception queue length
	inline int   GetReceiveQueueLength();
	// returns the send queue length
	inline int   GetSendQueueLength();

	//  operations:
public:
			//!ic! starts the CIPC thread for communication use only ONCE!
			void StartThread(BOOL bMultiThreaded=TRUE, int nPriority = THREAD_PRIORITY_NORMAL);	
			//!ic! stops the CIPC thread, no restart possible
			void StopThread();
			//!ic!	virtual run called by CIPC thread allways call base class from derived
	virtual BOOL Run(DWORD dwTimeOut);
			// sets time out for reception in ms
			void SetTimeoutCmdReceive(DWORD dwTimeout);
			// sets time out for sending in ms
			void SetTimeoutWrite(DWORD dwTimeout);
			// sets connect retry time in ms
			void SetConnectRetryTime(DWORD dwSleepTime);
			// establish connection
			void DoRequestConnection();
			// used for error communication dwCmd which caused the error, id if available
			// errorCode CIPCError message, iUnitCode unit specific error
			void DoIndicateError(DWORD dwCmd, CSecID id, 
								 CIPCError errorCode, int iUnitCode,
								 LPCTSTR sErrorMessage=NULL);
	//  unit related
public:
			//!ic! server sets group id of unit
			void DoRequestSetGroupID(WORD wGroupID);
			//!ic! unit confirms sets group id
			void DoConfirmSetGroupID(WORD wGroupID);

	//  time out handling
public:
			//!ic! special, to change state to CIPC_STATE_CREATED
			void DoDisconnect();

			// on timeout sending a command
			// class defined reaction to timeouts, returning TRUE means try again
	virtual BOOL OnTimeoutWrite(DWORD dwCmd);		
			// on timeout receiving a command
			// class defined reaction to timeouts, returning TRUE means try again
	virtual BOOL OnTimeoutCmdReceive();
			// on timeout waiting a command
			// class defined reaction to timeouts, returning TRUE means try again
	virtual void OnWaitFailed(DWORD dwCmd);
			// a virtual command handler was not overwritten
	virtual void OverrideError();
			// a command was received without extra memory, but should had have one
	virtual void OnMissingExtraMemory(DWORD dwCmd);
			// a command couldn't be send
	virtual void OnCommandSendError(DWORD dwCmd);
			// a command couldn't be received
	virtual void OnCommandReceiveError(DWORD dwCmd);

	//  special:
public:
		    // returns the name of CIPC command
	static  const _TCHAR* NameOfCmd(DWORD dwCmd);
		    // returns the system process id of the other application 0 if not connected
		    DWORD	   GetRemoteProcessID() const;	
		    // returns the last received command
		    DWORD	   GetLastCmd() const;

protected:
			// on write channel failed return TRUE if CIPC should retry
	virtual BOOL OnRetryWriteChannel();	

	//  basic protocol actions:
public:
			//!ic! called if other side created it's write channel
			//!ic! call base class or DoRequestConnection
	virtual void OnReadChannelFound();
			//!ic! called if other side request a connection
			//!ic! call base class or DoConfirmConnection
	virtual void OnRequestConnection();
			//!ic! called if other side confirmed a connection
			//!ic! call base class, data transfer can start
	virtual void OnConfirmConnection();
			//!ic! called if other side requests to set
			//!ic! a group id, for unit communication only
	virtual void OnRequestSetGroupID(WORD wGroupID);
			//!ic! called if other side confirms a set
			//!ic! a group id, for unit communication only
	virtual void OnConfirmSetGroupID(WORD wGroupID);
			//!ic! called if other side requests a disconnect
			//!ic! or disappears, stop any data transfer
	virtual void OnRequestDisconnect();
			//!ic! called if other side indicates an error
			//!ic! values and meaning depending on the last command
	virtual void OnIndicateError(DWORD dwCmd, 
							     CSecID id, 
								 CIPCError error, 
								 int iErrorCode,
								 const CString &sErrorMessage);
	//  get/set value:
public:
			//!ic! request get a value
			void DoRequestGetValue(CSecID id,
									const CString &sKey,
									DWORD dwServerData=0);
			//!ic! on request get a value
	virtual void OnRequestGetValue(CSecID id,
								   const CString &sKey,
								   DWORD dwServerData);
			//!ic! confirm get a value
			void DoConfirmGetValue(CSecID id,
									const CString &sKey,
									const CString &sValue,
									DWORD dwServerData);
			//!ic! on confirm get a value
	virtual void OnConfirmGetValue(CSecID id,
									const CString &sKey,
									const CString &sValue,
									DWORD dwServerData);
			//!ic! confirm set a value
			void DoConfirmSetValue(CSecID id,
									const CString &sKey,
									const CString &sValue,
									DWORD dwServerData);
			//!ic! on confirm set a value
	virtual void OnConfirmSetValue(CSecID id,
									const CString &sKey,
									const CString &sValue,
									DWORD dwServerData);
			//!ic! request set a value
			void DoRequestSetValue(CSecID id,
									const CString &sKey,
									const CString &sValue,
									DWORD dwServerData=0);
			//!ic! on request set a value
	virtual void OnRequestSetValue(CSecID id,
									const CString &sKey,
									const CString &sValue,
									DWORD dwServerData);
	//  bitrate:
public:
			//!ic! client requests bitrate of connection to server
			void DoRequestBitrate();
	virtual void OnRequestBitrate();
			void DoConfirmBitrate(DWORD dwBitrate);
	virtual void OnConfirmBitrate(DWORD dwBitrate);

	//  version:
public:
			void DoRequestVersionInfo(WORD wGroupID);
	virtual void OnRequestVersionInfo(WORD wGroupID);
			void DoConfirmVersionInfo(WORD wGroupID, DWORD dwVersion);
	virtual void OnConfirmVersionInfo(WORD wGroupID, DWORD dwVersion);

protected:
	const CIPCCmdRecord *GetActiveCmdRecord() const;

	// Write
	BOOL	WriteCmd(DWORD dwCmd,
				     DWORD dwParam1=0, 
					 DWORD dwParam2=0, 
					 DWORD dwParam3=0, 
					 DWORD dwParam4=0,
					 BOOL bDoSleep=TRUE);
	
	// CAVEAT: len first, data as second arg. This order avoids trouble with a misplaced cmd/len!
	BOOL	WriteCmdWithData(DWORD dwDataLen, 
							 const void *pData,
							 DWORD dwCmd,
							 DWORD dwParam1=0, 
							 DWORD dwParam2=0, 
							 DWORD dwParam3=0, 
							 DWORD dwParam4=0,
							 BOOL bDoSleep=TRUE);
#ifdef _UNICODE
	BOOL WriteCmdWithString(CString,
		                DWORD dwCmd,
		                DWORD dwParam1=0,
							 DWORD dwParam2=0,
							 DWORD dwParam3=0,
							 DWORD dwParam4=0,
							 BOOL bDoSleep=TRUE);
#endif
	
	BOOL	WriteCmdWithExtraMemory(CIPCExtraMemoryPtr& pMemoryReference,
									DWORD dwCmd,
									DWORD dwParam1=0, 
									DWORD dwParam2=0, 
									DWORD dwParam3=0, 
									DWORD dwParam4=0,
									BOOL bDoSleep = TRUE);
	
	// the real action, implemented in each first level subclass
	virtual BOOL HandleCmd(DWORD dwCmd,
						   DWORD dwParam1, DWORD dwParam2,
						   DWORD dwParam3, DWORD dwParam4,
						   const CIPCExtraMemory *pExtraMem);

	void InternalError(LPCTSTR szMessage);	// prepends CIPC %s with GetShmName
	void TraceOutstandingCmds();

private:	// functions
	void CreateWriteChannel();
	void DoConfirmConnection();	// encapsulated in OnRequestConnection
	void OnReceiveCmd();
	
	// queue related:
	void EnableWriteQueue(); // CAVEAT: call before StartThread() and only once
	void ConnectReadChannel();
	void AddToQueue(CIPCCmdRecord *pCmd, BOOL bDoSleep=TRUE);
	//
	// interrupted by StopThread(), returns TRUE if interrupted
	BOOL SpecialSleep(DWORD dwTimeout);
	void WriteCmdIntoChannel(const CIPCCmdRecord &cmd);
	//
	void PreCalcNames();
private:	// data
	HANDLE		m_hShutdownEvent;
	volatile BOOL m_bMarkedForDelete;	// set in DelayedDelete()
	// communication data:
	CString		m_sName;
	BOOL		m_bMaster;
	CIPCState	m_state;
	CIPCChannel	*m_pReadChannel;	
	CIPCChannel *m_pWriteChannel;
	CCriticalSection m_csInWrite;
	CIPCCmdRecord *m_pActiveCmd;
	// process/thread info 
	HANDLE m_hWriteThread;
	//
	DWORD		m_dwLastCmdSend;
	// queue data:
	CIPCCmdRecords m_cmdQueueList;
	CIPCCmdRecords m_receivedCmdsQueue;
	DWORD		   m_dwBytesInSendQueue;

	CCriticalSection m_csQueue;
	HANDLE		m_hQueueSemaphore;
	// thread data:
	friend UINT CIPCThreadFunction(LPVOID pParam);
	CWinThread *m_pThread;
	volatile BOOL m_bThreadRun;
	DWORD		m_dwConnectRetryTime;
	DWORD		m_dwTimeoutWrite;		// timeout to wait for cmdDone before the WriteCmd
	DWORD		m_dwTimeoutCmdReceive;
	DWORD		m_dwNumTimeoutsCmdReceive;	// how many timeouts in sequence
	DWORD		m_dwNumTimeoutsWrite;	// how many timeouts in sequence
	//
	DWORD		m_dwRunCount;

	// messages which, should not be printed too often
	CTimedMessage m_tmErrorIndication;
	CTimedMessage m_tmUnhandledCommand;
	CTimedMessage m_tmInternalError;
	//
	CTimedMessage m_tmZerosReceived;
	int m_iNumZerosReceived;	// only with delta from m_tmZeorsReceived
	//
	BOOL		m_bWriteCreateOkay;
	// pre calculated names for maping and mutex
	// set via PreCalcNames in the constructor
	CString m_sSharedMemoryMaster;
	CString m_sSharedMemorySlave;
	CString m_sMutexMaster;
	CString m_sMutexSlave;
private:
	CIPC(const CIPC& src);				// no implementation, to avoid implicit generation!
	void operator=(const CIPC& src);	// no implementation, to avoid implicit generation!

#ifdef _UNICODE
protected:	
	WORD   m_wCodePage; // The code page for the string data
public:
	WORD GetCodePage() const {return m_wCodePage;}; // returns the code page
	void SetCodePage(WORD);
	void SetCodePage(CString &);
#endif	

};

inline CIPCState CIPC::GetIPCState() const
{
	return (m_state);
}

inline LPCTSTR CIPC::GetShmName() const
{
	return (m_sName);
}

inline BOOL CIPC::GetIsMarkedForDelete() const
{
	return m_bMarkedForDelete;
}

inline BOOL CIPC::GetIsMaster() const
{
	return m_bMaster;
}

inline DWORD CIPC::GetTimeoutWrite() const
{
	return (m_dwTimeoutWrite);
}

inline DWORD CIPC::GetTimeoutCmdReceive() const
{
	return (m_dwTimeoutCmdReceive);
}

inline DWORD CIPC::GetConnectRetryTime() const
{
	return (m_dwConnectRetryTime);
}

inline DWORD CIPC::GetNumTimeoutsWrite () const
{
	return (m_dwNumTimeoutsWrite);
}

inline DWORD CIPC::GetNumTimeoutsCmdReceive () const
{
	return (m_dwNumTimeoutsCmdReceive);
}

inline int CIPC::GetReceiveQueueLength()
{
	int iResult;
	m_receivedCmdsQueue.Lock();
	iResult = m_receivedCmdsQueue.GetCount();
	m_receivedCmdsQueue.Unlock();

	return iResult;
}

inline int CIPC::GetSendQueueLength()
{
	int iResult;
	m_cmdQueueList.Lock();
	iResult = m_cmdQueueList.GetCount();
	m_cmdQueueList.Unlock();

	return iResult;

}

#ifdef _WK_CLASSES_DLL
	AFX_EXT_CLASS void SetCipcCodePage(UINT uCP);
#endif
#endif // __CIPC_H__
