/* GlobalReplace: line %s --> line %d */
// Sequence.cpp: implementation of the CSequence class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DBS.h"
#include "Sequence.h"

#include "OldPictData.h"
#include "Win32FindData.h"
#include "VDBSequenceHashEntry.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static TCHAR BASED_CODE szBYTE[] = _T("%02lx");
static TCHAR BASED_CODE szWORD[] = _T("%04lx");
static TCHAR BASED_CODE szDWORD[] = _T("%08lx");

static TCHAR BASED_CODE szIntFormat[] = _T("%d");
static TCHAR BASED_CODE szTimeFormat[] = _T("%02d%02d%02d");
static TCHAR BASED_CODE szDateFormat[] = _T("%04d%02d%02d");

static TCHAR BASED_CODE szFileFormat[] = _T("%s%s\\%04lx%04lx.%s");
static TCHAR BASED_CODE szDbf[] = _T("dbf");
static TCHAR BASED_CODE szDbx[] = _T("dbx");
static TCHAR BASED_CODE szDbi[] = _T("index.dbi");
static char BASED_CODE szMM[]  = "MM";

#define SCAN_DWORD  "%08x"
#define SCAN_INT    "%d"
#define SCAN_BYTE   "%02lx"

// the old database format
/*
FIELD4INFO	new_info [] =
{
	{	"ARC_NR",	'N',	3,					0	},
	{	"SKAMERA",	'C',	2,					0	},
	{	"DATUM",	'C',	8,					0	},	// YY.MM.DD 
	{	"ZEIT",		'C',	8,					0	},	// HH:MM:SS
	{	"TYP",		'N',	2,					0	},
	{	"COMP",		'N',	2,					0	},
	{	"VOLLBILD",	'C',	1,					0	},
	{	"DTP_ZEIT",	'C',	8,					0	},	// HH:MM:SS
	{	"GKAMERA",	'C',	1,					0	},
	{	"GA_NR",	'C',	SDI_GA_LEN,			0	},	//6
	{	"TA_NR",	'N',	SDI_TAN_LEN,		0	},	//4
	{	"KTO_NR",	'C',	SDI_KONTO_LEN,		0	},	//16
	{	"CARD_TYP",	'C',	2,					0	},
	{	"GAA_TYP",	'C',	2,					0	},
	{	"BLZ",		'N',	SDI_BLZ_LEN,		0	},	//8
	{	"BETRAG",	'N',	SDI_BETRAG_LEN,		0	},	//8
	{	"WAEHRUNG",	'C',	SDI_WAEHRUNG_LEN,	0	},	//3
	{	"STATION",	'C',	40,					0	},
	{	"KAM_NAME",	'C',	40,					0	},
	{	"VAR1",		'C',	10,					0	},
	{	"VAR2",		'C',	10,					0	},
	{	"VAR3",		'C',	10,					0	},
	{	"COMMENT",	'C',	50,					0	},
	{	0,			0,		0,					0	},
};
*/

int repair(Code4& c4, const char* fileName)
{
	CCallStackEntry cse(_T(__FUNCTION__));
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
	CCallStackEntry cse(_T(__FUNCTION__));
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
	DWORD dwHeaderLen = sizeof(fullHeader);
	rc = f.read(0L, &fullHeader, dwHeaderLen);
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
	
#ifdef S4BYTE_SWAP
	fullHeader.numRecs = x4reverseLong( (void *)&fullHeader.numRecs ) ;
	fullHeader.headerLen = x4reverseShort( (void *)&fullHeader.headerLen ) ;
	fullHeader.recordLen = x4reverseShort( (void *)&fullHeader.recordLen ) ;
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
		rc2 = rc;
	return rc2;
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CSequence::CSequence(BYTE bFlags,CArchiv* pArchiv, CIPCDrive* pDrive, WORD wID)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	m_wID	  = wID;
	m_pArchiv = pArchiv;
	m_sDrive = pDrive->GetRootString();
	m_dwNumRecords = 0;
	m_dwClusterSize = pDrive->GetClusterSize();
	m_dwDbfSize = 0;
	m_dwDbdSize = 0;
	m_dwDbxSize = 0;
	m_dwFieldSize = 0;
	m_bLockedByBackup = FALSE;
	m_dwDatWritePosition = 0;

	m_bRO = pDrive->IsCDROM();
	m_iOpenDbf = 0;
	m_iOpenDbd = 0;
	m_pData4 = NULL;

	m_dwLastVideo  = 0;
	m_dwLastAudio  = 0;

	if (bFlags == 0)
	{
		m_bFlags = Nr2Flags((WORD)pArchiv->GetType());
	}
	else
	{
		m_bFlags = bFlags;
	}
	m_stFirst.GetLocalTime();
	m_stLast = m_stFirst;
}
//////////////////////////////////////////////////////////////////////
WORD CSequence::GetArchivNr() const
{
	return m_pArchiv->GetNr();
}
//////////////////////////////////////////////////////////////////////
CSequence::~CSequence()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	Close(TRUE);
	WK_DELETE(m_pData4);
}
//////////////////////////////////////////////////////////////////////
CString CSequence::GetName() const
{
	return m_sName;
}
//////////////////////////////////////////////////////////////////////
CIPCInt64 CSequence::GetDbfSizeBytes() const
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CIPCInt64 r = 0;
	if (m_dwClusterSize>0)
	{
		r += CIPCInt64(RoundUp(m_dwDbfSize,m_dwClusterSize));
	}
	else
	{
		r += CIPCInt64(m_dwDbfSize);
	}
	return r;
}
//////////////////////////////////////////////////////////////////////
Code4& CSequence::GetCode4()
{
	return m_pArchiv->m_cb;
}
//////////////////////////////////////////////////////////////////////
CIPCInt64 CSequence::GetDatSizeBytes() const
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CIPCInt64 r = 0;
	if (m_dwClusterSize>0)
	{
		r += CIPCInt64(RoundUp(m_dwDbdSize,m_dwClusterSize));
		r += CIPCInt64(RoundUp(m_dwDbxSize,m_dwClusterSize));
	}
	else
	{
		r += CIPCInt64(m_dwDbdSize);
		r += CIPCInt64(m_dwDbxSize);
	}
	return r;
}
//////////////////////////////////////////////////////////////////////
CIPCInt64 CSequence::GetSizeBytes() const
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CIPCInt64 r = 0;
	if (m_dwClusterSize>0)
	{
		r += CIPCInt64(RoundUp(m_dwDbfSize,m_dwClusterSize));
		r += CIPCInt64(RoundUp(m_dwDbdSize,m_dwClusterSize));
		r += CIPCInt64(RoundUp(m_dwDbxSize,m_dwClusterSize));
	}
	else
	{
		r += CIPCInt64(m_dwDbfSize);
		r += CIPCInt64(m_dwDbdSize);
		r += CIPCInt64(m_dwDbxSize);
	}
	return r;
}
//////////////////////////////////////////////////////////////////////
CIPCInt64 CSequence::GetSizeBytes(const CSystemTime& stStart,
							      const CSystemTime& stEnd)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CIPCInt64 r = 0;

	if (m_stFirst > stEnd)
	{
		// Sequence is too young
//		TRACE(_T("GetSizeBytes %d Sequence is too young\n"),GetTapeNr());
		r = 0;
	}
	else if (m_stLast < stStart)
	{
		// Sequence is too old
//		TRACE(_T("GetSizeBytes %d Sequence is too old\n"),GetTapeNr());
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
			DWORD dwEndLength,dwStartLength;
			FILESIZE dwEndOffset,dwStartOffset;
			GetOffsetAndLength(dwStartRecord,dwStartOffset,dwStartLength);
			GetOffsetAndLength(dwEndRecord,dwEndOffset,dwEndLength);

			if (dwStartRecord<=dwEndRecord)
			{
				FILESIZE dwDbfSize = (dwEndRecord-dwStartRecord)*m_dwFieldSize;
				FILESIZE dwDatSize = dwEndOffset-dwStartOffset+dwEndLength;

				dwDbfSize += m_pArchiv->m_FieldInfo.numFields() * 32 + 34;
					
/*
				if (m_dwClusterSize>0)
				{
					r += CIPCInt64(RoundBinary(dwDbfSize,m_dwClusterSize));
					r += CIPCInt64(RoundBinary(dwDatSize,m_dwClusterSize));
				}
				else
*/
				{
					r += CIPCInt64(dwDbfSize);
					r += CIPCInt64(dwDatSize);
				}
/*
				WK_TRACE(_T("GetSizeBytes %s, %d->%d/%d %s Bytes\n"),
					m_stFirst.GetDateTime(),dwStartRecord,dwEndRecord,m_dwNumRecords,r.Format(TRUE));
*/
			}
		}
	}

	return r;
}
//////////////////////////////////////////////////////////////////////
BOOL CSequence::operator < (const CSequence& s)
{
	return m_stFirst < s.m_stFirst;
}
/////////////////////////////////////////////////////////////////////
BOOL CSequence::AreFieldsEqual()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	BOOL bRet = TRUE;

	CAutoCritSec acs(&m_cs);

	if (OpenDbf(TRUE))
	{
		int i,j,c,d;
		Field4info fields(*m_pData4);

		c = m_pArchiv->m_FieldInfo.numFields();
		d = fields.numFields();

		if (c == d)
		{
			int f = 0;
			// compare the fields
			for (i=0;i<c && bRet ;i++)
			{
				const FIELD4INFO* pInfoC = m_pArchiv->m_FieldInfo[i];
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
		CloseDbf(FALSE, TRUE);
	}
	else
	{
		// cannot open dbf
	}

	acs.Unlock();

	return bRet;
}
//////////////////////////////////////////////////////////////////////
void CSequence::GetNrAndFlags(const CString& sName, WORD& wNr,BYTE& bFlags)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	DWORD dw = 0;
	if (1 == _stscanf(sName,_T("%08lx.dbf"),&dw))
	{
		wNr	   = LOWORD(dw);
		bFlags = Nr2Flags(HIWORD(dw));
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CSequence::Scan(const CWin32FindData* pDbf, const CWin32FindData* pDbd, const CVDBSequenceMap& map)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	BOOL bRet = FALSE;

	CString sDbf(pDbf->cFileName);
	CString sDbdx(pDbd->cFileName);

	GetNrAndFlags(sDbf,m_wID,m_bFlags);

	if (!m_pArchiv->GetFixedDrive().IsEmpty())
	{
		if (0!=m_pArchiv->GetFixedDrive().CompareNoCase(m_sDrive.Left(2)))
		{
			WK_TRACE(_T("sequence on wrong drive %s should be on %s\n"),GetFileName(szDbf),m_pArchiv->GetFixedDrive());
		}
	}

	if (m_dwClusterSize>0)
	{
		m_dwDbfSize = RoundUp(pDbf->nFileSizeLow,m_dwClusterSize);
	}
	else
	{
		m_dwDbfSize = pDbf->nFileSizeLow;
	}
	sDbdx.MakeLower();
	CString sFind;
	sFind = _T(".") + CArchiv::m_sDbd;
	if (-1 != sDbdx.Find(sFind))
	{
		if (m_dwClusterSize>0)
		{
			m_dwDbdSize = RoundUp(pDbd->nFileSizeLow,m_dwClusterSize);
		}
		else
		{
			m_dwDbdSize = pDbd->nFileSizeLow;
		}
	}
	if (-1 != sDbdx.Find(_T(".dbx")))
	{
		if (m_dwClusterSize>0)
		{
			m_dwDbxSize = RoundUp(pDbd->nFileSizeLow,m_dwClusterSize);
		}
		else
		{
			m_dwDbxSize = pDbd->nFileSizeLow; 
		}
	}

	CVDBSequenceHashEntry* pSequenceHashEntry = map.GetSequenceHashEntry(m_pArchiv->GetOriginalNr(),m_wID);
	if (   pSequenceHashEntry
		&& pSequenceHashEntry->GetArchiveNr() == m_pArchiv->GetOriginalNr()
		&& pSequenceHashEntry->GetSequenceNr() == m_wID
		&& pSequenceHashEntry->GetRecords()>0)
	{
		m_stFirst = pSequenceHashEntry->GetFirst();
		m_stLast = pSequenceHashEntry->GetLast();
		m_dwNumRecords = pSequenceHashEntry->GetRecords();
		CString s;
		if (s != m_stFirst.GetDateTime())
		{
			m_sName = s;
		}
		bRet = TRUE;
/*
		TRACE(_T("Sequence hash entry     found %04hx, %04hx\n"),
			m_pArchiv->GetOriginalNr(),m_wID);
*/
	}
	else if (ScanDbi())
	{
		SetSequenceHashEntry();
		bRet = TRUE;
	}
	else
	{
/*
		TRACE(_T("Sequence hash entry not found %04hx, %04hx\n"),
			m_pArchiv->GetOriginalNr(),m_wID);
*/
		// scan dbf already does the repair
		if (ScanDbf())
		{
			SetSequenceHashEntry();
			bRet = TRUE;
		}
		else
		{
			CString sDbf = GetFileName(szDbf);
			WK_TRACE_ERROR(_T("scan failed %s\n"),sDbf);
		}
	}

	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CSequence::ReadTime(BOOL bFirst)
{
	CCallStackEntry cse(_T(__FUNCTION__));

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
				Field4 date(*m_pData4, CArchiv::m_sDATE);
				if (date.isValid())
				{
#ifdef _UNICODE
					CWK_String sDate(date.str());
					b1 = st.SetDBDate(sDate);
					if (!b1)
					{
						TRACE(_T("set db date failed C=%d T=%d, %d, %s\n"), GetArchivNr(),
							GetNr(), ret, sDate);
					}
#else
					b1 = st.SetDBDate(date.str());
#endif

					Field4 time(*m_pData4, CArchiv::m_sTIME);
					if (time.isValid())
					{
#ifdef _UNICODE
						CWK_String sTime(time.str());
						b2 = st.SetDBTime(sTime);
						if (!b1)
						{
							TRACE(_T("set db time failed C=%d T=%d, %d, %s\n"), GetArchivNr(),
								GetNr(), ret, sTime);
						}
#else
						b2 = st.SetDBTime(time.str());
#endif
						if (!IsAlarmList())
						{
							Field4 ms(*m_pData4,CArchiv::m_sMS);
							if (ms.isValid())
							{
								DWORD dwMilliseconds;
								sscanf(ms.str(),"%03d",&dwMilliseconds);
								st.wMilliseconds = (WORD)dwMilliseconds;
							}
							else
							{
								// ignore for older db format
								errorSet();
							}
						}
					}
					else
					{
						WK_TRACE_ERROR(_T("time field invalid C=%d T=%d, %d\n"), GetArchivNr(),
							GetNr(), ret);
						errorSet();
					}
				}
				else
				{
					WK_TRACE_ERROR(_T("date field invalid C=%d T=%d, %d\n"), GetArchivNr(),
						GetNr(), ret);
					errorSet();
				}
			}
			else
			{
				WK_TRACE_ERROR(_T("top/bottom failed C=%d T=%d, %d\n"), GetArchivNr(),
					GetNr(), ret);
				errorSet();
			}
		}
	}
	return b1 && b2;
}
//////////////////////////////////////////////////////////////////////
BOOL CSequence::ScanDbf()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	BOOL bRet = FALSE;

	TRY
	{
		if (OpenDbf(TRUE))
		{
			m_dwNumRecords = m_pData4->recCount();
			bRet = ReadTime(TRUE);
			bRet = bRet && ReadTime(FALSE);
			CloseDbf(FALSE, TRUE);
		}
/*
		else if (theApp.IsNetworkStorageAllowed())
		{
			theApp.CheckRemoveNetworkDrive(m_pArchiv->GetFixedDrive());
		}
*/
	}
	CATCH(CFileException, cfe)
	{
		CString sDrive;
		if (!cfe->m_strFileName.IsEmpty())
		{
			sDrive = cfe->m_strFileName.Left(3);
		}
		else if (!m_pArchiv->GetFixedDrive().IsEmpty())
		{
			sDrive = m_pArchiv->GetFixedDrive();
		}
		theApp.HandleDriveError(cfe->m_lOsError, sDrive);

		bRet = FALSE;
	}
	END_CATCH
	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CSequence::ScanDbi()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CString sDbi = GetIndexDbi();
	BOOL bIni = TRUE;

	if (DoesFileExist(szDbi))
	{
		CString sSection,sNumRecords;
		int iOpen = 1;
		sSection.Format(szWORD,GetNr());
		
		// always assume that file was opened before start
		iOpen = GetPrivateProfileInt(sSection,_T("OpenDbf"),iOpen,sDbi);
		
		if (iOpen == 0)
		{
			bIni = GetPrivateProfileString(sSection,_T("NumRecords"),_T(""),sNumRecords.GetBuffer(20),20,sDbi)>0;
			sNumRecords.ReleaseBuffer();
			
			if (bIni)
			{
				if (1 == _stscanf(sNumRecords, szDWORD, &m_dwNumRecords))
				{
					if (m_dwNumRecords>0)
					{
						CString sT1,sT2,sT3;
						
						GetPrivateProfileString(sSection, _T("FirstTime"), _T(""),
							sT1.GetBuffer(100),100,sDbi);
						sT1.ReleaseBuffer();
						if (m_stFirst.FromString(sT1))
						{
							GetPrivateProfileString(sSection, _T("LastTime"), _T(""),
								sT2.GetBuffer(100),100,sDbi);
							sT2.ReleaseBuffer();
							if (m_stLast.FromString(sT2))
							{
								GetPrivateProfileString(sSection, _T("Name"), _T(""),
									sT3.GetBuffer(100),100,sDbi);
								sT3.ReleaseBuffer();
								m_sName = sT3;
							}
							else
							{
								bIni = FALSE;
							}
						}
						else
						{
							bIni = FALSE;
						}
					}
					else
					{
						bIni = FALSE;
					}
				}
				else
				{
					bIni = FALSE;
				}
			}
		}
		else
		{
			// file was open before start
			// or missing dbi
			bIni = FALSE;
		}
	}
	else
	{
		bIni = FALSE;
	}

	return bIni;
}
//////////////////////////////////////////////////////////////////////
BOOL CSequence::CreateDbf(BOOL bThrow/*=FALSE*/)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	if ((m_pData4==NULL) && (m_dwNumRecords == 0))
	{
		CString sDbf(GetFileName(szDbf));
		CWK_String wksDbf(sDbf);

		m_pData4 = new Data4();

		int ret = m_pData4->create(m_pArchiv->m_cb,(LPCSTR)wksDbf,m_pArchiv->m_FieldInfo);

		if (r4success == ret)
		{
			m_iOpenDbf++;
			InitFields();
			m_dwDbfSize = m_pData4->numFields() * 32 + 34;
			// WK_TRACE(_T("creating %s\n"),sDbf);
		}
		else
		{
			WK_TRACE(_T("cannot create %s, %d\n"),(LPCTSTR)sDbf,ret);
			errorSet(0);
			WK_DELETE(m_pData4);
			CFileException fe(CFileException::generic, MAKELONG(0, ret), sDbf);
			theApp.HandleFileException(&fe, bThrow);
		}
	}
	return IsOpenDbf();
}
//////////////////////////////////////////////////////////////////////
BOOL CSequence::CreateDbd(BOOL bThrow/*=FALSE*/)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	BOOL bRet = TRUE;

	if (!IsAlarmList())
	{
		CString sDbd;
		CFileException cfe;
		sDbd = GetFileName(CArchiv::m_sDbd);
		if (m_fData.Open(sDbd,CFile::modeCreate|CFile::modeReadWrite,&cfe))
		{
			m_iOpenDbd++;
			m_dwDbdSize = 0;
			m_dwDatWritePosition = 0;

			FILESIZE dwSize = (FILESIZE)m_pArchiv->GetSequenceSizeBytes().GetInt64();
			CIPCDrive drive(m_sDrive,DVR_DB_DRIVE);
			drive.CheckSpace();
			CIPCInt64 iFreeOnDrive = drive.GetAvailableMB();
			iFreeOnDrive *= 1024*1024;
			CIPCInt64 iFreeInArchiv = m_pArchiv->GetBytesFree();
			CIPCInt64 iMinSize;


			if (   iFreeInArchiv > 0
				&& iFreeInArchiv < iFreeOnDrive)
			{
				iMinSize = iFreeInArchiv;
			}
			else
			{
				iMinSize = iFreeOnDrive;
			}

			if (iMinSize < m_pArchiv->GetSequenceSizeBytes())
			{
				dwSize = (DWORD)iMinSize.GetInt64();
//				TRACE(_T("new sequence limited %s %d Bytes %d MB %s\n"),m_pArchiv->GetName(),dwSize,dwSize/1024,sDbd);
			}
			dwSize = RoundUp(dwSize,m_dwClusterSize);

			TRY
			{
				m_fData.SetLength(dwSize);
			}
			CATCH(CFileException, pCfe)
			{
				WK_TRACE_ERROR(_T("cannot set length %s , Length=%d Bytes free %s Bytes because %s\n"),
					GetFileName(CArchiv::m_sDbd),dwSize,iFreeOnDrive.Format(TRUE),GetLastErrorString(pCfe->m_lOsError));
				theApp.HandleFileException(pCfe, bThrow);
			}
			END_CATCH
		}
		else
		{
			WK_TRACE(_T("cannot create %s because %s\n"),sDbd,
				GetLastErrorString(cfe.m_lOsError));
			theApp.HandleFileException(&cfe, bThrow);
			bRet = FALSE;
		}
	}
	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CSequence::CreateDir(BOOL bThrow/*=FALSE*/)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	BOOL bRet = TRUE;
	CString sDir,sDbf;
	sDir = GetDirectory();
	
	if (!DoesFileExist(sDir))
	{
		if (CreateDirectory(sDir,NULL))
		{
			WK_TRACE(_T("create directory %s\n"),sDir);
			m_pArchiv->NewDirectory(sDir);
		}
		else
		{
			DWORD dwError = GetLastError();
			WK_TRACE_ERROR(_T("cannot create directory %s, %s"),sDir,GetLastErrorString(dwError));
			CFileException cfe(CFileException::badPath, dwError, sDir);
			theApp.HandleFileException(&cfe, bThrow);
			bRet = FALSE;
		}
	}
	return bRet;
}
//////////////////////////////////////////////////////////////////////
void CSequence::errorSet(int e/*=0*/)
{
	m_pArchiv->m_cb.errorSet(e);
}
//////////////////////////////////////////////////////////////////////
BOOL CSequence::Create(BOOL bThrow/*=FALSE*/)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CAutoCritSec acs(&m_cs);

	BOOL bRet = CreateDir(bThrow);

	if (bRet)
	{
		bRet = CreateDbf(bThrow);
	}

	if (   bRet 
		&& !IsAlarmList())
	{
		bRet = CreateDbd(bThrow);
	}

	acs.Unlock();
	if (bRet && theApp.TraceSequence())
	{
		WK_STAT_PEAK(_T("Archiv"),2,m_pArchiv->GetName());
	}
	return bRet;
}
//////////////////////////////////////////////////////////////////////
void CSequence::InitFields()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	if (m_pData4)
	{
		int c,ret;

		c = m_pData4->numFields();
		m_dwFieldSize = 0;
		for (short i=1;i<=c;i++)
		{
			Field4 f;
			ret = f.init(*m_pData4,i);
			if (ret == r4success)
			{
//				TRACE(_T("field %s %d\n"),f.name(),f.len());
				m_dwFieldSize += f.len();
			}
			else
			{
				WK_TRACE_ERROR(_T("InitFields, Field4::init failed A=%d S=%d, %d\n"),
					GetArchivNr(),GetNr(),ret);
				errorSet(0);
			}
		}
		m_dwFieldSize++;
//		TRACE(_T("m_dwFieldSize = %d\n"),m_dwFieldSize);
	}
}
//////////////////////////////////////////////////////////////////////
CString CSequence::GetFileName(const CString& ext) const
{
	CCallStackEntry cse(_T(__FUNCTION__));
	// Beispiel d:\dbs\arch0001\00000001.dbf

	CString r;
	WORD wNr = Flags2Nr(m_bFlags);
	r.Format(szFileFormat,
			 m_sDrive,
			 m_pArchiv->GetDirectory(),
			 (WORD)wNr,
			 (WORD)GetNr(),
			 ext);
	return r;
}
//////////////////////////////////////////////////////////////////////
CString CSequence::GetDirectory() const
{
	CCallStackEntry cse(_T(__FUNCTION__));
	// Beispiel d:\dbs\arch0001

	CString r = GetFileName(szDbf);
	int p = r.ReverseFind('\\');
	r = r.Left(p);
	return r;
}
//////////////////////////////////////////////////////////////////////
CString CSequence::GetIndexDbi() const
{
	return GetDirectory() + '\\' + szDbi;
}
//////////////////////////////////////////////////////////////////////
BOOL CSequence::OpenDbf(BOOL bThrow/*=FALSE*/)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	m_iOpenDbf++;
	if (!IsOpenDbf())
	{
		CAutoCritSec acs(&m_cs);

		CString sDbf;
		int r;
		sDbf = GetFileName(szDbf);

		if (m_pData4 == NULL)
		{
			m_pData4 = new Data4();
		}
		r = m_pData4->open(m_pArchiv->m_cb, CWK_String(sDbf));
		if (r4success == r)
		{
			// success
			InitFields();
			m_dwNumRecords = m_pData4->recCount();
			if (   !IsAlarmList()
				&& !IsBackup())
			{
				ReadReferences();
			}
			if (theApp.TraceOpenClose())
			{
				WK_TRACE(_T("opened %s\n"),GetFileName(szDbf));
			}
		}
		else
		{
			m_iOpenDbf = 0;
			WK_TRACE_ERROR(_T("cannot open %s, %d\n"),sDbf,r);
			errorSet(0);
			WK_DELETE(m_pData4);
			CFileException fe(CFileException::generic, MAKELONG(0, r), sDbf);
			theApp.HandleFileException(&fe, bThrow);

			if (!DoesFileExist(sDbf))
			{
				WK_TRACE_ERROR(_T("file doesn't exist <%s>\n"),sDbf);
				CFileException fe(CFileException::badPath, ERROR_PATH_NOT_FOUND, sDbf);
				theApp.HandleFileException(&fe, bThrow);
			}
			else if (!IsBackup())
			{
				if (    r==e4data 
					&& !m_bLockedByBackup)
				{
					// file exists, so try to repair
					WK_TRACE_ERROR(_T("open failed %s\n"),sDbf);
					WK_TRACE(_T("repairing %s\n"),sDbf);
					if (r4success == repair(m_pArchiv->m_cb,CWK_String(sDbf)))
					{
						WK_TRACE(_T("repair success %s\n"),sDbf);
						m_pData4 = new Data4();
						r = m_pData4->open(m_pArchiv->m_cb,CWK_String(sDbf));
						if (r4success == r)
						{
							// success
							m_iOpenDbf++;
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
							WK_TRACE_ERROR(_T("cannot open after repair %s, %d\n"),sDbf,r);
							errorSet();
							WK_DELETE(m_pData4);
							CFileException fe(CFileException::generic, MAKELONG(0, r), sDbf);
							theApp.HandleFileException(&fe, bThrow);
						}
					}
					else
					{
						WK_TRACE_ERROR(_T("repair failed %s\n"),sDbf);
					}
				}
				else
				{
					WK_TRACE_ERROR(_T("no repair %s\n"),sDbf);
				}
			}
		}
		acs.Unlock();
	}
	return IsOpenDbf();
}
//////////////////////////////////////////////////////////////////////
BOOL CSequence::OpenDbd(BOOL bThrow/*=FALSE*/)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	if (!IsAlarmList())
	{
		m_iOpenDbd++;
		if (HasDbx())
		{
			return TRUE;
		}

		if (!IsOpenDbd())
		{
			CAutoCritSec acs(&m_cs);
			CFileException cfe;
			CString sDbd;
			UINT uFlags = 0;
			sDbd = GetFileName(CArchiv::m_sDbd);

			if (IsBackup())
			{
				uFlags = CFile::modeRead;
			}
			else
			{
				uFlags = CFile::modeReadWrite;
			}

			if (m_fData.Open(sDbd,uFlags,&cfe))
			{
				// OK
				if (theApp.TraceOpenClose())
				{
					WK_TRACE(_T("opened %s\n"),GetFileName(CArchiv::m_sDbd));
				}
				m_dwDatWritePosition = GetCurrentWritePosition();
			}
			else
			{
				m_iOpenDbd = 0;
				m_dwDatWritePosition = 0;
				WK_TRACE(_T("cannot open %s, %s\n"), sDbd, GetLastErrorString(cfe.m_lOsError));
				theApp.HandleFileException(&cfe, bThrow);
			}
			acs.Unlock();
		}
	}

	return IsOpenDbd();
}
//////////////////////////////////////////////////////////////////////
BOOL CSequence::IsOpen()
{
	return IsOpenDbf() && 
		(   IsAlarmList()
		|| IsOpenDbd());
}
//////////////////////////////////////////////////////////////////////
BOOL CSequence::Open(BOOL bThrow/*=FALSE*/)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	OpenDbf(bThrow);
	if (!IsAlarmList())
	{
		OpenDbd(bThrow);
	}
	return IsOpen();
}
//////////////////////////////////////////////////////////////////////
BOOL CSequence::ScanSize(BOOL bRoundUp)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CString sDbf,sDbd,sDbx;
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

	// dbd
	if (!IsAlarmList())
	{
		sDbd = GetFileName(CArchiv::m_sDbd);
		if (CFile::GetStatus(sDbd,cfs))
		{
			if (bRoundUp)
			{
				m_dwDbdSize = RoundUp(cfs.m_size,m_dwClusterSize);
			}
			else
			{
				m_dwDbdSize = cfs.m_size;
			}
		}
		else
		{
			m_dwDbxSize = 0;
		}

		// dbx
		sDbx = GetFileName(szDbx);
		if (CFile::GetStatus(sDbx,cfs))
		{
			if (bRoundUp)
			{
				m_dwDbxSize = RoundUp(cfs.m_size,m_dwClusterSize);
			}
			else
			{
				m_dwDbxSize = cfs.m_size;
			}
		}
		else
		{
			m_dwDbxSize = 0;
		}

		if (m_dwDbdSize + m_dwDbxSize == 0)
		{
			return FALSE;
		}

	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
BOOL CSequence::CloseDbf(BOOL bForce/*=FALSE*/, BOOL bThrow/*=FALSE*/)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	BOOL bRet = TRUE;
	
	CAutoCritSec acs(&m_cs);

	if (IsOpenDbf())
	{
		m_iOpenDbf--;

		if ((m_iOpenDbf==0) || bForce)
		{
			m_dwNumRecords = m_pData4->recCount();
			int ret = m_pData4->close();
			WK_DELETE(m_pData4);
			m_iOpenDbf = 0;
			if (ret == r4success)
			{
				if (theApp.TraceOpenClose())
				{
					WK_TRACE(_T("closed %s\n"),GetFileName(szDbf));
				}
				bRet = TRUE;
			}
			else
			{
				WK_TRACE_ERROR(_T("Data4::close failed A=%d S=%d, %d\n"), GetArchivNr(),GetNr(),ret);
				errorSet(0);
				bRet = FALSE;
				CFileException fe(CFileException::generic, MAKELONG(0, ret), GetFileName(szDbf));
				theApp.HandleFileException(&fe, bThrow);
			}
		}
		else
		{
			bRet = TRUE;
		}
	}

	acs.Unlock();

	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CSequence::CloseDbd(BOOL bForce /*= FALSE*/, BOOL bThrow/*=FALSE*/)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	BOOL bRet = TRUE;
	
	if (!IsAlarmList())
	{
		CAutoCritSec acs(&m_cs);

		m_iOpenDbd--;

		if (IsOpenDbd() && !HasDbx())
		{

			if ((m_iOpenDbd==0) || bForce)
			{
				TRY
				{
					m_fData.Close();
					m_dwDatWritePosition = 0;
				}
				CATCH(CFileException,cfe)
				{
					WK_TRACE_ERROR(_T("cannot close dat %s\n"),GetLastErrorString(cfe->m_lOsError));
					bRet = FALSE;
					theApp.HandleFileException(cfe, bThrow);
				}
				END_CATCH
				m_iOpenDbd = 0;
				if (bRet)
				{
					if (theApp.TraceOpenClose())
					{
						WK_TRACE(_T("closed %s\n"),GetFileName(CArchiv::m_sDbd));
					}
				}
			}
			else
			{
				bRet = TRUE;
			}
		}

		acs.Unlock();

	}
	return bRet;
}
//////////////////////////////////////////////////////////////////////
void CSequence::SetSequenceHashEntry()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	if (!IsBackup())
	{
		if (IsOpenDbf())
		{
			ReadTime(FALSE);
			m_dwNumRecords = m_pData4->recCount();
		}
		theApp.GetSequenceMap().SetSequenceHashEntry(GetArchivNr(),
			GetNr(),m_dwNumRecords,m_stFirst,m_stLast,m_sName);
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CSequence::Flush(BOOL bThrow/*=FALSE*/)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	BOOL bRet = TRUE;
	if (IsOpenDbf())
	{
		int r = m_pData4->flush();
		if (r4success==r)
		{
			if (theApp.TraceOpenClose())
			{
				WK_TRACE(_T("flushed %s\n"),GetFileName(szDbf));
			}
		}
		else
		{
			WK_TRACE_ERROR(_T("cannot flush dbf %d\n"),r);
			errorSet();
			bRet = FALSE;
			CFileException fe(CFileException::generic, MAKELONG(0, r), GetFileName(szDbf));
			theApp.HandleFileException(&fe, bThrow);
		}
	}
	if (IsOpenDbd())
	{
		TRY
		{
			m_fData.Flush();
			if (theApp.TraceOpenClose())
			{
				WK_TRACE(_T("flushed %s\n"),GetFileName(CArchiv::m_sDbd));
			}
		}
		CATCH(CFileException,cfe)
		{
			WK_TRACE_ERROR(_T("cannot flush dat %s\n"),GetLastErrorString(cfe->m_lOsError));
			bRet = FALSE;
			theApp.HandleFileException(cfe, bThrow);
		}
		END_CATCH
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
BOOL CSequence::Close(BOOL bForce /*= FALSE*/, BOOL bThrow/*=FALSE*/)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	BOOL bRet = TRUE;

	if (   bForce
		&& IsOpenDbf()
		&& m_iOpenDbf==1
		&& !m_bLockedByBackup)
	{
		SetSequenceHashEntry();
	}

	bRet &= CloseDbf(bForce, bThrow);
	if (!IsAlarmList())
	{
		bRet &= CloseDbd(bForce, bThrow);
	}

	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CSequence::SetName(const CString& sName)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	BOOL bRet = TRUE;

	m_sName = sName;

	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CSequence::Delete()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CAutoCritSec acs(&m_cs);

	Close(TRUE);

	BOOL bRet = FALSE;
	DWORD dwDeleteTick = 0;

	if (theApp.TraceDelete())
	{				
		dwDeleteTick = GetTickCount();
		WK_TRACE(_T("deleting %s\n"),GetFileName(szDbf));
	}
	if (DeleteFile(GetFileName(szDbf)))
	{
		if (theApp.TraceDelete())
		{
			WK_TRACE(_T("deleted %s\n"),GetFileName(szDbf));
		}
		if (IsAlarmList())
		{
			bRet = TRUE;
		}
		else
		{
			if (HasDbx())
			{
				if (theApp.TraceDelete())
				{
					WK_TRACE(_T("deleting %s\n"),GetFileName(szDbx));
				}
				if (DeleteFile(GetFileName(szDbx)))
				{
					if (theApp.TraceDelete())
					{
						WK_TRACE(_T("deleted %s\n"),GetFileName(szDbx));
					}
					bRet = TRUE;
				}
				else
				{
					WK_TRACE_ERROR(_T("cannot delete %s, %s\n"),GetFileName(CArchiv::m_sDbd),GetLastErrorString());
				}
			}
			else
			{
				if (theApp.TraceDelete())
				{
					WK_TRACE(_T("deleting %s\n"),GetFileName(CArchiv::m_sDbd));
				}
				if (DeleteFile(GetFileName(CArchiv::m_sDbd)))
				{
					if (theApp.TraceDelete())
					{
						WK_TRACE(_T("deleted %s\n"),GetFileName(CArchiv::m_sDbd));
					}
					bRet = TRUE;
				}
				else
				{
					WK_TRACE_ERROR(_T("cannot delete %s, %s\n"),GetFileName(CArchiv::m_sDbd),GetLastErrorString());
				}
			}
		}
	}
	else
	{
		WK_TRACE_ERROR(_T("cannot delete %s, %s\n"),GetFileName(szDbf),GetLastErrorString());
	}

	// only try it, may be it's empty
	CString sDir = GetDirectory();
	if (RemoveDirectory(sDir))
	{
		if (theApp.TraceDelete())
		{
			WK_TRACE(_T("removed %s\n"),sDir);
		}
		m_pArchiv->DeleteDirectory(sDir);
	}
	
	acs.Unlock();

	if (bRet && theApp.TraceSequence())
	{
		WK_STAT_PEAK(_T("Archiv"),3,m_pArchiv->GetName());
	}

	theApp.GetSequenceMap().DeleteSequenceHashEntry(GetArchivNr(),GetNr());

	if (theApp.TraceDelete())
	{
		WK_TRACE(_T("%s deleted in %d ms\n"),GetFileName(szDbf),GetTimeSpan(dwDeleteTick));
	}

	return bRet;
}
//////////////////////////////////////////////////////////////////////
void CSequence::ShrinkDatToFit()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CAutoCritSec acs(&m_cs);
	if (OpenDbd())
	{
		FILESIZE dwDatfileLength = RoundUp(m_dwDatWritePosition,m_dwClusterSize);
		if (dwDatfileLength==0)
		{
			TRACE(_T("dat file length is 0\n"));
		}
		TRY
		{
			FILESIZE dwCurrentLength = m_fData.GetLength();
			if (dwDatfileLength+128*1024<dwCurrentLength)
			{
#if _MFC_VER < 0x0700
				WK_TRACE(_T("shrinking file %s from %u to %u\n"),GetFileName(CArchiv::m_sDbd),dwCurrentLength,dwDatfileLength);
#else
				WK_TRACE(_T("shrinking file %s from %I64u to %I64u\n"),GetFileName(CArchiv::m_sDbd),dwCurrentLength,dwDatfileLength);
#endif
				m_fData.SetLength(dwDatfileLength);
			}
		}
		CATCH(CFileException, pCfe)
		{
			theApp.HandleFileException(pCfe, FALSE);
			WK_TRACE_ERROR(_T("cannot set length %s , %s\n"),
				GetFileName(CArchiv::m_sDbd),GetLastErrorString(pCfe->m_lOsError));
		}
		END_CATCH

		CloseDbd();
	}
	acs.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CSequence::AfterAppend(DWORD dwPictureLength,const CSystemTime& stStart,const CSystemTime& stEnd, DWORD dwNumPics)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	if (m_dwNumRecords==0)
	{
		// first picture set time stamp
		m_stFirst = stStart;
	}
	m_dwNumRecords += dwNumPics;
	m_dwDbfSize += (m_dwFieldSize*dwNumPics);
	m_dwDbdSize += dwPictureLength;
	m_stLast = stEnd;

}
//////////////////////////////////////////////////////////////////////
BOOL ReadDWORD(Data4& data, Code4& code_base, const char* szName,DWORD& dwValue)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	// Achtung alles nicht UNICODE sondern ASCII und das ist auch OK so
	Field4 f;
	int ret;
	ret = f.init(data,szName);
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
				TRACE(_T("cannot scan dword\n"));
			}
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		CString s(szName);
		WK_TRACE_ERROR(_T("Field4::read failed %s %d\n"),s,ret);
		code_base.errorSet(0);
	}
	return FALSE;
}
BOOL AssignInt(Data4& data, Code4& code_base, const char* szName, const int iValue)
{
	// Achtung alles nicht UNICODE sondern ASCII und das ist auch OK so
	CCallStackEntry cse(_T(__FUNCTION__));
	Field4 f;
	int ret;
	ret = f.init(data,szName);
	if (ret == r4success)
	{
		char szBuffer[9];
		sprintf(szBuffer,"%d\0",iValue);
		ret = f.assign(szBuffer);
		if (ret == r4success)
		{
			return TRUE;
		}
		else
		{
			CString s(szName);
			CString sBuffer(szBuffer);
			WK_TRACE_ERROR(_T("Field4::assign failed <%s>=<%s> %d\n"),
				s,sBuffer,ret);
			code_base.errorSet(0);
		}
	}
	else
	{
		CWK_String s(szName);
		WK_TRACE_ERROR(_T("Field4::init failed %s %d\n"),
			s,ret);
		code_base.errorSet(0);
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL AssignDWORD(Data4& data, Code4& code_base, const char* szName,const DWORD dwValue)
{
	// Achtung alles nicht UNICODE sondern ASCII und das ist auch OK so
	CCallStackEntry cse(_T(__FUNCTION__));
	Field4 f;
	int ret;
	ret = f.init(data,szName);
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
				CString s(szName);
				CString sBuffer(szBuffer);
				WK_TRACE_ERROR(_T("Field4::assign failed <%s>=<%s> %d\n"),
					s,sBuffer,ret);
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
		CWK_String s(szName);
		WK_TRACE_ERROR(_T("Field4::init failed %s %d\n"),
			s,ret);
		code_base.errorSet(0);
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL ReadWORD(Data4& data, Code4& code_base, const char* szName,WORD& wValue)
{
	// Achtung alles nicht UNICODE sondern ASCII und das ist auch OK so
	CCallStackEntry cse(_T(__FUNCTION__));
	Field4 f;
	int ret;
	ret = f.init(data,szName);
	if (ret == r4success)
	{
		if (f.len()>=4)
		{
			DWORD dwScan = 0;

			if (1==sscanf(f.str(),"%04hx",&dwScan))
			{
				wValue = (WORD)dwScan;
				return TRUE;
			}
			else
			{
				TRACE(_T("cannot scan word\n"));
			}
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		CString s(szName);
		WK_TRACE_ERROR(_T("Field4::read failed %s %d\n"),s,ret);
		code_base.errorSet(0);
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL AssignWORD(Data4& data, Code4& code_base, const char* szName,const WORD wValue)
{
	// Achtung alles nicht UNICODE sondern ASCII und das ist auch OK so
	CCallStackEntry cse(_T(__FUNCTION__));
	Field4 f;
	int ret;
	ret = f.init(data,szName);
	if (ret == r4success)
	{
		if (f.len()>=4)
		{
			char szBuffer[5];
			sprintf(szBuffer,"%04hx",wValue);
			szBuffer[4] = '\0';
			ret = f.assign(szBuffer);
			if (ret == r4success)
			{
				return TRUE;
			}
			else
			{
				CString s(szName);
				CString sBuffer(szBuffer);
				WK_TRACE_ERROR(_T("Field4::assign failed <%s>=<%s> %d\n"),
					s,sBuffer,ret);
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
		CWK_String s(szName);
		WK_TRACE_ERROR(_T("Field4::init failed %s %d\n"),
			s,ret);
		code_base.errorSet(0);
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL AssignField(Data4& data, Code4& code_base, const char* szName,const CString& value, BOOL bFillLeadingZeroes, BOOL bOverwrite)
{
	// TODO UNICODE
	CWK_String sValue;


	CCallStackEntry cse(_T(__FUNCTION__));
	Field4 f;
	int ret = f.init(data,szName);

	if (ret == r4success)
	{
		BOOL bIsEmpty = TRUE;
		if (!bOverwrite)
		{
			CStringA s(f.str());
			s.TrimLeft();
			bIsEmpty = s.IsEmpty();
		}

		if (bIsEmpty || bOverwrite)
		{
			if (bFillLeadingZeroes)
			{
				sValue = CIPCField::FillLeadingZeros(value,(WORD)f.len());
			}
			else
			{
				sValue = value;
			}
			if (!sValue.IsEmpty())
			{
		#ifdef _UNICODE
				if (f.type() == r4unicode)
				{
					f.assignUnicode(sValue);
				}
				else
				{
					CWK_String s(sValue);
					if (s.GetLength()<=(int)f.len())
					{
						ret = f.assign(s);
					}
					else
					{
	//					CWK_String sl(sValue.Left(f.len()));
						ret = f.assign(s,f.len());
					}
				}
		#else
				ret = f.assign(sValue);
		#endif
				if (ret == r4success)
				{
					return TRUE;
				}
				else
				{
					CWK_String sName(szName);
					WK_TRACE_ERROR(_T("Field4::assign CString failed <%s>=<%s>  %d\n"),
						sName,sValue,ret);
					if (theApp.GetDebugger())
					{
						theApp.GetDebugger()->Trace(GetCurrentThreadId());
					}
					code_base.errorSet(0);
				}
			}
			else
			{
				// empty value nothing todo
				return TRUE;
			}
		}
	}
	else
	{
		CString s((LPCSTR)szName);
		WK_TRACE_ERROR(_T("Field4::init failed %s %d\n"),
			s,ret);
		code_base.errorSet(0);
	}
	return FALSE;
}
BOOL AssignField(Data4& data, Code4& code_base,	const char* szName, const char* szValue)
{
	// TODO UNICODE
	CCallStackEntry cse(_T(__FUNCTION__));
	Field4 f;
	int ret;
	ret = f.init(data,szName);
	if (ret == r4success)
	{
		if (strlen(szValue)<=f.len())
		{
			ret = f.assign(szValue);
			if (ret == r4success)
			{
				return TRUE;
			}
			else
			{
				CString s(szName);
				CString value(szValue);
				WK_TRACE_ERROR(_T("Field4::assign const char failed <%s>=<%s>  %d\n"),
					s,value,ret);
				code_base.errorSet(0);
			}
		}
		else
		{
			CString s(szName);
			CString value(szValue);
			WK_TRACE_ERROR(_T("Field4::assign failed <%s>=<%s> value too long\n"),
				s,value);
			code_base.errorSet(0);
		}
	}
	else
	{
		CString s(szName);
		CString sValue(szValue);
		WK_TRACE_ERROR(_T("Field4::init failed <%s>=<%s> %d\n"),
			s,sValue,ret);
		code_base.errorSet(0);
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// die Rücksprung referenzen werden hier gesetzt
// es sind immer beide gesetzt
// eine 0 bedeutet, es gibt kein voriges Video/ Audio record
// z.B. beim ersten
void CSequence::AssignRWReferences(DWORD dwValueVideo,DWORD dwValueAudio)
{
	AssignDWORD(*m_pData4,m_pArchiv->m_cb,DBP_RWV_REF,dwValueVideo);
	AssignDWORD(*m_pData4,m_pArchiv->m_cb,DBP_RWA_REF,dwValueAudio);
}
//////////////////////////////////////////////////////////////////////
// Die Vorwärts referenzen können immer nur gesetzt werden,
// wen dann auch das passende Record gespeichert wird, dazu
// ist ein Rücksprung in der Tabelle nötig.
void CSequence::AssignFFReference(DWORD dwRecordNr, const char* szFieldName, DWORD dwValue)
{
	if (   dwRecordNr>0 
		&& szFieldName!=NULL)
	{
		DWORD dwCurrentRec = m_pData4->recNo();
		int ret = m_pData4->go(dwRecordNr);

		// auf record zurückspringen und reference setzen
		if (r4success == ret)
		{
			AssignDWORD(*m_pData4,m_pArchiv->m_cb,szFieldName,dwValue);
		}
		else
		{
			WK_TRACE_ERROR(_T("go failed %d C=%d, T=%d, %d\n"),dwRecordNr,GetArchivNr(),GetNr(),ret);
			errorSet();
		}
		// vor wieder zum aktuellen record
		ret = m_pData4->go(dwCurrentRec);
		if (r4success != ret)
		{
			WK_TRACE_ERROR(_T("go failed %d C=%d, T=%d, %d\n"),dwCurrentRec,GetArchivNr(),GetNr(),ret);
			errorSet();
		}
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CSequence::SaveData(CSaveData& data, WORD wPrevSequenceNr, DWORD dwNrOfRecords)
{
	if (IsAlarmList())
	{
		TRACE(_T("save alarm list %s\n"),data.GetTimeStamp().GetDateTime());
	}

	CCallStackEntry cse(_T(__FUNCTION__));
	CAutoCritSec acs(&m_cs);
	if (m_bLockedByBackup)
	{
		acs.Unlock();
		WK_TRACE(_T("sequence locked by backup %s\n"),GetFileName(szDbf));
		return FALSE;
	}

	DWORD dwCamID = data.GetDataID().GetID();

	if (  (m_dwLastVideo==0 && data.GetPictureRecord() && !data.IsIFrame())
		||(m_dwLastAudio==0 && data.GetMediaSampleRecord() && !data.IsIFrame())
		)
	{
		acs.Unlock();
		return TRUE;
	}

	BOOL bRet = TRUE;
	BOOL bOpen = IsOpen();

	if (!bOpen)
	{
		bOpen = Open(TRUE);
	}

	if (bOpen)
	{
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

		for (DWORD iPicture=0;iPicture<dwNumPictures;iPicture++)
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

				if (!IsAlarmList())
				{
					AssignDWORD(*m_pData4,m_pArchiv->m_cb,CArchiv::m_sCANR,dwCamID);
				}

				t = data.GetTimeStamp();

				if (iPicture==0)
				{
					stStart = t;
				}
				if (iPicture==dwNumPictures-1)
				{
					stEnd = t;
				}

				//TODO RKE/TKR
				//fields zu kein
				//TRACE ERROR:   Field4::assign failed <DBP_DATE>=<655356553565535> value too long
				//TRACE ERROR:   Field4::assign failed <DBP_TIME>=<655356553565535> value too long
				//TRACE ERROR:   Field4::assign failed <DBP_MS>=<65535> value too long
				// date
				char szDate[9];

				if (-1!=sprintf(szDate,"%04d%02d%02d\0",t.wYear,t.wMonth,t.wDay))
				{

					AssignField(*m_pData4,m_pArchiv->m_cb,
						CArchiv::m_sDATE,
						szDate);
				}
				else
				{
					WK_TRACE_ERROR(_T("sprintf FAILED on date %s\n"),t.GetDateTime());
				}
				// time
				char szTime[7];
				if (-1!=sprintf(szTime,"%02d%02d%02d",t.wHour,t.wMinute,t.wSecond))
				{
					AssignField(*m_pData4,m_pArchiv->m_cb,CArchiv::m_sTIME,szTime);
				}
				else
				{
					WK_TRACE_ERROR(_T("sprintf FAILED on time %s\n"),t.GetDateTime());
				}
				if (theApp.IsIDIP() || !IsAlarmList())
				{
					// ms
					char szMilli[4];
					if (-1!=sprintf(szMilli,"%03d\0",t.wMilliseconds))
					{
						AssignField(*m_pData4,m_pArchiv->m_cb,CArchiv::m_sMS,szMilli);
					}
					else
					{
						WK_TRACE_ERROR(_T("sprintf FAILED on ms %s\n"),t.GetDateTime());
					}
				}

				if (pPictureRecord)
				{
					CString s;
					// compression 2 Byte long
					if (data.GetPictureRecord()->GetCompression() != COMPRESSION_NONE)
					{
						s.Format(szIntFormat,(WORD)pPictureRecord->GetCompression());
					}
					AssignField(*m_pData4,m_pArchiv->m_cb,CArchiv::m_sCOMP,s,FALSE,TRUE);

					// resolution
					AssignInt(*m_pData4,m_pArchiv->m_cb,CArchiv::m_sRES,(BYTE)pPictureRecord->GetResolution());
					// compressiontyp
					AssignInt(*m_pData4,m_pArchiv->m_cb,CArchiv::m_sCOTY,(WORD)pPictureRecord->GetCompressionType());
					// picturetyp
					spt = pPictureRecord->GetPictureType();
					if (spt == SPT_GOP_PICTURE)
					{
						spt = (iPicture==0) ? SPT_FULL_PICTURE : SPT_DIFF_PICTURE;
					}
					AssignInt(*m_pData4,m_pArchiv->m_cb,CArchiv::m_sTYP,(int)spt);
				}
				else if (pMediaSampleRecord)
				{
					// CArchiv::m_sCOMP		2	verschiedene Compressionsraten
					// CArchiv::m_sRES		1	Auflösung
					// CArchiv::m_sCOTY	2	Bildtyp JPEG, H263
					// DBP_TYP				1	I, oder P Bild (Vollbild oder Differenz

					// we use image data field DBP_TYP to store
					// whether audio has long header info or not.
					// SPT_SUPER_BUBBLE=0, GOP
					// SPT_DIFF_PICTURE=1, P-frame 
					// SPT_FULL_PICTURE=2, I-frame or JPEG
					spt = pMediaSampleRecord->IsLongHeader() ? SPT_FULL_PICTURE : SPT_DIFF_PICTURE;
					AssignInt(*m_pData4,m_pArchiv->m_cb,CArchiv::m_sTYP,(BYTE)spt);
					AssignField(*m_pData4,m_pArchiv->m_cb,CArchiv::m_sCOTY,szMM);
					// achtung kein _T!
					AssignField(*m_pData4,m_pArchiv->m_cb,CArchiv::m_sRES,"0");
					AssignField(*m_pData4,m_pArchiv->m_cb,CArchiv::m_sCOMP,"0");

				}
				else
				{
					CIPCField* pTime = data.GetFields().GetCIPCField(CString(CArchiv::m_sTIME));
					CIPCField* pDate = data.GetFields().GetCIPCField(CString(CArchiv::m_sDATE));

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
					if (   pPictureRecord
						&& spt == SPT_FULL_PICTURE)
					{
						dwFFReferenceRecordVideo = m_dwLastVideo;
						m_dwLastVideo = m_dwNumRecords+1;
						dwFFReferenceValue = m_dwLastVideo;
					}
					// last audio reference
					if (   pMediaSampleRecord
						&& spt == SPT_FULL_PICTURE)
					{
						dwFFReferenceRecordAudio = m_dwLastAudio;
						m_dwLastAudio = m_dwNumRecords+1;
						dwFFReferenceValue = m_dwLastAudio;
					}
					AssignRWReferences(m_dwLastVideo,m_dwLastAudio);
				
					if (pPictureRecord)
					{
						pData = pPictureRecord->GetData(iPicture);
						dwDataLength = pPictureRecord->GetDataLength(iPicture);
					}
					else if (pMediaSampleRecord)
					{
						pData = data.GetData();
						dwDataLength = data.GetDataLength();
					}

					BYTE bCheckSum = 0;
					
					if (pData)
					{
						bCheckSum = CalcCheckSum(pData,dwDataLength);
						
						// assign the correct references
						AssignDWORD(*m_pData4,m_pArchiv->m_cb,CArchiv::m_sDPOS,(DWORD)dwPosition);
						dwPosition += dwDataLength;
						AssignDWORD(*m_pData4,m_pArchiv->m_cb,CArchiv::m_sDLEN,dwDataLength);
						char szCheckSum[3];
						sprintf(szCheckSum,"%02lx\0",bCheckSum);
						AssignField(*m_pData4,m_pArchiv->m_cb,CArchiv::m_sCSUM,szCheckSum);
					}

				} // !IsAlarmList 

				// write data
				for (int j=0; j<data.GetFields().GetSize();j++)
				{
					CIPCField* pField = data.GetFields().GetAtFast(j);
					BOOL bAssign = FALSE;

					CWK_String sFieldName(pField->GetName());
					if (theApp.IsIDIP())
					{
						sFieldName.Replace(_T("DVD_"), _T("DBD_"));
					}
					else if (theApp.IsDTS())
					{
						sFieldName.Replace(_T("DBD_"), _T("DVD_"));
					}
					// set camera name to map
					if (   pField->GetName() == CIPCField::m_sfCaNm
						|| sFieldName == CIPCField::m_sfCaNm)
					{
						m_pArchiv->SetCameraName(dwCamID,pField->GetValue());
					}

					if (   spt==SPT_FULL_PICTURE
						|| sFieldName != CString(CArchiv::m_sMD_S))
					{
						if (!pField->IsEmpty())
						{
							bAssign = AssignField(*m_pData4,m_pArchiv->m_cb,(LPCSTR)sFieldName,pField->GetValue(),pField->IsNumeric(),FALSE);
						}
					}
					if (   bAssign
						&& pField->GetName() == FIELD_DBD_POS)
					{
						data.GetFields().RemoveAt(j);
						j--;
						WK_DELETE(pField);
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
						GetArchivNr(),GetNr(),ret);
					bRet = FALSE;
					errorSet(0);
					break;
				}
			} // append start
			else
			{
				WK_TRACE_ERROR(_T("appendStart failed A=%d, S=%d, %d\n"),
					GetArchivNr(),GetNr(),ret);
				bRet = FALSE;
				errorSet(0);
				break;
			}
		}	// for
		
		if (r4success==ret)
		{
			AfterAppend(data.GetDataLength(),stStart,stEnd,dwNumPictures);
			if (m_dwNumRecords == dwNumPictures)
			{
				theApp.m_Clients.DoIndicateNewSequence(*this,wPrevSequenceNr,dwNrOfRecords);
			}
			bRet = TRUE;
		}
		
		if (   !IsAlarmList()
			&& ret == r4success)
		{
			const BYTE* pData = data.GetData();
			dwDataLength = data.GetDataLength();

			TRY
			{
				m_fData.Seek(m_dwDatWritePosition,CFile::begin);
				m_fData.Write(pData,dwDataLength);
				m_dwDatWritePosition += dwDataLength;
			}
			CATCH (CFileException, cfe)
			{
				theApp.HandleFileException(cfe, TRUE);
				bRet = FALSE;
				WK_TRACE(_T("cannot write picture %s %s\n"),
					GetFileName(CArchiv::m_sDbd),
					GetLastErrorString(cfe->m_lOsError));
			}
			END_CATCH
		}	// write data
	} // IsOpen
	else
	{
		bRet = FALSE;
	}
//	Unlock();
	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CSequence::Convert(const CConversion& cv)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	BOOL bRet = TRUE;
	CString sDbfPath,sDbxPath;

	sDbfPath = cv.GetDbfPath();
	sDbxPath = sDbfPath;
	sDbxPath.SetAt(sDbxPath.GetLength()-1,'x');

	m_sDrive = sDbfPath.Left(3);

	if (!CreateDir())
	{
		return FALSE;
	}

	// create only dbf, the dbx file will be reused in 4.0
	if (!CreateDbf())
	{
		return FALSE;
	}

	Data4 dbf;
	Code4 cb;
	int ret;

	cb.safety = FALSE;
	cb.errOff = TRUE;
	cb.readOnly = TRUE;
	cb.accessMode = OPEN4DENY_RW;
	cb.optimize = OPT4OFF;
	cb.optimizeWrite = OPT4OFF;

	ret = dbf.open(cb,CWK_String(sDbfPath));

	if (r4success == ret)
	{
		WK_TRACE(_T("CONVERT 3.x -> 4.0 %s with %d records\n"),
			sDbfPath,dbf.recCount());

		CSystemTime sTime;
		long lTyp;
		Compression comp = COMPRESSION_NONE;
		CIPCSavePictureType pt = SPT_FULL_PICTURE;
		lTyp = 0;

		for (dbf.top();!dbf.eof();dbf.skip())
		{
			CIPCFields fields;
			int iCam=0;
			BOOL bDate = FALSE;

			for (short i=1;i<=dbf.numFields();i++)
			{
				Field4 fField;
				ret = fField.init(dbf,i);
				if (ret != r4success)
				{
					cb.errorSet(0);
					continue;
				}

				bDate = FALSE;

				// de la rue data
				if (0==strcmp(fField.name(),"GA_NR"))
				{
					fields.Add(new CIPCField(CIPCField::m_sfGaNr,fField.str(),6,'N'));
				}
				else if (0==strcmp(fField.name(),"TA_NR"))
				{
					fields.Add(new CIPCField(CIPCField::m_sfTaNr,fField.str(),4,'N'));
				}
				else if (0==strcmp(fField.name(),"KTO_NR"))
				{
					fields.Add(new CIPCField(CIPCField::m_sfKtNr,fField.str(),16,'N'));
				}
				else if (0==strcmp(fField.name(),"BLZ"))
				{
					fields.Add(new CIPCField(CIPCField::m_sfBcNr,fField.str(),8,'N'));
				}
				else if (0==strcmp(fField.name(),"BETRAG"))
				{
					fields.Add(new CIPCField(CIPCField::m_sfAmnt,fField.str(),8,'N'));
				}
				else if (0==strcmp(fField.name(),"DTP_ZEIT"))
				{
					CString sDT(fField.str());
					sDT.TrimLeft();
					sDT.TrimRight();
					if (sDT.GetLength()==8)
					{
						sDT = sDT.Left(2) + sDT.Mid(3,2) + sDT.Mid(6);
						fields.Add(new CIPCField(CIPCField::m_sfTime,sDT,6,'C'));
						bDate = TRUE;
					}
				}
				else if (0==strcmp(fField.name(),"WAEHRUNG"))
				{
					fields.Add(new CIPCField(CIPCField::m_sfCurr,fField.str(),3,'C'));
				}
				else if (0==strcmp(fField.name(),"DATUM"))
				{
					DWORD dwYear, dwMonth, dwDay;
					sscanf(fField.str(),"%hu.%hu.%hu", &dwYear, &dwMonth, &dwDay);
					sTime.wYear  = (WORD)dwYear;
					sTime.wMonth = (WORD)dwMonth;
					sTime.wDay   = (WORD)dwDay;
					if (sTime.wYear<70)
					{
						sTime.wYear += 2000;
					}
					else
					{
						sTime.wYear += 1900;
					}
				}
				else if (0==strcmp(fField.name(),"ZEIT"))
				{
					DWORD dwHour, dwMinute, dwSecond;
					sscanf(fField.str(),"%hu:%hu:%hu", &dwHour, &dwMinute, &dwSecond);
					sTime.wHour   = (WORD)dwHour;
					sTime.wMinute = (WORD)dwMinute;
					sTime.wSecond = (WORD)dwSecond;
				}
				else if (0==strcmp(fField.name(),"COMP"))
				{
					sscanf(fField.str(),"%d",&comp);
				}
				else if (0==strcmp(fField.name(),"TYP"))
				{
					lTyp = (long)fField;
				}
				else if (0==strcmp(fField.name(),"VOLLBILD"))
				{
					if (0==strcmp(fField.str(),"1"))
					{
						pt = SPT_FULL_PICTURE;
					}
					else
					{
						pt = SPT_DIFF_PICTURE;
					}
				}
				else if (0==strcmp(fField.name(),"SKAMERA"))
				{
					iCam = atoi(fField.str());
				}
			}

			if (bDate)
			{
				CString sDate;
				sDate.Format(szDateFormat,sTime.GetYear(),
					sTime.GetMonth(),sTime.GetDay());
				fields.Add(new CIPCField(CIPCField::m_sfDate,sDate,8,'D'));
			}

			
			bRet &= SaveConvertPicture(sTime,
									   iCam,
									   comp,
									   GetResolution(lTyp),
									   GetCompressionType(lTyp),
									   pt,
									   fields);
		}
		if (bRet)
		{
			CString sDbx;
			sDbx = GetFileName(szDbx);
			if (!MoveFile(sDbxPath,sDbx))
			{
				WK_TRACE_ERROR(_T("MoveFile failed %s->%s, %s\n"),
								sDbxPath,sDbx,GetLastErrorString());
				bRet = FALSE;
			}
		}
		dbf.close();
	}
	else
	{
		WK_TRACE(_T("cannot open %s, %d\n"),sDbfPath,ret);
		cb.errorSet(0);
		bRet = FALSE;
	}
	if (cb.initUndo()<0)
	{
		WK_TRACE_ERROR(_T("initUndo failed, %d\n"),cb.errorCode);
		bRet = FALSE;
	}

	bRet &= CloseDbf();

	SetSequenceHashEntry();

	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CSequence::SaveConvertPicture(const CSystemTime& sysTime,
								   int iCameraNr,
								   Compression comp,
								   Resolution res,
								   CompressionType ct,
								   CIPCSavePictureType pt,
								   const CIPCFields& fields)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	BOOL bRet = TRUE;
	
	CAutoCritSec acs(&m_cs);
	
	if (!IsOpenDbf())
	{
		OpenDbf();
	}

	if (IsOpenDbf())
	{
		int ret = m_pData4->appendStart();
		if (r4success==ret)
		{
			m_pData4->blank();

			CString s,sDate,sTime;
				
			// datum
			AssignField(*m_pData4,m_pArchiv->m_cb,CArchiv::m_sDATE,sysTime.GetDBDate(),FALSE,TRUE);
			
			// zeit
			AssignField(*m_pData4,m_pArchiv->m_cb,CArchiv::m_sTIME,sysTime.GetDBTime(),FALSE,TRUE);

			// cam id
			CSecID idCam(SECID_GROUP_OUTPUT,(WORD)iCameraNr);
			AssignDWORD(*m_pData4,m_pArchiv->m_cb,DBP_CAMERANR,idCam.GetID());

			// compression
			if (comp != COMPRESSION_NONE)
			{
				s.Format(szIntFormat,(WORD)comp);
			}
			else
			{
				s.Empty();
			}
			s.Format(szIntFormat,(WORD)comp);
			AssignField(*m_pData4,m_pArchiv->m_cb,CArchiv::m_sCOMP,s,FALSE,TRUE);

			// resolution
			s.Format(szIntFormat,(BYTE)res);
			AssignField(*m_pData4,m_pArchiv->m_cb,CArchiv::m_sRES,s,FALSE,TRUE);

			// compressiontyp
			s.Format(szIntFormat,(WORD)ct);
			AssignField(*m_pData4,m_pArchiv->m_cb,CArchiv::m_sCOTY,s,FALSE,TRUE);

			// picturetyp
			s.Format(szIntFormat,(BYTE)pt);
			AssignField(*m_pData4,m_pArchiv->m_cb,CArchiv::m_sTYP,s,FALSE,TRUE);

			// last iframe reference
			if (SPT_FULL_PICTURE==pt)
			{
				m_dwLastVideo = m_dwNumRecords+1;
			}
			AssignDWORD(*m_pData4,m_pArchiv->m_cb,DBP_RWV_REF,m_dwLastVideo);

			AssignField(*m_pData4,m_pArchiv->m_cb,CArchiv::m_sDLEN,"00000000");
			AssignField(*m_pData4,m_pArchiv->m_cb,CArchiv::m_sDPOS,"00000000");


			// write data
			for (int i=0; i<fields.GetSize();i++)
			{
				CIPCField* pField = fields.GetAtFast(i);
				if (pField)
				{
					CWK_String sName(pField->GetName());
					AssignField(*m_pData4,
								m_pArchiv->m_cb,
						        sName,
								pField->GetValue(),
								pField->IsNumeric() && !pField->IsEmpty(),
								TRUE);
				}
			}

			// picture data, don't convert, read old dbx 
			ret = m_pData4->append();
			if (r4success==ret)
			{
				AfterAppend(0,sysTime,sysTime,1);
				bRet = TRUE;
			}
			else
			{
				WK_TRACE_ERROR(_T("append failed %d\n"),ret);
				errorSet(0);
			}
		}
		else
		{
			WK_TRACE_ERROR(_T("appendstart failed %d\n"),ret);
			errorSet(0);
		}
	}
	
	acs.Unlock();
	
	return bRet;
}
//////////////////////////////////////////////////////////////////////
CString CSequence::Format()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CString s;
	
	CAutoCritSec acs(&m_cs);

	s.Format(_T("arch%04lx, %04lx, NrP=%d D=%s S=%d"),
		m_pArchiv->GetNr(),GetNr(),m_dwNumRecords,m_stFirst.ToString(),
		GetSizeBytes());
	acs.Unlock();
	return s;
}
//////////////////////////////////////////////////////////////////////
void CSequence::Trace()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	if (theApp.TraceSequence())
	{
		WK_TRACE(_T("%s\n"),Format());
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CSequence::NewConfirmRecord(const DWORD dwCurrentRecordNr,
								 const DWORD dwNewRecordNr,
								 CSecID camID,
								 CIPCDatabaseServer* pCIPCDatabase)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	// attention we are in CIPCDatabaseServer thread
	BOOL bRet = FALSE;

	//TRACE(_T("RequestRecord S=%d %d,%d,%08lx\n"),GetNr(),dwCurrentRecordNr,dwNewRecordNr,camID.GetID());
	CAutoCritSec acs(&m_cs);

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
					if (dwRequestNr != 0)
					{
						TRACE(_T("typed data step    from %d to %d %d\n"), dwNewRecordNr, dwRequestNr,dwNewRecordNr-dwRequestNr);
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
				WK_TRACE_ERROR(_T("Data4::go failed %d A=%d, S=%d, %d\n"),
					dwNewRecordNr,GetArchivNr(),GetNr(),ret);
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
						DWORD dwID = GetDWORDValue(CArchiv::m_sCANR,(DWORD)-1);
						if (camID==dwID)
						{
							if (bIFrame)
							{
								dwRequestNr = GetIFrameRecordNr(dwNewRecordNr);
								if (dwRequestNr!=dwNewRecordNr)
								{
									TRACE(_T("I Frame step from %d -> %d\n"),
										dwNewRecordNr,dwRequestNr);
									ret = m_pData4->go(dwRequestNr);
									//camID = SECID_NO_ID;
								}
							}
	
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
								DWORD dwID = MAKELONG(GetNr(),GetArchivNr());
								pCIPCDatabase->DoIndicateError(dwID,
									camID.GetID(),CIPC_ERROR_INVALID_VALUE,
									1);
								bFound = FALSE;
								break; // leave the loop
							}
						}
					}
				}
				else if (camID.GetID() == (DWORD)(-1-NAVIGATION_LAST))
				{
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
							TRACE(_T("I Frame step from %d -> %d\n"),
								  dwNewRecordNr,dwRequestNr);
							ret = m_pData4->go(dwRequestNr);
							camID = SECID_NO_ID;
						}
					}
				}
				else if (camID.GetID() != (DWORD)(-1-NAVIGATION_ALL))
				{
					// search for 1,2,3 ...
					// skip to next DVD_MD_S value == _T('1'),_T('2'),_T('3') ...

					CStringA s;
					s.Format("%d",((DWORD)-1) - camID.GetID());
					TRACE(_T("searching for S VALUE %s at %d\n"),CString(s),m_pData4->recNo());

					while (!bFound)
					{
						Field4 mdsfield;
						if (r4success==mdsfield.init(*m_pData4,CArchiv::m_sMD_S))
						{
							if (0 == s.CompareNoCase(mdsfield.str()))
							{
								TRACE(_T("found S VALUE %s at %d\n"),CString(s), m_pData4->recNo());
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
										m_pArchiv->GetNr());
									pCIPCDatabase->DoIndicateError(dwID,
										camID.GetID(),CIPC_ERROR_INVALID_VALUE,
										bBackStep ? 6 : 5,_T("no more matching S record in sequence\n"));
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
				else 
				{
					// no cam id
					bFound = TRUE;
					if (bIFrame)
					{
						dwRequestNr = GetIFrameRecordNr(dwNewRecordNr);
						if (dwRequestNr!=dwNewRecordNr)
						{
							TRACE(_T("I Frame step from %d -> %d\n"),
								  dwNewRecordNr,dwRequestNr);
							ret = m_pData4->go(dwRequestNr);
							camID = SECID_NO_ID;
						}
					}
				}
				if (bFound)
				{
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
						if (pPR)
						{
	
							//TRACE(_T("confirm a=%d s=%d r=%d\n"),
							//	m_pArchiv->GetNr(),m_wID, dwRequestNr);
	
							pCIPCDatabase->DoConfirmRecordNr(m_pArchiv->GetNr(),
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
							pCIPCDatabase->DoConfirmRecordNr(m_pArchiv->GetNr(),
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
							DWORD dwID = MAKELONG(GetNr(),m_pArchiv->GetNr());
							pCIPCDatabase->DoIndicateError(dwID,
														   dwNewRecordNr,
														   CIPC_ERROR_INVALID_VALUE,
														   8);
							bRet = FALSE;
						}
					}
					else
					{
						DWORD dwID = MAKELONG(GetNr(),m_pArchiv->GetNr());
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
				errorSet(0);
				WK_TRACE_ERROR(_T("cannot goto record %d\n"),dwRequestNr);
			}
		}
		else
		{
			WK_TRACE_ERROR(_T("cannot open sequence for getting picture\n"));
		}
	}
	else
	{
		WK_TRACE_ERROR(_T("invalid record no %d / %d\n"),dwNewRecordNr,m_dwNumRecords);

		DWORD dwID = MAKELONG(GetNr(),GetArchivNr());
		pCIPCDatabase->DoIndicateError(dwID,dwNewRecordNr,CIPC_ERROR_INVALID_VALUE,2);
	}

	acs.Unlock();
	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CSequence::Move(CSequence& source)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	BOOL bRet = TRUE;
	CString sf,df;

	source.ShrinkDatToFit();
	source.Close(TRUE);

	CAutoCritSec acsS(&source.m_cs);
	CAutoCritSec acs(&m_cs);

	m_bFlags = source.m_bFlags;
	WK_CreateDirectory(GetDirectory());

	// dbf
	sf = source.GetFileName(szDbf); 
	df = GetFileName(szDbf);
	if (theApp.TracePreMove())
	{
		WK_TRACE(_T("MOVE %s --> %s\n"),sf,df);
	}
	if (!MoveFile(sf,df))
	{
		bRet = FALSE;
		WK_TRACE(_T("cannot move %s to %s because %s\n"),sf,df,GetLastErrorString());
	}

	// dbd
	sf = source.GetFileName(CArchiv::m_sDbd); 
	df = GetFileName(CArchiv::m_sDbd);
	if (theApp.TracePreMove())
	{
		WK_TRACE(_T("MOVE %s --> %s\n"),sf,df);
	}
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
		m_dwDbdSize = source.m_dwDbdSize;
	}

	acs.Unlock();
	acsS.Unlock();

	theApp.m_Clients.DoIndicateDeleteSequence(source);
	theApp.m_Clients.DoIndicateNewSequence(*this,0,0);

	return bRet;
}
//////////////////////////////////////////////////////////////////////
BYTE CSequence::CalcCheckSum(const BYTE* pData, DWORD dwDataLen)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	BYTE bCheckSum = 0;
	for (DWORD a=0;a<dwDataLen;a++)
	{
		bCheckSum ^= pData[a];
	}
	return bCheckSum;
}
//////////////////////////////////////////////////////////////////////
BOOL CSequence::GetRecord(DWORD dwRecNo, 
						  CIPCDatabaseServer* pCIPCDatabase, 
						  BOOL bFieldsAtPictureRecord,
						  CIPCPictureRecord*& pPR,
						  CIPCMediaSampleRecord*& pMSR,
						  CIPCFields& fields)
{
	if (IsAlarmList())
	{
		return FALSE;
	}
	CCallStackEntry cse(_T(__FUNCTION__));

	pPR = NULL;
	pMSR = NULL;
	CSecID id;
	Resolution res = RESOLUTION_NONE;
	Compression	comp = COMPRESSION_NONE;
	CompressionType ct = COMPRESSION_UNKNOWN;
	CIPCSavePictureType spt = SPT_FULL_PICTURE;
	BOOL  bNewFormat = FALSE;
	unsigned int iReadCheckSum	   = 0;
	DWORD dwLen = 0;
	DWORD dwPos = 0;
	PBYTE pBuffer = NULL;
	BOOL bIsDataAllowed = pCIPCDatabase->IsDataAllowed(m_pArchiv->GetNr());
	int ret;
	CSystemTime st;
	BOOL bIsMM = FALSE;

	CAutoCritSec acs(&m_cs);
	for (short i=1;i<=m_pData4->numFields();i++)
	{
		Field4 f;
		
		ret = f.init(*m_pData4,i);

		if (ret!=r4success)
		{
			errorSet(0);
			WK_TRACE_ERROR(_T("CSequence::GetRecord invalid field <%s>, %d\n"),
				f.name(),ret);
			continue;
		}

		if (0==strcmp(f.name(),CArchiv::m_sCANR))
		{
			DWORD dwID=0;
			if (1==sscanf(f.str(),SCAN_DWORD,&dwID))
			{
				id = dwID;
			}
			else
			{
				WK_TRACE_ERROR(_T("cannot read camera/media nr\n"));
			}
		}
		else if (0==strcmp(f.name(),CArchiv::m_sRES))
		{
			unsigned int iRes;
			if (1==sscanf(f.str(),SCAN_INT,&iRes))
			{
				res = (Resolution)iRes;
			}
			else
			{
				WK_TRACE_ERROR(_T("cannot read resolution\n"));
			}
		}
		else if (0==strcmp(f.name(),CArchiv::m_sCOMP))
		{
			DWORD dwComp;
			if (1==sscanf(f.str(),SCAN_INT,&dwComp))
			{
				comp = (Compression)dwComp;
			}
			else
			{
				comp = COMPRESSION_NONE;
			}
		}
		else if (0==strcmp(f.name(),CArchiv::m_sCOTY))
		{
			DWORD dwCT;
			if (0 == strcmp(f.str(),szMM))
			{
				// is a audio/multimedia bubble
				bIsMM = TRUE;
			}
			else
			{
				if (1==sscanf(f.str(),SCAN_INT,&dwCT))
				{
					ct = (CompressionType)dwCT;
				}
				else
				{
					WK_TRACE_ERROR(_T("cannot read compression typ\n"));
				}
			}
		}
		else if (0==strcmp(f.name(),CArchiv::m_sTYP))
		{
			unsigned int iSPT;
			if (1==sscanf(f.str(), SCAN_INT, &iSPT))
			{
				spt = (CIPCSavePictureType)iSPT;
			}
			else
			{
				WK_TRACE_ERROR(_T("cannot read picture type\n"));
			}
		}
		else if (0==strcmp(f.name(),CArchiv::m_sDATE))
		{
			st.SetDBDate(f.str());
		}
		else if (0==strcmp(f.name(),CArchiv::m_sTIME))
		{
			st.SetDBTime(f.str());
		}
		else if (0==strcmp(f.name(),CArchiv::m_sMS))
		{
			DWORD ms = 0;
			if (1==sscanf(f.str(),"%03d",&ms))
			{
				st.wMilliseconds = (WORD)ms;
			}
		}
		else if (0==strcmp(f.name(),CArchiv::m_sDPOS))
		{
			if (1==sscanf(f.str(),SCAN_DWORD,&dwPos))
			{
			}
			else
			{
				WK_TRACE(_T("cannot read off\n"));
			}
		}
		else if (0==strcmp(f.name(),CArchiv::m_sDLEN))
		{
			if (1==sscanf(f.str(),SCAN_DWORD,&dwLen))
			{
			}
			else
			{
				WK_TRACE(_T("cannot read len\n"));
			}
		}
		else if (0==strcmp(f.name(),CArchiv::m_sCSUM))
		{
			if (1==sscanf(f.str(),SCAN_BYTE, &iReadCheckSum))
			{
				bNewFormat = TRUE;
			}
			else
			{
				WK_TRACE(_T("cannot scan check sum\n"));
			}
		}
		else
		{
			if (bIsDataAllowed)
			{
				if (   0==strncmp("DBD_", f.name(),4)
					|| 0==strncmp("DVD_", f.name(),4))
				{
					CString sName(f.name());
					CIPCField* pDefField = theApp.m_Fields.GetCIPCField(sName);
					CString sValue;
					WORD wLen;
					char cType = _T('C');

#ifdef _UNICODE
					if (f.type() == r4unicode)
					{
						wLen = (WORD)(f.len()/2);
						LPCWSTR pStr = (LPCWSTR)f.ptr();

						if (pStr[0])
						{
							sValue = pStr;
						}
					}
					else
					{
						sValue = CString(f.str());
						wLen = (WORD)sValue.GetLength();
					}
					//TRACE(_T("%s=%s\n"),sName,sValue);
#else
					sValue = f.str();
					wLen = (WORD)f.len();
#endif
					if (pDefField)
					{
						cType = pDefField->GetType();
					}
					if (!sName.IsEmpty() && !sValue.IsEmpty() && (wLen>0))
					{
						fields.SafeAdd(new CIPCField(sName,sValue,wLen,cType));
					}
				}
				else if (0==strncmp("DBP_", f.name(),4))
				{
					// nothing todo
				}
				else if (0==strncmp("DVR_", f.name(),4))
				{
					// nothing todo
				}
				else
				{
					TRACE(_T("unknown field %s\n"),CString(f.name()));
				}
			}
		}
	} // for

	BOOL bRead = TRUE;

	// may be old data
	if (HasDbx())
	{
		COldPictData dbx;
		CString sDbx;
		sDbx = GetFileName(szDbx);
		if (dbx.MyOpen(sDbx))
		{
			dwLen = dbx.GetPictLen(dwRecNo);
			pBuffer = new BYTE[dwLen];
			dbx.GetPictData(dwRecNo,pBuffer);
			dbx.Close();
		}
	}
	else
	{
		pBuffer = new BYTE[dwLen];
		TRY
		{
			m_fData.Seek(dwPos,CFile::begin);
			m_fData.Read(pBuffer,dwLen);
		}
		CATCH(CFileException, pCfe)
		{
			theApp.HandleFileException(pCfe, FALSE);
			bRead = FALSE;
			WK_TRACE_ERROR(_T("cannot read picture %s, %s\n"),
				GetFileName(CArchiv::m_sDbd),GetLastErrorString(pCfe->m_lOsError));
		}
		END_CATCH
	}
	
	if (bRead)
	{
//		const char* pData = NULL;
		CString sData;

		if (bFieldsAtPictureRecord && bIsDataAllowed)
		{
			if (pCIPCDatabase->GetVersion()<1)
			{
				sData = fields.ToGeld();
			}
			else if (pCIPCDatabase->GetVersion()<3)
			{
				sData = fields.ToSerial();
			}
			WK_TRACE(_T("<%s> %d\n"),sData,sData.GetLength());
//			pData = (const char*)sData;
		}

		if (bNewFormat)
		{
			BYTE bCalcCheckSum = CalcCheckSum(pBuffer,dwLen);

			if (iReadCheckSum != (unsigned int)bCalcCheckSum)
			{
				WK_TRACE_ERROR(_T("invalid image checksum %d, %d, %d\n"),
					GetArchivNr(),GetNr(),dwRecNo);
				fields.Add(new CIPCField(DVD_ERROR,_T("CHECKSUM"),_T('C')));
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
	acs.Unlock();
	
	return (pPR!=NULL) || (pMSR !=NULL);
}
//////////////////////////////////////////////////////////////////////
BOOL CSequence::GetFields(DWORD dwRecordNr, CIPCFields& fields)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	BOOL bRet = FALSE;

	CAutoCritSec acs(&m_cs);

	if (OpenDbf())
	{
		if (r4success == m_pData4->go(dwRecordNr))
		{
			int c,ret;
			c = m_pData4->numFields();
			for (short i=1;i<=c;i++)
			{
				Field4 f;
				
				ret = f.init(*m_pData4, i);

				if (ret == r4success)
				{
					if (0==strncmp("DBD_",f.name(),4))
					{
						CIPCField* pField = theApp.m_Fields.GetCIPCField(CWK_String(f.name()));

						char cType = _T('C');
						if (pField)
						{
							cType = pField->GetType();
						}
						fields.SafeAdd(new CIPCField(f.name(),
													 CWK_String(f.str()),
													 (WORD)f.len(),
													 cType));
					}
				}
				else
				{
					WK_TRACE_ERROR(_T("Field4::init failed A=%d, S=%d, %d\n"),
						GetArchivNr(),GetNr(),ret);
					errorSet(0);
				}
			} // for
			bRet = TRUE;
		}
		else
		{
			errorSet(0);
		}
		CloseDbf();
	} // IsOpen
	acs.Unlock();
	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CSequence::GetFieldsAndRecords(DWORD dwFirstRecordNr,
							 DWORD dwLastRecordNr,
							 CIPCFields& fields,
							 CIPCFields& records)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CAutoCritSec acs(&m_cs);

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
		for (DWORD dwRecordNr = dwFirstRecordNr;dwRecordNr<=dwLastRecordNr;dwRecordNr++)
		{
			if (r4success == m_pData4->go(dwRecordNr))
			{
				short i;
				int ret,c;
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
								m_pArchiv->GetNr(),GetNr(),ret);
							errorSet();
						}
					} // loop over fields
				}
				CString sNr;
				sNr.Format(_T("%d"),dwRecordNr);
//				TRACE(_T("record %d=<%s> %d Bytes long\n"),dwRecordNr,sValue,sValue.GetLength());
				CIPCField* pField = new CIPCField;
				pField->SetName(sNr);
				int iRecordOffset = 1;
				if (theApp.IsDTS())
				{
					iRecordOffset = 3;
				}
				CWK_String sValue(m_pData4->record()+iRecordOffset);
				pField->SetValue(sValue);
				pField->SetMaxLength((WORD)sValue.GetLength());
				pField->SetType(_T('R'));
				records.SafeAdd(pField);
			} // go to record
			else
			{
				errorSet();
			}
		} // loop over records
		CloseDbf();
	}

	acs.Unlock();

	return records.GetSize()>0;
}
/////////////////////////////////////////////////////////////////////////////
/*
CString CSequence::GetCameraIDs()
{
	CString sIDs;
	if (m_dwNumRecords > 0)
	{
		CAutoCritSec acs(&m_cs);
		if (OpenDbf())
		{
			Relate4set relation;
			BOOL bRelInit = FALSE;
			try
			{
				int ret, rc;
				long nRecNo = 1;
				ret = m_pData4->go(nRecNo);
				if (ret != r4success) throw 1;

				Field4 f;
				ret = f.init(*m_pData4, CArchiv::m_sCANR);
				if (ret != r4success) throw 2;
				if (f.isValid())
				{
					CWK_String str(f.str());
					sIDs = str;
				}

				ret = relation.init(*m_pData4);
				if (ret != r4success) throw 3;
				bRelInit = TRUE;

				CWK_String sQueryString;
				sQueryString.Format(_T("(%s<>'%s'"), CString(CArchiv::m_sCANR), sIDs);
				ret = relation.querySet(sQueryString);
				if (ret != r4success) throw 0;
				
				for (rc = relation.top(); rc == r4success; rc=relation.skip(1))
				{
					nRecNo = m_pData4->recNo();
					ret = f.init(*m_pData4, CArchiv::m_sCANR);
					if (ret == r4success)
					{
						CWK_String str(f.str());
						sIDs += _T(";") + str;
					}
					else
					{
						throw 5;
					}
				}
				errorSet();
			}
			catch (int nError)
			{
				if (nError)
				{
					WK_TRACE(_T("Error GetCameraIDs: %d\n"), nError);
				}
				errorSet();
			}

			if (bRelInit)
			{
				relation.free();
			}
			CloseDbf();
		}
		else
		{
			errorSet(0);
		}
		acs.Unlock();
	}
	return sIDs;
}
*/
//////////////////////////////////////////////////////////////////////
BOOL CSequence::Query(CQuery& q)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	BOOL bRet = TRUE;

	CAutoCritSec acs(&m_cs);

	if (m_dwNumRecords>0)
	{
		// perform Time Check to get faster queries
		BOOL bMustSearch = CalcDateTimeOpt(q);

		if (bMustSearch)
		{
			//TRACE(_T("QUERY %d,%d\n"),GetArchivNr(),GetNr());
			if (OpenDbf())
			{
				Relate4set relation;
				int rc = 0;
				int ret = 0;
				DWORD dwQueryRecNo;

				ret = relation.init(*m_pData4);
				if (ret == r4success)
				{
					CWK_String sQueryString = q.GetExpression();
					rc = relation.querySet(sQueryString);
					if (rc==r4success)
					{
						int iSkip = 1;
						if (q.IsForward())
						{
							rc = relation.top();
							iSkip = 1;
						}
						else
						{
							rc = relation.bottom();
							iSkip = -1;
						}

						DWORD dwIFrameNr = 0;

						for (; rc == r4success; rc=relation.skip(iSkip))
						{
							// we are now on an found record send it
							dwQueryRecNo = m_pData4->recNo();
							Field4 fieldTyp(*m_pData4,CArchiv::m_sTYP);

							if (fieldTyp.isValid())
							{
								unsigned int iSPT = 0;
								CIPCSavePictureType spt = SPT_FULL_PICTURE;
								if (1==sscanf(fieldTyp.str(),SCAN_INT,&iSPT))
								{
									spt = (CIPCSavePictureType)iSPT;

									if (spt == SPT_DIFF_PICTURE)
									{
										if (dwIFrameNr == 0)
										{
											DWORD dwIFrameRef = GetIFrameRecordNr(dwQueryRecNo);
											for (DWORD dw=dwIFrameRef;dw<dwQueryRecNo;dw++)
											{
												TRACE(_T("add I/P frame query result %d\n"),dw);
												q.AddQueryResult(this,
																	CDBSId(GetArchivNr(),GetNr(),dw)
																	);
											}
											dwIFrameNr = dwIFrameRef;
										}
									}
									else
									{
										dwIFrameNr = dwQueryRecNo;
									}
								}
							}
							else
							{
								WK_TRACE_ERROR(_T("cannot read field TYP during query\n"));
								errorSet();
							}


							if (q.AddQueryResult(this,
												 CDBSId(GetArchivNr(),GetNr(),dwQueryRecNo)))
							{
								// continue searching still connected
							}
							else
							{
								bRet = TRUE;
								//WK_TRACE(_T("query result and disconnect or cancelled\n"));
								break;
							}
						}
						errorSet(0);
					}
					else
					{
						WK_TRACE_ERROR(_T("querySet failed A=%d, S=%d, %d\n"),GetArchivNr(),GetNr(),rc);
						errorSet(0);
					}
				}
				else
				{
					WK_TRACE_ERROR(_T("Relate4set::init failed A=%d, S=%d, %d\n"),GetArchivNr(),GetNr(),rc);
					errorSet(0);
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

	acs.Unlock();

	return bRet;
}
//////////////////////////////////////////////////////////////////////
DWORD CSequence::GetIFrameRecordNr(DWORD dwRecordNr)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CAutoCritSec acs(&m_cs);

	DWORD dwIFrame = dwRecordNr;
	int ret;
	CIPCSavePictureType eSavePictureType;

	// for delivering search results
	// still no i/p frame support
	Field4 fieldPictureType;
	Field4 fieldCompressionType;

	ret = fieldCompressionType.init(*m_pData4,CArchiv::m_sCOTY);
	if (ret == r4success)
	{
		CString sIReferenceFieldName(_T(DBP_RWV_REF));

		// wenn das ein Audio record ist die andere Referenz benutzen.
		if ((strcmp(szMM, fieldCompressionType.str()) == 0))
		{
			sIReferenceFieldName = _T(DBP_RWA_REF);
		}

		ret = fieldPictureType.init(*m_pData4,CArchiv::m_sTYP);

		if (ret == r4success)
		{
			unsigned int iSPT=0;
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
			WK_TRACE_ERROR(_T("Field4::init failed A=%d S=%d, %d\n"),GetArchivNr(),GetNr(),ret);
			errorSet();
		}
	}
	else
	{
		WK_TRACE_ERROR(_T("Field4::init failed A=%d S=%d, %d\n"),GetArchivNr(),GetNr(),ret);
		errorSet(0);
	}
	acs.Unlock();
	return dwIFrame;
}
//////////////////////////////////////////////////////////////////////////
DWORD CSequence::GetSpecificFrameRecordNr(CSecID camID, DWORD dwRecordNr, BOOL bBackStep)
{
	Field4 fieldCompressionType;
	CWK_String sMM(szMM);
	int ret =0;
	CompressionType ct;
	do 
	{
		ret = fieldCompressionType.init(*m_pData4, CArchiv::m_sCOTY);
		if (ret != r4success)
		{
			errorSet();
			break;
		}
		else if (camID.IsMediaID() && strcmp(sMM, fieldCompressionType.str()) == 0)
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
	} 
	while(ret == r4success);

	errorSet();

	return 0;
}
//////////////////////////////////////////////////////////////////////
DWORD CSequence::GetTypedRecordNr(CSecID camID, DWORD dwRecordNr, BOOL bBackStep)
{
	DWORD dwReturn = dwRecordNr;
	Field4 field;
	Field4 fieldCompressionType;
	int ret = r4success;
	BOOL bIsMedia = FALSE;

	ret = fieldCompressionType.init(*m_pData4, CArchiv::m_sCOTY); 
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
					CWK_String sFieldNameRW;
					if (camID.IsMediaID())
					{
						sFieldNameRW = _T(DBP_RWA_REF);
					}
					else if (camID.IsOutputID())
					{
						sFieldNameRW = _T(DBP_RWV_REF);
					}
					Field4 fieldRW;
					DWORD dwRW = 0;

					if (r4success == fieldRW.init(*m_pData4, sFieldNameRW))
					{
						if (1==sscanf(fieldRW.str(),SCAN_DWORD,&dwRW))
						{
							if (r4success == m_pData4->go(dwRW))
							{
								// Achtung das go hat das Feld mit umgesetzt
								if (1==sscanf(field.str(),SCAN_DWORD,&dwReturn))
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
								errorSet();
							}
						}
					}
					else
					{
						errorSet();
					}
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
		// richtiger Typ
		dwReturn = dwRecordNr;
	}


	return dwReturn;
}
//////////////////////////////////////////////////////////////////////
BOOL CSequence::Backup(CBackup& b, BOOL& bErrorSharingViolation)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	BOOL bRet = TRUE;

	CAutoCritSec acs(&m_cs);
	m_bLockedByBackup = TRUE;
	if (IsOpen())
	{
		if (theApp.TraceBackup())
		{
			WK_TRACE(_T("close sequence by backup %04hx,%d\n"),GetArchivNr(),GetNr());
		}
		Close(TRUE);
	}
	acs.Unlock();

	if (theApp.TraceBackup())
	{
		WK_TRACE(_T("backup archive <%s> sequence %d to <%s>\n"),m_pArchiv->GetName(),GetNr(),b.GetDestinationPath());
	}

	CString sDir;

	// create the directory
	sDir.Format(_T("%s\\arch%04lx"),b.GetDirectory(),m_pArchiv->GetNr());

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
			//if a network backup drive was missing (network was disconnected) the DBS deletes
			//this drive from the list (m_Drives).
			//when the network backup drive is connected again the DBS doesn`t recognize that
			//because a polling to a not connected network drive lasts 
			//up to 1 min because of network timeout and we are not polling.
			//when the next automatic backup starts and can create a directory on the network backup
			//drive, we know that the network drive is connected again. 
			//So we have to put this drive back into the drive list (m_Drives)

			CString sPath = b.GetDestinationPath();
			int c = theApp.m_Drives.GetSize();
			BOOL bFound = FALSE;
			CIPCDrive* pDrive = NULL;
			for (int i=0;i<c;i++)
			{
				pDrive = theApp.m_Drives.GetAtFast(i);
				
				if (   pDrive
					&& (pDrive->IsReadBackup() || pDrive->IsWriteBackup()))
				{
					CString sRoot = pDrive->GetRootString();
					if(sPath == sRoot)
					{
						bFound = TRUE;
						break; //everything is ok, network backup drive is already in list
					}
				}
			}
			if(!bFound)
			{
				char cLetter = (char)sPath.GetAt(0);
				CWK_Profile wkp;
				CIPCDrive* pNetworkBackupDrive = new CIPCDrive(cLetter, wkp);
				//add the network backup drive to the list
				theApp.m_Drives.Add(pNetworkBackupDrive);
			}

			if (theApp.TraceBackup())
			{
				WK_TRACE(_T("mdir SUCCESS %s\n"),sDir);
			}
		}

	}

	if (bRet)
	{
		CString sDbf,sDbd; // source names
		CString dDbf,dDbd; // destination names

		sDbf = GetFileName(szDbf);
		sDbd = GetFileName(CArchiv::m_sDbd);

		dDbf.Format(_T("%s\\%04lx%04lx.%s"),sDir,(WORD)Flags2Nr((BYTE)(m_bFlags|CAR_IS_BACKUP)),GetNr(),szDbf);
		dDbd.Format(_T("%s\\%04lx%04lx.%s"),sDir,(WORD)Flags2Nr((BYTE)(m_bFlags|CAR_IS_BACKUP)),GetNr(),CArchiv::m_sDbd);

		// now copy the files
		bRet = b.CopyFileTrace(GetArchivNr(),GetNr(),sDbf,dDbf);
		if(    !bRet 
			&& GetLastError() == ERROR_SHARING_VIOLATION)
		{
			bErrorSharingViolation = TRUE;
		}

		//if copy failed, delete the not complete copied dbf-file from backup drive
		if(!bRet)
		{
			if(DeleteFile(dDbf))
			{
				WK_TRACE(_T("deleted %s\n"),dDbf);
			}
		}

		if (   !IsAlarmList()
			&& bRet)
		{
			FILESIZE dwDataLen = GetCurrentWritePosition();
			bRet = b.CopyFileTrace(GetArchivNr(),GetNr(),sDbd,dDbd,dwDataLen);
			//if copy failed, delete the not complete copied dbd and dbf file from backup drive
			if(!bRet)
			{
				if(DeleteFile(dDbd))
				{
					WK_TRACE(_T("deleted %s\n"),dDbd);
				}
				else
				{
					WK_TRACE_ERROR(_T("cannot delete %s, %s\n"),dDbd,GetLastErrorString());
				}
				if(DeleteFile(dDbf))
				{
					WK_TRACE(_T("deleted %s\n"),dDbf);
				}
				else
				{
					WK_TRACE_ERROR(_T("cannot delete %s, %s\n"),dDbf,GetLastErrorString());
				}
			}
		}
	}


	CAutoCritSec acs2(&m_cs);
	m_bLockedByBackup = FALSE;
	acs2.Unlock();

	if (bRet)
	{
		b.GetSequenceMap().SetSequenceHashEntry(GetArchivNr(),GetNr(),m_dwNumRecords,m_stFirst,m_stLast,m_sName);
	}

	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CSequence::CalcDateTimeOpt(CQuery& query)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	BOOL bRet = TRUE;
	CSystemTime stEmpty;

	if (!(stEmpty == query.GetStartTime()))
	{
		if (m_stLast < query.GetStartTime())
		{
/*
			TRACE(_T("don't query by start time %s < %s %d,%d\n"),
				m_stLast.GetDateTime(),query.GetStartTime().GetDateTime(),GetArchivNr(),GetNr());
*/
			bRet = FALSE;
		}
	}
	if (!(stEmpty == query.GetEndTime()))
	{
		if (m_stFirst > query.GetEndTime())
		{
/*
			TRACE(_T("don't query by end time %s < %s %d,%d\n"),
				m_stFirst.GetDateTime(),query.GetEndTime().GetDateTime(),GetArchivNr(),GetNr());
*/
			bRet = FALSE;
		}
	}

	if (bRet)
	{
		//TRACE(_T("query time ok %s-%s %d,%d\n"),
		//	m_stFirst.GetDateTime(),m_stLast.GetDateTime(),GetArchivNr(),GetNr());
	}

	return bRet;
}
//////////////////////////////////////////////////////////////////////
DWORD CSequence::GetRecordNr(const CSystemTime& st, BOOL bGreaterThan)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	DWORD dwRet = 0;

	CAutoCritSec acs(&m_cs);

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
					CString(CArchiv::m_sTIME),sOp,sTime,
					CString(CArchiv::m_sDATE),sDate,
					CString(CArchiv::m_sDATE),sOp,sDate);

				rc = relation.querySet(sExpression);
				
/*
				TRACE(_T("Startzeit=%s,Endzeit=%s,Suchzeit=%s Op=%s\n"),
					m_stFirst.GetDateTime(),
					m_stLast.GetDateTime(),
					st.GetDateTime(),
					sOp);
				TRACE(_T("expression = %s\n"),sExpression);
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
						TRACE(_T("GetRecordNr cannot top relation %s, %d\n"),sExpression,
							rc);
						dwRet = 0;
						errorSet();
					}
				}
				else
				{
					TRACE(_T("GetRecordNr cannot set Query %s\n"),sExpression);
					dwRet = 0;
					errorSet();
				}
			}
			else
			{
				WK_TRACE_ERROR(_T("Relate4set::init failed C=%d, T=%d, %d\n"),
					GetArchivNr(),GetNr(),rc);
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

	acs.Unlock();

	return dwRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CSequence::GetOffsetAndLength(DWORD dwRecordNr, FILESIZE& dwOffset, DWORD& dwLen)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	DWORD dwOffsetLocal,dwLenLocal;

	dwOffsetLocal = 0;
	dwLenLocal = 0;
	CAutoCritSec acs(&m_cs);
	BOOL bRet = TRUE;

	if (OpenDbf())
	{
		int ret = m_pData4->go(dwRecordNr);
		if (r4success == ret)
		{
			bRet = ReadDWORD(*m_pData4,GetCode4(),CArchiv::m_sDLEN,dwLenLocal);
			bRet = bRet && ReadDWORD(*m_pData4,GetCode4(),CArchiv::m_sDPOS,dwOffsetLocal);
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

	acs.Unlock();

	dwLen = dwLenLocal;
	dwOffset = dwOffsetLocal;

	return bRet;
}
//////////////////////////////////////////////////////////////////////
FILESIZE CSequence::GetCurrentWritePosition()
{
	CCallStackEntry cse(_T(__FUNCTION__));
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
		WK_TRACE(_T("cannot open dbf file to get current write position %s\n"),GetFileName(szDbf));
	}

	return dwRet;
}
//////////////////////////////////////////////////////////////////////
DWORD CSequence::GetDWORDValue(const char* szFieldName, DWORD dwDefault)
{
	CCallStackEntry cse(_T(__FUNCTION__));
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
void CSequence::ReadReferences()
{
	CCallStackEntry cse(_T(__FUNCTION__));
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
	else
	{
		m_dwLastVideo  = 0;
		m_dwLastAudio  = 0;
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CSequence::IsFull(DWORD dwSizeOfNextPicture)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	if (IsAlarmList())
	{
		return m_dwNumRecords >= 1024;
	}
	else
	{
		return CIPCInt64(m_dwDatWritePosition+dwSizeOfNextPicture) > m_pArchiv->GetSequenceSizeBytes();
	}
}
/////////////////////////////////////////////////////////////////////////////////
BOOL CSequence::BackupByTime(CBackup& b, BOOL& bSequenceNotInTimeSpan, BOOL& bErrorSharingViolation)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	BOOL bRet = TRUE;
	DWORD dwCopiedBytes = 0;
	DWORD dwNumRecords = 0;
	CSystemTime stFirst,stLast;

	CAutoCritSec acs(&m_cs);
	m_bLockedByBackup = TRUE;
	if (IsOpen())
	{
		if (theApp.TraceBackup())
		{
			WK_TRACE(_T("close sequence by backup %04hx,%d\n"),GetArchivNr(),GetNr());
		}
		Close(TRUE);
	}
	acs.Unlock();

	if (theApp.TraceBackup())
	{
		WK_TRACE(_T("BackupByTime archive <%s> sequence %d to <%s>\n"),
			m_pArchiv->GetName(),GetNr(),b.GetDestinationPath());
	}
	
	if (m_stFirst > b.GetEndTime())
	{
		// Sequence is too young
		if (theApp.TraceBackup())
		{
			WK_TRACE(_T("Sequence %04hx is too young first is %s\n"),GetNr(),m_stFirst.GetDateTime());
		}
		bSequenceNotInTimeSpan = TRUE;
		bRet = FALSE;
	}
	else if (m_stLast < b.GetStartTime())
	{
		// Sequence is too old
		if (theApp.TraceBackup())
		{
			WK_TRACE(_T("Sequence %04hx is too old last is %s\n"),GetNr(),m_stLast.GetDateTime());
		}
		bSequenceNotInTimeSpan = TRUE;
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

		if (   dwStartRecord != 0
			&& dwEndRecord != 0)
		{
			if (   (dwStartRecord == 1)
				&& (dwEndRecord == m_dwNumRecords)
				)
			{
				// copy the entire sequence
				bRet = Backup(b, bErrorSharingViolation);
			}
			else
			{
				TRACE(_T("cut sequence from %d -> %d\n"),dwStartRecord,dwEndRecord);
				// cut calls the director cut
				CString sDbf,sDat; // source names
				CString tDbf,tDat; // temp names
				CString dDbf,dDat; // destination names
				CString sTempDir;
				
				sDbf = GetFileName(szDbf);
				sDat = GetFileName(CArchiv::m_sDbd);

				//
				GetTempPath(_MAX_PATH,sTempDir.GetBufferSetLength(_MAX_PATH));
				sTempDir.ReleaseBuffer();

				// TODO check space left on temp drive !!!
				
				GetTempFileName(sTempDir,szDbf,0,tDbf.GetBufferSetLength(_MAX_PATH));
				GetTempFileName(sTempDir,CArchiv::m_sDbd,0,tDat.GetBufferSetLength(_MAX_PATH));
				tDbf.ReleaseBuffer();
				tDat.ReleaseBuffer();

				DWORD dwNewDbfSize = 0;
				DWORD dwNewDatSize = 0;

				if (   CutDbf(dwStartRecord,dwEndRecord,tDbf,dwNewDbfSize)
					&& (   IsAlarmList()
					    || CutDat(dwStartRecord,dwEndRecord,tDat,dwNewDatSize)
						)
					)
				{
					CString sDir;

					// create the directory
					sDir.Format(_T("%s\\arch%04lx"),b.GetDirectory(),m_pArchiv->GetNr());

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
							WK_TRACE(_T("mdir SUCCESS %s\n"),sDir);
						}
					}

					if (bRet)
					{
						dDbf.Format(_T("%s\\%04lx%04lx.%s"),sDir,(WORD)Flags2Nr((BYTE)(m_bFlags|CAR_IS_BACKUP)),GetNr(),szDbf);
						dDat = dDbf;
						dDat.SetAt(dDat.GetLength()-2,_T('b'));
						dDat.SetAt(dDat.GetLength()-1,_T('d'));

						// now copy from temp to dest
						bRet = b.CopyFileTrace(GetArchivNr(),GetNr(),tDbf,dDbf);
						Sleep(50);

						if (bRet)
						{
							bRet = b.CopyFileTrace(GetArchivNr(),GetNr(),tDat,dDat);
							Sleep(50);
						}

						if (bRet)
						{
							if (   ((b.GetFlags() & BBT_PROGRESS) == 0)
								&& (b.GetFlags() & BBT_EXECUTE)
								)
							{
								b.IncreaseCopiedBytes(GetArchivNr(),
													  GetNr(),
													  dwNewDbfSize+dwNewDatSize,
													  BBT_FINISHED);
							}
						}
					}
				}
				else
				{
					bRet = FALSE;
				}
				// always delete temporary files
//				TRACE(_T("DeleteFile(%s)\n"),tDbf);
				DeleteFile(tDbf);
//				TRACE(_T("DeleteFile(%s)\n"),tDat);
				DeleteFile(tDat);
			}
		}
		else
		{
			bRet = FALSE;
		}
	}
	if (bRet)
	{
		b.IncreaseCopiedBytes(GetArchivNr(),GetNr(),dwCopiedBytes,BBT_FINISHED);
		b.GetSequenceMap().SetSequenceHashEntry(GetArchivNr(),GetNr(),dwNumRecords,stFirst,stLast,m_sName);
	}

	CAutoCritSec acs2(&m_cs);
	m_bLockedByBackup = FALSE;
	acs2.Unlock();

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
void CSequence::CorrectReferences(Data4& data, DWORD dwFirstRecordNr)
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
BOOL CSequence::CutDbf(DWORD dwFirstRecordNr,
						DWORD dwLastRecordNr,
						const CString& sDbf,
						DWORD& dwNewSizeBytes)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	BOOL bRet = TRUE;

	CAutoCritSec acs(&m_cs);

	if (OpenDbf())
	{
		Data4 data4;
		Field4info finfo(m_pArchiv->m_cb);
		int ret = finfo.add(*m_pData4);

		// copy the field info structure
		if (r4success == ret)
		{
			ret = data4.create(m_pArchiv->m_cb,CWK_String(sDbf),finfo);

			if (r4success == ret)
			{
				DWORD dwOffset = 0;
				DWORD dwLen = 0;
				CString sOffset;
				for (DWORD i = dwFirstRecordNr;i<=dwLastRecordNr;i++)
				{
					ret = m_pData4->go(i);
					if (r4success == ret)
					{
						ret = data4.appendStart();
						if (r4success != ret)
						{
							WK_TRACE_ERROR(_T("cannot append start temp dbf file %s,%d\n"),
								sDbf,ret);
							bRet = FALSE;
							break;
						}
						data4.blank();
						// copy the whole data at once
						// cheap and tricky
						strcpy(data4.record(),m_pData4->record());
						
						// now correct the offsets
						sOffset.Format(szDWORD,dwOffset);
						AssignField(data4,m_pArchiv->m_cb,CArchiv::m_sDPOS,sOffset,FALSE,TRUE);

						Field4 fLength(*m_pData4,CArchiv::m_sDLEN);
						if (fLength.isValid())
						{
							sscanf(fLength.str(),SCAN_DWORD,&dwLen);
							dwOffset += dwLen;
						}
						else
						{
							WK_TRACE_ERROR(_T("cannot read data length source dbf file %d\n"),
								ret);
							bRet = FALSE;
							break;
						}
						
						CorrectReferences(data4,dwFirstRecordNr);


						ret = data4.append();
						if (r4success != ret)
						{
							WK_TRACE_ERROR(_T("cannot append temp dbf file %s,%d\n"),
								sDbf,ret);
							bRet = FALSE;
							break;
						}
					}
					else
					{
						WK_TRACE_ERROR(_T("cannot goto source record,%d\n"),ret);
						bRet = FALSE;
						break;
					}
				}
				if (bRet)
				{
					ret = data4.flush();
					if (ret == r4success)
					{
						dwNewSizeBytes = m_dwFieldSize * data4.recCount();
						dwNewSizeBytes += m_pArchiv->m_FieldInfo.numFields() * 32 + 34;
					}
					else
					{
						WK_TRACE_ERROR(_T("cannot flush temp dbf file %s,%d\n"),
							sDbf,ret);
						bRet = FALSE;
					}
				}
				// always try to close, delete must work
				ret = data4.close();
				if (ret != r4success)
				{
					WK_TRACE_ERROR(_T("cannot close temp dbf file %s,%d\n"),
						sDbf,ret);
					bRet = FALSE;
				}
			}
			else
			{
				WK_TRACE_ERROR(_T("cannot create temp dbf file %s,%d\n"),
					sDbf,ret);
			}
		}
		else
		{
			WK_TRACE_ERROR(_T("cannot copy field info structure %s,%d\n"), sDbf, ret);
		}
		errorSet();
		CloseDbf();
	}
	
	acs.Unlock();

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSequence::CutDat(DWORD dwFirstRecordNr,
			       DWORD dwLastRecordNr,
			       const CString& sDat,
				   DWORD& dwNewSizeBytes)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	BOOL bRet = TRUE;

	CAutoCritSec acs(&m_cs);

	DWORD dwEndLength,dwStartLength;
	FILESIZE dwEndOffset,dwStartOffset;
	GetOffsetAndLength(dwFirstRecordNr,dwStartOffset,dwStartLength);
	GetOffsetAndLength(dwLastRecordNr,dwEndOffset,dwEndLength);
	FILESIZE dwDatSize = dwEndOffset-dwStartOffset+dwEndLength;

	if (OpenDbd())
	{
		CFile file;

		if (file.Open(sDat,CFile::modeCreate|CFile::modeWrite))
		{
			DWORD dwSize = 2048;
			DWORD dwWrote=0;
			BYTE szBuffer[2048];

			TRY
			{
				m_fData.Seek(dwStartOffset,CFile::begin);
				DWORD dwRead = 0;
				while (    bRet
						&& ((dwRead=m_fData.Read(&szBuffer,dwSize))>0)
						)
				{
					DWORD dwToWrite = (DWORD)min(dwRead,dwDatSize-dwWrote); 
					TRY
					{
						file.Write(szBuffer,dwToWrite);
					}
					CATCH (CFileException ,e)
					{
						theApp.HandleFileException(e, FALSE);
						bRet = FALSE;
					}
					END_CATCH
					if (bRet)
					{
						dwWrote += dwToWrite;
					}
				}
			}
			CATCH (CFileException ,e)
			{
				theApp.HandleFileException(e, FALSE);
				WK_TRACE_ERROR(_T("cannot read file for copy %d Bytes, %s\n"),
					dwWrote,GetLastErrorString(e->m_lOsError));
				bRet = FALSE;
			}
			END_CATCH

			file.Close();

			dwNewSizeBytes = dwWrote;
		}
		CloseDbd();
	}
	
	acs.Unlock();

	return bRet;
}
//////////////////////////////////////////////////////////////////////
BYTE CSequence::Nr2Flags(WORD wNr)
{
	CCallStackEntry cse(_T(__FUNCTION__));
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
		WK_TRACE_ERROR(_T("unkown sequence nr type %d\n"),wNr);
		break;
	}

	return bFlags;
}
//////////////////////////////////////////////////////////////////////
WORD CSequence::Flags2Nr(BYTE bFlags)
{
	CCallStackEntry cse(_T(__FUNCTION__));
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
		WK_TRACE_ERROR(_T("unkown sequence flag type %02x\n"),bFlags);
		break;
	}

	return wNr;
}
//////////////////////////////////////////////////////////////////////
void CSequence::DoRequestOpenSequence(DWORD dwClientID)
{
	BOOL bFound = FALSE;
	for (int i=0;i<m_dwClients.GetSize() && !bFound;i++)
	{
		bFound = m_dwClients.GetAt(i) == dwClientID;
	}
	if (!bFound)
	{
		m_dwClients.Add(dwClientID);
	}
}
//////////////////////////////////////////////////////////////////////
void CSequence::DoRequestCloseSequence(DWORD dwClientID)
{
	for (int i=0;i<m_dwClients.GetSize();i++)
	{
		if (m_dwClients.GetAt(i) == dwClientID)
		{
			m_dwClients.RemoveAt(i);
			break;
		}
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CSequence::IsOpenByClient(DWORD dwClientID)
{
	if (dwClientID == 0)
	{
		return m_dwClients.GetSize()>0;
	}
	for (int i=0;i<m_dwClients.GetSize();i++)
	{
		if (m_dwClients.GetAt(i) == dwClientID)
		{
			return TRUE;
		}
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSequence::Verify()
{
	if (IsBackup())
	{
		return TRUE;
	}

	CAutoCritSec acs(&m_cs);

	CSystemTime stFirst = m_stFirst;
	CSystemTime stLast = m_stLast;
	DWORD dwNumRecords = m_dwNumRecords;
	BOOL bEqual = TRUE;

//	TRACE(_T("verifying %d %d\n"),GetCollectionNr(),GetTapeNr());

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
			WK_TRACE(_T("correcting sequence hash entry %s %d %d\n"),
				GetArchiv()->GetName(), GetArchivNr(),GetNr());
			SetSequenceHashEntry();
		}

	}
	acs.Unlock();

	if (theApp.IsIDIP() &&
		!theApp.IsOffsetCorrected())
	{
		CorrectOffsets();
	}

	return bEqual;
}
/////////////////////////////////////////////////////////////////////////////
void CSequence::CorrectOffsets()
{
	CAutoCritSec acs(&m_cs);

	if (OpenDbf())
	{
		DWORD dwSum = 0;
		DWORD dwPos = 0;
		DWORD dwLen = 0;
		int iNumCorrected = 0;

		for (m_pData4->top();!m_pData4->eof();m_pData4->skip())
		{
			ReadDWORD(*m_pData4,m_pArchiv->m_cb,CArchiv::m_sDLEN,dwLen);
			ReadDWORD(*m_pData4,m_pArchiv->m_cb,CArchiv::m_sDPOS,dwPos);
			if (dwPos != dwSum)
			{
				AssignDWORD(*m_pData4,m_pArchiv->m_cb,CArchiv::m_sDPOS,dwSum);
				iNumCorrected++;
			}

			dwSum += dwLen;
		}

		if (iNumCorrected>0)
		{
			WK_TRACE(_T("%d offsets corrected %s\n"),iNumCorrected,GetFileName(szDbf));
		}
		else
		{
			WK_TRACE(_T("all offsets OK %s\n"),GetFileName(szDbf));
		}

		CloseDbf();
	}

	acs.Unlock();
}
