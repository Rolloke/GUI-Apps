/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CIPCInputIdipClientCommData.h $
// ARCHIVE:		$Archive: /Project/Clients/IdipClient/CIPCInputIdipClientCommData.h $
// CHECKIN:		$Date: 23.05.05 12:00 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 23.05.05 11:45 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
	
#ifndef _CIPCInputIdipClientCommData_H_
#define _CIPCInputIdipClientCommData_H_

class CIdipClientDoc;
//////////////////////////////////////////////////////////////////
class CIPCInputIdipClientCommData : public CIPCInput
{
	// construction / destruction
public:
	CIPCInputIdipClientCommData(LPCTSTR szShmName, CIdipClientDoc* pDoc);
	~CIPCInputIdipClientCommData();

	// overridables
public:
	virtual void OnReadChannelFound();
	virtual void OnConfirmConnection();
	virtual void OnConfirmInfoInputs(WORD wGroupID, int iNumGroups, int numRecords, const CIPCInputRecord records[]);
	virtual void OnRequestDisconnect();

	virtual void OnIndicateCommData(CSecID id, const CIPCExtraMemory &data, DWORD dwBlockNr);
	virtual void OnIndicateAlarmNr(CSecID id, BOOL bAlarm, DWORD dwData1, DWORD dwData2);

	virtual void OnConfirmHardware(WORD wGroupID,int iErrorCode);
	virtual void OnIndicateAlarmState(WORD wGroupID,DWORD inputMask, DWORD changeMask,
					LPCTSTR szString);	// 1 high, 0 low; 1 changed, 0 unchanged
	virtual void OnConfirmAlarmState(WORD wGroupID, DWORD inputMask);
public:
	CSecID	m_idCommPort;
private:
	CIdipClientDoc*	m_pDoc;
};
//////////////////////////////////////////////////////////////////
#endif	// _CIPCInputIdipClientCommData_H_