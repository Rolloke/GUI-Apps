/* GlobalReplace: CIPCServerControlIsdn --> CIPCServerControlISDN */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CipcServerControlIsdn.cpp $
// ARCHIVE:		$Archive: /Project/Capi/ISDNUnit/CipcServerControlIsdn.cpp $
// CHECKIN:		$Date: 18.02.04 12:07 $
// VERSION:		$Revision: 44 $
// LAST CHANGE:	$Modtime: 18.02.04 10:30 $
// BY AUTHOR:	$Author: Georg.feddern $
// $Nokeywords:$
 
#include "stdafx.h"
#include "CIPCServerControlISDN.h"

#include "ISDNUnit.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// counter to associate pipes with requests, in case is RemovePipe
// has to be made, the control might already be open another request
// incremented each time a request is made
DWORD CIPCServerControlISDN::m_dwRequestCounter=0;	
/////////////////////////////////////////////////////////////////////////////
CIPCServerControlISDN::CIPCServerControlISDN(
											LPCTSTR psSMName, 
											BOOL bAsMaster)
					: CIPCServerControl ( psSMName, bAsMaster )
{
	m_dwLastReconnectTry = 0;

	SetTimeoutCmdReceive(60*1000);
	SetConnectRetryTime(50);
}
/////////////////////////////////////////////////////////////////////////////
CIPCServerControlISDN::~CIPCServerControlISDN()
{
	StopThread();
}   
/////////////////////////////////////////////////////////////////////////////
void CIPCServerControlISDN::OnReadChannelFound()
{
	m_dwRequestCounter++;
}
/////////////////////////////////////////////////////////////////////////////
void CIPCServerControlISDN::OnRequestDisconnect()
{
	// always increase, to make sure it's like an unique id!
	m_dwRequestCounter++;
	// check connection for established pipes
	CISDNConnection* pConnection = theApp.GetISDNConnection();
	if (pConnection)
	{
		if (pConnection->AnyPipesExisting() == FALSE)
		{
			WK_TRACE(_T("CloseConnection, there were no pipes established\n"));
			pConnection->CloseConnection();
		}
	}
	else
	{
		WK_TRACE_WARNING(_T("CIPCServerControlISDN::OnRequestDisconnect m_pConnection == NULL\n"));
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CIPCServerControlISDN::RequestConnection(const CConnectionRecord &c,
											  DWORD cipcCmd
											  )
{
	BOOL bReturn = FALSE;
	CString sError;
	const CString &sHost = c.GetDestinationHost();
	if ( sHost.IsEmpty() )
	{
		sError.LoadString(IDS_EMPTY_HOST_NAME);
		DoNewIndicateError(cipcCmd,
						   SECID_NO_ID,
						   CIPC_ERROR_INVALID_HOST,
						   0,
						   sError
						   );
		bReturn = FALSE;
	}
	else
	{
		CISDNConnection* pConnection = theApp.GetISDNConnection();
		if (pConnection)
		{
			pConnection->RequestPipe(this, c, cipcCmd);
			bReturn = TRUE;
		}
		else
		{
			WK_TRACE_WARNING(_T("CIPCServerControlISDN::OnRequestInputConnection m_pConnection == NULL\n"));
			sError.LoadString(IDS_ISDN_NOT_AVAILABLE);
			DoNewIndicateError(cipcCmd,
							   SECID_NO_ID, 
							   CIPC_ERROR_UNABLE_TO_CONNECT,
							   0,
							   sError
							  );
			bReturn = FALSE;
		}
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
void CIPCServerControlISDN::OnRequestInputConnection(const CConnectionRecord &c)
{
	RequestConnection(c,SRV_CONTROL_REQUEST_INPUT_CONNECTION);
}
/////////////////////////////////////////////////////////////////////////////
void CIPCServerControlISDN::OnRequestOutputConnection(const CConnectionRecord &c)
{
	RequestConnection(c,SRV_CONTROL_REQUEST_OUTPUT_CONNECTION);
}
/////////////////////////////////////////////////////////////////////////////
void CIPCServerControlISDN::OnRequestDataBaseConnection(const CConnectionRecord &c)
{
	RequestConnection(c,SRV_CONTROL_REQUEST_DB_CONNECTION);
}
/////////////////////////////////////////////////////////////////////////////
void CIPCServerControlISDN::OnRequestAlarmConnection(const CConnectionRecord &c)
{
	RequestConnection(c,SRV_CONTROL_REQUEST_ALARM_CONNECTION);
}
/////////////////////////////////////////////////////////////////////////////
void CIPCServerControlISDN::OnRequestAudioConnection(const CConnectionRecord &c)
{
	RequestConnection(c,SRV_CONTROL_REQUEST_AUDIO_CONNECTION);
}
/////////////////////////////////////////////////////////////////////////////
void CIPCServerControlISDN::OnRequestServerReset(const CConnectionRecord &c)
{
	// NOT YET check permission/user/password
	WK_TRACE(_T("Reset request received. Shutting down....\n"));
	DoConfirmServerReset();
	CISDNConnection* pConnection = theApp.GetISDNConnection();
	if (pConnection)
	{
		pConnection->CloseConnection();
		Sleep(200);	// give the confirm a chance
	}
	else
	{
		// do not indicate error, because we handle reset anyway alone
//		WK_TRACE_WARNING(_T("CIPCServerControlISDN::OnRequestServerReset m_pConnection == NULL\n"));
	}

	// now end our app save
	// CAVEAT can't use AfxGetMainWnd here, since we are in the CIPC worker thread
	// but m_pMainWnd works fine :-)
	CWnd* pWnd = theApp.m_pMainWnd;
	if (WK_IS_WINDOW(pWnd))
	{
		pWnd->PostMessage(WM_COMMAND,ID_APP_EXIT);
	}
	else
	{
		WK_TRACE(_T(" no main wnd found to post something\n"));
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIPCServerControlISDN::DoNewIndicateError(DWORD dwCmd, CSecID id, 
						 CIPCError errorCode, int iUnitCode,
						 LPCTSTR sErrorMessage)
{
	// gf 2004/02/18 Darf nur aufgerufen werden, wenn auch verbunden, sonst GP
	if (GetIPCState() == CIPC_STATE_CONNECTED)
	{
		DoIndicateError(dwCmd, id, errorCode, iUnitCode, sErrorMessage);
	}

// gf: Wird nur auf Seite der RUFENDEN Station aufgerufen, dies ist kein virtueller Alarm
/*
	// Virtuellen Reject-Alarm auslösen...
	CString sOnlineMsg;
	sOnlineMsg.Format(_T("Reject Call from ISDN"));
	CWK_RunTimeError runtimeError(REL_MESSAGE, RTE_REJECT, sOnlineMsg,0,1);
	runtimeError.Send();
	Sleep(50);
	CWK_RunTimeError runtimeError2(REL_MESSAGE, RTE_REJECT, sOnlineMsg,0,0);
	runtimeError2.Send();
*/
}
