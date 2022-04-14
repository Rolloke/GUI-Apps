// IPCInputDVClient.h: interface for the CIPCInputDVClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPCINPUTDVCLIENT_H__43C9E49E_8B84_11D3_99EB_004005A19028__INCLUDED_)
#define AFX_IPCINPUTDVCLIENT_H__43C9E49E_8B84_11D3_99EB_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CInputClient;

class CIPCInputDVClient : public CIPCInputFileUpdate  
{
	// construction/destruction
public:
	CIPCInputDVClient(CInputClient* pClient, 
					  LPCTSTR shmName);
	virtual ~CIPCInputDVClient();

	// attributes
public:
	BOOL IsTimedOut();

	// overrides
public:
	virtual void OnConfirmReset(WORD wGroupID){;}
	virtual void OnRequestInfoInputs(WORD wGroupID);
	virtual void OnRequestDisconnect();
	virtual void OnRequestGetValue(CSecID id,
								   const CString &sKey,
								   DWORD dwServerData);
	virtual void OnRequestSetValue(CSecID id,
								   const CString &sKey,
								   const CString &sValue,
								   DWORD dwServerData);
	virtual	void OnRequestGetSystemTime();
	virtual	void OnRequestSetSystemTime(const CSystemTime& st);
	virtual void OnRequestResetApplication();

	virtual	void OnRequestGetTimeZoneInformation();
	virtual	void OnRequestSetTimeZoneInformation(CTimeZoneInformation tzi);
	virtual	void OnRequestGetTimeZoneInformations();
	virtual void OnRequestCameraControl(CSecID commID, 
										CSecID camID,
										CameraControlCmd cmd,
										DWORD dwValue);

private:
	CInputClient* m_pClient;
	DWORD		  m_dwTime;
};

#endif // !defined(AFX_IPCINPUTDVCLIENT_H__43C9E49E_8B84_11D3_99EB_004005A19028__INCLUDED_)
