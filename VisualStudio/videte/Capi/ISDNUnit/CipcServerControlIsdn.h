/* GlobalReplace: CIPCServerControlIsdn --> CIPCServerControlISDN */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CipcServerControlIsdn.h $
// ARCHIVE:		$Archive: /Project/Capi/ISDNUnit/CipcServerControlIsdn.h $
// CHECKIN:		$Date: 9.10.03 10:18 $
// VERSION:		$Revision: 21 $
// LAST CHANGE:	$Modtime: 9.10.03 10:02 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#ifndef CIPCServerControlISDN_H
#define CIPCServerControlISDN_H

#include "CIPCServerControl.h"

/////////////////////////////////////////////////////////////////////////////
class CIPCServerControlISDN : public CIPCServerControl
{
// Implementation
public:
	CIPCServerControlISDN(LPCTSTR psSMName, BOOL bAMaster);
	~CIPCServerControlISDN();

	void DoNewIndicateError(DWORD dwCmd, CSecID id, 
						 CIPCError errorCode, int iUnitCode,
						 LPCTSTR sErrorMessage=NULL);
	
	//
	inline DWORD GetRequestCounter() const;
protected:
	virtual void OnReadChannelFound();
	virtual void OnRequestDisconnect();
	// overwritten virtual functions  
	virtual void OnRequestInputConnection(const CConnectionRecord &c);
	virtual void OnRequestOutputConnection(const CConnectionRecord &c);
	virtual void OnRequestDataBaseConnection(const CConnectionRecord &c);
	virtual void OnRequestAlarmConnection(const CConnectionRecord &c);
	virtual void OnRequestServerReset(const CConnectionRecord &c);
	virtual void OnRequestAudioConnection(const CConnectionRecord &c);

private:
	BOOL	RequestConnection(const CConnectionRecord &c,
							  DWORD cipcCmd
							 );

// Data 
private:
	DWORD			m_dwLastReconnectTry;
	static	DWORD	m_dwRequestCounter;	// incremented each time a client connects/disconnects
};

inline DWORD CIPCServerControlISDN::GetRequestCounter() const
{
	return m_dwRequestCounter;
}

#endif	// CIPCServerControlISDN_H

