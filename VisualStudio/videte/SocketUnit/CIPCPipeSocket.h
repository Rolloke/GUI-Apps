/* GlobalReplace: ClientSocket --> DataSocket */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CIPCPipeSocket.h $
// ARCHIVE:		$Archive: /Project/SocketUnit/CIPCPipeSocket.h $
// CHECKIN:		$Date: 22.08.03 12:11 $
// VERSION:		$Revision: 21 $
// LAST CHANGE:	$Modtime: 22.08.03 11:28 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef CIPCPipeSocket_H
#define CIPCPipeSocket_H

#include "CIPC.h"
#include "SecID.h"

class CDataSocket;

/////////////////////////////////////////////////////////////////////////////
enum CIPCType
{
	CIPC_TYPE_INVALID     = 0,
	CIPC_INPUT_CLIENT     = 1,
	CIPC_OUTPUT_CLIENT    = 2,
	CIPC_INPUT_SERVER     = 3,
	CIPC_OUTPUT_SERVER    = 4,
	CIPC_DATABASE_CLIENT  = 5,
	CIPC_DATABASE_SERVER  = 6,
	CIPC_CONTROL_INPUT    = 7,
	CIPC_CONTROL_OUTPUT   = 8,
	CIPC_CONTROL_DATABASE = 9,
	CIPC_ALARM_SERVER     =10,
	CIPC_ALARM_CLIENT     =11,
	CIPC_CONTROL_AUDIO    =12,
	CIPC_AUDIO_SERVER     =13,
	CIPC_AUDIO_CLIENT     =14
};

extern LPCTSTR NameOfEnum(CIPCType ct);
extern LPCTSTR UserNameOfEnum(CIPCType ct);


class CConnectionThread;
class CDataSocket;


/////////////////////////////////////////////////////////////////////////////
class CIPCPipeSocket : public CIPC
{
public:
	CIPCPipeSocket(CConnectionThread *pConnection,
				LPCTSTR pSMName,
				CIPCType t,
				BOOL bAsMaster,
				DWORD dwOptions,
				WORD wID,
				LPCTSTR hostName=NULL
				);
	~CIPCPipeSocket();
	//
	BOOL PushCmd(DWORD dwDataLen, const void *pData,
				DWORD dwCmd,DWORD dwParam1=0, DWORD dwParam2=0, 
				DWORD dwParam3=0, DWORD dwParam4=0);
	void OnClose(int iError);	// called from socket
	
	inline void	SetNotConnectedTimeout(DWORD dwTimeout);
	BOOL		IsTimedOut() const;
	void		Disconnect();

	inline WORD GetId() const;
	inline WORD GetRemoteId() const;

	inline DWORD GetOptions() const;
	inline CIPCType GetCIPCType() const;
	inline CConnectionThread *GetConnection() const;

	void SetRemoteId(WORD wRemoteId);

protected:
	// CIPC functions
	virtual BOOL HandleCmd(DWORD dwCmd,
						  DWORD dwParam1, DWORD dwParam2,
						  DWORD dwParam3, DWORD dwParam4,
						  const CIPCExtraMemory* pExtraMem
						  );
	virtual void OnReadChannelFound();
	virtual void OnRequestDisconnect();

	virtual BOOL	OnTimeoutWrite(DWORD dwCmd);		// no CmdDone response, OOPS called before WriteCmd
	virtual BOOL	OnTimeoutCmdReceive();	// no new cmd received
private:	
public:
	//
	virtual void OnRequestBitrate();
	//
	inline CDataSocket *GetUsedSocket() const;
	//
	CConnectionThread *m_pConnection;
	CDataSocket	*m_pDataSocket;
	CDataSocket	*m_pServerSocket;
	DWORD			 m_dwOptions;
	CIPCType		 m_cipcType;	// set in constructor
	//
	WORD	m_wId;
	WORD	m_wRemoteId;
	DWORD	m_dwStarttimeForNotConnectedTimeout;
	DWORD	m_dwNotConnectedTimeout;
	DWORD   m_dwBitrate;
};

/////////////////////////////////////////////////////////////////////////////
inline WORD CIPCPipeSocket::GetId() const
{
	return m_wId;
}
inline WORD CIPCPipeSocket::GetRemoteId() const
{
	return m_wRemoteId;
}
/////////////////////////////////////////////////////////////////////////////
inline void CIPCPipeSocket::SetNotConnectedTimeout(DWORD dwTimeout)
{
	m_dwStarttimeForNotConnectedTimeout = GetTickCount();
	m_dwNotConnectedTimeout = dwTimeout;
}

/////////////////////////////////////////////////////////////////////////////
inline DWORD CIPCPipeSocket::GetOptions() const
{
	return (m_dwOptions);
}
/////////////////////////////////////////////////////////////////////////////
inline CIPCType CIPCPipeSocket::GetCIPCType() const
{
	return (m_cipcType);
}

inline CConnectionThread *CIPCPipeSocket::GetConnection() const
{
	return m_pConnection;
}


/////////////////////////////////////////////////////////////////////////////
typedef CIPCPipeSocket* CIPCPipeSocketPtr;

/////////////////////////////////////////////////////////////////////////////
WK_PTR_ARRAY(CIPCPipesArray,CIPCPipeSocketPtr);



inline CDataSocket *CIPCPipeSocket::GetUsedSocket() const
{
	if (m_pServerSocket) {
		return m_pServerSocket;
	} else if (m_pDataSocket) {
		return m_pDataSocket;
	} else {
		return NULL;
	}
}


#endif	// CIPCPipeSocket_H
