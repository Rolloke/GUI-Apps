/////////////////////////////////////////////////////////////////////////////
// PROJECT:		USBCamUnit
// FILE:		$Workfile: CipcInputUSBcamUnitMdAlarm.h $
// ARCHIVE:		$Archive: /Project/Units/USBCAM/USBCameraUnit/CipcInputUSBcamUnitMdAlarm.h $
// CHECKIN:		$Date: 26.08.03 15:07 $
// VERSION:		$Revision: 4 $
// LAST CHANGE:	$Modtime: 25.08.03 11:45 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CIPCInputUSBcamUnitMDAlarm_H__37D54821_381F_11D3_8D6E_004005A11E32__INCLUDED_)
#define AFX_CIPCInputUSBcamUnitMDAlarm_H__37D54821_381F_11D3_8D6E_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CIPCInputUSBcamUnit.h"

class CIPCInputUSBcamUnitMDAlarm : public CIPCInputUSBcamUnit  
{
public:
	CIPCInputUSBcamUnitMDAlarm(CUSBCameraUnitDlg* pMainWnd, LPCTSTR shmName);
	virtual ~CIPCInputUSBcamUnitMDAlarm();

	BOOL IsValid();

	// RequestXXX
	virtual void OnRequestReset(WORD wGroupID);	
	virtual void OnRequestAlarmState(WORD wGroupID);
	virtual void OnRequestSetFree(WORD wGroupID,DWORD openMask);	// 1 open, 0 closed
	
	// ResponseXXX
	virtual void OnResponseAlarmState(WORD wGroupID);
	virtual void OnResponseInputs(WORD wGroupID);

	void	AlarmStateChanged(WORD wAlarmState);
	void	OnReceivedMotionAlarm(WORD wSource, CMDPoints& Points, BOOL bState);

private:

};

#endif // !defined(AFX_CIPCInputUSBcamUnitMDAlarm_H__37D54821_381F_11D3_8D6E_004005A11E32__INCLUDED_)
