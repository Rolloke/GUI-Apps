// IPCServerToVision.cpp: implementation of the CIPCServerToVision class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IPCServerToVision.h"
#include "GuardTourProcess.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CIPCServerToVision::CIPCServerToVision(LPCTSTR szShmName, CSecID idHost)
	: CIPC(szShmName, TRUE)
{
	m_idHost = idHost;
	m_dwTick = WK_GetTickCount();
	m_bFetching = FALSE;
	StartThread();
}
//////////////////////////////////////////////////////////////////////
CIPCServerToVision::~CIPCServerToVision()
{
	StopThread();
}
//////////////////////////////////////////////////////////////////////
BOOL CIPCServerToVision::Run(DWORD dwTimeOut)
{
	DWORD dwTick = WK_GetTimeSpan(m_dwTick);
	if (   GetIPCState() == CIPC_STATE_WRITE_CREATED
		&& dwTick>30*1000
		&& !m_bFetching)
	{
		WK_TRACE(_T("disconnecting timed out control %08lx\n"),this);
		DoDisconnect();
	}

	return CIPC::Run(dwTimeOut);
}
//////////////////////////////////////////////////////////////////////
void CIPCServerToVision::RemoveProcesses()
{
	TRACE("CIPCServerToVision::RemoveProcesses %08lx\n",m_idHost.GetID());
	m_GuardTourProcesses.Lock(_T(__FUNCTION__));
	for (int i=0;i<m_GuardTourProcesses.GetSize();i++) 
	{
		CProcess* pProcess = m_GuardTourProcesses.GetAt(i);
		if (   pProcess
			&& pProcess->IsGuardTourProcess())
		{
			CGuardTourProcess* pGTProcess = (CGuardTourProcess*)pProcess;
			pGTProcess->ControlDisconnected(this);
		}
	}
	m_GuardTourProcesses.RemoveAll();

	m_GuardTourProcesses.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CIPCServerToVision::OnRequestDisconnect()
{
	TRACE(_T("CIPCServerToVision::OnRequestDisconnect %08lx\n"),m_idHost.GetID());
	RemoveProcesses();
	DelayedDelete();
}
//////////////////////////////////////////////////////////////////////
void CIPCServerToVision::OnIndicateError(DWORD dwCmd, 
										 CSecID id, 
										 CIPCError error, 
										 int iErrorCode,
										 const CString &sErrorMessage)
{

}
//////////////////////////////////////////////////////////////////////
void CIPCServerToVision::OnRequestGetValue(CSecID id,
								   const CString &sKey,
								   DWORD dwServerData)
{
}
//////////////////////////////////////////////////////////////////////
void CIPCServerToVision::OnConfirmGetValue(CSecID id,
									const CString &sKey,
									const CString &sValue,
									DWORD dwServerData)
{
}
//////////////////////////////////////////////////////////////////////
void CIPCServerToVision::OnConfirmSetValue(CSecID id,
									const CString &sKey,
									const CString &sValue,
									DWORD dwServerData)
{
	if (   id == SECID_NO_ID
		&& 0 == sKey.CompareNoCase(_T("cam")))
	{
		// TRACE(_T("Guard Tour switched camera %s-%s\n"),m_sDestination,sValue);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCServerToVision::OnRequestSetValue(CSecID id,
									const CString &sKey,
									const CString &sValue,
									DWORD dwServerData)
{
}
//////////////////////////////////////////////////////////////////////
void CIPCServerToVision::AddProcess(CGuardTourProcess* pProcess)
{
	m_GuardTourProcesses.Lock(_T(__FUNCTION__));
	BOOL bFound = FALSE;
	for (int i=0;i<m_GuardTourProcesses.GetSize() && !bFound;i++)
	{
		bFound = m_GuardTourProcesses.GetAt(i) == pProcess;
	}
	if (!bFound)
	{
		// tick fuer timeout neu setzen
		m_dwTick = WK_GetTickCount();
		m_GuardTourProcesses.Add(pProcess);
	}
	m_GuardTourProcesses.Unlock();
}
//////////////////////////////////////////////////////////////////////
BOOL CIPCServerToVision::RemoveProcess(CGuardTourProcess* pProcess)
{
	m_GuardTourProcesses.Lock(_T(__FUNCTION__));
	BOOL bFound = FALSE;
	for (int i=0;i<m_GuardTourProcesses.GetSize();i++)
	{
		if (m_GuardTourProcesses.GetAt(i) == pProcess)
		{
			TRACE(_T("%s removing GT %s\n"),GetShmName(),pProcess->GetMacro().GetName());
			m_GuardTourProcesses.RemoveAt(i);
			bFound = TRUE;
			break;
		}
	}
	m_GuardTourProcesses.Unlock();
	return bFound;
}
//////////////////////////////////////////////////////////////////////
int CIPCServerToVision::GetNrOfProcesses()
{
	return m_GuardTourProcesses.GetSize();
}
//////////////////////////////////////////////////////////////////////
CSecID CIPCServerToVision::GetDestination()
{
	return m_idHost;
}
//////////////////////////////////////////////////////////////////////
BOOL CIPCServerToVision::IsFetching()
{
	return m_bFetching;
}
//////////////////////////////////////////////////////////////////////
void CIPCServerToVision::SetFetching(BOOL bOn)
{
	m_bFetching = bOn;
}
