/////////////////////////////////////////////////////////////////////////////
// PROJECT:		CoCoUnit
// FILE:		$Workfile: CIPCInputCoCoUnit.h $
// ARCHIVE:		$Archive: /Project/Units/CoCoUnit/CIPCInputCoCoUnit.h $
// CHECKIN:		$Date: 5.08.98 9:43 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 5.08.98 9:43 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef _CIPCInputCoCoUnit_H_
#define _CIPCInputCoCoUnit_H_

#include "CipcInput.h"

class CCoCo;

class CIPCInputCoCoUnit : public CIPCInput
{
public:
	CIPCInputCoCoUnit(CCoCo *pCoCo, BYTE byCoCoID, const char *shmName);
	~CIPCInputCoCoUnit();
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

private:
	CCoCo*  m_pCoCo;
	BYTE	m_byCoCoID;
	WORD	m_wGroupID;
	BOOL	m_bOK;
	DWORD	m_dwHardwareState;
	DWORD	m_dwAlarmState;
	DWORD	m_dwFree;
	DWORD	m_dwEdge;
	DWORD	m_dwAck;
};

#endif	// _CIPCInputCoCoUnit_H_