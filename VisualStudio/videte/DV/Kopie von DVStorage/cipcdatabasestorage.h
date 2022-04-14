// CIPCDatabaseStorage.h: interface for the CIPCDatabaseStorage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CIPCDATABASESTORAGE_H)
#define AFX_CIPCDATABASESTORAGE_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CClient;

#include "Tape.h"

class CH26xEncoder;
class CMPEG4Encoder;

class CIPCDatabaseStorage : public CIPCDatabase  
{
	// construction/destruction
public:
	CIPCDatabaseStorage(CClient* pClient, LPCTSTR shmName);
	virtual ~CIPCDatabaseStorage();

	// attributes
public:
	inline BOOL GetArchivesRequests() const;
	inline BOOL GetAlarmListRequests() const;
	inline BOOL GetDriveRequests() const;

	// operations
public:
	void SetLastConfirm(DWORD dwRecordNr);
	void ConfirmRecordNr(WORD wArchivNr, 
						 WORD wSequenceNr, 
						 DWORD dwRecordNr,
						 DWORD dwNrOfRecords,
						 const CIPCPictureRecord &pict,
						 const CIPCFields& fields);

	// overrides	
public:	
	// version stuff
	virtual void OnRequestVersionInfo(WORD wGroupID);
	virtual void OnConfirmVersionInfo(WORD wGroupID, DWORD dwVersion);
	virtual void OnRequestGetValue(CSecID id,
								   const CString &sKey,
								   DWORD dwServerData);
	virtual void OnRequestSetValue(CSecID id,
								   const CString &sKey,
								   const CString &sValue,
								   DWORD dwServerData);

	// archiv info
	virtual void OnRequestInfo();
	virtual	void OnRequestAlarmListArchives();
	virtual	void OnRequestSequenceList(WORD wArchivNr);
	
	// field info
	virtual void OnRequestFieldInfo();
	virtual	void OnRequestCameraNames(WORD wArchivNr,DWORD dwUserData);

	// sequence operations
	virtual	void    OnRequestRecordNr(WORD  wArchivNr, 
									  WORD  wSequenceNr, 
									  DWORD dwCurrentRecordNr,
									  DWORD dwNewRecordNr,
									  DWORD dwCamID);
	virtual	void	OnRequestAlarmListRecord(WORD  wArchivNr, 
											 WORD  wSequenceNr, 
											 DWORD dwRecordNr);
	virtual	void	OnRequestRecords(WORD  wArchivNr, 
									 WORD  wSequenceNr, 
									 DWORD dwFirstRecordNr, // 0 for all
									 DWORD dwLastRecordNr); // 0 for all
	virtual void	OnRequestOpenSequence(WORD wArchiveNr, 
										  WORD wSequenceNr);

	// delete a sequence
	virtual void	OnRequestDeleteSequence(WORD wArchivNr, WORD wSequenceNr);
	virtual	void	OnRequestDeleteArchiv(WORD wArchivNr);
	virtual	void	OnRequestNewSavePicture(WORD wArchivNr1,
											WORD wArchivNr2,
			 								const CIPCPictureRecord &pict,
											int iNumRecords,
											const CIPCField fields[]);
	virtual	void	OnRequestSaveData(WORD wArchivNr,
									  int iNumRecords,
									  const CIPCField fields[]);
	virtual	void	OnRequestSaveMedia(	WORD wArchiveNr,
										CSecID hostID,
										BOOL	bIsAlarmConnection,
										BOOL	bIsSearchResult,
			 							const CIPCMediaSampleRecord &media,
										int iNumRecords,
										const CIPCField fields[]);

	// query stuff
	virtual void	OnResponseQueryResult(DWORD dwUserID);
	virtual	void	OnRequestNewQuery(DWORD dwUserID,
									  int iNumArchives,
									  const WORD archives[],
									  int iNumFields,
									  const CIPCField fields[],
									  DWORD dwMaxQueryResults=0);

	// backup stuff
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

	// disconnection
	virtual void	OnRequestDisconnect();

public:
	void TapeToSequenceRecord(const CTape& seq, CIPCSequenceRecord& rec);
	void CollectionToArchiveRecord(CCollection& arc, CIPCArchivRecord& rec);

	// data member
private:
	CClient*	 m_pClient;
	DWORD		 m_dwClientVersion;

	WORD		 m_wLastRequestArchiveNr;
	WORD		 m_wLastRequestSequenceNr;
	DWORD		 m_dwLastRequestRecordNr;


	WORD			m_currentArchive;
	WORD			m_currentSequence;
	DWORD			m_dwCurrentRecord;
	CJpeg*			m_pJpeg;
	CH26xEncoder*	m_pH263Encoder;
	CMPEG4Encoder*	m_pMpeg4Encoder;

	BOOL			m_bArchiveRequests;
	BOOL			m_bAlarmListRequests;
	BOOL			m_bDriveRequests;
	BOOL			m_bThreadIDTraced;
};
//////////////////////////////////////////////////////////////////////////
inline BOOL CIPCDatabaseStorage::GetArchivesRequests() const
{
    return m_bArchiveRequests;
}
//////////////////////////////////////////////////////////////////////////
inline BOOL CIPCDatabaseStorage::GetAlarmListRequests() const
{
	return m_bAlarmListRequests;
}
//////////////////////////////////////////////////////////////////////////
inline BOOL CIPCDatabaseStorage::GetDriveRequests() const
{
	return m_bDriveRequests;
}

#endif
