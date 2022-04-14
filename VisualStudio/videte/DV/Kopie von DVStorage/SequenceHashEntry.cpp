// SequenceHashEntry.cpp: implementation of the CSequenceHashEntry class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SequenceHashEntry.h"

#define ARCHIV_NR		"A_NR"
#define SEQUENCE_NR		"S_NR"
#define NR_RECORDS		"NR_R"
#define FIRST			"FIRST"
#define LAST			"LAST"
#define NAME			"NAME"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL( CSequenceHashEntry, CObject, 0);
//////////////////////////////////////////////////////////////////////
CSequenceHashEntry::CSequenceHashEntry()
{
	m_dwRecords = 0;
}
//////////////////////////////////////////////////////////////////////
CSequenceHashEntry::CSequenceHashEntry(WORD wArchiveNr, WORD wSequenceNr, DWORD dwRecords,CTime tFirst, CTime tLast)
{
	m_wArchiveNr = wArchiveNr;
	m_wSequenceNr = wSequenceNr;
	m_dwRecords = dwRecords;
	m_tFirst = tFirst;
	m_tLast = tLast;
}

CSequenceHashEntry::~CSequenceHashEntry()
{

}

void CSequenceHashEntry::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// add storing code here
		ar << m_wArchiveNr;
		ar << m_wSequenceNr;
		ar << m_dwRecords;
        ar << m_tFirst;
		ar << m_tLast;
//		TRACE(_T("stored %04x,%04x %04d rec\n"),m_wArchiveNr,m_wSequenceNr,m_dwRecords);
	}
	else
	{
		// add loading code here
		ar >> m_wArchiveNr;
		ar >> m_wSequenceNr;
		ar >> m_dwRecords;
        ar >> m_tFirst;
		ar >> m_tLast;
//		TRACE(_T("loaded %04x,%04x %04d rec\n"),m_wArchiveNr,m_wSequenceNr,m_dwRecords);
	}
}


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
	acs.Unlock();
}
//////////////////////////////////////////////////////////////////////
CVDBSequenceMap::~CVDBSequenceMap()
{
	CAutoCritSec acs(&m_cs);
	Close();
	m_CodeBase.initUndo();
	DeleteAll();
	acs.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CVDBSequenceMap::DeleteAll()
{
	CAutoCritSec acs(&m_cs);
	POSITION pos = GetStartPosition();
	while (pos != NULL)
	{
		DWORD dwKey;
		CVDBSequenceHashEntry* pSequenceHashEntry = NULL;
		GetNextAssoc(pos, dwKey, pSequenceHashEntry);
		WK_DELETE(pSequenceHashEntry);
	}
	RemoveAll();
	acs.Unlock();
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
	m_CodeBase.errorSet();
	if (r4success == rc)
	{
		m_sPathname = sPathname;
	}
	else
	{
		WK_TRACE(_T("cannot create/open sequence map\n"));
	}
	acs.Unlock();
	return r4success == rc;
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
			m_Data.pack();
		}
		else
		{
			WK_TRACE(_T("nothing to pack in sequence map\n"));
		}
		m_Data.close();
	}
	m_sPathname.Empty();
	acs.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CVDBSequenceMap::Flush()
{
	CAutoCritSec acs(&m_cs);
	if (m_Data.isValid())
	{
		m_Data.flush();
	}
	acs.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CVDBSequenceMap::Load(const CString& sPathname)
{
	CAutoCritSec acs(&m_cs);
	DWORD dwArchivNr = 0;
	DWORD dwSequenceNr = 0;
	DWORD dwNrRecords = 0;
	CSystemTime stFirst,stLast;
	DWORD dw = 0;
	CWK_String str, sPathName(sPathname);
	const char *s;

	if (DoesFileExist(sPathname))
	{
		WK_TRACE(_T("sequence map is %s\n"),sPathname);
		if (r4success == m_Data.open(m_CodeBase,sPathName))
		{
			WK_TRACE(_T("sequence map opened %s %d records\n"),sPathname,m_Data.recCount());
			m_sPathname = sPathname;
			for (m_Data.top();!m_Data.eof();m_Data.skip())
			{
				Field4 a_nr(m_Data,ARCHIV_NR);

				if (a_nr.isValid())
				{
					s = a_nr.str();
					sscanf(s, "%04hx", &dwArchivNr);

					Field4 s_nr(m_Data,SEQUENCE_NR);
					if (s_nr.isValid())
					{
						s = s_nr.str(); 
						sscanf(s, "%04hx", &dwSequenceNr);

						Field4 nr_r(m_Data,NR_RECORDS);
						if (nr_r.isValid())
						{
							s = nr_r.str();
							sscanf(s, "%08lx", &dwNrRecords);

							Field4 first(m_Data,FIRST);
							if (first.isValid())
							{
								str = first.str();
								str.TrimRight();

								stFirst.SetDBDate(str.Left(8));
								stFirst.SetDBTime(str.Mid(8,6));
								str = str.Right(3);
								_stscanf(str, _T("%03d"), &dw);
								stFirst.wMilliseconds = (WORD)dw;

								
								Field4 last(m_Data,LAST);
								if (last.isValid())
								{
									str = last.str();
									str.TrimRight();

									stLast.SetDBDate(str.Left(8));
									stLast.SetDBTime(str.Mid(8,6));
									str = str.Right(3);
									_stscanf(str, _T("%03d"), &dw);
									stLast.wMilliseconds = (WORD)dw;

									Field4 name(m_Data,6);
									if (name.isValid())
									{
										str = name.str();
										str.TrimLeft();
										str.TrimRight();

										CVDBSequenceHashEntry* pVDBSequenceHashEntry;
										pVDBSequenceHashEntry = new CVDBSequenceHashEntry((WORD)dwArchivNr,(WORD)dwSequenceNr,dwNrRecords,stFirst,stLast,str);
										SetAt(pVDBSequenceHashEntry->GetKey(),pVDBSequenceHashEntry);
									}
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
			WK_TRACE(_T("loaded sequence map %d entries\n"),GetCount());
//			m_Data.close();
		}
	}
	else
	{
		Create(sPathname);
	}
	acs.Unlock();
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
		
		while (pos != NULL)
		{
			DWORD dwKey;
			CVDBSequenceHashEntry* pSequenceHashEntry;
			GetNextAssoc(pos, dwKey, pSequenceHashEntry);

			if (pos)
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
	acs.Unlock();
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

	acs.Unlock();
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
	acs.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
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
			WK_TRACE(_T("map assign field failed <%s>=<%s>, %d\n"),
						field.name(),
						sValue, 
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
BOOL CVDBSequenceMap::AssignEntry(const CString& sArchiveNr,
								  const CString& sSequenceNr,
								  const CString& sNrRecords,
								  const CString& sFirst,
								  const CString& sLast,
								  const CString& sName)
{
	BOOL bRet = TRUE;

	bRet &= AssignField(ARCHIV_NR,sArchiveNr);
	bRet &= AssignField(SEQUENCE_NR,sSequenceNr);
	bRet &= AssignField(NR_RECORDS,sNrRecords);
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

		CString sANr,sSNr,sNrRecords,sFirst,sLast;
		
		sANr.Format(_T("%04hx"),wArchivNr);
		sSNr.Format(_T("%04hx"),wSequenceNr);

		sNrRecords.Format(_T("%08lx"),dwRecords);
		sFirst.Format(_T("%s%s%03d"),tFirst.GetDBDate(),
			tFirst.GetDBTime(),
			tFirst.GetMilliseconds());
		
		sLast.Format(_T("%s%s%03d"),tLast.GetDBDate(),
			tLast.GetDBTime(),
			tLast.GetMilliseconds());

		int ret = r4success;
		if (bFoundOneDeleted)
		{
			// record "ent"löschen und alles auf leer setzen
			m_Data.recall();
			m_Data.blank();
		}
		else
		{
			ret = m_Data.appendStart();
			if (r4success == ret)
			{
				m_Data.blank();
//				TRACE(_T("appending new entry %d %s,%s\n"),m_Data.recCount(),sANr,sSNr);
			}
			else
			{
				WK_TRACE(_T("cannot appendStart writing sequence map %d\n"),m_CodeBase.errorCode);
				m_CodeBase.errorSet();
			}
		}
		if (ret == r4success)
		{
			if (AssignEntry(sANr,sSNr,sNrRecords,sFirst,sLast,sName))
			{
				if (!bFoundOneDeleted)
				{
					m_Data.append();
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
		}
		
		if (m_CodeBase.errorCode != 0)
		{
			WK_TRACE(_T("codebase error %d writing sequence map file\n"),m_CodeBase.errorCode);
			m_CodeBase.errorSet();
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
			sExpression.Format(_T("(%s='%s') .AND. (%s='%s')"), _T(ARCHIV_NR), sANr, _T(SEQUENCE_NR), sSNr);
			int rc = relation.querySet(sExpression);
			if (rc==r4success)
			{
				if (r4success == relation.top())
				{
					bFound = TRUE;
				}
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
		CString sANr,sSNr,sNrRecords,sFirst,sLast;
		
		sANr.Format(_T("%04hx"),wArchivNr);
		sSNr.Format(_T("%04hx"),wSequenceNr);

		sNrRecords.Format(_T("%08lx"),dwRecords);
		sFirst.Format(_T("%s%s%03d"),tFirst.GetDBDate(),
			tFirst.GetDBTime(),
			tFirst.GetMilliseconds());
		
		sLast.Format(_T("%s%s%03d"),tLast.GetDBDate(),
			tLast.GetDBTime(),
			tLast.GetMilliseconds());
		TRACE(_T("set entry at %d new value %s,%s\n"),m_Data.recNo(),sANr,sSNr);
		AssignEntry(sANr,sSNr,sNrRecords,sFirst,sLast,sName);
	}
	else
	{
		AppendEntry(wArchivNr,wSequenceNr,dwRecords,tFirst,tLast,sName);
	}
	acs.Unlock();
}

