/////////////////////////////////////////////////////////////////////////////
// PROJECT:		USBCamUnit
// FILE:		$Workfile: CipcInputUSBcamunit.h $
// ARCHIVE:		$Archive: /Project/Units/USBCAM/USBCameraUnit/CipcInputUSBcamunit.h $
// CHECKIN:		$Date: 6.12.04 12:09 $
// VERSION:		$Revision: 6 $
// LAST CHANGE:	$Modtime: 2.12.04 15:41 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef _CIPCInputUSBcamUnit_H_
#define _CIPCInputUSBcamUnit_H_

#include "CipcInput.h"

class CUSBCameraUnitDlg;

class CIPCInputUSBcamUnit : public CIPCInput
{
public:
	CIPCInputUSBcamUnit(CUSBCameraUnitDlg* pMainWnd, LPCTSTR shmName);
	virtual ~CIPCInputUSBcamUnit();
	
	BOOL IsValid();
	BOOL IsAlarmActive(int);
	BOOL DoReset() { return m_bReset;}

	// operations
	void ConfirmReset();

   // RequestXXX
 	virtual void OnRequestSetGroupID(WORD wGroupID);
	virtual void OnRequestHardware(WORD wGroupID);
	virtual void OnRequestSetEdge(WORD wGroupID,DWORD edgeMask);	// 1 positive, 0 negative
	virtual void OnRequestSetFree(WORD wGroupID,DWORD openMask);	// 1 open, 0 closed
	virtual void OnRequestReset(WORD wGroupID);	
	virtual void OnRequestAlarmState(WORD wGroupID);
	virtual void OnRequestDisconnect();
	
	// ResponseXXX
	virtual void OnResponseAlarmState(WORD wGroupID);
	virtual void OnResponseInputs(WORD wGroupID);

	virtual void	AlarmStateChanged(WORD wCam, bool bSet);
	virtual DWORD	GetAllCurrentAlarmStatus();

protected:
	CUSBCameraUnitDlg*m_pMainWnd;
	WORD			m_wGroupID;

	BOOL			m_bOK;
	DWORD			m_dwAlarmState;
	DWORD			m_dwFree;
	DWORD			m_dwEdge;
	DWORD			m_dwAck;
	DWORD			m_dwAlarmMask;
	BOOL			m_bReset;
};

#endif	// _CIPCInputUSBcamUnit_H_
