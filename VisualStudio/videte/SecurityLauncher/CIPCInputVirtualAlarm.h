// CIPCInputVirtualAlarm.h: interface for the CIPCInputVirtualAlarm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CIPCINPUTVIRTUALALARM_H__B4B012D4_A633_11D2_8CA9_004005A11E32__INCLUDED_)
#define AFX_CIPCINPUTVIRTUALALARM_H__B4B012D4_A633_11D2_8CA9_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CipcInput.h"

////////////////////////////////////////////////////////////////

class CIPCInputVirtualAlarm : public CIPCInput
{
public:
	CIPCInputVirtualAlarm();
	virtual ~CIPCInputVirtualAlarm();
	BOOL OnErrorArrived(CWK_RunTimeError *pError);

	// OnRequestXXX virtual
 	virtual void OnRequestSetGroupID(WORD wGroupID);
	virtual void OnRequestHardware(WORD wGroupID);
	virtual void OnRequestSetEdge(WORD wGroupID,DWORD edgeMask);	// 1 positive, 0 negative
	virtual void OnRequestSetFree(WORD wGroupID,DWORD openMask);	// 1 open, 0 closed
	virtual void OnRequestReset(WORD wGroupID);	
	virtual void OnRequestAlarmState(WORD wGroupID);

	virtual void OnIndicateError(DWORD dwCmd, 
							     CSecID id, 
								 CIPCError error, 
								 int iErrorCode,
								 const CString &sErrorMessage);
private:
	WORD  m_wGroupID;
	DWORD m_dwAlarmState;
	DWORD m_dwEdgeMask;
	int	  m_nErrorCounter;

};

#endif // !defined(AFX_CIPCINPUTVIRTUALALARM_H__B4B012D4_A633_11D2_8CA9_004005A11E32__INCLUDED_)
