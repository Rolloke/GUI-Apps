// CIPCInputGenericUnitMDAlarm.cpp: implementation of the CIPCInputGenericUnitMDAlarm class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GenericUnitDlg.h"
#include "CIPCInputGenericUnit.h"
#include "CIPCInputGenericUnitMDAlarm.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPCInputGenericUnitMDAlarm::CIPCInputGenericUnitMDAlarm(CGenericUnitDlg* pMainWnd, CCodec *pCodec, const char *shmName)
	: CIPCInputGenericUnit(pMainWnd, pCodec, shmName)
{
//	TRACE("CIPCInputGenericUnitMDAlarm::CIPCInputGenericUnitMDAlarm\n");
	m_dwAlarmState	= 0;
	m_dwEdgeMask	= 0;
}

//////////////////////////////////////////////////////////////////////////////////////
CIPCInputGenericUnitMDAlarm::~CIPCInputGenericUnitMDAlarm()
{
//	TRACE("CIPCInputGenericUnitMDAlarm::~CIPCInputGenericUnitMDAlarm\n");
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCInputGenericUnitMDAlarm::IsValid()
{
	return CIPCInputGenericUnit::IsValid();
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputGenericUnitMDAlarm::OnRequestSetEdge(WORD wGroupID, DWORD dwEdgeMask)
{
//	TRACE("CIPCInputGenericUnitMDAlarm::OnRequestSetEdge\tGroupID=%i\t0x%lx\n", wGroupID, dwEdgeMask);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCInputGenericUnitMDAlarm::OnRequestSetEdge\tWrong wGroupID\n");
		return;
	}

	m_dwEdgeMask = dwEdgeMask;

	DoConfirmEdge(m_wGroupID, dwEdgeMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputGenericUnitMDAlarm::OnRequestSetFree(WORD wGroupID, DWORD dwOpenMask)
{
//	TRACE("CIPCInputGenericUnitMDAlarm::OnRequestSetFree\tGroupID=%i\tOpenMask=0x%lx\n",wGroupID, dwOpenMask);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCInputGenericUnitMDAlarm::OnRequestSetFree\tWrong wGroupID\n");
		return;
	}

	DoConfirmFree(m_wGroupID, dwOpenMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputGenericUnitMDAlarm::OnRequestReset(WORD wGroupID)	
{
	WK_TRACE("CIPCInputGenericUnitMDAlarm::OnRequestReset\tGroupID=%i\n", wGroupID);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCInputGenericUnitMDAlarm::OnRequestReset\tWrong wGroupID\n");

		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
	}
	else
	{	
		m_dwAlarmState = 0;
		DoConfirmReset(m_wGroupID);
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputGenericUnitMDAlarm::OnRequestAlarmState(WORD wGroupID)
{
	// TRACE("CIPCInputGenericUnitMDAlarm::OnRequestAlarmState\tGroupID=%i\n", wGroupID);
 
	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCInputGenericUnitMDAlarm::OnRequestAlarmState\tWrong wGroupID\n");
		return;
	}

	DoConfirmAlarmState(m_wGroupID, m_dwAlarmState ^ m_dwEdgeMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputGenericUnitMDAlarm::OnResponseAlarmState(WORD wGroupID)
{
//	TRACE("CIPCInputGenericUnitMDAlarm::OnResponseAlarmState   Not Yet \n");

   	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCInputGenericUnitMDAlarm::OnResponseAlarmState\tWrong wGroupID\n");
		return;
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputGenericUnitMDAlarm::OnResponseInputs(WORD wGroupID)
{
//	TRACE("CIPCInputGenericUnitMDAlarm::OnResponseInputs   Not Yet\n");

   	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCInputGenericUnitMDAlarm::OnResponseInputs\tWrong wGroupID\n");
		return;
	}
}

//////////////////////////////////////////////////////////////////////////////////////
/*void CIPCInputGenericUnitMDAlarm::AlarmStateChanged(WORD wAlarmState)
{
	DWORD dwNewState = m_dwAlarmState;

	// Die Generic besitzt 16 MD-Alarmeingänge
	dwNewState = dwNewState & 0xffff0000 | (DWORD)wAlarmState; 

	//	WK_TRACE("CIPCInputGenericUnitMDAlarm::AlarmStateChanged (0x%x,0x%x)\n", dwNewState, m_dwAlarmState);

	DoIndicateAlarmState(m_wGroupID, dwNewState, dwNewState^m_dwAlarmState);


	m_dwAlarmState = dwNewState;
}
*/

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputGenericUnitMDAlarm::OnReceivedMotionAlarm(WORD wSource, CPoint Point, BOOL bState)
{
	m_dwAlarmState = 1<<wSource;

	DoIndicateAlarmNr(CSecID(m_wGroupID, wSource), bState, MAKELONG(Point.x, Point.y), 0);
}

