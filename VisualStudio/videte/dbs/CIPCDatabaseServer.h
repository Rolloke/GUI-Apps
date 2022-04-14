// CIPCDatabaseServer.h: interface for the CIPCDatabaseServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CIPCDATABASESERVER_H__AB5B2379_3834_11D2_B58E_00C095EC9EFA__INCLUDED_)
#define AFX_CIPCDATABASESERVER_H__AB5B2379_3834_11D2_B58E_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CClient;

#include "Sequence.h"

class CIPCDatabaseServer : public CIPCDatabase  
{
	// construction/destruction
public:
	CIPCDatabaseServer(CClient* pClient, LPCTSTR szShmName, CPermission* pPermission);
	virtual ~CIPCDatabaseServer();

	// attributes
public:
	DWORD  GetVersion();
	BOOL   IsAllowed(WORD wArchivNr);
	BOOL   IsDataAllowed(WORD wArchivNr);
	BOOL   IsDeleteAllowed(WORD wArchivNr);

	inline BOOL GetArchivesRequests() const;
	inline BOOL GetAlarmListRequests() const;
	inline BOOL GetDriveRequests() const;
	inline BOOL GetFieldRequests() const;

	// operations
public:
	void   SetLastConfirm(DWORD dwRecordNr);

	// overrides	
public:	
	// version stuff
	virtual void OnRequestVersionInfo(WORD wGroupID);
	virtual void OnConfirmVersionInfo(WORD wGroupID, DWORD dwVersion);

	// archiv info
	virtual void OnRequestInfo();
	virtual	void OnRequestAlarmListArchives();
	virtual void OnRequestBackupInfo();
	virtual	void OnRequestSequenceList(WORD wArchivNr);

	// field info
	virtual void OnRequestFieldInfo();
	
	// sequence operations
	virtual	void    OnRequestRecordNr(WORD  wArchivNr, 
									  WORD  wSequenceNr, 
									  DWORD dwCurrentRecordNr,
									  DWORD dwNewRecordNr,
									  DWORD dwCamID);
	virtual	void    OnRequestCameraNames(WORD wArchivNr,DWORD dwUserData);

	// delete a sequence
	virtual void	OnRequestDeleteSequence(WORD wArchivNr, WORD wSequenceNr);

	// delete an archive, for backup only
	virtual	void	OnRequestDeleteArchiv(WORD wArchivNr);

	// storing stuff
	virtual void	OnRequestSavePicture(	int numArchives,
											const BYTE archiveIDs[],
											const CIPCPictureRecord &data);

	virtual void	OnRequestSavePictureForHost(
							CSecID	hostID,
							const	CIPCPictureRecord &data,
							BOOL	bIsAlarmConnection,
							BOOL	bIsSearchResult
							);
	virtual	void	OnRequestNewSavePicture(WORD wArchivNr1,
											WORD wArchivNr2,
			 								const CIPCPictureRecord &pict,
											int iNumRecords,
											const CIPCField fields[]);
	virtual	void	OnRequestNewSavePictureForHost(CSecID hostID,
												   BOOL	bIsAlarmConnection,
												   BOOL	bIsSearchResult,
			 									   const CIPCPictureRecord &pict,
												   int iNumRecords,
												   const CIPCField fields[]);
	virtual	void	OnRequestSaveMedia(	WORD wArchiveNr,
										CSecID hostID,
										BOOL	bIsAlarmConnection,
										BOOL	bIsSearchResult,
			 							const CIPCMediaSampleRecord &media,
										int iNumRecords,
										const CIPCField fields[]);

	virtual	void	OnRequestSaveData(WORD wArchivNr,
									int iNumRecords,
									const CIPCField fields[]);
	// query stuff
	virtual void	OnRequestQuery(DWORD dwUserID, const CString& sQuery);
	virtual void	OnResponseQueryResult(DWORD dwUserID);

	virtual	void	OnRequestNewQuery(DWORD dwUserID,
									  int iNumArchives,
									  const WORD archives[],
									  int iNumFields,
									  const CIPCField fields[],
									  DWORD dwMaxQueryResults=0);
	// disconnection
	virtual void	OnRequestDisconnect();

	virtual	void OnRequestDrives();

	virtual	void OnRequestBackup(DWORD dwUserData,
								 const CString& sDestinationPath,
								 const CString& sName,
								 int iNumIDs,
								 const DWORD dwIDs[]);
	virtual	void OnRequestBackupByTime(WORD  wUserData,
									   const CString& sDestinationPath,
									   const CString& sName,
									   int iNumIDs,
									   const DWORD dwIDs[],
									   const CSystemTime& start,
									   const CSystemTime& end,
									   WORD  wFlags,
									   DWORD dwMaximumInMB);
	virtual	void OnRequestCancelBackup(DWORD dwUserData);
	virtual void OnRequestGetValue(CSecID id,
								   const CString &sKey,
								   DWORD dwServerData);
	virtual	void	OnRequestAlarmListRecord(WORD  wArchivNr, 
											 WORD  wSequenceNr, 
											 DWORD dwRecordNr);
	virtual	void	OnRequestRecords(WORD  wArchivNr, 
									 WORD  wSequenceNr, 
									 DWORD dwFirstRecordNr, // 0 for all
									 DWORD dwLastRecordNr); // 0 for all

	virtual void OnRequestOpenSequence(WORD wArchiveNr, WORD wSequenceNr);
	virtual	void OnRequestCloseSequence(WORD wArchiveNr, WORD wSequenceNr);

public:
	void SequenceToSequenceRecord(const CSequence& seq, CIPCSequenceRecord& rec);
	void ArchiveToArchiveRecord(CArchiv& arc, CIPCArchivRecord& rec);

protected:
	CSecID GetAndCreateArchiveForHost(CSecID hostID,
									  BOOL	bIsAlarmConnection,
									  BOOL	bIsSearchResult);

	// data member
private:
	CPermission* m_pPermission;
	CClient*	 m_pClient;
	DWORD		 m_dwClientVersion;

	BOOL			m_bArchiveRequests;
	BOOL			m_bAlarmListRequests;
	BOOL			m_bDriveRequests;
	BOOL			m_bFieldRequests;

	WORD		 m_wLastRequestArchiveNr;
	WORD		 m_wLastRequestSequenceNr;
	DWORD		 m_dwLastRequestRecordNr;

};
//////////////////////////////////////////////////////////////////////////
inline BOOL CIPCDatabaseServer::GetArchivesRequests() const
{
	return m_bArchiveRequests;
}
//////////////////////////////////////////////////////////////////////////
inline BOOL CIPCDatabaseServer::GetAlarmListRequests() const
{
	return m_bAlarmListRequests;
}
//////////////////////////////////////////////////////////////////////////
inline BOOL CIPCDatabaseServer::GetDriveRequests() const
{
	return m_bDriveRequests;
}
//////////////////////////////////////////////////////////////////////////
inline BOOL CIPCDatabaseServer::GetFieldRequests() const
{
	return m_bFieldRequests;
}

#endif // !defined(AFX_CIPCDATABASESERVER_H__AB5B2379_3834_11D2_B58E_00C095EC9EFA__INCLUDED_)
