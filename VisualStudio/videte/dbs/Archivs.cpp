/* GlobalReplace: CDVRDrive --> CIPCDrive */
/* GlobalReplace: CDVRInt64 --> CIPCInt64 */
/* GlobalReplace: line %s --> line %d */
// Archivs.cpp: implementation of the CArchivs class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DBS.h"
#include "Archivs.h"
#include "Converter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CArchivs::CArchivs()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	m_dwDirectoryCluster = 0;
	m_pLastGetArchive = NULL;
	m_dwTickLastClose = GetTickCount();
#ifdef TRACE_LOCKS
	m_nCurrentThread = theApp.TraceLocks() ? 0 : -1;
#endif
}
//////////////////////////////////////////////////////////////////////
CArchivs::~CArchivs()
{
	CCallStackEntry cse(_T(__FUNCTION__));
}
//////////////////////////////////////////////////////////////////////
void CArchivs::Init(CWK_Profile& wkp)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	m_pLastGetArchive = NULL;
	WK_TRACE(_T("loading archiv data\n"));
	Load(wkp);
	for (int i=0;i<GetSize();i++)
	{
		GetAtFast(i)->InitPreAlarm(*this);
	}
	WK_TRACE(_T("scanning archiv data\n"));
	ScanDatabase();


	CSystemTime now;
	now.GetLocalTime();
	for (int i=0;i<GetSize();i++)
	{
		CArchiv* pArchiv = GetAtFast(i);
		if (   pArchiv 
			&& pArchiv->IsAlarmList()
			&& pArchiv->GetNrSequences()>0)
		{
			CSequence* pSequence = pArchiv->GetSequenceIndex(pArchiv->GetNrSequences()-1);
			if (   pSequence
				&& pSequence->GetFirstTime().GetDate()<now.GetDate())
			{
				// TODO pArchiv->SetFirstSequenceAfterRestart();
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CArchivs::Load(CWK_Profile& wkp)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	m_pLastGetArchive = NULL;
	int i,c;
	CArchivInfoArray ai;
	CArchivInfo* pArchivInfo;
	CArchiv* pArchiv;

	ai.Load(wkp);

	CAutoCritSec acs(&m_cs, _T(__FUNCTION__), DBS_LOCK_TIMEOUT);

	c = ai.GetSize();
	for (i=0;i<c;i++)
	{
		pArchivInfo = ai.GetAtFast(i);
		if (theApp.IsDTS())
		{
			if (pArchivInfo->GetType() == RING_ARCHIV)
			{
				pArchivInfo->SetDynamicSize(TRUE);
			}
		}
		pArchiv = new CArchiv(*pArchivInfo);
		SafeAdd(pArchiv);
	}

	acs.Unlock();

	return TRUE;
}
//////////////////////////////////////////////////////////////////////
void CArchivs::ScanDatabase()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	int i,c;
	CIPCDrive* pDrive;

	// scan my own files
	c = theApp.m_Drives.GetSize();
	for (i=0;i<c;i++)
	{
		pDrive = theApp.m_Drives.GetAtFast(i);
		if (   pDrive
			&& pDrive->IsConnected()
			&& (   pDrive->IsDatabase()
				|| pDrive->IsRemoveableDatabase()
				)
			)
		{
			ScanDatabaseDrive(pDrive);
		}
	}

	CAutoCritSec acs(&m_cs, _T(__FUNCTION__), DBS_LOCK_TIMEOUT);

	c = GetSize();
	for (i=0;i<c;i++)
	{
		GetAtFast(i)->AdjustSize();	
	}
	acs.Unlock();

	// now scan for conversion
	c = theApp.m_Drives.GetSize();
	for (i=0;i<c;i++)
	{
		pDrive = theApp.m_Drives.GetAtFast(i);
		if (pDrive && pDrive->IsDatabase())
		{
			ConvertDrive(pDrive);
		}
	}

	Trace();
}
//////////////////////////////////////////////////////////////////////
void CArchivs::ScanBackup()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	int i,c;
	CIPCDrive* pDrive;

	// scan my own files
	c = theApp.m_Drives.GetSize();
	for (i=0;i<c;i++)
	{
		pDrive = theApp.m_Drives.GetAtFast(i);
		if (   pDrive->IsReadBackup()
			|| pDrive->IsWriteBackup())
		{
			ScanBackupDrive(pDrive);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CArchivs::Trace()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	for (int i=0;i<GetSize();i++)
	{
		GetAtFast(i)->Trace();	
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CArchivs::ConvertDrive(CIPCDrive* pDrive)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CString sOldDatabase;
	BOOL bRet = TRUE;

	sOldDatabase = pDrive->GetRootString() + "database";
	if (   pDrive->IsConnected() 
		&& DoesFileExist(sOldDatabase))
	{
		WK_TRACE(_T("old 3.x database directory found %s\n"),sOldDatabase);
		WK_TRACE(_T("searching for files to convert\n"));
		CConverter c(sOldDatabase);
		bRet = c.Convert();
	}
	return bRet;
}
//////////////////////////////////////////////////////////////////////
void CArchivs::ScanDatabaseDrive(CIPCDrive* pDrive)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CString sDatabase,sSearch;
	
	sDatabase = pDrive->GetRootString() + CArchiv::m_sRootName;

	if (!DoesFileExist(sDatabase))
	{
		m_dwDirectoryCluster += pDrive->GetClusterSize();
		CreateDirectory(sDatabase,NULL);
		return;
	}

	sSearch = sDatabase + "\\*.*";

	CString sTmp;
	WIN32_FIND_DATA	found;
	HANDLE	hFound = NULL;

	// alle Verzeichnisse durchgehen
	hFound = FindFirstFile(sSearch, &found);
	
	if (hFound != (HANDLE)INVALID_HANDLE_VALUE)
	{
		// one cluster for the dbs dir
		m_dwDirectoryCluster += pDrive->GetClusterSize();

		for (BOOL	bFound = TRUE; bFound; bFound=FindNextFile (hFound, &found)) 
		{
			if (found.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				sTmp = found.cFileName; 
				sTmp.MakeLower();
				if (sTmp == "." || sTmp == ".." )
				{
					// akuelles und übergeordnetes Verzeichnis ignorieren
					continue;
				}
				DWORD dw = 0;
				if (1 == _stscanf(sTmp,_T("arch%04hx"),&dw))
				{
					CArchiv* pArchiv = GetArchiv((WORD)dw);
					if (pArchiv)
					{
						pArchiv->Scan(sDatabase + _T("\\") + sTmp,pDrive,theApp.GetSequenceMap());
					}
					else
					{
						CString sOldArchivDir = sDatabase + _T('\\') + sTmp;
						WK_TRACE(_T("invalid archiv dir found %s\n"),sOldArchivDir);
						if (DeleteDirRecursiv(sOldArchivDir))
						{
							WK_TRACE(_T("delete invalid archiv dir %s\n"),sOldArchivDir);
						}
						else
						{
							WK_TRACE(_T("cannot delete invalid archiv dir %s\n"),sOldArchivDir);
						}
					}
				}
				else
				{
					CString sForeignDir = sDatabase + '\\' + sTmp;
					WK_TRACE(_T("foreign dir found %s\n"),sForeignDir);
					if (DeleteDirRecursiv(sForeignDir))
					{
						WK_TRACE(_T("foreign dir dir %s\n"),sForeignDir);
					}
					else
					{
						WK_TRACE(_T("cannot delete foreign dir dir %s\n"),sForeignDir);
					}
				}
			}
			else
			{
				if (-1==sTmp.Find(_T("dbf")))
				{
					WK_TRACE(_T("foreign file found %s\\%s\n"),sDatabase,sTmp);
				}
			}
		}
	}
	FindClose(hFound); 
}
//////////////////////////////////////////////////////////////////////
void CArchivs::ScanBackupDrive(CIPCDrive* pDrive)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CString sSearch;

	WK_TRACE(_T("scanning backup drive %s\n"),pDrive->GetRootString());

	sSearch = pDrive->GetRootString() + "*.*";

	CString sTmp;
	WIN32_FIND_DATA	found;
	HANDLE	hFound = NULL;

	// alle Verzeichnisse durchgehen
	hFound = FindFirstFile(sSearch, &found);
	
	if (hFound != (HANDLE)INVALID_HANDLE_VALUE)
	{
		for (BOOL	bFound = TRUE; bFound; bFound=FindNextFile (hFound, &found)) 
		{
			if (found.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				sTmp = found.cFileName; 
				sTmp.MakeLower();
				if (sTmp == "." || sTmp == ".." )
				{
					// akuelles und übergeordnetes Verzeichnis ignorieren
					continue;
				}
				DWORD dw = 0;
				CString sFormat = CArchiv::m_sBackupName + _T("%04hx");
				if (1 == _stscanf(sTmp,sFormat,&dw))
				{
					ScanBackupDirectory(pDrive->GetRootString() + sTmp + '\\',pDrive);
				}
				else
				{
					// a dir in backup root dir
					// ignore it
				}
			}
			else
			{
				// a file in backup root dir
				// ignore it
			}
		}
	}
	FindClose(hFound); 
}
//////////////////////////////////////////////////////////////////////
void CArchivs::ScanBackupDirectory(const CString& sBackupDir, CIPCDrive* pDrive)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CString sSearch;

	WK_TRACE(_T("scanning backup dir %s\n"),sBackupDir);

	CVDBSequenceMap map;
	map.Load(sBackupDir + _T("sequence.dbf"));
	
	sSearch = sBackupDir + "*.*";

	CString sTmp;
	WIN32_FIND_DATA	found;
	HANDLE	hFound = NULL;

	// alle Verzeichnisse durchgehen
	hFound = FindFirstFile(sSearch, &found);
	
	if (hFound != (HANDLE)INVALID_HANDLE_VALUE)
	{
		for (BOOL	bFound = TRUE; bFound; bFound=FindNextFile (hFound, &found)) 
		{
			if (found.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				sTmp = found.cFileName; 
				sTmp.MakeLower();
				if (sTmp == "." || sTmp == ".." )
				{
					// akuelles und übergeordnetes Verzeichnis ignorieren
					continue;
				}
				DWORD dw = 0;
				if (1 == _stscanf(sTmp,_T("arch%04hx"),&dw))
				{
					// dynamically create a new archive id for
					// this one if not already exists
					CArchiv* pArchiv = CreateBackupArchiv(sBackupDir, (WORD)dw);
					if (pArchiv)
					{
						pArchiv->Scan(sBackupDir + sTmp,pDrive,map);
						Add(pArchiv);
						theApp.m_Clients.DoIndicateNewArchiv(*pArchiv);
					}
					else
					{
						// signals that the archive already exist
					}
				}
				else
				{
					// a dir in backup dir
					// ignore it
				}
			}
			else
			{
				// a file in backup root dir
				// may be some important info
				// about the backup
			}
		}
	}
	FindClose(hFound); 
	WK_TRACE(_T("scanned backup dir %s\n"),sBackupDir);
	m_saBackupDirectories.Add(sBackupDir.Left(sBackupDir.GetLength()-1));
}
//////////////////////////////////////////////////////////////////////
CArchiv* CArchivs::CreateBackupArchiv(const CString& sBackupDir, WORD wOriginalNr)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	int i;
	CArchiv* pArchiv = NULL;
	WORD wNewNr = 1;

	CAutoCritSec acs(&m_cs, _T(__FUNCTION__), DBS_LOCK_TIMEOUT);
	for (i=0;i<GetSize();i++)
	{
		pArchiv = GetAtFast(i);
		if (    pArchiv->IsBackup()
			&& 	(wOriginalNr == pArchiv->GetOriginalNr())
			&&  (sBackupDir == pArchiv->GetBackupDir())
		   )
		{
			// already existing archive
			acs.Unlock();
			return NULL;
		}
		if (   (pArchiv->GetNr()>wNewNr)
			&& (pArchiv->GetNr()!=255)
			)
		{
			wNewNr = pArchiv->GetNr();
		}
	}
	acs.Unlock();

	wNewNr++;
	if (wNewNr==255)
	{
		wNewNr++;
	}

	pArchiv = new CArchiv(sBackupDir,wNewNr,wOriginalNr);

	return pArchiv;
}
//////////////////////////////////////////////////////////////////////
CArchiv* CArchivs::GetArchiv(WORD wNr)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	if (   m_pLastGetArchive
		&& m_pLastGetArchive->GetNr()==wNr)
	{
		return m_pLastGetArchive;
	}
	int i;
	CArchiv* pArchiv = NULL;

	CAutoCritSec acs(&m_cs, _T(__FUNCTION__), DBS_LOCK_TIMEOUT);
	for (i=0;i<GetSize();i++)
	{
		if (GetAtFast(i)->GetNr()==wNr)
		{
			pArchiv = GetAtFast(i);
			break;
		}
	}
	acs.Unlock();

	m_pLastGetArchive = pArchiv;

	return pArchiv;
}
//////////////////////////////////////////////////////////////////////
DWORD CArchivs::GetFixedMB(CIPCDrive* pDrive)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	DWORD dwFixedMB = 0;

	int i,c;
	CArchiv* pArchiv = NULL;

	c = GetSize();
	for (i=0;i<c;i++)
	{
		pArchiv = GetAtFast(i);
		if (   (!pArchiv->GetFixedDrive().IsEmpty()) 
			&& (pDrive->GetRootString().GetAt(0) == pArchiv->GetFixedDrive().GetAt(0))
			)
		{
			dwFixedMB += pArchiv->GetSizeMB();
		}
	}

	return dwFixedMB;
}
//////////////////////////////////////////////////////////////////////
DWORD CArchivs::GetFixedUseMB(CIPCDrive* pDrive)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CIPCInt64 iFixedUse = 0;

	int i;
	CArchiv* pArchiv = NULL;

	for (i=0;i<GetSize();i++)
	{
		pArchiv = GetAtFast(i);
		if (   (!pArchiv->GetFixedDrive().IsEmpty())
			&& (pDrive->GetRootString().GetAt(0) == pArchiv->GetFixedDrive().GetAt(0))
			)
		{
			iFixedUse += pArchiv->GetSizeBytes();
		}
	}

	return iFixedUse.GetInMB();
}
//////////////////////////////////////////////////////////////////////
int	CArchivs::GetNrOfSequences()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	int iNr = 0;
	int i;

	for (i=0;i<GetSize();i++)
	{
		iNr += GetAtFast(i)->GetNrSequences();
	}

	return iNr;
}
//////////////////////////////////////////////////////////////////////
CArchiv* CArchivs::GetSafeRingArchiv(WORD wNr)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	int i,c;
	CArchiv* pArchiv = NULL;

	CAutoCritSec acs(&m_cs, _T(__FUNCTION__), DBS_LOCK_TIMEOUT);

	c = GetSize();
	for (i=0;i<c;i++)
	{
		if (GetAtFast(i)->GetSafeRingFor()==wNr)
		{
			pArchiv = GetAtFast(i);
			break;
		}
	}

	acs.Unlock();
	return pArchiv;
}
//////////////////////////////////////////////////////////////////////
DWORD CArchivs::GetMB(BOOL bAlsoFixedDrives/*=TRUE*/)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	DWORD dwMB = 0;
	int i,c;
	CArchiv *pA;

	CAutoCritSec acs(&m_cs, _T(__FUNCTION__), DBS_LOCK_TIMEOUT);
	c = GetSize();
	for (i=0;i<c;i++)
	{
		pA = GetAtFast(i);
		if (   !bAlsoFixedDrives
			&& !pA->GetFixedDrive().IsEmpty())
		{
			continue;
		}
		if (!GetAtFast(i)->IsBackup())
		{
			dwMB += GetAtFast(i)->GetSizeMB();
		}
	}

	acs.Unlock();

	return dwMB;
}
//////////////////////////////////////////////////////////////////////
CIPCInt64 CArchivs::GetUsedBytes()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CIPCInt64 iUsedMB = 0;
	int i;

	CAutoCritSec acs(&m_cs, _T(__FUNCTION__), DBS_LOCK_TIMEOUT);

	for (i=0;i<GetSize();i++)
	{
		iUsedMB += GetAtFast(i)->GetSizeBytes();
	}

	acs.Unlock();

	iUsedMB += CIPCInt64(m_dwDirectoryCluster);

	return iUsedMB;
}
//////////////////////////////////////////////////////////////////////
int	CArchivs::GetAlarmPercent()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CIPCInt64 iUsedBytes = 0;
	CIPCInt64 iBytes=0;
	int i;

	CAutoCritSec acs(&m_cs, _T(__FUNCTION__), DBS_LOCK_TIMEOUT);

	for (i=0;i<GetSize();i++)
	{
		if (GetAtFast(i)->IsAlarm())
		{
			CIPCInt64 is(GetAtFast(i)->GetSizeMB());
			is *= CIPCInt64(1024*1024);
			iBytes += is;
			iUsedBytes += GetAtFast(i)->GetSizeBytes();
		}
	}

	acs.Unlock();

	iUsedBytes += CIPCInt64(m_dwDirectoryCluster);

	if (iBytes>0)
	{
		int iPercent = (int)((iUsedBytes.GetInt64()*100) / iBytes.GetInt64());
		if (iPercent>100)
		{
			iPercent = 100;
		}
		return iPercent;
	}
	else
	{
		return 0;
	}
}
//////////////////////////////////////////////////////////////////////
void CArchivs::Finalize(BOOL bShutdown)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	for (int i=0;i<GetSize();i++)
	{
		CArchiv* pArchiv = GetAtFast(i);
		if (   pArchiv->IsAlarm()
			|| pArchiv->IsAlarmList()
			|| bShutdown)
		{
			pArchiv->Finalize(bShutdown);
		}
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CArchivs::DeleteArchive(WORD wArchivNr)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	int i;
	CArchiv* pArchiv;

	for (i=0;i<GetSize();i++)
	{
		pArchiv = GetAtFast(i);
		if (pArchiv && pArchiv->GetNr() == wArchivNr)
		{
			if (pArchiv->Delete())
			{
				theApp.m_Clients.DoIndicateDeleteArchiv(wArchivNr);
				CAutoCritSec acs(&m_cs, _T(__FUNCTION__), DBS_LOCK_TIMEOUT);
				RemoveAt(i);
				if (   m_pLastGetArchive
					&& (m_pLastGetArchive == pArchiv)
					)
				{
					m_pLastGetArchive = NULL;
				}
				acs.Unlock();
				WK_DELETE(pArchiv);
				return TRUE;
			}
		}
	}
	return FALSE;
}
///////////////////////////////////////////////////////////////////////////////////
void CArchivs::CheckBackupDirectories()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CAutoCritSec acs(&m_cs, _T(__FUNCTION__), DBS_LOCK_TIMEOUT);
	for (int i=m_saBackupDirectories.GetSize()-1;i>=0;i--)
	{
		CString sBackupDir = m_saBackupDirectories.GetAt(i);
		//TRACE(_T("checking %s\n"),sBackupDir);
		if (!DoesFileExist(sBackupDir))
		{
			for (int j=GetSize()-1;j>=0;j--)
			{
				CArchiv* pArchiv = GetAtFast(j);
				pArchiv->DeleteDatabaseDirectory(sBackupDir);	
				if (   pArchiv->IsBackup()
					&& pArchiv->GetNrSequences() == 0)
				{
					// Nur Backup Sequences ??
					WK_TRACE(_T("delete backup %s\n"),pArchiv->GetName());
					DeleteArchive(pArchiv->GetNr());
				}
			}
			m_saBackupDirectories.RemoveAt(i);
		}
	}
	acs.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CArchivs::DeleteDatabaseDrive(const CString& sRootString)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CAutoCritSec acs(&m_cs, _T(__FUNCTION__), DBS_LOCK_TIMEOUT);
	for (int i=GetSize()-1;i>=0;i--)
	{
		CArchiv* pArchiv = GetAtFast(i);
		pArchiv->DeleteDatabaseDrive(sRootString);	
		if (   pArchiv->IsBackup()
			&& pArchiv->GetNrSequences() == 0)
		{
			// Nur Backup Sequences ??
			WK_TRACE(_T("delete backup %s\n"),pArchiv->GetName());
			DeleteArchive(pArchiv->GetNr());
		}
	}

	for (i=m_saBackupDirectories.GetSize()-1;i>=0;i--)
	{
		CString sRoot = m_saBackupDirectories.GetAt(i).Left(sRootString.GetLength());
		if (0==sRootString.CompareNoCase(sRoot))
		{
			m_saBackupDirectories.RemoveAt(i);
		}
	}

	acs.Unlock();
}
//////////////////////////////////////////////////////////////////////
BOOL CArchivs::CloseAll()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	BOOL bRet = FALSE;
	if (GetTimeSpan(m_dwTickLastClose)>30*1000)
	{
//		TRACE("finalizing archives\n");
		Finalize(FALSE);
		m_dwTickLastClose = GetTickCount();
		bRet = TRUE;
	}

	return bRet;
}
//////////////////////////////////////////////////////////////////////
CSequence* CArchivs::GetNextSequence(WORD wArchivNr, WORD wSequenceNr)
{
	CCallStackEntry cse(_T(__FUNCTION__));

	CSequence* pReturn = NULL;
	CAutoCritSec acs(&m_cs, _T(__FUNCTION__), DBS_LOCK_TIMEOUT);

	if (GetSize())
	{
		CArchiv* pArchiv = NULL;
		
		if (wArchivNr == 0)
		{
			pArchiv = GetAtFast(0);
		}
		else
		{
			pArchiv = GetArchiv(wArchivNr);
		}
		
		while (pArchiv && pReturn==NULL)
		{
			CSequence* pSequence = pArchiv->GetNextSequence(wSequenceNr);

			if (pSequence)
			{
				pReturn = pSequence;
			}
			else
			{
				pArchiv = GetNextArchive(pArchiv->GetNr());
				wSequenceNr = 0;
			}
		}
	}

	acs.Unlock();

	return pReturn;
}
//////////////////////////////////////////////////////////////////////
CArchiv* CArchivs::GetNextArchive(WORD wArchiveNr)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CArchiv* pArchive = NULL;
	CAutoCritSec acs(&m_cs, _T(__FUNCTION__), DBS_LOCK_TIMEOUT);

	if (   wArchiveNr == 0
		&& GetSize()>0)
	{
		pArchive = GetAtFast(0);
	}
	else
	{
		for (int i=0;i<GetSize();i++)
		{
			if (GetAtFast(i)->GetNr() == wArchiveNr)
			{
				if (i+1<GetSize())
				{
					pArchive = GetAtFast(i+1);
				}
				break;
			}
		}
	}

	acs.Unlock();
	return pArchive;
}
//////////////////////////////////////////////////////////////////////
int CArchivs::GetNrOfDynamicArchives()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CAutoCritSec acs(&m_cs, _T(__FUNCTION__), DBS_LOCK_TIMEOUT);
	int iNr = 0;
	int i;

	for (i=0;i<GetSize();i++)
	{
		if (GetAtFast(i)->IsDynamicSize())
		{
			iNr++;
		}
	}

	acs.Unlock();

	return iNr;
}
//////////////////////////////////////////////////////////////////////
DWORD CArchivs::GetAverageDynamicSequenceSizeMB()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CAutoCritSec acs(&m_cs, _T(__FUNCTION__), DBS_LOCK_TIMEOUT);
	int iNr = 0;
	int i;
	DWORD dwSize = 0;

	for (i=0;i<GetSize();i++)
	{
		if (GetAtFast(i)->IsDynamicSize())
		{
			iNr++;
			dwSize += GetAtFast(i)->GetSequenceSizeBytes().GetInMB();
		}
	}

	acs.Unlock();

	if (iNr>0)
	{
		return dwSize / iNr;
	}
	else
	{
		return 0;
	}
}
//////////////////////////////////////////////////////////////////////
int CArchivs::GetNrOfFixedArchives()
{
	return GetSize()-GetNrOfDynamicArchives();
}
//////////////////////////////////////////////////////////////////////
DWORD CArchivs::GetFixedSizeDefinedMB()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CAutoCritSec acs(&m_cs, _T(__FUNCTION__), DBS_LOCK_TIMEOUT);
	int i;
	DWORD dwSize = 0;

	for (i=0;i<GetSize();i++)
	{
		if (!GetAtFast(i)->IsDynamicSize())
		{
			dwSize += GetAtFast(i)->GetSizeMB();
		}
	}

	acs.Unlock();

	return dwSize;
}
//////////////////////////////////////////////////////////////////////
DWORD CArchivs::GetFixedSizeUsedMB()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CAutoCritSec acs(&m_cs, _T(__FUNCTION__), DBS_LOCK_TIMEOUT);
	int i;
	DWORD dwSize = 0;

	for (i=0;i<GetSize();i++)
	{
		if (!GetAtFast(i)->IsDynamicSize())
		{
			dwSize += GetAtFast(i)->GetSizeBytes().GetInMB();
		}
	}

	acs.Unlock();

	return dwSize;
}
//////////////////////////////////////////////////////////////////////
void CArchivs::DeleteOldestDynamicSequence()
{
	// just find out the archive
	// with the oldest sequence
	CSystemTime stOldest;
	CSystemTime stOldestOK;
	CSystemTime stPivot;
	CArchiv* pPivot = NULL;
	CArchiv* pArchiv = NULL;
	CArchiv* pArchivOK = NULL;

	stOldest.GetLocalTime();
	stOldestOK = stOldest;

	Lock();
	for (int i=0;i<GetSize();i++)
	{
		pPivot = GetAtFast(i);
		if (   pPivot->IsDynamicSize()
			&& pPivot->GetNrSequences())
		{
			// must be ring
			// and must have any sequence
			// else GetSequenceIndex would fail with null pointer
			CSequence* pSequence = pPivot->GetOldestSequence();
			if (pSequence)
			{
				stPivot = pSequence->GetFirstTime();
				if (stPivot<stOldestOK)
				{
					if (stPivot<stOldest)
					{
						pArchiv = pPivot;
						stOldest = stPivot;
					}

					if (pPivot->GetSizeBytes().GetInMB()>
						pPivot->GetSizeMB())
					{
						// real archive size is larger
						// than minimum archive size
						pArchivOK = pArchiv;
						stOldestOK = stPivot;
					}
					else
					{
						// don't use that archive
						// to small
					}
				}
			}
		}
		else
		{
			// don't delete Alarm, Vorring, Alarmliste
		}
	}
	Unlock();

	if (pArchivOK)
	{
		/*
		WK_TRACE(_T("deleting system wide oldest sequence OK of Archive %04x,%04x, %s\n"),
		pArchivOK->GetNr(),
		pArchivOK->GetSequenceIndex(0)->GetSequenceNr(),
		pArchivOK->GetSequenceIndex(0)->GetFirstTime().GetDateTime());
		*/
		pArchivOK->DeleteOldestSequence();
	}
	else if (pArchiv)
	{
		// nothing found
		// but we must delete something use oldest ever
		/*
		WK_TRACE(_T("deleting system wide oldest sequence ever of Archive %04x,%04x, %s\n"),
		pArchiv->GetNr(),
		pArchiv->GetSequenceIndex(0)->GetSequenceNr(),
		pArchiv->GetSequenceIndex(0)->GetFirstTime().GetDateTime());
		*/
		pArchiv->DeleteOldestSequence();
	}
}
