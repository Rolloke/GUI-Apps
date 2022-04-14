// IPCDatabaseAlarmlist.h: interface for the CIPCDatabaseClientAlarmlist class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPCDATABASEALARMLIST_H__DF11BA55_02F4_4AE9_B873_64A7C5AC78EB__INCLUDED_)
#define AFX_IPCDATABASEALARMLIST_H__DF11BA55_02F4_4AE9_B873_64A7C5AC78EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CServerAlarmlist;
class CIPCDatabaseClientAlarmlist  : public CIPCDatabase  
{
	// construction/destruction
public:
	CIPCDatabaseClientAlarmlist(const char* shmName, CServerAlarmlist* pServer);
	virtual ~CIPCDatabaseClientAlarmlist();

public:
	inline CObList& GetAlarmObjectList();

public:
	CSystemTime GetFirstAlarmTime();
	void RequestAlarmListArchives(CSystemTime time);
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
	CCriticalSection	m_csList;
	CServerAlarmlist*	m_pServer;
	CIPCArchivRecords	m_Archives;
	CIPCSequenceRecords m_Sequences;
	CSystemTime			m_AlarmTime;
	CObList				m_AlarmObjectList;

	DWORD				m_dwNumberOfArchives;
	DWORD				m_dwNumberOfRequests;
protected:
	void DeleteAlarmObjectList();
};

///////////////////////////////////////////////////////////////////////////////////
inline CObList& CIPCDatabaseClientAlarmlist::GetAlarmObjectList() 
{ 
	return m_AlarmObjectList;
}

#endif // !defined(AFX_IPCDATABASEALARMLIST_H__DF11BA55_02F4_4AE9_B873_64A7C5AC78EB__INCLUDED_)
