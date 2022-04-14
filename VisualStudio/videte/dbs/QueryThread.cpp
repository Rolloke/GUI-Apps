// QueryThread.cpp: implementation of the CQueryThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dbs.h"
#include "mainfrm.h"
#include "QueryThread.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQueryThread::CQueryThread(CQuery* pQuery) : CWK_Thread(_T("QueryThread"),pQuery)
{
	//TRACE(_T("TKR ####### ::CQueryThread:  pQuery: 0x%x\n"), pQuery);
	m_pQuery = pQuery;
	if(m_pQuery)
	{
		m_pQuery->m_pQueryThread = this;
	}
	m_bCancelled = FALSE;
}
//////////////////////////////////////////////////////////////////////
CQueryThread::~CQueryThread()
{
	//TRACE(_T("TKR ####### ::~CQueryThread():  pQuery: 0x%x\n"), m_pQuery);
	WK_DELETE(m_pQuery);
}
//////////////////////////////////////////////////////////////////////
void CQueryThread::Cancel()
{
	//TRACE(_T("TKR ####### CQueryThread::Cancel()\n"));
	m_bCancelled = TRUE;

	m_pQuery->Cancel();
}
//////////////////////////////////////////////////////////////////////
BOOL CQueryThread::Run(LPVOID lpContext)
{
	BOOL bRet = FALSE;
	CClient* pClient;
	CArchiv* pArchiv;
	//TRACE(_T("TKR RUN 01\n"));

	pArchiv = m_pQuery->GetAndRemoveArchiv();

	pClient = theApp.m_Clients.GetClient(m_pQuery->GetClientID());
	if (pArchiv && pClient && !m_bCancelled)
	{
		// we still have an archiv for search and the client is
		// still connected
		//TRACE(_T("TKR RUN 02\n"));
		bRet = Query(pArchiv);

		//TRACE(_T("TKR RUN 03\n"));
		//TRACE(_T("TKR ######## Query OK: pArchiv: 0x%x  pClient: 0x%x  m_bCancelled: %d\n")
		//	, pArchiv, pClient,m_bCancelled);
	}
	else
	{
		// end of search
		
		// is everything send ??
		// now we have to wait for all responses
		if (!WaitForResponses())
		{
			bRet = FALSE;
		}
		else
		{
			pClient = theApp.m_Clients.GetClient(m_pQuery->GetClientID());
			if (pClient && pClient->IsConnected())
			{
				//TRACE(_T("TKR RUN 05\n"));
				//TRACE(_T("TKR ######## Query canceled: pArchiv: 0x%x  pClient: 0x%x  m_bCancelled: %d\n")
				//	, pArchiv, pClient,m_bCancelled);
				pClient->GetCIPCDatabaseServer()->DoConfirmQuery(m_pQuery->GetUserID());
			}
			// end of thread
			WK_TRACE(_T("QUERY%d SUCCESS\n"),m_pQuery->GetUserID());
			bRet = FALSE;
		}
	}

	if (bRet == FALSE)
	{
		m_pQuery->DoConfirmQuery();
		CWnd* pWnd = AfxGetMainWnd();
		if (WK_IS_WINDOW(pWnd))
		{
			pWnd->PostMessage(WM_USER,WPARAM_QUERY_FINISHED);
		}
	}
	return bRet;
}
//////////////////////////////////////////////////////////////////////
// returns FALSE if client is disconnected
BOOL CQueryThread::Query(CArchiv* pArchiv)
{
	int i;
	BOOL bRet = TRUE;
	CSequence* pSequence;

	if (m_pQuery->IsForward())
	{
		for (i=0;i<pArchiv->GetNrSequences();i++)
		{
			pSequence = pArchiv->GetSequenceIndex(i);
			CClient* pClient;
			pClient = theApp.m_Clients.GetClient(m_pQuery->GetClientID());
			if (pClient && pClient->IsConnected())
			{
				// we still have an sequence for search and the client is
				// still connected
				if (theApp.TraceQuery()) 
				{
					WK_TRACE(_T("QUERY%d A=%d, S=%d\n"),
						m_pQuery->GetUserID(),
						pArchiv->GetNr(),
						pSequence->GetNr());
				}
				
				if (pSequence->Query(*m_pQuery))
				{
					// send the results
					m_pQuery->SendQueryResults(pClient);
				}
				else
				{
					// cancelled or 
					// disconnect or 
					// query error
					return FALSE;
				}

				if (IsCancelled())
				{
					return FALSE;
				}

				// now we have to wait for all responses
				if (!WaitForResponses())
				{
					return FALSE;
				}
			}
			else
			{
				bRet = FALSE;
				break;
			}
		}
	}
	else
	{
		for (i=pArchiv->GetNrSequences()-1;i>=0;i--)
		{
			pSequence = pArchiv->GetSequenceIndex(i);
			CClient* pClient;
			pClient = theApp.m_Clients.GetClient(m_pQuery->GetClientID());
			if (pClient && pClient->IsConnected())
			{
				// we still have an sequence for search and the client is
				// still connected
				if (theApp.TraceQuery()) 
				{
					WK_TRACE(_T("QUERY%d A=%d, S=%d\n"),
						m_pQuery->GetUserID(),
						pArchiv->GetNr(),
						pSequence->GetNr());
				}
				
				if (pSequence->Query(*m_pQuery))
				{
					// send the results
					m_pQuery->SendQueryResults(pClient);
				}
				else
				{
					// cancelled or 
					// disconnect or 
					// query error
					return FALSE;
				}

				if (IsCancelled())
				{
					return FALSE;
				}

				// now we have to wait for all responses
				if (!WaitForResponses())
				{
					return FALSE;
				}
			}
			else
			{
				bRet = FALSE;
				break;
			}
		}
	}

	return bRet;
}
//////////////////////////////////////////////////////////////////////
// returns FALSE if client is disconnected
BOOL CQueryThread::WaitForResponses()
{
	CClient* pClient = NULL;
	// now we have to wait for all responses
	while(!m_pQuery->HasEnoughResponses())
	{
		if (WAIT_OBJECT_0==WaitForSingleObject(m_StopEvent,50))
		{
			WK_TRACE(_T("query thread stopped while waiting for query response giving up\n"));
			return FALSE;
		}
		pClient = theApp.m_Clients.GetClient(m_pQuery->GetClientID());
		if (   pClient 
			&& pClient->IsConnected()
			&& (!IsCancelled())
			)
		{
			// still connected and not cancelled
		}
		else
		{
			// not connected and timeout
			WK_TRACE(_T("client no longer connected or query cancelled while waiting for query response giving up\n"));
			return FALSE;
		}
	}
	return !IsCancelled();
}
