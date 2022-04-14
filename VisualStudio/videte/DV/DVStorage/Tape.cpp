/* GlobalReplace: line %s --> line %d */
// Tape.cpp: implementation of the CTape class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DVStorage.h"
#include "Tape.h"
#include "Collection.h"

#include "FindData.h"
#include "CIPCDatabaseStorage.h"
#include "Search.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static char BASED_CODE szBYTE[]  = "%02hx\0";
static char BASED_CODE szDWORD[] = "%08lx\0";

static char BASED_CODE szIntFormat[] = "%d\0";

static TCHAR BASED_CODE szDbf[] = _T("dbf");
static TCHAR BASED_CODE szDat[] = _T("dat");
char BASED_CODE szMM[]  = "MM";

#define SCAN_DWORD "%08x"
#define SCAN_INT   "%d"

int repair(Code4& c4, const char* fileName)
{
	File4 f;
	DATA4HEADER_FULL fullHeader;
	int rc = f.open(c4, (char *)fileName, 1);
	if (rc != 0)
	{
		CWK_String sFileName(fileName);
		WK_TRACE_ERROR(_T("cannot open dbf for repair %s,%d\n"),sFileName,rc);
		if (rc != r4noOpen)
			return(rc);
		else
			return(0);
	}

	rc = f.read(0L, &fullHeader, sizeof(fullHeader));
	if (rc < 0)
	{
		CWK_String sFileName(fileName);
		WK_TRACE_ERROR(_T("cannot read full header for repair %s,%d\n"),sFileName,rc);
		f.close();
		return(rc);
	}
	WK_TRACE(_T("DBF version   = %02d\n"),(TCHAR)fullHeader.version);
	WK_TRACE(_T("DBF update    = %04d,%02d,%02d\n"),(TCHAR)fullHeader.yy+1900,(TCHAR)fullHeader.mm,(TCHAR)fullHeader.dd);
	WK_TRACE(_T("DBF numRecs   = %d\n"),fullHeader.numRecs);
	WK_TRACE(_T("DBF headerLen = %d,0x%08lx\n"),fullHeader.headerLen,fullHeader.headerLen);
	WK_TRACE(_T("DBF recordLen = %d,0x%08lx\n"),fullHeader.recordLen,fullHeader.recordLen);

	if (fullHeader.recordLen>0)
	{
		// try to find out the last correct record
		unsigned char* pBuffer = new unsigned char[fullHeader.recordLen];
		long pos = fullHeader.headerLen;
		unsigned bytesread = 0;
		long lNumRecords = 0;

		for (int i=0;bytesread>=0;i++,pos+=bytesread)
		{
			ZeroMemory(pBuffer,fullHeader.recordLen);
			bytesread = f.read(pos,pBuffer,fullHeader.recordLen);
			/*
			CWK_String s;
			s.InitFromMemory(FALSE,pBuffer,fullHeader.recordLen);
			TRACE(_T("read %d bytes at pos %d record nr %d <%s>\n"),bytesread,pos,i,(LPCTSTR)s);
			*/

			unsigned bContainsZero = FALSE;
			for (int j=0;j<fullHeader.recordLen;j++)
			{
				if (pBuffer[j] == 0)
				{
					bContainsZero = TRUE;
					break; // the buffer loop
				}
			}
			if (bContainsZero)
			{
				lNumRecords = i;
				break;
			}
		}

		if (lNumRecords>0)
		{
			// file contains valid records
			rc = f.write(4L, &lNumRecords, sizeof(long));
			long dwFileLen = lNumRecords*fullHeader.recordLen+fullHeader.headerLen;
			f.setLen(dwFileLen);
			f.flush();
			WK_TRACE(_T("file contains %d valid records new file len will be set to %d\n"),
				lNumRecords,dwFileLen);
		}
		else
		{
			// cannot repair file
			rc = -1;
		}


		delete [] pBuffer;
	}

	int rc2 = f.close();
	if (rc != 0)
		rc2 = rc;
	return rc2;
}
//////////////////////////////////////////////////////////////////////
int checkDbfHeader(Code4& c4, const char *fileName)
{
	File4 f;
	DATA4HEADER_FULL fullHeader;
	int rc = f.open(c4, (char *)fileName, 1);
	if (rc != 0)
	{
		if (rc != r4noOpen)
			return(rc);
		else
			return(0);
	}
	
	rc = f.read(0L, &fullHeader, sizeof(fullHeader));
	if (rc < 0)
	{
		f.close();
		return(rc);
	}
	
#ifdef S4BYTE_SWAP
	fullHeader.numRecs   = x4reverseLong( (void *)&fullHeader.numRecs  );
	fullHeader.headerLen = x4reverseShort((void *)&fullHeader.headerLen);
	fullHeader.recordLen = x4reverseShort((void *)&fullHeader.recordLen);
#endif
	
	if (fullHeader.recordLen>0)
	{
		long lengthRecords = (f.len() - fullHeader.headerLen) / fullHeader.recordLen;
		
		if (fullHeader.numRecs != lengthRecords)
		{
#ifdef S4BYTE_SWAP
			lengthRecords = x4reverseLong( (void *)&lengthRecords ) ;
#endif
			rc = f.write(4L, &lengthRecords, sizeof(long));
		}
	}
	else
	{
		rc = -1;
	}
	int rc2 = f.close();
	if (rc != 0)
	{
		rc2 = rc;
	}
	return rc2;
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CTape::CTape(BYTE bFlags, 
			 CCollection* pCollection, 
			 CIPCDrive* pDrive, 
			 WORD wID)
{
	m_wID		  = wID;
	m_wOriginalID = wID;
	m_pCollection = pCollection;
	m_sDrive  = pDrive->GetRootString();
	m_sFolder = pCollection->GetFolder();
	m_dwNumRecords = 0;
	m_dwClusterSize = pDrive->GetClusterSize();
	m_dwDbfSize = 0;
	m_dwDatSize = 0;
	m_dwFieldSize = 0;
	
	m_dwLastVideo  = 0;
	m_dwLastAudio  = 0;

	if (bFlags == 0)
	{
		m_bFlags = Nr2Flags((WORD)pCollection->GetType());
	}
	else
	{
		m_bFlags = bFlags;
	}
	m_bLockedByBackup = FALSE;

	m_iOpenDbf = 0;
	m_iOpenDat = 0;
	m_pData4 = NULL;
	m_dwSavedWithoutFlush = 0;

	m_stFirst.GetLocalTime();
	m_stLast = m_stFirst;

	// m_fImages
	m_dwDatWritePosition = 0;
}
//////////////////////////////////////////////////////////////////////
WORD CTape::GetArchiveNr() const
{
	return m_pCollection->GetNr();
}
//////////////////////////////////////////////////////////////////////
CTape::~CTape()
{
	Close(TRUE);
}
//////////////////////////////////////////////////////////////////////
CString CTape::GetName() const
{
	return m_stFirst.GetDateTime();
}
//////////////////////////////////////////////////////////////////////
CIPCInt64 CTape::GetSizeBytes() const
{
	CIPCInt64 r = 0;
	if (m_dwClusterSize>0)
	{
		r += CIPCInt64(RoundUp(m_dwDbfSize, m_dwClusterSize));
		r += CIPCInt64(RoundUp(m_dwDatSize, m_dwClusterSize));
	}
	else
	{
		r += CIPCInt64(m_dwDbfSize);
		r += CIPCInt64(m_dwDatSize);
	}
	return r;
}
//////////////////////////////////////////////////////////////////////
Code4& CTape::GetCode4()
{
	return m_pCollection->m_CodeBase;
}
//////////////////////////////////////////////////////////////////////
BOOL CTape::IsFull(DWORD dwSizeOfNextPicture)
{
	if (theApp.GetNoFrag())
	{
		return m_dwDatWritePosition+dwSizeOfNextPicture > m_pCollection->GetTapeSizeBytes();
	}
	else
	{
		return GetSizeBytes() >= CIPCInt64(m_pCollection->GetTapeSizeBytes());
	}
}
//////////////////////////////////////////////////////////////////////
CIPCInt64 CTape::GetSizeBytes(const CSystemTime& stStart,
							  const CSystemTime& stEnd)
{
	CIPCInt64 r = 0;

	if (m_stFirst > stEnd)
	{
		// Sequence is too young
//		TRACE(_T("GetSizeBytes %d Sequence is too young\n"), GetNr());
		r = 0;
	}
	else if (m_stLast < stStart)
	{
		// Sequence is too old
//		TRACE(_T("GetSizeBytes %d Sequence is too old\n"), GetNr());
		r = 0;
	}
	else
	{
		// find out correct start and end record
		DWORD dwStartRecord = 1;
		DWORD dwEndRecord = m_dwNumRecords;

		if (m_stFirst < stStart)
		{
			// find out correct start record
			dwStartRecord = GetRecordNr(stStart,TRUE);
		}
		if (m_stLast > stEnd)
		{
			// find out correct end record
			dwEndRecord = GetRecordNr(stEnd,FALSE);
		}

		if (   dwStartRecord == 1
			&& dwEndRecord == m_dwNumRecords)
		{
			// ganze Sequence
			r = GetSizeBytes();
		}
		else if (   dwStartRecord != 0
			&& dwEndRecord != 0)
		{
			// find out size
			FILESIZE dwStartOffset,dwEndOffset;
			DWORD dwStartLength,dwEndLength;
			GetOffsetAndLength(dwStartRecord,dwStartOffset,dwStartLength);
			GetOffsetAndLength(dwEndRecord,dwEndOffset,dwEndLength);

			if (dwStartRecord<=dwEndRecord)
			{
				FILESIZE dwDbfSize = (dwEndRecord-dwStartRecord)*m_dwFieldSize;
				FILESIZE dwDatSize = dwEndOffset-dwStartOffset+dwEndLength;

				dwDbfSize += m_pCollection->m_FieldInfo.numFields() * 32 + 34;
					
/*
				if (m_dwClusterSize>0)
				{
					r += CIPCInt64(RoundUp(dwDbfSize,m_dwClusterSize));
					r += CIPCInt64(RoundUp(dwDatSize,m_dwClusterSize));
				}
				else
*/
				{
					r += CIPCInt64(dwDbfSize);
					r += CIPCInt64(dwDatSize);
				}
/*
				WK_TRACE(_T("GetSizeBytes %s, %d->%d/%d %s Bytes\n"),
					m_stFirst.GetDateTime(), dwStartRecord, dwEndRecord, m_dwNumRecords, r.Format(TRUE));
*/
			}
		}
	}

	return r;
}
//////////////////////////////////////////////////////////////////////
DWORD CTape::GetRecordNr(const CSystemTime& st, BOOL bGreaterThan)
{
	DWORD dwRet = 0;

	Lock(_T("S16"));

	if (m_dwNumRecords>0)
	{
		if (OpenDbf())
		{
			Relate4set relation;
			int rc = 0;
			int ret = 0;

			ret = relation.init(*m_pData4);
			if (ret == r4success)
			{
				CWK_String sExpression;
				CString sDate = st.GetDBDate();
				CString sTime = st.GetDBTime();
				CString sOp;

				if (bGreaterThan)
				{
					sOp = _T(">");
				}
				else
				{
					sOp = _T("<");
				}
				sExpression.Format(_T("((%s%s='%s') .AND. (%s='%s')) .OR. (%s%s'%s')"),
					_T(DVR_TIME), LPCTSTR(sOp), LPCTSTR(sTime),
					_T(DVR_DATE), LPCTSTR(sDate),
					_T(DVR_DATE), LPCTSTR(sOp), LPCTSTR(sDate));

				rc = relation.querySet(sExpression);
				
/*
				TRACE(_T("Startzeit=%s,Endzeit=%s,Suchzeit=%s Op=%s\n"),
					m_stFirst.GetDateTime(),
					m_stLast.GetDateTime(),
					st.GetDateTime(),
					sOp);
				TRACE(_T("expression = %s\n"), LPCTSTR(sExpression));
*/

				if (rc==r4success)
				{
					if (bGreaterThan)
						rc = relation.top();
					else
						rc = relation.bottom();

					if (r4success==rc)
					{
						// we are now on an found record
						dwRet = m_pData4->recNo();
					}
					else
					{
						TRACE(_T("GetRecordNr cannot top relation %s, %d\n"), LPCTSTR(sExpression), rc);
						dwRet = 0;
						errorSet();
					}
				}
				else
				{
					TRACE(_T("GetRecordNr cannot set Query %s\n"), LPCTSTR(sExpression));
					dwRet = 0;
					errorSet();
				}
			}
			else
			{
				WK_TRACE_ERROR(_T("Relate4set::init failed C=%d, T=%d, %d\n"),
					GetArchiveNr(),GetNr(), rc);
				errorSet();
			}
			relation.free();

			CloseDbf();
		}
		else // OpenDbf
		{
			TRACE(_T("GetRecordNr cannot open dbf\n"));
			dwRet = 0;
		}
	}
	else
	{
		// empty sequence
		dwRet = 0;
	}

	Unlock();

	return dwRet;
}
//////////////////////////////////////////////////////////////////////
void CTape::errorSet(int e/*=0*/)
{
	m_pCollection->m_CodeBase.errorSet(e);
}
//////////////////////////////////////////////////////////////////////
BOOL CTape::GetOffsetAndLength(DWORD dwRecordNr, FILESIZE& dwOffset, DWORD& dwLen)
{
	BOOL bRet = TRUE;
	DWORD dwOffsetLocal,dwLenLocal;

	dwOffsetLocal = 0;
	dwLenLocal = 0;

	Lock(_T("S99"));

	if (OpenDbf())
	{
		int ret = m_pData4->go(dwRecordNr);
		if (r4success == ret)
		{
			Field4 fLength;

			if (r4success==fLength.init(*m_pData4, DVR_DATA_LEN))
			{
				if (1 == sscanf(fLength.str(), SCAN_DWORD, &dwLenLocal))
				{
				}
				else
				{
					bRet = FALSE;
				}
			}
			else
			{
				errorSet();
				bRet = FALSE;
			}
			if (r4success == fLength.init(*m_pData4, DVR_DATA_POS))
			{
				if (1 == sscanf(fLength.str(),  SCAN_DWORD, &dwOffsetLocal))
				{
				}
				else
				{
					bRet = FALSE;
				}
			}
			else
			{
				errorSet();
				bRet = FALSE;
			}
		}
		else
		{
			errorSet();
		}
		CloseDbf();
	}
	else // OpenDbf
	{
		bRet = FALSE;
	}

	Unlock();

	dwLen = dwLenLocal;
	dwOffset = dwOffsetLocal;

	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CTape::operator < (const CTape& s)
{
	if (IsBackup() && !s.IsBackup())
	{
		return TRUE;
	}
	if (!IsBackup() && s.IsBackup())
	{
		return FALSE;
	}


	// same type
	return m_stFirst < s.m_stFirst;
}
//////////////////////////////////////////////////////////////////////
BYTE CTape::Nr2Flags(WORD wNr)
{
	BYTE bFlags = 0;

	switch(wNr)
	{
	case 1:
		bFlags = CAR_IS_ALARM;
		break;
	case 2:
		bFlags = CAR_IS_RING;
		break;
	case 3:
		bFlags = CAR_IS_SAFE_RING;
		break;
	case 4:
		bFlags = CAR_IS_SEARCH;
		break;
	case 5:
		bFlags = CAR_IS_BACKUP;
		break;
	case 6:
		bFlags = CAR_IS_SUSPECT;
		break;
	case 7:
		bFlags = CAR_IS_ALARM_LIST;
		break;
	case 11:
		bFlags = CAR_IS_ALARM|CAR_IS_BACKUP;
		break;
	case 12:
		bFlags = CAR_IS_RING|CAR_IS_BACKUP;
		break;
	case 13:
		bFlags = CAR_IS_SAFE_RING|CAR_IS_BACKUP;
		break;
	case 14:
		bFlags = CAR_IS_SEARCH|CAR_IS_BACKUP;
		break;
	case 15:
		bFlags = CAR_IS_BACKUP;
		break;
	case 16:
		bFlags = CAR_IS_SUSPECT|CAR_IS_BACKUP;
		break;
	case 17:
		bFlags = CAR_IS_ALARM_LIST|CAR_IS_BACKUP;
		break;
	default:
		WK_TRACE_ERROR(_T("unkown sequence nr type %d\n"), wNr);
		break;
	}

	return bFlags;
}
//////////////////////////////////////////////////////////////////////
WORD CTape::Flags2Nr(BYTE bFlags)
{
	WORD wNr = 0;

	switch(bFlags)
	{
	case CAR_IS_ALARM:
		wNr = 1;
		break;
	case CAR_IS_RING:
		wNr = 2;
		break;
	case CAR_IS_SAFE_RING:
		wNr = 3;
		break;
	case CAR_IS_SEARCH:
		wNr = 4;
		break;
	case CAR_IS_BACKUP:
		wNr = 5;
		break;
	case CAR_IS_SUSPECT:
		wNr = 6;
		break;
	case CAR_IS_ALARM_LIST:
		wNr = 7;
		break;
	case CAR_IS_ALARM|CAR_IS_BACKUP:
		wNr = 11;
		break;
	case CAR_IS_RING|CAR_IS_BACKUP:
		wNr = 12;
		break;
	case CAR_IS_SAFE_RING|CAR_IS_BACKUP:
		wNr = 13;
		break;
	case CAR_IS_SEARCH|CAR_IS_BACKUP:
		wNr = 14;
		break;
	case CAR_IS_SUSPECT|CAR_IS_BACKUP:
		wNr = 16;
		break;
	case CAR_IS_ALARM_LIST|CAR_IS_BACKUP:
		wNr = 17;
		break;
	default:
		WK_TRACE_ERROR(_T("unkown sequence flag type %02x\n"), bFlags);
		break;
	}

	return wNr;
}
//////////////////////////////////////////////////////////////////////
BOOL CTape::Scan(const CFindData* pDbf, 
				 const CFindData* pDat, 
				 const CString& sFolder, 
				 const CVDBSequenceMap& map)
{
	BOOL bRet = FALSE;

	CString sDbf,sDat;

	if (pDbf)
	{
		sDbf = pDbf->cFileName;
	}
	if (pDat)
	{
		sDat = pDat->cFileName;
	}
	
	int p = sDbf.Find(_T('.'));

	CString sID(sDbf.Left(p));
	CString sTyp(sID.Left(4));

	sID = sID.Mid(4);

	DWORD dw = 0;
	_stscanf(sTyp, _T("%x"), &dw);
	m_bFlags = Nr2Flags((WORD)dw);

	dw = 0;
	_stscanf(sID, _T("%x"), &dw);
	m_wOriginalID = (WORD)dw;

	if (!sFolder.IsEmpty())
	{
		m_sFolder = sFolder;
	}

	if (m_dwClusterSize>0)
	{
		m_dwDbfSize = RoundUp(pDbf->nFileSizeLow, m_dwClusterSize);
	}
	else
	{
		m_dwDbfSize = pDbf->nFileSizeLow;
	}
	if (pDat)
	{
		sDat.MakeLower();
		if (-1!=sDat.Find(_T(".dat")))
		{
			if (m_dwClusterSize>0)
			{
				m_dwDatSize = RoundUp(pDat->nFileSizeLow,m_dwClusterSize);
			}
			else
			{
				m_dwDatSize = pDat->nFileSizeLow;
			}
		}
	}

	CVDBSequenceHashEntry* pSequenceHashEntry = map.GetSequenceHashEntry(GetArchiveNr(), m_wOriginalID);
	if (  pSequenceHashEntry
		&& pSequenceHashEntry->GetArchiveNr()  == GetArchiveNr()
		&& pSequenceHashEntry->GetSequenceNr() == m_wOriginalID
		&& pSequenceHashEntry->GetRecords()>0)
	{
//		TRACE(_T("read sequence data from sequence map %04x,%04x\n"), GetArchiveNr(), m_wOriginalID);
		m_stFirst = pSequenceHashEntry->GetFirst();
		m_stLast = pSequenceHashEntry->GetLast();
		m_dwNumRecords = pSequenceHashEntry->GetRecords();
		bRet = TRUE;
	}
	else
	{
		if (ScanDbf())
		{
			SetSequenceHashEntry();
			bRet = TRUE;
		}
		else
		{
			CWK_String sDbf = GetFileName(szDbf);
			WK_TRACE_ERROR(_T("scan failed %s\n"), LPCTSTR(sDbf));
		}
	}

	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CTape::ReadTime(BOOL bFirst)
{
	BOOL b1,b2;
	b1 = b2 = FALSE;

	CSystemTime& st = bFirst ? m_stFirst : m_stLast;

	if (IsOpenDbf())
	{
		if (m_dwNumRecords>0)
		{
			int ret = bFirst ? m_pData4->top() : m_pData4->bottom();
			
			if (ret==r4success)
			{
				Field4 date(*m_pData4, DVR_DATE);
				if (date.isValid())
				{
#ifdef _UNICODE
					b1 = st.SetDBDate(CWK_String(date.str()));
#else
					b1 = st.SetDBDate(date.str());
#endif

					Field4 time(*m_pData4, DVR_TIME);
					if (time.isValid())
					{
#ifdef _UNICODE
						b2 = st.SetDBTime(CWK_String(time.str()));
#else
						b2 = st.SetDBTime(time.str());
#endif
						if (!IsAlarmList())
						{
							Field4 ms(*m_pData4,DVR_MS);
							if (ms.isValid())
							{
								DWORD dwMilliseconds;
								sscanf(ms.str(),"%03d",&dwMilliseconds);
								st.wMilliseconds = (WORD)dwMilliseconds;
							}
							else
							{
								WK_TRACE_ERROR(_T("millisecond field invalid\n"));
								errorSet();
							}
						}
					}
					else
					{
						errorSet();
					}
				}
				else
				{
					errorSet();
				}
			}
			else
			{
				WK_TRACE_ERROR(_T("top failed C=%d T=%d, %d\n"), GetArchiveNr(),
					GetNr(), ret);
				errorSet();
			}
		}
	}
	return b1 && b2;
}
//////////////////////////////////////////////////////////////////////
BOOL CTape::ScanDbf()
{
	BOOL bRet = FALSE;

	if (OpenDbf())
	{
		m_dwNumRecords = m_pData4->recCount();
		bRet = ReadTime(TRUE);
		bRet = bRet && ReadTime(FALSE);
		CloseDbf();
	}
	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CTape::CreateDbf()
{
	if ((m_pData4==NULL) && (m_dwNumRecords == 0))
	{
		CWK_String sDbf;
		sDbf = GetFileName(szDbf);

		m_pData4 = new Data4();

		if (theApp.m_bTraceOpen)
		{
			WK_TRACE(_T("create %s\n"), LPCTSTR(sDbf));
		}
		int ret = m_pData4->create(m_pCollection->m_CodeBase, sDbf, m_pCollection->m_FieldInfo);

		if (r4success == ret)
		{
			if (theApp.m_bTraceOpen)
			{
				WK_TRACE(_T("created %s\n"), LPCTSTR(sDbf));
			}
			m_iOpenDbf++;
			InitFields();
			m_dwDbfSize = m_pData4->numFields() * 32 + 34;
		}
		else
		{
			WK_TRACE(_T("cannot create %s, %d\n"), LPCTSTR(sDbf), ret);
			errorSet();
			WK_DELETE(m_pData4);
			theApp.OnError();
		}
	}
	return IsOpenDbf();
}
//////////////////////////////////////////////////////////////////////
BOOL CTape::CreateDat()
{
	BOOL bRet = TRUE;

	if (!m_pCollection->IsAlarmList())
	{
		CWK_String sDat;
		CFileException cfe;
		sDat = GetFileName(szDat);
		if (theApp.m_bTraceOpen)
		{
			WK_TRACE(_T("create %s\n"), LPCTSTR(sDat));
		}
		if (m_fImages.Open(sDat, CFile::modeCreate|CFile::modeReadWrite, &cfe))
		{
			if (theApp.m_bTraceOpen)
			{
				WK_TRACE(_T("created %s\n"), LPCTSTR(sDat));
			}
			m_iOpenDat++;
			m_dwDatSize = 0;
			m_dwDatWritePosition = 0;
			if (theApp.GetNoFrag())
			{
				TRY
				{
					m_fImages.SetLength(m_pCollection->GetTapeSizeBytes());
				}
				CATCH(CFileException, pCfe)
				{
					WK_TRACE_ERROR(_T("cannot set length %s , %s\n"),
						GetFileName(szDat), GetLastErrorString( pCfe->m_lOsError));
				}
				END_CATCH
			}
		}
		else
		{
			WK_TRACE(_T("cannot create %s because %s\n"), LPCTSTR(sDat),
				GetLastErrorString(cfe.m_lOsError));
			bRet = FALSE;
		}
	}
	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CTape::CreateDir()
{
	BOOL bRet = TRUE;
	CString sDir,sDbf;
	sDir = GetDirectory();
	
	if (!DoesFileExist(sDir))
	{
		if (CreateDirectory(sDir, NULL))
		{
			WK_TRACE(_T("create directory %s\n"), sDir);
			m_pCollection->NewFolder(sDir);
		}
		else
		{
			WK_TRACE_ERROR(_T("cannot create directory %s, %s"), sDir, GetLastErrorString());
			bRet = FALSE;
		}
	}
	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CTape::Create()
{
	Lock(_T("S1"));

	BOOL bRet = CreateDir();

	if (bRet)
	{
		bRet = CreateDbf();
	}

	if (   bRet 
		&& !m_pCollection->IsAlarmList())
	{
		bRet = CreateDat();
	}

	if (bRet)
	{
		int ret = m_pData4->flush();
		if (ret==r4success)
		{
			m_dwSavedWithoutFlush = 0;
		}
		else
		{
			WK_TRACE_ERROR(_T("flush failed C=%d T=%d, %d\n"),
				GetArchiveNr(), GetNr(), ret);
			errorSet();
		}
	}
	Unlock();
	return bRet;
}
//////////////////////////////////////////////////////////////////////
void CTape::InitFields()
{
	if (m_pData4)
	{
		int c,ret;
		short i;

		c = m_pData4->numFields();
		m_dwFieldSize = 0;
		for (i=1; i<=c; i++)
		{
			Field4 f;
			ret = f.init(*m_pData4,i);
			if (ret == r4success)
			{
				m_dwFieldSize += f.len();
			}
			else
			{
				WK_TRACE_ERROR(_T("InitFields, Field4::init failed C=%d T=%d, %d\n"),
					GetArchiveNr(), GetNr(), ret);
				errorSet();
			}
		}
		// delimiter
		m_dwFieldSize++;
	}
}
/////////////////////////////////////////////////////////////////////
BOOL CTape::AreFieldsEqual()
{
	BOOL bRet = TRUE;

	Lock(_T("S17"));

	if (OpenDbf())
	{
		int i,j,c,d;
		Field4info fields(*m_pData4);

		c = m_pCollection->m_FieldInfo.numFields();
		d = fields.numFields();

		if (c == d)
		{
			int f = 0;
			// compare the fields
			for (i=0;i<c && bRet ;i++)
			{
				const FIELD4INFO* pInfoC = m_pCollection->m_FieldInfo[i];
				for (j=0;j<d && bRet ;j++)
				{
					const FIELD4INFO* pInfoD = fields[j];

					if (0 == strcmp(pInfoC->name, pInfoD->name))
					{
						// found
//						TRACE(_T("comparing %s\n"), LPCTSTR(CWK_String(pInfoC->name)));
						f++;
						bRet &= (pInfoC->len == pInfoD->len);
						bRet &= (pInfoC->type == pInfoD->type);
						bRet &= (pInfoC->dec == pInfoD->dec);
						bRet &= (pInfoC->nulls == pInfoD->nulls);
						break;
					}
				}
			}

			bRet &= (f==c);
		}
		else
		{
			WK_TRACE(_T("number of fields not equal\n"));
			bRet = FALSE;
		}
		CloseDbf();
	}
	else
	{
		// cannot open dbf
	}


	Unlock();

	return bRet;
}
static TCHAR BASED_CODE szFileFormat[] = _T("%s%s\\%04hx%04hx.%s");
//////////////////////////////////////////////////////////////////////
CString CTape::GetFileName(const CString& ext) const
{
	// Beispiel d:\dvs\arch0001\00000001.dbf
	// m_sDrive = d:\
	// m_sFolder = dvs\arch0001
	// m_Type = 0000
	// m_wOriginalID = 0001
	// ext = dbf

	WORD wNr = Flags2Nr(m_bFlags);

	CString r;
	r.Format(szFileFormat,
			 m_sDrive,
			 m_sFolder,
			 (WORD)wNr,
			 (WORD)m_wOriginalID,
			 ext);
	return r;
}
//////////////////////////////////////////////////////////////////////
CString CTape::GetDirectory() const
{
	// Beispiel d:\dvs\arch0001

	CString r = GetFileName(szDbf);
	int p = r.ReverseFind(_T('\\'));
	r = r.Left(p);
	return r;
}
//////////////////////////////////////////////////////////////////////
BOOL CTape::OpenDbf()
{
	Lock(_T("S2"));
	
	m_iOpenDbf++;
	
	if (!IsOpenDbf())
	{

//		CString sDbf;
		CWK_String sDbf;
		int r;
		sDbf = GetFileName(szDbf);

		m_pData4 = new Data4();
		if (theApp.m_bTraceOpen)
		{
			WK_TRACE(_T("open %s\n"), LPCTSTR(sDbf));
		}
		r = m_pData4->open(m_pCollection->m_CodeBase, sDbf);
		if (r4success == r)
		{
			// success
			if (theApp.m_bTraceOpen)
			{
				WK_TRACE(_T("opened %s\n"), LPCTSTR(sDbf));
			}
			InitFields();
			m_dwNumRecords = m_pData4->recCount();
			if (   !IsAlarmList()
				&& !IsBackup())
			{
				ReadReferences();
			}
		}
		else
		{
			m_iOpenDbf = 0;
			WK_TRACE_ERROR(_T("cannot open %s, %d\n"), LPCTSTR(sDbf), r);
			errorSet();
			WK_DELETE(m_pData4);
			
			if (!DoesFileExist(sDbf))
			{
				WK_TRACE_ERROR(_T("file doesn't exist <%s>\n"), LPCTSTR(sDbf));
			}
			else if (!IsBackup())
			{
				if (    r==e4data 
					&& !m_bLockedByBackup)
				{
					// file exists, so try to repair
					WK_TRACE_ERROR(_T("open failed %s\n"), LPCTSTR(sDbf));
					WK_TRACE(_T("repairing %s\n"), LPCTSTR(sDbf));
					if (0 == repair(m_pCollection->m_CodeBase, sDbf))
					{
						WK_TRACE(_T("repair success %s\n"), sDbf);
						m_pData4 = new Data4();
						r = m_pData4->open(m_pCollection->m_CodeBase, sDbf);
						if (r4success == r)
						{
							// success
							m_iOpenDbf++;
							InitFields();
							m_dwNumRecords = m_pData4->recCount();
						}
						else
						{
							WK_TRACE_ERROR(_T("cannot open after repair %s, %d\n"), LPCTSTR(sDbf), r);
							errorSet();
							WK_DELETE(m_pData4);
						}
					}
					else
					{
						WK_TRACE_ERROR(_T("repair failed %s\n"), LPCTSTR(sDbf));
					}
				}
				else
				{
					WK_TRACE_ERROR(_T("no repair %s\n"), LPCTSTR(sDbf));
				}
			}
		}
	}
	Unlock();


	BOOL bRet = IsOpenDbf();
	if (!bRet)
	{
		m_iOpenDbf = 0;
		if (   !IsBackup()
			&& !m_bLockedByBackup)
		{
			theApp.OnError();
		}
	}
	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CTape::IsOpen()
{
	return    IsOpenDbf() 
		   && (   m_pCollection->IsAlarmList()
		       || IsOpenDat()
			   );
}
//////////////////////////////////////////////////////////////////////
BOOL CTape::IsOpenDat()
{
	Lock(_T("S20"));
#if _MFC_VER < 0x0700
	BOOL bRet = (m_fImages.m_hFile != (UINT)CFile::hFileNull);
#else
	BOOL bRet = (m_fImages.m_hFile != CFile::hFileNull);
#endif
	Unlock();
	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CTape::OpenDat()
{
	if (!m_pCollection->IsAlarmList())
	{
		Lock(_T("S3"));

		m_iOpenDat++;
		
		if (!IsOpenDat())
		{
			CFileException cfe;
			CWK_String sDat;
			UINT uFlags = 0;
			sDat = GetFileName(szDat);
			
			if (   IsBackup()
				|| theApp.IsReadOnly())
			{
				uFlags = CFile::modeRead;
			}
			else
			{
				uFlags = CFile::modeReadWrite|CFile::shareDenyNone;
			}
			
			if (theApp.m_bTraceOpen)
			{
				WK_TRACE(_T("open %s\n"), LPCTSTR(sDat));
			}
			if (m_fImages.Open(sDat, uFlags, &cfe))
			{
				if (theApp.m_bTraceOpen)
				{
					WK_TRACE(_T("opened %s\n"), LPCTSTR(sDat));
				}
				m_dwDatWritePosition = GetCurrentWritePosition();
			}
			else
			{
				m_iOpenDat = 0;
				m_dwDatWritePosition = 0;
				WK_TRACE(_T("cannot open %s, %s Flags %08lx\n"),
					LPCTSTR(sDat), GetLastErrorString(cfe.m_lOsError), uFlags);
			}
		}
		Unlock();
	}
	return IsOpenDat();
}
//////////////////////////////////////////////////////////////////////
BOOL CTape::Open()
{
	OpenDbf();
	if (!m_pCollection->IsAlarmList())
	{
		OpenDat();
	}
	return IsOpen();
}
//////////////////////////////////////////////////////////////////////
FILESIZE CTape::GetCurrentWritePosition()
{
	FILESIZE dwRet = 0;

	if (OpenDbf())
	{
		FILESIZE dwOffset = 0;
		DWORD dwLength = 0;
		GetOffsetAndLength(m_pData4->recCount(),dwOffset,dwLength);
		dwRet = dwOffset + dwLength;
		CloseDbf();
	}
	else
	{
		WK_TRACE(_T("cannot open dbf file to get current write position %s\n"), GetFileName(szDbf));
	}

	return dwRet;
}
//////////////////////////////////////////////////////////////////////
DWORD CTape::GetDWORDValue(const char* szFieldName, DWORD dwDefault)
{
	Field4 field;
	DWORD dwReturn = dwDefault;

	if (field.init(*m_pData4,szFieldName) == r4success)
	{
		sscanf(field.str(), SCAN_DWORD,&dwReturn);
	}
	else
	{
		errorSet();
	}
	return dwReturn;
}
//////////////////////////////////////////////////////////////////////
void CTape::ReadReferences()
{
	if (m_pData4->recCount()>0)
	{
		int ret;

		ret = m_pData4->bottom();
		if (ret == r4success)
		{
			m_dwLastVideo = GetDWORDValue(DBP_RWV_REF,0);
			m_dwLastAudio = GetDWORDValue(DBP_RWA_REF,0);
		}
		else
		{
			errorSet();
		}
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CTape::ScanSize(BOOL bRoundUp)
{
	CString sDbf,sDat;
	CFileStatus cfs;
	
	sDbf = GetFileName(szDbf);

	// dbf
	if (CFile::GetStatus(sDbf,cfs))
	{
		if (bRoundUp)
		{
			m_dwDbfSize = RoundUp(cfs.m_size,m_dwClusterSize);
		}
		else
		{
			m_dwDbfSize = cfs.m_size;
		}
	}
	else
	{
		m_dwDbfSize = 0;
		return FALSE;
	}

	// dat
	if (!m_pCollection->IsAlarmList())
	{
		sDat = GetFileName(szDat);
		if (CFile::GetStatus(sDat,cfs))
		{
			if (bRoundUp)
			{
				m_dwDatSize = RoundUp(cfs.m_size,m_dwClusterSize);
			}
			else
			{
				m_dwDatSize = cfs.m_size;
			}
		}

		if (m_dwDatSize == 0)
		{
			return FALSE;
		}
	}

	return TRUE;
}
//////////////////////////////////////////////////////////////////////
BOOL CTape::CloseDbf(BOOL bForce /*= FALSE*/)
{
	BOOL bRet = TRUE;
	
	Lock(_T("S4"));


	if (IsOpenDbf())
	{
		m_iOpenDbf--;

		if ((m_iOpenDbf==0) || bForce)
		{
			if (theApp.m_bTraceOpen)
			{
				WK_TRACE(_T("close %s\n"), LPCTSTR(CWK_String(m_pData4->fileName())));
			}
			int ret = m_pData4->close();
			if (ret == r4success)
			{
				if (theApp.m_bTraceOpen)
				{
					WK_TRACE(_T("closed %s\n"), GetFileName(szDbf));
				}
				bRet = TRUE;
			}
			else
			{
				WK_TRACE_ERROR(_T("Data4::close failed C=%hx T=%d, %d\n"),
					GetArchiveNr(),GetNr(),ret);
				errorSet();
				bRet = FALSE;
			}
			WK_DELETE(m_pData4);
			m_iOpenDbf = 0;
		}
		else
		{
			bRet = TRUE;
		}
	}

	Unlock();

	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CTape::CloseDat(BOOL bForce /*= FALSE*/)
{
	BOOL bRet = TRUE;

	if (!m_pCollection->IsAlarmList())
	{
		Lock(_T("S5"));

		m_iOpenDat--;

		if (IsOpenDat())
		{

			if ((m_iOpenDat==0) || bForce)
			{
				TRY
				{
					if (theApp.m_bTraceOpen)
					{
						WK_TRACE(_T("close %s\n"), m_fImages.GetFileName());
					}
					if (!IsBackup())
					{
						m_fImages.Flush();
					}
					m_fImages.Close();
					m_dwDatWritePosition = 0;
					if (theApp.m_bTraceOpen)
					{
						WK_TRACE(_T("closed %s\n"), GetFileName(szDat));
					}
				}
				CATCH(CFileException,cfe)
				{
					bRet = FALSE;
				}
				END_CATCH
				m_iOpenDat = 0;
			}
			else
			{
				bRet = TRUE;
			}
		}

		Unlock();
	}

	return bRet;
}
//////////////////////////////////////////////////////////////////////
void CTape::SetSequenceHashEntry()
{
	if (!IsBackup())
	{
		if (IsOpenDbf())
		{
			ReadTime(FALSE);
			m_dwNumRecords = m_pData4->recCount();
		}
		theApp.GetSequenceMap().SetSequenceHashEntry(GetArchiveNr(),
			GetNr(),m_dwNumRecords,m_stFirst,m_stLast,_T(""));
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CTape::Close(BOOL bForce /*= FALSE*/)
{
	BOOL bRet = TRUE;

	if (   bForce
		&& IsOpenDbf()
		&& m_iOpenDbf==1
		&& !m_bLockedByBackup)
	{
		SetSequenceHashEntry();
	}
	bRet &= CloseDbf(bForce);
	if (!m_pCollection->IsAlarmList())
	{
		bRet &= CloseDat(bForce);
	}
	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CTape::Delete()
{
	BOOL bRet = FALSE;

	TRACE(_T("Delete Sequence %d of Archiv %04hx\n"),GetNr(),m_pCollection->GetNr());

	if (!IsBackup())
	{
		Lock(_T("S6"));

		Close(TRUE);

//		TRACE(_T("DeleteFile(%s)\n"),GetFileName(szDbf));
		if (DeleteFile(GetFileName(szDbf)))
		{
			if (m_pCollection->IsAlarmList())
			{
				bRet = TRUE;
			}
			else
			{
//				TRACE(_T("DeleteFile(%s)\n"),GetFileName(szDat));
				if (DeleteFile(GetFileName(szDat)))
				{
					bRet = TRUE;
				}
				else
				{
					WK_TRACE_ERROR(_T("cannot delete %s, %s\n"),GetFileName(szDat),
						GetLastErrorString());
				}
			}
		}
		else
		{
			WK_TRACE_ERROR(_T("cannot delete %s, %s\n"),GetFileName(szDbf),
				GetLastErrorString());
		}

		// only try it, may be it's empty
/*
		CString sDir = GetDirectory();
		TRACE(_T("RemoveDirectory(%s)\n"),sDir);
		if (RemoveDirectory(sDir))
		{
			WK_TRACE(_T("removed directory %s\n"),sDir);
			m_pCollection->DeleteFolder(sDir);
		}
*/
		
		Unlock();
		theApp.GetSequenceMap().DeleteSequenceHashEntry(m_pCollection->GetNr(),GetNr());
	}
	else
	{
		WK_TRACE_ERROR(_T("cannot delete RO tape %s\n"),Format());
	}

	return bRet;
}
//////////////////////////////////////////////////////////////////////
void CTape::Flush()
{
	if (IsOpen())
	{
		if (m_dwSavedWithoutFlush>100)
		{
			int ret = m_pData4->flush();
			if (ret != r4success)
			{
				WK_TRACE_ERROR(_T("Data4::flush failed C=%d T=%d, %d\n"),
					GetArchiveNr(),GetNr(),ret);
				errorSet();
			}
			else
			{
//				TRACE(_T("open dbf=%d dat=%d\n"),m_iOpenDbf,m_iOpenDat);
			}

			if (IsOpenDat())
			{
				TRY
				{
					m_fImages.Flush();
				}
				CATCH(CFileException, pCfe)
				{
					WK_TRACE_ERROR(_T("cannot flush %s , %s\n"),
						GetFileName(szDat),GetLastErrorString(pCfe->m_lOsError));
				}
				END_CATCH
			}

			m_dwSavedWithoutFlush = 0;
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CTape::AfterAppend(DWORD dwPictureLength,const CSystemTime& stStart, const CSystemTime& stEnd,DWORD dwNumPics)
{
	if (m_dwNumRecords==0)
	{
		// first picture set time stamp
		m_stFirst = stStart;
	}
	m_dwNumRecords += dwNumPics;
	m_dwDbfSize += m_dwFieldSize*dwNumPics;
	m_dwDatSize += dwPictureLength;
	m_stLast = stEnd;

	Flush();
}
//////////////////////////////////////////////////////////////////////
BYTE CTape::CalcCheckSum(const BYTE* pData, DWORD dwDataLen)
{
	BYTE bCheckSum = 0;
	for (DWORD a=0; a<dwDataLen; a++)
	{
		bCheckSum ^= pData[a];
	}
	return bCheckSum;
}
//////////////////////////////////////////////////////////////////////
BOOL CTape::ReadDWORD(Data4& data, Code4& code_base,
						  const CString& name,DWORD& dwValue)
{
	// Achtung alles nicht UNICODE sondern ASCII und das ist auch OK so
	Field4 f;
	int ret;
	ret = f.init(data,CWK_String(name));
	if (ret == r4success)
	{
		if (f.len()>=8)
		{
			DWORD dwScan = 0;

			if (1==sscanf(f.str(),"%08x",&dwScan))
			{
				dwValue = dwScan;
				return TRUE;
			}
			else
			{
				WK_TRACE_ERROR(_T("cannot scan dword\n"));
			}
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		WK_TRACE_ERROR(_T("Field4::read failed %s %d\n"),
			name,ret);
		code_base.errorSet(0);
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CTape::AssignDWORD(Data4& data, Code4& code_base,
							const CString& name,const DWORD dwValue)
{
	// Achtung alles nicht UNICODE sondern ASCII und das ist auch OK so
	Field4 f;
	int ret;
	ret = f.init(data,CWK_String(name));
	if (ret == r4success)
	{
		if (f.len()>=8)
		{
			char szBuffer[9];
			sprintf(szBuffer,"%08x",dwValue);
			szBuffer[8] = '\0';
			ret = f.assign(szBuffer);
			if (ret == r4success)
			{
				return TRUE;
			}
			else
			{
				WK_TRACE_ERROR(_T("Field4::assign failed <%s>=<%s> %d\n"),
					name,szBuffer,ret);
				code_base.errorSet(0);
			}
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		WK_TRACE_ERROR(_T("Field4::init failed %s %d\n"),
			name,ret);
		code_base.errorSet(0);
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CTape::AssignField(const char* name, const char* value)
{
	Field4 f;
	int ret;
	ret = f.init(*m_pData4, name);
	if (ret == r4success)
	{
		ret = f.assign(value);
		if (ret == r4success)
		{
			return TRUE;
		}
		else
		{
			ASSERT(FALSE);
			WK_TRACE_ERROR(_T("Field4::assign failed <%s>=<%s> C=%d, T=%d, %d\n"),
				LPCTSTR(CWK_String(name)), LPCTSTR(CWK_String(value)), GetArchiveNr(), GetNr(), ret);
			errorSet();
		}
	}
	else
	{
		WK_TRACE_ERROR(_T("Field4::init failed %s C=%d, T=%d, %d\n"),
			LPCTSTR(CWK_String(name)), GetArchiveNr(), GetNr(), ret);
		errorSet();
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CTape::Store(const CImageData& data,WORD wPrevSequenceNr,DWORD dwNrOfRecords)
{
	Lock(_T("S7"));
	
	if (m_bLockedByBackup)
	{
		Unlock();
		WK_TRACE(_T("cannot store sequence locked by backup %s\n"),GetFileName(szDbf));
		return FALSE;
	}

	if (  (m_dwLastVideo==0 && data.GetPictureRecord() && !data.IsIFrame())
		||(m_dwLastAudio==0 && data.GetMediaSampleRecord() && !data.IsIFrame())
		)
	{
		Unlock();
		return TRUE;
	}

	BOOL bRet = TRUE;

	if (!IsOpen())
	{
		Open();
	}

	if (IsOpen())
	{
		char szFmt[64];
		int ret = r4success;
		DWORD dwFFReferenceRecordVideo,dwFFReferenceRecordAudio,dwFFReferenceValue;
		DWORD dwNumPictures = 1;
		FILESIZE dwPosition = m_dwDatWritePosition;
		const CIPCPictureRecord* pPictureRecord = data.GetPictureRecord();
		const CIPCMediaSampleRecord* pMediaSampleRecord = data.GetMediaSampleRecord();
		const BYTE* pData = NULL;
		DWORD dwDataLength = 0;
		CSystemTime stStart,stEnd;

		if (   pPictureRecord
			&& pPictureRecord->GetPictureType()==SPT_GOP_PICTURE)
		{
			dwNumPictures = pPictureRecord->GetNumberOfPictures();
		}

		for (DWORD i=0;i<dwNumPictures;i++)
		{
			pData = 0;
			dwDataLength = 0;
			dwFFReferenceRecordAudio = 0;
			dwFFReferenceRecordVideo = 0;
			dwFFReferenceValue  = 0;
		
			ret = m_pData4->appendStart();
			if (ret == r4success)
			{
				CIPCSavePictureType spt = SPT_FULL_PICTURE;
				CSystemTime t;
				m_pData4->blank();

				// version
				AssignField(DVR_VERSION, "11");

				if (pPictureRecord)
				{
					t = pPictureRecord->GetTimeStamp(i);
				}
				else if (pMediaSampleRecord)
				{
					t = data.GetTimeStamp();
				}
				if (i==0)
				{
					stStart = t;
				}
				if (i==dwNumPictures-1)
				{
					stEnd = t;
				}
				// date
				sprintf(szFmt,"%04d%02d%02d\0",t.wYear,t.wMonth,t.wDay);
				AssignField(DVR_DATE,szFmt);
				// time
				sprintf(szFmt,"%02d%02d%02d",t.wHour,t.wMinute,t.wSecond);
				AssignField(DVR_TIME,szFmt);
				if (!IsAlarmList())
				{
					// ms
					sprintf(szFmt,"%03d\0",t.wMilliseconds);
					AssignField(DVR_MS,szFmt);
					// cam id
					sprintf(szFmt, szDWORD, data.GetDataID());
					AssignField(DVR_CAMERANR, szFmt);
				}

				if (pPictureRecord)
				{
					// compression
					sprintf(szFmt, szIntFormat, (WORD)data.GetPictureRecord()->GetCompression());
					AssignField(DVR_COMPRESSION, szFmt);

					// resolution
					sprintf(szFmt, szIntFormat, (BYTE)data.GetPictureRecord()->GetResolution());
					AssignField(DVR_RESOLUTION, szFmt);

					// compressiontyp
					sprintf(szFmt, szIntFormat, (BYTE)data.GetPictureRecord()->GetCompressionType());
					AssignField(DVR_COMPRESSIONTYP, szFmt);

					// picturetyp
					// picturetyp
					spt = pPictureRecord->GetPictureType();
					if (spt == SPT_GOP_PICTURE)
					{
						spt = (i==0) ? SPT_FULL_PICTURE : SPT_DIFF_PICTURE;
					}
					sprintf(szFmt, szIntFormat, (BYTE)spt);
					AssignField(DVR_TYP, szFmt);
				}
				else if (pMediaSampleRecord)
				{
					spt = data.GetMediaSampleRecord()->IsLongHeader() ? SPT_FULL_PICTURE : SPT_DIFF_PICTURE;
					sprintf(szFmt, szIntFormat, (BYTE)spt);
					AssignField(DVR_TYP, szFmt);
					AssignField(DVR_COMPRESSIONTYP,szMM);
					
					sprintf(szFmt, szIntFormat, (BYTE)0);
					AssignField(DVR_RESOLUTION, szFmt);
	//				s.Format(szIntFormat,(WORD)0);
					AssignField(DVR_COMPRESSION, szFmt);
				}
				else
				{
					CIPCField* pTime = data.GetFields().GetCIPCField(_T(DVR_TIME));
					CIPCField* pDate = data.GetFields().GetCIPCField(_T(DVR_DATE));

					if (pTime)
					{
						stStart.SetDBTime(pTime->GetValue());
					}
					if (pDate)
					{
						stStart.SetDBDate(pDate->GetValue());
					}
					stEnd = stStart;
				}

			
				if (!IsAlarmList())
				{
					// last video reference
					if (   data.GetPictureRecord()
						&& spt == SPT_FULL_PICTURE)
					{
						dwFFReferenceRecordVideo = m_dwLastVideo;
						m_dwLastVideo = m_dwNumRecords+1;
						dwFFReferenceValue = m_dwLastVideo;
					}
					// last audio reference
					if (   data.GetMediaSampleRecord()
						&& spt == SPT_FULL_PICTURE)
					{
						dwFFReferenceRecordAudio = m_dwLastAudio;
						m_dwLastAudio = m_dwNumRecords+1;
						dwFFReferenceValue = m_dwLastAudio;
					}
					AssignRWReferences(m_dwLastVideo,m_dwLastAudio);
					
					if (pPictureRecord)
					{
						pData = pPictureRecord->GetData(i);
						dwDataLength = pPictureRecord->GetDataLength(i);
					}
					else if (pMediaSampleRecord)
					{
						pData = (const BYTE*)pMediaSampleRecord->GetBubble()->GetAddress();
						dwDataLength   = pMediaSampleRecord->GetBubble()->GetLength();
					}

					BYTE bCheckSum = 0;

					if (pData)
					{
						bCheckSum = CalcCheckSum(pData,dwDataLength);
						sprintf(szFmt, szDWORD, dwPosition);
						dwPosition += dwDataLength;
						AssignField(DVR_DATA_POS, szFmt);

						sprintf(szFmt, szDWORD, dwDataLength);
						AssignField(DVR_DATA_LEN, szFmt);

						sprintf(szFmt, szBYTE, bCheckSum);
						AssignField(DVR_CHECK_SUM, szFmt);
					}
				} // !IsAlarmList 

				// write data
				if (spt == SPT_FULL_PICTURE)
				{
					for (int j=0; j<data.GetFields().GetSize();j++)
					{
						CIPCField* pField = data.GetFields().GetAtFast(j);
						Field4 fData;
						CWK_String sName = pField->GetName();
						ret = fData.init(*m_pData4, sName);
						if (ret == r4success)
						{
							CWK_String sValue = pField->GetValue();
							if (pField->IsNumeric() && !pField->IsEmpty())
							{
								sValue = CIPCField::FillLeadingZeros(sValue, (WORD)fData.len());
							}
							if (sValue.GetLength()>(int)fData.len())
							{
								sValue = sValue.Left(fData.len());
							}
							ret = fData.assign(sValue);
							if (r4success!=ret)
							{
								WK_TRACE(_T("assign field failed <%s>=<%s>, %d\n"),
									pField->GetName(),
									pField->GetValue(), ret);
								errorSet();
							}
						}
						else
						{
							WK_TRACE(_T("invalid field <%s>, %d\n"), pField->GetName(),ret);
							errorSet();
						}
					}
				}
				
				ret = m_pData4->append();
				if (r4success==ret)
				{
					if (!IsAlarmList())
					{
						AssignFFReference(dwFFReferenceRecordVideo,DBP_FFV_REF,dwFFReferenceValue);
						AssignFFReference(dwFFReferenceRecordAudio,DBP_FFA_REF,dwFFReferenceValue);
					}
				}
				else
				{
					WK_TRACE_ERROR(_T("Data4::append failed A=%d, S=%d, %d\n"),
						GetArchiveNr(),GetNr(),ret);
					bRet = FALSE;
					errorSet(0);
					break;
				}
			} // append start
			else
			{
				WK_TRACE_ERROR(_T("appendStart failed A=%d, S=%d, %d\n"),
					GetArchiveNr(),GetNr(),ret);
				bRet = FALSE;
				errorSet(0);
				break;
			}
		}	// for
		if (data.GetPictureRecord())
		{
			dwDataLength   = data.GetPictureRecord()->GetDataLength(-1);
		}
		else if (data.GetMediaSampleRecord())
		{
			dwDataLength   = data.GetMediaSampleRecord()->GetBubble()->GetLength();
		}
		if (r4success==ret)
		{
			AfterAppend(dwDataLength,stStart,stEnd,dwNumPictures);
			if (m_dwNumRecords == dwNumPictures)
			{
				theApp.m_Clients.DoIndicateNewTape(*this,wPrevSequenceNr,dwNrOfRecords);
			}
			bRet = TRUE;
		}
		if (   !IsAlarmList()
			&& ret == r4success)
		{
			const BYTE* pData = NULL;

			if (data.GetPictureRecord())
			{
				pData = data.GetPictureRecord()->GetData();
			}
			else if (data.GetMediaSampleRecord())
			{
				pData = (const BYTE*)data.GetMediaSampleRecord()->GetBubble()->GetAddress();
			}

			TRY
			{
				m_fImages.Seek(m_dwDatWritePosition,CFile::begin);
				m_fImages.Write(pData,dwDataLength);
				m_dwDatWritePosition += dwDataLength;
			}
			CATCH (CFileException, cfe)
			{
				bRet = FALSE;
				WK_TRACE(_T("cannot write picture %s %s\n"),
					GetFileName(szDat),
					GetLastErrorString(cfe->m_lOsError));
			}
			END_CATCH
		}	// write data
	} // IsOpen
	else
	{
		bRet = FALSE;
	}
	Unlock();
	if (bRet)
	{
		m_dwSavedWithoutFlush++;
	}
	return bRet;
}
//////////////////////////////////////////////////////////////////////
// die Rücksprung referenzen werden hier gesetzt
// es sind immer beide gesetzt
// eine 0 bedeutet, es gibt kein voriges Video/ Audio record
// z.B. beim ersten
void CTape::AssignRWReferences(DWORD dwValueVideo,DWORD dwValueAudio)
{
	char szFmt[64];
	if (dwValueVideo>0)
	{
		sprintf(szFmt, szDWORD, dwValueVideo);
		AssignField(DBP_RWV_REF, szFmt);
	}

	if (dwValueAudio)
	{
		sprintf(szFmt, szDWORD, dwValueAudio);
		AssignField(DBP_RWA_REF, szFmt);
	}
}
//////////////////////////////////////////////////////////////////////
// Die Vorwärts referenzen können immer nur gesetzt werden,
// wen dann auch das passende Record gespeichert wird, dazu
// ist ein Rücksprung in der Tabelle nötig.
void CTape::AssignFFReference(DWORD dwRecordNr, const char*sFieldName, DWORD dwValue)
{
	if (   dwRecordNr>0 && sFieldName && sFieldName[0] != 0)
	{
		DWORD dwCurrentRec = m_pData4->recNo();
		int ret = m_pData4->go(dwRecordNr);

		// auf record zurückspringen und reference setzen
		if (r4success == ret)
		{
			char szFmt[64];
			sprintf(szFmt, szDWORD, dwValue);
			AssignField(sFieldName, szFmt);
		}
		else
		{
			WK_TRACE_ERROR(_T("go failed C=%d, T=%d, %d\n"),GetArchiveNr(),GetNr(),ret);
			errorSet();
		}
		// vor wieder zum aktuellen record
		ret = m_pData4->go(dwCurrentRec);
		if (r4success != ret)
		{
			WK_TRACE_ERROR(_T("go failed C=%d, T=%d, %d\n"),GetArchiveNr(),GetNr(),ret);
			errorSet();
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CTape::Lock(const CString& sName)
{
	DWORD dwTick1 = GetTickCount();
	m_cs.Lock();
	DWORD dwTick2 = GetTimeSpan(dwTick1);
	if (dwTick2>DVS_TIMEOUT)
	{
		WK_TRACE(_T("%s lock in %d ms Thread %08lx , Archiv Nr %04hx, Sequence Nr %08lx\n"),
			sName,dwTick2,GetCurrentThreadId(),GetArchiveNr(),GetNr());
	}
}
//////////////////////////////////////////////////////////////////////
void CTape::Unlock()
{
	m_cs.Unlock();
}
//////////////////////////////////////////////////////////////////////
CString CTape::Format()
{
	CString s;
	
	Lock(_T("S9"));

	s.Format(_T("A=%04lx, S=%04lx, NrP=%d Date=%s Flags=%02x Size=%d"),
		m_pCollection->GetNr(),GetNr(),m_dwNumRecords,m_stFirst.ToString(),
		m_bFlags,
		GetSizeBytes());
	Unlock();
	return s;
}
//////////////////////////////////////////////////////////////////////
void CTape::Trace()
{
	WK_TRACE(_T("%s\n"),Format());
}
//////////////////////////////////////////////////////////////////////
DWORD CTape::GetIFrameRecordNr(DWORD dwRecordNr)
{
	Lock(_T("S17"));

	DWORD dwIFrame = dwRecordNr;
	int ret;
	CIPCSavePictureType eSavePictureType;

	// for delivering search results
	// still no i/p frame support
	Field4 fieldPictureType;
	Field4 fieldCompressionType;

	ret = fieldCompressionType.init(*m_pData4,DVR_COMPRESSIONTYP);
	if (ret == r4success)
	{
		CString sIReferenceFieldName(DBP_RWV_REF);

		// wenn das ein Audio record ist die andere Referenz benutzen.
		if ((strcmp(szMM, fieldCompressionType.str()) == 0))
		{
			sIReferenceFieldName = DBP_RWA_REF;
		}

		ret = fieldPictureType.init(*m_pData4,DVR_TYP);

		if (ret == r4success)
		{
			unsigned int iSPT;
			if (1==sscanf(fieldPictureType.str(), SCAN_INT, &iSPT))
			{
				eSavePictureType = (CIPCSavePictureType)iSPT;
				if (eSavePictureType==SPT_DIFF_PICTURE)
				{
					// it's a p frame so look if client want's to
					// scroll back in sequence
					// we have to deliver the previous i frame
					Field4 fieldIFrameReference;
					
					ret = fieldIFrameReference.init(*m_pData4,CWK_String(sIReferenceFieldName));
					if (ret != r4success)
					{
						errorSet();
						fieldIFrameReference.init(*m_pData4,DBP_IFRAME_REF);
					}
					
					if (fieldIFrameReference.isValid())
					{
						sscanf(fieldIFrameReference.str(), SCAN_DWORD,&dwIFrame);
					}
				}
			}
			else
			{
				WK_TRACE_ERROR(_T("cannot read picture type\n"));
			}
		}
		else
		{
			WK_TRACE_ERROR(_T("Field4::init failed A=%d S=%d, %d\n"),GetArchiveNr(),GetNr(),ret);
			errorSet();
		}
	}
	else
	{
		WK_TRACE_ERROR(_T("Field4::init failed A=%d S=%d, %d\n"),GetArchiveNr(),GetNr(),ret);
		errorSet(0);
	}
	Unlock();
	return dwIFrame;
}
//////////////////////////////////////////////////////////////////////////
DWORD CTape::GetSpecificFrameRecordNr(CSecID camID, DWORD dwRecordNr, BOOL bBackStep)
{
	Field4 fieldCompressionType;
	int ret =0;
	CompressionType ct;
	do 
	{
		ret = fieldCompressionType.init(*m_pData4, DVR_COMPRESSIONTYP);
		if (ret != r4success)
		{
			errorSet();
			break;
		}
		else if (   camID.IsMediaID() 
			     && (strcmp(szMM, fieldCompressionType.str()) == 0)
				 )
		{
			return dwRecordNr;
		}
		else if (   camID.IsOutputID() 
			     && 1==sscanf(fieldCompressionType.str(), SCAN_INT, (int*)&ct)
				 && IsBetween(ct, COMPRESSION_H263, COMPRESSION_PRESENCE))
		{
			return dwRecordNr;
		}

		if (bBackStep)
		{
			if (--dwRecordNr == 0)
			{
				break;
			}
		}
		else
		{
			if (++dwRecordNr > m_dwNumRecords)
			{
				break;
			}
		}
		ret = m_pData4->go(dwRecordNr);
	} while(ret == r4success);

	errorSet();

	return 0;
}
//////////////////////////////////////////////////////////////////////
DWORD CTape::GetTypedRecordNr(CSecID camID, DWORD dwRecordNr, BOOL bBackStep)
{
	DWORD dwReturn = dwRecordNr;
	CString sFieldName;
	Field4 field;
	Field4 fieldCompressionType;
	int ret = r4success;
	BOOL bIsMedia = FALSE;

	ret = fieldCompressionType.init(*m_pData4, DVR_COMPRESSIONTYP); 
	if (ret == r4success)
	{
		bIsMedia = strcmp(szMM, fieldCompressionType.str()) == 0;
	}

	if (   (camID.IsMediaID() && !bIsMedia)
		|| (camID.IsOutputID() && bIsMedia))
	{
		// falscher Typ suchen!
		dwReturn = 0;

		if (camID.IsMediaID())
		{
			ret = field.init(*m_pData4,bBackStep ? DBP_RWA_REF : DBP_FFA_REF);
		}
		else if (camID.IsOutputID())
		{
			ret = field.init(*m_pData4,bBackStep ? DBP_RWV_REF : DBP_FFV_REF);
		}

		if (r4success == ret)
		{
			if (1==sscanf(field.str(),"%08x",&dwReturn))
			{
				// alles OK referenz gefunden
			}
			else if (!bBackStep)
			{
				// vorwärts und string leer?
				CString sValue(field.str());
				sValue.TrimLeft();
				sValue.TrimRight();

				if (sValue.IsEmpty())
				{
					// dann mal zurück zur letzten Referenz und dann wieder vorwärts
					CString sFieldNameRW;
					if (camID.IsMediaID())
					{
						sFieldNameRW = DBP_RWA_REF;
					}
					else if (camID.IsOutputID())
					{
						sFieldNameRW = DBP_RWV_REF;
					}
					Field4 fieldRW;
					DWORD dwRW = 0;

					if (r4success == fieldRW.init(*m_pData4,CWK_String(sFieldNameRW)))
					{
						if (1==sscanf(fieldRW.str(),"%08x",&dwRW))
						{
							if (r4success == m_pData4->go(dwRW))
							{
								// Achtung das go hat das Feld mit umgesetzt
								if (1==sscanf(field.str(),"%08x",&dwReturn))
								{
									// alles OK referenz gefunden
								}
								else
								{
									dwReturn = 0;
								}
							}
							else
							{
								dwReturn = 0;
								errorSet();
							}
						}
					}
					else
					{
						dwReturn = 0;
						errorSet();
					}
				}
			}
		}
		else
		{
			dwReturn = 0;
			errorSet();
		}
	}
	else
	{
		// richtiger Typ
		dwReturn = dwRecordNr;
	}


	return dwReturn;
}
//////////////////////////////////////////////////////////////////////
BOOL CTape::ConfirmRecord(DWORD dwCurrentRecordNr,
						  const DWORD dwNewRecordNr,
						  CSecID camID,
						  CIPCDatabaseStorage* pCIPCDatabase)
{
	Lock(_T("S10"));
	// attention we are in CIPCDatabaseStorage thread
	BOOL bRet = FALSE;

//	TRACE(_T("RequestRecord S=%d %d,%d,%08lx\n"),GetNr(),dwCurrentRecordNr,dwNewRecordNr,camID.GetID());

	// ist es eine korrekte Record Nr im Bereich ?
	if (   (dwNewRecordNr>0)
		&& (dwNewRecordNr<=m_dwNumRecords)
		)
	{
		// vorwärts oder rückwärts ?
		BOOL bBackStep = (dwNewRecordNr < dwCurrentRecordNr);
		BOOL bIFrame = FALSE;

		// bei mehr als 2 Differenz vorwärts
		// oder bei rückwärts Sprüngen passendes Vollbild/Longheader suchen.
		if (   (dwNewRecordNr < dwCurrentRecordNr)
			|| (camID.GetID() == (DWORD)(-1-NAVIGATION_LAST)))
		{
			bIFrame = TRUE;
		}
		else
		{
			if (dwNewRecordNr-dwCurrentRecordNr>1)
			{
				bIFrame = TRUE;
			}
		}

		// Datei öffnen
		if (!IsOpen())
		{
			Open();
		}

		if (IsOpen())
		{
			int ret = m_pData4->go(dwNewRecordNr);
			DWORD dwRequestNr = dwNewRecordNr;

			if (r4success == ret)
			{
				if ( (   camID.IsOutputID() 
					  || camID.IsMediaID())
					&& camID.GetSubID() == SECID_NO_SUBID
					)
				{
					dwRequestNr = GetTypedRecordNr(camID, dwNewRecordNr, bBackStep);
					TRACE(_T("### TKR dwNewRecordNr: %d  dwRequestNr: %d\n"), dwNewRecordNr, dwRequestNr);
					if (dwRequestNr != 0)
					{
						TRACE(_T("typed data step    from %d to %d %d\n"), dwNewRecordNr, dwRequestNr,dwRequestNr-dwNewRecordNr);
					}
					else
					{
						dwRequestNr = GetSpecificFrameRecordNr(camID, dwNewRecordNr, bBackStep);
						TRACE(_T("specific data step from %d to %d\n"), dwNewRecordNr, dwRequestNr);
					}

					if (dwRequestNr == 0)
					{
						dwRequestNr = m_pData4->recNo();	// deliver also non specific data to confirm the request
						bIFrame = FALSE;					// do not search for I-Frames in this case, because
															// it may be the first or the last record and the point to
															// change to annother sequence
						TRACE(_T("### RKE ### didn´t find specific data from pos %d, curr. pos: %d, error %d\n"), dwNewRecordNr, dwRequestNr, ret);
						TRACE(_T("BackStep %d\n"), bBackStep);
					}
					
					ret = m_pData4->go(dwRequestNr);
					camID = SECID_NO_ID;	// set to no ID to avoid the selective play mode
					//TKR bIFrame = FALSE;		// no additional I Frame step		
				}
				else
				{
					dwRequestNr = dwNewRecordNr;
				}

			}
			else
			{
				WK_TRACE_ERROR(_T("Data4::go failed A=%d, S=%d, %d\n"),GetArchiveNr(),GetNr(),ret);
				errorSet(0);
			}
			if (r4success == ret)
			{
				BOOL bFound = FALSE;
				if (   camID.IsOutputID()
				    || camID.IsMediaID())
				{
					// play only specified cam
					while (!bFound)
					{
						Field4 camfield;
						if (r4success==camfield.init(*m_pData4,DVR_CAMERANR))
						{
							DWORD dwID;
							if (1==sscanf(camfield.str(),"%08x",&dwID))
							{
								if (dwID == camID.GetID())
								{
									bFound = TRUE;
								}
								else
								{
									ret = m_pData4->skip(bBackStep ? -1 : +1);
									if (   (ret == r4eof) 
										|| (ret == r4bof)
									   )
									{
										TRACE(_T("no more cam num matching record\n"));
										// end ot bottom of file reached
										// without finding the specified cam
										// indicate an error
										DWORD dwID = MAKELONG(GetNr(),m_pCollection->GetNr());
										pCIPCDatabase->DoIndicateError(dwID,
											camID.GetID(),CIPC_ERROR_INVALID_VALUE,
											1);
										bFound = FALSE;
										break; // leave the loop
									}
								}
							}
							else
							{
								bFound = TRUE;
							}
						}
						else
						{
							errorSet();
							WK_TRACE_ERROR(_T("invalid cam field\n"));
							bFound = TRUE;
						}
					}
				}
				else if (camID.GetID() == (DWORD)(-1-NAVIGATION_ALL))
				{
					if (bIFrame)
					{
						dwRequestNr = GetIFrameRecordNr(dwNewRecordNr);
						if (dwRequestNr!=dwNewRecordNr)
						{
							TRACE(_T("I Frame step from %d -> %d\n"),dwNewRecordNr,dwRequestNr);
							ret = m_pData4->go(dwRequestNr);
							camID = SECID_NO_ID;
						}
						else
						{
							TRACE(_T("### NO new request\n"));
						}
					}
					else
					{
						TRACE(_T("### NO I Frame step from %d -> %d\n"),dwNewRecordNr,dwRequestNr);
					}
					// no cam id, no skip
					bFound = TRUE;
				}
				else if (camID.GetID() == (DWORD)(-1-NAVIGATION_LAST))
				{
					TRACE(_T("last image S=%d\n"),GetNr());
					// last image
					if (r4success == m_pData4->bottom())
					{
						bFound = TRUE;
					}
					else
					{
						errorSet();
						WK_TRACE_ERROR(_T("bottom failed field\n"));
					}
					if (bIFrame)
					{
						dwRequestNr = GetIFrameRecordNr(dwNewRecordNr);
						if (dwRequestNr!=dwNewRecordNr)
						{
							TRACE(_T("I Frame step from %d -> %d\n"),dwNewRecordNr,dwRequestNr);
							ret = m_pData4->go(dwRequestNr);
							camID = SECID_NO_ID;
						}
						else
						{
							TRACE(_T("### NO new request\n"));
						}
					}
					else
					{
						TRACE(_T("### NO I Frame step from %d -> %d\n"),dwNewRecordNr,dwRequestNr);
					}

				}
				else if (camID.GetID() != (DWORD)(-1-NAVIGATION_ALL))
				{
					// search for 1,2,3 ...
					// skip to next DVD_MD_S value == _T('1'),_T('2'),_T('3') ...

					CWK_String s;
					s.Format(_T("%d"),((DWORD)-1) - camID.GetID());
					TRACE(_T("searching for S VALUE %s at %d\n"),s,m_pData4->recNo());
					
					while (!bFound)
					{
						Field4 mdsfield;
						if (r4success==mdsfield.init(*m_pData4,DVD_MD_S))
						{
							if (0 == s.CompareNoCase(CWK_String(mdsfield.str())))
							{
								TRACE(_T("found S VALUE %s at %d\n"),LPCTSTR(s), m_pData4->recNo());
								bFound = TRUE;
							}
							else
							{
								ret = m_pData4->skip(bBackStep ? -1 : +1);
								if (   (ret == r4eof) 
									|| (ret == r4bof)
								   )
								{
									TRACE(_T("no more matching S record\n"));
									// end ot bottom of file reached
									// without finding the specified md flag
									// indicate an error
									DWORD dwID = MAKELONG(GetNr(),
										m_pCollection->GetNr());
									pCIPCDatabase->DoIndicateError(dwID,
										camID.GetID(),CIPC_ERROR_INVALID_VALUE,
										bBackStep ? 6 : 5);
									bFound = FALSE;
									break; // leave the loop
								}
							}
						}
						else
						{
							errorSet();
							WK_TRACE_ERROR(_T("invalid md s field\n"));
							bFound = TRUE;
						}
					}
				}

				if (bFound)
				{
#ifdef _DEBUG
					CSystemTime st;
					st.GetLocalTime();
					//TRACE(_T("##### found   record: A=%04x, S=%d R=%d %s,%03d\n"),
					//	m_pCollection->GetNr(), m_wID,dwRequestNr,st.GetTime(),st.wMilliseconds);
#endif
					dwRequestNr = m_pData4->recNo();
					CIPCFields fields;
					CIPCPictureRecord* pPR = NULL;
					CIPCMediaSampleRecord* pMSR = NULL;
					
					// audio or video ? 
					if (GetRecord(dwRequestNr,
						 	      pCIPCDatabase,
								  FALSE,
								  pPR,pMSR,
								  fields))
					{
						if (dwNewRecordNr!=dwRequestNr)
						{
							TRACE(_T("ConfirmRecord S=%d RN%d!=NN%d\n"),GetNr(),dwRequestNr,dwNewRecordNr);
						}
						if (pPR)
						{
#ifdef _DEBUG
							CSystemTime st;
							st.GetLocalTime();
							TRACE(_T("##### confirm record: A=%04x, S=%d R=%d %s,%03d\n"),
								m_pCollection->GetNr(), m_wID,dwRequestNr,st.GetTime(),st.wMilliseconds);
#endif
							pCIPCDatabase->ConfirmRecordNr(m_pCollection->GetNr(),
															 m_wID,
															 dwRequestNr,
															 m_dwNumRecords,
															 *pPR,
															 fields);
							bRet = TRUE;
							WK_DELETE(pPR);
						}
						else if (pMSR)
						{
							pCIPCDatabase->DoConfirmRecordNr(m_pCollection->GetNr(),
															 m_wID,
															 dwRequestNr,
															 m_dwNumRecords,
															 *pMSR,
															 fields);
							bRet = TRUE;
							WK_DELETE(pMSR);
						}
						else
						{
							DWORD dwID = MAKELONG(GetNr(),m_pCollection->GetNr());
							pCIPCDatabase->DoIndicateError(dwID,
														   dwNewRecordNr,
														   CIPC_ERROR_INVALID_VALUE,
														   8);
							bRet = FALSE;
						}
					}
					else
					{
						DWORD dwID = MAKELONG(GetNr(),m_pCollection->GetNr());
						pCIPCDatabase->DoIndicateError(dwID,
													   dwNewRecordNr,
													   CIPC_ERROR_INVALID_VALUE,
													   8);
						bRet = FALSE;
					}
				}
			} // go
			else
			{
				errorSet();
				WK_TRACE_ERROR(_T("cannot goto record %d\n"),dwRequestNr);
			}

			// no close keep the file open for faster playback
		}
		else
		{
			WK_TRACE_ERROR(_T("cannot open sequence for getting picture\n"));
			DWORD dwID = MAKELONG(GetNr(),m_pCollection->GetNr());
			pCIPCDatabase->DoIndicateError(dwID,dwNewRecordNr,CIPC_ERROR_INVALID_VALUE,7);
		}
	}
	else
	{
		WK_TRACE_ERROR(_T("invalid record no %d / %d\n"),dwNewRecordNr,m_dwNumRecords);
		DWORD dwID = MAKELONG(GetNr(),m_pCollection->GetNr());
		pCIPCDatabase->DoIndicateError(dwID,dwNewRecordNr,CIPC_ERROR_INVALID_VALUE,2);
	}

	Unlock();
	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CTape::Move(CTape& source)
{
	BOOL bRet = TRUE;
	CString sf,df;

	source.Lock(_T("S12"));

	Lock(_T("S13"));

	m_bFlags = source.m_bFlags;
	WK_CreateDirectory(GetDirectory());

	// dbf
	sf = source.GetFileName(szDbf); 
	df = GetFileName(szDbf);
	WK_TRACE(_T("MOVE %s --> %s\n"),sf,df);
	if (!MoveFile(sf,df))
	{
		bRet = FALSE;
		WK_TRACE(_T("cannot move %s to %s because %s\n"),sf,df,GetLastErrorString());
	}

	// dat
	sf = source.GetFileName(szDat); 
	df = GetFileName(szDat);
	WK_TRACE(_T("MOVE %s --> %s\n"),sf,df);
	if (!MoveFile(sf,df))
	{
		bRet = FALSE;
		WK_TRACE(_T("cannot move %s to %s because %s\n"),sf,df,GetLastErrorString());
	}

	if (bRet)
	{
		m_dwNumRecords = source.m_dwNumRecords;
		m_stFirst = source.m_stFirst;
		m_stLast = source.m_stLast;
		m_dwFieldSize = source.m_dwFieldSize;
		m_dwDbfSize = source.m_dwDbfSize;
		m_dwDatSize = source.m_dwDatSize;
	}

	Unlock();
	source.Unlock();

	theApp.m_Clients.DoIndicateNewTape(*this,0,0);

	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CTape::GetRecord(DWORD dwRecNo, 
					  CIPCDatabaseStorage* pCIPCDatabase, 
					  BOOL bFieldsAtPictureRecord,
					  CIPCPictureRecord*& pPR,
					  CIPCMediaSampleRecord*& pMSR,
					  CIPCFields& fields)
{
	if (IsAlarmList())
	{
		return FALSE;
	}
	Lock(_T("S14"));

	pPR = NULL;
	pMSR = NULL;
	CSecID id;
	Resolution          res  = RESOLUTION_NONE;
	Compression	        comp = COMPRESSION_NONE;
	CompressionType     ct   = COMPRESSION_UNKNOWN;
	CIPCSavePictureType spt  = SPT_FULL_PICTURE;
	BOOL  bNewFormat         = FALSE;
	DWORD dwReadCheckSum	    = 0;
	DWORD dwLen       = 0;
	DWORD dwPos       = 0;
	PBYTE pBuffer     = NULL;
	int ret;
	CSystemTime st;
	BOOL bIsMM = FALSE;

	for (short i=1;i<=m_pData4->numFields();i++)
	{
		Field4 f;
		
		ret = f.init(*m_pData4,i);

		if (ret!=r4success)
		{
			WK_TRACE_ERROR(_T("CTape::GetRecord invalid field <%s>, %d\n"),
				LPCTSTR(CWK_String(f.name())), ret);
			errorSet();
			continue;
		}

		if (0==strcmp(f.name(), DVR_CAMERANR))
		{
			DWORD dwID;
			if (1==sscanf(f.str(), SCAN_DWORD, &dwID))
			{
				id = dwID;
			}
			else
			{
				WK_TRACE_ERROR(_T("cannot read camera nr >%s< %s\n"), LPCTSTR(CWK_String(f.str())), GetFileName(szDbf));
			}
		}
		else if (0 == strcmp(f.name(), DVR_RESOLUTION))
		{
			DWORD dwRes;
			if (1 == sscanf(f.str(), SCAN_INT, &dwRes))
			{
				res = (Resolution)dwRes;
			}
			else
			{
				WK_TRACE_ERROR(_T("cannot read resolution >%s< %s\n"), LPCTSTR(CWK_String(f.str())), GetFileName(szDbf));
			}
		}
		else if (0 == strcmp(f.name(), DVR_COMPRESSION))
		{
			DWORD dwComp;
			if (1 == sscanf(f.str(), SCAN_INT, &dwComp))
			{
				comp = (Compression)dwComp;
			}
			else
			{
				WK_TRACE_ERROR(_T("cannot read compression >%s< %s\n"), LPCTSTR(CWK_String(f.str())), GetFileName(szDbf));
			}
		}
		else if (0==strcmp(f.name(),DVR_COMPRESSIONTYP))
		{
			DWORD dwCT;
			if (0 == strcmp(f.str(), szMM))
			{
				// is a audio/multimedia bubble
				bIsMM = TRUE;
			}
			else
			{
				if (1==sscanf(f.str(), SCAN_INT, &dwCT))
				{
					ct = (CompressionType)dwCT;
				}
				else
				{
					WK_TRACE_ERROR(_T("cannot read compression typ\n"));
				}
			}
		}
		else if (0 == strcmp(f.name(), DVR_TYP))
		{
			DWORD dwSPT;
			if (1 == sscanf(f.str(), SCAN_INT, &dwSPT))
			{
				spt = (CIPCSavePictureType)dwSPT;
			}
			else
			{
				WK_TRACE_ERROR(_T("cannot read picture type\n >%s< %s\n"), LPCTSTR(CWK_String(f.str())), GetFileName(szDbf));
			}
		}
		else if (0 == strcmp(f.name(), DVR_DATE))
		{
#ifdef _UNICODE
			st.SetDBDate(CWK_String(f.str()));
#else
			st.SetDBDate(f.str());
#endif
		}
		else if (0 == strcmp(f.name(), DVR_TIME))
		{
#ifdef _UNICODE
			st.SetDBTime(CWK_String(f.str()));
#else
			st.SetDBTime(f.str());
#endif
		}
		else if (0 == strcmp(f.name(), DVR_MS))
		{
			DWORD ms = 0;
			if (1 == sscanf(f.str(), "%03d", &ms))
			{
				st.wMilliseconds = (WORD)ms;
			}
		}
		else if (0 == strcmp(f.name(), DVR_DATA_POS))
		{
			if (1 == sscanf(f.str(), SCAN_DWORD, &dwPos))
			{
			}
			else
			{
				WK_TRACE(_T("cannot read picture off >%s< %s\n"), LPCTSTR(CWK_String(f.str())), GetFileName(szDbf));
			}
		}
		else if (0 == strcmp(f.name(), DVR_DATA_LEN))
		{
			if (1 == sscanf(f.str(), SCAN_DWORD, &dwLen))
			{
			}
			else
			{
				WK_TRACE(_T("cannot read picture len >%s< %s\n"), LPCTSTR(CWK_String(f.str())), GetFileName(szDbf));
			}
		}
		else if (0 == strcmp(f.name(), DVR_CHECK_SUM))
		{
//			char szBuffer[3];
//			strcpy(szBuffer, f.str());
//			if (1==sscanf(szBuffer, "%x",&dwReadCheckSum))
			if (1==sscanf(f.str(), "%x", &dwReadCheckSum))
			{
				bNewFormat = TRUE;
			}
			else
			{
				WK_TRACE(_T("cannot scan check sum >%s< %s\n"), LPCTSTR(CWK_String(f.str())), GetFileName(szDbf));
			}
		}
		else
		{
			if (0 == strncmp("DVD_", f.name(), 4))
			{
#ifdef _UNICODE
				CWK_String sName(f.name());
				CWK_String sValue(f.str());
				WORD wLen  = (WORD)f.len();
				char cType = 'C';
#else
				CString sName;
				CString sValue;
				WORD wLen;
				char cType = 'C';
				sName  = f.name();
				sValue = f.str();
				wLen   = (WORD)f.len();
#endif
				if (!sName.IsEmpty() && !sValue.IsEmpty() && (wLen>0))
				{
					fields.SafeAdd(new CIPCField(sName,sValue,wLen,cType));
				}
			}
		}
	} // for

#ifdef _DEBUG
	CSystemTime std;
	std.GetLocalTime();
	//TRACE(_T("##### before  record: A=%04x, S=%d R=%d %s,%03d\n"),
	//	m_pCollection->GetNr(), m_wID,dwRecNo,std.GetTime(),std.wMilliseconds);
#endif

	BOOL bRead = TRUE;
	pBuffer = new BYTE[dwLen];
	TRY
	{
		/*
		if (dwRecNo == 1)
		{
			m_fImages.SeekToBegin();
		}*/
		m_fImages.Seek(dwPos,CFile::begin);
		m_fImages.Read(pBuffer,dwLen);
	}
	CATCH(CFileException, pCfe)
	{
		bRead = FALSE;
		WK_TRACE_ERROR(_T("cannot read picture %s, %s\n"),
			GetFileName(szDat), GetLastErrorString(pCfe->m_lOsError));
	}
	END_CATCH
#ifdef _DEBUG
		std.GetLocalTime();
	//TRACE(_T("##### after   record: A=%04x, S=%d R=%d %s,%03d\n"),
	//	m_pCollection->GetNr(), m_wID,dwRecNo,std.GetTime(),std.wMilliseconds);
#endif

	if (bRead)
	{
		if (bNewFormat)
		{
			BYTE bCalcCheckSum = CalcCheckSum(pBuffer,dwLen);

			if ((BYTE)dwReadCheckSum != bCalcCheckSum)
			{
				WK_TRACE_ERROR(_T("invalid image checksum %d, %d, %d\n"),
					GetArchiveNr(),GetNr(),dwRecNo);
				fields.Add(new CIPCField(_T(DVD_ERROR),_T("CHECKSUM"),'C'));
			}
		}

		if (bIsMM)
		{
			CIPCExtraMemory extraMem;
			
			if (extraMem.Create(pCIPCDatabase,dwLen,pBuffer))
			{
				pMSR = new CIPCMediaSampleRecord(id,extraMem);
			}
			else
			{
				WK_TRACE_ERROR(_T("cannot create media bubble\n"));
			}
		}
		else
		{
			pPR = new CIPCPictureRecord(pCIPCDatabase,
										pBuffer,
										dwLen,
										id,
										res, comp, ct, st,
										0, NULL, 0,
										spt);
		}
	}
	WK_DELETE(pBuffer);
	Unlock();
	
	return (pPR!=NULL) || (pMSR !=NULL);
}
//////////////////////////////////////////////////////////////////////
BOOL CTape::GetFields(DWORD dwRecordNr, CIPCFields& fields)
{
	Lock(_T("S15"));

	BOOL bRet = FALSE;

	if (OpenDbf())
	{
		if (r4success == m_pData4->go(dwRecordNr))
		{
			int c,ret;
			short i;
			c = m_pData4->numFields();
			for (i=1;i<=c;i++)
			{
				Field4 f;
				
				ret = f.init(*m_pData4,i);

				if (ret == r4success)
				{
					if (  (0 == strncmp("DBD_", f.name(), 4))
						|| (0 == strncmp("DVR_", f.name(), 4)) // Add by ML
						)
					{
#ifdef _UNICODE
						fields.SafeAdd(new CIPCField(CWK_String(f.name()), CWK_String(f.str()), (WORD)f.len(), 'C'));
#else
						fields.SafeAdd(new CIPCField(f.name(), f.str(), (WORD)f.len(), 'C'));
#endif
					}
				}
				else
				{
					WK_TRACE_ERROR(_T("Field4::init failed C=%d, T=%d, %d\n"),
						m_pCollection->GetNr(),GetNr(),ret);
					errorSet();
				}
			}
			bRet = TRUE;
		}
		else
		{
			errorSet();
		}
		CloseDbf();
	}

	Unlock();
	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CTape::GetFieldsAndRecords(DWORD dwFirstRecordNr,
								 DWORD dwLastRecordNr,
								 CIPCFields& fields,
								 CIPCFields& records,
								 const CSystemTime& stAfter)
{
	Lock(_T("S50"));

	if (dwFirstRecordNr == 0)
	{
		dwFirstRecordNr = 1;
	}
	if (   dwLastRecordNr == 0 
		|| dwLastRecordNr>m_dwNumRecords)
	{
		dwLastRecordNr = m_dwNumRecords;
	}

	if (OpenDbf())
	{
		CSystemTime t;

		if (t < stAfter)
		{
			dwFirstRecordNr = GetRecordNr(stAfter,TRUE);
		}


		for (DWORD dwRecordNr = dwFirstRecordNr;dwRecordNr<=dwLastRecordNr;dwRecordNr++)
		{
			if (r4success == m_pData4->go(dwRecordNr))
			{
				int c,ret;
				short i;
				BOOL bFields = fields.GetSize()==0;
				c = m_pData4->numFields();
				if (bFields)
				{
					for (i=1;i<=c;i++)
					{
						Field4 f;
						ret = f.init(*m_pData4,i);

						if (ret == r4success)
						{
							if (0 != strcmp(DVR_VERSION, f.name()))
							{
#ifdef _UNICODE
								fields.SafeAdd(new CIPCField(CWK_String(f.name()), _T(""), (WORD)f.len(), (char)f.type()));
#else
								fields.SafeAdd(new CIPCField(f.name(), _T(""), (WORD)f.len(), (char)f.type()));
#endif
							}
						}
						else
						{
							WK_TRACE_ERROR(_T("Field4::init failed C=%d, T=%d, %d\n"),
								m_pCollection->GetNr(),GetNr(),ret);
							errorSet();
						}
					} // loop over fields
				}
				CString sNr;
				sNr.Format(_T("%d"),dwRecordNr);
				CIPCField* pField = new CIPCField;
				pField->SetName(sNr);
				// skip leading version!
				CWK_String sValue(m_pData4->record()+3);
				UTRACE(_T("record %d=<%s> %d Bytes long\n"), dwRecordNr, LPCTSTR(sValue), sValue.GetLength());
				pField->SetValue(sValue);
				pField->SetMaxLength((WORD)sValue.GetLength());
				pField->SetType('R');
				records.SafeAdd(pField);
			} // go to record
			else
			{
				errorSet();
			}
		} // loop over records
		CloseDbf();
	}

	Unlock();

	return records.GetSize()>0;
}
//////////////////////////////////////////////////////////////////////
BOOL CTape::CalcDateTimeOpt(const CSystemTime& stStart,
							const CSystemTime& stEnd)
{
	BOOL bRet = TRUE;
	CSystemTime stEmpty;

	if (!(stEmpty == stStart))
	{
		if (m_stLast < stStart)
		{
			bRet = FALSE;
		}
	}
	if (!(stEmpty == stEnd))
	{
		if (m_stFirst > stEnd)
		{
			bRet = FALSE;
		}
	}

	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CTape::Search(CSearch& search)
{
	Lock(_T("S16"));

	BOOL bRet = TRUE;

	if (m_dwNumRecords>0)
	{
		// perform Time Check to get faster queries
		BOOL bMustSearch = CalcDateTimeOpt(search.GetStartTime(),
										   search.GetEndTime());

		if (bMustSearch)
		{
			if (OpenDbf())
			{
				Relate4set relation;
				int rc = 0;
				int ret = 0;
				DWORD dwQueryRecNo;

				ret = relation.init(*m_pData4);
				if (ret == r4success)
				{
					CWK_String str = search.GetExpression();
					rc = relation.querySet(str);
					if (rc==r4success)
					{
						int iSkip = 1;
						if (search.IsForward())
						{
							rc = relation.top();
							iSkip = 1;
						}
						else
						{
							rc = relation.bottom();
							iSkip = -1;
						}

						for (; rc == r4success; rc=relation.skip(iSkip))
						{
							// we are now on an found record send it
							dwQueryRecNo = m_pData4->recNo();
							if (search.AddSearchResult(this, CStorageId(GetArchiveNr(), GetNr(), dwQueryRecNo)))
							{
								// continue searching still connected
							}
							else
							{
								bRet = FALSE;
//								WK_TRACE(_T("query result and disconnect or cancelled\n"));
								break;
							}
						}
						errorSet();
					}
					else
					{
						WK_TRACE_ERROR(_T("querySet failed C=%d, T=%d, %d\n"),
							GetArchiveNr(),GetNr(),rc);
						errorSet();
					}
				}
				else
				{
					WK_TRACE_ERROR(_T("Relate4set::init failed C=%d, T=%d, %d\n"),
						GetArchiveNr(),GetNr(),rc);
					errorSet();
				}
				relation.free();

				CloseDbf();
			}
			else
			{
				bRet = FALSE;
			}

		}
		else
		{
			// muss nicht suchen, da die Zeit falsch ist
			bRet = TRUE;
		}
	}
	else
	{
		// empty sequence
		bRet = FALSE;
	}

	Unlock();

	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CTape::Backup(CBackup& b)
{
	BOOL bRet = TRUE;

	Lock(_T("S18"));
	m_bLockedByBackup = TRUE;
	if (IsOpen())
	{
		if (theApp.m_bTraceBackup)
		{
			WK_TRACE(_T("close sequence by backup %04hx,%d\n"), GetArchiveNr(), GetNr());
		}
		Close(TRUE);
	}
	Unlock();

	if (theApp.m_bTraceBackup)
	{
		WK_TRACE(_T("backup archive <%s> sequence %d to <%s>\n"),
			m_pCollection->GetName(), GetNr(), b.GetDestinationPath());
	}

	CString sDir;

	// create the directory
	sDir.Format(_T("%s\\arch%04lx"),b.GetDirectory(),m_pCollection->GetNr());

	if (!DoesFileExist(sDir))
	{
		if (!CreateDirectory(sDir,NULL))
		{
			WK_TRACE_ERROR(_T("cannot create directory %s, %s\n"),
				sDir,GetLastErrorString());
			bRet = FALSE;
		}
		else
		{
			if (theApp.m_bTraceBackup)
				WK_TRACE(_T("mdir SUCCESS %s\n"),sDir);
		}
	}

	if (bRet)
	{
		CString sDbf,sDat; // source names
		CString dDbf,dDat; // destination names

		sDbf = GetFileName(szDbf);
		sDat = GetFileName(szDat);

		dDbf.Format(_T("%s\\%04lx%04lx.%s"), sDir, (WORD)Flags2Nr((BYTE)(m_bFlags|CAR_IS_BACKUP)), GetNr(), szDbf);
		dDat = dDbf;
		dDat.SetAt(dDat.GetLength()-2,_T('a'));
		dDat.SetAt(dDat.GetLength()-1,_T('t'));

		// now copy the files
		bRet = b.CopyFileTrace(GetArchiveNr(), GetNr(), sDbf, dDbf);

		if (   !m_pCollection->IsAlarmList()
			&& bRet)
		{
			if (theApp.GetNoFrag())
			{
				FILESIZE dwDataLen = GetCurrentWritePosition();
				bRet = b.CopyFileTrace(GetArchiveNr(), GetNr(), sDat, dDat, dwDataLen);
			}
			else
			{
				bRet = b.CopyFileTrace(GetArchiveNr(), GetNr(), sDat, dDat);
			}
		}
	}


	Lock(_T("S19"));
	m_bLockedByBackup = FALSE;
	Unlock();

	if (bRet)
	{
		b.GetSequenceMap().SetSequenceHashEntry(GetArchiveNr(),
												GetNr(),
												m_dwNumRecords,
												m_stFirst,
												m_stLast,
												_T(""));
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////////
BOOL CTape::BackupByTime(CBackup& b)
{
	BOOL bRet = TRUE;
	DWORD dwCopiedBytes = 0;
	DWORD dwNumRecords = 0;
	CSystemTime stFirst,stLast;

	Lock(_T("S18"));
	m_bLockedByBackup = TRUE;
	if (IsOpen())
	{
		if (theApp.m_bTraceBackup)
		{
			WK_TRACE(_T("close sequence by backup %04hx,%d\n"),GetArchiveNr(),GetNr());
		}
		Close(TRUE);
	}
	Unlock();

	if (theApp.m_bTraceBackup)
	{
		WK_TRACE(_T("BackupByTime archive <%s> sequence %d to <%s>\n"),
			m_pCollection->GetName(), GetNr(), b.GetDestinationPath());
	}
	
	if (m_stFirst > b.GetEndTime())
	{
		// Sequence is too young
		if (theApp.m_bTraceBackup)
		{
			WK_TRACE(_T("Sequence %04hx is too young first is %s\n"),GetNr(),m_stFirst.GetDateTime());
		}
		bRet = FALSE;
	}
	else if (m_stLast < b.GetStartTime())
	{
		// Sequence is too old
		if (theApp.m_bTraceBackup)
		{
			WK_TRACE(_T("Sequence %04hx is too old last is %s\n"),GetNr(),m_stLast.GetDateTime());
		}
		bRet = FALSE;
	}
	else
	{
		// find out correct start and end record
		DWORD dwStartRecord = 1;
		DWORD dwEndRecord = m_dwNumRecords;

		stFirst = m_stFirst;
		stLast = m_stLast;

		if (m_stFirst < b.GetStartTime())
		{
			// find out correct start record
			dwStartRecord = GetRecordNr(b.GetStartTime(),TRUE);
			stFirst = b.GetStartTime();
		}
		if (m_stLast > b.GetEndTime())
		{
			// find out correct end record
			dwEndRecord = GetRecordNr(b.GetEndTime(),FALSE);
			stLast = b.GetEndTime();
		}
		dwNumRecords = dwEndRecord-dwStartRecord+1;

		if (  dwStartRecord != 0
			&& dwEndRecord   != 0)
		{
			if (  (dwStartRecord == 1)
				&& (dwEndRecord   == m_dwNumRecords)
				)
			{
				// copy the entire sequence
				bRet = Backup(b);
			}
			else
			{
				CString sDir;

				// create the directory
				sDir.Format(_T("%s\\arch%04lx"), b.GetDirectory(), m_pCollection->GetNr());

				if (!DoesFileExist(sDir))
				{
					if (!CreateDirectory(sDir,NULL))
					{
						WK_TRACE_ERROR(_T("cannot create directory %s, %s\n"),
							sDir,GetLastErrorString());
						bRet = FALSE;
					}
					else
					{
						WK_TRACE(_T("mdir SUCCESS %s\n"), sDir);
					}
				}
				if (DoesFileExist(sDir))
				{
					if (theApp.m_bTraceBackup)
					{
						WK_TRACE(_T("cut sequence from %d -> %d\n"), dwStartRecord, dwEndRecord);
					}

					// cut calls the director cut
					CWK_String sDbf, sDat; // source names
					CWK_String tDbf, tDat; // temp names
					CWK_String dDbf, dDat; // destination names
					CWK_String sTempDir;
					
					sDbf = GetFileName(szDbf);
					sDat = GetFileName(szDat);

					//
					GetTempPath(_MAX_PATH, sTempDir.GetBufferSetLength(_MAX_PATH));
					sTempDir.ReleaseBuffer();
				
					GetTempFileName(sTempDir, szDbf, 0, tDbf.GetBufferSetLength(_MAX_PATH));
					GetTempFileName(sTempDir, szDat, 0, tDat.GetBufferSetLength(_MAX_PATH));
					tDbf.ReleaseBuffer();
					tDat.ReleaseBuffer();

					dDbf.Format(_T("%s\\%04lx%04lx.%s"), sDir, (WORD)Flags2Nr((BYTE)(m_bFlags|CAR_IS_BACKUP)), GetNr(), szDbf);
					dDat = dDbf;
					dDat.SetAt(dDat.GetLength()-2, _T('a'));
					dDat.SetAt(dDat.GetLength()-1, _T('t'));

					DWORD dwNewDbfSize = 0;
					DWORD dwNewDatSize = 0;

					if (b.IsBackupToRWMedium())
					{
						// first cut the dbf file
						bRet = CutDbf(dwStartRecord, dwEndRecord, dDbf, dwNewDbfSize);
						if (bRet)
						{
							if (b.GetFlags() & BBT_PROGRESS)
							{
								b.IncreaseCopiedBytes(GetArchiveNr(),
									GetNr(),
									dwNewDbfSize,
									BBT_PROGRESS);
							}

							if (   !m_pCollection->IsAlarmList()
								&& bRet)
							{
								// now cut the dat file directly to destination
								bRet = CutDat(dwStartRecord, dwEndRecord, dDat, dwNewDatSize, b, TRUE);
							}
						}
					}
					else
					{
						if (  CutDbf(dwStartRecord, dwEndRecord, tDbf, dwNewDbfSize)
							&& CutDat(dwStartRecord, dwEndRecord, tDat, dwNewDatSize, b, FALSE)
							)
						{
							// now copy from temp to dest
							bRet = b.CopyFileTrace(GetArchiveNr(),GetNr(),tDbf,dDbf);

							if (   !m_pCollection->IsAlarmList()
								&& bRet)
							{
								bRet = b.CopyFileTrace(GetArchiveNr(),GetNr(),tDat,dDat);
							}
						}
					}
					if (bRet)
					{
						if ( ((b.GetFlags() & BBT_PROGRESS) == 0)
							&& (b.GetFlags() & BBT_EXECUTE)
							)
						{
							b.IncreaseCopiedBytes(GetArchiveNr(), GetNr(), dwNewDbfSize+dwNewDatSize, BBT_FINISHED);
						}
					}
					// always delete temporary files
					DeleteFile(tDbf);
					DeleteFile(tDat);
				}
				else
				{
					bRet = FALSE;
				}
			}
		}
		else
		{
			bRet = FALSE;
		}
	}
	if (bRet)
	{
		b.IncreaseCopiedBytes(GetArchiveNr(), GetNr(), dwCopiedBytes, BBT_FINISHED);
		b.GetSequenceMap().SetSequenceHashEntry(GetArchiveNr(), 
												GetNr(),
												dwNumRecords,
												stFirst,
												stLast,
												_T(""));
	}

	Lock(_T("S19"));
	m_bLockedByBackup = FALSE;
	Unlock();

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
void CTape::CorrectReferences(Data4& data, DWORD dwFirstRecordNr)
{
	// now correct the I-Frame refs
	if (dwFirstRecordNr>1)
	{
		Field4 fIFrameRef(data,DBP_IFRAME_REF);
		DWORD dwRef = 0;
		if (fIFrameRef.isValid())
		{
			// old format
			// rewind video
			if (ReadDWORD(data,GetCode4(),DBP_IFRAME_REF,dwRef))
			{
				dwRef = dwRef - dwFirstRecordNr + 1;
				AssignDWORD(data,GetCode4(),DBP_IFRAME_REF,dwRef);
			}
		}
		else
		{
			errorSet();

			// fast forward video
			if (ReadDWORD(data,GetCode4(),DBP_FFV_REF,dwRef))
			{
				dwRef = dwRef - dwFirstRecordNr + 1;
				AssignDWORD(data,GetCode4(),DBP_FFV_REF,dwRef);
			}
			// rewind video
			if (ReadDWORD(data,GetCode4(),DBP_RWV_REF,dwRef))
			{
				dwRef = dwRef - dwFirstRecordNr + 1;
				AssignDWORD(data,GetCode4(),DBP_RWV_REF,dwRef);
			}
			// fast forward audio
			if (ReadDWORD(data,GetCode4(),DBP_FFA_REF,dwRef))
			{
				dwRef = dwRef - dwFirstRecordNr + 1;
				AssignDWORD(data,GetCode4(),DBP_FFA_REF,dwRef);
			}
			// rewind audio
			if (ReadDWORD(data,GetCode4(),DBP_RWA_REF,dwRef))
			{
				dwRef = dwRef - dwFirstRecordNr + 1;
				AssignDWORD(data,GetCode4(),DBP_RWA_REF,dwRef);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTape::CutDbf(DWORD dwFirstRecordNr,
			       DWORD dwLastRecordNr,
			       CWK_String& sDbf,
				   DWORD& dwNewSizeBytes)
{
	BOOL bRet = TRUE;

	Lock(_T("S21"));

	if (OpenDbf())
	{
		Data4 data4;
		Field4info finfo(m_pCollection->m_CodeBase);
		int ret = finfo.add(*m_pData4);

		// copy the field info structure
		if (r4success == ret)
		{
			ret = data4.create(m_pCollection->m_CodeBase,sDbf,finfo);

			if (r4success == ret)
			{
				DWORD dwOffset = 0;
				DWORD dwLen = 0;
				for (DWORD i = dwFirstRecordNr; i<=dwLastRecordNr; i++)
				{
					ret = m_pData4->go(i);
					if (r4success == ret)
					{
						ret = data4.appendStart();
						if (r4success != ret)
						{
							WK_TRACE_ERROR(_T("cannot append start temp dbf file %s,%d\n"), sDbf, ret);
							bRet = FALSE;
							break;
						}
						data4.blank();
						// copy the whole data at once
						// cheap and tricky
						strcpy(data4.record(), m_pData4->record());
						
						// now correct the offsets
						Field4 fOffset(data4,DVR_DATA_POS);
						if (fOffset.isValid())
						{
							char szFmt[64];
							sprintf(szFmt, szDWORD, dwOffset);
							ret = fOffset.assign(szFmt);
							if (r4success != ret)
							{
								WK_TRACE_ERROR(_T("cannot assign offset field temp dbf file %s,%d\n"), sDbf, ret);
								bRet = FALSE;
								break;
							}
						}
						else
						{
							WK_TRACE_ERROR(_T("cannot access offset field temp dbf file %s,%d\n"), sDbf, ret);
							bRet = FALSE;
							break;
						}
						Field4 fLength(*m_pData4, DVR_DATA_LEN);
						if (fLength.isValid())
						{
							sscanf(fLength.str(), SCAN_DWORD, &dwLen);
							dwOffset += dwLen;
						}
						else
						{
							WK_TRACE_ERROR(_T("cannot read data length source dbf file %d\n"), ret);
							bRet = FALSE;
							break;
						}

						CorrectReferences(data4,dwFirstRecordNr);

						ret = data4.append();
						if (r4success != ret)
						{
							WK_TRACE_ERROR(_T("cannot append temp dbf file %s,%d\n"), sDbf, ret);
							bRet = FALSE;
							break;
						}
					}
					else
					{
						WK_TRACE_ERROR(_T("cannot goto source record,%d\n"), ret);
						bRet = FALSE;
						break;
					}
				}
				if (bRet)
				{
					ret = data4.flush();
					if (ret == r4success)
					{
						dwNewSizeBytes  = m_dwFieldSize * data4.recCount();
						dwNewSizeBytes += m_pCollection->m_FieldInfo.numFields() * 32 + 34;
					}
					else
					{
						WK_TRACE_ERROR(_T("cannot flush temp dbf file %s,%d\n"), sDbf, ret);
						bRet = FALSE;
					}
				}
				// always try to close, delete must work
				ret = data4.close();
				if (ret != r4success)
				{
					WK_TRACE_ERROR(_T("cannot close temp dbf file %s,%d\n"), sDbf, ret);
					bRet = FALSE;
				}
			}
			else
			{
				WK_TRACE_ERROR(_T("cannot create temp dbf file %s,%d\n"), sDbf, ret);
			}
		}
		else
		{
			WK_TRACE_ERROR(_T("cannot copy field info structure %s,%d\n"), sDbf, ret);
		}
		errorSet();
		CloseDbf();
	}
	
	Unlock();

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTape::CutDat(DWORD dwFirstRecordNr,
			       DWORD dwLastRecordNr,
			       const CString& sDat,
				   DWORD& dwNewSizeBytes,
				   CBackup& b,
				   BOOL bIndicateProgress)
{
	BOOL bRet = TRUE;

	Lock(_T("S22"));

	DWORD dwEndLength,dwStartLength;
	FILESIZE dwEndOffset,dwStartOffset;
	GetOffsetAndLength(dwFirstRecordNr,dwStartOffset,dwStartLength);
	GetOffsetAndLength(dwLastRecordNr,dwEndOffset,dwEndLength);
	FILESIZE dwDatSize = dwEndOffset-dwStartOffset+dwEndLength;

	if (OpenDat())
	{
		CFile file;

		if (file.Open(sDat,CFile::modeCreate|CFile::modeWrite))
		{
			DWORD dwSize = 1024*1024;
			DWORD dwWrote=0;
			BYTE* pBuffer = new BYTE[dwSize];

			TRY
			{
				m_fImages.Seek(dwStartOffset, CFile::begin);
				DWORD dwRead = 0;
				while (    bRet
						&& ((dwRead=m_fImages.Read(pBuffer,dwSize))>0)
						)
				{
					DWORD dwToWrite = (DWORD)min(dwRead,dwDatSize-dwWrote); 
					TRY
					{
						file.Write(pBuffer,dwToWrite);
					}
					CATCH (CFileException ,e)
					{
						bRet = FALSE;
					}
					END_CATCH
					if (bRet)
					{
						dwWrote += dwToWrite;
						if (bIndicateProgress)
						{
							b.IncreaseCopiedBytes(GetArchiveNr(),GetNr(),dwToWrite,BBT_PROGRESS);
						}
					}
				}
			}
			CATCH (CFileException ,e)
			{
				WK_TRACE_ERROR(_T("cannot read file for copy %d Bytes, %s\n"),
					dwWrote,GetLastErrorString(e->m_lOsError));
				bRet = FALSE;
			}
			END_CATCH

			WK_DELETE_ARRAY(pBuffer);
			
			TRY
			{
				file.Close();
			}
			CATCH (CFileException,e)
			{
				WK_TRACE_ERROR(_T("cannot close file for cut %s, %s\n"),
					sDat,GetLastErrorString(e->m_lOsError));
				bRet = FALSE;
			}
			END_CATCH

			dwNewSizeBytes = dwWrote;
		}
		CloseDat();
	}
	
	Unlock();

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTape::Verify()
{
	if (IsBackup())
	{
		return TRUE;
	}

	Lock(_T("S23"));

	CSystemTime stFirst = m_stFirst;
	CSystemTime stLast = m_stLast;
	DWORD dwNumRecords = m_dwNumRecords;
	BOOL bEqual = TRUE;

	if (theApp.m_bTraceVerify)
	{
		WK_TRACE(_T("verifying %04x %08d\n"),GetArchiveNr(),GetNr());
	}

	// richtig die Datei öffnen und m_stFirst, m_stLast, m_dwNumRecords
	// neu einlesen
	if (ScanDbf())
	{
		if (!(stFirst == m_stFirst))
		{
			WK_TRACE_ERROR(_T("sequence hash entry wrong First %s,%03d!=%s,%03d\n"),
				stFirst.GetDateTime(),
				stFirst.GetMilliseconds(),
				m_stFirst.GetDateTime(),
				m_stFirst.GetMilliseconds());
			bEqual = FALSE;
			if (!(stFirst == m_stFirst))
			{
				bEqual = FALSE;
			}
		}
		if (!(stLast == m_stLast))
		{
			WK_TRACE_ERROR(_T("sequence hash entry wrong Last %s,%03d!=%s,%03d\n"),
				stLast.GetDateTime(),
				stLast.GetMilliseconds(),
				m_stLast.GetDateTime(),
				m_stLast.GetMilliseconds());
			bEqual = FALSE;
			if (!(stLast == m_stLast))
			{
				bEqual = FALSE;
			}
		}
		if (dwNumRecords != m_dwNumRecords)
		{
			WK_TRACE_ERROR(_T("sequence hash entry wrong NumRecords %d!=%d\n"),
				dwNumRecords,m_dwNumRecords);
			bEqual = FALSE;
		}

		if (!bEqual)
		{
			// correct the hash entry
			WK_TRACE(_T("correcting sequence hash entry %hx %d\n"),GetArchiveNr(),GetNr());
			SetSequenceHashEntry();
		}
		else if (theApp.m_bTraceVerify)
		{
			WK_TRACE(_T("verify OK %04x %08d\n"),GetArchiveNr(),GetNr());
		}

	}
	Unlock();
	return bEqual;
}
