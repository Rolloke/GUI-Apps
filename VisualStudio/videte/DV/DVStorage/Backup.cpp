// Backup.cpp: implementation of the CBackup class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DVStorage.h"
#include "Backup.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBackup::CBackup(DWORD dwUserData,
				 DWORD dwClientID,
				 const CString& sDestinationPath,
				 const CString& sName,
				 int iNumIDs,
				 const DWORD dwIDs[])
{
	Init(dwUserData,dwClientID,sDestinationPath,sName,iNumIDs,dwIDs);
}
//////////////////////////////////////////////////////////////////////
CBackup::CBackup(WORD wUserData,
				 DWORD dwClientID,
				 const CString& sDestinationPath,
				 const CString& sName,
				 int iNumIDs,
				 const DWORD dwIDs[],
				 const CSystemTime& start,
				 const CSystemTime& end,
				 WORD  wFlags,
				 DWORD dwMaximumInMB)
{
	Init(wUserData,dwClientID,sDestinationPath,sName,iNumIDs,dwIDs,wFlags,dwMaximumInMB);
	m_stStart = start;
	if (wFlags == BBT_CALCULATE_END)
	{
		m_stEnd.GetLocalTime();
	}
	else
	{
		m_stEnd = end;
	}
}
//////////////////////////////////////////////////////////////////////
void CBackup::SetBackupThread(CBackupThread* pBackupThread)
{
	m_pBackupThread = pBackupThread;
}
//////////////////////////////////////////////////////////////////////
void CBackup::Init(DWORD dwUserData,
				   DWORD dwClientID,
				   const CString& sDestinationPath,
				   const CString& sName,
				   int iNumIDs,
				   const DWORD dwIDs[],
				   WORD wFlags /*= 0*/,
				   DWORD dwMaximumInMB /* = 0*/)
{
	m_pBackupThread = NULL;
	m_bInitialized = FALSE;
	m_dwClientID = dwClientID;
	m_dwUserData = dwUserData;
	m_sDestinationPath = sDestinationPath;
	m_sName = sName;
	m_wFlags = wFlags;
	m_dwMaximumInMB = dwMaximumInMB;
	m_dwLastConfirm = 0;
	m_bTrace = theApp.m_bTraceBackup;

	for (int i=0;i<iNumIDs;i++)
	{
		DWORD dwID = dwIDs[i];
		WORD wCollectionNr   = HIWORD(dwID);
		WORD wTapeNr = LOWORD(dwID);
		theApp.m_Archives.Lock();
		CCollection* pCollection = theApp.m_Archives.GetCollection(wCollectionNr);
		if (pCollection==NULL)
		{
			// invalid ingore it
			WK_TRACE_ERROR(_T("invalid Collection to backup %d\n"),wCollectionNr);
		}
		else
		{
			if (   wTapeNr==0
				&& (m_wFlags != BBT_CALCULATE)
				&& (m_wFlags != BBT_CALCULATE_END)
				)
			{
				// backup the entire Collection
				for (int j=0;j<pCollection->GetNrTapes();j++)
				{
					CTape* pTape = pCollection->GetTapeIndex(j);
					if (   pTape
						&& !pTape->IsBackup())
					{
						wTapeNr = pCollection->GetTapeIndex(j)->GetNr();
						dwID = MAKELONG(wTapeNr,wCollectionNr);
						m_IDs.Add(dwID);
					}
				}
				if (pCollection->GetNrTapes()>0)
				{
					m_LastTapes.Add(dwID);
				}
			}
			else
			{
				m_IDs.Add(dwID);
			}
		}
		theApp.m_Archives.Unlock();
	}
}
//////////////////////////////////////////////////////////////////////
CBackup::~CBackup()
{

}
//////////////////////////////////////////////////////////////////////
BOOL CBackup::IsBackupToRWMedium() const
{
	CString sRoot = m_sDestinationPath.Left(3);
	UINT type = GetDriveType(sRoot);

	return    (type == DRIVE_REMOVABLE)
		   || (type == DRIVE_FIXED)
		   || (type == DRIVE_REMOTE);
}
//////////////////////////////////////////////////////////////////////
DWORD CBackup::GetIDNr(int iIndex) const
{
	if (iIndex>=0 && iIndex<m_IDs.GetSize())
	{
		return m_IDs.GetAt(iIndex);
	}
	else
	{
		return 0;
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CBackup::Initialize()
{
	// create a directory on path and so on

	WK_TRACE(_T("CBackup::Initialize()\n"));

	CString sDir;
	CString sTest;

	m_sDirectory.Empty();

	for (WORD i=1;i<0xFFFF;i++)
	{
		sDir.Format(_T("dvb%04lx"),i);
		sTest = m_sDestinationPath + sDir;

		if (!DoesFileExist(sTest))
		{
			m_sDirectory = sTest;
			break;
		}
	}

	if (m_sDirectory.IsEmpty())
	{
		return FALSE;
	}

	if (!CreateDirectory(m_sDirectory,NULL))
	{
		WK_TRACE_ERROR(_T("cannot create directory %s, %s\n"),
			m_sDirectory,GetLastErrorString());
		return FALSE;
	}
	else
	{
		WK_TRACE(_T("mdir SUCCESS %s\n"),sDir);
	}

	m_bInitialized = TRUE;

	return TRUE;
}
///////////////////////////////////////////////////////////////////////
BOOL CBackup::WasLastTapeOfCollection(WORD wCollection, WORD wTape)
{
	for (int i=0;i<m_LastTapes.GetSize();i++)
	{
		DWORD dwID = m_LastTapes.GetAt(i);
		WORD wCollectionNr   = HIWORD(dwID);
		WORD wTapeNr = LOWORD(dwID);
		if (   (wCollection==wCollectionNr)
			&& (wTapeNr==wTape))
		{
			return TRUE;
		}
	}
	return FALSE;
}
///////////////////////////////////////////////////////////////////////
BOOL CBackup::CleanUp()
{
	CIPCDrive* pDrive = theApp.m_Drives.GetDrive(m_sDestinationPath);
	if (pDrive && !pDrive->IsCDROM())
	{
		// we can delete all copied files
		WK_TRACE(_T("rmdir %s by backup cancelled\n"),m_sDirectory);
		if (DeleteDirRecursiv(m_sDirectory))
		{
			WK_TRACE(_T("%s by backup cancelled deleted\n"),m_sDirectory);
		}
		else
		{
			WK_TRACE_ERROR(_T("cannot delete %s\n"),m_sDirectory);
		}
	}
	return TRUE;
}
///////////////////////////////////////////////////////////////////////
void CBackup::IncreaseCopiedBytes(WORD wCollection, WORD wTape, FILESIZE dwBytes,WORD wFlags)
{
	m_iCopiedBytes += dwBytes;

	if (wFlags & BBT_PROGRESS)
	{
		// nur alle zehntel Sekunde confirmen
		if (   m_dwLastConfirm == 0
			|| GetTimeSpan(m_dwLastConfirm)>100)
		{
			CClient* pClient = theApp.m_Clients.GetClient(m_dwClientID);
			if (pClient && pClient->IsConnected())
			{
				CIPCDatabaseStorage* pCIPCDatabase = pClient->GetCIPC();
				DWORD dwID = MAKELONG(wTape,wCollection);
				pCIPCDatabase->DoConfirmBackupByTime((WORD)GetUserData(),
													 dwID,
													 m_stStart,
													 m_stEnd,
													 wFlags,
													 m_iCopiedBytes);
			}
			m_dwLastConfirm = GetTickCount();
		}
	}
	else
	{
		CClient* pClient = theApp.m_Clients.GetClient(m_dwClientID);
		if (pClient && pClient->IsConnected())
		{
			CIPCDatabaseStorage* pCIPCDatabase = pClient->GetCIPC();
			DWORD dwID = MAKELONG(wTape,wCollection);
			pCIPCDatabase->DoConfirmBackupByTime((WORD)GetUserData(),
												 dwID,
												 m_stStart,
												 m_stEnd,
												 wFlags,
												 m_iCopiedBytes);
		}
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CBackup::CopyFileTrace(WORD wCollection, 
							WORD wTape, 
							const CString& source, 
							const CString& dest, 
							FILESIZE dwMaxRead/*=-1*/)
{
	BOOL bRet = TRUE;
	
	if (m_bTrace)
		WK_TRACE(_T("copy START   %s -> %s\n"),source,dest);
	
	if (!MyCopyFile(wCollection,wTape,source,dest,dwMaxRead))
	{
		WK_TRACE_ERROR(_T("copy FAILED %s -> %s, %s\n"),source,dest,GetLastErrorString());
		bRet = FALSE;
	}
	else
	{
		if (m_bTrace)
			WK_TRACE(_T("copy SUCCESS %s -> %s\n"),source,dest);
	}
	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CBackup::MyCopyFile(WORD wCollection, 
						 WORD wTape, 
						 const CString& sSource, 
						 const CString& sDest, 
						 FILESIZE dwMaxRead/*=-1*/)
{
	BOOL bRet = TRUE;
	CFile fSource;
	CFileException cfeSource;
	CFile fDest;
	CFileException cfeDest;

	if (m_pBackupThread)
	{
		m_pBackupThread->SetLastAction();
	}

	if (fSource.Open(sSource,CFile::modeRead|CFile::shareDenyNone,&cfeSource))
	{
		if (dwMaxRead!=-1)
		{
			TRACE(_T("read only %d of %d Bytes\n"),dwMaxRead,fSource.GetLength());
		}
		if (fDest.Open(sDest,CFile::modeCreate|CFile::modeWrite|CFile::shareExclusive,&cfeDest))
		{
			DWORD dwSize = 1024*1024;
			FILESIZE dwWrote=0;
			FILESIZE dwRead=0;
			BYTE* pBuffer = new BYTE[dwSize];

			TRY
			{
				FILESIZE dwReadOnce = 0;
				FILESIZE dwToRead = dwSize;
				if (dwMaxRead-dwRead<dwSize)
				{
					dwToRead = dwMaxRead-dwRead;
				}

				fSource.SeekToBegin();

				while (    bRet
						&& (dwRead<dwMaxRead)
						&& ((dwReadOnce=fSource.Read(pBuffer,(DWORD)dwToRead))>0)
						)
				{
					dwRead += dwReadOnce;
//					WK_TRACE(_T("read %d bytes %d bytes all\n"),dwReadOnce,dwRead);
					TRY
					{
						fDest.Write(pBuffer,(DWORD)dwReadOnce);
					}
					CATCH (CFileException ,e)
					{
						WK_TRACE_ERROR(_T("cannot write file for copy %s %d Bytes, %s\n"),
							sDest,dwWrote,GetLastErrorString(e->m_lOsError));
						bRet = FALSE;
					}
					END_CATCH
					if (bRet)
					{
						dwWrote += dwReadOnce;
//						WK_TRACE(_T("wrote %d bytes %d bytes all\n"),dwReadOnce,dwWrote);
						if (GetFlags() & BBT_PROGRESS)
						{
							IncreaseCopiedBytes(wCollection,wTape,dwReadOnce,BBT_PROGRESS);
						}
					}
					if (m_pBackupThread)
					{
						m_pBackupThread->SetLastAction();
						if (m_pBackupThread->IsCancelled())
						{
							break;
						}
					}
				}
			}
			CATCH (CFileException ,e)
			{
				WK_TRACE_ERROR(_T("cannot read file for copy %s %d Bytes, %s\n"),
					sSource,dwWrote,GetLastErrorString(e->m_lOsError));
				bRet = FALSE;
			}
			END_CATCH

			WK_DELETE_ARRAY(pBuffer);

			TRY
			{
				fDest.Close();
			}
			CATCH (CFileException,e)
			{
				WK_TRACE_ERROR(_T("cannot close dest file for copy %s, %s\n"),
					sDest,GetLastErrorString(e->m_lOsError));
				bRet = FALSE;
			}
			END_CATCH
		}
		else
		{
			WK_TRACE_ERROR(_T("cannot open dest file for copy %s, %s\n"),
				sDest,GetLastErrorString(cfeDest.m_lOsError));
			bRet = FALSE;
		}

		TRY
		{
			fSource.Close();
		}
		CATCH (CFileException,e)
		{
			WK_TRACE_ERROR(_T("cannot close source file for copy %s, %s\n"),
				sSource,GetLastErrorString(e->m_lOsError));
			bRet = FALSE;
		}
		END_CATCH
	}
	else
	{
		WK_TRACE_ERROR(_T("cannot open source file for copy %s, %s\n"),
			sSource,GetLastErrorString(cfeSource.m_lOsError));
		bRet = FALSE;
	}
	return bRet;
}
///////////////////////////////////////////////////////////////////////
void CBackup::WriteSequenceMap()
{
	if (m_SequenceMap.GetCount()>0)
	{
		CString sDbf,dDbf;
		CString sTempDir;
		GetTempPath(_MAX_PATH,sTempDir.GetBufferSetLength(_MAX_PATH));
		sTempDir.ReleaseBuffer();
		GetTempFileName(sTempDir,_T("dbf"),0,sDbf.GetBufferSetLength(_MAX_PATH));
		sDbf.ReleaseBuffer();
		
		m_SequenceMap.Save(sDbf);

		if (DoesFileExist(sDbf))
		{
			dDbf = m_sDirectory + _T("\\sequence.dbf");
			
			MyCopyFile(0,0,sDbf,dDbf);
			
			if (!DeleteFile(sDbf))
			{
				WK_TRACE_ERROR(_T("cannot delete temp file %s, %s"),sDbf,GetLastErrorString());
			}
		}
	}
}
