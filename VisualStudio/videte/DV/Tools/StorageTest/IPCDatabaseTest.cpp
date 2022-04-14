// IPCDatabaseTest.cpp: implementation of the CIPCDatabaseTest class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StorageTest.h"
#include "IPCDatabaseTest.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPCDatabaseTest::CIPCDatabaseTest(const char *shmName)
	: CIPCDatabase(shmName,FALSE)
{
	WK_TRACE("connecting to storage\n");
	StartThread();
}
//////////////////////////////////////////////////////////////////////
CIPCDatabaseTest::~CIPCDatabaseTest()
{
	StopThread();
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseTest::OnReadChannelFound()
{
	DoRequestConnection();
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseTest::OnConfirmConnection()
{
	WK_TRACE("connected to storage\n");
	CIPCDatabase::OnConfirmConnection();
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseTest::OnRequestDisconnect()
{
	DelayedDelete();
	CWnd* pWnd = AfxGetMainWnd();
	if (WK_IS_WINDOW(pWnd))
	{
		pWnd->PostMessage(WM_USER);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseTest::OnConfirmNewSavePicture(WORD wArchivNr,CSecID camID)
{
	TRACE("OnConfirmNewSavePicture %hx, %08lx\n",wArchivNr,camID.GetID());
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseTest::OnIndicateError(DWORD dwCmd, 
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
				TRACE("cannot save during reset %08lx\n",id.GetID());
				break;
			}
			break;
		}
		break;
	}
}
