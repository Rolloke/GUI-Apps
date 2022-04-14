// LocalIPGang.cpp: implementation of the CLocalIPGang class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IPBook.h"
#include "LocalIPGang.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLocalIPGang::CLocalIPGang(CIPBookDlg *pDlg)
{
   m_pDlg = pDlg;
   m_bDestroy=FALSE;
}

CLocalIPGang::~CLocalIPGang()
{

}

void CLocalIPGang::OnSocketCreated()
{
	int iNrOfIP=0;
	LONG* pIPList=GetLocalIP(iNrOfIP);
	
	m_sLocalIP=inet_ntoa(*(struct in_addr*)&pIPList[iNrOfIP-1]);
	m_pDlg->SetLocalIP(m_sLocalIP);
	Bind(m_sLocalIP,m_uMulticastPort);	

}


void CLocalIPGang::OnSocketBind()
{
  
	JoinMulticastGroup(m_sMulticastAddress);

}

void CLocalIPGang::OnSocketClosed(CWinSocket* pSocket)
{

}

void CLocalIPGang::OnMulticastMembershipDropped(CString sMulticastIP, int iMulticastPort)
{

}

void CLocalIPGang::OnMultiplexMembershipDropped(CString sMulticastIP, int iMulticastPort)
{

}

void CLocalIPGang::OnMulticastGroupJoined(CString sMulticastIP, int iMulticastPort)
{
 	SetDuplexMode(TRUE,TRUE);
}


void CLocalIPGang::OnMultiplexMemberAdded(CString sMulticastIP, int iMulticastPort)
{

}

void CLocalIPGang::OnConfirmBandwidth(CString sRemoteIP, int iThroughput)
{
   	m_pDlg->SetBandwidth(sRemoteIP,iThroughput);
}

void CLocalIPGang::OnDataSend(DATAGRAM* pDatagram)
{
	if(pDatagram)
		DELETE_DATAGRAM(pDatagram);
	if(m_bDestroy)
		m_DestroyEvent.SetEvent();

}

void CLocalIPGang::OnNewData(DATAGRAM* pDatagram)
{
	if(!m_bDestroy)
	{

		CString sMessage,sHost,sIP;
		sMessage = (const char*)pDatagram->bDataBuffer;
		int iPos;

		if(sMessage.Left(5)=="HELLO")
		{
			//sMessage.Format("HELLO FROM %s",sH);
			iPos = sMessage.Find(":");

			sHost=sMessage.Mid(11,iPos-11);
			sIP = sMessage.Right(sMessage.GetLength()-(iPos+1));


			if(sIP != m_sLocalIP)
			{

			
				AddMember(sIP,sHost);
				RequestBandwidth(sIP,m_uMulticastPort);

				DATAGRAM *pDatagram = new DATAGRAM;

				INIT_DATAGRAM(pDatagram);

				
				CString sMessage ;

				sMessage.Format("OK FROM %s:%s",GetHostName(),m_sLocalIP);
				pDatagram->bDataBuffer=new BYTE[sMessage.GetLength()+1];
				pDatagram->uDataBufferSize = sMessage.GetLength()+1;
				CopyMemory(pDatagram->bDataBuffer,(BYTE*)(const char*)sMessage,sMessage.GetLength()+1);


				Send(pDatagram,m_sMulticastAddress,m_uMulticastPort,FLAG_MULTICAST);

			}

		}
		else if(sMessage.Left(2)=="OK")
		{	
			
			iPos = sMessage.Find(":");

			sHost=sMessage.Mid(8,iPos-8);
			sIP = sMessage.Right(sMessage.GetLength()-(iPos+1));

			if(sIP != m_sLocalIP)
			{
				RequestBandwidth(sIP,m_uMulticastPort);
				AddMember(sIP,sHost);
			}
		}
		else if(sMessage.Left(7)=="GOODBYE")
		{
			iPos = sMessage.Find(":");
			
			sHost=sMessage.Mid(13,iPos-13);
			sIP = sMessage.Right(sMessage.GetLength()-(iPos+1));

			if(sIP != m_sLocalIP)
			{
				RemoveMember(sIP,sHost);
			}

		}
		else if(sMessage.Left(7)="REFRESH")
		{
			
			iPos = sMessage.Find(":");
			
			sHost=sMessage.Mid(13,iPos-13);
			sIP = sMessage.Right(sMessage.GetLength()-(iPos+1));
			
			if(sIP != m_sLocalIP)
			{
				RefreshMember(sIP,sHost);
				RequestBandwidth(sIP,m_uMulticastPort);
			}
		}
	}	

   DELETE_DATAGRAM(pDatagram);

}

void CLocalIPGang::OnConnectionClosed(CWinSocket* pSocket)
{



}

void CLocalIPGang::OnSending(CWinSocket* pSocket)
{
	DATAGRAM *pDatagram = new DATAGRAM;

	INIT_DATAGRAM(pDatagram);

	
	CString sMessage ;

	sMessage.Format("HELLO FROM %s:%s",GetHostName(),m_sLocalIP);
	pDatagram->bDataBuffer=new BYTE[sMessage.GetLength()+1];
	pDatagram->uDataBufferSize = sMessage.GetLength()+1;
	CopyMemory(pDatagram->bDataBuffer,(BYTE*)(const char*)sMessage,sMessage.GetLength()+1);


	Send(pDatagram,m_sMulticastAddress,m_uMulticastPort,FLAG_MULTICAST);
	/*Sleep(100);

	pDatagram = new DATAGRAM;
	INIT_DATAGRAM(pDatagram);

	pDatagram->bDataBuffer=new BYTE[sMessage.GetLength()+1];
	pDatagram->uDataBufferSize = sMessage.GetLength()+1;
	CopyMemory(pDatagram->bDataBuffer,(BYTE*)(const char*)sMessage,sMessage.GetLength()+1);

	Send(pDatagram,m_sMulticastAddress,m_uMulticastPort,FLAG_MULTICAST);*/
	SendKeepAlives("ramon2",815,10,"Test");


}

void CLocalIPGang::OnReceiving(CWinSocket* pSocket)
{



}



void CLocalIPGang::SetMulticastAddress(CString sIP)
{
	m_sMulticastAddress=sIP;
}


void CLocalIPGang::SetMulticastPort(UINT uPort)
{
	m_uMulticastPort = uPort;
}

void CLocalIPGang::LeaveGang()
{
	DeleteList();

	DropFromMulticastGroup(m_sMulticastAddress);
	DATAGRAM *pDatagram = new DATAGRAM;

	INIT_DATAGRAM(pDatagram);
	
	CString sMessage ;

	sMessage.Format("GOODBYE FROM %s:%s",GetHostName(),m_sLocalIP);
	pDatagram->bDataBuffer=new BYTE[sMessage.GetLength()+1];
	pDatagram->uDataBufferSize = sMessage.GetLength()+1;
	CopyMemory(pDatagram->bDataBuffer,(BYTE*)(const char*)sMessage,sMessage.GetLength()+1);

	m_bDestroy=TRUE;
	Send(pDatagram,m_sMulticastAddress,m_uMulticastPort,FLAG_MULTICAST);

}

void CLocalIPGang::AddMember(CString sIP, CString sHost)
{
	MEMBER *pMember=new MEMBER;
	DWORD dwTick = GetTickCount();

	if(!FindMemberInList(sIP))
	{
		pMember->dwRefreshTick=dwTick;
		pMember->sIP=sIP;
		pMember->sHost=sHost;
		m_MemberList.AddTail(pMember); 
		m_pDlg->AddAddress(sIP,sHost);

	}

}

void CLocalIPGang::RemoveMember(CString sIP, CString sHost)
{
	MEMBER *pMember;
	POSITION pos = FindMemberInList(sIP);
	int i= m_MemberList.GetCount();
	if(pos)
	{
	   pMember = m_MemberList.GetAt(pos);
	   delete pMember;
	   m_MemberList.RemoveAt(pos);
       m_pDlg->RemoveAddress(sIP,sHost);
	}

}

POSITION CLocalIPGang::FindMemberInList(CString sIP)
{

	MEMBER *pMember;
	POSITION posA,posB;

	posA = m_MemberList.GetHeadPosition();
	posB = posA;
	while(posA)
	{
		pMember = m_MemberList.GetNext(posA);

		if(pMember->sIP == sIP)
		{
			return posB;	
		}
		posB = posA;
	}
	return NULL;
}


void CLocalIPGang::CheckTimeouts()
{
	MEMBER *pMember;
	POSITION posA;
	DWORD dwTick;
	int iTime;
	int i= m_MemberList.GetCount();

	posA = m_MemberList.GetHeadPosition();

	while(posA)
	{
		pMember = m_MemberList.GetNext(posA);
		dwTick=GetTickCount();
		iTime = dwTick - pMember->dwRefreshTick;

		if(iTime > TIMEOUT)
		{
			RemoveMember(pMember->sIP,pMember->sHost);		
		}
	}

}

void CLocalIPGang::SendRefresh()
{

	DATAGRAM *pDatagram = new DATAGRAM;

	INIT_DATAGRAM(pDatagram);

	
	CString sMessage ;

	sMessage.Format("REFRESH FROM %s:%s",GetHostName(),m_sLocalIP);
	pDatagram->bDataBuffer=new BYTE[sMessage.GetLength()+1];
	pDatagram->uDataBufferSize = sMessage.GetLength()+1;
	CopyMemory(pDatagram->bDataBuffer,(BYTE*)(const char*)sMessage,sMessage.GetLength()+1);


	Send(pDatagram,m_sMulticastAddress,m_uMulticastPort,FLAG_MULTICAST);

}

void CLocalIPGang::RefreshMember(CString sIP, CString sHost)
{
	MEMBER *pMember;

	POSITION pos = FindMemberInList(sIP);

	if(pos)
	{
		pMember = m_MemberList.GetAt(pos);
		pMember->dwRefreshTick = GetTickCount();
		return;
	}
	else
	{
		AddMember(sIP,sHost);
	}

}

void CLocalIPGang::DeleteList()
{
	MEMBER *pMember;
	POSITION posA;


	posA = m_MemberList.GetHeadPosition();

	while(posA)
	{
		pMember = m_MemberList.GetNext(posA);
		delete pMember;
	}


}
