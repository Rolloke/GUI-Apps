// CIPCDatabaseIdipClientLocal.cpp: implementation of the CIPCDatabaseIdipClientLocal class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IdipClient.h"
#include "mainfrm.h"
#include "CIPCDatabaseIdipClientLocal.h"
#include "idipClientDoc.h"
#include "ViewControllItems.h"
#include "ViewArchive.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPCDatabaseIdipClientLocal::CIPCDatabaseIdipClientLocal(CIdipClientDoc* pDoc ,LPCTSTR szShmName)
	: CIPCDatabaseClient(szShmName)
{
	m_pDoc = pDoc;
	StartThread();
}

CIPCDatabaseIdipClientLocal::~CIPCDatabaseIdipClientLocal()
{
	StopThread();
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCDatabaseIdipClientLocal::OnConfirmConnection()
{
	CIPCDatabaseClient::OnConfirmConnection();
	CViewArchive* pOV = theApp.GetMainFrame()->GetViewArchive();
	if (pOV)
	{
		pOV->PostMessage(WM_USER,(WPARAM)MAKELONG(WPARAM_DATABASE_CONNECTED,(WORD)0xFFFF));
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseIdipClientLocal::OnConfirmFieldInfo(int iNumRecords, const CIPCField data[])
{
	CServer *pServer = m_pDoc->GetLocalServer();
	if (pServer)
	{
		pServer->OnConfirmFieldInfo(iNumRecords,data);
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCDatabaseIdipClientLocal::OnRequestDisconnect()
{
	DelayedDelete();
	CViewArchive* pOV = theApp.GetMainFrame()->GetViewArchive();
	if (pOV)
	{
		pOV->PostMessage(WM_USER,(WPARAM)MAKELONG(WPARAM_DATABASE_DISCONNECTED, 0xFFFF));
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseIdipClientLocal::OnConfirmVersionInfo(WORD wGroupID, DWORD dwVersion)
{
//	WK_TRACE(_T("local database server version is %d\n"),dwVersion);
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseIdipClientLocal::OnRequestVersionInfo(WORD wGroupID)
{
	// Nr 1 is default
	// Nr 2 is Version 3.6 Build 133
	// Nr 3 is new Recherche
	DoConfirmVersionInfo(0,3);
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseIdipClientLocal::OnIndicateDeleteSequence(WORD wArchivNr, WORD wSequenceNr)
{
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseIdipClientLocal::OnIndicateNewSequence(const CIPCSequenceRecord& data, WORD  wPrevSequenceNr, DWORD dwNrOfRecords)
{
}
