// RequestThread.cpp: implementation of the CRequestThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DVProcess.h"
#include "RequestThread.h"
#include "CameraGroup.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRequestThread::CRequestThread(const CString& sName)
 : CWK_Thread(sName,NULL)
{
	m_pCameraGroup = NULL;
	m_dwStatTime = 4*60*60*1000;
	m_dwTick = WK_GetTickCount();
}
//////////////////////////////////////////////////////////////////////
CRequestThread::~CRequestThread()
{

}	
//////////////////////////////////////////////////////////////////////
void CRequestThread::SetCameraGroup(CCameraGroup* pGroup)
{
	m_pCameraGroup = pGroup;
}
//////////////////////////////////////////////////////////////////////
BOOL CRequestThread::IsTimedOut()
{
	BOOL bRet = FALSE;
	
	m_csTick.Lock();
	bRet = WK_GetTimeSpan(m_dwTick)>10000;
	m_csTick.Unlock();

	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CRequestThread::Run(LPVOID lpContext)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	m_csTick.Lock();
	m_dwTick = WK_GetTickCount();
	m_csTick.Unlock();
	if (m_pCameraGroup->GetIPCState() == CIPC_STATE_CONNECTED)
	{
		m_pCameraGroup->Request();
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
