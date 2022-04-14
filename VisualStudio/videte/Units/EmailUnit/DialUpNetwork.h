// DialUpNetwork.h: interface for the CDialUpNetwork class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALUPNETWORK_H__93E58C11_1433_11D4_A9F8_004005A26A3B__INCLUDED_)
#define AFX_DIALUPNETWORK_H__93E58C11_1433_11D4_A9F8_004005A26A3B__INCLUDED_
#include "EmailUnitDlg.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


typedef DWORD(CALLBACK *fpRasGetEntryDialParams)(LPCTSTR,LPRASDIALPARAMS,LPBOOL);
typedef DWORD(CALLBACK *fpRasDial)(LPRASDIALEXTENSIONS,LPCTSTR,LPRASDIALPARAMS,DWORD,LPVOID,LPHRASCONN);   
typedef DWORD(CALLBACK *fpRasGetConnectStatus)(HRASCONN,LPRASCONNSTATUS);
typedef DWORD(CALLBACK *fpRasHangUp)(HRASCONN); 
typedef DWORD(CALLBACK *fpRasGetEntryProperties)(LPCTSTR,LPCTSTR,LPRASENTRY,LPDWORD,LPBYTE,LPDWORD);
typedef DWORD(CALLBACK *fpRasEnumConnections)(LPRASCONN,LPDWORD,LPDWORD);
   
class CEmailUnitDlg;

class CDialUpNetwork
{

public:

	CDialUpNetwork(CEmailUnitDlg* pdlg);
	virtual ~CDialUpNetwork();

	BOOL HangUp();
	UINT GetConnectStatus();
	BOOL DialUp(CString sPhonebookEntry);

private:

	void GetDialParameter(CString sPhoneBookEntry);

	CWinThread*    m_pThread;
	CEmailUnitDlg* m_pDlg;
	HRASCONN       m_hRasConn;
	RASDIALPARAMS  m_stRasDialParameter;
};

#endif // !defined(AFX_DIALUPNETWORK_H__93E58C11_1433_11D4_A9F8_004005A26A3B__INCLUDED_)
