// Connection.cpp : implementation file
//

#include "stdafx.h"
#include "EmailUnit.h"
#include "Connection.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConnection

CConnection::CConnection(CSMTP *SMTPLayer,CPOP *POPLayer)
{
	m_pSMTPLayer = SMTPLayer;
	m_pPOPLayer  = POPLayer;
	m_bEmailSend = FALSE;
	m_iObjectLen = 0;

	InitSocketData();
	
}

CConnection::~CConnection()
{
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CConnection, CSocket)
//{{AFX_MSG_MAP(CConnection)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CConnection member functions


void CConnection::OnReceive(int nErrorCode) 
{
	
	m_cReceiveBuffer[Receive(m_cReceiveBuffer,RECEIVE_BUFFER_SIZE)]='\0';
	
	//Server Reply an Protokollschicht
	if(m_pSMTPLayer)
		m_pSMTPLayer->OnServerReply(m_cReceiveBuffer);

	else if(m_pPOPLayer)
		m_pPOPLayer->OnServerReply(m_cReceiveBuffer);
	
}

void CConnection::InitSocketData()
{
	const int iNameLen = 80;
	char szHostname[iNameLen];
	
	if((gethostname(szHostname, iNameLen)))
	{
		WK_TRACE(_T("Error getting hostname\n"));
	}
	else
	{
		m_sMyHostname = szHostname;
	}
	
}

CString CConnection::GetHostname()
{
	return m_sMyHostname;
	
}




BOOL CConnection::SendObjectToClient(BYTE *bObject, int iLen)
{
	m_mObjectToSend.Write(bObject,iLen);
	m_iObjectLen=iLen;
	int iBytes;
	BYTE *pObjectData=m_mObjectToSend.Detach();

	
	iBytes=Send(pObjectData,m_iObjectLen);


//	iBytes=Send(bObject,iLen);

	if(iBytes==m_iObjectLen)
	{
		WK_DELETE(pObjectData);
//		WK_DELETE(bObject);
		if(m_bEmailSend)//Das war das Ende der Übertragung
		{								 	
			AfxGetMainWnd( )->PostMessage(WM_MAILSEND, (UINT)0    ,(BOOL)TRUE);
		}							   //(...        , 0=LAN,1=DFÜ, TRUE=Mailübertragung erfolgreich abgeschlossen)
	}
	else
	{									//Fehler während der Mailübertragung
		WK_TRACE(_T("Fehler während der EMail Übertragung\n"));
		AfxGetMainWnd( )->PostMessage(WM_MAILSEND,(UINT)0,(BOOL)FALSE);
	}


	return 0;
}

void CConnection::OnClose(int nErrorCode) 
{
	WK_TRACE(_T("[CConnection::OnClose]\n"));
	if(m_pSMTPLayer)
	{
		AfxGetMainWnd( )->PostMessage(WM_MAILSEND,(UINT)0,(BOOL)FALSE);
		m_pSMTPLayer->SetConnectStatus(FALSE);
	}
	delete(this);
}

