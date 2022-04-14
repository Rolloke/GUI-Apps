/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: LauncherControl.cpp $
// ARCHIVE:		$Archive: /Project/SecurityLauncher/LauncherControl.cpp $
// CHECKIN:		$Date: 25.02.04 16:15 $
// VERSION:		$Revision: 12 $
// LAST CHANGE:	$Modtime: 25.02.04 15:57 $
// BY AUTHOR:	$Author: Georg.feddern $
// $Nokeywords:$

#include "stdafx.h"
#include "SecurityLauncher.h"

#include "LauncherControl.h"
#include "UpdateRecord.h"

extern CSecurityLauncherApp theApp;

//////////////////////////////////////////////////////////////////////////////////////
CLauncherControl::CLauncherControl()
	: CIPCServerControl(SM_LAUNCHER,TRUE)
{
	m_dwTick = GetTickCount();
	SetConnectRetryTime(250);
	StartThread();
	WK_TRACE(_T("update control created\n"));
}
//////////////////////////////////////////////////////////////////////////////////////
CLauncherControl::~CLauncherControl()
{
	StopThread();
	WK_TRACE(_T("update control destroyed\n"));
}
//////////////////////////////////////////////////////////////////////////////////////
void CLauncherControl::OnRequestFileUpdate(
				int iDestination,
				const CString &sFileName,
				const void *pData,
				DWORD dwDataLen,
				BOOL bNeedsReboot
				)
{
	CUpdateRecord* pUpdateRecord;

	WK_TRACE(_T("OnRequestFileUpdate(%d,%s,%d)\n"),iDestination,(LPCTSTR)sFileName,dwDataLen);

	pUpdateRecord = new CUpdateRecord(iDestination,sFileName,pData,dwDataLen,bNeedsReboot);
	AfxGetApp()->m_pMainWnd->PostMessage(WM_COMMAND,ID_UPDATE,(LPARAM)(LPVOID)pUpdateRecord);
	DoConfirmFileUpdate(sFileName);
}
//////////////////////////////////////////////////////////////////////////////////////
BOOL CLauncherControl::Run(DWORD dwTimeOut)
{
	m_dwTick = GetTickCount();
	return CIPCServerControl::Run(dwTimeOut);
}
//////////////////////////////////////////////////////////////////////////////////////
BOOL CLauncherControl::IsTimedOut()
{
	return GetTimeSpan(m_dwTick)>60*1000;
}
