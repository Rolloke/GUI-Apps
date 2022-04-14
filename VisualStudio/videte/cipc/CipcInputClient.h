/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CipcInputClient.h $
// ARCHIVE:		$Archive: /Project/CIPC/CipcInputClient.h $
// CHECKIN:		$Date: 16.04.04 12:10 $
// VERSION:		$Revision: 17 $
// LAST CHANGE:	$Modtime: 16.04.04 11:48 $
// BY AUTHOR:	$Author: Georg.feddern $
// $Nokeywords:$
	
#ifndef _CIPCInputClient_H_
#define _CIPCInputClient_H_

#include "CipcInput.h"

typedef CIPCInputRecord  *CIPCInputRecordPtr;

WK_PTR_ARRAY(CIPCInputRecordArray,CIPCInputRecordPtr)

class AFX_EXT_CLASS CIPCInputClient : public CIPCInput
{
	// construction / destruction
public:
	CIPCInputClient(LPCTSTR shmName, BOOL asMaster);
	virtual ~CIPCInputClient();

	// implementation
	CIPCInputRecord *GetInputRecordPtrFromSecID(CSecID id) const;
	const CIPCInputRecord& GetInputRecordFromSecID(CSecID id) const;
	const CIPCInputRecord& GetInputRecordFromIndex(int i) const;
	inline int	GetNumberOfInputs() const;

	// overridables
	virtual void OnReadChannelFound();
	virtual void OnConfirmConnection();
	virtual void OnConfirmInfoInputs(WORD wGroupID, int iNumGroups, int numRecords, const CIPCInputRecord records[]);
	virtual void OnConfirmHardware(WORD wGroupID,int iErrorCode);
	virtual void OnConfirmAlarmState(WORD wGroupID,DWORD inputMask);
	virtual void OnIndicateAlarmState(WORD wGroupID,DWORD inputMask, DWORD changeMask,
					LPCTSTR szString);	// 1 high, 0 low; 1 changed, 0 unchanged
	virtual	void OnIndicateAlarmNr(CSecID id,
								   BOOL bAlarm,
								   DWORD dwData1,
								   DWORD dwData2);

	// has to be overwritten by derived classes
	virtual void OnAddRecord(const CIPCInputRecord& pRec);
	virtual void OnUpdateRecord(const CIPCInputRecord& pRec);
	virtual void OnDeleteRecord(const CIPCInputRecord& pRec);

private:
	CIPCInputClient(const CIPCInputClient& src);		// no implementation, to avoid implicit generation!
	void operator=(const CIPCInputClient& src);	// no implementation, to avoid implicit generation!

	CIPCInputRecordArray	m_Array;
};

inline int CIPCInputClient::GetNumberOfInputs() const
{
	return m_Array.GetSize();
}

#endif	// _CIPCInputClient_H_

