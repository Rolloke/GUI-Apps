// Email.cpp: implementation of the CEmail class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EmailUnit.h"
#include "Email.h"
#include "MIME.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEmail::CEmail(BOOL mime)
{
	m_bMime         =mime;
	m_sSenderAddress="Anonymous";
	m_iEmailLen     =0;
	m_pMailContent  =new CMemFile(1);
	m_iAttemptToSend=1;
	m_nRefCnt = 0;
}

CEmail::~CEmail()
{
	WK_DELETE(m_pMailContent);
}

int CEmail::Release()
{
	int nRefCount = m_nRefCnt--;
	if (m_nRefCnt == 0)
	{
		delete this;
	}
	return nRefCount;
}

//////////////////////////////////////////////////////////////////////////////////////

CString CEmail::GetSenderAddress()
{
	return m_sSenderAddress;
}

//////////////////////////////////////////////////////////////////////////////////////
void CEmail::AddContentToMail(BYTE *pContent, int iLen, CString sContentType, CString sSubType,CString sEncoding,CString sContentName)
{
	// Inhalt der Email dynamisch erweitern
	// Content Types, SubTypes siehe constants.h
	
	if(m_bMime)	//um MIME Protokoll erweitern
	{	
		m_MimeMessage.SetContentType(sContentType);
		m_MimeMessage.SetSubtype(sSubType);
		m_MimeMessage.SetTransferEncoding(sEncoding);
		m_MimeMessage.SetContentName(sContentName);
		pContent = m_MimeMessage.CookMIMEMessage(pContent,iLen);
	}
	
	m_pMailContent->SeekToEnd();
	m_iEmailLen=m_iEmailLen+iLen;		//Gesamt Emaillänge entsprechend erhöhen
	m_pMailContent->Write(pContent,iLen);

	if(m_bMime)	//um MIME Protokoll erweitern
	{
		WK_DELETE(pContent);
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CEmail::SetReceiver(CString sReceiver)
{
	int iEPos=0;
	m_sReceiver=sReceiver;
	BOOL bSearchFinished=FALSE;

	if(sReceiver.Find(_T(";"))!=-1)			//Es sind mehrere Empfänger enthalten
	{									//rm@de;ra@de
	   iEPos=sReceiver.Find(_T(";"));		//erstes Begrenzungszeichen
		do								//iEPos=5;
		{
			m_sReceiver=sReceiver.Left(iEPos);			 //= rm@de
			if(m_sReceiver!=_T(""))
			{
				m_lsEmailReceiver.AddTail(m_sReceiver);
				WK_TRACE(_T("[CEmail::SetReceiver] Add RECPT : %s\n"),m_sReceiver);
			}
			if(iEPos!=sReceiver.GetLength())
			{
				sReceiver=sReceiver.Mid(iEPos+1);     		 //= ra@de
			}
			else
			{
				sReceiver=_T("");
			}
			iEPos=sReceiver.Find(_T(";"));			 //=-1
			if(iEPos==-1 && sReceiver.GetLength()>1)	 //= TRUE
			{
				iEPos=sReceiver.GetLength();			 //= 5
				//iSPos=0;
			}
			else if(iEPos==-1 && sReceiver.GetLength()<=1) // nur noch ;
			{
			   bSearchFinished=TRUE;
			}

		}while(!bSearchFinished);

	}
	else
	{
		m_lsEmailReceiver.AddTail(sReceiver);
	}
}
//////////////////////////////////////////////////////////////////////////////////////
BYTE* CEmail::GetMailContent(UINT &len)
{
	if(m_bMime)
	{
		CWK_String sHeader(m_MimeMessage.GetHeader());
		LPCSTR szMIMEHeader = sHeader;	//MIME sMIMEHeader anfordern
		m_pMailContent->SeekToBegin();
		m_iEmailLen = m_iEmailLen + strlen(szMIMEHeader);
		//CMemFile zwischenspeichern
		int	  nMailContent = (int)m_pMailContent->GetLength();
		BYTE *pMailContent = m_pMailContent->Detach();
		// CMemFile zurücksetzen
		m_pMailContent->Flush();
		m_pMailContent->SeekToBegin();
		//MIMEsMIMEHeader ins CMemFile
		m_pMailContent->Write(szMIMEHeader, strlen(szMIMEHeader));
		//Zwischenspeicher ins CMemFile
		m_pMailContent->Write(pMailContent, nMailContent);
		WK_DELETE(pMailContent);
		//Email Endekennung anfordern
		CWK_String sHelpMem;
		LPCSTR szHelpMem;
		sHelpMem = m_MimeMessage.GetEndBoundary();
		szHelpMem = sHelpMem;
		m_iEmailLen = m_iEmailLen + strlen(szHelpMem);
		//Ans CMemFile anhängen			
		m_pMailContent->Write(szHelpMem, strlen(szHelpMem));
		
		szHelpMem = "\r\n.\r\n";
		m_iEmailLen=m_iEmailLen + strlen(szHelpMem);
		m_pMailContent->Write(szHelpMem, strlen(szHelpMem));
	}
	
	len = m_iEmailLen;
	m_pMailContent->SeekToBegin();
	
	return m_pMailContent->Detach();
}
//////////////////////////////////////////////////////////////////////////////////////
void CEmail::SetSender(CString sender)
{
	m_sSenderAddress=sender;
	if(m_bMime)
	{
		m_MimeMessage.SetFrom(sender);
	}
	
}
//////////////////////////////////////////////////////////////////////////////////////
void CEmail::SetSubject(CString subject)
{
	m_MimeMessage.SetSubject(subject);
	AddMessageToMail();
}
//////////////////////////////////////////////////////////////////////////////////////
void CEmail::SetCharSet(CString sCharSet)
{
	m_MimeMessage.SetCharSet(sCharSet);
}
//////////////////////////////////////////////////////////////////////////////////////
void CEmail::SetMessage(CString sMessage)
{
	m_sMessage = sMessage;
}
//////////////////////////////////////////////////////////////////////////////////////
void CEmail::AddMessageToMail()
{
	int i, nLen = m_sMessage.GetLength();
	for (i=0; i<nLen; i++)
	{
		if (((WORD)m_sMessage.GetAt(i))>255)
		{
			break;
		}
	}

	if (i < nLen)
	{
		CString sCharSet = m_MimeMessage.GetCharSet();
		CString sSubject = m_MimeMessage.GetSubject();
		CWK_String sMessage;
		sCharSet.Replace(_T("\""), _T(""));
		sMessage.Format(_T("<HTML>\r\n<HEAD>\r\n<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html%s\"><META NAME=\"Generator\" CONTENT=\"videte EmailUnit\">\r\n\r\n<TITLE>%s</TITLE>\r\n</HEAD>\r\n<BODY>\r\n<FONT SIZE=2><b>%s</b><P>%s<BR></P></FONT>\r\n</BODY>\r\n</HTML>"), sCharSet, sSubject, sSubject, m_sMessage);
		LPCSTR szMessage = sMessage;
		AddContentToMail((BYTE*)szMessage, strlen(szMessage), _T("text"), _T("html"), _T("8bit"));
	}
	else
	{
		CWK_String sMessage(m_sMessage);
		LPCSTR szMessage = sMessage;
		AddContentToMail((BYTE*)szMessage, strlen(szMessage), _T("text"), _T("plain"), _T("quoted-printable"));
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CEmail::IncreaseAttemptToSend()
{
	m_iAttemptToSend++;
}

//////////////////////////////////////////////////////////////////////////////////////

UINT CEmail::GetAttemptToSend()
{
	return m_iAttemptToSend;
}

//////////////////////////////////////////////////////////////////////////////////////

CString CEmail::GetReceiver()
{
	return m_sReceiver;
}

//////////////////////////////////////////////////////////////////////////////////////

CList<CString,CString>* CEmail::GetReceiverList()	
{
   return &m_lsEmailReceiver;
}

//////////////////////////////////////////////////////////////////////////////////////

void CEmail::SetTo(CString sReceiver)
{
   	m_MimeMessage.SetTo(sReceiver);
}
