/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: Cipc.cpp $
// ARCHIVE:		$Archive: /Project/CIPC/Cipc.cpp $
// CHECKIN:		$Date: 9.06.06 14:11 $
// VERSION:		$Revision: 371 $
// LAST CHANGE:	$Modtime: 9.06.06 14:10 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$
/*
Description:

About sizes:<p>
When you do not want the size of the file to change 
(for example, when mapping read-only files),  call CreateFileMapping 
and specify zero for both dwMaximumSizeHigh and dwMaximumSizeLow. 
Doing this creates a file-mapping object exactly the same size 
as the file. Otherwise, you must calculate or estimate the size of the 
finished file because file-mapping objects are static in size; 
once created they cannot grow or shrink. 

About closing:<p>
a) A shared file-mapping object will not be destroyed 
until all processes that use it close their handles to it 
by using CloseHandle and call UnMapViewOfFile on all file views.

b) When a process is finished using a file mapping and has umapped all views, 
it must close the file mapping object's handle by calling CloseHandle.

<p><p>
The class CIPC handles interprocess communication. Two processes
create instances of CIPC derived classes, which communicate using shared
memory. CIPC instances can run in a separate thread, or are continuously
called from the main thread. Most time a CIPC thread spends in waiting
for several events:
- a cmd is received from a transmitter
- a receiver has executed a cmd
- a transmitter has terminated or signaled a disconnect

See <LINK CIPC::Run@DWORD, CIPC::Run> for a detailed description.

CIPC encapsulates the management of the shared memory and the details of
the communication. It offers <b> non-virtual </b> <i>DoXXX</i> functions 
to send commands, for example <i>DoRequestReset</i>.

And <b> virtual </b>  OnXXX functions, which are called
if a cmd is received OnRequestReset. This way subclasses only have to overwrite
the required virtual OnXXX functions. without worrying about the
parameter passing in the shared memory.

Most calls have the following form:
- transmitter: sends DoRequestXXX
- receiver:    OnRequestXXX is called
- receiver:    sends DoConfirmXXX
- transmitter: OnConfirmXXX is called

Subclasses:
- CIPCInput
- CIPCOutput
- CIPCDatabase
- CIPCServerControl

The subclasses inherit the basic communication abilities and add
their own specialized communication functions, for example
DoRequestSelectCamera, OnRequestSelectCamera etc. . Currently
both sides of the communication are within one class. So subclasses
from these subclasses
will only override the half of the OnXXX functions. For units
these are the OnRequestXXX and OnResponseXXX functions.


* CIPCState,overview *

CIPCState is an enumeration for the state of a CIPC connection.

  The following values are defined:
- <i> CIPC_STATE_INVALID </i>, the reset value from the constructor
- <i> CIPC_STATE_WRITE_CREATED </i>, set if the constructor was successful
- <i> CIPC_STATE_READ_FOUND<ni>, set if a read channel was found
- <i> CIPC_STATE_CONNECTED </i>, set if the initial communication took place
- (<i> CIPC_STATE_DISCONNECTED </i>, set if a disconnect is signaled)
<p>
Initially a instance of <LINK CIPC, CIPC> has the state CIPC_STATE_INVALID.
The constructor tries to create the required data structures for
its write channel (shared memory, events etc.). If it succeeds
the state advances to CIPC_STATE_WRITE_CREATED. 
<p><p>
While in the state CIPC_STATE_WRITE_CREATED the threads actions is
to look for a read channel.
If found the state advances to CIPC_STATE_READ_FOUND and
<LINK CIPC::OnReadChannelFound, OnReadChannelFound> is called.
<p><p>
Once CIPC_STATE_READ_FOUND is reached, everything is ready 
for communication. The first communication of the protocol is:
- one side issues a <LINK CIPC::DoRequestConnection, DoRequestConnection>
- the other sides receives the request, 
-   advances to CIPC_STATE_CONNECTED
-   and calls <LINK CIPC::OnRequestConnection, OnRequestConnection>, which has the default action
to send 'ConfirmConnection' back.
- the first side receives the confirmation, advances to CIPC_STATE_CONNECTED and calls
<LINK CIPC::OnConfirmConnection, OnConfirmConnection>.

The final communication is a disconnect. Either directly signaled by the
<LINK CIPC::~CIPC, Destructor> of the writer, 
or recognized if thread or process of the writer have terminated. 
For a short time the state changes to CIPC_DISCONNECTED 
and <LINK CIPC::OnRequestDisconnect, OnRequestDisconnect> 
is called. After that the state
CIPC_STATE_WRITE_CREATED is entered again, and the search for a 
read channel restarts.

 <p>
It is NOT YET possible to do something like <i> delete this </i>
in any OnXXX function.

<p>
* CIPCError overview *
<p>
The following standard errors are defined:

- CIPC_ERROR_USER_DEFINED 
- CIPC_ERROR_INVALID_GROUP an invalid groupID was used.
- CIPC_ERROR_INVALID_SUBID an invalid subID was used.
- CIPC_ERROR_UNHANDLED_CMD a CIPC cmd was not handled.
- CIPC_ERROR_INVALID_RANGE, some value was not within a valid range, especially in the context of Get/SetValue.
- CIPC_ERROR_INVALID_VALUE, reserved for Get/SetValue.
An invalid key was given.
- CIPC_ERROR_UNKNOWN an unknown error happend.
- CIPC_ERROR_INVALID_USER
- CIPC_ERROR_INVALID_PERMISSIONS
- CIPC_ERROR_INVALID_PASSWORD
- CIPC_ERROR_INVALID_HOST
- CIPC_ERROR_UNABLE_TO_CONNECT the unit code specifies the reason
<i>1 link-module not found<p>
   2 server not found<p>
   3 server performs reset<p>
   4 max clients reached</i>
- CIPC_ERROR_INVALID_ARCHIVE
- CIPC_ERROR_ARCHIVE_FULL
- CIPC_ERROR_ARCHIVE
- CIPC_ERROR_ARCHIVE_SEARCH
- CIPC_ERROR_CANT_DECODE
- CIPC_ERROR_CAMERA_NOT_PRESENT
<i>0: camera not present<p>
   1: camera back again
- CIPC_ERROR_DATA_CARRIER
- CIPC_ERROR_VIDEO_STREAM_ABORTED
<i>0: video stream not present<p>
   1: video stream back again

<TABLE>
Value or subcode               \description
-----------------------------  -----------------------------------
CIPC_ERROR_UNABLE_TO_CONNECT   the unit code specifies the reason
subcode 1                      link-module not found
subcode 2                      server not found
subcode 3                      server performs reset
subcode 4                      max clients reached
CIPC_ERROR_INVALID_ARCHIVE     NYD
CIPC_ERROR_ARCHIVE_FULL        NYD
CIPC_ERROR_ARCHIVE             NYD
</TABLE>

<LINK CIPC::DoIndicateError, DoIndicateError>, <LINK CIPC::OnIndicateError, OnIndicateError>
<LINK CIPC::NameOfCmd, ErrorName>


* CIPC,threads *
<b> Related topic: </b>
<LINK CIPC; CIPC>, <LINK CIPC::StartThread@BOOL@int, StartThread>, <LINK CIPC::StopThread, StopThread>,
<LINK CIPC::~CIPC, destructor>

<p>
CIPC's main action is handled by the <LINK CIPC::Run@DWORD, Run> function.
<p>
It's not possible to modify MFCDisplayObjects outside of the main thread.
Thus you should pass messages to the main thread, or call Run 
in the OnIdle function of the main thread. This way all OnXXX functions are within
the main thread.
*/

#include "stdcipc.h"

#include "wk.h"
#include "Cipc.h"
#include "CipcChannel.h"
#include "CipcExtraMemory.h"
#include "WK_Names.h"

#include "wkclasses\wk_utilities.h"

#ifndef _WK_CLASSES_DLL
#include "wkclasses\WK_ThreadPool.h"
extern CWK_ThreadPool g_ThreadPool;
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////
#define CIPC_MAX_QUEUE	1000

//////////////////////////////////////////////////////////////////////////////
class CQueueRecord 
{
public:
	CQueueRecord(DWORD dwTick,DWORD dwWriteQueue) {
		m_dwTick=dwTick;
		m_dwWriteQueue = dwWriteQueue;
	}
	DWORD m_dwTick;
	DWORD m_dwWriteQueue;
};

//////////////////////////////////////////////////////////////////////////////
enum MyInternalError
{
	MIE_UNKNOWN = 1,
	MIE_WRITE_PROCESS=2,
	MIE_QUEUE_OVERFLOW=3
};

// for NameOfCmd all cmd defines have to be known
#include "CipcInput.h"
#include "CipcOutput.h"
#include "CipcServerControl.h"
#include "CipcDataBase.h"
#include "CipcMedia.h"

//////////////////////////////////////////////////////////////////////////////
//
// PART: CIPCCmdRecord
//
CIPCCmdRecord::CIPCCmdRecord( const CIPCExtraMemory *pExtraMemory,
						DWORD cmd, 
						DWORD param1, DWORD param2,	
						DWORD param3, DWORD param4
		)
{
	m_dwCmd = cmd;
	m_dwParam1=param1;
	m_dwParam2=param2;
	m_dwParam3=param3;
	m_dwParam4=param4;
	m_pExtraMemory = NULL;

	if (pExtraMemory) 
	{
		m_pExtraMemory = new CIPCExtraMemory(*pExtraMemory);
	}
}
//////////////////////////////////////////////////////////////////////////////
// copy constructor
CIPCCmdRecord::CIPCCmdRecord(const CIPCCmdRecord &src)
{
	m_dwCmd = src.m_dwCmd;
	m_dwParam1= src.m_dwParam1;
	m_dwParam2= src.m_dwParam2;
	m_dwParam3= src.m_dwParam3;
	m_dwParam4= src.m_dwParam4;
	m_pExtraMemory = NULL;

	if (src.m_pExtraMemory) {
		m_pExtraMemory=new CIPCExtraMemory(*src.m_pExtraMemory);
	}
}
//////////////////////////////////////////////////////////////////////////////
CIPCCmdRecord::~CIPCCmdRecord()
{
	WK_DELETE(m_pExtraMemory);
}

//////////////////////////////////////////////////////////////////////////////
//
// PART CIPC
//
//////////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : CIPC
 Function   : CIPC
 Description: Initializes the CIPC Object.
              The initial state is CIPC_STATE_INVALID, on succes it is
              CIPC_STATE_WRITE_CREATED.
              After the constructor a call to <mf CIPC.StartThread> has to be made.
              Even for the single thread variants.
              
              With multithreading the < LINK CIPC::~CIPC, destructor> and 
              <LINK CIPC::StopThread, StopThread> may only be called from outside of the
				  CIPC-thread. That should be the thread, which called the constructor.
              
              If 5 zero cmds are received, the connection is disconnected.
 See also:
	<LINK CIPC, overview>, <LINK CIPC::GetIPCState@const, GetIPCState>, <LINK CIPC::~CIPC, destructor>

 Parameters:   
  shmName: (E): The name of the connection. Make sure they are the same on both sides of the connection!  (LPCTSTR)
  bMaster: (E): Used internally to assign the order of channels. Units have to set <i> bAsMaster=FALSE </i>  (BOOL)

 Result type:  (None)
 created: September, 26 2003
 <TITLE CIPC>
*********************************************************************************************/
CIPC::CIPC(LPCTSTR shmName, BOOL bMaster)
{
	m_bWriteCreateOkay = FALSE;	// for sub-state in CIPC_STATE_WRITE_CREATED
	m_hShutdownEvent=NULL;
	m_bMarkedForDelete=FALSE;
	m_iNumZerosReceived=0;
	m_state = CIPC_STATE_INVALID;
	m_bMaster		= bMaster;
	m_pReadChannel	= NULL;
	m_pWriteChannel = NULL;
	m_hQueueSemaphore = NULL;
	m_dwBytesInSendQueue = 0;
	//
	m_pActiveCmd = NULL;
	//
	m_hWriteThread=NULL;
	//
	m_dwLastCmdSend=0;
	//
	m_dwConnectRetryTime = 250;
	m_dwTimeoutCmdReceive = 9876;
	m_dwTimeoutWrite = 9876;
	// thread related data
	m_pThread	= NULL;
	m_bThreadRun= FALSE;
	m_dwNumTimeoutsWrite=0;
	m_dwNumTimeoutsCmdReceive=0;
	//
	m_dwRunCount = 0;

#ifdef _UNICODE
	m_wCodePage = CODEPAGE_UNICODE;
#endif	
	
	// timed messages
	m_tmErrorIndication.SetDelta(1000*10);	// plus dwCmd as userData
	m_tmUnhandledCommand.SetDelta(1000*10);	// plus dwCmd as userData
	// m_iNumZerosReceived only counts with m_tmZerosReceived
	m_tmZerosReceived.SetDelta(1000*10);	 
	// plus special user data MyInternalError (local enum)
	m_tmInternalError.SetDelta(1000);

	if (shmName==NULL || _tcslen(shmName)==0)
	{
		m_sName=_T("INVALID NAME");
	} 
	else
	{
		m_sName = shmName;
	}
	PreCalcNames();	// NEW 170397
	
	CString sShutdownName;
	sShutdownName.Format(_T("%s%s%d"),m_sName,_T("Shutdown"),bMaster);

	HANDLE hShutdownEvent = OpenEvent(EVENT_ALL_ACCESS,FALSE,sShutdownName);
	if (hShutdownEvent)
	{
		WK_TRACE_ERROR(_T("CIPC object already exists <%s>\n"),m_sName);
	}

	// CreateEvent(securityAttr, bManualReset, bInitialState );
	m_hShutdownEvent = CreateEvent(NULL, TRUE, FALSE, sShutdownName);
	if (m_hShutdownEvent==NULL) 
	{
		WK_TRACE_ERROR(_T("CIPC[%s],shutdown event failed\n"), LPCTSTR(m_sName));
	}
	
	// create two channels, writeChannel is created right now
	// the readChannel is opened, if it's found
	m_pReadChannel = new CIPCChannel(this);
	m_pWriteChannel = new CIPCChannel(this);

	CreateWriteChannel();	// first Try, see ::Run

	EnableWriteQueue();	
}
//////////////////////////////////////////////////////////////////////////////
void CIPC::PreCalcNames()
{
	// master 
	m_sSharedMemoryMaster= m_sName + _T("Master");
	m_sMutexMaster= m_sName + _T("Master") + _T("Mutex");
	// slave
	m_sSharedMemorySlave = m_sName + _T("Slave");
	m_sMutexSlave = m_sName + _T("Slave") + _T("Mutex");
}
//////////////////////////////////////////////////////////////////////////////
void CIPC::CreateWriteChannel()
{
	// pre: channel already created
	// now try to connect

	// use ReadAndWrite names
	LPCTSTR szMutexName, szSharedMemory;
	if (m_bMaster) 
	{
		szMutexName    = LPCTSTR(m_sMutexMaster);
		szSharedMemory = LPCTSTR(m_sSharedMemoryMaster);
	}
	else
	{
		szMutexName    = LPCTSTR(m_sMutexSlave);
		szSharedMemory = LPCTSTR(m_sSharedMemorySlave);
	}
	BOOL bMapOkay = m_pWriteChannel->CreateFileMapping(
							szMutexName, 
							szSharedMemory,
							TRUE, m_bMaster
							);

	if (bMapOkay)
	{
		// here is the only place where m_bWriteCreateOkay is set to TRUE
		// once TRUE it will never go back to FALSE
		m_bWriteCreateOkay = m_pWriteChannel->CreateEvents(m_sName,TRUE,m_bMaster);
	}
	else 
	{
		m_bWriteCreateOkay = FALSE;
		// no message, it's a valid sub-state
	}
	// NEW always enter new state but use sub-states
	m_state=CIPC_STATE_WRITE_CREATED;
}
//////////////////////////////////////////////////////////////////////////////
void CIPC::ConnectReadChannel()
{
	// OOPS, could check for the correct thread
	// should only be issued in the CIPC thread, not form outside
	// to avoid multiple OnReadChannelFound

	WK_ASSERT(m_state==CIPC_STATE_WRITE_CREATED);

	// use named for ReadOnly
	LPCTSTR szMutexName, szSharedMemory;
	if (m_bMaster)
	{
		szMutexName    = LPCTSTR(m_sMutexSlave);
		szSharedMemory = LPCTSTR(m_sSharedMemorySlave);
	} 
	else
	{
		szMutexName    = LPCTSTR(m_sMutexMaster);
		szSharedMemory = LPCTSTR(m_sSharedMemoryMaster);
	}
	BOOL okayR = m_pReadChannel->CreateFileMapping(
						szMutexName, 
						szSharedMemory,
						FALSE, m_bMaster
						);
	if (okayR)
	{
		BOOL okayR2 = m_pReadChannel->CreateEvents(m_sName,FALSE,m_bMaster);
		if (okayR2) 
		{
			// advance to new state
			m_state=CIPC_STATE_READ_FOUND;
		} 
		else 
		{
			// failed to create events
		}
	} 
	else 
	{
		// failed to connect
		/*
		if (!GetIsMaster())
		{
			TRACE(_T("failed to connect read channel %s %s\n"),GetShmName(),GetLastErrorString());
		}
		*/
	}
	if (m_state==CIPC_STATE_READ_FOUND)
	{
		// get process/thread info of the write process
		if (m_pReadChannel->m_hWriteProcess==NULL)
		{
			if(m_tmInternalError.OmitMessageWithData(MIE_WRITE_PROCESS==FALSE)) 
			{
				InternalError(_T("Failed to get WriteProcess handle"));
			}
		}

		int numRetries=0;
		BOOL bCopyOkay=FALSE;
		while (numRetries<5 && bCopyOkay==FALSE)
		{
			m_pReadChannel->AccessMutex();
			bCopyOkay = DuplicateHandle(
						m_pReadChannel->m_hWriteProcess,	// source process
						m_pReadChannel->m_pSharedMemoryStruct->hThread,	// source handle
						GetCurrentProcess(),		// target process
						&m_hWriteThread,			// target handle
							(SYNCHRONIZE 
								| THREAD_ALL_ACCESS
								| THREAD_QUERY_INFORMATION
							),	
						FALSE,						// inherit flag
						0							// options
						);		
			m_pReadChannel->ReleaseMutex();
			if (bCopyOkay==FALSE) 
			{
				WK_TRACE_ERROR(_T("CIPC %s:copy writeThread %d %s\n"),GetShmName(), numRetries, LPCTSTR(GetLastErrorString()));
				numRetries++;
				Sleep(15);
			}
		}
		// call virtual function, to allow reactions
		// OOPS this might run in a WriteCmd, but this fn returns into the read
		// think about that!
		OnReadChannelFound();
	}
}
//////////////////////////////////////////////////////////////////////////////
void CIPC::DoDisconnect()
{
	m_state=CIPC_STATE_DISCONNECTED;
	OnRequestDisconnect();		// subclass actions

	m_cmdQueueList.SafeDeleteAll();
	m_dwBytesInSendQueue = 0;
	m_receivedCmdsQueue.DeleteAll();
	m_dwRunCount = MAKELONG(0,HIWORD(m_dwRunCount));

	WK_DELETE(m_pReadChannel);	// drop current connection
	WK_CLOSE_HANDLE(m_hWriteThread);
	// erase last cmd !
	if (m_pWriteChannel && m_bWriteCreateOkay) {
		ResetEvent(m_pWriteChannel->m_hEventCmdSend);
		SetEvent(m_pWriteChannel->m_hEventCmdDone);
		if (m_pWriteChannel->m_pSharedMemoryStruct) {
			m_pWriteChannel->m_pSharedMemoryStruct->dwCmd=0;
		}
	}

	// NOT YET what about delete ?

	// wait a moment to make sure the other side disolves correctly
	Sleep(100);	// OOPS

	// and try again
	m_pReadChannel=new CIPCChannel(this);
	m_state=CIPC_STATE_WRITE_CREATED;	// back to 'initial' state
}
/*********************************************************************************************
 Class      : CIPC
 Function   : ~CIPC
 Description: First the destructor calls <mf CIPC.StopThread>
              Then it sends a disconnect signal to the other side, if there is a connection.
 Note:
	Subclasses should call StopThread() as early as possible, to avoid problems in the
	destructors.
	With multithreading enabled the call of the destructor or <LINK CIPC::StopThread, StopThread>
	have to come from outside of the CIPC-thread.

	Do not call <i>delete this</i> in OnXXX() functions, use <LINK CIPC::DelayedDelete, DelayedDelete>
	instead. (NOT YET working)

 Parameters: None 

 Result type:  (None)
 created: September, 26 2003
 <TITLE ~CIPC>
*********************************************************************************************/
CIPC::~CIPC()
{
	// thread related stuff first
	StopThread();
	WK_DELETE(m_pThread);	// autoDelete is FALSE

	// make sure no thread tries to add to the queue
	m_cmdQueueList.Lock();
	// It's better to delete the channel before sending the signal!
	// Actually it makes sure, the writeChannel is deleted on both sides
	// BUT it needs a local copy of the Disconnect handle
	// Read- und Writekanal löschen
	if (m_bWriteCreateOkay) {
		WK_CLOSE_HANDLE(m_pWriteChannel->m_pSharedMemoryStruct->hThread);	// close copied handle
	}
	WK_DELETE(m_pReadChannel);
	
	HANDLE hMyDisconnect=NULL;
	if (m_pWriteChannel && m_bWriteCreateOkay)
	{
		BOOL bCopyOkay = DuplicateHandle(
					GetCurrentProcess(),	// source process
					m_pWriteChannel->m_hEventDisconnect,	// source handle
					GetCurrentProcess(),				// target process
					&hMyDisconnect,	// target handle
					EVENT_ALL_ACCESS,		// ignored if SAME_ACCESS
					FALSE,						// inherit flag
					0							// options
					);		
		if (bCopyOkay==FALSE)
		{
			InternalError(_T("Failed to copy disconnect handle"));
		}
	}
	WK_DELETE(m_pWriteChannel);
	WK_CLOSE_HANDLE(m_hWriteThread);
	if (hMyDisconnect) {
		SetEvent(hMyDisconnect);
	}
	
	WK_CLOSE_HANDLE(hMyDisconnect);	// close the local copy
	
	// clear cmd queue
	m_cmdQueueList.DeleteAll();	// already locked
	m_dwBytesInSendQueue = 0;
	m_cmdQueueList.Unlock();
	m_receivedCmdsQueue.DeleteAll();


	WK_CLOSE_HANDLE(m_hQueueSemaphore);
	WK_CLOSE_HANDLE(m_hShutdownEvent);
}

/*********************************************************************************************
 Class      : CIPC
 Function   : DelayedDelete
 Description: This functions is a special variant for the destructor, which can be used in
				  OnXXX() functions, instead of <i>delete this</i> The problem arises around the
				  thread loop:

 Note:
	NOT YET checked
	- <i> while (p->Run()) { } </i>

	The OnXXX() functions are called within the <i> Run() </i>,
	thus a <i> delete this </i>, would leave <i> p </i> as
	a reference to a deleted instance. So the loop looks like this

	- <i>while (p->IsNotDeleted() && p->Run()) { }</i>

	Also does a StopThread().

 Parameters: None 

 Result type:  (void)
 created: September, 26 2003
 <TITLE DelayedDelete>
*********************************************************************************************/
void CIPC::DelayedDelete()
{
	m_bMarkedForDelete=TRUE;
	// OOPS StopThread();
}

/*********************************************************************************************
 Class      : CIPC
 Function   : GetRemoteProcessID
 Description: Returns the system process ID of the remote connection.
              Only possible in state CIPC_STATE_READ_FOUND or CIPC_STATE_CONNECTED, else 0 is
				  returned.

 Parameters: None 

 Result type:  (DWORD)
 created: September, 26 2003
 <TITLE GetRemoteProcessID>
*********************************************************************************************/
DWORD CIPC::GetRemoteProcessID() const
{
	DWORD dwPID = 0;

	if (m_pReadChannel && m_pReadChannel->m_pSharedMemoryStruct
		&& (GetIPCState()==CIPC_STATE_CONNECTED
			|| GetIPCState()==CIPC_STATE_READ_FOUND) 
		) {
		dwPID = m_pReadChannel->m_pSharedMemoryStruct->dwProcessID;
	}

	return dwPID;
}

/*********************************************************************************************
 Class      : CIPC
 Function   : EnableWriteQueue
 Description: Call only once and before StartThread();

 Parameters: None 

 Result type:  (void)
 created: September, 26 2003
 <TITLE EnableWriteQueue>
*********************************************************************************************/
void CIPC::EnableWriteQueue()
{
	CString sSemaName = _T("QueueSema");
	// unique names for BOTH sides, which use the same ShmName!
	if (m_bMaster) 
	{
		sSemaName += _T("M");
	} 
	else 
	{
		sSemaName += _T("S");
	}
	sSemaName += m_sName;
	
    // LPSECURITY_ATTRIBUTES, lInitialCount,lMaximumCount
	m_hQueueSemaphore = CreateSemaphore( NULL, 0, CIPC_MAX_QUEUE, sSemaName);

	if (m_hQueueSemaphore==NULL) 
	{
		InternalError(_T("failed to get m_hQueueSemaphore"));
	}
}
//////////////////////////////////////////////////////////////////////////////
void CIPC::WriteCmdIntoChannel(const CIPCCmdRecord &cmd)
{
	// normals cmd only if CONNECTED
	// only RequestConnection if READ_FOUND
	BOOL bInitialContinue=TRUE;

	if (   cmd.m_dwCmd!=CIPC_BASE_REQ_CONNECTION
		&& GetIPCState()!=CIPC_STATE_CONNECTED) 
	{ 
		WK_TRACE(_T("CIPC %s:can't send %s while not connected\n"), GetShmName(), NameOfCmd(cmd.m_dwCmd));
		bInitialContinue = FALSE;	
	} 
	else if (   cmd.m_dwCmd==CIPC_BASE_REQ_CONNECTION
			 && GetIPCState()!=CIPC_STATE_READ_FOUND) 
	{
		WK_TRACE(_T("CIPC %s:can't send %s while not read found\n"), GetShmName(), NameOfCmd(cmd.m_dwCmd));
		bInitialContinue = FALSE;	
	}
	if (bInitialContinue==FALSE) {
		Sleep(10);	// OOPS
		return;
	}
	
	m_dwNumTimeoutsWrite = 0;
	m_pWriteChannel->SetCmd(cmd);
}
//////////////////////////////////////////////////////////////////////////////
void CIPC::AddToQueue(CIPCCmdRecord *pCmd, BOOL bDoSleep)
{
	// add to queue
	m_cmdQueueList.Lock();
	if (   (m_cmdQueueList.GetCount()>=CIPC_MAX_QUEUE)
		|| (m_dwBytesInSendQueue>=32*1024*1024)) // max 32 MB
	{
		if (m_tmInternalError.OmitMessageWithData(MIE_QUEUE_OVERFLOW)==FALSE) 
		{
			WK_TRACE(_T("CIPC %s: queue exceeds %d (%s) %d Bytes\n"),
					 GetShmName(),
					 m_cmdQueueList.GetCount(),
					 NameOfCmd(pCmd->m_dwCmd),
					 m_dwBytesInSendQueue);
		}
		OnCommandSendError(pCmd->m_dwCmd);
		WK_DELETE(pCmd);	// not handled in queue, delete now
		m_cmdQueueList.Unlock();
	} 
	else 
	{
		DWORD dwCmdSize = sizeof(CIPCCmdRecord);
		if (pCmd->m_pExtraMemory)
		{
			dwCmdSize += pCmd->m_pExtraMemory->GetLength();
		}
		m_dwBytesInSendQueue += dwCmdSize;

/*
		// Test RKE put in some commands at head
		switch (pCmd->m_dwCmd)
		{	
//			case CIPC_CONFIRM_SYNC:// OOPS test HEDU
//				if (pCmd->m_dwParam3==1234)
//				{
//					m_cmdQueueList.AddHead( pCmd );
//				}break;
			case CIPC_BASE_REQUEST_SET_VALUE: case CIPC_BASE_REQUEST_GET_VALUE:
				m_cmdQueueList.AddHead(pCmd);
				break;
			default:
				m_cmdQueueList.AddTail(pCmd);
			break;
		} 
*/
		m_cmdQueueList.AddTail(pCmd);
		// CAVEAT unlock before setting the sema
		m_cmdQueueList.Unlock();
		ReleaseSemaphore(m_hQueueSemaphore,1,NULL);	// increase counter

		if (bDoSleep) 
		{
			Sleep(0);
		}
	}

}
//////////////////////////////////////////////////////////////////////////////
BOOL CIPC::WriteCmd(DWORD dwCmd,DWORD dwParam1, DWORD dwParam2, DWORD dwParam3, DWORD dwParam4,
					BOOL bDoSleep /* = TRUE */ )
{
	CIPCCmdRecord * pCmd = new CIPCCmdRecord(NULL, dwCmd,
											 dwParam1,dwParam2,
											 dwParam3,dwParam4);
	// add to queue
	AddToQueue(pCmd, bDoSleep);

	return TRUE;
}

BOOL CIPC::WriteCmdWithData(DWORD dataLen, 
							const void *pData,
						    DWORD dwCmd,
							DWORD dwParam1, 
							DWORD dwParam2, 
							DWORD dwParam3, 
							DWORD dwParam4,
							BOOL bDoSleep /* =TRUE */ )
{
	CIPCExtraMemory *pExtraMemory=NULL;
	
	// create extra memory bubble
	if (dataLen && pData) 
	{
		pExtraMemory = new CIPCExtraMemory();
		if (pExtraMemory->Create(this,dataLen,pData))
		{
			// create cmd record
			CIPCCmdRecord *pCmd = new CIPCCmdRecord(pExtraMemory,
													dwCmd,
													dwParam1, 
													dwParam2,
													dwParam3, 
													dwParam4);
			WK_DELETE(pExtraMemory);	// already copied into pCmd
			// add to queue
			AddToQueue(pCmd, bDoSleep);
			return TRUE;
		}
		else
		{
			WK_TRACE_ERROR(_T("Create CIPCMemory in %s failed\n"),NameOfCmd(dwCmd));
			WK_DELETE(pExtraMemory);
			
			OnCommandSendError(dwCmd);

			return FALSE;
		}
	} 
	else 
	{
		WK_TRACE(_T("CIPC %s:WriteCmdWithData %s no data? len %d ptr %x\n"),
					GetShmName(),
					NameOfCmd(dwCmd),
					dataLen,pData);
		return FALSE;
	}
}
//////////////////////////////////////////////////////////////////////////////
#ifdef _UNICODE
BOOL CIPC::WriteCmdWithString(CString sString, DWORD dwCmd, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3, DWORD dwParam4, BOOL bDoSleep)
{
	CIPCExtraMemory *pExtraMemory=NULL;
	
	// create extra memory bubble
	if (sString.GetLength()) 
	{
		pExtraMemory = new CIPCExtraMemory();
		if (pExtraMemory->Create(this, sString))
		{
			// create cmd record
			CIPCCmdRecord *pCmd = new CIPCCmdRecord(pExtraMemory,
													dwCmd,
													dwParam1, 
													dwParam2,
													dwParam3, 
													dwParam4);
			WK_DELETE(pExtraMemory);	// already copied into pCmd
			// add to queue
			AddToQueue(pCmd, bDoSleep);
			return TRUE;
		}
		else
		{
			WK_TRACE_ERROR(_T("Create CIPCMemory in %s failed\n"),NameOfCmd(dwCmd));
			WK_DELETE(pExtraMemory);
			
			OnCommandSendError(dwCmd);

			return FALSE;
		}
	} 
	else 
	{
		WK_TRACE(_T("CIPC %s:WriteCmdWithString %s no data?\n"),
					GetShmName(),
					NameOfCmd(dwCmd)
					);
		return FALSE;
	}
}
#endif
//////////////////////////////////////////////////////////////////////////////
BOOL CIPC::WriteCmdWithExtraMemory(CIPCExtraMemoryPtr& pMemoryReference,
							       DWORD dwCmd,
								   DWORD dwParam1, 
								   DWORD dwParam2, 
								   DWORD dwParam3, 
								   DWORD dwParam4, 
								   BOOL bDoSleep /* = TRUE */ )
{
	if (pMemoryReference==NULL) 
	{
		WK_TRACE_ERROR(_T("WriteCmdWithExtraMemory %s\n"),
			NameOfCmd(dwCmd));
		OnCommandSendError(dwCmd);
		return FALSE;
	} 
	else
	{
		// create cmd record
		CIPCCmdRecord *pCmd = new CIPCCmdRecord(pMemoryReference,dwCmd,
													dwParam1, dwParam2,
													dwParam3, dwParam4
													);
		WK_DELETE(pMemoryReference);
		// reset pointer, to make sure caller does not delete
		pMemoryReference=NULL;

		// store in queue or do it

		// add to queue
		AddToQueue(pCmd, bDoSleep);
		return TRUE;
	}
}

/*********************************************************************************************
 Class      : CIPC
 Function   : OnReadChannelFound
 Description: Called if the read channel of a shared memory connection is found.
              
 See also: <LINK CIPC::CIPCState, CIPCState> for details.
           <LINK CIPC::DoRequestConnection, DoRequestConnection>,
           <LINK CIPC::OnRequestConnection, OnRequestConnection>,
			  <LINK CIPC::OnRequestDisconnect, OnRequestDisconnect>

 Parameters: None 

 Result type:  (void)
 created: September, 26 2003
 <TITLE OnReadChannelFound>
*********************************************************************************************/
void CIPC::OnReadChannelFound()
{
	// no message, if not overwritten
}

/*********************************************************************************************
 Class      : CIPC
 Function   : DoRequestConnection
 Description: Only if the state is CIPC_STATE_READ_FOUND a RequestConnection msg is send.
			     If the confirmation answer is received, the state advances to
				  CIPC_STATE_CONNECTED and <LINK CIPC::OnConfirmConnection, OnConfirmConnection>
				  is called.
 Note:
  Units do not have to use DoRequestConnection, they are called from the < LINK Server, Servers>
  and are free to override <LINK CIPC::OnRequestConnection, OnRequestConnection>.
              
 Parameters: None 

 Result type:  (void)
 created: September, 26 2003
 <TITLE DoRequestConnection>
*********************************************************************************************/
void CIPC::DoRequestConnection()
{
	// OOPS
	if (m_state==CIPC_STATE_READ_FOUND) 
	{
		// m_state = CIPC_STATE_CONNECTED;
		WriteCmd(CIPC_BASE_REQ_CONNECTION);
	} 
	else
	{
		// OOPS still need timeouts/failure
		TRACE(_T("cannot send CIPC_BASE_REQ_CONNECTION while not in CIPC_STATE_READ_FOUND %s\n"),GetShmName());
	}
}
// private, to make sure it is called, which is done directly in HandleCmd
void CIPC::DoConfirmConnection()
{	
	WriteCmd(CIPC_BASE_CONF_CONNECTION);
}
/*********************************************************************************************
 Class      : CIPC
 Function   : OnRequestConnection
 Description: Performs the private CIPC::DoConfirmConnection().
              It is the only OnXXX functions which has a default behavior.
              If subclasses override this functions they still have to do a call
              of the base function CIPC::OnRequestConnection()
 Note:
	DoConfirmConnection() is now directly called in HandleCmd default behaviour !
 Parameters: None 

 Result type:  (void)
 created: September, 26 2003
 <TITLE OnRequestConnection>
*********************************************************************************************/
void CIPC::OnRequestConnection()
{
	// OLD now directly in HandleCmd default behaviour !
	// DoConfirmConnection();
}
/*********************************************************************************************
 Class      : CIPC
 Function   : OnConfirmConnection
 Description: Override this function to get the confirmation of the connection.

 Parameters: None 

 Result type:  (void)
 created: September, 26 2003
 <TITLE OnConfirmConnection>
*********************************************************************************************/
void CIPC::OnConfirmConnection()
{
	// fine, no action, no message
}
/*********************************************************************************************
 Class      : CIPC
 Function   : OnRequestDisconnect
 Description: Called if a disconnect is recognized. Either the instances dissolves or
              reenters the state CIPC_STATE_READ_FOUND.
              There is no DoRequestDisconnect, it's handled internally.

 Parameters: None 

 Result type:  (void)
 created: September, 26 2003
 <TITLE OnRequestDisconnect>
*********************************************************************************************/
void CIPC::OnRequestDisconnect()
{
	// no confirm on that one
	// no message "not overwritten"
}

/*********************************************************************************************
 Class      : CIPC
 Function   : DoRequestSetGroupID
 Description: Used to inform a unit about the assigned groupID.
  Note:
	Has to be answered with <LINK CIPC::DoConfirmSetGroupID@WORD, DoConfirmSetGroupID>.

 Parameters:   
  wGroupID: (E): GroupID   (WORD)

 Result type:  (void)
 created: September, 26 2003
 <TITLE DoRequestSetGroupID>
*********************************************************************************************/
void CIPC::DoRequestSetGroupID(WORD wGroupID)
{
	WriteCmd(CIPC_BASE_REQUEST_SET_GROUP_ID, wGroupID);
}


/*********************************************************************************************
 Class      : CIPC
 Function   : DoConfirmSetGroupID
 Description: Used to confirm the assigment of a groupID.
 See also::
  <LINK CIPC::OnRequestSetGroupID@WORD, OnRequestSetGroupID>

 Parameters:   
  wGroupID: (E): GroupID   (WORD)

 Result type:  (void)
 created: September, 26 2003
 <TITLE DoConfirmSetGroupID>
*********************************************************************************************/
void CIPC::DoConfirmSetGroupID(WORD wGroupID)
{
	WriteCmd(CIPC_BASE_CONFIRM_SET_GROUP_ID, wGroupID);
}
/*********************************************************************************************
 Class      : CIPC
 Function   : OnRequestSetGroupID
 Description: Called when the server has send a <LINK CIPC::DoRequestSetGroupID@WORD, DoRequestSetGroupID>,
              which assigns a groupID to a unit.

 Parameters:   
  wGroupID: (E): GroupID   (WORD)

 Result type:  (void)
 created: September, 26 2003
 <TITLE OnRequestSetGroupID>
*********************************************************************************************/
void CIPC::OnRequestSetGroupID(WORD wGroupID)
{
	OverrideError();
	DoIndicateError(CIPC_BASE_REQUEST_SET_GROUP_ID, SECID_NO_ID, CIPC_ERROR_UNHANDLED_CMD,0);
}
/*********************************************************************************************
 Class      : CIPC
 Function   : OnConfirmSetGroupID
 Description: Called if a confirmation for <LINK CIPC::DoRequestSetGroupID@WORD, DoRequestSetGroupID>
				  is received.


 Parameters:   
  wGroupID: (E): GroupID   (WORD)

 Result type:  (void)
 created: September, 26 2003
 <TITLE OnConfirmSetGroupID>
*********************************************************************************************/
void CIPC::OnConfirmSetGroupID(WORD wGroupID)
{
	OverrideError();
	DoIndicateError(CIPC_BASE_CONFIRM_SET_GROUP_ID, SECID_NO_ID, CIPC_ERROR_UNHANDLED_CMD,0);
}
/*********************************************************************************************
 Class      : CIPC
 Function   : DoIndicateError
 Description: Used to indicate an error, usually in one of the OnXXX functions.

 Parameters:   
  dwCmd    : (E): command, which caused the error. NOT YET list of commands  (DWORD)
  id       : (E): id if available  (CSecID)
  errorCode: (E): errorCode standard message  (<LINK CIPCError, CIPCError>)
  iUnitCode: (E): iUnitCode unit specific error code  (int)
  sErrorMsg: (E): sErrorString, optional error message  (LPCTSTR)

 Result type:  (void)
 created: September, 26 2003
 <TITLE DoIndicateError>
*********************************************************************************************/
void CIPC::DoIndicateError(DWORD dwCmd, 
						   CSecID id, 
						   CIPCError errorCode, 
						   int iUnitCode,
						   LPCTSTR sErrorMsg)
{
	if (sErrorMsg==NULL || _tcslen(sErrorMsg)==0) 
	{
		WriteCmd(CIPC_BASE_INDICATE_ERROR,dwCmd,id.GetID(),
									errorCode,iUnitCode);
	} 
	else 
	{
#ifdef _UNICODE
		WriteCmdWithString(sErrorMsg,
						 CIPC_BASE_INDICATE_ERROR,
						 dwCmd,
						 id.GetID(),
						 errorCode,
						 iUnitCode);
#else
		CString sTmp(sErrorMsg);
		WriteCmdWithData(sTmp.GetLength(),
						 LPCTSTR(sTmp),
						 CIPC_BASE_INDICATE_ERROR,
						 dwCmd,
						 id.GetID(),
						 errorCode,
						 iUnitCode);
#endif		
	}
}

/*********************************************************************************************
 Class      : CIPC
 Function   : OnIndicateError
 Description: Called if an error indication is received.

 Parameters:   
  dwCmd    : (E): command, which caused the error. NOT YET list of commands  (DWORD)
  id       : (E): id if available  (CSecID)
  errorCode: (E): errorCode standard message  (<LINK CIPCError, CIPCError>)
  iUnitCode: (E): iUnitCode unit specific error code  (int)
  sErrorMsg: (E): sErrorString, optional error message  (LPCTSTR)

 Result type:  (void)
 created: September, 26 2003
 <TITLE OnIndicateError>
*********************************************************************************************/
void CIPC::OnIndicateError(DWORD dwCmd, CSecID id, 
						   CIPCError errorCode, int iUnitCode,
						   const CString &sErrorMsg)
{
	if (m_tmErrorIndication.OmitMessageWithData(dwCmd)==FALSE) 
	{
		WK_TRACE(_T("%s:ErrorIndication: cmd %s, id %x, %d/%d %s\n"),
				GetShmName(), 
				NameOfCmd(dwCmd),
				id.GetID(),
				errorCode,iUnitCode,
				sErrorMsg
				);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIPC::DoRequestGetValue(CSecID id,
							 const CString &sKey,
							 DWORD dwServerData)
{
	if (sKey.GetLength()) 
	{
//	WK_TRACE(_T("CIPC::DoRequestGetValue %s\n"), sKey);
#ifdef _UNICODE
		WriteCmdWithString(sKey,
			          CIPC_BASE_REQUEST_GET_VALUE,
						 id.GetID(),		// dw1
						 dwServerData	// dw2
						 );
#else
		CString sTmp(sKey);
		WriteCmdWithData(sTmp.GetLength(),
						 LPCTSTR(sTmp),
						 CIPC_BASE_REQUEST_GET_VALUE,
						 id.GetID(),		// dw1
						 dwServerData	// dw2
						 );
#endif
	} 
	else 
	{
		// NOT YET INVALID_KEY
		OnCommandSendError(CIPC_BASE_REQUEST_GET_VALUE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIPC::OnRequestGetValue(CSecID id,
							 const CString& sKey,
							 DWORD /*dwServerData*/)
{
	OverrideError();
}
/////////////////////////////////////////////////////////////////////////////
void CIPC::DoConfirmGetValue(CSecID id, 
							 const CString &sKey,
							 const CString &sValue,
							 DWORD dwServerData)
{
	DWORD dwLenKey,dwLenValue;
	dwLenKey=sKey.GetLength();
	dwLenValue=sValue.GetLength();

	if (dwLenKey==0) 
	{
		// NOT YET INVALID_KEY
		OnCommandSendError(CIPC_BASE_CONFIRM_GET_VALUE);
		return;	// EXIT
	}

	// send as one string, and cut on the receiver side
#ifdef _UNICODE
	WriteCmdWithString(sKey+sValue,
					 CIPC_BASE_CONFIRM_GET_VALUE,
					 id.GetID(),	//dw1
					 dwLenKey,		//dw2
					 dwLenValue,	//dw3
					 dwServerData	//dw4
					 );
#else
	CString sMerge=sKey+sValue;
	WriteCmdWithData(sMerge.GetLength(),
					 LPCTSTR(sMerge),
					 CIPC_BASE_CONFIRM_GET_VALUE,
					 id.GetID(),	//dw1
					 dwLenKey,		//dw2
					 dwLenValue,	//dw3
					 dwServerData	//dw4
					 );
#endif
}
/////////////////////////////////////////////////////////////////////////////
void CIPC::DoConfirmSetValue(CSecID id,
							 const CString &sKey,
							 const CString &sValue,
							 DWORD dwServerData)
{
	DWORD dwLenKey,dwLenValue;
	dwLenKey=sKey.GetLength();
	dwLenValue=sValue.GetLength();

	if (dwLenKey==0) 
	{
		// NOT YET INVALID_KEY
		OnCommandSendError(CIPC_BASE_CONFIRM_SET_VALUE);
		return;	// EXIT
	}

	// send as one string, and cut on the receiver side
#ifdef _UNICODE
	WriteCmdWithString(sKey+sValue,
					 CIPC_BASE_CONFIRM_SET_VALUE,
					 id.GetID(),		//dw1
					 dwLenKey,		//dw2
					 dwLenValue,		//dw3
					 dwServerData	//dw4
					);
#else
	CString sMerge=sKey+sValue;
	WriteCmdWithData(sMerge.GetLength(),
					 LPCTSTR(sMerge),
					 CIPC_BASE_CONFIRM_SET_VALUE,
					 id.GetID(),		//dw1
					 dwLenKey,		//dw2
					 dwLenValue,		//dw3
					 dwServerData	//dw4
					);
#endif
}
/*********************************************************************************************
 Class      : CIPC
 Function   : OnConfirmGetValue
 Description: Confirms the get value request

 Parameters:   
  id          : (E): Might be used as group ID only  (CSecID)
  sKey        : (E): The key to identify the value to be set  (const CString&)
  sValue      : (E): The new value  (const CString&)
  dwServerData: (E): special server data  (DWORD)

 Result type:  (void)
 created: September, 26 2003
 <TITLE OnConfirmGetValue>
*********************************************************************************************/
void CIPC::OnConfirmGetValue(  CSecID         // id
									  , const CString& // sKey
									  , const CString& // sValue
									  , DWORD          //dwServerData
							 )
{
	OverrideError();
}

/*********************************************************************************************
 Class      : CIPC
 Function   : OnConfirmSetValue
 Description: Confirms the set value request

 Parameters:   
  id          : (E): Might be used as group ID only  (CSecID)
  sKey        : (E): The key to identify the value to be set  (const CString&)
  sValue      : (E): The new value  (const CString&)
  dwServerData: (E): special server data  (DWORD)

 Result type:  (void)
 created: September, 26 2003
 <TITLE OnConfirmGetValue>
*********************************************************************************************/
void CIPC::OnConfirmSetValue(CSecID /*id*/, 
							 const CString& /*sKey*/,
							 const CString& /*sValue*/,
							 DWORD /*dwServerData*/
							 )
{
	OverrideError();
}
/*********************************************************************************************
 Class      : CIPC
 Function   : DoRequestSetValue
 Description: Requests to set a value.

 Parameters:   
  id          : (E):  might be used as group ID only  (CSecID)
  sKey        : (E): The key to identify the value to be set  (const CString&)
  sValue      : (E): The new value  (const CString&)
  dwServerData: (E): special server data  (DWORD)

 Result type:  (void)
 created: September, 26 2003
 <TITLE DoRequestSetValue>
*********************************************************************************************/
void CIPC::DoRequestSetValue(CSecID id, // might be used as group ID only
							 const CString &sKey,
							 const CString &sValue,
							 DWORD dwServerData)
{
	DWORD dwLenKey,dwLenValue;
	dwLenKey=sKey.GetLength();
	dwLenValue=sValue.GetLength();

	if (dwLenKey==0) 
	{
		// NOT YET INVALID_KEY
		OnCommandSendError(CIPC_BASE_REQUEST_SET_VALUE);
		return;	// EXIT
	}
	
	// send as one string, and cut on the receiver side
#ifdef _UNICODE
	WriteCmdWithString(sKey+sValue,
					 CIPC_BASE_REQUEST_SET_VALUE,
					 id.GetID(),	//dw1
					 dwLenKey,		//dw2
					 dwLenValue,	//dw3
					 dwServerData	//dw4
					 );
#else
	CString sMerge=sKey+sValue;
	WriteCmdWithData(sMerge.GetLength(),
				     LPCTSTR(sMerge),
					 CIPC_BASE_REQUEST_SET_VALUE,
					 id.GetID(),	//dw1
					 dwLenKey,		//dw2
					 dwLenValue,	//dw3
					 dwServerData	//dw4
					 );
#endif
}
/*********************************************************************************************
 Class      : CIPC
 Function   : OnRequestSetValue
 Description: Retrieves a requests to set a value.

 Parameters:   
  id          : (E):  might be used as group ID only  (CSecID)
  sKey        : (E): The key to identify the value to be set  (const CString&)
  sValue      : (E): The new value  (const CString&)
  dwServerData: (E): special server data  (DWORD)

 Result type:  (void)
 created: September, 26 2003
 <TITLE DoRequestSetValue>
*********************************************************************************************/
void CIPC::OnRequestSetValue(
						CSecID /*id*/, // might be used as group ID only
						const CString& /*sKey*/,
						const CString& /*sValue*/,
						DWORD /*dwServerData*/
						)
{
	OverrideError();
}
/////////////////////////////////////////////////////////////////////////////
void CIPC::DoRequestVersionInfo(WORD wGroupID)
{
	WriteCmd(CIPC_BASE_REQUEST_VERSION,wGroupID);
}
/////////////////////////////////////////////////////////////////////////////
void CIPC::OnRequestVersionInfo(WORD wGroupID)
{
	// CAVEAT default behavior
	DoConfirmVersionInfo(wGroupID,1);
}
/////////////////////////////////////////////////////////////////////////////
void CIPC::DoConfirmVersionInfo(WORD wGroupID, DWORD dwVersion)
{
	WriteCmd(CIPC_BASE_CONFIRM_VERSION,wGroupID, dwVersion);
}
/////////////////////////////////////////////////////////////////////////////
void CIPC::OnConfirmVersionInfo(WORD /*wGroupID*/, DWORD /*dwVersion*/)
{
	// no error indication
}



// 
//
// PART: CIPC, Thread
//
//

/*********************************************************************************************
 Class      : 
 Function   : CIPCThreadFunction
 Description: control function for the worker thread returning from this suspends the thread

 Parameters:   
  pParam: ():    (LPVOID)

 Result type:  (UINT)
 created: September, 26 2003
 <TITLE CIPCThreadFunction>
*********************************************************************************************/
UINT CIPCThreadFunction(LPVOID pParam)
{
	CIPC * p = (CIPC*)pParam;
	XTIB::SetThreadName(p->m_sName);
	InitVideteLocaleLanguage();

#ifdef _WK_CLASSES_DLL
	AddThreadToPool(GetCurrentThreadId(), p->m_sName);
#else
	g_ThreadPool.AddThreadToPool(GetCurrentThreadId(), p->m_sName);
#endif
	while ( p->Run(p->GetTimeoutCmdReceive())) 
	{
		if (p->GetIPCState()==CIPC_STATE_CONNECTED) 
		{
			Sleep(0); // force thread switch
		}
	}
#ifdef _WK_CLASSES_DLL
	RemoveThreadFromPool(GetCurrentThreadId());
#else
	g_ThreadPool.RemoveThreadFromPool(GetCurrentThreadId());
#endif
	return 0;
}

////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : CIPC
 Function   : SpecialSleep
 Description: a special variant of Sleep(). May be used with the CIPC thread.
              It can be interrupted via <LINK CIPC::StopThread, StopThread>.

 Parameters:   
  dwTimeout: ():    (DWORD)

 Result type:  (BOOL)
 created: September, 26 2003
 <TITLE SpecialSleep>
*********************************************************************************************/
BOOL CIPC::SpecialSleep(DWORD dwTimeout)
{
	if (m_pThread==NULL) 
	{
		// do not sleep in single threaded CIPCs
		return FALSE;	// EXIT
	}
	else
	{
		DWORD dwAbort = WaitForSingleObject(m_hShutdownEvent,dwTimeout);
		if (dwAbort==WAIT_OBJECT_0)
		{
			// shutdown signaled
			return TRUE;
		} 
		else 
		{
			// normal sleep end
			return FALSE;
		}
	}
}

/*********************************************************************************************
 Class      : CIPC
 Function   : Run
 Description: Run is the main action. Depending on the initialization in
              <LINK CIPC::StartThread@BOOL@int, StartThread> it runs automatically as thread function,
				  or it is called in the OnIdle function of the main thread.

 Parameters:   
  dwTimeout: (E): specifies the time to wait for an event (in milli seconds).  (DWORD)

 Result type:  (BOOL)
 created: September, 26 2003
 <TITLE Run>
*********************************************************************************************/
BOOL CIPC::Run(DWORD dwTimeout)
{
	// still active
	if (m_bThreadRun==FALSE
		|| m_bMarkedForDelete) return FALSE;

	WORD wRunCount = LOWORD(m_dwRunCount);
	WORD wAliveCount = HIWORD(m_dwRunCount);
	wRunCount++;
	m_dwRunCount = MAKELONG(wRunCount,wAliveCount);

	// NEW HEDU 050297
	if (m_iNumZerosReceived>5)
	{
		m_iNumZerosReceived=0;
		WK_TRACE(_T("%x:%s:received 5 zero cmds, giving up\n"),
			GetCurrentProcess(),GetShmName());
		DoDisconnect();
		return TRUE;	// still keep thread alive
	}

	switch (m_state)
	{
		case CIPC_STATE_WRITE_CREATED:
			// CIPC_STATE_WRITE_CREATED extended
			// now splitted in two sub-states
			if (m_bWriteCreateOkay) 
			{
				ConnectReadChannel();
				if (m_state!=CIPC_STATE_READ_FOUND) 
				{
					if (dwTimeout!=0) 
					{
						// returns TRUE if interrupted
						if (SpecialSleep(m_dwConnectRetryTime)) 
						{
							return FALSE;
						}
					}
					return TRUE;
				}
				else 
				{
					// return TRUE;	// OOPS better performance for the DoRequestConnection ?
					// NEW ConnectRead only called in own thread
				}
			} 		  
			else 
			{
				// write not created retry
				CreateWriteChannel();
				if (m_bWriteCreateOkay==FALSE) 
				{
					if (OnRetryWriteChannel()) 
					{
						if (SpecialSleep(m_dwConnectRetryTime)) 
						{
							return FALSE;
						}
					} 
					else 
					{
						WK_TRACE(_T("no write channel\n"), GetShmName());
						DoDisconnect();
					}
				}
				return TRUE;	// continue thread <<<EXIT>>>
			}
			break;
		case CIPC_STATE_READ_FOUND:
			break;
		case CIPC_STATE_CONNECTED:
			if (LOWORD(m_dwRunCount) == 4192)
			{
//				TRACE(_T("%s requesting alive\n"), GetShmName());
				WriteCmd(CIPC_BASE_REQUEST_ALIVE);
				WORD wAliveCount = HIWORD(m_dwRunCount);
				wAliveCount++;
				m_dwRunCount = MAKELONG(0,wAliveCount);
			}
			// fine
			break;
		case CIPC_STATE_DISCONNECTED:
			m_dwRunCount = MAKELONG(0,0);
			break;
		default: 
			WK_TRACE(_T("CIPC %s: Run() invalid state %d\n"), GetShmName(), m_state);
			return TRUE;	// EXIT
	}

	CIPCCmdRecord *pCmd = NULL;
	BOOL	bAlreadyHadCmd=FALSE;
	if (   m_cmdQueueList.GetCount()==0
		|| (m_dwRunCount & 1)) 
	{	// operate send and receive queue alternating depending from m_dwRunCount
		if (m_receivedCmdsQueue.GetCount()) 
		{
			pCmd = m_receivedCmdsQueue.GetHead();
			m_receivedCmdsQueue.RemoveHead();
		} 
		else 
		{
			pCmd=NULL;
		}
	}

	if (pCmd) 
	{
		m_pActiveCmd = pCmd;	// store in member for GetActiveCmd
//		DWORD dwExtraMemUsed = 0;
//		if (pCmd->m_pExtraMemory)
//			dwExtraMemUsed = pCmd->m_pExtraMemory->GetLength();

		// here comes the real action, a call to the virtual HandleCmd!
		if (pCmd->m_dwCmd == CIPC_BASE_REQUEST_ALIVE)
		{
//			TRACE(_T("%s confirming alive\n"), GetShmName());
			WriteCmd(CIPC_BASE_CONFIRM_ALIVE);
		}
		else if (pCmd->m_dwCmd == CIPC_BASE_CONFIRM_ALIVE)
		{
//			TRACE(_T("%s received alive confirmation\n"),GetShmName());
			WORD wRunCount = LOWORD(m_dwRunCount);
			WORD wAliveCount = HIWORD(m_dwRunCount);
			wAliveCount--;
			m_dwRunCount = MAKELONG(wRunCount,wAliveCount);
		}
		else
		{
			m_dwLastCmdSend	= pCmd->m_dwCmd;
			if (!HandleCmd(pCmd->m_dwCmd, 
						   pCmd->m_dwParam1, 
						   pCmd->m_dwParam2, 
						   pCmd->m_dwParam3, 
						   pCmd->m_dwParam4,
						   pCmd->m_pExtraMemory))
			{
				WK_TRACE(_T("CIPC:%s:unhandled cmd %s\n"), LPCTSTR(m_sName), NameOfCmd(pCmd->m_dwCmd));
				// send an error indication back
				if (pCmd->m_dwCmd!=CIPC_BASE_INDICATE_ERROR) 
				{
					DoIndicateError(pCmd->m_dwCmd, SECID_NO_ID, CIPC_ERROR_UNHANDLED_CMD,0);
				}
			}
		} 
		WK_DELETE(pCmd);
		m_pActiveCmd = NULL;
		bAlreadyHadCmd=TRUE;
	}	

	// check write queue
	CIPCCmdRecord *pWriteCmd = m_cmdQueueList.SafeGetAndRemoveHead();


	BOOL bWaitAll=FALSE;
	HANDLE theEvents[7];
	int numTheEvents=0;

	if (pWriteCmd==NULL) 
	{
		// RECEIVE only events
		theEvents[0]=m_pReadChannel->m_hEventCmdSend;
		theEvents[1]=m_pReadChannel->m_hWriteProcess;
		theEvents[2]=m_hWriteThread;
		theEvents[3]=m_pReadChannel->m_hEventDisconnect;
		theEvents[4]=m_hShutdownEvent;
		theEvents[5]=m_hQueueSemaphore;	// might be NULL
		// only wait for CmdSend, or also queue events ?
		if (theEvents[5])
		{
			numTheEvents=6;
		}
		else
		{
			numTheEvents=5;
		}
	} 
	else 
	{
		DWORD dwCmdSize = sizeof(CIPCCmdRecord);

		if (pWriteCmd->m_pExtraMemory)
		{
			dwCmdSize += pWriteCmd->m_pExtraMemory->GetLength();
		}
		if (m_dwBytesInSendQueue>dwCmdSize)
		{
			m_dwBytesInSendQueue -= dwCmdSize;
		}
		else
		{
			m_dwBytesInSendQueue = 0;
		}
		// WRITE events plus listen to incoming events
		theEvents[0]=m_pReadChannel->m_hEventCmdSend;
		theEvents[1]=m_pReadChannel->m_hWriteProcess;
		theEvents[2]=m_hWriteThread;
		theEvents[3]=m_pReadChannel->m_hEventDisconnect;
		theEvents[4]=m_pWriteChannel->m_hEventCmdDone;
		theEvents[5]=m_hShutdownEvent;
		theEvents[6]=NULL; // might be NULL
		numTheEvents=6;
	}
	
	// workaround WAIT_FAILED
	BOOL bRetry=FALSE;
	int iNumRetries=0;
	do 
	{
		DWORD dwRet = WaitForMultipleObjects(numTheEvents, theEvents, 
											bWaitAll, dwTimeout);
		// might have changed meanwhile
		if (m_bThreadRun==FALSE) 
		{
			WK_DELETE(pWriteCmd);
			return FALSE;
		}

		if (   dwRet>=WAIT_OBJECT_0 
			&& dwRet < WAIT_OBJECT_0+numTheEvents) 
		{
			// which Event occured?
			int ix = dwRet - WAIT_OBJECT_0;
			if (theEvents[ix]==m_pReadChannel->m_hEventCmdSend) 
			{
				// get cmd from memory and add it to receive queue
				OnReceiveCmd();
				m_dwNumTimeoutsCmdReceive=0;
				// still something to send ?
				if (pWriteCmd==NULL) 
				{
					bRetry=FALSE;
				}
			} 
			else if (theEvents[ix]==m_hShutdownEvent) 
			{
				bRetry = FALSE;
			} 
			else if (   theEvents[ix]==m_pReadChannel->m_hWriteProcess
					 || theEvents[ix]==m_hWriteThread
					 || theEvents[ix]==m_pReadChannel->m_hEventDisconnect) 
			{
				// no need to ResetEvent, it all melting down anyway
				DoDisconnect();
				iNumRetries=0;	// OOPS for the following DoDisconnect
				bRetry=FALSE;
			} 
			else if (theEvents[ix]==m_hQueueSemaphore) 
			{
				// WriteCmdFromQueue();	// CIPC will do the job
				// next ::Run will remove the cmd from the queue
				// so the sema is used, to trigger a 'sleeping' Cipc
				// OOPS can only be NULL
				if (pWriteCmd==NULL) 
				{
					pWriteCmd = m_cmdQueueList.SafeGetAndRemoveHead();
					// can be NULL, because it was already removed
					// without waiting for the sema
					if (pWriteCmd) 
					{
						DWORD dwCmdSize = sizeof(CIPCCmdRecord);
						if (pWriteCmd->m_pExtraMemory)
						{
							dwCmdSize += pWriteCmd->m_pExtraMemory->GetLength();
						}
						if (m_dwBytesInSendQueue>dwCmdSize)
						{
							m_dwBytesInSendQueue -= dwCmdSize;
						}
						else
						{
							m_dwBytesInSendQueue = 0;
						}
						// OOPS same as above
						// WRITE events plus listen to incoming events
						theEvents[0]=m_pReadChannel->m_hEventCmdSend;
						theEvents[1]=m_pReadChannel->m_hWriteProcess;
						theEvents[2]=m_hWriteThread;
						theEvents[3]=m_pReadChannel->m_hEventDisconnect;
						theEvents[4]=m_pWriteChannel->m_hEventCmdDone;
						theEvents[5]=m_hShutdownEvent;
						theEvents[6]=NULL; // might be NULL
						numTheEvents=6;
					} 
					else 
					{
						bRetry=FALSE;
					}
				} 
				else 
				{
					TRACE(_T("%s:queueSema and pWriteCmd!?\n"), GetShmName());
					bRetry=FALSE;
				}
			} 
			else if (theEvents[ix]==m_pWriteChannel->m_hEventCmdDone) 
			{
				WK_ASSERT(pWriteCmd);
				WriteCmdIntoChannel(*pWriteCmd);
				WK_DELETE(pWriteCmd);
				bRetry=FALSE;
			} 
			else 
			{
				WK_TRACE_ERROR(_T("CIPC %s: invalid event index %d!\n"),GetShmName(),ix);
			}
		} 
		else if (dwRet==WAIT_TIMEOUT) 
		{
			/*
			if (pWriteCmd==NULL)  
			{
				m_dwNumTimeoutsCmdReceive++;
				OnTimeoutCmdReceive();
			} 
			else 
			{
				m_dwNumTimeoutsWrite++;
				OnTimeoutWrite(pWriteCmd->m_dwCmd);
			}
			*/
			bRetry=FALSE;
		} 
		else if (dwRet==WAIT_FAILED) 
		{
			WK_TRACE(_T("%s:[%d]WAIT_FAILED (read)(queue %d)\n"), GetShmName(),
					iNumRetries, m_cmdQueueList.GetCount());
			m_dwNumTimeoutsCmdReceive++;
			OnWaitFailed(0);
			iNumRetries++;
			bRetry=TRUE;
			Sleep(5);
		} 
		else 
		{
			InternalError(_T("WaitMultipleObjects"));
		}
	} 
	while (bRetry && iNumRetries<5);


	if (pWriteCmd) 
	{
		// back to queue
		m_cmdQueueList.Lock();
		DWORD dwCmdSize = sizeof(CIPCCmdRecord);
		if (pWriteCmd->m_pExtraMemory)
		{
			dwCmdSize += pWriteCmd->m_pExtraMemory->GetLength();
		}
		m_dwBytesInSendQueue += dwCmdSize;
		m_cmdQueueList.AddHead(pWriteCmd);
		m_cmdQueueList.Unlock();
	}

	if (bAlreadyHadCmd==FALSE) 
	{
		if (m_receivedCmdsQueue.GetCount()) 
		{
			pCmd = m_receivedCmdsQueue.GetHead();
			m_receivedCmdsQueue.RemoveHead();
		} 
		else 
		{
			pCmd=NULL;
		}
		if (pCmd && m_bThreadRun) 
		{	// double check before run calling HandleCmd
			m_pActiveCmd = pCmd;	// store in member for GetActiveCmd
//			DWORD dwExtraMemUsed = 0;
//			if (pCmd->m_pExtraMemory) {
//				dwExtraMemUsed = pCmd->m_pExtraMemory->GetLength();
//			}
			// here comes the real action, a call to the virtual HandleCmd!
			if (pCmd->m_dwCmd == CIPC_BASE_REQUEST_ALIVE)
			{
//				TRACE(_T("%s confirming alive\n"), GetShmName());
				WriteCmd(CIPC_BASE_CONFIRM_ALIVE);
			}
			else if (pCmd->m_dwCmd == CIPC_BASE_CONFIRM_ALIVE)
			{
//				TRACE(_T("%s received alive confirmation\n"), GetShmName());
				WORD wRunCount = LOWORD(m_dwRunCount);
				WORD wAliveCount = HIWORD(m_dwRunCount);
				wAliveCount--;
				m_dwRunCount = MAKELONG(wRunCount,wAliveCount);
			}
			else
			{
				m_dwLastCmdSend	= pCmd->m_dwCmd;
				if (!HandleCmd(pCmd->m_dwCmd, 
							   pCmd->m_dwParam1, 
							   pCmd->m_dwParam2, 
							   pCmd->m_dwParam3, 
							   pCmd->m_dwParam4,
							   pCmd->m_pExtraMemory))
				{
					WK_TRACE(_T("CIPC:%s:unhandled cmd %s\n"), LPCTSTR(m_sName), NameOfCmd(pCmd->m_dwCmd));
					// send an error indication back
					if (pCmd->m_dwCmd!=CIPC_BASE_INDICATE_ERROR) 
					{
						DoIndicateError(pCmd->m_dwCmd, SECID_NO_ID, CIPC_ERROR_UNHANDLED_CMD,0);
					}
				}
			}
			WK_DELETE(pCmd);
			m_pActiveCmd = NULL;
			bAlreadyHadCmd=TRUE;
		}
	}	

	if (iNumRetries>=5) 
	{
		WK_TRACE_ERROR(_T("disconnecting %s\n"),GetShmName());
		DoDisconnect();
	}

	if (   m_state == CIPC_STATE_CONNECTED
		&& HIWORD(m_dwRunCount)>20)
	{
		OnIndicateError(CIPC_BASE_CONFIRM_ALIVE,SECID_NO_ID,CIPC_ERROR_ALIVE_UNCONFIRMED,HIWORD(m_dwRunCount),_T(""));
	}
	
	if (m_bMarkedForDelete) 
	{
		// Set after call of DelayedDelete
		// don't run any longer
		return FALSE;
	} 
	else 
	{
		return m_bThreadRun;	// keep thread alive
	}
}
//////////////////////////////////////////////////////////////////////////////
void CIPC::OnReceiveCmd()
{
	if (GetIPCState()==CIPC_STATE_INVALID) 
	{ 
		// an event while not created ?
		WK_TRACE_ERROR(_T("Already got an event, while still in start...\n"));
		return;		// EXIT
	}

	CIPCCmdRecord *pCmd = m_pReadChannel->GetCmd();

	if (pCmd)
	{
		/*
		if (   pCmd->m_dwCmd  == CIPC_CONFIRM_SYNC 
			&& pCmd->m_dwParam3 ==1234) 
		{	
			// favor SYNC cmd in read
			m_receivedCmdsQueue.AddHead(pCmd);
		} 
		else 
		*/
		{
			m_receivedCmdsQueue.AddTail(pCmd);
		}
	}
	else
	{
		if (m_tmZerosReceived.OmitMessage()==FALSE) 
		{
			m_tmZerosReceived.Message(_T("%x:%s:received 0 cmd !?\n"),GetCurrentProcess(),GetShmName());
			m_iNumZerosReceived++;	// checked in ::Run
		} 
		else 
		{
			m_iNumZerosReceived=0;
		}
	}
}

/*********************************************************************************************
 Class      : CIPC
 Function   : StartThread
 Description: Start a separate thread if bMultithreaded is TRUE. It enables all communication
              in general. Even singlethreaded applications have to call StartThread before a
				  call to <LINK CIPC::Run@DWORD, Run> can be made.

 Parameters:   
  bMultiThreaded: (E): TRUE, FALSE  (BOOL)
  nPriority     : (): 0, ..., 5  (int)

 Result type:  (void)
 created: September, 26 2003
 <TITLE StartThread>
*********************************************************************************************/
void CIPC::StartThread(BOOL bMultiThreaded,int nPriority)
{
	m_bThreadRun = TRUE;
	if (bMultiThreaded) 
	{

		m_pThread = AfxBeginThread(CIPCThreadFunction, this, nPriority);
		WK_TRACE(_T("CIPCThread %s Handle=%08x  Id=%08x\n"), GetShmName(), m_pThread->m_hThread, m_pThread->m_nThreadID);//GetCurrentThreadId());
		m_pThread->m_bAutoDelete = FALSE;
	}
	// first try to connect ?
	// NOT YET, this would need a critical section.
	// or it could be before the thread start
	// ConnectReadChannel();
}

/*********************************************************************************************
 Class      : CIPC
 Function   : StopThread
 Description: If running in <LINK CIPC::.StartThread, StartThread> mode, the thread
              is terminated. Derived classes should call StopThread() in their
              destructor, if not it might happen, that the destructor chain
              '~derivedclassA...~derivedclass...~CIPC' is interrupted with a thread switch
              in such a way, that the derived class data is already deleted, but the
              thread assumes a complete object.

 Parameters: None 

 Result type:  (void)
 created: September, 26 2003
 <TITLE StopThread>
*********************************************************************************************/
void CIPC::StopThread()
{
	if (m_bThreadRun)	{
		m_bThreadRun = FALSE;
		// terminate possible WaitForMultipleObjects in the thread
		// StopThread is called from the 'outer' thread !
		PulseEvent(m_hShutdownEvent);
		Sleep(0);

		if (m_pReadChannel && m_pReadChannel->m_hEventCmdSend) {
			SetEvent(m_pReadChannel->m_hEventCmdSend);
		}
		if (m_pThread) {									
			// OOPS check for the correct thread
			// the initial thread should do the delete
			
			DWORD dwThreadEnd = WaitForSingleObject(m_pThread->m_hThread,5000);
			if (dwThreadEnd==WAIT_TIMEOUT) 
			{
				WK_TRACE(_T("OOPS, %s thread did not terminate!?\n"), GetShmName());
				TerminateThread(m_pThread->m_hThread,0xEEEE);
			} 
			else if (dwThreadEnd!=WAIT_OBJECT_0) 
			{
				TRACE(_T("dwThreadEnd = %d !?\n"), dwThreadEnd);
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
void CIPC::DoRequestBitrate()
{
	WriteCmd(CIPC_BASE_REQUEST_BITRATE);
}
/*********************************************************************************************
 Class      : CIPC
 Function   : OnRequestBitrate
 Description: default action is to send DoConfirmBitrate(0)

 Parameters: None 

 Result type:  (void)
 created: September, 26 2003
 <TITLE OnRequestBitrate>
*********************************************************************************************/
void CIPC::OnRequestBitrate()
{
	DoConfirmBitrate(0);	// CAVEAT default action
}
//////////////////////////////////////////////////////////////////////////////
void CIPC::DoConfirmBitrate(DWORD dwBitrate)
{
	WriteCmd(CIPC_BASE_CONFIRM_BITRATE,dwBitrate);
}
//////////////////////////////////////////////////////////////////////////////
void CIPC::OnConfirmBitrate(DWORD /*dwBitrate*/)
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////////////
BOOL CIPC::HandleCmd(DWORD dwCmd,
						DWORD dwParam1, DWORD dwParam2,
						DWORD dwParam3, DWORD dwParam4,
						const CIPCExtraMemory *pExtraMem
						)
{
	BOOL isOkay=TRUE;
	switch (dwCmd) 
	{
		case CIPC_BASE_REQ_CONNECTION:
		{
			m_state=CIPC_STATE_CONNECTED;
// TODO gf Here it is just the BASE connection of CIPC (READ channel and WRITE channel)!
// No connectionrecord is provided, so it never contains a codepage!
// Result: m_wCodePage is never changed!
// Yet keep the code to avoid side effects ...
#ifdef _UNICODE
			WORD wCodePage = m_wCodePage;
			m_wCodePage = 0;
			CIPCCmdRecord cmd(pExtraMem, dwCmd, dwParam1, dwParam2, dwParam3, dwParam4);
			CConnectionRecord cr(cmd);
			CString str;
			if (cr.GetFieldValue(CRF_CODEPAGE, str))
			{
				m_wCodePage = (WORD)_ttoi(str);
				WK_TRACE(_T("CIPC::HandleCmd %s Codepage[%s] from ConnectionRecord %u\n"), m_sName, str, m_wCodePage);
			}
			else
			{
				m_wCodePage = wCodePage;
//				WK_TRACE(_T("%s Codepage NOT in ConnectionRecord %u\n"), m_sName, m_wCodePage);
			}
#endif
			OnRequestConnection();
			DoConfirmConnection();
		}break;
		case CIPC_BASE_CONF_CONNECTION:
			m_state=CIPC_STATE_CONNECTED;	
			OnConfirmConnection();	// call virtual fn
			break;
	
		case CIPC_BASE_INDICATE_ERROR:
			{
				CSecID id=dwParam2;
				// NOT YET enum cast check
				CWK_String sErrorMsg;
				if (pExtraMem) 
				{
					const void* p = pExtraMem->GetAddress();
					if (p)
					{
						int nlen = pExtraMem->GetLength();
#ifdef _UNICODE
						sErrorMsg.InitFromMemory(m_wCodePage == CODEPAGE_UNICODE, p, nlen, m_wCodePage);
#else
						CopyMemory(sErrorMsg.GetBufferSetLength(nlen), p, nlen);
						sErrorMsg.ReleaseBuffer();
#endif
/*
						char *tmp= new char[pExtraMem->GetLength()+1];
						CopyMemory(tmp,p,pExtraMem->GetLength());
						tmp[pExtraMem->GetLength()]=0;	// terminate string
						sErrorMsg=tmp;
						WK_DELETE_ARRAY(tmp);
*/
					}
				}
				OnIndicateError(dwParam1,id,(CIPCError) dwParam3,dwParam4,sErrorMsg);
			}
			break;
		case CIPC_BASE_REQUEST_SET_GROUP_ID:
			// NOT YET, check for 0 in HIWORD
			OnRequestSetGroupID((WORD) dwParam1);
			break;
		case CIPC_BASE_CONFIRM_SET_GROUP_ID:
			// NOT YET, check for 0 in HIWORD
			OnConfirmSetGroupID((WORD) dwParam1);
			break;
		case CIPC_BASE_REQUEST_GET_VALUE:
			{
				// NOT YET
				CSecID id=dwParam1;
				DWORD dwServerData=dwParam2;
				if (pExtraMem) 
				{
					CString sKey = pExtraMem->GetString();
					if (id == SECID_NO_ID)
					{
						if (0 == sKey.CompareNoCase(_T("GetFileVersion")))
						{
							CString sVersion;
							CString sExe;
							GetModuleFileName(AfxGetApp()->m_hInstance,sExe.GetBufferSetLength(_MAX_PATH),_MAX_PATH);
							sExe.ReleaseBuffer();
							sVersion = WK_GetFileVersion(sExe);
							DoConfirmGetValue(id,sKey,sVersion,dwServerData);
							return isOkay;
						}
#ifdef _UNICODE
						else if (m_wCodePage == CODEPAGE_UNICODE)
						{
							if (sKey == CRF_CODEPAGE)	// answer only if unicode is used
							{
								CString sValue;
								sValue.Format(_T("%d"), m_wCodePage);
								WORD wCodePage = m_wCodePage;
								m_wCodePage = 0;		// but answer in !unicode
								DoConfirmGetValue(CSecID(), sKey, sValue, 0);
								m_wCodePage = wCodePage;
								return isOkay;
							}
						}
#endif
					}
					OnRequestGetValue(id,sKey,dwServerData);
				} 
				else 
				{
					OnMissingExtraMemory(dwCmd);
				}
			}
			break;
		case CIPC_BASE_CONFIRM_GET_VALUE:
			{
				CSecID id=dwParam1;
				DWORD dwLen1=dwParam2;
				// UNUSED DWORD dwLen2=dwParam3;
				DWORD dwServerData=dwParam4;
				if (pExtraMem) 
				{
					CString sBoth=pExtraMem->GetString();
					CString sKey,sValue;
					sKey=sBoth.Left(dwLen1);
					sValue=sBoth.Right(sBoth.GetLength()-dwLen1);
//					UTRACE(_T("Key:%s, Value:%s\n"), sKey, sValue);
#ifdef _UNICODE
					if (m_wCodePage != CODEPAGE_UNICODE)
					{
						if (sKey == CRF_CODEPAGE)		// this is answered only if !unicode
						{
							m_wCodePage = (WORD)_ttoi(sValue);// and must be unicode
							ASSERT(m_wCodePage == CODEPAGE_UNICODE);
							return isOkay;
						}
					}
#endif
					OnConfirmGetValue(id, sKey, sValue, dwServerData);
				}
			}
			break;
		case CIPC_BASE_CONFIRM_SET_VALUE:
			{
				CSecID id=dwParam1;
				DWORD dwLen1=dwParam2;
				// UNUSED DWORD dwLen2=dwParam3;
				DWORD dwServerData=dwParam4;
				if (pExtraMem) 
				{
					CString sBoth=pExtraMem->GetString();
					CString sKey,sValue;
					sKey=sBoth.Left(dwLen1);
					sValue=sBoth.Right(sBoth.GetLength()-dwLen1);
//					UTRACE(_T("CIPC_BASE_CONFIRM_SET_VALUE:Key:%s, Value:%s\n"), sKey, sValue);
					OnConfirmSetValue(id,sKey,sValue,dwServerData);
				}
			}
			break;

		case CIPC_BASE_REQUEST_SET_VALUE:
			{
				CSecID id=dwParam1;
				DWORD dwLen1=dwParam2;
				// UNUSED DWORD dwLen2=dwParam3;
				DWORD dwServerData=dwParam4;
				if (pExtraMem) 
				{
					CString sBoth=pExtraMem->GetString();
					CString sKey,sValue;
					sKey=sBoth.Left(dwLen1);
					sValue=sBoth.Right(sBoth.GetLength()-dwLen1);
//					UTRACE(_T("CIPC_BASE_REQUEST_SET_VALUE:Key:%s, Value:%s\n"), sKey, sValue);
					OnRequestSetValue(id,sKey,sValue,dwServerData);
				}
			}
			break;
		case CIPC_BASE_REQUEST_BITRATE:
			OnRequestBitrate();
			break;
		case CIPC_BASE_CONFIRM_BITRATE:
			OnConfirmBitrate(dwParam1);
			break;
		case CIPC_BASE_REQUEST_VERSION:
			OnRequestVersionInfo((WORD)dwParam1);
			break;
		case CIPC_BASE_CONFIRM_VERSION:
			OnConfirmVersionInfo((WORD)dwParam1, dwParam2);
			break;

		default:
			isOkay=FALSE;
	}
	return isOkay;
}	// end of HandleCmd

/*********************************************************************************************
 Class      : CIPC
 Function   : OnWaitFailed
 Description: OnWaitFailed is called, if a WAIT_FAILED occured in one of the
              WaitForMultipleObjects. This indicates some serious trouble with the
              interprocess communication.
              dwCmd is 0, if the error occured in the read wait. And != 0,
              if it occured in a write wait. Then {dwCmd} is the cmd, that
              could not be send.
              After the OnWaitFailed is called,  OnRequestDisconnect is performed. 
              relative functions: OnTimeoutCmdReceive, OnTimeoutWrite, OnRequestDisconnect


 Parameters:   
  dwCmd: (E): The Command that failed  (DWORD)

 Result type:  (void)
 created: March, 03 2003
 <TITLE OnWaitFailed>
*********************************************************************************************/
void CIPC::OnWaitFailed(DWORD /*dwCmd*/)
{
	// Hier, z.B. Meldung an debugger
}

/*********************************************************************************************
 Class      : CIPC
 Function   : OnTimeoutWrite
 Description: Called if it was not possible to send a command in the time specified by
              SetTimeoutWrite.
More details in later versions, in development.

 Parameters:   
  dwCmd: (E):    (DWORD)

 Result type: (TRUE, FALSE) (BOOL)
 created: March, 03 2003
 <TITLE OnTimeoutWrite>
*********************************************************************************************/
BOOL CIPC::OnTimeoutWrite(DWORD /*dwCmd*/)
{
	return TRUE;	// retry
}

/*********************************************************************************************
 Class      : CIPC
 Function   : OnTimeoutCmdReceive
 Description: Called if no command was received in the time specified by SetTimeoutCmdReceive.
              More details in later versions, in development.

 Parameters: None 

 Result type: (TRUE, FALSE) (BOOL)
 created: March, 03 2003
 <TITLE OnTimeoutCmdReceive>
*********************************************************************************************/
BOOL CIPC::OnTimeoutCmdReceive()
{
	return TRUE;	// OOPS, was kann das hier aussagen
}
/////////////////////////////////////////////////////////////////////////////
DWORD CIPC::GetLastCmd() const
{
	return m_dwLastCmdSend;
}
/////////////////////////////////////////////////////////////////////////////
const CIPCCmdRecord *CIPC::GetActiveCmdRecord() const
{
	return m_pActiveCmd;
}


/*********************************************************************************************
 Class      : CIPC
 Function   : SetTimeoutCmdReceive
 Description: Sets the timeout in milliseconds for receiving commands. 
              SetTimeoutCmdReceive is used in a WaitForMultipleObjects, if there is no event
              within dwTimeout milliseconds OnTimeoutCmdReceive is called.
              This value is only used when running in StartThread mode.
              relative functions: OnTimeoutCmdReceive, SetTimeoutWrite

 Parameters:   
  dwTimeout: (E): timeout in ms  (DWORD)

 Result type:  (void)
 created: March, 03 2003
 <TITLE SetTimeoutCmdReceive>
*********************************************************************************************/
void CIPC::SetTimeoutCmdReceive(
				DWORD dwTimeout
				)
{
	m_dwTimeoutCmdReceive=dwTimeout;
}

/*********************************************************************************************
 Class      : CIPC
 Function   : SetTimeoutWrite
 Description: Sets the timeout in milliseconds for writing commands. 
              SetTimeoutWrite is used in a WaitForMultipleObjects, if it is not possible to
              write/send a command within dwTimeout milliseconds OnTimeoutWrite called.
				  relative functions: OnTimeoutWrite, SetTimeoutCmdReceive

 Parameters:   
  dwTimeout: (E): Timeout in milliseconds  (DWORD)

 Result type: None (void)
 created: March, 03 2003
 <TITLE SetTimeoutWrite>
*********************************************************************************************/
void	CIPC::SetTimeoutWrite(DWORD dwTimeout)
{
	m_dwTimeoutWrite=dwTimeout;
}

/*********************************************************************************************
 Class      : CIPC
 Function   : SetConnectRetryTime
 Description: sets the timeout to wait before the next try is made to find a read channel.

 Parameters:   
  dwTimeout: (E): Timeout in milliseconds  (DWORD)

 Result type: None (void)
 created: March, 03 2003
 <TITLE SetConnectRetryTime>
*********************************************************************************************/
void CIPC::SetConnectRetryTime(DWORD dwTimeout)
{
	m_dwConnectRetryTime=dwTimeout;
}
//////////////////////////////////////////////////////////////////////////////
const _TCHAR *CIPC::NameOfCmd(DWORD dwCmd) 
{
	switch (dwCmd) 
	{
	// used by CIPC
	NAME_OF_CMD(CIPC_BASE_REQ_CONNECTION);
	NAME_OF_CMD(CIPC_BASE_CONF_CONNECTION);
	NAME_OF_CMD(CIPC_BASE_INDICATE_ERROR);
	
	NAME_OF_CMD(CIPC_BASE_REQUEST_SET_GROUP_ID);
	NAME_OF_CMD(CIPC_BASE_CONFIRM_SET_GROUP_ID);
	
	NAME_OF_CMD(CIPC_BASE_REQUEST_GET_VALUE);
	NAME_OF_CMD(CIPC_BASE_CONFIRM_GET_VALUE);
	
	NAME_OF_CMD(CIPC_BASE_REQUEST_SET_VALUE);
	NAME_OF_CMD(CIPC_BASE_CONFIRM_SET_VALUE);
	
	NAME_OF_CMD(CIPC_BASE_REQUEST_BITRATE);
	NAME_OF_CMD(CIPC_BASE_CONFIRM_BITRATE);
	
	NAME_OF_CMD(CIPC_BASE_REQUEST_VERSION);
	NAME_OF_CMD(CIPC_BASE_CONFIRM_VERSION);
	NAME_OF_CMD(CIPC_BASE_REQUEST_ALIVE);
	NAME_OF_CMD(CIPC_BASE_CONFIRM_ALIVE);

	// used by CIPCInput
	NAME_OF_CMD(INP_REQ_INFO);
	NAME_OF_CMD(INP_CONF_INFO);
	NAME_OF_CMD(INP_REQ_SETUP);
	NAME_OF_CMD(INP_CONF_SETUP);
	NAME_OF_CMD(INP_REQ_RESET);
	NAME_OF_CMD(INP_CONF_RESET);
	NAME_OF_CMD(INP_REQ_SETEDGE);
	NAME_OF_CMD(INP_CONF_EDGE);
	NAME_OF_CMD(INP_REQ_SETFREE);
	NAME_OF_CMD(INP_CONF_FREE);
	NAME_OF_CMD(INP_REQ_AL_STATE);
	NAME_OF_CMD(INP_CONF_AL_STATE);
	NAME_OF_CMD(INP_IND_AL_STATE);
	NAME_OF_CMD(INP_RESP_AL_STATE);
	NAME_OF_CMD(INP_REQ_HARDWARE);
	NAME_OF_CMD(INP_CONF_HARDWARE);
	NAME_OF_CMD(INP_REQUEST_GET_FILE);
	NAME_OF_CMD(INP_CONFIRM_GET_FILE);
	NAME_OF_CMD(INP_INDICATE_ALARM_NR);
	NAME_OF_CMD(INP_INDICATE_ALARM_FIELDS);
	NAME_OF_CMD(INP_INDICATE_COMM_DATA);
	NAME_OF_CMD(INP_WRITE_COMM_DATA);
	NAME_OF_CMD(INP_CONFIRM_WRITE_COMM_DATA);
	NAME_OF_CMD(INP_REQUEST_CAMERA_CONTROL);
	NAME_OF_CMD(INP_IND_ALARM_FIELD_STATE);
	NAME_OF_CMD(INP_IND_ALARM_FIELD_UPDATE);
	NAME_OF_CMD(INP_REQUEST_GET_SYSTEM_TIME);
	NAME_OF_CMD(INP_CONFIRM_GET_SYSTEM_TIME);
	NAME_OF_CMD(INP_REQUEST_SET_SYSTEM_TIME);
	NAME_OF_CMD(INP_CONFIRM_ALARM);

	NAME_OF_CMD(INP_REQUEST_GET_ALARM_OFF_SPANS);
	NAME_OF_CMD(INP_CONFIRM_GET_ALARM_OFF_SPANS);
	NAME_OF_CMD(INP_REQUEST_SET_ALARM_OFF_SPANS);
	NAME_OF_CMD(INP_CONFIRM_SET_ALARM_OFF_SPANS);

	NAME_OF_CMD(INP_REQUEST_GET_ZIME_ZONE_INFORMATION)
	NAME_OF_CMD(INP_CONFIRM_GET_ZIME_ZONE_INFORMATION)
	NAME_OF_CMD(INP_REQUEST_SET_ZIME_ZONE_INFORMATION)
	NAME_OF_CMD(INP_CONFIRM_SET_ZIME_ZONE_INFORMATION)
	NAME_OF_CMD(INP_REQUEST_GET_ZIME_ZONE_INFORMATIONS)
	NAME_OF_CMD(INP_CONFIRM_GET_ZIME_ZONE_INFORMATIONS)

	// used by CIPCOutput
	NAME_OF_CMD(OUTP_REQ_INFO);
	NAME_OF_CMD(CIPC_REQUEST_SYNC);
	NAME_OF_CMD(CIPC_CONFIRM_SYNC);
	NAME_OF_CMD(OUTP_CONF_INFO);
	NAME_OF_CMD(OUTP_REQ_SETUP);
	NAME_OF_CMD(OUTP_CONF_SETUP);
	NAME_OF_CMD(OUTP_REQ_RESET);
	NAME_OF_CMD(OUTP_CONF_RESET);
	NAME_OF_CMD(OUTP_REQUEST_SET_RELAY);
	NAME_OF_CMD(OUTP_CONFIRM_SET_RELAY);
	NAME_OF_CMD(OUTP_REQ_CUR_STATE	);
	NAME_OF_CMD(OUTP_CONF_CUR_STATE);
	NAME_OF_CMD(OUTP_REQ_HARDWARE);
	NAME_OF_CMD(OUTP_CONF_HARDWARE);

	NAME_OF_CMD(OUTP_INDICATE_CLIENT_ACTIVE);
	NAME_OF_CMD(OUTP_REQUEST_SELECT_CAMERA);
	NAME_OF_CMD(OUTP_REQUEST_ENCODED_VIDEO);
	NAME_OF_CMD(OUTP_INDICATE_CLIENT_CHANGED);

	NAME_OF_CMD(CIPC_OUTPUT_REQUEST_GET_MASK);
	NAME_OF_CMD(CIPC_OUTPUT_CONFIRM_GET_MASK);
	NAME_OF_CMD(CIPC_OUTPUT_REQUEST_SET_MASK);
	NAME_OF_CMD(CIPC_OUTPUT_CONFIRM_SET_MASK);
	NAME_OF_CMD(CIPC_OUTPUT_REQUEST_START_VIDEO);
	NAME_OF_CMD(CIPC_OUTPUT_REQUEST_STOP_VIDEO);
	NAME_OF_CMD(CIPC_OUTPUT_INDICATE_VIDEO);

	NAME_OF_CMD(PICT_REQ_JPEG_ENCODING)	
	NAME_OF_CMD(PICT_CONF_JPEG_ENCODING)	
	NAME_OF_CMD(PICT_REQ_WND_DISPLAY)	
	NAME_OF_CMD(PICT_CONF_WND_DISPLAY)	
	NAME_OF_CMD(PICT_REQ_WND_OUTP	)	
	NAME_OF_CMD(PICT_CONF_WND_OUTP)	
	NAME_OF_CMD(PICT_REQ_DECOMPRESS)	
	NAME_OF_CMD(PICT_CONF_DECOMPRESS);
	NAME_OF_CMD(PICT_REQ_UNIT_VIDEO);

	NAME_OF_CMD(PICT_REQ_START_H263);
	NAME_OF_CMD(PICT_CONF_START_H263);
	NAME_OF_CMD(PICT_REQ_STOP_H263);
	NAME_OF_CMD(PICT_CONF_STOP_H263);
	NAME_OF_CMD(PICT_INDICATE_H263_DATA);
	NAME_OF_CMD(PICT_REQ_DISPLAY_H263);
	NAME_OF_CMD(PICT_CONF_DISPLAY_H263);

	NAME_OF_CMD(OUTP_REQUEST_LOCAL_VIDEO);
	NAME_OF_CMD(OUTP_CONFIRM_LOCAL_VIDEO);
	NAME_OF_CMD(OUTP_INDICATE_LOCAL_VIDEO_DATA);
	NAME_OF_CMD(OUTP_REQUEST_STOP_H263_DECODING);
	NAME_OF_CMD(OUTP_CONFIRM_STOP_H263_DECODING);
	NAME_OF_CMD(OUTP_REQUEST_LOCAL_OVERLAY);
	NAME_OF_CMD(OUTP_CONFIRM_JPEG_ENCODING_NEW);
	NAME_OF_CMD(OUTP_CONFIRM_JPEG_DECODING_NEW);
	NAME_OF_CMD(OUTP_INDICATE_LOCAL_VIDEO_DATA_NEW);
	NAME_OF_CMD(OUTP_REQUEST_IDLE_PICTURES);

	// used by CIPCServerControl
	NAME_OF_CMD(SRV_CONTROL_REQUEST_OUTPUT_CONNECTION);
	NAME_OF_CMD(SRV_CONTROL_CONFIRM_OUTPUT_CONNECTION);
	NAME_OF_CMD(SRV_CONTROL_REQUEST_INPUT_CONNECTION);
	NAME_OF_CMD(SRV_CONTROL_CONFIRM_INPUT_CONNECTION);
	NAME_OF_CMD(SRV_CONTROL_REQUEST_ALARM_CONNECTION);
	NAME_OF_CMD(SRV_CONTROL_CONFIRM_ALARM_CONNECTION);
	NAME_OF_CMD(SRV_CONTROL_REQUEST_PIPE);
	NAME_OF_CMD(SRV_CONTROL_CONFIRM_PIPE);
	NAME_OF_CMD(SRV_CONTROL_REQUEST_REMOVE_PIPE);
	NAME_OF_CMD(SRV_CONTROL_CONFIRM_REMOVE_PIPE);
	NAME_OF_CMD(SRV_CONTROL_REQUEST_AUDIO_CONNECTION);
	NAME_OF_CMD(SRV_CONTROL_CONFIRM_AUDIO_CONNECTION);
	NAME_OF_CMD(SRV_CONTROL_REQUEST_DB_CONNECTION);
	NAME_OF_CMD(SRV_CONTROL_CONFIRM_DB_CONNECTION);

	// used by CIPCDatabase
	NAME_OF_CMD(CIPC_DB_REQUEST_INFO);
	NAME_OF_CMD(CIPC_DB_CONFIRM_INFO);
	NAME_OF_CMD(CIPC_DB_REQUEST_BACKUP_INFO);
	NAME_OF_CMD(CIPC_DB_CONFIRM_BACKUP_INFO);
	NAME_OF_CMD(CIPC_DB_REQUEST_SAVE_PICTURE);
	NAME_OF_CMD(CIPC_DB_REQUEST_QUERY);
	NAME_OF_CMD(CIPC_DB_CONFIRM_QUERY);

	NAME_OF_CMD(CIPC_DB_INDICATE_QUERY_RESULT);
	NAME_OF_CMD(CIPC_DB_RESPONSE_QUERY_RESULT);
	NAME_OF_CMD(CIPC_DB_REQUEST_SAVE_BY_HOST);

	NAME_OF_CMD(CIPC_DB_INDICATE_REMOVE_ARCHIV);
	NAME_OF_CMD(CIPC_DB_INDICATE_NEW_ARCHIV);

	// new 4.0
	NAME_OF_CMD(CIPC_DB_REQUEST_SEQUENCE_LIST);
	NAME_OF_CMD(CIPC_DB_CONFIRM_SEQUENCE_LIST);
	NAME_OF_CMD(CIPC_DB_REQUEST_RECORD_NR);
	NAME_OF_CMD(CIPC_DB_CONFIRM_RECORD_NR);
	NAME_OF_CMD(CIPC_DB_REQUEST_DELETE_SEQUENCE);
	NAME_OF_CMD(CIPC_DB_INDICATE_DELETE_SEQUENCE);
	NAME_OF_CMD(CIPC_DB_INDICATE_NEW_SEQUENCE);
	NAME_OF_CMD(CIPC_DB_REQUEST_FIELD_INFO);
	NAME_OF_CMD(CIPC_DB_CONFIRM_FIELD_INFO);
	NAME_OF_CMD(CIPC_DB_NEW_REQUEST_SAVE);
	NAME_OF_CMD(CIPC_DB_NEW_REQUEST_SAVE_FOR_HOST);
	NAME_OF_CMD(CIPC_DB_REQUEST_NEW_QUERY);
	NAME_OF_CMD(CIPC_DB_INDICATE_NEW_QUERY_RESULT);

	NAME_OF_CMD(CIPC_DB_REQUEST_DRIVES);
	NAME_OF_CMD(CIPC_DB_CONFIRM_DRIVES);
	NAME_OF_CMD(CIPC_DB_REQUEST_BACKUP);
	NAME_OF_CMD(CIPC_DB_CONFIRM_BACKUP);
	NAME_OF_CMD(CIPC_DB_REQUEST_CANCEL_BACKUP);
	NAME_OF_CMD(CIPC_DB_CONFIRM_CANCEL_BACKUP);
	NAME_OF_CMD(CIPC_DB_REQUEST_DELETE_ARCHIV);
	NAME_OF_CMD(CIPC_DB_REQUEST_CAMERA_NAMES);
	NAME_OF_CMD(CIPC_DB_CONFIRM_CAMERA_NAMES);
	NAME_OF_CMD(CIPC_DB_NEW_CONFIRM_SAVE);
	NAME_OF_CMD(CIPC_DB_REQUEST_BACKUP_BY_TIME);
	NAME_OF_CMD(CIPC_DB_CONFIRM_BACKUP_BY_TIME);
	NAME_OF_CMD(CIPC_DB_REQUEST_ALARM_LIST_ARCHIVES);
	NAME_OF_CMD(CIPC_DB_CONFIRM_ALARM_LIST_ARCHIVES);
	NAME_OF_CMD(CIPC_DB_REQUEST_ALARM_LIST_RECORD);
	NAME_OF_CMD(CIPC_DB_CONFIRM_ALARM_LIST_RECORD);
	NAME_OF_CMD(CIPC_DB_REQUEST_SAVE_DATA);
	NAME_OF_CMD(CIPC_DB_REQUEST_RECORDS);
	NAME_OF_CMD(CIPC_DB_CONFIRM_RECORDS);
	NAME_OF_CMD(CIPC_DB_REQUEST_OPEN_SEQUENCE);
	NAME_OF_CMD(CIPC_DB_REQUEST_CLOSE_SEQUENCE);
	NAME_OF_CMD(CIPC_DB_REQUEST_SAVE_MEDIA);
	NAME_OF_CMD(CIPC_DB_CONFIRM_RECORD_NR_MEDIA);

	NAME_OF_CMD(CIPC_MEDIA_REQUESTRESET);
	NAME_OF_CMD(CIPC_MEDIA_CONFIRMRESET);
	NAME_OF_CMD(CIPC_MEDIA_REQUESTHARDWARE);
	NAME_OF_CMD(CIPC_MEDIA_CONFIRMHARDWARE);
	NAME_OF_CMD(CIPC_MEDIA_REQUESTSTART_ENCODING);
	NAME_OF_CMD(CIPC_MEDIA_REQUESTSTOP_ENCODING);
	NAME_OF_CMD(CIPC_MEDIA_INDICATE_DATA);
	NAME_OF_CMD(CIPC_MEDIA_REQUEST_DECODING);
	NAME_OF_CMD(CIPC_MEDIA_REQUESTSTOP_DECODING);
	NAME_OF_CMD(CIPC_MEDIA_REQUEST_VALUES);
	NAME_OF_CMD(CIPC_MEDIA_CONFIRM_VALUES);
	NAME_OF_CMD(CIPC_MEDIA_REQUEST_INFO);
	NAME_OF_CMD(CIPC_MEDIA_CONFIRM_INFO);

	default: 
		// OOPS does not work if called multiple in one call
		static _TCHAR tmpString[20];	
		_stprintf(tmpString, _T("[%lu]"), dwCmd);
		return tmpString;
	}
}

//////////////////////////////////////////////////////////////
// returns a string representation of the enum value.
extern const _TCHAR *NameOfEnum(CIPCState s)
{
	switch (s)
	{
		case CIPC_STATE_INVALID:			return _T("CIPC_STATE_INVALID"); break;
		case CIPC_STATE_WRITE_CREATED:	return _T("CIPC_STATE_WRITE_CREATED"); break;
		case CIPC_STATE_READ_FOUND:		return _T("CIPC_STATE_READ_FOUND"); break;
		case CIPC_STATE_CONNECTED:			return _T("CIPC_STATE_CONNECTED"); break;
		case CIPC_STATE_DISCONNECTED:		return _T("CIPC_STATE_DISCONNECTED"); break;
		default:
			return _T("invalid CIPC_STATE_XXX");
	}

}
//////////////////////////////////////////////////////////////
// returns a string representation of the enum value.
extern const _TCHAR *NameOfEnum(CIPCError e)
{
	switch (e) 
	{
		NAME_OF_ENUM(CIPC_ERROR_OKAY);
		NAME_OF_ENUM(CIPC_ERROR_USER_DEFINED);
		NAME_OF_ENUM(CIPC_ERROR_INVALID_GROUP);
		NAME_OF_ENUM(CIPC_ERROR_INVALID_SUBID);
		NAME_OF_ENUM(CIPC_ERROR_UNHANDLED_CMD);
		NAME_OF_ENUM(CIPC_ERROR_INVALID_RANGE);
		NAME_OF_ENUM(CIPC_ERROR_INVALID_VALUE);
		NAME_OF_ENUM(CIPC_ERROR_UNKNOWN);
		NAME_OF_ENUM(CIPC_ERROR_INVALID_USER);
		NAME_OF_ENUM(CIPC_ERROR_INVALID_PERMISSIONS);
		NAME_OF_ENUM(CIPC_ERROR_INVALID_PASSWORD);
		NAME_OF_ENUM(CIPC_ERROR_INVALID_HOST);
			// errorCode 1:link-modul not found,  2: server not found,
			//			 3: server performs reset 4: max clients reached
		NAME_OF_ENUM(CIPC_ERROR_UNABLE_TO_CONNECT);
			//
		NAME_OF_ENUM(CIPC_ERROR_INVALID_ARCHIVE);
		NAME_OF_ENUM(CIPC_ERROR_ARCHIVE_FULL);
		NAME_OF_ENUM(CIPC_ERROR_ARCHIVE);
		NAME_OF_ENUM(CIPC_ERROR_ARCHIVE_SEARCH);
		NAME_OF_ENUM(CIPC_ERROR_ARCHIVE_LOCAL_SEARCH);
			//
		NAME_OF_ENUM(CIPC_ERROR_CANT_DECODE);
			//
		NAME_OF_ENUM(CIPC_ERROR_CAMERA_NOT_PRESENT);
			//
		NAME_OF_ENUM(CIPC_ERROR_FILE_UPDATE);
			//
		NAME_OF_ENUM(CIPC_ERROR_GET_FILE);

		NAME_OF_ENUM(CIPC_ERROR_ACCESS_DENIED);
		NAME_OF_ENUM(CIPC_ERROR_BACKUP);
		NAME_OF_ENUM(CIPC_ERROR_BUSY);
		NAME_OF_ENUM(CIPC_ERROR_CANCELLED);
		NAME_OF_ENUM(CIPC_ERROR_MEMORY);
		NAME_OF_ENUM(CIPC_ERROR_INVALID_SERIAL);
		NAME_OF_ENUM(CIPC_ERROR_ALIVE_UNCONFIRMED);
		NAME_OF_ENUM(CIPC_ERROR_DATA_CARRIER);
		NAME_OF_ENUM(CIPC_ERROR_TIMEOUT);
		NAME_OF_ENUM(CIPC_ERROR_VIDEO_STREAM_ABORTED);

			//
		default : 
			return _T("invalid CIPC_ERROR_XXX");
	}
}
//////////////////////////////////////////////////////////////////////////////
BOOL CIPC::OnRetryWriteChannel()
{
#ifdef _DEBUG
	WK_TRACE(_T("%x:%s:Trying CreateWriteChannel again\n"),
						GetCurrentProcess(), GetShmName());
#endif
	return TRUE;
}
//////////////////////////////////////////////////////
void CIPC::OnCommandSendError(DWORD dwCmd)
{
	if (   m_state == CIPC_STATE_CONNECTED
		|| m_state == CIPC_STATE_READ_FOUND)
	{
		WK_TRACE(_T("%s cannot send command %s, %s %d %08lx\n"), 
			GetShmName(), 
			NameOfCmd(dwCmd),
			NameOfEnum(m_state),
			m_bThreadRun,
			m_dwRunCount);
	}
}
//////////////////////////////////////////////////////
void CIPC::OnCommandReceiveError(DWORD dwCmd)
{
	WK_TRACE(_T("%s cannot receive command %s\n"),
			 GetShmName(), NameOfCmd(dwCmd));
}
//////////////////////////////////////////////////////
void CIPC::OverrideError()
{
	// called from OnXXX functions, if they are not overridden from subclasses
	const CIPCCmdRecord *pCmd = GetActiveCmdRecord();
	if (pCmd) 
	{
		WK_TRACE(_T("CIPC(%s) cmd %s not overridden\n"), GetShmName(), NameOfCmd(pCmd->m_dwCmd));
		DoIndicateError(pCmd->m_dwCmd,SECID_NO_ID,CIPC_ERROR_UNHANDLED_CMD,0);
	} 
	else 
	{
		WK_TRACE_ERROR(_T("CIPC(%s):OverrideError, but no cmd ?\n"), GetShmName());
	}
}
//////////////////////////////////////////////////////////////////////////////
void CIPC::OnMissingExtraMemory(DWORD dwCmd)
{
	// called from OnXXX functions, if they are not overridden from subclasses
	const CIPCCmdRecord *pCmd = GetActiveCmdRecord();
	if (pCmd) 
	{
		WK_TRACE(_T("CIPC(%s) cmd %s misses its data\n"),
			GetShmName(),
			NameOfCmd(pCmd->m_dwCmd)
			);
	} 
	else 
	{
		WK_TRACE_ERROR(_T("CIPC(%s):BubbleMissingError, but no cmd ?\n"), GetShmName());
	}
}
//////////////////////////////////////////////////////////////////////////////
void CIPC::InternalError(LPCTSTR szMessage)	// prepends CIPC %s with GetShmName
{
	if (szMessage) {
		WK_TRACE_ERROR(_T("Internal CIPC error [%s]: %s\n"),
			GetShmName(),
			szMessage
			);
	} else {
		WK_TRACE_ERROR(_T("Internal CIPC error [%s]: no message ?\n"),
			GetShmName()
			);
	}

}
//////////////////////////////////////////////////////////////////////////////
#ifdef _UNICODE
void CIPC::SetCodePage(WORD dw)
{
	m_wCodePage = dw;
	WK_TRACE(_T("%s Codepage set to %u\n"), m_sName, m_wCodePage);
}
//////////////////////////////////////////////////////////////////////////////
void CIPC::SetCodePage(CString &sCP)
{
	m_wCodePage = (WORD)_ttoi(sCP);
	WK_TRACE(_T("%s Codepage[%s] set to %u\n"), m_sName, sCP, m_wCodePage);
}
#endif
void CIPC::TraceOutstandingCmds()
{
	int iResult;
	POSITION pos;
	CIPCCmdRecord *pCmd;
	CString str, sFmt;
	m_receivedCmdsQueue.Lock();
	iResult = m_receivedCmdsQueue.GetCount();
	if (iResult)
	{
		pos = m_receivedCmdsQueue.GetHeadPosition();
		while (pos)
		{
			 pCmd = m_receivedCmdsQueue.GetNext(pos);
			 sFmt.Format(_T("%d, %x, %d|"), pCmd->m_dwCmd, pCmd->m_dwCmd, pCmd->m_pExtraMemory ? TRUE : FALSE);
			 str += sFmt;
		}
		WK_TRACE(_T("Received: %s\n"), str);
	}
	m_receivedCmdsQueue.Unlock();

	m_cmdQueueList.Lock();
	iResult = m_cmdQueueList.GetCount();
	if (iResult)
	{
		str.Empty();
		pos = m_cmdQueueList.GetHeadPosition();
		while (pos)
		{
			 pCmd = m_cmdQueueList.GetNext(pos);
			 sFmt.Format(_T("%d, %x, %d|"), pCmd->m_dwCmd, pCmd->m_dwCmd, pCmd->m_pExtraMemory ? TRUE : FALSE);
			 str += sFmt;
		}
		WK_TRACE(_T("Sent: %s\n"), str);
	}
	m_cmdQueueList.Unlock();
}
#ifdef _WK_CLASSES_DLL
void SetCipcCodePage(UINT uCP)
{
	CWK_String::SetCodePage(uCP);
}
#endif