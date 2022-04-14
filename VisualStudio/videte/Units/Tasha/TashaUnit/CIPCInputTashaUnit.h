/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaUnit
// FILE:		$Workfile: CIPCInputTashaUnit.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaUnit/CIPCInputTashaUnit.h $
// CHECKIN:		$Date: 11.03.04 15:53 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 11.03.04 15:51 $
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
	CIPCInputTashaUnit(CTashaUnitDlg* pMainWnd, CCodec *pCodec, LPCTSTR shmName);
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

	virtual void AlarmStateChanged(WORD wAlarmState);
	virtual void AdapterSelectStateChanged(WORD wAdapterSelectState);
	virtual void PCKeysStateChanged(WORD wPCKeysState);
	
	
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
	DWORD			m_dwAdapterSelectState;
	DWORD			m_dwPCKeysState;
	DWORD			m_dwFree;
	DWORD			m_dwEdge;
	DWORD			m_dwAck;
};

#endif	// _CIPCInputTashaUnit_H_
