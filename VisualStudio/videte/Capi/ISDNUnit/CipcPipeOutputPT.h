/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CipcPipeOutputPT.h $
// ARCHIVE:		$Archive: /Project/Capi/ISDNUnit/CipcPipeOutputPT.h $
// CHECKIN:		$Date: 27.06.03 9:05 $
// VERSION:		$Revision: 11 $
// LAST CHANGE:	$Modtime: 26.06.03 13:30 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef CIPCPipeOutputPT_H
#define CIPCPipeOutputPT_H

#include "CIPCPipeISDN.h"
#include "CIPCOutput.h"

/////////////////////////////////////////////////////////////////////////////
class CIPCPipeOutputPT : public CIPCPipeISDN, public CIPCOutput
{
public:
	CIPCPipeOutputPT(CIPCType t,
				CISDNConnection* pConnection,
				LPCTSTR pSMName,
				BOOL bAsMaster,
				DWORD dwOptions,
				WORD wID,
				DWORD dwRequestCounter,
				BOOL bIsAlarm=FALSE
				);
	virtual ~CIPCPipeOutputPT();

	virtual CIPC *GetCIPC();	// non const!
protected:
		// CIPC functions
	virtual void OnReadChannelFound();
	virtual void OnRequestDisconnect();
	//
	virtual void OnRequestInfoOutputs(WORD wGroupID);
	virtual void OnRequestBitrate();
	//
	virtual void OnRequestEncodedVideo(
								CSecID camID, 
								Resolution  res,
								Compression comp,
								WORD	wNumPictures,
								DWORD	dwBitrate,
								DWORD	dwUserData=0
							   );
	//
	virtual void OnRequestSetRelay(CSecID realyID, BOOL bClosed);
	//
	virtual void OnRequestGetValue(
						CSecID id, // might be used as group ID only
						const CString &sKey,
						DWORD dwServerData
					);

	virtual void OnRequestSetValue(
						CSecID id, // might be used as group ID only
						const CString &sKey,
						const CString &sValue,
						DWORD dwServerData
						);
};


#endif