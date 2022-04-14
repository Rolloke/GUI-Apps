// Search.cpp: implementation of the CSearch class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dvstorage.h"
#include "Search.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static const TCHAR szAND[] = _T(" .AND. ");

#define NR_SEND_QUERY_RESULTS	50

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CSearch::CSearch(DWORD dwUserID, 
			   DWORD dwClient, 
			   int iNumCollections,
			   const WORD Collections[],
			   int iNumFields,
			   const CIPCField fields[],
			   DWORD dwMaxQueryResults)
{

#ifdef _DEBUG
	for (int a=0;a<iNumFields;a++)
	{
		TRACE(_T("%d %s %c %s\n"),a,fields[a].GetName(), fields[a].GetType(),fields[a].GetValue());
	}
#endif
/////
	m_iResponses = 0;
	m_dwUserID = dwUserID;
	m_dwClient = dwClient;
	m_iLastIndicated = 0;
	m_bCancelled = FALSE;
	m_bConfirmed = FALSE;
	m_bForward = TRUE;

	m_dwMaxQueryResults = dwMaxQueryResults;
	m_dwSentQueryResults = 0;

	int i;
	CString sCollections;
	CCollection* pCollection;
	CString sStartTime,sStartDate,sEndTime,sEndDate;
	CString sRect;
	CString sMoreRect[4];
	CString sMoreRects;
	BOOL	bMilliseconds = FALSE;
	CString sRectangle;
	CString sIFrameOnly;

	WK_TRACE(_T("*** start query ***\n"));
	if (iNumCollections>0)
	{
		for (i=0;i<iNumCollections;i++)
		{
			pCollection = theApp.m_Archives.GetCollection(Collections[i]);
			if (pCollection)
			{
				m_Collections.Add(pCollection);
				TRACE(_T("query in %s\n"),pCollection->GetName());
			}
		}
	}


	CString op = _T('=');
	BOOL bLess = FALSE;
	BOOL bGreater = FALSE;
	CString sMilliseconds;
	CString sDate(_T(DVR_DATE));
	CString sTime(_T(DVR_TIME));

	DWORD dwStartms = 0;
	DWORD dwEndms = 0;

	for (i=0;i<iNumFields;i++)
	{
		bLess = FALSE;
		bGreater = FALSE;

		switch (fields[i].GetType())
		{
		case _T('<'):
			op = _T('<');
			bLess = TRUE;
			break;
		case _T('>'):
			op = _T('>');
			bGreater = TRUE;
			break;
		case _T('#'):
			op = _T('#');
			break;
		case _T('$'):
			op = _T('&');
			break;
		case _T('{'):
			op = _T("<=");
			bLess = TRUE;
			break;
		case _T('}'):
			op = _T(">=");
			bGreater = TRUE;
			break;
		default:
			op = _T('=');
			break;
		}

		if(   fields[i].GetName() == _T(DVD_MD_X)
			|| fields[i].GetName() == _T(DVD_MD_Y)
			|| fields[i].GetName() == _T(DBD_MD_X)
			|| fields[i].GetName() == _T(DBD_MD_Y))
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
			if (op==_T("$"))
			{
				sRect += _T("(") + fields[i].GetValue() + op + _T("'") + 
					fields[i].GetName() + _T("')");
				for(int j=0; j<4; j++)
				{
					s.Format(_T("%i"), j+2);
					sMoreRect[j] += _T("(") + fields[i].GetValue() + s + op + _T("'") + 
					fields[i].GetName() + _T("')");
				}
			}
			else
			{
				sRect += _T("(") + fields[i].GetName() + op + _T("'") + 
					fields[i].GetValue() + _T("')");
				for(int j=0; j<4; j++)
				{
					s.Format(_T("%i"), j+2);
					sMoreRect[j] += _T("(") + fields[i].GetName() + s + op + _T("'") + 
						fields[i].GetValue() + _T("')"); 
				}

			}
			//TRACE(_T("TKR ### sRect: %s\n"), sRect);
		}
		else if(fields[i].GetName() == _T(DVR_TYP))
		{
			//bei Tasha und Rechtecksuche nur IFrames suchen; IFrame = (DVR_TYP = 2)
			sIFrameOnly = _T("(") + fields[i].GetName() + op + _T("'") + 
				fields[i].GetValue() + _T("')");
		}
		else
		{
			if (!m_sExpression.IsEmpty())
			{
				m_sExpression += szAND;
			}

			// attention order of ops
			if (op==_T("$"))
			{
				m_sExpression += _T("(") + fields[i].GetValue() + op + _T("'") + 
					fields[i].GetName() + _T("')");
			}
			else
			{
				m_sExpression += _T("(") + fields[i].GetName() + op + _T("'") + 
					fields[i].GetValue() + _T("')");
			}
		}

		if (  (fields[i].GetName() == _T(DBP_DATE))
			|| (fields[i].GetName() == _T(DVR_DATE))
			)
		{
			if (bLess)
			{
				sEndDate = fields[i].GetValue();
			}
			if (bGreater)
			{
				sStartDate = fields[i].GetValue();
			}
		}
		else if ( (fields[i].GetName() == _T(DBP_TIME))
				 || (fields[i].GetName() == _T(DVR_TIME))
			    )
		{
			if (bLess)
			{
				sEndTime = fields[i].GetValue();
			}
			if (bGreater)
			{
				sStartTime = fields[i].GetValue();
			}
		}
		else if (fields[i].GetName() == _T(DVR_MS))
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
		}

	}

	m_sExpression = _T('(') + m_sExpression + _T(')');

	//Search between 2 dates and 2 times
	if (   !sStartTime.IsEmpty() 
		&& !sStartDate.IsEmpty() 
		&& !sEndTime.IsEmpty() 
		&& !sEndDate.IsEmpty())
	{
        TRACE(_T("special START END time optimize %s %s\n"),sStartDate,sStartTime);
		m_stStart.SetDBTime(sStartTime);
		m_stStart.SetDBDate(sStartDate);	
		m_stEnd.SetDBTime(sEndTime);
		m_stEnd.SetDBDate(sEndDate);
		if (bMilliseconds)
		{
			m_stStart.wMilliseconds = (WORD)dwStartms;
			m_stEnd.wMilliseconds = (WORD)dwEndms;
		}

		CString s1, s2, s3;


		//s1: (DVR_DATE = startdate && DVR_TIME >= starttime)
		s1 = _T("((") + sDate + _T("=\'") + sStartDate + _T("\') .AND. (");
		s1 += sTime + _T(">=\'") + sStartTime + _T("\'))");

		//s2: (DVR_DATE = enddate && DVR_TIME <= endtime)
		s2 = _T("((") + sDate + _T("=\'") + sEndDate + _T("\') .AND. (");
		s2 += sTime + _T("<=\'") + sEndTime + _T("\'))");

		//s3: (DVR_DATE > startdate && DVR_DATE < enddate)
		s3 = _T("((") + sDate + _T(">\'") + sStartDate + _T("\') .AND. (");
		s3 += sDate + _T("<\'") + sEndDate + _T("\'))");

		m_sExpression += _T(" .OR. ") + s1 + _T(" .OR. ") + s2 + _T(" .OR. ") + s3;
	}
	else if (!sStartTime.IsEmpty() && !sStartDate.IsEmpty())
	{
        TRACE(_T("special START time optimize %s %s\n"),sStartDate,sStartTime);
		m_stStart.SetDBTime(sStartTime);
		m_stStart.SetDBDate(sStartDate);
		if (bMilliseconds)
		{
			m_stStart.wMilliseconds = (WORD)dwStartms;
		}

		CString sAddon;
		m_sExpression = _T('(') + m_sExpression + _T(')');
		if (bMilliseconds)
		{
			// search for milliseconds too
			sAddon = _T(" .OR. (") + sDate + _T(">\'") + sStartDate + _T("\')");
			sAddon += _T(" .OR. (");
			sAddon += _T("(") + sTime + _T(">'") + sStartTime + _T("\')");
			sAddon += _T(" .AND. ");
			sAddon += _T("(") + sDate + _T("=\'") + sStartDate + _T("\')");
			sAddon += _T(")");
		}
		else
		{
			// auch das datum davor suchen
			sAddon = _T(" .OR. (") + sDate + _T(">\'") + sStartDate + _T("\')");
		}

		m_sExpression += sAddon;
	}
	else if (   !sEndTime.IsEmpty() 
		     && !sEndDate.IsEmpty())
	{
		// rückwärts suchen
        TRACE(_T("special END time optimize %s %s\n"),sEndDate,sEndTime);
		m_stEnd.SetDBTime(sEndTime);
		m_stEnd.SetDBDate(sEndDate);
		if (bMilliseconds)
		{
			m_stEnd.wMilliseconds = (WORD)dwEndms;
		}
		m_bForward = FALSE;

		CString sAddon;
		m_sExpression = _T('(') + m_sExpression + _T(')');
		if(bMilliseconds)
		{
			// search for milliseconds too
			sAddon = _T(" .OR. (") +sDate + _T("<\'") + sEndDate + _T("\')");
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
		m_sExpression += sAddon;
	}
	
	//WK_TRACE(_T("TKR ###### New Search ID: %d  DateTime: %s\n"), dwUserID, m_sExpression);
	if(!sRect.IsEmpty())
	{
		for (int j=0; j<4; j++)
		{
			if(sMoreRects.IsEmpty())
			{
				sMoreRects = _T("(") + sMoreRect[j] + _T(")");
			}
			else
			{
				sMoreRects += _T(" .OR. (") + sMoreRect[j] + _T(")");
				//TRACE(_T("TKR ### More: %s\n"), sMoreRects);
			}
		}

		sRect = _T("(") + sRect + _T(")") + _T(".OR.") + sMoreRects;
		sRect = _T('(') + sRect + _T(')');
		m_sExpression = sRect + _T(" .AND. (") + m_sExpression + _T(")");

		if(!sIFrameOnly.IsEmpty())
		{
			//bei Tasha und Rechtecksuche nur IFrames suchen; IFrame = (DVR_TYP = 2)
			m_sExpression = sIFrameOnly + _T(" .AND. (") + m_sExpression + _T(")");
		}

		//WK_TRACE(_T("TKR ### %s\n"), m_sExpression.Left(m_sExpression.GetLength()-300));
		//WK_TRACE(_T("TKR ### %s\n"), m_sExpression.Right(300));
		//WK_TRACE(_T("TKR ### Rectangle Search\n"));

	}
	else
	{
		TRACE(_T("no Rectangle Search\n"));
	}
	m_sExpression.Replace(_T("DBP_"),_T("DVR_"));
	m_sExpression.Replace(_T("DBD_"),_T("DVD_"));

	//WK_TRACE(_T("** Query Expression: %s\n"), m_sExpression);
}
//////////////////////////////////////////////////////////////////////
CSearch::~CSearch()
{
	m_Results.SafeDeleteAll();
}
//////////////////////////////////////////////////////////////////////
CCollection* CSearch::GetAndRemoveCollection()
{
	CCollection* pCollection = NULL;
	
	if (m_Collections.GetSize())
	{
		pCollection = m_Collections.GetAtFast(0);
		m_Collections.RemoveAt(0);
	}
	return pCollection;
}
//////////////////////////////////////////////////////////////////////
void CSearch::SendSearchResults(CClient* pClient)
{
	int diff = GetIndicationDifferenz();

	if ( (diff>=0) && (diff<NR_SEND_QUERY_RESULTS) )
	{
		if (m_iLastIndicated<m_Results.GetSize())
		{
			CIPCDatabaseStorage* pCIPC = pClient->GetCIPC();
			CTape* pTape = NULL;
			CStorageId* pStorageID = m_Results.GetAt(m_iLastIndicated++);
			
			// get the Tape
			CCollection* pCollection = theApp.m_Archives.GetCollection(pStorageID->GetCollectionNr());
			if (pCollection)
			{
				pTape = pCollection->GetTapeID(pStorageID->GetTapeNr());
			}
			if (pTape)
			{
				SendSearchResult(pCIPC,pTape,pStorageID);
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CSearch::AddSearchResult(CTape* pSeq, const CStorageId& id)
{

	if (m_bCancelled)
	{
		//WK_TRACE(_T("TKR ### CSearch::AddSearchResult m_bCancelled: %d\n"), m_bCancelled);
		return FALSE;
	}

	CClient* pClient = theApp.m_Clients.GetClient(GetClientID());
	if (pClient && pClient->IsConnected())
	{
/*		WK_TRACE(_T("QUERY%d result Nr= %d A=%d,S=%d,R=%d\n"),
				 GetUserID(),
				 m_Results.GetSize(),
				 id.GetCollectionNr(),id.GetTapeNr(),id.GetImageNr());*/


		// GF 22.01.2002
		// Die Suche kann ja weit mehr Results finden als angefragt
		// Es duerfen aber nicht mehr Results in die Queue, als angefordert wurden,
		// sonst wartet sich der Thread bei den Responses tot...
		// Daher ist m_Results.GetSize() das Ignorier-Kriterium
		
		if (   m_dwMaxQueryResults == 0
			|| (DWORD)m_Results.GetSize() < m_dwMaxQueryResults)
		{
			m_Results.SafeAdd(new CStorageId(id));
		}


		if (   m_dwMaxQueryResults > 0
			&& m_dwSentQueryResults >= m_dwMaxQueryResults
			)
		{
			Cancel();
			//WK_TRACE(_T("TKR ### CSearch::AddSearchResult: Cancel() setzen: m_dwSentQueryResults: %d\n"), m_dwSentQueryResults);
			return FALSE;
		}

		return TRUE;
	}
	else
	{
		//WK_TRACE(_T("TKR ### CSearch::AddSearchResult: (pClient && pClient->IsConnected()) == FALSE\n"));
		return FALSE;
	}

}
//////////////////////////////////////////////////////////////////////
void CSearch::SendSearchResult(CIPCDatabaseStorage* pCIPC,
							   CTape* pTape, 
							   CStorageId* pStorageID)
{
	if (   m_dwMaxQueryResults==0
		|| m_dwSentQueryResults<m_dwMaxQueryResults)
	{
		CIPCFields fields;
		// query result only data
		if (pTape->GetFields(pStorageID->GetImageNr(),fields))
		{
			pCIPC->DoIndicateNewQueryResult(GetUserID(),
				pStorageID->GetCollectionNr(),
				pStorageID->GetTapeNr(),
				pStorageID->GetImageNr(),
				fields);
			m_dwSentQueryResults++;
		}
	}
	else
	{
		Cancel();
	}
}
//////////////////////////////////////////////////////////////////////
void CSearch::IncreaseResponses()
{
	m_iResponses++;
}
//////////////////////////////////////////////////////////////////////
int CSearch::GetIndicationDifferenz()
{
	return (m_iLastIndicated+1) - m_iResponses;
}
//////////////////////////////////////////////////////////////////////
BOOL CSearch::HasEnoughResponses()
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
				CIPCDatabaseStorage* pCIPC = pClient->GetCIPC();
				CTape* pTape = NULL;

				while ( (diff>0) && (m_iLastIndicated<m_Results.GetSize()))
				{
					CStorageId* pStorageID = m_Results.GetAt(m_iLastIndicated++);
					
					// get the Tape
					CCollection* pCollection = theApp.m_Archives.GetCollection(pStorageID->GetCollectionNr());
					if (pCollection)
					{
						pTape = pCollection->GetTapeID(pStorageID->GetTapeNr());
						if (pTape)
						{
							SendSearchResult(pCIPC,pTape,pStorageID);
						}
					}
					diff--;
				}
			}
			return FALSE;
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
void CSearch::Cancel()
{
	if (!m_bCancelled)
	{
		//WK_TRACE(_T("TKR ### CSearch::Cancel() = TRUE\n"));
		m_bCancelled = TRUE;
	}
}
//////////////////////////////////////////////////////////////////////
void CSearch::DoConfirmQuery()
{
	if (!m_bConfirmed)
	{
		m_bConfirmed = TRUE;
		CClient* pClient = theApp.m_Clients.GetClient(GetClientID());
		if (pClient && pClient->IsConnected())
		{
			//WK_TRACE(_T("TKR ### CSearch::DoConfirmQuery  ID: %d\n"), GetUserID());
			pClient->GetCIPC()->DoConfirmQuery(GetUserID());
		}
	}
}
