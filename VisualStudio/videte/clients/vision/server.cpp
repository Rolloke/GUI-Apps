// Server.cpp: implementation of the CServer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Vision.h"
#include "mainfrm.h"
#include "VisionDoc.h"
#include "CIPCServerControlVision.h"
#include "CIPCControlAudioUnit.h"
#include "ObjectView.h"

#include "Server.h"

#include "ConnectingDialog.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// for DEMO Version
static _TCHAR szSerials[5][10] = {_T("911171851"),_T("941191352"),_T("941131958"),_T("961171356"),_T("971111863")};
int CServer::m_iSerialCounter = 0;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CServer::CServer(CVisionDoc* pDoc, CServers* pServers, WORD wID)
{
	m_pDoc = pDoc;
	m_pServers = pServers;
	m_wID = wID;

	// m_sName
	m_pInput  = NULL;
	m_pOutput = NULL;
	m_pControl  = NULL;
	m_pAudio = NULL;

	m_bLocal = FALSE;
	m_bAlarm = FALSE;
	m_bAlarmSwitch = FALSE;
	m_bControlConfirmed = FALSE;
	m_bControlSave = FALSE;
	m_bControlExclusive = FALSE;
	m_bIsDTS = FALSE;
	m_bShowErrorMessage = TRUE;

	m_pConnectDialog = NULL;
	m_pConnectionRecord = NULL;
	m_pConnectThread = NULL;
	m_pInputResult = NULL;
	m_pOutputResult = NULL;
	m_pAudioResult  = NULL;
	m_bConnectingCancelled = FALSE;
	m_dwConfirmedOutputOptions = 0;

	if (theApp.m_pDongle->IsDemo())
	{
		m_AutoLogout.SetValues(TRUE,TRUE,10,0,10,0);
	}
	else
	{
		CWK_Profile wkp;
		m_AutoLogout.Load(wkp);
	}
}
//////////////////////////////////////////////////////////////////////
CServer::~CServer()
{
	if (m_pConnectThread)
	{
		if (!m_Fetch.IsCancelled())
		{
			m_Fetch.Cancel();
		}
		WaitForSingleObject(m_pConnectThread->m_hThread,5*1000);
	}
	CIPCAudioVision *pLocalAudio = m_pDoc->GetLocalAudio();
	if (pLocalAudio)
	{
		if (pLocalAudio == m_pAudio)
		{
			m_pAudio = NULL;
		}
		pLocalAudio->SetServer(NULL);
	}

	WK_DELETE(m_pConnectThread);

	WK_DELETE(m_pInput);
	WK_DELETE(m_pOutput);
	WK_DELETE(m_pControl);
	WK_DELETE(m_pAudio);

	WK_DELETE(m_pConnectionRecord);
	if (m_pConnectDialog)
	{
		WK_TRACE_ERROR(_T("m_pConnectDialog not NULL in CServer::~CServer\n"));
	}
	WK_DESTROY_WINDOW(m_pConnectDialog);
	WK_DELETE(m_pInputResult);
	WK_DELETE(m_pOutputResult);
	WK_DELETE(m_pAudioResult);
}
//////////////////////////////////////////////////////////////////////
CString CServer::GetFullName() const
{
	CString r;

	r = m_sName;

	if (!m_sKind.IsEmpty())
	{
		r += _T(" (") + m_sKind +_T(")");
	}

	if (IsAlarm())
	{
		CString a;
		a.LoadString(IDS_ALARM);
		r += _T(" ") + a;
	}
	return r;
}
//////////////////////////////////////////////////////////////////////
void CServer::Lock()
{
	m_csSection.Lock();
}
//////////////////////////////////////////////////////////////////////
void CServer::Unlock()
{
	m_csSection.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CServer::OnInputConnected()
{
	if (m_pInput)
	{
		m_pDoc->ActualizeInput(m_pInput);
		ConfirmControlConnection();
	}
}
//////////////////////////////////////////////////////////////////////
void CServer::OnInputDisconnected()
{
	m_pServers->OnIdle();
}
//////////////////////////////////////////////////////////////////////
void CServer::OnOutputConnected()
{
	if (m_pOutput)
	{
		m_pDoc->ActualizeOutput(m_pOutput);
		ConfirmControlConnection();
	}
}
//////////////////////////////////////////////////////////////////////
void CServer::OnOutputDisconnected()
{
	m_pServers->OnIdle();
}
//////////////////////////////////////////////////////////////////////
void CServer::OnControlConnected()
{
	ConfirmControlConnection();
}
//////////////////////////////////////////////////////////////////////
void CServer::OnControlDisconnected()
{
	if (IsControl())
	{
		if (IsControlDisconnected())
		{
			if (   m_pConnectDialog
				&& m_pConnectThread
				&& (m_pInputResult == NULL))
			{
				// Verbindungsaufbau abbrechen
				TRACE("Cancel control connecting %s\n",m_sName);
				m_pConnectDialog->Cancel();
			}
			else if (IsConnected())
			{
				// wir sind verbunden
				if (IsControlExclusive())
				{
					// Verbindung trennen
					CMainFrame* pMF = (CMainFrame*)AfxGetMainWnd();
					if (WK_IS_WINDOW(pMF))
					{
						pMF->PostMessage(WM_COMMAND,m_wID+ID_DISCONNECT_LOW,0L);
					}
				}
				else
				{
					// von control zu normaler Verbindung
					WK_DELETE(m_pControl);
				}
			}
			else
			{
				// Aufbau von Input und Outputverbindung wieder unterbrechen
				// Achtung dieses Server Object wird dabei gelöscht (delete this!)
				m_pServers->Disconnect(GetID());
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CServer::OnAudioConnected()
{
	if (m_pAudio)
	{
		m_pDoc->ActualizeAudio(m_pAudio);
	}
}
//////////////////////////////////////////////////////////////////////
void CServer::OnAudioDisconnected()
{
	m_pServers->OnIdle();
}
//////////////////////////////////////////////////////////////////////
void CServer::OnInputFetchResult()
{
	Lock();
	if (m_pInputResult)
	{
		ConnectInput();
	}
	else
	{
		m_pServers->Trace();
		WK_TRACE_ERROR(_T("OnInputFetchResult m_pOutputResult is null %s\n"), m_sName);
	}
	Unlock();
}
//////////////////////////////////////////////////////////////////////
void CServer::OnOutputFetchResult()
{
	Lock();
	if (m_pOutputResult!=NULL)
	{
		ConnectOutput();
	}
	else
	{
		m_pServers->Trace();
		WK_TRACE_ERROR(_T("OnOutputFetchResult m_pOutputResult is null %s\n"),m_sName);
	}
	Unlock();
}
//////////////////////////////////////////////////////////////////////////
void CServer::OnAudioFetchResult()
{
	Lock();
	if (m_pAudioResult!=NULL)
	{
		ConnectAudio();
	}
	else
	{
		m_pServers->Trace();
		WK_TRACE_ERROR(_T("OnAudioFetchResult m_pAudioResult is null %s\n"),m_sName);
	}
	Unlock();
}
//////////////////////////////////////////////////////////////////////////
void CServer::OnConnectThreadEnd()
{
	if (m_pConnectThread)
	{
		WaitForSingleObject(m_pConnectThread->m_hThread,10);
	}
	WK_DELETE(m_pConnectThread);
}
//////////////////////////////////////////////////////////////////////
void CServer::CheckAutoLogout()
{
	if (IsConnected())
	{
		CMainFrame* pMF = (CMainFrame*)AfxGetMainWnd();
		// actualize GUI
		if (m_AutoLogout.IsConnectionOver())
		{
			if (theApp.m_pDongle->IsDemo())
			{
				pMF->PostMessage(WM_COMMAND,m_wID+ID_DISCONNECT_LOW,0L);
			}
			else
			{
				COemGuiApi::AutoDisconnectDialog(pMF,&m_AutoLogout,
					WM_COMMAND,m_wID+ID_DISCONNECT_LOW,0L);
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CServer::ConfirmControlConnection()
{
	if (IsControl())
	{
		if (IsConnected())
		{
			if (!m_bControlConfirmed)
			{
				m_pDoc->GetCIPCServerControlVision()->DoConfirmAlarmConnection(m_pInput->GetShmName(),
																			   m_pOutput->GetShmName(),
																			   _T(""),
																			   _T(""),
																			   0
#ifdef _UNICODE
																			   , MAKELONG(CODEPAGE_UNICODE, 0)
#endif
																				);
				Sleep(10);

				m_bControlConfirmed = TRUE;
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::IsConnected() const
{
	if (m_pInput && m_pOutput)
	{
		if ((m_pInput->GetIPCState()==CIPC_STATE_CONNECTED) &&
			(m_pOutput->GetIPCState()==CIPC_STATE_CONNECTED))
		{
			return TRUE;
		}
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::IsDisconnected() const
{
	if (m_pInput && m_pOutput)
	{
		if (   m_pInput->GetIsMarkedForDelete()
			|| m_pOutput->GetIsMarkedForDelete()
			)
		{
			return TRUE;
		}
	}
	else if (m_pInput && m_pInput->GetIsMarkedForDelete())
	{
		return TRUE;
	}
	else if (m_pOutput && m_pOutput->GetIsMarkedForDelete())
	{
		return TRUE;
	}
	else if (m_bConnectingCancelled)
	{
		if (m_pConnectThread == NULL)
		{
			return TRUE;
		}
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::IsControlConnected() const
{
	if (IsControl() && m_pControl)
	{
		if (m_pControl->GetIPCState()==CIPC_STATE_CONNECTED)
		{
			return TRUE;
		}
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::IsControlDisconnected() const
{
	if (IsControl() && m_pControl)
	{
		if (m_pControl->GetIsMarkedForDelete())
		{
			return TRUE;
		}
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::IsFetching() const
{
	return m_pConnectThread != NULL;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::IsConnectionFailed()
{
	BOOL bRet = FALSE;
	Lock();
	if (   m_pInputResult
		&& !m_pInputResult->IsOkay())
	{
		bRet = TRUE;
	}
	if (   m_pOutputResult
		&& !m_pOutputResult->IsOkay())
	{
		bRet = TRUE;
	}
	Unlock();
	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::FindOutAlarmHost()
{
	// try to get host id
	CHostArray& hostArray = theApp.GetHosts();
	for (int hi=0;hi<hostArray.GetSize() && m_idHost==SECID_NO_ID;hi++) 
	{
		if (0==m_sName.CompareNoCase(hostArray[hi]->GetName())) 
		{
			m_idHost = hostArray[hi]->GetID();
			m_sKind = hostArray[hi]->GetTyp();
			WK_TRACE(_T("host ID=%lx for Alarm %s\n"),m_idHost.GetID(),(LPCTSTR)m_sName);
			m_sNumber = hostArray[hi]->GetCleanedUpNumber();
			if (hostArray[hi]->GetAutoDisconnectTime()>0)
			{
				// nur die GetAutoDisconnectTime ändern!
				m_AutoLogout.SetValues(m_AutoLogout.GetAutoApplication(),
									   TRUE,
									   m_AutoLogout.GetApplicationLogoutTime(),
									   m_AutoLogout.GetApplicationConfirmTime(),
									   hostArray[hi]->GetAutoDisconnectTime(),
									   m_AutoLogout.GetConnectionConfirmTime());
			}
			return TRUE;
		}
	}
	if (0==m_sName.CompareNoCase(hostArray.GetLocalHost()->GetName())) 
	{
		m_idHost = hostArray.GetLocalHost()->GetID();
		m_sNumber = LOCAL_LOOP_BACK;
		WK_TRACE(_T("host ID=%lx for Alarm %s\n"),m_idHost.GetID(),(LPCTSTR)m_sName);
		return TRUE;
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
void CServer::AddAlarmID(CSecID id)
{
}
//////////////////////////////////////////////////////////////////////
void CServer::DeleteRecentAlarm(CSecID id)
{
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::AlarmConnection(const CConnectionRecord& c, CIPCServerControlVision* pControl)
{
	CSecID  idInput,idOutput;

	m_sName		= c.GetSourceHost();
	m_bAlarm	= TRUE;
	m_AlarmID	= c.GetCamID();
	m_dwConfirmedOutputOptions = c.GetOptions();
	m_bShowErrorMessage = FALSE;

	m_bAlarmSwitch = theApp.m_bFirstCam;

	// the alarm notification url
	c.GetFieldValue(NM_PARAMETER,m_sNotificationURL);
	// replace slash with backslashm_sAlarmURL
	m_sNotificationURL = StringReplace(m_sNotificationURL, _T('/'), _T('\\'));

	// the alarm notification
	c.GetFieldValue(NM_MESSAGE,m_sNotificationMessage);

	FindOutAlarmHost();
	// GF todo Wenn der Host unbekannt ist
	// RTE als Info, dass Bilder nicht gespeichert werden?

	CString sAdress;
	if (c.GetFieldValue(CRF_MSN,sAdress))
	{
		m_sKind = _T("ISDN");
	}
	else if (c.GetFieldValue(CRF_IPADDRESS,sAdress))
	{
		m_sKind = _T("TCP/IP");
	}
	m_bLocal = m_sKind.IsEmpty();
	CString s;
	if (c.GetFieldValue(CSD_IS_DV,s))
	{
		m_bIsDTS = s == CSD_ON; 
	}
	CString sBitrate;
	if (c.GetFieldValue(CRF_BITRATE,sBitrate))
	{
		DWORD dwRemoteBitrate = 0;
		if (1 == _stscanf(sBitrate,_T("%d"),&dwRemoteBitrate))
		{
			WK_TRACE(_T("bitrate is %d\n"),dwRemoteBitrate);
			
			if (dwRemoteBitrate>128*1024)
			{
				m_dwConfirmedOutputOptions &= ~SCO_JPEG_AS_H263;
			}
		}
	}

	pControl->DoConfirmAlarmConnection(c.GetInputShm(), c.GetOutputShm(),
									   _T(""), _T(""), m_dwConfirmedOutputOptions
#ifdef _UNICODE
									   , MAKELONG(CODEPAGE_UNICODE, 0)
#endif
									   );

	// connect to remote server
	Sleep(250); // nobody knows why we must sleep here a little bit
	m_pInput = new CIPCInputVision(c.GetInputShm(),this,idInput);
	Sleep(50);
	m_pOutput = new CIPCOutputVision(c.GetOutputShm(),this,idOutput);

	return TRUE;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::ControlConnection(const CConnectionRecord& c)
{
	m_sName		= c.GetSourceHost();
	m_bAlarm	= FALSE;
	m_bAlarmSwitch = FALSE;
	m_bShowErrorMessage = FALSE;
	if (GetOutput())
	{
		m_AlarmID = GetOutput()->GetCamID(c.GetCamID().GetSubID());
	}
	else
	{
		m_AlarmID = c.GetCamID();
	}

	CString sValue;
	if (c.GetFieldValue(CRF_FULLSCREEN,sValue))
	{
		CMainFrame* pMF = (CMainFrame*)theApp.GetMainWnd();

		// Fullscreen anschalten, falls das gefragt ist.
		if (sValue == CSD_ON)
		{
			if (!pMF->IsFullScreen())
			{
				pMF->ChangeFullScreenModus();
			}
		}
	}
	if (c.GetFieldValue(CRF_SAVE,sValue))
	{
		m_bControlSave = (sValue == CSD_ON);
	}
	if (c.GetFieldValue(CRF_EXCLUSIVE,sValue))
	{
		m_bControlExclusive = (sValue == CSD_ON);
	}

	m_pControl = new CIPCControl(c.GetInputShm(), this);
	Sleep(100); // nobody knows why we must sleep here a little bit

	if (IsConnected())
	{
		return TRUE;
	}
	else
	{
		if (FindOutAlarmHost())
		{
			Connect(m_sName,m_sNumber,m_idHost,(c.GetTimeout()*8)/10);
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::Connect(const CString& sNameRef,
					  const CString& sNumberRef,
					  CSecID idHost,
					  DWORD dwTimeOutInMS)
{
	DWORD dwConnectTimeout = 60*1000;
	if (dwTimeOutInMS!=0)
	{
		dwConnectTimeout = dwTimeOutInMS;
	}

	m_sName = sNameRef;
	m_sNumber = sNumberRef;
	m_idHost = idHost;

	// CAVEAT sNameRef and sNumberRef are references on members of CHost in CHostArray!!!
	// theApp.GetHosts() will delete and renew the CHostArray!
	// So the references are killed!!!
	CHostArray& ha = theApp.GetHosts();
	CHost*  pHost = ha.GetHost(m_idHost);
	if (pHost)
	{
		m_sKind = pHost->GetTyp();
		if (pHost->GetAutoDisconnectTime()>0)
		{
			// nur die GetAutoDisconnectTime ändern!
			m_AutoLogout.SetValues(m_AutoLogout.GetAutoApplication(),
								   TRUE,
								   m_AutoLogout.GetApplicationLogoutTime(),
								   m_AutoLogout.GetApplicationConfirmTime(),
								   pHost->GetAutoDisconnectTime(),
								   m_AutoLogout.GetConnectionConfirmTime());
		}
	}

	CString sDial;
	if (m_sNumber==LOCAL_LOOP_BACK)
	{
		m_bLocal = TRUE;
		m_bShowErrorMessage = FALSE;
		sDial = m_sNumber;
		m_sKind.Empty();
	}
	else
	{
		sDial = CHost::GetCleanedUpNumber(m_sNumber);
	}

	m_pConnectionRecord = new CConnectionRecord(sDial, 
											*theApp.m_pUser, 
											theApp.m_pPermission->GetName(),
											theApp.m_pPermission->GetPassword(),
											dwConnectTimeout);
	m_pConnectionRecord->SetSourceHost(ha.GetLocalHostName());
	m_pConnectionRecord->SetTimeout(dwConnectTimeout);
	if (   theApp.m_pPermission->IsSpecialReceiver()
		|| pHost->IsTOBS()
		|| pHost->IsPINRequired()
		)
	{
		CString sPin;
		if (pHost->IsPINRequired())
		{
			sPin = pHost->GetPIN();
		}
		if (sPin.IsEmpty())
		{
			COemGuiApi::PinDialog(NULL, sPin);
		}
		m_pConnectionRecord->SetFieldValue(CSD_PIN, sPin);
	}

	CString sSerial = _T("0");
	if (theApp.m_pDongle->IsDemo())
	{
		sSerial = szSerials[m_iSerialCounter];
		m_iSerialCounter++;
		if (m_iSerialCounter>4)
		{
			m_iSerialCounter = 0;
		}
	}
	else
	{
		CWK_Profile wkp;
#ifdef _DTS
		// by default a invalid serial!
		sSerial = wkp.GetString(_T("Version"),_T("Serial"),_T("0"));
#else
		// by default a valid serial!
		sSerial = wkp.GetString(_T("Version"),_T("Serial"),_T(""));
#endif
	}
	if (!sSerial.IsEmpty())
	{
		m_pConnectionRecord->SetFieldValue(CRF_SERIAL,sSerial);
	}

	WK_STAT_LOG(_T("Connect"),1,m_sName);
	m_pConnectThread = AfxBeginThread(ConnectThreadProc,(LPVOID)this);
	m_pConnectThread->m_bAutoDelete = FALSE;
	if (!IsLocal())
	{
		if (m_pConnectDialog)
		{
			WK_TRACE_ERROR(_T("m_pConnectDialog not NULL in CServer::Connect\n"));
		}
		WK_DESTROY_WINDOW(m_pConnectDialog);
		m_pConnectDialog = new CConnectingDialog(this, m_sName, dwConnectTimeout, AfxGetMainWnd());
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
void CServer::CancelConnecting()
{
	Lock();
	m_bConnectingCancelled = TRUE;
	m_Fetch.Cancel();
	Unlock();

	DWORD dwTime = GetTickCount();
	DWORD dwReturn = WaitForSingleObject(m_pConnectThread->m_hThread, 5000);
	if (dwReturn != WAIT_OBJECT_0)
	{
		DWORD dwSpan = GetTimeSpan(dwTime);
		WK_TRACE_ERROR(_T("CancelConnecting %s Wait for ConnectThread failed in %lu ms\n"), m_sName, dwSpan);
	}
	TRACE(_T("CancelConnecting %s\n"), m_sName);
	WK_DELETE(m_pConnectThread);
	WK_STAT_LOG(_T("Connect"),0,m_sName);
}
//////////////////////////////////////////////////////////////////////
UINT CServer::ConnectThreadProc(LPVOID pParam)
{
	CServer* pServer = (CServer*)pParam;

	pServer->Lock();
	CConnectionRecord c(*pServer->m_pConnectionRecord);
	pServer->Unlock();

	DWORD dwOptions = c.GetOptions();
#ifdef _DTS
	dwOptions |= SCO_IS_DV;
	c.SetOptions(dwOptions);
#endif


	// the real Fetch
	CIPCFetchResult frInput = pServer->m_Fetch.FetchInput(c);

	pServer->Lock();
	WK_DELETE(pServer->m_pInputResult);
	pServer->m_pInputResult = new CIPCFetchResult(frInput);
	if (   frInput.IsOkay()
		&& !frInput.GetErrorMessage().IsEmpty())
	{
		pServer->SetVersion(frInput.GetErrorMessage());
		pServer->SetDTS((frInput.GetOptions() & SCO_IS_DV)>0);
	}
	pServer->Unlock();
	CObjectView* pOV = pServer->GetDocument()->GetObjectView();
	if (WK_IS_WINDOW(pOV))
	{
		pOV->PostMessage(WM_USER,MAKELONG(WPARAM_INPUT_FETCHRESULT,pServer->GetID()));
	}
	else
	{
		WK_TRACE_ERROR(_T("ConnectThreadProc no ObjectView\n"));
	}

	if (frInput.IsOkay())
	{
		// give server time to free control connection from the preceeding FetchInput
		Sleep(100);	

		dwOptions |= SCO_WANT_RELAYS | SCO_WANT_CAMERAS_COLOR;
		if (!pServer->IsLocal())
		{
			dwOptions |= SCO_JPEG_AS_H263|SCO_NO_STREAM;
		}
		if (pServer->GetVersion()>= _T("4.5"))
		{
			dwOptions |= SCO_MULTI_CAMERA;
		}

		c.SetOptions(dwOptions);

		pServer->Lock();
		if (!pServer->m_bConnectingCancelled)
		{
			pServer->Unlock();

			// the real output fetch
			CIPCFetchResult frOutput = pServer->m_Fetch.FetchOutput(c);

			pServer->Lock();
			WK_DELETE(pServer->m_pOutputResult);
			pServer->m_pOutputResult = new CIPCFetchResult(frOutput);
			pServer->Unlock();
			pOV = pServer->GetDocument()->GetObjectView();
			if (WK_IS_WINDOW(pOV))
			{
				pOV->PostMessage(WM_USER,MAKELONG(WPARAM_OUTPUT_FETCHRESULT,pServer->GetID()));
			}
			else
			{
				WK_TRACE_ERROR(_T("ConnectThreadProc no ObjectView\n"));
			}

			if (   frOutput.IsOkay()
				&& !pServer->IsLocal()//	the local audio object is in CVisionDoc()
				&&	theApp.m_pDongle->RunAudioUnit()
				&& (pServer->CanAudio())
				&& !pServer->m_bConnectingCancelled
				)
			{
				// give server time to free control connection from the preceeding FetchOutput
				Sleep(100);	
				// the real audio fetch !
				CIPCFetchResult frAudio = pServer->m_Fetch.FetchAudio(c);

				pServer->Lock();
				WK_DELETE(pServer->m_pAudioResult);
				pServer->m_pAudioResult = new CIPCFetchResult(frAudio);
				pServer->Unlock();
				pOV = pServer->GetDocument()->GetObjectView();
				if (WK_IS_WINDOW(pOV))
				{
					pOV->PostMessage(WM_USER,MAKELONG(WPARAM_AUDIO_FETCHRESULT,pServer->GetID()));
				}
				else
				{
					WK_TRACE_ERROR(_T("ConnectThreadProc no ObjectView\n"));
				}
			}
		}
		else
		{
			pServer->Unlock();
		}

	}

	pOV = pServer->GetDocument()->GetObjectView();
	if (WK_IS_WINDOW(pOV))
	{
		pOV->PostMessage(WM_USER,MAKELONG(WPARAM_CONNECT_THREAD_END,pServer->GetID()));
	}

	return 0xC0;
}
//////////////////////////////////////////////////////////////////////
CString	CServer::GetErrorMessage(const CIPCFetchResult& fr)
{
	CString sMessage;
	UINT nID = IDP_NO_CONNECTION;

	// error conditions
	// CIPC_ERROR_UNABLE_TO_CONNECT with subcodes
	// CIPC_ERROR_INVALID_PERMISSIONS no subcode
	// CIPC_ERROR_INVALID_PASSWORD no subcode
	// CIPC_ERROR_INVALID_SERIAL no subcode
	if (fr.GetError()==CIPC_ERROR_UNABLE_TO_CONNECT)
	{
		// errorCode 1:link-modul not found,  2: server not found,
		//			 3: server performs reset 4: max clients reached
		switch (fr.GetErrorCode())
		{
		case 1:
			nID = IDP_NO_CONNECTION_LINK_MODUL;
			break;
		case 2:
			nID = IDP_NO_CONNECTION_SERVER;
			break;
		case 3:
			nID = IDP_NO_CONNECTION_RESET;
			break;
		case 4:
			nID = IDP_NO_CONNECTION_MAX_CLIENT;
			break;
		default:
			nID = IDP_NO_CONNECTION;
			break;
		}

		sMessage.LoadString(nID);

		// NEW HEDU 17.8.97, append Error Message in [ ] 
		// but only, if it's not the NOT_RUNNING error
		if ( fr.GetErrorCode()!=1	// not running
			&& fr.GetErrorMessage().GetLength()) 
		{
			sMessage += _T("\n[ ");
			sMessage += fr.GetErrorMessage();
			// drop trailing newline
			if (sMessage[sMessage.GetLength()-1]=='\n') {
				sMessage = sMessage.Left(sMessage.GetLength()-1);
			}
			sMessage += _T("]");
		}
	} 
	else if (fr.GetError()==CIPC_ERROR_INVALID_PERMISSIONS)
	{
		nID = IDP_NO_CONNECTION_INVALID_PERMISSION;
		sMessage.LoadString(nID);
		// there are no details, so there is nothing to append
	} 
	else if (fr.GetError()==CIPC_ERROR_INVALID_PASSWORD)
	{
		nID = IDP_NO_CONNECTION_INVALID_PASSWORD;
		sMessage.LoadString(nID);
		// there are no details, so there is nothing to append
	} 
	else if (fr.GetError()==CIPC_ERROR_INVALID_SERIAL)
	{
		nID = IDP_NO_CONNECTION_INVALID_SERIAL;
		sMessage.LoadString(nID);
		// there are no details, so there is nothing to append
	} 
	else
	{
		sMessage.LoadString(nID);
	}

	return sMessage;
}
//////////////////////////////////////////////////////////////////////
void CServer::ConnectInput()
{
	CString shmInputName;
	BOOL bSuccess = m_pInputResult->GetError()==CIPC_ERROR_OKAY;
	shmInputName = m_pInputResult->GetShmName();
	if (   (m_pInputResult->GetError()!=CIPC_ERROR_OKAY) 
		|| (shmInputName.GetLength()==0))
	{
		m_sLastError = GetErrorMessage(*m_pInputResult);
		WK_STAT_LOG(_T("Connect"),0,m_sName);
	}
	else
	{
		m_pInput = new CIPCInputVision(m_pInputResult->GetShmName(),
											  this,m_pInputResult->GetAssignedID());
		WK_STAT_LOG(_T("Connect"),2,m_sName);
	}
	WK_DESTROY_WINDOW(m_pConnectDialog);
	if (   IsControl()
		&& !bSuccess)
	{
		GetDocument()->GetCIPCServerControlVision()->DoIndicateError(0, SECID_NO_ID,CIPC_ERROR_UNABLE_TO_CONNECT, 
			m_pInputResult->GetErrorCode());
	}
	WK_DELETE(m_pInputResult);
}
//////////////////////////////////////////////////////////////////////
void CServer::ConnectOutput()
{
	CString shmOutputName;
	shmOutputName = m_pOutputResult->GetShmName();
	if ( (m_pOutputResult->GetError()!=CIPC_ERROR_OKAY) || (shmOutputName.GetLength()==0))
	{
		// something went wrong
		m_sLastError = GetErrorMessage(*m_pOutputResult);
		WK_STAT_LOG(_T("Connect"),0,m_sName);
	}
	else
	{
		m_dwConfirmedOutputOptions = m_pOutputResult->GetOptions();
		CString s;
		if (m_dwConfirmedOutputOptions & SCO_WANT_RELAYS)
		{
			s += _T("SCO_WANT_RELAYS|");
		}
		if (m_dwConfirmedOutputOptions & SCO_WANT_CAMERAS_BW)
		{
			s += _T("SCO_WANT_CAMERAS_BW|");
		}
		if (m_dwConfirmedOutputOptions & SCO_WANT_CAMERAS_COLOR)
		{
			s += _T("SCO_WANT_CAMERAS_COLOR|");
		}
		if (m_dwConfirmedOutputOptions & SCO_WANT_DECOMPRESS_BW)
		{
			s += _T("SCO_WANT_DECOMPRESS_BW|");
		}
		if (m_dwConfirmedOutputOptions & SCO_WANT_DECOMPRESS_COLOR)
		{
			s += _T("SCO_WANT_DECOMPRESS_COLOR|");
		}
		if (m_dwConfirmedOutputOptions & SCO_IS_DV)
		{
			s += _T("SCO_IS_DV|");
		}
		if (m_dwConfirmedOutputOptions & SCO_JPEG_AS_H263)
		{
			s += _T("SCO_JPEG_AS_H263|");
		}
		if (m_dwConfirmedOutputOptions & SCO_NO_STREAM)
		{
			s += _T("SCO_NO_STREAM|");
		}
		if (m_dwConfirmedOutputOptions & SCO_MULTI_CAMERA)
		{
			s += _T("SCO_MULTI_CAMERA|");
		}
		WK_TRACE(_T("ConfirmedOutputOption %s\n"),s);
		m_pOutput = new CIPCOutputVision(m_pOutputResult->GetShmName(),
										 this,
										 m_pOutputResult->GetAssignedID());
		WK_STAT_LOG(_T("Connect"),3,m_sName);
	}
	WK_DELETE(m_pOutputResult);
	WK_DESTROY_WINDOW(m_pConnectDialog);
}

//////////////////////////////////////////////////////////////////////
void CServer::ConnectAudio()
{
	CString shmAudioName;
	shmAudioName = m_pAudioResult->GetShmName();
	if ( (m_pAudioResult->GetError()!=CIPC_ERROR_OKAY) || (shmAudioName.GetLength()==0))
	{
		// something went wrong
		m_sLastError = GetErrorMessage(*m_pAudioResult);
		WK_STAT_LOG(_T("Connect"),0,m_sName);
	}
	else
	{
		m_pAudio = new CIPCAudioVision(m_pAudioResult->GetShmName(),
										 this,
										 m_pAudioResult->GetAssignedID());
		WK_STAT_LOG(_T("Connect"),3,m_sName);
	}
	WK_DELETE(m_pAudioResult);
	WK_DESTROY_WINDOW(m_pConnectDialog);
}

//////////////////////////////////////////////////////////////////////
BOOL CServer::Disconnect(BOOL bAsk)
{
	// GF: May be called from AutoLogout via CServers::Disconnect(wServerID),
	// even if still fetching!
	// CServers deletes this object after return, this will cause a GP,
	// if the ConnectThread for fetching is still running.
	// So we first have to ensure, that the ConnectThread is stopped
	if (m_pConnectThread)
	{
		CancelConnecting();
	}

	BOOL bDisconnect = TRUE;
	if (IsLocal())
	{
		if (m_pOutput && !m_pOutput->ReferenceComplete() && bAsk)
		{
			bDisconnect = (IDYES==AfxMessageBox(IDP_NOT_ALL_REFERENCE,MB_YESNO));
		}
	}
	if (bDisconnect)
	{
		if (m_pOutput)
		{
			m_pOutput->StopH263Encoding();
		}
		if (m_pAudio && (m_pAudio->HasDestinations() || m_pAudio->IsDestination()))
		{
			if (IsLocal())
			{
				for (int i=0; i<m_pAudio->GetNumberOfMedia(); i++)
				{
					const CIPCMediaRecord& rec = m_pAudio->GetMediaRecordFromIndex(i);
					if (rec.IsInput() && rec.IsCapturing())
					{
						m_pAudio->StopRecord(rec.GetID());
					}
				}
			}
			else
			{
				for (int i=0; i<m_pAudio->GetNumberOfMedia(); i++)
				{
					const CIPCMediaRecord& rec = m_pAudio->GetMediaRecordFromIndex(i);
					if (rec.IsOutput() && rec.IsRendering())
					{
						m_pAudio->StopPlay(rec.GetID());
					}
					if (rec.IsInput() && rec.IsCapturing())
					{
						m_pAudio->StopRecord(rec.GetID());
					}
				}
			}
			Sleep(500);
		}
		CIPCAudioVision *pLocalAudio = m_pDoc->GetLocalAudio();
		if (pLocalAudio)
		{
			if (pLocalAudio == m_pAudio)
			{
				m_pAudio = NULL;
			}
			pLocalAudio->SetServer(NULL);
		}
		
		WK_DELETE(m_pInput);
		WK_DELETE(m_pOutput);
		WK_DELETE(m_pControl);

		WK_DELETE(m_pAudio);

		WK_STAT_LOG(_T("Connect"),0,m_sName);
	}
	return bDisconnect;
}
////////////////////////////////////////////////////////////////////////////
void CServer::ResetConnectionAutoLogout()
{
	m_AutoLogout.ResetConnection();
}
//////////////////////////////////////////////////////////////////////
void CServer::SetAlarmSwitch(BOOL bOn)
{
	m_bAlarmSwitch = bOn;
}
//////////////////////////////////////////////////////////////////////
void CServer::SetAlarmID(CSecID id)
{
	m_AlarmID = id;
}
//////////////////////////////////////////////////////////////////////
void CServer::PictureData(const CIPCPictureRecord &pict, CSecID id)
{
	if (theApp.m_bTraceImage)
	{
		CString s;
		if (pict.GetPictureType() == SPT_DIFF_PICTURE)
		{
			s = _T("P");
		}
		else if (pict.GetPictureType() == SPT_FULL_PICTURE)
		{
			s = _T("I");
		}
		WK_TRACE(_T("Server %s Camera %08lx, %s %d Bytes Nr %d\n"),GetName(),id.GetID(),s,pict.GetDataLength(),pict.GetBlockNr());
	}
	m_pDoc->PictureData(m_wID,pict,id);
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::IsISDN() const
{
	return 0 == m_sKind.CompareNoCase(_T("ISDN"));
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::IsTCPIP() const
{
	return 0 == m_sKind.CompareNoCase(_T("TCP/IP"));
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::IsB3() const
{
	CString s(m_sKind);
	s.MakeLower();
	return -1 != s.Find(_T("pt200"));
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::IsB6() const
{
	CString s(m_sKind);
	s.MakeLower();
	return -1 != s.Find(_T("b6"));
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::IsDun() const
{
	return 0 == m_sKind.CompareNoCase(_T("DUN"));
}
//////////////////////////////////////////////////////////////////////
void CServer::SetVersion(const CString& sVersion)
{
	m_sVersion = sVersion;
	WK_TRACE(_T("%s is %s\n"),m_sName,m_sVersion);
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::CanActivityMask() const
{
	BOOL bRet = FALSE;

	if (IsDTS())
	{
		bRet = (m_sVersion >= _T("1.4.1"));
	}
	else
	{
		bRet = (m_sVersion >= _T("4.2.3"));
	}

	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::CanAudio() const
{
	BOOL bRet = FALSE;

	if (IsDTS())
	{	// not yet audio implemented
		return m_sVersion >= _T("2.6.0.622");
	}
	else
	{
		bRet = (   (m_sVersion >= _T("4.6.2.622"))
				&& !IsB3()
				&& !IsB6()
				);
	}

	WK_TRACE(_T("Server %s Audio allowed? %s\n"), GetName(), bRet?_T("Yes"):_T("No"));

	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::Can_DV_H263_QCIF() const
{
	BOOL bRet = FALSE;

	if (IsDTS())
	{
		bRet = (m_sVersion >= _T("1.5.0.511"));
	}

	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::IsMultiCamera() const
{
	if (IsDTS())
	{
		if (   IsTCPIP()
			&& GetOutput()->GetBitrate()>128*1024)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return 	   (m_sVersion >= _T("4.5"))
				&& ((m_dwConfirmedOutputOptions & SCO_MULTI_CAMERA)>0);
	}
}

CIPCAudioVision* CServer::GetAudio() const
{
	return m_pAudio;
}
