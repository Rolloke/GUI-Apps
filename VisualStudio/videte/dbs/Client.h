// Client.h: interface for the CClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLIENT_H__AB5B2378_3834_11D2_B58E_00C095EC9EFA__INCLUDED_)
#define AFX_CLIENT_H__AB5B2378_3834_11D2_B58E_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "CIPCDatabaseServer.h"

class CClient  
{
	// construction/destruction
public:
	CClient();
	virtual ~CClient();

	// attributes
public:
	inline DWORD	GetID() const;
	inline CString 	GetName() const;
	inline CString 	GetVersion() const;
	inline CIPCDatabaseServer* GetCIPCDatabaseServer() const;
		   BOOL	IsConnected() const;
		   BOOL	IsDisconnected() const;

	// operations
public:
	CIPCError Connect(const CConnectionRecord& c);

	// private data member
private:
	CIPCDatabaseServer* m_pCIPCDatabaseClient;
	DWORD				m_dwID;
	CConnectionRecord	m_ConnectionRecord;
	CString				m_sName;
	CString				m_sVersion;
};

inline DWORD CClient::GetID() const
{
	return m_dwID;
}
inline CIPCDatabaseServer* CClient::GetCIPCDatabaseServer() const
{
	return m_pCIPCDatabaseClient;
}
inline CString CClient::GetName() const
{
	return m_sName;
}
inline CString CClient::GetVersion() const
{
	return m_sVersion;
}


#endif // !defined(AFX_CLIENT_H__AB5B2378_3834_11D2_B58E_00C095EC9EFA__INCLUDED_)
