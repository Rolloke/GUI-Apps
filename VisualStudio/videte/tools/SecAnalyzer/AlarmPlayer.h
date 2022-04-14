#ifndef _CIPCInputAlarmPlayer_H
#define _CIPCInputAlarmPlayer_H

#include "CIPCInput.h"
#include "analyze.h"

class CIPCInputAlarmPlayer : public CIPCInput
{
public:
	CIPCInputAlarmPlayer(const CStatRecords &records);
	~CIPCInputAlarmPlayer();
	//	
	virtual void OnRequestDisconnect();
	virtual void OnRequestSetGroupID(WORD wGroupID);
	virtual void OnRequestHardware(WORD wGroupID);
	virtual void OnRequestSetEdge(WORD wGroupID,DWORD edgeMask);	// 1 positive, 0 negative
	virtual void OnRequestSetFree(WORD wGroupID,DWORD openMask);	// 1 open, 0 closed
	virtual void OnRequestReset(WORD wGroupID);	
	virtual void OnRequestAlarmState(WORD wGroupID);
	//
	virtual BOOL Run(DWORD dwTimeout);
	//
	CSecID	m_id;
	CStatRecords m_alarmRecords;
	//
	CTime m_realStartTime;
	CTime m_playStartTime;
	//
	DWORD m_dwStateMask;
	BOOL m_bInitDone;
};


#endif
