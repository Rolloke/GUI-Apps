// POP.h: interface for the CPOP class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POP_H__AADFB8C3_A020_11D4_8679_004005A26A3B__INCLUDED_)
#define AFX_POP_H__AADFB8C3_A020_11D4_8679_004005A26A3B__INCLUDED_

#include "EmailClient.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Profiler.h"
#include "EmailClient.h"

class CEmailClient;

class CPOP  
{
public:
	
	CPOP(CEmailClient *eClient);
	virtual ~CPOP();

	void OnServerReply(CString sReplyMessage);

private:

	BOOL GetDataFromReg();

	CString m_sPOPUser;
	CString m_sPOPPassword;
	CString m_sPOPServer;

	CEmailClient *m_pEmailClient;
	UINT m_uPOPPort;
	int m_iProtocolState;
};

#endif // !defined(AFX_POP_H__AADFB8C3_A020_11D4_8679_004005A26A3B__INCLUDED_)
