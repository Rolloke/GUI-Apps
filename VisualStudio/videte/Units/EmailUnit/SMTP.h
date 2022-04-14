// SMTP.h: interface for the CSMTP class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SMTP_H__662471C1_0EB0_11D4_A9E6_004005A26A3B__INCLUDED_)
#define AFX_SMTP_H__662471C1_0EB0_11D4_A9E6_004005A26A3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Connection.h"
#include "EmailClient.h"	// Added by ClassView
#include "Email.h"


class CEmailClient;
class CSMTP  
{




private:

	BOOL m_bConnected;

	UINT m_iActEmailLen;
	BYTE* m_pEmailContent;
	CEmail* m_pActEmail;
	CString m_sNextEmailReceiver;
	CEmailClient *m_pEmailClient;
	CList<CString,CString> *m_lsEmailReceiver; //Hier stehen die Email-Empfänger
	CList<CEmail*,CEmail*> *m_lpEmail;			
	UINT GetServerReplyCode(CString ServerReply);
	void IncreaseSequenceState();

	enum SequenceState {	//An dieser Sequenzstelle 
							//befindet sich die Kommunikation
							//gerade (RFC821, Seite 37)

		CONNECTION_ESTABLISHMENT = 0,
		HELO,
		MAIL,
		RCPT,
		DATA,
		//RSET,				//Diese Kommandos sind für iST nicht relevant
		//SEND,
		//SOML,
		//SAML,
		//VRFY,
		//EXPN,
		//HELP,
		//NOOP,
		QUIT,
		//TURN,
		SESSION_CLOSED
	}m_iSequenceState;

public:

	void SetConnectStatus(BOOL bStatus);
	void ResetSequence();
	void CloseTransaction();
	void OnServerReply(CString ReplyMessage);

	BOOL ConnectedToMailserver();
	CString CookCommandLine(enum CSMTP::SequenceState Sequence);

	CSMTP(CEmailClient* eClient,CList<CEmail*,CEmail*> *Email);
	virtual ~CSMTP();	

};

#endif // !defined(AFX_SMTP_H__662471C1_0EB0_11D4_A9E6_004005A26A3B__INCLUDED_)
