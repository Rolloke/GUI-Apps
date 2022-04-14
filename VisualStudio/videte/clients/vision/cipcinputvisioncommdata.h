/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CIPCInputVisionCommData.h $
// ARCHIVE:		$Archive: /Project/Clients/Vision/CIPCInputVisionCommData.h $
// CHECKIN:		$Date: 25.02.04 12:31 $
// VERSION:		$Revision: 4 $
// LAST CHANGE:	$Modtime: 25.02.04 11:26 $
// BY AUTHOR:	$Author: Georg.feddern $
// $Nokeywords:$
	
#ifndef _CIPCINPUTVISIONCOMMDATA_H_
#define _CIPCINPUTVISIONCOMMDATA_H_

class CVisionDoc;
//////////////////////////////////////////////////////////////////
class CIPCInputVisionCommData : public CIPCInput
{
	// construction / destruction
public:
	CIPCInputVisionCommData(LPCTSTR shmName,CVisionDoc* pDoc);
	~CIPCInputVisionCommData();

	// overridables
public:
	virtual void OnReadChannelFound();
	virtual void OnConfirmConnection();
	virtual void OnConfirmInfoInputs(WORD wGroupID, int iNumGroups, int numRecords, const CIPCInputRecord records[]);
	virtual void OnRequestDisconnect();

	virtual void OnIndicateCommData(CSecID id,
									const CIPCExtraMemory &data,
									DWORD dwBlockNr
									);

	virtual void OnConfirmHardware(WORD wGroupID,int iErrorCode);
	virtual void OnIndicateAlarmState(WORD wGroupID,DWORD inputMask, DWORD changeMask,
					LPCTSTR szString);	// 1 high, 0 low; 1 changed, 0 unchanged
public:
	CSecID	m_idCommPort;
private:
	CVisionDoc*	m_pVisionDoc;
};
//////////////////////////////////////////////////////////////////
#endif	// _LocalInput_H_