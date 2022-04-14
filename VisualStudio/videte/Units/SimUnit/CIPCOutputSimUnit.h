
#ifndef _CIPCOutputSimUnit_H_
#define _CIPCOutputSimUnit_H_

#include "CipcOutput.h"

class CSimUnitDlg;

class CIPCOutputSimUnit : public CIPCOutput
{
	// construction / destruction
public:
	CIPCOutputSimUnit(CSimUnitDlg* pDlg, LPCTSTR shmName);
	~CIPCOutputSimUnit();

	// overridables
	virtual void OnRequestConnection();
	virtual void OnRequestDisconnect();
	virtual void OnRequestVersionInfo(WORD wGroupID);

	virtual void OnRequestReset(WORD wGroupNr);	
	virtual void OnRequestSetGroupID(WORD wGroupID);
	virtual void OnRequestHardware(WORD wGroupNr);

	virtual void OnRequestCurrentState(WORD wGroupNr);
	virtual void OnRequestSetRelay(CSecID relayID, BOOL bClosed);
private:
	WORD			m_wGroupID;
	CSimUnitDlg*	m_pDlg;

};

#endif	// _CIPCOutputSimUnit_H_