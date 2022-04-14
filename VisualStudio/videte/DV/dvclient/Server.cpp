// Server.cpp: implementation of the CServer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dvclient.h"
#include "Server.h"

#include "MainFrame.h"
#include "CPanel.h"

#include "IPCDatabaseDVClient.h"
#include "IPCInputDVClient.h"
#include "IPCOutputDVClient.h"
#include "IPCDataCarrierClient.h"
#include "CIPCAudioDVClient.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CServer::CServer()
{
	Init();
	m_bAlarm = FALSE;
}
//////////////////////////////////////////////////////////////////////
CServer::CServer(const CConnectionRecord& c)
{
	Init();
	m_ConnectionRecord = c;
	m_bAlarm = TRUE;
	m_sHostName = m_ConnectionRecord.GetSourceHost();
	m_ConnectionRecord.GetFieldValue(CSD_PIN,m_sEnteredPIN);
	m_bUserForcedConnect = TRUE;
	CString s;
	if (m_ConnectionRecord.GetFieldValue(CRF_MSN,s))
	{
		m_sHostNumber = s;
	}
	if (m_ConnectionRecord.GetFieldValue(CRF_IPADDRESS,s))
	{
		m_sHostNumber = _T("tcp:") + s;
	}
	if (m_sHostNumber.IsEmpty())
	{
		m_sHostNumber = LOCAL_LOOP_BACK;
	}
}
//////////////////////////////////////////////////////////////////////
void CServer::Init()
{
	m_iMajorVersion = 0;
	m_iMinorVersion = 0;
	m_iSubVersion = 0;
	m_iBuildNr = 0;

	m_bTimerOnOffState = FALSE;
	m_bConfirmSetClock = FALSE;
	m_bAlarmOutputOnOffState = FALSE;

	m_pConnectionRecord  = NULL;
	m_pConnectThread     = NULL;
	
	m_pInputResult       = NULL;
	m_pOutputResult      = NULL;
	m_pDatabaseResult    = NULL;
	m_pDataCarrierResult = NULL;
	m_pAudioResult       = NULL;

	m_pDatabase    = NULL;
	m_pOutput      = NULL;	
	m_pInput       = NULL;
	m_pDataCarrier = NULL;
	m_pAudio	      = NULL;

	m_bCancelConnect = FALSE;
	m_bConnectThreadRunning = FALSE;

	m_dwReconnectTick = GetTickCount();

	m_bUserForcedConnect = FALSE;
	m_iNrOfVideoOut = 2;
	m_nResolution = RESOLUTION_2CIF;
	m_nSystemAGC = VIDEO_AGC_ON;
	m_nSystemVideoformat = VIDEO_FORMAT_MPEG4;
	m_sSystemVideonorm = CSD_PAL;
	m_nCardType	= CARD_TYPE_UNKNOWN;
}
//////////////////////////////////////////////////////////////////////
CServer::~CServer()
{
	WK_TRACE(_T("CServer::~CServer\n"));
	Disconnect();	// also deletes all connection objects
	WK_DELETE(m_pConnectionRecord);
}
//////////////////////////////////////////////////////////////////////
void CServer::Lock()
{
	m_cs.Lock();
}
//////////////////////////////////////////////////////////////////////
void CServer::Unlock()
{
	m_cs.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CServer::SetVersion(const CString& sVersion)
{
	WK_TRACE(_T("SERVER VERSION is <%s>\n"),sVersion);
	int iMajorVersion,iMinorVersion,iSubVersion,iBuildNr;

	iMajorVersion = iMinorVersion = iSubVersion = iBuildNr = 0;
	if (4 == _stscanf(sVersion,_T("%d.%d.%d.%d"),&iMajorVersion,&iMinorVersion,&iSubVersion,&iBuildNr))
	{
		m_iMajorVersion = iMajorVersion;
		m_iMinorVersion = iMinorVersion;
		m_iSubVersion = iSubVersion;
		m_iBuildNr = iBuildNr;
	}
}
//////////////////////////////////////////////////////////////////////
void CServer::AlarmConnection()
{
	WK_TRACE(_T("CServer::AlarmConnection %s\n"),m_ConnectionRecord.GetSourceHost());
	if (!m_pInput)
	{
		WK_TRACE(_T("Alarm from %s, connecting input %s\n"),m_ConnectionRecord.GetSourceHost(),
			m_ConnectionRecord.GetInputShm());
		ConnectInput(m_ConnectionRecord.GetInputShm(),CIPC_ERROR_OKAY);
		for (int i=0;i<100 && !IsInputConnected();i++)
		{
			Sleep(10);
		}
	}
	else
	{
		WK_TRACE_ERROR(_T("Alarm from %s, input already connected\n"),m_ConnectionRecord.GetSourceHost());
	}
	if (!m_pOutput)
	{
		WK_TRACE(_T("Alarm from %s, connecting output %s\n"),m_ConnectionRecord.GetSourceHost(),
			m_ConnectionRecord.GetOutputShm());
		ConnectOutput(m_ConnectionRecord.GetOutputShm(),CIPC_ERROR_OKAY);
		for (int i=0;i<100 && !IsOutputConnected();i++)
		{
			Sleep(10);
		}
	}
	else
	{
		WK_TRACE_ERROR(_T("Alarm from %s, output already connected\n"),m_ConnectionRecord.GetSourceHost());
	}
}
//////////////////////////////////////////////////////////////////////
void CServer::OnIdle()
{
	BOOL bDoReconnect = FALSE;

	Lock();
	if (m_pConnectThread && !m_bConnectThreadRunning)
	{
		WK_DELETE(m_pConnectThread);
//		WK_TRACE(_T("ConnectThread stopped\n"));
	}
	Unlock();

	if (   !theApp.IsReadOnly()
		&& !IsInputConnected()
		)
	{
		// has the fetch thread any result for a connection
		Lock();
		if (m_pInputResult)
		{
			ConnectInput(m_pInputResult->GetShmName(),m_pInputResult->GetError());
			WK_DELETE(m_pInputResult);
		}
		else
		{
			bDoReconnect = TRUE;
		}
		Unlock();
	}
	
	if (   !theApp.IsReadOnly()
		&& !IsOutputConnected()
		)
	{
		Lock();
		if (m_pOutputResult)
		{
			ConnectOutput(m_pOutputResult->GetShmName(),m_pOutputResult->GetError());
			WK_DELETE(m_pOutputResult);
		}
		else
		{
			bDoReconnect = TRUE;
		}
		Unlock();
	}

	if (   !theApp.IsReadOnly()
		&& !IsAudioConnected()
		)
	{
		Lock();
		if (m_pAudioResult)
		{
			ConnectAudio(m_pAudioResult->GetShmName(), m_pAudioResult->GetError());
			WK_DELETE(m_pAudioResult);
		}
		else
		{
			bDoReconnect = TRUE;
		}
		Unlock();
	}

	if (!IsDatabaseConnected())
	{
		Lock();
		if (m_pDatabaseResult)
		{
			ConnectDatabase(*m_pDatabaseResult);
			WK_DELETE(m_pDatabaseResult);
			Sleep(200); // For Win2000!??
		}
		else
		{
			bDoReconnect = TRUE;
		}
		Unlock();
	}

	//Only connect to DataCarrier (ACDC) if DVCLient is a Transmitter AND not is "Readonly"
	if(	    !theApp.IsReadOnly()
		 &&	theApp.IsTransmitter()
		 && !IsDataCarrierConnected()
		)
	{
		Lock();
		if (m_pDataCarrierResult)
		{
			ConnectDataCarrier(m_pDataCarrierResult->GetShmName(),m_pDataCarrierResult->GetError());
			WK_DELETE(m_pDataCarrierResult);
			Sleep(200); // For Win2000!??
		}
		else
		{
			bDoReconnect = TRUE;
		}
		Unlock();
		
	}

	if (IsInputDisconnected())
	{
		DisconnectInput();
	}
		
	if (IsOutputDisconnected())
	{
		DisconnectOutput();
	}
		
	if (IsDatabaseDisconnected())
	{
		DisconnectDatabase();
	}

	if(IsDataCarrierDisconnected())
	{
		DisconnectDataCarrier();
	}

// gf Mist Database braucht den Rückruf...
/*	// If it is a receiver we have to prevent an immediate CallBack at incomming alarm call
	// A receiver at alarm does not have to call, he is called
	if (   theApp.IsReceiver()
		&& IsAlarm()
		)
	{	
		bDoReconnect = FALSE;
	}
*/

	BOOL bDoReconnectNow = FALSE;
	if (   IsAlarm()
		&& IsInputConnected()
		&& IsOutputConnected()
		&& !IsDatabaseConnected())
	{
		bDoReconnectNow = TRUE;
	}
	
	if (bDoReconnect)
	{
		if (   bDoReconnectNow
			|| (GetTimeSpan(m_dwReconnectTick) > 10*1000)
			)
		{
			m_dwReconnectTick = GetTickCount();
			if (   !IsDatabaseConnected()
				|| !IsInputConnected()
				|| !IsOutputConnected()
				|| !IsDataCarrierConnected())
			{
				Reconnect();
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::IsOutputConnected()
{
	BOOL bRet = FALSE;

	Lock();
	bRet = (m_pOutput!=NULL) && (m_pOutput->GetIPCState()==CIPC_STATE_CONNECTED);
	Unlock();

	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::IsInputConnected()
{
	BOOL bRet = FALSE;

	Lock();
	bRet = (m_pInput!=NULL) && (m_pInput->GetIPCState()==CIPC_STATE_CONNECTED);
	Unlock();

	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::IsDatabaseConnected()
{
	BOOL bRet = FALSE;

	Lock();
	bRet = (m_pDatabase!=NULL) && (m_pDatabase->GetIPCState()==CIPC_STATE_CONNECTED);
	Unlock();

	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::IsOutputConnecting()
{
	BOOL bRet = FALSE;

	Lock();
	bRet = (m_pOutput!=NULL) && (m_pOutput->GetIPCState()==CIPC_STATE_WRITE_CREATED);
	Unlock();

	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::IsInputConnecting()
{
	BOOL bRet = FALSE;

	Lock();
	bRet = (m_pInput!=NULL) && (m_pInput->GetIPCState()==CIPC_STATE_WRITE_CREATED);
	Unlock();

	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::IsDatabaseConnecting()
{
	BOOL bRet = FALSE;

	Lock();
	bRet = (m_pDatabase!=NULL) && (m_pDatabase->GetIPCState()==CIPC_STATE_WRITE_CREATED);
	Unlock();

	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::IsAudioConnecting()
{
	BOOL bRet = FALSE;

	Lock();
	bRet = (m_pAudio!=NULL) && (m_pAudio->GetIPCState()==CIPC_STATE_WRITE_CREATED);
	Unlock();

	return bRet;
}
//////////////////////////////////////////////////////////////////////////
BOOL CServer::IsAudioConnected()
{
	BOOL bRet = FALSE;
	if (m_pAudio)
	{
		bRet = (m_pAudio->GetIPCState() == CIPC_STATE_CONNECTED);
	}
	else
	{
		bRet = FALSE;
	}
	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::IsConnected()
{
	if (   m_pInput 
		&& m_pOutput
		&& m_pDatabase)
	{
		if (   (m_pInput->GetIPCState()==CIPC_STATE_CONNECTED) 
			&& (m_pOutput->GetIPCState()==CIPC_STATE_CONNECTED)
			&& (m_pDatabase->GetIPCState()==CIPC_STATE_CONNECTED))
		{
			return TRUE;
		}
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::IsInputDisconnected()
{
	BOOL bRet = FALSE;

	Lock();
	bRet = (m_pInput!=NULL) && (m_pInput->GetIsMarkedForDelete());
	Unlock();

	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::IsOutputDisconnected()
{
	BOOL bRet = FALSE;

	Lock();
	bRet = (m_pOutput!=NULL) && (m_pOutput->GetIsMarkedForDelete());
	Unlock();

	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::IsDatabaseDisconnected()
{
	BOOL bRet = FALSE;

	Lock();
	bRet = (m_pDatabase!=NULL) && (m_pDatabase->GetIsMarkedForDelete());
	Unlock();

	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::IsDataCarrierDisconnected()
{
	BOOL bRet = FALSE;

	Lock();
	bRet = (m_pDataCarrier!=NULL) && (m_pDataCarrier->GetIsMarkedForDelete());
	Unlock();

	return bRet;
}
//////////////////////////////////////////////////////////////////////
void CServer::Connect(const CString& sHostNumber, const CString& sHostName, const CString& sPIN)
{
	m_sHostNumber = sHostNumber;
	WK_TRACE(_T("CServer::Connect %s\n"),m_sHostNumber);
	m_sHostName = sHostName;
	m_sEnteredPIN  = sPIN;
	m_bUserForcedConnect = TRUE;
	Reconnect();
}
//////////////////////////////////////////////////////////////////////
void CServer::Reconnect()
{
	if (m_pConnectThread == NULL)
	{
//		WK_TRACE(_T("CServer::Reconnect\n"));
		Lock();
		WK_DELETE(m_pConnectionRecord);
		// use same ConnectionRecord for all Input, Output and Database
		m_pConnectionRecord = new CConnectionRecord;
		m_pConnectionRecord->SetDestinationHost(m_sHostNumber);
		m_pConnectionRecord->SetPermission(_T("SuperVisor"));
		// GF todo hier Operator PIN
		m_pConnectionRecord->SetPassword(m_sEnteredPIN);
		m_pConnectionRecord->SetFieldValue(CSD_PIN,m_sEnteredPIN);
		if (m_sHostNumber.Left(4) == _T("dun:"))
		{
			m_pConnectionRecord->SetTimeout(60000);
		}

		if (   !theApp.IsReadOnly()
			&& theApp.IsReceiver())
		{
			CWK_Profile wkp;
			// by default a invalid serial!
			CString sSerial = wkp.GetString(_T("Version"),_T("Serial"),_T("0"));
			if (sSerial.IsEmpty())
			{
				sSerial = _T("0");
			}
			m_pConnectionRecord->SetFieldValue(CRF_SERIAL,sSerial);
		}
		m_bConnectThreadRunning = TRUE;
		Unlock();
		m_pConnectThread = AfxBeginThread(ConnectThreadProc,(LPVOID)this);
		m_pConnectThread->m_bAutoDelete = FALSE;
//		WK_TRACE(_T("ConnectThread started\n"));
	}
}
//////////////////////////////////////////////////////////////////////
void CServer::Disconnect()
{
	WK_TRACE(_T("CServer::Disconnect\n"));

	// Set flag to cancel any pending connections
	Lock();
	m_bCancelConnect = TRUE;
	m_Fetch.Cancel();
	Unlock();

	// Wait for ConnectThread stopped, if any
	if (m_pConnectThread)
	{
		WK_TRACE(_T("Stop ConnectThread...\n"));
		CWK_StopWatch watch;
		DWORD dwExitCode = STILL_ACTIVE;
		if (m_pConnectThread->m_hThread && GetExitCodeThread(m_pConnectThread->m_hThread, &dwExitCode) && (dwExitCode == STILL_ACTIVE))
		{
			dwExitCode = WaitForSingleObject(m_pConnectThread->m_hThread, 5000);
		}
		WK_TRACE(_T("ConnectThread %s %u ms\n"), (dwExitCode==WAIT_OBJECT_0?_T("stopped in"):_T("TIMEOUT")), watch.GetElapsedMS());
		WK_DELETE(m_pConnectThread);
	}

	Lock();
	// Delete FetchResults to avoid any further connections
	WK_DELETE(m_pInputResult);
	WK_DELETE(m_pOutputResult);
	WK_DELETE(m_pDatabaseResult);
	WK_DELETE(m_pDataCarrierResult);
	WK_DELETE(m_pAudioResult);

	// Delete CIPC connections
	WK_DELETE(m_pInput);
	WK_DELETE(m_pOutput);
	WK_DELETE(m_pDatabase);
	WK_DELETE(m_pDataCarrier);
	WK_DELETE(m_pAudio);

	m_sHostName.Empty();
	m_sHostNumber.Empty();
	m_bAlarm = FALSE;
	Unlock();
}
//////////////////////////////////////////////////////////////////////
UINT CServer::ConnectThreadProc(LPVOID pParam)
{
	CServer* pServer = (CServer*)pParam;

	XTIB::SetThreadName(_T("CServer::ConnectThreadProc"));

	BOOL bCancelled = FALSE;
	pServer->Lock();
	pServer->m_bConnectThreadRunning = TRUE;
	pServer->Unlock();

	if (theApp.GetMainFrame())
	{
		BOOL bInputSuccess = TRUE;

		if (!theApp.IsReadOnly())
		{
			// in read only modus
			// keine Alarme und kein Live Bild
			if (   !pServer->IsInputConnected()
				&& !pServer->IsInputConnecting())
			{
				pServer->Lock();
				CConnectionRecord c(*pServer->m_pConnectionRecord);
				bCancelled = pServer->m_bCancelConnect;
				pServer->Unlock();
				if (!bCancelled)
				{
					CIPCFetchResult frInput = pServer->m_Fetch.FetchInput(c);

					pServer->Lock();
					WK_DELETE(pServer->m_pInputResult);
					pServer->m_pInputResult = new CIPCFetchResult(frInput);
					if (   frInput.IsOkay()
						&& !frInput.GetErrorMessage().IsEmpty())
					{
						pServer->SetVersion(frInput.GetErrorMessage());
					}
					pServer->Unlock();
					theApp.ForceOnIdle();
					bInputSuccess = frInput.IsOkay();

					// give server time to free control connection from the preceeding FetchInput
					Sleep(200);	
				}
			}


			if (   !pServer->IsOutputConnected()
				&& !pServer->IsOutputConnecting()
				&& bInputSuccess)
			{
				pServer->Lock();
				CConnectionRecord c(*pServer->m_pConnectionRecord);
				DWORD dwOptions = SCO_WANT_RELAYS | SCO_WANT_CAMERAS_COLOR|SCO_IS_DV;
				if (theApp.GetIntValue(_T("CanGOP"), 1))
				{
					dwOptions |= SCO_CAN_GOP;
				}
				c.SetOptions(dwOptions);
				bCancelled = pServer->m_bCancelConnect;
				pServer->Unlock();
				if (!bCancelled)
				{
					CIPCFetchResult frOutput = pServer->m_Fetch.FetchOutput(c);

					pServer->Lock();
					WK_DELETE(pServer->m_pOutputResult);
					pServer->m_pOutputResult = new CIPCFetchResult(frOutput);
					pServer->Unlock();
					theApp.ForceOnIdle();

					// give server time to free control connection from the preceeding FetchOutput
					Sleep(200);	
				}
			}
			if (    theApp.IsAudioAllowed()				// Audio allowed?
				 && (   theApp.IsReceiver()				// receiver connects remote
				     || (   pServer->IsLocal()			// Transmitter connects only local
					     && WK_IS_RUNNING(WK_APP_NAME_AUDIOUNIT1)// is local Unit running
				    )   )
				 && !pServer->IsAudioConnected()		// do not connect twice
				 && !pServer->IsAudioConnecting())		// already in progress?
			{
				CConnectionRecord c(*pServer->m_pConnectionRecord);
				CString sSmName;
				CIPCFetch fetch;
				CIPCFetchResult frAudio = fetch.FetchAudio(c);
				pServer->Lock();
				WK_DELETE(pServer->m_pAudioResult);
				pServer->m_pAudioResult = new CIPCFetchResult(frAudio);
				pServer->Unlock();
				theApp.ForceOnIdle();
			}
		}

		if (   !pServer->IsDatabaseConnected()
			&& !pServer->IsDatabaseConnecting()
			&& bInputSuccess)
		{
			pServer->Lock();
			CConnectionRecord c(*pServer->m_pConnectionRecord);
			bCancelled = pServer->m_bCancelConnect;
			pServer->Unlock();
			if (!bCancelled)
			{
				CIPCFetchResult frDatabase = pServer->m_Fetch.FetchDatabase(c);

				pServer->Lock();
				WK_DELETE(pServer->m_pDatabaseResult);
				pServer->m_pDatabaseResult = new CIPCFetchResult(frDatabase);
				pServer->Unlock();
				theApp.ForceOnIdle();
				// give server time to free control connection from the preceeding FetchDatabase
				Sleep(200);	
			}
		}

		if (   !theApp.IsReadOnly()
			&& !pServer->IsDataCarrierConnected()
			&& !pServer->IsDataCarrierConnecting()
			&& bInputSuccess
			)
		{
			pServer->Lock();
			CConnectionRecord c(*pServer->m_pConnectionRecord);
			//c.SetDestinationHost(LOCAL_LOOP_BACK);
			bCancelled = pServer->m_bCancelConnect;
			pServer->Unlock();
			if (!bCancelled)
			{
				CIPCFetchResult frDataCarrier = pServer->m_Fetch.FetchDataCarrier(c);
				
				pServer->Lock();
				WK_DELETE(pServer->m_pDataCarrierResult);
				pServer->m_pDataCarrierResult = new CIPCFetchResult(frDataCarrier);
				pServer->Unlock();
				theApp.ForceOnIdle();

				// give server time to free control connection from the preceeding FetchDatabase
				Sleep(200);	
			}
		}
	}
	
	pServer->Lock();
	pServer->m_bConnectThreadRunning = FALSE;
	pServer->Unlock();
	return 0xC0;
}
//////////////////////////////////////////////////////////////////////
void CServer::ConnectInput(const CString& shmName, CIPCError error)
{
	Lock();

	if (   error != CIPC_ERROR_OKAY
		|| shmName.IsEmpty())
	{
		// connect failed in main thread
		WK_TRACE(_T("Input	connect FAILED\n"));
		theApp.ForceOnIdle();
		if (m_bUserForcedConnect && theApp.GetPanel())
		{
			theApp.GetPanel()->NotifyConnect(FALSE,error);
		}
		else
		{
			// shit no panel
		}
	}
	else
	{
		if (m_pInput)
			WK_TRACE_WARNING(_T("ConnectInput m_pInput already exist\n"));
		m_pInput = new CIPCInputDVClient(shmName,this);
		if (theApp.GetPanel())
		{
			theApp.GetPanel()->NotifyConnect(TRUE,error);
		}
		else
		{
			// shit no panel
		}
	}
	m_bUserForcedConnect = FALSE;
	Unlock();
}
//////////////////////////////////////////////////////////////////////
void CServer::ConnectOutput(const CString& shmName, CIPCError error)
{
	Lock();

	if (   error != CIPC_ERROR_OKAY
		|| shmName.IsEmpty())
	{
		WK_TRACE(_T("Output connect FAILED\n"));
		theApp.ForceOnIdle();
		if (m_bUserForcedConnect && theApp.GetPanel())
		{
			theApp.GetPanel()->NotifyConnect(FALSE,error);
		}
		else
		{
			// shit no panel
		}
	}
	else
	{
		if (m_pOutput)
			WK_TRACE_WARNING(_T("ConnectInput m_pOutput already exist\n"));

		m_pOutput = new CIPCOutputDVClient(shmName,this);
		theApp.ForceOnIdle();
		if (theApp.GetPanel())
		{
			theApp.GetPanel()->NotifyConnect(TRUE,error);
		}
		else
		{
			// shit no panel
		}
	}
	m_bUserForcedConnect = FALSE;
	Unlock();
}
//////////////////////////////////////////////////////////////////////
void CServer::ConnectAudio(const CString& shmName, CIPCError error)
{
	Lock();

	if (   error != CIPC_ERROR_OKAY
		|| shmName.IsEmpty())
	{
		WK_TRACE(_T("Audio connect FAILED\n"));
		theApp.ForceOnIdle();
		if (m_bUserForcedConnect && theApp.GetPanel())
		{
			theApp.GetPanel()->NotifyConnect(FALSE,error);
		}
		else
		{
			// shit no panel
		}
	}
	else
	{
		if (m_pAudio)
			WK_TRACE_WARNING(_T("Connect m_pAudio already exist\n"));

		m_pAudio = new CIPCAudioDVClient(shmName, this);
		theApp.ForceOnIdle();
		if (theApp.GetPanel())
		{
			theApp.GetPanel()->NotifyConnect(TRUE,error);
		}
		else
		{
			// shit no panel
		}
	}
	m_bUserForcedConnect = FALSE;
	Unlock();
}
//////////////////////////////////////////////////////////////////////
void CServer::ConnectDatabase(const CIPCFetchResult fr)
{
	Lock();

	if (   fr.GetError() != CIPC_ERROR_OKAY
		|| fr.GetShmName().IsEmpty())
	{
		WK_TRACE(_T("Database connect FAILED %s\n"), NameOfEnum(fr.GetError()));
		theApp.ForceOnIdle();
		if (m_bUserForcedConnect && theApp.GetPanel())
		{
			theApp.GetPanel()->NotifyConnect(FALSE,fr.GetError());
		}
		else
		{
			// shit no panel
		}
	}
	else
	{
		if (m_pDatabase)
		{
			WK_TRACE_WARNING(_T("ConnectInput m_pDatabase already exist\n"));
		}
		if (theApp.IsReadOnly())
		{
			SetVersion(fr.GetServerVersion());
		}
		m_pDatabase = new CIPCDatabaseDVClient(fr.GetShmName(),this);
		if (theApp.GetPanel())
		{
			theApp.GetPanel()->NotifyConnect(TRUE,fr.GetError());
		}
		else
		{
			// shit no panel
		}
	}
	m_bUserForcedConnect = FALSE;
	Unlock();
}
//////////////////////////////////////////////////////////////////////
void CServer::ConnectDataCarrier(const CString& shmName, CIPCError error)
{
	Lock();

	if (   error != CIPC_ERROR_OKAY
		|| shmName.IsEmpty())
	{
		WK_TRACE(_T("DataCarrier connect FAILED\n"));
		theApp.ForceOnIdle();
		if (m_bUserForcedConnect && theApp.GetPanel())
		{
			theApp.GetPanel()->NotifyConnect(FALSE,error);
		}
		WK_TRACE(_T("Panel()->NotifyConnect\n"));
	}
	else
	{
		if (m_pDataCarrier)
			WK_TRACE_WARNING(_T("ConnectInput m_pDataCarrier already exist\n"));
		m_pDataCarrier = new CIPCDataCarrierClient(shmName,this);
		if (theApp.GetPanel())
		{
			theApp.GetPanel()->NotifyConnect(TRUE,error);
		}
		else
		{
			// shit no panel
		}
	}
	m_bUserForcedConnect = FALSE;
	Unlock();

}
//////////////////////////////////////////////////////////////////////
void CServer::DisconnectInput()
{
	WK_TRACE(_T("DisconnectInput %s\n"),GetHostName());
	WK_DELETE(m_pInput);
	CMainFrame* pMF = theApp.GetMainFrame();
	if (pMF)
	{
		pMF->DisconnectInput();
	}
}
//////////////////////////////////////////////////////////////////////
void CServer::DisconnectOutput()
{
	WK_TRACE(_T("DisconnectOutput %s\n"),GetHostName());
	WK_DELETE(m_pOutput);
	CMainFrame* pMF = theApp.GetMainFrame();
	if (pMF)
	{
		pMF->DisconnectOutput();
	}
}
//////////////////////////////////////////////////////////////////////
void CServer::DisconnectDatabase()
{
	WK_DELETE(m_pDatabase);
	CMainFrame* pMF = theApp.GetMainFrame();
	if (pMF)
	{
		pMF->DisconnectDatabase();
	}
}

//////////////////////////////////////////////////////////////////////
void CServer::DisconnectDataCarrier()
{
	WK_DELETE(m_pDataCarrier);
/*	CMainFrame* pMF = theApp.GetMainFrame();
	if (pMF)
	{
		pMF->DisconnectDatabase();
	}
*/
}
//////////////////////////////////////////////////////////////////////
CString CServer::GetSupervisorPIN()
{
	if (IsInputConnected())
	{
		if (m_pInput)
		{
			m_evConfirmGetPIN.ResetEvent();
			m_pInput->DoRequestGetValue(SECID_NO_ID,CSD_PIN_SUPERVISOR,0);
			if (!m_evConfirmGetPIN.Lock(2000))
			{
				WK_TRACE_WARNING(_T("Get Supervisor-PIN timeout. No confirm\n"));
			}
			m_evConfirmGetPIN.Unlock();
		}
	}
	return m_sSupervisorPIN;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::SetSupervisorPIN(const CString& sPIN)
{
	BOOL	bResult = FALSE;

	if (IsOutputConnected())
	{
		if (m_pOutput)
		{
			m_pOutput->DoRequestSetValue(SECID_NO_ID,CSD_PIN_SUPERVISOR,sPIN);
			if (m_evConfirmSetPIN.Lock(2000))
			{
				bResult = TRUE;
			}
			else
			{
				WK_TRACE_WARNING(_T("Set Supervisor-PIN timeout. No confirm\n"));
			}
			m_evConfirmSetPIN.Unlock();
		}	
	}
	return bResult;
}
//////////////////////////////////////////////////////////////////////
CString CServer::GetEnteredPIN()
{
	return m_sEnteredPIN;
}
//////////////////////////////////////////////////////////////////////
CString CServer::GetOperatorPIN()
{
	if (IsInputConnected())
	{
		if (m_pInput)
		{
			m_evConfirmGetPIN.ResetEvent();
			m_pInput->DoRequestGetValue(SECID_NO_ID,CSD_PIN_OPERATOR,0);
			if (!m_evConfirmGetPIN.Lock(2000))
			{
				WK_TRACE_WARNING(_T("Get Operator-PIN timeout. No confirm\n"));
			}
			m_evConfirmGetPIN.Unlock();
		}
	}
	return m_sOperatorPIN;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::SetOperatorPIN(const CString& sPIN)
{
	BOOL	bResult = FALSE;

	if (IsInputConnected())
	{
		if (m_pInput)
		{
			m_pInput->DoRequestSetValue(SECID_NO_ID,CSD_PIN_OPERATOR,sPIN);
			if (m_evConfirmSetPIN.Lock(2000))
			{
				bResult = TRUE;
			}
			else
			{
				WK_TRACE_WARNING(_T("Set Operator-PIN timeout. No confirm\n"));
			}
			m_evConfirmSetPIN.Unlock();
		}	
	}
	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CServer::SetTimerOnOffState(BOOL bState)
{
	BOOL	bResult = FALSE;

	if (IsOutputConnected())
	{
		if (m_pOutput)
		{
			m_pOutput->DoRequestSetValue(SECID_NO_ID,CSD_DETECTOR_IGNORE_TIMER, (bState ? CSD_ON : CSD_OFF));
			if (m_evConfirmSetTimerOnOff.Lock(2000))
				bResult = TRUE;
			else
				WK_TRACE_WARNING(_T("Set TimerOnOffState timeout. No confirm\n"));
			m_evConfirmSetTimerOnOff.Unlock();
		}	
	}
	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CServer::GetTimerOnOffState(BOOL &bState)
{
	BOOL bResult = FALSE;
	if (IsOutputConnected())
	{
		if (m_pOutput)
		{
			m_evConfirmGetTimerOnOff.ResetEvent();
			m_pOutput->DoRequestGetValue(SECID_NO_ID,CSD_DETECTOR_IGNORE_TIMER,0);
			if (m_evConfirmGetTimerOnOff.Lock(2000))
			{
				bState = m_bTimerOnOffState;
				bResult = TRUE;
			}
			else
				WK_TRACE_WARNING(_T("Get TimerOnOffState timeout. No confirm\n"));
			m_evConfirmGetTimerOnOff.Unlock();
		}
	}
	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CServer::SetAlarmOutputOnOffState(BOOL bState)
{
	BOOL	bResult = FALSE;

	if (IsOutputConnected())
	{
		if (m_pOutput)
		{
			m_evConfirmSetAlarmOutputOnOff.ResetEvent();
			m_pOutput->DoRequestSetValue(SECID_NO_ID,CSD_ALARM_OUTPUT_ON_OFF_STATE, (bState ? CSD_ON : CSD_OFF));
			if (m_evConfirmSetAlarmOutputOnOff.Lock(2000))
				bResult = TRUE;
			else
				WK_TRACE_WARNING(_T("Set AlarmOutputOnOffState timeout. No confirm\n"));
			m_evConfirmSetAlarmOutputOnOff.Unlock();
		}	
	}
	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CServer::GetAlarmOutputOnOffState(BOOL &bState)
{
	BOOL bResult = FALSE;
	if (IsOutputConnected())
	{
		if (m_pOutput)
		{
			m_evConfirmGetAlarmOutputOnOff.ResetEvent();
			m_pOutput->DoRequestGetValue(SECID_NO_ID,CSD_ALARM_OUTPUT_ON_OFF_STATE,0);
			if (m_evConfirmGetAlarmOutputOnOff.Lock(2000))
			{
				bState = m_bAlarmOutputOnOffState;
				bResult = TRUE;
			}
			else
				WK_TRACE_WARNING(_T("Get TimerOnOffState timeout. No confirm\n"));
			m_evConfirmGetAlarmOutputOnOff.Unlock();
		}
	}
	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CServer::SetAlarmDialingNumber(int nAlarmDialingNumber, const CString& sAlarmDialingNumber)
{
	BOOL	bResult = FALSE;

	if (IsOutputConnected())
	{
		if (m_pOutput)
		{
			CString sCSD;
			sCSD.Format(_T("%s_%d"), CSD_ALARM_DIALING_NUMBER, nAlarmDialingNumber);

			m_pOutput->DoRequestSetValue(SECID_NO_ID,sCSD, sAlarmDialingNumber);
			if (m_evConfirmSetAlarmDialingNumber[nAlarmDialingNumber].Lock(2000))
				bResult = TRUE;
			else
				WK_TRACE_WARNING(_T("Set AlarmDialingNumber timeout. No confirm\n"));
			m_evConfirmSetAlarmDialingNumber[nAlarmDialingNumber].Unlock();
		}	
	}
	return bResult;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::GetAlarmDialingNumber(int nAlarmDialingNumber, CString& sAlarmDialingNumber)
{
	BOOL bResult = FALSE;
	if (IsOutputConnected())
	{
		if (m_pOutput)
		{
			CString sCSD;
			sCSD.Format(_T("%s_%d"), CSD_ALARM_DIALING_NUMBER, nAlarmDialingNumber);

			m_evConfirmGetAlarmDialingNumber[nAlarmDialingNumber].ResetEvent();
			m_pOutput->DoRequestGetValue(SECID_NO_ID,sCSD,0);
			if (m_evConfirmGetAlarmDialingNumber[nAlarmDialingNumber].Lock(2000))
			{
				sAlarmDialingNumber = m_sAlarmDialingNumber;
				bResult = TRUE;
			}
			else
				WK_TRACE_WARNING(_T("Get AlarmDialingNumber timeout. No confirm\n"));
			m_evConfirmGetAlarmDialingNumber[nAlarmDialingNumber].Unlock();
		}
	}
	return bResult;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::SetClock(const CSystemTime& time)
{
	BOOL bResult = FALSE;
	if (IsInputConnected())
	{
		if (m_pInput)
		{
			m_evConfirmSetClock.ResetEvent();
			m_pInput->DoRequestSetSystemTime(time);
			if (m_evConfirmSetClock.Lock(2000))
				bResult = m_bConfirmSetClock;
			else
				WK_TRACE_WARNING(_T("CServer::SetClock timeout. No confirm\n"));
			m_evConfirmSetClock.Unlock();
		}
	}

	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CServer::GetClock(CSystemTime& time)
{
	BOOL bResult = FALSE;
	if (IsInputConnected())
	{
		if (m_pInput)
		{
			m_evConfirmGetClock.ResetEvent();
			m_pInput->DoRequestGetSystemTime();
			if (m_evConfirmGetClock.Lock(2000))
			{
				bResult = TRUE;
				time =	m_SystemTime;
			}
			else
				WK_TRACE_WARNING(_T("CServer::GetClock timeout. No confirm\n"));
			m_evConfirmGetClock.Unlock();
		}
	}

	return bResult;
}

//////////////////////////////////////////////////////////////////////
void CServer::ConfirmGetSystemTime(const CSystemTime& st)
{
	m_SystemTime = st;
	m_evConfirmGetClock.SetEvent();
}

//////////////////////////////////////////////////////////////////////
void CServer::ConfirmSetSystemTime(BOOL bConfirmSetClock)
{
	m_bConfirmSetClock = bConfirmSetClock;
	m_evConfirmSetClock.SetEvent();
}

//////////////////////////////////////////////////////////////////////
BOOL CServer::GetTimeZoneInformation(CTimeZoneInformation& TimeZone)
{
	BOOL bResult = FALSE;
	if (IsInputConnected())
	{
		if (m_pInput)
		{
			m_evConfirmGetTimeZone.ResetEvent();
			m_pInput->DoRequestGetTimeZoneInformation();
			if (m_evConfirmGetTimeZone.Lock(8000))
			{
				TimeZone = m_TimeZone;
				bResult = TRUE;
			}
			else
				WK_TRACE_WARNING(_T("CServer::GetTimeZoneInformation timeout. No confirm\n"));
			m_evConfirmGetTimeZone.Unlock();
		}
	}

	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CServer::SetTimeZoneInformation(const CTimeZoneInformation& TimeZone)
{
	BOOL bResult = FALSE;
	if (IsInputConnected())
	{
		if (m_pInput)
		{
			m_evConfirmSetTimeZone.ResetEvent();
			m_pInput->DoRequestSetTimeZoneInformation(TimeZone);
			if (m_evConfirmSetTimeZone.Lock(2000))
				bResult = TRUE;
			else
				WK_TRACE_WARNING(_T("CServer::SetTimeZoneInformation timeout. No confirm\n"));
			m_evConfirmSetTimeZone.Unlock();
		}
	}

	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CServer::GetTimeZoneInformations(CTimeZoneInformations& TimeZones)
{
	BOOL bResult = FALSE;
	if (IsInputConnected())
	{
		if (m_pInput)
		{
			m_evConfirmGetTimeZones.ResetEvent();
			m_pInput->DoRequestGetTimeZoneInformations();
			if (m_evConfirmGetTimeZones.Lock(8000))
			{
				bResult = TRUE;
				TimeZones = m_TimeZones;
			}
			else
				WK_TRACE_WARNING(_T("CServer::GetTimeZoneInformations timeout. No confirm\n"));
			m_evConfirmGetTimeZones.Unlock();
		}
	}

	return bResult;
}

//////////////////////////////////////////////////////////////////////
void CServer::ConfirmGetTimeZone(const CTimeZoneInformation& TimeZone)
{
	m_TimeZone = TimeZone;
	m_evConfirmGetTimeZone.SetEvent();
}

//////////////////////////////////////////////////////////////////////
void CServer::ConfirmSetTimeZone()
{
	m_evConfirmSetTimeZone.SetEvent();
}

//////////////////////////////////////////////////////////////////////
void CServer::ConfirmGetTimeZones(const CTimeZoneInformations& TimeZones)
{
	m_TimeZones = TimeZones;
	m_evConfirmGetTimeZones.SetEvent();
}
//////////////////////////////////////////////////////////////////////
int CServer::GetNrOfVideoOut()
{
	return m_iNrOfVideoOut;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::GetVideoOutputMode(LPCTSTR sCSDPort, enumVideoOutputMode& eVideoOutputMode)
{
	BOOL bResult = FALSE;
	if (IsInputConnected())
	{
		if (m_pInput)
		{
			m_evConfirmGetVideoOutputMode.ResetEvent();
			m_pInput->DoRequestGetValue(SECID_NO_ID,sCSDPort,0);
			if (m_evConfirmGetVideoOutputMode.Lock(2000))
			{
				eVideoOutputMode = m_eVideoOutputMode;
				bResult = TRUE;
			}
			else
			{
				WK_TRACE_WARNING(_T("CServer::GetVideoOutputMode %s timeout. No confirm\n"), sCSDPort);
			}
			m_evConfirmGetVideoOutputMode.Unlock();
		}
	}

	return bResult;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::SetVideoOutputMode(LPCTSTR sCSDPort, enumVideoOutputMode& eVideoOutputMode)
{
	BOOL bResult = FALSE;
	CString sValue = CSD_VOUT_OFF;
	switch (eVideoOutputMode)
	{
		case VOUT_ACTIVITY:
			sValue = CSD_VOUT_ACTIVITY;
			break;
		case VOUT_CLIENT:
			sValue = CSD_VOUT_CLIENT;
			break;
		case VOUT_SEQUENCE:
			sValue = CSD_VOUT_SEQUENCE;
			break;
		default:
			sValue = CSD_VOUT_OFF;
			break;
	}
	if (IsInputConnected())
	{
		if (m_pInput)
		{
			m_evConfirmSetVideoOutputMode.ResetEvent();
			m_pInput->DoRequestSetValue(SECID_NO_ID,sCSDPort,sValue,0);
			if (m_evConfirmSetVideoOutputMode.Lock(2000))
			{
				eVideoOutputMode = m_eVideoOutputMode;
				bResult = TRUE;
			}
			else
			{
				WK_TRACE_WARNING(_T("CServer::SetVideoOutputMode %s timeout. No confirm\n"), sCSDPort);
			}
			m_evConfirmSetVideoOutputMode.Unlock();
		}
	}

	return bResult;
}
//////////////////////////////////////////////////////////////////////
void CServer::ConfirmGetVideoOutputMode(const CString& sPort,const CString& sMode)
{
	if (sMode == CSD_VOUT_ACTIVITY)
		m_eVideoOutputMode = VOUT_ACTIVITY;
	else if (sMode == CSD_VOUT_CLIENT)
		m_eVideoOutputMode = VOUT_CLIENT;
	else if (sMode == CSD_VOUT_SEQUENCE)
		m_eVideoOutputMode = VOUT_SEQUENCE;
	else
	{
		WK_TRACE_WARNING(_T("CServer::ConfirmGetVideoOutputMode %s UNKNOWN Mode %s\n"), sPort, sMode);
		m_eVideoOutputMode = VOUT_UNKNOWN;
	}
	m_evConfirmGetVideoOutputMode.SetEvent();
}
//////////////////////////////////////////////////////////////////////
void CServer::ConfirmSetVideoOutputMode(const CString& sPort,const CString& sMode)
{
	if (sMode == CSD_VOUT_ACTIVITY)
		m_eVideoOutputMode = VOUT_ACTIVITY;
	else if (sMode == CSD_VOUT_CLIENT)
		m_eVideoOutputMode = VOUT_CLIENT;
	else if (sMode == CSD_VOUT_SEQUENCE)
		m_eVideoOutputMode = VOUT_SEQUENCE;
	else
	{
		WK_TRACE_WARNING(_T("CServer::ConfirmSetVideoOutputMode %s UNKNOWN Mode %s\n"), sPort, sMode);
		m_eVideoOutputMode = VOUT_UNKNOWN;
	}
	m_evConfirmSetVideoOutputMode.SetEvent();
	if (   theApp.IsTransmitter()
		&& !theApp.IsReadOnly()
		&& (m_eVideoOutputMode == VOUT_CLIENT)
		)
	{
		// switch to active camera
		CPanel* pPanel = theApp.GetPanel();
		if (pPanel)
		{
			pPanel->SetCamera(pPanel->GetActiveCamera(), Live);
		}
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::SetExpansionCode(const CString& sExpCode)
{
	BOOL	bResult = FALSE;

	if (IsInputConnected())
	{
		if (m_pInput)
		{
			m_pInput->DoRequestSetValue(SECID_NO_ID,CSD_EXPANSION_CODE, sExpCode);
			if (m_evConfirmSetExpansionCode.Lock(2000))
			{
				bResult = TRUE;
			}
			else
			{
				WK_TRACE_WARNING(_T("Set ExpansionCode timeout. No confirm\n"));
			}
			m_evConfirmSetExpansionCode.Unlock();
		}	
	}
	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CServer::OnBeginConfig()
{
	if (IsInputConnected())
	{
		m_pInput->DoRequestSetValue(SECID_NO_ID,CSD_DV_CONFIG, CSD_ON);
	}
	if (IsOutputConnected())
	{
		m_pOutput->DoRequestSetValue(SECID_NO_ID,CSD_DV_CONFIG, CSD_ON);
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::OnEndConfig()
{
	if (IsInputConnected())
	{
		m_pInput->DoRequestSetValue(SECID_NO_ID,CSD_DV_CONFIG, CSD_OFF);
	}
	if (IsOutputConnected())
	{
		m_pOutput->DoRequestSetValue(SECID_NO_ID,CSD_DV_CONFIG, CSD_OFF);
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
void CServer::ConfirmGetValue(const CString &sKey,const CString &sValue)
{
//	TRACE(_T("CServer::ConfirmGetValue %s,%s\n"),sKey,sValue);
	if (sKey == CSD_PIN_SUPERVISOR)
	{
		m_sSupervisorPIN = sValue;
		m_evConfirmGetPIN.SetEvent();
	}
	else if (sKey == CSD_PIN_OPERATOR)
	{
		m_sOperatorPIN = sValue;
		m_evConfirmGetPIN.SetEvent();
	}
	else if (sKey == CSD_VOUT_COUNT)
	{
		DWORD dwScan = 0;
		if (1==_stscanf(sValue,_T("%d"),&dwScan))
		{
			m_iNrOfVideoOut = dwScan;
		}
	}
	else if (sKey == CSD_DETECTOR_IGNORE_TIMER)
	{
		if (sValue == CSD_ON)
			m_bTimerOnOffState = TRUE;	
		else if (sValue == CSD_OFF)
			m_bTimerOnOffState = FALSE;	
		else
			WK_TRACE_WARNING(_T("CServer::ConfirmGetValue for CSD_DETECTOR_IGNORE_TIMER unknown sValue <%s>\n"), sValue);
		m_evConfirmGetTimerOnOff.SetEvent();
	}
	else if (   sKey == CSD_PORT0
			 || sKey == CSD_PORT1
			 || sKey == CSD_PORT2
			 || sKey == CSD_PORT3
			 )
	{
		ConfirmGetVideoOutputMode(sKey, sValue);
	}
	else if (sKey == CSD_ALARM_OUTPUT_ON_OFF_STATE)
	{
		if (sValue == CSD_ON)
			m_bAlarmOutputOnOffState = TRUE;	
		else if (sValue == CSD_OFF)
			m_bAlarmOutputOnOffState = FALSE;	
		else
			WK_TRACE_WARNING(_T("CServer::ConfirmGetValue for CSD_ALARM_OUTPUT_ON_OFF_STATE unknown sValue <%s>\n"), sValue);
		m_evConfirmGetAlarmOutputOnOff.SetEvent();
	}
	else if (sKey.Find(CSD_ALARM_DIALING_NUMBER) != -1)
	{
		// Es sind maximal 10 Rufnummern möglich (0...9), da nur die letzte Stelle
		// des Strings interpretiert wird.
		int nCallingNumber = _ttoi(sKey.Right(1));
		nCallingNumber = max(min(nCallingNumber, 9), 0);
		m_sAlarmDialingNumber=sValue;
		m_evConfirmGetAlarmDialingNumber[nCallingNumber].SetEvent();
	}
	else if (0==sKey.CompareNoCase(_T("GetFileVersion")))
	{
		SetVersion(sValue);
	}
	else if (0==sKey.CompareNoCase(CSD_SYSTEM_INFO))
	{
		m_sInfo = sValue;
		m_evConfirmGetSystemInfo.SetEvent();
	}
	else if(0==sKey.CompareNoCase(CSD_RESOLUTION))
	{
		m_nResolution = EnumOfName(sValue);
		m_evConfirmGetResolution.SetEvent();
	}
	else if(0==sKey.CompareNoCase(CSD_CARD_TYPE))
	{
		if (sValue == CSD_CARD_TYPE_JACOB)
		{
			m_nCardType = CARD_TYPE_JACOB;
		}
		else if (sValue == CSD_CARD_TYPE_SAVIC)
		{
			m_nCardType = CARD_TYPE_SAVIC;
		}
		else if (sValue == CSD_CARD_TYPE_TASHA)
		{
			m_nCardType = CARD_TYPE_TASHA;
		}
		else if (sValue == CSD_CARD_TYPE_Q)
		{
			m_nCardType = CARD_TYPE_Q;
		}
		else
			m_nCardType = CARD_TYPE_UNKNOWN;
			
		m_evConfirmGetCardType.SetEvent();
	}
	else if (sKey == CSD_FPS)
	{
		CMainFrame* pMF = theApp.GetMainFrame();
		pMF->SetUnitFps(_ttoi(sValue));
	}
	else if (sKey == CSD_AGC)
	{
		m_nSystemAGC =   _ttoi(sValue);
		m_evConfirmGetSystemAGC.SetEvent();
	}
	else if (sKey == CSD_VIDEO_FORMAT)
	{
		m_nSystemVideoformat =   _ttoi(sValue);
		m_evConfirmGetSystemVideoformat.SetEvent();
	}
	else if (sKey == CSD_VIDEO_NORM)
	{
		m_sSystemVideonorm =   sValue;
		m_evConfirmGetSystemVideonorm.SetEvent();
	}
	else
	{
		WK_TRACE_WARNING(_T("CServer::ConfirmGetValue Unknown sKey <%s>\n"), sKey);
	}

}
//////////////////////////////////////////////////////////////////////
void CServer::ConfirmSetValue(const CString &sKey,const CString &sValue)
{
//	TRACE(_T("CServer::ConfirmSetValue %s,%s\n"),sKey,sValue);
	if ((sKey == CSD_PIN_SUPERVISOR) || (sKey == CSD_PIN_OPERATOR))
	{
		m_evConfirmSetPIN.SetEvent();
	}
	else if (sKey == CSD_DETECTOR_IGNORE_TIMER)
	{
		if (sValue == CSD_ON)
			m_bTimerOnOffState = TRUE;	
		else if (sValue == CSD_OFF)
			m_bTimerOnOffState = FALSE;	
		else
			WK_TRACE_WARNING(_T("CServer::ConfirmSetValue for CSD_DETECTOR_IGNORE_TIMER unknown sValue <%s>\n"), sValue);
		m_evConfirmSetTimerOnOff.SetEvent();
	}
	else if (   sKey == CSD_PORT0
			 || sKey == CSD_PORT1
			 || sKey == CSD_PORT2
			 || sKey == CSD_PORT3
			 )
	{
		ConfirmSetVideoOutputMode(sKey, sValue);
	}
	else if (sKey == CSD_ALARM_OUTPUT_ON_OFF_STATE)
	{
		if (sValue == CSD_ON)
			m_bAlarmOutputOnOffState = TRUE;	
		else if (sValue == CSD_OFF)
			m_bAlarmOutputOnOffState = FALSE;	
		else
			WK_TRACE_WARNING(_T("CServer::ConfirmSetValue for CSD_ALARM_OUTPUT_ON_OFF_STATE unknown sValue <%s>\n"), sValue);
		m_evConfirmSetAlarmOutputOnOff.SetEvent();
	}
	else if (sKey.Find(CSD_ALARM_DIALING_NUMBER) != -1)
	{
		// Es sind maximal 10 Rufnummern möglich (0...9), da nur die letzte Stelle
		// des Strings interpretiert wird.
		int nCallingNumber = _ttoi(sKey.Right(1));
		nCallingNumber = max(min(nCallingNumber, 9), 0);
		m_sAlarmDialingNumber=sValue;
		m_evConfirmSetAlarmDialingNumber[nCallingNumber].SetEvent();
	}
	else if (0 == sKey.CompareNoCase(CSD_EXPANSION_CODE))
	{
		m_evConfirmSetExpansionCode.SetEvent();
	}
	else if (0 == sKey.CompareNoCase(CSD_RESOLUTION))
	{
		m_nResolution = _ttoi(sValue);
		m_evConfirmSetResolution.SetEvent();
	}
	else if (0 == sKey.CompareNoCase(CSD_AGC))
	{
		if (sValue == CSD_ON)
			m_nSystemAGC = VIDEO_AGC_ON;
		else if (sValue == CSD_OFF)
			m_nSystemAGC = VIDEO_AGC_OFF;
		m_evConfirmSetSystemAGC.SetEvent();
	}
	else if (0 == sKey.CompareNoCase(CSD_VIDEO_FORMAT))
	{
		m_nSystemVideoformat = _ttoi(sValue);
		m_evConfirmSetSystemVideoformat.SetEvent();
	}
	else if (0 == sKey.CompareNoCase(CSD_VIDEO_NORM))
	{
		m_sSystemVideonorm = sValue;
		m_evConfirmSetSystemVideonorm.SetEvent();
	}
	else
	{
		WK_TRACE_WARNING(_T("CServer::ConfirmSetValue Unknown sKey <%s>\n"), sKey);
	}
}

//////////////////////////////////////////////////////////////////////
BOOL CServer::IsDataCarrierConnected()
{
	BOOL bRet = FALSE;

	Lock();
	bRet = (m_pDataCarrier!=NULL) && (m_pDataCarrier->GetIPCState()==CIPC_STATE_CONNECTED);
	Unlock();

	return bRet;
}

//////////////////////////////////////////////////////////////////////
BOOL CServer::IsDataCarrierConnecting()
{
	BOOL bRet = FALSE;

	Lock();
	bRet = (m_pDataCarrier!=NULL) && (m_pDataCarrier->GetIPCState()==CIPC_STATE_WRITE_CREATED);
	Unlock();

	return bRet;
}

//////////////////////////////////////////////////////////////////////
CString CServer::GetInfo()
{
	if (IsInputConnected())
	{
		if (m_pInput)
		{
			m_evConfirmGetSystemInfo.ResetEvent();
			m_pInput->DoRequestGetValue(SECID_NO_ID,CSD_SYSTEM_INFO,0);
			if (!m_evConfirmGetSystemInfo.Lock(2000))
			{
				WK_TRACE_WARNING(_T("Get System Info timeout. No confirm\n"));
			}
			m_evConfirmGetSystemInfo.Unlock();
		}
	}
	return m_sInfo;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::GetResolution(int &nResolution)
{
	BOOL bResult = FALSE;
	if (IsOutputConnected())
	{
		if (m_pOutput)
		{
			m_evConfirmGetResolution.ResetEvent();
			m_pOutput->DoRequestGetValue(SECID_NO_ID,CSD_RESOLUTION,0);
			if (!m_evConfirmGetResolution.Lock(2000))
			{
				WK_TRACE_WARNING(_T("Get resolution timeout. No confirm\n"));
			}
			else
			{
				nResolution = m_nResolution;
				bResult = TRUE;
			}
			m_evConfirmGetResolution.Unlock();
		}
	}
	return bResult;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::SetResolution(int nResolution)
{
	BOOL bResult = FALSE;
	if (IsOutputConnected())
	{
		if (m_pOutput)
		{
			CString sValue;
			sValue.Format(_T("%d"), nResolution);
			m_evConfirmSetResolution.ResetEvent();
			m_pOutput->DoRequestSetValue(SECID_NO_ID,CSD_RESOLUTION,sValue);

			if (!m_evConfirmSetResolution.Lock(2000))
			{
				WK_TRACE_WARNING(_T("Set resolution timeout. No confirm\n"));
			}
			else
			{
				bResult = TRUE;
			}
			m_evConfirmSetResolution.Unlock();
		}
	}
	return bResult;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::GetCardType(int nCamNr, int &nCardType)
{
	BOOL bResult = FALSE;
	if (IsOutputConnected())
	{
		if (m_pOutput)
		{
			if (m_pOutput->GetNrOfCameras() >= nCamNr)
			{
				CSecID camID = m_pOutput->GetCamIDFromNr((WORD)nCamNr);
				if (camID != SECID_NO_ID)
				{
					m_evConfirmGetCardType.ResetEvent();
					m_pOutput->DoRequestGetValue(camID, CSD_CARD_TYPE,0);
					if (!m_evConfirmGetCardType.Lock(2000))
					{
						WK_TRACE_WARNING(_T("GetCardType timeout. No confirm\n"));
					}
					else
					{
						nCardType = m_nCardType;
						bResult = TRUE;
					}
					m_evConfirmGetCardType.Unlock();		
				}
			}
		}
	}
	return bResult;
}

//////////////////////////////////////////////////////////////////////////
BOOL CServer::IsLocal()
{
	return m_sHostNumber == LOCAL_LOOP_BACK;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::IsLowBandwidth()
{
	BOOL bRet = FALSE;

	if (IsOutputConnected())
	{
//		bRet = m_pOutput->GetBitrate() <= 128*1024;
	}

	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::GetSystemAGC(BOOL& nSystemAGC, int nCamNr)
{
	BOOL bResult = FALSE;
	if (IsOutputConnected())
	{
		if (   m_pOutput
			&& m_pOutput->GetNrOfCameras() >= nCamNr)
		{
			CSecID camID = m_pOutput->GetCamIDFromNr((WORD)nCamNr);
			if (camID != SECID_NO_ID)
			{
				m_evConfirmGetSystemAGC.ResetEvent();
				m_pOutput->DoRequestGetValue(camID,CSD_AGC,0);
				if (!m_evConfirmGetSystemAGC.Lock(2000))
				{
					WK_TRACE_WARNING(_T("Get AGC timeout. No confirm\n"));
				}
				else
				{
					nSystemAGC = m_nSystemAGC;
					bResult = TRUE;
				}
			}
		}

		m_evConfirmGetSystemAGC.Unlock();
	}
	return bResult;	
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::SetSystemAGC(BOOL bAGC, int nCamNr)
{
	BOOL bResult = FALSE;
	if (IsOutputConnected())
	{
		if (   m_pOutput
			&& m_pOutput->GetNrOfCameras() >= nCamNr)
		{
			CSecID camID = m_pOutput->GetCamIDFromNr((WORD)nCamNr);
			if (camID != SECID_NO_ID)
			{
				CString sValue;
				sValue.Format(_T("%d"), bAGC);
				m_evConfirmSetSystemAGC.ResetEvent();
				m_pOutput->DoRequestSetValue(camID,CSD_AGC,sValue);

				if (!m_evConfirmSetSystemAGC.Lock(2000))
				{
					WK_TRACE_WARNING(_T("Set AGC timeout. No confirm\n"));
				}
				else
				{
					bResult = TRUE;
				}
			}
		}
		m_evConfirmSetSystemAGC.Unlock();
	}
	return bResult;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::GetSystemVideoformat(int& nSystemVideoformat, int nCamNr)
{
	BOOL bResult = FALSE;
	if (IsOutputConnected())
	{
		if (   m_pOutput
			&& m_pOutput->GetNrOfCameras() >= nCamNr)
		{
			CSecID camID = m_pOutput->GetCamIDFromNr((WORD)nCamNr);
			if (camID != SECID_NO_ID)
			{
				m_evConfirmGetSystemVideoformat.ResetEvent();
				m_pOutput->DoRequestGetValue(camID,CSD_VIDEO_FORMAT,0);
				if (!m_evConfirmGetSystemVideoformat.Lock(2000))
				{
					WK_TRACE_WARNING(_T("Get Videoformat timeout. No confirm\n"));
				}
				else
				{
					nSystemVideoformat = m_nSystemVideoformat;
					bResult = TRUE;
				}
			}
		}
		m_evConfirmGetSystemVideoformat.Unlock();
	}
	return bResult;	
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::SetSystemVideoformat(int nSystemVideoformat, int nCamNr)
{
	BOOL bResult = FALSE;
	if (IsOutputConnected())
	{
		if (   m_pOutput
			&& m_pOutput->GetNrOfCameras() >= nCamNr)
		{
			CSecID camID = m_pOutput->GetCamIDFromNr((WORD)nCamNr);
			if (camID != SECID_NO_ID)
			{
				CString sValue;
				sValue.Format(_T("%d"), nSystemVideoformat);
				m_evConfirmSetSystemVideoformat.ResetEvent();
				m_pOutput->DoRequestSetValue(camID,CSD_VIDEO_FORMAT,sValue);

				if (!m_evConfirmSetSystemVideoformat.Lock(2000))
				{
					WK_TRACE_WARNING(_T("Set Videoformat timeout. No confirm\n"));
				}
				else
				{
					bResult = TRUE;
				}
			}
		}
		m_evConfirmSetSystemVideoformat.Unlock();
	}
	return bResult;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::GetSystemVideonorm(CString& sSystemVideonorm, int nCamNr)
{
	BOOL bResult = FALSE;
	if (IsOutputConnected())
	{
		if (   m_pOutput
			&& m_pOutput->GetNrOfCameras() >= nCamNr)
		{
			CSecID camID = m_pOutput->GetCamIDFromNr((WORD)nCamNr);
			if (camID != SECID_NO_ID)
			{
				m_evConfirmGetSystemVideonorm.ResetEvent();
				m_pOutput->DoRequestGetValue(camID,CSD_VIDEO_NORM,0);
				if (!m_evConfirmGetSystemVideonorm.Lock(2000))
				{
					WK_TRACE_WARNING(_T("Get Videonorm timeout. No confirm\n"));
				}
				else
				{
					sSystemVideonorm = m_sSystemVideonorm;
					bResult = TRUE;
				}
			}
		}
		m_evConfirmGetSystemVideonorm.Unlock();
	}
	return bResult;	
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::SetSystemVideonorm(CString sSystemVideonorm, int nCamNr)
{
	BOOL bResult = FALSE;
	if (IsOutputConnected())
	{
		if (   m_pOutput
			&& m_pOutput->GetNrOfCameras() >= nCamNr)
		{
			CSecID camID = m_pOutput->GetCamIDFromNr((WORD)nCamNr);
			if (camID != SECID_NO_ID)
			{
				m_evConfirmSetSystemVideonorm.ResetEvent();
				m_pOutput->DoRequestSetValue(camID,CSD_VIDEO_NORM,sSystemVideonorm);
				//CSD_PAL, CSD_NTSC als sValue

				if (!m_evConfirmSetSystemVideonorm.Lock(2000))
				{
					WK_TRACE_WARNING(_T("Set Videonorm timeout. No confirm\n"));
				}
				else
				{
					bResult = TRUE;
				}
			}
		}
		m_evConfirmSetSystemVideonorm.Unlock();
	}
	return bResult;
}
//////////////////////////////////////////////////////////////////////
#ifdef _UNICODE
WORD CServer::GetInputCodePage()
{
	if (m_pInputResult)
	{
		return m_pInputResult->GetCodePage();
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////
WORD CServer::GetOutputCodePage()
{
	if (m_pOutputResult)
	{
		return m_pOutputResult->GetCodePage();
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////
WORD CServer::GetDataBaseCodePage()
{
	if (m_pDatabaseResult)
	{
		return m_pDatabaseResult->GetCodePage();
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////
WORD CServer::GetDataCarrierCodePage()
{
	if (m_pDataCarrierResult)
	{
		return m_pDataCarrierResult->GetCodePage();
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////
WORD CServer::GetAudioCodePage()
{
	if (m_pAudioResult)
	{
		return m_pAudioResult->GetCodePage();
	}
	return 0;
}
#endif

