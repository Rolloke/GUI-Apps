/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CIPCPipeIsdn.h $
// ARCHIVE:		$Archive: /Project/Capi/ISDNUnit/CIPCPipeIsdn.h $
// CHECKIN:		$Date: 27.06.03 9:05 $
// VERSION:		$Revision: 36 $
// LAST CHANGE:	$Modtime: 26.06.03 13:30 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef CIPCPipeISDN_H
#define CIPCPipeISDN_H

#include "CIPC.h"
#include "SecID.h"

/////////////////////////////////////////////////////////////////////////////
#include "CIPCType.h"

class CISDNConnection;
class CIPC;

class CIPCPipeISDN
{
public:
	CIPCPipeISDN(CIPCType t,
				CISDNConnection* pConnection,
				LPCTSTR pSMName,
				BOOL bIsMaster,
				DWORD dwOptions,
				WORD wID,
				DWORD dwRequestCounter,
				BOOL bIsAlarm=FALSE
				);
	virtual ~CIPCPipeISDN();
	virtual CIPC *GetCIPC();	// non-const !
	// access:
	inline CSecID GetID() const;
	inline BYTE GetIDByte() const;
	inline DWORD GetOptions() const;
	inline CIPCType GetCIPCType() const;

	inline BOOL		IsTimedOut() const;
	inline void		SetNotConnectedTimeout(DWORD dwTimeout);

	inline CSecID	GetRemoteID() const;
	inline void		SetRemoteID(CSecID newID);

	inline DWORD GetRequestCounter() const;
	//
	static BYTE GetNextGlobalID();
protected:
	BOOL	HandleVirtualAlarm(BOOL bConnect);
	BOOL	IsAlarm()const {return m_bIsAlarm;};
	
	CISDNConnection*	m_pConnection;
private:
	CSecID		m_ID;
	CSecID		m_remoteID;
	DWORD		m_dwOptions;
	CIPCType	m_cipcType;	// set in constructor
	DWORD		m_dwCounter;
	DWORD		m_dwRequestCounter;
	BOOL		m_bIsAlarm;

	DWORD		m_dwStarttimeForNotConnectedTimeout;
	DWORD		m_dwNotConnectedTimeout;
	

	static BYTE m_byGlobalIDCounter;
	static int m_nOutgoingCall;
	static int m_nIncomingCall;


};	// end of CIPCPipeISDN

inline DWORD CIPCPipeISDN::GetRequestCounter() const
{
	return m_dwRequestCounter;
}
/////////////////////////////////////////////////////////////////////////////
inline CSecID CIPCPipeISDN::GetID() const
{
	return m_ID;
}
inline BYTE  CIPCPipeISDN::GetIDByte() const
{
	return (BYTE)(m_ID.GetID() & 255);
}
/////////////////////////////////////////////////////////////////////////////
inline DWORD CIPCPipeISDN::GetOptions() const
{
	return (m_dwOptions);
}
/////////////////////////////////////////////////////////////////////////////
inline CIPCType CIPCPipeISDN::GetCIPCType() const
{
	return (m_cipcType);
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CIPCPipeISDN::IsTimedOut() const
{
	return (GetTimeSpan(m_dwStarttimeForNotConnectedTimeout) >= m_dwNotConnectedTimeout);
}
/////////////////////////////////////////////////////////////////////////////
inline void CIPCPipeISDN::SetNotConnectedTimeout(DWORD dwTimeout)
{
	m_dwStarttimeForNotConnectedTimeout = GetTickCount();
	m_dwNotConnectedTimeout = dwTimeout;
}
/////////////////////////////////////////////////////////////////////////////
inline CSecID CIPCPipeISDN::GetRemoteID() const
{
	return m_remoteID;
}
/////////////////////////////////////////////////////////////////////////////
inline void CIPCPipeISDN::SetRemoteID(CSecID newID)
{
//	WK_TRACE("SetRemoteID OwnID %08x, RemoteID old %08x new %08x\n",
//			m_ID.GetID(), m_remoteID.GetID(), newID.GetID());
	m_remoteID = newID;
}
/////////////////////////////////////////////////////////////////////////////
// **************************************************************************
/////////////////////////////////////////////////////////////////////////////
typedef CIPCPipeISDN* CIPCPipeISDNPtr;
/////////////////////////////////////////////////////////////////////////////
WK_PTR_ARRAY_CS(CIPCPipesArray,CIPCPipeISDNPtr,CIPCPipesArrayWithCS);
/////////////////////////////////////////////////////////////////////////////
class CIPCPipesArrayCS : public CIPCPipesArrayWithCS
{
public:
	CIPCPipeISDN *GetPipeByID(CSecID id) const;
};
/////////////////////////////////////////////////////////////////////////////
#endif	// CIPCPipeIST_H
