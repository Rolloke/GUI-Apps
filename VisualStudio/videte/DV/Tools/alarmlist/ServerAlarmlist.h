// Server.h: interface for the CServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVER_H__AB30DDE4_9A2C_4810_981C_E68E3308291C__INCLUDED_)
#define AFX_SERVER_H__AB30DDE4_9A2C_4810_981C_E68E3308291C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CIPCDatabaseClientAlarmlist;
class CAlarmlistDlg;
class CServerAlarmlist  
{
public:
	// construction / destruction
	CServerAlarmlist(CAlarmlistDlg* pDlg);
	virtual ~CServerAlarmlist();

	BOOL Connect(const CString& sHost, const CString& sPin);
	void OnIdle();

	CIPCDatabaseClientAlarmlist*	GetDatabase(){return m_pDatabase;}
	CAlarmlistDlg* GetAlarmlistDlg() {return m_pAlarmlistDlg;}

	BOOL IsDatabaseConnected();
	BOOL IsDatabaseDisconnected();

protected:
	void Lock();
	void Unlock();

	BOOL Reconnect();
	void ConnectDatabase();
	void DisconnectDatabase();


	
	static UINT ConnectThreadProc(LPVOID pParam);

private:
	// sync objects
	CCriticalSection				m_cs;
	CConnectionRecord*				m_pConnectionRecord;	// for connect thread
	CIPCFetchResult*				m_pDatabaseResult;
	CWinThread*						m_pConnectThread;
	BOOL							m_bConnectThreadRunning;
	CString							m_sHost;
	CString							m_sEnteredPIN;
	DWORD							m_dwReconnectTick;
	CIPCDatabaseClientAlarmlist*	m_pDatabase;
	CAlarmlistDlg*					m_pAlarmlistDlg;
};

#endif // !defined(AFX_SERVER_H__AB30DDE4_9A2C_4810_981C_E68E3308291C__INCLUDED_)
