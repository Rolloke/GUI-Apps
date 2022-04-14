// CIPCInputTashaUnitMDAlarm.cpp: implementation of the CIPCInputTashaUnitMDAlarm class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TashaUnitDlg.h"
#include "CIPCInputTashaUnit.h"
#include "CIPCInputTashaUnitMDAlarm.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPCInputTashaUnitMDAlarm::CIPCInputTashaUnitMDAlarm(CTashaUnitDlg* pMainWnd, CCodec *pCodec, const char *shmName)
	: CIPCInputTashaUnit(pMainWnd, pCodec, shmName)
{
//	TRACE("CIPCInputTashaUnitMDAlarm::CIPCInputTashaUnitMDAlarm\n");
	m_dwAlarmState	= 0;
	m_dwEdgeMask	= 0;
}

//////////////////////////////////////////////////////////////////////////////////////
CIPCInputTashaUnitMDAlarm::~CIPCInputTashaUnitMDAlarm()
{
//	TRACE("CIPCInputTashaUnitMDAlarm::~CIPCInputTashaUnitMDAlarm\n");
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCInputTashaUnitMDAlarm::IsValid()
{
	return CIPCInputTashaUnit::IsValid();
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputTashaUnitMDAlarm::OnRequestSetEdge(WORD wGroupID, DWORD dwEdgeMask)
{
//	TRACE("CIPCInputTashaUnitMDAlarm::OnRequestSetEdge\tGroupID=%i\t0x%lx\n", wGroupID, dwEdgeMask);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCInputTashaUnitMDAlarm::OnRequestSetEdge\tWrong wGroupID\n");
		return;
	}

	m_dwEdgeMask = dwEdgeMask;

	DoConfirmEdge(m_wGroupID, dwEdgeMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputTashaUnitMDAlarm::OnRequestSetFree(WORD wGroupID, DWORD dwOpenMask)
{
//	TRACE("CIPCInputTashaUnitMDAlarm::OnRequestSetFree\tGroupID=%i\tOpenMask=0x%lx\n",wGroupID, dwOpenMask);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCInputTashaUnitMDAlarm::OnRequestSetFree\tWrong wGroupID\n");
		return;
	}

	DoConfirmFree(m_wGroupID, dwOpenMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputTashaUnitMDAlarm::OnRequestReset(WORD wGroupID)	
{
	WK_TRACE("CIPCInputTashaUnitMDAlarm::OnRequestReset\tGroupID=%i\n", wGroupID);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCInputTashaUnitMDAlarm::OnRequestReset\tWrong wGroupID\n");

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
void CIPCInputTashaUnitMDAlarm::OnRequestAlarmState(WORD wGroupID)
{
	// TRACE("CIPCInputTashaUnitMDAlarm::OnRequestAlarmState\tGroupID=%i\n", wGroupID);
 
	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCInputTashaUnitMDAlarm::OnRequestAlarmState\tWrong wGroupID\n");
		return;
	}

	DoConfirmAlarmState(m_wGroupID, m_dwAlarmState ^ m_dwEdgeMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputTashaUnitMDAlarm::OnResponseAlarmState(WORD wGroupID)
{
//	TRACE("CIPCInputTashaUnitMDAlarm::OnResponseAlarmState   Not Yet \n");

   	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCInputTashaUnitMDAlarm::OnResponseAlarmState\tWrong wGroupID\n");
		return;
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputTashaUnitMDAlarm::OnResponseInputs(WORD wGroupID)
{
//	TRACE("CIPCInputTashaUnitMDAlarm::OnResponseInputs   Not Yet\n");

   	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCInputTashaUnitMDAlarm::OnResponseInputs\tWrong wGroupID\n");
		return;
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputTashaUnitMDAlarm::AlarmStateChanged(WORD wAlarmState)
{
	DWORD dwNewState = m_dwAlarmState;

	// Die Tasha besitzt 16 MD-Alarmeingänge
	dwNewState = dwNewState & 0xffff0000 | (DWORD)wAlarmState; 

	//	WK_TRACE("CIPCInputTashaUnitMDAlarm::AlarmStateChanged (0x%x,0x%x)\n", dwNewState, m_dwAlarmState);

	DoIndicateAlarmState(m_wGroupID, dwNewState, dwNewState^m_dwAlarmState);


	m_dwAlarmState = dwNewState;
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputTashaUnitMDAlarm::OnReceivedMotionAlarm(DataPacket* pPacket, BOOL bState)
{
	if (!pPacket)
		return;

	DWORD dwChangeMask = 1<<pPacket->wSource;
	if (bState)
	{
		m_dwAlarmState = 1<<pPacket->wSource;
	}
	else
	{
		m_dwAlarmState = 0;
	}

	CIPCFields fields;
	if (bState)
	{
		CString sValueX, sValueY;
		CString sDBDNameX, sDBDNameY;

		for (int nI = 0; nI < 5; nI++)
		{
			if ((pPacket->Point[nI].cx != 0) && (pPacket->Point[nI].cy != 0))
			{
				sValueX.Format("%04hx", pPacket->Point[nI].cx);
				sValueY.Format("%04hx", pPacket->Point[nI].cy);
				
				// Das 1. Element erhält aus kompatibilitätsgründen keine Nummer
				// Der Name ist (DB)atabase (D)ata
				// DBP ist das idip Format, für DTS wird in
				// DVProcess das DBP_ durch DVD_ (D)igital (V)ideo (D)ata ersetzt
				if (nI == 0)
				{
					sDBDNameX = "DBD_MD_X";
					sDBDNameY = "DBD_MD_Y";
				}
				else // > 1
				{
					sDBDNameX.Format("DBD_MD_X%01d", nI+1);
					sDBDNameY.Format("DBD_MD_Y%01d", nI+1);
				}

				fields.Add(new CIPCField(sDBDNameX, sValueX, 'C'));
				fields.Add(new CIPCField(sDBDNameY, sValueY, 'C'));
			}
		}
	}

	DoIndicateAlarmFieldsState(m_wGroupID,
							   m_dwAlarmState,	// 1 high, 0 low
							   dwChangeMask,	// 1 changed, 0 unchanged
							   fields);
}

