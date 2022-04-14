// AudioClient.h: interface for the CAudioClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUDIOCLIENT_H)
#define AFX_AUDIOCLIENT_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "IPCAudioServerClient.h"

class CAudioClient  
{
	// construction/destruction
public:
	CAudioClient();
	virtual ~CAudioClient();

	// attributes
public:
	inline DWORD	GetID() const;
	inline CIPCAudioDVClient* GetCIPC() const;
		   BOOL		IsConnected() const;
		   BOOL		IsDisconnected() const;
		   BOOL		IsTimedOut() const;
	inline BOOL		IsISDN() const;
	inline BOOL		IsTCPIP() const;
	inline int		GetBitrate() const;
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

	// private data member
private:
	CIPCAudioDVClient* m_pCIPC;
	DWORD				m_dwID;
	CConnectionRecord	m_ConnectionRecord;
	BOOL m_bISDN;
	BOOL m_bTCPIP;
	BOOL m_bSupervisor;
	BOOL m_bAlarm;
	int	 m_iBitrate;
	CString m_sSerial;
};
/////////////////////////////////////////////////////////////////////////////
inline DWORD CAudioClient::GetID() const
{
	return m_dwID;
}
/////////////////////////////////////////////////////////////////////////////
inline CIPCAudioDVClient* CAudioClient::GetCIPC() const
{
	return m_pCIPC;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CAudioClient::IsISDN() const
{
	return m_bISDN;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CAudioClient::IsTCPIP() const
{
	return m_bTCPIP;
}
/////////////////////////////////////////////////////////////////////////////
inline int CAudioClient::GetBitrate() const
{
	return m_iBitrate;
}
/////////////////////////////////////////////////////////////////////////////
inline CString CAudioClient::GetSerial() const
{
	return m_sSerial;
}
/////////////////////////////////////////////////////////////////////////////
inline CString CAudioClient::GetNumber() const
{
	if (m_bAlarm)
		return m_ConnectionRecord.GetDestinationHost();
	else
		return m_ConnectionRecord.GetSourceHost();
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CAudioClient::IsSupervisor() const
{
	return m_bSupervisor;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CAudioClient::IsAlarm() const
{
	return m_bAlarm;
}
/////////////////////////////////////////////////////////////////////////////
inline CString CAudioClient::GetSourceHost() const
{
	return m_ConnectionRecord.GetSourceHost();
}
/////////////////////////////////////////////////////////////////////////////
typedef CAudioClient* CAudioClientPtr;
WK_PTR_ARRAY_CS(CAudioClientArray,CAudioClientPtr,CAudioClientArrayCS);
/////////////////////////////////////////////////////////////////////////////
class CAudioClients : public CAudioClientArrayCS
{
	// construction / destruction
public:
	CAudioClients();
	virtual ~CAudioClients();

	// attributes
public:
	CAudioClient* GetAudioClient(DWORD dwID);
	BOOL		   IsConnected(const CString& sSerial,const CString& sSourceHost);
	int			   GetNrOfLicensed();

	// operations
public:
	CAudioClient* AddAudioClient();
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
