// IPCDatabaseAlarmlist.h: interface for the CIPCDatabaseClientAlarmlist class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPCDATABASEALARMLIST_H__DF11BA55_02F4_4AE9_B873_64A7C5AC78EB__INCLUDED_)
#define AFX_IPCDATABASEALARMLIST_H__DF11BA55_02F4_4AE9_B873_64A7C5AC78EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AlarmObject.h"

class CServerAlarmlist;

class CIPCDatabaseClientAlarmlist  : public CIPCDatabase  
{
	// construction/destruction
public:
	CIPCDatabaseClientAlarmlist(LPCTSTR shmName, CServerAlarmlist* pServer);
	virtual ~CIPCDatabaseClientAlarmlist();

public:
	inline CAlarmObjectListCS&	GetAlarmObjectList();
	inline CSystemTime			GetRequestedAlarmTime();

public:
	CSystemTime		GetFirstAlarmTime();
	void			RequestAlarmListArchives(CSystemTime time);

	virtual void OnReadChannelFound();
	virtual void OnConfirmConnection();
	virtual void OnRequestDisconnect();
	virtual	void OnConfirmAlarmListArchives(int iNumRecords, 
											   const CIPCArchivRecord data[]);
	
	virtual	void OnConfirmSequenceList(WORD wArchivNr, int iNumRecords, 
										  const CIPCSequenceRecord data[]);
	
	virtual	void OnConfirmRecords(WORD  wArchivNr, 
									 WORD  wSequenceNr, 
									 CIPCFields fields,
									 CIPCFields records);

	virtual	void OnIndicateNewArchiv(const CIPCArchivRecord& data);
	virtual	void OnIndicateRemoveArchiv(WORD wArchivNr);
	virtual void OnIndicateNewSequence(const CIPCSequenceRecord& data);
	virtual void OnIndicateDeleteSequence(WORD wArchivNr, WORD wSequenceNr);

	virtual void OnIndicateError(DWORD dwCmd, 
							     CSecID id, 
								 CIPCError error, 
								 int iErrorCode,
								 const CString &sErrorMessage);
private:
	CServerAlarmlist*	m_pServer;
	CIPCArchivRecords	m_Archives;
	CIPCSequenceRecords m_Sequences;
	CSystemTime			m_AlarmTime;
	CAlarmObjectListCS	m_AlarmObjectList;

	DWORD				m_dwNumberOfArchives;
	DWORD				m_dwNumberOfRequests;
};

///////////////////////////////////////////////////////////////////////////////////
inline CAlarmObjectListCS& CIPCDatabaseClientAlarmlist::GetAlarmObjectList()
{ 
	return m_AlarmObjectList;
}
///////////////////////////////////////////////////////////////////////////////////
inline CSystemTime CIPCDatabaseClientAlarmlist::GetRequestedAlarmTime()
{ 
	return m_AlarmTime;
}

#endif // !defined(AFX_IPCDATABASEALARMLIST_H__DF11BA55_02F4_4AE9_B873_64A7C5AC78EB__INCLUDED_)
