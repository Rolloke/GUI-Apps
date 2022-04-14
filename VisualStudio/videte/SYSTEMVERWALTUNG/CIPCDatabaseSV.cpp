// CIPCDatabaseSV.cpp: implementation of the CIPCDatabaseSV class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "systemverwaltung.h"
#include "CIPCDatabaseSV.h"
#include "RemoteThread.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPCDatabaseSV::CIPCDatabaseSV(CRemoteThread* pThread
	, LPCTSTR shmName
#ifdef _UNICODE
	,WORD wCodePage
#endif
)
	: CIPCDatabase(shmName,FALSE)
{
#ifdef _UNICODE
	m_wCodePage = wCodePage;
#endif
	m_pRemoteThread = pThread;
	StartThread();
}

CIPCDatabaseSV::~CIPCDatabaseSV()
{
	StopThread();
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseSV::OnReadChannelFound()
{
	CIPCDatabase::OnReadChannelFound();
	TRACE(_T("CIPCDatabaseSV::OnReadChannelFound()\n"));
	DoRequestConnection();
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseSV::OnConfirmConnection()
{
	CIPCDatabase::OnConfirmConnection();
	TRACE(_T("CIPCDatabaseSV::OnConfirmConnection() requesting drives\n"));
	DoRequestDrives();
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseSV::OnRequestDisconnect()
{
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseSV::OnConfirmDrives(int iNumDrives,
							 const CIPCDrive drives[])
{
	TRACE(_T("CIPCDatabaseSV::OnConfirmDrives()\n"));
	m_pRemoteThread->m_Drives.FromArray(iNumDrives,drives);
	m_eventGotDrives.SetEvent();
}
