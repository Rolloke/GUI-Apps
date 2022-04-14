/////////////////////////////////////////////////////////////////////////////
// PROJECT:		AkuUnit
// FILE:		$Workfile: CIPCInputAkuUnit.h $
// ARCHIVE:		$Archive: /Project/Units/Aku/AkuUnit/CIPCInputAkuUnit.h $
// CHECKIN:		$Date: 27.02.04 9:53 $
// VERSION:		$Revision: 4 $
// LAST CHANGE:	$Modtime: 27.02.04 9:02 $
// BY AUTHOR:	$Author: Georg.feddern $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef _CIPCInputAkuUnit_H_
#define _CIPCInputAkuUnit_H_

#include "CipcInput.h"

class CAku;

class CIPCInputAkuUnit : public CIPCInput
{
public:
	CIPCInputAkuUnit(CAku *pAku, LPCTSTR shmName);
	~CIPCInputAkuUnit();
	BOOL IsValid();


   // RequestXXX
 	virtual void OnRequestSetGroupID(WORD wGroupID);
	virtual void OnRequestHardware(WORD wGroupID);
	virtual void OnRequestSetEdge(WORD wGroupID,DWORD edgeMask);	// 1 positive, 0 negative
	virtual void OnRequestSetFree(WORD wGroupID,DWORD openMask);	// 1 open, 0 closed
	virtual void OnRequestReset(WORD wGroupID);	
	virtual void OnRequestAlarmState(WORD wGroupID);
	
	// ResponseXXX
	virtual void OnResponseAlarmState(WORD wGroupID);
	virtual void OnResponseInputs(WORD wGroupID);

	void	AlarmStateChanged(WORD wExtCard, WORD wAlarmState);
	DWORD	GetAllCurrentAlarmStatus();
	DWORD	GetAllAlarmStatus();
	void	SetAllAlarmEdge(DWORD dwEdgeMask);
	DWORD	GetAllAlarmEdge();
	void	SetAllAlarmAck(DWORD dwAckMask);
	DWORD	GetAllAlarmAck();

//	void CheckForEvent();

private:
	CAku*  m_pAku;
	WORD	m_wGroupID;
	BOOL	m_bOK;
	DWORD	m_dwHardwareState;
	DWORD	m_dwAlarmState;
	DWORD	m_dwFree;
	DWORD	m_dwEdge;
	DWORD	m_dwAck;
};

#endif	// _CIPCInputAkuUnit_H_