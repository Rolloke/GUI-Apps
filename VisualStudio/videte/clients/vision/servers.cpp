// Servers.cpp: implementation of the CServers class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Vision.h"
#include "mainfrm.h"
#include "VisionDoc.h"
#include "Servers.h"
#include "CIPCServerControlVision.h"

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
	CServer* pServer;

	for (int i=GetSize()-1 ; i>=0 ; i--)
	{
		pServer = GetAt(i);
		if (pServer)
		{
			if (pServer->IsDisconnected())
			{
				// remove from tree and array
				if (theApp.m_pUser)
				{
					WK_TRACE_USER(_T("%s disconnected %s\n"),
						theApp.m_pUser->GetName(),
						pServer->GetName());
				}
				m_pDoc->Remove(pServer->GetID());
				WK_DELETE(pServer);
				RemoveAt(i);
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
				}
				if (theApp.m_pUser)
				{
					WK_TRACE_USER(_T("%s Fehler beim Verbindungsaufbau %s %s\n"),
							theApp.m_pUser->GetName(),
							pServer->GetName(),
							pServer->GetError());
				}
				m_pDoc->Remove(pServer->GetID());
				WK_DELETE(pServer);
				RemoveAt(i);
			}
			else
			{
				pServer->CheckAutoLogout();
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
CServer* CServers::AddServer()
{
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
			pServer = GetAt(i);
			bFound = wID==pServer->GetID();
			if (bFound)
			{
				break;
			}
		}
	}

	pServer = new CServer(m_pDoc,this,wID);
	SafeAdd(pServer);

	Unlock();

	return pServer;
}
//////////////////////////////////////////////////////////////////////
CServer* CServers::GetLocalServer()
{
	CServer* pServer;
	int i;

	for (i=0;i<GetSize();i++)
	{
		pServer = GetAt(i);
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
	CServer* pServer;
	int i;

	for (i=0;i<GetSize();i++)
	{
		pServer = GetAt(i);
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
	CServer* pServer;
	int i;

	for (i=0;i<GetSize();i++)
	{
		pServer = GetAt(i);
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
	CServer* pServer = NULL;
	CString sCopy = sName;
	int i;

	for (i=0;i<GetSize();i++)
	{
		pServer = GetAt(i);
		TRACE(_T("%s = %s?\n"),sCopy,pServer->GetName());
		if (0==sCopy.CompareNoCase(pServer->GetName()))
		{
			return pServer;
			break;
		}
	}
	return NULL;
}
//////////////////////////////////////////////////////////////////////
CServer* CServers::GetAlarmServer(const CString& sName)
{
	CServer* pServer = NULL;
	CString sCopy = sName;
	int i;

	for (i=0;i<GetSize();i++)
	{
		pServer = GetAt(i);
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
	CServer* pServer;
	int iRet = 0;
	int i;

	for (i=0;i<GetSize();i++)
	{
		pServer = GetAt(i);
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
	CServer* pServer;
	int i;

	for (i=0;i<GetSize();i++)
	{
		pServer = GetAt(i);
		if (pServer && !pServer->IsLocal())
		{
			return pServer;
		}
	}
	return NULL;
}
//////////////////////////////////////////////////////////////////////
void CServers::Disconnect(WORD wServerID)
{
	CServer* pServer;
	int i;

	for (i=0;i<GetSize();i++)
	{
		pServer = GetAt(i);
		if (pServer->GetID()==wServerID)
		{
			pServer->Disconnect(FALSE);
			WK_DELETE(pServer);
			RemoveAt(i);
			break;
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CServers::Trace()
{
	CServer* pServer = NULL;
	for (int i=0;i<GetSize();i++)
	{
		pServer = GetAt(i);
		TRACE("%d, %s, %x\n",i,pServer->GetName(),pServer->GetID());
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CServers::DisconnectAll()
{
	CServer* pServer = NULL;
	BOOL bRet = TRUE;

	for (int i=0;i<GetSize();i++)
	{
		pServer = GetAt(i);
		bRet &= pServer->Disconnect(TRUE);
	}
	return bRet;
}
////////////////////////////////////////////////////////////////////////////
void CServers::ResetConnectionAutoLogout()
{
	CServer* pServer;
	for (int i=0;i<GetSize();i++)
	{
		pServer = GetAt(i);
		if (pServer && pServer->IsConnected())
		{
			pServer->ResetConnectionAutoLogout();
		}
	}
}
////////////////////////////////////////////////////////////////////////////
BOOL CServers::IsAnyServerFetching()
{
	CServer* pServer;
	for (int i=0;i<GetSize();i++)
	{
		pServer = GetAt(i);
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
	CServer* pServer;
	for (int i=0;i<GetSize();i++)
	{
		pServer = GetAt(i);
		if (   pServer 
			&& pServer->IsConnected()
			&& pServer->IsB3())
		{
			return TRUE;
		}
	}
	return FALSE;
}
