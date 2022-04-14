// IPCDataCarrierClient.h: interface for the CIPCDataCarrierClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPCDATACARRIERCLIENT_H__901A1346_AB02_484B_8A1D_D5E7ED4B0E49__INCLUDED_)
#define AFX_IPCDATACARRIERCLIENT_H__901A1346_AB02_484B_8A1D_D5E7ED4B0E49__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IPCDataCarrier.h"

class CServer;

class CIPCDataCarrierClient : public CIPCDataCarrier  
{
public:
	CIPCDataCarrierClient(LPCTSTR shmName, CServer* pServer);
	virtual ~CIPCDataCarrierClient();

public:
	// overridables
	virtual void OnReadChannelFound();
	virtual void OnConfirmConnection();
	virtual void OnRequestDisconnect();
	virtual void OnConfirmVolumeInfos(int iNumDrives, const CIPCDrive drives[]);
	virtual	void OnIndicateSessionProgress(DWORD dwSessionID, int iProgress);
	virtual	void OnConfirmSession(DWORD dwSessionID);
	virtual void OnIndicateError(DWORD dwCmd, CSecID id, 
								 CIPCError errorCode, int iUnitCode,
								 const CString &sErrorMsg);
	virtual void OnConfirmCancelSession(DWORD dwSessionID);
	virtual	void OnConfirmDeleteVolume(const CString& sVolume);
	virtual void OnConfirmGetValue(CSecID id, 
								   const CString &sKey,
								   const CString &sValue,
								   DWORD dwServerData);

	// functions
	CIPCDrive* GetBackupDrive();

private:
	//members

	CServer*	m_pServer;
	CIPCDrive*	m_pBackupDrive;
};

#endif // !defined(AFX_IPCDATACARRIERCLIENT_H__901A1346_AB02_484B_8A1D_D5E7ED4B0E49__INCLUDED_)

