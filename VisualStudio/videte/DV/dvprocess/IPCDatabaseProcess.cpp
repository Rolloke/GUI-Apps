// IPCDatabaseProcess.cpp: implementation of the CIPCDatabaseProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DVProcess.h"
#include "IPCDatabaseProcess.h"
#include "Camera.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CIPCDatabaseProcess::CIPCDatabaseProcess(LPCTSTR shmName)
	: CIPCDatabase(shmName,FALSE)
{
	WK_TRACE(_T("connecting to storage\n"));
	StartThread();
}
//////////////////////////////////////////////////////////////////////
CIPCDatabaseProcess::~CIPCDatabaseProcess()
{
	StopThread();
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseProcess::OnReadChannelFound()
{
	DoRequestConnection();
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseProcess::OnConfirmConnection()
{
	WK_TRACE(_T("connected to storage\n"));
	CIPCDatabase::OnConfirmConnection();
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseProcess::OnRequestDisconnect()
{
	DelayedDelete();
	CWnd* pWnd = AfxGetMainWnd();
	if (WK_IS_WINDOW(pWnd))
	{
		pWnd->PostMessage(WM_USER);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseProcess::OnConfirmNewSavePicture(WORD wArchivNr,CSecID camID)
{
//	TRACE(_T("OnConfirmNewSavePicture %hx, %08lx\n"),wArchivNr,camID.GetID());
	theApp.ConfirmRecordedFrame();
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseProcess::OnIndicateError(DWORD dwCmd, 
										  CSecID id, 
										  CIPCError error, 
										  int iErrorCode,
										  const CString &sErrorMessage)
{
	switch (dwCmd)
	{
	case CIPC_DB_NEW_REQUEST_SAVE:
		switch (error)
		{
		case CIPC_ERROR_ACCESS_DENIED:
			switch (iErrorCode)
			{
			case DBEC_SAVE_DURING_RESET:
				// handle as confirm save
				{
					theApp.ConfirmRecordedFrame();
				}
				break;
			}
			break;
		}
		break;
	}
}
