// IPBook.cpp: implementation of the CIPBook class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IPBook.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


void FillDatagram(DATAGRAM *pDatagram, const CWK_String& sMessage)
{
#ifdef _UNICODE
	DWORD dwStringLen = sMessage.CopyToMemory(FALSE,NULL);
#else
	DWORD dwStringLen = sMessage.CopyToMemory(NULL);
#endif
	pDatagram->bDataBuffer = new BYTE[dwStringLen+1];
	ZeroMemory(pDatagram->bDataBuffer,dwStringLen+1);

#ifdef _UNICODE
	sMessage.CopyToMemory(FALSE,pDatagram->bDataBuffer);
#else
	sMessage.CopyToMemory(pDatagram->bDataBuffer);
#endif
	pDatagram->uDataBufferSize = dwStringLen+1;
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#ifdef _DTS
CMapStringToString CIPBook::m_MapServerList;
#endif

CIPBook::CIPBook()
{
  m_bIsServer = FALSE;
}

CIPBook::~CIPBook()
{
   m_Hosts.DeleteAll();
   RemoveFromServerList();
}

///////////////////////////////////////////////////////////////////////////

void CIPBook::OnSocketCreated()
{
	int iNrOfIP=0;
	if(m_sLocalIP==_T(""))	//Zusatz aufgrund von Kompatibilitätsgründen
	{
		LONG* pIPList = GetLocalIP(iNrOfIP);
		m_sLocalIP    = inet_ntoa(*(struct in_addr*)&pIPList[0]);
	}
	ReuseAddr(VIDETE_CIPC_PORT);
 	//Bind(m_sLocalIP,VIDETE_CIPC_PORT);
}

/////////////////////////////////////////////////////////////////////////////

void CIPBook::OnSocketBind()
{
	//ReuseAddr(VIDETE_CIPC_PORT);
    JoinMulticastGroup(VIDETE_IP,m_sLocalIP);
}

/////////////////////////////////////////////////////////////////////////////

void CIPBook::OnSocketClosed(CWinSocket* pSocket)
{

}

/////////////////////////////////////////////////////////////////////////////

void CIPBook::OnMulticastMembershipDropped(CString sMulticastIP, int iMulticastPort)
{
	m_eventBookClosed.SetEvent();
	m_Hosts.DeleteAll();
}

/////////////////////////////////////////////////////////////////////////////

void CIPBook::OnMulticastGroupJoined(CString sMulticastIP, int iMulticastPort)
{
	SetDuplexMode(TRUE,TRUE);
}

/////////////////////////////////////////////////////////////////////////////

void CIPBook::OnDataSend(DATAGRAM* pDatagram)
{
	
	if(m_bIsServer)  
	{	
		CString sMsg, sName, sIP;
		ParseMessage((LPCSTR)pDatagram->bDataBuffer, sMsg, sIP, sName);

						//Server hat sich abgemeldet
		if (sMsg ==_T("DTS_IP_SERVER_OFF"))
		{
			DropFromMulticastGroup(VIDETE_IP,m_sLocalIP);
		}
	}
	else			   //Client hat sich angemeldet
	{
		
	}

	DELETE_DATAGRAM(pDatagram);
}

/////////////////////////////////////////////////////////////////////////////

void CIPBook::OnNewData(DATAGRAM* pDatagram)
{
	CString sMsg, sIP, sName;
	
	ParseMessage((LPCSTR)pDatagram->bDataBuffer, sMsg, sIP, sName);
	
	if (!m_bIsServer)  
	{
										//Server hat sich abgemeldet
		if (sMsg ==	_T("DTS_IP_SERVER_OFF"))
		{
			CWK_Dongle Dongle;
			if(!Dongle.IsDemo())
			{
				RemoveFromReg(sIP);
			}
			m_MapServerList.RemoveKey(sIP);
			SendKeepAlives(sIP,
					  	   VIDETE_CIPC_PORT,
						   0,
						   _T("")
					       );
		}
										  //Server hat sich gemeldet
		else if(sMsg ==	_T("DTS_IP_SERVER_ON"))
		{
			CWK_Dongle Dongle;
			if(!Dongle.IsDemo())
			{
				LoadAdditionalHostList();
				CString sHostIP;
				BOOL bAlreadyInAddList=FALSE;
				for (int nI = m_AdditionalHosts.GetSize()-1; nI >=0 ; nI--)
				{
					sHostIP=m_AdditionalHosts.GetAt(nI)->GetClearNumber();
					if(sIP==sHostIP)
					{
						bAlreadyInAddList=TRUE;
					}
				}
				if(!bAlreadyInAddList)
				{
					UpdateReg(sIP, sName);
				}
			}

		    m_MapServerList.SetAt(sIP,m_sLocalIP);
		    CString sMessage;	   
//			TRACE(_T("%s:%s, on Adapter(%s)\n"),sName,sIP,m_sLocalIP);	
			SendKeepAlives(sIP,
					  	   VIDETE_CIPC_PORT,
						   INTERVALL_TIME,
						   _T("")//sMessage
					       );
		}
	}
	else	
	{
		if(sMsg ==_T("REQ_DTS_SERVER_FROM"))
		{
									//Clients Request beantworten
			DATAGRAM *pDatagram = new DATAGRAM;
			INIT_DATAGRAM(pDatagram);
			CWK_String sMessage;
			
		    LoadHostList();
		    sMessage.Format(_T("DTS_IP_SERVER_ON %s % s"),m_sLocalIP, m_Hosts.GetLocalHostName());
			FillDatagram(pDatagram,sMessage);

			//WK_TRACE(_T("DTS_IP_RECEIVER %s logged in\n"),sIP);

		    Send(pDatagram, 
			     sIP,
			     VIDETE_CIPC_PORT,
			     FLAG_NORMAL);
		}
	}
	DELETE_DATAGRAM(pDatagram);
}

/////////////////////////////////////////////////////////////////////////////
void CIPBook::OnConnectionLost(CString sRemoteIP,int iRemotePort)
{
	if(m_bIsServer)  
	{					  
		//Egal
	}
	else			   
	{ 	//Server aus IPBook löschen
/*
		RemoveFromReg(sRemoteIP);
		m_MapServerList.RemoveKey(sRemoteIP);
*/
	}
}

/////////////////////////////////////////////////////////////////////////////

void CIPBook::OnSending(CWinSocket* pSocket)
{

	SetIdleTimer(IDLE_TIME);
}

/////////////////////////////////////////////////////////////////////////////

void CIPBook::OnReceiving(CWinSocket* pSocket)
{
   
   CWK_Dongle Dongle;
   if(!Dongle.IsDemo())
   {
	   DATAGRAM *pDatagram = new DATAGRAM;
	   INIT_DATAGRAM(pDatagram);
				//SERVER
	   if(m_bIsServer)
	   {
		   CWK_String sMessage;
		   //Server meldet sich an
		   LoadHostList();
		   sMessage.Format(_T("DTS_IP_SERVER_ON %s %s"),m_sLocalIP,m_Hosts.GetLocalHost());
		   FillDatagram(pDatagram,sMessage);
	   											
		   Send(pDatagram, 
				VIDETE_IP,
				VIDETE_CIPC_PORT,
				FLAG_MULTICAST);
	   }
	   else		//CLIENT   
	   {
		  CWK_String sMessage;	   
						 //Request an MulticastGruppe senden

		  sMessage.Format(_T("REQ_DTS_SERVER_FROM %s %s"),m_sLocalIP, _T("Client"));
		  FillDatagram(pDatagram,sMessage);
	   											
		   Send(pDatagram, 
				VIDETE_IP,
				VIDETE_CIPC_PORT,
				FLAG_MULTICAST);
		   RecoverAddHostIPAdapter();
		}
   }
}

/////////////////////////////////////////////////////////////////////////////

void CIPBook::OnReused(int iPort)
{
    Bind(m_sLocalIP,VIDETE_CIPC_PORT);
	//JoinMulticastGroup(VIDETE_IP,m_sLocalIP);
}

/////////////////////////////////////////////////////////////////////////////

void CIPBook::OnSocketError(CWinSocket* pSocket, int iErrorcode)
{
	WK_TRACE(_T("OnSocketError Code: %i\n"),iErrorcode);
}

/////////////////////////////////////////////////////////////////////////////

void CIPBook::OnKeepAliveMessage(CString sMessage)
{
	if(m_bIsServer)  
	{	
		CString sMsg, sIP, sName;
		CWK_String s1(sMessage);

		ParseMessage((LPCSTR)s1, sMsg, sIP, sName);
		if ( sMsg ==_T("REQ_DTS_SERVER_FROM"))
		{
									//Clients Request beantworten
		    DATAGRAM *pDatagram = new DATAGRAM;
			INIT_DATAGRAM(pDatagram);

			LoadHostList();
			CWK_String s2;
		    s2.Format(_T("DTS_IP_SERVER_ON %s % s"),m_sLocalIP, m_Hosts.GetLocalHost());
			FillDatagram(pDatagram,s2);
	   										
		    Send(pDatagram, 
			     sIP,
			     VIDETE_CIPC_PORT,
			     FLAG_NORMAL);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////


void CIPBook::StartIPBook(CString sLocalIP)
{
	CWK_Dongle Dongle;

	m_bIsServer = Dongle.GetProductCode() == IPC_DTS;

	m_sLocalIP		 = sLocalIP;
	Create(UDP);
	LoadHostList();
}

/////////////////////////////////////////////////////////////////////////////

void CIPBook::StopIPBook()
{
  CWK_Dongle Dongle;

  if(!Dongle.IsDemo())
  {
	  if(m_bIsServer)
	  {	
		  CWK_String sMessage;
		  //Aus Gruppe Abmelden
  		  DATAGRAM *pDatagram = new DATAGRAM;
		  INIT_DATAGRAM(pDatagram);

		  LoadHostList();
		  sMessage.Format(_T("DTS_IP_SERVER_OFF %s %s"),(LPCTSTR)m_sLocalIP, (LPCTSTR)m_Hosts.GetLocalHost());
		  FillDatagram(pDatagram,sMessage);
	   										
		  Send(pDatagram, 
			   VIDETE_IP,
			   VIDETE_CIPC_PORT,
			   FLAG_MULTICAST);

		  //WK_TRACE(_T("DTS_IP Server IPBook stoppt Sendung von ID KEEP_ALIVES\n"));
										//Event wird in OnMulticastMembershipDropped gesetzt
		  WaitForSingleObject(m_eventBookClosed,5000);
	   }
	   else
	   {				//KeepAlivesSenden Beenden
		   /*SendKeepAlives(VIDETE_IP,
						  VIDETE_PORT,
						  0);				  */
						//Aus Gruppe entfernen
		   DropFromMulticastGroup(VIDETE_IP,m_sLocalIP);
		   WaitForSingleObject(m_eventBookClosed,5000);
	   }
  }
}

/////////////////////////////////////////////////////////////////////////////

void CIPBook::RemoveFromReg(CString sIP)
{
	BOOL bFound = FALSE;

    LoadHostList();

	for (int nI = 0; nI < m_Hosts.GetSize(); nI++)
	{
		CString sListIP = m_Hosts.GetAt(nI)->GetNumber();
		if (sListIP == _T("tcp:")+sIP)
		{
			bFound = TRUE;
			break;
		}
	}
	if (bFound)
	{
		WK_TRACE(_T("removing host %s, %s\n"),m_Hosts.GetAt(nI)->GetName(),m_Hosts.GetAt(nI)->GetNumber());
		m_Hosts.RemoveAt(nI);
		CWK_Profile prof(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE,_T("DVRT\\DV"),_T(""));
		WK_TRACE(_T("saving DV host list %d hosts\n"),m_Hosts.GetSize());
		m_Hosts.Save(prof);
	}
}

/////////////////////////////////////////////////////////////////////////////

void CIPBook::UpdateReg(CString sIP, CString sName)
{
	BOOL bFoundIP	  = FALSE;
	BOOL bNameChanged = FALSE;

    LoadHostList();

	for (int nI = m_Hosts.GetSize()-1; nI >=0 ; nI--)
	{
		if (m_Hosts.GetAt(nI)->GetClearNumber() == sIP)
		{
			bFoundIP = TRUE;
			if (m_Hosts.GetAt(nI)->GetName() != sName)
			{
				delete(m_Hosts.GetAt(nI));
				m_Hosts.RemoveAt(nI);		
				bNameChanged = TRUE;
			}
			break;
		}
	}

	// Ist die IP-Adresse noch nicht in der Liste, oder hat sich der Name geändert?
	if (!bFoundIP || bNameChanged)
	{
		CHost* pHost = m_Hosts.AddHost();
		pHost->SetName(sName);
		pHost->SetNumber(_T("tcp:")+sIP);
		WK_TRACE(_T("new name host %s, %s\n"),pHost->GetName(),pHost->GetNumber());
		CWK_Profile prof(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE,_T("DVRT\\DV"),_T(""));
		WK_TRACE(_T("saving DV host list %d hosts\n"),m_Hosts.GetSize());
		m_Hosts.Save(prof);
	}
}

/////////////////////////////////////////////////////////////////////////////

BOOL CIPBook::ParseMessage(LPCSTR sMessage, CString& sMsg, CString& sIP, CString& sHostName)
{
	BOOL bResult = FALSE;

	TCHAR szMsg[128];							  
	TCHAR szIP[128];
	TCHAR szName[128];
	CWK_String s(sMessage);
						  //Maskierung : _T("Irgendwas | Irgendwas | Alle Zeichen von ASCIIcode + bis ASCIIcode z")
	if (_stscanf(s, _T("%s %s %[+-z ]"), &szMsg,&szIP,&szName) == 3)
	{
		sMsg = szMsg;
		sIP = szIP;
		sHostName = szName;

		bResult = TRUE;
	}
	else
	{
		//WK_TRACE_WARNING(_T("scanf error\n"));
	}		

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////

void CIPBook::OnIdleTimer()
{
			//SERVER
   if(m_bIsServer)
   {

	   DATAGRAM *pDatagram = new DATAGRAM;
	   INIT_DATAGRAM(pDatagram);

	   CWK_String sMessage;

		//Server meldet sich in regelmässigen Abständen
	   LoadHostList();
	   sMessage.Format(_T("DTS_IP_SERVER_ON %s %s"),m_sLocalIP,m_Hosts.GetLocalHostName());
	   FillDatagram(pDatagram,sMessage);
	   										
	   Send(pDatagram, 
	        VIDETE_IP,
			VIDETE_CIPC_PORT,
			FLAG_MULTICAST);
   }
   else
   {
	   RecoverAddHostIPAdapter();
   }

}

/////////////////////////////////////////////////////////////////////////////
BOOL CIPBook::LoadHostList()
{
	CWK_Profile prof(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE,_T("DVRT\\DV"),_T(""));

	m_Hosts.Load(prof);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

void CIPBook::LoadAdditionalHostList()
{
	CWK_Profile prof(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE,_T("DVRT\\DV\\ADDITIONALHOSTS"),_T(""));
	m_AdditionalHosts.Load(prof);
}

/////////////////////////////////////////////////////////////////////////////////////

void CIPBook::RecoverAddHostIPAdapter()
{
	LoadAdditionalHostList();
	CString sHostIP;
	CWK_String sMessage;
	DATAGRAM *pDatagram;
	for (int nI = m_AdditionalHosts.GetSize()-1; nI >=0 ; nI--)
	{
		sHostIP=m_AdditionalHosts.GetAt(nI)->GetClearNumber();

	    sMessage.Format(_T("REQ_DTS_SERVER_FROM %s %s"),m_sLocalIP, _T("Client"));

		for(int iN=0;iN<2;iN++)
		{

			pDatagram = new DATAGRAM;
			INIT_DATAGRAM(pDatagram);
			pDatagram->bFlag							 = FLAG_NORMAL;	
			FillDatagram(pDatagram,sMessage);
			Send(pDatagram,sHostIP,VIDETE_CIPC_PORT,FLAG_NORMAL,254);
			Sleep(10);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////
void CIPBook::RemoveFromServerList()
{
	POSITION pos;
	CString sRemoteIP, sAdapterIP;

	for( pos = m_MapServerList.GetStartPosition(); pos != NULL; )
	{
		m_MapServerList.GetNextAssoc(pos,sRemoteIP,sAdapterIP);
		if(sAdapterIP == m_sLocalIP)
		{
			m_MapServerList.RemoveKey(sRemoteIP);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////
CString CIPBook::GetAdapterIP()
{
	return m_sLocalIP;
}
