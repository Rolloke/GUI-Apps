/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CipcDatabaseClientServer.h $
// ARCHIVE:		$Archive: /Project/SecurityServer/CipcDatabaseClientServer.h $
// CHECKIN:		$Date: 29.05.06 17:16 $
// VERSION:		$Revision: 10 $
// LAST CHANGE:	$Modtime: 29.05.06 16:07 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef _CIPCDatabaseClientServer_H
#define _CIPCDatabaseClientServer_H

#include "CipcDatabaseClient.h"

class CIPCDatabaseClientServer : public CIPCDatabaseClient
{
	// construction / destruction
public:
	CIPCDatabaseClientServer(LPCTSTR shmName);
	virtual ~CIPCDatabaseClientServer();

	// attributes
public:
	inline CSecID GetAlarmListID() const;

	// overrides
public:
	virtual BOOL Run(DWORD dwTimeOut);
	virtual void OnConfirmConnection();
	virtual void OnConfirmInfo(int iNumRecords, const CIPCArchivRecord data[]);
	virtual void OnConfirmAlarmListArchives(int iNumRecords, const CIPCArchivRecord data[]);
	virtual void OnRequestDisconnect();
	virtual void OnIndicateError(DWORD dwCmd, 
								CSecID id, 
								CIPCError error, 
								int iErrorCode,
								const CString &sErrorMessage);
	virtual	void OnConfirmNewSavePicture(WORD wArchivNr,
										 CSecID camID);

private:
	CTimedMessage m_tmIamAlive;
	CSecID		  m_idAlarmList;
};
//////////////////////////////////////////////////////////////////////////
inline CSecID CIPCDatabaseClientServer::GetAlarmListID() const
{
	return m_idAlarmList;
}

#endif
