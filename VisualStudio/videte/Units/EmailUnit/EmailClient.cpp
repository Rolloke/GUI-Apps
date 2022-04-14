// EmailClient.cpp: implementation of the CEmailClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EmailUnit.h"
#include "EmailClient.h"
#include "SMTP.h"
#include "Profiler.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

fpRasGetEntryDialParams MyRasGetEntryDialParams;
fpRasDial MyRasDial;
fpRasGetConnectStatus MyRasGetConnectStatus;
fpRasHangUp MyRasHangUp;
fpRasGetEntryProperties MyRasGetEntryProperties;
fpRasEnumConnections MyRasEnumConnections;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEmailClient::CEmailClient(CEmailUnitDlg* pdlg)
{
	
	m_pDlg                  = pdlg;
	m_pEmail                = NULL;
	m_pSMTP                 = NULL;
	m_pPOP					= new CPOP(this);
	m_pDialUpNetwork		= NULL;
	m_iTimerElapse          = (1000*60)*1;
	m_bTimerIsSet           = FALSE;
	m_pSMTP                 = new CSMTP(this,&m_lEmail);
	m_bConnectionIsLAN      = TRUE;
	m_bConnectedToMailServer= FALSE;
	m_bConnectedToNetwork   = FALSE;
	m_bAuthenticated		= FALSE;
	m_bConnectedToPOPServer = FALSE;

}

CEmailClient::~CEmailClient()
{
	DeleteEmailList();
	if(m_pDialUpNetwork)
	{
		WK_DELETE(m_pDialUpNetwork);
	}
	WK_DELETE(m_pSMTP);
	WK_DELETE(m_pPOP);
}

///////////////////////////////////////////////////////////////////////////////////////////
//Funktion startet den Sendevorgang 

void CEmailClient::SendMail()
{
	CProfiler profiler;
	BOOL bAuthenticateByPOP; // , bConStatus=FALSE;
	
	
	bAuthenticateByPOP=profiler.GetPOPAuth();
	
	WK_TRACE(_T("[CEmailClient::SendMail]\n"));
	WK_TRACE(_T("start sending email procedure\n"));
	if(m_lEmail.IsEmpty())	//Gar nichts zum versenden vorhanden
	{
		TRACE(_T("Keine Email in Liste...\n"));
		return;
	}
	else
	{
		
		if(!m_bTimerIsSet)		//Timer nicht aktiv
		{
			if(!m_bConnectedToNetwork) // Netzwerkverbindung besteht nicht
			{
				WK_TRACE(_T("[CEmailClient::SendMail()]   ConnectToNetwork()\n"));
				
				m_bConnectedToNetwork=ConnectToNetwork();
			}
			else
			{
				//m_bConnectedToNetwork=TRUE;
			}
			//////////////////////
			
			if(!m_bConnectionIsLAN)
				//Weiter geht es bei Dialog::OnDialUp
			{
				
				if(!m_bConnectedToNetwork)
					return;
				
			}
			
			
			if(m_bConnectedToNetwork && !m_bConnectedToMailServer)
			{
				
				
				if(bAuthenticateByPOP && !m_bAuthenticated)
				{
					
					WK_TRACE(_T("\n----- POP3 Authentication ------------------------------------------\n"));
					
					WK_TRACE(_T("[CEmailClient::SendMail()]   ConnectToPOPServer()\n"));

					m_bConnectedToPOPServer = ConnectToPOPServer();
					if(!m_bConnectedToPOPServer)
					{
						WK_TRACE(_T("[CEmailClient::SendMail()] NoPOPConnection ParkEmail()\n"));
						TRACE(_T("1 ParkEmail\n"));
						ParkEmail();
						return;
					}
					return;
				}
				
				else
				{
					
					WK_TRACE(_T("\n----- SMTP Transfer -----------------------------------------------\n"));
					WK_TRACE(_T("[CEmailClient::SendMail()]   ConnectToSMTPServer()\n"));
		
					m_bConnectedToMailServer = ConnectToSMTPServer();
				}
			}
			//////////////////////			
			if(!(m_bConnectedToMailServer && m_bConnectedToNetwork))
			{
				WK_TRACE(_T("[CEmailClient::SendMail()]   ParkEmail()\n"));	 
				TRACE(_T("2 ParkEmail\n"));
				ParkEmail();
			}
			else
			{
				m_pDlg->KillTimer(NETWORK_TIMER); //Network Timeout löschen
			}
			
		}
		else
		{
			WK_TRACE(_T("[CEmailClient::SendMail()] m_bTimerIsSet=TRUE\n"));
		}
		
		
	}
	
}



/////////////////////////////////////////////////////////////////////////////////////

void CEmailClient::NextStep (UINT MessageType)
{
	if(MessageType==MAIL_SEND)
	{
		m_pSMTP->CloseTransaction();
		
	}
	
	else if(MessageType==READY)
	{
		DisconnectMailServer();
		Sleep(500);
		if(!m_bConnectionIsLAN)
		{
			m_pDlg->SetTimer(HANG_UP_TIMER,HANG_UP_TIME*1000,NULL);
		}
		else
		{
				if(m_lEmail.IsEmpty())
				{
					m_pDlg->ChangeIcon(GREY);
				}
				else
				{
					m_pDlg->ChangeIcon(BLUE);
				}
	
		}
		//DisconnectNetwork(); wird über Timer aufgerufen
		//m_bConnectedToNetwork = FALSE;
	}
	
	else if(MessageType==NEXT_MAIL)	//Nächster Aufruf zum Mailversand
	{
		m_pSMTP->OnServerReply("250");
		
	}
	else if(MessageType==SMTP_ERROR)
	{
		DisconnectMailServer();
		DisconnectNetwork();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////

CEmail* CEmailClient::CreateMail(BOOL Mime)
{
	m_pEmail=new CEmail(Mime);
	m_lEmail.AddTail(m_pEmail);
	m_pEmail->AddRef();

	return m_pEmail;	
}

///////////////////////////////////////////////////////////////////////////////////////////

void CEmailClient::ParkEmail(CEmail* pEmail)
{
	m_pDlg->KillTimer(NETWORK_TIMER);
	m_pDlg->ChangeIcon(BLUE);
	
	if(!m_bTimerIsSet)	//Timer nicht gesetzt
	{
		if(pEmail == NULL)	//Parameter = 0 : Alle geparkten Emails: Sendeversuche um 1 erhöhen
		{
			POSITION listElementPos, listPosSave;
			CEmail* plistElement;
			
			listElementPos = m_lEmail.GetHeadPosition();
			listPosSave = listElementPos;
			
			while(listElementPos != NULL)
			{
				plistElement = m_lEmail.GetNext(listElementPos);
				plistElement->IncreaseAttemptToSend();
				
				if( (plistElement->GetAttemptToSend()) > MAX_ATTEMPS_TO_SEND)
				{
					m_lEmail.RemoveAt(listPosSave);
					plistElement->Release();
					TRACE("Email aus Liste gelöscht...Max. Sendeversuche erreicht\n");
				}
				
				listPosSave=listElementPos;
			}
		}
		
		else
		{
			pEmail->IncreaseAttemptToSend();
			if((pEmail->GetAttemptToSend())>MAX_ATTEMPS_TO_SEND)
			{
				m_lEmail.RemoveAt(m_lEmail.Find(pEmail));
				pEmail->Release();
				TRACE("Email aus Liste gelöscht...Max. Sendeversuche erreicht");
			}
		}
	}
	if(!(m_lEmail.IsEmpty()))
	{
		//Liste nicht Leer Timer für nächsten Übertragungsversuch starten.
		if(!m_bTimerIsSet)
		{
			m_pDlg->KillTimer(EMAIL_PARKING_TIMER);
			TRACE("Timer set to %i min\n",(m_iTimerElapse/1000)/60);
			m_pDlg->SetTimer(EMAIL_PARKING_TIMER,m_iTimerElapse,NULL);
			m_iTimerElapse=m_iTimerElapse*2;	//3				//1min, 3min, 9min
			if((m_iTimerElapse/1000)/60==27)		//27
			{
				m_iTimerElapse=1;
			}
			m_bTimerIsSet=TRUE;
			
		}
		
	}
	else
	{
		WK_TRACE(_T("%i Emails geparkt\n"),m_lEmail.GetCount());
		m_pDlg->KillTimer(EMAIL_PARKING_TIMER);
		DisconnectMailServer();
		DisconnectNetwork();
	}
	TRACE(_T("Emails in Liste: %i\n"),m_lEmail.GetCount());
}

///////////////////////////////////////////////////////////////////////////////////////////

void CEmailClient::SetTimerIsSet(BOOL btimerSet)
{
	m_bTimerIsSet=btimerSet;
}

void CEmailClient::DeleteEmailList()
{
	POSITION listElementPos, listPosSave;
	CEmail* plistElement;
	
	listElementPos = m_lEmail.GetHeadPosition();
	listPosSave = listElementPos;
	
	while(listElementPos != NULL)
	{
		plistElement = m_lEmail.GetNext(listElementPos);
		plistElement->Release();
		m_lEmail.RemoveAt(listPosSave);
		
		listPosSave=listElementPos;
		
	}
	
}

///////////////////////////////////////////////////////////////////////////////////////////
//Funktion baut Verbindung mit Mail-Server auf.
//Netwerkverbindung besteht bereits(LAN oder DFÜ)
BOOL CEmailClient::ConnectToSMTPServer()
{
	CProfiler profile;
	CSocket MySocket;
	CString sSMTPServer;
	UINT uSMTPPort;
	
	//Servernamen und Port aus Registry holen und Socket konstruieren
	
	sSMTPServer=profile.GetServer();
	uSMTPPort  =profile.GetPort();
	
	if(sSMTPServer=="")
		return FALSE;
	
	if(uSMTPPort==0)
		return FALSE;
	
	
	MySocket.Create();
	
	if(!MySocket.Connect(sSMTPServer,uSMTPPort))
	{
		
		m_bConnectedToMailServer=FALSE;
		m_pSMTP->SetConnectStatus(FALSE);
		DWORD t=GetLastError();
		WK_TRACE(_T("Error %u, connecting to SMTP-Server.\n"), t);
		return FALSE;
	}
	m_pDlg->ChangeIcon(GREEN);
	m_pSMTP->ResetSequence();
	m_bConnectedToMailServer=TRUE;
	SOCKET hSocket=MySocket.Detach();
	
	m_pConnection = new CConnection(m_pSMTP,NULL);
	m_pConnection->Attach(hSocket);
	
	WK_TRACE(_T("Socketconnection to SMTP-Server.\n"));
	
	return TRUE;
}
///////////////////////////////////////////////////////////////////////////////////////////
//Funktion baut Verbindung mit Netzwerk (LAN oder DFÜ) auf
BOOL CEmailClient::ConnectToNetwork()
{				 
	m_pDlg->ChangeIcon(RED);
	CProfiler profile;
	m_pDlg->SetTimer(NETWORK_TIMER,NETWORK_TIMEOUT*1000,NULL);
	if(!profile.ConnectionIsLAN())		//Registry:DFÜ Netzwerk aufbauen;
	{
		
		if(CheckRasAPI())
		{
			m_bConnectionIsLAN=FALSE;
			m_pDialUpNetwork= new CDialUpNetwork(m_pDlg);
			TRACE(_T("Verbindung ist DFÜ Netzwerk\n"));
			m_pDialUpNetwork->DialUp(profile.GetDFUE());	
			return FALSE;
		}
		else
		{
			m_pDlg->ChangeIcon(BLUE);
			WK_TRACE(_T("[CEmailClient::ConnectToNetwork]\n"));
			WK_TRACE(_T("Fehler beim importieren der RasAPI32.dll\n"));
			ParkEmail();
			return FALSE;
		}
		
	}
	else
	{
	  	m_bConnectedToNetwork=TRUE;
	}
	//hier noch testen auf LAN
	m_pDlg->ChangeIcon(YELLOW);
	m_bConnectionIsLAN=TRUE;

	return TRUE;
}
///////////////////////////////////////////////////////////////////////////////////////////
void CEmailClient::DisconnectNetwork()
{
	WK_TRACE(_T("[CEmailClient::DisconnectNetwork]\n"));
	m_pDlg->ChangeIcon(RED);
	if((!m_bConnectionIsLAN) && (m_bConnectedToNetwork))
	{
		m_pDialUpNetwork->HangUp();		
		WK_DELETE(m_pDialUpNetwork);
	}
	m_bConnectedToNetwork	= FALSE;
	if(m_lEmail.IsEmpty())
	{
		m_pDlg->ChangeIcon(GREY);
	}
	else
	{
		m_pDlg->ChangeIcon(BLUE);
	}
	
}
///////////////////////////////////////////////////////////////////////////////////////////
void CEmailClient::DisconnectMailServer()
{	
	WK_TRACE(_T("[CEmailClient::DisconnectMailServer]\n"));
	if(m_bConnectedToMailServer)
	{
		WK_DELETE(m_pConnection);
		m_bConnectedToMailServer= FALSE;
		m_bAuthenticated = FALSE;
		WK_TRACE(_T("Connection to Server, closed.\n"));
		m_pDlg->ChangeIcon(YELLOW);
	}
}
///////////////////////////////////////////////////////////////////////////////////////////
void CEmailClient::SendData(BYTE *Data, int DataLen)
{
	m_pConnection->SendObjectToClient(Data,DataLen);
	
}
///////////////////////////////////////////////////////////////////////////////////////////
CDialUpNetwork* CEmailClient::GetPtrToDialUpNetwork()
{
	return m_pDialUpNetwork;
}
///////////////////////////////////////////////////////////////////////////////////////////
BOOL CEmailClient::CheckRasAPI()
{
	
	HINSTANCE hModule;
	hModule=LoadLibraryEx(_T("RasApi32.dll"),NULL,0);
	
	if(!hModule)	//RasApi32.dll nicht gefunden oder vorhanden
	{
		WK_TRACE(_T("[CEmailClient::CheckRasAPI]\n"));
		WK_TRACE(_T("RasAPI32.dll konnte nicht geladen werden\n"));
		return FALSE;
	}
	
	else
	{
		
		if(!BuildRasFunctions(hModule))	//Funktion nicht gefunden
		{
			WK_TRACE(_T("[CEmailClient::CheckRasAPI]\n"));
			WK_TRACE(_T("Fehler beim holen der Funktionen aus der RasAPI32.dll\n"));
			return FALSE;
		}
		
		return TRUE;
	}
}
///////////////////////////////////////////////////////////////////////////////////////////
//RasGetEntryDialParams,RasDial,RasGetConnectStatus,RasHangUp
//muessen aus dll geholt werden werden
BOOL CEmailClient::BuildRasFunctions(HINSTANCE hModule)
{
	BOOL bOK;
	
#ifdef _UNICODE
	MyRasGetEntryDialParams = (fpRasGetEntryDialParams)GetProcAddress(hModule,"RasGetEntryDialParamsW");
	MyRasDial = (fpRasDial)GetProcAddress(hModule,"RasDialW");
	MyRasGetConnectStatus = (fpRasGetConnectStatus)GetProcAddress(hModule,"RasGetConnectStatusW");
	MyRasHangUp = (fpRasHangUp)GetProcAddress(hModule,"RasHangUpW");
	MyRasGetEntryProperties=(fpRasGetEntryProperties)GetProcAddress(hModule,"RasGetEntryPropertiesW");
	MyRasEnumConnections=(fpRasEnumConnections)GetProcAddress(hModule,"RasEnumConnectionsW");
#else
	MyRasGetEntryDialParams = (fpRasGetEntryDialParams)GetProcAddress(hModule,"RasGetEntryDialParamsA");
	MyRasDial = (fpRasDial)GetProcAddress(hModule,"RasDialA");
	MyRasGetConnectStatus = (fpRasGetConnectStatus)GetProcAddress(hModule,"RasGetConnectStatusA");
	MyRasHangUp = (fpRasHangUp) GetProcAddress(hModule,"RasHangUpA");
	MyRasGetEntryProperties=(fpRasGetEntryProperties)GetProcAddress(hModule,"RasGetEntryPropertiesA");
	MyRasEnumConnections=(fpRasEnumConnections)GetProcAddress(hModule,"RasEnumConnectionsA");
#endif
	bOK = MyRasGetEntryDialParams && MyRasDial && MyRasGetConnectStatus && MyRasHangUp &&	MyRasGetEntryProperties
		&& MyRasEnumConnections;
	
	return bOK; // Alle Funktionen gefunden
}
///////////////////////////////////////////////////////////////////////////////////////////
BOOL CEmailClient::ConnectToPOPServer()
{
	CProfiler profile;
	CSocket MySocket;
	CString sPOPServer;
	UINT uPOPPort;
	
	//Servernamen und Port aus Registry holen und Socket konstruieren
	
	sPOPServer=profile.GetPOPServer();
	uPOPPort  =profile.GetPOPPort();
	
	if(sPOPServer=="")
		return FALSE;
	
	if(uPOPPort==0)
		return FALSE;
	
	MySocket.Create();
	
	if(!MySocket.Connect(sPOPServer,uPOPPort))
	{
		m_bConnectedToPOPServer=FALSE;
		WK_TRACE(_T("[CEmailClient::ConnectToPOPServer] Error, connecting to POP-Server.\n"));
		return FALSE;
	}
	m_pDlg->ChangeIcon(GREEN);
	
	m_bConnectedToPOPServer=TRUE;
	SOCKET hSocket=MySocket.Detach();
	
	m_pConnection = new CConnection(NULL,m_pPOP);
	m_pConnection->Attach(hSocket);
	
	WK_TRACE(_T("Socketconnection to POP-Server.\n"));
	
	
	return TRUE;
	
}
///////////////////////////////////////////////////////////////////////////////////////////
void CEmailClient::Authenticated()
{
	m_bAuthenticated=TRUE;
}
///////////////////////////////////////////////////////////////////////////////////////////
void CEmailClient::DisconnectPOPServer()
{
	if(m_bConnectedToPOPServer)
	{
		
		WK_DELETE(m_pConnection);
		m_bConnectedToPOPServer= FALSE;
		
		WK_TRACE(_T("CEmailClient::DisconnectPOPServer() Connection to POP Server, closed.\n"));
		m_pDlg->ChangeIcon(GREEN);
	}
}

BOOL CEmailClient::IsAuthenticated()
{
   return m_bAuthenticated;
}

void CEmailClient::SetConnectedToNetwork(BOOL bstatus)
{
   m_bConnectedToNetwork = bstatus;
}
///////////////////////////////////////////////////////////////////////////////////////////
CString CEmailClient::GetRemoteIP()
{
	CString s;
	UINT p;
	if (m_pConnection)
	{
		m_pConnection->GetPeerName(s,p);
	}
	return s;
}
