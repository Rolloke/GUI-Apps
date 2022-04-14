/////////////////////////////////////////////////////////////////////////////
// PROJECT:		USBCamUnit
// FILE:		$Workfile: CipcInputIPcamunit.h $
// ARCHIVE:		$Archive: /Project/Units/IPCameraUnit/CipcInputIPcamunit.h $
// CHECKIN:		$Date: 28.08.06 13:01 $
// VERSION:		$Revision: 5 $
// LAST CHANGE:	$Modtime: 28.08.06 12:46 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef _CIPCInputIpCamUnit_H_
#define _CIPCInputIpCamUnit_H_

#include "CipcInput.h"

class CIpCameraUnitDlg;
class CHttpResult;
class CHttpRequestThread;

class CIPCInputIpCamUnit : public CIPCInput
{
public:
	CIPCInputIpCamUnit(CIpCameraUnitDlg* pDlg, LPCTSTR shmName);
	virtual ~CIPCInputIpCamUnit();

	//attributes
public:
	BOOL IsValid();
	BOOL IsAlarmActive(int nInput);
	inline BOOL DoReset();
	inline const CStringArray& GetInputs();
	DWORD	GetAlarmState();
	inline WORD GetGroupID() const;

	// operations
public:
	inline void CheckInputs();
protected:
	void ConfirmReset();
	BOOL CheckGroupID(WORD wGid, LPCTSTR sFunction);
	void InitInputStrings();
	BOOL DoRequestInputs(const CString& sInputs);
	void	AlarmStateChanged(WORD wInput, BOOL bSet);

	virtual BOOL Run(DWORD dwTimeOut);
   // RequestXXX
 	virtual void OnRequestSetGroupID(WORD wGroupID);
	virtual void OnRequestHardware(WORD wGroupID);
	virtual void OnRequestSetEdge(WORD wGroupID,DWORD edgeMask);	// 1 positive, 0 negative
	virtual void OnRequestSetFree(WORD wGroupID,DWORD openMask);	// 1 open, 0 closed
	virtual void OnRequestReset(WORD wGroupID);	
	virtual void OnRequestAlarmState(WORD wGroupID);
	virtual void OnRequestDisconnect();
	virtual void OnWriteCommData(CSecID id, const CIPCExtraMemory &data, DWORD dwBlockNr);
	virtual void OnRequestCameraControl(CSecID commID, CSecID camID, CameraControlCmd cmd, DWORD dwValue);
	virtual void OnRequestGetValue(CSecID id, const CString &sKey, DWORD dwUserData);
	
	// ResponseXXX
	virtual void OnResponseAlarmState(WORD wGroupID);
	virtual void OnResponseInputs(WORD wGroupID);

//	for asychronus input request in a thread
//	static void CALLBACK OnConfirmResults(CHttpResult*pResult, LPARAM lParam);
//	void OnConfirmResults(CHttpResult*pResult);


protected:
	CIpCameraUnitDlg*m_pDlg;
	WORD			m_wGroupID;
	CStringArray	m_saInputs;
//	CHttpRequestThread *m_pRequestThread;

	BOOL			m_bOK;
	BOOL			m_bReset;
	volatile BOOL	m_bCheck;
	BOOL			m_bMDinput;
	DWORD			m_dwAlarmState;
	DWORD			m_dwFree;
	DWORD			m_dwEdge;
};

inline BOOL CIPCInputIpCamUnit::DoReset()
{
	return m_bReset;
}

inline void CIPCInputIpCamUnit::CheckInputs()
{
	m_bCheck = TRUE;
}

inline const CStringArray& CIPCInputIpCamUnit::GetInputs()
{
	return m_saInputs;
}
inline WORD CIPCInputIpCamUnit::GetGroupID() const
{
	return m_wGroupID;
}

#endif	// _CIPCInputIpCamUnit_H_
