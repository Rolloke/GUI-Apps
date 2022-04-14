// Collections.cpp: implementation of the CCollections class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DVStorage.h"
#include "Collections.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CCollection *g_pSearchCollection = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCollections::CCollections()
{
	m_dwDirectoryCluster = 0;
	m_pLastGetCollection = NULL;
	m_dwSequenceSizeInMB = 16;
	m_dwTickLastClose = GetTickCount();
	if (g_pSearchCollection == NULL)
	{
		g_pSearchCollection = new CCollection(_T("c:"), 0);
	}
}
//////////////////////////////////////////////////////////////////////
CCollections::~CCollections()
{
	WK_DELETE(g_pSearchCollection);
}
//////////////////////////////////////////////////////////////////////
DWORD CCollections::GetSequenceSizeInMB()
{
	return m_dwSequenceSizeInMB;
}
//////////////////////////////////////////////////////////////////////
void CCollections::Init(CWK_Profile& wkp)
{
	m_pLastGetCollection = NULL;
	if (theApp.GetNoFrag())
	{
		DWORD dwAVMB = theApp.m_Drives.GetAvailableMB();
		WK_TRACE(_T("Available for db = %d MB\n"),dwAVMB);
		m_dwSequenceSizeInMB = 16;
		if (dwAVMB>20*1024)
		{
			m_dwSequenceSizeInMB = 32;
		}
		if (dwAVMB>40*1024)
		{
			m_dwSequenceSizeInMB = 64;
		}
		if (dwAVMB>80*1024)
		{
			m_dwSequenceSizeInMB = 128;
		}
		if (dwAVMB>160*1024)
		{
			m_dwSequenceSizeInMB = 256;
		}
	}
	WK_TRACE(_T("Sequence Size = %d MB\n"),m_dwSequenceSizeInMB);

	WK_TRACE(_T("loading archive infos\n"));
	Load(wkp);
	WK_TRACE(_T("archive infos loaded\n"));
	Trace();

	if (GetSize()>0)
	{
		WK_TRACE(_T("scanning database files\n"));
		ScanDatabase();
		WK_TRACE(_T("database files scanned\n"));
		Trace();
	}
	else
	{
		WK_TRACE_ERROR(_T("no archive defined ??\n"));
		theApp.PrepareToShutDown_NoArchives();
		
	}

	CSystemTime now;
	now.GetLocalTime();
	for (int i=0;i<GetSize();i++)
	{
		CCollection* pCollection = GetAtFast(i);
		if (   pCollection->IsAlarmList()
			&& pCollection->GetNrTapes()>0)
		{
			CTape* pTape = pCollection->GetTapeIndex(pCollection->GetNrTapes()-1);
			if (   pTape
				&& pTape->GetFirstTime().GetDate()<now.GetDate())
			{
				pCollection->SetFirstSequenceAfterRestart();
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
int __cdecl CCollections::CCollectionsComp(const void *p1, const void *p2)
{
	CCollection*c1 = (CCollection*)*((int*)p1);
	CCollection*c2 = (CCollection*)*((int*)p2);
	if (c1->GetNr() == c2->GetNr())
	{
		return 0;
	}
	return (c1->GetNr() > c2->GetNr()) ? 1 : -1;
}
//////////////////////////////////////////////////////////////////////
BOOL CCollections::Load(CWK_Profile& wkp)
{
	m_pLastGetCollection = NULL;
	int i,c;
	CArchivInfoArray ai;
	CArchivInfo* pInfo;
	CCollection* pCollection;

	ai.Load(wkp);
	WK_TRACE(_T("%d archive info's loaded\n"),ai.GetSize());

	CAutoCritSec acs(&m_cs, _T(__FUNCTION__), DVS_TIMEOUT);

	c = ai.GetSize();
	for (i=0;i<c;i++)
	{
		pInfo = ai.GetAt(i);
		pCollection = new CCollection(*pInfo);
		Add(pCollection);
	}

	qsort(m_pData, m_nSize, sizeof(CCollection*), CCollectionsComp);

#ifdef _DEBUG
	CString sCollectionNumbers, sNr;
	for (i=0; i<GetSize(); i++)
	{
		sNr.Format(_T("\nCurrNr: %04x, %dd"), GetAtFast(i)->GetNr(), GetAtFast(i)->GetNr());
		sCollectionNumbers += sNr;
	}
	TRACE(_T("%s\n"), sCollectionNumbers);
#endif

	acs.Unlock();

	return TRUE;
}
void CCollections::SafeAdd(CCollection*pCollection)
{
	Lock();
	Add(pCollection);
	qsort(m_pData, m_nSize, sizeof(CCollection*), CCollectionsComp);
	Unlock();
}
//////////////////////////////////////////////////////////////////////
void CCollections::ScanDatabase()
{
	int i,c;
	CIPCDrive* pDrive;

	// scan my own files
	c = theApp.m_Drives.GetSize();
	for (i=0;i<c;i++)
	{
		pDrive = theApp.m_Drives.GetAtFast(i);
		if (pDrive->IsDatabase())
		{
			ScanDatabaseDrive(pDrive);
		}
	}

	CAutoCritSec acs(&m_cs, _T(__FUNCTION__), DVS_TIMEOUT);

	c = GetSize();
	for (i=0;i<c;i++)
	{
		GetAtFast(i)->AdjustSize();	
	}

	acs.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CCollections::Trace()
{
	for (int i=0;i<GetSize();i++)
	{
		GetAtFast(i)->Trace();	
	}
}
//////////////////////////////////////////////////////////////////////
void CCollections::DeleteDatabaseDrive(const CString& sRootString)
{
	CAutoCritSec acs(&m_cs, _T(__FUNCTION__), DVS_TIMEOUT);

	for (int i=GetSize()-1;i>=0;i--)
	{
		CCollection* pCollection = GetAtFast(i);
		pCollection->DeleteDatabaseDrive(sRootString);	
		if (   pCollection->IsBackup()
			&& pCollection->GetNrTapes() == 0)
		{
			// Nur Backup Tapes ??
			WK_TRACE(_T("delete backup %s\n"),pCollection->GetName());
			DeleteCollection(pCollection->GetNr());
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
///////////////////////////////////////////////////////////////////////////////////
void CCollections::CheckBackupDirectories()
{
	CAutoCritSec acs(&m_cs, _T(__FUNCTION__), DVS_TIMEOUT);
	for (int i=m_saBackupDirectories.GetSize()-1;i>=0;i--)
	{
		CString sBackupDir = m_saBackupDirectories.GetAt(i);
		TRACE(_T("checking %s\n"),sBackupDir);
		if (!DoesFileExist(sBackupDir))
		{
			for (int j=GetSize()-1;j>=0;j--)
			{
				CCollection* pCollection = GetAtFast(j);
				pCollection->DeleteDatabaseDirectory(sBackupDir);	
				if (   pCollection->IsBackup()
					&& pCollection->GetNrTapes() == 0)
				{
					// Nur Backup Tapes ??
					WK_TRACE(_T("delete backup %s\n"),pCollection->GetName());
					DeleteCollection(pCollection->GetNr());
				}
			}
			m_saBackupDirectories.RemoveAt(i);
		}
	}
	acs.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CCollections::ScanDatabaseDrive(CIPCDrive* pDrive)
{
	CString sDatabase,sSearch;
	
	sDatabase = pDrive->GetRootString() + _T("dvs");

	if (!DoesFileExist(sDatabase))
	{
		m_dwDirectoryCluster += pDrive->GetClusterSize();
		CreateDirectory(sDatabase,NULL);
		return;
	}

	sSearch = sDatabase + _T("\\*.*");

	CString sTmp;
	WIN32_FIND_DATA	found;
	HANDLE	hFound = NULL;

	// alle Verzeichnisse durchgehen
	hFound = FindFirstFile(LPCTSTR(sSearch), &found);
	
	if (hFound != INVALID_HANDLE_VALUE)
	{
		// one cluster for the dvs dir
		m_dwDirectoryCluster += pDrive->GetClusterSize();

		for (BOOL	bFound = TRUE; bFound; bFound=FindNextFile (hFound, &found)) 
		{
			if (found.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				sTmp = found.cFileName; 
				sTmp.MakeLower();
				if (sTmp == _T(".") || sTmp == _T("..") )
				{
					// aktuelles und übergeordnetes Verzeichnis ignorieren
					continue;
				}
				DWORD dw = 0;
				if (1==_stscanf(sTmp,_T("arch%x"),&dw))
				{
					CCollection* pCollection = GetCollection((WORD)dw);
					if (pCollection)
					{
						pCollection->Scan(sDatabase + _T("\\") + sTmp,
										  pDrive,
										  FALSE,
										  FALSE,
										  theApp.GetSequenceMap());
					}
					else
					{
						CString sOldCollectionDir = sDatabase + _T('\\') + sTmp;
						WK_TRACE(_T("invalid Collection dir found %s\n"),sOldCollectionDir);
						WK_TRACE(_T("rmdir %s by invalid archive nr\n"),sOldCollectionDir);
						if (DeleteDirRecursiv(sOldCollectionDir))
						{
							WK_TRACE(_T("delete invalid Collection dir %s\n"),sOldCollectionDir);
						}
						else
						{
							WK_TRACE(_T("cannot delete invalid Collection dir %s\n"),sOldCollectionDir);
						}
					}
				}
				else
				{
					CString sForeignDir = sDatabase + _T('\\') + sTmp;
					WK_TRACE(_T("foreign dir found %s\n"),sForeignDir);
					WK_TRACE(_T("rmdir %s by foreign directory\n"),sForeignDir);
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
				WK_TRACE(_T("foreign file found %s\\%s\n"),sDatabase,sTmp);
			}
		}
	}
	FindClose(hFound); 
}
//////////////////////////////////////////////////////////////////////
CCollection* CCollections::GetCollectionByCameraNr(BYTE bNr)
{
	int i;
	BYTE b;

	CAutoCritSec acs(&m_cs, _T(__FUNCTION__), DVS_TIMEOUT);
	for (i=0;i<GetSize();i++)
	{
		if (   GetAtFast(i)->IsRing()
			|| GetAtFast(i)->IsAlarm())
		{
			b = LOBYTE(GetAtFast(i)->GetNr());
			if (b==bNr)
			{
				return GetAtFast(i);
			}
		}
	}
	return NULL;
}
//////////////////////////////////////////////////////////////////////
CCollection* CCollections::GetCollection(WORD wNr)
{
	if (   m_pLastGetCollection
		&& m_pLastGetCollection->GetNr()==wNr)
	{
		return m_pLastGetCollection;
	}

	int i;
	CCollection* pCollection = NULL;
/*
	// mittlere Zugriffszeit binär 2,5 µs 
	g_pSearchCollection->SetNr(wNr);
	Lock();
	CCollection**ppCol = (CCollection**) bsearch(&g_pSearchCollection, m_pData, m_nSize, sizeof(CCollection*), CCollectionsComp);
	if (ppCol)
	{
		pCollection = *ppCol;
	}
	else
	{
		pCollection = NULL;
	}
	Unlock();
*/

	// mittlere Zugriffszeit sequenziell 2 µs 
	Lock();
	for (i=0;i<GetSize();i++)
	{
		if (GetAtFast(i)->GetNr()==wNr)
		{
			pCollection = GetAtFast(i);
			break;
		}
	}
	Unlock();

	if(!pCollection)
	{
		CString sCollectionNumbers, sNr;
		for (i=0; i<GetSize(); i++)
		{
			sNr.Format(_T("\nCurrNr: %04x"),GetAtFast(i)->GetNr());
			sCollectionNumbers += sNr;
		}
		WK_TRACE(_T("**** Collection Size: %d Search: %d Numbers: %s\n"), GetSize(),wNr, sCollectionNumbers);
	}
	m_pLastGetCollection = pCollection;

	return pCollection;
}
//////////////////////////////////////////////////////////////////////
int	CCollections::GetNrOfRingArchives()
{
	int iNr = 0;
	int i;

	Lock();

	for (i=0;i<GetSize();i++)
	{
		if (GetAtFast(i)->IsRing())
		{
			iNr++;
		}
	}

	Unlock();

	return iNr;
}
//////////////////////////////////////////////////////////////////////
int	CCollections::GetNrOfAlarmArchives()
{
	int iNr = 0;
	int i;
	
	Lock();
	
	for (i=0;i<GetSize();i++)
	{
		if (GetAtFast(i)->IsAlarm())
		{
			iNr++;
		}
	}
	
	Unlock();
	
	return iNr;
}
//////////////////////////////////////////////////////////////////////
int	CCollections::GetNrOfSequences()
{
	int iNr = 0;
	int i;

	Lock();

	for (i=0;i<GetSize();i++)
	{
		iNr += GetAtFast(i)->GetNrTapes();
	}

	Unlock();

	return iNr;
}
//////////////////////////////////////////////////////////////////////
CCollection* CCollections::GetSafeRingCollection(WORD wNr)
{
	int i,c;

	CAutoCritSec acs(&m_cs, _T(__FUNCTION__), DVS_TIMEOUT);

	c = GetSize();
	for (i=0;i<c;i++)
	{
		if (GetAtFast(i)->GetSafeRingFor()==wNr)
		{
			return GetAtFast(i);
		}
	}

	return NULL;
}
//////////////////////////////////////////////////////////////////////
DWORD CCollections::GetMB()
{
	DWORD dwMB = 0;

	CAutoCritSec acs(&m_cs, _T(__FUNCTION__), DVS_TIMEOUT);

	for (int i=0;i<GetSize();i++)
	{
		dwMB += GetAtFast(i)->GetSizeMB();
	}

	acs.Unlock();

	return dwMB;
}
//////////////////////////////////////////////////////////////////////
CIPCInt64 CCollections::GetUsedBytes()
{
	CIPCInt64 iUsedMB = 0;
	int i;

	CAutoCritSec acs(&m_cs, _T(__FUNCTION__), DVS_TIMEOUT);

	for (i=0;i<GetSize();i++)
	{
		iUsedMB += GetAtFast(i)->GetSizeBytes();
	}

	acs.Unlock();

	iUsedMB += CIPCInt64(m_dwDirectoryCluster);

	return iUsedMB;
}
//////////////////////////////////////////////////////////////////////
DWORD CCollections::GetAlarmAndPreAlarmDefinedMB()
{
	DWORD dwMB = 0;

	CAutoCritSec acs(&m_cs, _T(__FUNCTION__), DVS_TIMEOUT);

	for (int i=0;i<GetSize();i++)
	{
		if (   GetAtFast(i)->IsAlarm()
			|| GetAtFast(i)->IsPreAlarm()
			)
		{
			dwMB += GetAtFast(i)->GetSizeMB();
		}
	}

	acs.Unlock();

	return dwMB;
}
//////////////////////////////////////////////////////////////////////
DWORD CCollections::GetAlarmAndPreAlarmUsedMB()
{
	CIPCInt64 iUsedMB = 0;
	int i;

	CAutoCritSec acs(&m_cs, _T(__FUNCTION__), DVS_TIMEOUT);

	for (i=0;i<GetSize();i++)
	{
		if (   GetAtFast(i)->IsAlarm()
			|| GetAtFast(i)->IsPreAlarm()
			)
		{
			iUsedMB += GetAtFast(i)->GetSizeBytes();
		}
	}

	acs.Unlock();

	iUsedMB += CIPCInt64(m_dwDirectoryCluster);

	return iUsedMB.GetInMB();
}
//////////////////////////////////////////////////////////////////////
int	CCollections::GetAlarmPercent()
{
	CIPCInt64 iUsedBytes = 0;
	CIPCInt64 iBytes=0;
	int i;

	CAutoCritSec acs(&m_cs, _T(__FUNCTION__), DVS_TIMEOUT);

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
BOOL CCollections::DeleteCollection(WORD wCollectionNr)
{
	int i;
	CCollection* pCollection;
							
	for (i=0;i<GetSize();i++)
	{
		pCollection = GetAtFast(i);
		if (pCollection->GetNr() == wCollectionNr)
		{
			if (pCollection->Delete())
			{
				theApp.m_Clients.DoIndicateDeleteCollection(wCollectionNr);
				Lock();
				RemoveAt(i);
				Unlock();
				WK_DELETE(pCollection);
				return TRUE;
			}
		}
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
void CCollections::ScanBackupDrive(CIPCDrive* pDrive, BOOL bIndicateToClients)
{
	CString sSearch;

	WK_TRACE(_T("scanning backup drive %s\n"),pDrive->GetRootString());

	sSearch = pDrive->GetRootString() + _T("*.*");

	CString sTmp;
	WIN32_FIND_DATA	found;
	HANDLE	hFound = NULL;

	// alle Verzeichnisse durchgehen
	hFound = FindFirstFile(LPCTSTR(sSearch), &found);
	
	if (hFound != INVALID_HANDLE_VALUE)
	{
		for (BOOL	bFound = TRUE; bFound; bFound=FindNextFile (hFound, &found)) 
		{
			if (found.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				sTmp = found.cFileName; 
				sTmp.MakeLower();
				if (sTmp == _T(".") || sTmp == _T("..") )
				{
					// akuelles und übergeordnetes Verzeichnis ignorieren
					continue;
				}
				DWORD dw = 0;
				if (1==_stscanf(sTmp,_T("dvb%x"),&dw))
				{
					ScanBackupDirectory(pDrive->GetRootString() + sTmp + _T('\\'),
									    pDrive,
										bIndicateToClients);
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
	WK_TRACE(_T("scanned backup drive %s\n"),pDrive->GetRootString());
}
//////////////////////////////////////////////////////////////////////
void CCollections::ScanBackupDirectory(const CString& sBackupDir, 
									   CIPCDrive* pDrive, 
									   BOOL bIndicateToClients)
{
	CString sSearch;

	CVDBSequenceMap map;
	map.Load(sBackupDir + _T("sequence.dbf"));

	WK_TRACE(_T("scanning backup dir %s\n"),sBackupDir);
	sSearch = sBackupDir + _T("*.*");

	CString sTmp;
	WIN32_FIND_DATA	found;
	HANDLE	hFound = NULL;

	// alle Verzeichnisse durchgehen
	hFound = FindFirstFile(LPCTSTR(sSearch), &found);
	
	if (hFound != INVALID_HANDLE_VALUE)
	{
		for (BOOL	bFound = TRUE; bFound; bFound=FindNextFile (hFound, &found)) 
		{
			if (found.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				sTmp = found.cFileName; 
				sTmp.MakeLower();
				if (sTmp == _T(".") || sTmp == _T("..") )
				{
					// akuelles und übergeordnetes Verzeichnis ignorieren
					continue;
				}
				DWORD dw = 0;
				if (1==_stscanf(sTmp,_T("arch%x"),&dw))
				{
					// dynamically create a new Collection id for
					// this one if not already exists
					CCollection* pCollection = GetCollectionByCameraNr(LOBYTE((WORD)dw));
					if (pCollection)
					{
						pCollection->Scan(sBackupDir + sTmp,pDrive,TRUE,bIndicateToClients,map);
					}
					else
					{
						pCollection = CreateBackupCollection(sBackupDir,(WORD)dw);
						if (pCollection)
						{
							pCollection->Scan(sBackupDir + sTmp,pDrive,TRUE,bIndicateToClients,map);
							Add(pCollection);
							theApp.m_Clients.DoIndicateNewCollection(*pCollection);
						}
						else
						{
							// signals that the Collection already exists
						}
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
CCollection* CCollections::CreateBackupCollection(const CString& sBackupDir, 
												  WORD wOriginalNr)
{
	int i;
	CCollection* pCollection = NULL;
	CAutoCritSec acs(&m_cs);

	for (i=0;i<GetSize();i++)
	{
		pCollection = GetAtFast(i);
		if (    pCollection->IsBackup()
			&& 	(wOriginalNr == pCollection->GetNr()))
		{
			// already existing Collection
			return pCollection;
		}
	}

	acs.Unlock();

	pCollection = new CCollection(sBackupDir,wOriginalNr);

	return pCollection;
}
//////////////////////////////////////////////////////////////////////
void CCollections::DeleteOldestRingSequence()
{
	// just find out the archive
	// with the oldest sequence
	CSystemTime stOldest;
	CSystemTime stOldestOK;
	CSystemTime stPivot;
	CCollection* pPivot = NULL;
	CCollection* pCollection = NULL;
	CCollection* pCollectionOK = NULL;

	stOldest.GetLocalTime();
	stOldestOK = stOldest;

	Lock();
	for (int i=0;i<GetSize();i++)
	{
		pPivot = GetAtFast(i);
		if (   pPivot->IsRing()
			&& pPivot->GetNrTapes())
		{
			// must be ring
			// and must have any sequence
			// else GetTapeIndex would fail with null pointer
			CTape* pTape = pPivot->GetOldestTape();
			if (pTape)
			{
				stPivot = pTape->GetFirstTime();
				if (stPivot<stOldestOK)
				{
					if (stPivot<stOldest)
					{
						pCollection = pPivot;
						stOldest = stPivot;
					}
					
					if (pPivot->GetSizeBytes().GetInMB()>
						pPivot->GetSizeMB())
					{
						// real archive size is larger
						// than minimum archive size
						pCollectionOK = pCollection;
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

	if (pCollectionOK)
	{
		/*
		WK_TRACE(_T("deleting system wide oldest sequence OK of Archive %04x,%04x, %s\n"),
			pCollectionOK->GetNr(),
			pCollectionOK->GetTapeIndex(0)->GetTapeNr(),
			pCollectionOK->GetTapeIndex(0)->GetFirstTime().GetDateTime());
			*/
		pCollectionOK->DeleteOldestTape();
	}
	else if (pCollection)
	{
		// nothing found
		// but we must delete something use oldest ever
		/*
		WK_TRACE(_T("deleting system wide oldest sequence ever of Archive %04x,%04x, %s\n"),
			pCollection->GetNr(),
			pCollection->GetTapeIndex(0)->GetTapeNr(),
			pCollection->GetTapeIndex(0)->GetFirstTime().GetDateTime());
			*/
		pCollection->DeleteOldestTape();
	}
}
//////////////////////////////////////////////////////////////////////
void CCollections::CloseAll()
{
	// alle Dateien nach 1 Minute schliessen
	if (GetTimeSpan(m_dwTickLastClose)>60*1000)
	{
		if (theApp.m_bTraceOpen)
		{
			WK_TRACE(_T("closing all sequences\n"));
		}
		for (int i=0;i<GetSize();i++)
		{
			if (GetAtFast(i)->IsAlarm())
			{
				GetAtFast(i)->CloseAll();
			}
		}
		m_dwTickLastClose = GetTickCount();
	}
}
//////////////////////////////////////////////////////////////////////
CTape* CCollections::GetNextSequence(WORD wArchivNr, WORD wSequenceNr)
{
	CTape* pReturn = NULL;

	Lock();

	if (GetSize())
	{
		CCollection* pCollection = NULL;
		
		if (wArchivNr == 0)
		{
			pCollection = GetAtFast(0);
		}
		else
		{
			pCollection = GetCollection(wArchivNr);
		}
		
		while (pCollection && pReturn==NULL)
		{
			CTape* pTape = pCollection->GetNextSequence(wSequenceNr);

			if (pTape)
			{
				pReturn = pTape;
			}
			else
			{
				pCollection = GetNextArchive(pCollection->GetNr());
				wSequenceNr = 0;
			}
		}
	}

	Unlock();

	return pReturn;
}
//////////////////////////////////////////////////////////////////////
CCollection* CCollections::GetNextArchive(WORD wArchiveNr)
{
	CCollection* pArchive = NULL;
	Lock();

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

	Unlock();
	return pArchive;
}
