// Email.cpp : implementation file
//

#include "stdafx.h"
#include "Cemail.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
CString GetErrorString(LONG uOsError)
{
	CString sRet;
	LPVOID lpMsgBuf;
	
	FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		uOsError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL );
	
	sRet = (LPCSTR)lpMsgBuf;
	
	// Free the buffer.
	LocalFree( lpMsgBuf );
	
	return sRet;
}
/////////////////////////////////////////////////////////////////////////////
// CEmail

CEmail::CEmail()
{
	m_iError	     = 0;
	m_iSequenceState = 0;
	m_iEmailLen      = 0;
	
	m_pbEmailContent = NULL;
	m_pReceiveThread = NULL;
	
	
	m_hSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	
	if(!m_hSocket)
	{
		OnMailError();
	}
	InitSocketData();
}

CEmail::~CEmail()
{
	if (m_pReceiveThread)
	{
		delete m_pReceiveThread;
	}
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CEmail, CSocket)
//{{AFX_MSG_MAP(CEmail)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0


/////////////////////////////////////////////////////////////////////////////
// CEmail member functions

void CEmail::ConnectToSMTPServer(CString sServer, WORD wPort)
{
	
	int iErr;
	struct sockaddr_in Addr;
	
	unsigned long lIP = inet_addr(sServer);
	
	CString sIP;
	
	if(lIP==INADDR_NONE) //keine gültige IP adresse
	{
		
		char *cp;
		BYTE b1,b2,b3,b4;
		
		struct hostent *phost = gethostbyname(sServer);
		
		if(!phost)
		{
			OnMailError();
		}
		
		cp = phost->h_addr_list[0];
		
		b1 = *(BYTE*)cp;
		b2 = *((BYTE*)(++cp));
		b3 = *((BYTE*)(++cp));
		b4 = *((BYTE*)(++cp));
		
		sIP.Format("%i.%i.%i.%i",b1,b2,b3,b4);
		
		lIP = inet_addr(sIP);
		Addr.sin_addr.S_un.S_addr = lIP;
		Addr.sin_port = htons(wPort);
		Addr.sin_family=AF_INET;
	}
	
	
	iErr = connect(m_hSocket,                          
		(struct sockaddr*)&Addr,
		sizeof(struct sockaddr)                        
		);
	
	
	if(iErr == SOCKET_ERROR)
	{
		OnMailError();
	}
}

/////////////////////////////////////////////////////////////////////////////

void CEmail::OnMailError()
{
	m_iError = WSAGetLastError();
	printf("mail error %s\n",GetErrorString(m_iError));
}

/////////////////////////////////////////////////////////////////////////////

BOOL CEmail::SendMail(LPCSTR sServer, LPCSTR psEmailSender, LPCSTR psEmailReceiver, LPCSTR psSubject, LPCSTR psBODY, WORD wServerPort/*=25*/)
{
	
	
	m_sSender   = psEmailSender;
	m_sReceiver = psEmailReceiver;
	
	m_pbEmailContent = CookEmail(psEmailSender,	psEmailReceiver, psSubject, psBODY,	m_iEmailLen);
	
	ConnectToSMTPServer(sServer,wServerPort);
	if (m_iError)
	{
		return FALSE;
	}
	
	m_pReceiveThread = AfxBeginThread(ReceiveThreadFunc,this,THREAD_PRIORITY_NORMAL);
	
	DWORD dwRet = WaitForSingleObject(m_pReceiveThread->m_hThread,10000);
	if(dwRet == WAIT_TIMEOUT)
	{
		StopReceiving();
		return FALSE;
	}
	
	m_pReceiveThread = NULL;
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

BYTE* CEmail::CookEmail(LPCSTR psEmailSender, LPCSTR psEmailReceiver, LPCSTR psSubject, LPCSTR psBody, int &iBufferLen)
{
	
	CString  sEmailBody, sEmailHeader, sEmail ,sBody(psBody);
	
	BYTE* pbContentBuffer;

	if (   -1 != sBody.Find("<html>")
		|| -1 != sBody.Find("<HTML>")
		)
	{
		// seems to be a html body
		sEmailHeader.Format("From: %s\r\nTo: %s\r\nSubject: %s\r\nMIME-Version: 1.0\r\nContent-type: text/html;\r\n\r\n", psEmailSender,	psEmailReceiver, psSubject);
	}
	else
	{
		sEmailHeader.Format("From: %s\r\nTo: %s\r\nSubject: %s\r\nMIME-Version: 1.0\r\nContent-type: text/plain;\r\n\r\n", psEmailSender,	psEmailReceiver, psSubject);
	}

	sEmailBody.Format("%s\r\n.\r\n",psBody);
	
	sEmail = sEmailHeader + sEmailBody;
	
	pbContentBuffer = new BYTE[sEmail.GetLength()];
	
	CopyMemory(pbContentBuffer,(BYTE*)(const char*)sEmail,sEmail.GetLength());

	iBufferLen = sEmail.GetLength();
	
	return pbContentBuffer;
	
}

/////////////////////////////////////////////////////////////////////////////

void CEmail::StartReceive() 
{
	
}

/////////////////////////////////////////////////////////////////////////////

void CEmail::InitSocketData()
{
	int iErr,
		iSize = sizeof(unsigned int);
	
	
	iErr=
		getsockopt(m_hSocket,
		SOL_SOCKET,
		SO_RCVBUF,
		(char*)&m_iRecvBufferSize,
		&iSize);
	
	if(iErr == SOCKET_ERROR)
	{
		OnMailError();
	}
}

/////////////////////////////////////////////////////////////////////////////

BOOL CEmail::WorkOutServerReply(char* psServerReply)
{
	
	CString sServerReply = psServerReply;
	delete psServerReply;
	TRACE("RECEIVE <%s>\n",sServerReply.Left(sServerReply.GetLength()-1));
	
	int ReplyCode = atoi(sServerReply.Left(3));
	
	CString sCommandLine;
	
	
	
	if(ReplyCode < 300)							//CommandLine fehlerfrei vom Server verarbeitet
	{											//Nächsten Schritt in der Sequence einleiten
		//HELO->MAIL->RECPT->...->QUIT
		m_iSequenceState++;
		
		if(m_iSequenceState==QUIT)
		{
			delete m_pbEmailContent;
			m_pbEmailContent = NULL;
		}
		else if(m_iSequenceState==SESSION_CLOSED)
		{
			m_iSequenceState = CONNECTION_ESTABLISHMENT;
			return FALSE;
		}
		
								//Kommandozeile erzeugen und senden
		sCommandLine=CookSMTPCommandLine(m_iSequenceState);
		Send((BYTE*)(const char*)sCommandLine,sCommandLine.GetLength());
		
	}
	else if(ReplyCode == 354)					//Start senden EMail ....
	{
		return Send(m_pbEmailContent, m_iEmailLen);
	}
	else										//Fehler in der Protokollschicht
	{
		m_iSequenceState=CONNECTION_ESTABLISHMENT;
		m_iError=1;
		return FALSE;
	}
	
	return TRUE; 
}

/////////////////////////////////////////////////////////////////////////////

CString CEmail::CookSMTPCommandLine(int iSequencestate)
{
	CString sCommandLine;
	
	switch(iSequencestate)			
	{
		
	case HELO: 
		sCommandLine.Format("HELO\r\n");
		break;
	case MAIL:	
		sCommandLine.Format("MAIL FROM: %s\r\n",m_sSender);
		break;
	case RCPT: 
		sCommandLine.Format("RCPT TO: <%s>\r\n",m_sReceiver);
		break;
	case DATA:	
		sCommandLine.Format("DATA\r\n");
		break;
	case QUIT:  
		sCommandLine.Format("QUIT\r\n");
		break;
	}
	
	return sCommandLine;
	
}

/////////////////////////////////////////////////////////////////////////////

UINT CEmail::ReceiveThreadFunc(LPVOID ThreadObject)
{
	
   	CEmail *pEmail = (CEmail*)ThreadObject;
	
	int  iErr ,iSize;
//	int  iBufferSize  = 0;
//	BYTE *pRecvBuffer = NULL;
	char *cbuf = NULL;
	
	struct timeval timeout;
	
	timeout.tv_sec=0;
	timeout.tv_usec=0;
	fd_set sockset;
	
	while(1)
	{
		
		DWORD dwRet = WaitForSingleObject(pEmail->m_eStopReceiveThread,10);
		if(dwRet == WAIT_OBJECT_0)
		{
			
			return 0;
		}
		FD_ZERO(&sockset); 
		FD_SET(pEmail->m_hSocket, &sockset);
		
		if(select(0,&sockset,NULL,NULL,&timeout))
		{
	
			if(pEmail->m_hSocket)
			{
				
				iSize = pEmail->m_iRecvBufferSize;
				
				cbuf=new char[iSize+1];
				
				iErr = recv(pEmail->m_hSocket,cbuf,	iSize,0);
				
			}
			else
			{
				pEmail->OnMailError();
				delete cbuf;
				return 0;
			}
			
			if(iErr == SOCKET_ERROR)
			{
				delete cbuf;
				pEmail->OnMailError();
				return 0;
			}
			else if(iErr == 0)
			{
				delete cbuf;
				pEmail->OnMailError();
				return 0;
			}
			else
			{
				//Alles Ok
				if(iErr>0)
				{
					cbuf[iErr] = '\0';
					if(!pEmail->WorkOutServerReply(cbuf))
					{
						
						return 0;
					}
					
				}
			}
		}
		
		Sleep(10);
	}
	
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////

BOOL CEmail::Send(BYTE *pBuffer, int iBufferLen)
{
	
	int iErr;

	CString s((LPCTSTR)pBuffer,iBufferLen-1);
	TRACE("SEND <%s>\n",s);
	iErr = send(m_hSocket,(char*)pBuffer,iBufferLen,0);
	
	if(iErr != SOCKET_ERROR)
	{
		return TRUE;	
	}
	else
	{
		OnMailError();
		return FALSE;
	}
	
}

/////////////////////////////////////////////////////////////////////////////

void CEmail::StopReceiving()
{
	m_eStopReceiveThread.SetEvent();
//	DWORD dwRet = 
	WaitForSingleObject(m_pReceiveThread->m_hThread,5000);
}
