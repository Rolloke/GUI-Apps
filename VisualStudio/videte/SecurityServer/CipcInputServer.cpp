/* GlobalReplace: GetTickCount --> WK_GetTickCount */
/* GlobalReplace: pApp\-\> --> theApp. */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CipcInputServer.cpp $
// ARCHIVE:		$Archive: /Project/SecurityServer/CipcInputServer.cpp $
// CHECKIN:		$Date: 30.08.06 13:02 $
// VERSION:		$Revision: 139 $
// LAST CHANGE:	$Modtime: 30.08.06 12:49 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"
#include "sec3.h"
#include "input.h"
#include "CipcInputServer.h"
#include "Processmacro.h"
#include "ProcessScheduler.h"


#include "CipcServerControlServerSide.h"
#include "CipcServerControlClientSide.h"
#include "CipcInputServerClient.h"

#include "OutputList.h"

/*@TOPIC{CIPCInputServer Overview|CIPCInputServer,Overview}
@RELATED @LINK{members|CIPCInputServer},
*/
/*
@MLIST @RELATED @LINK{CIPCInputServerClient}
*/

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
/////////////////////////////////////////////////////////////////////////////
// CIPCInputServer
/////////////////////////////////////////////////////////////////////////////
/*@MHEAD{constructor:}*/
/*@MD NYD*/
CIPCInputServer::CIPCInputServer(LPCTSTR pName, int iSize, LPCTSTR pSMName)
	: CIPCInput(pSMName, TRUE)
	, CIOGroup(pName)

{
	m_bInitDone = FALSE;	// set to TRUE after the inital CIPC com.
	m_dwFreeState	= 0;	// Alle Eingänge sind default frei.
	m_dwConnectTime=0;

	CString s(GetShmName());
	m_bIsMD = (    0 == s.Find(SM_MICO_MD_INPUT) 
				|| 0 == s.Find(SM_SAVIC_MD_INPUT)
				|| 0 == s.Find(SM_TASHA_MD_INPUT))
				|| 0 == s.Find(SM_Q_MD_INPUT)
				|| 0 == s.Find(SM_IP_CAMERA_MD_INPUT)
				;
	m_bIsSDI = 0 == s.Find(SM_SDIUNIT_INPUT);


	m_bTasha = (0 == s.Find(_T("Tasha")));
	m_bSavic = (0 == s.Find(_T("SaVic")));
	m_bQ = (0 == s.Find(_T("QUnit")));
	m_bAudio = (0 == s.Find(_T("Audio")));
	m_bIP = (0 == s.Find(_T("IPCamera")));
	TRACE(_T("CIPCInputServer::CIPCInputServer %s\n"),s);
}

/*@MHEAD{destructor:}*/
/*@MD NYD*/
CIPCInputServer::~CIPCInputServer()
{
 	StopThread();
	DeleteAllInputs();
}
/////////////////////////////////////////////////////////////////////////////
void CIPCInputServer::StartInitialProcesses()
{
	for (int i=0;i<GetSize();i++) 
	{
		GetInputAtWritable(i)->StartInitialProcesses();
	}
}

/*@MHEAD{timeouts:}*/
/*@MD NYD*/
BOOL CIPCInputServer::OnTimeoutWrite(DWORD dwCmd)
{
	// NOT YET make use of dwCmd
#if 1
	if (GetIPCState()!=CIPC_STATE_CONNECTED) 
	{
		// Sleep(1000);	// wait a second and try to reconnect
		// OOPS DoRequestConnection();
		// avoid recursion, let it run into OnTimeoutCmdReceive
		return FALSE;
	} 
	else 
	{
		if (GetNumTimeoutsCmdReceive()*GetTimeoutCmdReceive()>15000) 
		{
			WK_TRACE(_T("%s:WriteTimeout %d times, giving up, trying to reconnect...\n"),GetShmName(),GetNumTimeoutsCmdReceive());
			m_iHardware = -1;
			for (int i=0;i<GetSize();i++) 
			{
				CInput *pInput=GetInputAtWritable(i);
				StopProcesses(pInput,FALSE);	// even keepAlive
			}
		} 
		else 
		{
			DoRequestAlarmState(m_wGroupID);
		}
	}
#endif

	return TRUE;	// retry
}

/*@MD NYD*/
BOOL CIPCInputServer::OnTimeoutCmdReceive()
{
	/*
	WK_TRACE(_T("%s has a timeout for CmdReceive num %d timeut is %d\n"),GetShmName(),
		GetNumTimeoutsCmdReceive(),
		GetTimeoutCmdReceive()
		);
	*/
	if (GetIPCState()!=CIPC_STATE_CONNECTED) {
		// CIPC will try to (re)connect
	} else {
		if (GetNumTimeoutsCmdReceive()*GetTimeoutCmdReceive()>5*60*1000) {
			DoRequestAlarmState(m_wGroupID);
		}
	}
	return TRUE;
}

/*@MHEAD{processes:}*/
/*@MD NYD*/
void CIPCInputServer::StartMissingProcesses(WORD wOutputGroupID, const CSecTimer *pTimer)
{
	if (IsInitDone()) 
	{
		for (int i=0;i<GetSize();i++) 
		{
			CInput *pInput=GetInputAtWritable(i);
			pInput->StartMissingProcesses(wOutputGroupID, pTimer);
		}
	} 
}
// send a stop to ALL output-units/process schedulers
/*@MD NYD*/
void CIPCInputServer::StopProcesses(CInput* pInput, BOOL bNewState)
{
	for (int i=0;i<theApp.GetOutputGroups().GetSize();i++) 
	{
		CIPCOutputServer *pOutputGroup= theApp.GetOutputGroups().GetGroupAt(i);
		if (pOutputGroup) 
		{
			pOutputGroup->GetProcessScheduler()->KillProcesses(pInput);
		}
	}		
}

// OnConfirmXXX, virtual!
/*@MHEAD{connection:}*/
/*@MLIST The initial connection is made in the following sequence:*/
/*@MD NYD*/
void CIPCInputServer::OnReadChannelFound()
{
	DoRequestConnection();
	CString sMsg;
	sMsg.Format(_T("InputUnit|%s"),GetShmName());
	WK_STAT_LOG(_T("Units"),1,sMsg);
}
/*@MD NYD*/
void CIPCInputServer::OnConfirmConnection()
{
	CIPC::OnConfirmConnection();
	DoRequestVersionInfo(m_wGroupID);
	DoRequestSetGroupID(m_wGroupID);
	TRACE(_T("### %s DoRequestSetGroupID %d\n"),GetShmName(),m_wGroupID);
	theApp.UpdateStateInfo();
}

/*@MDT{optional} NYD*/
void CIPCInputServer::OnConfirmVersionInfo(WORD wGroupID, DWORD dwVersion)
{
	m_dwVersion = dwVersion;
	TRACE(_T("%s has version %d\n"),GetShmName(),dwVersion);
}

void CIPCInputServer::OnConfirmSetGroupID(WORD wGroupID)
{
	TRACE(_T("%s OnConfirmSetGroupID %d\n"),GetShmName(),m_wGroupID);
	DoRequestHardware(m_wGroupID);
}
/*@MD NYD*/
void CIPCInputServer::OnConfirmHardware(WORD wGroupID,int iErrorCode)
{
	if (iErrorCode==0) 
	{	// OOPS ignore old state?
		DWORD dwMask = 0L;
		for (int i=0;i<GetSize();i++)
		{
			const CInput* pInput = GetInputAt(i);
			if (pInput && ((int)pInput->wState & I_STATE_EDGE)) 
			{
				setbit(dwMask, i, 1); 
			}
		}
		DoRequestSetEdge(wGroupID, dwMask);
	}
	m_iHardware = iErrorCode;

	// inform clients
	const CIPCInputClientsArray & clients= theApp.GetInputClients();
	for (int ci=0;ci<clients.GetSize();ci++) {
		clients[ci]->DoConfirmHardware(wGroupID,iErrorCode);
	}

}
	
/*@MD NYD*/
void CIPCInputServer::OnConfirmEdge(WORD wGroupID,DWORD edgeBitmask)	// 1 positive, 0 negative
{
	DoRequestSetFree(m_wGroupID, m_dwFree);
}
/*@MD NYD*/
void CIPCInputServer::OnConfirmFree(WORD wGroupID,DWORD openBitmask)	// 1 open, 0 closed
{
	DoRequestReset(m_wGroupID);
}
/*@MD NYD*/
void CIPCInputServer::OnConfirmReset(WORD wGroupID)
{
	m_dwConnectTime = WK_GetTickCount();
	SetTimeoutCmdReceive(100);	// avoid the default 9876 timeout, see ::Run

	m_dwState=0; 
	for (int i=0;i<GetSize();i++) 
	{
		if (   GetState(i)==GMS_ACTIVE
			|| GetState(i)==GMS_INACTIVE) 
		{
			GetInputAtWritable(i)->SetActive(FALSE, FALSE);	// do not start processes now
		}
	}
	// the initial RequestAlarmState is made in ::Run
}


/*@MHEAD{disconnect}*/
/*@MD NYD*/
void CIPCInputServer::OnRequestDisconnect()
{
	if (m_bShutdownInProgress) {
		return;	// EXIT!
	}
	CString sMsg;
	sMsg.Format(_T("InputUnit|%s"),GetShmName());
	WK_STAT_LOG(_T("Units"),0,sMsg);
	m_iHardware = -1;
	m_dwConnectTime=0;
	m_bInitDone = FALSE;
	for (int i=0;i<GetSize();i++) 
	{
		CInput *pInput=GetInputAtWritable(i);
		StopProcesses(pInput,FALSE);	// even keepAlive, fake OK, OOPS
	}
	
	// inform clients
	const CIPCInputClientsArray & clients= theApp.GetInputClients();
	for (int ci=0;ci<clients.GetSize();ci++) {
		clients[ci]->DoConfirmHardware(m_wGroupID,-1);
	}
	theApp.UpdateStateInfo();
}


/*@MD NYD*/
void CIPCInputServer::OnConfirmAlarmState(WORD wGroupID, DWORD inputMask)	// 1 high, 0 low; 1 changed, 0 unchanged
{
	if (m_bShutdownInProgress) 
	{
		return;	// EXIT!
	}

	if (m_bInitDone==FALSE) 
	{
		CString sMsg;
		sMsg = CString(_T("InputUnit|")) + GetShmName();
		WK_STAT_LOG(_T("Units"),-1,sMsg);	// peak to show delay
		m_bInitDone=TRUE;	// CAVEAT has to be BEFORE SetActive
		m_dwState = inputMask;

		WK_TRACE(_T("Initial input states %s %08lx\n"),GetShmName(),m_dwState);
		// reset all inputs
		DWORD dwOneBit=1;
		
		for (WORD i=0;i<GetSize();i++, dwOneBit <<= 1) 
		{
			if (   GetState(i) == GMS_ACTIVE
				|| GetState(i) == GMS_INACTIVE) 
			{
				CInput* pInput = GetInputAtWritable(i);
				BOOL bActive = (m_dwState & dwOneBit)!=0;
				WK_TRACE(_T("initial input state used %s %d\n"),GetInputAt(i)->GetName(),bActive);
				pInput->SetActive(bActive, FALSE);	// set and do not start
				if (theApp.IsUpAndRunning())
				{
					pInput->StartInitialProcesses();
				}
				// NOT YET NeedsConf....
			}
			else
			{
//				TRACE(_T("input state not used %s %s\n"),GetInputAt(i)->GetName(),NameOfEnum(GetState(i)));
			}
		}

		// inform clients, client is also infored via OnIndicateAlarmState
		// BUT NOT during the reset
		const CIPCInputClientsArray & clients= theApp.GetInputClients();
		for (int ci=0;ci<clients.GetSize();ci++) 
		{
			clients[ci]->DoConfirmAlarmState(wGroupID,inputMask);
		}
	}	// end of init NOT already done
}
/////////////////////////////////////////////////////////////////////////////
void CIPCInputServer::StartProcesses(WORD wGroupID,
						DWORD inputMask, 	// 1 high, 0 low
						DWORD changeMask,	// 1 changed, 0 unchanged
						const CIPCFields& fields
						)
{
	// calc changes from own data and cmp with received changes
	DWORD myChangeMask = m_dwState ^ inputMask;
	if (myChangeMask!=changeMask) 
	{
		// OOPS %x vs %s
		/*
		WK_TRACE(_T("%s: different change masks my %s != unit %s\n"),
						GetShmName(),
						BinaryString(myChangeMask, GetSize()),
						BinaryString(changeMask, GetSize())
						);
		WK_TRACE(_T("%s: different change masks state %s, inputmask %s\n"),
						GetShmName(),
						BinaryString(m_dwState, GetSize()),
						BinaryString(inputMask, GetSize())
						);*/
		// CAVEAT uses OWN mask instead of received mask
		changeMask = myChangeMask;	
	}
	m_dwState = inputMask;	// Bitmask-Status der Eingänge

	DWORD dwOneBit=1;
	for (WORD i=0;i<GetSize();i++, dwOneBit <<= 1) 
	{
		if (changeMask & dwOneBit)	
		{
			// Bitmask-Status veränderte Eingänge
			if (GetState(i)==GMS_OFF) 
			{
				continue;
			}

			CInput *pInput=GetInputAtWritable(i);
			BOOL bActive= (m_dwState & dwOneBit)!=0;

			// update infostring OOPS check condition ?
			if (bActive) 
			{
				pInput->SetFields(fields);
			}
			else 
			{
				// do not modify in OK state
			}
			// inform clients NOT YET
			// set the new state
			pInput->SetActive( bActive, TRUE );	// set and start processes
		}	//	end of bit changed
	}	// end of loop over changeMask
}
/*@MHEAD{alarm:}*/
/*@MD NYD
@PAR Set the infostring only for ALARM indications.
@ALSO @LINK{AlarmInfoString} @MLINK{CIPCInputServer::OnIndicateAlarmInfo}
*/
// inputMask 1 high, 0 low; changeMask 1 changed, 0 unchanged
void CIPCInputServer::OnIndicateAlarmState(WORD wGroupID,DWORD inputMask, 
										   DWORD changeMask,
										   LPCTSTR szInfoString)
{
	if (m_bShutdownInProgress || !m_bInitDone) 
	{
		return;	// EXIT!
	}

	if (!IsMD())
	{
		if (theApp.GetServerControl())
			theApp.GetServerControl()->IndicateAlarmToClients(wGroupID,inputMask,changeMask);
	}

	CIPCFields fields;

	if (szInfoString)
	{
		fields.FromString(szInfoString);
	}

	StartProcesses(wGroupID,inputMask,changeMask,fields);
}
///////////////////////////////////////////////////////////////////////
void CIPCInputServer::OnIndicateAlarmFieldsState(WORD wGroupID,
									  DWORD inputMask, 	// 1 high, 0 low
									  DWORD changeMask,	// 1 changed, 0 unchanged
									  int iNumFields,
									  const CIPCField fields[]
									  )
{
	if (m_bShutdownInProgress || !m_bInitDone) 
	{
		return;	// EXIT!
	}

//	TRACE(_T("OnIndicateAlarmFieldsState inputMask=%04hx changeMask=%04hx\n"),(WORD)inputMask,(WORD)changeMask);

	if (!IsMD())
	{
		if (theApp.GetServerControl())
			theApp.GetServerControl()->IndicateAlarmToClients(wGroupID,inputMask,changeMask);
	}

	CIPCFields fs;

	if (iNumFields>0)
	{
		fs.FromArray(iNumFields,fields);
	}

	StartProcesses(wGroupID,inputMask,changeMask,fs);
}
///////////////////////////////////////////////////////////////////////
void CIPCInputServer::OnIndicateAlarmNr(CSecID id,
								   BOOL bAlarm,
								   DWORD dwData1,
								   DWORD dwData2)
{
	if (m_bShutdownInProgress || !m_bInitDone) 
	{
		return;	// EXIT!
	}

	int i = id.GetSubID();

	if (GetState(i) == GMS_OFF) 
	{
		return;	// EXIT!
	}

	CInput *pInput = GetInputAtWritable(i);

	if (pInput)
	{
		// aktuelle Bitmaske der Stati aktualisieren
		DWORD dwBit = 1 << pInput->GetID().GetSubID();

		if (bAlarm)
		{
			m_dwState |= dwBit;
		}
		else
		{
            m_dwState &= ~dwBit;
		}

		if (!IsMD())
		{
			if (theApp.GetServerControl())
				theApp.GetServerControl()->IndicateAlarmToClients(GetGroupID(),m_dwState,dwBit);
		}

		pInput->SetActive( bAlarm, TRUE );	// set and start processes

		if (bAlarm)
		{
			// im Alarmfall Motion Detection Koordinaten merken
			CIPCFields fields;
			CString sx,sy;
			sx.Format(_T("%04x"),LOWORD(dwData1));
			sy.Format(_T("%04x"),HIWORD(dwData1));

			CIPCField* pfx = new CIPCField(_T("DBD_MD_X"),sx,_T('C'));
			CIPCField* pfy = new CIPCField(_T("DBD_MD_Y"),sy,_T('C'));

			fields.SafeAdd(pfx);
			fields.SafeAdd(pfy);

			pInput->AddFields(fields);
		}
	}
}
///////////////////////////////////////////////////////////////////////
void CIPCInputServer::OnIndicateAlarm(CSecID id, 
									 BOOL bAlarm,
									 int iNumRecords,
									 const CIPCField fields[])
{
	if (m_bShutdownInProgress || !m_bInitDone) 
	{
		return;	// EXIT!
	}

	int i = id.GetSubID();

	if (GetState(i) == GMS_OFF) 
	{
		return;	// EXIT!
	}

	CInput *pInput = GetInputAtWritable(i);

	if (pInput)
	{
		if (i<32)
		{
			// aktuelle Bitmaske der Stati aktualisieren nur die 1ten 32 Alarme
			DWORD dwBit = 1 << pInput->GetID().GetSubID();

			if (bAlarm)
			{
				m_dwState |= dwBit;
			}
			else
			{
				m_dwState &= ~dwBit;
			}
			if (!IsMD())
			{
				if (theApp.GetServerControl())
				{
					theApp.GetServerControl()->IndicateAlarmToClients(GetGroupID(),m_dwState,dwBit);
				}
			}
		}
		else
		{
			if (!IsMD())
			{
				if (theApp.GetServerControl())
				{
					theApp.GetServerControl()->IndicateAlarmToClients(id,bAlarm,0,0);
				}
			}
		}


		pInput->SetActive( bAlarm, TRUE );	// set and start processes

		if (bAlarm)
		{
			// im Alarmfall Motion Detection Koordinaten merken
			CIPCFields fieldsCopy;
			fieldsCopy.FromArray(iNumRecords,fields);
			pInput->SetFields(fieldsCopy);
		}
	}
}
///////////////////////////////////////////////////////////////////////
void CIPCInputServer::OnIndicateAlarmFieldsUpdate(CSecID id,
									   int iNumFields,
									   const CIPCField fields[])
{
	CInput *pInput = GetInputAtWritable(id.GetSubID());

//	TRACE(_T("OnIndicateAlarmFieldsUpdate %d\n"),id.GetSubID());
	if (   pInput
		&& pInput->IsActive()) 
	{
		CIPCFields fs;
		fs.FromArray(iNumFields,fields);
		pInput->AddFields(fs);
		pInput->RestartAlarmProcesses();
	}
}

/*@MHEAD{misc:}*/
/*@MD NYD*/
void CIPCInputServer::OnIndicateError(DWORD dwCmd, 
									 CSecID id, 
									 CIPCError error, 
									 int iErrorCode,
									 const CString &sErrorMessage)
{
	if (dwCmd == CIPC_BASE_CONFIRM_ALIVE)
	{
		WK_TRACE(_T("resetting by unconfirmed alive (CIPCInputServer %s)\n") ,GetShmName());
		theApp.EmergencyReset();
	}
	else if (error == CIPC_ERROR_UNHANDLED_CMD)
	{
		WK_TRACE(_T("UNIT %s:unhandled cmd %s\n"),GetShmName(),NameOfCmd(dwCmd));
	}
}

/*@MD NYD*/
BOOL CIPCInputServer::Run(DWORD dwTimeout)
{
	// connected not initialized, wait the delay 
	if (m_bInitDone==FALSE && m_dwConnectTime!=0
		&& GetIPCState()==CIPC_STATE_CONNECTED) {
		int iDelta = (int)(WK_GetTickCount()-m_dwConnectTime);
		if ( iDelta > 1000 ) {
			m_dwConnectTime=WK_GetTickCount();
			DoRequestAlarmState(GetGroupID());
			SetTimeoutCmdReceive(9876);	// back to default
		}
	}

	// CAVEAT this call 'sleeps' GetTimeoutCmdReceive() ms
	// if there are no commands exchanged
	return CIPC::Run(dwTimeout);
}

/*@MD NYD*/
void CIPCInputServer::OnConfirmAlarmReset(WORD wGroupID,DWORD resetMask)	// 1 reset done 
{
	// NOT YET
}


/*@------------------MHEAD{comm data:}*/
/*@MD NYD*/
void CIPCInputServer::OnIndicateCommData(CSecID id,
									const CIPCExtraMemory &data,
									DWORD dwBlockNr)
{
	if (theApp.GetServerControl()==NULL)
	{
		return;	// EXIT
	}


	CClientLinkArray & usage = theApp.GetServerControl()->m_usageCommData;
	usage.Lock(_T(__FUNCTION__));
	for (int i=0;i<usage.GetSize();i++)
	{
			usage[i]->m_pInputClient->DoIndicateCommData(id, data, dwBlockNr);
	}
	usage.Unlock();
}

/*@MD NYD*/
void CIPCInputServer::OnConfirmWriteCommData(CSecID id, DWORD dwBlockNr)
{
	if (theApp.GetServerControl()==NULL)
	{
		return;	// EXIT
	}

	CClientLinkArray & usage = theApp.GetServerControl()->m_usageCommData;
	usage.Lock(_T(__FUNCTION__));
	for (int i=0;i<usage.GetSize();i++)
	{
		usage[i]->m_pInputClient->DoConfirmWriteCommData(id, dwBlockNr);
	}
	usage.Unlock();
}


/*@MD NYD */
void CIPCInputServer::OnConfirmGetValue(
						CSecID id, // might be used as group ID only
						const CString &sKey,
						const CString &sValue,
						DWORD dwServerData
						)
{
	if (dwServerData!=0)
	{
		CSecID clientID(dwServerData);
		CIPCInputClientsArray &clients=theApp.GetInputClients();
		clients.Lock(_T(__FUNCTION__));
		CIPCInputServerClient *pClient=NULL;
		pClient=clients.GetClientByID(clientID);
		if (pClient) 
		{
			DWORD dwid = 0;
			CString sValueCopy(sValue);
			if (1 == _stscanf(sKey, _T("GetControlFunctions(%08x)"), &dwid))
			{
				// funktionen scannen
				DWORD dwControlFunctions = 0;
				_stscanf(sValue, _T("%08lx"), &dwControlFunctions);

				// funktionen merken
				BOOL bPTZSetPosPossible = (dwControlFunctions & PTZF_SET_POS)>0;
				BOOL bPTZConfigPossible = (dwControlFunctions & PTZF_CONFIG)>0;

				// erstmal alles verbieten
				dwControlFunctions &= ~PTZF_SET_POS;
				dwControlFunctions &= ~PTZF_CONFIG;
				
				// und dann sehen, was erlaubt ist.
				CPermission* pPermission = theApp.GetPermissions().GetPermission(pClient->GetPermissionID());
				if (   pPermission
					&& ((pPermission->GetFlags() & WK_ALLOW_PTZ_CONFIG)>0)
					)
				{
					if (bPTZSetPosPossible)
					{
						dwControlFunctions |= PTZF_SET_POS;
					}
					if (bPTZConfigPossible)
					{
						dwControlFunctions |= PTZF_CONFIG;
					}
				}
				sValueCopy.Format(_T("%08x"),dwControlFunctions);
			}
			pClient->DoConfirmGetValue(id,sKey,sValueCopy,0);
		} 
		else 
		{
			// client already disappeared
			WK_TRACE_ERROR(_T("OnConfirmGetValue(%s,%s) client %x not found\n"),
				sKey,sValue,clientID.GetID());
		}
		clients.Unlock();
	}
}

/*@MD NYD */
void CIPCInputServer::OnConfirmSetValue(
						CSecID id, // might be used as group ID only
						const CString &sKey,
						const CString &sValue,
						DWORD dwServerData
						)
{
	if (dwServerData!=0)
	{
		CSecID clientID(dwServerData);
		CIPCInputClientsArray &clients=theApp.GetInputClients();
		clients.Lock(_T(__FUNCTION__));
		CIPCInputServerClient *pClient=NULL;
		pClient=clients.GetClientByID(clientID);
		if (pClient) 
		{
			pClient->DoConfirmSetValue(id,sKey,sValue,0);
		} 
		else 
		{
			// client already disappeared
			WK_TRACE_ERROR(_T("OnConfirmSetValue(%s,%s) client %x not found\n"),
				sKey,sValue,clientID.GetID());
		}
		clients.Unlock();
	}
}







