/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CipcChannel.cpp $
// ARCHIVE:		$Archive: /Project/CIPC/CipcChannel.cpp $
// CHECKIN:		$Date: 20.01.06 9:28 $
// VERSION:		$Revision: 41 $
// LAST CHANGE:	$Modtime: 19.01.06 20:30 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdcipc.h"

#include "CipcChannel.h"
#include "CipcExtraMemory.h"


#define _USE_CIPC_CHANNEL_MUTEX
//
//
// PART: CIPCChannel
//
//
static void ResetSharedMemory(CIPC_SHM_STRUCT *pSM)
{
	// Shared Memory reset für Write Kanäle
	pSM->dwMagicNumber=987654;
	//
	pSM->dwCmd	= 0;
	pSM->dwParam1	= 0;
	pSM->dwParam2	= 0;
	pSM->dwParam3	= 0;
	pSM->dwParam4	= 0;
	// extra memory
	pSM->hExtraMemory	= NULL;
	pSM->dwExtraMemUsed =0;
	// process/thread info
	pSM->dwProcessID = 0;
	pSM->hThread=NULL;
	//
	pSM->dwVersion = CIPC_VERSION;
}

CIPCChannel::CIPCChannel(CIPC *pParent)
{
	m_pParent		= pParent;
	m_hMap			= NULL;
	m_hMutex		= NULL;
	m_pSharedMemoryStruct = NULL;
	m_hWriteProcess	= NULL;
	m_hEventCmdSend	= NULL;
	m_hEventCmdDone	= NULL;
	m_pChannelExtraMemory	= NULL;
	m_hEventDisconnect = NULL;
}

CIPCChannel::~CIPCChannel()
{
	WK_CLOSE_HANDLE(m_hEventCmdSend);
	WK_CLOSE_HANDLE(m_hEventCmdDone);
	WK_CLOSE_HANDLE(m_hWriteProcess);
	WK_CLOSE_HANDLE(m_hEventDisconnect);

	if (m_pSharedMemoryStruct) {
		UnmapViewOfFile(m_pSharedMemoryStruct);
		m_pSharedMemoryStruct=NULL;
	}
	WK_CLOSE_HANDLE(m_hMap);
	WK_DELETE(m_pChannelExtraMemory);

	WK_CLOSE_HANDLE(m_hMutex);
}

// NEW behaviour: 
// -write: only does Create
// -read: only does Open
//
BOOL CIPCChannel::CreateFileMapping(LPCTSTR szMutexName, 
									LPCTSTR szSharedMemory, 
									BOOL bReadAndWrite,
									BOOL asMaster)
{
	DWORD dwAccess, dwProtection;
	if (bReadAndWrite) 
	{
		dwAccess=FILE_MAP_WRITE;
		dwProtection = PAGE_READWRITE;
	} 
	else 
	{
		dwAccess = FILE_MAP_READ;
		dwProtection = PAGE_READONLY;
	}

#ifdef _USE_CIPC_CHANNEL_MUTEX
	WK_CLOSE_HANDLE(m_hMutex);	// drop possible old one
	if (bReadAndWrite) 
	{
		// check for existing mutex
		m_hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, szMutexName);
		if (m_hMutex==NULL) 
		{
			// free to create
			// seconrd arg "initialy owned"
			m_hMutex = CreateMutex(NULL, FALSE, szMutexName);
			if (m_hMutex==NULL)
			{
				TRACE(_T("Could not create Mutex %s\n"), szSharedMemory, szMutexName);
			}
		}
		else 
		{
			// already there, BUSY!
			WK_CLOSE_HANDLE(m_hMutex);
		}
	}
	else 
	{	// read-only
		m_hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, szMutexName);
	}

	if (m_hMutex==NULL) 
	{
		return FALSE;	// >>>EXIT<<<
	}
	AccessMutex();
#endif
	
	// create/get a handle to a filemapping
	if (bReadAndWrite)
	{	// write
		int iNumTries=0;
		BOOL bChannelFree=FALSE;
		// first check is there is an old, or already existing handle
		do 
		{	// if there is an old handle, we have to retry
			m_hMap = OpenFileMapping(dwAccess, FALSE, szSharedMemory);
			if (m_hMap) 
			{
				WK_CLOSE_HANDLE(m_hMap);
				Sleep(10);	// give units time to disconnect and free channel
				bChannelFree=FALSE;
				iNumTries++;
			} 
			else 
			{
				bChannelFree=TRUE;
				if (iNumTries) 
				{
					WK_TRACE(_T("CIPC %s: write channel free after %d tries\n"), szSharedMemory, iNumTries);
				}
			}
		} while (iNumTries < 100 && bChannelFree==FALSE);

		if (m_hMap) 
		{
			// NOT YET, still there is an old one, what now ?
			WK_TRACE(_T("CIPC %s:Could not create write channel, old one still there\n"), szSharedMemory);
		} 
		else 
		{
			m_hMap = ::CreateFileMapping((HANDLE)INVALID_HANDLE_VALUE, 
							(LPSECURITY_ATTRIBUTES)NULL, 
							dwProtection, 
							0, sizeof(CIPC_SHM_STRUCT),		// min/max Size
							szSharedMemory);
			if (m_hMap) 
			{
				// FINE
			} 
			else 
			{
				WK_TRACE(_T("CIPC:Failed to create write channel %s\n"), szSharedMemory);
			}
		}
	}
	else 
	{	// readonly
		m_hMap = OpenFileMapping(dwAccess, FALSE, szSharedMemory);
	}

	// if there is a handle, do the mapping and fill info in the write channel
	if (m_hMap) 
	{
		// now get the address from the handle, via mapping
		m_pSharedMemoryStruct = (CIPC_SHM_STRUCT*)MapViewOfFile(m_hMap, dwAccess, 
											0, 0, // offsets
											sizeof(CIPC_SHM_STRUCT)
											);
		if (m_pSharedMemoryStruct==NULL) 
		{
			TRACE(_T("MapViewOfFile %s failed\n"), szSharedMemory);
		}
		else 
		{
			// fill processID and threadHandle into the channel
			if (bReadAndWrite)
			{
				ResetSharedMemory(m_pSharedMemoryStruct);
				// set the processID
				m_pSharedMemoryStruct->dwProcessID = GetCurrentProcessId();
				HANDLE pseudoHandle = GetCurrentThread();
				BOOL bCopyOkay = DuplicateHandle(
					GetCurrentProcess(),	// source process
					pseudoHandle,	// source handle
					GetCurrentProcess(),				// target process
					&m_pSharedMemoryStruct->hThread,	// target handle
					THREAD_QUERY_INFORMATION,		// ignored if SAME_ACCESS
					FALSE,						// inherit flag
					0							// options
					);	
				
				if (bCopyOkay==FALSE) 
				{
					TRACE(_T("%s:Failed to copy thread handle, error %s\n"), szSharedMemory, LPCTSTR(GetLastErrorString()));
				}
			}
			else 
			{ 
				// readonly
				m_hWriteProcess = OpenProcess(
									( PROCESS_ALL_ACCESS
									| PROCESS_DUP_HANDLE
									| PROCESS_QUERY_INFORMATION
									| SYNCHRONIZE), 
										TRUE,	// bInherithandle
										m_pSharedMemoryStruct->dwProcessID);
				if (m_hWriteProcess==NULL) {
					WK_TRACE_ERROR(_T("Failed to copy write process handle\n"));
				}
			}
		}
	}	// end of valid hMap
#ifdef _USE_CIPC_CHANNEL_MUTEX
	ReleaseMutex();
#endif

	return (m_hMap!=NULL && m_pSharedMemoryStruct!=NULL);
}

BOOL CIPCChannel::CreateEvents(const CString &shmName, BOOL bReadAndWrite, BOOL asMaster)
{
	CString sName=shmName;
	CString szEventName;

	if (bReadAndWrite) 
	{
		if (asMaster) sName += _T("MasterEvent");
		else sName += _T("SlaveEvent");
	} 
	else
	{	// readChannel
		if (asMaster) sName += _T("SlaveEvent");
		else sName += _T("MasterEvent");
	}

	szEventName = sName + _T("Send");

	// try to open existing one, else create it
	m_hEventCmdSend = OpenEvent(EVENT_MODIFY_STATE|SYNCHRONIZE, FALSE, szEventName);
	if (m_hEventCmdSend==NULL) 
	{
		// CreateEvent(securityAttr, bManualReset, bInitialState );
		m_hEventCmdSend = CreateEvent(NULL, TRUE, FALSE, szEventName);
	}
	if (m_hEventCmdSend) 
	{
		if (bReadAndWrite) 
		{
			ResetEvent(m_hEventCmdSend);
		}
	} 
	else 
	{
		TRACE(_T("FAILED to create %s\n"),szEventName);
	}

	szEventName = sName;
	szEventName += _T("Done");
	// try to open existing one, else create it
	m_hEventCmdDone = OpenEvent(EVENT_MODIFY_STATE|SYNCHRONIZE, FALSE, szEventName);
	if (m_hEventCmdDone==NULL) 
	{
		// CreateEvent(securityAttr, bManualReset, bInitialState );
		m_hEventCmdDone = CreateEvent(NULL, TRUE, FALSE, szEventName);
	}
	if (m_hEventCmdDone) 
	{
		if (bReadAndWrite) 
		{
			SetEvent(m_hEventCmdDone);
		}
	} 
	else 
	{
		TRACE(_T("FAILED to create %s\n"),szEventName);
	}

	szEventName = sName;
	szEventName += _T("Disconnect");
	// try to open existing one, else create it
	m_hEventDisconnect = OpenEvent(EVENT_MODIFY_STATE|SYNCHRONIZE, FALSE, szEventName);
	if (m_hEventDisconnect==NULL) {
		// CreateEvent(securityAttr, bManualReset, bInitialState );
		m_hEventDisconnect = CreateEvent(NULL, TRUE, FALSE, szEventName);
	}
	if (m_hEventDisconnect)
	{
		if (bReadAndWrite) 
		{
			ResetEvent(m_hEventDisconnect);
		}
	} 
	else 
	{
		TRACE(_T("FAILED to create %s\n"), szEventName);
	}

	return (m_hEventCmdSend!=NULL && m_hEventCmdDone!=NULL && m_hEventDisconnect!=NULL);

}

void CIPCChannel::AccessMutex()
{
#ifdef _USE_CIPC_CHANNEL_MUTEX
	if (m_hMutex)
	{
		DWORD dwRet = WaitForSingleObject(m_hMutex,5000);
		switch (dwRet) 
		{
		case WAIT_OBJECT_0: // fine
			break;
		case WAIT_FAILED:
			WK_TRACE(_T("CIPC WAIT_FAILED AccessMutex\n"), m_pParent->GetShmName());
			break;
		default:
			WK_TRACE(_T("CIPC %s:Could not access Mutex\n"), m_pParent->GetShmName());
		}
	}
#endif
}
void CIPCChannel::ReleaseMutex()
{
#ifdef _USE_CIPC_CHANNEL_MUTEX
	if (m_hMutex)
	{
		if (::ReleaseMutex(m_hMutex)==FALSE)
		{
			WK_TRACE(_T("CIPC %s:Could not release Mutex [%s]\n"), m_pParent->GetShmName(), LPCTSTR(GetLastErrorString()));
		}
	}
#endif
}
////////////////////////////////////////////////////////////////////////
void CIPCChannel::CreateExtraMemory(const CIPCExtraMemory &src)
{
	WK_DELETE(m_pChannelExtraMemory);
	m_pChannelExtraMemory = new CIPCExtraMemory(src);
	m_pSharedMemoryStruct->hExtraMemory = m_pChannelExtraMemory->GetHandle();
	m_pSharedMemoryStruct->dwExtraMemUsed = m_pChannelExtraMemory->GetLength();
}
////////////////////////////////////////////////////////////////////////
void CIPCChannel::ReleaseExtraMemory()
{
	WK_DELETE(m_pChannelExtraMemory);
	m_pSharedMemoryStruct->dwExtraMemUsed = 0;
	m_pSharedMemoryStruct->hExtraMemory = NULL;
}
////////////////////////////////////////////////////////////////////////
void CIPCChannel::SetCmd(const CIPCCmdRecord& cmd)
{
	AccessMutex();
	
	ReleaseExtraMemory();

	if (cmd.m_pExtraMemory)
	{
		CreateExtraMemory(*cmd.m_pExtraMemory);
	}
	m_pSharedMemoryStruct->dwCmd	= cmd.m_dwCmd;
	m_pSharedMemoryStruct->dwParam1	= cmd.m_dwParam1;
	m_pSharedMemoryStruct->dwParam2	= cmd.m_dwParam2;
	m_pSharedMemoryStruct->dwParam3	= cmd.m_dwParam3;
	m_pSharedMemoryStruct->dwParam4	= cmd.m_dwParam4;

	// reset the done event, and set the 'here is a cmd'
	if (!ResetEvent(m_hEventCmdDone))
	{
		TRACE(_T("%s ResetEvent failed\n"),m_pParent->GetShmName());
	}
	if (!SetEvent(m_hEventCmdSend))
	{
		TRACE(_T("%s SetEvent failed\n"),m_pParent->GetShmName());
	}

	ReleaseMutex();
}
////////////////////////////////////////////////////////////////////////
CIPCCmdRecord* CIPCChannel::GetCmd()
{
	AccessMutex();


	if (m_pSharedMemoryStruct->dwCmd==0)
	{
		ReleaseMutex();
		return NULL;
	}
	CIPCCmdRecord* pCmd = NULL;

	// read a real command
	CIPCExtraMemory* pExtraMemory = NULL;

	if (m_pSharedMemoryStruct->dwExtraMemUsed>0)
	{
		pExtraMemory = new CIPCExtraMemory(this);
		if (pExtraMemory->GetAddress() == NULL)
		{
			WK_TRACE(_T("Deleting Extramemory, not mapped\n"));
			WK_DELETE(pExtraMemory); 
		}
	}

	pCmd = new CIPCCmdRecord(pExtraMemory,
								m_pSharedMemoryStruct->dwCmd,
								m_pSharedMemoryStruct->dwParam1,
								m_pSharedMemoryStruct->dwParam2,
								m_pSharedMemoryStruct->dwParam3,
								m_pSharedMemoryStruct->dwParam4);
	WK_DELETE(pExtraMemory);

	// ok, I got your 'I have a cmd' and now I tell you 'I read all data'
	// CmdDone actually means CmdRead, reading is done
	// CAVEAT: "GetExtraMemory" has copied from the handle already
	//		   the ReleaseExtraMemory after the HandleCmd will reset it.
	BOOL bEventOkay;
	bEventOkay = ResetEvent(m_hEventCmdSend);
	if (bEventOkay==FALSE) 
	{
		WK_TRACE(_T("ResetEvent FAILED\n"));
	}
	bEventOkay = SetEvent(m_hEventCmdDone);
	if (bEventOkay==FALSE) 
	{
		WK_TRACE(_T("SetEvent FAILED\n"));
	}

	ReleaseMutex();

	return pCmd;
}
