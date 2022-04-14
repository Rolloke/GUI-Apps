#if !defined(AFX_CONNECTION_H__662471C3_0EB0_11D4_A9E6_004005A26A3B__INCLUDED_)
#define AFX_CONNECTION_H__662471C3_0EB0_11D4_A9E6_004005A26A3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Connection.h : header file
//
#include "smtp.h"
#include "POP.h"


class CSMTP;
/////////////////////////////////////////////////////////////////////////////
// CConnection command target

class CConnection : public CSocket
{

public:
	CConnection(CSMTP *SMTPLayer=NULL, CPOP *POPLayer=NULL);
	virtual ~CConnection();

public:

	BOOL SendObjectToClient(BYTE* bObject, int iLen);
	CString GetHostname();

	CMemFile m_mObjectToSend;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConnection)
	public:
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CConnection)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation

private:

	void InitSocketData();

	CPOP*   m_pPOPLayer;
	CSMTP*  m_pSMTPLayer;
	char    m_cReceiveBuffer[RECEIVE_BUFFER_SIZE];
	CString m_sMyHostname;
	BOOL    m_bEmailSend;
	int     m_iObjectLen;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONNECTION_H__662471C3_0EB0_11D4_A9E6_004005A26A3B__INCLUDED_)
