// ClientRelayProcess.cpp: implementation of the CClientRelayProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sec3.h"
#include "ClientRelayProcess.h"
#include "CIPCOutputServerClient.h"
#include "Output.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CClientRelayProcess::CClientRelayProcess(CProcessScheduler* pScheduler,CSecID idClient, const COutput &relay)
: CClientProcess(pScheduler, idClient)
{
	TRACE("CClientRelayProcess\n");
	m_type = SPT_RELAY;
	m_pRelay = & relay;
	CIPCOutputServerClient* pClient = theApp.GetOutputClients().GetClientByID(idClient);
	CProcessMacro *pMacro = new CProcessMacro(PMT_RELAY, "Relais " + pClient->GetConnection().GetSourceHost(), SECID_NO_ID);

	if (relay.GetRelayControlType()==RCT_EDGE) 
	{
		pMacro->SetOverallTime(relay.GetRelayClosingTime()*1000);
	} 
	else 
	{
		pMacro->SetOverallTime(CPM_TIME_INFINITY);
	}

	pMacro->SetRecordValues(1000,1);	// OOPS
	pMacro->SetIsIntervall( TRUE );	// OOPS okay ?

	m_pActivation = new CActivation(pMacro, this);
	m_pActivation->SetOutputID(relay.GetID());
	m_pActivation->SetPriority( pClient->GetPriority() ); 
}

CClientRelayProcess::~CClientRelayProcess()
{
	TRACE("~CClientRelayProcess\n");
}
