// Collection.cpp: implementation of the CCollection class.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DVStorage.h"
#include "Collection.h"

#include "FindData.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static TCHAR BASED_CODE szBYTE[] = _T("%02lx");
static TCHAR BASED_CODE szWORD[] = _T("%04lx");
static TCHAR BASED_CODE szDWORD[] = _T("%08lx");

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
void CCollection::Init()
{
	CAutoCritSec acs(&m_csCollection, _T(__FUNCTION__), DVS_TIMEOUT);

/*
	m_dwStatTime = 60*60*1000;
	m_bIsRunning = TRUE;
	m_dwMaxInQueue = 0;
*/

	m_wTapeID = 0;
	m_bSearch = FALSE;
	m_dwNrOfTapesDebit = 20;
	m_dwLastMoveTick = 0;
	m_bMoved = FALSE;
	m_bFullSend = FALSE;
	m_b60Send = FALSE;
	m_bFirstSequenceAfterRestart = FALSE;

	m_CodeBase.safety = FALSE;
	m_CodeBase.errOff = TRUE;
	m_CodeBase.accessMode = OPEN4DENY_NONE;
	m_CodeBase.optimize = OPT4ALL;
	m_CodeBase.optimizeWrite = OPT4ALL;

	m_iFolderCluster = 0;
	m_iSizeBytes = 0;
	m_iTapeSizeBytes = 0;
	m_bDoFieldCheck = TRUE;

	acs.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CCollection::SetSize(DWORD dwSize)
{
	m_dwSizeMB = dwSize;
	InitTapeDebit();
	m_iSizeBytes = (__int64)(m_dwSizeMB) * 1024 * 1024;
	if (IsRing())
	{
//		m_iTapeSizeBytes = DVS_SEQUENCE_SIZE;
		m_iTapeSizeBytes = theApp.m_Archives.GetSequenceSizeInMB() * 1024 * 1024;
	}
	else
	{
		// Alarm/Pre Alarm
		m_iTapeSizeBytes = (DWORD)(m_iSizeBytes.GetInt64() / ((__int64)m_dwNrOfTapesDebit));
	}
	AdjustSize();
	m_sFormat = InitialFormat();
}
//////////////////////////////////////////////////////////////////////
void CCollection::SetName(const CString sName)
{
	UTRACE(_T("CCollection::SetName:%s\n"), sName);
	m_sName = sName;
}
//////////////////////////////////////////////////////////////////////
CCollection::CCollection(const CArchivInfo& ai)
	: /*CWK_Thread(ai.GetName(),NULL) ,*/ m_FieldInfo(m_CodeBase)
{
	Init();

	// copy data from config
	m_idCollection   = ai.GetID();
	m_wSafeRingFor = ai.GetSafeRingFor();
	m_Typ = ai.GetType();
	m_sFolder = _T("dvs");

	SetName(ai.GetName());
	SetSize(ai.GetSizeMB());

	InitFields();
}
//////////////////////////////////////////////////////////////////////
CCollection::CCollection(const CString& sBackupDir, WORD wArchivNr)
	: /*CWK_Thread(sBackupDir,NULL),*/ m_FieldInfo(m_CodeBase)
{
	Init();
	m_idCollection.Set(SECID_GROUP_ARCHIVE,wArchivNr);
	m_sBackupDir = sBackupDir;
	int p;
	p = sBackupDir.Find(_T('\\'));
	m_sFolder = sBackupDir.Mid(p+1);
	if (m_sFolder.GetAt(m_sFolder.GetLength()-1)==_T('\\'))
	{
		m_sFolder = m_sFolder.Left(m_sFolder.GetLength()-1);
	}
	m_sFormat = InitialFormat();
	m_Typ = BACKUP_ARCHIV;
	m_CodeBase.readOnly = TRUE;
}
//////////////////////////////////////////////////////////////////////
void CCollection::InitTapeDebit()
{
	if (m_dwSizeMB>25)
	{
		m_dwNrOfTapesDebit = 25;
	}
	if (m_dwSizeMB>50)
	{
		m_dwNrOfTapesDebit = 33;
	}
	if (m_dwSizeMB>100)
	{
		m_dwNrOfTapesDebit = 50;
	}
	if (m_dwSizeMB>500)
	{
		m_dwNrOfTapesDebit = 100;
	}
}
//////////////////////////////////////////////////////////////////////
void CCollection::InitFields()
{
	CAutoCritSec acs(&m_csCollection, _T(__FUNCTION__), DVS_TIMEOUT);

	m_FieldInfo.add(DVR_VERSION, 'C', 2, 0);
	if (IsAlarmList())
	{
		m_FieldInfo.add(DVR_TYP , 'C', 1, 0);
		m_FieldInfo.add(DVR_DATE, 'C', 8, 0);
		m_FieldInfo.add(DVR_TIME, 'C', 6, 0);
	}
	else
	{
		// picture record data
		m_FieldInfo.add(DVR_CAMERANR,       'C', 8, 0);
		m_FieldInfo.add(DVR_RESOLUTION,     'C', 1, 0);
		m_FieldInfo.add(DVR_COMPRESSION,    'C', 2, 0);
		m_FieldInfo.add(DVR_COMPRESSIONTYP, 'C', 2, 0);
		m_FieldInfo.add(DVR_TYP,            'C', 1, 0);

		m_FieldInfo.add(DBP_FFV_REF,		 'C', 8, 0);
		m_FieldInfo.add(DBP_RWV_REF,		 'C', 8, 0);
		m_FieldInfo.add(DBP_FFA_REF,		 'C', 8, 0);
		m_FieldInfo.add(DBP_RWA_REF,		 'C', 8, 0);
		
		m_FieldInfo.add(DVR_DATE,           'C', 8, 0);
		m_FieldInfo.add(DVR_TIME,           'C', 6, 0);
		m_FieldInfo.add(DVR_MS,             'C', 3, 0);

		m_FieldInfo.add(DVR_DATA_POS,       'C', 8, 0);
		m_FieldInfo.add(DVR_DATA_LEN,       'C', 8, 0);
		m_FieldInfo.add(DVR_CHECK_SUM,      'C', 2, 0);

		m_FieldInfo.add(DVD_MD_X,           'C', 4, 0);
		m_FieldInfo.add(DVD_MD_Y,           'C', 4, 0);
		m_FieldInfo.add(DVD_MD_X2,          'C', 4, 0);
		m_FieldInfo.add(DVD_MD_Y2,          'C', 4, 0);
		m_FieldInfo.add(DVD_MD_X3,          'C', 4, 0);
		m_FieldInfo.add(DVD_MD_Y3,          'C', 4, 0);
		m_FieldInfo.add(DVD_MD_X4,          'C', 4, 0);
		m_FieldInfo.add(DVD_MD_Y4,          'C', 4, 0);
		m_FieldInfo.add(DVD_MD_X5,          'C', 4, 0);
		m_FieldInfo.add(DVD_MD_Y5,          'C', 4, 0);

		m_FieldInfo.add(DVD_MD_S,           'C', 1, 0);
	}

	acs.Unlock();
}
//////////////////////////////////////////////////////////////////////
CCollection::~CCollection()
{
	CAutoCritSec acs(&m_csCollection, _T(__FUNCTION__), DVS_TIMEOUT);
	SafeDeleteAll();
/*
	DWORD dwSize = m_ImageDataQueue.GetCount();
	WK_TRACE(_T("thread %s had max %d pictures in queue and now still %d\n"),
		m_sName,m_dwMaxInQueue,dwSize);
	m_ImageDataQueue.SafeDeleteAll();
*/
	CAutoCritSec acsCB(&m_csCB);
	m_CodeBase.initUndo();
	acsCB.Unlock();
	acs.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CCollection::SetFirstSequenceAfterRestart()
{
	CAutoCritSec acs(&m_csCollection, _T(__FUNCTION__), DVS_TIMEOUT);
	m_bFirstSequenceAfterRestart = TRUE;
	acs.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CCollection::Trace()
{
	DWORD d = GetSizeMB();
	CString s = Format();
	WK_TRACE(_T("%s\n"),s);
	WK_STAT_LOG(_T("CollectionSize"),d,m_sName);

	CTape* pTape = NULL;

	CAutoCritSec acs(&m_csCollection, _T(__FUNCTION__), DVS_TIMEOUT);

	for (int i=0;i<GetSize();i++)
	{
		pTape = GetAtFast(i);
		pTape->Trace();
	}

	acs.Unlock();
}
//////////////////////////////////////////////////////////////////////
CString CCollection::InitialFormat()
{
	CString s;
	CString sTyp;

	switch (GetType())
	{
	case NO_ARCHIV:
		sTyp.LoadString(IDS_UNDEFINED);
		break;
	case ALARM_ARCHIV:
		sTyp.LoadString(IDS_ALARM);
		break;
	case RING_ARCHIV:
		sTyp.LoadString(IDS_RING);
		break;
	case SICHERUNGS_RING_ARCHIV:
		sTyp.LoadString(IDS_SAFERING);
		break;
	case ALARM_LIST_ARCHIV:
		sTyp.LoadString(IDS_ALARM_LIST);
		break;
	}

	s.Format(_T("0x%04lx %10s %10s, %06d MB"),
		GetNr(),m_sName,sTyp,GetSizeMB());

	return s;
}
//////////////////////////////////////////////////////////////////////
CString CCollection::Format()
{
	CString s;

	CAutoCritSec acs(&m_csCollection, _T(__FUNCTION__), DVS_TIMEOUT);

	CString sOldest;
	if (GetSize()>0)
	{
		sOldest = GetAtFast(0)->GetFirstTime().GetDateTime();
	}
	s.Format(_T("(%06dMB) Seq(%03d) Frames(%08d) Old(%s)"),
		GetSizeBytes().GetInMB(),GetSize(),
		GetNumberOfPictures(),
		sOldest);
	acs.Unlock();

	s = m_sFormat + s;

	return s;
}
//////////////////////////////////////////////////////////////////////
void CCollection::NewFolder(const CString& sDir)
{
	UTRACE(_T("CCollection::NewFolder:%s\n"), sDir);
	CIPCDrive* pDrive = theApp.m_Drives.GetDrive((char)sDir[0]);
	if (pDrive)
	{
		m_iFolderCluster += 2*pDrive->GetClusterSize();
		// the dir
		// and the index.dbi file
	}
}
//////////////////////////////////////////////////////////////////////
void CCollection::DeleteFolder(const CString& sDir)
{
	UTRACE(_T("CCollection::DeleteFolder:%s\n"), sDir);
	CIPCDrive* pDrive = theApp.m_Drives.GetDrive((char)sDir[0]);
	if (pDrive)
	{
		m_iFolderCluster -= pDrive->GetClusterSize();
	}
}
//////////////////////////////////////////////////////////////////////
CString	CCollection::GetFolder() const
{
	CString ret;

	ret.Format(_T("%s\\arch%04lx"),m_sFolder,GetNr());

	return ret;
}
//////////////////////////////////////////////////////////////////////
void CCollection::DeleteDatabaseDrive(const CString& sRootString)
{
	UTRACE(_T("CCollection::DeleteDatabaseDrive:%s\n"), sRootString);
	// delete all tapes on this drive
	CAutoCritSec acs(&m_csCollection, _T(__FUNCTION__), DVS_TIMEOUT);
	CTape* pTape;
	for (int i=GetSize()-1;i>=0;i--)
	{
		pTape = GetAtFast(i);
		if (0 == sRootString.CompareNoCase(pTape->GetDrive()))
		{
			// delete it
			DeleteTape(pTape->GetNr());
		}
	}
	acs.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CCollection::DeleteDatabaseDirectory(const CString& sDirectory)
{
	UTRACE(_T("CCollection::DeleteDatabaseDirectory:%s\n"), sDirectory);
	// delete all tapes on this drive
	CAutoCritSec acs(&m_csCollection, _T(__FUNCTION__), DVS_TIMEOUT);
	CTape* pTape;
	for (int i=GetSize()-1;i>=0;i--)
	{
		pTape = GetAtFast(i);
		CString sTapeDirectory = pTape->GetDirectory();
		sTapeDirectory = sTapeDirectory.Left(sDirectory.GetLength());
		if (0 == sDirectory.CompareNoCase(sTapeDirectory))
		{
			// delete it
			DeleteTape(pTape->GetNr());
		}
	}
	acs.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CCollection::Scan(const CString& sDir, 
					   CIPCDrive* pDrive, 
					   BOOL bBackup,
					   BOOL bIndicate,
					   const CVDBSequenceMap& map)
{
	WK_TRACE(_T("scanning sequences in %s\n"),sDir);
	CString sSearch;
	CString sFolder;

	sSearch = sDir + _T("\\*.*");

	if (bBackup)
	{
		int p = sDir.Find(_T('\\'));
		sFolder = sDir.Mid(p+1);
	}

	CString sTmp;
	DWORD dwNrOfFiles = 0;
	CFindDatas dbfFiles;
	CFindDatas datFiles;
	WIN32_FIND_DATA	data;
	HANDLE	hFound = NULL;

	// alle Verzeichnisse durchgehen
	hFound = FindFirstFile(LPCTSTR(sSearch), &data);
	
	if (hFound == INVALID_HANDLE_VALUE)
	{
		FindClose(hFound);
		return; // EXIT
	}

	m_iFolderCluster += pDrive->GetClusterSize();

	for (BOOL bFound = TRUE; bFound; bFound=FindNextFile (hFound, &data)) 
	{
		sTmp = data.cFileName; 
		sTmp.MakeLower();
		if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (sTmp==_T(".") || sTmp==_T(".."))
			{
				// akuelles und übergeordnetes Verzeichnis ignorieren
				continue;
			}
			else
			{
				// old format or foreign file
				WK_TRACE_ERROR(_T("foreign dir found %s\n"),sDir+_T('\\')+sTmp);
				dwNrOfFiles++;
			}
		}
		else
		{
			dwNrOfFiles++;
			if (-1!=sTmp.Find(_T(".dbf")))
			{
				dbfFiles.Add(new CFindData(data));
			}
			else if (-1!=sTmp.Find(_T(".dat")))
			{
				datFiles.Add(new CFindData(data));
			}
			else
			{
				WK_TRACE(_T("foreign file found %s\\%s\n"),sDir,sTmp);
			}
		}
	}
	FindClose(hFound); 


	int i,j;
	BOOL bDat;
	CFindData*	pDbf;
	CFindData*	pDat;

	for (i=0;i<dbfFiles.GetSize();i++)
	{
		pDbf = dbfFiles.GetAtFast(i);
		bDat = FALSE;
		pDat = NULL;
		if (IsAlarmList())
		{
			bDat = TRUE;
		}
		else
		{
			// search for dat image file
			for (j=0;j<datFiles.GetSize();j++)
			{
				pDat = datFiles.GetAtFast(j);
				if (0==_tcsnicmp(pDbf->cFileName,
								pDat->cFileName,_tcslen(pDbf->cFileName)-3))
				{
					bDat = TRUE;
					datFiles.RemoveAt(j);
					break;
				}
			}
		}

		if (bDat)
		{
			CString sDbf(pDbf->cFileName);
			CString sID;
			sID = sDbf.Mid(4,4);
			DWORD dwID   = 0;
			_stscanf(sID,_T("%x"),&dwID);
			CTape* pTape = GetTapeID((WORD)dwID);
			CTape* pNewTape;
			
			if (pTape)
			{
				dwID = GetNewID();
				WK_TRACE(_T("%s tape nr already exists %s %s %d->%d\n"),
						  GetName(),sDir,sDbf,pTape->GetNr(),dwID);
			}
			pNewTape = new CTape(0,this,pDrive,(WORD)dwID);
			if (pNewTape->Scan(pDbf,pDat,sFolder,map))
			{
				if (   pTape
					&& (pNewTape->GetNrOfRecords() <= pTape->GetNrOfRecords())
					&& (pNewTape->GetFirstTime() >= pTape->GetFirstTime())
					&& (pNewTape->GetLastTime() <= pTape->GetLastTime())
					)
				{
					WK_TRACE(_T("%s tape already exists %s %s \n"),GetName(),sDir,sDbf);
					// same tape don't take it twice
					WK_DELETE(pNewTape);
				}
				else
				{
					AddScannedTape(pNewTape);
					if (bIndicate)
					{
						if (!IsPreAlarm())
						{
							theApp.m_Clients.DoIndicateNewTape(*pNewTape,0,0);
						}
					}
				}
			}
			else
			{
				pNewTape->Delete();
				WK_DELETE(pNewTape);
			}
		}
		else
		{
			// dbf found but not dbd or dbx
			CString sName = sDir + _T("\\") + pDbf->cFileName;
			WK_TRACE(_T("dbf found but not dat %s\n"),sName);
//			TRACE(_T("DeleteFile(%s)\n"),sName);
			if (bBackup || DeleteFile(sName))
			{
				dwNrOfFiles--;
			}
			else
			{
				WK_TRACE_ERROR(_T("cannot delete %s, %s\n"),sName,GetLastErrorString());
			}
		}
		WK_DELETE(pDat);
	} // for dbfFiles

	for (j=0;j<datFiles.GetSize();j++)
	{
		pDat = datFiles.GetAtFast(j);
		CString sName = sDir + _T("\\") + pDat->cFileName;
		WK_TRACE(_T("dat found but not dbf %s\n"),sName);
//		TRACE(_T("DeleteFile(%s)\n"),sName);
		if (bBackup || DeleteFile(sName))
		{
			dwNrOfFiles--;
		}
		else
		{
			WK_TRACE_ERROR(_T("cannot delete %s, %s\n"),sName,GetLastErrorString());
		}
	}

	if (!bBackup)
	{
		if (dwNrOfFiles == 0)
		{
			// no file in dir just remove it
			TRACE(_T("RemoveDirectory(%s)\n"),sDir);
			if (RemoveDirectory(sDir))
			{
				WK_TRACE(_T("removing empty dir %s\n"),sDir);
			}

		}
	}

	datFiles.DeleteAll();
	dbfFiles.DeleteAll();

	if (IsBackup())
	{
		m_dwSizeMB = GetSizeBytes().GetInMB();
		m_sName.Format(_T("%02d"),(GetNr()&0xFF)+1);
		m_sFormat = InitialFormat();
	}

	if (bBackup)
	{
		BOOL bCancel = FALSE;
		Sort(bCancel);
	}
	WK_TRACE(_T("scanned sequences in %s\n"),sDir);
}
/////////////////////////////////////////////////////////////////////
CTape* CCollection::CheckNewTape(BOOL bSuspect, 
								 BOOL& bNewSequenceCreated, 
								 const CImageData& data,
								 WORD& wPrevSequenceNr,
								 DWORD& dwNrOfRecords)
{
	CTape* pTape = NULL;
	DWORD dwSizeOfNextPicture = data.GetLength();
	BOOL bIFrame = data.IsIFrame();

	CAutoCritSec acs(&m_csCollection, _T(__FUNCTION__), DVS_TIMEOUT);

	// check for empty Collection or restart
	if ((GetNrTapes()==0) || (m_bFirstSequenceAfterRestart))
	{
		if (bIFrame)
		{
			// no Tapes or restart create a new one
			pTape = GetNewTape(bSuspect);
			bNewSequenceCreated = TRUE;
			if (pTape)
			{
				if (pTape->Create())
				{
					m_bFirstSequenceAfterRestart = FALSE;
				}
				else
				{
					RemoveTape(pTape->GetNr());
					WK_DELETE(pTape);
				}
			}
			acs.Unlock();
			return pTape;
		}
		else
		{
			// start with P Frame makes no sense
			acs.Unlock();
			return NULL;
		}
	}


	acs.Unlock();

	// we have Tapes, get the last one
	pTape = GetAtFast(GetSize()-1);

	BOOL bFieldsChanged = FALSE;
	if (m_bDoFieldCheck)
	{
		bFieldsChanged = !pTape->AreFieldsEqual();
		if (bFieldsChanged)
		{
			WK_TRACE(_T("fields changed creating new tape\n"));
		}
		else
		{
			// WK_TRACE(_T("fields equal\n"));
		}
	}

	BOOL bTapeToLarge = pTape->IsFull(dwSizeOfNextPicture);
	BOOL bEmpty = pTape->IsEmpty();
	BOOL bTypeDifferent =    (bSuspect && !pTape->IsSuspect())
						  || (!bSuspect && pTape->IsSuspect())
						  ;
	BOOL bFirstArchiveSequence = (IsAlarm()||IsRing()) && pTape->IsPreAlarm();
	BOOL bNewDay = FALSE;

	if (!bEmpty)
	{
		bNewDay = pTape->GetLastTime().GetDay() != data.GetTimeStamp().GetDay();
	}

	if (    (   bFieldsChanged
			 ||	bTapeToLarge 
			 || m_bMoved
			 || bTypeDifferent
			 || bFirstArchiveSequence
			 || bNewDay
			 )
		 && (!bEmpty)
		 && (bIFrame)
		 && (!IsAlarmList())
		)
	{
		// create a new Tape
		pTape->Close(TRUE);
		wPrevSequenceNr = pTape->GetNr();
		dwNrOfRecords = pTape->GetNrOfRecords();

		// if it's an alarm archive send current
		// state to DV starter
		if (IsAlarm())
		{
			// bei Fuellstand > 60 % Alarm geben 
			if (   !m_b60Send
				&& !IsFull()
				&& Is60PercentFull()
				)
			{
				SendAlarm60();
			}
		}

		if (IsAlarm() && IsFull())
		{
			// Alarm Collection voll
			return NULL;
		}
		else
		{
			AdjustSize();
			AdjustFree();
			pTape = GetNewTape(bSuspect);
			if (pTape)
			{
				bNewSequenceCreated = TRUE;
				if (!pTape->Create())
				{
					RemoveTape(pTape->GetNr());
					WK_DELETE(pTape);
				}
			}
		}
	}
	else
	{
		// use the current Tape
		// to append the picture
	}

	return pTape;
}
/////////////////////////////////////////////////////////////////////
CTape* CCollection::GetNewTape(BOOL bSuspect, CTape* pSourceTape /*= NULL*/)
{
	CTape* pTape = NULL;
	CIPCDrive* pDrive = NULL;
	WORD wCollectionIdPart = m_idCollection.GetSubID() & 0x0fff;
	if (pSourceTape)
	{
		// for moving Tapes from safe ring to alarm ring
		pDrive = theApp.m_Drives.GetDrive(pSourceTape->GetDrive());
	}
	else
	{
		// normal new Tape
		pDrive = theApp.m_Drives.GetFreeDrive(wCollectionIdPart);
	}

	if (pDrive)
	{
		pTape = AddNewTape(bSuspect, pDrive);
		if (theApp.m_bEqualArchiveDistribution)
		{
			theApp.m_Drives.SetCollectionsToDrive(pDrive->GetRootString(), wCollectionIdPart, TRUE);
		}

		CWnd* pWnd = AfxGetMainWnd();
		if (WK_IS_WINDOW(pWnd))
		{
			pWnd->PostMessage(WM_COMMAND,ID_FILE_ACTUALIZE);
		}
	}
	else
	{
		WK_TRACE_ERROR(_T("no valid drive for storing\n"));
	}

	return pTape;
}
/////////////////////////////////////////////////////////////////////
BOOL CCollection::Store(const CImageData& data, BOOL& bNewSequenceCreated)
{
	BOOL bRet = TRUE;

	CAutoCritSec acs(&m_csCollection, _T(__FUNCTION__), DVS_TIMEOUT);

	// attention save thread
	if (IsAlarm() && IsFull())
	{
		// cannot save alarm picture if archive is full
		if (!m_bFullSend)
		{
			WK_TRACE(_T("sending alarm %s full alarm detector\n"),GetName());
			SendAlarmFull();
		}
		bRet = FALSE;
	}

	if (bRet)
	{
		BOOL bSuspect = FALSE;
		if (!IsPreAlarm())
		{
			// is there any Tape in our safe ring
			// so just move it
			// that's the cheapest way
			DWORD dwTick = GetTickCount();

			if (IsAlarm())
			{
				CIPCField* pSuspect = data.GetFields().GetCIPCField(_T(DVD_MD_S));
				CString s;
				s.Format(_T("%d"),NAVIGATION_SUSPECT);
				if (pSuspect && pSuspect->GetValue() == s)
				{
					bSuspect = TRUE;
				}
			}

			if (!bSuspect)
			{
				if (   (m_dwLastMoveTick == 0)
					|| (GetTimeSpan(m_dwLastMoveTick))>10*1000)
				{
					CAutoCritSec acsA(&theApp.m_Archives.m_cs);
					CCollection* pCollection = theApp.m_Archives.GetSafeRingCollection(m_idCollection.GetSubID());
					if (   pCollection 
						&& pCollection->IsPreAlarm())
					{
						if (pCollection && pCollection->GetNrTapes()>0)
						{
							// do the move
							MoveTapes(pCollection);
							BOOL bCancel = FALSE;
							Sort(bCancel);
						}
					}
					acsA.Unlock();
					m_dwLastMoveTick = dwTick;
				}
			}
		} // !IsSafeRing

		WORD wPrevSequenceNr = 0;
		DWORD dwNrOfRecords = 0;
		CTape* pTape = CheckNewTape(bSuspect,bNewSequenceCreated,data,wPrevSequenceNr,dwNrOfRecords);

		if (pTape==NULL)
		{
			// may be no new Tape because of alarm Collection
			if (IsAlarm() && IsFull())
			{
				if (!m_bFullSend)
				{
					WK_TRACE(_T("sending alarm %s full alarm detector\n"),GetName());
					SendAlarmFull();
				}
				WK_TRACE_ERROR(_T("cannot create a new Tape for %s\n"),GetName());
			}
		}
		else
		{
			if (pTape->Store(data,wPrevSequenceNr,dwNrOfRecords))
			{
				// ok
				bRet = TRUE;
				m_bDoFieldCheck = FALSE;
				m_bMoved = FALSE;
			}
			else
			{
				// close the current one and try again
				pTape->Close();
				if (pTape->IsEmpty())
				{
					// delete empty sequences
					DeleteTape(pTape->GetNr());
					pTape = NULL;
				}

				// this works only for pre-alarm archives!
				AdjustSize();
				AdjustFree();
				
				pTape = GetNewTape(bSuspect);
				bNewSequenceCreated = TRUE;
				if (pTape)
				{
					if (pTape->Create())
					{
						// second try
						if (pTape->Store(data,wPrevSequenceNr,dwNrOfRecords))
						{
							WK_TRACE(_T("wrote picture in second try\n"));
							m_bDoFieldCheck = FALSE;
							m_bMoved = FALSE;
							bRet = TRUE;
						}
						else
						{
							WK_TRACE(_T("cannot write picture after second try\n"));
							if (pTape->IsEmpty())
							{
								// delete empty sequences
								DeleteTape(pTape->GetNr());
								pTape = NULL;
							}
						}
					}
					else
					{
						RemoveTape(pTape->GetNr());
						WK_DELETE(pTape);
						bRet = FALSE;
					}
				}
			}
		}
	}

	acs.Unlock();

	return bRet;
}
//////////////////////////////////////////////////////////////////////
void CCollection::SendAlarmFull()
{
	CString sError;
	sError.Format(IDS_ALARM_FULL,GetName());
	{
		CWK_RunTimeError e(WAI_DATABASE_SERVER, 
						   REL_MESSAGE, 
						   RTE_ARCHIVE_FULL, 
						   sError,0,1);
		e.Send();
	}
	{
		CWK_RunTimeError e(WAI_DATABASE_SERVER, 
						   REL_MESSAGE, 
						   RTE_ARCHIVE_FULL, 
						   sError,0,0);
		e.Send();
	}
	m_bFullSend = TRUE;
}
//////////////////////////////////////////////////////////////////////
BOOL CCollection::Is60PercentFull()
{
	CIPCInt64 s(GetSizeMB());
	CIPCInt64 i = GetSizeBytes();
	s *= CIPCInt64(1024*1024);
	CIPCInt64 iPercentage((i.GetInt64() * 100) / s.GetInt64());
	return (iPercentage.GetInt64()>=60);
}
//////////////////////////////////////////////////////////////////////
void CCollection::SendAlarm60()
{
	if (IsAlarm())
	{
		CString sError;
		sError.Format(IDS_ALARM_60,GetName(),60);
		{
			CWK_RunTimeError e(WAI_DATABASE_SERVER, 
							   REL_MESSAGE, 
							   RTE_ARCHIVE_60PERCENT, 
							   sError,0,1);
			e.Send();
		}
		{
			CWK_RunTimeError e(WAI_DATABASE_SERVER, 
							   REL_MESSAGE, 
							   RTE_ARCHIVE_60PERCENT, 
							   sError,0,0);
			e.Send();
		}
		m_b60Send = TRUE;
	}
}
//////////////////////////////////////////////////////////////////////
void CCollection::MoveTapes(CCollection* pCollection)
{
	CTape* pSourceTape;
	CTape* pDestinationTape;

	CString s;

	CAutoCritSec acs(pCollection->GetCS());

	s.Format(_T("MOVE %s --> %s"),pCollection->GetName(),GetName());
	WK_TRACE(_T("%s by alarm\n"),s);

	CWK_StopWatch w;
	while (pCollection->GetSize() && !IsFull())
	{
		pSourceTape = pCollection->GetAtFast(0);
		if (pSourceTape->GetNrOfRecords()>0)
		{
			// wait a while to close source sequence
			int c = 10;
			BOOL bClosed = FALSE;
			while ( (c--) && (!bClosed))
			{
				bClosed = pSourceTape->Close(TRUE);
				Sleep(20);
			}
			if (bClosed)
			{
				pDestinationTape = GetNewTape(GetType(),pSourceTape);
				if (pDestinationTape)
				{
					if (!pDestinationTape->Move(*pSourceTape))
					{
						WK_TRACE_ERROR(_T("cannot move sequence %s to %s\n"),
							pSourceTape->Format(),pDestinationTape->Format());

						DeleteTape(pDestinationTape->GetNr());
						pDestinationTape = NULL;
					}
				}
				else
				{
					WK_TRACE_ERROR(_T("cannot get new sequence to move %s\n"),
						GetName());
				}
			}
			else
			{
				WK_TRACE_ERROR(_T("cannot close sequence to move %s\n"),
					pSourceTape->Format());
			}
			WK_DELETE(pSourceTape);
			pCollection->RemoveAt(0);
		}
		else
		{
			// don't move empty sequence
		}
	}
	acs.Unlock();
	w.StopWatch(s);
	m_bMoved = TRUE;
}
//////////////////////////////////////////////////////////////////////
BOOL CCollection::IsFull()
{
	CIPCInt64 iSize = GetSizeBytes();
	if (iSize >= m_iSizeBytes)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
//////////////////////////////////////////////////////////////////////
void CCollection::AdjustSize()
{
	// always sort
	BOOL bCancel = FALSE;
	Sort(bCancel);

	if (IsPreAlarm())
	{
		while (IsFull())
		{
			DeleteOldestTape();
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CCollection::AdjustFree()
{
	if (IsPreAlarm())
	{
		//
		CIPCInt64 iSize = GetSizeBytes();
		if (iSize.GetInt64() + (__int64)m_iTapeSizeBytes > m_iSizeBytes.GetInt64())
		{
			// with the next Tape the Collection
			// would be larger than Collection size!
			DeleteOldestTape();
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
BOOL CCollection::Delete()
{
	WK_TRACE(_T("deleting Collection %s\n"),GetName());
	if (DeleteAll())
	{
		// now delete all directories
		if (IsBackup())
		{
			WK_TRACE(_T("rmdir %s by archive delete\n"),m_sBackupDir);
			return DeleteDirRecursiv(m_sBackupDir);
		}
		else
		{
			BOOL bRet = TRUE;
			for (int i=0;i<theApp.m_Drives.GetSize();i++)
			{
				CIPCDrive* pDrive = theApp.m_Drives.GetAtFast(i);
				if (pDrive->IsDatabase())
				{
					CString sDir;
					sDir.Format(_T("%sdvs\\arch%04lx"),pDrive->GetRootString(),GetNr());
					WK_TRACE(_T("rmdir %s by delete archive\n"),sDir);
					bRet &= DeleteDirRecursiv(sDir);
				}
			}
			return bRet;
		}
	}
	else
	{
		return FALSE;
	}
}
//////////////////////////////////////////////////////////////////////////////
void CCollection::CloseAll()
{
	CAutoCritSec acs(&m_csCollection, _T(__FUNCTION__), DVS_TIMEOUT);
	for (int i=0;i<GetSize();i++)
	{
		GetAtFast(i)->Close();
	}
	acs.Unlock();
}
//////////////////////////////////////////////////////////////////////////////
BOOL CCollection::DeleteAll()
{
	while (DeleteOldestTape())
	{
	}
	return GetSize() == 0;
}
//////////////////////////////////////////////////////////////////////////////
BOOL CCollection::DeleteAllOlder(const CSystemTime& st)
{
	TRACE(_T("Archive %04x deleting all older than %s\n"),
		GetNr(),st.GetDate());
	BOOL bRet = TRUE;

	CAutoCritSec acs(&m_csCollection, _T(__FUNCTION__), DVS_TIMEOUT);
	while (GetSize()>0)
	{
		CTape* pTape = GetAtFast(0);
		TRACE(_T("Archive Nr %04x Check Sequence Nr %04x %s\n"),
			   GetNr(),
			   pTape->GetNr(),
			   pTape->GetFirstTime().GetDate());
		if (pTape->GetFirstTime().GetDBDate()<st.GetDBDate())
		{
			bRet = DeleteTape(pTape->GetNr());
			TRACE(_T("Deleted Sequence Nr %04x %s\n"),pTape->GetNr(),
				pTape->GetFirstTime().GetDate());
		}
		else
		{
			break;
		}
	}

	acs.Unlock();
	return bRet;
}
//////////////////////////////////////////////////////////////////////////////
CTape* CCollection::GetTapeIndex(int i)
{
	CTape* pTape = NULL;
	CAutoCritSec acs(&m_csCollection, _T(__FUNCTION__), DVS_TIMEOUT);
	if (i<GetSize() && i>=0)
	{
		pTape = GetAtFast(i);
	}
	acs.Unlock();
	return pTape;
}
//////////////////////////////////////////////////////////////////////
CTape* CCollection::GetNextSequence(WORD wSequenceNr)
{
	CTape* pTape = NULL;
	CAutoCritSec acs(&m_csCollection, _T(__FUNCTION__), DVS_TIMEOUT);

	if (   wSequenceNr == 0
		&& GetSize()>0)
	{
		pTape = GetAtFast(0);
	}
	else
	{
		for (int i=0;i<GetSize();i++)
		{
			if (GetAtFast(i)->GetNr() == wSequenceNr)
			{
				if (i+1<GetSize())
				{
					pTape = GetAtFast(i+1);
				}
				break;
			}
		}
	}

	acs.Unlock();
	return pTape;
}
//////////////////////////////////////////////////////////////////////
CTape* CCollection::GetTapeID(WORD wID)
{
	int i;
	CTape* pTape = NULL;

	CAutoCritSec acs(&m_csCollection, _T(__FUNCTION__), DVS_TIMEOUT);

	for (i=0;i<GetSize();i++)
	{
		if (GetAtFast(i)->GetNr()==wID)
		{
			pTape = GetAtFast(i);
			break;
		}
	}
	acs.Unlock();

	return pTape;
}
//////////////////////////////////////////////////////////////////////
CTape* CCollection::GetFirstSequence(const CSystemTime& s)
{
	int i;
	CSystemTime t = s;
	CTape* pTape = NULL;

	CAutoCritSec acs(&m_csCollection, _T(__FUNCTION__), DVS_TIMEOUT);

	for (i=0;i<GetSize();i++)
	{
		CTape* pSequence = GetAtFast(i);
/*
		CSystemTime s,e;
		s = pSequence->GetFirstTime();
		e = pSequence->GetLastTime();

		TRACE(_T("%04hx %04hx GetFirstSequence %s,%d<%s,%d<%s,%d ?\n"),GetNr(),
											   pSequence->GetNr(),
											   s.GetDateTime(),
											   s.GetMilliseconds(),
											   t.GetDateTime(),
											   t.GetMilliseconds(),
											   e.GetDateTime(),
											   e.GetMilliseconds()
											   );
*/
		if (   pSequence->GetFirstTime() <= t
			&& t <= pSequence->GetLastTime())
		{
			pTape = pSequence;
			break;
		}
	}
	acs.Unlock();

	return pTape;
}
//////////////////////////////////////////////////////////////////////
CTape* CCollection::GetFirstTape(CIPCDrive* pDrive)
{
	int i;
	CTape* pTape = NULL;

	CAutoCritSec acs(&m_csCollection, _T(__FUNCTION__), DVS_TIMEOUT);

	for (i=0;i<GetSize();i++)
	{
		if (0 == GetAtFast(i)->GetDrive().CompareNoCase(pDrive->GetRootString()))
		{
			pTape = GetAtFast(i);
			break;
		}
	}
	acs.Unlock();

	return pTape;
}
//////////////////////////////////////////////////////////////////////
CIPCInt64 CCollection::GetSizeBytes()
{
	CIPCInt64 r = 0;

	CTape* pTape = NULL;

	CAutoCritSec acs(&m_csCollection, _T(__FUNCTION__), DVS_TIMEOUT);

	for (int i=0;i<GetSize();i++)
	{
		pTape = GetAtFast(i);
		if (   IsBackup() 
			|| !pTape->IsBackup())
		{
			r += pTape->GetSizeBytes();
		}
	}

	acs.Unlock();

	r += CIPCInt64(m_iFolderCluster); 

	return r;
}
//////////////////////////////////////////////////////////////////////
CIPCInt64 CCollection::GetSizeBytes(const CSystemTime& stStart,
							        const CSystemTime& stEnd,
									CIPCInt64& i64Sum,
									DWORD dwMaximumInMB,
									BOOL& bCancelled)
{
	CIPCInt64 ret = 0;
	
	if (!IsBackup())
	{
		CIPCInt64 one = 0;
		CTape* pTape = NULL;
		
		CAutoCritSec acs(&m_csCollection, _T(__FUNCTION__), DVS_TIMEOUT);
		
		for (int i=0;i<GetSize();i++)
		{
			pTape = GetAtFast(i);
			if (!pTape->IsBackup())
			{
				one = pTape->GetSizeBytes(stStart,stEnd);
				ret += one;
				i64Sum += one;
				if (   dwMaximumInMB > 0
					&& i64Sum.GetInMB() > dwMaximumInMB)
				{
					break;
				}
			}
			if (bCancelled)
			{
				break;
			}
		}
		
		acs.Unlock();
	}

	return ret;
}
//////////////////////////////////////////////////////////////////////
DWORD CCollection::GetNumberOfPictures()
{
	DWORD n = 0;
	int i;

	CTape* pTape = NULL;

	CAutoCritSec acs(&m_csCollection, _T(__FUNCTION__), DVS_TIMEOUT);

	for (i=0;i<GetSize();i++)
	{
		pTape = GetAtFast(i);
		if (   IsBackup() 
			|| !pTape->IsBackup())
		{
			n += pTape->GetNrOfRecords();
		}
	}

	acs.Unlock();

	return n;
}
//////////////////////////////////////////////////////////////////////
void CCollection::AddScannedTape(CTape* pTape)
{
	CAutoCritSec acs(&m_csCollection, _T(__FUNCTION__), DVS_TIMEOUT);
	Add(pTape);
	acs.Unlock();
}
//////////////////////////////////////////////////////////////////////
WORD CCollection::GetNewID()
{
	CTape* pTape=NULL;

	CAutoCritSec acs(&m_csCollection, _T(__FUNCTION__), DVS_TIMEOUT);

	do
	{
		m_wTapeID++;
		if (m_wTapeID == 0)
		{
			m_wTapeID = 1;
		}
		pTape = GetTapeID(m_wTapeID);
	}
	while (pTape!=NULL);

	acs.Unlock();

	return m_wTapeID;
}
//////////////////////////////////////////////////////////////////////
CTape* CCollection::AddNewTape(BOOL bSuspect, CIPCDrive* pDrive)
{
	CTape* pTape=NULL;
	WORD wID = 0;

	wID = GetNewID();
	pTape = new CTape((BYTE)(bSuspect ? CAR_IS_SUSPECT : 0),this,pDrive,wID);
	
	CAutoCritSec acs(&m_csCollection, _T(__FUNCTION__), DVS_TIMEOUT);
	Add(pTape);
	acs.Unlock();

	return pTape;
}
//////////////////////////////////////////////////////////////////////
CTape* CCollection::GetOldestTape()
{
	CAutoCritSec acs(&m_csCollection, _T(__FUNCTION__), DVS_TIMEOUT);

	CTape* pTape = NULL;

	for (int i=0; i<GetSize();i++)
	{
		pTape = GetAtFast(i);
		if (!pTape->IsBackup())
		{
			break;
		}
		// reset to nothing found
		pTape = NULL;
	}

	acs.Unlock();

	return pTape;
}
//////////////////////////////////////////////////////////////////////
BOOL CCollection::DeleteOldestTape()
{
	BOOL bRet = FALSE;
	CAutoCritSec acs(&m_csCollection, _T(__FUNCTION__), DVS_TIMEOUT);

	CTape* pTape = NULL;

	for (int i=0; i<GetSize();i++)
	{
		pTape = GetAtFast(i);
		if (!pTape->IsBackup())
		{
			RemoveAt(i);
			if (!IsPreAlarm())
			{
				theApp.m_Clients.DoIndicateDeleteTape(*pTape);
			}
			break;
		}
		// reset to nothing found
		pTape = NULL;
	}

	acs.Unlock();
	
	if (pTape)
	{
		bRet = pTape->Delete();
		if (bRet)
		{
		}
		WK_DELETE(pTape);
	}

	if (IsRing())
	{
		CAutoCritSec acsA(&theApp.m_Archives.m_cs);
		CCollection* pAlarmList = theApp.m_Archives.GetSafeRingCollection(GetNr());
		if (   pAlarmList
			&& pAlarmList->IsAlarmList())
		{
			if (GetSize())
			{
				CSystemTime st;
				st = GetAtFast(0)->GetFirstTime();
				pAlarmList->DeleteAllOlder(st);
			}
			else
			{
				pAlarmList->DeleteAll();
			}
		}
		acsA.Unlock();
	}

	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CCollection::DeleteTape(WORD wID)
{
	BOOL bWas60PercentFull = FALSE;
	
	if (IsAlarm())
	{
		bWas60PercentFull = Is60PercentFull();
	}

	int i;
	CTape* pTape = NULL;
	BOOL bRet = FALSE;

	CAutoCritSec acs(&m_csCollection, _T(__FUNCTION__), DVS_TIMEOUT);

	for (i=0;i<GetSize();i++)
	{
		if (GetAtFast(i)->GetNr()==wID)
		{
			//TODO TKR UF if (!pTape->IsBackup())
			pTape = GetAtFast(i);
			RemoveAt(i);
			if (!IsPreAlarm())
			{
				theApp.m_Clients.DoIndicateDeleteTape(*pTape);
			}
			break;
		}
	}
	acs.Unlock();

	if (pTape)
	{
		if (   pTape->IsBackup()	// don't delete a read only tape
			|| pTape->Delete())
		{
			bRet = TRUE;
		}
		WK_DELETE(pTape);
	}
	
	if (bRet)
	{
		if (IsAlarm())
		{
			BOOL bIs60PercentFull = Is60PercentFull();

			m_bFullSend = FALSE;

			if (bWas60PercentFull && !bIs60PercentFull)
			{
				m_b60Send = FALSE;
			}
		}
	}

	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CCollection::RemoveTape(WORD wID)
{
	int i;
	BOOL bRet = FALSE;

	CAutoCritSec acs(&m_csCollection, _T(__FUNCTION__), DVS_TIMEOUT);

	for (i=0;i<GetSize();i++)
	{
		if (GetAtFast(i)->GetNr()==wID)
		{
			//TODO TKR UF if (!pTape->IsBackup())
			RemoveAt(i);
			bRet = TRUE;
			break;
		}
	}
	acs.Unlock();

	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CCollection::Verify(BOOL& bCancel)
{
	BOOL bRet = TRUE;
	if (!IsBackup())
	{
		for (int i=0;i<GetSize() && !bCancel;i++)
		{
			bRet &= GetAtFast(i)->Verify();
		}
	}

	if (!bRet)
	{
		Sort(bCancel);
	}

	return bRet;
}
//////////////////////////////////////////////////////////////////////
