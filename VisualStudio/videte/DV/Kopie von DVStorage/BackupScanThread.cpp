// BackupScanThread.cpp: implementation of the CBackupScanThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dvstorage.h"
#include "mainfrm.h"
#include "BackupScanThread.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBackupScanThread::CBackupScanThread(CIPCDrive* pDrive, BOOL bIndicateToClients)
: CWK_Thread(_T("BackupScanThread"))
{
	m_pDrive = pDrive;
	m_bIndicateToClients = bIndicateToClients;
}

CBackupScanThread::~CBackupScanThread()
{

}
//////////////////////////////////////////////////////////////////////
BOOL CBackupScanThread::Run(LPVOID lpContext)
{
	theApp.m_Archives.ScanBackupDrive(m_pDrive,m_bIndicateToClients);

	CWnd* pWnd = AfxGetMainWnd();
	if (WK_IS_WINDOW(pWnd))
	{
		pWnd->PostMessage(WM_USER,WPARAM_BACKUP_SCAN_FINISHED);
	}
	return FALSE;
}
