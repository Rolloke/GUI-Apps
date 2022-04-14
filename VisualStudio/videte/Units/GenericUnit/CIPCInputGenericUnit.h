/////////////////////////////////////////////////////////////////////////////
// PROJECT:		GenericUnit
// FILE:		$Workfile: CIPCInputGenericUnit.h $
// ARCHIVE:		$Archive: /Project/Units/GenericUnit/CIPCInputGenericUnit.h $
// CHECKIN:		$Date: 19.10.00 8:46 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 19.10.00 8:10 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef _CIPCInputGenericUnit_H_
#define _CIPCInputGenericUnit_H_

#include "CipcInput.h"

class CIPCInputGenericUnit : public CIPCInput
{
public:
	CIPCInputGenericUnit(CGenericUnitDlg* pMainWnd, CCodec *pCodec, const char *shmName);
	virtual ~CIPCInputGenericUnit();
	
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

	virtual void	AlarmStateChanged(WORD wAlarmState);
	virtual DWORD	GetAllCurrentAlarmStatus();
	virtual void	SetAllAlarmEdge(DWORD dwEdgeMask);
	virtual DWORD	GetAllAlarmEdge();

protected:
	CCodec*			m_pCodec;
	CGenericUnitDlg*	m_pMainWnd;
	WORD			m_wGroupID;

private:
	BOOL			m_bOK;
	DWORD			m_dwAlarmState;
	DWORD			m_dwFree;
	DWORD			m_dwEdge;
	DWORD			m_dwAck;
};

#endif	// _CIPCInputGenericUnit_H_
