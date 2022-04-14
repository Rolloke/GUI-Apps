/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CIPCDatabaseClient.h $
// ARCHIVE:		$Archive: /Project/CIPC/CIPCDatabaseClient.h $
// CHECKIN:		$Date: 13.05.03 16:33 $
// VERSION:		$Revision: 9 $
// LAST CHANGE:	$Modtime: 13.05.03 14:52 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#if !defined(AFX_CIPCDATABASECLIENT_H__E639AA81_98B3_11D2_B541_004005A19028__INCLUDED_)
#define AFX_CIPCDATABASECLIENT_H__E639AA81_98B3_11D2_B541_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CIPCDatabase.h"
///////////////////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CIPCDatabaseClient : public CIPCDatabase
{
	// construction/destruction
public:
	CIPCDatabaseClient(LPCTSTR shmName);
	virtual ~CIPCDatabaseClient();

	// attributes
public:
	inline CIPCArchivRecords& GetRecords();
	inline BOOL  GotRecords() const;
	
	// operations
public:
	void RefreshInfo();
	
	// overrides
public:	
	virtual void OnReadChannelFound();
	virtual void OnConfirmConnection();
	virtual void OnConfirmInfo(int iNumRecords, const CIPCArchivRecord data[]);
	virtual	void OnIndicateNewArchiv(const CIPCArchivRecord& data);
	virtual	void OnIndicateRemoveArchiv(WORD wArchivNr);
	
	// access:
private:
	CIPCDatabaseClient(const CIPCDatabaseClient& src);		// no implementation, to avoid implicit generation!
	void operator=(const CIPCDatabaseClient& src);	// no implementation, to avoid implicit generation!

	CIPCArchivRecords	m_archives;
	BOOL m_bGotRecords;
};
///////////////////////////////////////////////////////////////////////////////////
inline CIPCArchivRecords& CIPCDatabaseClient::GetRecords() 
{ 
	return m_archives;
}
///////////////////////////////////////////////////////////////////////////////////
inline BOOL  CIPCDatabaseClient::GotRecords() const 
{ 
	return m_bGotRecords; 
}
#endif // !defined(AFX_CIPCDATABASECLIENT_H__E639AA81_98B3_11D2_B541_004005A19028__INCLUDED_)
