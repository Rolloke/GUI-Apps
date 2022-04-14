// DialUpNetwork.cpp: implementation of the CDialUpNetwork class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "emailunit.h"
#include "DialUpNetwork.h"
#include "Profiler.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern fpRasGetEntryDialParams MyRasGetEntryDialParams;
extern fpRasDial MyRasDial;
extern fpRasGetConnectStatus MyRasGetConnectStatus;
extern fpRasHangUp MyRasHangUp;
extern fpRasGetEntryProperties MyRasGetEntryProperties;
extern fpRasEnumConnections MyRasEnumConnections;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDialUpNetwork::CDialUpNetwork(CEmailUnitDlg* pdlg)

{
	
	m_hRasConn=NULL;
	m_pDlg=pdlg;
	
}

CDialUpNetwork::~CDialUpNetwork()
{
	if(m_hRasConn)
	{
		HangUp();
	}
}


void CDialUpNetwork::GetDialParameter(CString sPhoneBookEntryName)
{
	m_stRasDialParameter.dwSize = sizeof(RASDIALPARAMS);
	memcpy(m_stRasDialParameter.szEntryName, sPhoneBookEntryName, (sPhoneBookEntryName.GetLength()+1)*sizeof(TCHAR));
	
	CProfiler profile;
	CString sUser = profile.GetUser();
	memcpy(m_stRasDialParameter.szUserName, sUser, (sUser.GetLength()+1)*sizeof(TCHAR));
	CString sPassword = profile.GetPassword();
	memcpy(m_stRasDialParameter.szPassword, sPassword, (sPassword.GetLength()+1)*sizeof(TCHAR));

	BOOL lpfPassword=FALSE ;
	if (!MyRasGetEntryDialParams(
								NULL,				// pointer to the full path and filename
															// of the phone-book file
								&m_stRasDialParameter,
															// pointer to a structure that receives
															// the connection parameters
								&lpfPassword				// indicates whether the user's password
															// was retrieved
								)
		)
	{
//		(CString)m_stRasDialParameter.szUserName = profile.GetUser();
		DWORD lpdwEntryInfoSize = 0;
		RASENTRY stRasEntry;

		 stRasEntry.dwSize=sizeof(RASENTRY);

		 MyRasGetEntryProperties(
								NULL, 
								m_stRasDialParameter.szEntryName,
								NULL,
								&lpdwEntryInfoSize,
                     			NULL,
       							NULL 
								);
		
		 MyRasGetEntryProperties(
								NULL,								
								m_stRasDialParameter.szEntryName,   
								&stRasEntry,						
								&lpdwEntryInfoSize,					
								NULL,								
								NULL
                              );

		 CString sTelNummer = (CString)stRasEntry.szAreaCode + stRasEntry.szLocalPhoneNumber;
		 memcpy(m_stRasDialParameter.szPhoneNumber, sTelNummer, (sTelNummer.GetLength()+1)*sizeof(TCHAR));
	}
	else 
	{
		WK_TRACE(_T("[CDialUpNetwork::GetDialParameter]\n"));
		WK_TRACE(_T("Error in RasGetEntryDialParams\n"));
//		(CString)m_stRasDialParameter.szUserName = profile.GetUser();
//		(CString)m_stRasDialParameter.szPassword = profile.GetPassword();
	}
}

BOOL CDialUpNetwork::DialUp(CString sPhonebookEntry)
{
	LPCTSTR lpszPhonebook = NULL;
	DWORD dwNotifierType  = 0xFFFFFFFF;
	GetDialParameter(sPhonebookEntry);
	

	DWORD dwRet;    
	RASCONNSTATUS rStatus;
	ZeroMemory(&rStatus, sizeof(RASCONNSTATUS));
	rStatus.dwSize = sizeof(RASCONNSTATUS);
	dwRet = MyRasGetConnectStatus(m_hRasConn, &rStatus);

	//EINE Verbindung besteht bereits
	if (dwRet != ERROR_INVALID_HANDLE)    
	{
		return TRUE;
	}
	if(!MyRasDial(NULL,lpszPhonebook,&m_stRasDialParameter,dwNotifierType,   
		m_pDlg->m_hWnd,&m_hRasConn))
	{
		return TRUE;
	}
	else 
	{	
		m_pDlg->ChangeIcon(RED);
		WK_TRACE(_T("[CDialUpNetwork::DialUp]\n"));
		WK_TRACE(_T("Error dialing up\n"));
		return FALSE;
	}
	
}

UINT CDialUpNetwork::GetConnectStatus()
{
	RASCONNSTATUS rasconnstatus;
	DWORD dwError;
	rasconnstatus.dwSize=sizeof(RASCONNSTATUS);
	
	dwError=MyRasGetConnectStatus(
				m_hRasConn,		 // handle to RAS connection of interest
				&rasconnstatus   // buffer to receive status data
				);
	if(dwError==0)
	{

		
		if(rasconnstatus.rasconnstate==RASCS_Connected)
		{
			WK_TRACE(_T("[CDialUpNetwork::GetConnectStatus]\n"));
			WK_TRACE(_T("DFÜ Connection established\n"));
			return 1;//Dialing ready
		}
		else if(rasconnstatus.rasconnstate==RASCS_Disconnected)
		{	
			m_pDlg->ChangeIcon(RED);
			WK_TRACE(_T("[CDialUpNetwork::GetConnectStatus]\n"));
			WK_TRACE(_T("DFÜ Disconnected by Error\n"));
			return 3;//Dial Up Quit
		}
		else if(rasconnstatus.dwError!=0)
		{
			WK_TRACE(_T("[CDialUpNetwork::GetConnectStatus] Error\n"));
			return 2;//Dialing Error
		}
		else
		{
			return 0;//Dialing Process
		}
	}
	else if(dwError==6)
	{
		return 0;
	}
	else
	{
//		DWORD dwError = GetLastError();
		TRACE(_T("Error RasGetConnectStatus %lu\n"), dwError);
		return 4;//function error
	}
	
}


BOOL CDialUpNetwork::HangUp()
{
	
	DWORD dwRet;    
	RASCONNSTATUS rStatus;
	ZeroMemory(&rStatus, sizeof(RASCONNSTATUS));
	rStatus.dwSize = sizeof(RASCONNSTATUS);
	dwRet = MyRasGetConnectStatus(m_hRasConn, &rStatus);
	if (dwRet != ERROR_INVALID_HANDLE)    
	{
		MyRasHangUp(m_hRasConn);  // hRasConn - valid handle to the RAS connection
		MSG msg;
		while (dwRet != ERROR_INVALID_HANDLE)        
		{
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))            
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}        
			dwRet = MyRasGetConnectStatus(m_hRasConn, &rStatus);
		}        
		WK_TRACE(_T("[CDialUpNetwork::HangUp]\n"));
		WK_TRACE(_T("DFÜ Hanging UP ok\n"));
		m_hRasConn=NULL;
	}
	else
	{
		
		WK_TRACE(_T("[CDialUpNetwork::HangUp]\n"));
		WK_TRACE(_T("No Handle to Dial up Network\n"));
		m_hRasConn=NULL;
	}

	Sleep(1000);		//Nur zur Sicherheit;
	return TRUE;
	
	
}
