// WorkSocket.cpp: implementation of the CWorkSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RemoteControlDef.h"
#include "RemoteControl.h"
#include "RemoteControlDlg.h"
#include "ClientControl.h"
#include "WorkSocket.h"
#include "..\MiniClient\MiniClientDef.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
CWorkSocket::CWorkSocket(CRemoteControlDlg *pWnd, const CString &sAppID, const CString &sIPAddr, int nPort, DWORD dwConnectTimeOut) : 
				CHHLASocket(sIPAddr, nPort, dwConnectTimeOut)
{
	m_nMaxClients = GetPrivateProfileInt(_T("Private"),	_T("MaxCameras"),MAX_CLIENTS, INIFILENAME);
	m_nMaxClients = min(m_nMaxClients, MAX_CLIENTS);

	m_bWaitForEchoRequest	= TRUE;
	m_pWnd					= pWnd;
	m_sAppID				= (sAppID + _T("        ")).Left(8);
	m_nCamSetNr				= 0;
	memset(&m_wCameraSet[0], 0, sizeof(m_wCameraSet));			
}

/////////////////////////////////////////////////////////////////////////////
CWorkSocket::~CWorkSocket()
{
	// Alle Clients minimieren
	MinimizeAllClients();
}

/////////////////////////////////////////////////////////////////////////////
void CWorkSocket::OnClose(int nErrorCode) 
{
	if (m_pWnd && IsWindow(m_pWnd->m_hWnd))
		m_pWnd->PostMessage(WM_SOCKET_CLOSED, 0, 0);

	CSocket::OnClose(nErrorCode);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CWorkSocket::OnReceivePacketType00(WORD wStatusCode, WORD wTimeOut, int nWorkerPort)
{
	WK_TRACE(_T("--------------------------->\n"));
	WK_TRACE(_T("WorkSocket: PacketType:00\n"));
	WK_TRACE(_T("\tStatusCode.......:%u\n"),  wStatusCode);
	WK_TRACE(_T("\tTimeOut..........:%u\n"),  wTimeOut);
	WK_TRACE(_T("\tWorkerPort.......:%d\n"),  nWorkerPort);
	WK_TRACE(_T("<---------------------------\n"));
	
	// Verbindung physikalisch offline, Socket schließen
	if (wStatusCode == STATUS_CODE99)
	{
		if (m_pWnd && IsWindow(m_pWnd->m_hWnd))
			m_pWnd->PostMessage(WM_SOCKET_CLOSED, 0, 0);
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CWorkSocket::OnReceivePacketType01(CLIENT_STRUCT Clients)
{
	WK_TRACE(_T("--------------------------->\n"));
	WK_TRACE(_T("WorkSocket: PacketType:01\n"));
	WK_TRACE(_T("\tMaxCameras.......:%u\n"),  Clients.wMaxCameras);
	WK_TRACE(_T("\tKamera...........:%u\n"),  Clients.wCamera);
	WK_TRACE(_T("\tKameraOnline.....:%u\n"),  Clients.bCameraOnline);
	WK_TRACE(_T("\tTop..............:%u\n"),  Clients.rcWndPos.top);
	WK_TRACE(_T("\tLeft.............:%u\n"),  Clients.rcWndPos.left);
	WK_TRACE(_T("\tWidth............:%u\n"),  Clients.rcWndPos.Width());
	WK_TRACE(_T("\tHeight...........:%u\n"),  Clients.rcWndPos.Height());
	WK_TRACE(_T("<---------------------------\n"));

	if (!m_pWnd)
		return FALSE;

	if (Clients.wCamera > m_nMaxClients)
	{
		WK_TRACE_WARNING(_T("Kameranummer zu hoch (%d)\n"), Clients.wCamera);
		return FALSE;
	}

	CClientControl * pClient = m_pWnd->GetClientControl(Clients.wCamera);

	if (!pClient)
		return FALSE;

	if (Clients.bCameraOnline)
	{
		pClient->DoRequestSetWindowPosition(Clients.rcWndPos.left,
											Clients.rcWndPos.top,
											Clients.rcWndPos.Width(),
											Clients.rcWndPos.Height());
	}
	else
	{
		pClient->DoRequestSetWindowPosition(-1, -1, -1, -1);
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CWorkSocket::OnReceivePacketType02(INDEX_STRUCT Index)
{
	BOOL bRet = FALSE;

	WK_TRACE(_T("--------------------------->\n"));
	WK_TRACE(_T("WorkSocket: PacketType:02\n"));
	WK_TRACE(_T("\tContainerPrefix..:<%s>\n"),  CString(Index.sContainerPrefix));
	WK_TRACE(_T("\tContainerNumber..:<%s>\n"),  CString(Index.sContainerNumber));
	WK_TRACE(_T("\tISOCode..........:<%s>\n"),  CString(Index.sISOCode));
	WK_TRACE(_T("\tIMDGCode1........:<%s>\n"),  CString(Index.sIMDGCode1));
	WK_TRACE(_T("\tIMDGCode2........:<%s>\n"),  CString(Index.sIMDGCode2));
	WK_TRACE(_T("\tSpezialkennung...:<%s>\n"),  CString(Index.sSpecialID));
	WK_TRACE(_T("\tRufzeichen.......:<%s>\n"),  CString(Index.sRufzeichen));
	WK_TRACE(_T("\tReisenummer......:<%s>\n"),  CString(Index.sReiseNummer));
	WK_TRACE(_T("\tReeder...........:<%s>\n"),  CString(Index.sReeder));
	WK_TRACE(_T("\tSiegelnummer.....:<%s>\n"),  CString(Index.sSiegelNummer));
	WK_TRACE(_T("\tRichtung.........:<%s>\n"),  CString(Index.sRichtung));
	WK_TRACE(_T("\tBeschädigungscode:<%s>\n"),  CString(Index.sDamageCode));
	WK_TRACE(_T("\tDatum............:<%s>\n"),  CString(Index.sDate));
	WK_TRACE(_T("\tUhrzeit..........:<%s>\n"),  CString(Index.sTime));
	WK_TRACE(_T("\tFileName.........:<%s>\n"),  CString(Index.sFileName));
	WK_TRACE(_T("<---------------------------\n"));

	if (!m_pWnd)
		return FALSE;

	TRY
	{
		// Index-Datei erstellen
		CString sFileName = Index.sFileName;

		sFileName.TrimRight();
		sFileName.Format(_T("%s.IDX"), sFileName);
		CFile File(sFileName,	CFile::modeCreate | CFile::modeWrite  |
								CFile::typeBinary | CFile::shareDenyWrite);

		File.Write(Index.sContainerPrefix, Index.sContainerPrefix.GetLength()); 
		File.Write(Index.sContainerNumber, Index.sContainerNumber.GetLength()); 
		File.Write(Index.sISOCode, Index.sISOCode.GetLength()); 
		File.Write(Index.sIMDGCode1, Index.sIMDGCode1.GetLength()); 
		File.Write(Index.sIMDGCode2, Index.sIMDGCode2.GetLength()); 
		File.Write(Index.sSpecialID, Index.sSpecialID.GetLength()); 
		File.Write(Index.sRufzeichen, Index.sRufzeichen.GetLength()); 
		File.Write(Index.sReiseNummer, Index.sReiseNummer.GetLength()); 
		File.Write(Index.sReeder, Index.sReeder.GetLength()); 
		File.Write(Index.sSiegelNummer, Index.sSiegelNummer.GetLength()); 
		File.Write(Index.sRichtung, Index.sRichtung.GetLength()); 
		File.Write(Index.sDamageCode, Index.sDamageCode.GetLength()); 
		File.Write(Index.sDate, Index.sDate.GetLength()); 
		File.Write(Index.sTime, Index.sTime.GetLength()); 

		// Jedem Client des aktuellen Kamerasets die Speicherungs-aufforderung senden.
		for (WORD wCam = 0; wCam <= 5; wCam++)
		{
			if (m_wCameraSet[wCam] != 0)
			{
				CClientControl * pClient = m_pWnd->GetClientControl(m_wCameraSet[wCam]);

				if (pClient)
				{
					sFileName = Index.sFileName;
					sFileName.TrimRight();
					sFileName.Format(_T("%s.K%02u"), sFileName, m_wCameraSet[wCam]);

					pClient->DoRequestSavePicture(sFileName);
					
					sFileName += CStringA(_T(' '), 128);
					sFileName = sFileName.Left(128);
					File.Write(sFileName, sFileName.GetLength());
				}
			}
		}

		bRet = TRUE;
	}
	CATCH( CFileException, e )
	{
		WK_TRACE(_T("CFileException can't store Indexfile\n"));
		bRet = FALSE;
	}
	END_CATCH

	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CWorkSocket::OnReceivePacketType03(CAMERA_SET_STRUCT CameraSet)
{
	WK_TRACE(_T("--------------------------->\n"));
	WK_TRACE(_T("WorkSocket: PacketType:03\n"));
	WK_TRACE(_T("\tKamera...........:%u\n"),  CameraSet.wCamera);
	if (CameraSet.nAction == FREEZE_IT)
		WK_TRACE(_T("\tRefreshType......:Freeze\n"));
 	if (CameraSet.nAction == SHOW_IT)
		WK_TRACE(_T("\tRefreshType......:Anzeigen\n"));
	if (CameraSet.nAction == REFRESH_IT)
		WK_TRACE(_T("\tRefreshType......:Refresh\n"));
	if (CameraSet.bMiniMize == TRUE)
		WK_TRACE(_T("\tDarstellung.....:Minimiert\n"));
	else
		WK_TRACE(_T("\tDarstellung.....:Default\n"));
	WK_TRACE(_T("<---------------------------\n"));

	if (CameraSet.wCamera > m_nMaxClients)
	{
		WK_TRACE_WARNING(_T("Kameranummer zu hoch (%d)\n"), CameraSet.wCamera);
		return FALSE;
	}

	if (!m_pWnd)
		return FALSE;

	CClientControl * pClient = m_pWnd->GetClientControl(CameraSet.wCamera);

	if (!pClient)
		return FALSE;

	pClient->DoRequestSetInputSource(CameraSet.bMiniMize, CameraSet.nAction);

	// Merke die Kameras, die zum ausgewählten Kameraset gehören.
	m_nCamSetNr++;
	if (m_nCamSetNr >= 5)
		m_nCamSetNr = 0;

	if (!CameraSet.bMiniMize)
		m_wCameraSet[m_nCamSetNr] = CameraSet.wCamera;
	else
		m_wCameraSet[m_nCamSetNr] = 0;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CWorkSocket::OnReceivePacketType04(CAMERA_PARAM_STRUCT CameraParam)
{
	WK_TRACE(_T("--------------------------->\n"));
	WK_TRACE(_T("WorkSocket: PacketType:04\n"));
	WK_TRACE(_T("\tKamera...........:%u\n"),  CameraParam.wCamera);
	WK_TRACE(_T("\tBrightness.....:.:%u\n"),  CameraParam.wBrightness);
	WK_TRACE(_T("\tContrast.........:%u\n"),  CameraParam.wContrast);
	WK_TRACE(_T("\tSaturation.......:%u\n"),  CameraParam.wSaturation);
	WK_TRACE(_T("\tIsZoomed.........:%u\n"),  CameraParam.bIsZoomed);
	WK_TRACE(_T("\tRequestTyp.......:%d\n"),  CameraParam.nRequestTyp);
	WK_TRACE(_T("<---------------------------\n"));

	CClientControl * pClient = NULL;
	WORD	wI = 0;
	
	if (!m_pWnd)
		return FALSE;

	switch (CameraParam.nRequestTyp)
	{
		case REQ_ALL_PARAM:
			for (wI = 1; wI <= m_nMaxClients; wI++)
			{
				pClient = m_pWnd->GetClientControl(wI);
				if (!pClient)
					return FALSE;
				pClient->DoRequestCameraParameter();
			}
			break;
		case REQ_SINGLE_PARAM:
			if (CameraParam.wCamera > m_nMaxClients)
			{
				WK_TRACE_WARNING(_T("Kameranummer zu hoch (%d)\n"), CameraParam.wCamera);
				return FALSE;
			}
			pClient = m_pWnd->GetClientControl(CameraParam.wCamera);
			if (!pClient)
				return FALSE;
			pClient->DoRequestCameraParameter();
			break;
		case SET_SINGLE_PARAM:
			if (CameraParam.wCamera > m_nMaxClients)
			{
				WK_TRACE_WARNING(_T("Kameranummer zu hoch (%d)\n"), CameraParam.wCamera);
				return FALSE;
			}
			pClient = m_pWnd->GetClientControl(CameraParam.wCamera);
			if (!pClient)
				return FALSE;
			pClient->DoRequestSetCameraParameter(CameraParam.wBrightness, CameraParam.wContrast, CameraParam.wSaturation);
			break;
		default:
			WK_TRACE_ERROR(_T("Unknown RequestType for Cameraparameter\n"));
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CWorkSocket::OnReceivePacketType05(WORD wStatusCode, WORD wCamera)
{
	CClientControl * pClient = NULL;

	WK_TRACE(_T("--------------------------->\n"));
	WK_TRACE(_T("WorkSocket: PacketType:05\n"));
	WK_TRACE(_T("\tKamera...........:%u\n"),  wCamera);
	WK_TRACE(_T("\tStatuscode.......:%u\n"),  wStatusCode);
	WK_TRACE(_T("<---------------------------\n"));

	if (!m_pWnd)
		return FALSE;

	if (wStatusCode == CHECK_CAMERA)
	{
		// Eine Kamera prüfen
 		pClient = m_pWnd->GetClientControl(wCamera);
		if (pClient)
			pClient->DoRequestCheckCamera();
	}
	else if (wStatusCode == CHECK_ALL_CAMERAS)
	{
 		// Alle Kameras prüfen
		for (WORD wI = 1; wI <= m_nMaxClients; wI++)
		{		
 			pClient = m_pWnd->GetClientControl(wI);
			if (pClient)
				pClient->DoRequestCheckCamera();
		}
	}
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CWorkSocket::OnConfirmOnline()
{
	WK_TRACE(_T(">>>>ONLINE<<<<\n"));
	m_bWaitForEchoRequest = TRUE;
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CWorkSocket::DoConfirmOnline()
{
	CStringA sMsg = _T("");

	// Wurde der letzte Request bestätigt?
	if (!m_bWaitForEchoRequest)
	{
		return FALSE;
	}

	m_bWaitForEchoRequest = FALSE;

  	sMsg += _T("  ");			// Checksum				
	sMsg += _T("38   ");		// PacketLength			
	sMsg += _T("D");			// PacketID				
	sMsg += _T("00");			// Packet Counter		
	sMsg += m_sAppID;			// ApplicationID		
	sMsg += _T("00");			// PacketType 00		
	sMsg += _T("\r\n");			// CRLF
	sMsg += _T("00");			// StatusCode			 
	sMsg += _T(" ");			// Reserved
	sMsg += _T("  ");			// Max.Errors			
	sMsg += _T(" ");			// Reserved
	sMsg += _T("  ");			// Timeout				
	sMsg += _T(" ");			// Reserved
	sMsg += _T("     ");		// Workerport			
	sMsg += _T("\r\n");			// CRLF
	
	return DoRequestSendPacket(sMsg);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CWorkSocket::DoConfirmGetCameraParameter(WORD wCamera, int nBrightness, int nContrast, int nSaturation)
{
	CStringA sMsg = _T("");
	CStringA sCam;
	CStringA sMaxCam;
	CStringA sBrightness;
	CStringA sContrast;
	CStringA sSaturation;

	WK_TRACE(_T("Requested Parameter from Camera %u Brightness=%d Contrast=%d Saturation=%d\n"),
							wCamera, nBrightness, nContrast, nSaturation);

	sCam.Format("%02u", wCamera); 
	sMaxCam.Format("%02u", MAX_CLIENTS); 
	sBrightness.Format("%02d",  nBrightness); 
	sContrast.Format("%02d",    nContrast); 
	sSaturation.Format("%02d",  nSaturation); 
	
  	sMsg += _T("  ");			// Checksum				
	sMsg += _T("35   ");		// PacketLength			
	sMsg += _T("K");			// PacketID				
	sMsg += _T("00");			// Packet Counter		
	sMsg += m_sAppID;		// ApplicationID		
	sMsg += _T("04");			// PacketType 00		
	sMsg += _T("\r\n");			// CRLF
	sMsg += sMaxCam;		// Max Cameras
	sMsg += sCam;			// Kamera
	sMsg += sBrightness;	// Helligkeit
	sMsg += sContrast;		// Kontrast
	sMsg += sSaturation;	// Farbsättigung
	sMsg += _T("D");			// ToDo D und Z
	sMsg += _T("\r\n");			// CRLF
	
	return DoRequestSendPacket(sMsg);
}
							 
/////////////////////////////////////////////////////////////////////////////
BOOL CWorkSocket::CameraMissing(WORD wCamera, BOOL bFlag)
{
	CStringA sMsg = _T("");
	CStringA sCam = _T("");
	CStringA sFlag= _T("");

	// Kameranummer als String
	sCam.Format("%02u", wCamera);
  	
	sMsg += _T("  ");			// Checksum				
	sMsg += _T("28   ");		// PacketLength			
	sMsg += _T("K");			// PacketID				
	sMsg += _T("00");			// Packet Counter		
	sMsg += m_sAppID;		// ApplicationID		
	sMsg += _T("05");			// PacketType 00		
	sMsg += _T("\r\n");			// CRLF

	if (bFlag)
		sFlag.Format("%02u", CAMERA_DEFECT);
	else		    
		sFlag.Format("%02u", CAMERA_OK);
	
	sMsg += sFlag;
	sMsg += sCam;
	sMsg += _T("\r\n");			// CRLF

	return DoRequestSendPacket(sMsg);
}

/////////////////////////////////////////////////////////////////////////////
void CWorkSocket::MinimizeAllClients()
{
	// Alle Clients minimieren.
	for (WORD wI = 1; wI <= m_nMaxClients; wI++)
	{
		CClientControl * pClient = m_pWnd->GetClientControl(wI);

		if (pClient)
			pClient->DoRequestSetWindowPosition(-1, -1, -1, -1);
	}
}