/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicUnit
// FILE:		$Workfile: CIPCInputSavicUnitMDAlarm.h $
// ARCHIVE:		$Archive: /Project/Units/Savic/SavicUnit/CIPCInputSavicUnitMDAlarm.h $
// CHECKIN:		$Date: 20.06.03 13:15 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 20.06.03 12:22 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CIPCINPUTSAVICUNITMDALARM_H__37D54821_381F_11D3_8D6E_004005A11E32__INCLUDED_)
#define AFX_CIPCINPUTSAVICUNITMDALARM_H__37D54821_381F_11D3_8D6E_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CIPCInputSaVicUnitMDAlarm : public CIPCInputSaVicUnit  
{
public:
	CIPCInputSaVicUnitMDAlarm(CSaVicUnitDlg* pMainWnd, CCodec *pCodec, LPCTSTR shmName);
	virtual ~CIPCInputSaVicUnitMDAlarm();

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

#endif // !defined(AFX_CIPCINPUTSAVICUNITMDALARM_H__37D54821_381F_11D3_8D6E_004005A11E32__INCLUDED_)
