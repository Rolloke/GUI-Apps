// IPCDataCarrierClient.h: interface for the CIPCDataCarrierClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPCDATACARRIERCLIENT_H__888B13B7_A13E_4B31_8E62_EAE8576E8A71__INCLUDED_)
#define AFX_IPCDATACARRIERCLIENT_H__888B13B7_A13E_4B31_8E62_EAE8576E8A71__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CIPCDataCarrierClient : public CIPCDataCarrier  
{
public:
	CIPCDataCarrierClient(LPCTSTR shmName);
	virtual ~CIPCDataCarrierClient();

public:
	// overridables
	virtual void OnReadChannelFound();
	virtual void OnConfirmConnection();
	virtual void OnConfirmVolumeInfos(int iNumDrives, const CIPCDrive drives[]);
	virtual	void OnIndicateSessionProgress(DWORD dwSessionID, int iProgress);
	virtual	void OnConfirmSession(DWORD dwSessionID);
	virtual void OnRequestDisconnect();
	virtual void OnIndicateError(DWORD dwCmd, CSecID id, 
								 CIPCError errorCode, int iUnitCode,
								 const CString &sErrorMsg);
	virtual void OnConfirmCancelSession(DWORD dwSessionID);


	CStringArray&	GetPathNames();
	CString			GetDriveLetter();
	
//members
private:

	CString			m_sPathToBurn;
	CStringArray	m_saPathNames;
	CString			m_sDriveLetter;
	int				m_iBurnCounts;
	DWORD			m_dwTick;
};

#endif // !defined(AFX_IPCDATACARRIERCLIENT_H__888B13B7_A13E_4B31_8E62_EAE8576E8A71__INCLUDED_)
