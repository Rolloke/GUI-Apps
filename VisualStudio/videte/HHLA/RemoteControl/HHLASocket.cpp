// HHLASocket.cpp : implementation file
//

#include "stdafx.h"
#include "wk.h"
#include "RemoteControl.h"
#include "Ini2Reg.h"
#include "HHLASocket.h"
#include "RemoteControlDef.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHHLASocket

// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CHHLASocket, CSocket)
	//{{AFX_MSG_MAP(CHHLASocket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
CHHLASocket::CHHLASocket()
{
	Init();
}

/////////////////////////////////////////////////////////////////////////////
CHHLASocket::CHHLASocket(const CString &sIPAddr, int nPort, DWORD dwConnectTimeOut)
{
	DWORD dwTC	= 0;

	// TRUE -> alle eintreffenden Pakete werden protokolliert
	m_bTraceHexDump = MyGetPrivateProfileInt(_T("Debug"), _T("HexDump"), 0, _T("RemoteControl"));	

	Init();
	m_sIPAddr	= sIPAddr;
	m_nPort		= nPort;
 
	if (Create())
	{
		m_bOK			= TRUE;
		dwTC			= GetTickCount();
		m_bConnected	= FALSE;

		// Versuche 'dwConnectTimeOut' ms einen Connect durchzuführen
		do
		{
			if (Connect(m_sIPAddr, nPort))
			{
				m_bConnected = TRUE;
				break;
			}

		}
		while(GetTickCount() < dwTC + dwConnectTimeOut);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CHHLASocket::Init()
{
	m_sIPAddr			= _T("");
	m_nPort				= 0;
	m_bOK				= FALSE;
	m_bConnected		= FALSE;
	m_bReadingInProzess	= FALSE;
	m_sInputBuffer		= _T("");
}

/////////////////////////////////////////////////////////////////////////////
CHHLASocket::~CHHLASocket()
{
}

/////////////////////////////////////////////////////////////////////////////
BOOL CHHLASocket::IsValid()
{
	return m_bOK;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CHHLASocket::IsConnected()
{
	return m_bConnected;
}

/////////////////////////////////////////////////////////////////////////////
void CHHLASocket::OnReceive(int nErrorCode) 
{
	char*	pMsg	= NULL;
	DWORD	dwLen	= 0;

	// Liegen Daten vor?
	IOCtl(FIONREAD, &dwLen);

	// Nein, dann gleich wieder raus
	if (dwLen == 0)
		return;

	// Buffer für die Daten anlegen
	pMsg = new char[dwLen];
	if (!pMsg)
		return;

	// Daten lesen
	Receive(pMsg, dwLen);

	for (DWORD dwI = 0; dwI < dwLen; dwI++)
	{
		if (m_bTraceHexDump)
			WK_TRACE(_T("%d <%c>, %d\n"),dwI, pMsg[dwI], (int)pMsg[dwI]);
		
		if (m_bReadingInProzess)
		{
			if (pMsg[dwI] == ETX)
			{
				m_bReadingInProzess = FALSE;
				m_Packet.SetPacketMsg(m_sInputBuffer);
				OnReceivePacket(m_Packet);

				m_sInputBuffer = _T("");
 			}
			else
				m_sInputBuffer += pMsg[dwI];

		}
		else
		{
			if (pMsg[dwI] == STX)
			{
				m_sInputBuffer = _T("");
				m_bReadingInProzess = TRUE;
			}
		}
	}
	
	WK_DELETE(pMsg);

	CSocket::OnReceive(nErrorCode);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CHHLASocket::OnReceivePacket(CPacket &Packet)
{
	WORD wI				= 0;
//	WORD wCamMax		= 0;
//	WORD wStatusCode	= 0;
	INDEX_STRUCT		Index;
	CLIENT_STRUCT		Clients;
	CAMERA_SET_STRUCT	CameraSet;
	CAMERA_PARAM_STRUCT	CameraParam;

	// Strukturen löschen
	memset (&Clients,		0 , sizeof(Clients));
	memset (&CameraSet,		0 , sizeof(CameraSet));
	memset (&CameraParam,	0 , sizeof(CameraParam));

	// Quittungstelegramm?
	if (Packet.GetPacketID() == _T('Q'))
	{
		// ONLINE bestätigung
		if (Packet.GetPacketType() == TYP00)
		{
			if (Packet.GetType00StatusCode() == STATUS_CODE00)
				OnConfirmOnline();
		}
		return TRUE;
	}
	
	// Datentelegramme werden immer quittiert.
	if (Packet.GetPacketID() == _T('D'))
	{
		DoConfirmPacket();
	}

	switch(Packet.GetPacketType())
	{
		case TYP00:
			OnReceivePacketType00(Packet.GetType00StatusCode(), Packet.GetType00TimeOut(), Packet.GetType00WorkerPort());
			break;

		case TYP01:
			Clients.wMaxCameras			= Packet.GetType01MaxCam();
			for (wI = 1; wI <= Clients.wMaxCameras; wI++)
			{
				Clients.wCamera			= wI;
				Clients.bCameraOnline	= Packet.GetType01CamOnOff(wI);
				Clients.rcWndPos		= Packet.GetType01WndPos(wI);
				OnReceivePacketType01(Clients);
			}
			break;

		case TYP02:
			Index.sContainerPrefix	= Packet.GetType02ContainerPrefix();
			Index.sContainerNumber	= Packet.GetType02ContainerNumber();
			Index.sISOCode			= Packet.GetType02ISOCode();
			Index.sIMDGCode1		= Packet.GetType02IMDGCode1();
			Index.sIMDGCode2		= Packet.GetType02IMDGCode2();
			Index.sSpecialID		= Packet.GetType02SpecialID();
			Index.sRufzeichen		= Packet.GetType02Rufzeichen();
			Index.sReiseNummer		= Packet.GetType02ReiseNummer();
			Index.sReeder			= Packet.GetType02Reeder();
			Index.sSiegelNummer		= Packet.GetType02SiegelNummer();
			Index.sRichtung			= Packet.GetType02Direction();
			Index.sDamageCode		= Packet.GetType02DamageCode();
			Index.sDate				= Packet.GetType02Date();
			Index.sTime				= Packet.GetType02Time();
			Index.sFileName			= Packet.GetType02FileName();
#if (0)
			Index.sContainerPrefix	= _T("1111");
			Index.sContainerNumber	= _T("2222222222");
			Index.sISOCode			= _T("3333");
			Index.sIMDGCode1		= _T("444");
			Index.sIMDGCode2		= _T("555");
			Index.sSpecialID		= _T("66");
			Index.sRufzeichen		= _T("77777777");
			Index.sReiseNummer		= _T("88888888");
			Index.sReeder			= _T("99999999999999999999");
			Index.sSiegelNummer		= _T("00000000");
			Index.sRichtung			= _T("A");
			Index.sDamageCode		= _T("BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB");
			Index.sDate				= _T("11071999");
			Index.sTime				= _T("175255");
			Index.sFileName			= _T("C:\\Temp\\Martin") + CString(_T(' '), 128);
			Index.sFileName			= Index.sFileName.Left(128);
#endif
			OnReceivePacketType02(Index);
			break;

		case TYP03:
			for (wI = 1; wI <= 5; wI++)
			{
				CameraSet.wCamera	= Packet.GetType03Camera(wI);
				CameraSet.nAction	= Packet.GetType03Action(wI);
				CameraSet.bMiniMize = Packet.GetType03Minimized(wI);
				OnReceivePacketType03(CameraSet);
			}
			break;

		case TYP04:
			if (Packet.GetPacketMsg().GetLength() == 24)	// Nur Header
			{
				CameraParam.nRequestTyp = REQ_ALL_PARAM;
			}
			if (Packet.GetPacketMsg().GetLength() == 28)	// Header mit Kameranummer
			{
				CameraParam.nRequestTyp = REQ_SINGLE_PARAM;
				CameraParam.wCamera		= Packet.GetType04Camera();
			}
			if (Packet.GetPacketMsg().GetLength() == 36)	// Header mit Kameranummer und Parametern
			{
				CameraParam.nRequestTyp = SET_SINGLE_PARAM;
				CameraParam.wCamera		= Packet.GetType04Camera();
				CameraParam.wBrightness	= Packet.GetType04Brightness();
				CameraParam.wContrast	= Packet.GetType04Contrast();
				CameraParam.wSaturation	= Packet.GetType04Saturation();
				CameraParam.bIsZoomed	= Packet.GetType04IsZoomed();
			}

			OnReceivePacketType04(CameraParam);
			break;		

		case TYP05:
			if (Packet.GetPacketMsg().GetLength() == 24)	// Nur Header
				OnReceivePacketType05(CHECK_ALL_CAMERAS, 0);
			else
				OnReceivePacketType05(CHECK_CAMERA, Packet.GetType05Camera());
			break;

		default:
			WK_TRACE_ERROR(_T("Unknown Telegramm Type\n"));
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CHHLASocket::DoRequestSendPacket(const CStringA &sPacket)
{
	char	*pMsg = NULL;
	DWORD	dwLen = sPacket.GetLength();

	if (dwLen == 0)
		return FALSE;

	pMsg = new char[dwLen + 2]; // Two extra Bytes for STX and ETX
	if (!pMsg)
		return FALSE;

	pMsg[0] = STX;
	for (DWORD dwI = 0; dwI < dwLen; dwI++)
		pMsg[dwI+1] = sPacket.GetAt(dwI);
	pMsg[dwLen+1] = ETX;

	SendData(pMsg, dwLen + 2);

	WK_DELETE(pMsg);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CHHLASocket::DoConfirmPacket()
{
	CStringA sMsg = m_Packet.GetPacketMsg();
	if (sMsg.GetLength() < 8)
		return FALSE;

	// 'D' durch 'Q' ersetzen
	sMsg.SetAt(7, _T('Q'));
	DoRequestSendPacket(sMsg);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CHHLASocket::SendData(char *pMsg, DWORD dwLen)
{
	if (!pMsg)
		return FALSE;

	if (Send((void*)pMsg, dwLen) == SOCKET_ERROR)
		return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CHHLASocket::OnReceivePacketType00(WORD wStatusCode, WORD wTimeOut, int nWorkerPort)
{
	WK_TRACE_WARNING(_T("OnReceivePacketType00 not overridden\n"));
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CHHLASocket::OnReceivePacketType01(CLIENT_STRUCT Clients)
{
	WK_TRACE_WARNING(_T("OnReceivePacketType01 not overridden\n"));
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CHHLASocket::OnReceivePacketType02(INDEX_STRUCT Index)
{
	WK_TRACE_WARNING(_T("OnReceivePacketType02 not overridden\n"));
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CHHLASocket::OnReceivePacketType03(CAMERA_SET_STRUCT CameraSet)
{
	WK_TRACE_WARNING(_T("OnReceivePacketType03 not overridden\n"));
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CHHLASocket::OnReceivePacketType04(CAMERA_PARAM_STRUCT CameraParam)
{
	WK_TRACE_WARNING(_T("OnReceivePacketType04 not overridden\n"));
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CHHLASocket::OnReceivePacketType05(WORD wStatusCode, WORD wCamera)
{
	WK_TRACE_WARNING(_T("OnReceivePacketType05 not overridden\n"));
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CHHLASocket::OnConfirmOnline()
{
	WK_TRACE_WARNING(_T("OnReceiveConfirm not overridden\n"));
	return TRUE;
}

