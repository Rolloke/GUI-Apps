// VDBSequenceHashEntry.cpp: implementation of the CVDBSequenceHashEntry class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VDBSequenceHashEntry.h"
#include "sequence.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define ARCHIV_NR		"A_NR"
#define SEQUENCE_NR		"S_NR"
#define NR_RECORDS		"NR_R"
#define FIRST			"FIRST"
#define LAST			"LAST"
#define NAME			"NAME"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CVDBSequenceHashEntry::CVDBSequenceHashEntry(WORD wArchiveNr, 
											 WORD wSequenceNr, 
											 DWORD dwRecords, 
											 const CSystemTime& tFirst, 
											 const CSystemTime& tLast,
											 const CString& sName)
{
	m_wArchiveNr = wArchiveNr;
	m_wSequenceNr = wSequenceNr;
	m_dwRecords = dwRecords;
	m_stFirst = tFirst;
	m_stLast = tLast;
	m_sName = sName;
}
//////////////////////////////////////////////////////////////////////
CVDBSequenceHashEntry::CVDBSequenceHashEntry(const CVDBSequenceHashEntry& source)
{
	m_wArchiveNr = source.m_wArchiveNr;
	m_wSequenceNr = source.m_wSequenceNr;
	m_dwRecords = source.m_dwRecords;
	m_stFirst = source.m_stFirst;
	m_stLast = source.m_stLast;
	m_sName = source.m_sName;
}
//////////////////////////////////////////////////////////////////////
CVDBSequenceHashEntry::~CVDBSequenceHashEntry()
{

}
//////////////////////////////////////////////////////////////////////
CVDBSequenceMap::CVDBSequenceMap()
	: m_Fields(m_CodeBase)
{
	CAutoCritSec acs(&m_cs);

	m_CodeBase.safety = FALSE;
	m_CodeBase.errOff = TRUE;
	m_CodeBase.accessMode = OPEN4DENY_NONE;
	m_CodeBase.optimize = OPT4OFF;
	m_CodeBase.optimizeWrite = OPT4OFF;

	m_Fields.add(ARCHIV_NR,'C',4,0);
	m_Fields.add(SEQUENCE_NR,'C',4,0);
	m_Fields.add(NR_RECORDS,'C',8,0);
	m_Fields.add(FIRST,'C',18,0);
	m_Fields.add(LAST,'C',18,0);
	m_Fields.add(NAME,'C',64,0);
}
//////////////////////////////////////////////////////////////////////
CVDBSequenceMap::~CVDBSequenceMap()
{
	CAutoCritSec acs(&m_cs);
	Close();
	m_CodeBase.initUndo();
	DeleteAll();
}
//////////////////////////////////////////////////////////////////////
void CVDBSequenceMap::DeleteAll()
{
	CAutoCritSec acs(&m_cs);
	POSITION pos = GetStartPosition();
	DWORD dwKey;
	CVDBSequenceHashEntry* pSequenceHashEntry;
	while (pos != NULL)
	{
		pSequenceHashEntry = NULL;
		GetNextAssoc(pos, dwKey, pSequenceHashEntry);
		WK_DELETE(pSequenceHashEntry);
	}
	RemoveAll();
}
//////////////////////////////////////////////////////////////////////
BOOL CVDBSequenceMap::Create(const CString& sPathname)
{
	CAutoCritSec acs(&m_cs);
	int rc;
	CWK_String sPathName(sPathname);
	if (DoesFileExist(sPathname))
	{
		rc = m_Data.open(m_CodeBase,sPathName);
	}
	else
	{
		rc = m_Data.create(m_CodeBase,sPathName,m_Fields);
	}
	if (r4success == rc)
	{
		m_sPathname = sPathname;
	}
	else
	{
		WK_TRACE(_T("cannot create/open sequence map\n"));
	}
	m_CodeBase.errorSet();
	return r4success == rc;
}
//////////////////////////////////////////////////////////////////////
void CVDBSequenceMap::Flush()
{
	CAutoCritSec acs(&m_cs);
	if (m_Data.isValid())
	{
		m_Data.flush();
	}
}
//////////////////////////////////////////////////////////////////////
void CVDBSequenceMap::Close()
{
	CAutoCritSec acs(&m_cs);
	if (m_Data.isValid())
	{
		int iDeleted = 0;
		for (m_Data.top();!m_Data.eof();m_Data.skip())
		{
			if (m_Data.deleted())
			{
				iDeleted++;
			}
		}
		if (iDeleted>0)
		{
			WK_TRACE(_T("packing sequence map %d entries are marked as deleted\n"),iDeleted);
			int ret = m_Data.pack();
			if (r4success == ret)
			{
				WK_TRACE(_T("packed sequence map\n"));
			}
			else
			{
				WK_TRACE(_T("cannot pack sequence map %d\n"),ret);
				m_CodeBase.errorSet();
			}
		}
		else
		{
			WK_TRACE(_T("nothing to pack in sequence map\n"));
		}
		m_Data.close();
		WK_TRACE(_T("sequence map closed %s\n"),m_sPathname);
	}
	m_sPathname.Empty();
}
//////////////////////////////////////////////////////////////////////
void CVDBSequenceMap::Delete()
{
	CAutoCritSec acs(&m_cs);
	CString sPathname = m_sPathname;
	Close();
	DeleteAll();
	DeleteFile(sPathname);
}
//////////////////////////////////////////////////////////////////////
void CVDBSequenceMap::Load(const CString& sPathname)
{
	CAutoCritSec acs(&m_cs);
	WORD  wArchivNr = 0;
	WORD  wSequenceNr = 0;
	DWORD dwNrRecords = 0;
	CSystemTime stFirst,stLast;
	DWORD dw = 0;
	CString s;
	CWK_String str, sPathName(sPathname);

	if (DoesFileExist(sPathname))
	{
		WK_TRACE(_T("sequence map is %s\n"),sPathname);
		if (r4success == m_Data.open(m_CodeBase,sPathName))
		{
			int iDeleted = 0;
			WK_TRACE(_T("sequence map opened %s %d records\n"),sPathname,m_Data.recCount());
			m_sPathname = sPathname;
			for (m_Data.top();!m_Data.eof();m_Data.skip())
			{
				if (m_Data.deleted())
				{
					iDeleted++;
				}
				else
				{
					if (ReadWORD(m_Data,m_CodeBase,ARCHIV_NR,wArchivNr))
				{
					if (ReadWORD(m_Data,m_CodeBase,SEQUENCE_NR,wSequenceNr))
					{
						if (ReadDWORD(m_Data,m_CodeBase,NR_RECORDS,dwNrRecords))
						{
							Field4 first(m_Data,FIRST);
							if (first.isValid())
							{
								s = first.str();
								s.TrimRight();
								stFirst.SetDBDate(s.Left(8));
								stFirst.SetDBTime(s.Mid(8,6));
								s = s.Right(3);
								_stscanf(s,_T("%03d"),&dw);
								stFirst.wMilliseconds = (WORD)dw;
								
								Field4 last(m_Data,LAST);
								if (last.isValid())
								{
									s = last.str();
									s.TrimRight();

									stLast.SetDBDate(s.Left(8));
									stLast.SetDBTime(s.Mid(8,6));
									s = s.Right(3);
									_stscanf(s,_T("%03d"),&dw);
									stLast.wMilliseconds = (WORD)dw;

									Field4 name(m_Data,NAME);
									if (name.isValid())
									{
										s = name.str();
										s.TrimLeft();
										s.TrimRight();

										DWORD dwKey = MAKELONG(wArchivNr,wSequenceNr);
										CVDBSequenceHashEntry* pSequenceHashEntry = NULL;
										if (Lookup(dwKey,pSequenceHashEntry))
										{
											delete pSequenceHashEntry;
										}
										pSequenceHashEntry = new CVDBSequenceHashEntry(wArchivNr,
																					   wSequenceNr,
																					   dwNrRecords,
																					   stFirst,
																					   stLast,
																					   s);
//										TRACE(_T("set SMHE %04hx %04hx\n"),wArchivNr,wSequenceNr);
										SetAt(pSequenceHashEntry->GetKey(),pSequenceHashEntry);
									}
									else
									{
										TRACE(_T("name field invalid\n"));
									}
								}
								else
								{
									TRACE(_T("last field invalid\n"));
								}
							}
							else
							{
								TRACE(_T("first field invalid\n"));
							}
						}
					}
				}
				}
				if (m_CodeBase.errorCode != 0)
				{
					WK_TRACE(_T("codebase error %d reading sequence map file\n"),m_CodeBase.errorCode);
					m_CodeBase.errorSet();
				}
			}
			WK_TRACE(_T("loaded sequence map %d records %d entries %d deleted\n"),m_Data.recCount(), GetCount(),iDeleted);
		}
	}
	else
	{
		Create(sPathname);
	}
}
//////////////////////////////////////////////////////////////////////
void CVDBSequenceMap::Save(const CString& sPathname)
{
	CAutoCritSec acs(&m_cs);
	int rc = r4success;
	CWK_String sPathName(sPathname);

	if (!m_Data.isValid())
	{
		rc = m_Data.create(m_CodeBase,sPathName,m_Fields);
		m_sPathname = sPathname;
	}
	if (r4success == rc)
	{
		WK_TRACE(_T("saving sequence map to %s\n"),m_sPathname);
		POSITION pos = GetStartPosition();
		CVDBSequenceHashEntry* pSequenceHashEntry;
		DWORD dwKey;
		
		while (pos != NULL)
		{
			pSequenceHashEntry = NULL;
			GetNextAssoc(pos, dwKey, pSequenceHashEntry);

			if (pSequenceHashEntry)
			{
				AppendEntry(pSequenceHashEntry->GetArchiveNr(),
					pSequenceHashEntry->GetSequenceNr(),
					pSequenceHashEntry->GetRecords(),
					pSequenceHashEntry->GetFirst(),
					pSequenceHashEntry->GetLast(),
					pSequenceHashEntry->GetName());
			}
		}
		Close();
		WK_TRACE(_T("saved sequence map %d entries\n"),GetCount());
	}
	else
	{
		WK_TRACE_ERROR(_T("cannot create sequence map to %s %d\n"),sPathname,rc);
		m_sPathname.Empty();
		m_CodeBase.errorSet();
	}
}
/////////////////////////////////////////////////////////////////////////////
CVDBSequenceHashEntry* CVDBSequenceMap::GetSequenceHashEntry(WORD wArchivNr,
 														    WORD wSequenceNr) const
{
	CVDBSequenceHashEntry* pVDBSequenceHashEntry = NULL;
	DWORD dwKey = MAKELONG(wArchivNr,wSequenceNr);

	if (Lookup(dwKey,pVDBSequenceHashEntry))
	{
		return pVDBSequenceHashEntry;
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
void CVDBSequenceMap::SetSequenceHashEntry(WORD wArchivNr,
											 WORD wSequenceNr,
											 DWORD dwRecords,
											 const CSystemTime& tFirst,
											 const CSystemTime& tLast,
											 const CString& sName)
{
	CAutoCritSec acs(&m_cs);
	if (m_Data.isValid())
	{
		SetEntry(wArchivNr,wSequenceNr,dwRecords,tFirst,tLast,sName);
	}
	
	DWORD dwKey = MAKELONG(wArchivNr,wSequenceNr);
	CVDBSequenceHashEntry* pSequenceHashEntry = NULL;
	if (Lookup(dwKey,pSequenceHashEntry))
	{
		delete pSequenceHashEntry;
	}
	pSequenceHashEntry = new CVDBSequenceHashEntry(wArchivNr,
		wSequenceNr,
		dwRecords,
		tFirst,
		tLast,
		sName);
	
	SetAt(pSequenceHashEntry->GetKey(),pSequenceHashEntry); 
}
/////////////////////////////////////////////////////////////////////////////
void CVDBSequenceMap::DeleteSequenceHashEntry(WORD wArchivNr,
			 								  WORD wSequenceNr)
{
	CAutoCritSec acs(&m_cs);
	if (m_Data.isValid())
	{
		if (GotoEntry(wArchivNr,wSequenceNr))
		{
			TRACE(_T("delete entry %d\n"),m_Data.recNo());
			m_Data.deleteRec();
		}
	}
	DWORD dwKey = MAKELONG(wArchivNr,wSequenceNr);
	CVDBSequenceHashEntry* pSequenceHashEntry = NULL;
	if (Lookup(dwKey,pSequenceHashEntry))
	{
		delete pSequenceHashEntry;
		RemoveKey(dwKey);
	}
}
////////////////////////////////////////////////////////////////////////////
BOOL CVDBSequenceMap::AssignField(const char *name, const CString& sValue)
{
	Field4 field(m_Data,name);
	int ret = 0;
	BOOL bRet = FALSE;
	CWK_String s(sValue);

	if (field.isValid())
	{
#ifdef _UNICODE 
		ret = field.assign(s);
#else
		ret = field.assign(sValue);
#endif
		if (r4success==ret)
		{
			// alles OK
			bRet = TRUE;
		}
		else
		{
			CWK_String sName(field.name());
			WK_TRACE(_T("map assign field failed <%s>=<%s>, %d\n"),
				(LPCTSTR)sName,
				(LPCTSTR)sValue, 
				ret);
			m_CodeBase.errorSet();
		}
	}
	else
	{
		WK_TRACE(_T("map assign field invalid <%s>=<%s>\n"),
			field.name(),
			sValue);
		m_CodeBase.errorSet();
	}

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CVDBSequenceMap::AssignEntry(WORD wArchivNr,
								  WORD wSequenceNr,
								  DWORD dwNumRecords,
								  const CString& sFirst,
								  const CString& sLast,
								  const CString& sName)
{
	BOOL bRet = TRUE;

	bRet &= AssignWORD(m_Data,m_CodeBase, ARCHIV_NR,wArchivNr);
	bRet &= AssignWORD(m_Data,m_CodeBase, SEQUENCE_NR,wSequenceNr);
	bRet &= AssignDWORD(m_Data,m_CodeBase, NR_RECORDS,dwNumRecords);
	bRet &= AssignField(FIRST,sFirst);
	bRet &= AssignField(LAST,sLast);
	bRet &= AssignField(NAME,sName);

	if (m_CodeBase.errorCode!=0)
	{
		WK_TRACE(_T("codebase error %d assigning entry\n"),m_CodeBase.errorCode);
		m_CodeBase.errorSet();
		bRet = FALSE;
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
void CVDBSequenceMap::AppendEntry(WORD wArchivNr,
								  WORD wSequenceNr,
								  DWORD dwRecords,
								  const CSystemTime& tFirst,
								  const CSystemTime& tLast,
								  const CString& sName)
{
	if (m_Data.isValid())
	{
		BOOL bFoundOneDeleted = FALSE;
		
		for (m_Data.top();!m_Data.eof();m_Data.skip())
		{
			if (m_Data.deleted())
			{
				bFoundOneDeleted = TRUE;
				break;
			}
		}
		if (bFoundOneDeleted)
		{
			TRACE(_T("found one deleted entry %d\n"),m_Data.recNo());
		}

		CString sFirst,sLast;
		
		sFirst.Format(_T("%s%s%03d"),tFirst.GetDBDate(),
			tFirst.GetDBTime(),
			tFirst.GetMilliseconds());
		
		sLast.Format(_T("%s%s%03d"),tLast.GetDBDate(),
			tLast.GetDBTime(),
			tLast.GetMilliseconds());

		int ret = r4success;
		if (bFoundOneDeleted)
		{
			m_Data.recall();
			m_Data.blank();
		}
		else
		{
			ret = m_Data.appendStart();
			if (r4success == ret)
			{
				m_Data.blank();
			}
			else
			{
				WK_TRACE(_T("cannot appendStart writing sequence map %d\n"),m_CodeBase.errorCode);
				m_CodeBase.errorSet();
			}
		}
		if (r4success == ret)
		{
			//	TRACE(_T("appending new entry %d %s,%s\n"),m_Data.recCount(),sANr,sSNr);
			BOOL bRet = AssignEntry(wArchivNr,wSequenceNr,dwRecords,sFirst,sLast,sName);
			if (bRet)
			{
				if (!bFoundOneDeleted)
				{
					if (r4success != m_Data.append())
					{
						WK_TRACE(_T("cannot append writing sequence map %d\n"),m_CodeBase.errorCode);
					}
				}
			}
			else
			{
				// assign fehlgeschlagen, wert wieder löschen
				if (bFoundOneDeleted)
				{
					m_Data.deleteRec();
				}
			}
			if (m_CodeBase.errorCode != 0)
			{
				WK_TRACE(_T("codebase error %d writing sequence map file\n"),m_CodeBase.errorCode);
				m_CodeBase.errorSet();
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CVDBSequenceMap::GotoEntry(WORD wArchivNr,
							   WORD wSequenceNr)
{
	BOOL bFound = FALSE;
	if (m_Data.isValid())
	{
		CString sANr,sSNr,sNrRecords,sFirst,sLast;
		
		sANr.Format(_T("%04hx"),wArchivNr);
		sSNr.Format(_T("%04hx"),wSequenceNr);

		Relate4set relation;
		int ret = relation.init(m_Data);
		if (ret == r4success)
		{
			CWK_String sExpression;
			sExpression.Format(_T("(%s='%s') .AND. (%s='%s')"),_T(ARCHIV_NR),sANr,_T(SEQUENCE_NR),sSNr);
			int rc = relation.querySet(sExpression);
			if (rc==r4success)
			{
				if (r4success == relation.top())
				{
					bFound = TRUE;
				}
			}
			else
			{
				m_CodeBase.errorSet();
			}
			relation.free();
		}
		else
		{
			m_CodeBase.errorSet();
		}
	}
	return bFound;
}
/////////////////////////////////////////////////////////////////////////////
void CVDBSequenceMap::SetEntry(WORD wArchivNr,
							   WORD wSequenceNr,
							   DWORD dwRecords,
							   const CSystemTime& tFirst,
							   const CSystemTime& tLast,
							   const CString& sName)
{
	CAutoCritSec acs(&m_cs);
	if (GotoEntry(wArchivNr,wSequenceNr))
	{
		CString sFirst,sLast;
		
		sFirst.Format(_T("%s%s%03d"),tFirst.GetDBDate(),
			tFirst.GetDBTime(),
			tFirst.GetMilliseconds());
		
		sLast.Format(_T("%s%s%03d"),tLast.GetDBDate(),
			tLast.GetDBTime(),
			tLast.GetMilliseconds());
//		TRACE(_T("set entry at %d new value %s,%s\n"),m_Data.recNo(),sANr,sSNr);
		AssignEntry(wArchivNr,wSequenceNr,dwRecords,sFirst,sLast,sName);
	}
	else
	{
		AppendEntry(wArchivNr,wSequenceNr,dwRecords,tFirst,tLast,sName);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVDBSequenceMap::AppendMap(const CVDBSequenceMap& map)
{
	POSITION pos = map.GetStartPosition();
	CVDBSequenceHashEntry* pSequenceHashEntry;
	DWORD dwKey;

	while (pos != NULL)
	{
		pSequenceHashEntry = NULL;
		map.GetNextAssoc(pos, dwKey, pSequenceHashEntry);

		if (pSequenceHashEntry)
		{
			if (NULL == GetSequenceHashEntry(pSequenceHashEntry->GetArchiveNr(),
				pSequenceHashEntry->GetSequenceNr()))
			{
				AppendEntry(pSequenceHashEntry->GetArchiveNr(),
					pSequenceHashEntry->GetSequenceNr(),
					pSequenceHashEntry->GetRecords(),
					pSequenceHashEntry->GetFirst(),
					pSequenceHashEntry->GetLast(),
					pSequenceHashEntry->GetName());
			}
			else
			{
				WK_TRACE_ERROR(_T("cannot append already existing entry %d,%d\n"),
					pSequenceHashEntry->GetArchiveNr(),
					pSequenceHashEntry->GetSequenceNr());
			}
		}
	}
}
