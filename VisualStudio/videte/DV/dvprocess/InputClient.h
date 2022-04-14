// InputClient.h: interface for the CInputClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INPUTCLIENT_H)
#define AFX_INPUTCLIENT_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "IPCInputDVClient.h"

class CInputClient  
{
	// construction/destruction
public:
	CInputClient();
	virtual ~CInputClient();

	// attributes
public:
	inline DWORD	GetID() const;
	inline CIPCInputDVClient* GetCIPC() const;
		   BOOL		IsConnected() const;
		   BOOL		IsDisconnected() const;
		   BOOL		IsTimedOut() const;
	inline const CConnectionRecord& GetConnectionRecord() const;
	inline CString	GetSerial() const;
	inline CString	GetSourceHost() const;
	inline BOOL		IsSupervisor() const;

	// operations
public:
	BOOL Connect(const CConnectionRecord& c, BOOL bSupervisor);
	void SetSupervisor(BOOL bSupervisor);
	BOOL Disconnect();

	// private data member
private:
	CIPCInputDVClient* m_pCIPC;
	DWORD				m_dwID;
	CConnectionRecord	m_ConnectionRecord;
	CString				m_sSerial;
	BOOL				m_bSupervisor;
};
/////////////////////////////////////////////////////////////////////////////
inline DWORD CInputClient::GetID() const
{
	return m_dwID;
}
/////////////////////////////////////////////////////////////////////////////
inline CIPCInputDVClient* CInputClient::GetCIPC() const
{
	return m_pCIPC;
}
/////////////////////////////////////////////////////////////////////////////
inline const CConnectionRecord& CInputClient::GetConnectionRecord() const
{
	return m_ConnectionRecord;
}
/////////////////////////////////////////////////////////////////////////////
inline CString CInputClient::GetSerial() const
{
	return m_sSerial;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CInputClient::IsSupervisor() const
{
	return m_bSupervisor;
}
/////////////////////////////////////////////////////////////////////////////
inline CString CInputClient::GetSourceHost() const
{
	return m_ConnectionRecord.GetSourceHost();
}
/////////////////////////////////////////////////////////////////////////////
typedef CInputClient* CInputClientPtr;
WK_PTR_ARRAY_CS(CInputClientArray,CInputClientPtr,CInputClientArrayCS);
/////////////////////////////////////////////////////////////////////////////
class CInputClients : public CInputClientArrayCS
{
	// construction / destruction
public:
	CInputClients();
	virtual ~CInputClients();

	// attributes
public:
	CInputClient* GetInputClient(DWORD dwID);
	BOOL		  IsConnected(const CString& sSerial,const CString& sSourceHost);
	int			   GetNrOfLicensed();

	// operations
public:
	CInputClient* AddInputClient();
	void	 OnIdle();
	void     UpdateAllClients(DWORD dwID = (DWORD)-1);
	void	 ClientDisconnected(DWORD dwID);

	// indications
public:
	void DoIndicateAlarmNr(CSecID id ,BOOL bActive,DWORD dwData1=0,DWORD dwData2=0);
	void DoConfirmSetValue(CSecID id, const CString& sKey, const CString& sValue, DWORD dwParam);

private:
	BOOL m_bIdle;
};


#endif // !defined(AFX_INPUTCLIENT_H)
