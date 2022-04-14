/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CIPCSequenceRecord.h $
// ARCHIVE:		$Archive: /Project/CIPC/CIPCSequenceRecord.h $
// CHECKIN:		$Date: 7.07.04 17:09 $
// VERSION:		$Revision: 14 $
// LAST CHANGE:	$Modtime: 7.07.04 17:08 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#if !defined(AFX_CIPCSEQUENCERECORD_H__E639AA84_98B3_11D2_B541_004005A19028__INCLUDED_)
#define AFX_CIPCSEQUENCERECORD_H__E639AA84_98B3_11D2_B541_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ArchivDefines.h"
#include "CIPCInt64.h"
#include "SystemTime.h"

class CIPC;
class CIPCExtraMemory;

class AFX_EXT_CLASS CIPCSequenceRecord : public CObject
{
	DECLARE_DYNAMIC(CIPCSequenceRecord)
	// construction / destruction
public:
	CIPCSequenceRecord();
	CIPCSequenceRecord(const CIPCSequenceRecord& src);
	virtual ~CIPCSequenceRecord();

	void operator = (const CIPCSequenceRecord& src);
	// attributes:
public:
	inline const CString& GetName() const;
	inline WORD  GetArchiveNr() const;
	inline WORD  GetSequenceNr() const;
	inline DWORD GetNrOfPictures() const;
	inline BYTE	 GetFlags() const;
	inline CIPCInt64 GetSize() const;
	inline CSystemTime GetTime() const;

	inline BOOL IsAlarm() const;
	inline BOOL IsRing() const;
	inline BOOL IsSafeRing() const;
	inline BOOL IsSearch() const;
	inline BOOL IsBackup() const;
	inline BOOL IsDeleteAllowed() const;
	inline BOOL IsSuspect() const;
	inline BOOL IsAlarmList() const;

	// operations:
public:
	void SetName(const CString &sName);
	void SetArchiveNr(WORD wNr);
	void SetSequenceNr(WORD wNr);
	void SetFlags(BYTE bNewFlags);
	void SetNrOfPictures(DWORD dwNrOfPictures);
	void SetSize(CIPCInt64 iSize);
	void SetTime(const CSystemTime& t);

	// operators
public:
	BOOL operator < (const CIPCSequenceRecord& s);

public:
	static CIPCExtraMemory* BubbleFromSequenceRecords(CIPC* pCipc,
													  int iNumRecords,
													  const CIPCSequenceRecord records[]);
	static CIPCSequenceRecord* SequenceRecordsFromBubble(int iNumRecords,
														 const CIPCExtraMemory *pExtraMem);

	// data
private:
	WORD		m_wArchivNr;
	WORD		m_wSequenceNr;
	DWORD		m_dwNrOfPictures;
	CIPCInt64	m_iSize;						  
	CSystemTime m_Time;
	BYTE		m_bFlags;
	CString		m_sName;
};
//////////////////////////////////////////////////////////////////////
typedef CIPCSequenceRecord* CIPCSequenceRecordPtr;
WK_PTR_ARRAY_CS(CIPCSequenceRecordArray, CIPCSequenceRecordPtr, CIPCSequenceRecordCS)
//////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CIPCSequenceRecords : public CIPCSequenceRecordCS
{
	// attributes
public:
	CIPCInt64			  GetSizeBytes();
	CIPCSequenceRecord*	  GetSequence(WORD wArchivNr, WORD wSequenceNr);

	// operations
public:
	BOOL RemoveSequence(WORD wArchivNr, WORD wSequenceNr);
	void Sort();
};
//////////////////////////////////////////////////////////////////////
#include "CIPCSequenceRecord.inl"
//////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_CIPCSEQUENCERECORD_H__E639AA84_98B3_11D2_B541_004005A19028__INCLUDED_)
