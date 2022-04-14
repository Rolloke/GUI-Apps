/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CIPCArchivRecord.h $
// ARCHIVE:		$Archive: /Project/CIPC/CIPCArchivRecord.h $
// CHECKIN:		$Date: 7.07.04 17:01 $
// VERSION:		$Revision: 11 $
// LAST CHANGE:	$Modtime: 7.07.04 16:56 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#if !defined(AFX_CIPCARCHIVRECORD_H__5E240FE3_98AF_11D2_B541_004005A19028__INCLUDED_)
#define AFX_CIPCARCHIVRECORD_H__5E240FE3_98AF_11D2_B541_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ArchivDefines.h"
#include "CIPCInt64.h"

class CIPC;
class CIPCExtraMemory;

///////////////////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CIPCArchivRecord : public CObject
{
	DECLARE_DYNAMIC(CIPCArchivRecord)
	// construction / destruction
public:
	CIPCArchivRecord();
	CIPCArchivRecord(const CIPCArchivRecord&src);

	// attributes:
public:
	inline const CString& GetName() const;
	inline WORD GetID() const;
	inline WORD GetSubID() const;
	inline WORD GetSafeRingID() const;
	inline BYTE GetFlags() const;
	inline ArchivSize	BenutzterPlatz() const;
	inline DWORD SizeInMB() const;
	
	// archive type
	inline BOOL IsAlarm() const;
	inline BOOL IsRing() const;
	inline BOOL IsSafeRing() const;
	inline BOOL IsBackup() const;
	inline BOOL IsSearch() const;
	inline BOOL IsAlarmList() const;

	// operations:
public:
	void SetName(const CString &sName);
	void SetID(WORD wID);
	void SetSafeRingID(WORD wSafeID);
	void SetFlags(BYTE bNewFlags);
	void SetSizeInMB(DWORD dwSize);
	void SetBenutzterPlatz(ArchivSize as);

public:
	static CIPCExtraMemory* BubbleFromArchivRecords(CIPC *pCipc,
													int iNumRecords,
													const CIPCArchivRecord data[]);
	static CIPCArchivRecord* ArchivRecordsFromBubble(int iNumRecords,
												     const CIPCExtraMemory *pExtraMem);
	// data member
private:
	CString		m_sName;
	WORD		m_wID;
	WORD		m_wSafeRingID;
	DWORD		m_dwSizeInMB;
	ArchivSize	m_AsBenutzt;
	BYTE		m_bFlags;
};
#include "CIPCArchivRecord.inl"
///////////////////////////////////////////////////////////////////////////////////
typedef CIPCArchivRecord *CIPCArchivRecordPtr;
WK_PTR_ARRAY_CS(CIPCArchivRecordArrayPlain,CIPCArchivRecordPtr,CIPCArchivRecordArray)
///////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CIPCArchivRecords : public CIPCArchivRecordArray
{
	// attributes
public:
	CIPCInt64			  GetSizeBytes();
	CIPCArchivRecord*	  GetArchiv(WORD wArchivNr);

	// operations
public:
	BOOL	  RemoveArchiv(WORD wArchivNr);
};

#endif // !defined(AFX_CIPCARCHIVRECORD_H__5E240FE3_98AF_11D2_B541_004005A19028__INCLUDED_)
