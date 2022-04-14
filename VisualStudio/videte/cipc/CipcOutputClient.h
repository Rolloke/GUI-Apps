/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CipcOutputClient.h $
// ARCHIVE:		$Archive: /Project/CIPC/CipcOutputClient.h $
// CHECKIN:		$Date: 16.04.04 12:10 $
// VERSION:		$Revision: 17 $
// LAST CHANGE:	$Modtime: 16.04.04 11:49 $
// BY AUTHOR:	$Author: Georg.feddern $
// $Nokeywords:$
	
#ifndef _CIPCOutputClient_H_
#define _CIPCOutputClient_H_

#include "CipcOutput.h"
#include "CipcOutputRecord.h"
#include "PictureRecord.h"


class AFX_EXT_CLASS CIPCOutputClient : public CIPCOutput
{
	// construction / destruction
public:
	CIPCOutputClient(LPCTSTR shmName, BOOL asMaster);
	virtual ~CIPCOutputClient();

	// implementation
	CIPCOutputRecord* GetOutputRecordPtrFromSecID(CSecID id);
	const CIPCOutputRecord& GetOutputRecordFromSecID(CSecID id);
	const CIPCOutputRecord& GetOutputRecordFromIndex(int i);
	inline int	GetNumberOfOutputs() const;

	// overridables
	virtual void OnReadChannelFound();
	virtual void OnConfirmConnection();
	virtual void OnConfirmHardware(WORD wGroupID,	
									int errorCode,	// 0==okay, !=0 some error
									BOOL bCanSWCompress,
									BOOL bCanSWDecompress,
									BOOL bCanColorCompress,
									BOOL bCanColorDecompress,
									BOOL bCanOverlay
									);
	virtual void OnConfirmInfoOutputs(WORD wGroupID, int iNumGroups, int numRecords, const CIPCOutputRecord records[]);
	virtual void OnConfirmSetRelay(CSecID relayID, BOOL bClosed);
	//
	virtual void OnAddRecord(const CIPCOutputRecord &pRec);
	virtual void OnUpdateRecord(const CIPCOutputRecord &pRec);
	virtual void OnDeleteRecord(const CIPCOutputRecord &pRec);

private:
	CIPCOutputClient(const CIPCOutputClient& src);		// no implementation, to avoid implicit generation!
	void operator=(const CIPCOutputClient& src);	// no implementation, to avoid implicit generation!

	CIPCOutputRecordArray	m_Array;
	CIPCOutputRecord *m_pLastRecord;	// mini cache
};

inline int CIPCOutputClient::GetNumberOfOutputs() const
{
	return m_Array.GetSize();
}

#endif	// _CIPCOutputClient_H_
