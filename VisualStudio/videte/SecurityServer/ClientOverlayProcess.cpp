// ClientOverlayProcess.cpp: implementation of the CClientOverlayProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sec3.h"
#include "ClientOverlayProcess.h"

#include "CIPCOutputServerClient.h"
#include "ProcessListView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CClientOverlayProcess::CClientOverlayProcess(CProcessScheduler* pScheduler,
											 CSecID idClient,
											 CSecID camID,
											 const CRect &rect)
 : CClientProcess(pScheduler,idClient)
{
	CIPCOutputServerClient* pClient = theApp.GetOutputClients().GetClientByID(idClient);
	CProcessMacro *pMacro = new CProcessMacro(PMT_RECORD,"Relais " + pClient->GetConnection().GetSourceHost(),SECID_NO_ID);

	pMacro->SetOverallTime(CPM_TIME_INFINITY);
	pMacro->SetRecordValues(1000,1);	// OOPS
	
	pMacro->SetIsIntervall( TRUE );	// OOPS okay ?
	m_rectOverlay = rect;

	m_pActivation = new CActivation(pMacro, this);
	m_pActivation->SetOutputID(camID);
	m_pActivation->SetPriority(pClient->GetPriority()); 
}
/////////////////////////////////////////////////////////////////////
CClientOverlayProcess::~CClientOverlayProcess()
{

}
/////////////////////////////////////////////////////////////////////
void CClientOverlayProcess::Modify(CSecID camID, const CRect &rect)
{
	UpdateProcessListView(FALSE,this);

	Lock(_T(__FUNCTION__));
	m_pActivation->SetOutputID(camID);
	m_rectOverlay = rect;
	Unlock();

	UpdateProcessListView(TRUE,this);
}
