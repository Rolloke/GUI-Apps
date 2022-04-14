// Servers.cpp: implementation of the CServers class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "IdipClient.h"
#include "mainfrm.h"
#include "IdipClientDoc.h"
#include "Servers.h"
#include "CIPCServerControlIdipClient.h"
#include "wkclasses\wk_utilities.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CServers::CServers()
{

}
//////////////////////////////////////////////////////////////////////
CServers::~CServers()
{
	DeleteAll();
}
//////////////////////////////////////////////////////////////////////
void CServers::OnIdle()
{
	if (!theApp.IsInMainThread())
	{
		TRACE(_T("###### %s in Thread %s\n"), _T(__FUNCTION__), XTIB::GetThreadName()); 
	}
	CServer* pServer;
	WORD wDisconnected = SECID_NO_SUBID;

	CAutoCritSec acs(&m_cs);
	for (int i=GetSize()-1 ; i>=0 ; i--)
	{
		pServer = GetAtFast(i);
		if (pServer)
		{
			if (pServer->IsDisconnected())
			{
				// remove from tree and array
				if (theApp.m_pUser)
				{
					WK_TRACE_USER(_T("%s disconnected %s\n"), theApp.m_pUser->GetName(),
						pServer->GetName());
				}
				wDisconnected = pServer->GetID();
				break;
			}
			else if	(   pServer->GetError().GetLength()
				     || pServer->IsConnectionFailed())
			{
				// error
				// something went wrong
				if (   pServer->ShowErrorMessage()
					&& pServer->GetError().GetLength())
				{
					COemGuiApi::MessageBox(pServer->GetError(),20);
					wDisconnected = pServer->GetID();
					if (   pServer->IsConnectionFailed()
						&& pServer->IsLocal())
					{
						AfxGetMainWnd()->PostMessage(WM_COMMAND,ID_NEW_LOGIN);
					}
				}
				else
				{
					TRACE(_T("do not show error message from connection failed\n"));
				}
				if (theApp.m_pUser)
				{
					WK_TRACE_USER(_T("%s Fehler beim Verbindungsaufbau %s %s\n"), theApp.m_pUser->GetName(),
							pServer->GetName(),
							pServer->GetError());
				}
				break;
			}
			else
			{
				pServer->OnIdle();
			}
		}
	}
	acs.Unlock();
	if (wDisconnected != SECID_NO_SUBID)
	{
		m_pDoc->Disconnect(wDisconnected);
	}
}
//////////////////////////////////////////////////////////////////////
CServer* CServers::AddServer()
{
	if (!theApp.IsInMainThread())
	{
		TRACE(_T("###### %s in Thread %s\n"), _T(__FUNCTION__), XTIB::GetThreadName()); 
	}
	// Reset the timer counter for reconnection of the database
	theApp.GetMainFrame()->ResetTimerCounter();

	WORD wID = 0;
	CServer* pServer = NULL;
	BOOL bFound = TRUE;

	Lock();
	while (bFound)
	{
		// neuen ID probieren
		wID++;
		bFound = FALSE;
		
		for (int i=0;i<GetSize();i++)
		{
			pServer = GetAtFast(i);
			bFound = wID==pServer->GetID();
			if (bFound)
			{
				break;
			}
		}
	}

	pServer = new CServer(m_pDoc,this,wID);
	Add(pServer);

	Unlock();

	return pServer;
}
//////////////////////////////////////////////////////////////////////
CServer* CServers::GetLocalServer()
{
	if (!theApp.IsInMainThread())
	{
		TRACE(_T("###### %s in Thread %s\n"), _T(__FUNCTION__), XTIB::GetThreadName()); 
	}
	CServer* pServer;
	int i;

	CAutoCritSec acs(&m_cs);
	for (i=0;i<GetSize();i++)
	{
		pServer = GetAtFast(i);
		if (pServer->IsLocal())
		{
			return pServer;
		}
	}
	return NULL;
}
//////////////////////////////////////////////////////////////////////
CServer* CServers::GetServer(WORD wID)
{
	if (!theApp.IsInMainThread())
	{
		TRACE(_T("###### %s in Thread %s\n"), _T(__FUNCTION__), XTIB::GetThreadName()); 
	}
	CServer* pServer;
	int i;

	CAutoCritSec acs(&m_cs);
	for (i=0;i<GetSize();i++)
	{
		pServer = GetAtFast(i);
		if (wID==pServer->GetID())
		{
			return pServer;
		}
	}
	return NULL;
}
//////////////////////////////////////////////////////////////////////
CServer* CServers::GetServer(CSecID idHost)
{
	if (!theApp.IsInMainThread())
	{
		TRACE(_T("###### %s in Thread %s\n"), _T(__FUNCTION__), XTIB::GetThreadName()); 
	}
	CServer* pServer;
	int i;

	CAutoCritSec acs(&m_cs);
	for (i=0;i<GetSize();i++)
	{
		pServer = GetAtFast(i);
		if (idHost==pServer->GetHostID())
		{
			return pServer;
		}
	}
	return NULL;
}
//////////////////////////////////////////////////////////////////////
CServer* CServers::GetServer(const CString& sName)
{
	if (!theApp.IsInMainThread())
	{
		TRACE(_T("###### %s in Thread %s\n"), _T(__FUNCTION__), XTIB::GetThreadName()); 
	}
	CServer* pServer = NULL;
	CString sCopy = sName;
	int i;

	CAutoCritSec acs(&m_cs);
	for (i=0;i<GetSize();i++)
	{
		pServer = GetAtFast(i);
		TRACE(_T("%s = %s?\n"),sCopy,pServer->GetName());
		if (0==sCopy.CompareNoCase(pServer->GetName()))
		{
			return pServer;
		}
	}
	return NULL;
}
//////////////////////////////////////////////////////////////////////
CServer* CServers::GetAlarmServer(const CString& sName)
{
	if (!theApp.IsInMainThread())
	{
		TRACE(_T("###### %s in Thread %s\n"), _T(__FUNCTION__), XTIB::GetThreadName()); 
	}
	CServer* pServer = NULL;
	CString sCopy = sName;
	int i;
	CAutoCritSec acs(&m_cs);
	for (i=0;i<GetSize();i++)
	{
		pServer = GetAtFast(i);
		if (   0 == sCopy.CompareNoCase(pServer->GetName())
			&& pServer->IsAlarm()
			)
		{
			return pServer;
		}
	}
	return NULL;
}
//////////////////////////////////////////////////////////////////////
int	CServers::GetNrOfRemoteServers()
{
	if (!theApp.IsInMainThread())
	{
		TRACE(_T("###### %s in Thread %s\n"), _T(__FUNCTION__), XTIB::GetThreadName()); 
	}
	CServer* pServer;
	int iRet = 0;
	int i;

	CAutoCritSec acs(&m_cs);
	for (i=0;i<GetSize();i++)
	{
		pServer = GetAtFast(i);
		if (pServer && !pServer->IsLocal())
		{
			iRet++;
		}
	}
	return iRet;
}
//////////////////////////////////////////////////////////////////////
CServer* CServers::GetRemoteServer()
{
	if (!theApp.IsInMainThread())
	{
		TRACE(_T("###### %s in Thread %s\n"), _T(__FUNCTION__), XTIB::GetThreadName()); 
	}
	CServer* pServer;
	int i;

	CAutoCritSec acs(&m_cs);
	for (i=0;i<GetSize();i++)
	{
		pServer = GetAtFast(i);
		if (pServer && !pServer->IsLocal())
		{
			return pServer;
		}
	}
	return NULL;
}
//////////////////////////////////////////////////////////////////////
BOOL CServers::IsSearching() const
{
	if (!theApp.IsInMainThread())
	{
		TRACE(_T("###### %s in Thread %s\n"), _T(__FUNCTION__), XTIB::GetThreadName()); 
	}
	CAutoCritSec acs((CCriticalSection*) &m_cs);
	CServer* pServer;
	for (int i=0;i<GetSize();i++)
	{
		pServer = GetAtFast(i);
		if (pServer && pServer->IsSearching())
		{
			return TRUE;
		}
	}
	return FALSE;
}//////////////////////////////////////////////////////////////////////
void CServers::Disconnect(WORD wServerID)
{
	if (!theApp.IsInMainThread())
	{
		TRACE(_T("###### %s in Thread %s\n"), _T(__FUNCTION__), XTIB::GetThreadName()); 
	}
	CServer* pServer = NULL;
	int i;
	Lock();
	for (i=0;i<GetSize();i++)
	{
		pServer = GetAtFast(i);
		if (pServer->GetID()==wServerID)
		{
			break;
		}
		pServer = NULL;
	}
	Unlock();
	if (pServer)
	{
//		TRACE(_T("\n######################################################################\n\n")); 
		pServer->Disconnect();
//		TRACE(_T("\n######################################################################\n\n")); 

		Lock();
		RemoveAt(i);
		WK_DELETE(pServer);
		Unlock();
	}
}
//////////////////////////////////////////////////////////////////////
void CServers::Trace()
{
	if (!theApp.IsInMainThread())
	{
		TRACE(_T("###### %s in Thread %s\n"), _T(__FUNCTION__), XTIB::GetThreadName()); 
	}
	CServer* pServer = NULL;
	for (int i=0;i<GetSize();i++)
	{
		pServer = GetAtFast(i);
		TRACE(_T("%d, %s, %x\n"),i,pServer->GetName(),pServer->GetID());
	}
}
////////////////////////////////////////////////////////////////////////////
void CServers::ResetConnectionAutoLogout()
{
	if (!theApp.IsInMainThread())
	{
		TRACE(_T("###### %s in Thread %s\n"), _T(__FUNCTION__), XTIB::GetThreadName()); 
	}
	CAutoCritSec acs(&m_cs);
	CServer* pServer;
	for (int i=0;i<GetSize();i++)
	{
		pServer = GetAtFast(i);
		// TODO GF which connection is relevant? All or single one?
		if (pServer->IsConnected())
		{
			pServer->ResetConnectionAutoLogout();
		}
	}
}
////////////////////////////////////////////////////////////////////////////
BOOL CServers::IsAnyServerFetching()
{
	if (!theApp.IsInMainThread())
	{
		TRACE(_T("###### %s in Thread %s\n"), _T(__FUNCTION__), XTIB::GetThreadName()); 
	}
	CAutoCritSec acs(&m_cs);
	CServer* pServer;
	for (int i=0;i<GetSize();i++)
	{
		pServer = GetAtFast(i);
		if (   pServer 
			&& pServer->IsFetching())
		{
			return TRUE;
		}
	}
	return FALSE;
}
////////////////////////////////////////////////////////////////////////////
BOOL CServers::IsAnyB3Connection()
{
	if (!theApp.IsInMainThread())
	{
		TRACE(_T("###### %s in Thread %s\n"), _T(__FUNCTION__), XTIB::GetThreadName()); 
	}
	CAutoCritSec acs(&m_cs);
	CServer* pServer;
	for (int i=0;i<GetSize();i++)
	{
		pServer = GetAtFast(i);
		if (   pServer->IsConnectedLive()
			&& pServer->IsB3())
		{
			return TRUE;
		}
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CServers::PopupDisconnectMenu(CPoint pt)
{
	if (!theApp.IsInMainThread())
	{
		TRACE(_T("###### %s in Thread %s\n"), _T(__FUNCTION__), XTIB::GetThreadName()); 
	}
	int i, nSize = GetSize();
	if (nSize)
	{
		DWORD dwID;
		CSkinMenu menu;
		CServer*pServer;
		CString sItemText,sText;

		menu.CreatePopupMenu();
		CAutoCritSec acs(&m_cs);
		for (i=0;i<nSize;i++)
		{
			pServer = GetAtFast(i);
			// TODO! RKE: Check if it is the host id or the server id
			dwID = pServer->GetID();
			sItemText = pServer->GetFullName();
			sText.Format(_T("&%d %s"),i++,sItemText);
			menu.AppendMenu(MF_STRING, dwID+ID_DISCONNECT_LOW, sText);
		}
		acs.Unlock();
		menu.ConvertMenu(TRUE);
        menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, AfxGetMainWnd());
	}
}
/////////////////////////////////////////////////////////////////////////////
void CServers::Reset()
{
	CAutoCritSec acs(&m_cs);
	for (int i=0;i<GetSize();i++)
	{
		GetAtFast(i)->Reset();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CServers::CheckDatabaseConnections()
{
	CAutoCritSec acs(&m_cs);
	for (int i=0; i<GetSize(); i++)
	{
		CServer *pServer = GetAtFast(i);
		if (!pServer->IsAlarm())
		{
			pServer->CheckDatabaseConnection();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CServers::UpdateTimeOffsets()
{
	CAutoCritSec acs(&m_cs);
	for (int i=0;i<GetSize();i++)
	{
		GetAtFast(i)->UpdateTimeOffset();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CServers::ResetPictureTimeOffsets()
{
	CAutoCritSec acs(&m_cs);
	for (int i=0;i<GetSize();i++)
	{
		GetAtFast(i)->m_nMaxPictureTimeOffset = 0;
	}
}
