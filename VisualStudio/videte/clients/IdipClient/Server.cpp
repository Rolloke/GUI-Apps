// Server.cpp: implementation of the CServer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "IdipClient.h"
#include "Server.h"

#include "mainfrm.h"
#include "IdipClientDoc.h"
#include "ViewIdipClient.h"
#include "ViewCamera.h"
#include "ViewAlarmList.h"
#include "ViewArchive.h"

#include "CIPCServerControlIdipClient.h"
#include "CIPCAudioIdipClient.h"
#include "CIPCDatabaseIdipClient.h"
#include "DlgDatabaseInfo.h"
#include "WndPlay.h"
#include "DlgConnecting.h"
#include "QueryResult.h"
#include "DlgBackup.h"

#include "oemgui\DlgLoginNew.h"
#include "wkclasses\wk_utilities.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*
#ifndef _DEBUG
void* AFX_CDECL operator new(size_t nSize, LPCSTR lpszFileName, int nLine)
{
	if (nSize>1*1024*1024)
	{
		CWK_String s(lpszFileName);
		WK_TRACE(_T("new size to large %d %s, %d\n"),nSize,s,nLine);
		return NULL;
	}
	else 	
	{
		return ::operator new(nSize);
	}
}
#endif
*/
//////////////////////////////////////////////////////////////////////
// for DEMO Version
static _TCHAR szSerials[5][10] = {_T("911171851"),_T("941191352"),_T("941131958"),_T("961171356"),_T("971111863")};
int CServer::m_iSerialCounter = 0;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CServer::CServer(CIdipClientDoc* pDoc, CServers* pServers, WORD wID)
{
	// allgemein
	m_pDoc = pDoc;
	m_pServers = pServers;
	m_wID = wID;
	// m_sName
	// m_sKind
	m_bLocal = false;
	m_bIsDTS = false;
	// m_AutoLogout
	// m_csSectin
	// m_sLastError
	// m_sVersion
	m_iMajorVersion = 0;
	m_iMinorVersion = 0;
	m_iSubVersion = 0;
	m_iBuildNr = 0;
	m_hDlgConnecting = NULL;
	// m_Fetch
	m_pConnectionRecord = NULL;
	m_pConnectThread = NULL;
	m_bConnectingCancelled = FALSE;

	// Vision
	// m_sNumber
	// m_sNotificationURL;
	// m_sNotificationMessage;
	m_dwConfirmedOutputOptions = 0;
	m_bAlarm = false;
	m_bAlarmSwitch = false;
	m_bControlConfirmed = false;
	m_bShowErrorMessage = true;
	// m_AlarmID
	// m_AlarmIDs
	m_pInput  = NULL;
	m_pOutput = NULL;
	m_pControl  = NULL;
	m_pAudio = NULL;
	m_bControlSave = false;
	m_bControlExclusive = false;
	m_pInputResult = NULL;
	m_pOutputResult = NULL;
	m_pAudioResult  = NULL;

	// Recherche
	m_pDatabase = NULL;
	m_pDatabaseResult  = NULL;
	m_dwQueryID = QUERY_SERVER;
	// m_Fields;
	m_pDlgDatabaseInfo = NULL;
	// m_srSequences;
	// m_srNewSequences;
	// m_arNewFiles;
	// m_arFiles;
	// m_arToOpenFiles;
	// m_arToDelete;
	// m_arNewArchivs;
	// m_arUpdatedArchivs;
	// m_QueryArchives;
	m_bSearching = false;
	// m_QueryResultsToCopy;
	m_bFirstQueryResult = false;
	m_wQueryArchiveNr = 0;
	m_wQuerySequenceNr = 0;
	m_dwQueryRecordNr = 0;
	m_bStopSearchCopy = false;
	m_iQueryResultsCopied = 0;
	// m_Drives;
	// m_saErrorMessages;
	m_bBackupMode = false;
	m_bIsPINDlgOpen = false;
	m_bDoReconnectDataBase = true;

	if (theApp.m_pDongle->IsDemo())
	{
		m_AutoLogout.SetValues(TRUE,TRUE,10,0,10,0);
	}
	else
	{
		CWK_Profile wkp;
		m_AutoLogout.Load(wkp);
	}

	m_nRemoteTimeOffset		= INVALID_TIME_OFFSET;
	m_nMaxPictureTimeOffset = 0;
	m_nRequestState			= REQUEST_STATE_NORMAL;
	m_nStateCounter			= 0;
	m_nMaxRequestedFps		= 0;
	m_nMaxWith25Fps			= 0;
	m_nMaxWith5Fps			= 0;

#ifdef _DEBUG
	m_srNewSequences.m_nCurrentThread = 0;
	m_arNewArchivs.m_nCurrentThread = 0;
	m_arUpdatedArchivs.m_nCurrentThread = 0;
	m_QueryResultsToCopy.m_nCurrentThread = 0;
//	m_nCurrentThread = 0;
	m_hCurrentThread = NULL;
#endif
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
	m_pConnectThread = NULL;
//	WK_DELETE(m_pConnectThread);

	CIPCAudioIdipClient *pLocalAudio = m_pDoc->GetLocalAudio();
	if (pLocalAudio)
	{
		if (pLocalAudio == m_pAudio)
		{
			m_pAudio->SetServerID(SECID_NO_SUBID);
			m_pAudio = NULL;
		}
	}


	WK_DELETE(m_pInput);
	WK_DELETE(m_pOutput);
	WK_DELETE(m_pControl);
	WK_DELETE(m_pAudio);
	WK_DELETE(m_pDatabase);

	WK_DELETE(m_pConnectionRecord);

	CloseConnectingDlg();

	WK_DELETE(m_pInputResult);
	WK_DELETE(m_pOutputResult);
	WK_DELETE(m_pAudioResult);

	WK_DELETE(m_pDatabaseResult);
	m_arNewArchivs.SafeDeleteAll();
	m_arUpdatedArchivs.SafeDeleteAll();
	m_srNewSequences.SafeDeleteAll();
	m_srSequences.SafeDeleteAll();
	m_QueryResultsToCopy.SafeDeleteAll();
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
void CServer::Lock(LPCTSTR sFkt)
{
#ifdef _DEBUG
	if (m_nCurrentThread != (UINT)-1 && m_nCurrentThread != 0 && m_nCurrentThread != GetCurrentThreadId())
	{
		TRACE(_T("CServer Locked by %s, Id:%x, H:%x in %s\n"), m_sCurrentThread, m_nCurrentThread, m_hCurrentThread, sFkt ? sFkt : _T("?"));
	}
	m_csSection.Lock();
	m_sCurrentThread = XTIB::GetThreadName();
	if (sFkt)
	{
		m_sCurrentThread = m_sCurrentThread + _T(" in ") + sFkt;
	}
	if (m_nCurrentThread != (UINT)-1)
	{
		m_nCurrentThread = GetCurrentThreadId();
	}
	m_hCurrentThread = GetCurrentThread(); 
#else
	m_csSection.Lock();
#endif
}
//////////////////////////////////////////////////////////////////////
void CServer::Unlock()
{
	m_csSection.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CServer::DoGetWWWDir()
{
	if (   IsConnectedOutput()
		&& IsConnectedInput())
	{
		if (   m_pOutput->GotInfo()
			&& m_pInput->GotInfo()
			&& !IsLocal()
			&& !IsDTS()
			&& !IsB3())
		{
			m_pInput->DoRequestWWWDir();
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CServer::OnInputConnected()
{
	if (m_pInput)
	{
		m_pDoc->ActualizeInput(m_pInput);
		ConfirmControlConnection();

		DoGetWWWDir();
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

		DoGetWWWDir();	
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
			if (   m_hDlgConnecting
				&& m_pConnectThread
				&& (m_pInputResult == NULL))
			{
				// Verbindungsaufbau abbrechen
				TRACE(_T("Cancel control connecting %s\n"),m_sName);
				::PostMessage(m_hDlgConnecting, WM_COMMAND, IDCANCEL, 0);
			}
			// TODO GF which connection is relevant? All or single one?
			else if (IsConnected())
			{
				// wir sind verbunden
				//Verbindung immer trennen, nicht nur bei der exclusiven Darstellung
				//exclusive = jede Kamera einzeln anzeigen, die vorherige schließen
				CString sValue;
				m_cAlarmConnectionRecord.GetFieldValue(CRF_DISCONNECT,sValue);
				if (sValue != CSD_OFF)
				{
					// Verbindung trennen
					theApp.GetMainFrame()->PostCommand(m_wID+ID_DISCONNECT_LOW);
				}
				else
				{
					WK_DELETE(m_pControl);
					m_AlarmID = SECID_NO_ID;
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
	Lock(_T(__FUNCTION__));
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
	Lock(_T(__FUNCTION__));
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
	Lock(_T(__FUNCTION__));
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
void CServer::OnDatabaseFetchResult()
{
	Lock(_T(__FUNCTION__));
	if (m_pDatabaseResult!=NULL)
	{
		ConnectDatabase();
	}
	else
	{
		m_pServers->Trace();
		WK_TRACE_ERROR(_T("OnDatabaseFetchResult m_pDatabaseResult is null %s\n"),m_sName);
	}
	Unlock();
}
//////////////////////////////////////////////////////////////////////////
void CServer::OnConnectThreadEnd()
{
/*
	if (m_pConnectThread)
	{
		WaitForSingleObject(m_pConnectThread->m_hThread,10);
	}
	WK_DELETE(m_pConnectThread);
*/
	if (   theApp.m_bConnectToLocalServer == DO_CONNECT_LOCAL 
		&& IsLocal() 
		&& (IsConnectedInput() || IsConnectedOutput() || IsConnectedDatabase()))
	{
		theApp.m_bConnectToLocalServer = CONNECTED_1ST_TIME;
	}
}
//////////////////////////////////////////////////////////////////////
void CServer::OnIdle()
{
	if (IsConnectedPlay())
	{
		CMainFrame* pMF = (CMainFrame*)AfxGetMainWnd();

		// actualize GUI
		if (m_arNewArchivs.GetSize())
		{
			CIPCArchivRecord *pArchiv;

			m_arNewArchivs.Lock(_T(__FUNCTION__));
			while(m_arNewArchivs.GetSize())
			{
				pArchiv = m_arNewArchivs.GetAtFast(0);
				// Recherche calls UpdateMyViews!
				m_pDoc->UpdateAllViews(NULL, MAKELONG(VDH_ADD_ARCHIV, GetID()), pArchiv);
				m_arNewArchivs.RemoveAt(0);
				WK_DELETE(pArchiv);
			}
			m_arNewArchivs.Unlock();
		}
		if (m_arUpdatedArchivs.GetSize())
		{
			CIPCArchivRecord *pArchiv;
			WK_TRACE(_T("OnIdle UpdatedArchives\n"));
			m_arUpdatedArchivs.Lock(_T(__FUNCTION__));
			while(m_arUpdatedArchivs.GetSize())
			{
				pArchiv = m_arUpdatedArchivs.GetAtFast(0);
//				m_pDoc->UpdateAllViews(NULL, MAKELONG(VDH_, GetID()), pArchiv);
				m_arUpdatedArchivs.RemoveAt(0);
				WK_DELETE(pArchiv);
			}
			m_arUpdatedArchivs.Unlock();
		}

		if (!IsLocal() && m_AutoLogout.IsConnectionOver())
		{
			if (theApp.IsDemo())
			{
				pMF->PostMessage(WM_COMMAND,m_wID+ID_DISCONNECT_LOW,0L);
			}
			else
			{
				COemGuiApi::AutoDisconnectDialog(pMF, &m_AutoLogout, WM_COMMAND, m_wID+ID_DISCONNECT_LOW, 0L);
			}
		}

		if (GetInfoDialog())
		{
			GetDatabase()->ActualizeArchivInfo();
		}

		if (m_saErrorMessages.GetSize())
		{
			Lock(_T(__FUNCTION__));
			CString sError = m_saErrorMessages.GetAt(0);
			m_saErrorMessages.RemoveAt(0);
			Unlock();
			CString sMsg;
			sMsg.Format(IDS_ERROR_ON, m_sName, sError);
			COemGuiApi::MessageBox(sMsg, 20, MB_OK|MB_ICONSTOP);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CServer::ConfirmControlConnection()
{
	if (IsControl())
	{
		if (IsConnectedLive())
		{
			if (!m_bControlConfirmed)
			{
				DWORD dwOptions = SCO_WANT_RELAYS | SCO_WANT_CAMERAS_COLOR | SCO_NO_STREAM | SCO_JPEG_AS_H263 | SCO_MULTI_CAMERA;
				if (theApp.GetIntValue(_T("CanGOP"), 1))
				{
					dwOptions |= SCO_CAN_GOP;
				}
				m_pDoc->GetCIPCServerControlIdipClient()->DoConfirmAlarmConnection(m_pInput->GetShmName(),
																			   m_pOutput->GetShmName(),
																			   _T(""),
																			   _T(""),
																			   dwOptions
#ifdef _UNICODE
																			   , MAKELONG(CODEPAGE_UNICODE, 0)
#endif
																				);
				Sleep(10);

				m_bControlConfirmed = true;
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::IsConnected() const
{
	if (m_pInput && (m_pInput->GetIPCState()==CIPC_STATE_CONNECTED))
	{
		if (m_pOutput && (m_pOutput->GetIPCState()==CIPC_STATE_CONNECTED))
		{
			if (m_pDatabase && (m_pDatabase->GetIPCState()==CIPC_STATE_CONNECTED))
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::IsConnectedLive() const
{
	if (   IsConnectedInput()
		&& IsConnectedOutput()
		)
	{
		return TRUE;
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::IsConnectedPlay() const
{
	if (IsConnectedDatabase())
	{
		return TRUE;
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::IsConnectedInput() const
{
	if (m_pInput && (m_pInput->GetIPCState()==CIPC_STATE_CONNECTED))
	{
		return TRUE;
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::IsConnectedOutput() const
{
	if (m_pOutput && (m_pOutput->GetIPCState()==CIPC_STATE_CONNECTED))
	{
		return TRUE;
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::IsConnectedDatabase() const
{
	if (m_pDatabase && (m_pDatabase->GetIPCState()==CIPC_STATE_CONNECTED))
	{
		return TRUE;
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::IsConnectedAudio() const
{
	if (m_pAudio && (m_pAudio->GetIPCState()==CIPC_STATE_CONNECTED))
	{
		return TRUE;
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::IsConnectingInput() const
{
	BOOL bRet = FALSE;
	bRet = (m_pInput!=NULL) && (m_pInput->GetIPCState()==CIPC_STATE_WRITE_CREATED);
	TRACE(_T("IsConnectingInput %u\n"), bRet);
	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::IsConnectingOutput() const
{
	BOOL bRet = FALSE;
	bRet = (m_pOutput!=NULL) && (m_pOutput->GetIPCState()==CIPC_STATE_WRITE_CREATED);
	TRACE(_T("IsConnectingOutput %u\n"), bRet);
	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::IsConnectingDatabase() const
{
	BOOL bRet = FALSE;
	bRet = (m_pDatabase!=NULL) && (m_pDatabase->GetIPCState()==CIPC_STATE_WRITE_CREATED);
	TRACE(_T("IsConnectingDatabase %u\n"), bRet);
	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::IsConnectingAudio() const
{
	BOOL bRet = FALSE;
	bRet = (m_pAudio!=NULL) && (m_pAudio->GetIPCState()==CIPC_STATE_WRITE_CREATED);
	TRACE(_T("IsConnectingAudio %u\n"), bRet);
	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::IsDisconnected() const
{
	BOOL bServerDisconnected   = FALSE;
	BOOL bDatabaseDisconnected = FALSE;

	if (m_pInput && m_pInput->GetIsMarkedForDelete())
	{
		bServerDisconnected = TRUE;
	}
	else if (m_pOutput && m_pOutput->GetIsMarkedForDelete())
	{
		bServerDisconnected = TRUE;
	}
	if (m_pDatabase && m_pDatabase->GetIsMarkedForDelete())
	{
		// RKE: why should we disconnect,
		// remove only database objects
//		bDatabaseDisconnected = TRUE;
	}
	if (m_bConnectingCancelled)
	{
		if (m_pConnectThread == NULL)
		{
			bServerDisconnected = TRUE;
			bDatabaseDisconnected = TRUE;
		}
	}
	return (bServerDisconnected || bDatabaseDisconnected);
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
	Lock(_T(__FUNCTION__));
	if (   m_pInputResult
		&& !m_pInputResult->IsOkay())
	{
		bRet = TRUE;
		WK_TRACE(_T("ConnectionFailed, IR:%d \n"), m_pInputResult);
	}
	if (   m_pOutputResult
		&& !m_pOutputResult->IsOkay())
	{
		bRet = TRUE;
		WK_TRACE(_T("ConnectionFailed, OR:%d \n"), m_pOutputResult);
	}
	Unlock();
	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::FindOutAlarmHost()
{
	// try to get host id
	CHostArray& hostArray = theApp.GetHosts();
	int iBestFit = -1;
	//changes for VisualStudio 2005
	int hi = 0;
	for (hi=0;hi<hostArray.GetSize() && m_idHost==SECID_NO_ID;hi++) 
	{
		if (0==m_sName.CompareNoCase(hostArray[hi]->GetName())) 
		{
			if (iBestFit == -1)
			{
				iBestFit = hi;
			}
			else if (hostArray[hi]->GetTyp() == m_sKind)
			{
				iBestFit = hi;
			}
		}
	}
	if (iBestFit != -1)
	{
		hi = iBestFit;
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
void CServer::CheckDatabaseConnection()
{
	CAutoCritSec as (&m_csSection);
	//only check DB Connection if the PIN Dialog for connections to a DTS System
	//is not open
	if (   m_bDoReconnectDataBase
		&& m_pConnectThread  == NULL // kein ConnectThread laufend
		&& m_pDatabaseResult == NULL // kein Fetch gerade fertig
		&& !m_bIsPINDlgOpen			 // kein PIN Dialog offen
		&& m_pDatabase == NULL		 // Database CIPC Objekt ist 0
		&& (   (IsLocal() && theApp.IsDataBasePresent()) // Archive und Laufwerke sind eingetragen
		    || !IsLocal())			 // remote ist es egal
		)
	{
 		WK_TRACE(_T("Reconnecting Database\n"));
		m_pConnectThread = AfxBeginThread(ConnectThreadProc,(LPVOID)this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
		m_pConnectThread->m_bAutoDelete = TRUE;
		m_pConnectThread->ResumeThread();
	}
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
BOOL CServer::AlarmConnection(const CConnectionRecord& c, CIPCServerControlIdipClient* pControl)
{
	CSecID  idInput,idOutput;
	CString sPin;
	LPCTSTR pszPin = NULL;

	m_cAlarmConnectionRecord = c;
	m_sName		= c.GetSourceHost();
	m_bAlarm	= true;
	m_AlarmID	= c.GetCamID();
	m_dwConfirmedOutputOptions = c.GetOptions();
	m_bShowErrorMessage = false;
	if (theApp.GetIntValue(_T("CanGOP"), 1))
	{
		m_dwConfirmedOutputOptions |= SCO_CAN_GOP;
	}

	m_bAlarmSwitch = (theApp.m_bFirstCam == TRUE) ? true : false;

	// the alarm notification url if any
	c.GetFieldValue(NM_PARAMETER,m_sNotificationURL);
	// replace slash with backslash m_sAlarmURL
	m_sNotificationURL = StringReplace(m_sNotificationURL, _T('/'), _T('\\'));

	// the alarm notification message (txt) if any
	c.GetFieldValue(NM_MESSAGE,m_sNotificationMessage);
	if (!m_sNotificationMessage.IsEmpty())
	{
		CString sCodePage;
		c.GetFieldValue(NM_CODEPAGE, sCodePage);
		m_sNotificationMessage += NM_CODEPAGE + sCodePage;
	}

	CString  sTemp;
	if (c.GetFieldValue(CRF_MSN,sTemp))
	{
		m_sKind = _T("ISDN");
		m_sNumber = sTemp;
	}
	else if (c.GetFieldValue(CRF_IPADDRESS,sTemp))
	{
		m_sKind = _T("TCP/IP");
		m_sNumber = _T("tcp:") + sTemp;
	}
	if (c.GetFieldValue(CRF_VERSION,sTemp))
	{
		m_sVersion = sTemp;
	}

	m_bLocal = m_sKind.IsEmpty() ? true : false;
	if (m_bLocal)
	{
		m_sNumber = LOCAL_LOOP_BACK;
	}
	CString sKind(m_sKind);
	CString sNumber(m_sNumber);
	if (!FindOutAlarmHost())
	{
		// TODO GF Wenn der Host unbekannt ist
		// RTE als Info, dass Bilder nicht gespeichert werden?
	}
	m_sKind   = m_sKind;
	m_sNumber = sNumber;

	CString s;
	if (c.GetFieldValue(CSD_IS_DV,s))
	{
		m_bIsDTS = (s == CSD_ON) ? true : false;
		c.GetFieldValue(CRF_PIN, sPin);
		if (!sPin.IsEmpty())
		{
			pszPin = sPin;
		}
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
									   , MAKELONG(CODEPAGE_UNICODE, 0)
									   );

	// connect to remote server
	WORD wCodePage = 0; // Assume remote host can nothing (is old non-unicode system
	CString sCodePage;
	if (c.GetFieldValue(CRF_CODEPAGE, sCodePage))
	{
		wCodePage = (WORD)_ttoi(sCodePage);
	}
	Sleep(100); // TODO nobody knows why we must sleep here a little bit
	if (m_pInput)
	{
		ASSERT(FALSE);
	}
	m_pInput = new CIPCInputIdipClient(c.GetInputShm(), this, idInput, wCodePage);
	Sleep(10); // TODO nobody knows why we must sleep here a little bit
	if (m_pOutput)
	{
		ASSERT(FALSE);
	}
	m_pOutput = new CIPCOutputIdipClient(c.GetOutputShm(), this, idOutput, wCodePage);

	Sleep(10); 

	/*
	int iCounter = 0;
	while (   !IsConnectedInput()
		   && !IsConnectedOutput()
		   && (iCounter < 50)
		   )
	{
		iCounter++;
		TRACE(_T("AlarmConnection Wait for connecting Output %i ms\n"), iCounter*10);
		Sleep(10); // Give previous 
	}	
	*/
	Connect(m_sName, m_sNumber, m_idHost, 30000, pszPin);

	return TRUE;
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::ControlConnection(const CConnectionRecord& c)
{
	m_sName		= c.GetSourceHost();
	m_bAlarm	= false;
	m_bAlarmSwitch = false;
	m_bShowErrorMessage = false;
	m_cAlarmConnectionRecord = c;
	TRACE(_T("control alarm id = %08lx\n"),c.GetCamID().GetID());
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
		if (sValue == CSD_ON)	// Fullscreen anschalten, falls das gefragt ist.
		{
			theApp.GetMainFrame()->ChangeFullScreenModus(TRUE|FULLSCREEN_FORCE|FULLSCREEN_POST);
		}
	}
	if (c.GetFieldValue(CRF_SAVE,sValue))
	{
		m_bControlSave = (sValue == CSD_ON) ? true : false;
	}
	if (c.GetFieldValue(CRF_EXCLUSIVE,sValue))
	{
		m_bControlExclusive = (sValue == CSD_ON) ? true : false;
	}

	m_pControl = new CIPCControl(c.GetInputShm(), this);
	Sleep(100); // nobody knows why we must sleep here a little bit

	if (IsConnectedLive())
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
void CServer::Connect(const CString& sNameRef,
					  const CString& sNumberRef,
					  CSecID idHost,
					  DWORD dwTimeOutInMS,
					  LPCTSTR pszPin/*=NULL*/)
{
	CAutoCritSec as (&m_csSection);
	if (m_pConnectThread == NULL)
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
			m_bLocal = true;
			sDial = m_sNumber;
			m_sKind.Empty();
		}
		else if (m_sName == m_sNumber)	// ISDN connection without transmitted numbers
		{
			sDial = m_sNumber;
		}
		else
		{
			sDial = CHost::GetCleanedUpNumber(m_sNumber);
		}

		// Are we in the login state?
		CString sPermissionName,sPermissionPassword;

		if (theApp.m_pPermission)
		{
			sPermissionName = theApp.m_pPermission->GetName();
			sPermissionPassword = theApp.m_pPermission->GetPassword();
		}

		m_pConnectionRecord = new CConnectionRecord(sDial, 
												*theApp.m_pUser, 
												sPermissionName,
												sPermissionPassword,
												dwConnectTimeout);
		m_pConnectionRecord->SetSourceHost(ha.GetLocalHostName());
		m_pConnectionRecord->SetTimeout(dwConnectTimeout);
		
		if (pszPin)
		{
			// PIN is given for alarm connections
			m_pConnectionRecord->SetFieldValue(CSD_PIN, pszPin);
		}
		else if (   (theApp.m_pPermission->IsSpecialReceiver() && !IsLocal())
			     || (pHost && pHost->IsTOBS())
			     || (pHost && pHost->IsPINRequired())
			    )
		{
			// otherwise use system PIN or ask
			CString sPin;
			if (pHost && pHost->IsPINRequired())
			{
				sPin = pHost->GetPIN();
			}

			if (sPin.IsEmpty())
			{
				m_bIsPINDlgOpen = true;
				COemGuiApi::PinDialog(NULL, sPin);
				m_bIsPINDlgOpen = false;
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
			// by default a valid serial!
			sSerial = wkp.GetString(_T("Version"),_T("Serial"),_T(""));
		}
		if (!sSerial.IsEmpty())
		{
			m_pConnectionRecord->SetFieldValue(CRF_SERIAL,sSerial);
		}

		WK_STAT_LOG(_T("Connect"),1,m_sName);
		m_pConnectThread = AfxBeginThread(ConnectThreadProc,(LPVOID)this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
		m_pConnectThread->m_bAutoDelete = TRUE;
		m_pConnectThread->ResumeThread();
		if (!IsLocal())
		{
			CloseConnectingDlg();
			if (theApp.IsInMainThread())
			{
				CDlgConnecting *pDlgConnecting = new CDlgConnecting(this, m_sName, dwConnectTimeout, AfxGetMainWnd());
				m_hDlgConnecting = pDlgConnecting->m_hWnd;
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CServer::CloseConnectingDlg()
{
	if (   m_hDlgConnecting
		&& ::IsWindow(m_hDlgConnecting))
	{
		::PostMessage(m_hDlgConnecting, WM_COMMAND, IDOK, 0);
//		::DestroyWindow(m_hDlgConnecting);
	}
	m_hDlgConnecting = NULL;
}
//////////////////////////////////////////////////////////////////////
void CServer::CancelConnecting()
{
	Lock(_T(__FUNCTION__));
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
	m_pConnectThread = NULL;

//	WK_DELETE(m_pConnectThread);
	WK_STAT_LOG(_T("Connect"),0,m_sName);
}
//////////////////////////////////////////////////////////////////////
UINT CServer::ConnectThreadProc(LPVOID pParam)
{
	CServer* pServer = (CServer*)pParam;
	CString sName;

	sName.Format(_T("Server[%s]ConnectThread"), pServer->m_sName);
	XTIB::SetThreadName(sName);

	WK_TRACE(_T("%s, Id:%08lx\n"),sName, GetCurrentThreadId());
	UINT nRet = pServer->ConnectThread();
	return nRet;
}
//////////////////////////////////////////////////////////////////////
UINT CServer::ConnectThread()
{
	Lock(_T(__FUNCTION__));
	CConnectionRecord c(*m_pConnectionRecord);
	Unlock();

	DWORD dwOptions = c.GetOptions();
#ifdef _DTS
	dwOptions |= SCO_IS_DV;
	c.SetOptions(dwOptions);
#endif

	BOOL bFetchInputFailed = FALSE;

	// the real Fetch
	if (theApp.IsReadOnlyModus() == FALSE)
	{
		if (   !IsConnectedInput()
			&& !IsConnectingInput()
			&& !IsAlarm()
			)
		{
			CIPCFetchResult frInput = m_Fetch.FetchInput(c);

			Lock(_T(__FUNCTION__));
			WK_DELETE(m_pInputResult);
			m_pInputResult = new CIPCFetchResult(frInput);
			bFetchInputFailed = !frInput.IsOkay();
			if (   !bFetchInputFailed
				&& !frInput.GetErrorMessage().IsEmpty()
				)
			{
				SetVersion(frInput.GetErrorMessage());
				SetDTS((frInput.GetOptions() & SCO_IS_DV)>0);
			}
			Unlock();
			CViewAlarmList* pViewAlarms = theApp.GetMainFrame()->GetViewAlarmList();
			if (pViewAlarms)
			{
				TRACE(_T("PostMessage WPARAM_INPUT_FETCHRESULT %d\n"),GetID());
				pViewAlarms->PostMessage(WM_USER,MAKELONG(WPARAM_INPUT_FETCHRESULT, GetID()));
			}
			else
			{
				WK_TRACE_ERROR(_T("ConnectThreadProc no ViewAlarms\n"));
			}
		}

		if (   !bFetchInputFailed
			&& !m_bConnectingCancelled
			&& !IsConnectedOutput()
			&& !IsConnectingOutput()
			&& !IsAlarm()
			)
		{
			// give server time to free control connection from the preceeding FetchInput
			Sleep(100);	
			dwOptions |= SCO_WANT_RELAYS | SCO_WANT_CAMERAS_COLOR | SCO_NO_STREAM;
			if (theApp.GetIntValue(_T("CanGOP"), 1))
			{
				dwOptions |= SCO_CAN_GOP;
			}

			if (!IsLocal())
			{
				dwOptions |= SCO_JPEG_AS_H263;
			}

			if (GetVersion()>= _T("4.5"))
			{
				dwOptions |= SCO_MULTI_CAMERA;
			}

			c.SetOptions(dwOptions);

			Lock(_T(__FUNCTION__));
			if (!m_bConnectingCancelled)
			{
				Unlock();

				// the real output fetch
				CIPCFetchResult frOutput = m_Fetch.FetchOutput(c);

				Lock(_T(__FUNCTION__));
				WK_DELETE(m_pOutputResult);
				m_pOutputResult = new CIPCFetchResult(frOutput);
				Unlock();
				CViewCamera* pViewCamera = theApp.GetMainFrame()->GetViewCamera();
				if (pViewCamera)
				{
					pViewCamera->PostMessage(WM_USER,MAKELONG(WPARAM_OUTPUT_FETCHRESULT, GetID()));
				}
				else
				{
					WK_TRACE_ERROR(_T("ConnectThreadProc no ViewCamera\n"));
				}
			}
			else
			{
				Unlock();
			}
		}
		if (   !bFetchInputFailed
			&& !IsLocal()//	the local audio object is in CIdipClientDoc()
			&&	theApp.m_pDongle->RunAudioUnit()
			&& (CanAudio())
			&& !m_bConnectingCancelled
			&& !IsConnectedAudio()
			&& !IsConnectingAudio()
			)
		{
			// give server time to free control connection from the preceeding FetchOutput
			Sleep(100);	
			// the real audio fetch !
			CIPCFetchResult frAudio = m_Fetch.FetchAudio(c);

			Lock(_T(__FUNCTION__));
			WK_DELETE(m_pAudioResult);
			m_pAudioResult = new CIPCFetchResult(frAudio);
			Unlock();

			CView* pView = (CView*) theApp.GetMainFrame()->GetViewAudio();
			if (pView)
			{
				pView->PostMessage(WM_USER,MAKELONG(WPARAM_AUDIO_FETCHRESULT,GetID()));
			}
			else
			{
				WK_TRACE_ERROR(_T("ConnectThreadProc no ViewCamera/Audio\n"));
			}
		}
	} // end of !ReadOnly

	Lock(_T(__FUNCTION__));
	if (   !m_bConnectingCancelled
		&& !IsConnectedDatabase()
		&& !IsConnectingDatabase()
		&& (!bFetchInputFailed || theApp.IsReadOnlyModus())
		)
	{
		Unlock();
		CIPCFetchResult frDatabase = m_Fetch.FetchDatabase(c);
		Lock(_T(__FUNCTION__));
		if (!m_bConnectingCancelled)
		{
			Unlock();

			Lock(_T(__FUNCTION__));
			WK_DELETE(m_pDatabaseResult);
			m_pDatabaseResult = new CIPCFetchResult(frDatabase);
			Unlock();
			CViewArchive* pViewArchive = theApp.GetMainFrame()->GetViewArchive();
			if (pViewArchive)
			{
				pViewArchive->PostMessage(WM_USER,MAKELONG(WPARAM_DATABASE_FETCHRESULT,GetID()));	
			}
			else
			{
				WK_TRACE_ERROR(_T("ConnectThreadProc no ViewArchive\n"));
			}
		}
		else
		{
			Unlock();
		}
	}
	else
	{
		Unlock();
	}

	
	Lock(_T(__FUNCTION__));
	SetEvent(m_pConnectThread->m_hThread);
	m_pConnectThread = NULL;
	Unlock();

	CViewCamera* pViewCamera = theApp.GetMainFrame()->GetViewCamera();
	if (pViewCamera)
	{
		pViewCamera->PostMessage(WM_USER,MAKELONG(WPARAM_CONNECT_THREAD_END, GetID()));
	}

	return 0xC0;
}
//////////////////////////////////////////////////////////////////////
CString	CServer::GetErrorMessage(const CIPCFetchResult& fr)
{
	CString sMessage;
	UINT nID = 0;

	// error conditions
	// CIPC_ERROR_UNABLE_TO_CONNECT with subcodes
	// CIPC_ERROR_INVALID_PERMISSIONS no subcode
	// CIPC_ERROR_INVALID_PASSWORD no subcode
	// CIPC_ERROR_INVALID_SERIAL no subcode
	switch (fr.GetError())
	{
	case CIPC_ERROR_UNABLE_TO_CONNECT:
	{
		// errorCode 1:link-modul not found,  2: server not found,
		//			 3: server performs reset 4: max clients reached
		switch (fr.GetErrorCode())
		{
		case 1:  nID = IDP_NO_CONNECTION_LINK_MODUL; break;
		case 2:  nID = IDP_NO_CONNECTION_SERVER; break;
		case 3:  nID = IDP_NO_CONNECTION_RESET; break;
		case 4:  nID = IDP_NO_CONNECTION_MAX_CLIENT; break;
		default: nID = IDP_NO_CONNECTION; break;
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
		return sMessage;
	} break;
	case CIPC_ERROR_INVALID_PERMISSIONS:
		nID = IDP_NO_CONNECTION_INVALID_PERMISSION;
		break;
	case CIPC_ERROR_INVALID_PASSWORD:
		nID = IDP_NO_CONNECTION_INVALID_PASSWORD;
		break;
	case CIPC_ERROR_INVALID_SERIAL:
		nID = IDP_NO_CONNECTION_INVALID_SERIAL;
		break;
//	case CIPC_ERROR_ARCHIVE:
	default:
		nID = IDP_NO_CONNECTION;
		break;
	}
	if (nID)
	{
		sMessage.LoadString(nID);
	}

	return sMessage;
}
//////////////////////////////////////////////////////////////////////
void CServer::ConnectInput()
{
	CString shmInputName;
	BOOL bSuccess = m_pInputResult->IsOkay();
	shmInputName = m_pInputResult->GetShmName();
	if (!bSuccess)
	{
		m_sLastError = GetErrorMessage(*m_pInputResult);
		TRACE(_T("Error from Input Fetch result %s\n"),m_sLastError);
		WK_STAT_LOG(_T("Connect"),0,m_sName);
	}
	else
	{
		if (m_pInput)
		{
			WK_TRACE_ERROR(_T("deleting old Input Connection %s\n"), m_pInput->GetShmName());
			delete m_pInput;
		}
		m_pInput = new CIPCInputIdipClient(m_pInputResult->GetShmName(),
											this,
											m_pInputResult->GetAssignedID(),
											m_pInputResult->GetCodePage()
											);
		WK_STAT_LOG(_T("Connect"),2,m_sName);
		WK_DELETE(m_pInputResult);
	}
	if (   IsControl()
		&& !bSuccess)
	{
		GetDocument()->GetCIPCServerControlIdipClient()->DoIndicateError(0, SECID_NO_ID,CIPC_ERROR_UNABLE_TO_CONNECT, 
			m_pInputResult->GetErrorCode());
	}
	CloseConnectingDlg();
}
//////////////////////////////////////////////////////////////////////
void CServer::ConnectOutput()
{
	CString shmOutputName;
	shmOutputName = m_pOutputResult->GetShmName();
	if (   (m_pOutputResult->GetError()!=CIPC_ERROR_OKAY) 
		|| (shmOutputName.GetLength()==0))
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
		WK_TRACE(_T("ConfirmedOutputOption %08lx,%s\n"),m_dwConfirmedOutputOptions,s);

		if (m_pOutput)
		{
			WK_TRACE_ERROR(_T("deleting old Output Connection %s\n"), m_pOutput->GetShmName());
			delete m_pOutput;
#ifdef _DEBUG
			Beep(1000, 200);
#endif
		}
		m_pOutput = new CIPCOutputIdipClient(m_pOutputResult->GetShmName(),
												this,
                                                m_pOutputResult->GetAssignedID(),
												m_pOutputResult->GetCodePage()
												);
		TRACE(_T("New Output ConnectOutput 08x\n"), m_pOutput);
		WK_STAT_LOG(_T("Connect"),3,m_sName);
	}
	WK_DELETE(m_pOutputResult);
	CloseConnectingDlg();
}
//////////////////////////////////////////////////////////////////////
void CServer::ConnectDatabase()
{
	CString sShmDatabaseName = m_pDatabaseResult->GetShmName();
	BOOL bSuccess = m_pDatabaseResult->GetError()==CIPC_ERROR_OKAY;
	if (   (bSuccess  == FALSE)
		|| (sShmDatabaseName.GetLength() == 0)
		)
	{
		switch (m_pDatabaseResult->GetError())
		{
			case CIPC_ERROR_INVALID_PASSWORD:
			case CIPC_ERROR_INVALID_PERMISSIONS:
			case CIPC_ERROR_ARCHIVE:
				m_bDoReconnectDataBase = false;
				// stop reconnection
			break;
		}
		WK_STAT_LOG(_T("Connect"), 0, m_sName);
	}
	else
	{
		if (m_pDatabase)
		{
			WK_TRACE_ERROR(_T("deleting old Database Connection %s\n"), m_pDatabase->GetShmName());
			delete m_pDatabase;
#ifdef _DEBUG
			Beep(1000, 200);
#endif
		}

		m_pDatabase = new CIPCDatabaseIdipClient(m_pDatabaseResult->GetShmName(),
												this,
												m_pDatabaseResult->GetAssignedID(),
												m_pDatabaseResult->GetCodePage()
												);
		WK_STAT_LOG(_T("Connect"), 5, m_sName);
	}
	CloseConnectingDlg();
	if (   IsControl()
		&& !bSuccess)
	{
		GetDocument()->GetCIPCServerControlIdipClient()->DoIndicateError(0, SECID_NO_ID, CIPC_ERROR_UNABLE_TO_CONNECT, 
			m_pDatabaseResult->GetErrorCode());
	}
	WK_DELETE(m_pDatabaseResult);
}
//////////////////////////////////////////////////////////////////////
void CServer::ConnectAudio()
{
	CString shmAudioName;
	shmAudioName = m_pAudioResult->GetShmName();
	if (   (m_pAudioResult->GetError()!=CIPC_ERROR_OKAY) 
		|| (shmAudioName.GetLength()==0))
	{
		// something went wrong
		m_sLastError = GetErrorMessage(*m_pAudioResult);
		WK_STAT_LOG(_T("Connect"),0,m_sName);
	}
	else
	{
		if (m_pAudio)
		{
			WK_TRACE_ERROR(_T("deleting old Audio Connection %s\n"), m_pAudio->GetShmName());
			delete m_pAudio;
#ifdef _DEBUG
			Beep(1000, 200);
#endif
		}
		m_pAudio = new CIPCAudioIdipClient(m_pAudioResult->GetShmName(),
										 this,
										 m_pAudioResult->GetAssignedID(),
										 m_pAudioResult->GetCodePage()
										 );
		WK_STAT_LOG(_T("Connect"),4,m_sName);
	}
	WK_DELETE(m_pAudioResult);
	CloseConnectingDlg();
}

//////////////////////////////////////////////////////////////////////
BOOL CServer::Disconnect()
{
	// GF: May be called from AutoLogout via CServers::Disconnect(wServerID),
	// even if still fetching!
	// CServers deletes this object after return, this will cause a GP,
	// if the ConnectThread for fetching is still running.
	// So we first have to ensure, that the ConnectThread is stopped
	CAutoCritSec acs1(&m_srNewSequences.m_cs);
	CAutoCritSec acs2(&m_srSequences.m_cs);
	if (m_pConnectThread)
	{
		CancelConnecting();
	}

	if (m_pOutput)
	{
		m_pOutput->StopH263Encoding();
	}
	if (m_pAudio && (m_pAudio->HasDestinations() || m_pAudio->IsDestination()))
	{
		BOOL	bWaitForEnd = FALSE;
		// Audioverbindungen trennen, wenn Kanäle offen sind
		if (IsLocal())
		{
			for (int i=0; i<m_pAudio->GetNumberOfMedia(); i++)
			{
				const CIPCMediaRecord& rec = m_pAudio->GetMediaRecordFromIndex(i);
				if (rec.IsInput() && rec.IsCapturing())
				{
					m_pAudio->StopRecord(rec.GetID());
					bWaitForEnd = TRUE;
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
					bWaitForEnd = TRUE;
				}
				if (rec.IsInput() && rec.IsCapturing())
				{
					m_pAudio->StopRecord(rec.GetID());
					bWaitForEnd = TRUE;
				}
			}
		}
		if (bWaitForEnd)
		{
			Sleep(500);
		}
	}

	CIPCAudioIdipClient *pLocalAudio = m_pDoc->GetLocalAudio();
	if (pLocalAudio)
	{	// lokale Audioverbindung wird nicht getrennt
		if (pLocalAudio == m_pAudio)
		{
			m_pAudio->SetServerID(SECID_NO_SUBID);
			m_pAudio = NULL;	// sondern vom lokalen Server entfernt.
		}
	}
	
	WK_DELETE(m_pInput);
	WK_DELETE(m_pOutput);
	WK_DELETE(m_pControl);

	WK_DELETE(m_pAudio);

	if (WK_IS_WINDOW(m_pDlgDatabaseInfo))
	{
		m_pDlgDatabaseInfo->DestroyWindow();
	}

	DeleteDataBaseObject();

	Sleep(200);
	if (theApp.m_pUser)
	{
		WK_TRACE_USER(_T("%s trennt Verbindung zu %s\n"),
			theApp.m_pUser->GetName(),GetName());
	}
	WK_STAT_LOG(_T("Connect"),0,m_sName);

	return TRUE;
}
////////////////////////////////////////////////////////////////////////////
void CServer::ResetConnectionAutoLogout()
{
	m_AutoLogout.ResetConnection();
}
//////////////////////////////////////////////////////////////////////
void CServer::SetAlarmSwitch(BOOL bOn)
{
	m_bAlarmSwitch = bOn ? true : false;
}
//////////////////////////////////////////////////////////////////////
void CServer::SetAlarmID(CSecID id)
{
	m_AlarmID = id;
	if (id != SECID_NO_ID)
	{
		m_bAlarm = true;
	}
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
BOOL CServer::IsLowBandwidth() const
{
	BOOL bRet = FALSE;

	if (IsConnectedOutput())
	{
		bRet = m_pOutput->GetBitrate() <= 128*1024;
	}

	return bRet;
}
//////////////////////////////////////////////////////////////////////
void CServer::SetVersion(const CString& sVersion)
{
	m_sVersion = sVersion;
	WK_TRACE(_T("%s is %s\n"),m_sName,m_sVersion);

	int iMajorVersion,iMinorVersion,iSubVersion,iBuildNr;
	iMajorVersion = iMinorVersion = iSubVersion = iBuildNr = 0;
	if (4 == _stscanf(sVersion, _T("%d.%d.%d.%d"), &iMajorVersion, &iMinorVersion, &iSubVersion, &iBuildNr))
	{
		m_iMajorVersion = (short)iMajorVersion;
		m_iMinorVersion = (short)iMinorVersion;
		m_iSubVersion   = (short)iSubVersion;
		m_iBuildNr      = (short)iBuildNr;
	}
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
//////////////////////////////////////////////////////////////////////
CIPCAudioIdipClient* CServer::GetAudio() const
{
	return m_pAudio;
}
//////////////////////////////////////////////////////////////////////
CString CServer::GetArchiveName(WORD wArchiv)
{
	if (IsConnectedPlay())
	{
		return m_pDatabase->GetArchiveName(wArchiv);
	}
	return _T("");
}
//////////////////////////////////////////////////////////////////////
void CServer::AddNewArchiv(const CIPCArchivRecord& data)
{
	CIPCArchivRecord* pArchiv = new CIPCArchivRecord(data);
	m_arNewArchivs.Lock(_T(__FUNCTION__));
	m_arNewArchivs.Add(pArchiv);
	m_arNewArchivs.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CServer::UpdateArchiv(const CIPCArchivRecord& data)
{
	CIPCArchivRecord* pArchiv = new CIPCArchivRecord(data);
	m_arUpdatedArchivs.Lock(_T(__FUNCTION__));
	m_arUpdatedArchivs.Add(pArchiv);
	m_arUpdatedArchivs.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CServer::AddNewSequences(int iNumRecords, const CIPCSequenceRecord data[])
{
	m_srNewSequences.Lock(_T(__FUNCTION__));
	m_srSequences.Lock(_T(__FUNCTION__));
	for (int i=0; i<iNumRecords; i++)
	{
		AddNewSequence(data[i],0,0);
	}
	m_srNewSequences.Unlock();
	m_srSequences.Unlock();
}
//////////////////////////////////////////////////////////////////////
BOOL CServer::AddNewSequence(const CIPCSequenceRecord& seq,
							 WORD  wPrevSequenceNr,
							 DWORD dwNrOfRecords)
{
	CIPCSequenceRecord* pS = m_srNewSequences.GetSequence(seq.GetArchiveNr(), seq.GetSequenceNr());
	if (pS)
	{	// do not insert twice
		return FALSE;
	}
	pS = m_srSequences.GetSequence(seq.GetArchiveNr(), seq.GetSequenceNr());
	if (pS)
	{	// do not insert twice
		return FALSE;
	}

//	TRACE(_T("#### AddNewSequence: PrevSequ: %d  NrOfRec: %d\n"),wPrevSequenceNr,dwNrOfRecords );
	m_srNewSequences.SafeAdd(new CIPCSequenceRecord(seq));

	// search for the right position
	m_srSequences.Lock(_T(__FUNCTION__));
	WORD wSeqArchive  = seq.GetArchiveNr();
	CSystemTime tSeqTime = seq.GetTime();

	int i, nSize = m_srSequences.GetSize();
	for (i=nSize-1; i>0; i--)
	{
		CIPCSequenceRecord* pSeq = m_srSequences.GetAtFast(i);
		if (pSeq->GetArchiveNr() == wSeqArchive)
		{
			if (tSeqTime < pSeq->GetTime())
			{
				m_srSequences.InsertAt(i, new CIPCSequenceRecord(seq));
	//			TRACE(_T("#### m_srSequences.InsertAt(%d), %d\n"), i, nSize);
				nSize = -1;
				break;
			}
		}
	}

	if (nSize != -1)
	{
		m_srSequences.Add(new CIPCSequenceRecord(seq));
	}
	if (   wPrevSequenceNr >0
		&& dwNrOfRecords>0)
	{
		CIPCSequenceRecord* pPrev = m_srSequences.GetSequence(seq.GetArchiveNr(),wPrevSequenceNr);
		if (pPrev)
		{
			pPrev->SetNrOfPictures(dwNrOfRecords);
		}
	}
	m_srSequences.Unlock();
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
void CServer::RemoveSequence(WORD wArchivNr, WORD wSequenceNr)
{
	DWORD dw =MAKELONG(wArchivNr, wSequenceNr);

	// TODO! RKE: derive a CHint Class from CObject for type verification
	// Recherche calls UpdateMyViews!
	m_pDoc->UpdateAllViews(NULL, MAKELONG(VDH_DEL_SEQUENCE, GetID()), (CObject*)dw);
	int i,c;
	m_srSequences.Lock(_T(__FUNCTION__));
	c = m_srSequences.GetSize();
	for (i=0;i<c;i++)
	{
		CIPCSequenceRecord* pSequence = m_srSequences.GetAtFast(i);
		if 	( (pSequence->GetArchiveNr()  == wArchivNr)	
			&&(pSequence->GetSequenceNr() == wSequenceNr) )
		{
			m_srSequences.RemoveAt(i);
			WK_DELETE(pSequence);
			break;
		}
	}
	m_srSequences.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CServer::RemoveArchiveSequences(WORD wArchivNr)
{
	int i,c;
	m_srSequences.Lock(_T(__FUNCTION__));
	c = m_srSequences.GetSize();
	for (i=c-1; i>=0; i--)
	{
		CIPCSequenceRecord* pSequence = m_srSequences.GetAtFast(i);
		if 	(pSequence->GetArchiveNr()  == wArchivNr)	
		{
			m_srSequences.RemoveAt(i);
			WK_DELETE(pSequence);
		}
	}
	m_srSequences.Unlock();
}
//////////////////////////////////////////////////////////////////////
CIPCSequenceRecord* CServer::GetCIPCSequenceRecord(WORD wArchiv,WORD wSequenceNr)
{
	m_srSequences.Lock(_T(__FUNCTION__));
	CIPCSequenceRecord *pRec = m_srSequences.GetSequence(wArchiv,wSequenceNr);
	m_srSequences.Unlock();
	return pRec;
}
//////////////////////////////////////////////////////////////////////
CIPCSequenceRecord* CServer::GetNextCIPCSequenceRecord(const CIPCSequenceRecord& s)
{
	CIPCSequenceRecord* pNextSequence = NULL;
	// first find the sequence
	BOOL f = FALSE;

	m_srSequences.Lock(_T(__FUNCTION__));
	for (int i=0;i<m_srSequences.GetSize();i++)
	{
		CIPCSequenceRecord* pSequence = m_srSequences.GetAtFast(i);
		if (!f)
		{
			if (   pSequence->GetArchiveNr()  == s.GetArchiveNr()
				&& pSequence->GetSequenceNr() == s.GetSequenceNr())
			{	// gefunden
				f = TRUE;
			}
		}
		else
		{
			if (pSequence->GetArchiveNr() == s.GetArchiveNr())
			{
				pNextSequence = pSequence;
				break;
			}
		}
	}
	m_srSequences.Unlock();

	return pNextSequence;
}
//////////////////////////////////////////////////////////////////////
CIPCSequenceRecord* CServer::GetPrevCIPCSequenceRecord(const CIPCSequenceRecord& s)
{
	CIPCSequenceRecord* pPrevSequence = NULL;
	BOOL f = FALSE;
	// first find the sequence

	m_srSequences.Lock(_T(__FUNCTION__));
	for (int i=m_srSequences.GetSize()-1;i>=0;i--)
	{
		CIPCSequenceRecord* pSequence = m_srSequences.GetAtFast(i);
		if (!f)
		{
			if (   pSequence->GetArchiveNr()  == s.GetArchiveNr()
				&& pSequence->GetSequenceNr() == s.GetSequenceNr())
			{
				// gefunden
				f = TRUE;
			}
		}
		else
		{
			if (pSequence->GetArchiveNr() == s.GetArchiveNr())
			{
				pPrevSequence = pSequence;
				break;
			}
		}
	}
	m_srSequences.Unlock();

	return pPrevSequence;
}
//////////////////////////////////////////////////////////////////////
CIPCSequenceRecord* CServer::GetFirstSequenceRecord(WORD wArchiveNr)
{
	CIPCSequenceRecord* pFirstSequence = NULL;
	CSystemTime st;
	st.GetLocalTime();

	m_srSequences.Lock(_T(__FUNCTION__));
	for (int i=0;i<m_srSequences.GetSize();i++)
	{
		CIPCSequenceRecord* pSequence = m_srSequences.GetAtFast(i);
		if (   (pSequence->GetArchiveNr() == wArchiveNr)
			&& (pSequence->GetTime() < st)
			)
		{
			st = pSequence->GetTime();
			pFirstSequence = pSequence;
		}
	}
	m_srSequences.Unlock();

	return pFirstSequence;
}
//////////////////////////////////////////////////////////////////////
CIPCSequenceRecord* CServer::GetLastSequenceRecord(WORD wArchiveNr)
{
	CIPCSequenceRecord* pLastSequence = NULL;
	CSystemTime st(0);

	m_srSequences.Lock(_T(__FUNCTION__));
	for (int i=0;i<m_srSequences.GetSize();i++)
	{
		CIPCSequenceRecord* pSequence = m_srSequences.GetAtFast(i);
		if (   (pSequence->GetArchiveNr() == wArchiveNr)
			&& (pSequence->GetTime() > st)
			)
		{
			st = pSequence->GetTime();
			pLastSequence = pSequence;
		}
	}
	m_srSequences.Unlock();

	return pLastSequence;
}
//////////////////////////////////////////////////////////////////////
int	 CServer::GetMaxFPS()
{
	int nMaxFps = m_nMaxRequestedFps;
	if (nMaxFps == 0) nMaxFps = 100;
	int nPL = theApp.GetPerformanceLevel();
	nPL -= INITIAL_PERFORMANCE_LEVEL;
	if (nPL < 0) nPL = 0;
	int nRange = MAX_PERFORMANCE_LEVEL - INITIAL_PERFORMANCE_LEVEL;
	nPL = nRange - nPL;
	return MulDiv(nMaxFps, nPL+1, nRange+1);
}
//////////////////////////////////////////////////////////////////////
CIPCSequenceRecords& CServer::GetNewSequences()
{
	return m_srNewSequences;
}
//////////////////////////////////////////////////////////////////////
CIPCArchivRecord* CServer::GetCIPCArchivRecord(WORD wArchiv)
{
	if (IsConnectedPlay())
	{
		return m_pDatabase->GetArchive(wArchiv);
	}
	return NULL;
}
//////////////////////////////////////////////////////////////////////
void CServer::InitFields3x()
{
	m_Fields.SafeDeleteAll();
	CWK_Profile wkp;
	m_Fields.Load(SECTION_DATABASE_FIELDS, wkp);
	// check for old style before 4.6.2
	if (m_Fields.GetSize() == 0)
	{
		CString s;
		s.LoadString(IDS_DTP_DATE);
		m_Fields.SafeAdd(new CIPCField(CIPCField::m_sfDate,s,8,'D'));
		s.LoadString(IDS_DTP_TIME);
		m_Fields.SafeAdd(new CIPCField(CIPCField::m_sfTime,s,6,'T'));
		s.LoadString(IDS_GA_NR);
		m_Fields.SafeAdd(new CIPCField(CIPCField::m_sfGaNr,s,6,'C'));
		s.LoadString(IDS_TA_NR);
		m_Fields.SafeAdd(new CIPCField(CIPCField::m_sfTaNr,s,4,'C'));
		s.LoadString(IDS_KTO_NR);
		m_Fields.SafeAdd(new CIPCField(CIPCField::m_sfKtNr,s,16,'C'));
		s.LoadString(IDS_BANKLEITZAHL);
		m_Fields.SafeAdd(new CIPCField(CIPCField::m_sfBcNr,s,16,'C'));
		s.LoadString(IDS_VALUE);
		m_Fields.SafeAdd(new CIPCField(CIPCField::m_sfAmnt,s,8,'C'));
		s.LoadString(IDS_CURRENCY);
		m_Fields.SafeAdd(new CIPCField(CIPCField::m_sfCurr,s,3,'C'));
	}
}
//////////////////////////////////////////////////////////////////////
void CServer::OnConfirmFieldInfo(int iNumRecords, const CIPCField data[])
{
	m_Fields.SafeDeleteAll();
	m_Fields.FromArray(iNumRecords,data);

#ifdef _DEBUG
	for (int i=0;i<m_Fields.GetSize();i++)
	{
		TRACE(_T("%s, %s\n"), m_Fields.GetAtFast(i)->GetName(), m_Fields.GetAtFast(i)->GetValue());
	}
#endif

	AfxGetMainWnd()->PostMessage(WM_USER, INIT_FIELDS);
//	theApp.InitFields(); // to update the field map

	if (IsLocal())
	{
//	RKE: BackupMode obsolete
//		if (!theApp.m_bBackupModus)
		{
			if (theApp.m_bSearchMaskAtStart)
			{
				AfxGetMainWnd()->PostMessage(WM_COMMAND,ID_SEARCH);
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
CString CServer::GetFieldName(const CString& sField)
{
	CIPCField* pField = m_Fields.GetCIPCField(sField);
	if (pField)
	{
		return pField->GetValue();
	}
	return _T("");
}
//////////////////////////////////////////////////////////////////////
void CServer::OpenSequence(WORD wArchiv, WORD wSequence, DWORD dwRecordNr /* = 1*/)
{
	CViewIdipClient* pViewIdip = theApp.GetMainFrame()->GetViewIdipClient();
	if (pViewIdip)
	{
		if (!IsDTS())
		{
			m_pDatabase->DoRequestOpenSequence(wArchiv,wSequence);
		}
		CWndPlay* pDW = NULL;

		if (pViewIdip->UseSingleWindow())
		{
			pDW = pViewIdip->GetWndPlay(m_wID,wArchiv, 0);
		}
		else
		{
			pDW = pViewIdip->GetWndPlay(m_wID,wArchiv, wSequence);
		}
		if (pDW==NULL)
		{
			if (theApp.m_pUser)
			{
				WK_TRACE_USER(_T("%s öffnet Archiv %d Sequence %d auf %s\n"),
					theApp.m_pUser->GetName(), wArchiv, wSequence, GetName());
			}
			CIPCSequenceRecord* pSequence = GetCIPCSequenceRecord(wArchiv, wSequence); 
			if (pSequence)
			{
				// Recherche calls UpdateMyViews!
				m_pDoc->UpdateAllViews(NULL, MAKELONG(VDH_OPEN_SEQUENCE, GetID()), pSequence);
				pDW = pViewIdip->GetWndPlay(m_wID,wArchiv, wSequence);
			}
		}
		// goto the record!
		if (pDW)
		{
			//TKR if the choosen playwnd is not activ, activate it
			CWndSmall* pWndSmallActive = pViewIdip->GetWndSmallCmdActive();
			if(    pWndSmallActive
				&& pWndSmallActive != (CWndSmall*)pDW)
			{
				pViewIdip->SetWndSmallCmdActive(pDW);
				if(pViewIdip->IsView1plus())
				{
					pViewIdip->SetWndSmallBig(pDW);
				}
				else if(pViewIdip->Is1x1())
				{
					//if view 1-mode is active, switch new sequence into 1-mode
					pViewIdip->SetView1();
				}
			}

			if (pViewIdip->UseSingleWindow())
			{
				if (pDW->GetSequenceNr() == wSequence)
				{
					pDW->Navigate(dwRecordNr);
				}
				else
				{
					CIPCSequenceRecord* pSequence = GetCIPCSequenceRecord(wArchiv, wSequence); 
					pDW->Navigate(*pSequence,dwRecordNr);
				}
			}
			else
			{
				pDW->Navigate(dwRecordNr);
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CServer::DeleteArchivFiles()
{
	if (IsConnectedPlay())
	{
		DWORD dw;
		WORD  wArchivNr;
		WORD  wSequenceNr;

		while (m_arToDelete.GetSize())
		{
			dw = m_arToDelete.GetAt(0);
			wArchivNr = LOWORD(dw);
			wSequenceNr = HIWORD(dw);
			if (theApp.m_pUser)
			{
				WK_TRACE_USER(_T("%s löscht Sequence %d aus Archiv %d\n"),
					theApp.m_pUser->GetName(),wSequenceNr,wArchivNr);
			}
			m_pDatabase->DoRequestDeleteSequence(wArchivNr,wSequenceNr);
			m_arToDelete.RemoveAt(0);
			Sleep(5);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CServer::DeleteDataBaseObject()
{
//	WK_TRACE(_T("Lösche Datenbankobjekt in Server:%x\n"), this);
	WK_TRACE(_T("DeleteDataBaseObject: nS:%d, S:%d, TD:%d, nA:%d, nUA:%d\n"),
		m_srNewSequences.GetSize(),
		m_srSequences.GetSize(),
		m_arToDelete.GetSize(),
		m_arNewArchivs.GetSize(),
		m_arUpdatedArchivs.GetSize()
		);
	m_srNewSequences.SafeDeleteAll();
	m_srSequences.SafeDeleteAll();
	m_arToDelete.RemoveAll();
	m_arNewArchivs.SafeDeleteAll();
	m_arUpdatedArchivs.SafeDeleteAll();
	WK_DELETE(m_pDatabase);
//	WK_TRACE(_T("Datenbankobjekt gelöscht\n"));
}
//////////////////////////////////////////////////////////////////////
void CServer::AddDeleteArchivFile(WORD wArchiv, WORD wSequence)
{
	if (IsConnectedPlay())
	{
		m_arToDelete.Add(MAKELONG(wArchiv, wSequence)); // low / high
	}
}
//////////////////////////////////////////////////////////////////////
void CServer::ShowInfoDialog()
{
	if (m_pDlgDatabaseInfo)
	{
		m_pDlgDatabaseInfo->ShowWindow(SW_SHOW);
	}
	else
	{
		m_pDlgDatabaseInfo = new CDlgDatabaseInfo(AfxGetMainWnd(),this);
	}
}
//////////////////////////////////////////////////////////////////////
void CServer::RequestRecord(WORD wArchiv, 
							WORD wSequence, 
							DWORD dwCurrentRecord,
							DWORD dwNewRecord,
							DWORD dwCamID)
{
	if (IsConnectedPlay())
	{
		/*
		if (dwCamID == (DWORD)-1)
		{
			TRACE(_T("requesting A=%d, S=%d, CR=%d, NR=%d\n"),
				wArchiv,wSequence,dwCurrentRecord,dwNewRecord);
		}
		else
		{
			TRACE(_T("requesting A=%d, S=%d, CR=%d, NR=%d, CAM=%08lx\n"),
				wArchiv,wSequence,dwCurrentRecord,dwNewRecord,dwCamID);
		}*/
		m_pDatabase->DoRequestRecordNr(wArchiv,
			wSequence,
			dwCurrentRecord,
			dwNewRecord,
			dwCamID);
	}
}
//////////////////////////////////////////////////////////////////////
void CServer::StopSearch()
{
	if (IsConnectedPlay() && IsSearching())
	{
		m_pDatabase->DoResponseQueryResult(0);
	}
	m_bSearching = false;
	StopSearchCopy();
}
//////////////////////////////////////////////////////////////////////
void CServer::StopSearchCopy()
{
	m_bFirstQueryResult = false;
	m_iQueryResultsCopied = 0;
	m_wQueryArchiveNr = 0;
	m_wQuerySequenceNr = 0;
	m_dwQueryRecordNr = 0;
	TRACE(_T("deleting query results by stop search srv %d\n"),m_wID);
	m_QueryResultsToCopy.SafeDeleteAll();
}
//////////////////////////////////////////////////////////////////////
void CServer::ClearSearch()
{
	m_bSearching = false;
}
//////////////////////////////////////////////////////////////////////
void CServer::StartSearch()
{
	if (IsConnectedPlay())
	{
		m_bStopSearchCopy = false;

		TRACE(_T("deleting query results by start search srv%d\n"),m_wID);
		m_QueryResultsToCopy.SafeDeleteAll();

		CIPCFields fields;

		for (int i=0;i<theApp.GetQueryParameter().GetFields().GetSize();i++)
		{
			fields.Add(new CIPCField(*theApp.GetQueryParameter().GetFields().GetAtFast(i)));
		}

		//search between 2 dates and times
		if(theApp.GetQueryParameter().UseTimeFromTo())
		{
			if(    theApp.GetQueryParameter().UseExactDate()
				&& theApp.GetQueryParameter().UseExactTime())
			{

				CString sStartDatum = theApp.GetQueryParameter().GetDateExact().GetDBDate();
				CString sEndDatum = theApp.GetQueryParameter().GetPCDateEnd().GetDBDate();
				CString sStartTime = theApp.GetQueryParameter().GetTimeExact().GetDBTime();
				CString sEndTime = theApp.GetQueryParameter().GetTimeEnd().GetDBTime();

				TRACE(_T("Startzeit: %s, Endzeit: %s, Startdatum: %s, Enddatum: %s\n"),
					sStartTime,
					sEndTime,
					sStartDatum,
					sEndDatum);

				if(sStartDatum == sEndDatum && sStartTime == sEndTime)
				{
					fields.Add(new CIPCField(_T("DBP_DATE"),
						theApp.GetQueryParameter().GetDateExact().GetDBDate(),
						8,_T('D')));
					fields.Add(new CIPCField(_T("DBP_TIME"),
						theApp.GetQueryParameter().GetTimeExact().GetDBTime(),
						8,_T('T')));
				}
				else if(sStartDatum == sEndDatum)
				{
					fields.Add(new CIPCField(_T("DBP_TIME"),
						theApp.GetQueryParameter().GetTimeExact().GetDBTime(),
						8,_T('}')));
					fields.Add(new CIPCField(_T("DBP_TIME"),
						theApp.GetQueryParameter().GetTimeEnd().GetDBTime(),
						8,_T('{')));
					fields.Add(new CIPCField(_T("DBP_DATE"),
						theApp.GetQueryParameter().GetDateExact().GetDBDate(),
						8,_T('D')));				
				}
				else
				{
					fields.Add(new CIPCField(_T("DBP_TIME"),
						theApp.GetQueryParameter().GetTimeExact().GetDBTime(),
						8,_T('}')));
					fields.Add(new CIPCField(_T("DBP_TIME"),
						theApp.GetQueryParameter().GetTimeEnd().GetDBTime(),
						8,_T('{')));
					fields.Add(new CIPCField(_T("DBP_DATE"), 
						theApp.GetQueryParameter().GetDateExact().GetDBDate(), 
						8,_T('}')));
					fields.Add(new CIPCField(_T("DBP_DATE"),
						theApp.GetQueryParameter().GetPCDateEnd().GetDBDate(),
						8,_T('{')));
				}

			}

			else if (theApp.GetQueryParameter().UseExactDate())
			{
				fields.Add(new CIPCField(_T("DBP_DATE"), 
					theApp.GetQueryParameter().GetDateExact().GetDBDate(), 
					8,_T('}')));
				fields.Add(new CIPCField(_T("DBP_DATE"),
					theApp.GetQueryParameter().GetPCDateEnd().GetDBDate(),
					8,_T('{')));
			}
		}
		else
		{
			if (theApp.GetQueryParameter().UseExactDate())
			{

				fields.Add(new CIPCField(_T("DBP_DATE"),
					theApp.GetQueryParameter().GetDateExact().GetDBDate(),
					8,_T('D')));
			}
			if (theApp.GetQueryParameter().UseExactTime())
			{
				fields.Add(new CIPCField(_T("DBP_TIME"),
					theApp.GetQueryParameter().GetTimeExact().GetDBTime(),
					8,_T('T')));
			}
		}

		/*			Y-Achse

		^
		sY1__|_ _ _ _ _______________
		|		|				|
		|		|				|
		|		|	       _|_..|.... Fadenkreuz auf MD-Bild
		|		|			|	|	  mit Koordinaten
		|		|				|	  DBD_MD_X und DBD_MD_Y
		sY2__|_ _ _ _|_______________|
		|		|				|
		|
		|_______|_______________|__ > X-Achse

		|				|
		sX1			   sX2
		*/	

		//search only within the rectangle
		if(theApp.GetQueryParameter().UseRectToQuery())
		{
			CString sX1, sY1, sX2, sY2;
			sX1.Format(_T("%04x"), theApp.GetQueryParameter().GetRectToQuery().left);
			fields.Add(new CIPCField(_T("DBD_MD_X"),
				sX1,
				4, _T('}')));
			sY1.Format(_T("%04x"), theApp.GetQueryParameter().GetRectToQuery().top);
			fields.Add(new CIPCField(_T("DBD_MD_Y"),
				sY1,
				4, _T('}')));
			sX2.Format(_T("%04x"), theApp.GetQueryParameter().GetRectToQuery().right);
			fields.Add(new CIPCField(_T("DBD_MD_X"),
				sX2,
				4, _T('{')));
			sY2.Format(_T("%04x"), theApp.GetQueryParameter().GetRectToQuery().bottom);
			fields.Add(new CIPCField(_T("DBD_MD_Y"),
				sY2,
				4, _T('{')));

		}

		int iNumArchives = m_QueryArchives.GetArchives().GetSize();

		if (m_pDatabase->GetVersion()<3)
		{
			CString sQuery;

			if (iNumArchives+1!=m_pDatabase->GetRecords().GetSize())
			{
				sQuery = m_QueryArchives.GetOldArchives();
			}

			sQuery += fields.BuildOldQueryString();

			if (!sQuery.IsEmpty())
			{
				if (theApp.m_pUser)
				{
					WK_TRACE_USER(_T("%s startet suche %d auf <%s>\n"),
						theApp.m_pUser->GetName(),m_dwQueryID,GetName());
					WK_TRACE_USER(_T("nach <%s>\n"),sQuery);
				}

				m_bFirstQueryResult = !theApp.GetQueryParameter().ShowPictures() ? true : false;
				m_iQueryResultsCopied = 0;
				m_wQueryArchiveNr = 0;
				m_wQuerySequenceNr = 0;
				m_dwQueryRecordNr = 0;
				m_bSearching = true;

				m_pDatabase->DoRequestQuery(m_dwQueryID,sQuery);
				m_dwQueryID += 8;

			}
		}
		else
		{
			WORD* pArchives = NULL;
			if (iNumArchives>0)
			{
				pArchives = new WORD[iNumArchives];
				for (int i=0;i<iNumArchives;i++)
				{
					pArchives[i] = m_QueryArchives.GetArchives().GetAt(i);
				}
			}

			if ((iNumArchives>0) || (fields.GetSize()>0))
			{
				if (theApp.m_pUser)
				{
					CString s,a,t;
					int i;
					for (i=0;i<iNumArchives;i++)
					{
						t.Format(_T("%d,"),pArchives[i]);
						a += t;
					}
					for	(i=0;i<fields.GetSize();i++)
					{
						t.Format(_T("%s,%s;"),fields.GetAtFast(i)->GetName(),
							fields.GetAtFast(i)->GetValue());
						s += t;
					}
					WK_TRACE_USER(_T("%s startet suche %d auf <%s>\n"),
						theApp.m_pUser->GetName(),
						m_dwQueryID,GetName());
					WK_TRACE_USER(_T("in %s\n"),a);
					WK_TRACE_USER(_T("nach %s\n"),s);
				}

				m_bSearching = true;
				m_bFirstQueryResult = true;
				m_dwQueryRecordNr   = 0;
				m_wQueryArchiveNr   = 0;
				m_wQuerySequenceNr   = 0;

				m_pDatabase->DoRequestNewQuery(m_dwQueryID,
					iNumArchives,
					pArchives,
					fields);
				//TRACE("CServer::StartSearch() ID: 0x%x\n", m_dwQueryID);
				m_dwQueryID += 8;
			}
			WK_DELETE_ARRAY(pArchives);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CServer::OnConfirmDrives(int iNumDrives, const CIPCDrive drives[])
{
	m_Drives.SafeDeleteAll();
	m_Drives.FromArray(iNumDrives,drives);

	// are we already in backup mode
	if (IsInBackupMode())
	{
		CViewIdipClient* pViewIdip = theApp.GetMainFrame()->GetViewIdipClient();
		if (pViewIdip)
		{
			CDlgBackup* pBD = pViewIdip->GetDlgBackup();
			if (pBD)
			{
				pBD->PostMessage(WM_USER, ID_DRIVE_INFO, NULL);
			}
		}
	}
	// special case app started in backup mode
//	RKE: BackupMode obsolete
//	else if (IsLocal() && (theApp.m_bBackupModus))
//	{
//		AfxGetMainWnd()->PostMessage(WM_COMMAND,ID_ARCHIV_BACKUP);
//	}
}
//////////////////////////////////////////////////////////////////////
void CServer::AddErrorMessage(const CString& sMessage)
{
	Lock(_T(__FUNCTION__));
	// Workaround for multiple messages e.g _T("No permission") at archive delete
	BOOL bAdd= TRUE;
	int i = m_saErrorMessages.GetSize();
	if (i > 0)
	{
		if (sMessage == m_saErrorMessages.GetAt(i-1))
		{
			// One message is enough
			bAdd = FALSE;
		}
	}
	if (bAdd)
	{
		m_saErrorMessages.Add(sMessage);
	}
	Unlock();
}
//////////////////////////////////////////////////////////////////////
void CServer::AddQueryResult(DWORD dwUserID, WORD wArchivNr,WORD wSequenceNr, DWORD dwRecordNr,
							 int iNumFields, const CIPCField fields[])
{
	CQueryResult* pQR = new CQueryResult(dwUserID,m_wID,wArchivNr,wSequenceNr,
											dwRecordNr,iNumFields,fields);
	m_QueryResultsToCopy.SafeAdd(pQR);

	if (m_bFirstQueryResult)
	{
		CopyFirstQueryResult();
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CServer::IsQueryResult(WORD wArchivNr,WORD wSequenceNr, DWORD dwRecordNr)
{
	BOOL bRet = FALSE;

	m_QueryResultsToCopy.Lock(_T(__FUNCTION__));
	for (int i=0;i<m_QueryResultsToCopy.GetSize();i++)
	{
		CQueryResult* pQueryResult = m_QueryResultsToCopy.GetAtFast(i);
		if (pQueryResult && 
			pQueryResult->IsEqual(m_wID,wArchivNr,wSequenceNr,dwRecordNr))
		{
			bRet = TRUE;
			break;
		}
	}
	m_QueryResultsToCopy.Unlock();

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CServer::CopyFirstQueryResult()
{
	BOOL bRet = FALSE;
	CQueryResult* pQueryResult = NULL;

	m_QueryResultsToCopy.Lock(_T(__FUNCTION__));
	if (m_bFirstQueryResult)
	{
		if(m_QueryResultsToCopy.GetSize()>0)
		{
			pQueryResult = m_QueryResultsToCopy.GetAtFast(0);
			TRACE(_T("CopyFirstQueryResult\n"));
			DoRequestQueryResult(pQueryResult,m_wQueryArchiveNr,m_wQuerySequenceNr,m_dwQueryRecordNr);
			bRet = TRUE;
		}
		m_bFirstQueryResult = false;
	}
	m_QueryResultsToCopy.Unlock();

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CServer::CopyNextQueryResult(WORD wArchivNr,WORD wSequenceNr, DWORD dwRecordNr)
{
	CQueryResult* pQueryResult = NULL;
	BOOL bDoCopy = FALSE;

	m_QueryResultsToCopy.Lock(_T(__FUNCTION__));
	if(m_QueryResultsToCopy.GetSize()>0)
	{
		pQueryResult = m_QueryResultsToCopy.GetAtFast(0);
		if (pQueryResult->IsEqual(m_wID,wArchivNr,wSequenceNr,dwRecordNr))
		{
			m_QueryResultsToCopy.RemoveAt(0);
			WK_DELETE(pQueryResult);
			bDoCopy = TRUE;
			if (m_QueryResultsToCopy.GetSize()>0)
			{
				pQueryResult = m_QueryResultsToCopy.GetAtFast(0);
			}
			else
			{
				TRACE(_T("no more query result to copy\n"));
			}
		}
		else
		{
			//
			TRACE(_T("query result not equal\n"));
			bDoCopy = FALSE;
			pQueryResult = NULL;
		}
	}
	if (pQueryResult)
	{
		DoRequestQueryResult(pQueryResult,wArchivNr,wSequenceNr,dwRecordNr);
	}
	else
	{
		m_bFirstQueryResult = true;
	}
	m_QueryResultsToCopy.Unlock();

	return bDoCopy;
}
/////////////////////////////////////////////////////////////////////////////
void CServer::DoRequestQueryResult(CQueryResult* pQueryResult,WORD wArchivNr,WORD wSequenceNr, DWORD dwRecordNr)
{
	// get the picture and copy it 
	if (IsConnectedPlay())
	{
		if (m_bStopSearchCopy)
		{
			m_bStopSearchCopy = false;
		}
		else
		{
			CIPCDatabaseIdipClient* pDatabase = GetDatabase();
			if (pDatabase->GetVersion()>2)
			{
				DWORD dwCurrentRecord = 0;

				if (   wArchivNr == pQueryResult->GetArchiveNr()
					&& wSequenceNr == pQueryResult->GetSequenceNr())
				{
					dwCurrentRecord = dwRecordNr;
				}
				else
				{
					TRACE(_T("new sequence current record is A%d=%d,S%d=%d\n"),
						wArchivNr,pQueryResult->GetArchiveNr(),
						wSequenceNr,pQueryResult->GetSequenceNr());
				}

				TRACE(_T("request %d,%d,%d,%d\n"),pQueryResult->GetArchiveNr(),
					pQueryResult->GetSequenceNr(),
					dwCurrentRecord,
					pQueryResult->GetRecordNr());

				pDatabase->DoRequestRecordNr(pQueryResult->GetArchiveNr(),
					pQueryResult->GetSequenceNr(),
					dwCurrentRecord,
					pQueryResult->GetRecordNr(),
					(DWORD)-1);
				m_wQueryArchiveNr = pQueryResult->GetArchiveNr();
				m_wQuerySequenceNr = pQueryResult->GetSequenceNr();
				m_dwQueryRecordNr = pQueryResult->GetRecordNr();
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CServer::SetBackupMode(BOOL bBackup)
{
	m_bBackupMode = bBackup ? true : false;
}
//////////////////////////////////////////////////////////////////////
void CServer::Reset()
{
	m_bDoReconnectDataBase = true;
}
//////////////////////////////////////////////////////////////////////
void CServer::UpdateTimeOffset()
{
	if (!IsLocal() && IsConnectedInput())
	{
		m_pInput->UpdateServerTimeOffset();
	}
}
