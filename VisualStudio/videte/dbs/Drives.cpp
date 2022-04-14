/* GlobalReplace: CDVRDrive --> CIPCDrive */
// Drives.cpp: implementation of the CDrives class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DBS.h"
#include "Drives.h"
#include "CopyReadOnlyVersion.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CDrives::CDrives()
{

}
//////////////////////////////////////////////////////////////////////
CDrives::~CDrives()
{
}
//////////////////////////////////////////////////////////////////////
// find's out the drive with most free space
CIPCDrive* CDrives::GetFreeDrive()
{
	CIPCDrive* pDriveMax = NULL;
	CIPCDrive* pDrive;
	DWORD   dwFixed = 0;
	DWORD   dwFixedUsed = 0;
	DWORD	dwFreeDrive = 0;
	DWORD	dwFree		= 0;
	DWORD	dwFreeMax	= 0;
	int i;

	for (i=0;i<GetSize();i++)
	{
		pDrive = GetAtFast(i);
		if (   pDrive->IsEnabled()
			&& (pDrive->IsDatabase()
			||  pDrive->IsRemoveableDatabase()))
		{
			pDrive->CheckSpace();
			dwFixed = theApp.m_Archives.GetFixedMB(pDrive);
			dwFreeDrive = pDrive->GetFreeMB();

			if (dwFixed>0)
			{
				// is there any archive fixed to this drive ?
				// calc space use by these archives
				dwFixedUsed	= theApp.m_Archives.GetFixedUseMB(pDrive);
				if (dwFixed>=dwFixedUsed)
				{
					if (dwFreeDrive > (dwFixed - dwFixedUsed))
					{
						dwFree = dwFreeDrive - (dwFixed - dwFixedUsed);
					}
					else
					{
						dwFree = 0;
					}
				}
				else
				{
					dwFree = 0;
				}
			}
			else
			{
				dwFree = dwFreeDrive;
			}

			if (theApp.TraceDrives())
			{
				WK_TRACE(_T("%s , fixed=%04d fixeduse=%04d freedrive=%04d free=%04d\n"),pDrive->GetRootString(),dwFixed,dwFixedUsed,dwFreeDrive,dwFree);
			}

			if (pDriveMax==NULL || dwFree>dwFreeMax)
			{
				pDriveMax = pDrive;
				dwFreeMax = dwFree;
			}
		}
	}

	if (theApp.TraceDrives())
	{
		WK_TRACE(_T("using %s %04d free\n"),pDriveMax->GetRootString(),dwFreeMax);
	}
	return pDriveMax;
}
///////////////////////////////////////////////////////////////////////////////////
CIPCDrive* CDrives::GetDriveEx(_TCHAR cDriveLetter)
{
	for (int i=0;i<GetSize();i++)
	{
		CIPCDrive *pD = GetAtFast(i);
		if (pD->GetLetter() == cDriveLetter)
		{
			return pD;
		}
	}
	return NULL;
}
///////////////////////////////////////////////////////////////////////////////////
BOOL CDrives::IsRO(const CString& sRoot)
{
	CString sTest1,sTest2,sTest3;
	BOOL bRet = FALSE;

	sTest1 = sRoot + _T("autorun.inf");
	sTest2.Format(_T("%sdbb0001"),sRoot);
	sTest3.Format(_T("%sdvb0001"),sRoot);
	
	if (   DoesFileExist(sTest1) 
		&& (DoesFileExist(sTest2)||DoesFileExist(sTest3))
		)
	{
		bRet = TRUE;
	}

	return bRet;
}
///////////////////////////////////////////////////////////////////////////////////
BOOL CDrives::IsEmpty(const CString& sRoot)
{
	CString sDbs;
	BOOL bRet = TRUE;

	sDbs.Format(_T("%s%s"),sRoot,CArchiv::m_sRootName);
	if (DoesFileExist(sDbs))
	{
		// check for archives inside
		CFileFind finder;
		BOOL bWorking = finder.FindFile(sDbs + _T("\\*.*"));
		
		while (bWorking && bRet)
		{
			bWorking = finder.FindNextFile();
			bRet = !finder.IsDots();
		}
	}
	if (bRet)
	{
		sDbs.Format(_T("%s%s0001"),sRoot,CArchiv::m_sBackupName);
		if (DoesFileExist(sDbs))
		{
			// check for archives inside
			CFileFind finder;
			BOOL bWorking = finder.FindFile(sDbs + _T("\\*.*"));
			
			while (bWorking && bRet)
			{
				bWorking = finder.FindNextFile();
				bRet = !finder.IsDots();
			}
		}
	}

	return bRet;
}
///////////////////////////////////////////////////////////////////////////////////
BOOL CDrives::IsRW(const CString& sRoot)
{
	return !IsRO(sRoot) && !IsEmpty(sRoot);
}
///////////////////////////////////////////////////////////////////////////////////
BOOL CDrives::DoRWToRO(CIPCDrive* pDrive)
{
	BOOL bRet = FALSE;
	CWordArray archives;

	WK_TRACE(_T("RO2RW converting drive %s\n"),pDrive->GetRootString());

	// 1. copy RO software
	WK_TRACE(_T("RO2RW copying RO software %s\n"),pDrive->GetRootString());
	CCopyReadOnlyVersion crov(pDrive->GetRootString().Left(2));
	if (!crov.IsAlreadyDone())
	{
		crov.Run(TRUE);
	}
	WK_TRACE(_T("RO2RW copied RO software %s\n"),pDrive->GetRootString());
	
	// 2. rename dbs to dbb0001
	WK_TRACE(_T("RO2RW renaming dbs %s\n"),pDrive->GetRootString());

	CString s;
	s = CArchiv::m_sBackupName + _T("0001");

	if (!MoveFile(pDrive->GetRootString()+CArchiv::m_sRootName,pDrive->GetRootString()+s))
	{
		WK_TRACE_ERROR(_T("cannot rename dir %s\n"),GetLastErrorString());
	}
	WK_TRACE(_T("RO2RW renamed dbs %s\n"),pDrive->GetRootString());
	
	// 3. write camera name files
	WK_TRACE(_T("RO2RW writing camera name files %s\n"),pDrive->GetRootString());
	CString sDir = pDrive->GetRootString() + s;
	{
		CFileFind finder;
		BOOL bWorking = finder.FindFile(sDir + _T("\\arch????.*"));
		
		while (bWorking)
		{
			bWorking = finder.FindNextFile();
			if (   !finder.IsDots()
				&& finder.IsDirectory())
			{
				TRACE(_T("archive %s found on RHD\n"),finder.GetFileName());
				DWORD dwArchiveNr = 0;

				if (1 == _stscanf(finder.GetFileName(),_T("arch%04lx"),&dwArchiveNr))
				{
					WORD wArchiveNr = (WORD)dwArchiveNr;
					archives.Add(wArchiveNr);

					CString sSource,sDestination;

					sSource.Format(_T("%s\\%04lx.dbf"),theApp.m_sMapDirectory,wArchiveNr);
					sDestination.Format(_T("%s%s\\%04lx.dbf"),pDrive->GetRootString(),s,wArchiveNr);
					if (CopyFile(sSource,sDestination,FALSE))
					{
						WK_TRACE(_T("copy file %s,%s\n"),sSource,sDestination);
					}
					else
					{
						WK_TRACE_ERROR(_T("cannot copy file %s,%s\n"),sSource,GetLastErrorString());
					}
				}
			}
		}
	}
	WK_TRACE(_T("RO2RW wrote camera name files %s\n"),pDrive->GetRootString());
	
	
	// 4  write backup.dbi
	WK_TRACE(_T("RO2RW writing backup.dbi %s\n"),pDrive->GetRootString());
	{
		CString sDbi;
		sDbi = sDir + _T("\\backup.dbi");
		CBackup::WriteBackupDbi(sDbi,_T(""),archives);
	}
	WK_TRACE(_T("RO2RW wrote backup.dbi %s\n"),pDrive->GetRootString());
	
	
	// 5. write sequence hash table
	WK_TRACE(_T("RO2RW writing sequence hash table %s\n"),pDrive->GetRootString());
	CVDBSequenceMap map;
	// search for all dbf's and check the map entries
	for (int i=0;i<archives.GetSize();i++)
	{
		WORD wArchiveNr = archives.GetAt(i);
		CString sDir2;
		sDir2.Format(_T("%s\\arch%04lx"),sDir,wArchiveNr);
		CFileFind finder;
		BOOL bWorking = finder.FindFile(sDir2 + _T("\\*.dbf"));
		
		while (bWorking)
		{
			bWorking = finder.FindNextFile();
			if (   !finder.IsDots()
				&& !finder.IsDirectory())
			{
				WORD wSequenceNr = 0;
				BYTE bFlags = 0;
				CSequence::GetNrAndFlags(finder.GetFileName(),wSequenceNr,bFlags);
				TRACE(_T("Sequence %s %04lx\n"),finder.GetFilePath(),wSequenceNr);

				CVDBSequenceHashEntry* pEntry = theApp.GetSequenceMap().GetSequenceHashEntry(wArchiveNr,wSequenceNr);
				if (pEntry)
				{
					map.SetSequenceHashEntry(pEntry->GetArchiveNr(),
											 pEntry->GetSequenceNr(),
											 pEntry->GetRecords(),
											 pEntry->GetFirst(),
											 pEntry->GetLast(),
											 pEntry->GetName());
					theApp.GetSequenceMap().DeleteSequenceHashEntry(wArchiveNr,wSequenceNr);
				}
			}
		}
		TRACE(_T("\n"));
	}
	if (map.GetCount())
	{
		map.Save(sDir+_T("\\sequence.dbf"));
	}
	WK_TRACE(_T("RO2RW wrote sequence hash table %s\n"),pDrive->GetRootString());
	

	WK_TRACE(_T("RO2RW converted drive %s\n"),pDrive->GetRootString());
	return bRet;
}
///////////////////////////////////////////////////////////////////////////////////
BOOL CDrives::DoROToRW(CIPCDrive* pDrive)
{
	BOOL bRet = FALSE;
	CString sDir = pDrive->GetRootString() + CArchiv::m_sBackupName + _T("0001");

	WK_TRACE(_T("RW2RO converting drive %s\n"),pDrive->GetRootString());

	// 1. read and delete sequence hash table
	{
		CVDBSequenceMap map;
		map.Load(sDir+_T("\\sequence.dbf"));
		theApp.GetSequenceMap().AppendMap(map);
		map.Delete();
	}

	// 2. delete backup.dbi
	{
		CString sDbi;
		sDbi = sDir + _T("\\backup.dbi");
		DeleteFile(sDbi);
	}
	
	// 3. delete camera name files
	WK_TRACE(_T("RW2RO deleting camera name files %s\n"),pDrive->GetRootString());
	{
		CFileFind finder;
		BOOL bWorking = finder.FindFile(sDir + _T("\\*.dbf"));
		
		while (bWorking)
		{
			bWorking = finder.FindNextFile();
			if (   !finder.IsDots()
				&& !finder.IsDirectory())
			{
				if (DeleteFile(finder.GetFilePath()))
				{
					WK_TRACE(_T("delete file %s\n"),finder.GetFilePath());
				}
				else
				{
					WK_TRACE_ERROR(_T("cannot delete file %s,%s\n"),finder.GetFilePath(),GetLastErrorString());
				}
			}
		}
	}
	WK_TRACE(_T("RW2RO delete camera name files %s\n"),pDrive->GetRootString());
	
	
	// 4. rename dbb0001 to dbs
	WK_TRACE(_T("RW2RO renaming %s\n"),pDrive->GetRootString());
	if (!MoveFile(pDrive->GetRootString()+CArchiv::m_sBackupName + _T("0001"),pDrive->GetRootString()+CArchiv::m_sRootName))
	{
		WK_TRACE_ERROR(_T("cannot rename dir %s\n"),GetLastErrorString());
	}
	WK_TRACE(_T("RW2RO renamed %s\n"),pDrive->GetRootString());
	// 5. delete RO software
	WK_TRACE(_T("RW2RO deleting RO software %s\n"),pDrive->GetRootString());
	CCopyReadOnlyVersion crov(pDrive->GetRootString().Left(2));
	crov.Run(FALSE);
	WK_TRACE(_T("RW2RO deleted RO software %s\n"),pDrive->GetRootString());

	WK_TRACE(_T("RW2RO converted drive %s\n"),pDrive->GetRootString());

	return bRet;
}
