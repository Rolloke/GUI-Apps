/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicUnit
// FILE:		$Workfile: CIPCInputSavicUnit.h $
// ARCHIVE:		$Archive: /Project/Units/Savic/SavicUnit/CIPCInputSavicUnit.h $
// CHECKIN:		$Date: 20.06.03 13:15 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 20.06.03 12:22 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef _CIPCInputSaVicUnit_H_
#define _CIPCInputSaVicUnit_H_

#include "CipcInput.h"

class CCodec;
class CIPCInputSaVicUnit : public CIPCInput
{
public:
	CIPCInputSaVicUnit(CSaVicUnitDlg* pMainWnd, CCodec *pCodec, LPCTSTR shmName);
	virtual ~CIPCInputSaVicUnit();
	
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
	CSaVicUnitDlg*	m_pMainWnd;
	WORD			m_wGroupID;

private:
	BOOL			m_bOK;
	DWORD			m_dwAlarmState;
	DWORD			m_dwFree;
	DWORD			m_dwEdge;
	DWORD			m_dwAck;
};

#endif	// _CIPCInputSaVicUnit_H_
