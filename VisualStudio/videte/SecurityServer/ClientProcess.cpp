// ClientProcess.cpp: implementation of the CClientProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sec3.h"
#include "ClientProcess.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CClientProcess::CClientProcess(CProcessScheduler* pScheduler,CSecID idClient)
:CProcess(pScheduler)
{
	m_idClient = idClient;
}
//////////////////////////////////////////////////////////////////////
CClientProcess::~CClientProcess()
{
	if (m_pActivation)
	{
		m_pActivation->SetActiveProcess(NULL);
		WK_DELETE(m_pActivation);
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CClientProcess::Terminate(BOOL bShutDown)
{
	CIPCOutputServerClient* pClient = theApp.GetOutputClients().GetClientByID(m_idClient);
	if (pClient) 
	{
		pClient->RemoveProcess(this);
		// delete temporary created activation (and the included macro)
		m_pActivation->SetActiveProcess(NULL);
		WK_DELETE(m_pActivation);
	}
	BOOL bRet = CProcess::Terminate(bShutDown);
	m_idClient = SECID_NO_ID;
	return bRet;
}
//////////////////////////////////////////////////////////////////////
CSecID CClientProcess::GetClientID() const
{
	return m_idClient;
}
//////////////////////////////////////////////////////////////////////
CString CClientProcess::GetStateName() const
{
	return "CL ";
}
//////////////////////////////////////////////////////////////////////
void CClientProcess::ClearClientID()
{
	m_idClient = SECID_NO_ID;
}
//////////////////////////////////////////////////////////////////////
CString CClientProcess::Format()
{
	CString sRet;

	sRet.Format(_T("%s CLID=%08lx"),CProcess::Format(),m_idClient.GetID());

	return sRet;
}

