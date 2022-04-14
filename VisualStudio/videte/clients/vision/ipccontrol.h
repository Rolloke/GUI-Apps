// IPCGemos.h: interface for the CIPCControl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPCCONTROL_H__B90A2FA7_5619_11D3_9988_004005A19028__INCLUDED_)
#define AFX_IPCCONTROL_H__B90A2FA7_5619_11D3_9988_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CServer;

typedef enum{On, Off, Enabled, Disabled}GemosInputState;

class CIPCControl : public CIPC  
{
	// construction/destruction
public:
	CIPCControl(LPCTSTR shmName, CServer* pServer);
	virtual ~CIPCControl();

	// operations
public:
   void		ConfirmCamera(CSecID idCam, GemosInputState eState);
//ML 29.9.99 Begin Insertation{
   void		ConfirmRelay(CSecID idRelay, GemosInputState eState);
   void		ConfirmAlarm(CSecID idAlarm, GemosInputState eState);
//ML 29.9.99 End Insertation}

	// Overrides
public:
	virtual BOOL Run(DWORD dwTimeOut);
	virtual void OnReadChannelFound();
	virtual void OnConfirmConnection();
	virtual void OnRequestDisconnect();
	virtual void OnRequestGetValue(CSecID id, // might be used as group ID only
								   const CString &sKey,
								   DWORD dwServerData);
	virtual void OnConfirmGetValue(CSecID id, // might be used as group ID only
									const CString &sKey,
									const CString &sValue,
									DWORD dwServerData
									);
	virtual void OnConfirmSetValue(CSecID id, // might be used as group ID only
									const CString &sKey,
									const CString &sValue,
									DWORD dwServerData
									);
	virtual void OnRequestSetValue(CSecID id, // might be used as group ID only
									const CString &sKey,
									const CString &sValue,
									DWORD dwServerData
									);
	
	// data member
private:
	CServer* m_pServer;
	DWORD	 m_dwTick;
};

#endif // !defined(AFX_IPCGEMOS_H__B90A2FA7_5619_11D3_9988_004005A19028__INCLUDED_)
