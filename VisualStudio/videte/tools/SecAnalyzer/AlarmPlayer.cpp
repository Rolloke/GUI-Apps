
#include "stdafx.h"

#include "WK_Names.h"

#include "AlarmPlayer.h"
#include "SecAnalyzerDoc.h"

CIPCInputAlarmPlayer::CIPCInputAlarmPlayer(const CStatRecords &records)
	: CIPCInput(SM_SIMUNIT_INPUT,FALSE)
{
	CStatRecords alarmRecords;
	// collect all alarm records
	for (int i=0;i<records.GetSize();i++) {
		const CStatRecord &r = *records[i];
		if (r.m_sType=="AlarmLog") {
			CStatRecord *pNewRecord = new CStatRecord(r);
			m_alarmRecords.Add(pNewRecord);
			TRACE("Added %s with %d entries\n",pNewRecord->m_sName,
				pNewRecord->m_values.GetSize());
		}
	}
	
	m_dwStateMask = 0;
	
	m_realStartTime = CTime::GetCurrentTime();
	m_bInitDone=FALSE;

	m_playStartTime = m_alarmRecords[0]->m_values[0]->m_time;

	SetTimeoutCmdReceive(500);	// OOPS
	StartThread();
}

CIPCInputAlarmPlayer::~CIPCInputAlarmPlayer()
{
	StopThread();
}

void CIPCInputAlarmPlayer::OnRequestDisconnect()
{
}
void CIPCInputAlarmPlayer::OnRequestSetGroupID(WORD wGroupID)
{
	m_id.Set(wGroupID,0);
	DoConfirmSetGroupID(wGroupID);
}
void CIPCInputAlarmPlayer::OnRequestHardware(WORD wGroupID)
{
	DoConfirmHardware(wGroupID,0);
}
void CIPCInputAlarmPlayer::OnRequestSetEdge(WORD wGroupID,DWORD edgeMask)
{
	DoConfirmEdge(wGroupID,edgeMask);
}
void CIPCInputAlarmPlayer::OnRequestSetFree(WORD wGroupID,DWORD openMask)
{
	DoConfirmFree(wGroupID, openMask);
}
void CIPCInputAlarmPlayer::OnRequestReset(WORD wGroupID)
{
	DoConfirmReset(wGroupID);
}
void CIPCInputAlarmPlayer::OnRequestAlarmState(WORD wGroupID)
{
	m_bInitDone=TRUE;
	m_realStartTime = CTime::GetCurrentTime();
	DoConfirmAlarmState(wGroupID,0);	// OOPS
}


// thread function
BOOL CIPCInputAlarmPlayer::Run(DWORD dwTimeout)
{
	if (m_bInitDone) {
	int delta = (CTime::GetCurrentTime() - m_realStartTime).GetTotalSeconds();
	if ((delta % 10)==0) {
		TRACE("------------------\n");
		for (int j=0;j<m_alarmRecords.GetSize();j++) {
			CStatRecord *pRecord = m_alarmRecords[j];
			TRACE("%s with %d entries:\n",pRecord->m_sName,
				pRecord->m_values.GetSize());
			for (int d=0;d<pRecord->m_values.GetSize();d++) {
				TRACE("%d ",(pRecord->m_values[d]->m_time - m_playStartTime).GetTotalSeconds());
			}
			TRACE("\n");

		}
	}
	for (int i=0;i<m_alarmRecords.GetSize();i++) {
		DWORD dwOneBit = 1<<(m_alarmRecords.GetSize()-1-i);	// reverse order
		CStatRecord *pRecord = m_alarmRecords[i];
		BOOL bFuture = FALSE;
		while (pRecord->m_values.GetSize() && bFuture==FALSE) {
			int delta2 = (pRecord->m_values[0]->m_time - m_playStartTime).GetTotalSeconds();
			if (delta2 <= delta) {
				// indicate an alarm
				BOOL bCurrentState = (m_dwStateMask & dwOneBit)!=0;
				BOOL bNewState = pRecord->m_values[0]->m_dwValue;
				TRACE("Alarm[%d] at delta %d...\n",i,delta);
				if (bNewState != bCurrentState) {
					DWORD oldMask = m_dwStateMask;
					if (bNewState) {
						m_dwStateMask |= dwOneBit;
					} else {
						m_dwStateMask &= ~dwOneBit;
					}
					DoIndicateAlarmState(m_id.GetGroupID(),
							m_dwStateMask, 
							oldMask ^ m_dwStateMask
							);
				} else {
					TRACE("SAME state\n");
				}
				// remove it
				delete pRecord->m_values[0];
				pRecord->m_values.RemoveAt(0);
			} else {
				bFuture=TRUE;
			}
		}	// end of loop over values	
		if (pRecord->m_values.GetSize()==0) {
			delete pRecord;
			m_alarmRecords.RemoveAt(i);
			i--;	// adjust loop
		}
	}	// end of loop over records
	}
	return CIPCInput::Run(dwTimeout);
}

const CStatValues & CStatValues::operator = (const CStatValues &src)
{
	DeleteAll();
	for (int i=0;i<src.GetSize();i++) {
		Add( new CStatValue( *src[i] ) );
	}
	return *this;
}

CStatRecord::CStatRecord(const CStatRecord &src)
{
	m_sName = src.m_sName;
	m_values = src.m_values;
	m_sType = src.m_sType;
	m_dwMaxValue = src.m_dwMaxValue;
	m_dwMinValue = src.m_dwMinValue;
	m_Sum = src.m_Sum;
}


void CSecAnalyzerDoc::OnAlarmPlayback() 
{
	WK_DELETE(m_pPlayer);
	// NOT YET if (WK_IS_RUNNING(...
	m_pPlayer = new CIPCInputAlarmPlayer(*m_pRecords);
}

void CSecAnalyzerDoc::OnStopPlayer() 
{
	WK_DELETE(m_pPlayer);
}
