#if !defined(AFX_EMAIL_H__99F90FDF_D0E2_11D4_86C4_004005A26A3B__INCLUDED_)
#define AFX_EMAIL_H__99F90FDF_D0E2_11D4_86C4_004005A26A3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Email.h : header file
//

#define	CONNECTION_ESTABLISHMENT  0
#define	HELO					  1
#define	MAIL				      2
#define	RCPT					  3
#define	DATA					  4
#define	QUIT					  5
#define	SESSION_CLOSED			  6

#include <afxmt.h>


/////////////////////////////////////////////////////////////////////////////
// CEmail command target

class CEmail
{
	// construction/destruction
public:
	CEmail();
	virtual ~CEmail();

	// Operations
public:
	BOOL SendMail(LPCSTR sServer, 
		          LPCSTR psEmailSender, 
				  LPCSTR sEmailReceiver, 
				  LPCSTR psSubject, 
				  LPCSTR psBODY, 
				  int iServerPort=25);

	// implementation
protected:

	virtual void OnMailError();	

	// implementation
private:
	CString CookSMTPCommandLine(int iSequencestate);
	BOOL WorkOutServerReply(char* psServerReply);
	void InitSocketData();

	BYTE* CookEmail(LPCSTR psEmailSender, 
		            LPCSTR psEmailReceiver, 
		 		    LPCSTR psSubject, 
				    LPCSTR psBody,
					int &iBufferLen);

   	void ConnectToSMTPServer(CString sServer, int iPort=25);
	void StartReceive();
	void StopReceiving();

	static UINT ReceiveThreadFunc(LPVOID ThreadObject);
	BOOL Send(BYTE* pBuffer, int iBufferLen);



	// Attributes
private:

	CWinThread* m_pReceiveThread;
	CEvent m_eStopReceiveThread;

	SOCKET m_hSocket;
	int m_iError;
	int m_iEmailLen;
	int m_iSequenceState;
	int m_iRecvBufferSize;

	CString m_sReceiver;
	CString m_sSender;

	BYTE* m_pbEmailContent;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EMAIL_H__99F90FDF_D0E2_11D4_86C4_004005A26A3B__INCLUDED_)
