// MultipleCallProcess.cpp: implementation of the CMultipleCallProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sec3.h"

#include "MultipleCallProcess.h"

#include "CallThread.h"
#include "CIPCInputServerCall.h"
#include "CIPCOutputServerCall.h"

#include "outputlist.h"
#include "wk_msg.h"

 
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMultipleCallProcess::CMultipleCallProcess(CProcessScheduler* pScheduler,CActivation *pActivation, SecProcessType type)
:CProcess(pScheduler)
{
	// m_FailedHosts;
	BOOL bLoaded = FALSE;
	m_iTimeOutCounter = 0;
	m_iConnectionTimeOutCounter = 0;
	m_pCallThread = NULL;
	m_pCIPCInputServerCall = NULL;
	m_pCIPCOutputServerCall = NULL;
	m_pActivation = pActivation;
	pActivation->SetActiveProcess(this);
	InitTimer();
	m_type = type;

	m_iCurrentHost = 0;

	CWK_Profile wkp;
	CString sDir = wkp.GetString(SECTION_LOG, LOG_PROTOCOLPATH, DEFAULT_PROTOCOLPATH);

	CString sFileName = sDir + _T("\\CheckCall.ini");

	DWORD dwFA = GetFileAttributes(sFileName);		// File present?
	if (dwFA != INVALID_FILE_ATTRIBUTES && !(dwFA & FILE_ATTRIBUTE_DIRECTORY))
	{
		CWK_Profile wkpIni(CWK_Profile::WKP_INIFILE_ABSOLUT, NULL, _T(""), sFileName);
		BOOL bUBF = m_Hosts.m_bUseBinaryFormat;
		m_Hosts.m_bUseBinaryFormat = FALSE;
		m_Hosts.Load(wkpIni);						// Load Hosts from ini file
		m_Hosts.m_bUseBinaryFormat = bUBF;

		CHost *pHost;								// Check the Hosts
		int i, nHosts = m_Hosts.GetCount();
		WK_TRACE(_T("loading %s %d hosts\n"),sFileName,nHosts);
		for (i=0; i<nHosts; i++)
		{
			pHost = m_Hosts.GetAtFast(i);
			if (pHost->IsPINRequired())				// if Pin is required
			{
				pHost->SetPIN(_T("0000"));			// generate standard pin
			}
		}
		bLoaded = nHosts > 0 ? TRUE : FALSE;
	}

	if (!bLoaded)									// default functionality 
	{
		m_Hosts.Load(wkp);							// load all hosts from registry
		bLoaded = TRUE;
	}

	m_eState = MCS_INITIALIZED;
	m_iStartUpCounter = 15;

//	InitProtocolFile();
	m_bProtokolInitialised = FALSE;

	if (IsImageCallProcess())
	{
		theApp.GetOutputGroups().WriteNamesIni();
	}

}
//////////////////////////////////////////////////////////////////////
CMultipleCallProcess::~CMultipleCallProcess()
{
	ExitProtocolFile();
	CleanUpCIPC();
}
//////////////////////////////////////////////////////////////////////
void CMultipleCallProcess::CleanUpCIPC()
{
	m_iTimeOutCounter = 0;
	m_iStartUpCounter = 0;
	WK_DELETE(m_pCallThread);
	WK_DELETE(m_pCIPCInputServerCall);
	WK_DELETE(m_pCIPCOutputServerCall);
}
//////////////////////////////////////////////////////////////////////
void CMultipleCallProcess::CheckError()
{
	CString sIni;
	if (IsImageCallProcess())
	{
		CString s;
		sIni.Format(_T("%s\\ProcessResults.ini"),
			CNotificationMessage::GetWWWRoot());
		s.Format(_T("%d"),m_FailedHosts.GetSize());
		WritePrivateProfileString(_T("Failed"),_T("Counter"),s,sIni);
	}
	if (m_FailedHosts.GetSize()>0)
	{
		CString sAll;
		int j = 0;

		for (int i=0;i<m_FailedHosts.GetSize();i++)
		{
			if (IsCheckCallProcess())
			{
				sAll += m_FailedHosts.GetAt(i);
				if (i<m_FailedHosts.GetSize()-1)
				{
					sAll += _T(",");
				}
			}
			if (IsImageCallProcess())
			{
				CString sKey;
				sKey.Format(_T("%d"),j++);
				WritePrivateProfileString(_T("Failed"),sKey,m_FailedHosts.GetAt(i),sIni);
			}
			WK_TRACE_ERROR(_T("%s , %s failed\n"),GetName(),m_FailedHosts.GetAt(i));
		}

		if (IsImageCallProcess())
		{
			// flush the ini file
			WritePrivateProfileString(NULL,NULL,NULL,sIni);
		}

		if (IsCheckCallProcess())
		{
			{
				CWK_RunTimeError e(WAI_SECURITY_SERVER,
								   REL_ERROR,
								   RTE_CHECK_CALL_ERROR,
								   sAll,0,1);
				e.Send();
			}
			Sleep(50);
			{
				CWK_RunTimeError e(WAI_SECURITY_SERVER,
								   REL_ERROR,
								   RTE_CHECK_CALL_ERROR,
								   NULL,0,0);
				e.Send();
			}
			CString s;
			s.LoadString(IDS_FAILED_HOSTS);
			WriteProtocol(_T("<h3>") + s + _T("</h3>\n"));
			WriteProtocol(sAll + _T("<br>\n"));
		}
	}
	else
	{
		// OK All hosts reached
		if (IsCheckCallProcess())
		{
			{
				CWK_RunTimeError e(WAI_SECURITY_SERVER,
								   REL_ERROR,
								   RTE_CHECK_CALL_OK,
								   NULL,0,1);
				e.Send();
			}
			Sleep(50);
			{
				CWK_RunTimeError e(WAI_SECURITY_SERVER,
								   REL_ERROR,
								   RTE_CHECK_CALL_OK,
								   NULL,0,0);
				e.Send();
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
CHost* CMultipleCallProcess::GetCurrentHost()
{
	if (m_iCurrentHost < m_Hosts.GetSize())
	{
		return m_Hosts.GetAt(m_iCurrentHost);
	}
	else
	{
		return NULL;
	}
}
//////////////////////////////////////////////////////////////////////
CHost* CMultipleCallProcess::GetValidHost(BOOL bNext)
{
	CHost* pHost = NULL;
	BOOL bFound = FALSE;
	int i = m_iCurrentHost;
	if (bNext)
	{
		i++;
	}
	for (i ; !bFound && i<m_Hosts.GetSize() ; i++)
	{
		pHost = m_Hosts.GetAt(i);
		if (IsValidHost(pHost))
		{
			m_iCurrentHost = i;
			bFound = TRUE;
		}
	}
	if (!bFound)
	{
		pHost = NULL;
	}
	return pHost;
}
//////////////////////////////////////////////////////////////////////
void CMultipleCallProcess::Run()
{
	switch (m_eState)
	{
	case MCS_INITIALIZED:
		TRACE(_T("MCS_INITIALIZED\n"));
		OnInitialized();
		break;
	case MCS_START:
		TRACE(_T("MCS_START\n"));
		OnStart();
		break;
	case MCS_FETCHING:
		TRACE(_T("MCS_FETCHING\n"));
		OnFetching();
		break;
	case MCS_CONNECTING:
		TRACE(_T("MCS_CONNECTING\n"));
		OnConnecting();
		break;
	case MCS_CONNECTED:
		TRACE(_T("MCS_CONNECTED\n"));
		OnConnection();
		break;
	case MCS_RUNNING_LEVEL_1:
		TRACE(_T("MCS_RUNNING_LEVEL_1\n"));
		OnRunningLevel1();
		break;
	case MCS_RUNNING_LEVEL_2:
		TRACE(_T("MCS_RUNNING_LEVEL_2\n"));
		OnRunningLevel2();
		break;
	case MCS_RUNNING_LEVEL_3:
		TRACE(_T("MCS_RUNNING_LEVEL_3\n"));
		OnRunningLevel3();
		break;
	case MCS_CONFIRMED:
		TRACE(_T("MCS_CONFIRMED\n"));
		OnConfirm();
		break;
	case MCS_FAILED:
		TRACE(_T("MCS_FAILED\n"));
		NextCall();
		break;
	case MCS_FINISHED:
		TRACE(_T("MCS_FINISHED\n"));
		CleanUpCIPC();
		CheckError();
		SetState(SPS_TERMINATED,WK_GetTickCount());
		break;
	default:
		WK_TRACE_ERROR(_T("invalid call process state\n"));
		break;
	}
}
//////////////////////////////////////////////////////////////////////
void CMultipleCallProcess::OnPause()
{
}
//////////////////////////////////////////////////////////////////////
void CMultipleCallProcess::OnInitialized()
{
	m_iStartUpCounter++;
	if (m_iStartUpCounter>15)
	{
		m_eState = MCS_START;
		m_iStartUpCounter = 0;
	}
}
//////////////////////////////////////////////////////////////////////
void CMultipleCallProcess::OnStart()
{
	m_iTimeOutCounter = 0;
	m_iStartUpCounter = 0;

	if (!m_bProtokolInitialised)
	{
		m_bProtokolInitialised = TRUE;
		InitProtocolFile();
	}

	// ML 23.9.99 Die Liste könnte leer sein. Wenn keine Gegenstationen eingetragen
	// sind stürzte der Server bei einem Referenzbildprozeß an dieser Stelle ab.
	// Statt dessen wird der State auf MCS_FINISHED gesetzt.
	if (m_Hosts.GetSize() > 0)
	{
		CHost *pHost = GetValidHost(FALSE);
		
		if (pHost)
		{
			WriteProtocol(_T("<h3>") + pHost->GetName() + _T(" ; ") + pHost->GetNumber() + _T("</h3>\n"));
			CString s;
			s.LoadString(IDS_START_CONNECT);
			CSystemTime st;
			st.GetLocalTime();
			WriteProtocol(st.GetTime() + _T(" ") + s + _T("<br>\n"));
			WK_TRACE(_T("starting connection to %s\n"),pHost->GetName());
			if (pHost->IsISDN())
			{
				HWND hWnd = FindWindow(WK_APP_NAME_LAUNCHER, NULL);
				if (hWnd)
				{
					::PostMessage(hWnd, LAUNCHER_APPLICATION, WAI_ISDN_UNIT, MAKELONG(WM_ISDN_B_CHANNELS, 0));
				}
			}

			m_pCallThread = new CCallThread(this);
			m_pCallThread->StartThread();
			m_eState = MCS_FETCHING;
		}
		else
		{
			WK_TRACE(_T("no valid host found\n"));
			CString s;
			s.LoadString(IDS_NO_VALID_HOST_FOUND);
			WriteProtocol(_T("<h3>") + s + _T("</h3>\n"));
			m_eState = MCS_FINISHED;
		}
	}
	else
	{
		WK_TRACE(_T("host list is empty\n"));
		CString s;
		s.LoadString(IDS__HOST_LIST_EMPTY);
		WriteProtocol(_T("<h3>") + s + _T("</h3>\n"));
		m_eState = MCS_FINISHED;
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CMultipleCallProcess::IsConnected() const
{
	if (   m_pCIPCInputServerCall 
		&& m_pCIPCOutputServerCall)
	{
		if ((m_pCIPCInputServerCall->GetIPCState()==CIPC_STATE_CONNECTED) &&
			(m_pCIPCOutputServerCall->GetIPCState()==CIPC_STATE_CONNECTED))
		{
			return TRUE;
		}
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CMultipleCallProcess::IsDisconnected() const
{
	if (   m_pCIPCInputServerCall 
		&& m_pCIPCOutputServerCall)
	{
		if ((m_pCIPCInputServerCall->GetIsMarkedForDelete()) ||
			(m_pCIPCOutputServerCall->GetIsMarkedForDelete()))
		{
			return TRUE;
		}
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CMultipleCallProcess::IsValidHost(CHost* pHost) const
{
	BOOL bRet = FALSE;
	if (pHost)
	{
		if (IsCheckCallProcess())
		{
			if (   !pHost->IsSMS()
				&& !pHost->IsEMail()
				&& !pHost->IsFAX()
				)
			{
				bRet = TRUE;
			}
		}
		else if (IsImageCallProcess())
		{
			if (   !pHost->IsSMS() 
				&& !pHost->IsEMail()
				&& !pHost->IsFAX()
				&& !pHost->IsTOBS()
				&& !pHost->IsB3()
				&& !pHost->IsB6()
				)
			{
				bRet = TRUE;
			}
		}
	}
	else
	{
		bRet = FALSE;
	}
	return bRet;
}
//////////////////////////////////////////////////////////////////////
void CMultipleCallProcess::OnFetching()
{
	if (m_pCallThread)
	{
		BOOL bInputOK = TRUE;
		m_pCallThread->Lock();
		CIPCFetchResult* pInputResult = m_pCallThread->GetInputResult();
		if (pInputResult)
		{
			CIPCFetchResult inputResult(*pInputResult);
			m_pCallThread->Unlock();
			CString shmInputName;
			shmInputName = inputResult.GetShmName();
			if (   (inputResult.GetError()!=CIPC_ERROR_OKAY) 
				|| (shmInputName.GetLength()==0))
			{
				m_FailedHosts.Add(GetCurrentHost()->GetName());
				WK_TRACE(_T("fetch failed %s\n"),GetCurrentHost()->GetName());
				CString s, sReason, sError;
				s.LoadString(IDS_NO_CONNECTION);
				sReason.LoadString(IDS_REASON);
				CSystemTime st;
				st.GetLocalTime();
				CString sTime = st.GetTime();
				CString sEmpty = CString(_T("&nb"), sTime.GetLength());

				if (inputResult.GetError()!=CIPC_ERROR_OKAY)
				{
					sError = inputResult.GetErrorMessage();
				}
				else
				{
					sError = _T("Internal error");
				}
				WriteProtocol(sTime + _T(" ") + s + _T("<br>\n"));
				WriteProtocol(_T("<pre>       </pre>") + sReason + _T(" ") + sError + _T("<br>\n"));
				NextCall();
				bInputOK = FALSE;
			}
			else
			{
				WK_TRACE(_T("multiple call connecting input to %s\n"),GetCurrentHost()->GetName());
				
				m_iConnectionTimeOutCounter = 0;
				
				m_pCIPCInputServerCall = new CIPCInputServerCall(this,inputResult.GetShmName());
				m_pCallThread->DeleteInputResult();
			}
		}
		else
		{
			m_pCallThread->Unlock();
		}

		if (bInputOK)
		{
			m_pCallThread->Lock();
			CIPCFetchResult* pOutputResult = m_pCallThread->GetOutputResult();
			
			if (pOutputResult)
			{
				CIPCFetchResult outputResult(*pOutputResult);
 				
				m_pCallThread->Unlock();

				CString shmOutputName;
				shmOutputName = outputResult.GetShmName();
				if (   (outputResult.GetError()==CIPC_ERROR_OKAY)
					&& (shmOutputName.GetLength()>0)
					)
				{
					WK_TRACE(_T("multiple call connecting output to %s\n"),GetCurrentHost()->GetName());
					m_iConnectionTimeOutCounter = 0;
					m_pCIPCOutputServerCall = new CIPCOutputServerCall(this,outputResult.GetShmName());
					m_pCallThread->DeleteOutputResult();
					m_eState = MCS_CONNECTING;
				}
				else
				{
					m_FailedHosts.Add(GetCurrentHost()->GetName());
					WK_TRACE(_T("fetch failed %s\n"),GetCurrentHost()->GetName());
					CString s, sReason, sError;
					s.LoadString(IDS_NO_CONNECTION);
					sReason.LoadString(IDS_REASON);
					CSystemTime st;
					st.GetLocalTime();
					CString sTime = st.GetTime();
					CString sEmpty = CString(_T("&nb"), sTime.GetLength());

					if (outputResult.GetError()!=CIPC_ERROR_OKAY)
					{
						sError = outputResult.GetErrorMessage();
					}
					else
					{
						sError = _T("Internal error");
					}
					WriteProtocol(sTime + _T(" ") + s + _T("<br>\n"));
					WriteProtocol(_T("<pre>       </pre>") + sReason + _T(" ") + sError + _T("<br>\n"));
					NextCall();
				}
			}
			else
			{
				m_pCallThread->Unlock();
				WK_TRACE(_T("call waiting for connection %s\n"),GetCurrentHost()->GetName());
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CMultipleCallProcess::OnConnecting()
{
	if (IsConnected())
	{
		WK_TRACE(_T("connected to remote host\n"));
		m_eState = MCS_CONNECTED;
	}
	else
	{
		m_iConnectionTimeOutCounter++;
		if (m_iConnectionTimeOutCounter>30)
		{
			WK_TRACE(_T("call timeout, connection failed\n"));
			m_iConnectionTimeOutCounter = 0;
			m_FailedHosts.Add(GetCurrentHost()->GetName());
			m_eState = MCS_FAILED;
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CMultipleCallProcess::OnConnection()
{
	if (IsConnected())
	{
		CString s;
		s.LoadString(IDS_CONNECTION);
		CSystemTime st;
		st.GetLocalTime();
		WriteProtocol(st.GetTime() + _T(" ") + s + _T("<br>\n"));
		if (IsImageCallProcess())
		{
			WK_TRACE(_T("got connection requesting cameras %s\n"),
				GetCurrentHost()->GetName());
			CString s;
			s.LoadString(IDS_REQUEST_CAMERA);
			CSystemTime st;
			st.GetLocalTime();
			WriteProtocol(st.GetTime() + _T(" ") + s + _T("<br>\n"));
			m_eState = MCS_RUNNING_LEVEL_1;
		}
		else if (IsCheckCallProcess())
		{
			if (GetMacro().GetSetTime())
			{
				m_pCIPCInputServerCall->RequestSetTime();
			}
			m_eState = MCS_RUNNING_LEVEL_1;
		}
	}
	else
	{
		m_eState = MCS_FAILED;
	}
}
//////////////////////////////////////////////////////////////////////
void CMultipleCallProcess::OnRunningLevel1()
{
	if (IsConnected())
	{
		if (IsImageCallProcess())
		{
			CString s;
			// waiting for images
			if (m_pCIPCOutputServerCall->IsReady())
			{
				WK_TRACE(_T("got information about cameras requesting images %s\n"),
					GetCurrentHost()->GetName());
				s.LoadString(IDS_CONFIRM_CAMERA);
				CSystemTime st;
				st.GetLocalTime();
				WriteProtocol(st.GetTime() + _T(" ") + s + _T("<br>\n"));
				m_pCIPCInputServerCall->RequestImages();
				m_eState = MCS_RUNNING_LEVEL_2;
			}
			m_iTimeOutCounter++;
			if (m_iTimeOutCounter>20*60) // zwanzig Minuten = 1200 s timeout
			{
				s.LoadString(IDS_TIMEOUT_IMAGE);
				CSystemTime st;
				st.GetLocalTime();
				WriteProtocol(st.GetTime() + _T(" ") + s + _T("<br>\n"));
				m_eState = MCS_CONFIRMED;
			}
		}
		else if (IsCheckCallProcess())
		{
			CString s;

			// waiting for set time
			if (   m_pCIPCInputServerCall->IsReady()
				&& m_pCIPCOutputServerCall->IsReady())
			{
				if (GetMacro().GetSetTime())
				{
					if (m_pCIPCInputServerCall->IsError())
					{
						s.LoadString(IDS_NO_CONFIRM_TIME);
					}
					else
					{
						s.LoadString(IDS_CONFIRM_TIME);
					}
					CSystemTime st;
					st.GetLocalTime();
					WriteProtocol(st.GetTime() + _T(" ") + s + _T("<br>\n"));
				}
				m_eState = MCS_CONFIRMED;
			}
			m_iTimeOutCounter++;
			if (m_iTimeOutCounter>20) // 20 s time out to set time
			{
				s.LoadString(IDS_TIMEOUT_CHECK);
				CSystemTime st;
				st.GetLocalTime();
				WriteProtocol(st.GetTime() + _T(" ") + s + _T("<br>\n"));
				m_eState = MCS_CONFIRMED;
			}
		}
	}
	else
	{
		WK_TRACE_ERROR(_T("connection lost in waiting for cameras %s\n"),
			GetCurrentHost()->GetName());
		m_eState = MCS_FAILED;
	}
}
//////////////////////////////////////////////////////////////////////
void CMultipleCallProcess::OnRunningLevel2()
{
	if (IsConnected())
	{
		if (m_pCIPCInputServerCall->IsReady())
		{
			WK_TRACE(_T("disconnecting %s\n"),GetCurrentHost()->GetName());
			WK_DELETE(m_pCIPCInputServerCall);
			m_eState = MCS_CONFIRMED;
		}
	}
	else
	{
		WK_TRACE_ERROR(_T("connection lost in waiting for images %s\n"),
			GetCurrentHost()->GetName());
		m_eState = MCS_FAILED;
	}
}
//////////////////////////////////////////////////////////////////////
void CMultipleCallProcess::OnRunningLevel3()
{
}
//////////////////////////////////////////////////////////////////////
void CMultipleCallProcess::OnConfirm()
{
	CString s;
	s.LoadString(IDS_DISCONNECT);
	CSystemTime st;
	st.GetLocalTime();
	WriteProtocol(st.GetTime() + _T(" ") + s + _T("<br>\n"));

	NextCall();
}
//////////////////////////////////////////////////////////////////////
void CMultipleCallProcess::NextCall()
{
	CHost* pHost = GetValidHost(TRUE);
	if (pHost)
	{
		WK_TRACE(_T("next call\n"));
		CleanUpCIPC();
		m_eState = MCS_INITIALIZED;
	}
	else
	{
		WK_TRACE(_T("all hosts called, finishing process\n"));
		CString s;
		s.LoadString(IDS_ALL_HOSTS_CALLED);
		WriteProtocol(_T("<h3>") + s + _T("</h3>\n"));
		m_eState = MCS_FINISHED;
	}
}
