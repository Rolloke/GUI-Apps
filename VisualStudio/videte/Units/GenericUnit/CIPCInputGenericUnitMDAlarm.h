/////////////////////////////////////////////////////////////////////////////
// PROJECT:		GenericUnit
// FILE:		$Workfile: CIPCInputGenericUnitMDAlarm.h $
// ARCHIVE:		$Archive: /Project/Units/GenericUnit/CIPCInputGenericUnitMDAlarm.h $
// CHECKIN:		$Date: 19.10.00 8:46 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 19.10.00 8:14 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CIPCINPUTGENERICUNITMDALARM_H__37D54821_381F_11D3_8D6E_004005A11E32__INCLUDED_)
#define AFX_CIPCINPUTGENERICUNITMDALARM_H__37D54821_381F_11D3_8D6E_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CIPCInputGenericUnitMDAlarm : public CIPCInputGenericUnit  
{
public:
	CIPCInputGenericUnitMDAlarm(CGenericUnitDlg* pMainWnd, CCodec *pCodec, const char *shmName);
	virtual ~CIPCInputGenericUnitMDAlarm();

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
	void	OnReceivedMotionAlarm(WORD wSource, CPoint Point, BOOL bState);

private:
	DWORD			m_dwAlarmState;
	DWORD			m_dwEdgeMask;


};

#endif // !defined(AFX_CIPCINPUTGENERICUNITMDALARM_H__37D54821_381F_11D3_8D6E_004005A11E32__INCLUDED_)
