// SaveData.h: interface for the CSaveData class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_SAVEDATA_H__922AB921_7EBB_11D2_B503_004005A19028__INCLUDED_)
#define AFX_SAVEDATA_H__922AB921_7EBB_11D2_B503_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CArchiv;

class CSaveData  
{
	// construction / destruction
public:
	CSaveData(CArchiv* pArchiv, 
			  const CIPCPictureRecord& pict);
	CSaveData(CArchiv* pArchiv, 
		      const CIPCPictureRecord& pict,
		      int iNumRecords,
			  const CIPCField fields[]);
	CSaveData(CArchiv* pArchiv, 
		      const CIPCMediaSampleRecord& media,
		      int iNumRecords,
			  const CIPCField fields[]);
	CSaveData(CArchiv* pArchiv, 
			  int iNumRecords,
			  const CIPCField fields[]);
	virtual ~CSaveData();

	// attributes
public:
	inline CArchiv* GetArchiv() const;
	inline CSecID   GetDataID() const;
	inline const CIPCPictureRecord* GetPictureRecord() const;
	inline const CIPCMediaSampleRecord* GetMediaSampleRecord() const;
	inline CIPCFields& GetFields();
	inline const CString& GetSequenceName() const;
	inline DWORD   GetFileError() const;

	BOOL				IsIFrame() const;
	DWORD				GetDataLength() const;
	const BYTE*			GetData() const;
	const CSystemTime&	GetTimeStamp() const;

	// operations
public:
	BOOL Save(BOOL& bNewSequenceCreated);

	// data
private:
	CArchiv* 				m_pArchiv;
	CIPCPictureRecord*		m_pPicture;
	CIPCMediaSampleRecord*	m_pMedia;
	CIPCFields				m_Fields;
	CString					m_sSequenceName;
	CSystemTime				m_stTime;
	CSecID					m_ID;
	DWORD					m_dwDataLength;
	DWORD					m_dwFileError;
};
//////////////////////////////////////////////////////////////////////
inline CArchiv* CSaveData::GetArchiv() const
{
	return m_pArchiv;
}
//////////////////////////////////////////////////////////////////////
inline const CIPCPictureRecord* CSaveData::GetPictureRecord() const
{
	return m_pPicture;
}
//////////////////////////////////////////////////////////////////////
inline const CIPCMediaSampleRecord* CSaveData::GetMediaSampleRecord() const
{
	return m_pMedia;
}
//////////////////////////////////////////////////////////////////////
inline CIPCFields& CSaveData::GetFields()
{
	return m_Fields;
}
//////////////////////////////////////////////////////////////////////
inline const CString& CSaveData::GetSequenceName() const
{
	return m_sSequenceName;
}
//////////////////////////////////////////////////////////////////////
inline CSecID CSaveData::GetDataID() const
{
	return m_ID;
}
inline DWORD CSaveData::GetFileError() const
{
	return m_dwFileError;
}
//////////////////////////////////////////////////////////////////////
typedef CSaveData* CSaveDataPtr;
WK_PTR_LIST_SEMA(CSaveDataQueuePlain,CSaveDataPtr,CSaveDataQueue)
//////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_SAVEDATA_H__922AB921_7EBB_11D2_B503_004005A19028__INCLUDED_)
