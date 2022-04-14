// CIPCDataBaseVision.cpp: implementation of the CIPCDataBaseVision class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "vision.h"
#include "CIPCDataBaseVision.h"
#include "VisionDoc.h"
#include "ObjectView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPCDataBaseVision::CIPCDataBaseVision(CVisionDoc* pDoc ,LPCTSTR shmName)
	: CIPCDatabaseClient(shmName)
{
	m_pDoc = pDoc;
	StartThread();
}

CIPCDataBaseVision::~CIPCDataBaseVision()
{
	StopThread();
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCDataBaseVision::OnConfirmConnection()
{
	CIPCDatabaseClient::OnConfirmConnection();
	CObjectView* pOV = m_pDoc->GetObjectView();
	if (WK_IS_WINDOW(pOV))
	{
		pOV->PostMessage(WM_USER,WPARAM_DATABASE_CONNECTED);
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCDataBaseVision::OnRequestDisconnect()
{
	DelayedDelete();
	CObjectView* pOV = m_pDoc->GetObjectView();
	if (WK_IS_WINDOW(pOV))
	{
		pOV->PostMessage(WM_USER,WPARAM_DATABASE_DISCONNECTED);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDataBaseVision::OnConfirmVersionInfo(WORD wGroupID, DWORD dwVersion)
{
//	WK_TRACE(_T("local database server version is %d\n"),dwVersion);
}
//////////////////////////////////////////////////////////////////////
void CIPCDataBaseVision::OnRequestVersionInfo(WORD wGroupID)
{
	// Nr 1 is default
	// Nr 2 is Version 3.6 Build 133
	// Nr 3 is new Recherche
	DoConfirmVersionInfo(0,3);
}

