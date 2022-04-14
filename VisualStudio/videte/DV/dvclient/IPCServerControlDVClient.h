// IPCServerControlDVClient.h: interface for the CIPCServerControlDVClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPCSERVERCONTROLDVCLIENT_H__B4D87AEC_8FD1_4647_94D1_771C17573583__INCLUDED_)
#define AFX_IPCSERVERCONTROLDVCLIENT_H__B4D87AEC_8FD1_4647_94D1_771C17573583__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CIPCServerControlDVClient : public CIPCServerControl  
{
	// construction / destruction
public:
	CIPCServerControlDVClient(LPCTSTR shmName);
	virtual ~CIPCServerControlDVClient();

	// overrides
public:
	virtual void OnRequestAlarmConnection(const CConnectionRecord &c);

};

#endif // !defined(AFX_IPCSERVERCONTROLDVCLIENT_H__B4D87AEC_8FD1_4647_94D1_771C17573583__INCLUDED_)
