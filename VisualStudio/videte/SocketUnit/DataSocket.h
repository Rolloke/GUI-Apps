#ifndef __CLNTSOCK_H__
#define __CLNTSOCK_H__


#define TCP_BUFFER_SIZE (256*1024)

#include "SocketUtil.h"

class CConnectionThread;
class CIPCPipeSocket;
class CIPCExtraMemory;

class CDataSocket : public CSocket
{
	DECLARE_DYNAMIC(CDataSocket)
	// construction / destruction
public:
	CDataSocket(CConnectionThread * pConnection);
	CDataSocket(CIPCPipeSocket * pPipe, CConnectionThread *pConnection);
	virtual ~CDataSocket();

	// attributes
public:
	inline BOOL IsConnected() const;
	inline BOOL CanSend() const;
	
	// operations
public:
	void SetSocketOpts();

	void DoSend();
	BOOL DoSend(WORD wLocalId1,
				WORD wRemoteId,
				DWORD dwCmd,
				DWORD dwParam1,
				DWORD dwParam2,
				DWORD dwParam3,
				DWORD dwParam4,
				const CIPCExtraMemory* pExtraMem
				);


	UINT	GetPort();
	UINT	GetRemotePort();

	CString GetPipeTypeString() const;
	CString GetName();
	CString GetRemoteName();

	void AddToReceiveBuffer(BYTE* pBuffer, int iBufferSize);
	void SetCIPCPipeSocket(CIPCPipeSocket *pPipe);

	CIPCPipeSocket* GetCIPCPipeSocket();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Overridable callbacks
protected:
	virtual void OnSend(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);

private:
	BYTE* RemoveFromReceiveBuffer(int iSize);
	void HandleBuffer();
	void Init();
	
	CDataSocket(const CDataSocket& rSrc);         // no implementation
	void operator=(const CDataSocket& rSrc);      // no implementation


private:
	int m_iRecvLowWatermark;
	BOOL m_bFirstCMDReceived;
	int m_iCounter;
	DWORD m_dwMemory;
	int m_iSendBufferSize;

	CIPCPipeSocket *m_pPipe;

	DWORD m_dwCMDRecordArray[7];

	BOOL m_bReceivedClose;
	BOOL m_bDataIsCMDRecord;

	BYTE* m_pSendBuffer;
	DWORD m_dwSendBufferSize;

	BYTE* m_pReceiveBuffer;
	BYTE* m_pMaxReceiveBuffer;

	int m_iReceiveBufferSize;
	int m_iMaxReceiveBufferSize;
	int m_iSizeOfDataNextToRead;
	int m_iSendErrorCounter;	

	CConnectionThread *m_pConnection;

	CString m_sStoredName;
	CString m_sStoredRemoteName;

	WORD m_wId1;
	WORD m_wId2;

	BOOL m_bCanSend;
	BOOL m_bConnected;
};

//////////////////INLINER///////////////////////////////////////////////////////////////
inline BOOL CDataSocket::CanSend() const
{
	return m_bCanSend;
}
////////////////////////////////////////////////////////////////////////////////////////
inline CString CDataSocket::GetName() 
{
	if (m_sStoredName.IsEmpty()) 
	{	// first time ?
		CString tmp;
		UINT p;
		GetSockName(tmp,p);
		m_sStoredName = CWK_DNS::LookUp(tmp);
	}
	return m_sStoredName;
}

////////////////////////////////////////////////////////////////////////////////////////

inline CString CDataSocket::GetRemoteName() 
{
	if (m_sStoredRemoteName.IsEmpty()) 
	{	// first time ?
		CString tmp;
		UINT p;
		GetPeerName(tmp,p);
		m_sStoredRemoteName = CWK_DNS::LookUp(tmp);
	}
	return m_sStoredRemoteName;
}

////////////////////////////////////////////////////////////////////////////////////////

inline UINT	CDataSocket::GetPort()
{
	CString tmp;
	UINT p;
	GetSockName(tmp,p);
	return p;
}

////////////////////////////////////////////////////////////////////////////////////////

inline UINT CDataSocket::GetRemotePort() 
{
	CString tmp;
	UINT p;
	GetPeerName(tmp,p);
	return p;
}

#endif // __CLNTSOCK_H__
