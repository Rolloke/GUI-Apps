// CHHLA.cpp: implementation of the CHHLA class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cipc.h"
#include "RemoteControl.h"
#include "CHHLA.h"
#include "RemoteControlDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
CHHLA::CHHLA(CRemoteControlDlg *pWnd)
{
	m_nWorkPort		= 0;
	m_pCSocket		= NULL;
	m_pWSocket		= NULL;
	m_pWnd			= pWnd;

	m_TimeOut.dwConnectConnectPort	= GetPrivateProfileInt(_T("TIMEOUTS"),		_T("CONNECT_CONNECT_PORT"),	5, INIFILENAME)*1000;
	m_TimeOut.dwConnectWorkPort		= GetPrivateProfileInt(_T("TIMEOUTS"),		_T("CONNECT_WORK_PORT"),	5, INIFILENAME)*1000;
	m_TimeOut.dwGetWorkPort			= GetPrivateProfileInt(_T("TIMEOUTS"),		_T("GET_WORK_PORT"),		5, INIFILENAME)*1000;
	m_TimeOut.dwNoTraffic			= GetPrivateProfileInt(_T("TIMEOUTS"),		_T("NO_TRAFFIC"),			5, INIFILENAME)*1000;
	m_TimeOut.dwNoResponseOnReq		= GetPrivateProfileInt(_T("TIMEOUTS"),		_T("NO_RESPONS_ON_REQUEST"),5, INIFILENAME)*1000;
}

/////////////////////////////////////////////////////////////////////////////
CHHLA::~CHHLA()
{
	WK_DELETE(m_pCSocket);
	WK_DELETE(m_pWSocket);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CHHLA::DoConnection(const CString &sIPAddr, int nConnectionPort, const CString& sAppID)
{
	CString sStatus;

	if (!m_pWnd)
		return FALSE;

	// ConnectSocket anlegen
	WK_TRACE(_T("Try to connect to %s ConnectPort:%u...\n"), sIPAddr, nConnectionPort);
	sStatus.Format(_T("Try to connect to %s ConnectPort:%u...\n"), sIPAddr, nConnectionPort);
	m_pWnd->SetStatusText(sStatus);
	m_pCSocket = new CConnectSocket(m_pWnd, sIPAddr, nConnectionPort, m_TimeOut.dwConnectConnectPort);
	
	if (!m_pCSocket)
		return FALSE;

	if (!m_pCSocket->IsConnected())
	{
		WK_TRACE(_T("\tConnection to %s ConnectPort:%u failed\n"), sIPAddr, nConnectionPort);
		sStatus.Format(_T("Connection to %s ConnectPort:%u failed\n"), sIPAddr, nConnectionPort);
		m_pWnd->SetStatusText(sStatus);

		// ConnectSocket schließen
		WK_DELETE(m_pCSocket);
		
		return FALSE;
	}

	WK_TRACE(_T("\tConnection to %s ConnectPort:%d successfull\n"), sIPAddr, nConnectionPort);
	sStatus.Format(_T("Connection to %s ConnectPort:%d successfull\n"), sIPAddr, nConnectionPort);
	m_pWnd->SetStatusText(sStatus);

//#define TEST // OOPS Test
#ifdef TEST
	// Server zum verschicken des Workerports anregen
	m_pCSocket->DoRequestSendPacket(_T("SendPT00"));
#endif
	// Warte bis Workerport verfügbar ist (Maximal GET_WORK_PORT Sekunden)
	WK_TRACE(_T("Waiting to get WorkPort...\n"));
	sStatus.Format(_T("Waiting to get WorkPort...\n"));
	m_pWnd->SetStatusText(sStatus);
	m_nWorkPort = m_pCSocket->WaitForWorkport(m_TimeOut.dwGetWorkPort);

	// ConnectSocket schließen
	WK_DELETE(m_pCSocket);	
	Sleep(250);

	if (!m_nWorkPort)
	{
		WK_TRACE(_T("\tWaiting to get WorkPort timeout\n"));
		sStatus.Format(_T("Waiting to get WorkPort timeout\n"));
		m_pWnd->SetStatusText(sStatus);
		return FALSE;
	}

	// WorkSocket öffnen.
	WK_TRACE(_T("Try to connect to %s WorkPort:%u...\n"), sIPAddr, m_nWorkPort);
	sStatus.Format(_T("Try to connect to %s WorkPort:%u...\n"), sIPAddr, m_nWorkPort);
	m_pWnd->SetStatusText(sStatus);

	m_pWSocket = new CWorkSocket(m_pWnd, sAppID, sIPAddr, m_nWorkPort, m_TimeOut.dwConnectWorkPort);
	if (!m_pWSocket)
		return FALSE;

	if (!m_pWSocket->IsConnected())
	{
		WK_TRACE(_T("\tConnection to %s WorkPort:%u failed\n"), sIPAddr, m_nWorkPort);
		sStatus.Format(_T("Connection to %s WorkPort:%u failed\n"), sIPAddr, m_nWorkPort);
		m_pWnd->SetStatusText(sStatus);

		// WorkSocket schließen
		WK_DELETE(m_pWSocket);

		return FALSE;
	}

	// ONLINE-Telegramm senden
	m_pWSocket->DoConfirmOnline();

	WK_TRACE(_T("\tConnection to %s WorkPort:%d successfull\n"), sIPAddr, m_nWorkPort);
	sStatus.Format(_T("Connection to %s WorkPort:%d successfull\n"), sIPAddr, m_nWorkPort);

	m_pWnd->SetStatusText(sStatus, TRUE);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CHHLA::DoConfirmOnline()
{
	if (!m_pWSocket)
		return FALSE;

	return m_pWSocket->DoConfirmOnline();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CHHLA::CameraMissing(WORD wSource, BOOL bFlag)
{
	if (!m_pWSocket)
		return FALSE;

	return m_pWSocket->CameraMissing(wSource, bFlag);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CHHLA::DoConfirmGetCameraParameter(WORD wCamera, int nBrightness, int nContrast, int nSaturation)
{
	if (!m_pWSocket)
		return FALSE;

	return m_pWSocket->DoConfirmGetCameraParameter(wCamera, nBrightness, nContrast, nSaturation);
}