// SMTP.cpp: implementation of the CSMTP class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EmailUnit.h"
#include "SMTP.h"
#include "Profiler.h"




#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSMTP::CSMTP(CEmailClient *eClient,CList<CEmail*,CEmail*> *Email)
{
	m_pEmailClient =eClient;
	m_lpEmail      =Email;
	m_pActEmail    =NULL;
	m_pEmailContent=NULL;
	m_bConnected   =FALSE;
	m_iSequenceState = CONNECTION_ESTABLISHMENT;
}

CSMTP::~CSMTP()
{

}


/////////////////////////////////////////////////////////////////////////////////////////////





//////////////////////////////////////////////////////////////////////////////////////////////
//Wird von CConnection::OnReceive aufgerufen
void CSMTP::OnServerReply(CString ReplyMessage)
{
	UINT ReplyCode;
	CString CommandLine;
	
	ReplyCode = GetServerReplyCode(ReplyMessage);
	WK_TRACE(_T("[CSMTP::OnServerReply] <%s>\n"),ReplyMessage);
	WK_TRACE(_T("ServerReply : %i\n"),ReplyCode);
	
	if(ReplyCode < 300)							//CommandLine fehlerfrei vom Server verarbeitet
	{											//Nächsten Schritt in der Sequence einleiten
		//HELO->MAIL->RECPT->...->QUIT
		IncreaseSequenceState();
		
		
		if(m_iSequenceState==MAIL)
		{										//Email aus Liste holen
			m_pActEmail=m_lpEmail->GetHead();
												//Empfänger für Email holen
		}
		
		else if (m_iSequenceState==RCPT || m_iSequenceState==DATA)
		{
			m_lsEmailReceiver=m_pActEmail->GetReceiverList();
			if (!m_lsEmailReceiver->IsEmpty())
			{
				m_sNextEmailReceiver=m_lsEmailReceiver->RemoveHead();
				m_pActEmail->SetTo(m_sNextEmailReceiver);
				m_iSequenceState=RCPT;
			}
		}
								//Mail wurde versendet, Verbindung offen(Es kann noch mehr 
								//gesendet werden.
		else if(m_iSequenceState==QUIT)
		{
			m_pActEmail=m_lpEmail->RemoveHead();//Letzte Mail kann gelöscht werden
			WK_DELETE(m_pActEmail);
			WK_DELETE(m_pEmailContent);

			if(!(m_lpEmail->IsEmpty()))			//Es sind noch weitere Emails vorhanden
			{
				m_iSequenceState=HELO;
				m_pEmailClient->NextStep(NEXT_MAIL);//Anweisung an Client 
				return;
			}
			m_pEmailClient->NextStep(MAIL_SEND);
			return;
		}
		//Session mit Mailserver komplett beendet
		else if(m_iSequenceState==SESSION_CLOSED)
		{
			m_pEmailClient->NextStep(READY);
			return;
		}
		
		//Kommandozeile erzeugen und senden
		CommandLine=CookCommandLine(m_iSequenceState);
		WK_TRACE(_T("[CSMTP::OnServerReply]\n"));
		WK_TRACE(_T("Returned CommandLine : %s\n"),CommandLine);
		
		// SMTP want's to have ASCII
		CommandLine.ReleaseBuffer();
		CWK_String sTemp(CommandLine);
		LPCSTR szTemp = sTemp;
		m_pEmailClient->SendData((BYTE*)szTemp, strlen(szTemp));
		
	}
	else if(ReplyCode == 354)					//Start Mail ....
	{
		m_pEmailContent=m_pActEmail->GetMailContent(m_iActEmailLen);
		m_pEmailClient->SendData(m_pEmailContent,m_iActEmailLen);

		return;
		
	}
	else										//Fehler in der Protokollschicht
	{
		WK_TRACE(_T("[CSMTP::OnServerReply]\n"));
		WK_TRACE(_T("Error in SMTP Sequence :\n%s"),ReplyMessage);
		//Mail speichern, später erneut versuchen
		if(!m_pActEmail)
		{
			m_pActEmail=m_lpEmail->RemoveHead();//Mail entfernen und hinten anstellen
			m_lpEmail->AddTail(m_pActEmail);
		}
		m_pEmailClient->ParkEmail(m_pActEmail); //Mail parken
		m_iSequenceState=HELO;
		m_pEmailClient->NextStep  (SMTP_ERROR);
		return;
	}
	
	
}

//////////////////////////////////////////////////////////////////////////////////////////////
//Reply Code aus Serverantwort ermitteln
UINT CSMTP::GetServerReplyCode(CString ServerReply)
{
	return _ttoi(ServerReply.Left(3));
}


//////////////////////////////////////////////////////////////////////////////////////////////


void CSMTP::IncreaseSequenceState()
{
	int i;
	
	i=(int)m_iSequenceState;
	i++;
	m_iSequenceState=(SequenceState)i;
}

//////////////////////////////////////////////////////////////////////////////////////////////


CString CSMTP::CookCommandLine(enum CSMTP::SequenceState Sequence)
{
	CString CommandLine;
	
	switch(Sequence)			
	{										
	case HELO: 
		{
			CString sServer = m_pEmailClient->GetRemoteIP();
			if (sServer.IsEmpty())
			{
				CommandLine.Format(_T("HELO\r\n"));
			}
			else
			{
				CommandLine.Format(_T("HELO %s\r\n"),sServer);
			}
		}
		break;
	case MAIL:	
		CommandLine.Format(_T("MAIL FROM: %s\r\n"),m_pActEmail->GetSenderAddress());
		break;
	case RCPT: 
		CommandLine.Format(_T("RCPT TO: <%s>\r\n"),m_sNextEmailReceiver);
		break;
	case DATA:	
		CommandLine.Format(_T("DATA\r\n"));
		break;
	case QUIT:  
		CommandLine.Format(_T("QUIT\r\n"));
		break;
		
	}
	
	return CommandLine;
}


//////////////////////////////////////////////////////////////////////////////////////////////


void CSMTP::CloseTransaction()
{
	CWK_String sCommandLine = CookCommandLine(QUIT);
	LPCSTR szCommandLine = sCommandLine;
	m_pEmailClient->SendData((BYTE*)szCommandLine, strlen(szCommandLine));
}

/////////////////////////////////////////////////////////////////////////////////////////////



BOOL CSMTP::ConnectedToMailserver()
{
	return m_bConnected;
}



void CSMTP::ResetSequence()
{
	m_iSequenceState = CONNECTION_ESTABLISHMENT;
}


void CSMTP::SetConnectStatus(BOOL bStatus)
{
	m_bConnected=bStatus;
}
