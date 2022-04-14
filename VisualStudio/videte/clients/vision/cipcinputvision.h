// CIPCInputVision.h
// Author : Uwe Freiwald


#ifndef _CIPCINPUTVISION_H
#define _CIPCINPUTVISION_H

#include <SecIDArray.h>

class CServer;

/////////////////////////////////////////////////////////////////////
class CIPCInputVision : public CIPCInputClient
{
	// construction / destruction
public:
	CIPCInputVision(LPCTSTR shmName, CServer* pServer, CSecID id);
	~CIPCInputVision();

	// attributes
public:
	inline BOOL		HasAlarmOffSpanFeature() const;
	inline CSecID	GetCommID() const;
	inline CServer* GetServer() const;

	// operations
public:
	//ML 30.9.99 Begin Insertation{
	WORD   GetNrFromAlarmID(CSecID alarmID);
//ML 30.9.99 End Insertation{
	inline CSystemTimeSpans* GetAlarmOffSpans();
	CString GetInputNameFromID(CSecID id);

public:
	void Lock();
	void Unlock();

	// implementation
public:
	void	RequestSetAlarmOffSpans(CSecID id, const CSystemTimeSpans& spans);

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
	void	RequestWWWDir();
	void	WriteAlarmOffSpanProtocol(CSecID idInput);

	// data members
private:
	CSecID				m_idCommPort;
	CServer*			m_pServer;
	CSecID				m_ID;
	BOOL				m_bGotInfo2;
	BOOL				m_bAlarmOffSpanFeature;
	DWORD				m_dwTime;
	CCriticalSection	m_csInputs;
	CSystemTimeSpans	m_AlarmOffSpans;
};
/////////////////////////////////////////////////////////////////////
inline BOOL CIPCInputVision::HasAlarmOffSpanFeature() const
{
	return m_bAlarmOffSpanFeature;
}
/////////////////////////////////////////////////////////////////////
inline CSecID CIPCInputVision::GetCommID() const
{
	return m_idCommPort;
}
/////////////////////////////////////////////////////////////////////
inline CServer* CIPCInputVision::GetServer() const
{
	return m_pServer;
}
/////////////////////////////////////////////////////////////////////
inline CSystemTimeSpans* CIPCInputVision::GetAlarmOffSpans()
{
	return &m_AlarmOffSpans;
}


#endif	// _CIPCInputVision_H_