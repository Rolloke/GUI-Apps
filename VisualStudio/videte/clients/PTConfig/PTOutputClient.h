
#ifndef _CPTOutputClient_h
#define _CPTOutputClient_h

#include "CIPCOutputClient.h"

class CPTConfigDoc;

class CPTOutputClient : public CIPCOutputClient
{
public:
	CPTOutputClient(CPTConfigDoc *pDoc, const char *shmName);
	~CPTOutputClient();
//
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
	virtual void OnAddRecord(const CIPCOutputRecord &pRec);
	virtual void OnUpdateRecord(const CIPCOutputRecord &pRec);
	virtual void OnDeleteRecord(const CIPCOutputRecord &pRec);
	//
	virtual void OnConfirmGetValue(
					CSecID id, // might be used as group ID only
					const CString &sKey,
					const CString &sValue,
					DWORD dwServerData
					);

private:
	CPTConfigDoc *m_pDoc;
};

#endif