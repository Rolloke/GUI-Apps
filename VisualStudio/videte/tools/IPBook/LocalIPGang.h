// LocalIPGang.h: interface for the CLocalIPGang class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOCALIPGANG_H__B5B72941_CC12_11D4_86C3_004005A26A3B__INCLUDED_)
#define AFX_LOCALIPGANG_H__B5B72941_CC12_11D4_86C3_004005A26A3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define TIMEOUT 60000
#include "netcomm\winsocket.h"
#include "IPBookDlg.h"

class CIPBookDlg;
class CLocalIPGang : public CWinSocket  
{
public:

	void SendRefresh();
	void CheckTimeouts();
	void LeaveGang();
	void SetMulticastPort(UINT uPort);
	void SetMulticastAddress(CString sIP);

	CLocalIPGang(CIPBookDlg *pDlg);
	virtual ~CLocalIPGang();
	CEvent m_DestroyEvent;


protected:

	virtual void OnSocketBind();
	virtual void OnSocketCreated();
	virtual void OnSocketClosed(CWinSocket* pSocket);
	virtual void OnMultiplexMembershipDropped(CString sMulticastIP, int iMulticastPort);
	virtual void OnMulticastMembershipDropped(CString sMulticastIP, int iMulticastPort);
	virtual void OnMulticastGroupJoined(CString sMulticastIP, int iMulticastPort);
	virtual void OnMultiplexMemberAdded(CString sMulticastIP, int iMulticastPort);
	virtual void OnConfirmBandwidth(CString sRemoteIP, int iThroughput);
	virtual void OnDataSend(DATAGRAM* pDatagram);
	virtual void OnNewData(DATAGRAM* pDatagram);
	virtual void OnConnectionClosed(CWinSocket* pSocket);
	virtual void OnSending(CWinSocket* pSocket);
	virtual void OnReceiving(CWinSocket* pSocket);

private:
	void DeleteList();
	void RefreshMember(CString sIP, CString sHost);
	POSITION FindMemberInList(CString sIP);
	void RemoveMember(CString sIP, CString sHost);
	void AddMember(CString sIP, CString sHost);


	BOOL m_bDestroy;
	CString m_sLocalIP;
	UINT m_uMulticastPort;
	CString m_sMulticastAddress;
	CIPBookDlg* m_pDlg;

typedef struct MEMBER 
{
	CString sIP;
	DWORD dwRefreshTick;
	CString sHost;

} Member;

	CList <MEMBER*,MEMBER*>m_MemberList;

};

#endif // !defined(AFX_LOCALIPGANG_H__B5B72941_CC12_11D4_86C3_004005A26A3B__INCLUDED_)
