// DsIpCamera.cpp : implementation file
//

#include "stdafx.h"

#include "IPCameraUnit.h"
#include "IPCameraUnitDlg.h"

#include "IPCamSocket.h"
#include "CIPCOutputIPcamUnit.h"
#include "DsIpCamera.h"

#define RECEIVE_TIMER 11

LPCTSTR NameOfHTTPStatus(DWORD dwStatus)
{
	switch(dwStatus)
	{
		NAME_OF_ENUM(HTTP_STATUS_CREATED);
		NAME_OF_ENUM(HTTP_STATUS_ACCEPTED);
		NAME_OF_ENUM(HTTP_STATUS_NO_CONTENT);
		NAME_OF_ENUM(HTTP_STATUS_REDIRECT);
//		NAME_OF_ENUM(HTTP_STATUS_TEMP_REDIRECT);
		NAME_OF_ENUM(HTTP_STATUS_NOT_MODIFIED);
		NAME_OF_ENUM(HTTP_STATUS_BAD_REQUEST);
//		NAME_OF_ENUM(HTTP_STATUS_AUTH_REQUIRED);
		NAME_OF_ENUM(HTTP_STATUS_FORBIDDEN);
		NAME_OF_ENUM(HTTP_STATUS_NOT_FOUND);
		NAME_OF_ENUM(HTTP_STATUS_REQUEST_TOO_LARGE);
		NAME_OF_ENUM(HTTP_STATUS_SERVER_ERROR);
//		NAME_OF_ENUM(HTTP_STATUS_NOT_IMPLEMENTED);
		NAME_OF_ENUM(HTTP_STATUS_BAD_GATEWAY);
//		NAME_OF_ENUM(HTTP_STATUS_SERVICE_NA);
	}
	return _T("");
}

/////////////////////////////////////////////////////////////////////////////
// CDsIpCamera
IMPLEMENT_DYNAMIC(CDsIpCamera, CDsCamera)
/////////////////////////////////////////////////////////////////////////////
CDsIpCamera::CDsIpCamera(const CString&sName, DWORD dwFlags) :
	CDsCamera(sName, dwFlags)
{
	m_nPort = INTERNET_DEFAULT_HTTP_PORT;
	m_pInternetSession = NULL;
	m_pHttpServer = NULL;
	m_pStream = NULL;

	m_pClient = NULL;
	m_pEvents = NULL;
	m_pRTCSession = NULL;

	m_pPiture = NULL;
	m_lCurrentPosition = -1;
}
/////////////////////////////////////////////////////////////////////////////
CDsIpCamera::~CDsIpCamera()
{
	CloseStream();
	CloseHTTPServer();
	CloseSession();

	WK_DELETE(m_pPiture);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDsIpCamera::InitInstance()
{
	BOOL bReturn = CDsCamera::InitInstance();

	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CDsIpCamera::InitFilters()
{
	HRESULT hr = S_OK;
	if (m_CompressionType == COMPRESSION_MPEG4)
	{
		try
		{
			if (m_dwFlags & DSTF_NO_GRAPH_INIT)
			{
				hr = CoInitialize(0);
			}
			HRESULT_EXCEPTION(hr);

			// Initialize the RTC COM object
			hr = CoCreateInstance (CLSID_RTCClient, NULL, CLSCTX_INPROC_SERVER, IID_IRTCClient, (LPVOID *)&m_pClient);
			HRESULT_EXCEPTION(hr);

			hr = m_pClient->Initialize();
			HRESULT_EXCEPTION(hr);

			m_pClient->SetPreferredMediaTypes ( RTCMT_VIDEO_RECEIVE, VARIANT_TRUE);
			HRESULT_EXCEPTION(hr);

			long lEventMask = RTCEF_SESSION_STATE_CHANGE |
							RTCEF_MESSAGING |
							RTCEF_MEDIA |
							RTCEF_INTENSITY |
							RTCEF_CLIENT;

			// Initialize the event filter this demo application want callback.
			hr = m_pClient->put_EventFilter( lEventMask );
			HRESULT_EXCEPTION(hr);

			m_pEvents = new CIpCamRTCEvents;
			if (!m_pEvents) HRESULT_ERROR_AT_POS(E_OUTOFMEMORY);

			// initialize the event handler
			hr = m_pEvents->Advise( m_pClient, this );
			HRESULT_EXCEPTION(hr);
			// Set the listen mode for RTC client
			// RTCLM_BOTH opens the standard SIP port 5060, as well as
			// a dynamic port.
			hr = m_pClient->put_ListenForIncomingSessions(RTCLM_BOTH); 
			HRESULT_EXCEPTION(hr);
		}
		catch (ErrorStruct *ps)
		{
			ReleaseFilters();
			ReportError(ps, FALSE);
		}
	}
	else
	{
		hr = CDsCamera::InitFilters();
	}
	return hr;
}
/////////////////////////////////////////////////////////////////////////////
void CDsIpCamera::ReleaseFilters()
{
	TRACE(_T("ReleaseFilters%d(): %s\n"), GetCameraSubID(), GetTickCountEx());
	
	if (m_pRTCSession)
	{
		RTC_TERMINATE_REASON term = RTCTR_NORMAL;
		if (theApp.GetDlg()->IsShuttingDown())
		{
			term = RTCTR_SHUTDOWN;
		}
		m_pRTCSession->Terminate(term);
		m_pRTCSession->Release();
		m_pRTCSession = NULL;
	}
	if (m_pClient)
	{
		m_pClient->PrepareForShutdown();

		if (m_pEvents)
		{
			m_pEvents->Unadvise(m_pClient);
			m_pEvents = NULL;
		}
		m_pClient->Shutdown();
		m_pClient->Release();
		m_pClient = NULL;
	}

	StartReceiveTimer(FALSE);
	CDsCamera::ReleaseFilters();
}
/////////////////////////////////////////////////////////////////////////////
int CDsIpCamera::ExitInstance()
{
	return CDsCamera::ExitInstance();
}
/////////////////////////////////////////////////////////////////////////////
CString CDsIpCamera::GetLocation()
{
	return m_sURL;
}
/////////////////////////////////////////////////////////////////////////////
CString CDsIpCamera::GetType()
{
	return _T("DirectShow IP Camera");
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CDsIpCamera::OnEvent(LONG lCode, LONG lParam1, LONG lParam2)
{
	HRESULT hr = S_FALSE;
	if (lCode == EC_SET_MEDIA_STATE)
	{
		OAFilterState newState = (OAFilterState)lParam1;
		TRACE(_T("OnEvent%d(set media state:%s): %s\n"), GetCameraSubID(), NameOfEnum(newState), GetTickCountEx());
		if (newState == State_Running)
		{
			if (m_pStream)
			{
				DWORD dwResult;
				m_pStream->QueryInfoStatusCode(dwResult);
				if (dwResult == HTTP_STATUS_OK)
				{
					hr = StartReceiveTimer();
				}
				else
				{
					CIpCameraUnitDlg::ReadHTTPAnswer(m_pStream, dwResult);
					hr = E_PENDING;
				}
			}
			else
			{
				hr = E_POINTER;
			}
			if (FAILED(hr))
			{
				TRACE(_T("stop media: %s\n"), GetTickCountEx());
				ReleaseFilters();
			}
		}
		else
		{
			hr = StartReceiveTimer(FALSE);
		}
	}
	return hr;
}

#define TRACE_EVENT(x)  case x: TRACE(_T(#x) _T(" %s: %s\n"), GetName(), GetTickCountEx()); break;
/////////////////////////////////////////////////////////////////////////////
void CDsIpCamera::OnInternetEvent(DWORD dwContext,	// Context
									 DWORD dwIS,		// InternetStatus
									 void* lpvSI,		// StatusInformation
									 DWORD dwSIlen)		// StatusInformationLength
{
	switch (dwIS)
	{
		case INTERNET_STATUS_REQUEST_SENT:
			TRACE(_T("INTERNET_STATUS_REQUEST_SENT %s: %s\n"), GetName(), GetTickCountEx());
			if (dwContext == (DWORD)CONTEXTID_STREAM)
			{
				SetMediaState(State_Running);
			} break;
		case INTERNET_STATUS_CONNECTION_CLOSED:
			TRACE(_T("INTERNET_STATUS_CONNECTION_CLOSED %s: %s\n"), GetName(), GetTickCountEx());
			if (   dwContext == (DWORD)CONTEXTID_STREAM
				&& m_sMediaState == State_Running 
                && !theApp.GetDlg()->IsShuttingDown())
			{
				SetDirty(TRUE);
				SetMediaState(State_Running);
			}
			break;
		TRACE_EVENT(INTERNET_STATUS_RESOLVING_NAME);
		TRACE_EVENT(INTERNET_STATUS_NAME_RESOLVED);
		TRACE_EVENT(INTERNET_STATUS_CONNECTING_TO_SERVER);
		TRACE_EVENT(INTERNET_STATUS_CONNECTED_TO_SERVER);
		TRACE_EVENT(INTERNET_STATUS_SENDING_REQUEST);
//		TRACE_EVENT(INTERNET_STATUS_RECEIVING_RESPONSE);
//		TRACE_EVENT(INTERNET_STATUS_RESPONSE_RECEIVED);
		TRACE_EVENT(INTERNET_STATUS_CTL_RESPONSE_RECEIVED);
		TRACE_EVENT(INTERNET_STATUS_PREFETCH);
		TRACE_EVENT(INTERNET_STATUS_CLOSING_CONNECTION);
		TRACE_EVENT(INTERNET_STATUS_HANDLE_CREATED);
		TRACE_EVENT(INTERNET_STATUS_HANDLE_CLOSING);
		TRACE_EVENT(INTERNET_STATUS_DETECTING_PROXY );
		TRACE_EVENT(INTERNET_STATUS_REQUEST_COMPLETE);
		TRACE_EVENT(INTERNET_STATUS_REDIRECT);
		TRACE_EVENT(INTERNET_STATUS_INTERMEDIATE_RESPONSE);
		TRACE_EVENT(INTERNET_STATUS_USER_INPUT_REQUIRED);
		TRACE_EVENT(INTERNET_STATUS_STATE_CHANGE);
		TRACE_EVENT(INTERNET_STATUS_COOKIE_SENT);
		TRACE_EVENT(INTERNET_STATUS_COOKIE_RECEIVED);
		TRACE_EVENT(INTERNET_STATUS_PRIVACY_IMPACTED);
		TRACE_EVENT(INTERNET_STATUS_P3P_HEADER);
		TRACE_EVENT(INTERNET_STATUS_P3P_POLICYREF);
		TRACE_EVENT(INTERNET_STATUS_COOKIE_HISTORY);
	}
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CDsIpCamera::OnRTCEvent(RTC_EVENT enEvent, IDispatch *pDisp)
{
    switch (enEvent)
    {
/*		case RTCE_MEDIA:
		{
			IRTCMediaEvent * pEvent = NULL;

			// Get the event handle associated with the current session.
			hr = pDisp->QueryInterface( IID_IRTCMediaEvent, (void **)&pEvent );

			if (SUCCEEDED(hr))
			{
			}
			RELEASE_OBJECT(pEvent);

		}  break;
*/
		TRACE_EVENT(RTCE_REGISTRATION_STATE_CHANGE);
		TRACE_EVENT(RTCE_SESSION_STATE_CHANGE);
		TRACE_EVENT(RTCE_SESSION_OPERATION_COMPLETE);
		TRACE_EVENT(RTCE_PARTICIPANT_STATE_CHANGE);
		TRACE_EVENT(RTCE_MEDIA);
		TRACE_EVENT(RTCE_INTENSITY);
		TRACE_EVENT(RTCE_MESSAGING);
		TRACE_EVENT(RTCE_BUDDY);
		TRACE_EVENT(RTCE_WATCHER);
		TRACE_EVENT(RTCE_PROFILE);
	}

	return S_OK;
}
#undef TRACE_EVENT
/////////////////////////////////////////////////////////////////////////////
void CDsIpCamera::SetURLandPort(const CString&sURL, int nPort)
{
	if (   m_sURL != sURL
		|| m_nPort != nPort)
	{
		CloseStream();
		CloseHTTPServer();
		m_sURL = sURL;
		m_nPort = nPort;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDsIpCamera::SetProxy(CString&sProxyName, CString&sProxyBypass)
{
	if (   sProxyName   != m_sProxyName
		|| sProxyBypass != m_sProxyBypass)
	{
		CloseStream();
		CloseHTTPServer();
		WK_DELETE(m_pInternetSession);
		m_sProxyName = sProxyName;
		m_sProxyBypass = sProxyBypass;
	}
}
CInternetSession* CDsIpCamera::GetSession()
{
	if (m_pInternetSession == NULL)
	{
		LPCTSTR pszProxyName = NULL, pszProxyBypass = NULL;
		DWORD dwAccessType = INTERNET_OPEN_TYPE_DIRECT;	// Connect directly to Internet
		if (!m_sProxyName.IsEmpty())
		{
			pszProxyName = m_sProxyName;
		}
		if (!m_sProxyBypass.IsEmpty())
		{
			pszProxyBypass = m_sProxyBypass;
		}
		if (pszProxyName || pszProxyBypass)
		{
			dwAccessType = INTERNET_OPEN_TYPE_PROXY;	// Connect through a CERN proxy
		}
		// PRE_CONFIG_INTERNET_ACCESS					// Connect using preconfigured settings in the registry
		m_pInternetSession = new CIpCamInetSession(this, GetName(),
			1,											// context
			dwAccessType,								
			pszProxyName, pszProxyBypass,				// Proxy name and bypass
			INTERNET_FLAG_DONT_CACHE);					// flags
		m_pInternetSession->EnableStatusCallback();
		TRACE(_T("InternetSession %s created\n"), GetName());
	}
	return m_pInternetSession;
}
/////////////////////////////////////////////////////////////////////////////
CHttpConnection* CDsIpCamera::GetHTTPConnection()
{
	if (m_pHttpServer == NULL)
	{
		// TODO! RKE: implement username and password if necessary
		m_pHttpServer = GetSession()->GetHttpConnection(m_sURL, (INTERNET_PORT)m_nPort);
		TRACE(_T("HttpConnection %s:%s:%d created\n"), GetName(), m_sURL, m_nPort);
	}
	return m_pHttpServer;
}

/////////////////////////////////////////////////////////////////////////////
CString CDsIpCamera::HTTPRequest(const CString &sRequest, DWORD &dwResult, BOOL bPost/*=0*/, DWORD dwContext/*=1*/)
{
	CString sAnswer;
	CHttpFile *pFile = NULL;
	CHttpConnection *pCon = NULL;
	CInternetSession* pSession = NULL;

	try
	{
		// PRE_CONFIG_INTERNET_ACCESS					// Connect using preconfigured settings in the registry
		pSession = new CInternetSession(GetName(),
			dwContext, //1,											// context
			INTERNET_OPEN_TYPE_DIRECT,								
			NULL, NULL,									// Proxy name and bypass
			INTERNET_FLAG_DONT_CACHE);					// flags

		pCon  = pSession->GetHttpConnection(m_sURL, (INTERNET_PORT)m_nPort);
		pFile = pCon->OpenRequest(bPost ? CHttpConnection::HTTP_VERB_PUT : CHttpConnection::HTTP_VERB_GET, 
			sRequest,
			NULL, // pstrReferer
			dwContext,
			NULL, // ppstrAcceptTypes
			NULL, // pstrVersion
			INTERNET_FLAG_RELOAD|INTERNET_FLAG_DONT_CACHE/*|INTERNET_FLAG_EXISTING_CONNECT*/);
		pFile->SetOption(INTERNET_OPTION_SEND_TIMEOUT, 10000);
		pFile->SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 10000);

		// Make the request
		if (pFile->SendRequest())
		{
			// Check the response to the request
			sAnswer = CIpCameraUnitDlg::ReadHTTPAnswer(pFile, dwResult);

			if (dwResult != HTTP_STATUS_OK)
			{
				WK_TRACE(sAnswer);
			}
		}
	}
	catch (CInternetException* pEx)
	{
		theApp.ReportError(pEx);
	}

	if (pFile)
	{
		pFile->Close();
		delete pFile;
	}
	if (pCon)
	{
		pCon->Close();
		delete pCon;
	}
	if (pSession)
	{
		pSession->Close();
		delete pSession;
	}
	return sAnswer;
}
/////////////////////////////////////////////////////////////////////////////
void CDsIpCamera::CloseStream()
{
	if (m_pStream)
	{
		try
		{
			m_pStream->Close();
			delete m_pStream;
			m_pStream = NULL;
		}
		catch (CInternetException* pEx)
		{
			theApp.ReportError(pEx);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDsIpCamera::CloseHTTPServer()
{
	if (m_pHttpServer)
	{
		try
		{
			m_pHttpServer->Close();
			delete m_pHttpServer;
			m_pHttpServer = NULL;
		}
		catch (CInternetException* pEx)
		{
			theApp.ReportError(pEx);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDsIpCamera::CloseSession()
{
	if (m_pInternetSession)
	{
		m_pInternetSession->Close();
		delete m_pInternetSession;
		m_pInternetSession = NULL;
	}
}
/////////////////////////////////////////////////////////////////////////////
UINT CDsIpCamera::ReadStream(void*pBuff, UINT nCount)
{
	if (m_pStream)
	{
		try
		{
			return m_pStream->Read(pBuff, nCount);
		}
		catch (CInternetException* pEx)
		{
			theApp.ReportError(pEx);
		}
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CDsIpCamera::StartReceiveTimer(BOOL bStart)
{
	DWORD dwTimeout = 0;
	if (bStart)
	{
		if (m_iFPS > 0)
		{
			dwTimeout = MulDiv(1, 1000, m_iFPS) - 1;
			SetThreadTimer(RECEIVE_TIMER, dwTimeout);
		}
		else
		{
			return E_INVALIDARG;
		}
	}
	else
	{
		KillThreadTimer(RECEIVE_TIMER);
	}
//	TRACE(_T("StartReceiveTimer%d(%d, %d): %s\n"), GetCameraSubID(), bStart, dwTimeout, GetTickCountEx());
	return S_OK;
}

////////////////////////////////////////////////////////////////////////////
void CDsIpCamera::ReadJPG()
{
//	TRACE(_T("ReadJPG(%d): %s\n"), GetCameraSubID(), GetTickCountEx());
	CIPCOutputIPcamUnit*pOutput = theApp.GetDlg()->GetOutput(GetCameraSubID());
	if (   pOutput 
#ifndef _DEBUG
		&& pOutput->IsConnected()
#endif
		)
	{
		long	lRead, lPictureSize = 0, lToRead;
		LARGE_INTEGER liStart = theTimer.GetMicroTicks();
		LARGE_INTEGER liStop = {0};
		if (m_lCurrentPosition == -1)
		{
			char	szBuff[128];
			CSecID camID(pOutput->GetGroupID(), GetCameraSubID());
			CSystemTime	TimeStamp;
			TimeStamp.GetLocalTime();

			lRead = ReadStream(szBuff, 127);
			szBuff[lRead] = '\0';
			char *psz = strstr((const char*)szBuff, m_sLengthID);
			if (psz)
			{
				for (psz = &psz[strlen(m_sLengthID)]; *psz != 0; psz++)
				{
					if (isdigit(*psz))
						break;
				}
				lPictureSize = atoi(psz);
				psz = strstr(psz, m_sStartID);
				if (psz)
				{
					psz += m_sStartID.GetLength();
				}
			}
			if (lPictureSize && psz)
			{
				m_pPiture = new	CIPCPictureRecord(pOutput,
					(const unsigned char*)NULL,
					lPictureSize,
					camID,
					m_Resolution,
					m_Compression,
					COMPRESSION_JPEG,
					TimeStamp,
					TRUE,
					NULL,
					0,
					SPT_FULL_PICTURE,
					m_wPackageNr++);
				const BYTE *pData = m_pPiture->GetData();
				long lStart = (long)(psz - szBuff);
				m_lCurrentPosition = lRead - lStart;
				if (pData)
				{
					CopyMemory((void*)pData, psz, m_lCurrentPosition);
				}
			}
		}
		if (   m_pPiture != NULL 
			&& m_lCurrentPosition != -1 
			&& m_lCurrentPosition < (long)m_pPiture->GetDataLength())
		{
			const BYTE *pData = m_pPiture->GetData();
			lPictureSize = m_pPiture->GetDataLength();
			lToRead = lPictureSize - m_lCurrentPosition;
			lRead = ReadStream((void*)&pData[m_lCurrentPosition], lToRead);
			m_lCurrentPosition += lRead;
			if (m_lCurrentPosition == lPictureSize)
			{
				liStop = theTimer.GetMicroTicks();
				DWORD		dwMDx = 0,
							dwMDy = 0;
#ifdef _DEBUG
				if (pOutput->IsConnected())
#endif
				{
					pOutput->DoIndicateVideo(*m_pPiture, dwMDx, dwMDy, GetUserData(), SECID_NO_ID);
				}
				DrawPicture(m_pPiture);
				theApp.GetDlg()->AddPictureStatistic(lPictureSize);
				m_lCurrentPosition = -1;
				WK_DELETE(m_pPiture);
			}
			else
			{
				TRACE(_T("left to read(%d): %s\n"), lPictureSize - m_lCurrentPosition, GetTickCountEx());
			}
		}

		if (liStop.QuadPart)
		{
			DWORD dwSpan = (DWORD)(liStop.QuadPart - liStart.QuadPart);
			if (dwSpan > 8000)
			{
				StartReceiveTimer();// readjust timer
			}
			theApp.GetDlg()->AddSpanStatistic(dwSpan);
		}
	}
	else
	{
		SetMediaState(State_Stopped);
	}
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDsIpCamera, CDsCamera)
	//{{AFX_MSG_MAP(CDsIpCamera)
	ON_THREAD_MESSAGE(WM_TIMER, OnTimer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDsIpCamera message handlers
void CDsIpCamera::OnTimer(WPARAM lEvent, LPARAM)
{
	if (lEvent == RECEIVE_TIMER)
	{
		if (m_CompressionType == COMPRESSION_JPEG)
		{
			ReadJPG();
		}
	}
}
