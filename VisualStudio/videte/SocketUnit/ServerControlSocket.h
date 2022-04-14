/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ServerControlSocket.h $
// ARCHIVE:		$Archive: /Project/SocketUnit/ServerControlSocket.h $
// CHECKIN:		$Date: 22.08.03 12:11 $
// VERSION:		$Revision: 17 $
// LAST CHANGE:	$Modtime: 22.08.03 12:00 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef _ServerControlSocket_H_
#define _ServerControlSocket_H_

#include "CipcServerControl.h"
#include "CipcPipeSocket.h"	// fo CIPCType
#include "IPInterface.h"
class CSocketUnitDoc;

class CServerControlSocket : public CIPCServerControl
{
public:
	CServerControlSocket(CSocketUnitDoc *pDoc, LPCTSTR shmName);
   ~CServerControlSocket();

    virtual void OnReadChannelFound();
    virtual void OnRequestDisconnect();

	virtual void OnRequestInputConnection(const CConnectionRecord &c);
	virtual void OnRequestOutputConnection(const CConnectionRecord &c);
	virtual void OnRequestDataBaseConnection(const CConnectionRecord &c);
	virtual void OnRequestAlarmConnection(const CConnectionRecord &c);
	virtual void OnRequestServerReset(const CConnectionRecord &c);
	virtual void OnRequestAudioConnection(const CConnectionRecord &c);

	BOOL	CheckIfClientIsStillListening(CConnectionThread* pThread);
private:
	void GetLocalIPList(int &iNrOfIP);
	CString ResolveIPAdapter(CString sRemoteIP);
	void CreateConnection(const CConnectionRecord &c, 
						CIPCType cipcType
						);

private:
	CSocketUnitDoc*		m_pDoc;
	CConnectionThread*	m_pConnectionThread;
	unsigned long		m_lLocalIp[10];

	CCriticalSection	m_csConnection;
};

#endif
