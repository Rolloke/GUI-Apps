// OutputClient.h: interface for the COutputClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OUTPUTCLIENT_H)
#define AFX_OUTPUTCLIENT_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "IPCOutputDVClient.h"

class COutputClient  
{
	// construction/destruction
public:
	COutputClient();
	virtual ~COutputClient();

	// attributes
public:
	inline DWORD	GetID() const;
	inline CIPCOutputDVClient* GetCIPC() const;
		   BOOL		IsConnected() const;
		   BOOL		IsDisconnected() const;
		   BOOL		IsTimedOut() const;
	inline BOOL		IsISDN() const;
	inline BOOL		IsTCPIP() const;
	inline BOOL		IsDTS() const;
	inline BOOL		IsLocal() const;
	inline DWORD    GetOptions() const;
	
	inline int		GetBitrate() const;
	inline BOOL		UseH263() const;
	inline BOOL		CanMpeg4() const;
	inline BOOL		CanGOP() const;
	inline BOOL		IsLowBandwidth() const;

	inline CString	GetSerial() const;
	inline CString	GetSourceHost() const;
	inline CString  GetNumber() const;
	inline BOOL		IsSupervisor() const;
	inline BOOL		IsAlarm() const;

	// operations
public:
	BOOL Connect(const CConnectionRecord& c,BOOL bSupervisor,BOOL bAlarm=FALSE);
	void SetSupervisor(BOOL bSupervisor);
	BOOL Disconnect();
	void SetFetchResult(CIPCFetchResult& fr);
	void InitH263Mpeg4();

	// private data member
private:
	CIPCOutputDVClient* m_pCIPC;
	DWORD				m_dwID;
	CConnectionRecord	m_ConnectionRecord;
	DWORD				m_dwOptions;
	BOOL m_bISDN;
	BOOL m_bTCPIP;
	BOOL m_bSupervisor;
	BOOL m_bAlarm;
	int	 m_iBitrate;
	
	BOOL m_bH263;
	BOOL m_bMpeg4;
	BOOL m_bIsDTS;
	BOOL m_bCanGOP;
	
	CString m_sSerial;
	CString m_sVersion;
};
/////////////////////////////////////////////////////////////////////////////
inline DWORD COutputClient::GetID() const
{
	return m_dwID;
}
/////////////////////////////////////////////////////////////////////////////
inline CIPCOutputDVClient* COutputClient::GetCIPC() const
{
	return m_pCIPC;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	COutputClient::IsISDN() const
{
	return m_bISDN;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	COutputClient::IsTCPIP() const
{
	return m_bTCPIP;
}
/////////////////////////////////////////////////////////////////////////////
inline int COutputClient::GetBitrate() const
{
	return m_iBitrate;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	COutputClient::UseH263() const
{
	return m_bH263;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	COutputClient::CanMpeg4() const
{
	return m_bMpeg4;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	COutputClient::CanGOP() const
{
	return m_bCanGOP;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	COutputClient::IsLowBandwidth() const
{
	return IsISDN() || (m_iBitrate <= 128*1024);
}
/////////////////////////////////////////////////////////////////////////////
inline CString COutputClient::GetSerial() const
{
	return m_sSerial;
}
/////////////////////////////////////////////////////////////////////////////
inline CString COutputClient::GetNumber() const
{
	if (m_bAlarm)
		return m_ConnectionRecord.GetDestinationHost();
	else
		return m_ConnectionRecord.GetSourceHost();
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	COutputClient::IsSupervisor() const
{
	return m_bSupervisor;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	COutputClient::IsAlarm() const
{
	return m_bAlarm;
}
/////////////////////////////////////////////////////////////////////////////
inline CString COutputClient::GetSourceHost() const
{
	return m_ConnectionRecord.GetSourceHost();
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	COutputClient::IsDTS() const
{
	return m_bIsDTS;
}
/////////////////////////////////////////////////////////////////////////////
inline DWORD COutputClient::GetOptions() const
{
	return m_dwOptions;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL COutputClient::IsLocal() const
{
	return !IsISDN() && !IsTCPIP();
}
/////////////////////////////////////////////////////////////////////////////
typedef COutputClient* COutputClientPtr;
WK_PTR_ARRAY_CS(COutputClientArray,COutputClientPtr,COutputClientArrayCS);
/////////////////////////////////////////////////////////////////////////////
class COutputClients : public COutputClientArrayCS
{
	// construction / destruction
public:
	COutputClients();
	virtual ~COutputClients();

	// attributes
public:
	COutputClient* GetOutputClient(DWORD dwID);
	BOOL		   IsConnected(const CString& sSerial,const CString& sSourceHost);
	int			   GetNrOfLicensed();

	// operations
public:
	COutputClient* AddOutputClient();
	void		   OnIdle();
	void		   ClientDisconnected(DWORD dwID);

	// indications
public:
	void UpdateAllClients(DWORD dwID = (DWORD)-1);
	void DoConfirmGetValue(CSecID id, const CString& sKey, const CString& sValue);

private:
	BOOL m_bIdle;
};


#endif
