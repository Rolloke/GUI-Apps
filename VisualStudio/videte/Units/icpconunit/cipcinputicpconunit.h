/////////////////////////////////////////////////////////////////////////////
// PROJECT:		ICPCONUnit
// FILE:		$Workfile: cipcinputicpconunit.h $
// ARCHIVE:		$Archive: /Project/Units/ICPCONUnit/cipcinputicpconunit.h $
// CHECKIN:		$Date: 27.02.04 13:42 $
// VERSION:		$Revision: 6 $
// LAST CHANGE:	$Modtime: 27.02.04 13:39 $
// BY AUTHOR:	$Author: Georg.feddern $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef _CIPCInputICPCONUnit_H_
#define _CIPCInputICPCONUnit_H_

#include "CipcInput.h"

class CIPCInputICPCONUnit : public CIPCInput
{
friend class CICPI7000Module;
public:
	BOOL GetState(int);
	WORD GetGroupID();
	bool IsInputActive(int);
	int  GetNoOfInputs();
	CIPCInputICPCONUnit(CWnd* pMainWnd, LPCTSTR shmName);
	virtual ~CIPCInputICPCONUnit();
	
	BOOL IsValid();

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

	void SetAlarm(int nBit, bool bSet);

protected:

private:
	CWnd					*m_pMainWnd;
	WORD					m_wGroupID;
	DWORD             m_dwEdge;
	DWORD             m_dwMask;
	DWORD             m_dwState;
};

#endif	// _CIPCInputICPCONUnit_H_
