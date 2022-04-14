// CIPCMediaClient.h: interface for the CIPCMediaClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CIPCMEDIACLIENT_H__A78BB03B_7015_45EC_87F3_AFE35476816C__INCLUDED_)
#define AFX_CIPCMEDIACLIENT_H__A78BB03B_7015_45EC_87F3_AFE35476816C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CIPCMedia.h"
#include "CIPCMediaRecord.h"

class AFX_EXT_CLASS CIPCMediaClient : public CIPCMedia  
{
public:
	CIPCMediaClient(LPCTSTR shmName, BOOL bAsMaster);
	virtual ~CIPCMediaClient();

	//!ic! implementation
	CIPCMediaRecord *GetMediaRecordPtrFromSecID(CSecID id) const;
	const CIPCMediaRecord& GetMediaRecordFromSecID(CSecID id) const;
	const CIPCMediaRecord& GetMediaRecordFromIndex(int i) const;
	int	GetNumberOfMedia() {return m_Array.GetSize();}

	//!ic! overridables
	virtual void OnReadChannelFound();
	virtual void OnConfirmConnection();
	virtual	void OnConfirmInfoMedia(WORD, int, int, const CIPCMediaRecordArray&);

	//!ic! has to be overwritten by derived classes
	// called, when a record is added to the array
	virtual void OnAddRecord(const CIPCMediaRecord& pRec);
	// called, when a record is updated to the array
	virtual void OnUpdateRecord(const CIPCMediaRecord& pRec);
	// called, when a record is deleted from the array
	virtual void OnDeleteRecord(const CIPCMediaRecord& pRec);

private:
	CIPCMediaRecordArray m_Array;
};

#endif // !defined(AFX_CIPCMEDIACLIENT_H__A78BB03B_7015_45EC_87F3_AFE35476816C__INCLUDED_)
