/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CipcInputServerClient.h $
// ARCHIVE:		$Archive: /Project/SecurityServer/CipcInputServerClient.h $
// CHECKIN:		$Date: 12.05.06 20:19 $
// VERSION:		$Revision: 46 $
// LAST CHANGE:	$Modtime: 12.05.06 20:19 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef __CIPC_INPUT_SERVER_CLIENT_H
#define __CIPC_INPUT_SERVER_CLIENT_H


// CIPInputServerClient ist die Verbindung zu den CIPCInputClient's

class CIPCServerControlServerSide;

class CIPCInputServerClient : public CIPCInputFileUpdate
{
	// construction/destruction
public:
	CIPCInputServerClient(const CConnectionRecord &c,
							CIPCServerControlServerSide *pControl,
							LPCTSTR  shmName,
							WORD wNr);
    virtual ~CIPCInputServerClient();

	// attributes
public:
	inline CSecID  GetPermissionID() const;
	
	inline BOOL	   IsTimedOut() const;
	inline BOOL	   IsConnectionTimedOut() const;
	
	inline CString GetSerial() const;
	inline CString GetRemoteHost() const;

	// set
public:
	inline void	SetConnectionTime(DWORD dwTime);

	// overrides
public:
   	virtual void OnReadChannelFound();
	virtual void OnRequestConnection();
	virtual void OnRequestDisconnect();
   	virtual void OnRequestVersionInfo(WORD wGroupID);
	virtual void OnRequestGetFile(int iDestination, const CString &sFileName);

   	virtual BOOL OnTimeoutWrite(DWORD dwCmd);// no CmdDone response,
	virtual BOOL OnTimeoutCmdReceive();	// no new cmd received
	virtual void OnWaitFailed(DWORD dwCmd);
	virtual void OnRequestInfoInputs(WORD wGroupID);

	virtual void OnRequestGetValue(
						CSecID id, // might be used as group ID only
						const CString &sKey,
						DWORD dwServerData
					);
	virtual void OnRequestSetValue(
						CSecID id, // might be used as group ID only
						const CString &sKey,
						const CString &sValue,
						DWORD dwServerData
						);
	// CommData related
	virtual void OnWriteCommData(CSecID id,
									const CIPCExtraMemory &data,
									DWORD dwBlockNr
									);
	virtual void OnRequestCameraControl(
								CSecID commID, CSecID camID,
								CameraControlCmd cmd,
								DWORD dwValue
								);
	virtual	void OnRequestGetSystemTime();
	virtual	void OnRequestSetSystemTime(const CSystemTime& st);
	virtual	void OnConfirmAlarm(CSecID id);
	virtual	void OnRequestGetAlarmOffSpans(CSecID id);
	virtual	void OnRequestSetAlarmOffSpans(CSecID id, int iNumRecords, const CSystemTimeSpan data[]);
	virtual void OnRequestResetApplication();
	virtual	void OnRequestGetTimeZoneInformation();
	virtual	void OnRequestSetTimeZoneInformation(CTimeZoneInformation tzi);
	virtual	void OnRequestGetTimeZoneInformations();

	inline CSecID GetID() const;

	// Implementation
private:

	// data members
private:
	CIPCServerControlServerSide *m_pControl;
	CSecID m_id;
	// client connection data
	CConnectionRecord	m_ConnectionRecord;
	CString	m_sSerial;
	CString m_sUserName;
	CSecID	m_permissionID;

	DWORD	m_dwConnectionTime;
	DWORD	m_dwTickInfoConfirmed;
	DWORD	m_dwStarttimeForNotConnectedTimeout;
	DWORD	m_dwNotConnectedTimeout;
	BOOL	m_bInfoConfirmed;
};

inline CSecID CIPCInputServerClient::GetID() const
{
	return m_id;
}
inline CSecID CIPCInputServerClient::GetPermissionID() const
{
	return m_permissionID;
}
/////////////////////////////////////////////////////////////////////////////
inline void	CIPCInputServerClient::SetConnectionTime(DWORD dwTime)
{
	m_dwConnectionTime = dwTime;
}
/////////////////////////////////////////////////////////////////////////////
inline CString CIPCInputServerClient::GetRemoteHost() const
{
	return m_ConnectionRecord.GetSourceHost();
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CIPCInputServerClient::IsTimedOut() const
{
	if (   GetIPCState() == CIPC_STATE_READ_FOUND
		|| GetIPCState() == CIPC_STATE_WRITE_CREATED)
	{
		return (GetTimeSpan(m_dwStarttimeForNotConnectedTimeout) >= m_dwNotConnectedTimeout);
	}
	else
	{
		return FALSE;
	}
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CIPCInputServerClient::IsConnectionTimedOut() const
{
	if (   GetIPCState() == CIPC_STATE_READ_FOUND
		|| GetIPCState() == CIPC_STATE_WRITE_CREATED
		|| GetIPCState() == CIPC_STATE_CONNECTED)
	{
		if (   m_dwTickInfoConfirmed != 0
			&& m_dwConnectionTime>0)
		{
			return (GetTimeSpan(m_dwTickInfoConfirmed) >= m_dwConnectionTime*1000);
		}
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
inline CString CIPCInputServerClient::GetSerial() const
{
	return m_sSerial;
}
/////////////////////////////////////////////////////////////////////////////
typedef CIPCInputServerClient * CIPCInputServerClientPtr;
WK_PTR_ARRAY_CS(CIPCInputClientsArrayPlain,CIPCInputServerClientPtr,CIPCInputClientsArraySafe)

class CIPCInputClientsArray : public CIPCInputClientsArraySafe
{
public:
	CIPCInputClientsArray();
	CIPCInputServerClient *GetClientByID(CSecID id) const;
	BOOL		   IsConnected(const CString& sSerial,const CString& sSourceHost);
	int			   GetNrOfLicensed();
};

// now static 29.11.97 CString DuplicateBackSlash(CString s);
// now static 29.11.97 HKEY GetSecKey();
// now static void ExportRegistry( HKEY hKey, LPTSTR pszRegPath , CFile& file);

#endif
