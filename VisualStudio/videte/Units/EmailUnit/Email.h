// Email.h: interface for the CEmail class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EMAIL_H__662471C2_0EB0_11D4_A9E6_004005A26A3B__INCLUDED_)
#define AFX_EMAIL_H__662471C2_0EB0_11D4_A9E6_004005A26A3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "MIME.h"

class CEmail  
{

public:

	CEmail(BOOL mime=TRUE);
	virtual ~CEmail();
	int	AddRef() { return ++m_nRefCnt; };
	int Release();

	UINT  GetAttemptToSend();
	BYTE* GetMailContent(UINT &len);
	void AddMessageToMail();

	void SetReceiver(CString email);
	void AddContentToMail(BYTE *pcontent, int ilen, CString sContentType="message", CString sSubType="rfc822",CString sEncoding="7bit",CString sContentName="");
	void SetSender(CString sender);
	void SetTo(CString sReceiver);
	void IncreaseAttemptToSend();
	void SetSubject(CString subject);
	void SetCharSet(CString sCharSet);
	void SetMessage(CString sMessage);

	CString GetSenderAddress();
	CString GetReceiver();

	CList<CString,CString>* GetReceiverList();

private:

	CList<CString,CString> m_lsEmailReceiver;
	
	CString   m_sReceiver;

	UINT      m_iAttemptToSend;
	UINT      m_iEmailLen;

	CMIME     m_MimeMessage;
	BOOL      m_bMime;
	BYTE*     m_MailBuffer;
	CMemFile* m_pMailContent;
	CString	  m_sMessage;

	CString   m_sSenderAddress;
	int m_nRefCnt;
	
};

#endif // !defined(AFX_EMAIL_H__662471C2_0EB0_11D4_A9E6_004005A26A3B__INCLUDED_)
