/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CipcInputServer.h $
// ARCHIVE:		$Archive: /Project/SecurityServer/CipcInputServer.h $
// CHECKIN:		$Date: 30.08.06 13:02 $
// VERSION:		$Revision: 58 $
// LAST CHANGE:	$Modtime: 30.08.06 12:48 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef __INPGRP_H_
#define __INPGRP_H_

#include "IOGroup.h"
#include "input.h"

/////////////////////////////////////////////////////////////////////////////
// CIPCInputServer Class:
// Enthaelt alle Daten für eine Eingangsgruppe

// CAVEAT: multiple inheritance!

class CIPCInputServer : public CIPCInput, public CIOGroup
{
	// construction/destruction
public:
	// pName:	Der Name dieser Gruppe
	// iNr:		Die Anzahl der Eingaenge der Gruppe.
	CIPCInputServer(LPCTSTR pName, int iNr, LPCTSTR pSMName);
	virtual ~CIPCInputServer();

	// attributes
public:
	inline BOOL IsInitDone() const;	// inital protocol passed
	inline BOOL IsMD() const;
	inline BOOL IsSDI() const;
	inline BOOL IsTasha() const;
	inline BOOL IsSavic() const;
	inline BOOL IsQ() const;
	inline BOOL IsAudio() const;
	inline BOOL IsIP() const;

	//
	void StartInitialProcesses();
	// a) SECID_NO_GROUPID for all output groups
	// b) explicit output groupID
	// c) SECID_NO_ID but for explicit timer
	void StartMissingProcesses(WORD wOutputGroupID, const CSecTimer *pTimer);	
	
protected:
	// OnConfirmXXX, virtual!
	virtual void OnConfirmConnection();
	virtual void OnConfirmVersionInfo(WORD wGroupID, DWORD dwVersion);
	virtual	void OnRequestDisconnect();
	virtual void OnConfirmSetGroupID(WORD wGroupID);
	virtual void OnConfirmReset(WORD wGroupID);
	virtual void OnConfirmEdge(WORD wGroupID,DWORD edgeBitmask);	// 1 positive, 0 negative
	virtual void OnConfirmFree(WORD wGroupID,DWORD openBitmask);	// 1 open, 0 closed
	virtual void OnConfirmAlarmState(WORD wGroupID,DWORD inputMask);	// 1 high, 0 low; 1 changed, 0 unchanged
	virtual void OnConfirmAlarmReset(WORD wGroupID,DWORD resetMask);	// 1 reset done 
	virtual void OnConfirmHardware(WORD wGroupID,int iErrorCode);

	// OnIndicateXXX, virtual
	// alarm state send by input unit
	virtual void OnIndicateAlarmState(WORD wGroupID,DWORD inputMask, DWORD changeMask,
										LPCTSTR szInfoString
										);	// 1 high, 0 low; 1 changed, 0 unchanged
	virtual void OnIndicateAlarm(CSecID id, 
								 BOOL bAlarm,
								 int iNumRecords,
								 const CIPCField fields[]);
	virtual	void OnIndicateAlarmNr(CSecID id,
								   BOOL bAlarm,
								   DWORD dwData1,
								   DWORD dwData2);
	//
	virtual void OnIndicateAlarmFieldsState(WORD wGroupID,
									  DWORD inputMask, 	// 1 high, 0 low
									  DWORD changeMask,	// 1 changed, 0 unchanged
									  int iNumFields,
									  const CIPCField fields[]
									  );
	virtual void OnIndicateAlarmFieldsUpdate(CSecID id,
									   int iNumFields,
									   const CIPCField fields[]);
	//
	virtual void OnIndicateError(DWORD dwCmd, 
							     CSecID id, 
								 CIPCError error, 
								 int iErrorCode,
								 const CString &sErrorMessage);
	//
	virtual void OnReadChannelFound();
	virtual BOOL OnTimeoutCmdReceive();
	virtual BOOL OnTimeoutWrite(DWORD dwCmd);
	//
	virtual BOOL Run(DWORD dwTimeout);
	//	CommData related functions
	virtual void OnIndicateCommData(CSecID id,
									const CIPCExtraMemory &data,
									DWORD dwBlockNr
									);
	virtual void OnConfirmWriteCommData(CSecID id, DWORD dwBlockNr);

	virtual void OnConfirmGetValue(
						CSecID id, // might be used as group ID only
						const CString &sKey,
						const CString &sValue,
						DWORD dwServerData
						);
	virtual void OnConfirmSetValue(
						CSecID id, // might be used as group ID only
						const CString &sKey,
						const CString &sValue,
						DWORD dwServerData
						);

	// implementation
private:
	// functions:
	void StopProcesses(CInput *pInput, BOOL bNewState);
	void StartProcesses(WORD wGroupID,
						DWORD inputMask, 	// 1 high, 0 low
						DWORD changeMask,	// 1 changed, 0 unchanged
						const CIPCFields& fields
						);

	// data member
private:
	DWORD	m_dwConnectTime;
	BOOL	m_bInitDone;
	DWORD	m_dwFreeState;		// Freigabemaske der Eingänge, wird aus wState jedes eing. ermittelt

	BOOL m_bIsMD;
	BOOL m_bIsSDI;
	BOOL m_bTasha;
	BOOL m_bSavic;
	BOOL m_bQ;
	BOOL m_bAudio;
	BOOL m_bIP;
};
////////////////////////////////////////////////////
inline BOOL CIPCInputServer::IsInitDone() const 
{
	return m_bInitDone;
}
////////////////////////////////////////////////////
inline BOOL CIPCInputServer::IsMD() const
{
	return m_bIsMD;
}
////////////////////////////////////////////////////
inline BOOL CIPCInputServer::IsSDI() const
{
	return m_bIsSDI;
}
////////////////////////////////////////////////////
inline BOOL CIPCInputServer::IsTasha() const
{
	return m_bTasha;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CIPCInputServer::IsSavic() const
{
	return m_bSavic;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CIPCInputServer::IsAudio() const
{
	return m_bAudio;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CIPCInputServer::IsQ() const
{
	return m_bQ;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CIPCInputServer::IsIP() const
{
	return m_bIP;
}
/////////////////////////////////////////////////////////////////////////////
#endif // __INPGRP_H_
