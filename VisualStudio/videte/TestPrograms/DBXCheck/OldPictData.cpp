/* GlobalReplace: PictData6 --> COldPictData */
/* GlobalReplace: Header6Entry --> COldPictDataHeaderRecord */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: OldPictData.cpp $
// ARCHIVE:		$Archive: /Project/Tools/TstTools/DBXCheck/OldPictData.cpp $
// CHECKIN:		$Date: 8.09.98 12:45 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 8.09.98 12:45 $
// BY AUTHOR:	$Author: Hedu $
// $Nokeywords:$

#include "stdafx.h"

#include "OldPictData.h"
#include "util.h"
#include "DBXCheck.h"
void COldPictDataHeaderRecord::AlreadyInUseError() const
{
	WK_TRACE_ERROR("COldPictDataHeaderRecord already in use\n");
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

BOOL COldPictData::IsFile(const char *szName)
{
    WIN32_FIND_DATA FindFileData;
	HANDLE h = FindFirstFile(szName, &FindFileData);
	FindClose(h);
	return (h != INVALID_HANDLE_VALUE);
}

CString COldPictData::ConstructFileName(const char *szName)
{
	CString s = szName;
	int index = s.ReverseFind('.');
	if (index != -1){
		s = s.Left(index);
		s += ".DBX";
	}else{
		WK_ASSERT(0);
	}
	return s;
}

BOOL COldPictData::MyOpen(const char *szName)
{
	BOOL bReadOnly = TRUE;
	CString sTemp = ConstructFileName(szName);
	if (m_hFile != CFile::hFileNull){
		if (m_sName == sTemp){
			return TRUE;
		}
		else{
			//DB_ERROR
			MY_WK_TRACE("MyOpen error in %s\n",szName);
			return FALSE;
		}
	}
	m_sName = sTemp;

    WIN32_FIND_DATA FindFileData;
	HANDLE h = FindFirstFile(m_sName, &FindFileData);
	FindClose(h);
	
	if ((h == INVALID_HANDLE_VALUE)){
		WK_TRACE_ERROR("Could not open '%s'\n",m_sName);
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
		//WK_TRACE("Datei geˆffnet: %s\n", (const char*)m_sName);
		m_OpenTyp = OPENED;
		if (FindFileData.nFileSizeHigh) {
			MY_WK_TRACE("Have highFileSize %d\n",FindFileData.nFileSizeHigh);
		}
		m_dwFileSize = FindFileData.nFileSizeLow;
		WK_ASSERT (m_dwFileSize >= HeaderSize());
		ReadHeader();
		return TRUE;
	}
	WK_ASSERT(0);
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
		MY_WK_TRACE("Oversized PictData %d in %s\n",dwLen,m_sName);
	}

	if (dwPosition > m_dwFileSize) {
		MY_WK_TRACE("Invalid Position %d > %d \n",dwPosition, m_dwFileSize);
	}

	TRY
	{
		Seek(dwPosition, CFile::begin);
		Read(pData, dwLen);
	}
	CATCH( CFileException, e )
	{
		CString sLE = GetLastErrorString(e->m_lOsError); 
		MY_WK_TRACE("Exception: COldPictData::GetPictData()  %s, %s", 
			(const char*)GetFileName(),  
			(const char*)GetLastErrorString(e->m_lOsError));
		if (e->m_cause == CFileException::hardIO)
		{
			//DB_ERROR
			MY_WK_TRACE("Fehler beim Lesen eines Bildes\n");
		}
		else
		{
			//DB_ERROR
			MY_WK_TRACE("Fehler beim Lesen eines Bildes \n");
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
		MY_WK_TRACE("Exception: COldPictData::ReadHeader()  %s, %s", 
			(const char*)GetFileName(),  
			(const char*)GetLastErrorString(e->m_lOsError));
		if (e->m_cause == CFileException::hardIO)
		{
			//DB_ERROR
			MY_WK_TRACE("Fehler beim Lesen einer Bildinformation %s\n",sLE);
		}
		else
		{
			//DB_ERROR
			MY_WK_TRACE("Fehler beim Lesen einer Bildinformation %s\n",sLE);
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
		MY_WK_TRACE("Exception: COldPictData::Close()  %s, %s", 
			(const char*)GetFileName(),  
			(const char*)GetLastErrorString(e->m_lOsError));
		if (e->m_cause == CFileException::hardIO)
		{
			//DB_ERROR
			MY_WK_TRACE("Fehler beim Schlieﬂen einer Datei %s", sLE);
		}
		else
		{
			//DB_ERROR
			MY_WK_TRACE("Fehler beim Schlieﬂen einer Datei %s\n",sLE);
		}
	}
	END_CATCH
	//WK_TRACE("COldPictData::Close(%s)\n", (const char*)m_sName);
}

