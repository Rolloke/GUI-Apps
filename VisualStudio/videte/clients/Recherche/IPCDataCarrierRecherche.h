// IPCDataCarrierRecherche.h: interface for the CIPCDataCarrierRecherche class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPCDATACARRIERRECHERCHE_H__6469EA17_8EFB_49A1_9399_B8B3B14B976A__INCLUDED_)
#define AFX_IPCDATACARRIERRECHERCHE_H__6469EA17_8EFB_49A1_9399_B8B3B14B976A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CServer;

class CIPCDataCarrierRecherche : public CIPCDataCarrier  
{
public:
	CIPCDataCarrierRecherche(LPCTSTR shmName, CServer* pServer, CSecID id);
	virtual ~CIPCDataCarrierRecherche();

	// attributes
public:
	inline CServer* GetServer() const;
	inline DWORD	GetVersion() const;
	inline BOOL		GotVolumeInfo() const;

	// operations
protected:

	// overrides
protected:
	virtual void OnReadChannelFound();
	virtual void OnConfirmConnection();
	// value confirms
	virtual void OnConfirmGetValue(CSecID id, 
								   const CString &sKey, 
								   const CString &sValue, 
								   DWORD dwServerData);
	virtual void OnConfirmVersionInfo(WORD wGroupID, DWORD dwVersion);
	virtual void OnIndicateError(DWORD dwCmd, 
							     CSecID id, 
								 CIPCError error, 
								 int iErrorCode,
								 const CString &sErrorMessage);

	virtual	void OnConfirmVolumeInfos(int iNumDrives,
									  const CIPCDrive drives[]);
	// session
	virtual	void OnIndicateSessionProgress(DWORD dwSessionID, int iProgress);

	virtual	void OnConfirmSession(DWORD dwSessionID);

	virtual	void OnConfirmCancelSession(DWORD dwSessionID);

	virtual	void OnConfirmDeleteVolume(const CString& sVolume);

	// private data
private:
	CServer*			m_pServer;
	CSecID				m_ID;
	CCriticalSection	m_cs;
	BOOL				m_bGotVolumeInfo;
	DWORD				m_dwServerVersion;

};
/////////////////////////////////////////////////////////////////////
inline CServer* CIPCDataCarrierRecherche::GetServer() const
{
	return m_pServer;
}
/////////////////////////////////////////////////////////////////////
inline DWORD CIPCDataCarrierRecherche::GetVersion() const
{
	return m_dwServerVersion;
}
/////////////////////////////////////////////////////////////////////
inline BOOL CIPCDataCarrierRecherche::GotVolumeInfo() const
{
	return m_bGotVolumeInfo;
}
/////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_IPCDATACARRIERRECHERCHE_H__6469EA17_8EFB_49A1_9399_B8B3B14B976A__INCLUDED_)
