/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: PictureRecord.h $
// ARCHIVE:		$Archive: /Project/CIPC/PictureRecord.h $
// CHECKIN:		$Date: 6/16/05 3:41p $
// VERSION:		$Revision: 26 $
// LAST CHANGE:	$Modtime: 6/16/05 3:39p $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef __CIPCPictureRecord_H
#define __CIPCPictureRecord_H

#include "Cipc.h"
#include "SystemTime.h"

/////////////////////////////////////////////////////////////////////////////
// 
/////////////////////////////////////////////////////////////////////////////
/*
 CIPCPictureRecord | The class for any kind of picture or image
inside the CIPC. The memory allocated by a CIPCPictureRecord is always shared
memory. So it's expensive. A CIPCPictureRecord is always used with an CIPC
derived class to create the shared memory.
 <c CIPC>, <t Resolution>, <t Compression>, <t CompressionType>, <c CSystemTime>
*/
/////////////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CIPCPictureRecord 
{
	//  construction/destruction
public:
	//!ic! parameter constructor
	CIPCPictureRecord(const CIPC *pCipc, 
					  const BYTE *pData, DWORD dwDataLen,
					  CSecID  camID,
					  Resolution res,
					  Compression comp,
					  CompressionType ct,
					  const CSystemTime stTimesStamp,
					  DWORD dwJobTime,
					  LPCTSTR szInfoString=NULL,
					  DWORD dwBitrate=0,
					  CIPCSavePictureType pictureType = SPT_FULL_PICTURE,
					  WORD wBlockNr=0,
					  void*pHeader=NULL,
					  int nHeaderLen=0); 
	CIPCPictureRecord(const CIPC *pCipc, 
					  CSecID  camID,
					  Resolution res,
					  Compression comp,
					  CompressionType ct,
					  BOOL bIsVideoPresent,
					  WORD wBlockNr,
					  const CPtrArray& datas,
					  const CDWordArray& dwLengths,
					  const CSystemTimeArray& times,
					  DWORD dwFrameTimeStepMikroSeconds=0,
					  DWORD dwNoOfMDPoints=0,
					  CPoint *pMDPoints=NULL
					  ); 
	//!ic! copy constructor with shared memory reference counting
	CIPCPictureRecord(const CIPCPictureRecord &pict);
	//!ic! special constructor from shared memory
	CIPCPictureRecord(const CIPCExtraMemory &receivedBubble);
	//!ic! destructor
	virtual ~CIPCPictureRecord();

	// attributes:
public:
	//!ic! returns the camera id
	inline CSecID				GetCamID() const;
	//!ic! returns the resolution of picture
	inline Resolution			GetResolution() const;
	//!ic! returns the compression rate of picture
	inline Compression			GetCompression() const;
	//!ic! returns the compression type of picture
	inline CompressionType		GetCompressionType() const;
	//!ic! returns the job time (GetTickCount) of picture, for server-unit
	inline DWORD				GetJobTime() const;
	//!ic! returns the picture type
	inline CIPCSavePictureType	GetPictureType() const;
	//!ic! returns the bitrate for H.263
	inline DWORD				GetBitrate() const;
	//!ic! returns the frame time step for GOPs in µs
	DWORD						GetFrameTimeStep() const;
	DWORD						GetNumberOfMDPoints() const;
	CPoint						GetMDPoint(int i) const;
	//!ic! returns the block nr.
	inline WORD					GetBlockNr() const;
	//!ic! returns the owning CIPC object
	inline const CIPC*			GetCIPC() const;
	//!ic! returns the alarm info string for pre 4.0 data
	inline const CString&		GetInfoString() const;
	//!ic! returns the grabbing time of the picture
	inline const CSystemTime&	GetTimeStamp() const;

	//  picture data:
public:
	//!ic! returns the corresponding shared memory
	inline const CIPCExtraMemory* GetBubble() const;
	//!ic! returns a pointer to the image data buffer
		   const BYTE* GetData(int i=0) const;
	//!ic! returns the image data buffer length
				 DWORD GetDataLength(int i=0) const;
	//!ic! returns the grabbing time of the picture
				 CSystemTime GetTimeStamp(int i) const;
				 //!ic! returns the image data buffer length
	inline const DWORD GetNumberOfPictures() const;

	//  operations:
public:
	//!ic! sets the cam id, only in memory, not in shared memory
	inline void	SetCamID(CSecID id);
	//!ic! sets the bitrate for H.263, only in memory, not in shared memory
	inline void	SetBitrate(DWORD dwBitrate);
	//!ic! sets the infostring, only in memory, not in shared memory
	inline void	SetInfoString(const CString& s);

	void ReleaseBubble();

	// implementation
private:
	int  GetDataOffset() const;
	void CreateBubble(const BYTE *pData, DWORD dwDataLen);
	void CreateBubble(const CPtrArray& datas,
					  const CDWordArray& dwLengths,
					  const CSystemTimeArray& times);

	inline PBYTE GetTimes() const;
	
	// data member
private:
	// stored in shared memory
	CSecID				m_camID;
	Resolution			m_resolution;
	Compression			m_compression;
	CompressionType		m_compressionType;
	DWORD				m_dwJobTime;
	CIPCSavePictureType m_pictureType;
	DWORD				m_dwBitrate;
	DWORD				m_dwBlockNr;
	CString				m_sInfoString;
	CSystemTime			m_stTimeStamp;
	CDWordArray			m_dwLengths;
	DWORD				m_dwFlags;
	int					m_nNoOfMDPoints;
	CPoint*				m_pMDPoints;
	
	// not stored in shared memory
	const CIPC *m_pCipc;	
	CIPCExtraMemory *m_pBubble;
	DWORD			 m_dwTimeOffset;
	void	*m_pHeader;
	int    m_nHeaderLen;

	//
	LPBYTE m_pBuffer;
	DWORD  m_dwBufferLen;
};
/////////////////////////////////////////////////////////////////////////////
typedef CIPCPictureRecord * CIPCPictureRecordPtr;
WK_PTR_ARRAY_CS(CIPCPictureRecordArrayPlain,
				CIPCPictureRecordPtr,
				CIPCPictureRecordArraySafe)
/////////////////////////////////////////////////////////////////////////////
//  CIPCPictureRecords
/////////////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CIPCPictureRecords : public CIPCPictureRecordArraySafe
{
public:
	virtual ~CIPCPictureRecords();	 // performs DeleteAll
	//
	CIPCPictureRecord *CreateSuperBubble(
							const CIPC *pCIPC,
							LPCTSTR szInfoString
							);	// performs DeleteAll
};
/*********************************************************************************************
 Class      : CIPCPictureRecord
 Function   : GetResolution
 Description: returns the resolution of picture

 Parameters: None 

 Result type:  (Resolution)
 created: June, 16 2005
 <TITLE GetResolution>
*********************************************************************************************/
inline Resolution CIPCPictureRecord::GetResolution() const
{
	return m_resolution;
}
/*********************************************************************************************
 Class      : CIPCPictureRecord
 Function   : GetCompression
 Description: returns the compression of picture

 Parameters: None 

 Result type:  (Compression)
 created: June, 16 2005
 <TITLE GetCompression>
*********************************************************************************************/
inline Compression CIPCPictureRecord::GetCompression() const
{
	return m_compression;
}
/*********************************************************************************************
 Class      : CIPCPictureRecord
 Function   : GetCompressionType
 Description: returns the compression type of picture

 Parameters: None 

 Result type:  (CompressionType )
 created: June, 16 2005
 <TITLE GetCompressionType>
*********************************************************************************************/
inline CompressionType  CIPCPictureRecord::GetCompressionType() const
{
	return m_compressionType;
}
/*********************************************************************************************
 Class      : CIPCPictureRecord
 Function   : GetCamID
 Description: returns the id of camera of the picture

 Parameters: None 

 Result type:  (CSecID)
 created: June, 16 2005
 <TITLE GetCamID>
*********************************************************************************************/
inline CSecID CIPCPictureRecord::GetCamID() const
{
	return m_camID;
}
/*********************************************************************************************
 Class      : CIPCPictureRecord
 Function   : SetCamID
 Description: sets the camera id of picture in memory, not in shared memory,
              so you can use the new value only in the same process, not
              to send the CIPCPictureRecord to another process

 Parameters:   
  id: (E): Camera ID  (CSecID)

 Result type:  (void)
 created: June, 16 2005
 <TITLE SetCamID>
*********************************************************************************************/
inline void	CIPCPictureRecord::SetCamID(CSecID id)
{
	m_camID = id;
}
/*********************************************************************************************
 Class      : CIPCPictureRecord
 Function   : GetBitrate
 Description: returns the bitrate with witch the picture was recorded, 
              only valid for H.263 or MPEG

 Parameters: None 

 Result type: the bitrate as DWORD in bits per second (DWORD)
 created: June, 16 2005
 <TITLE GetBitrate>
*********************************************************************************************/
inline DWORD CIPCPictureRecord::GetBitrate() const
{
	return m_dwBitrate;
}
/*********************************************************************************************
 Class      : CIPCPictureRecord
 Function   : SetBitrate
 Description: sets the bitrate of picture in memory, not in shared memory,
              so you can use the new value only in the same process, not
              to send the CIPCPictureRecord to another process 

 Parameters:   
  dwBitrate: (E): the new bitrate in bits per second  (DWORD)

 Result type:  (inline void)
 created: June, 16 2005
 <TITLE SetBitrate>
*********************************************************************************************/
inline void CIPCPictureRecord::SetBitrate(DWORD dwBitrate)
{
	m_dwBitrate = dwBitrate;
}
inline WORD CIPCPictureRecord::GetBlockNr() const
{
	return (WORD)m_dwBlockNr;
}
inline DWORD CIPCPictureRecord::GetJobTime() const
{
	return m_dwJobTime;
}
inline CIPCSavePictureType CIPCPictureRecord::GetPictureType() const
{
	return m_pictureType;
}
inline const CIPC *CIPCPictureRecord::GetCIPC() const
{
	return m_pCipc;
}
inline const CString& CIPCPictureRecord::GetInfoString() const
{
	return m_sInfoString;
}
inline void	CIPCPictureRecord::SetInfoString(const CString& s)
{
	m_sInfoString = s;
}
inline const CSystemTime& CIPCPictureRecord::GetTimeStamp() const
{
	return m_stTimeStamp;
}
inline const CIPCExtraMemory* CIPCPictureRecord::GetBubble() const
{
	return m_pBubble;
}
inline const DWORD CIPCPictureRecord::GetNumberOfPictures() const
{
	return m_dwLengths.GetSize();
}
inline PBYTE CIPCPictureRecord::GetTimes() const
{
	ASSERT(m_pBubble);
	return (PBYTE)m_pBubble->GetAddress() + (7+m_dwLengths.GetSize())*sizeof(DWORD);
}

#endif
