/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CIPCPipeSocket.cpp $
// ARCHIVE:		$Archive: /Project/SocketUnit/CIPCPipeSocket.cpp $
// CHECKIN:		$Date: 24.07.06 16:46 $
// VERSION:		$Revision: 49 $
// LAST CHANGE:	$Modtime: 24.07.06 15:31 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"
#include "SecID.h"
#include "CIPCPipeSocket.h"
#include "DataSocket.h"
#include "CipcExtraMemory.h"
#include "SocketUnitDoc.h"
#include "SocketUnit.h"

#include "SocketCmdRecord.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
void CIPCPipeSocket::OnReadChannelFound()
{
	SetConnectRetryTime(500);	// once connected, that should be okay
	CIPC::OnReadChannelFound();
}
/////////////////////////////////////////////////////////////////////////////
CIPCPipeSocket::CIPCPipeSocket(	CConnectionThread *pConnection,
							LPCTSTR pSMName,
						    CIPCType t,
							BOOL bAsMaster,
							DWORD dwOptions,
							WORD wID,
							LPCTSTR hostName
							)
			: CIPC(pSMName, bAsMaster)
{
#ifdef _DEBUG
	// OOPS WK_TRACE(_T("NewPipe %3d:%s\n"),wID, pSMName);
#endif
	SetConnectRetryTime(50);	// for the inital connection
	m_pConnection = pConnection;

	m_cipcType = t;
	m_wId = wID;
	m_pDataSocket = NULL; 
	m_pServerSocket = NULL; 
	m_dwOptions		= dwOptions;
	m_dwStarttimeForNotConnectedTimeout	= GetTickCount();
	m_dwNotConnectedTimeout = (DWORD)-1; // infinite til set
	m_dwBitrate = MyGetApp()->GetSelectedBitrate();
}
/////////////////////////////////////////////////////////////////////////////
CIPCPipeSocket::~CIPCPipeSocket()
{
	m_pConnection->GetDoc()->RemoveMe(this);	// OOPS not here but in connection

	StopThread();
	// CAVEAT do not delete the socket here
}
/////////////////////////////////////////////////////////////////////////////
BOOL CIPCPipeSocket::IsTimedOut() const
{
	BOOL bTimedOut = FALSE;
	if (   GetIPCState() != CIPC_STATE_CONNECTED
		&& GetIPCState() != CIPC_STATE_READ_FOUND
		)
	{
		bTimedOut = (GetTickCount() - m_dwStarttimeForNotConnectedTimeout
						>= m_dwNotConnectedTimeout);
	}
	return bTimedOut;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CIPCPipeSocket::PushCmd(DWORD dwDataLen, const void *pData,
							DWORD dwCmd,DWORD dwParam1, DWORD dwParam2, 
							DWORD dwParam3, DWORD dwParam4)
{
	int iNumRetries=0;
	while (GetIPCState()==CIPC_STATE_WRITE_CREATED && iNumRetries<200) {
		Sleep(25);	// max 200 * 25 ms
		iNumRetries++;
	}
	if (iNumRetries>=200) {
		TRACE(_T("ReadChannel not found\n"));
	} else if (iNumRetries!=0) {
		TRACE(_T("%s ReadChannel found after %d tries\n"),GetShmName(),iNumRetries);
	}
	
	//@ WK_TRACE(_T("@Push   %15s: %s MemoryLen: %u\n"), GetShmName(),NameOfCmd(dwCmd), dwDataLen);
	
	if (dwCmd==CIPC_BASE_CONF_CONNECTION) {
		CIPC::HandleCmd(CIPC_BASE_REQ_CONNECTION,dwParam1,dwParam2,dwParam3,dwParam4,NULL);
		return TRUE;
		
	} else 	{
		if (dwDataLen==0) 
		{
			return WriteCmd(dwCmd, dwParam1, dwParam2, dwParam3, dwParam4);
		}
		else
		{
			return WriteCmdWithData(dwDataLen, pData, dwCmd, dwParam1, dwParam2, dwParam3, dwParam4);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CIPCPipeSocket::HandleCmd(DWORD dwCmd,
								DWORD dwParam1, DWORD dwParam2,
								DWORD dwParam3, DWORD dwParam4,
								const CIPCExtraMemory* pExtraMem
								)
{
	if (GetIsMarkedForDelete()) {
		WK_TRACE(_T("HandleCmd still called after DelayedDelete !?\n"));
		return FALSE;
	}
#if 0
	if (pExtraMem) {
		TRACE(_T("@Handle %15s: %s len %d\n"), GetShmName(), NameOfCmd(dwCmd), pExtraMem->GetLength());
	} else {
		TRACE(_T("@Handle %15s: %s\n"), GetShmName(), NameOfCmd(dwCmd));
	}
#endif

	if (dwCmd==CIPC_BASE_CONF_CONNECTION) 
	{
		CIPC::HandleCmd(dwCmd,dwParam1,dwParam2,dwParam3,dwParam4,NULL);
	} 
	else if (dwCmd==CIPC_BASE_REQUEST_BITRATE) 
	{
		CIPC::HandleCmd(CIPC_BASE_REQUEST_BITRATE,dwParam1,dwParam2,dwParam3,dwParam4,NULL);
		return TRUE;
	}

	if (m_pConnection) 
	{
		m_pConnection->AddToSendQueue(new CSocketCmdRecord(m_wId, m_wRemoteId,
														   dwCmd,dwParam1,dwParam2,dwParam3,dwParam4,pExtraMem),TRUE);
	}
	else
	{
		WK_TRACE_ERROR(_T("internal error pipe without connection\n"));
	}

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CIPCPipeSocket::Disconnect()
{
	// OOPS gf try to cancel cancelled fetch earlier than from connecting timeout
	// same as OnRequestDisconnect(), but public access
	OnRequestDisconnect();
}
/////////////////////////////////////////////////////////////////////////////
void CIPCPipeSocket::OnRequestDisconnect()
{
	// CAEVAT no need for RemoveMe, it is done via ShutDown..OnClose anyway
	DelayedDelete();
	m_pConnection->OnRequestDisconnect();
}
/////////////////////////////////////////////////////////////////////////////
// called from socket
void CIPCPipeSocket::OnClose(int iError)
{
	DelayedDelete();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CIPCPipeSocket::OnTimeoutWrite(DWORD dwCmd)
{
	TRACE(_T("%s TIMEOUT write\n"),GetShmName());
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CIPCPipeSocket::OnTimeoutCmdReceive()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CIPCPipeSocket::OnRequestBitrate()
{
	DoConfirmBitrate(m_dwBitrate);
}
/////////////////////////////////////////////////////////////////////////////
void CIPCPipeSocket::SetRemoteId(WORD wRemoteId)
{
	m_wRemoteId = wRemoteId;
#ifdef _DEBUG
	// OOPS WK_TRACE(_T("Set RemoteId %d on %d\n"),m_wRemoteId,m_wId);
#endif
}
/////////////////////////////////////////////////////////////////////////////
LPCTSTR NameOfEnum(CIPCType ct)
{
	switch(ct) {
	case CIPC_TYPE_INVALID: return _T("CIPC_TYPE_INVALID"); break;
	case CIPC_INPUT_CLIENT	: return _T("CIPC_INPUT_CLIENT	"); break;
	case CIPC_OUTPUT_CLIENT	: return _T("CIPC_OUTPUT_CLIENT	"); break;
	case CIPC_INPUT_SERVER	: return _T("CIPC_INPUT_SERVER	"); break;
	case CIPC_OUTPUT_SERVER	: return _T("CIPC_OUTPUT_SERVER	"); break;
	case CIPC_DATABASE_CLIENT: return _T("CIPC_DATABASE_CLIENT"); break;
	case CIPC_DATABASE_SERVER	: return _T("CIPC_DATABASE_SERVER	"); break;
	case CIPC_CONTROL_INPUT: return _T("CIPC_CONTROL_INPUT"); break;
	case CIPC_CONTROL_OUTPUT: return _T("CIPC_CONTROL_OUTPUT"); break;
	case CIPC_CONTROL_DATABASE: return _T("CIPC_CONTROL_DATABASE"); break;
	case CIPC_ALARM_SERVER: return _T("CIPC_ALARM_SERVER"); break;
	case CIPC_ALARM_CLIENT: return _T("CIPC_ALARM_CLIENT"); break;
	case CIPC_CONTROL_AUDIO: return _T("CIPC_CONTROL_AUDIO"); break;
	case CIPC_AUDIO_SERVER: return _T("CIPC_AUDIO_SERVER"); break;
	case CIPC_AUDIO_CLIENT: return _T("CIPC_AUDIO_CLIENT"); break;
	default:
		return _T("CIPC_TYPE_ERROR");
	}
}
/////////////////////////////////////////////////////////////////////////////
LPCTSTR UserNameOfEnum(CIPCType ct)
{
	switch(ct) {
	case CIPC_TYPE_INVALID: return _T("Type unknown"); break;
	case CIPC_INPUT_CLIENT	: return _T("InputClient"); break;
	case CIPC_OUTPUT_CLIENT	: return _T("OutputClient"); break;
	case CIPC_INPUT_SERVER	: return _T("InputServer"); break;
	case CIPC_OUTPUT_SERVER	: return _T("OutputServer"); break;
	case CIPC_DATABASE_CLIENT: return _T("DatabaseClient"); break;
	case CIPC_DATABASE_SERVER	: return _T("DatabaseServer"); break;
	case CIPC_CONTROL_INPUT: return _T("ControlInput"); break;
	case CIPC_CONTROL_OUTPUT: return _T("ControlOutput"); break;
	case CIPC_CONTROL_DATABASE: return _T("ControlDatabase"); break;
	case CIPC_ALARM_SERVER: return _T("AlarmServer"); break;
	case CIPC_ALARM_CLIENT: return _T("AlarmClient"); break;
	case CIPC_CONTROL_AUDIO: return _T("ControlAudio"); break;
	case CIPC_AUDIO_SERVER: return _T("AudioServer"); break;
	case CIPC_AUDIO_CLIENT: return _T("AudioClient"); break;
	default:
		return _T("CIPC_TYPE_ERROR");
	}
}