// IPCServerControlSMS.h: interface for the CIPCServerControlSMS class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPCSERVERCONTROLSMS_H__60D27066_9E3A_11D2_B54A_004005A19028__INCLUDED_)
#define AFX_IPCSERVERCONTROLSMS_H__60D27066_9E3A_11D2_B54A_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CIPCServerControlSMS : public CIPCServerControl  
{
public:
	CIPCServerControlSMS();
	virtual ~CIPCServerControlSMS();

public:
	virtual void OnRequestAlarmConnection(const CConnectionRecord &c);
	virtual void OnRequestServerReset(const CConnectionRecord &c);


public:
	void DoIndicateError(const CString& sMessage);
};

#endif // !defined(AFX_IPCSERVERCONTROLSMS_H__60D27066_9E3A_11D2_B54A_004005A19028__INCLUDED_)
