// IPCAudioServerClient.h: interface for the CIPCAudioServerClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPCAUDIOSERVERCLIENT_H__1CA6371D_A9E6_4A89_8EE6_DA8ADAB7B941__INCLUDED_)
#define AFX_IPCAUDIOSERVERCLIENT_H__1CA6371D_A9E6_4A89_8EE6_DA8ADAB7B941__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CIPCAudioServer;

class CIPCAudioServerClient : public CIPCMedia  
{
public:
	CIPCAudioServerClient(const CConnectionRecord &c,
						  CIPCServerControlServerSide *pControl,
						  LPCTSTR shmName,
						  WORD wNr);
	virtual ~CIPCAudioServerClient();

public:
	inline CSecID	GetID() const;
	inline DWORD	GetOptions() const;
	inline const CConnectionRecord &GetConnection() const;
	inline DWORD	GetPriority() const;
	inline CSecID   GetPermissionID() const;


public:
	// info
	virtual void OnRequestInfoMedia(WORD wGroupID);

public:
	// the real data
	virtual void OnRequestStartMediaEncoding(CSecID mediaID, DWORD dwFlags, DWORD dwUserData);
	virtual void OnRequestStopMediaEncoding(CSecID mediaID, DWORD dwFlags);

	virtual void OnRequestMediaDecoding(const CIPCMediaSampleRecord&, CSecID mediaID, DWORD dwFlags);
	virtual void OnRequestStopMediaDecoding(CSecID mediaID, DWORD dwFlags);

	virtual void OnRequestHardware(CSecID secID, DWORD dwIDRequest);

	virtual void OnRequestValues(CSecID secID, DWORD dwCmd, DWORD dwID, DWORD dwValue, const CIPCExtraMemory*pData);

	virtual void OnRequestDisconnect();

protected:
	void DoClientEncoding(CIPCAudioServer* pCIPCAudioServer, CSecID mediaID, DWORD dwFlags, BOOL bStart);

private:
	CIPCServerControlServerSide *m_pControl;
	CConnectionRecord m_ConnectionRecord;
	CSecID m_id;
	DWORD  m_dwOptions;
	DWORD  m_dwPriority;
	CSecID m_idPermission;
	BOOL   m_bInfoConfirmed;

	CProcesses  m_Processes;
public:
	static BOOL gm_bTraceDoClientEncoding;
	static BOOL gm_bTraceOnRequestStartMediaEncoding;
	static BOOL gm_bTraceOnRequestStopMediaEncoding;
	static BOOL gm_bTraceRequestMediaDecoding;
	static BOOL gm_bTraceOnRequestStopMediaDecoding;
	static BOOL gm_bTraceOnRequestValues;
};
///////////////////////////////////////////////////////////////////////
inline CSecID CIPCAudioServerClient::GetID() const
{
	return m_id;
}
///////////////////////////////////////////////////////////////////////
inline DWORD CIPCAudioServerClient::GetOptions() const
{
	return m_dwOptions;
}
///////////////////////////////////////////////////////////////////////
inline const CConnectionRecord& CIPCAudioServerClient::GetConnection() const
{
	return m_ConnectionRecord;
}
///////////////////////////////////////////////////////////////////////
inline DWORD CIPCAudioServerClient::GetPriority() const
{
	return m_dwPriority;
}
///////////////////////////////////////////////////////////////////////
inline CSecID  CIPCAudioServerClient::GetPermissionID() const
{
	return m_idPermission;
}
///////////////////////////////////////////////////////////////////////
typedef CIPCAudioServerClient * CIPCAudioServerClientPtr;

WK_PTR_ARRAY_CS(CIPCAudioClientsArrayPlain,CIPCAudioServerClientPtr,CIPCAudioClientsArraySafe)

class CIPCAudioClientsArray : public CIPCAudioClientsArraySafe
{
public:
	CIPCAudioClientsArray();
	CIPCAudioServerClient *GetClientByID(CSecID id) const
	{
		if (id != SECID_NO_ID)
		{
			for (int i=0;i<GetSize();i++) 
			{
				if (GetAtFast(i)->GetID()==id) 
				{
					return GetAtFast(i);	// EXIT
				}
			}
		}
		return NULL;
	}
	CIPCAudioServerClient *GetClientBySubID(WORD wSubID) const
	{
		if (wSubID != SECID_NO_SUBID)
		{
			for (int i=0;i<GetSize();i++) 
			{
				if (GetAtFast(i)->GetID().GetSubID()==wSubID) 
				{
					return GetAtFast(i);	// EXIT
				}
			}
		}
		return NULL;
	}
};

#endif // !defined(AFX_IPCAUDIOSERVERCLIENT_H__1CA6371D_A9E6_4A89_8EE6_DA8ADAB7B941__INCLUDED_)
