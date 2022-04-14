/////////////////////////////////////////////////////////////////////////////
// PROJECT:		USBCamUnit
// FILE:		$Workfile: CipcInputUSBcamUnitMdAlarm.h $
// ARCHIVE:		$Archive: /Project/Units/USBCAM/USBCamUnit/CipcInputUSBcamUnitMdAlarm.h $
// CHECKIN:		$Date: 17.11.03 14:01 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 28.04.03 12:41 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CIPCInputUSBcamUnitMDAlarm_H__37D54821_381F_11D3_8D6E_004005A11E32__INCLUDED_)
#define AFX_CIPCInputUSBcamUnitMDAlarm_H__37D54821_381F_11D3_8D6E_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CIPCInputUSBcamUnitMDAlarm : public CIPCInputUSBcamUnit  
{
public:
	CIPCInputUSBcamUnitMDAlarm(CUSBCamUnitDlg* pMainWnd, LPCTSTR shmName);
	virtual ~CIPCInputUSBcamUnitMDAlarm();

	BOOL IsValid();

	// RequestXXX
	virtual void OnRequestSetEdge(WORD wGroupID,DWORD edgeMask);	// 1 positive, 0 negative
	virtual void OnRequestSetFree(WORD wGroupID,DWORD openMask);	// 1 open, 0 closed
	virtual void OnRequestReset(WORD wGroupID);	
	virtual void OnRequestAlarmState(WORD wGroupID);
	
	// ResponseXXX
	virtual void OnResponseAlarmState(WORD wGroupID);
	virtual void OnResponseInputs(WORD wGroupID);

//	void	AlarmStateChanged(WORD wAlarmState);
	void	OnReceivedMotionAlarm(WORD wSource, CMDPoints& Points, BOOL bState);

private:
	DWORD			m_dwAlarmState;
	DWORD			m_dwEdgeMask;


};

#endif // !defined(AFX_CIPCInputUSBcamUnitMDAlarm_H__37D54821_381F_11D3_8D6E_004005A11E32__INCLUDED_)
