/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CIPCPipeIST.h $
// ARCHIVE:		$Archive: /Project/Capi/ISDNUnit/CIPCPipeIST.h $
// CHECKIN:		$Date: 27.06.03 9:05 $
// VERSION:		$Revision: 6 $
// LAST CHANGE:	$Modtime: 26.06.03 13:30 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef CIPCPipeIST_H
#define CIPCPipeIST_H

#include "CIPCPipeISDN.h"

/////////////////////////////////////////////////////////////////////////////
class CIPCPipeIST : public CIPCPipeISDN, public CIPC
{
public:
	CIPCPipeIST(CIPCType t,
				CISDNConnection* pConnection,
				LPCTSTR pSMName,
				BOOL bAsMaster,
				DWORD dwOptions,
				WORD wID,
				DWORD dwRequestCounter,
				BOOL bIsAlarm=FALSE
				);
	virtual ~CIPCPipeIST();

	virtual CIPC *GetCIPC();	// non const!

	BOOL PushCipcCmd(DWORD dwDataLen, const void *pData,
				DWORD dwCmd,DWORD dwParam1=0, DWORD dwParam2=0, 
				DWORD dwParam3=0, DWORD dwParam4=0);
protected:
	// CIPC functions
	virtual void OnReadChannelFound();
	virtual void OnRequestDisconnect();

	virtual BOOL HandleCmd(DWORD dwCmd,
						  DWORD dwParam1, DWORD dwParam2,
						  DWORD dwParam3, DWORD dwParam4,
						  const CIPCExtraMemory* pExtraMem
						  );
};


#endif