// Backup.cpp: implementation of the CBackup class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dbs.h"
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
void CBackup::Init(DWORD dwUserData,
				   DWORD dwClientID,
				   const CString& sDestinationPath,
				   const CString& sName,
				   int iNumIDs,
				   const DWORD dwIDs[],
				   WORD wFlags /*= 0*/,
				   DWORD dwMaximumInMB /* = 0*/)
{
	m_bInitialized = FALSE;
	m_dwClientID = dwClientID;
	m_dwUserData = dwUserData;
	m_sDestinationPath = sDestinationPath;
	m_sName = sName;
	m_wFlags = wFlags;
	m_dwMaximumInMB = dwMaximumInMB;
	m_dwLastConfirm = 0;

	for (int i=0;i<iNumIDs;i++)
	{
		DWORD dwID = dwIDs[i];
		WORD wArchivNr   = HIWORD(dwID);
		WORD wSequenceNr = LOWORD(dwID);
		CArchiv* pArchiv = theApp.m_Archives.GetArchiv(wArchivNr);

		if (pArchiv==NULL)
		{
			// invalid ingore it
			WK_TRACE_ERROR(_T("invalid Archiv to backup %d\n"),wArchivNr);
		}
		else
		{
			if (   wSequenceNr==0
				&& (m_wFlags != BBT_CALCULATE)
				&& (m_wFlags != BBT_CALCULATE_END)
				)
			{
				// backup the entire Archiv
				for (int j=0;j<pArchiv->GetNrSequences();j++)
				{
					CSequence* pSequence = pArchiv->GetSequenceIndex(j);
					if (   pSequence
						&& !pSequence->IsBackup())
					{
						wSequenceNr = pArchiv->GetSequenceIndex(j)->GetNr();
						dwID = MAKELONG(wSequenceNr,wArchivNr);
						m_IDs.Add(dwID);
						//
						pSequence->DoRequestOpenSequence(m_dwUserData);
					}
				}
				if (pArchiv->GetNrSequences()>0)
				{
					m_LastSequences.Add(dwID);
				}
			}
			else
			{
				m_IDs.Add(dwID);
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
CBackup::~CBackup()
{

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
		CString sFormat = CArchiv::m_sBackupName + _T("%04hx");
		sDir.Format(sFormat,i);
		sTest = m_sDestinationPath + sDir;

		if (!DoesFileExist(sTest))
		{
			if(GetLastError() == 53) //Der Netzwerkpfad wurde nicht gefunden
			{
				return FALSE;
			}
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

	// ini file with backup information
	CString sTempDir,sTempFile;
	DWORD dwLen = GetTempPath(_MAX_PATH,sTempDir.GetBuffer(_MAX_PATH));
	sTempDir.ReleaseBuffer();
	if (dwLen == 0)
	{
		WK_TRACE_ERROR(_T("get temp path failed, %s\n"),GetLastErrorString());
		return FALSE;
	}
	UINT uID = GetTempFileName(sTempDir,CArchiv::m_sRootName,0,sTempFile.GetBuffer(_MAX_PATH));
	sTempFile.ReleaseBuffer();
	if (uID == 0)
	{
		WK_TRACE_ERROR(_T("get temp file failed, %s\n"),GetLastErrorString());
		return FALSE;
	}

	CWordArray ids;
	WORD wArchivNr=0;
	for (int j=0; j< m_IDs.GetSize();j++)
	{
		wArchivNr = HIWORD(m_IDs.GetAt(j));
		ids.Add(wArchivNr);
	}

	WriteBackupDbi(sTempFile,m_sName,ids);

	Sleep(50);

	CString sDest = m_sDirectory + _T("\\backup.dbi");
	if (!DbsCopyFile(sTempFile,sDest,TRUE))
	{
		return FALSE;
	}

	if (!DeleteFile(sTempFile))
	{
		WK_TRACE_ERROR(_T("delete FAILED %s %s\n"),
			sTempFile,GetLastErrorString());
	}

	m_bInitialized = TRUE;

	return TRUE;
}
///////////////////////////////////////////////////////////////////////
void CBackup::WriteBackupDbi(const CString& sPathName,
							  const CString& sName,
							  const CWordArray& ids)
{
	CString sVersion;
	CWK_Profile wkp;
	sVersion = wkp.GetString(_T("Version"),_T("Number"),_T(""));

	CStdioFileU file;
	CString s;

	if (file.Open(sPathName,CFile::modeCreate|CFile::modeWrite))
	{
		file.SetFileMCBS((WORD)CWK_String::GetCodePage());
		TRY
		{
			s = _T("[Backup]\n");
			file.WriteString(s);
			s = _T("Version=") + sVersion + _T('\n');
			file.WriteString(s);
			s = _T("Title=") + sName + _T('\n');
			file.WriteString(s);
			s.Format(_T("CodePage=%d\n"), CWK_String::GetCodePage());
			file.WriteString(s);
			s = _T("[OrigNames]\n");
			file.WriteString(s);

			WORD wArchivNr=0;
			for (int j=0; j< ids.GetSize();j++)
			{
				wArchivNr = ids.GetAt(j);
				CArchiv* pArchiv = theApp.m_Archives.GetArchiv(wArchivNr);
				if (pArchiv)
				{
					s.Format(_T("%04lx=%s\n"),wArchivNr,pArchiv->GetName());
					file.WriteString(s);
				}
			}

			file.Close();
		}
		CATCH(CFileException , e)
		{
			theApp.HandleFileException(e, FALSE);
		}
		END_CATCH
	}
}
///////////////////////////////////////////////////////////////////////
BOOL CBackup::WasLastSequenceOfArchive(WORD wArchive, WORD wSequence)
{
	for (int i=0;i<m_LastSequences.GetSize();i++)
	{
		DWORD dwID = m_LastSequences.GetAt(i);
		WORD wArchivNr   = HIWORD(dwID);
		WORD wSequenceNr = LOWORD(dwID);
		if (   (wArchive==wArchivNr)
			&& (wSequenceNr==wSequence))
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
// update all necessary files from automatic backup, when not all sequence were copied
BOOL CBackup::CleanUpPartly()
{
	CIPCDrive* pDrive = theApp.m_Drives.GetDrive(m_sDestinationPath);
	if (pDrive && !pDrive->IsCDROM())
	{
		//get all complete copied sequences and write the backup.dbi file again (update)
		if(CleanUpPartly_Files())
		{
			WK_TRACE(_T("backup files updated successfully\n"));
		}
		else
		{
			WK_TRACE_ERROR(_T("could not update backup files\n"));
		}
	}
	return TRUE;
}
///////////////////////////////////////////////////////////////////////
BOOL CBackup::CleanUpPartly_Files()
{
	BOOL bRet = TRUE;
	//delete old backup.dbi file

	CString sDest = m_sDirectory + _T("\\backup.dbi");
	if(!DeleteFile(sDest))
	{
		WK_TRACE_ERROR(_T("deleting old backup.dbi failed, %s\n"),GetLastErrorString());
		bRet = FALSE;
	}

	// init file with backup information
	if(bRet)
	{
		CString sTempDir,sTempFile;
		DWORD dwLen = GetTempPath(_MAX_PATH,sTempDir.GetBuffer(_MAX_PATH));
		sTempDir.ReleaseBuffer();
		if (dwLen == 0)
		{
			WK_TRACE_ERROR(_T("get temp path failed, %s\n"),GetLastErrorString());
			bRet = FALSE;
		}
		if(bRet)
		{
			UINT uID = GetTempFileName(sTempDir,CArchiv::m_sRootName,0,sTempFile.GetBuffer(_MAX_PATH));
			sTempFile.ReleaseBuffer();
			if (uID == 0)
			{
				WK_TRACE_ERROR(_T("get temp file failed, %s\n"),GetLastErrorString());
				bRet = FALSE;
			}

			if(bRet)
			{
				CWordArray ids;
				WORD wArchivNr=0;
				for (int j=0; j< m_IDs.GetSize();j++)
				{
					wArchivNr = HIWORD(m_IDs.GetAt(j));
					ids.Add(wArchivNr);
				}

				WriteBackupDbi(sTempFile,m_sName,ids);
				Sleep(50);

				//copy the backup.dbi file from temp to destination
				if (!DbsCopyFile(sTempFile,sDest,TRUE))
				{
					bRet = FALSE;
				}
			}
		}
	}

	//update archiv.dbf (e.g. 0076.dbf) and sequence.dbf files
	if(bRet)
	{
		// write Sequence Map 
		WriteCameraMaps();
		// write Sequence Map 
		WriteSequenceMap();
	}
	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CBackup::CopyFileTrace(WORD wArchiv, 
							WORD wSequence, 
							const CString& source, 
							const CString& dest, 
							FILESIZE dwMaxRead/*=-1*/)
{
	BOOL bRet = TRUE;
	if (theApp.TraceBackup())
		WK_TRACE(_T("copy START   %s -> %s\n"),source,dest);
	if (!MyCopyFile(wArchiv,wSequence,source,dest,dwMaxRead))
	{
		WK_TRACE_ERROR(_T("copy FAILED %s -> %s, %s\n"),source,dest,GetLastErrorString());
		bRet = FALSE;
	}
	else
	{
		if (theApp.TraceBackup())
			WK_TRACE(_T("copy SUCCESS %s -> %s\n"),source,dest);
	}
	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CBackup::MyCopyFile(WORD wArchiv, 
						 WORD wSequence, 
						 const CString& sSource, 
						 const CString& sDest, 
						 FILESIZE dwMaxRead/*=-1*/)
{
	BOOL bRet = TRUE;
	CFile fSource;
	CFileException cfeSource;
	CFile fDest;
	CFileException cfeDest;

	if (fSource.Open(sSource,CFile::modeRead|CFile::shareExclusive,&cfeSource))
	{
		if (dwMaxRead!=-1)
		{
			if (theApp.TraceBackup())
				WK_TRACE(_T("read only %d of %d Bytes\n"),dwMaxRead,fSource.GetLength());
		}
		if (fDest.Open(sDest,CFile::modeCreate|CFile::modeWrite|CFile::shareExclusive,&cfeDest))
		{
			FILESIZE dwSize = 4096;
			FILESIZE dwWrote=0;
			FILESIZE dwRead=0;
			BYTE szBuffer[4096];

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
						&& ((dwReadOnce=fSource.Read(&szBuffer,(DWORD)dwToRead))>0)
						)
				{
					dwRead += dwReadOnce;
//					WK_TRACE(_T("read %d bytes %d bytes all\n"),dwReadOnce,dwRead);
					TRY
					{
						fDest.Write(szBuffer,(DWORD)dwReadOnce);
					}
					CATCH (CFileException ,e)
					{
						theApp.HandleFileException(e, FALSE);
						WK_TRACE_ERROR(_T("cannot write file for copy %s %d Bytes, %s\n"),
							sDest,dwWrote,GetLastErrorString(e->m_lOsError));
						bRet = FALSE;
					}
					END_CATCH
					if (bRet)
					{
						dwWrote += dwReadOnce;
//						WK_TRACE(_T("wrote %d bytes %d bytes all\n"),dwReadOnce,dwWrote);
						if (   (GetFlags() & BBT_PROGRESS)
							&& (wArchiv !=0)
							)
						{
							IncreaseCopiedBytes(wArchiv,wSequence,dwReadOnce,BBT_PROGRESS);
						}
					}
				}
			}
			CATCH (CFileException ,e)
			{
				theApp.HandleFileException(e, FALSE);
				WK_TRACE_ERROR(_T("cannot read file for copy %s %d Bytes, %s\n"),
					sSource,dwWrote,GetLastErrorString(e->m_lOsError));
				bRet = FALSE;
			}
			END_CATCH

			TRY
			{
				fDest.Close();
			}
			CATCH (CFileException,e)
			{
				theApp.HandleFileException(e, FALSE);
				WK_TRACE_ERROR(_T("cannot close dest file for copy %s, %s\n"),
					sDest,GetLastErrorString(e->m_lOsError));
				bRet = FALSE;
			}
			END_CATCH
		}
		else
		{
			theApp.HandleFileException(&cfeDest, FALSE);
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
			theApp.HandleFileException(e, FALSE);
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
void CBackup::IncreaseCopiedBytes(WORD wCollection, WORD wTape, FILESIZE dwBytes,WORD wFlags)
{
	m_iCopiedBytes += dwBytes;

	if (wFlags & BBT_PROGRESS)
	{
		// nur alle Sekunde confirmen
		if (   m_dwLastConfirm == 0
			|| GetTimeSpan(m_dwLastConfirm)>1000)
		{
			CClient* pClient = theApp.m_Clients.GetClient(m_dwClientID);
			if (pClient && pClient->IsConnected())
			{
				CIPCDatabaseServer* pCIPCDatabase = pClient->GetCIPCDatabaseServer();
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
			CIPCDatabaseServer* pCIPCDatabase = pClient->GetCIPCDatabaseServer();
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
///////////////////////////////////////////////////////////////////////
void CBackup::WriteCameraMaps()
{
	CWordArray wArchiveIDs;

	for (int i=0;i<m_IDs.GetSize();i++)
	{
		WORD wArchiveID = HIWORD(m_IDs.GetAt(i));
		BOOL bFound = FALSE;
		for (int j=0;j<wArchiveIDs.GetSize()&&!bFound;j++)
		{
			bFound = wArchiveIDs.GetAt(j)==wArchiveID;
		}
		if (!bFound)
		{
			wArchiveIDs.Add(wArchiveID);
		}
	}
	CString sTempDir;
	GetTempPath(_MAX_PATH,sTempDir.GetBufferSetLength(_MAX_PATH));
	sTempDir.ReleaseBuffer();
	for (i=0;i<wArchiveIDs.GetSize();i++)
	{
		CArchiv* pArchiv = theApp.m_Archives.GetArchiv(wArchiveIDs[i]);
		if (pArchiv)
		{
			CString sDbf,dDbf;
			GetTempFileName(sTempDir,_T("dbf"),0,sDbf.GetBufferSetLength(_MAX_PATH));
			sDbf.ReleaseBuffer();
			
			if (pArchiv->SaveCameraMap(sDbf))
			{
				dDbf.Format(_T("%s\\%04hx.dbf"),m_sDirectory,pArchiv->GetNr());
				
				MyCopyFile(0,0,sDbf,dDbf);
				
				if (!DeleteFile(sDbf))
				{
					WK_TRACE_ERROR(_T("cannot delete temp file %s, %s"),sDbf,GetLastErrorString());
				}
			}
		}
	}
}
///////////////////////////////////////////////////////////////////////
