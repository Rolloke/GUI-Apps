/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: IsdnConnection.cpp $
// ARCHIVE:		$Archive: /Project/Capi/ISDNUnit/IsdnConnection.cpp $
// CHECKIN:		$Date: 3.03.06 11:57 $
// VERSION:		$Revision: 306 $
// LAST CHANGE:	$Modtime: 3.03.06 11:56 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

// @doc

#include "stdafx.h"
#include "IsdnUnit.h"

#include "hdr.h"
#include "CapiQueue.h"
#include "CipcOutput.h"
#include "Net_Messages.h"
#include "CIPCExtraMemory.h"
#include "CIPCPipeISDN.h"
#include "CIPCServerControlISDN.h"
#include "IPCFetch.h"
#include "CIPCServerControlClientSide.h"
#include "IsdnConnection.h"
#include "WK_Names.h"
#include "WK_Dongle.h"

#include "CIPCPipeISDN.h"
#include "CIPCPipeIST.h"
#include "CIPCPipeInputPT.h"
#include "CIPCPipeOutputPT.h"

#include "KnockKnockBox.h"
#include "PTBox.h"

#include "CIPCstringdefs.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
static TCHAR BASED_CODE szEndOfList[] = _T("<<EndeDerListe>>");

/////////////////////////////////////////////////////////////////////////////
#define DUMP_PIPE_STATE				FALSE
#define PENDING_PIPE_TIMEOUT		((DWORD)30000)
#define MAX_PHYSICAL_B_CHANNELS		2

/////////////////////////////////////////////////////////////////////////////
BOOL CISDNConnection::ms_bSupportsMSN = FALSE;

/////////////////////////////////////////////////////////////////////////////
CISDNConnection::CISDNConnection(CISDNUnitApp* pApp, CIPCServerControlISDN* pControl, WORD wID)
{
	m_bDoResetCapi = FALSE;	// for thread communication
	m_bDoCloseCapi = FALSE;
	m_pCapi = NULL;
	m_pKnockKnockBox=NULL;

	m_bIsValid		= FALSE;
	m_pCipcSendQ	= NULL;
	m_pCipcReceiveQ	= NULL;
	m_pControl		= pControl;

	m_CallState		= CALL_NONE;
	m_ConnectState	= CONNECT_CLOSED;
	m_wNrBChannels	= 0;
	m_wNrBChannelsWanted = 0;

	m_typeConnectRequest		= CIPC_NONE;

	m_bThereWerePipesBefore		= FALSE;
	m_bActiveRemovingPipes		= FALSE;
	m_dwReceivedBlockCounter	= 0;

	m_pCachedPipe=NULL;

	m_wCapiInfo =0;
	m_bNoNumbersFromCapi = FALSE;
	m_bCompressed = FALSE;

	if (MyGetApp()->RunAsPTUnit()) 
	{
		m_bIsPTConnection = TRUE;
	}
	else
	{
		m_bIsPTConnection = FALSE;
	}
	
	m_pPTBox = new CPTBox(this);	// always created
	m_bHandlingCapiGetMessageError = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
CISDNConnection::~CISDNConnection()
{
	DeleteAllPipesAndResetQueues();

	if (m_pCapi)
	{
		m_pCapi->Close();
		m_pCapi->Reset(TRUE);
	}

	WK_DELETE(m_pPTBox);

	WK_DELETE(m_pCapi);
	m_bIsValid = FALSE;

	WK_DELETE(m_pCipcSendQ);
	WK_DELETE(m_pCipcReceiveQ);

	DeleteKnockKnockBox();
	NrBChannelsChanged(0);
}
/////////////////////////////////////////////////////////////////////////////
/*@mfunc
A variant to reset the capi from other threads,
That is after SetupChanged.
*/
void CISDNConnection::CrossThreadResetCapi()
{
	// WK_TRACE(_T("Reset Capi...\n"));
	// So we are called from another thread
	// and the CaiThread should kill and create m_pCapi.
	// 'signal' the special state and wait for 'result'
	// OOPS Lock dropped to avoid deadlock
	m_bDoResetCapi = TRUE;

	int iNumWait=100;
	while (m_bDoResetCapi && iNumWait) 
	{
		Sleep(50);
		iNumWait--;
	}
	if (iNumWait==0)
	{
		WK_TRACE_ERROR(_T("Failed to reset Capi\n"));
	}
	else
	{
		// WK_TRACE(_T("Reset Capi done.\n"));
	}
}
/////////////////////////////////////////////////////////////////////////////
/*@mfunc
m_pCapi creation and more.
*/
BOOL CISDNConnection::InitCapi(HWND hWnd) 
{
	m_bIsValid = TRUE;
	
	if (m_pCapi)
	{
		m_pCapi->Close();
		m_pCapi->Reset();
	}
	else
	{
		m_pCapi = new CCapi();

		BOOL bUseRawData;
		if (MyGetApp()->RunAsPTUnit()==FALSE)
		{
			bUseRawData = FALSE;
		}
		else
		{
			bUseRawData = TRUE;
		}
		int iCapiRet = m_pCapi->NewCreate(	this, hWnd, 
										NET_CAPI_MSG, 
										(WORD)MyGetApp()->GetNumConfiguredBChannels(),
										7, // DEFAULT_X_BUFFERS (4)
										PROT_NOHANDSAKE, 
										7, // wMyMaxMovingBlocks=MAX_MOVING_BLOCKS,
										TRUE,	// bChannelBundling, not for raw data anyway
										bUseRawData
										);

		if (iCapiRet != 0)
		{
			CapiError(iCapiRet);
			return FALSE;
		}
	}
	// disable CCapi Password handshake
	m_pCapi->SetPasswort(NULL);

	if (m_pCipcSendQ==NULL) 
	{
		m_pCipcSendQ = new CCapiQueue(m_pCapi);
	}
	else
	{
		m_pCipcSendQ->Reset();
	}
	if (m_pCipcReceiveQ==NULL)
	{
		m_pCipcReceiveQ = new CCapiQueue(m_pCapi);;
	}
	else
	{
		m_pCipcReceiveQ->Reset();
	}

	m_bIsValid = TRUE;
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CISDNConnection::HandlePipeRequest(const CIPCCmdRecord &cmd)
{
	Lock();
	CConnectionRecord connection(cmd);
	CString sHost = connection.GetDestinationHost();
	LPCTSTR pszHostName = NULL;
	DWORD dwRequestCounter=0;	// NOT YET

	if (m_bNoNumbersFromCapi)	// No transmitted numbers
	{
		pszHostName = sHost;	// identify pipe by remote hostname
	}
	else						// identify it via number
	{
		int p = -1;
		p = sHost.Find(ROUTING_CHAR);
		if (p!=-1)
		{
			sHost = sHost.Left(p);
		}

		// OOPS set some member for the protocol?
		// OOPS modify in ConnetionRecord?
		if (sHost.Find(_T("pres:"))!=-1)
		{
			// OOPS assume leading pres:
			sHost = sHost.Right(sHost.GetLength()-strlen("pres:"));
			m_bIsPTConnection = TRUE;	// OOPS this is only for outgoing
			m_pCapi->EnableRawData(TRUE);
		}
		else
		{
			// OOPS make sure the right protocol ist set
			m_pCapi->EnableRawData(FALSE);
		}

		if (   sHost.GetLength()>=theApp.GetMinimumPrefix())
		{
			sHost = theApp.GetPrefix() + sHost;
		}
	}
	WORD wBChannels = 0;
	CString sBChannels;
	if (connection.GetFieldValue(CRF_B_CHANNELS, sBChannels))
	{
		wBChannels = (WORD)_ttol(sBChannels);
	}
	CString sError;
	sError.LoadString(IDS_ISDN_BUSY);

	WK_TRACE(_T("Connecting to |%s| while connected to No:|%s| at host|%s|, cmd:%s\n"),
		sHost, GetRemoteNumber(), GetRemoteHostName(), CIPC::NameOfCmd(cmd.m_dwCmd));

	switch(cmd.m_dwCmd) 
	{
		case SRV_CONTROL_REQUEST_INPUT_CONNECTION:
			{
				// already connected?
				if ( IsConnected(sHost, pszHostName) && MyGetApp()->RunAsPTUnit()==FALSE)
				{
					 AddInputPipeClient(connection, dwRequestCounter);
				}
				else if ( IsConnected(_T("")) )
				{
					CallRemoteAddress(sHost, wBChannels);
					AddInputPipeClient(connection, dwRequestCounter);
					// m_sRemoteNumber  is not set, if the connection is listening
				}
				else
				{
					// already connected to another host
					WK_TRACE(_T("Busy, can't connect to '%s' while connected to '%s'\n"),
						sHost, m_sRemoteNumber);

					m_pControl->DoNewIndicateError(SRV_CONTROL_REQUEST_INPUT_CONNECTION,
						SECID_NO_ID, CIPC_ERROR_UNABLE_TO_CONNECT,
						0, sError);
				}
			}
		break;
		case SRV_CONTROL_REQUEST_OUTPUT_CONNECTION:
			{
				// already connected?
				if ( IsConnected(sHost, pszHostName) )
				{
					 AddOutputPipeClient(connection, dwRequestCounter);
					 if (IsPTConnection())
					 {
						 if (m_pPTBox->m_pOutputPT==NULL) {
							 PanicClose(_T("Should not have OutputPT"));
						 }
						 else
						 {
							 // OOPS hardcoded, input first
	 						CIPCType cipcRequestType = CIPC_OUTPUT_SERVER;
							CIPCFetchResult remoteFetchResult(
									_T("remote PT"),	// hidden remote hostname
									CIPC_ERROR_OKAY, 0,	
									_T(""),
									0,
									SECID_NO_ID
#ifdef _UNICODE
									, 0 //we do not know, which codepage PT supports
#endif
							);

							OnCipcPipeConfirm(cipcRequestType,
												m_pPTBox->m_pOutputPT->GetID(),
												CSecID(SECID_NO_ID),
												CSecID(SECID_NO_ID),
												CSecID(SECID_NO_ID),
												remoteFetchResult
												);
						 }

					 }	// end of PTConnection
				}
				else if (IsConnected(_T("")))
				{
					// m_sRemoteNumber  is not set, if the connection is listening
					CallRemoteAddress(sHost, wBChannels);
					AddOutputPipeClient(connection, dwRequestCounter);
				}
				else
				{
					WK_TRACE(_T("Busy, can't connect to '%s' while connected to '%s'\n"),
						sHost, m_sRemoteNumber);

					{	// alreay connected to another host
						m_pControl->DoNewIndicateError(SRV_CONTROL_REQUEST_OUTPUT_CONNECTION,
							SECID_NO_ID, CIPC_ERROR_UNABLE_TO_CONNECT,
							0, sError);
					}
				}
			}
		break;
		case SRV_CONTROL_REQUEST_AUDIO_CONNECTION:
			{
				if (IsISTConnection())
				{
					// already connected?
					if ( IsConnected(sHost, pszHostName) )
					{
						 AddAudioPipeClient(connection, dwRequestCounter);
					}
					else if (IsConnected(_T("")))
					{
						// m_sRemoteNumber  is not set, if the connection is listening
						CallRemoteAddress(sHost, wBChannels);
						AddAudioPipeClient(connection, dwRequestCounter);
					}
					else
					{
						WK_TRACE(_T("Busy, can't connect to '%s' while connected to '%s'\n"),
							sHost, m_sRemoteNumber);

						{	// alreay connected to another host
							m_pControl->DoNewIndicateError(SRV_CONTROL_REQUEST_AUDIO_CONNECTION,
								SECID_NO_ID, CIPC_ERROR_UNABLE_TO_CONNECT,
								0, sError);
						}
					}
				}
				else if (IsPTConnection())
				{
					PanicClose(_T("Should not have AudioPT"));
				}
				else
				{
					PanicClose(_T("Audio at UNKNOWN connection type"));
				}
			}
		break;
		case SRV_CONTROL_REQUEST_DB_CONNECTION:
			{
				if (IsISTConnection())
				{
					// already connected?
					if ( IsConnected(sHost, pszHostName) )
					{
						 AddDataBasePipeClient(connection, dwRequestCounter);
					}
					else if ( IsConnected(_T("")))
					{
						// m_sRemoteNumber  is not set, if the connection is listening
						CallRemoteAddress(sHost, wBChannels);
						AddDataBasePipeClient(connection, dwRequestCounter);
					}
					else
					{
						WK_TRACE(_T("Busy, can't connect to '%s' while connected to '%s'\n"),
							sHost, m_sRemoteNumber);
	
						{	// alreay connected to another host
							m_pControl->DoNewIndicateError(SRV_CONTROL_REQUEST_DB_CONNECTION,
								SECID_NO_ID, CIPC_ERROR_UNABLE_TO_CONNECT,
								0, sError);
						}
					}
				}
				else if (IsPTConnection())
				{
					PanicClose(_T("Should not have DatabasePT"));
				}
				else
				{
					PanicClose(_T("Database at UNKNOWN connection type"));
				}
			}
		break;
		case SRV_CONTROL_REQUEST_ALARM_CONNECTION:
			{
				// already connected?
				if ( IsConnected(sHost, pszHostName) )
				{
					 AddAlarmPipes(connection, dwRequestCounter);
				}
				else if (IsConnected(_T("")))
				{
					// m_sRemoteNumber  is not set, if the connection is listening
					CallRemoteAddress(sHost, wBChannels);
					AddAlarmPipes(connection, dwRequestCounter);
				}
				else
				{
					// alreay connected to another host
					m_pControl->DoNewIndicateError(
						SRV_CONTROL_REQUEST_ALARM_CONNECTION,
						SECID_NO_ID, 
						CIPC_ERROR_UNABLE_TO_CONNECT,
						0,
						sError
					);
				}
			}
			break;
		default:
			WK_TRACE_ERROR(_T("Unsupported cmd %s in HandlePipeRequest\n"),
				CIPC::NameOfCmd(cmd.m_dwCmd)
				);
	}
	Unlock();
}
/////////////////////////////////////////////////////////////////////////////
int CISDNConnection::PollConnection() 
{
	Lock();

	// NOT YET use queueSemaphore
	CIPCCmdRecord *pPipeRequest = m_pipeRequests.SafeGetAndRemoveHead();
	if (pPipeRequest) {
		HandlePipeRequest(*pPipeRequest);
		WK_DELETE(pPipeRequest);
	}
	// Connection ueberpruefen, ob Leichen herumliegen und lieber abgebaut werden soll
	CheckConnection();

	int iInternalLoop =1;
	if (IsPTConnection())
	{
		iInternalLoop=3;
	}
	int iNumActions = 0;
	for (int x=0;x<iInternalLoop;x++)
	{
		// Bei Capi nachgucken, ob was anliegt
		iNumActions += m_pCapi->Poll();

		if (IsPTConnection())
		{
			// reduce sending
			// always send at least one
			if (x==0 || m_pPTBox->GetNumOutstandingCmds()<5)
			{
				m_pCipcSendQ->SendData(this);	// this for protocol checks
			}
		}
		else
		{
			m_pCipcSendQ->SendData(this);	// this for protocol checks
		}

		// Bei Capi nachgucken, ob was anliegt
		iNumActions += m_pCapi->Poll();
		if (iInternalLoop>1)
		{
			Sleep(25);
		}
	}
	Unlock();
	return iNumActions;
}
/////////////////////////////////////////////////////////////////////////////
void CISDNConnection::DeleteAllPipesAndResetQueues()
{
	Lock();
	if (theApp.TracePipes())
	{
		WK_TRACE(_T("DeleteAllPipesAndResetQueues(): %s -> %s(%s)\n"), m_sOwnNumber, m_sRemoteNumber, m_sRemoteHostName);
	}
	m_InputPipesClient.SafeDeleteAll();
	m_InputPipesServer.SafeDeleteAll();
	m_OutputPipesClient.SafeDeleteAll();
	m_OutputPipesServer.SafeDeleteAll();
	m_DataBasePipesClient.SafeDeleteAll();
	m_DataBasePipesServer.SafeDeleteAll();
	m_AudioPipesClient.SafeDeleteAll();
	m_AudioPipesServer.SafeDeleteAll();
	
	
	m_pCachedPipe = NULL; // NEW CT
	m_pPTBox->m_pInputPT=NULL;
	m_pPTBox->m_pOutputPT=NULL;

	if (m_pCipcSendQ)
	{
		m_pCipcSendQ->Reset();
	}
	if (m_pCipcReceiveQ)
	{
		m_pCipcReceiveQ->Reset();
	}
	m_dwReceivedBlockCounter = 0;
	Unlock();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CISDNConnection::CrossThreadCloseConnection(BOOL bWaitForResponse)
{
	// WK_TRACE(_T("CrossThreadCloseConnection...\n"));
	// So we are called from another thread
	// and the CapiThread should do m_pCapi->Close().
	// 'signal' the special state and wait for 'result'

	// OOPS Lock dropped to avoid dead lock
	m_bDoCloseCapi = TRUE;

	int iNumWait=100;
	while (m_bDoCloseCapi && iNumWait>=0)
	{
		Sleep(100);
		iNumWait--;
	}
	if (iNumWait==0)
	{
		WK_TRACE_ERROR(_T("Failed to close Capi\n"));
		return FALSE;
	}
	else
	{
		// WK_TRACE(_T("CrossThreadCloseConnection done.\n"));
		return TRUE;
	}


}
/////////////////////////////////////////////////////////////////////////////
void CISDNConnection::OpenSecondChannel()
{
	if (m_pCapi)
	{
		m_pCapi->Open(NULL, 0);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CISDNConnection::CheckCloseOneChannel()
{
	if (   theApp.DoCloseOneChannel() 
		&& m_ConnectState == CONNECT_OPEN
		&& m_wNrBChannels == 2 )
	{
		CloseOneChannel();
		return FALSE;
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CISDNConnection::CloseOneChannel()
{
	if (m_pCapi)
	{
		m_pCapi->Close(TRUE);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CISDNConnection::CloseConnection(BOOL bWaitForResponse)
{
	Lock();

	DeleteAllPipesAndResetQueues();	// OOPS order? droppedClient causes Close?

	BOOL bTimeOut = FALSE;
	if (GetConnectState() != CONNECT_CLOSED
		&& GetConnectState() != CONNECT_CLOSING)
	{
		TRACE(_T("CloseConnection: %d\n"), GetTickCount());
		m_pCapi->Close();
		m_ConnectState = CONNECT_CLOSING;
		// der Zustand closed wird erst gesetzt, wenn alle Kanäle geschlossen sind
		// bei zwei Kanälen dauert dies ca 300 ms.
		if (bWaitForResponse)	// das warten innerhalb dieser Funktion ist nicht sinnvoll
		{						// da die Kanäle erst geschlossen werden, wenn diese Funktion
			Sleep(10);			// verlassen wird. Aber 10 ms können nicht schaden.
		}
		//
		if (m_wNrBChannels == 0)
		{
			m_ConnectState = CONNECT_CLOSED;
		}
	}
	Unlock();

	return !bTimeOut;

}
/////////////////////////////////////////////////////////////////////////////
BOOL CISDNConnection::CheckConnection()
{
	BOOL bReturn = TRUE;

	RemoveOldPipes();

#if 0
	// OOPS OBSOLETE
	// Timeout fuer ConnectRequest ueberpruefen
	CheckTimeoutForConnectRequest();
#endif

	// Wenn Connected...
	if ( GetConnectState() == CONNECT_OPEN)
	{
		CheckForExistingPipes();

		// special for PTConnection
		CheckPTConnection();

		// check for unused but confirmed pipe
		// that is a client made a FetchXXX, got a confirmation, but did NOT create a CIPC
		CheckForPendingPipes();
	}

	// NEW 100597
	// wenn nicht connected aber pipes PanicClose
	// OOPS okay for incoming?
	if (GetConnectState()==CONNECT_CLOSED)
	{
		if (AnyPipesExisting())
		{
			PanicClose(_T("Still have pipes while not connected"));
		}
	}

	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
void CISDNConnection::RemoveOldPipes()
{
	m_removedPipes.Lock();
	while (m_removedPipes.GetSize())
	{
		CIPCPipeISDN *pOldPipe = m_removedPipes[0];
		if (theApp.TracePipes())
		{
			WK_TRACE(_T("Drop removed pipe %x\n"),pOldPipe->GetID().GetID());
		}
		m_removedPipes.RemoveAt(0);
		RemoveRemotePipe(pOldPipe->GetID(), pOldPipe->GetRemoteID(), pOldPipe->GetCIPCType());
		if (pOldPipe==m_pPTBox->m_pInputPT)
		{
			m_pPTBox->m_pInputPT = NULL;
		}
		if (pOldPipe==m_pPTBox->m_pOutputPT)
		{
			m_pPTBox->m_pOutputPT = NULL;
		}

		WK_DELETE(pOldPipe);
		m_pCachedPipe = NULL;
	}
	m_removedPipes.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
void CISDNConnection::CheckForExistingPipes()
{
	// Keine Pipes mehr?
	if (AnyPipesExisting() == FALSE)
	{
		// Gab es vorher Pipes, dann close Connection
		if (m_bThereWerePipesBefore)
		{
			// special for PTConnection
			if (IsPTConnection())
			{
				m_pPTBox->SendPTCmd(0x98,0,NULL);	// EXIT

				// OOPS TESTHACK
				int iMaxCount=100;
				while (iMaxCount>0 && m_pCipcSendQ->GetSendSize())
				{
					m_pCipcSendQ->SendData(this);	// send the exit cmd
					iMaxCount--;
				}
				m_pPTBox->SetPTConnectionState( PCS_NOT_CONNECTED );	// OOPS here?

				Sleep(100);
				// NOT YET
			}
			m_bThereWerePipesBefore = FALSE;
			WK_TRACE(_T("CloseConnection, there are no more pipes\n"));
			CloseConnection();
		}
	}
	else
	{
		// es sind noch Pipes vorhanden, Verbindung muss offen bleiben
	}
}
/////////////////////////////////////////////////////////////////////////////
void CISDNConnection::CheckPTConnection()
{
	if (IsPTConnection())
	{
		if (   m_pPTBox->GetPTConnectionState() != PCS_CONNECTED
			&& GetTickCount()-m_pPTBox->GetLastStateChange() > 30000
			)
		{
			// if it is an alam connection, try a missing Config
			// to update the state
			if (IsPTConnection() && GetCallState()==CALL_PASSIVE
				&& m_pPTBox->GetPTConnectionState()==PCS_NOT_CONNECTED) {	// OOPS fragile
				WK_TRACE(_T("Send missing PTConfig request\n"));
				MyGetApp()->ReadDebugOptions();	// to reset TestFrameCount
				m_pPTBox->SendPTConfig();
			}
			else
			{
				WK_TRACE(_T("Close pending PTConnection\n"));
				m_pPTBox->SendPTCmd(0x98,0,NULL);	// EXIT

				// OOPS TESTHACK
				int iMaxCount=100;
				while (iMaxCount>0 && m_pCipcSendQ->GetSendSize())
				{
					m_pCipcSendQ->SendData(this);	// send the exit cmd
					iMaxCount--;
				}
				m_pPTBox->SetPTConnectionState( PCS_NOT_CONNECTED );	// OOPS here?

				CloseConnection();
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CISDNConnection::CheckForPendingPipes()
{
	CheckPipeArrayForPendingPipes(m_InputPipesClient);
	CheckPipeArrayForPendingPipes(m_OutputPipesClient);
	CheckPipeArrayForPendingPipes(m_DataBasePipesClient);
	CheckPipeArrayForPendingPipes(m_AudioPipesClient);
	// Really necessary for server pipes?
	CheckPipeArrayForPendingPipes(m_InputPipesServer);
	CheckPipeArrayForPendingPipes(m_OutputPipesServer);
	CheckPipeArrayForPendingPipes(m_DataBasePipesServer);
	CheckPipeArrayForPendingPipes(m_AudioPipesServer);
}
/////////////////////////////////////////////////////////////////////////////
void CISDNConnection::CheckPipeArrayForPendingPipes(CIPCPipesArrayCS& pipes)
{
	CIPCPipeISDN* pPipe = NULL;
	pipes.Lock();
	for (int i=0 ; i < pipes.GetSize() ; i++)
	{
		pPipe = pipes.GetAt(i);
		CString sShmName =pPipe->GetCIPC()->GetShmName();
		if (   (pPipe->GetCIPC()->GetIPCState() != CIPC_STATE_CONNECTED)
			&& (pPipe->GetCIPC()->GetIPCState() != CIPC_STATE_READ_FOUND)
			)
		{
			if (pPipe->IsTimedOut() && !pPipe->GetCIPC()->GetIsMarkedForDelete())
			{
				WK_TRACE(_T("Pending Pipe remove %s %s 0x%x\n"),
					NameOfEnum(pPipe->GetCIPCType()), pPipe->GetCIPC()->GetShmName(), pPipe->GetID().GetID() );
				RemoveMe(pPipe);
			}
		}
	}
	pipes.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
void CISDNConnection::CancelActiveRequests()
{
	// OOPS still old style error message
// gf 2004/02/18 check obsolet, wird in der Funktion geprueft
//	if ( m_pControl->GetIPCState() == CIPC_STATE_CONNECTED )
	{
		CString sMsg;
		// OOPS what about timeout msg NOT YET
		// OOPS is m_wCapiInfo the current info
		// or are there some lost, if there are multiple once
		sMsg = NameOfCapiInfo(m_wCapiInfo);

		m_pControl->DoNewIndicateError(
					SRV_CONTROL_REQUEST_INPUT_CONNECTION,	// OOPS
					SECID_NO_ID, 
					CIPC_ERROR_UNABLE_TO_CONNECT, 
					0,
					sMsg					
					);
	}
	m_typeConnectRequest = CIPC_NONE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CISDNConnection::IsConnected(LPCTSTR psRemoteAddress, LPCTSTR pszRemotHostName/*=NULL*/) 
{
	BOOL bReturn = FALSE;
	if (pszRemotHostName)
	{
		bReturn = m_sRemoteHostName.CompareNoCase(pszRemotHostName) == 0;
		if (bReturn)
		{
			WK_TRACE(_T("Confirming connection, host names: %s == %s\n"), m_sRemoteHostName, pszRemotHostName);
		}
		return bReturn;
	}
	else if (m_sRemoteNumber == psRemoteAddress)
	{
		WK_TRACE(_T("Confirming connection numbers are equal\n"));
		return TRUE;
	}
	else
	{
		CString sNrConnected(m_sRemoteNumber);
		CString sNrRequested(psRemoteAddress);

		if (   sNrConnected.IsEmpty()
			|| sNrRequested.IsEmpty())
		{
			WK_TRACE(_T("Not Connected, one string is empty c:%s, r:%s\n"), sNrConnected, sNrRequested);
			return bReturn;
		}
		else
		{
			sNrConnected.MakeReverse();
			sNrRequested.MakeReverse();

			if (sNrConnected.GetLength()<sNrRequested.GetLength())
			{
				bReturn = 0 == sNrRequested.Find(sNrConnected);
			}
			else
			{
				bReturn = 0 == sNrConnected.Find(sNrRequested);
			}
			if (bReturn)
			{
				WK_TRACE(_T("Confirming connection numbers partly equal %s <> %s\n"), m_sRemoteNumber, psRemoteAddress);
			}
			return bReturn;
		}
	}

	/*
	return ( m_sRemoteNumber == psRemoteAddress );
	*/
}
/////////////////////////////////////////////////////////////////////////////
void CISDNConnection::CallRemoteAddress(const CString& sRemoteAddress, WORD wBChannels/*=0*/)
{
	if ( GetConnectState()	== CONNECT_CLOSED )
	{
		CString sNumber;
		sNumber = sRemoteAddress;
		if (IsPTConnection()) 
		{
			m_pPTBox->SetPTConnectionState(PCS_CALLING);
			m_pPTBox->ClearInternalData();
		}
		m_pCapi->SetPasswort(NULL);	// disable CAPI_ID handshake

		SetRemoteNumber(FilterDigits(sNumber));
		CString sRemoteNumber(GetRemoteNumber());
		m_wNrBChannelsWanted = wBChannels;
		if (m_wNrBChannelsWanted == 0)
		{
			m_wNrBChannelsWanted = (WORD)MyGetApp()->GetNumConfiguredBChannels();
		}
		else
		{
			WK_TRACE(_T("Restricted call to %s, use only %u channel(s)\n"), (LPCTSTR)sRemoteNumber, m_wNrBChannelsWanted);
		}
		m_pCapi->Open(sRemoteNumber, m_wNrBChannelsWanted);
		m_ConnectState	= CONNECT_REQUEST;
		m_CallState = CALL_ACTIVE;
	}
	else
	{
		WK_TRACE_ERROR(_T("Can't call '%s' while not in CLOSED state\n"),
						LPCTSTR (sRemoteAddress));
	}
}
/////////////////////////////////////////////////////////////////////////////
void CISDNConnection::SendPipeRequest(const CConnectionRecord &cIn,
									  CIPCType requestType, 
									  CIPCPipeISDN *pNewPipe,
									  CIPCPipeISDN *pNewOutputPipe)
{
	if (IsISTConnection())
	{
		CConnectionRecord c(cIn);	
		// make a local copy to allow modification
		// ConnectionRecord in CmdRecord umformen und einen CapiQueueRecord damit anlegen
		// hide data in strings, sigh
		CHostArray hosts;
#ifdef _DTS
		CWK_Profile wkp(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE,_T("DVRT\\DV"),_T(""));
#else
		CWK_Profile wkp;
#endif
		hosts.Load(wkp);
		CString sOwnName = hosts.GetLocalHostName();
		CString sMSN(theApp.GetOwnNumber());
		if (sMSN.IsEmpty())
		{
			sMSN = _T("0");
		}
		c.SetFieldValue(CRF_MSN,sMSN);
		if (c.GetSourceHost().IsEmpty())
		{
			c.SetSourceHost(sOwnName);
		}

		CString sBitrate;
		sBitrate.Format(_T("%d"),GetNumBChannels()*64*1024);
		c.SetFieldValue(CRF_BITRATE,sBitrate);

		CString sData;
		if (requestType!=CIPC_ALARM_CLIENT)
		{
			pNewPipe->SetNotConnectedTimeout(cIn.GetTimeout());
			sData.Format(_T("%d %d"),(DWORD) requestType, pNewPipe->GetID().GetID() );
		}
		else
		{
			sData.Format(_T("%d %d %d"),(DWORD) CIPC_ALARM_CLIENT,
							pNewPipe->GetID().GetID(),
							pNewOutputPipe->GetID().GetID()
							);
		}
		c.SetInputShm(sData);

		CIPCCmdRecord* pCmdRecord = c.CreateCmdRecord(pNewPipe->GetCIPC(),
													SRV_CONTROL_REQUEST_PIPE
													);
		CCapiQueueRecord* pNewRecord = new CCapiQueueRecord;
		pNewRecord->FillFromCmdRecord(pNewPipe->GetID(),
											SECID_NO_ID,
											requestType,	// OOPS unused?
											*pCmdRecord,
											m_bCompressed
											);
		WK_DELETE(pCmdRecord);
		m_pCipcSendQ->PrependToSendQueue(pNewRecord);
	}
	else if (IsPTConnection())
	{
		// NOT YET some internal state about the PT connection
		//  OOPS now in OutgoingOk
	}
	else
	{ 
		WK_TRACE_ERROR(_T("Invalid connection type\n"));
	}
}
/////////////////////////////////////////////////////////////////////////////
void CISDNConnection::AddInputPipeClient(const CConnectionRecord &cIn, 
										 DWORD dwRequestCounter) 
{
	WORD wInputPipeID = CIPCPipeISDN::GetNextGlobalID();
	// set the source
	CConnectionRecord c(cIn);
	
	// create a pipe connection first
	CString sShmNameInput;

	CIPCPipeISDN* pNewPipe=NULL;
	CString sSuffix;
	if (MyGetApp()->IsSecondInstance())
	{
		sSuffix = _T("_2");
	}
	if (IsISTConnection())
	{
		sShmNameInput.Format(_T("IsdnPipeInput%d"),wInputPipeID);
		sShmNameInput += sSuffix;
		pNewPipe = new CIPCPipeIST(CIPC_INPUT_CLIENT,
								this,
								(LPCTSTR)sShmNameInput,
								CIPC_MASTER,
								0,
								wInputPipeID, dwRequestCounter
								);
	}
	else if (IsPTConnection())
	{
		if (m_pPTBox->m_pInputPT)
		{
			PanicClose(_T("Internal error, still have inputPT\n"));
			return;	// <<< EXIT >>>
		}
		else
		{
			sShmNameInput.Format(_T("PTPipeInput%d"),wInputPipeID);
			sShmNameInput += sSuffix;
			m_pPTBox->m_pInputPT = new CIPCPipeInputPT(CIPC_INPUT_CLIENT,
									this,
									(LPCTSTR)sShmNameInput,
									CIPC_MASTER,
									0,
									wInputPipeID, dwRequestCounter
									);
			pNewPipe = m_pPTBox->m_pInputPT;
		}
	}
	else
	{
		PanicClose(_T("Invalid connection type\n"));
		return;	// <<< EXIT >>>
	}

	m_typeConnectRequest = CIPC_INPUT_CLIENT;


	m_InputPipesClient.SafeAdd(pNewPipe);
	pNewPipe->GetCIPC()->StartThread(TRUE);
	m_bThereWerePipesBefore = TRUE;

	if (theApp.TracePipes())
	{
		WK_TRACE(_T("%s '%s'\n"), _T(__FUNCTION__), sShmNameInput);
	}

	SendPipeRequest(c,CIPC_INPUT_SERVER,pNewPipe);
}
/////////////////////////////////////////////////////////////////////////////
void CISDNConnection::AddOutputPipeClient(const CConnectionRecord &cIn, 
										  DWORD dwRequestCounter
										  ) 
{
	WORD wOutputPipeID = CIPCPipeIST::GetNextGlobalID();
	// set the source
	CConnectionRecord c(cIn);

	CIPCPipeISDN *pNewPipe=NULL;
	// create a client connection first
	CString sSuffix;
	if (MyGetApp()->IsSecondInstance())
	{
		sSuffix = _T("_2");
	}
	CString sShmNameOutput;
	if (IsISTConnection())
	{
		sShmNameOutput.Format(_T("IsdnPipeOutput%d"),wOutputPipeID);
		sShmNameOutput += sSuffix;
		pNewPipe = new CIPCPipeIST(CIPC_OUTPUT_CLIENT,
									this,
									(LPCTSTR)sShmNameOutput,
									CIPC_MASTER,
									c.GetOptions(),
									wOutputPipeID, dwRequestCounter
									);
	}
	else if (IsPTConnection())
	{
		if (m_pPTBox->m_pOutputPT)
		{
			PanicClose(_T("Internal error, still have outputPT\n"));
			return;	// <<< EXIT >>>
		}
		else
		{
			sShmNameOutput.Format(_T("PTPipeOutput%d"),wOutputPipeID);
			sShmNameOutput += sSuffix;
			m_pPTBox->m_pOutputPT = new CIPCPipeOutputPT(CIPC_OUTPUT_CLIENT,
									this,
									(LPCTSTR)sShmNameOutput,
									CIPC_MASTER,
									c.GetOptions(),
									wOutputPipeID, dwRequestCounter
								);
			pNewPipe = m_pPTBox->m_pOutputPT;
		}
	}
	else
	{
		PanicClose(_T("Invalid connection type\n"));
		return;	// <<< EXIT >>>
	}
	m_typeConnectRequest = CIPC_OUTPUT_CLIENT;

	m_OutputPipesClient.SafeAdd(pNewPipe);
	pNewPipe->GetCIPC()->StartThread(TRUE);
	m_bThereWerePipesBefore = TRUE;

	if (theApp.TracePipes())
	{
		WK_TRACE(_T("%s '%s'\n"), _T(__FUNCTION__), sShmNameOutput);
	}

	SendPipeRequest(c,CIPC_OUTPUT_SERVER,pNewPipe);
}
/////////////////////////////////////////////////////////////////////////////
void CISDNConnection::AddDataBasePipeClient(const CConnectionRecord &cIn, 
											DWORD dwRequestCounter) 
{	
	CString sSuffix;
	if (MyGetApp()->IsSecondInstance())
	{
		sSuffix = _T("_2");
	}

	WORD wPipeID = CIPCPipeIST::GetNextGlobalID();
	// set the source
	CConnectionRecord c(cIn);

	// create a client connection first
	CString sShmNameDataBase;
	sShmNameDataBase.Format(_T("IsdnPipeDataBase%d"),wPipeID);
	sShmNameDataBase += sSuffix;
	CIPCPipeIST *pNewPipe;
	pNewPipe = new CIPCPipeIST(CIPC_DATABASE_CLIENT,
								this,
								(LPCTSTR)sShmNameDataBase,
								CIPC_MASTER,
								0,
								wPipeID, dwRequestCounter
								);
	m_typeConnectRequest = CIPC_DATABASE_CLIENT;

	m_DataBasePipesClient.SafeAdd(pNewPipe);
	pNewPipe->StartThread(TRUE);
	m_bThereWerePipesBefore = TRUE;

	if (theApp.TracePipes())
	{
		WK_TRACE(_T("AddDataBasePipeClient() '%s'\n"), sShmNameDataBase);
	}

	SendPipeRequest(c,CIPC_DATABASE_SERVER,pNewPipe);
}
/////////////////////////////////////////////////////////////////////////////
void CISDNConnection::AddAlarmPipes(const CConnectionRecord &cIn,
									DWORD dwRequestCounter
									)
{
	WORD wInputPipeID = CIPCPipeIST::GetNextGlobalID();
	WORD wOutputPipeID = CIPCPipeIST::GetNextGlobalID();
	// set the source
	CConnectionRecord c(cIn);
	// create a client connection first

	if (theApp.TracePipes())
	{
		WK_TRACE(_T("AddAlarmPipes %x %x %s\n"),wInputPipeID, wOutputPipeID,c.GetDestinationHost());
	}
	CIPCPipeIST* pNewInputPipe = NULL;
	pNewInputPipe = new CIPCPipeIST(CIPC_INPUT_SERVER,
								this,
								(LPCTSTR)c.GetInputShm(),
								CIPC_SLAVE,
								0,
								wInputPipeID, dwRequestCounter,
								TRUE); // alarm
	CIPCPipeIST* pNewOutputPipe = NULL;
	pNewOutputPipe = new CIPCPipeIST(CIPC_OUTPUT_SERVER,
								this,
								(LPCTSTR)c.GetOutputShm(),
								CIPC_SLAVE,
								0,
								wOutputPipeID, dwRequestCounter,
								TRUE);	// alarm

	// TODO! RKE: Audio Alarm ?

	m_typeConnectRequest = CIPC_ALARM_SERVER;

	m_InputPipesServer.SafeAdd(pNewInputPipe);
	pNewInputPipe->StartThread(TRUE);
	m_OutputPipesServer.SafeAdd(pNewOutputPipe);
	pNewOutputPipe->StartThread(TRUE);
	m_bThereWerePipesBefore = TRUE;
	Sleep(10);	// OOPS test

	if (theApp.TracePipes())
	{
		WK_TRACE(_T("AddAlarmPipes() Server '%s[%x]' and '%s[%x]'\n"),
				(LPCTSTR)c.GetInputShm(), 
				pNewInputPipe->GetID().GetID(),
				(LPCTSTR)c.GetOutputShm(),
				pNewOutputPipe->GetID().GetID()
				);
	}

	SendPipeRequest(c,CIPC_ALARM_CLIENT,pNewInputPipe,pNewOutputPipe);
}
/////////////////////////////////////////////////////////////////////////////
void CISDNConnection::AddAudioPipeClient(const CConnectionRecord &cIn, 
										  DWORD dwRequestCounter
										  ) 
{
	WORD wAudioPipeID = CIPCPipeIST::GetNextGlobalID();
	// set the source
	CConnectionRecord c(cIn);

	CIPCPipeISDN *pNewPipe=NULL;
	// create a client connection first
	CString sSuffix;
	if (MyGetApp()->IsSecondInstance())
	{
		sSuffix = _T("_2");
	}
	CString sShmNameAudio;
	if (IsISTConnection())
	{
		sShmNameAudio.Format(_T("IsdnPipeAudio%d"),wAudioPipeID);
		sShmNameAudio += sSuffix;
		pNewPipe = new CIPCPipeIST(CIPC_AUDIO_CLIENT,
									this,
									(LPCTSTR)sShmNameAudio,
									CIPC_MASTER,
									c.GetOptions(),
									wAudioPipeID, dwRequestCounter
									);
	}
	else if (IsPTConnection())
	{
		PanicClose(_T("Internal error, No AudioPT\n"));
	}
	else
	{
		PanicClose(_T("Invalid connection type\n"));
		return;	// <<< EXIT >>>
	}
	m_typeConnectRequest = CIPC_AUDIO_CLIENT;

	m_AudioPipesClient.SafeAdd(pNewPipe);
	pNewPipe->GetCIPC()->StartThread(TRUE);
	m_bThereWerePipesBefore = TRUE;

	if (theApp.TracePipes())
	{
		WK_TRACE(_T("AddAudioPipeClient() '%s'\n"), sShmNameAudio);
	}

	SendPipeRequest(c,CIPC_AUDIO_SERVER,pNewPipe);
}
/////////////////////////////////////////////////////////////////////////////
// a) called from OnRequestDisconnect
// b) called if FetchAlarmConnection failed
void CISDNConnection::RemovePipe(CIPCPipeISDN* pRemovePipe) 
{
	Lock();
	if (theApp.TracePipes())
	{
		WK_TRACE(_T("RemovePipe %s\n"),pRemovePipe->GetCIPC()->GetShmName());
	}

	m_bActiveRemovingPipes = TRUE;
	CIPCType cipcType = pRemovePipe->GetCIPCType();
	CSecID id = pRemovePipe->GetID();
	CIPCPipeISDN* pGetPipe = NULL;
	int i = 0;
	BOOL bFound = FALSE;

	switch ( cipcType )
	{
		case CIPC_INPUT_CLIENT:
			bFound = FALSE;
			for ( i=0 ; (!bFound && i < m_InputPipesClient.GetSize() ) ; i++ )
			{
				pGetPipe = m_InputPipesClient.GetAt(i);
				if (pGetPipe == pRemovePipe) 
				{
					bFound = TRUE;
					WK_TRACE(_T("RemovePipe() INPUT_CLIENT '%s' %x\n"), pRemovePipe->GetCIPC()->GetShmName(), id.GetID() );
				}
			}
			if ( !bFound )
			{
				WK_TRACE_WARNING(_T("RemovePipe() INPUT_CLIENT '%s' %x NOT FOUND\n"), pRemovePipe->GetCIPC()->GetShmName(), id.GetID() );
				for (int x=0;x<m_InputPipesClient.GetSize();x++)
				{
					WK_TRACE(_T("InputClient[%d] is %d\n"),m_InputPipesClient[x]->GetIDByte());
				}
			}	
			break;
		case CIPC_INPUT_SERVER:
			bFound = FALSE;
			for ( i=0 ; (!bFound && i < m_InputPipesServer.GetSize() ) ; i++ )
			{
				pGetPipe = m_InputPipesServer.GetAt(i);
				if (pGetPipe == pRemovePipe)
				{
					bFound = TRUE;
					WK_TRACE(_T("RemovePipe() INPUT_SERVER '%s' %x\n"), pRemovePipe->GetCIPC()->GetShmName(), id.GetID() );
				}
			}
			if ( !bFound )
			{
				WK_TRACE_WARNING(_T("RemovePipe() INPUT_SERVER '%s' %x NOT FOUND\n"), pRemovePipe->GetCIPC()->GetShmName(), id.GetID() );
			}	
			break;
		case CIPC_OUTPUT_CLIENT:
			bFound = FALSE;
			for ( i=0 ; (!bFound && i < m_OutputPipesClient.GetSize() ) ; i++ )
			{
				pGetPipe = m_OutputPipesClient.GetAt(i);
				if (pGetPipe == pRemovePipe)
				{
					bFound = TRUE;
					WK_TRACE(_T("RemovePipe() OUTPUT_CLIENT '%s' %x\n"), pRemovePipe->GetCIPC()->GetShmName(), id.GetID() );
				}
			}
			if ( !bFound )
			{
				WK_TRACE_WARNING(_T("RemovePipe() OUTPUT_CLIENT '%s' %x NOT FOUND\n"), pRemovePipe->GetCIPC()->GetShmName(), id.GetID() );
			}	
			break;
		case CIPC_OUTPUT_SERVER:
			bFound = FALSE;
			for ( i=0 ; (!bFound && i < m_OutputPipesServer.GetSize() ) ; i++ )
			{
				pGetPipe = m_OutputPipesServer.GetAt(i);
				if (pGetPipe == pRemovePipe)
				{
					bFound = TRUE;
					WK_TRACE(_T("RemovePipe() OUTPUT_SERVER '%s' %x\n"), pRemovePipe->GetCIPC()->GetShmName(), id.GetID() );
				}
			}
			if ( !bFound )
			{
				WK_TRACE_WARNING(_T("RemovePipe() OUTPUT_SERVER '%s' %x NOT FOUND\n"), pRemovePipe->GetCIPC()->GetShmName(), id.GetID() );
			}	
			break;
		case CIPC_DATABASE_CLIENT:
			bFound = FALSE;
			for ( i=0 ; (!bFound && i < m_DataBasePipesClient.GetSize() ) ; i++ )
			{
				pGetPipe = m_DataBasePipesClient.GetAt(i);
				if (pGetPipe == pRemovePipe)
				{
					bFound = TRUE;
					WK_TRACE(_T("RemovePipe() DATABASE_CLIENT '%s' %x\n"), pRemovePipe->GetCIPC()->GetShmName(), id.GetID() );
				}
			}
			if ( !bFound )
			{
				WK_TRACE_WARNING(_T("RemovePipe() DATABASE_CLIENT '%s' %x NOT FOUND\n"), pRemovePipe->GetCIPC()->GetShmName(), id.GetID() );
			}	
			break;
		case CIPC_DATABASE_SERVER:
			bFound = FALSE;
			for ( i=0 ; (!bFound && i < m_DataBasePipesServer.GetSize() ) ; i++ )
			{
				pGetPipe = m_DataBasePipesServer.GetAt(i);
				if (pGetPipe == pRemovePipe)
				{
					bFound = TRUE;
					WK_TRACE(_T("RemovePipe() DATABASE_SERVER '%s' %x\n"), pRemovePipe->GetCIPC()->GetShmName(), id.GetID() );
				}
			}
			if ( !bFound ) {
				WK_TRACE_WARNING(_T("RemovePipe() DATABASE_SERVER '%s' %x NOT FOUND\n"), pRemovePipe->GetCIPC()->GetShmName(), id.GetID() );
			}	
			break;
		case CIPC_AUDIO_CLIENT:
			bFound = FALSE;
			for ( i=0 ; (!bFound && i < m_AudioPipesClient.GetSize() ) ; i++ )
			{
				pGetPipe = m_AudioPipesClient.GetAt(i);
				if (pGetPipe == pRemovePipe)
				{
					bFound = TRUE;
					WK_TRACE(_T("RemovePipe() AUDIO_CLIENT '%s' %x\n"), pRemovePipe->GetCIPC()->GetShmName(), id.GetID() );
				}
			}
			if ( !bFound )
			{
				WK_TRACE_WARNING(_T("RemovePipe() AUDIO_CLIENT '%s' %x NOT FOUND\n"), pRemovePipe->GetCIPC()->GetShmName(), id.GetID() );
			}	
			break;
		case CIPC_AUDIO_SERVER:
			bFound = FALSE;
			for ( i=0 ; (!bFound && i < m_AudioPipesServer.GetSize() ) ; i++ )
			{
				pGetPipe = m_AudioPipesServer.GetAt(i);
				if (pGetPipe == pRemovePipe)
				{
					bFound = TRUE;
					WK_TRACE(_T("RemovePipe() AUDIO_SERVER '%s' %x\n"), pRemovePipe->GetCIPC()->GetShmName(), id.GetID() );
				}
			}
			if ( !bFound )
			{
				WK_TRACE_WARNING(_T("RemovePipe() AUDIO_SERVER '%s' %x NOT FOUND\n"), pRemovePipe->GetCIPC()->GetShmName(), id.GetID() );
			}	
			break;
		default:
			WK_TRACE_ERROR(_T("RemovePipe() WRONG TYPE '%s' %x\n"), pRemovePipe->GetCIPC()->GetShmName(), id.GetID() );
			break;
	}

	if ( bFound && pGetPipe )
	{
		RemoveRemotePipe(pGetPipe->GetID(),pGetPipe->GetRemoteID(), pRemovePipe->GetCIPCType());
		pGetPipe->GetCIPC()->DelayedDelete();	// OOPS check thread?
	}
	
	Unlock();
}	// end of RemovePipe
/////////////////////////////////////////////////////////////////////////////
void CISDNConnection::RemoveRemotePipe(CSecID localID, CSecID remoteID, CIPCType ct)
{
	// NOT YET even if closing or reset?
	if (theApp.TracePipes())
	{
		WK_TRACE(_T("RemoveRemotePipe(%08x, %08x, %s)\n"), localID.GetID(), remoteID.GetID(), NameOfEnum(ct));
	}

	if (remoteID==SECID_NO_ID)
	{
		// do not send for _T('dangling') pipes
		return;	// <<< EXIT >>>
	}

	DWORD dwRemoveCmd = SRV_CONTROL_REQUEST_REMOVE_PIPE;
	CIPCCmdRecord* pCmdRecord = new CIPCCmdRecord(NULL,
													dwRemoveCmd,
													remoteID.GetID()
													);
	CCapiQueueRecord* pNewRecord = new CCapiQueueRecord;
	pNewRecord->FillFromCmdRecord(SECID_NO_ID, SECID_NO_ID,
									CIPC_NONE,
									*pCmdRecord,
									m_bCompressed
									);
	WK_DELETE(pCmdRecord);

	m_pCipcSendQ->PrependToSendQueue(pNewRecord);
}
/////////////////////////////////////////////////////////////////////////////
// called from CIPCPipeIST::HandleCmd
BOOL CISDNConnection::CipcDataRequest(CSecID idFrom, 
									  CSecID idTo,
									  CIPCType cipcType, 
									  DWORD dwCmd,
									  DWORD dwParam1, DWORD dwParam2,
									  DWORD dwParam3, DWORD dwParam4,
									  const CIPCExtraMemory* pExtraMem
									  ) 
{

	if (m_bDoCloseCapi || m_bDoResetCapi)
	{
		return FALSE;
	}
	// should be safe without Lock, the queue itself is threadsafe
	if (   dwCmd == CIPC_BASE_REQ_CONNECTION)
	{
#ifdef _DTS
		CWK_Profile wkp(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE,_T("DVRT\\DV"),_T(""));
#else
		CWK_Profile wkp;
#endif
		int nCompression = wkp.GetInt(theApp.GetProfileSection(), CSD_COMPRESSION, 0);
		if (m_bCompressed == 0 && nCompression != 0)
		{
			WK_TRACE(_T("Requesting Compression %d\n"), nCompression);
			dwParam1 = MAKELONG(nCompression, HIWORD(dwParam1));
		}
	}
	else if (dwCmd == CIPC_BASE_CONF_CONNECTION)
	{
		if (m_bCompressed)
		{
			WK_TRACE(_T("Confirming Compression %d\n"), m_bCompressed);
			dwParam1 = MAKELONG(m_bCompressed, HIWORD(dwParam1));
		}
	}

	CCapiQueueRecord* pNewRecord = new CCapiQueueRecord;
	pNewRecord->FillFromCipcData	(
									idFrom, idTo,
									cipcType, 
									dwCmd,
									dwParam1, dwParam2,
									dwParam3, dwParam4,
									pExtraMem,
									m_bCompressed
									) ;
	BOOL bRemoveOldest=FALSE;

	// OOPS only SWCodec not MiCo???
	if (   dwCmd == PICT_CONF_JPEG_ENCODING || dwCmd == OUTP_CONFIRM_JPEG_ENCODING_NEW)
	{
		bRemoveOldest=TRUE;
	}

	m_pCipcSendQ->AddToSendQueue(pNewRecord, bRemoveOldest);

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
long CISDNConnection::ReceiveCapiMessage(WPARAM wParam, LPARAM lParam)
{
	long lReturn = 0;
	switch (wParam)
	{   
		case C_OUTGOING:
		{
			// lParam LPSTR Nr
			// NOT YET any validation on szRemoteNumber
			CString str((LPCTSTR)lParam);
			SetRemoteHostName(_T(""));
			SetRemoteNumber(str);
			WK_TRACE(_T("Outgoing Call to '%s'\n"), str);
		}
		break;
		case C_REJECTING: 
			// lParam PCALLINFO
			if (lParam)
			{
				LPCALLINFO pCallInfo= (LPCALLINFO)lParam;
				CString sCalledParty(pCallInfo->szCalledParty);
				CString sCallingParty(pCallInfo->szCallingParty);
				WK_TRACE(_T("CallRejected from '%s' to '%s'\n"),sCallingParty,
													sCalledParty);
				UpdateMSNSupport(sCalledParty);
			}
			break;
		case C_OUTGOINGOK:
		{
			// lParam LPSTR Nr
			CString str((LPCTSTR)lParam);
			ConnectOK(str);
			WK_TRACE(_T("Outgoing Call OK '%s'\n"), str);
		} break;
		case C_INCOMMINGOK:
		{
			// lParam LPSTR Nr
			CString str((LPCTSTR)lParam);
			ConnectOK(str);
			WK_TRACE(_T("Incomming Call OK '%s'\n"), str);
		} break;
		case C_HANGUP:
			// lParam 0L
			ConnectClosed();
			SetRemoteHostName(_T(""));
		    break;      
		case C_CHANNEL_HANGUP:
		{
			// lParam LPSTR Nr
			// OBSOLETE
			CString str((LPCTSTR)lParam);
			WK_TRACE(_T("ChannelClosed %s\n"), str);
			if (m_bCompressed)
			{
				double dRate = CCapiQueueRecord::gm_avgCompression.GetAverage();
				WK_TRACE(_T("Avg Compression Rate: %f\n"), dRate);
			}
		} break;
		case C_NODISCONNECT:
		{
			// lParam LPSTR Nr
			CString str((LPCTSTR)lParam);
			CloseFailed(str);
			WK_TRACE(_T("No disconnect '%s'\n"), str);
		} break;
		case C_INFO:
			// LOWORD -lParam INF-Msg
			InfoMsg( LOWORD(lParam) );
			break;
		case C_NR_CONNECT: 
			// LOWORD -lParam B-Kanaele
			NrBChannelsChanged( LOWORD(lParam) );
			break; 
		case C_UNKNOWNDATA:
			// lParam - LPBYTE Data
			{
				const BYTE *pData = (const BYTE *)lParam;
				// do not check if already closing
				// that is more data coming in after the PanicClose
				CString sLangMsg;
				if (m_ConnectState != CONNECT_CLOSING && m_ConnectState != CONNECT_CLOSED)
				{
					if (wParam>=1 && pData[0]==0xA8)	// OOPS single byte cmp
					{
						if (sLangMsg.LoadString(IDS_PROCTOCOL_ERROR_REMOTE_PTUNIT)==FALSE)
						{
							sLangMsg=_T("Protocol error, called by remote PTUnit.");
						}

						CWK_RunTimeError runTimeError(REL_MESSAGE, RTE_PROTOCOL, sLangMsg);
						runTimeError.Send();

						PanicClose(_T("Protocol error, called from remote PTUnit"));
					}
					else if (wParam>=1 && pData[0]==0xE4)	// OOPS single byte cmp
					{
						if (sLangMsg.LoadString(IDS_PROCTOCOL_ERROR_REMOTE_PTBOX)==FALSE)
						{
							sLangMsg=_T("Protocol error, called by remote PTBox.");
						}

						CWK_RunTimeError runTimeError(REL_MESSAGE, RTE_PROTOCOL, sLangMsg);
						runTimeError.Send();

						PanicClose(_T("Protocol error, called from remote PTBox"));
					}
					else
					{
						if (sLangMsg.LoadString(IDS_PROCTOCOL_ERROR_REMOTE_UNKNOWN)==FALSE)
						{
							sLangMsg=_T("Protocol error, called by unknown remote system.");
						}

						CWK_RunTimeError runTimeError(REL_MESSAGE, RTE_PROTOCOL, sLangMsg);
						runTimeError.Send();

						PanicClose(_T("Received unknown data"));
						HexDump(pData,wParam,_T("Unknown data"));
					}
				}
			}
			break;           
		case C_LISTENREQUEST:
			// LOWORD -lParam INF-Msg or 0
			ListenConfirm( LOWORD(lParam) );
			SetRemoteHostName(_T(""));
			break;
		case C_USER_BUSY:
			// lParam = 0L	// OOPS ever called?
			WK_TRACE(_T("ReceiveCapiMessage() User busy\n"));
			WhatsHappened(INF_20_USER_BUSY);
			SetRemoteHostName(_T(""));
			break;
		case C_CALL_REJECTED:
			// lParam = 0L	// OOPS ever called?
			WK_TRACE(_T("ReceiveCapiMessage() Call rejected\n"));
			WhatsHappened(INF_20_CALL_REJECTED);
			SetRemoteHostName(_T(""));
			break;
		default:
			WK_TRACE(_T("Old style Capi function called %d\n"),wParam);
	}	// end of switch(wParam)
	return lReturn;
}
/////////////////////////////////////////////////////////////////////////////
void CISDNConnection::ListenConfirm(WORD wInfo)
{
	WK_TRACE(_T("ListenConfirm Info %04x OwnNr '%s'\n"), wInfo, (LPCTSTR)MyGetApp()->GetOwnNumber());
	if ( wInfo )
	{
		// some error NOT yet
	}
	else
	{
		// okay
	}
	if (m_ConnectState != CONNECT_CLOSED)
	{
		TRACE(_T("Forcing ConnectState(%s) to Closed!\n"), NameOfEnum(m_ConnectState));
		m_ConnectState = CONNECT_CLOSED;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CISDNConnection::ReceiveCipcData(const CIPCHDR *pCipcHdr)
{

	if (m_bDoCloseCapi || m_bDoResetCapi)
	{
		return;
	}
	BYTE byService = pCipcHdr->byService;
	CIPCType cipcType;
	CCapiQueue::SplitTypeFromService(byService,cipcType);

	// WK_TRACE(_T("TraceReceive service is %d wDataLen %d\n"),byService,pCipcHdr->wDataLen);

	m_dwReceivedBlockCounter++;
	switch(byService)
	{
		case CIPC_DATA_SMALL:
		case CIPC_DATA_LARGE_END:
			{
				m_pCipcReceiveQ->AddToReceiveQueue(pCipcHdr);
				CCapiQueueRecord* pRecord = m_pCipcReceiveQ->GetReceivedCmd();
				if (pRecord)
				{
					if ( PushCipcCmd(pRecord) )	// includes handling of control cmds
					{
						// fine
					}
					else
					{
						// no msg: it's okay if client is removing/removed
					}
					WK_DELETE(pRecord);
				}
				else
				{
					// not all data collected
				}
			}
			break;
		case CIPC_DATA_LARGE:
		case CIPC_DATA_LARGE_START:
			m_pCipcReceiveQ->AddToReceiveQueue(pCipcHdr);
			break;
		default:
			WK_TRACE_ERROR(_T("Unknown service type %d\n"),byService);
	}	// end of switch over service
}
/////////////////////////////////////////////////////////////////////////////
void CISDNConnection::ConnectOK(LPCTSTR pCalledParty)
{
	WK_STAT_LOG(_T("Link"),1,pCalledParty);
	WK_TRACE(_T("ConnectOK with '%s'\n"), pCalledParty);

	m_ConnectState = CONNECT_OPEN;
	m_sRemoteNumber  = pCalledParty;
	CWK_Profile wkp;
	wkp.WriteString(theApp.GetProfileSection(), szConnectedRemoteNumber, m_sRemoteNumber);

	if (IsISTConnection())
	{
		// all done
	}
	else if (IsPTConnection())
	{
		MyGetApp()->ReadDebugOptions();	// to reset TestFrameCount
		m_pPTBox->SendPTConfig();
	}
	theApp.GetMainWnd()->PostMessage(ISDNUNITWND_UPDATE_ICON, 1, 0);
}
/////////////////////////////////////////////////////////////////////////////
void CISDNConnection::ConnectClosed()
{
	CAutoCritSec acs(&m_cs);
	WK_STAT_LOG(_T("Link"),0,LPCTSTR(m_sRemoteNumber));

	BOOL bFullReset = FALSE;
	if (m_ConnectState != CONNECT_CLOSED)
	{
		ConnectionState eCS = m_ConnectState;
		m_ConnectState	= CONNECT_CLOSED;
		WK_TRACE(_T("Forcing ConnectState(%s) to %s!\n"), NameOfEnum(eCS), NameOfEnum(m_ConnectState));
		DumpPipeState();
		bFullReset = TRUE;
	}
	
	m_CallState		= CALL_NONE;
	m_sRemoteNumber  = _T("");
	theApp.ClearReservedCallingID();

	if (m_typeConnectRequest != CIPC_NONE)
	{
		CancelActiveRequests();
	}

	DeleteAllPipesAndResetQueues();

	m_bThereWerePipesBefore = FALSE;
	m_bActiveRemovingPipes = FALSE;

	if (bFullReset)
	{
		acs.Unlock();
		OnCapiGetMessageError();
	}
	else
	{
		m_pCapi->Reset();
		m_pCapi->ListenRequest(m_sOwnNumber);//, byController=1, dwServiceMask=0x0000020, dwInfoMask=0);
	}
	acs.Unlock();
	theApp.GetMainWnd()->PostMessage(ISDNUNITWND_UPDATE_ICON, 1, 0);
}
/////////////////////////////////////////////////////////////////////////////
void CISDNConnection::DoListen(const CString &sNumber)
{
	m_sOwnNumber = sNumber;
	CWK_String str(sNumber);
	m_pCapi->ListenRequest( str
//							 byController=1,
//							 dwServiceMask=0x0000020,
//							 dwInfoMask=0
						);
	// Timeout fuer Verbindung ohne Datenverkehr setzen
	m_pCapi->SetConnectionTimeout(30000);	// OOPS
}
/////////////////////////////////////////////////////////////////////////////
void CISDNConnection::CloseFailed(LPCTSTR pCalledParty )
{
	WK_TRACE(_T("CloseFailed from '%s'\n"), pCalledParty);
	m_ConnectState = CONNECT_OPEN;
	m_sRemoteNumber = _T("");
	// numBChannels etc? OOPS
}
/////////////////////////////////////////////////////////////////////////////
void CISDNConnection::NrBChannelsChanged(WORD wBChannels)
{
	m_wNrBChannels = wBChannels;
	WK_STAT_LOG(_T("Reset"),m_wNrBChannels,_T("NrBChannels"));
	CWK_Profile wkp;
	wkp.WriteInt(theApp.GetProfileSection(), szChannelsConnected, GetNumBChannels());

//	CWK_RunTimeError e(theApp.GetApplicationId(), 
	CWK_RunTimeError e(REL_MESSAGE, 
					   RTE_ISDN_BCHANNEL, 
					   NULL,
					   0,
					   m_wNrBChannels);
	e.Send();

	if (m_ConnectState == CONNECT_CLOSING && m_wNrBChannels == 0)
	{
		TRACE(_T("NrBChannelsChanged(%s): %d\n"), NameOfEnum(m_ConnectState), GetTickCount());
		m_ConnectState = CONNECT_CLOSED;
	}
}
/////////////////////////////////////////////////////////////////////////////
/*@TOPIC Prüfung eines eingehenden Anrufs|ISDNUnit,IncomingCallCheck
Ein eingehender Anruf enthaelt folgende Information:
<nl>Berechtigungsstufe
<nl>Password (der Berechtigungsstufe)
<nl>Name der anrufenden Station

Es gelten folgende Bedingungen:
<nl>die Berechtigungsstufe <b>muss<nb> bekannt sein
<nl>das mitgelieferte Passwort stimmt mit dem lokalen überein,
	<nl>ODER das lokale Passwort ist leer.
<nl>der anrufende Stationsname ist bekannt
	<nl>ODER das lokale Passwort ist leer.

Dies erlaubt offene Rechner, die noch nicht mal die anrufenden Stationen beim
Namen kennen. Einziger gemeinsamer Nenner ist der Name der Berechtigunsstufe,
die kein Passwort hat. Wenn auf der anrufenden Station ein Passwort ist,
macht das gar nix.
*/
// called from ReceiveCipcData
void CISDNConnection::OnCipcPipeRequest(const CConnectionRecord &connectIn)
{
	if (m_bDoCloseCapi || m_bDoResetCapi)
	{
		return;
	}
	Lock();

	DWORD dw1,dw2,dw3,dw4;
	CSecID idFromOne,idFromTwo;
	CIPCType cipcTypeOut =CIPC_NONE;
	CConnectionRecord connectRecord(connectIn);

	CString sMSN;
	if (m_bNoNumbersFromCapi)	// if there are not transmitted numbers, use
	{							// the remote hostname to identify the 
								// reconnecting pipe.
		connectRecord.SetFieldValue(CRF_MSN, connectRecord.GetSourceHost());
	}
	else if (!connectRecord.GetFieldValue(CRF_MSN,sMSN))
	{
		sMSN = _T("0");
	}
	if (   sMSN == _T("0")
		|| sMSN == _T("A"))
	{
		sMSN = GetRemoteNumber();
		if (!sMSN.IsEmpty())
		{
			connectRecord.SetFieldValue(CRF_MSN,sMSN);
		}
	}

	CString sCompress;
	CString sBitrate;
	sBitrate.Format(_T("%d"),GetNumBChannels()*64*1024);
	connectRecord.SetFieldValue(CRF_BITRATE, sBitrate);

	int iNumRead = _stscanf(connectRecord.GetInputShm(),_T("%d %d %d %d"), &dw1, &dw2, &dw3, &dw4);
	if (iNumRead==2)
	{
		cipcTypeOut = (CIPCType)dw1;
		idFromOne = dw2;
	}
	else if (iNumRead==3)
	{
		cipcTypeOut = (CIPCType)dw1;
		idFromOne = dw2;
		idFromTwo = dw3;
	}
	else
	{
		WK_TRACE_ERROR(_T("ID format error found %d args in '%s'\n"),iNumRead, (LPCTSTR)connectRecord.GetInputShm());
	}


	CIPCFetchResult *pSendFetchResult= NULL;
	BOOL bFetch = TRUE;

	CString sDestination = connectIn.GetDestinationHost();
	int ix = -1;
	ix = sDestination.Find(ROUTING_CHAR);
	if (ix != -1) 
	{
		// is routing allowed???
		CWK_Dongle dongle;
		if (dongle.AllowRouting())
		{
			CString sMyName;
			CString sNextHost;
			sMyName = sDestination.Left(ix-1);
			sNextHost = sDestination.Mid(ix+1);
			WK_TRACE(_T("NextDestination is %s\n"),sNextHost);	// OBSOLETE
			connectRecord.SetDestinationHost(sNextHost);
			connectRecord.SetSourceHost(connectIn.GetSourceHost());
			sDestination = sMyName;
		}
		else
		{
			CString sMessage;
			sMessage.LoadString(IDS_NO_ROUTING);
			// no outgoing calls at all!
			WK_TRACE(_T("Routing Call denied from ") + connectIn.GetSourceHost());
			pSendFetchResult = new CIPCFetchResult(
					_T("No pipe created"),
					CIPC_ERROR_UNABLE_TO_CONNECT, 6,
					sMessage,
					0,
					CSecID(SECID_NO_ID)
#ifdef _UNICODE
					, CODEPAGE_UNICODE	// it is our own created fetchresult
#endif
					);
			bFetch = FALSE;
		}
		// NOT YET collect path in SourceHost
	}
	else
	{
		connectRecord.SetDestinationHost(LOCAL_LOOP_BACK); // NOT YET routing
	}
	
	CIPCPipeIST* pNewPipeOne = NULL;
	CIPCPipeIST* pNewPipeTwo = NULL;
	CString sShmName;
	CSecID idAssigned;
	WORD wNewPipeOneNr = 0;
	WORD wNewPipeTwoNr = 0;
	// UNNUSED DWORD dwOptionsReturn = 0;


	BOOL bAccessGranted = FALSE;
	CIPCError denyError = CIPC_ERROR_OKAY;
	int iDenyErrorCode = 0;
	CString sDenyErrorMessage;
	sDenyErrorMessage.LoadString(IDS_INVALID_PERMISSION_ACCESS_DENIED);

	if (bFetch)
	{
	// Hostliste ist beim DV in einem Unterzweig!!!
#ifdef _DTS
		CWK_Profile wkp(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, _T("DVRT\\DV"), _T(""));
#else
		CWK_Profile wkp;
#endif
		CHostArray hosts;
		hosts.Load(wkp);
		CString sOwnName = hosts.GetLocalHostName();

#ifdef NO_PERMISSION_CHECK
		bAccessGranted = TRUE;
#else
		// NEW 100597 check host and permission

		// a) check for correct destination
		// NOT YET cascading addresses
		// NOT YET can't compare 77==Hermelin.wk.de, SIGH

		// In words:
		// check for destination == OwnHostName NOT YET
		// check for a known permission
		// check password for permission
		// check host
		// UNUSED if (sDestination==sOwnName) { can't do that !!!
		{
			// b)
			TRACE(_T("DEBUG Incoming permission %s[%s]\n"),
				connectIn.GetPermission(),
				connectIn.GetPassword() 
				);
			CPermissionArray permissions;
			permissions.Load(wkp);
			const CPermission *pPermission;
			pPermission = permissions.CheckPermission(connectIn);
			if (pPermission)
			{
					bAccessGranted = hosts.CheckHost(connectIn.GetSourceHost(), pPermission);
					if (bAccessGranted==FALSE)
					{
						WK_TRACE(_T("Invalid access host '%s', permission '%s'\n"),
								connectIn.GetSourceHost(), 
								connectIn.GetPermission()
							);
						denyError = CIPC_ERROR_INVALID_PERMISSIONS;
						iDenyErrorCode = 2;
						sDenyErrorMessage.LoadString(IDS_INVALID_PERMISSION_ACCESS_DENIED);
					}
					else
					{
						// fine, access granted 
						denyError = CIPC_ERROR_OKAY;
						iDenyErrorCode = 0;
						sDenyErrorMessage = _T("");
					}
			}
			else
			{
				denyError = CIPC_ERROR_INVALID_PERMISSIONS;
				iDenyErrorCode = 1;
				sDenyErrorMessage.LoadString(IDS_INVALID_PERMISSION_ACCESS_DENIED);
				// an invalid permission
				WK_TRACE(_T("Invalid access unknown permission or wrong password for '%s'\n"),
							connectIn.GetPermission()
							);
			}
		}
		/* <UNUSED> else {
			// destination does not match own host name
			denyError = CIPC_ERROR_INVALID_HOST;
			WK_TRACE(_T("'%s' does not match own hostname '%s'\n"),
						(const char *)sDestination,
						(const char *)hosts.GetLocalHostName()
						);
		}
		</UNUSED>
		*/
		// end of host/permission check
#endif
		if (bAccessGranted) 
		{
			CIPCFetch fetch;
			CIPCFetchResult fetchResult;
			switch ( cipcTypeOut )	
			{
				case CIPC_INPUT_SERVER:
					// Add new input pipe to ISDN connection
					wNewPipeOneNr = CIPCPipeIST::GetNextGlobalID();
					fetchResult = fetch.FetchInput(connectRecord);
					if (fetchResult.GetShmName().GetLength())
					{
						pNewPipeOne = new CIPCPipeIST(cipcTypeOut,
														this,
														fetchResult.GetShmName(),
														CIPC_SLAVE,
														connectRecord.GetOptions(),
														wNewPipeOneNr,
														0	// request counter
														);
						m_InputPipesServer.SafeAdd(pNewPipeOne);
						pNewPipeOne->StartThread(TRUE);
						if (theApp.TracePipes())
						{
							WK_TRACE(_T("%s Server '%s'\n"), _T(__FUNCTION__), fetchResult.GetShmName());
						}
					}
					break;
				case CIPC_OUTPUT_SERVER:
					// NOT YET ueberpruefen der angeforderten ServerOutputOptions
					// Add new output pipe to ISDN connection
					wNewPipeOneNr = CIPCPipeIST::GetNextGlobalID();
					fetchResult = fetch.FetchOutput(connectRecord);
					// OOPS, dwOptionsReturn immer 0, daher HACK auf Requested
					// WK_TRACE(_T("options %d returned %d\n"),connectRecord.GetOptions(),dwOptionsReturn);
					if (fetchResult.GetShmName().GetLength())
					{
						pNewPipeOne = new CIPCPipeIST(cipcTypeOut,
													this,
													fetchResult.GetShmName(),
													CIPC_SLAVE,
													fetchResult.GetOptions(),
													wNewPipeOneNr,
													0	// request counter
													);
						m_OutputPipesServer.SafeAdd(pNewPipeOne);
						pNewPipeOne->StartThread(TRUE);
						if (theApp.TracePipes())
						{
							WK_TRACE(_T("%s Server '%s'\n"), _T(__FUNCTION__), fetchResult.GetShmName());
						}
					}
					break;
				case CIPC_DATABASE_SERVER:
					// NOT YET ueberpruefen der angeforderten ServerOutputOptions
					// Add new database pipe to ISDN connection
					wNewPipeOneNr = CIPCPipeIST::GetNextGlobalID();
					fetchResult = fetch.FetchDatabase(connectRecord);
					if (fetchResult.GetShmName().GetLength())
					{
						pNewPipeOne = new CIPCPipeIST(cipcTypeOut, this,
													fetchResult.GetShmName(),
													CIPC_SLAVE,
													connectRecord.GetOptions(),
													wNewPipeOneNr,
												0	// request counter
													);
						m_DataBasePipesServer.SafeAdd(pNewPipeOne);
						pNewPipeOne->StartThread(TRUE);
						if (theApp.TracePipes())
						{
							WK_TRACE(_T("%s Server '%s'\n"), _T(__FUNCTION__), fetchResult.GetShmName());
						}
					}
					break;
				case CIPC_AUDIO_SERVER:
					// NOT YET ueberpruefen der angeforderten ServerOutputOptions
					// Add new audio pipe to ISDN connection
					wNewPipeOneNr = CIPCPipeIST::GetNextGlobalID();
					fetchResult = fetch.FetchAudio(connectRecord);
					// OOPS, dwOptionsReturn immer 0, daher HACK auf Requested
					// WK_TRACE(_T("options %d returned %d\n"),connectRecord.GetOptions(),dwOptionsReturn);
					if (fetchResult.GetShmName().GetLength())
					{
						pNewPipeOne = new CIPCPipeIST(cipcTypeOut,
													this,
													fetchResult.GetShmName(),
													CIPC_SLAVE,
													fetchResult.GetOptions(),
													wNewPipeOneNr,
													0	// request counter
													);
						m_AudioPipesServer.SafeAdd(pNewPipeOne);
						pNewPipeOne->StartThread(TRUE);
						if (theApp.TracePipes())
						{
							WK_TRACE(_T("%s Server '%s'\n"), _T(__FUNCTION__), fetchResult.GetShmName());
						}
					}
					break;
				case CIPC_ALARM_CLIENT:
					{
						LPCTSTR strRemoteNumber = GetRemoteNumber();
						int i, nDigits=0, nLen = GetRemoteNumber().GetLength();
						for (i=0; i<nLen; i++)
						{
							if (_istdigit(strRemoteNumber[i]))
							{
								nDigits++;
							}
							if (_istalpha(strRemoteNumber[i]))
							{
								nDigits = 0;
								break;
							}
						}
						if (nDigits>0)	// does the remote number contain a phone number
						{
							connectRecord.SetFieldValue(CRF_MSN, GetRemoteNumber());
						}
						WK_TRACE(_T("CIPC_ALARM_CLIENT <%s> idFromOne %x idFromTwo %x\n"),
							(LPCTSTR)connectRecord.GetSourceHost(),
							idFromOne.GetID(),idFromTwo.GetID());
						// NEW 080597
						if (connectRecord.GetDestinationHost()==LOCAL_LOOP_BACK)
						{
#ifdef _UNICODE
							if (WK_IS_RUNNING(WK_APP_NAME_IDIP_CLIENT)==FALSE) 
#else
							if (WK_IS_RUNNING(WK_APP_NAME_EXPLORER)==FALSE) 
#endif
							{
								if (CIPCServerControlClientSide::StartVision()) 
								{
									Sleep(100);
								} 
								else 
								{
									// failed to start
								}
							}
						}
						//
						// create client connections first
						// the created shared memory names are used in the FetchAlarmConnection

						// Add new input pipe to ISDN connection
						wNewPipeOneNr = CIPCPipeIST::GetNextGlobalID();;
						sShmName.Format(_T("IsdnPipeInput%d"),wNewPipeOneNr);
						pNewPipeOne = new CIPCPipeIST(CIPC_INPUT_CLIENT,
							this,
							(LPCTSTR)sShmName,
							CIPC_MASTER,
							connectRecord.GetOptions(),
							wNewPipeOneNr,
							0,	// request counter
							TRUE	// alarm connection
							);
						m_InputPipesClient.SafeAdd(pNewPipeOne);
						pNewPipeOne->StartThread(TRUE);

						if (theApp.TracePipes())
						{
							WK_TRACE(_T("%s '%s'\n"), _T(__FUNCTION__), sShmName);
						}
						// Add new output pipe to ISDN connection
						wNewPipeTwoNr = CIPCPipeIST::GetNextGlobalID();
						sShmName.Format(_T("IsdnPipeOutput%d"),wNewPipeTwoNr);
						pNewPipeTwo = new CIPCPipeIST(CIPC_OUTPUT_CLIENT,
							this,
							sShmName,
							CIPC_MASTER,
							connectRecord.GetOptions(),
							wNewPipeTwoNr,
							0,	// request counter
							TRUE	// alarm connection
							);
						m_OutputPipesClient.SafeAdd(pNewPipeTwo);
						pNewPipeTwo->StartThread(TRUE);
						if (theApp.TracePipes())
						{
							WK_TRACE(_T("%s '%s'\n"), _T(__FUNCTION__), sShmName);
						}

						connectRecord.SetInputShm(pNewPipeOne->GetShmName());
						connectRecord.SetOutputShm(pNewPipeTwo->GetShmName());
						Sleep(250);	// OOPS test, there seem to be a timing problem
						// TODO! Check OnReadChannelFound() nur DoRequestConnection, wenn Master !?

						// TODO! RKE: Audio Client Alarm Connection?
						fetchResult = fetch.FetchAlarmConnection(connectRecord);

						if (fetchResult.IsOkay()) 
						{
							// confirm is below
							WK_TRACE(_T("CipcPipeRequest() Alarm Request %u successful\n"),
								connectRecord.GetOptions());
							Sleep(50);	// OOPS test, there seems to be a timing problem
						}
						else
						{
							CIPCFetchResult dummyFetchResult(
								_T(""),
								CIPC_ERROR_UNABLE_TO_CONNECT, 0,	// NOT YET
								_T("Alarm connection failed"),
								0,
								SECID_NO_ID
#ifdef _UNICODE
								, CODEPAGE_UNICODE	// it is our own created fetchresult
#endif
								);
							fetchResult = dummyFetchResult;
							WK_TRACE_ERROR(_T("CISDNConnection::CipcPipeRequest() FetchAlarm failed\n"));				// failed
							RemovePipe(pNewPipeOne);
							pNewPipeOne = NULL;
							RemovePipe(pNewPipeTwo);
							pNewPipeTwo = NULL;
						}
					}
					break;
				default:
					WK_TRACE_ERROR(_T("CipcPipeConfirm() WRONG TYPE %u\n"), cipcTypeOut);
					break;
			}	// end of switch cipcType

			pSendFetchResult = new CIPCFetchResult(
				sOwnName,	// hidden in shmName
				fetchResult.GetError(),
				fetchResult.GetErrorCode(),
				fetchResult.GetErrorMessage(),
				fetchResult.GetOptions(),
				fetchResult.GetAssignedID()
#ifdef _UNICODE
				, fetchResult.GetCodePage()
#endif
				);

		} 
		else 
		{
			// acess denied fill fetchResult
			pSendFetchResult = new CIPCFetchResult(sOwnName,	// hidden in shmName
													denyError,
													iDenyErrorCode,
													sDenyErrorMessage,
													0,
													SECID_NO_ID
#ifdef _UNICODE
													, CODEPAGE_UNICODE	// it is our own created fetchresult
#endif
													);
		}
	}

	DWORD dwIDAnswer1, dwIDAnswer2;

	dwIDAnswer1 = (((DWORD)(idFromOne.GetID() & 255))<<16);
	dwIDAnswer2 = (((DWORD)(idFromTwo.GetID() & 255))<<16);

	// add answer id in the low byte
	// if there are no answer ids, the FetchResult is != CIPC_ERROR_OKAY
	if (pNewPipeOne)
	{
		dwIDAnswer1 |=  ((DWORD)pNewPipeOne->GetIDByte());

	}
	if (pNewPipeTwo)
	{
		dwIDAnswer2 |=  ((DWORD)pNewPipeTwo->GetIDByte());
	}

	// Confirmation der requested Input/Output/DataBase pipes
	if (pNewPipeOne)
	{
		m_bThereWerePipesBefore = TRUE;

		// Setzen der Remote ID der Pipe
		pNewPipeOne->SetRemoteID(idFromOne);
		SetRemoteHostName(connectIn.GetSourceHost());
		// Sonderfall ALARM
		if (pNewPipeTwo) {
			// Setzen der Remote ID der Pipe
			pNewPipeTwo->SetRemoteID(idFromTwo);
		}
	}

	WK_TRACE(_T("Type:%s CallState:%d\n"), NameOfEnum(cipcTypeOut), GetCallState());
							
	if (pSendFetchResult && (!pSendFetchResult->IsOkay()))
	{
		// Virtuellen Reject-Alarm auslösen...
		CString sMsg;
		sMsg.Format(IDS_REJECT_OUTGOING_CALL, m_sRemoteNumber);
		CWK_RunTimeError runtimeError(REL_MESSAGE, RTE_REJECT, sMsg,0,1);
		runtimeError.Send();
		Sleep(50);
		CWK_RunTimeError runtimeError2(REL_MESSAGE, RTE_REJECT,NULL ,0,0);
		runtimeError2.Send();
	}

	// same answer format for all cases
	CIPCExtraMemory *pResultBubble = pSendFetchResult->CreateBubble(m_pControl);
	CIPCCmdRecord *pCmdRecord = new CIPCCmdRecord(
								pResultBubble,
								SRV_CONTROL_CONFIRM_PIPE,
								(DWORD) cipcTypeOut,
								dwIDAnswer1,
								dwIDAnswer2
								);


	CCapiQueueRecord* pNewRecord = new CCapiQueueRecord;
	pNewRecord->FillFromCmdRecord(SECID_NO_ID,
									SECID_NO_ID,
									cipcTypeOut,	// OOPS unused?
									*pCmdRecord,
									m_bCompressed
									);
	
	m_pCipcSendQ->PrependToSendQueue(pNewRecord);

	WK_DELETE(pCmdRecord);
	WK_DELETE(pResultBubble);
	WK_DELETE(pSendFetchResult);	// NEW 14.1.99

	Unlock();
}	// end of OnCipcPipeRequest
/////////////////////////////////////////////////////////////////////////////
void CISDNConnection::OnCipcPipeConfirm(CIPCType cipcRequestType,
										CSecID idLocal,
										CSecID idAnswer,
										CSecID idLocal2,
										CSecID idAnswer2,
										CIPCFetchResult &remoteFetchResult
										)
{
	if (m_bDoCloseCapi || m_bDoResetCapi)
	{
		return;
	}
	Lock();

	TRACE(_T("OnCipcPipeConfirm remote %x local %x remote result %s[%d]\n"), 
					idAnswer.GetID(),
					idLocal.GetID(),
					NameOfEnum(remoteFetchResult.GetError()),
					remoteFetchResult.GetError()
					);
#if DUMP_PIPE_STATE
	DumpPipeState();
	WK_TRACE(_T("Removing ....\n"));
#endif

	if (remoteFetchResult.GetError()!=CIPC_ERROR_OKAY)
	{
		// needs one or more removes!
		switch (cipcRequestType) {
			case CIPC_INPUT_SERVER:
			case CIPC_OUTPUT_SERVER:
			case CIPC_DATABASE_SERVER:
			case CIPC_AUDIO_SERVER:
				OnCipcPipeRemove(idLocal);
				break;
			case CIPC_ALARM_CLIENT:
				OnCipcPipeRemove(idLocal);
				OnCipcPipeRemove(idLocal2);
				break;
			default:
				WK_TRACE_ERROR(_T("Invalid cipcType in PipeConfirm %d\n"),cipcRequestType);
				PanicClose(_T("CIPCType in remoteFetchResult"));
		}

// gf 2004/02/18 check obsolet, wird in der Funktion geprueft
//		if ( m_pControl->GetIPCState() == CIPC_STATE_CONNECTED )
		{
			// NOT YET use RequestCounter
			m_pControl->DoNewIndicateError(SRV_CONTROL_REQUEST_INPUT_CONNECTION,
											SECID_NO_ID, 
											remoteFetchResult.GetError(),
											remoteFetchResult.GetErrorCode(),
											remoteFetchResult.GetErrorMessage()
			);
		}
#if DUMP_PIPE_STATE
		WK_TRACE(_T("Removing (not OKAY) DONE\n"));
		DumpPipeState();
#endif
		SetRemoteHostName(_T(""));
	}
	else
	{ // now the okay part
		// hidden name
		SetRemoteHostName(remoteFetchResult.GetShmName());
		BOOL bDoConfirm = TRUE;
		// NOT YET check request count too
		if ( m_pControl->GetIPCState() != CIPC_STATE_CONNECTED )
		{
			WK_TRACE(_T("CipcPipeConfirm but no waiting client\n"));
			bDoConfirm=FALSE;
			// the corresponding local pipe can be removed
		}

		CIPCPipeISDN* pGetPipe = NULL;
		CIPCPipeISDN* pGetPipeTwo = NULL;
		int i = 0;
		BOOL bFound = FALSE;
		switch ( cipcRequestType )
		{
			case CIPC_INPUT_SERVER:
				for ( i=0 ; (!bFound && i < m_InputPipesClient.GetSize() ) ; i++ )
				{
					pGetPipe = m_InputPipesClient.GetAt(i);
					if ( pGetPipe->GetID() == idLocal)
					{
						bFound = TRUE;
						if (bDoConfirm) {
							pGetPipe->SetRemoteID(idAnswer);
							m_pControl->DoConfirmInputConnection(pGetPipe->GetID(),
																 pGetPipe->GetCIPC()->GetShmName(),
																 remoteFetchResult.GetErrorMessage(),
																 remoteFetchResult.GetOptions()
#ifdef _UNICODE
																 , remoteFetchResult.GetCodePage()
#endif
																 );
							WK_TRACE(_T("CipcPipeConfirm() ConfirmInputConnection '%s'\n"),
																			pGetPipe->GetCIPC()->GetShmName());
						}
					}
				}
				break;
			case CIPC_OUTPUT_SERVER:
				for ( i=0 ; (!bFound && i < m_OutputPipesClient.GetSize() ) ; i++ )
				{
					pGetPipe = m_OutputPipesClient.GetAt(i);
					if ( pGetPipe->GetID() == idLocal)
					{
						bFound = TRUE;
						if ( bDoConfirm)
						{
							pGetPipe->SetRemoteID(idAnswer);
							CString sVersion = remoteFetchResult.GetErrorMessage();
							if (sVersion.IsEmpty())
							{
								sVersion = _T("unknown");
							}
							m_pControl->DoConfirmOutputConnection( pGetPipe->GetID(),
																	pGetPipe->GetCIPC()->GetShmName(),
																	remoteFetchResult.GetErrorMessage(),
																	remoteFetchResult.GetOptions()
#ifdef _UNICODE
																	, remoteFetchResult.GetCodePage()
#endif
																	);
							WK_TRACE(_T("CipcPipeConfirm() ConfirmOutputConnection '%s'\n"),
													pGetPipe->GetCIPC()->GetShmName() );
						}
					}
				}
				break;
			case CIPC_AUDIO_SERVER:
				for ( i=0 ; (!bFound && i < m_AudioPipesClient.GetSize() ) ; i++ )
				{
					pGetPipe = m_AudioPipesClient.GetAt(i);
					if ( pGetPipe->GetID() == idLocal)
					{
						bFound = TRUE;
						if ( bDoConfirm)
						{
							pGetPipe->SetRemoteID(idAnswer);
							CString sVersion = remoteFetchResult.GetErrorMessage();
							if (sVersion.IsEmpty())
							{
								sVersion = _T("unknown");
							}
							m_pControl->DoConfirmAudioConnection(pGetPipe->GetID(),
																	pGetPipe->GetCIPC()->GetShmName(),
																	remoteFetchResult.GetErrorMessage(),
																	remoteFetchResult.GetOptions()
#ifdef _UNICODE
																	, remoteFetchResult.GetCodePage()
#endif
																	);
							WK_TRACE(_T("CipcPipeConfirm() ConfirmAudioConnection '%s'\n"),
													pGetPipe->GetCIPC()->GetShmName() );
						}
					}
				}
				break;
			case CIPC_DATABASE_SERVER:
				for ( i=0 ; (!bFound && i < m_DataBasePipesClient.GetSize() ) ; i++ )
				{
					pGetPipe = m_DataBasePipesClient.GetAt(i);
					if ( pGetPipe->GetID() == idLocal )
					{
						bFound = TRUE;
						if ( bDoConfirm )
						{
							pGetPipe->SetRemoteID(idAnswer);
							m_pControl->DoConfirmDataBaseConnection(pGetPipe->GetID(),
																	pGetPipe->GetCIPC()->GetShmName(),
																	remoteFetchResult.GetErrorMessage(),
																	pGetPipe->GetOptions()
#ifdef _UNICODE
																	, remoteFetchResult.GetCodePage()
#endif
																	);
							WK_TRACE(_T("CipcPipeConfirm() ConfirmDataBaseConnection '%s' %u\n"),
																			pGetPipe->GetCIPC()->GetShmName(),
																			pGetPipe->GetOptions() );
						}

					}
				}
				if ( bFound )
				{
				}
				else
				{
					WK_TRACE_WARNING(_T("CipcPipeConfirm() ConfirmDataBaseConnection DATABASE_CLIENT %x NOT FOUND\n"), idLocal.GetID() );
				}
				break;
			case CIPC_ALARM_CLIENT:
				// ALARM input ueberpruefen
				for ( i=0 ; (!bFound && i < m_InputPipesServer.GetSize() ) ; i++ )
				{
					pGetPipe = m_InputPipesServer.GetAt(i);
					if ( pGetPipe->GetID() == idLocal )
					{
						bFound = TRUE;
					}
				}
				if ( bFound )
				{
					if ( m_pControl->GetIPCState() != CIPC_STATE_CONNECTED )
					{
						WK_TRACE_ERROR(_T("CipcPipeConfirm() ConfirmAlarmConnection '%s' CONTROL NOT CONNECTED\n"),
																pGetPipe->GetCIPC()->GetShmName() );
					}
					else
					{
						// ALARM output ueberpruefen
						bFound = FALSE;
						for ( i=0 ; (!bFound && i < m_OutputPipesServer.GetSize() ) ; i++ )
						{
							pGetPipeTwo = m_OutputPipesServer.GetAt(i);
							if ( pGetPipeTwo && (pGetPipeTwo->GetID() == idLocal2) )
							{
								bFound = TRUE;
							}
						}
						if ( bFound )
						{
							if ( m_pControl->GetIPCState() != CIPC_STATE_CONNECTED )
							{
								WK_TRACE_ERROR(_T("CipcPipeConfirm() ConfirmAlarmConnection '%s' CONTROL NOT CONNECTED\n"),
															pGetPipeTwo->GetCIPC()->GetShmName() );
							}
							else
							{
								pGetPipe->SetRemoteID(idAnswer);
								pGetPipeTwo->SetRemoteID(idAnswer2);

								DWORD dwBitrate = GetNumBChannels()*64*1024;
								CString sBitrate;
								sBitrate.Format(_T("%d"),dwBitrate);


								m_pControl->DoConfirmAlarmConnection(pGetPipe->GetCIPC()->GetShmName(),
																	 pGetPipeTwo->GetCIPC()->GetShmName(),
																	 sBitrate,
																	 remoteFetchResult.GetServerVersion(),
																	 remoteFetchResult.GetOptions()
#ifdef _UNICODE
																	, remoteFetchResult.GetCodePage()
#endif
																	 );
								WK_TRACE(_T("CipcPipeConfirm() ConfirmAlarmConnection '%s' '%s'\n"),
																				pGetPipe->GetCIPC()->GetShmName(),
																				pGetPipeTwo->GetCIPC()->GetShmName() );
							}
						}
						else
						{
							WK_TRACE_WARNING(_T("CipcPipeConfirm() ConfirmAlarmConnection OUTPUT_SERVER %x NOT FOUND\n"), idLocal2.GetID() );
						}
					}
				}
				else
				{
					WK_TRACE_WARNING(_T("CipcPipeConfirm() ConfirmAlarmConnection INPUT_SERVER %x NOT FOUND\n"), idLocal.GetID() );
				}
				break;
			default:
				WK_TRACE_ERROR(_T("OnCipcPipeConfirm() WRONG TYPE %d\n") , cipcRequestType);
				break;
		}	// end of switch(cipcType)
		if (bFound)
		{
			m_typeConnectRequest = CIPC_NONE;
			if (cipcRequestType != CIPC_ALARM_CLIENT)
			{
				// safety check for cancelled clients
				bDoConfirm = (m_pControl->GetIPCState() == CIPC_STATE_CONNECTED);
				if (bDoConfirm == FALSE)
				{
					// pipe without client!
					WK_TRACE(_T("Remove pipe without client; %s 0x%x\n"),
								NameOfEnum(pGetPipe->GetCIPCType()), pGetPipe->GetID().GetID());
					RemoveMe(pGetPipe);
				}
				else
				{
					// set timeout for client connect to 5s
					pGetPipe->SetNotConnectedTimeout(5*1000);
				}
			}
		}
		else
		{
			WK_TRACE_WARNING(_T("PipeConfirm %s pipe %x not found\n"),
					NameOfEnum(cipcRequestType), 
					idLocal.GetID() 
					);
		}

#if DUMP_PIPE_STATE
	WK_TRACE(_T("Removing DONE\n"));
	DumpPipeState();
#endif
	}	// end of okay part

	Unlock();
}	// end of OnCipcPipeConfirm
/////////////////////////////////////////////////////////////////////////////
// called if a CIPC_REMOVE is received from CAPI,
// that means it is the main thread --> no DelayedDelete
// called from PipeConfirm
void CISDNConnection::OnCipcPipeRemove(CSecID id) 
{
	if (m_bDoCloseCapi || m_bDoResetCapi)
	{
		return;
	}

	Lock();

	m_pCachedPipe=NULL;	// always reset

	CIPCPipeISDN* pPipe = NULL;
	BOOL bRemoved=FALSE;

#if 0
	WK_TRACE(_T("TimeoutTest HACK OOPS\n"));
	pPipe = (CIPCPipeISDN *)0x1243;	// OOPS TEST HACK
	bRemoved=TRUE;
#endif

	/////// OUTPUT
	if (pPipe==NULL)
	{
		pPipe = m_InputPipesServer.GetPipeByID(id);
		if (pPipe)
		{
			m_InputPipesServer.Remove(pPipe);
			WK_DELETE(pPipe);
			bRemoved=TRUE;
		}
	}
	if (bRemoved==FALSE)
	{
		pPipe = m_InputPipesClient.GetPipeByID(id);
		if (pPipe)
		{
			m_InputPipesClient.Remove(pPipe);
			WK_DELETE(pPipe);
			bRemoved=TRUE;
		}
	}
	/////// OUTPUT
	if (bRemoved==FALSE)
	{
		pPipe = m_OutputPipesServer.GetPipeByID(id);
		if (pPipe)
		{
			m_OutputPipesServer.Remove(pPipe);
			WK_DELETE(pPipe);
			bRemoved=TRUE;
		}
	}
	if (bRemoved==FALSE)
	{
		pPipe = m_OutputPipesClient.GetPipeByID(id);
		if (pPipe)
		{
			m_OutputPipesClient.Remove(pPipe);
			WK_DELETE(pPipe);
			bRemoved=TRUE;
		}
	}
	//////// DATABASE
	if (bRemoved==FALSE)
	{
		pPipe = m_DataBasePipesServer.GetPipeByID(id);
		if (pPipe)
		{
			m_DataBasePipesServer.Remove(pPipe);
			WK_DELETE(pPipe);
			bRemoved=TRUE;
		}
	}
	if (bRemoved==FALSE)
	{
		pPipe = m_DataBasePipesClient.GetPipeByID(id);
		if (pPipe) {
			m_DataBasePipesClient.Remove(pPipe);
			WK_DELETE(pPipe);
			bRemoved=TRUE;
		}
	}

	/////// AUDIO
	if (bRemoved==FALSE)
	{
		pPipe = m_AudioPipesServer.GetPipeByID(id);
		if (pPipe)
		{
			m_AudioPipesServer.Remove(pPipe);
			WK_DELETE(pPipe);
			bRemoved=TRUE;
		}
	}
	if (bRemoved==FALSE)
	{
		pPipe = m_AudioPipesClient.GetPipeByID(id);
		if (pPipe)
		{
			m_AudioPipesClient.Remove(pPipe);
			WK_DELETE(pPipe);
			bRemoved=TRUE;
		}
	}
	if (bRemoved==FALSE)
	{
		WK_TRACE(_T("Remove Pipe %x not found\n"),id.GetID());
		PanicClose(_T("RemovePipe"));
	}

	Unlock();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CISDNConnection::PushCipcCmd(CCapiQueueRecord* pRecord)
{
	if (m_bDoCloseCapi || m_bDoResetCapi)
	{
		return FALSE;
	}
	DWORD dwCmd;

	BOOL bReturn = FALSE;
	int iCount = 0;
	int i=0;
	CIPCPipeISDN* pPipe = NULL;
	CIPCPipeIST* pPipeIST = NULL;

	BOOL bFound = FALSE;

	BYTE byCmdFrom = CIPC_NONE;
	CSecID idFrom;
	CSecID idTo;

	DWORD dwParam1;		
	DWORD dwParam2;		
	DWORD dwParam3;		
	DWORD dwParam4;
	DWORD dwExtraMemoryLen	= 0;
	BYTE* pExtraMem			= NULL;

	pRecord->GetCipcCmd(byCmdFrom,
							  idFrom,
							  idTo,
							  dwExtraMemoryLen,
							  pExtraMem,
							  dwCmd,
							  dwParam1,
							  dwParam2,
							  dwParam3,
							  dwParam4
							  );

	if (dwCmd == CIPC_BASE_REQ_CONNECTION)
	{
		if (LOWORD(dwParam1))
		{
#ifdef _DTS
			CWK_Profile wkp(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE,_T("DVRT\\DV"),_T(""));
#else
			CWK_Profile wkp;
#endif
			m_bCompressed = LOWORD(dwParam1);
			WK_TRACE(_T("Receiving Compression Request %d\n"), m_bCompressed);
		}
	}
	if (dwCmd == CIPC_BASE_CONF_CONNECTION)
	{
		if (LOWORD(dwParam1))
		{
#ifdef _DTS
			CWK_Profile wkp(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE,_T("DVRT\\DV"),_T(""));
#else
			CWK_Profile wkp;
#endif
			m_bCompressed = LOWORD(dwParam1);
			WK_TRACE(_T("Receiving Confirmed Compression Request %d\n"), m_bCompressed);
		}
	}

	if (m_bCompressed)
	{
		int nVal = CCapiQueueRecord::gm_avgCompression.GetNoOfAvgValues();
		if (nVal == -1)
		{
			CCapiQueueRecord::gm_avgCompression.SetNoOfAvgValues(200);
		}
	}
	
	// TRACE(_T("CIPCCmd %d/%s\n"),dwCmd,CIPC::NameOfCmd(dwCmd));
	if (dwCmd==SRV_CONTROL_REQUEST_REMOVE_PIPE)
	{ 
		TRACE(_T("CIPCCmd SRV_CONTROL_REQUEST_REMOVE_PIPE %08x\n"), dwParam1);
		CSecID id = dwParam1;
		OnCipcPipeRemove(id);
		return TRUE;	// <<< EXIT >>>
	}
	else if (dwCmd==SRV_CONTROL_CONFIRM_PIPE)
	{ 
		if (pExtraMem) 
		{
			BOOL bRet = FALSE;
			CIPCType ct = (CIPCType) dwParam1;
			DWORD dwAnswerIDs1 = dwParam2;
			DWORD dwAnswerIDs2 = dwParam3;
			// unpack ids
			CSecID idLocal,idAnswer;
			idLocal.Set(0x1000,(WORD)((dwAnswerIDs1 >> 16)&0xffff));
			idAnswer.Set(0x1000,(WORD)((dwAnswerIDs1&0xffff)));

			CSecID idLocal2,idAnswer2;
			idLocal2.Set(0x1000,(WORD)((dwAnswerIDs2 >> 16)&0xffff));
			idAnswer2.Set(0x1000,(WORD)((dwAnswerIDs2&0xffff)));

			CIPCExtraMemory tmpBubble;
			if (tmpBubble.Create(m_pControl,dwExtraMemoryLen,pExtraMem))
			{
				CIPCFetchResult remoteFetchResult(tmpBubble);
				OnCipcPipeConfirm(ct, idLocal,idAnswer, idLocal2, idAnswer2, remoteFetchResult);
				bRet = TRUE;	// << EXIT >>>
			}
			return bRet;
		}
		else
		{
			return FALSE;	// << EXIT >>>
		}

	}
	else if (dwCmd==SRV_CONTROL_REQUEST_PIPE)
	{ 
		CIPCExtraMemory tmpBubble;
		if (tmpBubble.Create(m_pControl,dwExtraMemoryLen,pExtraMem))
		{
			CIPCCmdRecord cmdRecord(&tmpBubble,dwCmd,dwParam1,dwParam2,dwParam3,dwParam4);
			CConnectionRecord connectRecord(cmdRecord);
			OnCipcPipeRequest(connectRecord);
			return TRUE;	// << EXIT >>>
		}
	}
	else
	{
		if (m_pCachedPipe && m_pCachedPipe->GetID()==idTo)
		{
			pPipe = m_pCachedPipe;
			bFound=TRUE;
		}
		else
		{
			switch (byCmdFrom)
			{
				case CIPC_INPUT_CLIENT:
					iCount = m_InputPipesServer.GetSize();
					for (i=0 ; i<iCount ; i++)
					{
						pPipe = m_InputPipesServer.GetAt(i);
						if ( pPipe && (pPipe->GetID() == idTo) )
						{
							bFound = TRUE;
							break;
						}
					}
					break;
				case CIPC_OUTPUT_CLIENT:
					iCount = m_OutputPipesServer.GetSize();
					for (i=0 ; i<iCount ; i++)
					{
						pPipe = m_OutputPipesServer.GetAt(i);
						if ( pPipe && (pPipe->GetID() == idTo) )
						{
							bFound = TRUE;
							break;
						}
					}
					break;
				case CIPC_DATABASE_CLIENT:
					iCount = m_DataBasePipesServer.GetSize();
					for (i=0 ; i<iCount ; i++)
					{
						pPipe = m_DataBasePipesServer.GetAt(i);
						if ( pPipe && (pPipe->GetID() == idTo) )
						{
							bFound = TRUE;
							break;
						}
					}
					break;
				case CIPC_AUDIO_CLIENT:
					iCount = m_AudioPipesServer.GetSize();
					for (i=0 ; i<iCount ; i++)
					{
						pPipe = m_AudioPipesServer.GetAt(i);
						if ( pPipe && (pPipe->GetID() == idTo) )
						{
							bFound = TRUE;
							break;
						}
					}
					break;
				case CIPC_INPUT_SERVER:
					iCount = m_InputPipesClient.GetSize();
					for (i=0 ; i<iCount; i++)
					{
						pPipe = m_InputPipesClient.GetAt(i);
						if ( pPipe && (pPipe->GetID() == idTo) )
						{
							bFound = TRUE;
							break;
						}
					}
					break;
				case CIPC_OUTPUT_SERVER:
					iCount = m_OutputPipesClient.GetSize();
					for (i=0 ; i<iCount ; i++)
					{
						pPipe = m_OutputPipesClient.GetAt(i);
						if ( pPipe && (pPipe->GetID() == idTo) )
						{
							bFound = TRUE;
							break;
						}
					}
					break;
				case CIPC_AUDIO_SERVER:
					iCount = m_AudioPipesClient.GetSize();
					for (i=0 ; i<iCount ; i++)
					{
						pPipe = m_AudioPipesClient.GetAt(i);
						if ( pPipe && (pPipe->GetID() == idTo) )
						{
							bFound = TRUE;
							break;
						}
					}
					break;
				case CIPC_DATABASE_SERVER:
					iCount = m_DataBasePipesClient.GetSize();
					for (i=0 ; i<iCount ; i++) {
						pPipe = m_DataBasePipesClient.GetAt(i);
						if ( pPipe && (pPipe->GetID() == idTo) )
						{
							bFound = TRUE;
							break;
						}
					}
					break;
				default:
					WK_TRACE_ERROR(_T("PushCipcCmd() WRONG Type %u\n"), byCmdFrom);
					break;
			}
		}	// end of no chaed pipe

		if (bFound)
		{
			if ( pPipe->GetCIPC()->GetIsMarkedForDelete() )
			{
				// no message
				m_pCachedPipe = NULL;
			}
			else
			{
				m_pCachedPipe = pPipe;
				pPipeIST = (CIPCPipeIST *) pPipe;	// cast to subclass OOPS
				if ( !pPipeIST->PushCipcCmd(dwExtraMemoryLen, pExtraMem, dwCmd,
										dwParam1, dwParam2, dwParam3, dwParam4) )
				{
					WK_TRACE_ERROR(_T("PushCipcCmd() FAILED '%s' ID %x\n"),
																			pPipe->GetCIPC()->GetShmName(),
																			pPipe->GetID());
				}
				else
				{
//					WK_TRACE(_T("PushCipcCmd() OK Cmd '%s' Len %u\n"),
//												CIPC::NameOfCmd(dwCmd),
//												dwExtraMemoryLen );
					bReturn = TRUE;
				}
			}
		}
		else
		{
			// no message sigh
		}
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CISDNConnection::AcceptCallForCalledPartyNumber(const CAbstractCapiCallInfo &callInfo)
{
	BOOL bAccept = FALSE;
	CString sCalledParty = callInfo.GetCalledParty();
	CString sCallingParty = callInfo.GetCallingParty();
	CWK_Profile prof;


	// accecpt if _T("A") or Capi does not send sCalledParty
	if (m_sOwnNumber == LISTEN_ALL)
	{
		bAccept = TRUE;
		WK_TRACE(_T("Accepted by ListenAll\n"));
	}
	else if (sCalledParty.GetLength()==0 )
	{
		bAccept = TRUE;
		WK_TRACE(_T("Accepted by empty calledParty\n"));
	}
	else
	{
		if ( sCalledParty==m_sOwnNumber.Right(sCalledParty.GetLength()) )
		{
			// right part of sCalledParty matches own number
			bAccept = TRUE;
			WK_TRACE(_T("Accepted by number match: '%s' in '%s'\n"),
				(LPCTSTR)sCalledParty,
				(LPCTSTR)m_sOwnNumber
				);
		}
		else if (	// own number can be smaller than the system number
					m_sOwnNumber.GetLength()
				 && (m_sOwnNumber.GetLength() < sCalledParty.GetLength())
				 && m_sOwnNumber==sCalledParty.Right(m_sOwnNumber.GetLength()) )
		{
			// right part of m_sOwnNumber matches sCalledParty
			bAccept = TRUE;
			WK_TRACE(_T("Accepted by reverse number match: '%s' in '%s'\n"),
				(LPCTSTR)sCalledParty,
				(LPCTSTR)m_sOwnNumber
			);
		}
		else
		{
			// no match
			bAccept = FALSE;
			WK_TRACE(_T("Denieded by number: '%s' not '%s'\n"),
				(LPCTSTR)sCalledParty,
				(LPCTSTR)m_sOwnNumber
				);
		}
	}

	return bAccept;
}
/////////////////////////////////////////////////////////////////////////////
// CAQbstractCapiClient functions
const CString &CISDNConnection::GetOwnNumber() const
{
	return MyGetApp()->GetOwnNumber();
}
/////////////////////////////////////////////////////////////////////////////
int CISDNConnection::GetNumChannels() const
{
	return MyGetApp()->GetNumConfiguredBChannels();
}
/////////////////////////////////////////////////////////////////////////////
void CISDNConnection::OnListenConfirm(WORD wListen)
{
}
/////////////////////////////////////////////////////////////////////////////
void CISDNConnection::UpdateMSNSupport(const CString &sToText)
{
	BOOL bHasSupport = (sToText.IsEmpty()==FALSE);
	if ( bHasSupport !=ms_bSupportsMSN) 
	{
		CWK_Profile wkp;
		ms_bSupportsMSN = bHasSupport;
		wkp.WriteInt(MyGetApp()->GetProfileSection(), 
			_T("SupportMSN"), ms_bSupportsMSN
			);
	}
}
/////////////////////////////////////////////////////////////////////////////
/*@TOPIC Annahme eines Anrufes|ISDNUnit,IncomingCall
NYD

Es wird es auf die Nummer/MSN geprüft, dann abhängig vom Zustand
und auf dei Anzahl der erlaubten B-Kanäle.
*/
CapiIncomingAnswer CISDNConnection::OnIncomingCall(CapiHandle newCh, const CAbstractCapiCallInfo &callInfo)
{
	WK_TRACE(_T("IncommingCall() from '%s' to '%s'\n"),
					callInfo.GetCallingParty(),
					callInfo.GetCalledParty()
			);

	UpdateMSNSupport(callInfo.GetCalledParty());
	CapiIncomingAnswer answer = INCOMING_REJECTED_IGNORE;
	CString sCalledParty = callInfo.GetCalledParty();
	CString sCallingParty = callInfo.GetCallingParty();

	m_bNoNumbersFromCapi = sCallingParty.IsEmpty() || sCalledParty.IsEmpty();

	if (m_bNoNumbersFromCapi)
	{
		WK_TRACE(_T("NoNumbersFromCapi\n"));
	}

	// check for number/MSN independend from state
	BOOL bAcceptNumber = AcceptCallForCalledPartyNumber(callInfo);

	if (bAcceptNumber)
	{
		// if there are 2 or more units listening at the same MSN, we have to check some more
		CWK_Profile wkp;
		CString sFirstConnected	= wkp.GetString(_T("ISDNUnit"), szConnectedRemoteNumber, _T(""));
		int		iFirstChannels	= wkp.GetInt(_T("ISDNUnit"), szChannelsConnected, 0);
		CString sSecondConnected= wkp.GetString(_T("ISDNUnit2"), szConnectedRemoteNumber, _T(""));
//		int		iSecondChannels	= wkp.GetInt(_T("ISDNUnit2"), szChannelsConnected, 0);
		if (!ScanHostList(sCallingParty))
		{
			WK_TRACE(_T("IncommingCall() rejected by calling number\n"));
			answer = INCOMING_REJECTED;

			// Virtuellen Reject-Alarm auslösen...
			CString sOnlineMsg;
			sOnlineMsg.Format(_T("ISDN Reject Call from '%s'"), sCallingParty);
			CWK_RunTimeError runtimeError(REL_MESSAGE, RTE_REJECT, sOnlineMsg,0,1);
			runtimeError.Send();
			Sleep(50);
			CWK_RunTimeError runtimeError2(REL_MESSAGE, RTE_REJECT, sOnlineMsg,0,0);
			runtimeError2.Send();
		}
		else if (GetConnectState() == CONNECT_CLOSED)
		{
			if (   theApp.IsSecondInstance()
				&& sFirstConnected.IsEmpty()
			   )
			{
				WK_TRACE(_T("First Incomming call ignored by second instance\n"));
				answer = INCOMING_REJECTED_IGNORE;
			}
			else if (   theApp.IsSecondInstance()
					 && (sFirstConnected == sCallingParty)
				    )
			{
				WK_TRACE(_T("Incomming call ignored by second instance for channel bundling\n"));
				answer = INCOMING_REJECTED_IGNORE;
			}
			else
			{	// first instance or other calling id than first instance
				// Check for ALL B-Channels already used, if second instance
				if (   theApp.IsSecondInstance()
					&& (iFirstChannels >= MAX_PHYSICAL_B_CHANNELS)
					)
				{
					WK_TRACE(_T("Incomming call rejected by second instance, all channels in use at first unit\n"));
					answer = INCOMING_REJECTED_BUSY;
					// KnockKnock will be handled at first unit
				}
				else
				{
					// reserve this caller for possible channel bundling and public notice
					CWK_Profile wkp;
					wkp.WriteString(theApp.GetProfileSection(), szConnectedRemoteNumber, sCallingParty);
					m_CallState = CALL_PASSIVE;
					m_ConnectState = CONNECT_REQUEST;
					m_sRemoteNumber  = sCallingParty;
					WK_TRACE(_T("IncommingCall() accept FIRST channel\n"));
					answer = INCOMING_ACCEPTED;
					if (IsPTConnection())
					{
						// OOPS reset internal state/tick
						m_pPTBox->SetPTConnectionState(PCS_NOT_CONNECTED);
					}
				}
			}
		}
		else if (GetConnectState() == CONNECT_REQUEST)
		{
			if (m_CallState == CALL_ACTIVE)
			{
				// reject while calling out? OOPS NOT YET verified
				// GF: Seems to be not possible in CAPI protocol, but IF?
				// Decision: Will be rejected/ignored!
				if (   (theApp.IsSecondInstance() && sFirstConnected.IsEmpty())
					|| (   !theApp.IsSecondInstance()
					    && theApp.IsSecondInstanceAllowed()
					    && sSecondConnected.IsEmpty()
						)
				   )
				{
					WK_TRACE(_T("Incomming call ignored while calling out\n"));
					answer = INCOMING_REJECTED_IGNORE;
				}
				else
				{
					WK_TRACE(_T("Incomming call rejected while calling out\n"));
					// NOT YET any KnockKnock action?
					answer = INCOMING_REJECTED_BUSY;
				}
			}
			else if (m_CallState == CALL_PASSIVE)
			{
				// reject while just called? OOPS NOT YET verified
				// GF: Seems to be not possible in CAPI protocol, but IF?
				// Decision: Will be rejected!
				if (   (theApp.IsSecondInstance() && sFirstConnected.IsEmpty())
					|| (   !theApp.IsSecondInstance()
					    && theApp.IsSecondInstanceAllowed()
					    && sSecondConnected.IsEmpty()
						)
				   )
				{
					WK_TRACE(_T("Incomming call ignored while just called\n"));
					answer = INCOMING_REJECTED_IGNORE;
				}
				else
				{
					WK_TRACE(_T("Incomming call rejected while just called\n"));
					// NOT YET any KnockKnock action?
					answer = INCOMING_REJECTED_BUSY;
				}
			}
		}
		else if (GetConnectState() == CONNECT_OPEN)
		{
			// no need for check on 'A"? if m_sRemoteNumber  is set correctly
			if (m_sRemoteNumber == sCallingParty)
			{
				int iNumPossibleChannels = MyGetApp()->GetNumConfiguredBChannels();
				if ( GetNumBChannels()+1 > iNumPossibleChannels)
				{
					WK_TRACE(_T("IncommingCall rejected. No free channel\n"));
					answer = INCOMING_REJECTED_NO_CHANNEL;
				}
				else
				{
					if (   (theApp.IsSecondInstance() && sFirstConnected.IsEmpty())
						|| (   !theApp.IsSecondInstance()
							&& !theApp.IsSecondInstanceAllowed()
							)
						|| (   !theApp.IsSecondInstance()
							&& theApp.IsSecondInstanceAllowed()
							&& sSecondConnected.IsEmpty()
							)
					   )
					{
						WK_TRACE(_T("Incomming call accept NEXT channel\n"));
						answer = INCOMING_ACCEPTED;
					}
					else
					{
						// other unit has a channel also, no more free
						WK_TRACE(_T("IncommingCall rejected. No free channel, other unit\n"));
						answer = INCOMING_REJECTED_NO_CHANNEL;
					}
				}
			}
			else
			{	// other caller
				if (   theApp.IsSecondInstance()
					&& sFirstConnected.IsEmpty()
					)
				{	// first unit is free, giv'em a chance
					WK_TRACE(_T("Incomming call ignore NEXT caller by calling number\n"));
					answer = INCOMING_REJECTED_IGNORE;
				}
				else if (   (   !theApp.IsSecondInstance()
					    && theApp.IsSecondInstanceAllowed()
					    && sSecondConnected.IsEmpty()
						)
				   )
				{	// second unit is free, giv'em a chance
					// but check, if any channel is available üat all
					if (iFirstChannels >= MAX_PHYSICAL_B_CHANNELS)
					{
						WK_TRACE(_T("Incomming call rejected by first instance, all channels already in use\n"));
						answer = INCOMING_REJECTED_BUSY;
						ShowKnockKnockBox(callInfo, TRUE);
					}
					else
					{
						WK_TRACE(_T("Incomming call ignore NEXT caller by calling number\n"));
						answer = INCOMING_REJECTED_IGNORE;
					}
				}
				else
				{
					WK_TRACE(_T("Incomming call reject NEXT channel by calling number\n"));
					answer = INCOMING_REJECTED_BUSY;
					ShowKnockKnockBox(callInfo, TRUE);
				}
			}
		}
		else if (GetConnectState() == CONNECT_CLOSING)
		{
			if (   (theApp.IsSecondInstance() && sFirstConnected.IsEmpty())
				|| (   !theApp.IsSecondInstance()
					&& theApp.IsSecondInstanceAllowed()
					&& sSecondConnected.IsEmpty()
					)
			   )
			{	// other unit is free, giv'em a chance
				WK_TRACE(_T("Incomming call ignored while calling out\n"));
				answer = INCOMING_REJECTED_IGNORE;
			}
			else
			{
				WK_TRACE(_T("Incomming call rejected while calling out\n"));
				answer = INCOMING_REJECTED_BUSY;
				// NOT YET any KnockKnock action?
			}
		}
		else
		{
			// what's going on here
			// neither CONNECT_CLOSED not CONNECT_REQUEST nor CONNECT_OPEN nor CONNECT_CLOSING
			// MUST be CONNECT_ERROR
			WK_TRACE_ERROR(_T("Incomming call internal error 2135789\n"));
			answer = INCOMING_REJECTED_IGNORE;
		}
	}
	else
	{
		WK_TRACE(_T("IncommingCall() ignored by called number\n"));
		answer = INCOMING_REJECTED_IGNORE;
	}	// end of number check
	return answer;
}
/////////////////////////////////////////////////////////////////////////////
void CISDNConnection::OnIncomingOkay(CapiHandle newCh, const CString &sNumber)
{
}
/////////////////////////////////////////////////////////////////////////////
long CISDNConnection::OnOutgoingDone(CapiHandle ch, CapiOutgoingResult result)
{
	// NOT YET
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CISDNConnection::OnNumBChannelsChanged(CapiHandle ch, WORD wChannels)
{
	if (m_ConnectState != CONNECT_CLOSED && wChannels == 0)
	{
		TRACE(_T("Forcing ConnectState(%s) to Closed!\n"), NameOfEnum(m_ConnectState));
		m_ConnectState = CONNECT_CLOSED;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CISDNConnection::OnConnectionClosed(CapiHandle ch)
{
	if (m_ConnectState != CONNECT_CLOSED)
	{
		TRACE(_T("Forcing ConnectState(%s) to Closed!\n"), NameOfEnum(m_ConnectState));
		m_ConnectState = CONNECT_CLOSED;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CISDNConnection::OnReceiveData(CapiHandle ch, DEFHDR *pData)
{
	if (IsPTConnection())
	{
		WK_TRACE_ERROR(_T("internal error: received iST data for PT protocol\n"));
	}
	// NOT YET check service/type
	if (m_bDoResetCapi==FALSE && m_bDoCloseCapi==FALSE)
	{
		const CIPCHDR *pCipcHdr = (const CIPCHDR*)pData;
		ReceiveCipcData(pCipcHdr);
	}
	else
	{
		// OOPS ignore incoming data while closing or in reset
	}
}
/////////////////////////////////////////////////////////////////////////////
// NEW 17.8.97
void CISDNConnection::OnCallRejected()
{
	// handled via OnInfo
	TRACE(_T("CallRejected\n"));
}
/////////////////////////////////////////////////////////////////////////////
void CISDNConnection::OnUserBusy()
{
	// handled via OnInfo
	TRACE(_T("UserBusy\n"));
}
/////////////////////////////////////////////////////////////////////////////
void CISDNConnection::OnUnableToConnect()	// 0x34A2
{
	// handled via OnInfo
	TRACE(_T("Unable to connect\n"));
}
/////////////////////////////////////////////////////////////////////////////
void CISDNConnection::OnInfo(WORD wInfo)
{
	if (m_wCapiInfo == 0)
	{
		m_wCapiInfo = wInfo;
/*
		TRACE(_T("Info %x\n"),(DWORD)wInfo);
		switch (m_wCapiInfo)
		{
			//-- informations about the clearing of a physical connection -----
//			case 0x3301: // Protocol error layer 1 (broken line or B-channel removed by signalling protocol)
			case 0x3302: // Protocol error layer 2
			case 0x3303: // Protocol error layer 3
			case 0x3304: // Another application got that call
				OnCapiGetMessageError();
				break;
		}
*/
	}
	else
	{
//		if (m_wCapiInfo != wInfo)
//		{
//			WK_TRACE(_T("Info changed from %x to %x\n"), (DWORD)m_wCapiInfo, (DWORD)wInfo);
//		}
		m_wCapiInfo = wInfo;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CISDNConnection::OnCapiGetMessageError()
{
	TRACE(_T("OnCapiGetMessageError\n"));
	// Something got totally wrong, we should close those connection and reinitialise CAPI
	// Post the message only once!
	if (m_bHandlingCapiGetMessageError == FALSE)
	{
		TRACE(_T("Post OnCapiGetMessageError to App\n"));
		theApp.GetMainWnd()->PostMessage(ISDNUNITWND_CAPI_GET_MESSAGE_ERROR);
		m_bHandlingCapiGetMessageError = TRUE;
	}
}
// end of CAQbstractCapiClient functions
/////////////////////////////////////////////////////////////////////////////
// called from clients in OnRequestDisconnect
void CISDNConnection::RemoveMe(CIPCPipeISDN *pPipe)
{
	TRACE(_T("RemoveMe %s\n"),pPipe->GetCIPC()->GetShmName());
	pPipe->GetCIPC()->DelayedDelete();

	if (pPipe==m_pPTBox->m_pInputPT)
	{
		TRACE(_T("Remove PTInput\n"),pPipe->GetCIPC()->GetShmName());
		m_pPTBox->m_pInputPT=NULL;
		m_pPTBox->SetPTConnectionState(PCS_CLOSING);
	}
	if (pPipe==m_pPTBox->m_pOutputPT)
	{
		TRACE(_T("Remove PTOutput\n"),pPipe->GetCIPC()->GetShmName());
		m_pPTBox->m_pOutputPT=NULL;
		m_pPTBox->SetPTConnectionState(PCS_CLOSING);	// OOPS
	}

	// OOPS remove anywhere
	m_InputPipesClient.Remove(pPipe);
	m_OutputPipesClient.Remove(pPipe);
	m_DataBasePipesClient.Remove(pPipe);
	m_AudioPipesClient.Remove(pPipe);
	m_InputPipesServer.Remove(pPipe);
	m_OutputPipesServer.Remove(pPipe);
	m_DataBasePipesServer.Remove(pPipe);
	m_AudioPipesServer.Remove(pPipe);

	m_removedPipes.Lock();
	m_removedPipes.SafeAdd(pPipe);
	m_removedPipes.Unlock();

}
/////////////////////////////////////////////////////////////////////////////
void CISDNConnection::RequestPipe(CIPC *pCipc,
								  const CConnectionRecord &c,
								  DWORD dwCmd)
{
	// check for valid host here or already done? NOT YET
	// check for busy? here or in thread?
	CIPCCmdRecord *pCmd = c.CreateCmdRecord(pCipc,dwCmd);
	if (theApp.TracePipes())
	{
		WK_TRACE(_T("RequestPipe(%s -> %s)\n"), c.GetSourceHost(), c.GetDestinationHost());
	}
	m_pipeRequests.SafeAddTail(pCmd);
}
/////////////////////////////////////////////////////////////////////////////
void CISDNConnection::PanicClose(LPCTSTR szMsg)
{
	WK_STAT_PEAK(_T("Reset"),1,_T("Panic"));

	WK_TRACE_ERROR(_T(">>> PanicClose around %s<<< \n"),szMsg);
	DumpPipeState();
	CloseConnection();
}
/////////////////////////////////////////////////////////////////////////////
void CISDNConnection::DumpPipeState()
{
	CIPCPipeISDN *pPipe=NULL;
	int i=0;

	WK_TRACE(_T("InputPipes %d\n"),m_InputPipesServer.GetSize());
	for (i=0;i<m_InputPipesServer.GetSize();i++)
	{
		pPipe = m_InputPipesServer.GetAt(i);
		WK_TRACE(_T("\tpipe %x\n"),pPipe->GetID().GetID());
	}
	WK_TRACE(_T("InputPipes clients %d\n"),m_InputPipesClient.GetSize());
	for (i=0;i<m_InputPipesClient.GetSize();i++)
	{
		pPipe = m_InputPipesClient.GetAt(i);
		WK_TRACE(_T("\tpipe %x\n"),pPipe->GetID().GetID());
	}

	WK_TRACE(_T("OutputPipes %d\n"),m_OutputPipesServer.GetSize());
	for (i=0;i<m_OutputPipesServer.GetSize();i++)
	{
		pPipe = m_OutputPipesServer.GetAt(i);
		WK_TRACE(_T("\tpipe %x\n"),pPipe->GetID().GetID());
	}
	WK_TRACE(_T("OutputPipes clients %d\n"),m_OutputPipesClient.GetSize());
	for (i=0;i<m_OutputPipesClient.GetSize();i++)
	{
		pPipe = m_OutputPipesClient.GetAt(i);
		WK_TRACE(_T("\tpipe %x\n"),pPipe->GetID().GetID());
	}

	WK_TRACE(_T("DatabasePipes %d\n"),m_DataBasePipesServer.GetSize());
	for (i=0;i<m_DataBasePipesServer.GetSize();i++)
	{
		pPipe = m_DataBasePipesServer.GetAt(i);
		WK_TRACE(_T("\tpipe %x\n"),pPipe->GetID().GetID());
	}
	WK_TRACE(_T("DatabasePipes clients %d\n"),m_DataBasePipesClient.GetSize());
	for (i=0;i<m_DataBasePipesClient.GetSize();i++)
	{
		pPipe = m_DataBasePipesClient.GetAt(i);
		WK_TRACE(_T("\tpipe %x\n"),pPipe->GetID().GetID());
	}

	WK_TRACE(_T("AudioPipes %d\n"),m_AudioPipesServer.GetSize());
	for (i=0;i<m_AudioPipesServer.GetSize();i++)
	{
		pPipe = m_AudioPipesServer.GetAt(i);
		WK_TRACE(_T("\tpipe %x\n"),pPipe->GetID().GetID());
	}
	WK_TRACE(_T("AudioPipes clients %d\n"),m_AudioPipesClient.GetSize());
	for (i=0;i<m_AudioPipesClient.GetSize();i++)
	{
		pPipe = m_AudioPipesClient.GetAt(i);
		WK_TRACE(_T("\tpipe %x\n"),pPipe->GetID().GetID());
	}
}
/////////////////////////////////////////////////////////////////////////////
void CISDNConnection::ShowKnockKnockBox(const CAbstractCapiCallInfo &callInfo, BOOL bCanClose)
{
	if (MyGetApp()->IsKnockBoxEnabled())
	{
		WK_TRACE(_T("Show KnockKnockBox...\n"));
		if (m_pKnockKnockBox==NULL) {
			m_pKnockKnockBox = new CKnockKnockBox(this, NULL);
			m_pKnockKnockBox->Create(m_pKnockKnockBox->IDD, NULL);
		}
		else
		{
			// already there any kind of Flash?
			m_pKnockKnockBox->SetCallInfo(callInfo, bCanClose);
		}
		m_pKnockKnockBox->SetCallInfo(callInfo, bCanClose);	// does showWindow etc. too
	}
}
/////////////////////////////////////////////////////////////////////////////
void CISDNConnection::DeleteKnockKnockBox()
{
		// OOPS wie macht macht man das richtig
	if (m_pKnockKnockBox)
	{
		m_pKnockKnockBox->DestroyWindow();
	}
	WK_DELETE(m_pKnockKnockBox);
}
/////////////////////////////////////////////////////////////////////////////
void CISDNConnection::UpdateKnockKnockBox(const CAbstractCapiCallInfo &callInfo)
{
	// NOT YET here, still all in CCapiThread::OnIdle
}
/////////////////////////////////////////////////////////////////////////////////
BOOL CISDNConnection::ScanHostList(const CString &sCallingNumber)
{
	CWK_Profile prof;
	CString sKey;

	if (theApp.GetApplicationId() == WAI_ISDN_UNIT)
	{
		sKey = _T("ISDNUnit");
	}
	else if (theApp.GetApplicationId() == WAI_ISDN_UNIT_2)
	{
		sKey = _T("ISDNUnit2");
	}
	else
		return TRUE;	// PTBox erstmal nicht überprüfen.

	// Wenn keine Nummer übermittelt wurde, auch keine Suche starten
	if (sCallingNumber.IsEmpty())
	{
		// Sollen Rufe ohne Rufnummer abgewiesen werden?
		if (prof.GetInt(sKey, _T("RejectWithoutNumber"), 0) == 1)
		{
			WK_TRACE(_T("Rejecting incoming call. No Callingnumber\n"));
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}

	// Soll die Rufnummer in der Hostliste gesucht werden?
	if (prof.GetInt(sKey, _T("UseHostList"), 0) == 0)
	{
		return TRUE;
	}

	// Nummer in der Hostliste suchen.
	CString sLineNr;
	CString sHostlistNrOrig;
	CString sHostlistNr;
	CString sCallingNrReverse;
	CString sHostlistNrReverse;

	BOOL bFoundInList = FALSE;
	int nI = 0;

	// Get first list entry
	do
	{
		sLineNr.Format(_T("%05d"), nI++);
		sHostlistNrOrig	= prof.GetString(sKey+_T("\\HostList"), sLineNr, szEndOfList);
		if (sHostlistNrOrig != szEndOfList)
		{
			// Alle Zeichen außer den Ziffern aus dem Suchstring entfernen
			sHostlistNr = StringOnlyCharsInSet(sHostlistNrOrig, _T("0123456789"));

			// Nummer in der Liste gefunden?
			if (sHostlistNr.IsEmpty() == FALSE)
			{
				// ISDN uebermittelt die Rufnummern in der Regel ohne führende Nullen
				// der Vergleich klappt dann nicht, wenn die Nummern aus der Gegenstationsliste uebernommen wurde
				if (sHostlistNr == sCallingNumber)
				{
					bFoundInList = TRUE;
					WK_TRACE(_T("Accept incoming call by hostlist number match <%s> == <%s>\n"), sCallingNumber, sHostlistNr);
				}
				// toleranter Vergleich von hinten, aber nur, wenn überhaupt eine Ziffer im Listeneintrag ist!
				else 
				{
					sCallingNrReverse = sCallingNumber;
					sHostlistNrReverse = sHostlistNr;
					sCallingNrReverse.MakeReverse();
					sHostlistNrReverse.MakeReverse();
					if (0 == _tcsncicmp(sCallingNrReverse, sHostlistNrReverse, min(sCallingNumber.GetLength(), sHostlistNr.GetLength()) ))
					{
						bFoundInList = TRUE;
						WK_TRACE(_T("Accept incoming call by tolerant hostlist number match <%s> => <%s>\n"),
																			sCallingNumber, sHostlistNr);
					}
				}
			}
			else
			{
				WK_TRACE_WARNING(_T("Host list entry <%s> does not contain a valid ISDN number\n"), sHostlistNrOrig);
			}
		}
		else
		{
			break;
		}
	}
	while ((bFoundInList == FALSE));

	if (!bFoundInList)
	{
		WK_TRACE(_T("Rejecting incoming call. Calling number <%s> not in the hostlist\n"), sCallingNumber);
	}
	return bFoundInList;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CISDNConnection::AnyPipesExisting()
{
	BOOL bReturn = FALSE;
	if (   m_InputPipesClient.GetSize()
		|| m_OutputPipesClient.GetSize()
		|| m_DataBasePipesClient.GetSize()
		|| m_AudioPipesClient.GetSize()
		|| m_InputPipesServer.GetSize()
		|| m_OutputPipesServer.GetSize()
		|| m_DataBasePipesServer.GetSize()
		|| m_AudioPipesServer.GetSize()
		)
	{
		bReturn = TRUE;
	}
	return bReturn;
}

LPCTSTR NameOfEnum(ConnectionState cs)
{
	switch (cs)
	{
		NAME_OF_ENUM(CONNECT_ERROR);
		NAME_OF_ENUM(CONNECT_CLOSED);
		NAME_OF_ENUM(CONNECT_REQUEST);
		NAME_OF_ENUM(CONNECT_OPEN);
		NAME_OF_ENUM(CONNECT_CLOSING);
	}
	return _T("");
}
