/* GlobalReplace: pApp\-\> --> theApp. */
/* GlobalReplace: m_pO-> --> m_pOutputGroups-> */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CipcServerControlServerSide.cpp $
// ARCHIVE:		$Archive: /Project/SecurityServer/CipcServerControlServerSide.cpp $
// CHECKIN:		$Date: 18.07.06 14:37 $
// VERSION:		$Revision: 114 $
// LAST CHANGE:	$Modtime: 18.07.06 14:36 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$
 
#include "stdafx.h"

#include "CipcServerControlServerSide.h"
#include "CipcInputServerClient.h"
#include "CipcOutputServerClient.h"
#include "IpcAudioServerClient.h"
#include "CipcOutputServer.h"	// to stop H263 encoding etc.
#include "sec3.h"

#include "OutputList.h"
#include <WKClasses/WK_String.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

///////////////////////////////////////////////////////////////////////////////////////////
DWORD CClientLink::m_idCounter=0;
///////////////////////////////////////////////////////////////////////////////////////////
WORD CIPCServerControlServerSide::m_wNumInputConnections=0;
WORD CIPCServerControlServerSide::m_wNumOutputConnections=0;
WORD CIPCServerControlServerSide::m_wNumAudioConnections=0;
///////////////////////////////////////////////////////////////////////////////////////////
CClientLink::CClientLink(CIPCOutputServerClient *pClient, DWORD dwUserData)
{
	m_idCounter++;
	if (m_idCounter>0xffff) {
		m_idCounter=1;	// limit to word range
	}
	m_dwID = m_idCounter;
	m_dwUserData=0;
	m_pOutputClient = NULL;	// INIT
	m_pInputClient = NULL;	// INIT

	m_dwUserData = dwUserData;
	m_pOutputClient = pClient;
}
///////////////////////////////////////////////////////////////////////////////////////////
CClientLink::CClientLink(CIPCInputServerClient *pClient)
{
	m_idCounter++;
	if (m_idCounter>0xffff) {
		m_idCounter=1;	// limit to word range
	}
	m_dwID = m_idCounter;
	m_dwUserData=0;
	m_pOutputClient = NULL;	// INIT
	m_pInputClient = NULL;	// INIT

	m_pInputClient = pClient;
}
///////////////////////////////////////////////////////////////////////////////////////////
CClientLinkArray::CClientLinkArray()
{
#ifdef TRACE_LOCKS
	m_nCurrentThread = theApp.m_bTraceLocks ? 0 : -1;
#endif
}
///////////////////////////////////////////////////////////////////////////////////////////
void CClientLinkArray::DropAndSwap(int ix)
{
	CClientLink *pDrop = GetAt(ix);
	// TRACE(_T("DropAndSwap %x [%d]\n"),pDrop->m_dwID,GetSize());
	SetAt( ix, GetAt(GetSize()-1) );	// fill with last one
	RemoveAt(GetSize()-1);			// shrink
	delete pDrop;
}
///////////////////////////////////////////////////////////////////////////////////////////
//*****************************************************************************************
///////////////////////////////////////////////////////////////////////////////////////////
CIPCServerControlServerSide::CIPCServerControlServerSide()
	: CIPCServerControl ( WK_SMServerControl,TRUE)
{
	m_iCounter = 0;
	
	m_idActiveClient     = SECID_NO_ID;
	m_idOverlayClient    = SECID_NO_ID;
	m_idDecompressClient = SECID_NO_ID;

	m_wLastOverlayDecoderID = SECID_NO_GROUPID;
#ifdef _UNICODE
	m_wCodePage = (WORD)CWK_String::GetCodePage();
#endif
	SetConnectRetryTime(50);
	m_dwRuntick = WK_GetTickCount();
	StartThread();
}
///////////////////////////////////////////////////////////////////////////////////////////
CIPCServerControlServerSide::~CIPCServerControlServerSide()
{
	m_inputClientsDropped.SafeDeleteAll();
	m_outputClientsDropped.SafeDeleteAll();
	m_audioClientsDropped.SafeDeleteAll();

	m_usageH263Decoding.SafeDeleteAll();
	m_usageJpegDecoding.SafeDeleteAll();
}
/////////////////////////////////////////////////////////////////////////
void CIPCServerControlServerSide::DisconnectAll()
{
	m_inputClients.Lock(_T(__FUNCTION__));
	while (m_inputClients.GetSize()) 
	{
		m_inputClients[0]->StopThread();
		RemoveMe(m_inputClients[0]);
	}
	WK_STAT_LOG(_T("Client"),m_inputClients.GetSize(),_T("InputClients"));
	m_inputClients.Unlock();

	m_outputClients.Lock(_T(__FUNCTION__));
	while (m_outputClients.GetSize()) 
	{
		m_outputClients[0]->StopThread();
		RemoveMe(m_outputClients[0]);
	}
	WK_STAT_LOG(_T("Client"),m_outputClients.GetSize(),_T("OutputClients"));
	m_outputClients.Unlock();

	m_audioClients.Lock(_T(__FUNCTION__));
	while (m_audioClients.GetSize()) 
	{
		m_audioClients[0]->StopThread();
		RemoveMe(m_audioClients[0]);
	}
	WK_STAT_LOG(_T("Client"),m_audioClients.GetSize(),_T("AudioClients"));
	m_audioClients.Unlock();
}
/////////////////////////////////////////////////////////////////////////
void CIPCServerControlServerSide::DisconnectTimedoutClients(CSecID idTimer)
{
	m_inputClients.Lock(_T(__FUNCTION__));
	for (int i=m_inputClients.GetSize()-1;i>=0;i--) 
	{
		CIPCInputServerClient* pClient = m_inputClients[i];
		if (pClient->GetIPCState() == CIPC_STATE_CONNECTED)
		{
			CSecID idPermission = pClient->GetPermissionID();
			CPermission* pPermission = theApp.GetPermissions().GetPermission(idPermission);
			if (   pPermission
				&& pPermission->GetTimer() == idTimer)
			{
				RemoveMe(pClient);
			}
		}
	}
	m_inputClients.Unlock();

	m_outputClients.Lock(_T(__FUNCTION__));
	for (int i=m_outputClients.GetSize()-1;i>=0;i--) 
	{
		CIPCOutputServerClient* pClient = m_outputClients[i];
		if (pClient->GetIPCState() == CIPC_STATE_CONNECTED)
		{
			CSecID idPermission = pClient->GetPermissionID();
			CPermission* pPermission = theApp.GetPermissions().GetPermission(idPermission);
			if (   pPermission
				&& pPermission->GetTimer() == idTimer)
			{
				RemoveMe(pClient);
			}
		}
	}
	m_outputClients.Unlock();

	m_audioClients.Lock(_T(__FUNCTION__));
	for (int i=m_audioClients.GetSize()-1;i>=0;i--) 
	{
		CIPCAudioServerClient* pClient = m_audioClients[i];
		if (pClient->GetIPCState() == CIPC_STATE_CONNECTED)
		{
			CSecID idPermission = pClient->GetPermissionID();
			CPermission* pPermission = theApp.GetPermissions().GetPermission(idPermission);
			if (   pPermission
				&& pPermission->GetTimer() == idTimer)
			{
				RemoveMe(pClient);
			}
		}
	}
	m_audioClients.Unlock();
}
/////////////////////////////////////////////////////////////////////////
void CIPCServerControlServerSide::Shutdown()
{
	StopThread();
	DisconnectAll();
}
/////////////////////////////////////////////////////////////////////////
BOOL CIPCServerControlServerSide::IsTimeout()
{
	BOOL bTimeout = FALSE;
	m_csRuntick.Lock();
	bTimeout = WK_GetTimeSpan(m_dwRuntick) > (30 * 1000);
	m_csRuntick.Unlock();
	return bTimeout = FALSE;
}
///////////////////////////////////////////////////////////////////////////////////////////
CIPCInputServerClient *CIPCServerControlServerSide::AddInputClient(
											const CConnectionRecord &cr
											)
{
	m_inputClients.Lock(_T(__FUNCTION__));
	m_wNumInputConnections++;
	CString sShmName;
	sShmName.Format(_T("SrvInput%d"),m_wNumInputConnections);

	// create a client connection
	CIPCInputServerClient *pNewClient;
	pNewClient = new CIPCInputServerClient(cr, this, sShmName, m_wNumInputConnections);
#ifdef _UNICODE
	CString sCP;
	if (cr.GetFieldValue(CRF_CODEPAGE, sCP))
	{
		pNewClient->SetCodePage(sCP);
	}
	else
	{
		pNewClient->SetCodePage((WORD)CWK_String::GetCodePage());
	}
#endif
	m_inputClients.Add(pNewClient);

	// OOPS always add, NOT YET some flag
	CClientLink *pUsage = new CClientLink(pNewClient);
	m_usageCommData.Add(pUsage);

	WK_STAT_LOG(_T("Client"), m_inputClients.GetSize(), _T("InputClients"));
	m_inputClients.Unlock();

	pNewClient->StartThread();
	WK_TRACE(_T("new input client %s\n"), cr.GetSourceHost());
	theApp.UpdateStateInfo();

	return pNewClient;
}
///////////////////////////////////////////////////////////////////////////////////////////
CIPCOutputServerClient *CIPCServerControlServerSide::AddOutputClient(const CConnectionRecord &cr, BOOL bAlarm, const CString& sRemoteHost)
{
	m_outputClients.Lock(_T(__FUNCTION__));
	m_wNumOutputConnections++;

	CString sShmName;
	sShmName.Format(_T("SrvOutput%d"),m_wNumOutputConnections);

	// create a client connection
	CIPCOutputServerClient *pNewClient;
	pNewClient = new CIPCOutputServerClient(cr, 
											this, 
											sShmName,
											m_wNumOutputConnections,
											bAlarm,
											sRemoteHost);
#ifdef _UNICODE
	CString sCP;
	if (cr.GetFieldValue(CRF_CODEPAGE, sCP))
	{
		pNewClient->SetCodePage(sCP);
	}
	else
	{
		pNewClient->SetCodePage((WORD)CWK_String::GetCodePage());
	}
#endif
	m_outputClients.Add(pNewClient);
	WK_STAT_LOG(_T("Client"), m_outputClients.GetSize(), _T("OutputClients"));
	m_outputClients.Unlock();

	pNewClient->StartThread();

	theApp.UpdateStateInfo();

	return pNewClient;
}
///////////////////////////////////////////////////////////////////////////////////////////
CIPCAudioServerClient* CIPCServerControlServerSide::AddAudioClient(const CConnectionRecord &cr)
{
	m_audioClients.Lock(_T(__FUNCTION__));
	m_wNumAudioConnections++;
	CString sShmName;
	sShmName.Format(_T("SrvAudio%d"), m_wNumAudioConnections);

	// create a client connection
	CIPCAudioServerClient *pNewClient = new CIPCAudioServerClient(cr, this, sShmName, m_wNumAudioConnections);
#ifdef _UNICODE
	CString sCP;
	if (cr.GetFieldValue(CRF_CODEPAGE, sCP))
	{
		pNewClient->SetCodePage(sCP);
	}
	else
	{
		pNewClient->SetCodePage((WORD)CWK_String::GetCodePage());
	}
#endif
	m_audioClients.Add(pNewClient);

	WK_STAT_LOG(_T("Client"),m_audioClients.GetSize(),_T("AudioClients"));
	m_audioClients.Unlock();

	pNewClient->StartThread();
	WK_TRACE(_T("new audio client %s\n"), cr.GetSourceHost());
	theApp.UpdateStateInfo();

	return pNewClient;
}
///////////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCServerControlServerSide::Run(DWORD dwTimeout)
{
	// no need to lock against OnRequestXXX,
	// since this is within the CIPC::Run

	m_csRuntick.Lock();
	m_dwRuntick = WK_GetTickCount();
	m_csRuntick.Unlock();

	m_inputClientsDropped.SafeDeleteAll();
	m_outputClientsDropped.SafeDeleteAll();
	m_audioClientsDropped.SafeDeleteAll();


	m_iCounter++;
	if ((m_iCounter & 15) == 0)
	{
		// check for time out alarm
		CIPCInputServerClient* pInputClient = NULL;
		m_inputClients.Lock(_T(__FUNCTION__));
		for (int i=0;i<m_inputClients.GetSize();i++)
		{
			pInputClient = m_inputClients.GetAtFast(i);
			if (   pInputClient
				&& pInputClient->IsConnectionTimedOut())
			{
				// only remove one client at once
				break;
			}
			pInputClient = NULL;
		}
		m_inputClients.Unlock();
		if (pInputClient)
		{
			WK_TRACE(_T("removing timed out alarm input client %s\n"),pInputClient->GetConnection().GetSourceHost());
			pInputClient->OnRequestDisconnect();
		}

		CIPCOutputServerClient* pOutputClient = NULL;
		m_outputClients.Lock(_T(__FUNCTION__));
		for (i=0;i<m_outputClients.GetSize();i++)
		{
			pOutputClient = m_outputClients.GetAtFast(i);
			if (   pOutputClient
				&& pOutputClient->IsAlarm()
				&& pOutputClient->IsConnectionTimedOut())
			{
				// only remove one client at once
				break;
			}
			pOutputClient = NULL;
		}
		m_outputClients.Unlock();
		if (pOutputClient)
		{
			WK_TRACE(_T("removing timed out alarm output client %s\n"),pOutputClient->GetConnection().GetSourceHost());
			pOutputClient->OnRequestDisconnect();
		}
	}
	if ((m_iCounter & 31) == 0)
	{
		// check for time out input clients
		m_inputClients.Lock(_T(__FUNCTION__));
		for (int i=0;i<m_inputClients.GetSize();i++)
		{
			CIPCInputServerClient* pInputClient = m_inputClients.GetAtFast(i);
			if (   pInputClient
				&& pInputClient->IsTimedOut())
			{
				WK_TRACE(_T("removing timed out input client %s\n"),pInputClient->GetConnection().GetSourceHost());
				pInputClient->OnRequestDisconnect();
				// only remove one client at once
				break;
			}
		}
		m_inputClients.Unlock();
		// check for time out output clients
		m_outputClients.Lock(_T(__FUNCTION__));
		for (int i=0;i<m_outputClients.GetSize();i++)
		{
			CIPCOutputServerClient* pOutputClient = m_outputClients.GetAtFast(i);
			if (   pOutputClient
				&& pOutputClient->IsTimedOut())
			{
				WK_TRACE(_T("removing timed out output client %s\n"),pOutputClient->GetConnection().GetSourceHost());
				pOutputClient->OnRequestDisconnect();
				// only remove one client at once
				break;
			}
		}
		m_outputClients.Unlock();
	}

	// ab und zu mal ein OnIdle provozieren
	if (m_iCounter==400) 
	{
		m_iCounter = 0;
		CWnd* pWnd = AfxGetMainWnd();
		if (WK_IS_WINDOW(pWnd))
		{
			pWnd->PostMessage(WM_USER);
		}
	}

	return CIPC::Run(dwTimeout);
}
///////////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCServerControlServerSide::CheckPermission(CConnectionRecord &connectIn,CIPCFetchResult &fetchResult)
{
	BOOL bAccessGranted=FALSE;

	CIPCError denyError= CIPC_ERROR_INVALID_PERMISSIONS;
	int iDenyErrorCode = 0;

	CWK_Profile wkp;
	CHostArray hosts;
	hosts.Load(wkp);
	CString sOwnName = hosts.GetLocalHostName();

	TRACE(_T("DEBUG Incoming permission %s[%s]\n"),
		connectIn.GetPermission(),
		connectIn.GetPassword() 
		);

	// empty permission
	// find user
	CSecID idPermission;
	if (connectIn.GetPermission().IsEmpty())
	{
		CUserArray users;
		users.Load(wkp);
		CUser* pUser = NULL;
		for (int i=0;i<users.GetSize();i++)
		{
			pUser = users.GetAtFast(i);
			if (   pUser
				&& pUser->GetName() == connectIn.GetUser()
				&& pUser->GetPassword() == connectIn.GetPassword())
			{
				idPermission = pUser->GetPermissionID();
				break;
			}
		}
	}

	CPermissionArray permissions;
	permissions.Load(wkp);
	const CPermission* pPermission = NULL;
	if (idPermission.IsPermissionID())
	{
		pPermission = permissions.GetPermission(idPermission);
		connectIn.SetPassword(pPermission->GetPassword());
		connectIn.SetPermission(pPermission->GetName());
	}
	else
	{
		pPermission = permissions.CheckPermission(connectIn);
	}
	if (pPermission)
	{
		if (hosts.CheckHost(connectIn.GetSourceHost(), pPermission))
		{
			CSecID idTimer = pPermission->GetTimer();
			const CSecTimer* pTimer = theApp.GetTimers().GetTimerByID(idTimer);

			if (   !idTimer.IsTimerID() 
				|| (pTimer != NULL && pTimer->IsIncluded(CTime::GetCurrentTime()))
				)
			{
				// fine, access granted 
				denyError = CIPC_ERROR_OKAY;
				iDenyErrorCode = 0;
				bAccessGranted = TRUE;
			}
			else
			{
				// timer nicht vorhanden
				// kein Access
				denyError = CIPC_ERROR_INVALID_PERMISSIONS;
				iDenyErrorCode = 3;
				bAccessGranted = FALSE;
			}
		}
		else
		{
			// host unknown, access denied
			WK_TRACE(_T("Invalid access host '%s', permission '%s'\n"),
				connectIn.GetSourceHost(), 
				connectIn.GetPermission()
				);
			denyError = CIPC_ERROR_INVALID_PERMISSIONS;
			iDenyErrorCode = 2;
		}
	}
	else
	{
		// an invalid permission
		WK_TRACE(_T("Invalid access unknown permission or wrong password for '%s'\n"),connectIn.GetPermission());
		denyError = CIPC_ERROR_INVALID_PERMISSIONS;
		iDenyErrorCode = 1;
	}

	if (bAccessGranted==FALSE)
	{
		WK_TRACE(_T("Access denied\n"));
		// acess denied fill fetchResult
		CIPCFetchResult tmpResult(sOwnName,	// hidden in shmName
			denyError,
			iDenyErrorCode,
			_T(""),
			0,
			SECID_NO_ID
#ifdef _UNICODE
			, CODEPAGE_UNICODE
#endif
			);
		fetchResult=tmpResult;
	}
	hosts.DeleteAll();

	return bAccessGranted;
}
///////////////////////////////////////////////////////////////////////////////////////////
void CIPCServerControlServerSide::OnRequestInputConnection(const CConnectionRecord &c)
{
	CConnectionRecord cr(c);
	DWORD dwOptions = c.GetOptions();
	if (theApp.AllowClientAccess(this,SRV_CONTROL_REQUEST_INPUT_CONNECTION)
		&& ((dwOptions&SCO_IS_DV) == 0)) 
	{	
		if (m_inputClients.GetNrOfLicensed()>=theApp.m_iMaxNrOfClients)
		{
			WK_TRACE_ERROR(_T("max number of clients reached\n"));
			DoIndicateError(0, SECID_NO_ID, CIPC_ERROR_UNABLE_TO_CONNECT, 4);
		}
		else
		{
			CString sSerial;
			c.GetFieldValue(CRF_SERIAL,sSerial);

			if (   sSerial.IsEmpty()
				|| (   CRSA::IsValidSerial(sSerial)
				    && !m_outputClients.IsConnected(sSerial,c.GetSourceHost()))
				)
			{
				CIPCFetchResult fr;
				BOOL bAccessGranted = CheckPermission(cr,fr);
				if (bAccessGranted)
				{
					// Incoming Call
					CIPCInputServerClient *pNewClient = AddInputClient(c);
#ifdef _UNICODE
					DoConfirmInputConnection(pNewClient->GetID().GetID(), pNewClient->GetShmName(),
						_T(""), 0, MAKELONG(pNewClient->GetCodePage(), 0));
#else
					DoConfirmInputConnection(pNewClient->GetID().GetID(), pNewClient->GetShmName(), _T(""), 0);
#endif
				}
				else
				{
					WK_TRACE_ERROR(_T("no input connection invalid timer\n"));
					DoIndicateError(0,SECID_NO_ID,fr.GetError(),fr.GetErrorCode());
				}
			}
			else
			{
				WK_TRACE_ERROR(_T("invalid Serial Nr %s\n"),sSerial);
				DoIndicateError(0, SECID_NO_ID, CIPC_ERROR_INVALID_SERIAL, 1);
			}
		}
	}
	else
	{
		DoIndicateError(SRV_CONTROL_REQUEST_INPUT_CONNECTION,SECID_NO_ID,CIPC_ERROR_UNABLE_TO_CONNECT,0);
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////
void CIPCServerControlServerSide::OnRequestOutputConnection(const CConnectionRecord &c)
{
	CConnectionRecord cr(c);
	DWORD dwOptions = c.GetOptions();
	if (theApp.AllowClientAccess(this,SRV_CONTROL_REQUEST_OUTPUT_CONNECTION)
		&& ((dwOptions&SCO_IS_DV) == 0)) 
	{	
		if (m_outputClients.GetNrOfLicensed()>=theApp.m_iMaxNrOfClients)
		{
			WK_TRACE_ERROR(_T("max number of clients reached\n"));
			DoIndicateError(0, SECID_NO_ID, CIPC_ERROR_UNABLE_TO_CONNECT, 4);
		}
		else
		{
			CString sSerial;
			c.GetFieldValue(CRF_SERIAL,sSerial);
			
			if (   sSerial.IsEmpty()
				|| (   CRSA::IsValidSerial(sSerial)
				    && !m_outputClients.IsConnected(sSerial,c.GetSourceHost()))
					)
			{
				CIPCFetchResult fr;
				BOOL bAccessGranted = CheckPermission(cr,fr);
				if (bAccessGranted)
				{
					// Incoming Call
					CIPCOutputServerClient *pNewClient = AddOutputClient(c,FALSE,_T(""));
					TRACE(_T("confirming output connection %08lx\n"),pNewClient->GetOptions());
#ifdef _UNICODE
					DoConfirmOutputConnection(pNewClient->GetID().GetID(), pNewClient->GetShmName(),
						_T(""), pNewClient->GetOptions(), MAKELONG(pNewClient->GetCodePage(), 0));
#else
					DoConfirmOutputConnection(pNewClient->GetID().GetID(), pNewClient->GetShmName(),
						_T(""), pNewClient->GetOptions());
#endif
				}
				else
				{
					WK_TRACE_ERROR(_T("no output connection invalid timer\n"));
					DoIndicateError(0,SECID_NO_ID,fr.GetError(),fr.GetErrorCode());
				}
			}
			else
			{
				WK_TRACE_ERROR(_T("invalid Serial Nr %s\n"),sSerial);
				DoIndicateError(0, SECID_NO_ID, CIPC_ERROR_INVALID_SERIAL, 1);
			}
		}
	}
	else
	{
		DoIndicateError(SRV_CONTROL_REQUEST_OUTPUT_CONNECTION,SECID_NO_ID,CIPC_ERROR_UNABLE_TO_CONNECT,0);
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////
void CIPCServerControlServerSide::OnRequestAudioConnection(const CConnectionRecord &c)
{
	CConnectionRecord cr(c);
	DWORD dwOptions = c.GetOptions();
	if (theApp.AllowClientAccess(this,SRV_CONTROL_REQUEST_AUDIO_CONNECTION)
		&& ((dwOptions&SCO_IS_DV) == 0)) 
	{	
		CString sExe,sVersion;
		GetModuleFileName(AfxGetApp()->m_hInstance,sExe.GetBufferSetLength(_MAX_PATH),_MAX_PATH);
		sExe.ReleaseBuffer();
		sVersion = WK_GetFileVersion(sExe);
		CIPCFetchResult fr;
		BOOL bAccessGranted = CheckPermission(cr,fr);
		if (bAccessGranted)
		{
			// Incoming Call
			CIPCAudioServerClient *pNewClient = AddAudioClient(c);
	#ifdef _UNICODE
			DoConfirmAudioConnection(pNewClient->GetID().GetID(), pNewClient->GetShmName(),
										sVersion, pNewClient->GetOptions(), MAKELONG(pNewClient->GetCodePage(), 0));
	#else
			DoConfirmAudioConnection(pNewClient->GetID().GetID(), pNewClient->GetShmName(),
										sVersion, pNewClient->GetOptions());
	#endif
		}
		else
		{
			WK_TRACE_ERROR(_T("no audio connection invalid timer\n"));
			DoIndicateError(0,SECID_NO_ID,fr.GetError(),fr.GetErrorCode());
		}
	}
	else
	{
		DoIndicateError(SRV_CONTROL_REQUEST_AUDIO_CONNECTION,SECID_NO_ID,CIPC_ERROR_UNABLE_TO_CONNECT,0);
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////
void CIPCServerControlServerSide::RemoveMe(CIPCInputServerClient *pClient)
{
	if (m_inputClients.Index(pClient)== -1) {
		return;	 // <<< EXIT >>>
	}


	pClient->DelayedDelete();
	m_inputClients.SafeRemove(pClient);
	WK_STAT_LOG(_T("Client"),m_inputClients.GetSize(),_T("InputClients"));

	for (int i=0;i<m_usageCommData.GetSize();i++) {
		if (m_usageCommData[i]->m_pInputClient==pClient) {
			delete m_usageCommData[i];
			m_usageCommData.RemoveAt(i);
			i--;
		}
	}

	m_inputClientsDropped.Add(pClient);
	theApp.UpdateStateInfo();
}
/////////////////////////////////////////////////////////////////////////////////////////////////
void CIPCServerControlServerSide::RemoveMe(CIPCAudioServerClient *pClient)
{
	if (m_audioClients.Index(pClient)== -1) {
		return;	 // <<< EXIT >>>
	}

	pClient->DelayedDelete();
	m_audioClients.SafeRemove(pClient);
	WK_STAT_LOG(_T("Client"),m_audioClients.GetSize(),_T("AudioClients"));

	m_audioClientsDropped.Add(pClient);
	theApp.UpdateStateInfo();
}
/////////////////////////////////////////////////////////////////////////////////////////////////
void CIPCServerControlServerSide::RemoveMe(CIPCOutputServerClient *pClient)
{
	if (m_outputClients.Index(pClient)== -1) 
	{
		return;	 // <<< EXIT >>>
	}

	if (pClient->GetIPCState() == CIPC_STATE_CONNECTED)
	{
		if (pClient->GetEncoderThread())
		{
			pClient->GetEncoderThread()->RemoveClient(pClient->GetID().GetID());
		}
		pClient->DelayedDelete();
	}
	m_outputClients.SafeRemove(pClient);

	WK_STAT_LOG(_T("Client"),m_outputClients.GetSize(),_T("OutputClients"));

	if (m_outputClients.GetSize() == 0)
	{
		theApp.GetOutputGroups().OnLastClientDisconnected();
	}
	
	m_csDisplayClient.Lock();
	if (   m_idOverlayClient != SECID_NO_ID
		&& m_idOverlayClient == pClient->GetID()) 
	{	// drop overlay client
		m_idOverlayClient = SECID_NO_ID;
		// OOPS can't inform 'old' client, there is no stack
		// NEW 230897 disable overlay window
		CSecID id(m_wLastOverlayDecoderID,0);
		CIPCOutputServer *pDecoder = theApp.GetOutputGroups().GetGroupByID(id);
		if (pDecoder) 
		{
			CRect rect(0,0,1,1);
			pDecoder->DoRequestSetDisplayWindow(m_wLastOverlayDecoderID, rect);
		} 
		else 
		{
			// silent ignore
			WK_TRACE(_T("Decoder not found to stop overlay\n"));	// OBSOLETE
		}
	} 
	else 
	{
		// _T("Nonactive overlay client dropped\n")
	}
	
	if (   m_idDecompressClient != SECID_NO_ID
		&& m_idDecompressClient == pClient->GetID()) 
	{	// drop decompress client
		m_idDecompressClient = SECID_NO_ID;
	}
	
	if (   m_idActiveClient  != SECID_NO_ID
		&& m_idActiveClient == pClient->GetID()) 
	{	// drop active client
		m_idActiveClient = SECID_NO_ID;
	}
	m_csDisplayClient.Unlock();

	m_usageH263Decoding.Lock(_T(__FUNCTION__));
	for (int i=0;i<m_usageH263Decoding.GetSize();i++) {
		if (m_usageH263Decoding[i]->m_pOutputClient==pClient) {
			// drop entry
			delete m_usageH263Decoding[i];
			m_usageH263Decoding.RemoveAt(i);
			i--;
		}
	}
	m_usageH263Decoding.Unlock();

	theApp.UpdateStateInfo();
	m_outputClientsDropped.SafeAdd(pClient);
}
///////////////////////////////////////////////////////////////////////////////////////////
void CIPCServerControlServerSide::OnRequestServerReset(const CConnectionRecord &c)
{
	if (theApp.AllowClientAccess(this,SRV_CONTROL_REQUEST_SERVER_RESET)) 
	{
		// NOT YET check for right destination/permission
		DoConfirmServerReset();
		WK_TRACE(_T("Reset because of client request %s Dest:%s Source:%s User:%s Permission:%s\n"),
					GetShmName(),
					c.GetDestinationHost(),
					c.GetSourceHost(),
					c.GetUser(),
					c.GetPermission()
					);
		theApp.DoReset();
	}
	else
	{
		DoIndicateError(SRV_CONTROL_REQUEST_SERVER_RESET,SECID_NO_ID,CIPC_ERROR_UNABLE_TO_CONNECT,0);
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CIPCServerControlServerSide::SetActiveClient(CIPCOutputServerClient *pClient, BOOL bActive)
{
	m_csDisplayClient.Lock();
	
	CString sMsg;
	sMsg.Format(_T("ClientActive%s"),pClient->GetShmName());
	WK_STAT_LOG(_T("Client"),bActive,sMsg);
	
	if (bActive) 
	{
		WK_STAT_PEAK(_T("Client"),1,_T("ClientChanged"));
		m_idActiveClient = pClient->GetID();
	}

	m_csDisplayClient.Unlock();
	// overlay/decompress client are changed in their respective fns.
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CIPCServerControlServerSide::SetOverlayClient(CSecID idClient,	WORD wDecoderID)
{
#if 1
	// OOPS each time ?
	// OBSOLETE ?, too slow
	CIPCOutputServer *pOutput = theApp.GetOutputGroups().GetGroupByID(CSecID(wDecoderID,0));
#endif
	
	if (pOutput->GetCompressionType()!=COMPRESSION_H263) 
	{
		WK_TRACE_ERROR(_T("SetOverlay for non H263 %s\n"),pOutput->GetShmName());
	}
	
	if (   idClient != m_idOverlayClient
		&& (   m_idOverlayClient==SECID_NO_ID
			|| idClient != m_idOverlayClient)
		) {
		m_csDisplayClient.Lock();
		WK_STAT_PEAK(_T("Client"),1,_T("OverlayChanged"));
		// inform old client
		CIPCOutputServerClient* pOverlayClient = m_outputClients.GetClientByID(m_idOverlayClient);
		if (pOverlayClient) 
		{
			pOverlayClient->DoIndicateClientChanged(CDR_OVERLAY);
		}
		m_idOverlayClient = idClient;
		m_wLastOverlayDecoderID=wDecoderID;
		m_csDisplayClient.Unlock();
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CIPCServerControlServerSide::SetDecompressClient(CIPCOutputServerClient *pClient)
{
	if (    pClient->GetID() != m_idDecompressClient
		&& (   m_idDecompressClient==SECID_NO_ID
			|| pClient->GetID()!=m_idDecompressClient)
		
		) {
		m_csDisplayClient.Lock();
		WK_TRACE(_T("SetDecompressClient %x\n"),pClient->GetID());

		// inform old client
		CIPCOutputServerClient* pDecompressClient = m_outputClients.GetClientByID(m_idOverlayClient);
		if (pDecompressClient) 
		{
			pDecompressClient->DoIndicateClientChanged(CDR_DECOMPRESS_H263);
		}
		m_idDecompressClient = pClient->GetID();
		m_csDisplayClient.Unlock();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIPCServerControlServerSide::IndicateAlarmToClients(WORD wGroupID,
											DWORD inputMask, 	// 1 high, 0 low
											DWORD changeMask)	// 1 changed, 0 unchanged
{
	// inform all clients
	// NOT YET depending on HasOutstanding...
	for (int i=0;i<m_inputClients.GetSize();i++) 
	{
		if (m_inputClients[i]->GetIPCState() == CIPC_STATE_CONNECTED)
		{
			m_inputClients[i]->DoIndicateAlarmState(wGroupID,inputMask,changeMask,NULL);// clients don't get infos
		}
	}

}
/////////////////////////////////////////////////////////////////////////////
void CIPCServerControlServerSide::IndicateAlarmToClients(CSecID id, BOOL bAlarm, DWORD dwData1, DWORD dwData2)
{
	for (int i=0;i<m_inputClients.GetSize();i++) 
	{
		if (m_inputClients[i]->GetIPCState() == CIPC_STATE_CONNECTED)
		{
			m_inputClients[i]->DoIndicateAlarmNr(id,bAlarm,dwData1,dwData2);
		}
	}

}
