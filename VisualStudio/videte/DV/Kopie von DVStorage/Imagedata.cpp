// ImageData.cpp: implementation of the CImageData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dvstorage.h"
#include "ImageData.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CImageData::CImageData(CSecID idCollection,
					   CIPCPictureRecord* pPict,
					   int iNumRecords,
					   const CIPCField fields[])
{
	m_idCollection = idCollection;
	m_pPicture = pPict;
	m_pMedia = NULL;
	m_stTime = m_pPicture->GetTimeStamp();
	m_ID = m_pPicture->GetCamID();
	for (int i=0;i<iNumRecords;i++)
	{
		m_Fields.SafeAdd(new CIPCField(fields[i]));
	}
}
//////////////////////////////////////////////////////////////////////
CImageData::CImageData(CSecID idCollection,
					   int iNumRecords,
					   const CIPCField fields[])
{
	m_idCollection = idCollection;
	m_pPicture = NULL;
	m_pMedia = NULL;
	for (int i=0;i<iNumRecords;i++)
	{
		m_Fields.SafeAdd(new CIPCField(fields[i]));
	}
}
//////////////////////////////////////////////////////////////////////
CImageData::CImageData(CSecID idCollection, 
					   CIPCMediaSampleRecord* pMedia,
					   int iNumRecords,
					   const CIPCField fields[])
{
	m_idCollection = idCollection;
	m_pPicture = NULL;
	m_pMedia = pMedia;
	m_ID = m_pMedia->GetID();
	m_pMedia->GetMediaTime(m_stTime);
	for (int i=0;i<iNumRecords;i++)
	{
		m_Fields.SafeAdd(new CIPCField(fields[i]));
	}
}
//////////////////////////////////////////////////////////////////////
CImageData::~CImageData()
{
	WK_DELETE(m_pPicture);
	WK_DELETE(m_pMedia);
}
//////////////////////////////////////////////////////////////////////
BOOL CImageData::IsIFrame() const
{
	if (m_pPicture)
	{
		return m_pPicture->GetPictureType() == SPT_FULL_PICTURE || m_pPicture->GetPictureType() == SPT_GOP_PICTURE;
	}
	else if(m_pMedia)
	{
		return m_pMedia->IsLongHeader();
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
DWORD CImageData::GetLength(int i) const
{
	if (m_pPicture)
	{
		return m_pPicture->GetDataLength(i);
	}
	else if(m_pMedia)
	{
		return m_pMedia->GetBubble()->GetLength();
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////
const CSystemTime& CImageData::GetTimeStamp() const
{
	return m_stTime;
}
