// Query.cpp: implementation of the CQuery class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dbs.h"
#include "Query.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static const TCHAR szAND[] = _T(" .AND. ");
static const TCHAR szOR[] = _T(" .OR. ");

#define NR_SEND_QUERY_RESULTS	50

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQuery::CQuery(DWORD dwUserID, DWORD dwClient, const CString& sQuery)
{
	m_iResponses = 0;
	m_dwUserID = dwUserID;
	m_dwClient = dwClient;
	m_sQuery = sQuery;
	m_iLastIndicated = 0;
	m_bCancelled = FALSE;
	m_bConfirmed = FALSE;
	m_dwMaxQueryResults = 0;
	m_dwSentQueryResults = 0;
	m_bForward = TRUE;
#ifdef TRACE_LOCKS
	int nCurrentThread = theApp.TraceLocks() ? 0 : -1;
	m_Results.m_nCurrentThread = nCurrentThread;
#endif

	Analyze();
}
//////////////////////////////////////////////////////////////////////
void AddToString(CString& sString, const CString& sAddon, const CString& sOp)
{
	if (!sAddon.IsEmpty())
	{
		if (sString.IsEmpty())
		{
			sString = sAddon;
		}
		else
		{
			sString = sString + sOp + sAddon;
		}
	}
}
//////////////////////////////////////////////////////////////////////
CQuery::CQuery(DWORD dwUserID, 
			   DWORD dwClient, 
			   int iNumArchives,
			   const WORD archives[],
			   int iNumFields,
			   const CIPCField fields[],
			   DWORD dwMaxQueryResults)
{

	m_dwMaxQueryResults = (DWORD)LOWORD(dwMaxQueryResults);
	m_bSpecialQueryCancel = HIWORD(dwMaxQueryResults); //special query cancel on rectangle play
													   // see comment at IsSpecialQueryCancel()
	m_dwSentQueryResults = 0;
	m_bForward = TRUE;

	m_iResponses = 0;
	m_dwUserID = dwUserID;
	m_dwClient = dwClient;
	m_iLastIndicated = 0;
	m_bCancelled = FALSE;
	m_bConfirmed = FALSE;

	int i;
	CString sArchives;
	CArchiv* pArchiv;
	CString sStartTime,sStartDate,sEndTime,sEndDate,sRect,sExactDate,sExactTime;
	CString sMoreRect[4];
	CString sMoreRects;
	CString sMilliseconds;

	BOOL bMilliseconds = FALSE;
	DWORD dwStartms = 0;
	DWORD dwEndms = 0;

	if (iNumArchives>0)
	{
		for (i=0;i<iNumArchives;i++)
		{
			pArchiv = theApp.m_Archives.GetArchiv(archives[i]);
			if (pArchiv)
			{
				m_Archives.Add(pArchiv);
				WK_TRACE(_T("query in %s\n"),pArchiv->GetName());
			}
			else
			{
				WK_TRACE_ERROR(_T("query INVALID archive %04x\n"),archives[i]);
			}
		}
	}

	CString op = '=';
	BOOL bLess = FALSE;
	BOOL bGreater = FALSE;
	BOOL bEqual = FALSE;
	
	CString sDate(CArchiv::m_sDATE);
	CString sTime(CArchiv::m_sTIME);
	CString s;
	CString sDBD = _T("DBD_");
	if (theApp.IsDTS())
	{
		sDBD = _T("DVD_");
	}

	CString sExpressionDateTime,sExpressionData;

	for (i=0;i<iNumFields;i++)
	{
		bLess = FALSE;
		bGreater = FALSE;

		switch (fields[i].GetType())
		{
		case '<':
			op = '<';
			bLess = TRUE;
			break;
		case '>':
			op = '>';
			bGreater = TRUE;
			break;
		case '#':
			op = '#';
			break;
		case '$':
			op = '&';
			break;
		case '{':
			op = "<=";
			bLess = TRUE;
			break;
		case '}':
			op = ">=";
			bGreater = TRUE;
			break;
		default:
			if (0==fields[i].GetName().Find(sDBD))
			{
				op = '$';
			}
			else
			{
				op = '=';
				bEqual = TRUE;
			}
			break;
		}

		//TRACE(_T("TKR FeldName: %s\n"), fields[i].GetName());

		if(	   fields[i].GetName() == _T(DBP_MD_X)
			|| fields[i].GetName() == _T(DBP_MD_Y)
			|| fields[i].GetName() == _T(DVD_MD_X)
			|| fields[i].GetName() == _T(DVD_MD_Y))
		{
			if (!sRect.IsEmpty())
			{
				sRect += szAND;
				for(int iCount=0; iCount<4; iCount++)
				{
					sMoreRect[iCount] += szAND;
				}
			}
			
			 CString s;
			// attention order of ops
			if (op=="$")
			{
				sRect += "(" + fields[i].GetValue() + op + "'" + 
					fields[i].GetName() + "')";
				for(int j=0; j<4; j++)
				{
					s.Format(_T("%i"), j+2);
					sMoreRect[j] += "(" + fields[i].GetValue() + s + op + "'" + 
					fields[i].GetName() + "')";
				}
			}
			else
			{
				sRect += "(" + fields[i].GetName() + op + "'" + 
					fields[i].GetValue() + "')";
				for(int j=0; j<4; j++)
				{
					s.Format(_T("%i"), j+2);
					sMoreRect[j] += "(" + fields[i].GetName() + s + op + "'" + 
						fields[i].GetValue() + "')"; 
				}

			}
		}
		else if (   fields[i].GetName() == _T(DBP_DATE)
				 || fields[i].GetName() == _T(DVR_DATE))
		{
			if (bLess)
			{
				sEndDate = fields[i].GetValue();
			}
			if (bGreater)
			{
				sStartDate = fields[i].GetValue();
			}
			if (bEqual)
			{
				sExactDate = fields[i].GetValue();
			}
			s = "(" + fields[i].GetName() + op + "'" + fields[i].GetValue() + "')";
			AddToString(sExpressionDateTime,s,szAND);
		}
		else if (   fields[i].GetName() == _T(DBP_TIME)
			     || fields[i].GetName() == _T(DVR_TIME))
		{
			if (bLess)
			{
				sEndTime = fields[i].GetValue();
			}
			if (bGreater)
			{
				sStartTime = fields[i].GetValue();
			}
			if (bEqual)
			{
				sExactTime = fields[i].GetValue();
			}
			s = "(" + fields[i].GetName() + op + "'" + fields[i].GetValue() + "')";
			AddToString(sExpressionDateTime,s,szAND);
		}
		else if (   fields[i].GetName() == _T(DBP_MS)
				 || fields[i].GetName() == _T(DVR_MS))
		{
			bMilliseconds = TRUE;
			if (bLess)
			{
				_stscanf(fields[i].GetValue(),_T("%03d"),&dwEndms);
			}
			if (bGreater)
			{
				_stscanf(fields[i].GetValue(),_T("%03d"),&dwStartms);
			}
			s = "(" + fields[i].GetName() + op + "'" + fields[i].GetValue() + "')";
			AddToString(sExpressionDateTime,s,szAND);
		}
		else
		{
			// attention order of ops
			if (op=="$")
			{
				CString sV = fields[i].GetValue();
				sV.MakeUpper();
				s = "('" + sV + "' " + op  + " UPPER("+fields[i].GetName() + "))";
			}
			else
			{
				s = "(" + fields[i].GetName() + op + "'" + fields[i].GetValue() + "')";
			}
			AddToString(sExpressionData,s,szAND);
		}
	}


	TRACE(_T("Start %s,%s\n"),sStartDate,sStartTime);
	TRACE(_T("End %s,%s\n"),sEndDate,sEndTime);
	TRACE(_T("Exact %s,%s\n"),sExactDate,sExactTime);
	
	//Search between 2 dates and 2 times
	if (   !sStartTime.IsEmpty() 
		&& !sStartDate.IsEmpty()
		&& !sEndTime.IsEmpty() 
		&& !sEndDate.IsEmpty())
	{
        WK_TRACE(_T("special start time optimize %s %s\n"),sStartDate,sStartTime);
		m_stStart.SetDBTime(sStartTime);
		m_stStart.SetDBDate(sStartDate);	
		m_stEnd.SetDBTime(sEndTime);
		m_stEnd.SetDBDate(sEndDate);

		CString s1, s2, s3;

		//s1: (DBP_DATE = startdate && DBP_TIME >= starttime)
		s1 = _T("((") + sDate + _T("=\'") + sStartDate + _T("\') .AND. (");
		s1 += CString(CArchiv::m_sTIME) + _T(">=\'") + sStartTime + _T("\'))");

		//s2: (DBP_DATE = enddate && DBP_TIME <= endtime)
		s2 = _T("((") + sDate + _T("=\'") + sEndDate + _T("\') .AND. (");
		s2 += CString(CArchiv::m_sTIME) + _T("<=\'") + sEndTime + _T("\'))");

		//s3: (DBP_DATE > startdate && DBP_DATE < enddate)
		s3 = _T("((") + sDate + _T(">\'") + sStartDate + _T("\') .AND. (");
		s3 += sDate + _T("<\'") + sEndDate + _T("\'))");

		sExpressionDateTime += szOR + s1 + szOR + s2 + szOR + s3;
	}
	else if (   !sStartTime.IsEmpty() 
		     && !sStartDate.IsEmpty())
	{
        WK_TRACE(_T("special START time optimize %s %s\n"),sStartDate,sStartTime);
		m_stStart.SetDBTime(sStartTime);
		m_stStart.SetDBDate(sStartDate);
		if (bMilliseconds)
		{
			m_stStart.wMilliseconds = (WORD)dwStartms;
		}

		CString sAddon;
		sExpressionDateTime = _T('(') + sExpressionDateTime + _T(')');
		if (bMilliseconds)
		{
			// search for milliseconds too
			sAddon = _T(" .OR. (") + sDate + _T(">\'") + sStartDate + _T("\')");
			sAddon += _T(" .OR. (");
			sAddon += _T("(") + sTime + _T(">'") + sStartTime + _T("\')");
			sAddon += szAND;
			sAddon += _T("(") + sDate + _T("=\'") + sStartDate + _T("\')");
			sAddon += _T(")");
		}
		else
		{
			// auch das datum davor suchen
			sAddon = _T(" .OR. (") + sDate + _T(">\'") + sStartDate + _T("\')");
		}

		sExpressionDateTime += sAddon;
	}
	else if (   !sEndTime.IsEmpty() 
		     && !sEndDate.IsEmpty())
	{
		// rückwärts suchen
        WK_TRACE(_T("special END time optimize %s %s\n"),sEndDate,sEndTime);
		m_stEnd.SetDBTime(sEndTime);
		m_stEnd.SetDBDate(sEndDate);
		if (bMilliseconds)
		{
			m_stEnd.wMilliseconds = (WORD)dwEndms;
		}
		m_bForward = FALSE;

		CString sAddon;
		sExpressionDateTime = '(' + sExpressionDateTime + ')';
		if(bMilliseconds)
		{
			// search for milliseconds too
			sAddon = _T(" .OR. (") + sDate + _T("<\'") + sEndDate + _T("\')");
			sAddon += _T(" .OR. (");
			sAddon += _T("(") + sTime + _T("<'") + sEndTime + _T("\')");
			sAddon += _T(".AND. ");
			sAddon += _T("(") + sDate + _T("=\'") + sEndDate + _T("\')");
			sAddon += _T(")");
		}
		else
		{
			sAddon = _T(" .OR. (") + sDate + _T("<\'") + sEndDate + _T("\')");
		}
		sExpressionDateTime += sAddon;
	}
	else if (	!sStartTime.IsEmpty()
		     && !sEndTime.IsEmpty()
			 && !sExactDate.IsEmpty())
	{
		m_stStart.SetDBDate(sExactDate);
		m_stStart.SetDBTime(sStartTime);
		m_stEnd.SetDBDate(sExactDate);
		m_stEnd.SetDBTime(sEndTime);
	}
	else if (	!sExactDate.IsEmpty()
			 && !sExactTime.IsEmpty())
	{
		m_stStart.SetDBDate(sExactDate);
		m_stStart.SetDBTime(sExactTime);
		m_stEnd.SetDBDate(sExactDate);
		m_stEnd.SetDBTime(sExactTime);
	}

	if(!sRect.IsEmpty())
	{
//#ifndef _DEBUG
		for(int j=0; j<4; j++)
		{
			if(sMoreRects.IsEmpty())
			{
				sMoreRects = _T("(") + sMoreRect[j] + _T(")");
			}
			else
			{
				sMoreRects += _T(" .OR. (") + sMoreRect[j] + _T(")");
				//TRACE(_T("TKR More: %s\n"), sMoreRects);
			}
		}

		sRect = _T("(") + sRect + _T(")") + szOR + sMoreRects;
//#endif
		sRect = _T('(') + sRect + _T(')');

		if (theApp.IsIDIP())
		{
			sRect.Replace(_T("DVD_MD"),_T("DBD_MD"));
		}
		else if (theApp.IsDTS())
		{
			sRect.Replace(_T("DBD_MD"),_T("DVD_MD"));
		}
		//TRACE(_T("** Rectangle Search <%s>\n"),sRect);
	}
	else
	{
		TRACE(_T("** no Rectangle Search\n"));
	}

	if (!sExpressionDateTime.IsEmpty())
	{
		sExpressionDateTime = _T('(') + sExpressionDateTime + _T(')');
	}
	if (!sExpressionData.IsEmpty())
	{
		sExpressionData = _T('(') + sExpressionData + _T(')');
	}

	//TRACE(_T("DateTime =<%s>\n"),sExpressionDateTime);
	//TRACE(_T("Data =<%s>\n"),sExpressionData);
	//TRACE(_T("Rect =<%s>\n"),sRect);

	m_sExpression = sExpressionDateTime;
	AddToString(m_sExpression,sExpressionData,szAND);
	AddToString(m_sExpression,sRect,szAND);

	if (theApp.IsIDIP())
	{
		m_sExpression.Replace(_T("DVR_"),_T("DBP_"));
	}
	else
	{
		m_sExpression.Replace(_T("DBP_"),_T("DVR_"));
	}

	CSystemTime emptyTime;
	if (m_stStart>emptyTime)
	{
		WK_TRACE(_T("query Start <%s>\n"),m_stStart.GetDateTime());
	}
	if (m_stEnd>emptyTime)
	{
		WK_TRACE(_T("query End <%s>\n"),m_stEnd.GetDateTime());
	}

	if (theApp.TraceQuery())
	{
		if (m_sExpression.GetLength()>300)
		{
			WK_TRACE(_T("Max=%d, EXPRESSION1=<%s>\n"),m_dwMaxQueryResults,m_sExpression.Left(300));
			WK_TRACE(_T("EXPRESSION2=<%s>\n"),m_sExpression.Mid(300));
		}
		else
		{
			WK_TRACE(_T("Max=%d, EXPRESSION=<%s>\n"),m_dwMaxQueryResults,m_sExpression);
		}
	}
}
//////////////////////////////////////////////////////////////////////
CQuery::~CQuery()
{
	m_Results.SafeDeleteAll();
}
//////////////////////////////////////////////////////////////////////
void CQuery::Analyze()
{
	// sample
	// ((ARC_NR = 8) .OR. (ARC_NR = 1) .OR. (ARC_NR = 14)) .AND. (KTO_NR = '0000011111')
	WK_TRACE(_T("<%s>\n"),m_sQuery);
	
	int i;
	CString sQuery;
	CString sArchivs,sArchiv;

	sQuery = m_sQuery;
	if (-1 != sQuery.Find(_T("ARC_NR")))
	{
		// there are some special archivs
		int p;
		p = sQuery.Find(_T(".AND."));
		if (p!=-1)
		{
			sArchivs = sQuery.Left(p);
			m_sExpression = sQuery.Mid(p+1+sizeof(_T(".AND.")));
			m_sExpression.TrimLeft();
			m_sExpression.TrimRight();
		}
		else
		{
			// only archivs ?? no expression
			sArchivs = sQuery;
		}
		// not only expression search archivs
		for (i=0; i<theApp.m_Archives.GetSize();i++)
		{
			sArchiv.Format(_T("(ARC_NR = %d)"),theApp.m_Archives.GetAtFast(i)->GetNr());
			if (-1!=sArchivs.Find(sArchiv))
			{
				m_Archives.Add(theApp.m_Archives.GetAtFast(i));
			}
		}
	}
	else
	{
		// only expression all archivs
		for (i=0; i< theApp.m_Archives.GetSize();i++)
		{
			m_Archives.Add(theApp.m_Archives.GetAtFast(i));
		}
		m_sExpression = sQuery;
		m_sExpression.TrimLeft();
		m_sExpression.TrimRight();
	}

	for (i=0; i< m_Archives.GetSize();i++)
	{
		WK_TRACE(_T("searching in %d %s for <%s>\n"),m_Archives.GetAtFast(i)->GetNr(),
			m_Archives.GetAtFast(i)->GetName(),m_sExpression);
	}

	CStringArray saFields;

	SplitString(m_sExpression,saFields,CString(szAND));

	CString sComparison;
	m_sExpression.Empty();
	for (i=0;i<saFields.GetSize();i++)
	{
		sComparison = TranslateComparison(saFields.GetAt(i));

		if (!sComparison.IsEmpty())
		{
			if (!m_sExpression.IsEmpty())
			{
				m_sExpression += szAND;
			}
			m_sExpression += sComparison;
		}
	}

	WK_TRACE(_T("expression = <%s>\n"),m_sExpression);
}
//////////////////////////////////////////////////////////////////////
CString CQuery::TranslateComparison(const CString& sComparison)
{
	CString sTemp,sRet,sField,sValue;
	int p;
	char c = '0';

	sTemp = sComparison;
	sTemp.TrimLeft();
	sTemp.TrimRight();

	if (sTemp.IsEmpty())
	{
		return sRet;
	}
	if (sTemp[0]!='(')
	{
		return sRet;
	}
	sTemp = sTemp.Mid(1);
	p = sTemp.Find('=');
	if (p!=-1)
	{
		c = '=';
	}
	else
	{
		p = sTemp.Find('<');
		if (p!=-1)
		{
			c = '<';
		}
		else
		{
			p = sTemp.Find('>');
			if (p!=-1)
			{
				c = '>';
			}
			else
			{
				return sRet;
			}
		}
	}

	sField = sTemp.Left(p);
	sValue = sTemp.Mid(p+1);

	sField.TrimLeft();
	sField.TrimRight();

	sValue.TrimLeft();
	sValue.TrimRight();

	if (sValue.IsEmpty() || sField.IsEmpty())
	{
		return sRet;
	}

	// field is OK
	// value is '1111') or 1111)
	// so drop quotes and )
	if (sValue.GetAt(sValue.GetLength()-1)==')')
	{
		sValue = sValue.Left(sValue.GetLength()-1);
	}
	if (sValue.IsEmpty())
	{
		return sRet;
	}
	// value is '1111' or 1111
	if (sValue.GetAt(0)=='\'')
	{
		sValue = sValue.Mid(1);
	}
	if (sValue.IsEmpty())
	{
		return sRet;
	}
	if (sValue.GetAt(sValue.GetLength()-1)=='\'')
	{
		sValue = sValue.Left(sValue.GetLength()-1);
	}
	if (sValue.IsEmpty())
	{
		return sRet;
	}
	// now both are OK
	sField = CIPCField::Translate3xTo4x(sField);
	if (sField.IsEmpty())
	{
		return sRet;
	}

	CIPCField* pField = theApp.m_Fields.GetCIPCField(sField);
	if (NULL==pField)
	{
		return sRet;
	}

	sValue = CIPCField::FillLeadingZeros(sValue,pField->GetMaxLength());

	sRet = _T("(") + sField + _T(" ") + c + _T("'") + sValue + _T("')");

	return sRet;
}
//////////////////////////////////////////////////////////////////////
CArchiv* CQuery::GetAndRemoveArchiv()
{
	CArchiv* pArchiv = NULL;
	
	if (m_Archives.GetSize())
	{
		pArchiv = m_Archives.GetAtFast(0);
		m_Archives.RemoveAt(0);
	}
	return pArchiv;
}

//////////////////////////////////////////////////////////////////////
void CQuery::SendQueryResults(CClient* pClient)
{
	int diff = GetIndicationDifferenz();

	if ( (diff>=0) && (diff<NR_SEND_QUERY_RESULTS) )
	{
		if (m_iLastIndicated<m_Results.GetSize())
		{
			CIPCDatabaseServer* pCIPC = pClient->GetCIPCDatabaseServer();
			CSequence* pSequence = NULL;
			if (theApp.TraceQuery())
			{
				WK_TRACE(_T("QUERY%d sending result nr %d\n"),GetUserID(),m_iLastIndicated);
			}
			CDBSId* pDbsId = m_Results.GetAtFast(m_iLastIndicated++);
			
			// get the sequence
			CArchiv* pArchiv = theApp.m_Archives.GetArchiv(pDbsId->GetArchiveNr());
			if (pArchiv)
			{
				pSequence = pArchiv->GetSequenceID(pDbsId->GetSequenceNr());
			}
			if (pSequence)
			{
				SendQueryResult(pCIPC,pSequence,pDbsId);
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CQuery::AddQueryResult(CSequence* pSeq, const CDBSId& id)
{
	if (m_bCancelled)
	{
		return FALSE;
	}

	CClient* pClient = theApp.m_Clients.GetClient(GetClientID());
	if (pClient && pClient->IsConnected())
	{
		if (m_dwMaxQueryResults>0)
		{
			if ((DWORD)m_Results.GetSize()<m_dwMaxQueryResults)
			{
				if (theApp.TraceQueryResults())
				{
					WK_TRACE(_T("QUERY%d result Nr= %d A=%d,S=%d,R=%d Expression=%s\n"),
						GetUserID(),
						m_Results.GetSize(),
						id.GetArchiveNr(),id.GetSequenceNr(),id.GetRecordNr(),
						GetExpression());
				}
				 
				m_Results.SafeAdd(new CDBSId(id));
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			if (theApp.TraceQueryResults())
			{
				WK_TRACE(_T("QUERY%d result Nr= %d A=%d,S=%d,R=%d Expression=%s\n"),
					GetUserID(),
					m_Results.GetSize(),
					id.GetArchiveNr(),id.GetSequenceNr(),id.GetRecordNr(),
					GetExpression());
			}
					 
			m_Results.SafeAdd(new CDBSId(id));
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}

}
//////////////////////////////////////////////////////////////////////
void CQuery::SendQueryResult(CIPCDatabaseServer* pCIPC,
							 CSequence* pSequence, 
							 CDBSId* pDbsId)
{
	CIPCFields fields;
	if (pCIPC->GetVersion()<3)
	{
		// query result with picture
		CIPCMediaSampleRecord* pMSR = NULL;
		CIPCPictureRecord* pPR = NULL;
		
		pSequence->GetRecord((WORD)pDbsId->GetRecordNr(),pCIPC,TRUE,pPR,pMSR,fields);
		if (pPR)
		{
			pCIPC->DoIndicateQueryResult(GetUserID(),*pPR);
		}
		else
		{
			// TODO
		}
		WK_DELETE(pPR);
		WK_DELETE(pMSR);
	}
	else
	{
		// query result only data
		if (   m_dwMaxQueryResults==0
			|| m_dwSentQueryResults<m_dwMaxQueryResults)
		{
			if (pSequence->GetFields(pDbsId->GetRecordNr(),fields))
			{
				//WK_TRACE(_T("TKR ######### DoIndicateNewQueryResult() RecNr: %d  Thread: %d\n"), 
				//	pDbsId->GetRecordNr(), GetCurrentThreadId());
				pCIPC->DoIndicateNewQueryResult(GetUserID(),
					pDbsId->GetArchiveNr(),
					pDbsId->GetSequenceNr(),
					pDbsId->GetRecordNr(),
					fields);
			}
			m_dwSentQueryResults++;
			if(IsSpecialQueryCancel())
			{
				//TRACE(_T("TKR +++++++ CQuery::SendQueryResult() selbstständiges Cancel\n"));
				if(m_pQueryThread)
				{
					m_pQueryThread->Cancel();
				}
			}
		}
		else
		{
			Cancel();
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CQuery::IncreaseResponses()
{
	if (theApp.TraceQuery())
	{
		WK_TRACE(_T("QUERY%d response %d\n"),GetUserID(),m_iResponses);
	}
	m_iResponses++;
}
//////////////////////////////////////////////////////////////////////
int CQuery::GetIndicationDifferenz()
{
	return (m_iLastIndicated+1) - m_iResponses;
}
//////////////////////////////////////////////////////////////////////
BOOL CQuery::HasEnoughResponses()
{
	if (m_iLastIndicated==0)
	{
		// the start don't wait
		return TRUE;
	}

	int diff = GetIndicationDifferenz();
	
	if ( (diff>=0) && (diff<NR_SEND_QUERY_RESULTS))
	{
		if (m_iLastIndicated<m_Results.GetSize())
		{
			CClient* pClient = theApp.m_Clients.GetClient(GetClientID());
			if (pClient && pClient->IsConnected())
			{
				CIPCDatabaseServer* pCIPC = pClient->GetCIPCDatabaseServer();
				CSequence* pSequence = NULL;

				while (   (diff>0) 
					   && (m_iLastIndicated<m_Results.GetSize())
					   && !m_bCancelled)
				{
					if (theApp.TraceQuery())
					{
						WK_TRACE(_T("QUERY%d sending result nr %d\n"),GetUserID(),m_iLastIndicated);
					}
					CDBSId* pDbsId = m_Results.GetAtFast(m_iLastIndicated++);
					
					// get the sequence
					CArchiv* pArchiv = theApp.m_Archives.GetArchiv(pDbsId->GetArchiveNr());
					if (pArchiv)
					{
						pSequence = pArchiv->GetSequenceID(pDbsId->GetSequenceNr());
						if (pSequence)
						{
							SendQueryResult(pCIPC,pSequence,pDbsId);
						}
					}
					diff--;
				}
			}
			return m_bCancelled;
		}
		else
		{
			return TRUE;
		}
	}
	else
	{
		// too many results waiting for responses
		// in HasEnoughResponses
		return FALSE;
	}
}
///////////////////////////////////////////////////////////
void CQuery::Cancel()
{
	m_bCancelled = TRUE;
}
//////////////////////////////////////////////////////////////////////
void CQuery::DoConfirmQuery()
{
	if (!m_bConfirmed)
	{
		m_bConfirmed = TRUE;
		CClient* pClient = theApp.m_Clients.GetClient(GetClientID());
		if (pClient && pClient->IsConnected())
		{
			TRACE(_T("DoConfirmQuery %d\n"),GetUserID());
			pClient->GetCIPCDatabaseServer()->DoConfirmQuery(GetUserID());
		}
	}
}
