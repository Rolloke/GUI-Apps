// CIPCDatabaseSV.h: interface for the CIPCDatabaseSV class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CIPCDATABASESV_H__1DB2B833_B757_11D2_B56D_004005A19028__INCLUDED_)
#define AFX_CIPCDATABASESV_H__1DB2B833_B757_11D2_B56D_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRemoteThread;

class CIPCDatabaseSV : public CIPCDatabase  
{
public:
	CIPCDatabaseSV(CRemoteThread* pThread, LPCTSTR shmName
#ifdef _UNICODE
				,WORD wCodePage
#endif
		);
	virtual ~CIPCDatabaseSV();

public:
	virtual void OnReadChannelFound();
	virtual void OnConfirmConnection();
	virtual void OnRequestDisconnect();
	virtual	void OnConfirmDrives(int iNumDrives,
								 const CIPCDrive drives[]);

public:
	CEvent		m_eventGotDrives;

private:
	CRemoteThread* m_pRemoteThread;
};

#endif // !defined(AFX_CIPCDATABASESV_H__1DB2B833_B757_11D2_B56D_004005A19028__INCLUDED_)
