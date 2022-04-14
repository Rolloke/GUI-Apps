
#ifndef _CPTInputClient_h
#define _CPTInputClient_h

#include "CIPCInputClient.h"

class CPTConfigDoc;

class CPTInputClient : public CIPCInputClient
{
public:
	CPTInputClient(CPTConfigDoc *pDoc,const char *shmName);
	~CPTInputClient();
//
	virtual void OnReadChannelFound();
	virtual void OnConfirmConnection();
	virtual void OnConfirmInfoInputs(WORD wGroupID, int iNumGroups, int numRecords, const CIPCInputRecord records[]);
	virtual void OnAddRecord(const CIPCInputRecord &pRec);
	virtual void OnUpdateRecord(const CIPCInputRecord &pRec);
	virtual void OnDeleteRecord(const CIPCInputRecord &pRec);
private:
	CPTConfigDoc *m_pDoc;
};

#endif