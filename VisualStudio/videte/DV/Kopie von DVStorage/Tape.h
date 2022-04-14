//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TAPE_H)
#define AFX_TAPE_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ImageData.h"
#include "Backup.h"

class CCollection;
class CSearch;
class CFindData;
class CIPCDatabaseStorage;

inline FILESIZE RoundUp(FILESIZE a, FILESIZE d)
{
	FILESIZE r = a & (d-1);
	return (r==0) ? a : (a-r+d);
}

class CTape  
{
	// construction / destruction
public:
	// the default = 0 is for scan
	// a drive with(!) backslash ie d:\ lala
	CTape(BYTE bFlags,
		  CCollection* pCollection, 
		  CIPCDrive* pDrive, 
		  WORD wID);
	virtual ~CTape();

	// attributes
public:
	inline WORD  GetNr() const;
		   WORD  GetArchiveNr() const;
		   Code4& GetCode4();
	inline BOOL  IsEmpty() const;
	inline CCollection* GetCollection() const;
	inline const CString& GetDrive() const;
				 CString  GetDirectory() const;
	inline DWORD GetNrOfRecords() const;
		   CIPCInt64 GetSizeBytes() const;
		   BOOL		 IsFull(DWORD dwSizeOfNextPicture);
		   CIPCInt64 GetSizeBytes(const CSystemTime& stStart,
							      const CSystemTime& stEnd);
		   BOOL	 IsOpen();
	inline BOOL	 IsOpenDbf();
		   BOOL	 IsOpenDat();
	inline BOOL	 IsBackup() const;
	inline BOOL	 IsSuspect() const;
	inline BOOL	 IsAlarm() const;
	inline BOOL	 IsAlarmList() const;
	inline BOOL	 IsRing() const;
	inline BOOL	 IsPreAlarm() const;
	inline BYTE  GetFlags() const;
	
	inline CSystemTime GetFirstTime() const;
	inline CSystemTime GetLastTime() const;

		   CString GetName() const;

	// operators
public:
	BOOL operator < (const CTape& s);

	// operations
public:
	BOOL Scan(const CFindData* pDbf, 
			  const CFindData* pDat, 
			  const CString& sFolder, 
			  const CVDBSequenceMap& map);
	BOOL Create();
	BOOL CreateDir();
	BOOL CreateDbf();
	BOOL CreateDat();
	BOOL Open();
	BOOL OpenDbf();
	BOOL OpenDat();
	BOOL Close(BOOL bForce = FALSE);
	BOOL CloseDbf(BOOL bForce = FALSE);
	BOOL CloseDat(BOOL bForce = FALSE);
	BOOL Delete();
	BOOL Move(CTape& source);

	// picture/data stuff
	BOOL Store(const CImageData& data,WORD wPrevSequenceNr,DWORD dwNrOfRecords);
	BOOL ConfirmRecord(DWORD dwCurrentRecordNr,
					   const DWORD dwNewRecordNr,
					   CSecID camID,
					   CIPCDatabaseStorage* pCIPCDatabase);

	BOOL GetRecord(DWORD dwRecNo, 
				   CIPCDatabaseStorage* pCIPCDatabase, 
				   BOOL bFieldsAtPictureRecord,
				   CIPCPictureRecord*& pPR,
				   CIPCMediaSampleRecord*& pMSR,
				   CIPCFields& fields);

	BOOL GetFields(DWORD dwRecordNr, CIPCFields& fields);
	BOOL GetFieldsAndRecords(DWORD dwFirstRecordNr,
							 DWORD dwLastRecordNr,
							 CIPCFields& fields,
							 CIPCFields& records,
							 const CSystemTime& stAfter);
	BOOL AreFieldsEqual();
	BOOL Search(CSearch& search);
	BOOL Backup(CBackup& b);
	BOOL BackupByTime(CBackup& b);
	BOOL CutDbf(DWORD dwFirstRecordNr,
			    DWORD dwLastRecordNr,
			    CWK_String& sDbf,
				DWORD& dwNewSizeBytes);
	void CorrectReferences(Data4& data, DWORD dwFirstRecordNr);
	BOOL CutDat(DWORD dwFirstRecordNr,
			    DWORD dwLastRecordNr,
			    const CString& sDat,
				DWORD& dwNewSizeBytes,
				CBackup& b,
				BOOL bIndicateProgress);
	static BOOL ReadDWORD(Data4& data, Code4& code_base,const CString& name,DWORD& dwValue);
	static BOOL AssignDWORD(Data4& data, Code4& code_base,const CString& name,const DWORD dwValue);

	// diagnostic
	void Trace();
	CString Format();


	BOOL Verify();

	// implementation
private:
	void errorSet(int e=0);
	BOOL ScanSize(BOOL bRoundUp);
	BOOL ScanDbf();
	void InitFields();
	void Flush();
	void AfterAppend(DWORD dwPictureLength, const CSystemTime& stStart, const CSystemTime& stEnd, DWORD dwNumPics);
	CString GetFileName(const CString& ext) const;

	BOOL AssignField(const char* name, const char* value);
	static BYTE CalcCheckSum(const BYTE* pData, DWORD dwDataLen);

	BOOL CalcDateTimeOpt(const CSystemTime& stStart,
					     const CSystemTime& stEnd);
	DWORD GetRecordNr(const CSystemTime& st, BOOL bGreaterThan);
	BOOL  GetOffsetAndLength(DWORD dwRecordNr, FILESIZE& dwOffset, DWORD& dwLen);
	FILESIZE GetCurrentWritePosition();
	void  ReadReferences();

	void  SetSequenceHashEntry();
	BOOL  ReadTime(BOOL bFirst);
	DWORD GetIFrameRecordNr(DWORD dwRecordNr);
	DWORD GetSpecificFrameRecordNr(CSecID camID, DWORD dwRecordNr, BOOL bBackStep);

	DWORD GetTypedRecordNr(CSecID camID, DWORD dwRecordNr, BOOL bBackStep);
	void  AssignRWReferences(DWORD dwValueVideo, DWORD dwValueAudio);
	void  AssignFFReference(DWORD dwRecordNr, const char* sFieldName, DWORD dwValue);
	DWORD GetDWORDValue(const char* szFieldName, DWORD dwDefault);
	
	static BYTE Nr2Flags(WORD wNr);
	static WORD Flags2Nr(BYTE bFlags);

	// private data member
private:
	CCollection*	m_pCollection;
	WORD			m_wID;
	WORD			m_wOriginalID;
	BYTE			m_bFlags;
	CString			m_sFolder;
	
	CString		m_sDrive;
	DWORD		m_dwNumRecords;

	CSystemTime m_stFirst;
	CSystemTime m_stLast;
	
	DWORD		m_dwFieldSize;
	DWORD		m_dwClusterSize;
	FILESIZE	m_dwDbfSize;
	FILESIZE	m_dwDatSize;

	CCriticalSection m_cs;

	DWORD	m_dwSavedWithoutFlush;
	BOOL	m_bLockedByBackup;
	
	DWORD   m_dwLastVideo;
	DWORD   m_dwLastAudio;

	// picture data file dbd
	CFile	m_fImages;
	FILESIZE	m_dwDatWritePosition;

	// CodeBase stuff
	Data4*	m_pData4;

	int		m_iOpenDbf;
	int		m_iOpenDat;
};
//////////////////////////////////////////////////////////////////////
inline WORD CTape::GetNr() const
{
	return m_wID;
}
//////////////////////////////////////////////////////////////////////
inline const CString& CTape::GetDrive() const
{
	return m_sDrive;
}
//////////////////////////////////////////////////////////////////////
inline BYTE CTape::GetFlags() const
{
	return m_bFlags;
}
//////////////////////////////////////////////////////////////////////
inline BOOL CTape::IsEmpty() const
{
	return m_dwNumRecords == 0;
}
//////////////////////////////////////////////////////////////////////
inline CCollection* CTape::GetCollection() const
{
	return m_pCollection;
}
//////////////////////////////////////////////////////////////////////
inline DWORD CTape::GetNrOfRecords() const
{
	return m_dwNumRecords;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CTape::IsOpenDbf()
{
	CAutoCritSec acs(&m_cs, _T("S19"), 250);
	BOOL bRet = (m_pData4 != NULL) && (m_pData4->isValid());
	acs.Unlock();
	return bRet;
}
//////////////////////////////////////////////////////////////////////
inline CSystemTime CTape::GetFirstTime() const
{
	return m_stFirst;
}
//////////////////////////////////////////////////////////////////////
inline CSystemTime CTape::GetLastTime() const
{
	return m_stLast;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CTape::IsPreAlarm() const
{
	return (m_bFlags & CAR_IS_SAFE_RING)!=0; 
}
//////////////////////////////////////////////////////////////////////
inline BOOL CTape::IsSuspect() const
{
	return (m_bFlags & CAR_IS_SUSPECT)!=0; 
}
//////////////////////////////////////////////////////////////////////
inline BOOL CTape::IsAlarm() const
{
	return (m_bFlags & CAR_IS_ALARM)!=0; 
}
//////////////////////////////////////////////////////////////////////
inline BOOL CTape::IsRing() const
{
	return (m_bFlags & CAR_IS_RING)!=0; 
}
//////////////////////////////////////////////////////////////////////
inline BOOL CTape::IsBackup() const
{
	return (m_bFlags & CAR_IS_BACKUP)!=0; 
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CTape::IsAlarmList() const
{
	return (m_bFlags & CAR_IS_ALARM_LIST)!=0; 
}
/////////////////////////////////////////////////////////////////////////////
typedef CTape* CTapePtr;
WK_PTR_ARRAY_CS(CTapeArray,CTapePtr,CTapeArrayCS);
/////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_Tape_H__A30FED61_38DB_11D2_B58F_00C095EC9EFA__INCLUDED_)
