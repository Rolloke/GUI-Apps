// IPCDatabaseDBCheck.h: interface for the CIPCDatabaseDBCheck class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPCDATABASEDBCHECK_H__28E7B271_CFD5_40F0_999D_AD558AE67019__INCLUDED_)
#define AFX_IPCDATABASEDBCHECK_H__28E7B271_CFD5_40F0_999D_AD558AE67019__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CServer;

class CIPCDatabaseDBCheck : public CIPCDatabaseClient  
{
public:
	CIPCDatabaseDBCheck(const char*, CServer*, CSecID);
	virtual ~CIPCDatabaseDBCheck();

	// attributes
public:
	inline BOOL		GotInfo() const;
	inline CServer* GetServer() const;
	inline DWORD	GetVersion() const;
		   CString	GetArchivName(WORD wArchiv);
		   CIPCArchivRecord* GetArchiv(WORD bArchiv);

	// operations
public:
	inline void	ClearGotInfo();
		   void ActualizeArchivInfo();
		   void	Lock();
		   void	Unlock();
         bool  IsLocked();

	// overrides
public:
	CCriticalSection& GetCriticalSection();
//	virtual void OnConfirmConnection();
	// value confirms
//	virtual void OnConfirmGetValue(CSecID id, const CString &sKey, const CString &sValue, DWORD dwServerData);
   virtual void OnConfirmInfo(int iNumRecords, const CIPCArchivRecord data[]);
   virtual void OnConfirmAlarmListArchives(int iNumRecords, const CIPCArchivRecord data[]);
   virtual void OnConfirmFieldInfo(int iNumRecords, const CIPCField data[]);
   virtual void OnConfirmSequenceList(WORD wArchivNr, int iNumRecords, const CIPCSequenceRecord data[]);
   virtual void OnConfirmRecords(WORD wArchivNr,WORD wSequenceNr,CIPCFields fields,CIPCFields records);
   virtual void OnConfirmRecordNr(WORD wArchivNr,WORD wSequenceNr,DWORD dwRecordNr,DWORD dwNrOfRecords,const CIPCPictureRecord &pict,int iNumFields,const CIPCField fields[]);

   virtual void OnRequestRecords(WORD  wArchivNr,WORD wSequenceNr,DWORD dwFirstRecordNr, DWORD dwLastRecordNr);

//	virtual void OnIndicateArchivFile(int iNumRecords, const CIPCArchivFileRecord data[]);
//	virtual void OnIndicateRemoveFile(const CIPCArchivFileRecord &data);
//	virtual void OnIndicateDeleteSequence(WORD wArchivNr, WORD wSequenceNr);
//	virtual void OnIndicateNewSequence(const CIPCSequenceRecord& data);
//	virtual void OnConfirmOpenFile(const CIPCArchivFileRecord &data);
//	virtual void OnConfirmRecNo(const CIPCArchivFileRecord &data, const CIPCPictureRecord &pictData);
//	virtual	void OnConfirmCameraNames(WORD wArchivNr,
//									  DWORD dwUserData,
//									  int iNumFields,
//									  const CIPCField fields[]);
//	virtual void OnIndicateFileClosed(const CIPCArchivFileRecord &Data){};
   virtual void OnIndicateError(DWORD dwCmd, 
								CSecID id, 
								CIPCError error,
								int iErrorCode,
								const CString &sErrorMessage);

//	virtual void OnConfirmQuery(DWORD dwUserID);
//	virtual void OnIndicateQueryResult(DWORD dwUserID, const CIPCPictureRecord &pictData);
//	virtual	void OnIndicateNewQueryResult(DWORD dwUserID,
//										  WORD wArchivNr,
//										  WORD wSequenceNr,
//										  DWORD wRecordNr,
//										  int iNumFields,
//										  const CIPCField fields[]);
//	virtual void OnIndicateNewArchiv(const CIPCArchivRecord& data);
//	virtual	void OnIndicateRemoveArchiv(WORD wArchivNr);
//	virtual void OnRequestVersionInfo(WORD wGroupID);
//	virtual void OnConfirmVersionInfo(WORD wGroupID, DWORD dwVersion);
//	virtual void OnRequestDisconnect();

	virtual	void OnConfirmDrives(int iNumDrives, const CIPCDrive drives[]);
//	virtual	void OnConfirmBackup(DWORD dwUserData, const CString& sDestinationPath, DWORD dwID);
//	virtual	void OnConfirmCancelBackup(DWORD dwUserData);
//	virtual	void	OnConfirmRecords(WORD  wArchivNr, WORD  wSequenceNr, CIPCFields fields, CIPCFields records);

	// private data
private:
	CServer*		m_pServer;
	CSecID		m_ID;
	BOOL			m_bGotInfo;
//	BOOL			m_bActualizing;
//	DWORD			m_dwLastActualizing;
	DWORD			m_dwServerVersion;

   CCriticalSection m_cs;
   volatile bool    m_bLocked;
};

inline CServer* CIPCDatabaseDBCheck::GetServer() const
{
	return m_pServer;
}
/////////////////////////////////////////////////////////////////////
inline void	CIPCDatabaseDBCheck::ClearGotInfo()
{
	m_bGotInfo = FALSE;
}
/////////////////////////////////////////////////////////////////////
inline BOOL CIPCDatabaseDBCheck::GotInfo() const
{
	return m_bGotInfo;
}
/////////////////////////////////////////////////////////////////////
inline DWORD CIPCDatabaseDBCheck::GetVersion() const
{
	return m_dwServerVersion;
}

#endif // !defined(AFX_IPCDATABASEDBCHECK_H__28E7B271_CFD5_40F0_999D_AD558AE67019__INCLUDED_)
