/////////////////////////////////////////////////////////////////////////////
// PROJECT:		QUnit
// FILE:		$Workfile: CIPCInputQUnit.h $
// ARCHIVE:		$Archive: /Project/Units/QUnit/CIPCInputQUnit.h $
// CHECKIN:		$Date: 1.12.05 16:09 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 1.12.05 14:53 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef _CIPCInputQUnit_H_
#define _CIPCInputQUnit_H_

#include "CipcInput.h"

class CUDP;
class CIPCInputQUnit : public CIPCInput
{
public:
	CIPCInputQUnit(CQUnitDlg* pMainWnd, CUDP* pUDP, LPCTSTR shmName);
	virtual ~CIPCInputQUnit();
	
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

	virtual void	AlarmStateChanged(DWORD dwAlarmState);
	virtual DWORD	GetAlarmEdge();

protected:
	CUDP*			m_pUDP;
	CQUnitDlg*	m_pMainWnd;
	WORD			m_wGroupID;

private:
	BOOL			m_bOK;
	DWORD			m_dwAlarmState;
	DWORD			m_dwFree;
	DWORD			m_dwEdge;
	DWORD			m_dwAck;
};

#endif	// _CIPCInputQUnit_H_
