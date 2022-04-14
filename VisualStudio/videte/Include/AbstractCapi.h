/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: AbstractCapi.h $
// ARCHIVE:		$Archive: /Project/Include/AbstractCapi.h $
// CHECKIN:		$Date: 11.10.02 14:59 $
// VERSION:		$Revision: 17 $
// LAST CHANGE:	$Modtime: 11.10.02 14:59 $
// BY AUTHOR:	$Author: Georg.feddern $
// $Nokeywords:$

#ifndef _CAbstractCapi_H
#define _CAbstractCapi_H

#include "afx.h"	// for CString

typedef int CapiHandle;
#define INVALID_CAPI_HANDLE -1

enum CapiIncomingAnswer
{
	INCOMING_ACCEPTED,
	INCOMING_WAITING,	// not yet used?
	INCOMING_REJECTED_NO_CHANNEL,
	INCOMING_REJECTED_BUSY,
	INCOMING_REJECTED_IGNORE,
	INCOMING_REJECTED
};

inline const char *NameOfEnum(CapiIncomingAnswer answer)
{
	switch (answer) {
	case INCOMING_ACCEPTED:
			return "INCOMING_ACCEPTED";
			break;
	case INCOMING_WAITING:
			return "INCOMING_WAITING";
			break;
	case INCOMING_REJECTED_NO_CHANNEL:
			return "INCOMING_REJECTED_NO_CHANNEL";
			break;
	case INCOMING_REJECTED_BUSY:
			return "INCOMING_REJECTED_BUSY";
			break;
	case INCOMING_REJECTED_IGNORE:
			return "INCOMING_REJECTED_INGORE";
			break;
	case INCOMING_REJECTED:
			return "INCOMING_REJECTED";
			break;

	default:
		return "INCOMING_INVALID";
	}
}
enum CapiOutgoingResult
{
	OUTGOING_ACCEPTED		= 1,
	OUTGOING_REJECTED		= 2,
	OUTGOING_NOT_AVAILABLE	= 3,
	OUTGOING_USER_BUSY		= 4
};


#include "hdr.h"	// SIGH	OOPS better use a class wrap

class CAbstractCapiClient;	// forward declaration

// all inlined
class CAbstractCapiCallInfo {
public:
	inline CAbstractCapiCallInfo();
	inline CAbstractCapiCallInfo(
		const CString &sCallingParty, 
		const CString &sCalledParty
		);
	// acesss
	inline const CString &GetCallingParty() const;
	inline const CString &GetCalledParty() const;
	//
	inline void SetCallingParty(const CString &sCallingParty);
	inline void SetCalledParty(const CString &sCallingParty);
private:
	CString m_sCallingParty;
	CString m_sCalledParty;
};

class CAbstractCapiClient {
public:
	// access to client data:
	virtual const CString &GetOwnNumber() const=0;	// not yet used

	virtual int GetNumChannels() const=0;			// not yet used
	//
	virtual void OnListenConfirm(WORD wListen)=0;	// not yet used
	//
	virtual CapiIncomingAnswer OnIncomingCall(	CapiHandle newCh, 
												const  CAbstractCapiCallInfo &callInfo
												)=0;

	virtual void OnIncomingOkay(CapiHandle newCh, const CString &sNumber)=0;	// not yet used
	//
	virtual long OnOutgoingDone(CapiHandle ch, CapiOutgoingResult result)=0;	// not yet used
	//
	virtual void OnNumBChannelsChanged(CapiHandle ch, WORD wChannels)=0;		// not yet used
	
	virtual void OnConnectionClosed(CapiHandle ch)=0;							// not yet used
	//
	virtual void OnReceiveData(CapiHandle ch, DEFHDR *pData)=0;
	//
	virtual void OnReceiveData(CapiHandle ch, BYTE *pData, DWORD dwDataLen)=0;

	// NEW 17.8.97
	virtual void OnCallRejected()=0;	// 0x3415
	virtual void OnUserBusy()=0;		// 0x3411
	virtual void OnUnableToConnect()=0;	// 0x34A2
	virtual void OnInfo(WORD wInfo)=0;	// always called !
	// NEW 5.9.97
	virtual void OnConnectB3() { };
	// NEW 11.10.2002 GF
	virtual void OnCapiGetMessageError()=0;
};

inline CAbstractCapiCallInfo::CAbstractCapiCallInfo()
{
	// empty strings
}
inline CAbstractCapiCallInfo::CAbstractCapiCallInfo(
			const CString &sCallingParty, 
			const CString &sCalledParty
		)
{
	m_sCallingParty = sCallingParty;
	m_sCalledParty = sCalledParty;
}
	// acesss
inline const CString &CAbstractCapiCallInfo::GetCallingParty() const
{
	return m_sCallingParty;
}
inline const CString &CAbstractCapiCallInfo::GetCalledParty() const
{
	return m_sCalledParty;
}
//
inline void CAbstractCapiCallInfo::SetCallingParty(const CString &sCallingParty)
{
	m_sCallingParty = sCallingParty;
}
inline void CAbstractCapiCallInfo::SetCalledParty(const CString &sCalledParty)
{
	m_sCalledParty = sCalledParty;
}


#endif