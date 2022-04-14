// CUpdateSystem.cpp: implementation of the CUpdateSystem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dvclient.h"
#include "cpanel.h"
#include "CUpdateSystem.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
CUpdateSystem::CUpdateSystem()
{
	m_pBuffer			= NULL;
	m_dwLen				= NULL;
	m_nUpdateDiskCounter= 0;
	m_dwTimeStamp		= 0;
	m_dwBuild			= 0;
	m_eOem				= OemDTS;
	m_bCDUpdate			= FALSE;
	m_bFDDUpdate		= FALSE;
	m_bRemoteUpdate		= FALSE;
}

/////////////////////////////////////////////////////////////////////////////
CUpdateSystem::~CUpdateSystem()
{
	delete []m_pBuffer;
	m_pBuffer = NULL;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CUpdateSystem::LoadFile(const CString& sSource)
{
	BOOL bResult = FALSE;
	TRY
	{
		CFile file(sSource, CFile::modeRead | CFile::shareDenyNone);
		
		DWORD dwLen = (DWORD)file.GetLength();
		if (dwLen > 0)
		{
			if (m_pBuffer)
			{
				delete []m_pBuffer;
				m_pBuffer = NULL;
			}

			m_pBuffer = new BYTE[dwLen];
			if (file.Read(m_pBuffer, dwLen) == dwLen)
			{
				m_dwLen = dwLen;
				bResult = TRUE;
			}
		}
	}
	CATCH(CFileException, e)
	{
		WK_TRACE_ERROR(_T("CUpdateSystem::LoadFile Exception %s\n"),GetLastErrorString(e->m_lOsError));
	}
	END_CATCH

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
DWORD CUpdateSystem::GetCRC()
{
	DWORD dwCRC = 0;
	
	if (m_pBuffer && (m_dwLen > 0))
	{
		dwCRC = 0;
		for (DWORD dwI = 0; dwI < m_dwLen; dwI++)
			dwCRC += m_pBuffer[dwI]*dwI;
	}
	
	return dwCRC;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CUpdateSystem::Decode()
{
	BOOL bResult = FALSE;
	
	if (m_pBuffer && (m_dwLen > 0))
	{
		for (DWORD dwI = 0; dwI < m_dwLen; dwI++)
			m_pBuffer[dwI] ^= 0x85;
		bResult = TRUE;
	}
	
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CUpdateSystem::SaveFile(const CString& sTarget)
{
	BOOL bResult = FALSE;
	
	TRY
	{
		// Evtl. Schreibschutz entfernen
		SetFileAttributes(sTarget, FILE_ATTRIBUTE_NORMAL);
	
		// Datei schreiben
		CFile file(sTarget, CFile::modeWrite | CFile::modeCreate | CFile::shareDenyNone);
		if (m_pBuffer && (m_dwLen > 0))
		{
			file.Write(m_pBuffer, m_dwLen);
			bResult = TRUE;
		}
	}
	CATCH(CFileException, e)
	{
		WK_TRACE_ERROR(_T("CUpdateSystem::SaveFile Exception %s\n"),GetLastErrorString(e->m_lOsError));
	}
	END_CATCH

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CUpdateSystem::StartUpdate(OEM eOem)
{
	m_nUpdateDiskCounter = 1;
	m_eOem = eOem;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
int CUpdateSystem::ContinueUpdate()
{
	CString sIniFileName;
	CString sTarget;
	CString sTempDir;
	CString sSource;
	int		nResult = UPDATE_NO_UPDATE;

	sIniFileName.Format(_T("a:\\Disk%d\\Disk"), m_nUpdateDiskCounter);
	sSource.Format(_T("a:\\Disk%d\\Update"), m_nUpdateDiskCounter);
	if (DoesFileExist(sSource))
	{
		m_bFDDUpdate = TRUE;
	}
	else
	{
		sIniFileName.Format(_T("a:\\Disk"));
		sSource.Format(_T("a:\\Update"));
		if (DoesFileExist(sSource))
		{
			m_bFDDUpdate = TRUE;
		}
		else
		{
			BOOL bUpdateFoundOnCD = FALSE;
			DWORD dwLD = GetLogicalDrives();
			DWORD dwBit = 4;
			char c = _T('c');

			while (dwBit!=0)
			{
				if (dwLD & dwBit)
				{
					CString sRoot;
					sRoot.Format(_T("%c:\\"),c);
					
//					if (DRIVE_CDROM == GetDriveType(sRoot))
					if (DRIVE_CDROM == GetDriveType(sRoot) || (GetDriveType(sRoot) == DRIVE_REMOVABLE))
					{
						sSource.Format(_T("%c:\\Disk%d\\Update"),c,m_nUpdateDiskCounter);
						if (DoesFileExist(sSource))
						{
							sIniFileName.Format(_T("%c:\\Disk%d\\Disk"),c,m_nUpdateDiskCounter);
							bUpdateFoundOnCD = TRUE;
							m_bCDUpdate = TRUE;
							break;
						}
					}

				}
				dwBit <<= 1;
				c++;
			}

			if (!bUpdateFoundOnCD)
			{
				return UPDATE_NO_UPDATE_DISK;
			}
		}
	}

	int nOem			= GetPrivateProfileInt(REGKEY_SETUP, REGKEY_OEM,		-1, sIniFileName);
	int nThisDisk		= GetPrivateProfileInt(REGKEY_SETUP, _T("ThisDisk"),	-1, sIniFileName);
	int nNextDisk		= GetPrivateProfileInt(REGKEY_SETUP, _T("NextDisk"),	-1, sIniFileName);
	DWORD dwCRC			= GetPrivateProfileInt(REGKEY_SETUP, _T("CRC"),		-1, sIniFileName);
	DWORD dwBuild		= GetPrivateProfileInt(REGKEY_SETUP, _T("Build"),	-1, sIniFileName);
	DWORD dwTimeStamp	= GetPrivateProfileInt(REGKEY_SETUP, _T("TimeStamp"),-1, sIniFileName);

	// nOem=-1 bedeutet ignoriere den Oem!
	if ((nOem != -1 ) && (nOem != m_eOem))
		nResult = UPDATE_WRONG_OEM;
	else
	{
		// Stimmt die Diskettennummer?
		if (nThisDisk != m_nUpdateDiskCounter)
			nResult = UPDATE_WRONG_DISK;
		else	// Diskettennummer ok!
		{		
			::GetTempPath(_MAX_PATH,sTempDir.GetBuffer(_MAX_PATH));
			sTempDir.ReleaseBuffer();
			
			// Zielfilename ermitteln
			if (m_nUpdateDiskCounter == 1)
			{
				sTarget.Format(_T("%sSetup.exe"), sTempDir);
				m_dwTimeStamp = GetPrivateProfileInt(REGKEY_SETUP, _T("TimeStamp"),	-1, sIniFileName);
				m_dwBuild	  = GetPrivateProfileInt(REGKEY_SETUP, _T("Build"),		-1, sIniFileName);
			}
			else
			{
				sTarget.Format(_T("%sSetup.w%02d"), sTempDir, m_nUpdateDiskCounter);
			}

			// Buildnummer und Timestamp korrekt.
			if ((dwBuild != m_dwBuild) || (dwTimeStamp != m_dwTimeStamp))
				nResult = UPDATE_WRONG_BUILD;
			else
			{
				if (!LoadFile(sSource))
					nResult = UPDATE_DISK_READ_FAILED;
				else
				{
					if (!Decode())
						nResult = UPDATE_DECODE_FAILED;
					else
					{
						// Stimmt die Prüfsumme?
						if (dwCRC != GetCRC())
							nResult = UPDATE_WRONG_CRC;
						else	// Prüfsumme ok!
						{
							WK_TRACE(_T("Copy %s to %s\n"), sSource, sTarget);
							
							// Datei ins Tempverzeichnis kopieren
							if (!SaveFile(sTarget))
								nResult = UPDATE_COPY_FAILED;
							else
							{
								m_nUpdateDiskCounter = nNextDisk;
								
								// Letzte Diskette?
								if (nNextDisk == 0)
									nResult = UPDATE_LAST_DISK;
								else // Nicht die letze Diskette!
									nResult = UPDATE_NEXT_DISK;
							}
						}
					}
				}
			}
		}
	}
	return nResult;
}
