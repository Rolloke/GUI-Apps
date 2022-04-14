/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaUnit
// FILE:		$Workfile: CIPCInputTashaUnitMDAlarm.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaUnit/CIPCInputTashaUnitMDAlarm.h $
// CHECKIN:		$Date: 11.03.04 15:53 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 11.03.04 15:51 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CIPCINPUTJACOBUNITMDALARM_H__37D54821_381F_11D3_8D6E_004005A11E32__INCLUDED_)
#define AFX_CIPCINPUTJACOBUNITMDALARM_H__37D54821_381F_11D3_8D6E_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CIPCInputTashaUnitMDAlarm : public CIPCInputTashaUnit  
{
public:
	CIPCInputTashaUnitMDAlarm(CTashaUnitDlg* pMainWnd, CCodec *pCodec, LPCTSTR shmName);
	virtual ~CIPCInputTashaUnitMDAlarm();

	BOOL IsValid();

	// RequestXXX
	virtual void OnRequestSetEdge(WORD wGroupID,DWORD edgeMask);	// 1 positive, 0 negative
	virtual void OnRequestSetFree(WORD wGroupID,DWORD openMask);	// 1 open, 0 closed
	virtual void OnRequestReset(WORD wGroupID);	
	virtual void OnRequestAlarmState(WORD wGroupID);
	
	// ResponseXXX
	virtual void OnResponseAlarmState(WORD wGroupID);
	virtual void OnResponseInputs(WORD wGroupID);

	void	AlarmStateChanged(WORD wAlarmState);
	void	OnReceivedMotionAlarm(DATA_PACKET* pPacket, BOOL bState);

private:
	DWORD			m_dwAlarmState;
	DWORD			m_dwEdgeMask;
};

#endif // !defined(AFX_CIPCINPUTJACOBUNITMDALARM_H__37D54821_381F_11D3_8D6E_004005A11E32__INCLUDED_)
