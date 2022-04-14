/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CIPCPipeIsdn.cpp $
// ARCHIVE:		$Archive: /Project/Capi/ISDNUnit/CIPCPipeIsdn.cpp $
// CHECKIN:		$Date: 20.01.06 12:11 $
// VERSION:		$Revision: 71 $
// LAST CHANGE:	$Modtime: 20.01.06 10:59 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"
#include "resource.h"
#include "SecID.h"
#include "CIPCPipeISDN.h"
#include "IsdnConnection.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
BYTE CIPCPipeISDN::m_byGlobalIDCounter = 0;
int CIPCPipeISDN::m_nOutgoingCall = 0;
int CIPCPipeISDN::m_nIncomingCall = 0;

BYTE CIPCPipeISDN::GetNextGlobalID()
{
	m_byGlobalIDCounter++;	// it's a byte, so overflow should be handled by the type, no need for an if
	return m_byGlobalIDCounter;
}
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CIPCPipeISDN::CIPCPipeISDN(	CIPCType t,
						    CISDNConnection* pConnection,
							LPCTSTR pSMName,
							BOOL bAsMaster,
							DWORD dwOptions,
							WORD wID,
							DWORD dwRequestCounter,
							BOOL bIsAlarm
							)
{
	m_bIsAlarm= bIsAlarm;

	m_dwRequestCounter = dwRequestCounter;
	m_cipcType = t;
	switch (m_cipcType)
	{
		case CIPC_INPUT_CLIENT:
		case CIPC_INPUT_SERVER:
			m_ID.Set(0x1000, wID);
			break;
		case CIPC_OUTPUT_CLIENT:
		case CIPC_OUTPUT_SERVER:
			m_ID.Set(0x1000, wID);
			break;
		case CIPC_DATABASE_CLIENT:
		case CIPC_DATABASE_SERVER:
			m_ID.Set(0x1000, wID);
			break;
		case CIPC_AUDIO_CLIENT:
		case CIPC_AUDIO_SERVER:
			m_ID.Set(0x1000, wID);
			break;
		default:
			WK_TRACE(_T("ERROR: CIPCPipeIST() WRONG TYPE ID %u SMName '%s'\n"), wID, pSMName);
			ASSERT(0);
			break;
	}
	
	TRACE(_T("New pipe %x type %s\n"),m_ID.GetID(), NameOfEnum(m_cipcType));
	m_pConnection	= pConnection;
	m_dwOptions		= dwOptions;
	m_remoteID		= SECID_NO_ID;
	m_dwCounter		= 0;
	m_dwStarttimeForNotConnectedTimeout	= GetTickCount();
	m_dwNotConnectedTimeout = (DWORD)-1; // infinite til set

	HandleVirtualAlarm(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
CIPCPipeISDN::~CIPCPipeISDN()
{
	HandleVirtualAlarm(FALSE);
}


/////////////////////////////////////////////////////////////////////////////
/*@MD
Called from CIPC::Run, also im CIPC thread.
*/

LPCTSTR NameOfEnum(CIPCType ct)
{
	switch (ct) 
	{
		case CIPC_NONE: return _T("CIPC_NONE"); break;
		case CIPC_INPUT_CLIENT: return _T("CIPC_INPUT_CLIENT"); break;
		case CIPC_OUTPUT_CLIENT: return _T("CIPC_OUTPUT_CLIENT"); break;
		case CIPC_DATABASE_CLIENT: return _T("CIPC_DATABASE_CLIENT"); break;
		case CIPC_ALARM_CLIENT: return _T("CIPC_ALARM_CLIENT"); break;
		case CIPC_INPUT_SERVER: return _T("CIPC_INPUT_SERVER"); break;
		case CIPC_OUTPUT_SERVER: return _T("CIPC_OUTPUT_SERVER"); break;
		case CIPC_DATABASE_SERVER: return _T("CIPC_DATABASE_SERVER"); break;
		case CIPC_ALARM_SERVER: return _T("CIPC_ALARM_SERVER"); break;
		case CIPC_CONTROL_AUDIO: return _T("CIPC_CONTROL_AUDIO"); break;
		case CIPC_AUDIO_SERVER: return _T("CIPC_AUDIO_SERVER"); break;
		case CIPC_AUDIO_CLIENT: return _T("CIPC_AUDIO_CLIENT"); break;
		default:
			return _T("CIPCTypeINVALID");

	}
}


CIPCPipeISDN *CIPCPipesArrayCS::GetPipeByID(CSecID id) const
{
	for (int i=0;i<GetSize();i++) {
		if (GetAt(i)->GetID()==id) {
			return GetAt(i);	// <<< EXIT >>>
		}
	}
	return NULL;
}


CIPC *CIPCPipeISDN::GetCIPC()	// non const!
{
	WK_TRACE_ERROR(_T("internal error in GetCIPC\n"));
	return NULL;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCPipeISDN::HandleVirtualAlarm(BOOL bConnect/* TRUE -> Aufbau, FALSE = Abbau */) 
{
	BOOL bActive = FALSE;

	switch (GetCIPCType())
	{
		case CIPC_INPUT_CLIENT:
		case CIPC_OUTPUT_CLIENT:
		case CIPC_DATABASE_CLIENT:
		case CIPC_AUDIO_CLIENT:
			if (IsAlarm())
				bActive	= FALSE;
			else 
				bActive	= TRUE;
			break;
  		case CIPC_INPUT_SERVER:
  		case CIPC_OUTPUT_SERVER:
		case CIPC_DATABASE_SERVER:
		case CIPC_AUDIO_SERVER:
			if (IsAlarm())
				bActive	= TRUE;
			else 
				bActive	= FALSE;
			break;
		default:
			WK_TRACE(_T("Unknown CIPCType\n"));
	}

	// Hostname oder -nummer fuer RTE holen
	CString sHost = m_pConnection->GetRemoteHostName();
	if (sHost.IsEmpty())
	{
		sHost = m_pConnection->GetRemoteNumber();
		if (sHost.IsEmpty())
		{
			sHost = _T("UNKNOWN");
		}
	}

	if (bConnect)
	{
		if (bActive)
		{
			if (m_nOutgoingCall == 0)
			{
				// Virtuellen Outgoing Call-Alarm auslösen...
				Sleep(50);
				CString sMsg;
				sMsg.Format(IDS_OUTGOING_CALL, sHost);
				CWK_RunTimeError runtimeError(REL_MESSAGE, RTE_OUTGOING_CALL, sMsg,0,1);	
				runtimeError.Send();
			}
			m_nOutgoingCall++;
		}
		else
		{
			if (m_nIncomingCall == 0)
			{
				// Virtuellen Incoming Call-Alarm auslösen...
				Sleep(50);
				CString sMsg;
				sMsg.Format(IDS_INCOMING_CALL, sHost);
				CWK_RunTimeError runtimeError(REL_MESSAGE, RTE_INCOMING_CALL, sMsg,0,1);	
				runtimeError.Send();
			}
			m_nIncomingCall++;
		}
	}
	else
	{
		if (bActive)
		{
			m_nOutgoingCall--;
			m_nOutgoingCall = max(m_nOutgoingCall, 0);
			if (m_nOutgoingCall == 0)
			{
				// Virtuellen Outgoing Offline-Alarm auslösen...
				Sleep(50);
				CString sMsg;
				sMsg.Format(IDS_CLOSE_OUTGOING_CALL, sHost);
				CWK_RunTimeError runtimeError(REL_MESSAGE, RTE_OUTGOING_CALL, sMsg,0,0);	
				runtimeError.Send();
			}
		}
		else
		{
			m_nIncomingCall--;
			m_nIncomingCall = max(m_nIncomingCall, 0);

			if (m_nIncomingCall == 0)
			{
				// Virtuellen Incoming Offline-Alarm auslösen...
				Sleep(50);
				CString sMsg;
				sMsg.Format(IDS_CLOSE_INCOMING_CALL, sHost);
				CWK_RunTimeError runtimeError(REL_MESSAGE, RTE_INCOMING_CALL, sMsg,0,0);	
				runtimeError.Send();
			}
		}
	}

	return TRUE;
}

