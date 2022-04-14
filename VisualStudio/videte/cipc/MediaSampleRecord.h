/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: MediaSampleRecord.h $
// ARCHIVE:		$Archive: /Project/CIPC/MediaSampleRecord.h $
// CHECKIN:		$Date: 20.01.06 9:28 $
// VERSION:		$Revision: 36 $
// LAST CHANGE:	$Modtime: 19.01.06 20:25 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef __CIPCMediaSampleRecord_H
#define __CIPCMediaSampleRecord_H

#include "Cipc.h"
#include "SystemTime.h"
#include "wkclasses\wk_util.h"

/////////////////////////////////////////////////////////////////////////////
// 
/////////////////////////////////////////////////////////////////////////////
/*
*/
/////////////////////////////////////////////////////////////////////////////

class AFX_EXT_CLASS CIPCMediaSampleRecord 
{
public:
	CIPCMediaSampleRecord(CIPC* pCIPC, CSecID mediaID);
	CIPCMediaSampleRecord(CSecID mediaID, const CIPCExtraMemory& bubble, bool bInitMembers=true);
	CIPCMediaSampleRecord(const CIPCMediaSampleRecord& src);

	struct MediaSampleFlags
	{
		DWORD LongHeader          : 1;	// 1
		DWORD ContainsData        : 1;	// 2
		DWORD IsPreroll           : 1;	// 3
		DWORD IsDiscontinuty      : 1;	// 4
		DWORD IsSyncPoint         : 1;	// 5
		DWORD FixedSizeSamples    : 1;	// 6
		DWORD TemporalCompression : 1;	// 7
		DWORD ReferenceTimePresent: 1;	// 8
		DWORD LongLongTimePresent : 1;	// 9
		DWORD BufferAllocated     : 1;	// 10
		DWORD FormatAllocated     : 1;	// 11
		DWORD ReInitFilters       : 1;	// 12
		DWORD CommentPresent      : 1;	// 13
		DWORD AllocPropPresent    : 1;	// 14
		DWORD MembersInitialized  : 1;	// 15
		DWORD TimeMediaIsSystem   : 1;	// 15
		DWORD Dummy               : 6;
	};

	virtual ~CIPCMediaSampleRecord();

	// attributes:
public:
	void AttachData(long nPackageNumber, DWORD dwDataLen, BYTE*pData);
	void SetData(long nPackageNumber, DWORD dwDataLen, const BYTE*pData);
	void SetTimes(LONGLONG llTimeStart, LONGLONG llTimeEnd, LONGLONG rtTimeStart, LONGLONG rtTimeEnd);
	void SetMediaInfo(BOOL bIsPreroll, BOOL bIsDiscontinuity, BOOL bIsSyncPoint);
	void SetLongHeaderData(GUID Majortype, GUID Subtype, GUID Formattype, 
								 ULONG lSampleSize, 
								 ULONG cbFormat, BYTE* pbFormat, BOOL bAllocFormat, 
								 long cBuffers, long cbBuffer, long cbAlign, long cbPrefix);
	void SetComment(CString&sComment);
	void SetMediaTime(CSystemTime&st);
	void SetMediaTimes(LONGLONG llStart, LONGLONG llEnd);
	void SetReferenceTimes(LONGLONG llStart, LONGLONG llEnd);
	void SetSyncPoint(bool bSync) { m_Flags.m_msFlags.IsSyncPoint = bSync; }
	bool SetContainsData(bool bData);
	
public:
	/*****************************************
	  \Returns the corresponding shared memory
	  *****************************************/
	inline const CIPCExtraMemory* GetBubble() const {return m_pBubble;};
	// !ic! returns a pointer to the data buffer
	const BYTE* GetData() const;
	BYTE* DetachData();

	CSecID   GetID()                  const {return m_SecID;};
	DWORD    GetFlags()               const {return m_Flags.m_dwFlags;};
	BOOL     GetIsPreroll()           const {return m_Flags.m_msFlags.IsPreroll;};
	BOOL     GetIsDiscontinuty()      const {return m_Flags.m_msFlags.IsDiscontinuty;};
	BOOL     GetIsSyncPoint()         const {return m_Flags.m_msFlags.IsSyncPoint;};
	BOOL     GetFixedSizeSamples()    const {return m_Flags.m_msFlags.FixedSizeSamples;};
	BOOL     GetTemporalCompression() const {return m_Flags.m_msFlags.TemporalCompression;};
	BOOL     IsDataInitialized()      const {return m_Flags.m_msFlags.MembersInitialized;};
	BOOL     IsLongHeader()           const {return m_Flags.m_msFlags.LongHeader;};
	BOOL     ContainsData()           const {return m_Flags.m_msFlags.ContainsData;};

	long     GetPackageNumber()const {return m_nPackageNumber;};
	ULONG    GetActualLength() const {return m_nActualLength;};
	
	BOOL     GetMediaTime(CSystemTime& st);
	BOOL     GetMediaTimes(LONGLONG*pllStart, LONGLONG*pllEnd);
	BOOL     GetReferenceTimes(LONGLONG*pllStart, LONGLONG*pllEnd);
	int      GetReferenceTime();
	CString  GetComment()		{return m_sComment;};
	int      GetSampleLengthInMS();

	GUID     GetMajorType()    {return m_Majortype;};
	GUID     GetSubType()      {return m_Subtype;};
	GUID     GetFormatType()   {return m_Formattype;};
	ULONG    GetSampleSize()   {return m_lSampleSize;};
	ULONG    GetFormatSize()   {return m_cbFormat;};
	BYTE*    GetFormat()       {return m_pbFormat;};
	BOOL     GetAllocatorProperties(long&, long&, long&, long&);

	//  operations:
public:
	void CreateBubble();
	void InitFromBubble();
	void UpdateFlagsToBubble();

	// implementation
private:
	
	// data member
private:
	// short Header
	// transmitted in dwParam
	CSecID      m_SecID;				// ID
	long        m_nPackageNumber;	// Package number
	ULONG       m_nActualLength;	// actual length of the buffers data
	// transmitted in shared memory
	LONGLONG    m_llTimeStart;		// the beginning media time
	LONGLONG    m_llTimeEnd;		// the ending media time
	LONGLONG    m_rtTimeStart;		// sets the stream times when this sample SHOULD begin
	LONGLONG    m_rtTimeEnd;		// and finish
	CString     m_sComment;			// Comment or name
	// long Header: MediaType
	GUID        m_Majortype;		// Major Mediatype
	GUID        m_Subtype;			// Sub Mediatype
	GUID        m_Formattype;		// Format Type
	ULONG       m_lSampleSize;		// Sample Size: length of the buffer
   long        m_cBuffers;			// Allocator Properties
   long        m_cbBuffer;
   long        m_cbAlign;
   long        m_cbPrefix;
	ULONG       m_cbFormat;			// Size of the Format Buffer
	BYTE*       m_pbFormat;			// Format Buffer
	// Data
	BYTE       *m_pBuffer;			// the Buffer pointer
	// not stored in shared memory
	const CIPC *m_pCipc;	
	CIPCExtraMemory *m_pBubble;
	union MSFlags
	{
		MediaSampleFlags m_msFlags;
		DWORD            m_dwFlags;
	}           m_Flags;				// Flags
};
typedef CIPCMediaSampleRecord * CIPCMediaSampleRecordPtr;
WK_PTR_LIST_CS(CIPCMediaSampleRecordListCS,CIPCMediaSampleRecordPtr,CIPCMediaSampleRecords)
/////////////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CIPCMediaSampleRecordsEx : public CIPCMediaSampleRecords
{
public:
	// Construction
	CIPCMediaSampleRecordsEx();
	// operations
	CIPCMediaSampleRecord* MergeSampleRecords();
	// Attributes
	BOOL GetReferenceTimes(REFERENCE_TIME*prtStart, REFERENCE_TIME*prtEnd);
	REFERENCE_TIME GetTotalReferenceTime();
};

#endif // __CIPCMediaSampleRecord_H
