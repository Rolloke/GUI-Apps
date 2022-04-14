//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_IMAGEDATA_H)
#define AFX_IMAGEDATA_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCollection;

class CImageData  
{
	// construction / destruction
public:
	CImageData(CSecID idCollection,
			   CIPCPictureRecord* pPict,
		       int iNumRecords,
			   const CIPCField fields[]);
	CImageData(CSecID idCollection,
		       int iNumRecords,
			   const CIPCField fields[]);
	CImageData(CSecID idCollection, 
		       CIPCMediaSampleRecord* pMedia,
		       int iNumRecords,
			   const CIPCField fields[]);
	virtual ~CImageData();

	// attributes
public:
	inline CSecID GetCollectionID() const;
	inline const CIPCPictureRecord* GetPictureRecord() const;
	inline const CIPCMediaSampleRecord* GetMediaSampleRecord() const;
	inline const CIPCFields& GetFields() const;
	BOOL				IsIFrame() const;
	DWORD				GetLength(int i=0) const;
	const CSystemTime&	GetTimeStamp() const;
	inline CSecID   GetDataID() const;

	// operations
public:

	// data
private:
	CSecID					m_idCollection;
	CIPCPictureRecord*		m_pPicture;
	CIPCMediaSampleRecord*	m_pMedia;
	CIPCFields				m_Fields;
	CSystemTime				m_stTime;
	CSecID					m_ID;
};
//////////////////////////////////////////////////////////////////////
inline CSecID CImageData::GetCollectionID() const
{
	return m_idCollection;
}
//////////////////////////////////////////////////////////////////////
inline const CIPCPictureRecord* CImageData::GetPictureRecord() const
{
	return m_pPicture;
}
//////////////////////////////////////////////////////////////////////
inline const CIPCMediaSampleRecord* CImageData::GetMediaSampleRecord() const
{
	return m_pMedia;
}
//////////////////////////////////////////////////////////////////////
inline const CIPCFields& CImageData::GetFields() const
{
	return m_Fields;
}
//////////////////////////////////////////////////////////////////////
inline CSecID CImageData::GetDataID() const
{
	return m_ID;
}
//////////////////////////////////////////////////////////////////////
typedef CImageData* CImageDataPtr;
WK_PTR_LIST_SEMA(CImageDataQueuePlain,CImageDataPtr,CImageDataQueue);
//////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_SAVEDATA_H__922AB921_7EBB_11D2_B503_004005A19028__INCLUDED_)
