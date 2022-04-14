// CIPCServerControlProcess.h: interface for the CIPCServerControlProcess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPCSERVERCONTROLPROCESS_H)
#define AFX_IPCSERVERCONTROLPROCESS_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CIPCServerControlProcess : public CIPCServerControl  
{
	// construction/destruction
public:
	CIPCServerControlProcess();
	virtual ~CIPCServerControlProcess();

public:
	virtual void OnRequestInputConnection(const CConnectionRecord &c);
	virtual void OnRequestOutputConnection(const CConnectionRecord &c);
	virtual void OnRequestAudioConnection(const CConnectionRecord &c);
	virtual void OnRequestServerReset(const CConnectionRecord &c);
	virtual BOOL Run(DWORD dwTimeOut);

private:
	int m_iCounter;
};

#endif // !defined(AFX_IPCSERVERCONTROLPROCESS_H)
