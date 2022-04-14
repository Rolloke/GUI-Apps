/////////////////////////////////////////////////////////////////////////////
// PROJECT:		USBCamUnit
// FILE:		$Workfile: CipcInputUSBcamunit.h $
// ARCHIVE:		$Archive: /Project/Units/USBCAM/USBCamUnit/CipcInputUSBcamunit.h $
// CHECKIN:		$Date: 17.11.03 14:01 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 28.04.03 12:41 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef _CIPCInputUSBcamUnit_H_
#define _CIPCInputUSBcamUnit_H_

#include "CipcInput.h"


class CIPCInputUSBcamUnit : public CIPCInput
{
public:
	CIPCInputUSBcamUnit(CUSBCamUnitDlg* pMainWnd, LPCTSTR shmName);
	virtual ~CIPCInputUSBcamUnit();
	
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
	CUSBCamUnitDlg*m_pMainWnd;
	WORD			m_wGroupID;

private:
	BOOL			m_bOK;
	DWORD			m_dwAlarmState;
	DWORD			m_dwFree;
	DWORD			m_dwEdge;
	DWORD			m_dwAck;
	DWORD			m_dwAlarmMask;
};

#endif	// _CIPCInputUSBcamUnit_H_
