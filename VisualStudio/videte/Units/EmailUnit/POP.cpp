// POP.cpp: implementation of the CPOP class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "emailunit.h"
#include "POP.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

////////////////////////////////////////////////////////////////////////
// Construction/Destruction
////////////////////////////////////////////////////////////////////////
CPOP::CPOP(CEmailClient *eClient)
{
	m_iProtocolState = 0;	
	m_pEmailClient = eClient;
}
////////////////////////////////////////////////////////////////////////
CPOP::~CPOP()
{

}
////////////////////////////////////////////////////////////////////////
void CPOP::OnServerReply(CString sReplyMessage)
{
	CWK_String sLine;
//	BYTE *pBuffer=NULL;
//	int iLen = 0;

	WK_TRACE(_T("CPOP::OnServerReply  %s\n"), sReplyMessage);
	sReplyMessage.MakeUpper();

	if(m_iProtocolState==0)
	{
		if(!GetDataFromReg())
		{
		  m_pEmailClient->ParkEmail();
		  WK_TRACE(_T("CPOP::OnServerReply  GetDataFromReg Error\n"));
		}
	}

	if(sReplyMessage.Find(_T("+OK"))!=-1)
	{
	   m_iProtocolState++;
	}
	else
	{
		m_pEmailClient->ParkEmail();
	}

	if(m_iProtocolState==1)
	{
//		iLen = strlen((LPCSTR)sLine);
//		pBuffer = new BYTE[iLen];
//		CopyMemory(pBuffer,(BYTE*)(LPCSTR)sLine, iLen);
//		m_pEmailClient->SendData(pBuffer, iLen);
		sLine.Format(_T("USER %s\r\n"), m_sPOPUser);
		m_pEmailClient->SendData((BYTE*)sLine.DetachAnsiString(), sLine.GetLength());
		WK_TRACE(_T("CPOP::OnServerReply  Send: Username\n"));
	}
	else if(m_iProtocolState==2)
	{
//		iLen = strlen((LPCSTR)sLine);
//		pBuffer = new BYTE[iLen];
//		CopyMemory(pBuffer,(BYTE*)(LPCSTR)sLine, iLen);
//		m_pEmailClient->SendData(pBuffer, iLen);
		sLine.Format(_T("PASS %s\r\n"),m_sPOPPassword);
		m_pEmailClient->SendData((BYTE*)sLine.DetachAnsiString(), sLine.GetLength());
		WK_TRACE(_T("CPOP::OnServerReply  Send: Password\n"));
	}
	else if(m_iProtocolState==3)
	{
//		iLen = strlen((LPCSTR)sLine);
//		pBuffer = new BYTE[iLen];
//		CopyMemory(pBuffer,(BYTE*)(LPCSTR)sLine, iLen);
//		m_pEmailClient->SendData(pBuffer, iLen);
		sLine=_T("QUIT\r\n");
		m_pEmailClient->SendData((BYTE*)sLine.DetachAnsiString(), sLine.GetLength());
		WK_TRACE(_T("CPOP::OnServerReply  Send: %s\n"), sLine);
	}
	else if(m_iProtocolState==4)
	{
	   m_pEmailClient->Authenticated();
	   m_pEmailClient->DisconnectPOPServer();
	   
	   if(!(m_pEmailClient->ConnectToSMTPServer()))//Verbindung zum MailServer herstellen
		{
			//Verbindungsaufbau erfolglos, Mail speichern, später erneut versuchen
			//(Fehler in Netzwerkschicht)
			WK_TRACE(_T("[CPOP::OnServerReply]\n"));
			WK_TRACE(_T("Socketverbindung zum SMTP Server nicht möglich\n"));
			m_pEmailClient->ParkEmail();
			m_pEmailClient->DisconnectNetwork();
		}
		m_iProtocolState=0;
	}
	else
	{
		m_pEmailClient->ParkEmail();
	}
}

////////////////////////////////////////////////////////////////////////

BOOL CPOP::GetDataFromReg()
{
	CProfiler profile;

	m_sPOPServer = profile.GetPOPServer();

	if(m_sPOPServer == _T(""))
		return FALSE;

	m_uPOPPort = profile.GetPOPPort();

	if(m_uPOPPort == 0)
		return FALSE;

	m_sPOPUser = profile.GetPOPUser();

	if(m_sPOPUser == _T(""))
		return FALSE;

	
	m_sPOPPassword = profile.GetPOPPassword();

	if(m_sPOPPassword == _T(""))
		return FALSE;


	return TRUE;
}


