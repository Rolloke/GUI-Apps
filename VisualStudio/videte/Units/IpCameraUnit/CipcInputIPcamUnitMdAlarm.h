/////////////////////////////////////////////////////////////////////////////
// PROJECT:		USBCamUnit
// FILE:		$Workfile: CipcInputIPcamUnitMdAlarm.h $
// ARCHIVE:		$Archive: /Project/Units/IPCameraUnit/CipcInputIPcamUnitMdAlarm.h $
// CHECKIN:		$Date: 30.08.06 10:56 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 30.08.06 10:45 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CIPCInputIpCamUnitMDAlarm_H__37D54821_381F_11D3_8D6E_004005A11E32__INCLUDED_)
#define AFX_CIPCInputIpCamUnitMDAlarm_H__37D54821_381F_11D3_8D6E_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CIPCInputIpCamUnit.h"

class CIPCInputIpCamUnitMDAlarm : public CIPCInputIpCamUnit  
{
public:
	CIPCInputIpCamUnitMDAlarm(CIpCameraUnitDlg* pDlg, LPCTSTR shmName);
	virtual ~CIPCInputIpCamUnitMDAlarm();

	BOOL IsValid();
	BOOL IsConnected();
	void OnReceivedMotionAlarm(WORD wSource, BOOL bState);
	
	// RequestXXX
	virtual void OnRequestAlarmState(WORD wGroupID);
	virtual void OnRequestSetFree(WORD wGroupID,DWORD openMask);	// 1 open, 0 closed
	
	// ResponseXXX
	virtual void OnResponseAlarmState(WORD wGroupID);
	virtual void OnResponseInputs(WORD wGroupID);

private:

};

#endif // !defined(AFX_CIPCInputIpCamUnitMDAlarm_H__37D54821_381F_11D3_8D6E_004005A11E32__INCLUDED_)
