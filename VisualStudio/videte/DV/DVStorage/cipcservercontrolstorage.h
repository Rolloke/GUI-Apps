// CIPCServerControlStorage.h: interface for the CIPCServerControlStorage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPCSERVERCONTROLSTORAGE_H)
#define AFX_IPCSERVERCONTROLSTORAGE_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CIPCServerControlStorage : public CIPCServerControl  
{
	// construction/destruction
public:
	CIPCServerControlStorage();
	virtual ~CIPCServerControlStorage();

public:
	virtual void OnRequestDataBaseConnection(const CConnectionRecord &c);
	virtual void OnRequestServerReset(const CConnectionRecord &c);
	virtual BOOL Run(DWORD dwTimeOut);	// default is GetTimeoutCmdReceive()

private:
	int m_iCounter;
};

#endif // !defined(AFX_IPCSERVERCONTROLSTORAGE_H)
