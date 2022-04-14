// Sequence.h: interface for the CSequence class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SEQUENCE_H__A30FED61_38DB_11D2_B58F_00C095EC9EFA__INCLUDED_)
#define AFX_SEQUENCE_H__A30FED61_38DB_11D2_B58F_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Conversion.h"
#include "SaveData.h"
#include "Backup.h"

class CArchiv;
class CQuery;
class CWin32FindData;
class CIPCDatabaseServer;

BOOL ReadDWORD(Data4& data, Code4& code_base, const char* szName,DWORD& dwValue);
BOOL AssignDWORD(Data4& data, Code4& code_base, const char* szName, const DWORD dwValue);

BOOL ReadWORD(Data4& data, Code4& code_base, const char* szName,WORD& wValue);
BOOL AssignWORD(Data4& data, Code4& code_base, const char* szName, const WORD wValue);

BOOL AssignField(Data4& data, Code4& code_base,	const char* szName, const CString& value, BOOL bFillLeadingZeroes,BOOL bOverwrite);
BOOL AssignField(Data4& data, Code4& code_base,	const char* szName, const char* szValue);
BOOL AssignInt(Data4& data, Code4& code_base, const char* szName, const int iValue);


inline FILESIZE RoundUp(FILESIZE a, FILESIZE d)
{
	FILESIZE r = a & (d-1);
	return (r==0) ? a : (a-r+d);
}

class CSequences;

class CSequence  
{
	// construction / destruction
public:
	// the default = 0 is for scan
	// a drive with(!) backslash ie d:\ lala
	CSequence(BYTE bFlags,CArchiv* pArchiv, CIPCDrive* pDrive, WORD wID);
	virtual ~CSequence();

	// attributes
public:
	inline WORD  GetNr() const;
		   WORD  GetArchivNr() const;
	inline BOOL  IsEmpty() const;
	inline CArchiv* GetArchiv() const;
		   Code4& GetCode4();
	inline const CString& GetDrive() const;
	inline DWORD GetNrOfPictures() const;
		   CIPCInt64 GetSizeBytes() const;
		   CIPCInt64 GetDbfSizeBytes() const;
		   CIPCInt64 GetDatSizeBytes() const;
		   CIPCInt64 GetSizeBytes(const CSystemTime& stStart,
							      const CSystemTime& stEnd);
		   BOOL	 IsOpen();
	inline BOOL	 IsOpenDbf();
	inline BOOL	 IsOpenDbd();
	inline BOOL	 IsSuspect() const;
	inline BOOL	 IsAlarm() const;
	inline BOOL	 IsAlarmList() const;
	inline BOOL	 IsRing() const;
	inline BOOL	 IsPreAlarm() const;
	inline BOOL	 IsBackup() const;
	inline BYTE  GetFlags() const;
	inline BOOL	 IsRO() const;
	inline BOOL  HasDbx() const;
		   BOOL  IsFull(DWORD dwSizeOfNextPicture);
		   BOOL  IsOpenByClient(DWORD dwClientID);
	
	inline CSystemTime GetFirstTime() const;
	inline CSystemTime GetLastTime() const;

	CString GetDirectory() const;
	CString GetName() const;
	CString GetIndexDbi() const;

	// operators
public:
	BOOL operator < (const CSequence& s);

	// operations
public:
	BOOL Scan(const CWin32FindData* pDbf, 
			  const CWin32FindData* pDbd,
			  const CVDBSequenceMap& map);
	BOOL Create(BOOL bThrow=FALSE);
	BOOL Flush(BOOL bThrow=FALSE);
	BOOL Close(BOOL bForce, BOOL bThrow=FALSE);
	void ShrinkDatToFit();
	BOOL Delete();
	BOOL Move(CSequence& source);

	// picture/data stuff
	BOOL SetName(const CString& sName);
	BOOL SaveData(CSaveData& data,WORD wPrevSequenceNr,DWORD dwNrOfRecords);
	BOOL SaveConvertPicture(const CSystemTime& sTime,
							int iCameraNr,
						    Compression comp,
						    Resolution res,
						    CompressionType ct,
						    CIPCSavePictureType pt,
							const CIPCFields& fields);

	BOOL Convert(const CConversion& conversion);
	BOOL NewConfirmRecord(const DWORD dwCurrentRecordNr,
						  const DWORD dwNewRecordNr,
						  CSecID camID,
						  CIPCDatabaseServer* pCIPCDatabase);

	BOOL GetRecord(DWORD dwRecNo, 
				   CIPCDatabaseServer* pCIPCDatabase, 
				   BOOL bFieldsAtPictureRecord,
				   CIPCPictureRecord*& pPR,
				   CIPCMediaSampleRecord*& pMSR,
				   CIPCFields& fields);
	BOOL GetFields(DWORD dwRecordNr, CIPCFields& fields);
	BOOL GetFieldsAndRecords(DWORD dwFirstRecordNr,
							 DWORD dwLastRecordNr,
							 CIPCFields& fields,
							 CIPCFields& records);

	// query
	BOOL Query(CQuery& q);

	// diagnostic
	void Trace();
	CString Format();

	BOOL Backup(CBackup& b, BOOL& bErrorSharingViolation);
	BOOL BackupByTime(CBackup& b, BOOL& bSequenceNotInTimeSpan, BOOL& bErrorSharingViolation);
	BOOL CalcDateTimeOpt(CQuery& query);

	void DoRequestOpenSequence(DWORD dwClientID);
	void DoRequestCloseSequence(DWORD dwClientID);

	BOOL AreFieldsEqual();
	BOOL Verify();
	void CorrectOffsets();

	static void GetNrAndFlags(const CString& sName, WORD& wNr,BYTE& bFlags);

	// implementation
private:
	void errorSet(int e=0);
	BOOL Open(BOOL bThrow=FALSE);
	BOOL OpenDbf(BOOL bThrow=FALSE);
	BOOL OpenDbd(BOOL bThrow=FALSE);
	BOOL CreateDir(BOOL bThrow=FALSE);
	BOOL CreateDbf(BOOL bThrow=FALSE);
	BOOL CreateDbd(BOOL bThrow=FALSE);
	BOOL CloseDbf(BOOL bForce=FALSE, BOOL bThrow=FALSE);
	BOOL CloseDbd(BOOL bForce=FALSE, BOOL bThrow=FALSE);
	BOOL ScanSize(BOOL bRoundUp);
	BOOL ScanDbf();
	BOOL ScanDbi();
	void InitFields();
	void AfterAppend(DWORD dwPictureLength,const CSystemTime& stStart,const CSystemTime& stEnd,DWORD dwNumPics);
	BOOL CutDbf(DWORD dwFirstRecordNr,
			    DWORD dwLastRecordNr,
			    const CString& sDbf,
				DWORD& dwNewSizeBytes);
	void CorrectReferences(Data4& data, DWORD dwFirstRecordNr);
	BOOL CutDat(DWORD dwFirstRecordNr,
			    DWORD dwLastRecordNr,
			    const CString& sDat,
				DWORD& dwNewSizeBytes);

	static BYTE CalcCheckSum(const BYTE* pData, DWORD dwDataLen);

	CString GetFileName(const CString& ext) const;
	
	DWORD GetIFrameRecordNr(DWORD dwRecordNr);
	DWORD GetSpecificFrameRecordNr(CSecID camID, DWORD dwRecordNr, BOOL bBackStep);
	DWORD GetTypedRecordNr(CSecID camID, DWORD dwRecordNr, BOOL bBackStep);
	DWORD GetDWORDValue(const char* szFieldName, DWORD dwDefault);
	void  AssignRWReferences(DWORD dwValueVideo,DWORD dwValueAudio);
	void  AssignFFReference(DWORD dwRecordNr, const char* szFieldName, DWORD dwValue);

	BOOL	 GetOffsetAndLength(DWORD dwRecordNr, FILESIZE& dwOffSet, DWORD& dwLen);
	FILESIZE GetCurrentWritePosition();
	void  ReadReferences();
	DWORD GetRecordNr(const CSystemTime& st, BOOL bGreaterThan);

	void  SetSequenceHashEntry();
	BOOL  ReadTime(BOOL bFirst);

public:
	static BYTE Nr2Flags(WORD wNr);
	static WORD Flags2Nr(BYTE bFlags);

	// private data member
private:
	CArchiv*	m_pArchiv;
	WORD		m_wID;
	BYTE		m_bFlags;
	
	CString		m_sDrive;
	DWORD		m_dwNumRecords;

	CString		m_sName;
	CSystemTime m_stFirst;
	CSystemTime m_stLast;
	
	DWORD		m_dwFieldSize;
	DWORD		m_dwClusterSize;

	FILESIZE	m_dwDbfSize;
	FILESIZE	m_dwDbdSize;
	FILESIZE	m_dwDbxSize;

	CCriticalSection m_cs;

	DWORD   m_dwLastVideo;
	DWORD   m_dwLastAudio;
	BOOL	m_bLockedByBackup;

	// picture data file dbd
	CFile		m_fData;
	FILESIZE	m_dwDatWritePosition;

	// CodeBase stuff
	Data4*	m_pData4;

	int		m_iOpenDbf;
	int		m_iOpenDbd;
	BOOL    m_bRO;
	CDWordArray m_dwClients;

	friend class CSequences;
};
//////////////////////////////////////////////////////////////////////
inline WORD CSequence::GetNr() const
{
	return m_wID;
}
//////////////////////////////////////////////////////////////////////
inline const CString& CSequence::GetDrive() const
{
	return m_sDrive;
}
//////////////////////////////////////////////////////////////////////
inline CArchiv* CSequence::GetArchiv() const
{
	return m_pArchiv;
}
//////////////////////////////////////////////////////////////////////
inline BOOL CSequence::IsEmpty() const
{
	return m_dwNumRecords == 0;
}
//////////////////////////////////////////////////////////////////////
inline DWORD CSequence::GetNrOfPictures() const
{
	return m_dwNumRecords;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CSequence::IsOpenDbf()
{
	CAutoCritSec acs(&m_cs);
	BOOL bRet = (m_pData4 != NULL) && (m_pData4->isValid());
	acs.Unlock();
	return bRet;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CSequence::IsOpenDbd()
{
	CAutoCritSec acs(&m_cs);
#if _MFC_VER < 0x0700
	BOOL bRet = (m_fData.m_hFile != (UINT)CFile::hFileNull) || HasDbx();
#else
	BOOL bRet = (m_fData.m_hFile != CFile::hFileNull) || HasDbx();
#endif
	acs.Unlock();
	return bRet;
}
//////////////////////////////////////////////////////////////////////
inline CSystemTime CSequence::GetFirstTime() const
{
	return m_stFirst;
}
//////////////////////////////////////////////////////////////////////
inline CSystemTime CSequence::GetLastTime() const
{
	return m_stLast;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CSequence::IsPreAlarm() const
{
	return (m_bFlags & CAR_IS_SAFE_RING)!=0; 
}
//////////////////////////////////////////////////////////////////////
inline BOOL CSequence::IsBackup() const
{
	return (m_bFlags & CAR_IS_BACKUP)!=0; 
}
//////////////////////////////////////////////////////////////////////
inline BOOL CSequence::IsSuspect() const
{
	return (m_bFlags & CAR_IS_SUSPECT)!=0; 
}
//////////////////////////////////////////////////////////////////////
inline BOOL CSequence::IsAlarm() const
{
	return (m_bFlags & CAR_IS_ALARM)!=0; 
}
//////////////////////////////////////////////////////////////////////
inline BOOL CSequence::IsRing() const
{
	return (m_bFlags & CAR_IS_RING)!=0; 
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CSequence::IsAlarmList() const
{
	return (m_bFlags & CAR_IS_ALARM_LIST)!=0; 
}
//////////////////////////////////////////////////////////////////////
inline BOOL CSequence::HasDbx() const
{
	return m_dwDbxSize > 0;
}
//////////////////////////////////////////////////////////////////////
inline BOOL CSequence::IsRO() const
{
	return m_bRO;
}
//////////////////////////////////////////////////////////////////////
inline BYTE CSequence::GetFlags() const
{
	return m_bFlags;
}

#endif // !defined(AFX_SEQUENCE_H__A30FED61_38DB_11D2_B58F_00C095EC9EFA__INCLUDED_)
