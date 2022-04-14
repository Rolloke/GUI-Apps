// Client.h: interface for the CClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLIENT_H)
#define AFX_CLIENT_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "CIPCDatabaseStorage.h"

class CClient  
{
	// construction/destruction
public:
	CClient();
	virtual ~CClient();

	// attributes
public:
	inline DWORD	GetID() const;
	inline CIPCDatabaseStorage* GetCIPC() const;
		   BOOL		IsConnected() const;
		   BOOL		IsDisconnected() const;
	inline BOOL		IsISDN() const;
	inline BOOL		IsTCPIP() const;
	inline int		GetBitrate() const;
	inline BOOL		IsLowBandwidth() const;
	inline BOOL		UseH263() const;
	inline BOOL		UseMpeg4() const;
	inline CString	GetSerial() const;
	inline CString	GetSourceHost() const;

	// operations
public:
	BOOL Connect(const CConnectionRecord& c);

	// private data member
private:
	CIPCDatabaseStorage* m_pCIPCDatabaseStorage;
	DWORD				m_dwID;
	CConnectionRecord	m_ConnectionRecord;
	BOOL				m_bISDN;
	BOOL				m_bTCPIP;
	BOOL				m_bH263;
	BOOL				m_bMpeg4;
	int					m_iBitrate;
	CString				m_sSerial;
	CString				m_sVersion;
};
/////////////////////////////////////////////////////////////////////////////
inline DWORD CClient::GetID() const
{
	return m_dwID;
}
/////////////////////////////////////////////////////////////////////////////
inline CIPCDatabaseStorage* CClient::GetCIPC() const
{
	return m_pCIPCDatabaseStorage;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CClient::IsISDN() const
{
	return m_bISDN;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CClient::IsTCPIP() const
{
	return m_bTCPIP;
}
/////////////////////////////////////////////////////////////////////////////
inline int CClient::GetBitrate() const
{
	return m_iBitrate;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CClient::UseH263() const
{
	return m_bH263;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CClient::UseMpeg4() const
{
	return m_bMpeg4;
}
/////////////////////////////////////////////////////////////////////////////
inline CString CClient::GetSerial() const
{
	return m_sSerial;
}
/////////////////////////////////////////////////////////////////////////////
inline CString CClient::GetSourceHost() const
{
	return m_ConnectionRecord.GetSourceHost();
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CClient::IsLowBandwidth() const
{
	return IsISDN() || (m_iBitrate <= 128*1024);
}
/////////////////////////////////////////////////////////////////////////////
typedef CClient* CClientPtr;
WK_PTR_ARRAY_CS(CClientArray,CClientPtr,CClientArrayCS);
/////////////////////////////////////////////////////////////////////////////
class CClients : public CClientArrayCS
{
	// construction / destruction
public:
	CClients();
	virtual ~CClients();

	// attributes
public:
	CClient* GetClient(DWORD dwID);
	BOOL	 IsConnected(const CString& sSerial,const CString& sSourceHost);
	int		 GetNrOfLicensed();

	// operations
public:
	CClient* AddClient();
	void	 OnIdle();

	// indications
public:
	void	DoIndicateDeleteTape(const CTape& tape);
	void	DoIndicateNewTape(const CTape& tape,WORD wPrevSequenceNr,DWORD dwNrOfRecords);

	void	DoIndicateNewCollection(CCollection& coll);
	void	DoIndicateDeleteCollection(WORD wCollectionNr);

	void	DoIndicateNewCollectionName(DWORD dwClientID, CCollection& coll);

	void	DoIndicateDrives();
};


#endif // !defined(AFX_CLIENT_H)
