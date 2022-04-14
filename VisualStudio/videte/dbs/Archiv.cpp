/* GlobalReplace: CDVRDrive --> CIPCDrive */
/* GlobalReplace: CDVRInt64 --> CIPCInt64 */
/* GlobalReplace: line %s --> line %d */
// Archiv.cpp: implementation of the CArchiv class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DBS.h"
#include "Archiv.h"

#include "Win32FindData.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static TCHAR szCameraSection[] = _T("Camera");
static TCHAR BASED_CODE szDontUse[]	= _T("dontuse");

CStringA CArchiv::m_sTYP = DBP_TYP;
CStringA CArchiv::m_sDATE = DBP_DATE;
CStringA CArchiv::m_sTIME = DBP_TIME;
CStringA CArchiv::m_sMS = DBP_MS;

CStringA CArchiv::m_sCANR = DBP_CAMERANR;
CStringA CArchiv::m_sRES = DBP_RESOLUTION;
CStringA CArchiv::m_sCOMP = DBP_COMPRESSION;
CStringA CArchiv::m_sCOTY = DBP_COMPRESSIONTYP;

CStringA CArchiv::m_sDPOS = DBP_DATA_POS;
CStringA CArchiv::m_sDLEN = DBP_DATA_LEN;
CStringA CArchiv::m_sCSUM = DBP_CHECK_SUM;

CStringA CArchiv::m_sMD_X = DBP_MD_X;
CStringA CArchiv::m_sMD_Y = DBP_MD_Y;
CStringA CArchiv::m_sMD_X2 = DBP_MD_X2;
CStringA CArchiv::m_sMD_Y2 = DBP_MD_Y2;
CStringA CArchiv::m_sMD_X3 = DBP_MD_X3;
CStringA CArchiv::m_sMD_Y3 = DBP_MD_Y3;
CStringA CArchiv::m_sMD_X4 = DBP_MD_X4;
CStringA CArchiv::m_sMD_Y4 = DBP_MD_Y4;
CStringA CArchiv::m_sMD_X5 = DBP_MD_X5;
CStringA CArchiv::m_sMD_Y5 = DBP_MD_Y5;
CStringA CArchiv::m_sMD_S = DBP_MD_S;

CString CArchiv::m_sRootName = _T("dbs");
CString CArchiv::m_sBackupName = _T("dbb");
CString CArchiv::m_sBackupInfoFilename = _T("oem.ini");

CString CArchiv::m_sDbd = _T("dbd");



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
void CArchiv::Init()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CAutoCritSec acs(&m_Sequences.m_cs);

	m_pPreAlarm = NULL;
	m_bSearch = FALSE;
	m_dwNrOfSequencesDebit = 15;

	m_cb.safety = FALSE;
	m_cb.errOff = FALSE;
	m_cb.accessMode = OPEN4DENY_WRITE;
	m_cb.optimize = OPT4ALL;
	m_cb.optimizeWrite = OPT4ALL;

	m_iDirectoryCluster = 0;
	m_wOriginalNr = 0;
	m_bAlarmFullSend = FALSE;
	m_bAlarm60Send = FALSE;
	m_iSizeBytes = 0;
	m_iSequenceSizeBytes = 0;
	m_dwMaxStorageTime = 0;
	m_bDynamicSize = FALSE;
	m_bDoFieldCheck = TRUE;
	m_bMoved = FALSE;
	m_bMoving = FALSE;

	acs.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CArchiv::InitPreAlarm(CArchivs& archives)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	m_pPreAlarm = archives.GetSafeRingArchiv(GetNr());
	if (m_pPreAlarm)
	{
		WK_TRACE(_T("%s has pre alarm %s\n"),GetName(),m_pPreAlarm->GetName());
	}
}
//////////////////////////////////////////////////////////////////////
CArchiv::CArchiv(const CArchivInfo& ai)
	: m_FieldInfo(m_cb)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	Init();

	// copy data from config
	m_sName = ai.GetName();
	m_idArchiv   = ai.GetID();
	m_wOriginalNr = m_idArchiv.GetSubID();
	m_wSafeRingFor = ai.GetSafeRingFor();
	m_Typ = ai.GetType();
	m_dwSizeMB = ai.GetSizeMB();
	m_sFixedDrive = ai.GetFixedDrive();
	m_sDirectory = CArchiv::m_sRootName;
	m_dwMaxStorageTime = ai.GetMaxStorageTime();
	m_bDynamicSize = ai.GetDynamicSize();


	InitSequenceDebit();

	m_iSizeBytes = (__int64)(m_dwSizeMB) * 1024 * 1024;

	m_iSequenceSizeBytes = m_iSizeBytes.GetInt64() / ((__int64)m_dwNrOfSequencesDebit);
	WK_TRACE(_T("Archiv %s recommended sequence size is %s Bytes = %d MB\n"),
		GetName(),m_iSequenceSizeBytes.Format(TRUE),m_iSequenceSizeBytes.GetInMB());

	m_sFormat = InitialFormat();

	if (theApp.IsDTS())
	{
		InitFieldsDTS();
	}
	else
	{
		InitFieldsIdip();
	}

	if (theApp.TraceCodebase())
	{
		CString sErrorFile;
		sErrorFile.Format(_T("%s\\%s.log"), GetLogPath(), GetName());
		m_cb.errorFile(CWK_String(sErrorFile));
	}

	CHost* pHost = theApp.GetHosts().GetHostForArchive(m_idArchiv);
	if (pHost)
	{
		// I am an archive for a host
		m_idHost = pHost->GetID();
		if (pHost->GetAlarmArchive() == m_idArchiv)
		{
		}
		else if (pHost->GetNormalArchive() == m_idArchiv)
		{
		}
		else if (pHost->GetSearchArchive() == m_idArchiv)
		{
			m_bSearch = TRUE;
		}
	}

	if (GetNr()==255)
	{
		m_bSearch = TRUE;
	}

	m_sCameraMapPathname.Format(_T("%s\\%04lx.dbf"),theApp.m_sMapDirectory,GetNr());
}
//////////////////////////////////////////////////////////////////////
void CArchiv::SetName(const CString sName)
{
	m_sName = sName;
}
//////////////////////////////////////////////////////////////////////
void CArchiv::SetSize(DWORD dwSize)
{
	m_dwSizeMB = dwSize;
	InitSequenceDebit();
	m_iSizeBytes = (__int64)(m_dwSizeMB) * 1024 * 1024;
	if (IsRing())
	{
		//	m_iTapeSizeBytes = DVS_SEQUENCE_SIZE;
		m_iSequenceSizeBytes = 16 * 1024 * 1024;
	}
	else
	{
		// Alarm/Pre Alarm
		m_iSequenceSizeBytes = (DWORD)(m_iSizeBytes.GetInt64() / ((__int64)m_dwNrOfSequencesDebit));
	}
	AdjustSize();
	m_sFormat = InitialFormat();
}
//////////////////////////////////////////////////////////////////////
void CArchiv::InitSequenceDebit()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	m_dwNrOfSequencesDebit = 32;
	// groesser 64MB
	if (m_dwSizeMB>64)
	{
		m_dwNrOfSequencesDebit = 64;
	}
	// groesser 256 MB
	if (m_dwSizeMB>256)
	{
		m_dwNrOfSequencesDebit = 96;
	}
	// groesser 1 GB
	if (m_dwSizeMB>1*1024)
	{
		m_dwNrOfSequencesDebit = 128;
	}
	// groesser 64 GB
	if (m_dwSizeMB>64*1024)
	{
		m_dwNrOfSequencesDebit = 256;
	}
	// groesser 256 GB
	if (m_dwSizeMB>512*1024)
	{
		m_dwNrOfSequencesDebit = 512;
	}
	// groesser 1 TB
	if (m_dwSizeMB>1*1024*1024)
	{
		m_dwNrOfSequencesDebit = 1024;
	}
	// groesser 4 TB
	if (m_dwSizeMB>4*1024*1024)
	{
		m_dwNrOfSequencesDebit = 2048;
	}
}
//////////////////////////////////////////////////////////////////////
void CArchiv::InitFieldsIdip()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CAutoCritSec acs(&m_Sequences.m_cs);

	if (IsAlarmList())
	{
		m_FieldInfo.add(m_sTYP,'C',1,0);
		m_FieldInfo.add(m_sDATE,'C',8,0);
		m_FieldInfo.add(m_sTIME,'C',6,0);
		m_FieldInfo.add(m_sMS,'C',3,0);
		m_FieldInfo.add(m_sCANR,'C',8,0);
		m_FieldInfo.add(DBP_ARCHIVENR,'C',8,0);
		m_FieldInfo.add(DBP_INPUTNR,'C',8,0);
	}
	else
	{
		// picture record data
		m_FieldInfo.add(m_sCANR,'C',8,0);
		m_FieldInfo.add(m_sRES,'C',1,0);
		m_FieldInfo.add(m_sCOMP,'C',2,0);
		m_FieldInfo.add(m_sCOTY,'C',2,0);
		m_FieldInfo.add(m_sTYP,'C',1,0);

		m_FieldInfo.add(DBP_FFV_REF,		 'C', 8, 0);
		m_FieldInfo.add(DBP_RWV_REF,		 'C', 8, 0);
		m_FieldInfo.add(DBP_FFA_REF,		 'C', 8, 0);
		m_FieldInfo.add(DBP_RWA_REF,		 'C', 8, 0);

		m_FieldInfo.add(m_sDATE,'C',8,0);
		m_FieldInfo.add(m_sTIME,'C',6,0);
		m_FieldInfo.add(m_sMS,'C',3,0);

		m_FieldInfo.add(m_sDPOS,'C',8,0);
		m_FieldInfo.add(m_sDLEN,'C',8,0);
		m_FieldInfo.add(m_sCSUM,'C',2,0);

		if (!theApp.IsReadOnly())
		{
			CWK_Dongle dongle;
			if (   dongle.IsReceiver()
				|| dongle.AllowMotionDetection()
				)
			{
				m_FieldInfo.add(m_sMD_X,'C',4,0);
				m_FieldInfo.add(m_sMD_Y,'C',4,0);
				m_FieldInfo.add(m_sMD_X2,'C',4,0);
				m_FieldInfo.add(m_sMD_Y2,'C',4,0);
				m_FieldInfo.add(m_sMD_X3,'C',4,0);
				m_FieldInfo.add(m_sMD_Y3,'C',4,0);
				m_FieldInfo.add(m_sMD_X4,'C',4,0);
				m_FieldInfo.add(m_sMD_Y4,'C',4,0);
				m_FieldInfo.add(m_sMD_X5,'C',4,0);
				m_FieldInfo.add(m_sMD_Y5,'C',4,0);

				m_FieldInfo.add(m_sMD_S, 'C', 1, 0);
			}
		}
	}
	for (int i=0; i<theApp.m_Fields.GetSize();i++)
	{
		CIPCField* pField = theApp.m_Fields.GetAtFast(i);
		if (pField->GetValue() != szDontUse)
		{
			if (   !IsAlarmList()
				|| (   pField->GetName() != CIPCField::m_sfStNm
				    && pField->GetName() != CIPCField::m_sfCaNm
				    && pField->GetName() != CIPCField::m_sfInNm)
				)
			{
				CStringA sName = pField->GetName();
				m_FieldInfo.add(sName,r4str,pField->GetMaxLength(),0);
			}
		}
	}

	acs.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CArchiv::InitFieldsDTS()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CAutoCritSec acs(&m_Sequences.m_cs);

	m_FieldInfo.add(DVR_VERSION, 'C', 2, 0);
	if (IsAlarmList())
	{
		m_FieldInfo.add(m_sTYP , 'C', 1, 0);
		m_FieldInfo.add(m_sDATE, 'C', 8, 0);
		m_FieldInfo.add(m_sTIME, 'C', 6, 0);
	}
	else
	{
		// picture record data
		m_FieldInfo.add(m_sCANR, 'C', 8, 0);
		m_FieldInfo.add(m_sRES,  'C', 1, 0);
		m_FieldInfo.add(m_sCOMP, 'C', 2, 0);
		m_FieldInfo.add(m_sCOTY, 'C', 2, 0);
		m_FieldInfo.add(m_sTYP,  'C', 1, 0);

		m_FieldInfo.add(DBP_FFV_REF,		 'C', 8, 0);
		m_FieldInfo.add(DBP_RWV_REF,		 'C', 8, 0);
		m_FieldInfo.add(DBP_FFA_REF,		 'C', 8, 0);
		m_FieldInfo.add(DBP_RWA_REF,		 'C', 8, 0);

		m_FieldInfo.add(m_sDATE,'C', 8, 0);
		m_FieldInfo.add(m_sTIME,'C', 6, 0);
		m_FieldInfo.add(m_sMS,  'C', 3, 0);

		m_FieldInfo.add(m_sDPOS,'C', 8, 0);
		m_FieldInfo.add(m_sDLEN,'C', 8, 0);
		m_FieldInfo.add(m_sCSUM,'C', 2, 0);

		m_FieldInfo.add(m_sMD_X, 'C', 4, 0);
		m_FieldInfo.add(m_sMD_Y, 'C', 4, 0);
		m_FieldInfo.add(m_sMD_X2,'C', 4, 0);
		m_FieldInfo.add(m_sMD_Y2,'C', 4, 0);
		m_FieldInfo.add(m_sMD_X3,'C', 4, 0);
		m_FieldInfo.add(m_sMD_Y3,'C', 4, 0);
		m_FieldInfo.add(m_sMD_X4,'C', 4, 0);
		m_FieldInfo.add(m_sMD_Y4,'C', 4, 0);
		m_FieldInfo.add(m_sMD_X5,'C', 4, 0);
		m_FieldInfo.add(m_sMD_Y5,'C', 4, 0);

		m_FieldInfo.add(m_sMD_S, 'C', 1, 0);
	}

	acs.Unlock();
}
//////////////////////////////////////////////////////////////////////
CArchiv::CArchiv(const CString& sBackupDir, WORD wArchivNr, WORD wOriginalNr)
	: m_FieldInfo(m_cb)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	Init();
	m_idArchiv.Set(SECID_GROUP_ARCHIVE,wArchivNr);
	m_wOriginalNr = wOriginalNr;
	m_sBackupDir = sBackupDir;
	int p;
	p = sBackupDir.Find('\\');
	m_sFixedDrive = sBackupDir.Left(p);
	m_sDirectory = sBackupDir.Mid(p+1);
	if (m_sDirectory.GetAt(m_sDirectory.GetLength()-1)=='\\')
	{
		m_sDirectory = m_sDirectory.Left(m_sDirectory.GetLength()-1);
	}
	m_sFormat = InitialFormat();
	m_Typ = BACKUP_ARCHIV;
#ifdef _DEBUG
	m_cb.readOnly = FALSE;
#else
	m_cb.readOnly = TRUE;
#endif
	m_sCameraMapPathname.Format(_T("%s%04hx.dbf"),m_sBackupDir,m_wOriginalNr);
}
//////////////////////////////////////////////////////////////////////
CArchiv::~CArchiv()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CAutoCritSec acs(&m_Sequences.m_cs);
	m_Sequences.SafeDeleteAll();
	CAutoCritSec acsCB(&m_csCB);
	m_cb.initUndo();
	acsCB.Unlock();
	acs.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CArchiv::Trace()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	DWORD d = m_Sequences.GetSizeMB();
	CString s = Format();
	WK_TRACE(_T("%s\n"),s);
	WK_STAT_LOG(_T("ArchivSize"),d,m_sName);
	m_Sequences.Trace();
}
//////////////////////////////////////////////////////////////////////
CString CArchiv::InitialFormat()
{
	CCallStackEntry cse(_T(__FUNCTION__));
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
	case SUCHERGEBNIS_ARCHIV:
		sTyp.LoadString(IDS_SEARCH);
		break;
	case BACKUP_ARCHIV:
		sTyp.LoadString(IDS_BACKUP);
		break;
	case ALARM_LIST_ARCHIV:
		sTyp.LoadString(IDS_ALARM_LIST);
		break;
	}

	if (theApp.IsDTS())
	{
		s.Format(_T("Nr %04hx %s -> %s Typ %s, %d MB"),
			GetNr(),m_sName,m_sFixedDrive,sTyp,GetSizeMB());
	}
	else
	{
		s.Format(_T("Nr %d %s -> %s Typ %s, %s %d MB"),
			GetNr(),m_sName,m_sFixedDrive,sTyp,IsDynamicSize()?_T("dyn"):_T("fix"),GetSizeMB());
	}

	return s;
}
//////////////////////////////////////////////////////////////////////
CString CArchiv::Format()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CString s;
	CIPCInt64 d = m_Sequences.GetSizeBytes();
	DWORD dwInMB = d.GetInMB();

	CAutoCritSec acs(&m_Sequences.m_cs);

	if (dwInMB>0)
	{
		s.Format(_T("( %d MB) %d/%d Sequencen %d Records %s Bytes %d h"),
			dwInMB,m_Sequences.GetSize(),m_dwNrOfSequencesDebit,
			m_Sequences.GetNumberOfPictures(),d.Format(TRUE),m_dwMaxStorageTime);
	}
	else
	{
		dwInMB = (DWORD)d.GetInt64()/1024;
		s.Format(_T("( %d kB) %d/%d Sequencen %d Records %s Bytes %d h"),
			dwInMB,m_Sequences.GetSize(),m_dwNrOfSequencesDebit,
			m_Sequences.GetNumberOfPictures(),d.Format(TRUE),m_dwMaxStorageTime);
	}

	acs.Unlock();
	s = m_sFormat + s;

	return s;
}
//////////////////////////////////////////////////////////////////////
void CArchiv::NewDirectory(const CString& sDir)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CIPCDrive* pDrive = theApp.m_Drives.GetDrive((char)sDir.GetAt(0));
	if (pDrive)
	{
		m_iDirectoryCluster += 2*pDrive->GetClusterSize();
		// the dir
		// and the index.dbi file
	}
}
//////////////////////////////////////////////////////////////////////
void CArchiv::DeleteDirectory(const CString& sDir)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CIPCDrive* pDrive = theApp.m_Drives.GetDrive((char)sDir.GetAt(0));
	if (pDrive)
	{
		m_iDirectoryCluster -= pDrive->GetClusterSize();
	}
}
//////////////////////////////////////////////////////////////////////
CString	CArchiv::GetDirectory() const
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CString ret;

	ret.Format(_T("%s\\arch%04lx"),m_sDirectory,m_wOriginalNr);

	return ret;
}
//////////////////////////////////////////////////////////////////////
void CArchiv::Scan(const CString& sDir, CIPCDrive* pDrive, const CVDBSequenceMap& map)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	WK_TRACE(_T("scanning sequences in %s\n"),sDir);
	CString sSearch;

	sSearch = sDir + _T("\\*.*");

	CString sTmp;
	CString sDbi;
	DWORD dwNrOfFiles = 0;
	CWin32FindDatas dbfFiles;
	CWin32FindDatas dbdxFiles;
	WIN32_FIND_DATA	data;
	HANDLE	hFound = NULL;

	// alle Verzeichnisse durchgehen
	hFound = FindFirstFile(sSearch, &data);
	
	if (hFound == (HANDLE)INVALID_HANDLE_VALUE)
	{
		FindClose(hFound);
		return; // EXIT
	}

	m_iDirectoryCluster += pDrive->GetClusterSize();

	for (BOOL bFound = TRUE; bFound; bFound=FindNextFile (hFound, &data)) 
	{
		sTmp = data.cFileName; 
		sTmp.MakeLower();
		if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (sTmp=="." || sTmp=="..")
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
				dbfFiles.Add(new CWin32FindData(data));
			}
			else if (-1!=sTmp.Find(_T(".")+CArchiv::m_sDbd))
			{
				dbdxFiles.Add(new CWin32FindData(data));
			}
			else if (-1!=sTmp.Find(_T(".dbx")))
			{
				dbdxFiles.Add(new CWin32FindData(data));
			}
			else if (-1!=sTmp.Find(_T(".dbi")))
			{
				// ignore our own dbi file
				sDbi = sTmp;
			}
			else
			{
				WK_TRACE(_T("foreign file found %s\\%s\n"),sDir,sTmp);
			}
		}
	}
	FindClose(hFound); 


	int i,j;
	BOOL bDbdx;
	CWin32FindData*	pDbf = NULL;
	CWin32FindData*	pDbdx = NULL;

	for (i=0;i<dbfFiles.GetSize();i++)
	{
		BYTE bFlags = 0;
		DWORD dwNr = 0;
		BOOL bIsAlarmList = FALSE;

		pDbf = dbfFiles.GetAtFast(i);
		_stscanf(pDbf->cFileName,_T("%08lx"),&dwNr);
		bFlags = CSequence::Nr2Flags(HIWORD(dwNr));
		if (bFlags & CAR_IS_ALARM_LIST)
		{
			bIsAlarmList = TRUE;
		}

		bDbdx = FALSE;
		pDbdx = NULL;
		if (   IsAlarmList()
			|| bIsAlarmList)
		{
			bDbdx = TRUE;
		}
		else
		{
			// search for matching dbt memo file
			for (j=0;j<dbdxFiles.GetSize();j++)
			{
				pDbdx = dbdxFiles.GetAtFast(j);
				if (0 == _tcsnicmp(pDbf->cFileName, pDbdx->cFileName, _tcslen(pDbf->cFileName)-2))
				{
					bDbdx = TRUE;
					dbdxFiles.RemoveAt(j);
					break;
				}
			}
		}

		if (bDbdx)
		{
			CSequence* pSequence = new CSequence(0,this,pDrive,0);
			if (pSequence->Scan(pDbf,pDbdx,map))
			{
				m_Sequences.AddScannedSequence(pSequence);
			}
			else
			{
				pSequence->Delete();
				delete pSequence;
			}
			WK_DELETE(pDbdx);
		}
		else
		{
			// dbf found but not dbd or dbx
			CString sName = sDir + "\\" + pDbf->cFileName;
			WK_TRACE(_T("dbf found but not dbd %s\n"),sName);
			if (DeleteFile(sName))
			{
				dwNrOfFiles--;
			}
			else
			{
				WK_TRACE_ERROR(_T("cannot delete %s, %s\n"),sName,GetLastErrorString());
			}
		}
	}

	for (j=0;j<dbdxFiles.GetSize();j++)
	{
		pDbdx = dbdxFiles.GetAt(j);
		CString sName = sDir + "\\" + pDbdx->cFileName;
		WK_TRACE(_T("dbd or dbdx found but not dbf %s\n"),sName);
		if (DeleteFile(sName))
		{
			dwNrOfFiles--;
		}
		else
		{
			WK_TRACE_ERROR(_T("cannot delete %s, %s\n"),sName,GetLastErrorString());
		}
	}
	
	if ((dwNrOfFiles==1) && (!sDbi.IsEmpty()))
	{
		sDbi = sDir + '\\' + sDbi;
		if (DeleteFile(sDbi))
		{
			dwNrOfFiles--;
		}
		else
		{
			WK_TRACE_ERROR(_T("cannot delete %s, %s\n"),sDbi,GetLastErrorString());
		}
		sDbi.Empty();
	}

	if (dwNrOfFiles == 0)
	{
		// no file in dir just remove it
		if (RemoveDirectory(sDir))
		{
			WK_TRACE(_T("removing empty dir %s\n"),sDir);
		}

	}
	dbdxFiles.DeleteAll();
	dbfFiles.DeleteAll();

	if (IsBackup())
	{
		m_dwSizeMB = GetSizeBytes().GetInMB();

		char szName[MAX_PATH];
		CWK_String sName;
		CWK_String sTitle;
		CWK_String sKey;
		CWK_String sFile;
		int p = sDir.ReverseFind('\\');
		int nChars;

		sFile = sDir.Left(p) + "\\backup.dbi";

		sKey.Format(_T("%04lx"),m_wOriginalNr);

		UINT uCodePage = GetPrivateProfileInt(_T("Backup"), _T("CodePage"), -1, sFile);

		nChars = GetPrivateProfileStringA("Backup", "Title", "", szName, MAX_PATH, sFile);
		sTitle.InitFromMemory(FALSE, szName, nChars, uCodePage);

		nChars = GetPrivateProfileStringA("OrigNames", sKey, "", szName, MAX_PATH, sFile);
		sName.InitFromMemory(FALSE, szName, nChars, uCodePage);
		m_sName = sName + _T(" ") + sTitle;

		m_sFormat = InitialFormat();
	}

	{
		CString sDbiPath = sDir + _T('\\') +sDbi;
		TRACE(_T("checking for %s as camera name map file\n"),m_sCameraMapPathname);
		if (DoesFileExist(m_sCameraMapPathname))
		{
			m_mapCamera.Load(m_sCameraMapPathname);
			if (!IsBackup())
			{
				DeleteFile(sDbiPath);
			}
		}
		else if (!sDbi.IsEmpty())
		{
			LoadCameraMap(sDbiPath);
		}
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CArchiv::ConvertSequence(const CConversion& conversion)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CSequence* pSequence;
	CIPCDrive* pDrive = theApp.m_Drives.GetDrive(conversion.GetDatabaseDir().Left(3));
	pSequence = m_Sequences.AddNewSequence(FALSE,this,pDrive);
	BOOL bRet = pSequence->Convert(conversion);
	if (bRet)
	{
		BOOL bCancel = FALSE;
		m_Sequences.Sort(bCancel);
	}
	else
	{
		DeleteSequence(pSequence->GetNr());
		pSequence = NULL;
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////
CSequence* CArchiv::CheckNewSequence(BOOL bSuspect, const CSaveData& data,WORD& wPrevSequenceNr,DWORD& dwNrOfRecords,BOOL& bNewSequenceCreated)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CSequence* pSequence = NULL;
	BOOL bIFrame = data.IsIFrame();
	DWORD dwSizeOfNextPicture = data.GetDataLength();
	CString sName(data.GetSequenceName());

	CAutoCritSec acs(&m_Sequences.m_cs);

	// check for empty archive or restart
	if (GetNrSequences()==0)
	{
		// no sequences or restart create a new one
		if (bIFrame)
		{
			// always start with I Frame
			pSequence = GetNewSequence(bSuspect);
			if (pSequence == NULL)
			{
				return pSequence;
			}
			else if (pSequence->Create(TRUE))
			{
				bNewSequenceCreated = TRUE;
				pSequence->SetName(sName);
			}
			else
			{
				m_Sequences.RemoveSequence(pSequence->GetNr());
				WK_DELETE(pSequence);
			}
			return pSequence;
		}
		else
		{
			// start with P Frame makes no sense
			WK_TRACE(_T("cannot start with P-Frame %s\n"),GetName());
			return NULL;
		}
	}

	acs.Unlock();

	// we have sequences, get the last one
	pSequence = m_Sequences.GetAtFast(m_Sequences.GetSize()-1);

	BOOL bFieldsChanged = FALSE;
	if (m_bDoFieldCheck)
	{
		bFieldsChanged = !pSequence->AreFieldsEqual();
		if (bFieldsChanged)
		{
			WK_TRACE(_T("fields changed creating new Sequence\n"));
		}
		else
		{
			// WK_TRACE(_T("fields equal\n"));
		}
	}

	BOOL bSequenceToLarge      = pSequence->IsFull(dwSizeOfNextPicture);;
	BOOL bHasDbx               = pSequence->HasDbx();
	BOOL bFirstArchiveSequence = (IsAlarm()||IsRing()) && pSequence->IsPreAlarm();
	BOOL bSequenceNameChanged  = (!sName.IsEmpty()) && (sName!=pSequence->GetName());
    BOOL bEmpty = pSequence->IsEmpty();
	BOOL bNewDay = FALSE;
	BOOL bTypeDifferent =    (bSuspect && !pSequence->IsSuspect())
						|| (!bSuspect && pSequence->IsSuspect());

	if (!bEmpty)
	{
		bNewDay = pSequence->GetLastTime().GetDay() != data.GetTimeStamp().GetDay();
	}

	if (    (   bSequenceToLarge 
		     || bHasDbx 
			 || bFirstArchiveSequence
			 || bSequenceNameChanged
			 || bTypeDifferent
			 || bNewDay
			 )
		 && (bIFrame)
		 && (!bEmpty)
		)
	{
		// close old sequence
		if (   bNewDay
			&& theApp.IsIDIP())
		{
			pSequence->ShrinkDatToFit();
		}
		pSequence->Close(TRUE);
		wPrevSequenceNr = pSequence->GetNr();
		dwNrOfRecords = pSequence->GetNrOfPictures();

		// if it's an alarm archive send current
		// state to launcher
		if (IsAlarm())
		{
			// bei Fuellstand > 60 % Alarm geben 
			if (!m_bAlarm60Send)
			{
				if (Is60PercentFull())
				{
					SendAlarmArchive60();
				}
			}
			
			theApp.DoSendAlarmFuel();
		}

		if (IsAlarm() && IsFull())
		{
			// Alarm Archiv voll
			return NULL;
		}
		else
		{
			AdjustSize();
			AdjustFree();
			pSequence = GetNewSequence(bSuspect);
			if (pSequence == NULL)
			{
				return NULL;
			}
			else if (pSequence->Create())
			{
				bNewSequenceCreated = TRUE;
				pSequence->SetName(sName);
			}
			else
			{
				m_Sequences.RemoveSequence(pSequence->GetNr());
				WK_DELETE(pSequence);
			}
		}
	}
	else
	{
		// use the current sequence
		// to append the picture
	}

	return pSequence;
}
/////////////////////////////////////////////////////////////////////
CSequence* CArchiv::GetNewSequence(BOOL bSuspect, CSequence* pSourceSequence /*= NULL*/, BOOL bThrow/*=FALSE*/)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CSequence* pSequence = NULL;
	CIPCDrive* pDrive = NULL;
	CString sDrive;
	
	if (pSourceSequence)
	{
		// for moving sequences from safe ring to alarm ring
		sDrive = pSourceSequence->GetDrive();
		pDrive = theApp.m_Drives.GetDrive(sDrive);
	}
	else
	{
		// normal new sequence
		if (m_sFixedDrive.IsEmpty())
		{
			pDrive = theApp.m_Drives.GetFreeDrive();
		}
		else
		{
			sDrive = m_sFixedDrive + '\\';
			pDrive = theApp.m_Drives.GetDrive(sDrive);
		}
	}

	if (pDrive)
	{
		pSequence = m_Sequences.AddNewSequence(bSuspect,this,pDrive,pSourceSequence);

		CWnd* pWnd = AfxGetMainWnd();
		if (WK_IS_WINDOW(pWnd))
		{
			pWnd->PostMessage(WM_COMMAND, ID_ACTUALIZE_TIMED);
		}
	}
	else
	{
		WK_TRACE_ERROR(_T("no valid drive for storing\n"));
		if (bThrow)
		{
			AfxThrowFileException(CFileException::badPath, ERROR_PATH_NOT_FOUND, sDrive);
		}
	}

	return pSequence;
}
/////////////////////////////////////////////////////////////////////
BOOL CArchiv::SaveData(CSaveData& data,BOOL& bNewSequenceCreated)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	BOOL bRet = TRUE;

	CAutoCritSec acs(&m_Sequences.m_cs);

	// attention save thread
	if (IsAlarm() && IsFull())
	{
		// cannot save alarm picture if archive is full
		if (!m_bAlarmFullSend)
		{
			WK_TRACE(_T("sending alarm archiv %s full alarm detector\n"),GetName());
			SendAlarmArchiveFull();
		}
		bRet = FALSE;
	}

	if (IsPreAlarm() && m_bMoving)
	{
		bRet = FALSE;
	}

	if (bRet)
	{
		BOOL bSuspect = FALSE;
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

		if (   m_pPreAlarm
			&& !bSuspect
			&& m_pPreAlarm->IsPreAlarm())
		{
			// is there any sequence in our safe ring
			// so just move it
			// that's the cheapest way
			if (!m_bMoved)
			{
				// do the move
				m_pPreAlarm->m_bMoving = TRUE;
				theApp.m_MoveThread.AddArchiveID(CSecID(SECID_GROUP_ARCHIVE,GetNr()));
				m_bMoved = TRUE;
			}
		}

		WORD wPrevSequenceNr = 0;
		DWORD dwNrOfRecords = 0;
		CSequence* pSequence = CheckNewSequence(bSuspect,data,wPrevSequenceNr,dwNrOfRecords,bNewSequenceCreated);

		if (pSequence == NULL)
		{
			// may be no new sequence because of alarm archiv
			if (IsAlarm() && IsFull())
			{
				if (!m_bAlarmFullSend)
				{
					WK_TRACE(_T("sending alarm archiv %s full alarm detector\n"),GetName());
					SendAlarmArchiveFull();
				}
				bRet = TRUE;
			}
			else
			{
				WK_TRACE_ERROR(_T("cannot create a new sequence for %s\n"),GetName());
				bRet = FALSE;
			}
		}
		else
		{
			if (pSequence->SaveData(data,wPrevSequenceNr,dwNrOfRecords))
			{
				// ok
				bRet = TRUE;
				m_bDoFieldCheck = FALSE;
			}
			else
			{
				// close the current one and try again
				pSequence->Close(TRUE);

				if (pSequence->IsEmpty())
				{
					// delete empty sequences
					m_Sequences.DeleteSequence(pSequence->GetNr());
					pSequence = NULL;
				}
				
				AdjustSize();
				AdjustFree();
				
				pSequence = GetNewSequence(bSuspect);
				bNewSequenceCreated = TRUE;
				if (pSequence == NULL)
				{
					bRet = FALSE;
				}
				else if (pSequence->Create())
				{
					// second try
					if (pSequence->SaveData(data,wPrevSequenceNr,dwNrOfRecords))
					{
						WK_TRACE(_T("wrote picture in second try\n"));
						bRet = TRUE;
						m_bDoFieldCheck = FALSE;
					}
					else
					{
						WK_TRACE(_T("cannot write picture after second try\n"));
						if (pSequence->IsEmpty())
						{
							// delete empty sequences
							m_Sequences.DeleteSequence(pSequence->GetNr());
							pSequence = NULL;
						}
					}
				}
				else
				{
					m_Sequences.RemoveSequence(pSequence->GetNr());
					WK_DELETE(pSequence);
					bRet = FALSE;
				}
			}
		}
	}

//	Unlock();

	return bRet;
}
//////////////////////////////////////////////////////////////////////
void CArchiv::SendAlarmArchiveFull()
{
	if (IsAlarm())
	{
		CCallStackEntry cse(_T(__FUNCTION__));
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
		m_bAlarmFullSend = TRUE;
		theApp.DoSendAlarmFuel();
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CArchiv::Is60PercentFull()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CIPCInt64 s(GetSizeMB());
	CIPCInt64 i = GetSizeBytes();
	s *= CIPCInt64(1024*1024);
	CIPCInt64 iPercentage((i.GetInt64() * 100) / s.GetInt64());
	return (iPercentage.GetInt64()>=60);
}
//////////////////////////////////////////////////////////////////////
void CArchiv::SendAlarmArchive60()
{
	CCallStackEntry cse(_T(__FUNCTION__));
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
		m_bAlarm60Send = TRUE;
	}
}
//////////////////////////////////////////////////////////////////////
void CArchiv::MoveSequences(CArchiv* pPreAlarm)
{
	CCallStackEntry cse(_T(__FUNCTION__));

	if (pPreAlarm->GetNrSequences())
	{
		// make sure that ring archive has enough space
		// for pre alarm
		if (IsRing())
		{
			__int64 iPreAlarmSize = pPreAlarm->GetSizeBytes().GetInt64();
			__int64 iFreeInMB = 0;
			__int64 iDefinedSizeInBytes = (__int64)GetSizeMB() * (__int64)(1024*1024);

			iFreeInMB =  iDefinedSizeInBytes - GetSizeBytes().GetInt64();

			while (iFreeInMB < iPreAlarmSize)
			{
				m_Sequences.DeleteOldestSequence();
				iFreeInMB = iDefinedSizeInBytes - GetSizeBytes().GetInt64();
			}
		}

		/*
		if (m_Sequences.GetSize())
		{
			CSequence* pLastSequence;
			pLastSequence = m_Sequences.GetAtFast(m_Sequences.GetSize()-1);
			if (pLastSequence)
			{
				pLastSequence->ShrinkDatToFit();
			}
		}*/

		CSequence* pSourceSequence;
		CSequence* pDestinationSequence;

		CString s;

		s.Format(_T("MOVE %s --> %s"),pPreAlarm->GetName(),GetName());
		WK_TRACE(_T("%s by alarm\n"),s);

		CWK_StopWatch w;
		pPreAlarm->Lock(_T(__FUNCTION__));
		while (pPreAlarm->m_Sequences.GetSize() && !IsFull())
		{
			pSourceSequence = pPreAlarm->m_Sequences.GetAtFast(0);
			pPreAlarm->m_Sequences.RemoveAt(0);
			pPreAlarm->Unlock();
			if (pSourceSequence->GetNrOfPictures()>0)
			{
				pDestinationSequence = GetNewSequence(FALSE,pSourceSequence);
				if (pDestinationSequence)
				{
					if (!pDestinationSequence->Move(*pSourceSequence))
					{
						WK_TRACE_ERROR(_T("cannot move sequence %s to %s\n"),
							pSourceSequence->Format(),pDestinationSequence->Format());

						DeleteSequence(pDestinationSequence->GetNr());
						pDestinationSequence = NULL;
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
				// don't move empty sequence
				WK_TRACE(_T("cannot move empty sequence\n"));
				theApp.m_Clients.DoIndicateDeleteSequence(*pSourceSequence);
			}
			WK_DELETE(pSourceSequence);
			// lock for the next while comparison
			pPreAlarm->Lock(_T(__FUNCTION__));
		}
		pPreAlarm->Unlock();
		w.StopWatch(s);
		AdjustSize();
		theApp.DoSendAlarmFuel();
	}
	pPreAlarm->m_bMoving = FALSE;
	m_bMoved = FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CArchiv::IsFull()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CIPCInt64 iSize = m_Sequences.GetSizeBytes();

	if (   IsRing()
		|| IsPreAlarm())
	{
		iSize += m_iSequenceSizeBytes;
	}
	return (iSize >= m_iSizeBytes);
}
//////////////////////////////////////////////////////////////////////
void CArchiv::SetSizeInMB(DWORD dwNewSize)
{
	m_dwSizeMB = dwNewSize;
	m_iSizeBytes = (__int64)(m_dwSizeMB) * 1024 * 1024;
}
//////////////////////////////////////////////////////////////////////
void CArchiv::AdjustSize()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	// always sort
	BOOL bCancel = FALSE;
	m_Sequences.Sort(bCancel);

	if (   !IsBackup()
		&& !IsAlarm()
		&& !IsDynamicSize()
		)
	{
		while (IsFull())
		{
			m_Sequences.DeleteOldestSequence();
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CArchiv::AdjustFree()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	if (!IsDynamicSize())
	{
		if (IsRing() || IsPreAlarm())
		{
			//
			CIPCInt64 iSize = m_Sequences.GetSizeBytes();
			if (iSize.GetInt64() + m_iSequenceSizeBytes.GetInt64() > m_iSizeBytes.GetInt64())
			{
				// with the next sequence the archiv
				// would be larger than archiv size!
				// WK_TRACE(_T("deleting oldest sequence for ring %s\n"),GetName());
				m_Sequences.DeleteOldestSequence();
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
int CArchiv::GetNrSequences()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CAutoCritSec acs(&m_Sequences.m_cs);
	int r = m_Sequences.GetSize();
	acs.Unlock();
	return r;
}
//////////////////////////////////////////////////////////////////////////////
CIPCInt64 CArchiv::GetBytesFree()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	return m_iSizeBytes.GetInt64() - GetSizeBytes().GetInt64();
}
//////////////////////////////////////////////////////////////////////////////
CSequence* CArchiv::GetSequenceIndex(int i)
{
	return m_Sequences.GetSequenceIndex(i);
}
//////////////////////////////////////////////////////////////////////
CSequence* CArchiv::GetNextSequence(WORD wSequenceNr)
{
	CSequence* pSequence = NULL;
	CAutoCritSec acs(&m_Sequences.m_cs);

	if (   wSequenceNr == 0
		&& m_Sequences.GetSize()>0)
	{
		pSequence = m_Sequences.GetAtFast(0);
	}
	else
	{
		for (int i=0;i<m_Sequences.GetSize();i++)
		{
			if (m_Sequences.GetAtFast(i)->GetNr() == wSequenceNr)
			{
				if (i+1<m_Sequences.GetSize())
				{
					pSequence = m_Sequences.GetAtFast(i+1);
				}
				break;
			}
		}
	}

	acs.Unlock();
	return pSequence;
}
//////////////////////////////////////////////////////////////////////////////
CSequence* CArchiv::GetSequenceID(WORD wID)
{
	return m_Sequences.GetSequenceID(wID);
}
//////////////////////////////////////////////////////////////////////////////
void CArchiv::Lock(LPCTSTR sFkt/*=NULL*/)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	m_Sequences.Lock(sFkt);
}
//////////////////////////////////////////////////////////////////////////////
void CArchiv::Unlock()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	m_Sequences.Unlock();
}
//////////////////////////////////////////////////////////////////////////////
BOOL CArchiv::Delete()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	WK_TRACE(_T("deleting archive %s\n"),GetName());
	BOOL bRet = FALSE;
	if (DeleteAll())
	{
		CString sDir;

		// now delete all directories
		bRet = TRUE;
		if (IsBackup())
		{
			// only try to delete the backup base dir
			sDir.Format(_T("%s\\arch%04lx"),m_sBackupDir,GetNr());
			bRet &= DeleteDirRecursiv(m_sBackupDir);
			if(!bRet)
			{
				WK_TRACE_ERROR(_T("cannot delete archive: %s\n"),GetLastErrorString());
			}
		}
		else
		{
			for (int i=0;i<theApp.m_Drives.GetSize();i++)
			{
				CIPCDrive* pDrive = theApp.m_Drives.GetAtFast(i);
				if (   pDrive 
					&& (   pDrive->IsDatabase()
					    || pDrive->IsRemoveableDatabase()
						)
					)
				{
					sDir.Format(_T("%sdbs\\arch%04lx"),pDrive->GetRootString(),GetNr());
					bRet &= DeleteDirRecursiv(sDir);
				}
			}
		}
	}
	return bRet;
}
//////////////////////////////////////////////////////////////////////////////
BOOL CArchiv::DeleteAll()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CSequences Temp;

	//on manual delete a backup archive it could be possible that at the same time the
	//check process CArchivs::CheckBackupDirectories() 
	//(check if the backup files still on the backup drive) 
	//finds out that the backup files are not longer exists and also wants delete the
	//sequence list. Because of this we copy we sequence list.
	CAutoCritSec acs(&m_Sequences.m_cs);

	int i, nCount = m_Sequences.GetCount();
	for (i=0; i<nCount; i++)
	{
		Temp.Add(m_Sequences.GetAtFast(i));
	}
	m_Sequences.RemoveAll();
	acs.Unlock();

	while (Temp.DeleteOldestSequence())
	{
	}

	CAutoCritSec acs2(&m_csCameraMap);
	m_mapCamera.DeleteContent();
	acs2.Unlock();
	return m_Sequences.GetSize() == 0;
}
//////////////////////////////////////////////////////////////////////////////
BOOL CArchiv::DeleteSequence(WORD wSequence)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	BOOL bWas60PercentFull = FALSE;
	
	if (IsAlarm())
	{
		bWas60PercentFull = Is60PercentFull();
	}

	// TODO! RKE: Update the camera map if a sequence is deleted
	// If there is only one sequence left, delete the whole map
	// If there are more sequences, first save the camera ID´s of the
	// sequence to delete in a separate camera map (e.g. m_mapCamToVerify)
	// Then lookup (e.g. in the verify thread), whether these camera ID´s are
	// contained in any other of the remaining sequences.
	// It is supposable, that the deleted camera ID is found in the very next
	// sequence, where the search can be stopped, because the camera ID is
	// still remaining. Only in the special case, that one camera ID is deleted
	// in a large archive and is the last mohawk, the search will last longer

	// now delete
	BOOL bRet = m_Sequences.DeleteSequence(wSequence);

	if (bRet)
	{
		if (IsAlarm())
		{
			BOOL bIs60PercentFull = Is60PercentFull();

			m_bAlarmFullSend = FALSE;

			if (bWas60PercentFull && !bIs60PercentFull)
			{
				m_bAlarm60Send = FALSE;
			}

			theApp.DoSendAlarmFuel();
		}

		if (m_Sequences.GetSize() == 0)
		{
			CAutoCritSec acs(&m_csCameraMap);
			m_mapCamera.DeleteContent();
			acs.Unlock();
		}

	}

	return bRet;
}
//////////////////////////////////////////////////////////////////////////////
void CArchiv::Finalize(BOOL bShutDown)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	if (!IsBackup())
	{
		// close all sequences and store
		// time and record count
		m_Sequences.Finalize(bShutDown,m_dwMaxStorageTime);

		CString sCPn = m_sCameraMapPathname;
		sCPn.MakeLower();
		if (   bShutDown
			&& (m_Sequences.GetSizeBytes()>0)
			&& theApp.m_Drives.GetDrive((char)sCPn.GetAt(0)) != NULL
			)
		{
			m_mapCamera.Save(m_sCameraMapPathname);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CArchiv::SetCameraName(DWORD dwID,const CString& sName)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CString sID;
	sID.Format(_T("%08lx"),dwID);

	CAutoCritSec acs(&m_csCameraMap);
	m_mapCamera.SetAt(sID, sName);
	acs.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CArchiv::GetCameraName(DWORD dwID,CString& sName)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CString sID,sLookup;
	BOOL bRet = FALSE;
	sID.Format(_T("%08lx"),dwID);

	CAutoCritSec acs(&m_csCameraMap);
	bRet = m_mapCamera.Lookup(sID,sLookup);
	acs.Unlock();
	if (bRet)
	{
		sName = sLookup;
	}

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CArchiv::SaveCameraMap(const CString& sPathname)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	BOOL bRet = FALSE;
	if (m_mapCamera.GetCount())
	{
		m_mapCamera.Save(sPathname);
		bRet = TRUE;
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
void CArchiv::LoadCameraMap(const CString& sPathname)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CString s;
	CString rKey,rValue;
	int i,c;

	c = GetPrivateProfileInt(szCameraSection,_T("Nr"),0,sPathname);

	for (i=0;i<c;i++)
	{
		rKey.Empty();
		rValue.Empty();
		s.Format(_T("CN%d"),i);
		GetPrivateProfileString(szCameraSection,s,_T(""),
			rValue.GetBuffer(64),64,sPathname);
		rValue.ReleaseBuffer();
		s.Format(_T("ID%d"),i);
		GetPrivateProfileString(szCameraSection,s,_T(""),
			rKey.GetBuffer(64),64,sPathname);
		rKey.ReleaseBuffer();
		if (   !rKey.IsEmpty()
			&& !rValue.IsEmpty())
		{
			CAutoCritSec acs(&m_csCameraMap);
			m_mapCamera.SetAt(rKey, rValue);
			acs.Unlock();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CArchiv::DoConfirmCameraNames(CIPCDatabaseServer* pCIPC,DWORD dwUserData)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CIPCFields fields;
	CString rKey,rValue;
	POSITION pos;

	CAutoCritSec acs(&m_csCameraMap);
	for( pos = m_mapCamera.GetStartPosition(); pos != NULL; )   
	{
		rKey.Empty();
		rValue.Empty();
		m_mapCamera.GetNextAssoc(pos, rKey, rValue);
		if (   !rKey.IsEmpty() 
			&& !rValue.IsEmpty())
		{
			fields.Add(new CIPCField(rKey,rValue,64,'C'));
		}
	}
	acs.Unlock();

	pCIPC->DoConfirmCameraNames(GetNr(),dwUserData,fields);
}
//////////////////////////////////////////////////////////////////////
CIPCInt64 CArchiv::GetSizeBytes(const CSystemTime& stStart,
							        const CSystemTime& stEnd,
									CIPCInt64& i64Sum,
									DWORD dwMaximumInMB,
									BOOL& bCancelled)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CIPCInt64 ret = 0;
	
	if (!IsBackup())
	{
		CIPCInt64 one = 0;
		CSequence* pSequence = NULL;
		
		CAutoCritSec acs(&m_Sequences.m_cs);
		
		for (int i=0;i<m_Sequences.GetSize();i++)
		{
			pSequence = m_Sequences.GetAtFast(i);
			if (!pSequence->IsBackup())
			{
				one = pSequence->GetSizeBytes(stStart,stEnd);
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
void CArchiv::DeleteDatabaseDrive(const CString& sRootString)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	// delete all Sequences on this drive
	CAutoCritSec acs(&m_Sequences.m_cs);
	CSequence* pSequence;
	for (int i=m_Sequences.GetSize()-1;i>=0;i--)
	{
		pSequence = m_Sequences.GetAtFast(i);
		if (0 == sRootString.CompareNoCase(pSequence->GetDrive()))
		{
			// delete it
			DeleteSequence(pSequence->GetNr());
		}
	}
	acs.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CArchiv::DeleteDatabaseDirectory(const CString& sDirectory)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	// delete all Sequences on this drive
	CAutoCritSec acs(&m_Sequences.m_cs);
	CSequence* pSequence;
	for (int i=m_Sequences.GetSize()-1;i>=0;i--)
	{
		pSequence = m_Sequences.GetAtFast(i);
		CString sSequenceDirectory = pSequence->GetDirectory();
		sSequenceDirectory = sSequenceDirectory.Left(sDirectory.GetLength());
		if (0 == sDirectory.CompareNoCase(sSequenceDirectory))
		{
			// delete it
			DeleteSequence(pSequence->GetNr());
		}
	}
	acs.Unlock();
}
//////////////////////////////////////////////////////////////////////
BOOL CArchiv::Verify(BOOL& bCancel)
{
	BOOL bRet = TRUE;
	if (!IsBackup())
	{
		for (int i=0;i<m_Sequences.GetSize() && !bCancel;i++)
		{
			bRet &= m_Sequences.GetAtFast(i)->Verify();
		}
	}

	if (!bRet)
	{
		m_Sequences.Sort(bCancel);
	}

	return bRet;
}
//////////////////////////////////////////////////////////////////////
CSequence* CArchiv::GetOldestSequence()
{
	Lock(_T("C15"));

	CSequence* pSequence = NULL;

	for (int i=0; i<m_Sequences.GetSize();i++)
	{
		pSequence = m_Sequences.GetAtFast(i);
		if (!pSequence->IsBackup())
		{
			break;
		}
		// reset to nothing found
		pSequence = NULL;
	}

	Unlock();

	return pSequence;
}
//////////////////////////////////////////////////////////////////////
BOOL CArchiv::DeleteOldestSequence()
{
	BOOL bRet = FALSE;
	Lock(_T("C10"));

	CSequence* pSequence = NULL;

	for (int i=0; i<m_Sequences.GetSize();i++)
	{
		pSequence = m_Sequences.GetAtFast(i);
		if (!pSequence->IsBackup())
		{
			m_Sequences.RemoveAt(i);
			if (!IsPreAlarm())
			{
				theApp.m_Clients.DoIndicateDeleteSequence(*pSequence);
			}
			break;
		}
		// reset to nothing found
		pSequence = NULL;
	}

	Unlock();

	if (pSequence)
	{
		bRet = pSequence->Delete();
		if (bRet)
		{
		}
		WK_DELETE(pSequence);
	}

	if (IsRing())
	{
		theApp.m_Archives.Lock();
		CArchiv* pAlarmList = theApp.m_Archives.GetSafeRingArchiv(GetNr());
		if (   pAlarmList
			&& pAlarmList->IsAlarmList())
		{
			if (m_Sequences.GetSize())
			{
				CSystemTime st;
				st = m_Sequences.GetAtFast(0)->GetFirstTime();
				pAlarmList->DeleteAllOlder(st);
			}
			else
			{
				pAlarmList->DeleteAll();
			}
		}
		theApp.m_Archives.Unlock();
	}

	return bRet;
}
//////////////////////////////////////////////////////////////////////////////
BOOL CArchiv::DeleteAllOlder(const CSystemTime& st)
{
	TRACE(_T("Archive %04x deleting all older than %s\n"),
		GetNr(),st.GetDate());
	BOOL bRet = TRUE;

	Lock(_T("C55"));
	while (m_Sequences.GetSize()>0)
	{
		CSequence* pSequence = m_Sequences.GetAtFast(0);
		TRACE(_T("Archive Nr %04x Check Sequence Nr %04x %s\n"),
			GetNr(),
			pSequence->GetNr(),
			pSequence->GetFirstTime().GetDate());
		if (pSequence->GetFirstTime().GetDBDate()<st.GetDBDate())
		{
			bRet = DeleteSequence(pSequence->GetNr());
			TRACE(_T("Deleted Sequence Nr %04x %s\n"),pSequence->GetNr(),
				pSequence->GetFirstTime().GetDate());
		}
		else
		{
			break;
		}
	}

	Unlock();
	return bRet;
}
