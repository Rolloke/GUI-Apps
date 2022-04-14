#pragma once
#include "cipcinputclient.h"

#define TSTBIT(w,b)    ((BOOL)((DWORD)(w) & (1L << (b)) ? TRUE : FALSE))
#define MAX_NUMBER_OF_INPUTS 32

class CIPCInputRecord;
class CMCLTCtrl;

class CIPCInputOCX :
	public CIPCInputClient
{
	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////
public:
	CIPCInputOCX(CMCLTCtrl* pMCLTCtrl, LPCTSTR szShmName, WORD wCodePage);
	~CIPCInputOCX(void);

	//////////////////////////////////////////////////////////////////////
	// overrides
	//////////////////////////////////////////////////////////////////////
protected:

	virtual void			OnAddRecord(const CIPCInputRecord& pRec);
	virtual void			OnUpdateRecord(const CIPCInputRecord& pRec);
	virtual void			OnConfirmConnection();
	virtual void			OnConfirmInfoInputs(WORD wGroupID, 
		int iNumGroups, 
		int numRecords, 
		const CIPCInputRecord records[]);

	virtual void			OnIndicateAlarmState(WORD wGroupID,
		DWORD inputMask, 
		DWORD changeMask,	
		LPCTSTR sInfoString);

	//////////////////////////////////////////////////////////////////////
	// members
	//////////////////////////////////////////////////////////////////////
private:
	CIPCInputRecordArray	m_paInputRecords;			//pointerarray of all
	CMCLTCtrl*				m_pMCLTCtrl;
	//input records (alarm detectors)
};
//////////////////////////////////////////////////////////////////////
