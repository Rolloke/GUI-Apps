// IPCFetch.h: interface for the CIPCFetch class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPCFETCH_H__C5C11171_0AB6_11D4_A0D0_004005A19028__INCLUDED_)
#define AFX_IPCFETCH_H__C5C11171_0AB6_11D4_A0D0_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CipcServerControlClientSide.h"

class AFX_EXT_CLASS CIPCFetch  
{
	// construction / destruction
public:
	CIPCFetch();
	virtual ~CIPCFetch();

	// attributes
public:
	inline ServerControlGroup GetType() const;
	inline BOOL IsCancelled() const;
	inline BOOL IsFetching() const;

	// operations
public:
	CIPCFetchResult FetchInput(const CConnectionRecord &c);
	CIPCFetchResult FetchOutput(const CConnectionRecord &c);
	CIPCFetchResult FetchDatabase(const CConnectionRecord &c);
	CIPCFetchResult FetchDataCarrier(const CConnectionRecord &c);
	
	BOOL FetchServerReset(const CConnectionRecord &c);
	CIPCFetchResult FetchAlarmConnection(const CConnectionRecord &c);
	BOOL FetchUpdate(int iDestination,
					 const CString &sFileName,
					 const void *pData,
					 DWORD dwDataLen,
					 BOOL bNeedsReboot);
	CIPCFetchResult FetchAudio(const CConnectionRecord &c);

	void Cancel();

	// implementation
protected:
	CIPCFetchResult DoFetch(const CConnectionRecord& c);
	CIPCFetchResult InternalFetch(const CConnectionRecord& c,
								  const CString& sModulName,
								  const CString& sUnitName);
	BOOL			TrySecondUnit(const CString& sModulName);

	// data member
private:
	BOOL				m_bCancel;
	CEvent				m_evCancel;
	BOOL				m_bFetching;
	ServerControlGroup	m_Type;
	CIPCServerControlClientSide* m_pConnection;
};
//////////////////////////////////////////////////////////////////////
inline ServerControlGroup CIPCFetch::GetType() const
{
	return m_Type;
}
//////////////////////////////////////////////////////////////////////
inline BOOL CIPCFetch::IsCancelled() const
{
	return m_bCancel;
}
//////////////////////////////////////////////////////////////////////
inline BOOL CIPCFetch::IsFetching() const
{
	return m_bFetching;
}

#endif // !defined(AFX_IPCFETCH_H__C5C11171_0AB6_11D4_A0D0_004005A19028__INCLUDED_)
