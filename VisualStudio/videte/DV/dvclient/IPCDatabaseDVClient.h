// IPCDatabaseDVClient.h: interface for the CIPCDatabaseDVClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPCDATABASEDVCLIENT_H__6B5BBEE1_9757_11D3_A870_004005A19028__INCLUDED_)
#define AFX_IPCDATABASEDVCLIENT_H__6B5BBEE1_9757_11D3_A870_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CServer;

class CIPCDatabaseDVClient : public CIPCDatabaseClient  
{
	// construction / destruction
public:
	CIPCDatabaseDVClient(LPCTSTR shmName, CServer* pServer);
	virtual ~CIPCDatabaseDVClient();

	// attributes
public:
	CIPCArchivRecord* GetArchive(CSecID idArchive);
	CIPCDrive* GetBackupDrive();

	// overrides
public:
	virtual void OnConfirmConnection();
	virtual void OnConfirmInfo(int iNumRecords, const CIPCArchivRecord data[]);
	virtual void OnConfirmVersionInfo(WORD wGroupID, DWORD dwVersion);
	virtual void OnRequestVersionInfo(WORD wGroupID);
	virtual void OnRequestDisconnect();

	virtual	void OnIndicateNewArchiv(const CIPCArchivRecord& data);
	virtual	void OnIndicateRemoveArchiv(WORD wArchivNr);
	virtual	void OnConfirmSequenceList(WORD wArchivNr, int iNumRecords, 
									   const CIPCSequenceRecord data[]);
	virtual void OnIndicateDeleteSequence(WORD wArchivNr, WORD wSequenceNr);
	virtual void OnIndicateNewSequence(const CIPCSequenceRecord& data,
										WORD  wPrevSequenceNr,
										DWORD dwNrOfRecords);

	virtual	void OnConfirmRecordNr(WORD wArchivNr, WORD wSequenceNr, 
									  DWORD dwRecordNr,
									  DWORD dwNrOfRecords,
									  const CIPCPictureRecord &pict,
									  int iNumFields,
									  const CIPCField fields[]);
	virtual	void OnConfirmRecordNr(WORD wArchivNr, 
									  WORD wSequenceNr, 
									  DWORD dwRecordNr,
									  DWORD dwNrOfRecords,
									  const CIPCMediaSampleRecord &media,
									  int iNumFields,
									  const CIPCField fields[]);
	virtual void OnIndicateError(DWORD dwCmd, CSecID id, 
								 CIPCError error, int iErrorCode,
								 const CString &sErrorMessage);
	virtual	void OnIndicateNewQueryResult(DWORD dwUserID,
											 WORD wArchivNr,
											 WORD wSequenceNr,
											 DWORD wRecordNr,
											 int iNumFields,
											 const CIPCField fields[]);
	virtual void OnConfirmQuery(DWORD dwUserID);

	virtual void OnConfirmGetValue(CSecID id,
									const CString &sKey,
									const CString &sValue,
									DWORD dwServerData);

	virtual	void OnConfirmBackup(DWORD dwUserData,
								 const CString& sDestinationPath,
								 DWORD dwID);
	virtual	void OnConfirmDrives(int iNumDrives,
								 const CIPCDrive drives[]);
	
	virtual void OnConfirmBackupByTime(WORD wUserData,
									   DWORD dwID,
									   const CSystemTime& start,
									   const CSystemTime& end,
									   WORD wFlags,
									   CIPCInt64 i64Size);
	virtual void OnConfirmCancelBackup(DWORD dwUserData);
	// data member
private:
	CServer*	m_pServer;
	CIPCDrives	m_Drives;
	BOOL		m_bBackupIgnoreConfirmQuery; 
};

#endif // !defined(AFX_IPCDATABASEDVCLIENT_H__6B5BBEE1_9757_11D3_A870_004005A19028__INCLUDED_)
