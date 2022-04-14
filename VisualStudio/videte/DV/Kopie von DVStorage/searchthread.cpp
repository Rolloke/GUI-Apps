// SearchThread.cpp: implementation of the CSearchThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dvstorage.h"
#include "SearchThread.h"
#include "MainFrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSearchThread::CSearchThread(CSearch* pSearch) 
: CWK_Thread(_T("SearchThread"),pSearch)
{
	m_pSearch = pSearch;
	m_bCancelled = FALSE;
	m_bIsRunning = TRUE;
}
//////////////////////////////////////////////////////////////////////
CSearchThread::~CSearchThread()
{
	WK_DELETE(m_pSearch);
}
//////////////////////////////////////////////////////////////////////
void CSearchThread::Cancel()
{
	m_bCancelled = TRUE;
	m_pSearch->Cancel();
}
//////////////////////////////////////////////////////////////////////
BOOL CSearchThread::Run(LPVOID lpContext)
{
	BOOL bRet = FALSE;
	CClient* pClient;
	CCollection* pCollection;

	pCollection = m_pSearch->GetAndRemoveCollection();
	pClient = theApp.m_Clients.GetClient(m_pSearch->GetClientID());
	if (pCollection && pClient && !m_bCancelled)
	{
		// we still have an archiv for search and the client is
		// still connected
		bRet = Search(pCollection);
	}
	else
	{
		// end of search
		
		// is everything send ??
		// now we have to wait for all responses
		if (WaitForResponses())
		{
			// end of thread
			WK_TRACE(_T("QUERY%d SUCCESS\n"),m_pSearch->GetUserID());
			bRet = FALSE;
		}
	}

	if (bRet == FALSE)
	{
		m_pSearch->DoConfirmQuery();
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
BOOL CSearchThread::Search(CCollection* pCollection)
{
	int i;
	BOOL bRet = TRUE;
	CTape* pTape;

	if (m_pSearch->IsForward())
	{
		for (i=0;i<pCollection->GetNrTapes();i++)
		{
			pTape = pCollection->GetTapeIndex(i);

			if (!Search(pTape))
			{
				bRet = FALSE;
				break;
			}
		}
	}
	else
	{
		for (i=pCollection->GetNrTapes()-1;i>=0;i--)
		{
			pTape = pCollection->GetTapeIndex(i);

			if (!Search(pTape))
			{
				bRet = FALSE;
				break;
			}
		}
	}


	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CSearchThread::Search(CTape* pTape)
{
	BOOL bRet = TRUE;
	CClient* pClient = theApp.m_Clients.GetClient(m_pSearch->GetClientID());
	
	if (pClient && pClient->IsConnected())
	{
		if (pTape->Search(*m_pSearch))
		{
			// send the results
			m_pSearch->SendSearchResults(pClient);
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
	}

	return bRet;
}
//////////////////////////////////////////////////////////////////////
// returns FALSE if client is disconnected
BOOL CSearchThread::WaitForResponses()
{
	CClient* pClient = NULL;
	// now we have to wait for all responses
	while(!m_pSearch->HasEnoughResponses())
	{
		if (WAIT_OBJECT_0==WaitForSingleObject(m_StopEvent,50))
		{
			WK_TRACE(_T("query thread stopped while waiting for query response giving up\n"));
			return FALSE;
		}
		pClient = theApp.m_Clients.GetClient(m_pSearch->GetClientID());
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
	return TRUE;
}
