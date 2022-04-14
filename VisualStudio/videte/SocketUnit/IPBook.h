// IPBook.h: interface for the CIPBook class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPBOOK_H__C60DE764_E07E_11D4_86CA_004005A26A3B__INCLUDED_)
#define AFX_IPBOOK_H__C60DE764_E07E_11D4_86CA_004005A26A3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "netcomm\winsocket.h"

#define VIDETE_IP			"234.5.6.7"
#define VIDETE_CIPC_PORT    1927
#define VIDETE_OLD_PORT		888
#define INTERVALL_TIME	15        //Keep Alive Intervall in Sekunden
#define IDLE_TIME       10000	   //ms	  Zeitintervall in dem OnIdle aufgerufen wird

class CIPBook : public CWinSocket  
{
	
	/*	1.Server
		- An Multicastgruppe anmelden
		- Eigenen Namen an Gruppe senden
		- Auf Client Request warten
			- Eigenen Namen an Client senden
		- Im Intervall eigenen Namen an Gruppe senden

		2.Client
		- An Multicastgruppe anmelden
		- Servernamenanforderung an Gruppe senden
		- Servernamen in Registry eintragen
		- Servernamen in Registry updaten
    */

public:
	CString GetAdapterIP();

	CIPBook();
	virtual ~CIPBook();	

	void StartIPBook(CString sLocalIP="");
	void StopIPBook();
	void RecoverAddHostIPAdapter();
	static CMapStringToString m_MapServerList;

private:
	void RemoveFromServerList();

	void LoadAdditionalHostList();

	BOOL ParseMessage(LPCSTR sMessage, CString& sMsg, CString& sIP, CString& sHostName);
	BOOL LoadHostList();

	void UpdateReg(CString sIP, CString sName);
	void RemoveFromReg(CString sIP);

	void OnIdleTimer();
	
	////////// Für alle Sockettypen betreffendes Interface //////////
			//Socketfehler
	 void OnSocketError(CWinSocket* pSocket, int iErrorcode);
	        //Socket wurde erfolgreich an IP und Port gebunden
	 void OnSocketBind();							  
		    //Socketobject wurde erfolgreich angelegt
	 void OnSocketCreated();							  
	        //Socket wurde geschlossen	über API closesocket
	 void OnSocketClosed(CWinSocket* pSocket);
			//Daten wurde versendet. (Datagram kann nun gelöscht werden)
	 void OnDataSend(DATAGRAM* pDatagram);
			//Neue Daten sind eingetroffen		
	 void OnNewData(DATAGRAM* pDatagram);
			//Daten können gesendet werden. Der zuständige Thread wurde gestartet
	 void OnSending(CWinSocket* pSocket);
			//Daten können empfangen werden. Der zuständige Thread wurde gestartet
	 void OnReceiving(CWinSocket* pSocket);
			//Ein Request wurde beantwortet
	 void OnReused(int iPort);  
			//Einem KeepAlive wurde eine Message angehangen
	 void OnKeepAliveMessage(CString sMessage);
			//Verbindung zum Remote Rechner verloren
	 void OnConnectionLost(CString sRemoteIP,int iRemotePort);


	////////// UDP betreffendes Interface ///////////////////////////

			//Multicast Mitglied wurde aus Gruppe entfernt
	 void OnMulticastMembershipDropped(CString sMulticastIP, int iMulticastPort);
			//Multicast Mitglied wurde in Guppe IP-Port angefügt
	 void OnMulticastGroupJoined(CString sMulticastIP, int iMulticastPort);

	 CEvent m_eventBookClosed;
	 BOOL m_bIsServer;

	 CString m_sLocalIP;

	 CHostArray m_Hosts,m_AdditionalHosts;
};

#endif // !defined(AFX_IPBOOK_H__C60DE764_E07E_11D4_86CA_004005A26A3B__INCLUDED_)
