// DirectCDBackup.cpp: implementation of the CDirectCDBackup class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WatchCD.h"
#include "DirectCDBackup.h"
#include "WatchCDDlg.h"
#include "DirectCDError.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDirectCDBackup::CDirectCDBackup(CWatchCDDlg* pWatchCD, CDirectCDError* pDCDError)
{

	m_pWatchCD			= pWatchCD;
	m_pDCDError			= pDCDError;
	m_DCDBackupStatus = BACKUP_START_BACKUP;
}

CDirectCDBackup::~CDirectCDBackup()
{

}

//////////////////////////////////////////////////////////////////////
BOOL CDirectCDBackup::IsBackupComplete()
{
	BOOL bRet = FALSE;

	switch(m_DCDBackupStatus)
	{

	case BACKUP_START_BACKUP:
		OnBackupStartBackup();
		break;

	case BACKUP_WAIT_FOR_BACKUP_COMPLETE:
		OnBackupWaitForBackupComplete();
		break;

	case BACKUP_COMPLETE:
		bRet = TRUE;
		break;

	case BACKUP_ERROR:
		OnBackupError();
		break;

	default:
		break;

	}

	return bRet;
}

//////////////////////////////////////////////////////////////////////
void CDirectCDBackup::OnBackupStartBackup()
{
	if(StartBackup())
	{
		ChangeDCDBackupStatus(BACKUP_WAIT_FOR_BACKUP_COMPLETE);
	}
}


//////////////////////////////////////////////////////////////////////
void CDirectCDBackup::OnBackupWaitForBackupComplete()
{
	if(WaitForBackupComplete())
	{
		ChangeDCDBackupStatus(BACKUP_COMPLETE);
	}
}

//////////////////////////////////////////////////////////////////////
void CDirectCDBackup::OnBackupError()
{
	m_pDCDError->SetError(TRUE);
}


//////////////////////////////////////////////////////////////////////
BOOL CDirectCDBackup::StartBackup()
{
	//post message to DVClient, that DVClient can start with backup
	BOOL bRet = FALSE;
	if(m_pWatchCD->IsDVClient())
	{
		HWND hDVClient = m_pWatchCD->GetHandleDVClient();
		::PostMessage(hDVClient, WM_CHECK_BACKUP_STATUS, BS_DCD_START_BACKUP, 0);
					
		//times checks, if DVClient still is copying data to CDR
		m_pWatchCD->SetTimer(1, 60*1000, NULL);
		bRet = TRUE;
	}
	else
	{
		ChangeDCDBackupStatus(BACKUP_ERROR);
		m_pDCDError->ChangeErrorStatus(ERROR_DVCLIENT_NOT_RUNNING);
	}
	return bRet;
}


//////////////////////////////////////////////////////////////////////
BOOL CDirectCDBackup::WaitForBackupComplete()
{
	//wait until DVClient`s backup is complete
	BOOL bRet = FALSE;
	if(m_pWatchCD->IsDVClientBackupComplete())
	{
		bRet = TRUE;
	}

	if(m_pWatchCD->IsDVClientCopyingDataError())
	{
		//DVClient kopiert nicht mehr und BackupComplete Message
		//kam noch nicht an
		WK_TRACE_ERROR(_T("CDirectCDBackup::WaitForBackupComplete(): DVClient not longer copies data to CDR\n"));
		ChangeDCDBackupStatus(BACKUP_ERROR);
		m_pDCDError->ChangeErrorStatus(ERROR_DVCLIENT_ABNORMAL_STOP_COPYING_DATA);
	}
	return bRet;
}

////////////////////////////////////////////////////////////////////////////
void CDirectCDBackup::ChangeDCDBackupStatus(DirectCDStatus DCDBackupStatus)
{
	DirectCDStatus LastDCDBackupStatus = m_DCDBackupStatus;
	m_DCDBackupStatus = DCDBackupStatus;

	WK_TRACE(_T("CDirectCDBackup() DCDBackupStatus: from last %s to new %s\n")
				,NameOfEnumDirectCD(LastDCDBackupStatus)
				,NameOfEnumDirectCD(m_DCDBackupStatus));

}

