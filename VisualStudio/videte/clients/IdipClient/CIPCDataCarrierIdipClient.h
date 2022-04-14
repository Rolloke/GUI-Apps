// CIPCDataCarrierIdipClient.h: interface for the CIPCDataCarrierIdipClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CIPCDataCarrierIdipClient_H__6469EA17_8EFB_49A1_9399_B8B3B14B976A__INCLUDED_)
#define AFX_CIPCDataCarrierIdipClient_H__6469EA17_8EFB_49A1_9399_B8B3B14B976A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CIdipClientDoc;

class CIPCDataCarrierIdipClient : public CIPCDataCarrier  
{
public:
	CIPCDataCarrierIdipClient(LPCTSTR shmName, CIdipClientDoc* pDoc, CSecID id);
	virtual ~CIPCDataCarrierIdipClient();

	// attributes
public:
	inline DWORD	GetVersion() const;
	inline BOOL		GotVolumeInfo() const;
	inline BOOL		IsReady() const;

	// operations
protected:

	// overrides
protected:
	virtual void OnReadChannelFound();
	virtual void OnConfirmConnection();
	virtual void OnRequestDisconnect();
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
	CIdipClientDoc*		m_pIdipClientDoc;
	CSecID				m_ID;
	CCriticalSection	m_cs;
	BOOL				m_bGotVolumeInfo;
	DWORD				m_dwServerVersion;
	BOOL				m_bIsReady;

};
/////////////////////////////////////////////////////////////////////
inline DWORD CIPCDataCarrierIdipClient::GetVersion() const
{
	return m_dwServerVersion;
}
/////////////////////////////////////////////////////////////////////
inline BOOL CIPCDataCarrierIdipClient::GotVolumeInfo() const
{
	return m_bGotVolumeInfo;
}
/////////////////////////////////////////////////////////////////////
inline BOOL CIPCDataCarrierIdipClient::IsReady() const
{
	return m_bIsReady;
}
/////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_CIPCDataCarrierIdipClient_H__6469EA17_8EFB_49A1_9399_B8B3B14B976A__INCLUDED_)
