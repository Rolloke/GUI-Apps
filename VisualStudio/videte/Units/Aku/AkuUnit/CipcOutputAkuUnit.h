/////////////////////////////////////////////////////////////////////////////
// PROJECT:		AkuUnit
// FILE:		$Workfile: CipcOutputAkuUnit.h $
// ARCHIVE:		$Archive: /Project/Units/Aku/AkuUnit/CipcOutputAkuUnit.h $
// CHECKIN:		$Date: 27.02.04 9:53 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 27.02.04 9:02 $
// BY AUTHOR:	$Author: Georg.feddern $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef _CIPCOutputAkuUnit_H_
#define _CIPCOutputAkuUnit_H_
#include "CipcOutput.h"

class CAku;

class CIPCOutputAkuUnit : public CIPCOutput
{
public:
	CIPCOutputAkuUnit(CAkuUnitDlg* pMainWnd, CAku *pAku, LPCTSTR shmName);
	~CIPCOutputAkuUnit();
	BOOL IsValid();

	//
	void OnRequestDisconnect();
   // RequestXXX
	virtual void OnRequestVersionInfo(WORD wGroupID);
	virtual void OnRequestReset(WORD wGroupID);	
	virtual void OnRequestSetRelay(CSecID relayID, BOOL bClosed);
	virtual void OnRequestCurrentState(WORD wGroupID);
	virtual void OnRequestHardware(WORD wGroupID);
	virtual void OnRequestSetGroupID(WORD wGroupID);

	DWORD	GetAllRelaisState();
	void	SetAllRelaisState(DWORD dwRelaisState);

private:
	CAku*			m_pAku;
	CAkuUnitDlg*	m_pMainWnd;
	WORD			m_wGroupID;
	DWORD			m_dwHardwareState;

	BOOL			m_bOK;
};

#endif	// _CIPCOutputAkuUnit_H_