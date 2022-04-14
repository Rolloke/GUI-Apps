/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CIPCDatabase.h $
// ARCHIVE:		$Archive: /Project/CIPC/CIPCDatabase.h $
// CHECKIN:		$Date: 9.11.05 9:29 $
// VERSION:		$Revision: 82 $
// LAST CHANGE:	$Modtime: 9.11.05 9:09 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef __CIPCDatabase_H__
#define __CIPCDatabase_H__

#include "CIPC.h"

#include "ArchivDefines.h"
#include "CIPCArchivRecord.h"
#include "CIPCSequenceRecord.h"
#include "CIPCField.h"
#include "CIPCDrives.h"
#include "MediaSampleRecord.h"

/////////////////////////////////////////////////////////////////////////////
// 
class CIPCPictureRecord;

//  only calculate the backup
#define BBT_CALCULATE		0x01
//  directly execute the backup
#define BBT_EXECUTE			0x02
//  backup of sequence is in progress
#define BBT_PROGRESS		0x04
//  backup of sequence is finished
#define BBT_FINISHED		0x08
//  only calculate the backup
#define BBT_CALCULATE_END	0x10
//  directly execute an automatic partly backup from an archive
#define BBT_EXECUTE_AUTO	0x20
/////////////////////////////////////////////////////////////////////////////
//  database error code , client tried to get archive list during database reset
#define DBEC_ARCHIVE_DURING_RESET			0
//  database error code , client denied to delete a sequence
#define DBEC_DELETE_SEQUENCE_DENIED			1
//  database error code , failed to delete archive
#define DBEC_ARCHIVE_DELETE_FAILED			1
//  database error code , client denied to delete archive
#define DBEC_ARCHIVE_DELETE_DENIED			2
//  database error code , client tried to get a record but sequence No is invalid
#define DBEC_RECORD_NR_NO_SEQUENCE			3
//  database error code , client tried to get a record but archive No is invalid
#define DBEC_RECORD_NR_NO_ARCHIVE			4
//  database error code , client tried to get a sequence list during database reset
#define DBEC_SEQUENCE_DURING_RESET			4
//  database error code , client tried to save during database reset
#define DBEC_SAVE_DURING_RESET				6
//  database error code , client tried to get a record during database reset
#define DBEC_RECORD_NR_DURING_RESET			11
//  database error code , client tried to get camera names during database reset
#define DBEC_CAMERA_NAMES_DURING_RESET		12
//  database error code , client tried to query during database reset
#define DBEC_QUERY_DURING_RESET				16
//  database error code , client tried to delete a sequence during database reset
#define DBEC_DELETE_SEQUENCE_DURING_RESET	18
//  database error code , client tried to delete an archive during database reset
#define DBEC_DELETE_ARCHIVE_DURING_RESET	19
//  database error code , client tried to get drives during database reset
#define DBEC_DRIVES_DURING_RESET			20
//  database error code , client tried to backup during database reset
#define DBEC_BACKUP_DURING_RESET			21
//  database error code , client tried to get field info during database reset
#define DBEC_FIELD_INFO_DURING_RESET		23
/////////////////////////////////////////////////////////////////////////////
//  CIPCDatabase | Communication class for database connections.
// Database connections playback functionality, queries and backup
// of images and data
// Class is used for Client/Server communication only
// public | CIPC
//  <c CIPC>
///////////////////////////////////////////////////////////////////////////////////
// CIPCDatabase 
///////////////////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CIPCDatabase : public CIPC
{
	//  construction / destruction
public:
	//!ic! constructor
	CIPCDatabase(LPCTSTR shmName, BOOL bMaster);

	//  Client/Server initial protocol
public:
	//!ic! client does requests the archive list V3.0-
			void	DoRequestInfo();
	//!ic! server gets request for archive list V3.0-
	virtual void	OnRequestInfo();
	//!ic! server does confirm the archive list V3.0-
			void	DoConfirmInfo(int iNumRecords, const CIPCArchivRecord data[]);
	//!ic! client gets confirmed archive list V3.0-
	virtual void	OnConfirmInfo(int iNumRecords, const CIPCArchivRecord data[]);

	//!ic! nyd V4.0-
			void	DoRequestFieldInfo();
	//!ic! nyd V4.0-
	virtual void	OnRequestFieldInfo();
	//!ic! nyd V4.0-
			void	DoConfirmFieldInfo(const CIPCFields& fields);
	//!ic! nyd V4.0-
	virtual void	OnConfirmFieldInfo(int iNumRecords, const CIPCField data[]);

	// DTS only V 1.4
	//!ic! nyd DTS 1.4 nyi
			void	DoRequestAlarmListArchives();
	//!ic! nyd DTS 1.4 nyi
	virtual	void	OnRequestAlarmListArchives();
	//!ic! nyd DTS 1.4 nyi
			void	DoConfirmAlarmListArchives(int iNumRecords, 
											   const CIPCArchivRecord data[]);
	//!ic! nyd DTS 1.4 nyi
	virtual	void	OnConfirmAlarmListArchives(int iNumRecords, 
											   const CIPCArchivRecord data[]);

	//  3.5x and 3.6x backup archive protocol
public:
	//!ic! nyd V3.5 V3.6
			void	DoRequestBackupInfo();
	//!ic! nyd V3.5 V3.6
	virtual void	OnRequestBackupInfo();
	//!ic! nyd V3.5 V3.6
			void	DoConfirmBackupInfo(int iNumRecords, const CIPCArchivRecord data[]);
	//!ic! nyd V3.5 V3.6
	virtual void	OnConfirmBackupInfo(int iNumRecords, const CIPCArchivRecord data[]);

	//  save commands V3.0 - 3.6x
public:
	//!ic! nyd V3.0-V3.6
			void	DoRequestSavePicture(int numArchives,
										 const BYTE archiveIDs[],
										 const CIPCPictureRecord &data);
	//!ic! nyd V3.0-V3.6
	virtual void	OnRequestSavePicture(int numArchives,
										 const BYTE archiveIDs[],
										 const CIPCPictureRecord &data);
	//!ic! nyd V3.0-V3.6
			void	DoRequestSavePictureForHost(CSecID	hostID,
												const	CIPCPictureRecord &data,
												BOOL	bIsAlarmConnection,
												BOOL	bIsSearchResult);
	//!ic! nyd V3.0-V3.6
	virtual void	OnRequestSavePictureForHost(CSecID	hostID,
												const	CIPCPictureRecord &data,
												BOOL	bIsAlarmConnection,
												BOOL	bIsSearchResult);

	//  save commands V4.0 - 
public:
	//!ic! nyd V4.0-
			void	DoRequestNewSavePicture(WORD wArchivNr1,
											WORD wArchivNr2,
			 								const CIPCPictureRecord &pict,
											const CIPCFields& fields);
	//!ic! nyd V4.0-
	virtual	void	OnRequestNewSavePicture(WORD wArchivNr1,
											WORD wArchivNr2,
			 							    const CIPCPictureRecord &pict,
										    int iNumRecords,
										    const CIPCField fields[]);
	//!ic! nyd V4.0-
			void	DoConfirmNewSavePicture(WORD wArchivNr,
											CSecID camID);
	//!ic! nyd V4.0-
	virtual	void	OnConfirmNewSavePicture(WORD wArchivNr,
											CSecID camID);
	//!ic! nyd V4.0-
			void	DoRequestNewSavePictureForHost(CSecID hostID,
												   BOOL	bIsAlarmConnection,
												   BOOL	bIsSearchResult,
			 									   const CIPCPictureRecord &pict,
												   const CIPCFields& fields);
	//!ic! nyd V4.0-
	virtual	void	OnRequestNewSavePictureForHost(CSecID hostID,
												   BOOL	bIsAlarmConnection,
												   BOOL	bIsSearchResult,
			 									   const CIPCPictureRecord &pict,
												   int iNumRecords,
												   const CIPCField fields[]);
	//!ic! nyd DTS 1.4 nyi
			void	DoRequestSaveData(WORD wArchivNr,
								      const CIPCFields& fields);
	//!ic! nyd DTS 1.4 nyi
	virtual	void	OnRequestSaveData(WORD wArchivNr,
									  int iNumRecords,
									  const CIPCField fields[]);

	// audio save
			void	DoRequestSaveMedia(WORD wArchiveNr,
									   CSecID hostID,
									   BOOL	bIsAlarmConnection,
									   BOOL	bIsSearchResult,
			 						   const CIPCMediaSampleRecord &media,
									   const CIPCFields& fields);
	//!ic! nyd V4.0-
	virtual	void	OnRequestSaveMedia(	WORD wArchiveNr,
										CSecID hostID,
										BOOL	bIsAlarmConnection,
										BOOL	bIsSearchResult,
			 							const CIPCMediaSampleRecord &media,
										int iNumRecords,
										const CIPCField fields[]);

	//  Client/Server sequence protocol 4.0
public:	
	//!ic! nyd V4.0 -
			void	DoRequestSequenceList(WORD wArchivNr);
	//!ic! nyd V4.0 -
	virtual	void	OnRequestSequenceList(WORD wArchivNr);

			void	DoConfirmSequenceList(WORD wArchivNr, int iNumRecords, 
										  const CIPCSequenceRecord data[]);

	virtual	void	OnConfirmSequenceList(WORD wArchivNr, int iNumRecords, 
										  const CIPCSequenceRecord data[]);
	//!ic! nyd V4.0 -
			void	DoIndicateNewSequence(const CIPCSequenceRecord& data,
										  WORD  wPrevSequenceNr,
										  DWORD dwNrOfRecords);
	//!ic! nyd V4.0 -
	virtual void	OnIndicateNewSequence(const CIPCSequenceRecord& data,
										  WORD  wPrevSequenceNr,
										  DWORD dwNrOfRecords);


	//  data access protocol 4.0-
public:

			void    DoRequestRecordNr(WORD  wArchivNr, 
									  WORD  wSequenceNr, 
									  DWORD dwCurrentRecordNr,
									  DWORD dwNewRecordNr,
									  DWORD dwCamID);
	//!ic! nyd 4.0 -
	virtual	void    OnRequestRecordNr(WORD  wArchivNr, 
									  WORD  wSequenceNr, 
									  DWORD dwCurrentRecordNr,
									  DWORD dwNewRecordNr,
									  DWORD dwCamID);
	//!ic! nyd 4.0 -
	// video confirms
			void    DoConfirmRecordNr(WORD wArchivNr, 
									  WORD wSequenceNr, 
									  DWORD dwRecordNr,
									  DWORD dwNrOfRecords,
									  const CIPCPictureRecord &pict,
									  const CIPCFields& fields);

	virtual	void    OnConfirmRecordNr(WORD wArchivNr, 
									  WORD wSequenceNr, 
									  DWORD dwRecordNr,
									  DWORD dwNrOfRecords,
									  const CIPCPictureRecord &pict,
									  int iNumFields,
									  const CIPCField fields[]);
	// !ic! audio confirms
			void    DoConfirmRecordNr(WORD wArchivNr, 
									  WORD wSequenceNr, 
									  DWORD dwRecordNr,
									  DWORD dwNrOfRecords,
									  const CIPCMediaSampleRecord &media,
									  const CIPCFields& fields);

	virtual	void    OnConfirmRecordNr(WORD wArchivNr, 
									  WORD wSequenceNr, 
									  DWORD dwRecordNr,
									  DWORD dwNrOfRecords,
									  const CIPCMediaSampleRecord &media,
									  int iNumFields,
									  const CIPCField fields[]);
	//!ic! nyd V 1.4 
			void	DoRequestAlarmListRecord(WORD  wArchivNr, 
											 WORD  wSequenceNr, 
											 DWORD dwRecordNr);
	//!ic! nyd V 1.4
	virtual	void	OnRequestAlarmListRecord(WORD  wArchivNr, 
											 WORD  wSequenceNr, 
											 DWORD dwRecordNr);
	//!ic! nyd V 1.4
			void    DoConfirmAlarmListRecord(WORD wArchivNr, 
											 WORD wSequenceNr, 
											 DWORD dwRecordNr,
											 DWORD dwNrOfRecords,
											 const CIPCFields& fields);
	//!ic! nyd V 1.4
	virtual	void    OnConfirmAlarmListRecord(WORD wArchivNr, 
											 WORD wSequenceNr, 
											 DWORD dwRecordNr,
											 DWORD dwNrOfRecords,
											 int iNumFields,
											 const CIPCField fields[]);

			void	DoRequestRecords(WORD  wArchivNr, 
									 WORD  wSequenceNr, 
									 DWORD dwFirstRecordNr, // 0 for all
									 DWORD dwLastRecordNr); // 0 for all
	//!ic! nyd V 1.4 nyi
	virtual	void	OnRequestRecords(WORD  wArchivNr, 
									 WORD  wSequenceNr, 
									 DWORD dwFirstRecordNr, // 0 for all
									 DWORD dwLastRecordNr); // 0 for all
	//!ic! nyd V 1.4 nyi
			void	DoConfirmRecords(WORD  wArchivNr, 
									 WORD  wSequenceNr, 
									 const CIPCFields& fields,
									 const CIPCFields& records);
	//!ic! nyd V 1.4 nyi
	virtual	void	OnConfirmRecords(WORD  wArchivNr, 
									 WORD  wSequenceNr, 
									 CIPCFields fields,
									 CIPCFields records);

	//  Query protocol V3.0-3.6x
public:
	//!ic! nyd 3.0 - 3.6x
			void	DoRequestQuery(DWORD dwUserID, const CString& sQuery);
	//!ic! nyd 3.0 - 3.6x
	virtual void	OnRequestQuery(DWORD dwUserID, const CString& sQuery);

	//!ic! nyd 3.0 - 3.6x
			void	DoIndicateQueryResult(DWORD dwUserID, 
										  const CIPCPictureRecord &pictData);
	//!ic! nyd 3.0 - 3.6x
	virtual void	OnIndicateQueryResult(DWORD dwUserID, 
										  const CIPCPictureRecord &pictData);

	//  Query confirmation protocol 3.0- 4.0 too!
	//!ic! nyd
			void	DoResponseQueryResult(DWORD dwUserID);
	//!ic! nyd
	virtual void	OnResponseQueryResult(DWORD dwUserID);

	//!ic! nyd
			void	DoConfirmQuery(DWORD dwUserID);
	//!ic! nyd
	virtual void	OnConfirmQuery(DWORD dwUserID);

	//  Query protocol V4.0-
public:
	//!ic! nyd 4.0 -
			void	DoRequestNewQuery(DWORD dwUserID,
									  int iNumArchives,
									  const WORD archives[],
									  const CIPCFields& fields,
									  DWORD dwMaxQueryResults=0);
	//!ic! nyd 4.0 -
	virtual	void	OnRequestNewQuery(DWORD dwUserID,
									  int iNumArchives,
									  const WORD archives[],
									  int iNumFields,
									  const CIPCField fields[],
									  DWORD dwMaxQueryResults);
	//!ic! nyd 4.0 -
			void	DoIndicateNewQueryResult(DWORD dwUserID,
											 WORD wArchivNr,
											 WORD wSequenceNr,
											 DWORD wRecordNr,
											 const CIPCFields& fields);

	//!ic! nyd 4.0 -
	virtual	void	OnIndicateNewQueryResult(DWORD dwUserID,
											 WORD wArchivNr,
											 WORD wSequenceNr,
											 DWORD wRecordNr,
											 int iNumFields,
											 const CIPCField fields[]);

	//  deleting sequences 
public:
	//!ic! nyd 4.0 - 
			void	DoRequestDeleteSequence(WORD wArchivNr, WORD wSequenceNr);
	//!ic! nyd 4.0 - 
	virtual void	OnRequestDeleteSequence(WORD wArchivNr, WORD wSequenceNr);
	//!ic! nyd 4.0 - 
			void	DoIndicateDeleteSequence(WORD wArchivNr, WORD wSequenceNr);
	//!ic! nyd 4.0 - 
	virtual void	OnIndicateDeleteSequence(WORD wArchivNr, WORD wSequenceNr);

	//  dynamically created and delete archives 3.0 - 4.0 
public:
	//!ic! nyd 4.0 - 
			void	DoIndicateNewArchiv(const CIPCArchivRecord& data);
	//!ic! nyd 4.0 - 
	virtual	void	OnIndicateNewArchiv(const CIPCArchivRecord& data);
	//!ic! nyd 4.0 - 
			void	DoIndicateRemoveArchiv(WORD wArchivNr);
	//!ic! nyd 4.0 - 
	virtual	void	OnIndicateRemoveArchiv(WORD wArchivNr);

	//!ic! nyd 4.0 - 
			void	DoRequestDeleteArchiv(WORD wArchivNr);
	//!ic! nyd 4.0 - 
	virtual	void	OnRequestDeleteArchiv(WORD wArchivNr);

	//  drive info 4.0
public:
	//!ic! nyd 4.0 - 
			void DoRequestDrives();
	//!ic! nyd 4.0 - 
	virtual	void OnRequestDrives();
	//!ic! nyd 4.0 - 
			void DoConfirmDrives(const CIPCDrives& drives);
	//!ic! nyd 4.0 - 
	virtual	void OnConfirmDrives(int iNumDrives,
								 const CIPCDrive drives[]);

	//  Backup
public:
	//!ic! nyd 4.0 - 
			void DoRequestBackup(DWORD dwUserData,
								 const CString& sDestinationPath,
								 const CString& sName,
								 int iNumIDs,
								 const DWORD dwIDs[]);
	//!ic! nyd 4.0 - 
	virtual	void OnRequestBackup(DWORD dwUserData,
								 const CString& sDestinationPath,
								 const CString& sName,
								 int iNumIDs,
								 const DWORD dwIDs[]);
	//!ic! nyd 4.0 - 
			void DoConfirmBackup(DWORD dwUserData,
								 const CString& sDestinationPath,
								 DWORD dwID);
	//!ic! nyd 4.0 - 
	virtual	void OnConfirmBackup(DWORD dwUserData,
								 const CString& sDestinationPath,
								 DWORD dwID);
	//!ic! nyd 4.0 - 
			void DoRequestCancelBackup(DWORD dwUserData);
	//!ic! nyd 4.0 - 
	virtual	void OnRequestCancelBackup(DWORD dwUserData);
	//!ic! nyd 4.0 - 
			void DoConfirmCancelBackup(DWORD dwUserData);
	//!ic! nyd 4.0 - 
	virtual	void OnConfirmCancelBackup(DWORD dwUserData);

	//!ic! nyd DTS 1.5
			void DoRequestBackupByTime(WORD wUserData,
									   const CString& sDestinationPath,
									   const CString& sName,
									   int iNumIDs,
									   const DWORD dwIDs[],
									   const CSystemTime& start,
									   const CSystemTime& end,
									   WORD  wFlags,
									   DWORD dwMaximumInMB=0);
	//!ic! nyd DTS 1.5
	virtual	void OnRequestBackupByTime(WORD  wUserData,
									   const CString& sDestinationPath,
									   const CString& sName,
									   int iNumIDs,
									   const DWORD dwIDs[],
									   const CSystemTime& start,
									   const CSystemTime& end,
									   WORD  wFlags,
									   DWORD dwMaximumInMB);
	//!ic! nyd DTS 1.5
			void DoConfirmBackupByTime(WORD wUserData,
									   DWORD dwID,
									   const CSystemTime& start,
									   const CSystemTime& end,
									   WORD wFlags,
									   CIPCInt64 i64Size);
	//!ic! nyd DTS 1.5
	virtual	void OnConfirmBackupByTime(WORD wUserData,
									   DWORD dwID,
									   const CSystemTime& start,
									   const CSystemTime& end,
									   WORD wFlags,
									   CIPCInt64 i64Size);

	//  camera names 4.0
public:
	//!ic! nyd 4.0 - 
			void DoRequestCameraNames(WORD wArchivNr,DWORD dwUserData);
	//!ic! nyd 4.0 - 
	virtual	void OnRequestCameraNames(WORD wArchivNr,DWORD dwUserData);
	//!ic! nyd 4.0 - 
			void DoConfirmCameraNames(WORD wArchivNr,
									  DWORD dwUserData,
									  const CIPCFields& fields);
	//!ic! nyd 4.0 - 
	virtual	void OnConfirmCameraNames(WORD wArchivNr,
									  DWORD dwUserData,
									  int iNumFields,
									  const CIPCField fields[]);

	//  client indicates that a sequence is open and should nor be deleted by time
public:
		    void DoRequestOpenSequence(WORD wArchiveNr, WORD wSequenceNr);
	virtual void OnRequestOpenSequence(WORD wArchiveNr, WORD wSequenceNr);
			void DoRequestCloseSequence(WORD wArchiveNr, WORD wSequenceNr);
	virtual	void OnRequestCloseSequence(WORD wArchiveNr, WORD wSequenceNr);

protected:
	virtual BOOL HandleCmd(DWORD dwCmd,
						   DWORD dwParam1, 
						   DWORD dwParam2,
						   DWORD dwParam3, 
						   DWORD dwParam4,
						   const CIPCExtraMemory *pExtraMem);

private:
	CIPCDatabase(const CIPCDatabase& src);		// no implementation, to avoid implicit generation!
	void operator=(const CIPCDatabase& src);	// no implementation, to avoid implicit generation!
};
///////////////////////////////////////////////////////////////////////////////////
// commands
#define CIPC_DB_REQUEST_INFO			12001
#define CIPC_DB_CONFIRM_INFO			12002
#define CIPC_DB_REQUEST_SAVE_PICTURE	12003

#define CIPC_DB_REQUEST_QUERY			12012
#define CIPC_DB_CONFIRM_QUERY			12013

#define CIPC_DB_INDICATE_QUERY_RESULT	12016
#define CIPC_DB_RESPONSE_QUERY_RESULT	12017

#define CIPC_DB_REQUEST_SAVE_BY_HOST	12021
#define CIPC_DB_REQUEST_BACKUP_INFO		12022
#define CIPC_DB_CONFIRM_BACKUP_INFO		12023
// new 3.6
#define CIPC_DB_INDICATE_REMOVE_ARCHIV		12031
#define CIPC_DB_INDICATE_NEW_ARCHIV			12032
// new 4.0
#define CIPC_DB_REQUEST_SEQUENCE_LIST		12033
#define CIPC_DB_CONFIRM_SEQUENCE_LIST		12034
#define CIPC_DB_REQUEST_RECORD_NR			12035
#define CIPC_DB_CONFIRM_RECORD_NR			12036
#define CIPC_DB_REQUEST_DELETE_SEQUENCE		12037
#define CIPC_DB_INDICATE_DELETE_SEQUENCE	12038
#define CIPC_DB_INDICATE_NEW_SEQUENCE		12039
#define CIPC_DB_REQUEST_FIELD_INFO			12040
#define CIPC_DB_CONFIRM_FIELD_INFO			12041
#define CIPC_DB_NEW_REQUEST_SAVE			12042
#define CIPC_DB_NEW_REQUEST_SAVE_FOR_HOST	12043
#define CIPC_DB_REQUEST_NEW_QUERY			12044
#define CIPC_DB_INDICATE_NEW_QUERY_RESULT	12045
#define CIPC_DB_REQUEST_DRIVES				12046
#define CIPC_DB_CONFIRM_DRIVES				12047
#define CIPC_DB_REQUEST_BACKUP				12048
#define CIPC_DB_CONFIRM_BACKUP				12049
#define CIPC_DB_REQUEST_CANCEL_BACKUP		12050
#define CIPC_DB_CONFIRM_CANCEL_BACKUP		12051
#define CIPC_DB_REQUEST_DELETE_ARCHIV		12052
#define CIPC_DB_REQUEST_CAMERA_NAMES		12053
#define CIPC_DB_CONFIRM_CAMERA_NAMES		12054
#define CIPC_DB_NEW_CONFIRM_SAVE			12055
#define CIPC_DB_REQUEST_BACKUP_BY_TIME		12056
#define CIPC_DB_CONFIRM_BACKUP_BY_TIME		12057
#define CIPC_DB_REQUEST_ALARM_LIST_ARCHIVES	12058
#define CIPC_DB_CONFIRM_ALARM_LIST_ARCHIVES	12059
#define CIPC_DB_REQUEST_ALARM_LIST_RECORD	12060
#define CIPC_DB_CONFIRM_ALARM_LIST_RECORD	12061
#define CIPC_DB_REQUEST_SAVE_DATA			12062
#define CIPC_DB_REQUEST_RECORDS				12063
#define CIPC_DB_CONFIRM_RECORDS				12064
#define CIPC_DB_REQUEST_OPEN_SEQUENCE		12065
#define CIPC_DB_REQUEST_CLOSE_SEQUENCE		12066
#define CIPC_DB_REQUEST_SAVE_MEDIA			12067
#define CIPC_DB_CONFIRM_RECORD_NR_MEDIA		12068
///////////////////////////////////////////////////////////////////////////////////
#endif
