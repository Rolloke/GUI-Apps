// SaveData.cpp: implementation of the CSaveData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dbs.h"
#include "SaveData.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CSaveData::CSaveData(CArchiv* pArchiv, const CIPCPictureRecord& pict)
{
	m_pPicture = new CIPCPictureRecord(pict);
//	m_pPicture->ReleaseBubble();

	m_stTime = pict.GetTimeStamp();
	m_pMedia = NULL;
	m_pArchiv = pArchiv;
	m_Fields.FromString(pict.GetInfoString());
	m_ID = pict.GetCamID();
	m_dwDataLength = pict.GetDataLength(-1);
	m_dwFileError = 0;
	// m_sSequenceName
}
//////////////////////////////////////////////////////////////////////
CSaveData::CSaveData(CArchiv* pArchiv, 
					  const CIPCPictureRecord& pict,
					  int iNumRecords,
					  const CIPCField fields[])
{
	m_pPicture = new CIPCPictureRecord(pict);
	m_dwDataLength = pict.GetDataLength(-1);
	m_dwFileError = 0;
	m_ID = pict.GetCamID();
	m_stTime = pict.GetTimeStamp();
	m_pMedia = NULL;
	m_pArchiv = pArchiv;
	for (int i=0;i<iNumRecords;i++)
	{
		if (fields[i].GetName()==CIPCField::m_sfSqNm)
		{
			m_sSequenceName = fields[i].GetValue();
		}
		else
		{
			m_Fields.SafeAdd(new CIPCField(fields[i]));
		}
	}
}
//////////////////////////////////////////////////////////////////////
CSaveData::CSaveData(CArchiv* pArchiv, 
					  const CIPCMediaSampleRecord& media,
					  int iNumRecords,
					  const CIPCField fields[])
{
	m_pArchiv = pArchiv;
	m_pPicture = NULL;
	m_pMedia = new CIPCMediaSampleRecord(media);
	m_ID = m_pMedia->GetID();
	m_pMedia->GetMediaTime(m_stTime);
	m_dwDataLength = m_pMedia->GetBubble()->GetLength();
	m_dwFileError = 0;

	for (int i=0;i<iNumRecords;i++)
	{
		if (fields[i].GetName()==CIPCField::m_sfSqNm)
		{
			m_sSequenceName = fields[i].GetValue();
		}
		else
		{
			m_Fields.SafeAdd(new CIPCField(fields[i]));
		}
	}
}
//////////////////////////////////////////////////////////////////////
CSaveData::CSaveData(CArchiv* pArchiv, 
					int iNumRecords,
					const CIPCField fields[])
{
	m_pArchiv = pArchiv;
	m_pPicture = NULL;
	m_pMedia =NULL;
	m_dwDataLength = NULL;
	m_dwFileError = 0;

	for (int i=0;i<iNumRecords;i++)
	{
		TRACE(_T("%s %s\n"),fields[i].GetName(),fields[i].GetValue());
		if (fields[i].GetName()==CIPCField::m_sfSqNm)
		{
			m_sSequenceName = fields[i].GetValue();
		}
		else 
		{
			if (fields[i].GetName() == CString(CArchiv::m_sDATE))
			{
				m_stTime.SetDBDate(fields[i].GetValue());
			}
			if (fields[i].GetName() == CString(CArchiv::m_sTIME))
			{
				m_stTime.SetDBTime(fields[i].GetValue());
			}
			if (fields[i].GetName() == CString(CArchiv::m_sMS))
			{
				DWORD dwMS = 0;
				CString s = fields[i].GetValue();
				_stscanf((LPCTSTR)s,_T("%d"),&dwMS);
				m_stTime.wMilliseconds = (WORD)dwMS;
			}
			m_Fields.SafeAdd(new CIPCField(fields[i]));
		}
	}
}
//////////////////////////////////////////////////////////////////////
CSaveData::~CSaveData()
{
	WK_DELETE(m_pPicture);
	WK_DELETE(m_pMedia);
}
//////////////////////////////////////////////////////////////////////
BOOL CSaveData::Save(BOOL& bNewSequenceCreated)
{
	CCallStackEntry cse(_T("CSaveData::Save"));
	BOOL bRet = FALSE;
	if (m_pArchiv)
	{
		TRY
		{
			bRet = m_pArchiv->SaveData(*this,bNewSequenceCreated);
		}
		CATCH(CFileException, cfe)
		{
			WORD wDrive = 0;
			if (    m_pArchiv->GetFixedDrive().IsEmpty()
				&& !cfe->m_strFileName.IsEmpty())
			{
				cfe->m_strFileName.MakeLower();
				wDrive = cfe->m_strFileName.GetAt(0);
			}
			m_dwFileError = MAKELONG(cfe->m_lOsError, wDrive);
			WK_TRACE(_T("FileException in %s, Error:%d, %d, %s\n"), _T(__FUNCTION__), cfe->m_lOsError, cfe->m_cause, cfe->m_strFileName); 
			bRet = FALSE;
		}
		END_CATCH
	}
	else
	{
		WK_TRACE_ERROR(_T("no archive to save image data\n"));
		bRet = FALSE;
	}
	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CSaveData::IsIFrame() const
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
const CSystemTime& CSaveData::GetTimeStamp() const
{
	return m_stTime;
}
//////////////////////////////////////////////////////////////////////
DWORD CSaveData::GetDataLength() const
{
	return m_dwDataLength;
}
//////////////////////////////////////////////////////////////////////
const BYTE*	CSaveData::GetData() const
{
	if (m_pPicture)
	{
		return m_pPicture->GetData();
	}
	else if(m_pMedia)
	{
		return (const BYTE*)m_pMedia->GetBubble()->GetAddress();
	}
	return NULL;
}
