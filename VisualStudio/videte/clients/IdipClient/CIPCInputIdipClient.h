// CIPCInputIdipClient.h
// Author : Uwe Freiwald


#ifndef _CIPCInputIdipClient_H
#define _CIPCInputIdipClient_H

#include <SecIDArray.h>

class CServer;

/////////////////////////////////////////////////////////////////////
class CIPCInputIdipClient : public CIPCInputClient, public CObject
{
	DECLARE_DYNAMIC(CIPCInputIdipClient)
	// construction / destruction
public:
	CIPCInputIdipClient(LPCTSTR szShmName, CServer* pServer, CSecID id, WORD wCodePage);
	~CIPCInputIdipClient();

	// attributes
public:
	inline BOOL		HasAlarmOffSpanFeature() const;
	inline BOOL		GotInfo() const;
	inline CSecID	GetCommID() const;
	inline CServer* GetServer() const;

	// operations
public:
	//ML 30.9.99 Begin Insertation{
	WORD   GetNrFromAlarmID(CSecID alarmID);
//ML 30.9.99 End Insertation{
	inline CSystemTimeSpans* GetAlarmOffSpans();
	CString GetInputNameFromID(CSecID id);
	void	UpdateServerTimeOffset();

public:
	void Lock();
	void Unlock();

	// implementation
public:
	void	RequestSetAlarmOffSpans(CSecID id, const CSystemTimeSpans& spans);
	void	DoRequestWWWDir();

	virtual BOOL	Run(DWORD dwTimeOut);	// default is GetTimeoutCmdReceive()
	//overridables
protected:
	virtual void OnReadChannelFound();
	virtual void OnConfirmConnection();
	virtual void OnConfirmInfoInputs(WORD wGroupID, int iNumGroups, int numRecords, const CIPCInputRecord records[]);
	virtual void OnAddRecord(const CIPCInputRecord& pRec);
	virtual void OnUpdateRecord(const CIPCInputRecord& pRec);
	virtual void OnDeleteRecord(const CIPCInputRecord& pRec);
	virtual void OnIndicateCommData(CSecID id,
									const CIPCExtraMemory &data,
									DWORD dwBlockNr
									);
	virtual void OnConfirmAlarmState(WORD wGroupID, DWORD inputMask);

	virtual void OnRequestDisconnect();

//ML 29.9.99 Begin Insertation{
	virtual void OnIndicateAlarmState(WORD wGroupID,
									  DWORD inputMask, 	// 1 high, 0 low
									  DWORD changeMask,	// 1 changed, 0 unchanged
									  LPCTSTR sInfoString
									  );
//ML 29.9.99 End Insertation{
	virtual	void OnIndicateAlarmNr(CSecID id,
								   BOOL bAlarm,
								   DWORD dwData1,
								   DWORD dwData2);

	// value confirms
	virtual void OnConfirmGetValue(CSecID id, 
								   const CString &sKey, 
								   const CString &sValue, 
								   DWORD dwServerData);
	virtual void OnConfirmGetFile(	int iDestination,
									const CString &sFileName,
									const void *pData,
									DWORD dwDataLen, DWORD dwCodePage);
	virtual	void OnConfirmGetSystemTime(const CSystemTime& st);
	virtual	void OnConfirmSetSystemTime();

	virtual void OnIndicateError(DWORD dwCmd, CSecID id, 
								 CIPCError error, int iErrorCode,
								 const CString &sErrorMessage);
	
	virtual	void OnConfirmGetAlarmOffSpans(CSecID id,			 /*RM*/
										   int iNumRecords,
										   const CSystemTimeSpan data[]);		


	virtual	void OnConfirmSetAlarmOffSpans(CSecID id);			 /*RM*/

private:
	void	WriteAlarmOffSpanProtocol(CSecID idInput);

	// data members
private:
	CSecID				m_idCommPort;
	CServer*			m_pServer;
	CSecID				m_ID;
	BOOL				m_bGotInfo;
	BOOL				m_bAlarmOffSpanFeature;
	DWORD				m_dwTime;
	CCriticalSection	m_csInputs;
	CSystemTimeSpans	m_AlarmOffSpans;
};
/////////////////////////////////////////////////////////////////////
inline BOOL CIPCInputIdipClient::HasAlarmOffSpanFeature() const
{
	return m_bAlarmOffSpanFeature;
}
/////////////////////////////////////////////////////////////////////
inline BOOL CIPCInputIdipClient::GotInfo() const
{
	return m_bGotInfo;
}
/////////////////////////////////////////////////////////////////////
inline CSecID CIPCInputIdipClient::GetCommID() const
{
	return m_idCommPort;
}
/////////////////////////////////////////////////////////////////////
inline CServer* CIPCInputIdipClient::GetServer() const
{
	return m_pServer;
}
/////////////////////////////////////////////////////////////////////
inline CSystemTimeSpans* CIPCInputIdipClient::GetAlarmOffSpans()
{
	return &m_AlarmOffSpans;
}


#endif	// _CIPCInputIdipClient_H_