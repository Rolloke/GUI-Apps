/////////////////////////////////////////////////////////////////////////////
// PROJECT:		QUnit
// FILE:		$Workfile: CIPCInputQUnitMDAlarm.h $
// ARCHIVE:		$Archive: /Project/Units/QUnit/CIPCInputQUnitMDAlarm.h $
// CHECKIN:		$Date: 1.12.05 16:09 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 1.12.05 14:49 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CIPCINPUTQUnitMDALARM_H__37D54821_381F_11D3_8D6E_004005A11E32__INCLUDED_)
#define AFX_CIPCINPUTQUnitMDALARM_H__37D54821_381F_11D3_8D6E_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CIPCInputQUnitMDAlarm : public CIPCInputQUnit  
{
public:
	CIPCInputQUnitMDAlarm(CQUnitDlg* pMainWnd, CUDP *pUDP, LPCTSTR shmName);
	virtual ~CIPCInputQUnitMDAlarm();

	BOOL IsValid();

	// RequestXXX
	virtual void OnRequestSetEdge(WORD wGroupID,DWORD edgeMask);	// 1 positive, 0 negative
	virtual void OnRequestSetFree(WORD wGroupID,DWORD openMask);	// 1 open, 0 closed
	virtual void OnRequestReset(WORD wGroupID);	
	virtual void OnRequestAlarmState(WORD wGroupID);
	
	// ResponseXXX
	virtual void OnResponseAlarmState(WORD wGroupID);
	virtual void OnResponseInputs(WORD wGroupID);

	void	OnReceivedMotionAlarm(int nCamera, CMDPoints& Points, BOOL bState);

private:
	DWORD			m_dwAlarmState;
	DWORD			m_dwEdgeMask;


};

#endif // !defined(AFX_CIPCINPUTQUnitMDALARM_H__37D54821_381F_11D3_8D6E_004005A11E32__INCLUDED_)
