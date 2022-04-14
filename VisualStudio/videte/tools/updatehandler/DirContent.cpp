/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: DirContent.cpp $
// ARCHIVE:		$Archive: /Project/Tools/UpdateHandler/DirContent.cpp $
// CHECKIN:		$Date: 13.12.05 12:01 $
// VERSION:		$Revision: 6 $
// LAST CHANGE:	$Modtime: 13.12.05 9:18 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#include "stdafx.h"
#include "UpdateHandler.h"
#include "DirContent.h"
#include "IPCInputFileUpdate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////////
CDirContent::CDirContent(const CString& sDir, const void* pData, DWORD dwDataLen)
{
	m_bContinued = FALSE;
#ifdef _UNICODE
	BOOL bUnicode = dwDataLen & FLAG_FILE_DATA_IS_UNICODE ? TRUE : FALSE;
	dwDataLen &= ~FLAG_FILE_DATA_IS_UNICODE;
	if (pData && (dwDataLen>0))
	{
		if (bUnicode)
		{
			DWORD dwFD = sizeof(WIN32_FIND_DATAW);
			int i;
			int iSize = dwDataLen/dwFD;
			LPWIN32_FIND_DATAW lpWFD;
			BYTE* pD = (BYTE*)pData;

			m_Files.SetSize(iSize);
			for (i=0;i<iSize;i++)
			{
				lpWFD = new WIN32_FIND_DATAW;
				CopyMemory(lpWFD,pD,dwFD);
				if (i==0)
				{
					if (lpWFD->dwFileAttributes & FILE_ATTR_CONTINUED_TRANSMISSION)
					{
						lpWFD->dwFileAttributes &= ~FILE_ATTR_CONTINUED_TRANSMISSION;
						m_bContinued = TRUE;
					}
				}
				m_Files.SetAt(i,lpWFD);
				pD += dwFD;
			}
		}
		else
		{
			DWORD dwFDa = sizeof(WIN32_FIND_DATAA);
			int i;
			int iSize = dwDataLen/dwFDa;
			LPWIN32_FIND_DATAW lpWFDw;
			BYTE* pD = (BYTE*)pData;

			m_Files.SetSize(iSize);
			for (i=0;i<iSize;i++)
			{
				lpWFDw = new WIN32_FIND_DATAW;
				CopyMemory(lpWFDw,pD,dwFDa);
				CWK_String sFileName(((WIN32_FIND_DATAA*)pD)->cFileName);
				_tcscpy(lpWFDw->cFileName, sFileName);
				CWK_String sAlternateFileName(((WIN32_FIND_DATAA*)pD)->cAlternateFileName);
				_tcscpy(lpWFDw->cAlternateFileName, sAlternateFileName);
				m_Files.SetAt(i,lpWFDw);
				pD += dwFDa;
			}
		}
		m_sDirectory = sDir;
	}
#else
	if (pData && (dwDataLen>0))
	{
		DWORD dwFD = sizeof(WIN32_FIND_DATA);
		int i;
		int iSize = dwDataLen/dwFD;
		LPWIN32_FIND_DATA lpWFD;
		BYTE* pD = (BYTE*)pData;

		m_Files.SetSize(iSize);
		for (i=0;i<iSize;i++)
		{
			lpWFD = new WIN32_FIND_DATA;
			CopyMemory(lpWFD,pD,dwFD);
			m_Files.SetAt(i,lpWFD);
			pD += dwFD;
		}
		m_sDirectory = sDir;
	}
#endif
}
/////////////////////////////////////////////////////////////////////////////////
CDirContent::~CDirContent()
{
	LPWIN32_FIND_DATA lpWFD;
	int i;

	for (i=0;i<m_Files.GetSize();i++)
	{
		lpWFD = (LPWIN32_FIND_DATA)m_Files[i];
		delete lpWFD;
	}

	m_Files.RemoveAll();
}
/////////////////////////////////////////////////////////////////////////////////
const int CDirContent::GetSize() const
{
	return m_Files.GetSize();
}
/////////////////////////////////////////////////////////////////////////////////
const CString& CDirContent::GetDirectory() const
{
	return m_sDirectory; 
}
/////////////////////////////////////////////////////////////////////////////////
const LPWIN32_FIND_DATA CDirContent::GetAt(int i) const
{
	if ( (i>=0) && (i< m_Files.GetSize()))
	{
		return (LPWIN32_FIND_DATA)m_Files[i];
	}
	else
	{
		return NULL;
	}
}
/////////////////////////////////////////////////////////////////////////////////
BOOL CDirContent::IsEmpty() const
{
	return 0==m_Files.GetSize();
}
/////////////////////////////////////////////////////////////////////////////////
BOOL  CDirContent::IsContinued() const
{
	return m_bContinued;
}
/////////////////////////////////////////////////////////////////////////////////
BOOL CDirContent::Remove(const CString& sFile)
{
	LPWIN32_FIND_DATA lpWFD;
	int i;

	for (i=0;i<m_Files.GetSize();i++)
	{
		lpWFD = (LPWIN32_FIND_DATA)m_Files[i];
		if (0==sFile.CompareNoCase(lpWFD->cFileName))
		{
			delete lpWFD;
			m_Files.RemoveAt(i);
			return TRUE;
		}
	}
	return FALSE;
}
