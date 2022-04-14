/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaUnit
// FILE:		$Workfile: CIPCInputTashaUnit.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha(Developer)/TashaUnit/CIPCInputTashaUnit.h $
// CHECKIN:		$Date: 5.01.04 9:54 $
// VERSION:		$Revision: 1 $
// LAST CHANGE:	$Modtime: 5.01.04 9:25 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef _CIPCInputTashaUnit_H_
#define _CIPCInputTashaUnit_H_

#include "CipcInput.h"

class CCodec;
class CIPCInputTashaUnit : public CIPCInput
{
public:
	CIPCInputTashaUnit(CTashaUnitDlg* pMainWnd, CCodec *pCodec, const char *shmName);
	virtual ~CIPCInputTashaUnit();
	
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

	void OnConfirmAlarmState(DWORD dwAlarmStateMask);
	void OnConfirmSetEdge(DWORD dwAlarmEdge);
	void OnConfirmGetEdge(DWORD dwAlarmEdge);

protected:
	CCodec*			m_pCodec;
	CTashaUnitDlg*	m_pMainWnd;
	WORD			m_wGroupID;

private:
	BOOL			m_bOK;
	DWORD			m_dwAlarmState;
	DWORD			m_dwFree;
	DWORD			m_dwEdge;
	DWORD			m_dwAck;
};

#endif	// _CIPCInputTashaUnit_H_
