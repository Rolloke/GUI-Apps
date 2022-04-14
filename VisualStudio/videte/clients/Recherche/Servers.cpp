// Servers.cpp: implementation of the CServers class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Recherche.h"
#include "mainfrm.h"
#include "RechercheDoc.h"
#include "Servers.h"

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

	Lock();
	for (int i=0;i<GetSize();i++)
	{
		pServer = GetAt(i);
		if (pServer)
		{
			if (pServer->IsDisconnected())
			{
				// remove from tree and array
				WK_TRACE(_T("server %s is disconnected\n"),pServer->GetName());
				m_pDoc->Remove(pServer->GetID());
				WK_DELETE(pServer);
				RemoveAt(i);
			}
			else if	(pServer->GetError().GetLength())
			{
				// error
				// something went wrong
				if (pServer->IsLocal())
				{
					m_pDoc->NoLocalConnection();
				}
				COemGuiApi::MessageBox(pServer->GetError(),20);
				WK_TRACE_ERROR(_T("connection %s %s\n"),
						pServer->GetName(),
						pServer->GetError());
				m_pDoc->Remove(pServer->GetID());
				WK_DELETE(pServer);
				RemoveAt(i);
			}
			else
			{
				pServer->OnIdle();
			}
		}
	}
	Unlock();
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

	for (int i=0;i<GetSize();i++)
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

	for (int i=0;i<GetSize();i++)
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

	for (int i=0;i<GetSize();i++)
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
	CServer* pServer;
	CString sCopy = sName;

	for (int i=0;i<GetSize();i++)
	{
		pServer = GetAt(i);
		if (0==sCopy.CompareNoCase(pServer->GetName()))
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

	for (int i=0;i<GetSize();i++)
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
BOOL CServers::IsSearching() const
{
	CServer* pServer;
	for (int i=0;i<GetSize();i++)
	{
		pServer = GetAt(i);
		if (pServer && pServer->IsSearching())
		{
			return TRUE;
		}
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
CServer* CServers::GetRemoteServer()
{
	CServer* pServer;
	for (int i=0;i<GetSize();i++)
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

	for (int i=0;i<GetSize();i++)
	{
		pServer = GetAt(i);
		if (pServer->GetID()==wServerID)
		{
			pServer->Disconnect();
			WK_DELETE(pServer);
			RemoveAt(i);
			break;
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CServers::DisconnectAll()
{
	for (int i=0;i<GetSize();i++)
	{
		GetAt(i)->Disconnect();
	}
}
//////////////////////////////////////////////////////////////////////
void CServers::ResetConnectionAutoLogout()
{
	for (int i=0;i<GetSize();i++)
	{
		GetAt(i)->ResetConnectionAutoLogout();
	}
}
