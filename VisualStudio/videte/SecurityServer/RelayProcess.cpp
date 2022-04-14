// RelayProcess.cpp: implementation of the CRelayProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sec3.h"
#include "OutputList.h"
#include "RelayProcess.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRelayProcess::CRelayProcess(CProcessScheduler* pScheduler,CActivation *pActivation)
:CProcess(pScheduler)
{
	// CAVEAT, can't create a new activation here
	// since there is a pointer link in the input
	m_type = SPT_RELAY;
	m_pRelay = theApp.GetOutputGroups().GetOutputByID(pActivation->GetOutputID());
	if (m_pRelay==NULL) 
	{
		WK_TRACE_ERROR(_T("Relay %x not found\n"),pActivation->GetOutputID().GetID());
	}

	m_pActivation = pActivation;
	pActivation->SetActiveProcess(this);

	InitTimer();
}

CRelayProcess::~CRelayProcess()
{

}
