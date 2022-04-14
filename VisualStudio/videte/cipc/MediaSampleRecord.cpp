/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: MediaSampleRecord.cpp $
// ARCHIVE:		$Archive: /Project/CIPC/MediaSampleRecord.cpp $
// CHECKIN:		$Date: 1.03.04 16:18 $
// VERSION:		$Revision: 41 $
// LAST CHANGE:	$Modtime: 1.03.04 16:17 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdcipc.h"

#include "CipcOutput.h"
#include "CipcExtraMemory.h"
#include "VideoJob.h"
#include "CipcOutputRecord.h"
#include "MediaSampleRecord.h"

// 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*********************************************************************************************
 Class      : CIPCMediaSampleRecord
 Function   : CIPCMediaSampleRecord
 Description: Constructor with arguments

 Parameters:   
  pCipc           : (E): pointer to CIPC object  (const CIPC*)
  SecID           : (E): Ident No.  (CSecID)

 Result type:  (None)
 Author: Rolf Kary-Ehlers
 created: July, 18 2003
 <TITLE CIPCMediaSampleRecord>
*********************************************************************************************/
CIPCMediaSampleRecord::CIPCMediaSampleRecord(CIPC* pCipc, CSecID SecID)
{
	m_pCipc          = pCipc;
	m_pBubble        = NULL;

	m_SecID          = SecID;

	m_Flags.m_dwFlags= 0;
	m_nPackageNumber = 0;
	m_nActualLength  = 0;

	m_llTimeStart    = 0;
	m_llTimeEnd      = 0;
	m_rtTimeStart    = 0;
	m_rtTimeEnd      = 0;

	m_pBuffer        = NULL;
	
	ZeroMemory(&m_Majortype, sizeof(GUID));
	ZeroMemory(&m_Subtype, sizeof(GUID));
	ZeroMemory(&m_Formattype, sizeof(GUID));
	m_lSampleSize = 0;
	m_cbFormat    = 0;
	m_pbFormat    = NULL;
   m_cBuffers    = 0;
   m_cbBuffer    = 0;
   m_cbAlign     = 0;
   m_cbPrefix    = 0;
}

/*********************************************************************************************
 Class      : CIPCMediaSampleRecord
 Function   : SetLongHeaderData
 Description: Sets more information to be delivered for the initialisation of DirectShow
              media samples
 Declaration: 
	for pbFormat
	  * Pointer to the format block. The structure type is specified
	    by the formattype member. The format structure must be present,
	    unless formattype is GUID_NULL or FORMAT_None.  
	for bTemporalCompression
	  * If TRUE, samples are compressed using temporal 
       (interframe) compression. A value of TRUE indicates that not all
	    frames are key frames. This field is informational only.

 Parameters:   
  Majortype      : (E): Specifies the major type of the media sample.  (<EXTLINK GUID>GUID</EXTLINK>)
  Subtype        : (E): Specifies the subtype of the media sample.  (<EXTLINK GUID>GUID</EXTLINK>)
  Formattype     : (E): Specifies the subtype of the media sample.  (<EXTLINK GUID>GUID</EXTLINK>)
  lSampleSize    : (E): Size of the sample in bytes.  (ULONG)
  cbFormat       : (E): Size of the format block, in bytes  (ULONG)
  pbFormat       : (E): Formatblock (BYTE*)
  bAllocFormat   : (E): If TRUE, the format block is allocated and copied.  (BOOL)
  bFixedSizeSamp : (E): If TRUE, samples are of a fixed size.  (BOOL)
  bTemporalCompression: (E): Tepmoral Compression (BOOL)
  cBuffers: (E): Number of buffers of the allocator  (long&)
  cbBuffer: (E): Buffer size of the allocator  (long&)
  cbAlign : (E): Alignement of the buffers of the allocator  (long&)
  cbPrefix: (E): Prefix of the buffers of the allocator  (long&)

 Result type:  (void)
 created: July, 18 2003
 <TITLE SetLongHeaderData>
*********************************************************************************************/
void CIPCMediaSampleRecord::SetLongHeaderData(GUID Majortype, GUID Subtype, GUID Formattype, 
															 ULONG lSampleSize, 
															 ULONG cbFormat, BYTE* pbFormat, BOOL bAllocFormat, 
															 long cBuffers, long cbBuffer, long cbAlign, long cbPrefix)
{
	m_Flags.m_msFlags.LongHeader = 1;
	m_Majortype   = Majortype;
	m_Subtype     = Subtype;
	m_Formattype  = Formattype;
	m_lSampleSize = lSampleSize;
	m_cbFormat    = cbFormat;
	m_Flags.m_msFlags.FormatAllocated = bAllocFormat;

   m_cBuffers    = cBuffers;
   m_cbBuffer    = cbBuffer;
   m_cbAlign     = cbAlign;
   m_cbPrefix    = cbPrefix;
	
	if (bAllocFormat)
	{
		m_pbFormat = new BYTE[m_cbFormat];
		memcpy(m_pbFormat, pbFormat, m_cbFormat);
	}
	else
	{
		m_pbFormat    = pbFormat;
	}
	
}

/*********************************************************************************************
 Class      : CIPCMediaSampleRecord
 Function   : CreateBubble
 Description: Creates a bubble with the data to be sent via CIPC. Call this function before
              you call GetBubble().

 Parameters: None 

 Result type:  (void)
 created: July, 18 2003
 <TITLE CreateBubble>
*********************************************************************************************/
void CIPCMediaSampleRecord::CreateBubble()
{
	ASSERT(m_pBubble == NULL);

	if (m_nActualLength >1024*1024) 
	{
		WK_TRACE_ERROR(_T("MediaSampleBubble %d exceeds 1MB\n"), m_nActualLength);
		return;
	}
	BYTE *pData = NULL;
	// m_nPackageNumber,m_nActualLength,m_dwFlags
	DWORD dwBubbleLen = sizeof(DWORD)*3;
	WORD  wCommentLength = 0;

	if (m_Flags.m_msFlags.LongLongTimePresent)
	{
		dwBubbleLen += sizeof(LONGLONG) * 2;			// Time: Start, End
	}
	if (m_Flags.m_msFlags.ReferenceTimePresent) 
	{
		dwBubbleLen += sizeof(LONGLONG) * 2;			// Reference Time:  Start, End
	}
	
	if (m_Flags.m_msFlags.CommentPresent)
	{
		dwBubbleLen += sizeof(WORD);						// TextSize
#ifdef _UNICODE
		wCommentLength = CIPCExtraMemory::CString2Memory(pData, m_sComment, m_pCipc->GetCodePage());
#else
		wCommentLength = (WORD)m_sComment.GetLength();
#endif
		dwBubbleLen += wCommentLength;					// Text data length
	}
	
	if (m_Flags.m_msFlags.LongHeader)
	{
		dwBubbleLen += sizeof(GUID)*3						// Majortype, Subtype, Formattype
			         +  sizeof(ULONG)*2					// SampleSize, FormatHeaderSize
						+  sizeof(long)*4						// Allocator: Buffers, BufferSize, Alignment, Prefix
						+  m_cbFormat;							// FormatHeaderLentgh
	}

	if (m_Flags.m_msFlags.ContainsData)
	{
		dwBubbleLen += m_nActualLength;					// Data length
	}

	m_pBubble = new CIPCExtraMemory();

	if (m_pBubble->Create(m_pCipc,dwBubbleLen))
	{
		pData = (BYTE *)m_pBubble->GetAddressForWrite();

		CIPCExtraMemory::DWORD2Memory(pData,m_Flags.m_dwFlags);
		CIPCExtraMemory::DWORD2Memory(pData,m_nPackageNumber);
		CIPCExtraMemory::DWORD2Memory(pData,m_nActualLength);

		if (m_Flags.m_msFlags.LongLongTimePresent)
		{
			*((LONGLONG*)pData) = m_llTimeStart;
			pData += sizeof(LONGLONG);
			*((LONGLONG*)pData) = m_llTimeEnd;
			pData += sizeof(LONGLONG);
		}

		if (m_Flags.m_msFlags.ReferenceTimePresent)
		{
			*((LONGLONG*)pData) = m_rtTimeStart;
			pData += sizeof(LONGLONG);
			*((LONGLONG*)pData) = m_rtTimeEnd;
			pData += sizeof(LONGLONG);
		}

		if (m_Flags.m_msFlags.CommentPresent)
		{
#ifdef _UNICODE
			CIPCExtraMemory::CString2Memory(pData, m_sComment, m_pCipc->GetCodePage());
#else
			CIPCExtraMemory::CString2Memory(pData, m_sComment);
#endif
		}
		
		if (m_Flags.m_msFlags.LongHeader)
		{
			*((GUID*)pData) = m_Majortype;
			pData += sizeof(GUID);
			*((GUID*)pData) = m_Subtype;
			pData += sizeof(GUID);
			*((GUID*)pData) = m_Formattype;
			pData += sizeof(GUID);

			*((ULONG*)pData) = m_lSampleSize;
			pData += sizeof(ULONG);
			*((ULONG*)pData) = m_cbFormat;
			pData += sizeof(ULONG);

			*((long*)pData) = m_cBuffers;			// Allocator Properties
			pData += sizeof(long);
			*((long*)pData) = m_cbBuffer;
			pData += sizeof(long);
			*((long*)pData) = m_cbAlign;
			pData += sizeof(long);
			*((long*)pData) = m_cbPrefix;
			pData += sizeof(long);

			CopyMemory(pData, m_pbFormat, m_cbFormat);
			pData += m_cbFormat;
		}

		if (m_Flags.m_msFlags.ContainsData)
		{
			CopyMemory(pData, m_pBuffer, m_nActualLength);
			pData += m_nActualLength;
		}
#ifdef _DEBUG
		DWORD dwLen = (DWORD)(pData - (BYTE *)m_pBubble->GetAddressForWrite());
		ASSERT(dwBubbleLen == dwLen);

		CWK_Profile wkp;
		if (wkp.GetInt(_T("Debug"), _T("MediaSampleRecordFile"), 0))
		{
			pData = (BYTE *)m_pBubble->GetAddressForWrite();
			CString str;
			str.Format(_T("MediaSampleRecordFile%d.bin"), m_nPackageNumber);
			CFile file(str, CFile::modeCreate|CFile::modeWrite);
			file.Write(pData, m_pBubble->GetLength());
		}
#endif
	}
	else
	{
		WK_DELETE(m_pBubble);
		WK_TRACE_ERROR(_T("cannot create MediaSampleRecord Bubble Len %d\n"),dwBubbleLen);
	}
}

void CIPCMediaSampleRecord::InitFromBubble()
{
	if (m_pBubble && !m_Flags.m_msFlags.MembersInitialized)
	{
		const BYTE *pData = (const BYTE*)m_pBubble->GetAddress();
		if (pData)
		{
			m_Flags.m_dwFlags = CIPCExtraMemory::Memory2DWORD(pData);
			m_nPackageNumber  = CIPCExtraMemory::Memory2DWORD(pData);
			m_nActualLength   = CIPCExtraMemory::Memory2DWORD(pData);

#ifdef _DEBUG
			CWK_Profile wkp;
			if (wkp.GetInt(_T("Debug"), _T("MediaSampleRecordFile"), 0)) 
			{
				CString str;
				str.Format(_T("MediaSampleRecordFile%d.bin"), m_nPackageNumber);
				CFile file(str, CFile::modeCreate|CFile::modeWrite);
				file.Write((const BYTE*)m_pBubble->GetAddress(), m_pBubble->GetLength());
			}
#endif
			if (m_Flags.m_msFlags.LongLongTimePresent)
			{
				m_llTimeStart = *((LONGLONG*)pData);
				pData += sizeof(LONGLONG);
				m_llTimeEnd   = *((LONGLONG*)pData);
				pData += sizeof(LONGLONG);
			}
			else
			{
				m_llTimeStart = m_llTimeEnd = 0;
			}
			if (m_Flags.m_msFlags.ReferenceTimePresent)
			{
				m_rtTimeStart = *((LONGLONG*)pData);
				pData += sizeof(LONGLONG);
				m_rtTimeEnd   = *((LONGLONG*)pData);
				pData += sizeof(LONGLONG);
			}
			else
			{
				m_rtTimeStart = m_rtTimeEnd = 0;
			}
			if (m_Flags.m_msFlags.CommentPresent)
			{
	#ifdef _UNICODE
				m_sComment = CIPCExtraMemory::Memory2CString(pData, m_pCipc->GetCodePage());
	#else
				m_sComment = CIPCExtraMemory::Memory2CString(pData);
	#endif
			}
			if (m_Flags.m_msFlags.LongHeader)
			{
				m_Majortype  = *((GUID*)pData);
				pData += sizeof(GUID);
				m_Subtype    = *((GUID*)pData);
				pData += sizeof(GUID);
				m_Formattype = *((GUID*)pData);
				pData += sizeof(GUID);

				m_lSampleSize = *((ULONG*)pData);
				pData += sizeof(ULONG);
				m_cbFormat = *((ULONG*)pData);
				pData += sizeof(ULONG);

				m_cBuffers = *((long*)pData);			// Allocator Properties
				pData += sizeof(long);
				m_cbBuffer = *((long*)pData);
				pData += sizeof(long);
				m_cbAlign = *((long*)pData);
				pData += sizeof(long);
				m_cbPrefix = *((long*)pData);
				pData += sizeof(long);


				m_Flags.m_msFlags.FormatAllocated = 1;
				m_pbFormat = new BYTE[m_cbFormat];
				CopyMemory(m_pbFormat, pData, m_cbFormat);
				pData += m_cbFormat;
			}
			else
			{
				ZeroMemory(&m_Majortype, sizeof(GUID));
				ZeroMemory(&m_Subtype, sizeof(GUID));
				ZeroMemory(&m_Formattype, sizeof(GUID));
				m_lSampleSize = 0;
				m_cbFormat    = 0;
				m_pbFormat    = NULL;
			}

			if (m_Flags.m_msFlags.ContainsData)
			{
				m_Flags.m_msFlags.BufferAllocated = 1;
				m_pBuffer = new BYTE[m_nActualLength];
				CopyMemory((void*)m_pBuffer, pData, m_nActualLength);
				pData += m_nActualLength;
			}
			m_Flags.m_msFlags.MembersInitialized = true;
		}
	}
}

/*********************************************************************************************
 Class      : CIPCMediaSampleRecord
 Function   : CIPCMediaSampleRecord
 Description: Constructor with arguments

 Parameters:   
  SecID         : (E): SecID of the media unit delivered by dwParam1   (CSecID)
  src           : (E): Additional data of the Media Sample    (<LINK CIPCExtraMemory, const CIPCExtraMemory&>)
  [bInitMembers]: (E): Default = true   (bool)
                       If true: the members are fully initialized.
							  If false: the Flags, PackageNumber, ActualLength and time information are initialized


 Result type:  ()
*********************************************************************************************/
CIPCMediaSampleRecord::CIPCMediaSampleRecord(CSecID SecID, const CIPCExtraMemory &src, bool bInitMembers/* = true */)
{
	m_SecID          = SecID;

	m_Flags.m_dwFlags= 0;

	m_pBuffer  = NULL;
	m_pCipc    = src.GetCIPC();
	m_pBubble  = new CIPCExtraMemory(src);
	if (bInitMembers)
	{
		InitFromBubble();
	}
	else
	{
		const BYTE *pData = (const BYTE*)src.GetAddress();
		if (pData)
		{
			m_Flags.m_dwFlags = CIPCExtraMemory::Memory2DWORD(pData);
			m_nPackageNumber  = CIPCExtraMemory::Memory2DWORD(pData);
			m_nActualLength   = CIPCExtraMemory::Memory2DWORD(pData);
			if (m_Flags.m_msFlags.LongLongTimePresent)
			{
				m_llTimeStart = *((LONGLONG*)pData);
				pData += sizeof(LONGLONG);
				m_llTimeEnd   = *((LONGLONG*)pData);
				pData += sizeof(LONGLONG);
			}
			else
			{
				m_llTimeStart = m_llTimeEnd = 0;
			}
			if (m_Flags.m_msFlags.ReferenceTimePresent)
			{
				m_rtTimeStart = *((LONGLONG*)pData);
				pData += sizeof(LONGLONG);
				m_rtTimeEnd   = *((LONGLONG*)pData);
				pData += sizeof(LONGLONG);
			}
			else
			{
				m_rtTimeStart = m_rtTimeEnd = 0;
			}
		}
		else
		{
			m_nPackageNumber = 0;
			m_nActualLength  = 0;
			m_llTimeStart    = 0;
			m_llTimeEnd      = 0;
			m_rtTimeStart    = 0;
			m_rtTimeEnd      = 0;
		}


		ZeroMemory(&m_Majortype, sizeof(GUID));
		ZeroMemory(&m_Subtype, sizeof(GUID));
		ZeroMemory(&m_Formattype, sizeof(GUID));
		m_lSampleSize = 0;
		m_cbFormat    = 0;
		m_pbFormat    = NULL;
		m_cBuffers    = 0;
		m_cbBuffer    = 0;
		m_cbAlign     = 0;
		m_cbPrefix    = 0;
	}
}
//////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : CIPCMediaSampleRecord
 Function   : CIPCMediaSampleRecord
 Description: Copy constructor

 Parameters:   
  src: (E): Source Media Sample Record  (const CIPCMediaSampleRecord&)

 Result type:  ()
 created: December, 09 2003
 <TITLE CIPCMediaSampleRecord>
*********************************************************************************************/
CIPCMediaSampleRecord::CIPCMediaSampleRecord(const CIPCMediaSampleRecord& src)
{
	m_SecID           = src.m_SecID;
	m_Flags.m_dwFlags = src.m_Flags.m_dwFlags;

	m_nPackageNumber  = src.m_nPackageNumber;
	m_nActualLength   = src.m_nActualLength;
	if (src.m_pBuffer)
	{
		m_Flags.m_msFlags.BufferAllocated = 1;
		m_pBuffer = new BYTE[m_nActualLength];
		CopyMemory((void*)m_pBuffer, src.m_pBuffer, m_nActualLength);
	}
	else
	{
		m_Flags.m_msFlags.BufferAllocated = 0;
		m_pBuffer  = NULL;
	}

	m_llTimeStart     = src.m_llTimeStart;
	m_llTimeEnd       = src.m_llTimeEnd;
	m_rtTimeStart     = src.m_rtTimeStart;
	m_rtTimeEnd       = src.m_rtTimeEnd;

	m_Majortype       = src.m_Majortype;
	m_Subtype         = src.m_Subtype;
	m_Formattype      = src.m_Formattype;

	m_lSampleSize     = src.m_lSampleSize;
	m_cbFormat        = src.m_cbFormat;
	if (src.m_pbFormat)
	{
		m_Flags.m_msFlags.FormatAllocated = 1;
		m_pbFormat = new BYTE[src.m_cbFormat];
		CopyMemory(m_pbFormat, src.m_pbFormat, src.m_cbFormat);
	}
	else
	{
		m_Flags.m_msFlags.FormatAllocated = 0;
		m_pbFormat        = NULL;
	}
	m_cBuffers        = src.m_cBuffers;
	m_cbBuffer        = src.m_cbBuffer;
	m_cbAlign         = src.m_cbAlign;
	m_cbPrefix        = src.m_cbPrefix;

	m_pCipc           = src.m_pCipc;
	if (src.m_pBubble) 
	{
		m_pBubble = new CIPCExtraMemory(*src.m_pBubble);	
	}
	else
	{
		m_pBubble = NULL;
	}
}
//////////////////////////////////////////////////////////////////////////
CIPCMediaSampleRecord::~CIPCMediaSampleRecord()
{
	WK_DELETE(m_pBubble);
	if (m_Flags.m_msFlags.BufferAllocated)
	{
		WK_DELETE(m_pBuffer);
	}
	if (m_Flags.m_msFlags.FormatAllocated)
	{
		WK_DELETE(m_pbFormat);
	}
}

/*********************************************************************************************
 Class      : *CIPCMediaSampleRecord
 Function   : GetData
 Description: Retrieves the data pointer

 Parameters: None 

 Result type: Pointer to the media data (BYTE*)
 created: September, 26 2003
 <TITLE GetData>
*********************************************************************************************/
const BYTE *CIPCMediaSampleRecord::GetData() const
{
	if (m_pBuffer) 
	{
		return (const BYTE *)m_pBuffer;
	}
	else
	{
		return NULL;
	}
}

/*********************************************************************************************
 Class      : CIPCMediaSampleRecord
 Function   : DetachData
 Description: Detaches the data from the Media sample record. Attention: the data pointer has
              to be deleted.

 Parameters: None 

 Result type: Pointer to the media data (BYTE*)
 created: September, 26 2003
 <TITLE DetachData>
*********************************************************************************************/
BYTE* CIPCMediaSampleRecord::DetachData()
{
	if (m_Flags.m_msFlags.BufferAllocated)
	{
		const BYTE *pBuffer = m_pBuffer;
		m_pBuffer = NULL;
		m_Flags.m_msFlags.BufferAllocated = FALSE;
		return (BYTE*) pBuffer;
	}
	return NULL;
}


/*********************************************************************************************
 Class      : CIPCMediaSampleRecord
 Function   : SetComment
 Description: Sets a comment for teh mediadata

 Parameters:   
  s: (E): Comment  (CString&)

 Result type:  (void)
 created: August, 01 2003
 <TITLE SetComment>
*********************************************************************************************/
void CIPCMediaSampleRecord::SetComment(CString &s)
{
	m_sComment = s;
	m_Flags.m_msFlags.CommentPresent = !m_sComment.IsEmpty();
}

/*********************************************************************************************
 Class      : CIPCMediaSampleRecord
 Function   : GetAllocatorProperties
 Description: Retrieves the allocator properties

 Parameters:   
  cBuffers: (A): Number of buffers of the allocator  (long&)
  cbBuffer: (A): Buffer size of the allocator  (long&)
  cbAlign : (A): Alignement of the buffers of the allocator  (long&)
  cbPrefix: (A): Prefix of the buffers of the allocator  (long&)

 Result type:  (BOOL)
 created: August, 01 2003
 <TITLE GetAllocatorProperties>
*********************************************************************************************/
BOOL CIPCMediaSampleRecord::GetAllocatorProperties(long &cBuffers, long &cbBuffer, long &cbAlign, long &cbPrefix)
{
	if (m_cbBuffer != 0)
	{
		cBuffers = m_cBuffers;			// Allocator Properties
		cbBuffer = m_cbBuffer;
		cbAlign  = m_cbAlign;
		cbPrefix = m_cbPrefix;
		return TRUE;
	}
	return FALSE;
}

/*********************************************************************************************
 Class      : CIPCMediaSampleRecord
 Function   : SetTimes
 Description: Sets all times for the Mediasample

 Parameters:   
  llTimeStart     : (E): Media start time  (LONGLONG)
  llTimeEnd       : (E): Media end time  (LONGLONG)
  rtTimeStart     : (E): Start time  (LONGLONG)
  rtTimeEnd       : (E): End time  (LONGLONG)

 Result type:  (void)
 created: August, 07 2003
 <TITLE SetTimes>
*********************************************************************************************/
void CIPCMediaSampleRecord::SetTimes(
		LONGLONG		llTimeStart,
		LONGLONG		llTimeEnd,
		LONGLONG    rtTimeStart,
		LONGLONG    rtTimeEnd)
{
	m_llTimeStart    = llTimeStart;
	m_llTimeEnd      = llTimeEnd;
	m_rtTimeStart    = rtTimeStart;
	m_rtTimeEnd      = rtTimeEnd;
	m_Flags.m_msFlags.LongLongTimePresent  = (llTimeStart != 0) || (llTimeEnd != 0) ? 1:0;
	m_Flags.m_msFlags.ReferenceTimePresent = (rtTimeStart != 0) && (rtTimeEnd != 0) ? 1:0;
}

/*********************************************************************************************
 Class      : CIPCMediaSampleRecord
 Function   : GetMediaTime
 Description: Sets the Media times and checks if they are valid

 Parameters:   
  llStart: (E): Start Time  (LONGLONG)
  llEnd  : (E): End Time  (LONGLONG)

 Result type: (void)
 created: December, 09 2003
 <TITLE GetMediaTime>
*********************************************************************************************/
void CIPCMediaSampleRecord::SetMediaTimes(LONGLONG llStart, LONGLONG llEnd)
{
	m_Flags.m_msFlags.TimeMediaIsSystem   = 0;
	m_Flags.m_msFlags.LongLongTimePresent = (m_llTimeStart != 0) ? 1 : 0;
	m_llTimeStart = llStart;
	m_llTimeEnd   = llEnd;
}

/*********************************************************************************************
 Class      : CIPCMediaSampleRecord
 Function   : GetReferenceTimes
 Description: Sets the Reference Times and checks if they are valid

 Parameters:   
  llStart: (E): Start Time  (LONGLONG)
  llEnd  : (E): End Time  (LONGLONG)

 Result type: (void)
 created: December, 09 2003
 <TITLE GetMediaTime>
*********************************************************************************************/
void CIPCMediaSampleRecord::SetReferenceTimes(LONGLONG llStart, LONGLONG llEnd)
{
	m_rtTimeStart = llStart;
	m_rtTimeEnd   = llEnd;
	ASSERT(llStart < llEnd);
	m_Flags.m_msFlags.ReferenceTimePresent = (llStart != 0 && llEnd != 0) ? 1 : 0;
}

/*********************************************************************************************
 Class      : CIPCMediaSampleRecord
 Function   : SetData
 Description: Set the data for the sample

 Parameters:   
  nPackageNumber  : (E): Number of the data package to deliver  (long)
  dwDataLen       : (E): Length of the data package  (DWORD)
  pData           : (E): Pointer to the data buffer  (const BYTE*)

 Result type:  (void)
 created: August, 07 2003
 <TITLE SetData>
*********************************************************************************************/
void CIPCMediaSampleRecord::AttachData(long nPackageNumber, DWORD dwDataLen, BYTE*pData)
{
	SetData(nPackageNumber, dwDataLen, pData);
	m_Flags.m_msFlags.BufferAllocated = 1;
}
void CIPCMediaSampleRecord::SetData(long nPackageNumber, DWORD dwDataLen, const BYTE* pData)
{
	if (m_Flags.m_msFlags.BufferAllocated)
	{
		WK_DELETE(m_pBuffer);
	}
	m_nPackageNumber = nPackageNumber;
	m_nActualLength  = dwDataLen;
	m_pBuffer        = (BYTE*)pData;
	m_Flags.m_msFlags.ContainsData   = pData != NULL ? 1:0;
	m_Flags.m_msFlags.BufferAllocated = 0;
}

/*********************************************************************************************
 Class      : CIPCMediaSampleRecord
 Function   : SetMediaInfo
 Description: Sets infor about the media sample type

 Parameters:   
  bIsPreroll      : (E): Indicates a preroll property. If TRUE, this sample is for (BOOL)
								 preroll only and should not be displayed
  bIsDiscontinuity: (E): Indicates a discontinuity property. If TRUE, this sample  (BOOL)
								 is not contiguous with previous sample.
  bIsSyncPoint    : (E): Determines if the beginning of a sample is a              (BOOL)
								 synchronization point.

 Result type:  (void)
 created: August, 07 2003
 <TITLE SetMediaInfo>
*********************************************************************************************/
void CIPCMediaSampleRecord::SetMediaInfo(
		BOOL        bIsPreroll,
		BOOL        bIsDiscontinuity,
		BOOL        bIsSyncPoint)
{
	m_Flags.m_msFlags.IsPreroll      = bIsPreroll;
	m_Flags.m_msFlags.IsDiscontinuty = bIsDiscontinuity;
	m_Flags.m_msFlags.IsSyncPoint    = bIsSyncPoint;
}

/*********************************************************************************************
 Class      : CIPCMediaSampleRecord
 Function   : SetMediaTime
 Description: Initializes the media time with the system time

 Parameters:   
  st: (E): The determined system time  (<LINK CSystemTime, CSystemTime&>)

 Result type:  (void)
 created: December, 09 2003
 <TITLE SetMediaTime>
*********************************************************************************************/
void CIPCMediaSampleRecord::SetMediaTime(CSystemTime &st)
{
	LONGLONG llTimes[2];
	ASSERT(sizeof(llTimes) == sizeof(st));
	BYTE *pTime = (BYTE*)&llTimes;
	st.WriteIntoBubble(pTime);
	m_llTimeStart = llTimes[0];
	m_llTimeEnd   = llTimes[1];
	m_Flags.m_msFlags.LongLongTimePresent = 1;
	m_Flags.m_msFlags.TimeMediaIsSystem   = 1;
}

/*********************************************************************************************
 Class      : CIPCMediaSampleRecord
 Function   : GetMediaTime
 Description: returns the systemtime

 Parameters: None 

 Result type: System time (CSystemTime)
 created: December, 09 2003
 <TITLE GetMediaTime>
*********************************************************************************************/
BOOL CIPCMediaSampleRecord::GetMediaTime(CSystemTime &st)
{
	if (   m_Flags.m_msFlags.LongLongTimePresent
		 && m_Flags.m_msFlags.TimeMediaIsSystem)
	{
		LONGLONG llTimes[2];
		llTimes[0] = m_llTimeStart;
		llTimes[1] = m_llTimeEnd;
		BYTE *pTime = (BYTE*)&llTimes;
		st.ReadFromBubble(pTime);
		return TRUE;
	}
	return FALSE;
}

/*********************************************************************************************
 Class      : CIPCMediaSampleRecord
 Function   : GetMediaTime
 Description: returns the Media times and checks if they are valid

 Parameters:   
  llStart: (A): Start Time  (LONGLONG*)
  llEnd  : (A): End Time  (LONGLONG*)

 Result type: Valid times (TRUE, FALSE) (BOOL)
 created: December, 09 2003
 <TITLE GetMediaTime>
*********************************************************************************************/
BOOL CIPCMediaSampleRecord::GetMediaTimes(LONGLONG*pllStart, LONGLONG*pllEnd)
{
	if (   m_Flags.m_msFlags.LongLongTimePresent
		 && !m_Flags.m_msFlags.TimeMediaIsSystem 
		 && pllStart && pllEnd)
	{
		if (m_llTimeEnd == 0)
		{
			*pllStart = m_llTimeStart;
			*pllEnd   = m_llTimeEnd;
			return TRUE;
		}
		else if (m_llTimeEnd > m_llTimeStart)
		{
			*pllStart = m_llTimeStart;
			*pllEnd   = m_llTimeEnd;
			return TRUE;
		}
	}
	return FALSE;
}

/*********************************************************************************************
 Class      : CIPCMediaSampleRecord
 Function   : GetReferenceTimes
 Description: returns the Reference Times and checks if they are valid

 Parameters:   
  llStart: (A): Start Time  (LONGLONG*)
  llEnd  : (A): End Time  (LONGLONG*)

 Result type: Valid times (TRUE, FALSE) (BOOL)
 created: December, 09 2003
 <TITLE GetMediaTime>
*********************************************************************************************/
BOOL CIPCMediaSampleRecord::GetReferenceTimes(LONGLONG*pllStart, LONGLONG*pllEnd)
{
	if (   m_Flags.m_msFlags.ReferenceTimePresent
		 && pllStart && pllEnd)
	{
		*pllStart = m_rtTimeStart;
		*pllEnd   = m_rtTimeEnd;
		return TRUE;
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////
int CIPCMediaSampleRecord::GetReferenceTime()
{
	REFERENCE_TIME rtStart, rtEnd;
	if (GetReferenceTimes(&rtStart, &rtEnd))
	{
		return UNITS_100NS_TO_MILLISECONDS(rtEnd-rtStart);
	}
	return 0;
}

/*********************************************************************************************
 Class      : CIPCMediaSampleRecord
 Function   : GetSampleLengthInMS
 Description: Returns the length of a media sample in [ms]

 Parameters: None 

 Result type: Length (int)
 created: December, 17 2003
 <TITLE GetSampleLengthInMS>
*********************************************************************************************/
int CIPCMediaSampleRecord::GetSampleLengthInMS()
{
	return (int)((m_rtTimeEnd - m_rtTimeStart) / 10000);
}

/*********************************************************************************************
 Class      : CIPCMediaSampleRecord
 Function   : SetContainsData
 Description: Sets the flag wheter the sample record contains data.

 Parameters:   
  bData: (E): Sample record contains data  (bool)

 Result type:  (bool)
 created: December, 19 2003
 <TITLE SetContainsData>
*********************************************************************************************/
bool CIPCMediaSampleRecord::SetContainsData(bool bData)
{
	bool  bOld = m_Flags.m_msFlags.ContainsData;
	m_Flags.m_msFlags.ContainsData = bData; 
	return bOld;
}

/*********************************************************************************************
 Class      : CIPCMediaSampleRecord
 Function   : UpdateFlagsToBubble
 Description: Updates the Flags in the Bubble

 Parameters:   

 Result type:  (void)
 created: December, 19 2003
 <TITLE SetSyncPoint>
*********************************************************************************************/
void CIPCMediaSampleRecord::UpdateFlagsToBubble()
{
	if (m_pBubble)
	{
		BYTE *pData = (BYTE *)m_pBubble->GetAddress();
		CIPCExtraMemory::DWORD2Memory(pData, m_Flags.m_dwFlags);
	}
}

//////////////////////////////////////////////////////////////////////////
// class CIPCMediaSampleRecordsEx
CIPCMediaSampleRecordsEx::CIPCMediaSampleRecordsEx()
{
}
/*********************************************************************************************
 Class      : CIPCMediaSampleRecordsEx
 Function   : MergeSampleRecords
 Description: Merges the collected medias amples to one media sample record
			  Make sure that samples are added by AddTail(..).
 Parameters: None 

 Result type: Merged media sample record (CIPCMediaSampleRecord*)
 created: February, 18 2004
 <TITLE MergeSampleRecords>
*********************************************************************************************/
CIPCMediaSampleRecord* CIPCMediaSampleRecordsEx::MergeSampleRecords()
{
	CIPCMediaSampleRecord *pMerged = NULL;
	int nCount = GetCount();
	if (nCount == 1)
	{
		pMerged = new CIPCMediaSampleRecord(*GetHead());
		pMerged->CreateBubble();
	}
	else if (nCount)
	{
		CIPCMediaSampleRecord *pSR;
		POSITION pos;
		int  nSize = 0, nBP, nAL, nLH = -1, i;
		bool bIsSyncPoint     = false;
		bool bIsPreroll       = false;
		bool bIsDiscontinuity = false;
		BYTE *pBuffer = NULL;
		REFERENCE_TIME rtStart, rtEnd;
		CSystemTime st;

		GetReferenceTimes(&rtStart, &rtEnd);
		
		for (pos=GetHeadPosition(), i=0; pos!=NULL; i++)
		{
			pSR = GetNext(pos);
			if (i==0)
			{
				pSR->GetMediaTime(st);
			}
			nSize += pSR->GetActualLength();
			if (pSR->GetIsSyncPoint()   ) bIsSyncPoint     = true;
			if (pSR->GetIsPreroll()     ) bIsPreroll       = true;
			if (pSR->GetIsDiscontinuty()) bIsDiscontinuity = true;
			if (pSR->IsLongHeader()     ) nLH = i;
		}
		if (nLH == -1)
		{
			nLH = 0;
		}
		if (nSize > 0)
		{
			pBuffer = new BYTE[nSize];
			for (pos=GetHeadPosition(), nBP=0, i=0; pos!=NULL; i++)
			{
				pSR = GetNext(pos);
				nAL = pSR->GetActualLength();
				if (i==nLH)
				{
					BYTE *pTemp = pSR->DetachData();
					ASSERT(pTemp != NULL);
					memcpy(&pBuffer[nBP], pTemp, nAL);
					delete pTemp;
					pMerged = new CIPCMediaSampleRecord(*pSR);
				}
				else
				{
					memcpy(&pBuffer[nBP], pSR->GetData(), nAL);
				}
				nBP += nAL;
			}

			pMerged->AttachData(GetHead()->GetPackageNumber(), nSize, pBuffer);
			pMerged->SetReferenceTimes(rtStart, rtEnd);
			pMerged->SetMediaTime(st);

			pMerged->CreateBubble();
		}
	}
	return pMerged;
}
/*********************************************************************************************
 Class      : CIPCMediaSampleRecordsEx
 Function   : GetReferenceTimes
 Description: Retrieves the start time of the first sample and the end time of the last sample
              in this list.
			  Make sure that samples are added by AddTail(..).

 Parameters:   
  rtStart: (A): Start time of the sample collection  (REFERENCE_TIME*)
  rtEnd  : (A): End time of the sample collection  (REFERENCE_TIME*)

 Result type:  (BOOL)
 created: February, 18 2004
 <TITLE GetReferenceTimes>
*********************************************************************************************/
BOOL CIPCMediaSampleRecordsEx::GetReferenceTimes(REFERENCE_TIME*prtStart, REFERENCE_TIME*prtEnd)
{
	REFERENCE_TIME  rtTemp;
	if (GetCount())
	{
		GetHead()->GetReferenceTimes(prtStart, &rtTemp);
		GetTail()->GetReferenceTimes(&rtTemp , prtEnd);
		return TRUE;
	}
	return FALSE;
}
/*********************************************************************************************
 Class      : CIPCMediaSampleRecordsEx
 Function   : GetTotalReferenceTime
 Description: Retrieves the time span of the inserted media samples.
			  Make sure that samples are added by AddTail(..).

 Parameters: None 

 Result type: time span (REFERENCE_TIME )
 created: February, 18 2004
 <TITLE GetTotalReferenceTime>
*********************************************************************************************/
REFERENCE_TIME  CIPCMediaSampleRecordsEx::GetTotalReferenceTime()
{
	REFERENCE_TIME rtStart, rtTemp, rtEnd;
	ZERO_INIT(rtTemp);
	if (GetReferenceTimes(&rtStart, &rtEnd))
	{
		rtTemp = rtEnd - rtStart;
	}
	
	return rtTemp;
}
