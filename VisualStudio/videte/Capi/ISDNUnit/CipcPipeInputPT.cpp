/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CipcPipeInputPT.cpp $
// ARCHIVE:		$Archive: /Project/Capi/ISDNUnit/CipcPipeInputPT.cpp $
// CHECKIN:		$Date: 27.07.05 10:29 $
// VERSION:		$Revision: 13 $
// LAST CHANGE:	$Modtime: 27.07.05 10:27 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"
#include "SecID.h"
#include "CIPCPipeInputPT.h"

#include "IsdnConnection.h"
#include "PTBox.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


CIPCPipeInputPT::CIPCPipeInputPT(	CIPCType t,
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
			)  , CIPCInput (pSMName,bAsMaster)
{

}

CIPCPipeInputPT::~CIPCPipeInputPT()
{
	StopThread();
}


/////////////////////////////////////////////////////////////////////////////
void CIPCPipeInputPT::OnReadChannelFound()
{
	TRACE(_T("ReadChannelFound '%s'\n"), GetShmName());	// OBSOLETE
}

/////////////////////////////////////////////////////////////////////////////
void CIPCPipeInputPT::OnRequestDisconnect()
{
	DelayedDelete();
	m_pConnection->RemoveMe(this);
}

CIPC *CIPCPipeInputPT::GetCIPC()
{
	return this;
}

void CIPCPipeInputPT::OnRequestInfoInputs(WORD wGroupID)
{
	WK_TRACE(_T("PTInputPipe:OnRequestInfoInputs\n"));
	int iRetry=100;
	while (iRetry && m_pConnection->GetPTBox()->GetPTConnectionState()!=PCS_CONNECTED) {
		WK_TRACE(_T("Input waiting for PCS_CONNECTED\n"));
		Sleep(50);
		iRetry--;
	}
	if (m_pConnection->GetPTBox()->GetPTConnectionState()!=PCS_CONNECTED) {
		WK_TRACE_ERROR(_T("RequestInfoInputs: Not connected ?\n"));
	}


	const CPTAlarms & alarms = m_pConnection->GetPTBox()->GetAlarms();
	CIPCInputRecord records[4];	// OOPS max size
	int c = 0;
	for (int a=1;a<alarms.GetSize();a++) 
	{
		BYTE byFlags = 	IREC_STATE_OKAY|IREC_STATE_ENABLED;
		if (alarms[a]->IsActive()) 
		{
			byFlags |= IREC_STATE_ALARM;
		}
		// NOT YET edge, needed ?
		records[c].Set(
				alarms[a]->GetName(),
				alarms[a]->GetID(),
				byFlags
				);
		c++;
	}
	DoConfirmInfoInputs(wGroupID, 1, c,	records);
}
//////////////////////////////////////////////////////////////////////////////
void CIPCPipeInputPT::OnRequestBitrate()
{
	// PT Connections always use one B Channel
	DoConfirmBitrate(64*1024);
}
//////////////////////////////////////////////////////////////////////////////
void CIPCPipeInputPT::OnRequestGetSystemTime()
{
	WK_TRACE(_T("get system time requested\n"));
}
//////////////////////////////////////////////////////////////////////////////
void CIPCPipeInputPT::OnRequestSetSystemTime(const CSystemTime& st)
{
	WK_TRACE(_T("set system time requested %s\n"),st.GetDateTime());
	if (m_pConnection->GetPTBox()->GetPTConnectionState()==PCS_CONNECTED) 
	{
		m_pConnection->GetPTBox()->DoSetTime(st);
	}
}
//////////////////////////////////////////////////////////////////////////////
void CIPCPipeInputPT::OnConfirmAlarm(CSecID id)
{
	//
	WK_TRACE(_T("alarm %d confirmed\n"),id.GetSubID());
	if (m_pConnection->GetPTBox()->GetPTConnectionState()==PCS_CONNECTED) 
	{
		m_pConnection->GetPTBox()->DoConfirmAlarm(id);
	}
}
