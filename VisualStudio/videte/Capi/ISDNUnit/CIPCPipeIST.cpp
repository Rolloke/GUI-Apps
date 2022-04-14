/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CIPCPipeIST.cpp $
// ARCHIVE:		$Archive: /Project/Capi/ISDNUnit/CIPCPipeIST.cpp $
// CHECKIN:		$Date: 16.12.05 14:10 $
// VERSION:		$Revision: 12 $
// LAST CHANGE:	$Modtime: 16.12.05 12:31 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#include "stdafx.h"
#include "SecID.h"
#include "CIPCPipeIST.h"
#include "IsdnConnection.h"
#include "IsdnUnit.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


CIPCPipeIST::CIPCPipeIST(	CIPCType t,
						    CISDNConnection* pConnection,
							LPCTSTR pSMName,
							BOOL bAsMaster,
							DWORD dwOptions,
							WORD wID,
							DWORD dwRequestCounter,
							BOOL bIsAlarm
							)
			: CIPCPipeISDN(		// call super class constructor
			t, pConnection, pSMName,
			bAsMaster, dwOptions,wID,dwRequestCounter, bIsAlarm
			), CIPC(pSMName, bAsMaster)
{
#ifdef _UNICODE
	// at start we must assume NON-Unicode,
	// the CmdRecord/ConnectionRecord will be bubbled with this setting
	m_wCodePage = 0;
#endif
	SetTimeoutCmdReceive(60*1000*30);
	SetConnectRetryTime(50);
}

CIPCPipeIST::~CIPCPipeIST()
{
	StopThread();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CIPCPipeIST::PushCipcCmd(DWORD dwDataLen, const void *pData,
							DWORD dwCmd,DWORD dwParam1, DWORD dwParam2, 
							DWORD dwParam3, DWORD dwParam4)
{
	int iNumRetries=0;
	while (GetIPCState()==CIPC_STATE_WRITE_CREATED 
				&& iNumRetries<200) {
		Sleep(50);
		// wait on CIPC thread action
		iNumRetries++;
	}
	if (iNumRetries) {
		if (GetIPCState()==CIPC_STATE_CONNECTED) {
			WK_TRACE(_T("PushCmd() ID %x READ FOUND after %d tries\n"), GetID().GetID(),iNumRetries);
		}
		else {
			WK_TRACE(_T("PushCmd() ID %x STATE_CONNECTED NOT reached after %d tries\n"), GetID().GetID(),iNumRetries);
		}
	}

	if (dwCmd==CIPC_BASE_CONF_CONNECTION) 
	{
		CIPC::HandleCmd(CIPC_BASE_REQ_CONNECTION,dwParam1,dwParam2,dwParam3,dwParam4,NULL);
		return TRUE;	// <<< EXIT >>>
	} else {
		if (dwDataLen==0) {
			return WriteCmd(dwCmd, dwParam1, dwParam2, dwParam3, dwParam4);
		} else {
			return WriteCmdWithData(dwDataLen, pData, dwCmd, dwParam1, dwParam2, dwParam3, dwParam4);
		}
	}
}


BOOL CIPCPipeIST::HandleCmd(DWORD dwCmd,
								DWORD dwParam1, DWORD dwParam2,
								DWORD dwParam3, DWORD dwParam4,

								const CIPCExtraMemory* pExtraMem
								)
{
	if (theApp.TraceReceivedCommands())
	{
		WK_TRACE(_T("HandleCmd() ID %u PipeCmd[%s]: '%s'\n"), GetID().GetID(), NameOfEnum(GetCIPCType()), NameOfCmd(dwCmd));
	}
	
	// special for RequestBitrate: direct answer
	if ( dwCmd==CIPC_BASE_REQUEST_BITRATE) 
	{
		WORD wNrBChannels = m_pConnection->GetNumBChannels();
		DWORD dwBitrate = ((DWORD)(-1));
		if ( wNrBChannels >0 && wNrBChannels <= 2 )
		{
			dwBitrate = wNrBChannels * 64 * 1024;
		}
		DoConfirmBitrate(dwBitrate);
		return TRUE; /// <<< EXIT >>>
	} 

	if (dwCmd==CIPC_BASE_CONF_CONNECTION)
	{
		// adjust internal state in CIPC
		CIPC::HandleCmd(dwCmd,dwParam1,dwParam2,dwParam3,dwParam4,NULL);
	}

	// send to the other side
	m_pConnection->CipcDataRequest(	GetID(),
									GetRemoteID(),
									GetCIPCType(),
									dwCmd,
									dwParam1, dwParam2,
									dwParam3, dwParam4,
									pExtraMem);

	return TRUE;	// ignore result of sending (can be in reset)
}


/////////////////////////////////////////////////////////////////////////////
void CIPCPipeIST::OnReadChannelFound()
{
	TRACE(_T("ReadChannelFound '%s'\n"), GetShmName());	// OBSOLETE
}

/////////////////////////////////////////////////////////////////////////////
void CIPCPipeIST::OnRequestDisconnect()
{
	DelayedDelete();
	m_pConnection->RemoveMe(this);
}


CIPC *CIPCPipeIST::GetCIPC()	// non const!
{
	return this;
}