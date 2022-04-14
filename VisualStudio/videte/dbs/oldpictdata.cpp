/* GlobalReplace: PictData6 --> COldPictData */
/* GlobalReplace: Header6Entry --> COldPictDataHeaderRecord */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: oldpictdata.cpp $
// ARCHIVE:		$Archive: /Project/DBS/oldpictdata.cpp $
// CHECKIN:		$Date: 20.01.06 9:38 $
// VERSION:		$Revision: 7 $
// LAST CHANGE:	$Modtime: 19.01.06 21:27 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"

#include "OldPictData.h"

void COldPictDataHeaderRecord::AlreadyInUseError() const
{
	WK_TRACE_ERROR(_T("COldPictDataHeaderRecord already in use\n"));
}


////////////////////////////////////////////////////////////////////////


COldPictData::COldPictData()
{
	m_dwPictCount	=	0;
	m_OpenTyp		=	CLOSED;
}

COldPictData::~COldPictData()
{
	if (m_hFile != CFile::hFileNull){
		Close();
	}
}

BOOL COldPictData::IsFile(LPCTSTR szName)
{
    WIN32_FIND_DATA FindFileData;
	HANDLE h = FindFirstFile(szName, &FindFileData);
	FindClose(h);
	return (h != (HANDLE)INVALID_HANDLE_VALUE);
}

CString COldPictData::ConstructFileName(LPCTSTR szName)
{
	CString s = szName;
	int index = s.ReverseFind('.');
	if (index != -1){
		s = s.Left(index);
		s += ".DBX";
	}else{
//		WK_ASSERT(0);
	}
	return s;
}

BOOL COldPictData::MyOpen(LPCTSTR szName)
{
	BOOL bReadOnly = TRUE;
	CString sTemp = ConstructFileName(szName);
	if (m_hFile != CFile::hFileNull){
		if (m_sName == sTemp){
			return TRUE;
		}
		else{
			//DB_ERROR
			WK_TRACE(_T("MyOpen error in %s\n"),szName);
			return FALSE;
		}
	}
	m_sName = sTemp;

    WIN32_FIND_DATA FindFileData;
	HANDLE h = FindFirstFile(m_sName, &FindFileData);
	FindClose(h);
	
	if ((h == (HANDLE)INVALID_HANDLE_VALUE)){
		WK_TRACE_ERROR(_T("Could not open '%s'\n"),m_sName);
		return FALSE;
	}

	// Direct CD
	UINT nFlags = 0;
	if (bReadOnly)
	{
		nFlags = modeRead | shareDenyNone | typeBinary;
	}
	else
	{
		nFlags = modeReadWrite | shareDenyNone | typeBinary;
	}

	if (Open(m_sName, nFlags))
	{
		//WK_TRACE(_T("Datei geˆffnet: %s\n"), (const char*)m_sName);
		m_OpenTyp = OPENED;
		if (FindFileData.nFileSizeHigh) {
			WK_TRACE(_T("Have highFileSize %d\n"),FindFileData.nFileSizeHigh);
		}
		m_dwFileSize = FindFileData.nFileSizeLow;
		WK_ASSERT (m_dwFileSize >= HeaderSize());
		ReadHeader();
		return TRUE;
	}
	return FALSE;
}


DWORD COldPictData::GetPictData (DWORD dwNr, void* pData)
{
	WK_ASSERT(m_OpenTyp==OPENED);
	if (!Header[dwNr-1].IsUsed()){
		return 0;
	}
	DWORD	dwPosition	= Header[dwNr-1].Position();
	DWORD	dwLen		= Header[dwNr-1].Len();

	if (dwLen > 100*1024) {
		WK_TRACE(_T("Oversized PictData %d in %s\n"),dwLen,m_sName);
	}

	if (dwPosition > m_dwFileSize) {
		WK_TRACE(_T("Invalid Position %d > %d \n"),dwPosition, m_dwFileSize);
	}

	TRY
	{
		Seek(dwPosition, CFile::begin);
		Read(pData, dwLen);
	}
	CATCH( CFileException, e )
	{
		WK_TRACE(_T("Exception: COldPictData::GetPictData()  %s, %s"), 
					GetFileName(),  
					GetLastErrorString(e->m_lOsError));
		if (e->m_cause == CFileException::hardIO)
		{
			//DB_ERROR
			WK_TRACE(_T("Fehler beim Lesen eines Bildes\n"));
		}
		else
		{
			//DB_ERROR
			WK_TRACE(_T("Fehler beim Lesen eines Bildes \n"));
		}
	}
	END_CATCH
	return dwLen;
}

DWORD COldPictData::GetPictLen	(DWORD dwNr)
{
	WK_ASSERT(m_OpenTyp==OPENED);
	WK_ASSERT(dwNr < MAX_PICT_IN_FILE);
	WK_ASSERT(dwNr > 0);
	return Header[dwNr-1].Len();
}

UINT COldPictData::HeaderSize()
{
	return (MAX_PICT_IN_FILE*12);
}



void COldPictData::ReadHeader()
{
	WK_ASSERT(m_OpenTyp==OPENED);
	TRY
	{
		SeekToBegin();
		// CAVEAT order of members is important here
		// It is a direct read.
		Read(&Header, HeaderSize());
		m_dwPictCount = 0;

		for (int i=0; i < MAX_PICT_IN_FILE; i++)
		{
			if (Header[i].IsUsed()){
				m_dwPictCount++;
			}
		}
	}
	CATCH( CFileException, e )
	{
		CString sLE = GetLastErrorString(e->m_lOsError); 
		WK_TRACE(_T("Exception: COldPictData::ReadHeader()  %s, %s"), 
					GetFileName(),  
					GetLastErrorString(e->m_lOsError));
		if (e->m_cause == CFileException::hardIO)
		{
			//DB_ERROR
			WK_TRACE(_T("Fehler beim Lesen einer Bildinformation %s\n"),sLE);
		}
		else
		{
			//DB_ERROR
			WK_TRACE(_T("Fehler beim Lesen einer Bildinformation %s\n"),sLE);
		}
	}
	END_CATCH
}

void COldPictData::Close()
{
	for (int i=0; i < MAX_PICT_IN_FILE; i++)
	{
		Header[i].Reset();
	}
	
	m_OpenTyp = CLOSED;
	m_dwPictCount=0;
	TRY
	{
		CFile::Close();
	}
	CATCH( CFileException, e )
	{
		CString sLE = GetLastErrorString(e->m_lOsError); 
		WK_TRACE(_T("Exception: COldPictData::Close()  %s, %s"), 
					GetFileName(),  
					GetLastErrorString(e->m_lOsError));
		if (e->m_cause == CFileException::hardIO)
		{
			//DB_ERROR
			WK_TRACE(_T("Fehler beim Schlieﬂen einer Datei %s"), sLE);
		}
		else
		{
			//DB_ERROR
			WK_TRACE(_T("Fehler beim Schlieﬂen einer Datei %s\n"),sLE);
		}
	}
	END_CATCH
	//WK_TRACE(_T("COldPictData::Close(%s)\n"), (const char*)m_sName);
}

