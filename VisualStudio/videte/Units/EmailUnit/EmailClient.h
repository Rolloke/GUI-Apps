// EmailClient.h: interface for the CEmailClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EMAILCLIENT_H__662471C4_0EB0_11D4_A9E6_004005A26A3B__INCLUDED_)
#define AFX_EMAILCLIENT_H__662471C4_0EB0_11D4_A9E6_004005A26A3B__INCLUDED_

#include "SMTP.h"	// Added by ClassView
#include "Email.h"
#include "DialUpNetwork.h"
#include "POP.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CSMTP;
class CEmailClient
{

public:
	
	CEmailClient(CEmailUnitDlg* pdlg);
	virtual ~CEmailClient();
	
	void SetConnectedToNetwork(BOOL bstatus);
	void DisconnectPOPServer();
	void SendData(BYTE* Data,int DataLen);
	void DisconnectMailServer();
	void DisconnectNetwork();
	void DeleteEmailList();
	void SetTimerIsSet(BOOL btimerSet);
	void ParkEmail(CEmail* pEmail=NULL);
	void NextStep (UINT MessageType);
	void SendMail();
	void Authenticated();

	CString GetRemoteIP();
	BOOL IsAuthenticated();
	BOOL ConnectToPOPServer();
	BOOL CheckRasAPI();

	BOOL ConnectToNetwork();
	BOOL ConnectToSMTPServer();

	CDialUpNetwork* GetPtrToDialUpNetwork();
    CEmail* CreateMail(BOOL Mime=TRUE);
	BOOL m_bConnectedToNetwork;

	CList<CEmail*,CEmail*> m_lEmail;

private:

	BOOL m_bConnectedToPOPServer;
	BOOL m_bAuthenticated;
	BOOL BuildRasFunctions(HINSTANCE hModule);
	BOOL m_bConnectedToMailServer;
	BOOL m_bConnectionIsLAN;
	BOOL m_bTimerIsSet;

	CConnection*    m_pConnection;
	CDialUpNetwork* m_pDialUpNetwork;
	CEmailUnitDlg*  m_pDlg;

	CEmail  *m_pEmail;
	CSMTP   *m_pSMTP;
	CPOP	*m_pPOP;

	int m_iTimerElapse;
};

#endif // !defined(AFX_EMAILCLIENT_H__662471C4_0EB0_11D4_A9E6_004005A26A3B__INCLUDED_)
