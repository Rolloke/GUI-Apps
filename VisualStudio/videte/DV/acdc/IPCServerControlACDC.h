// IPCServerControlACDC.h: interface for the CIPCServerControlACDC class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPCSERVERCONTROLACDC_H__91545434_C063_4948_921F_4CF1BDF672A9__INCLUDED_)
#define AFX_IPCSERVERCONTROLACDC_H__91545434_C063_4948_921F_4CF1BDF672A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CIPCBurnDataCarrier;
class CIPCServerControlACDC : public CIPCServerControl  
{
public:
	CIPCServerControlACDC();
	virtual ~CIPCServerControlACDC();

public:
	CIPCBurnDataCarrier* GetBurnDataCarrier();

	virtual void OnRequestDCConnection(const CConnectionRecord &c);
	virtual BOOL Run(DWORD dwTimeOut);	// default is GetTimeoutCmdReceive()

	CString GetNewMessage();

private:
	CIPCBurnDataCarrier*	m_pClient;
	CString m_sNewMessage;
};

#endif // !defined(AFX_IPCSERVERCONTROLACDC_H__91545434_C063_4948_921F_4CF1BDF672A9__INCLUDED_)
