// IPCBurnDataCarrier.h: interface for the CIPCBurnDataCarrier class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPCBURNDATACARRIER_H__0605CE26_8A97_4AEF_AA76_E81AF2BCDB86__INCLUDED_)
#define AFX_IPCBURNDATACARRIER_H__0605CE26_8A97_4AEF_AA76_E81AF2BCDB86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CIPCBurnDataCarrier : public CIPCDataCarrier 
{
public:
	CIPCBurnDataCarrier(LPCTSTR shmName);
	virtual ~CIPCBurnDataCarrier();

	// overrides
public:
	virtual void OnRequestDisconnect();
	virtual void OnRequestVolumeInfos();
	virtual	void OnRequestSession(DWORD dwSessionID,
								  const CString& sVolume,
								  const CStringArray& sPathnames,
								  DWORD dwFlags);
	virtual void OnRequestCancelSession(DWORD dwSessionID);

	void OnRequestSession_Nero(DWORD dwSessionID,
								  const CString& sVolume,
								  const CStringArray& sPathnames,
								  DWORD dwFlags);
	virtual void OnRequestGetValue(CSecID id,
								   const CString &sKey,
								   DWORD dwServerData);


	DWORD GetSessionID();
//members
private:

	DWORD m_dwSessionID;
};

#endif // !defined(AFX_IPCBURNDATACARRIER_H__0605CE26_8A97_4AEF_AA76_E81AF2BCDB86__INCLUDED_)
