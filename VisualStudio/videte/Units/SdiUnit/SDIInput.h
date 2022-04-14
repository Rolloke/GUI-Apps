/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: SDIInput.h $
// ARCHIVE:		$Archive: /Project/Units/SDIUnit/SDIInput.h $
// CHECKIN:		$Date: 7/20/06 2:20p $
// VERSION:		$Revision: 12 $
// LAST CHANGE:	$Modtime: 7/20/06 12:41p $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef SDIInput_H
#define SDIInput_H

#include "CipcInput.h"

////////////////////////////////////////////////////////
class CSDIControlRecord;

class CSDIInput : public CIPCInput
{
	// construction / destruction
public:
	CSDIInput(LPCTSTR shmName, CSDIControlRecord* pControlRecord);
	~CSDIInput();

	// Implementation
	void	ConfirmSetValue(const CString &sValue);

	// member variables access
	WORD	GetGroupID() const {return m_wGroupID;}
	void	IndicateAlarm(DWORD dwMask, const CIPCFields& DataFields, BOOL bAlarm=TRUE);
	void	IndicateAlarm(WORD wSubID, BOOL bAlarm, const CIPCFields& DataFields);
	void	UpdateAlarm(WORD wSubID, const CIPCFields& DataFields);

	BOOL    GetAlarmState(WORD wSubID);

	// overridables
public:	
	// requests
	virtual void OnRequestSetGroupID(WORD wGroupID);
	virtual void OnRequestHardware(WORD wGroupID);
	virtual void OnRequestSetEdge(WORD wGroupID,DWORD edgeMask);	// 1 positive, 0 negative
	virtual void OnRequestSetFree(WORD wGroupID,DWORD openMask);	// 1 open, 0 closed
	virtual void OnRequestReset(WORD wGroupID);	
	virtual void OnRequestAlarmState(WORD wGroupID);
	virtual void OnRequestDisconnect();
	virtual void OnRequestGetValue(
						CSecID id, // might be used as group ID only
						const CString &sKey,
						DWORD dwServerData
					);
	virtual void OnRequestSetValue(
						CSecID id, // might be used as group ID only
						const CString &sKey,
						const CString &sValue,
						DWORD dwServerData
						);
	// private data
private:
	WORD			m_wGroupID;
	DWORD			m_dwOpenMask;
	DWORD			m_dwAlarmState;

	CSDIControlRecord*	m_pControlRecord;
	DWORD				m_dwServerData;
	CString				m_sKey;
};
/////////////////////////////////////////////////////////////////////////////////
WK_PTR_ARRAY(CSDIInputs,CSDIInput*);
/////////////////////////////////////////////////////////////////////////////////
#endif	// SDIInput_H
