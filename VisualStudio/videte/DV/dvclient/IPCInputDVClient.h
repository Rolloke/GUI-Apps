// IPCInputDVClient.h: interface for the CIPCInputDVClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPCINPUTDVCLIENT_H__6B5BBEDE_9757_11D3_A870_004005A19028__INCLUDED_)
#define AFX_IPCINPUTDVCLIENT_H__6B5BBEDE_9757_11D3_A870_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CServer;

class CIPCInputDVClient : public CIPCInputClient  
{
	// construction/destruction
public:
	CIPCInputDVClient(LPCTSTR shmName, CServer* pServer);
	virtual ~CIPCInputDVClient();

	// attributes
public:
	BOOL GetAlarmState(WORD wCameraNr);
	inline CSecID	GetComPortInputID() const;

	// overrides
public:
	virtual void OnConfirmConnection();
	virtual void OnRequestDisconnect();

	// alarm states
	virtual void OnConfirmInfoInputs(WORD wGroupID, int iNumGroups, int numRecords, const CIPCInputRecord records[]);
	virtual void OnAddRecord(const CIPCInputRecord& rec);
	virtual void OnUpdateRecord(const CIPCInputRecord& rec);
	virtual void OnDeleteRecord(const CIPCInputRecord& rec);

	virtual	void OnIndicateAlarmNr(CSecID id,
								   BOOL bAlarm,
								   DWORD dwData1,
								   DWORD dwData2);
	
	virtual	void OnConfirmGetSystemTime(const CSystemTime& st);
	virtual	void OnConfirmSetSystemTime();

	virtual	void OnConfirmGetTimeZoneInformations(const CTimeZoneInformations& tzi);
	virtual void OnConfirmGetTimeZoneInformation(CTimeZoneInformation TimeZone);
	virtual	void OnConfirmSetTimeZoneInformation();

	virtual void OnConfirmGetValue(CSecID id,
									const CString &sKey,
									const CString &sValue,
									DWORD dwServerData);

	virtual void OnConfirmSetValue(CSecID id,
									const CString &sKey,
									const CString &sValue,
									DWORD dwServerData);
	virtual void OnIndicateError(DWORD dwCmd, 
							     CSecID id, 
								 CIPCError error, 
								 int iErrorCode,
								 const CString &sErrorMessage);
	virtual void OnConfirmGetFile(int iDestination,
								  const CString &sFileName,
								  const void *pData,
								  DWORD dwDataLen);
	// data member
private:
	CServer*	m_pServer;
	CSecID		m_idComPortInput;
};
/////////////////////////////////////////////////////////////////////
inline CSecID CIPCInputDVClient::GetComPortInputID() const
{
	return m_idComPortInput;
}

#endif // !defined(AFX_IPCINPUTDVCLIENT_H__6B5BBEDE_9757_11D3_A870_004005A19028__INCLUDED_)
