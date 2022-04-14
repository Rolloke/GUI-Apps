/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CipcPipeInputPT.h $
// ARCHIVE:		$Archive: /Project/Capi/ISDNUnit/CipcPipeInputPT.h $
// CHECKIN:		$Date: 27.06.03 9:05 $
// VERSION:		$Revision: 7 $
// LAST CHANGE:	$Modtime: 26.06.03 13:30 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef CIPCPipeInputPT_H
#define CIPCPipeInputPT_H

#include "CIPCPipeISDN.h"
#include "CIPCInput.h"

/////////////////////////////////////////////////////////////////////////////
class CIPCPipeInputPT : public CIPCPipeISDN, public CIPCInput
{
public:
	CIPCPipeInputPT(CIPCType t,
				CISDNConnection* pConnection,
				LPCTSTR pSMName,
				BOOL bAsMaster,
				DWORD dwOptions,
				WORD wID,
				DWORD dwRequestCounter,
				BOOL bIsAlarm=FALSE
				);
	virtual ~CIPCPipeInputPT();

	virtual CIPC *GetCIPC();	// non const!
protected:
	// CIPC functions
	virtual void OnReadChannelFound();
	virtual void OnRequestDisconnect();
	//
	virtual void OnRequestInfoInputs(WORD wGroupID);
	virtual void OnRequestBitrate();

	virtual	void OnRequestGetSystemTime();
	virtual	void OnRequestSetSystemTime(const CSystemTime& st);
	virtual	void OnConfirmAlarm(CSecID id);

};


#endif