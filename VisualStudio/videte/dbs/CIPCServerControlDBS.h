// CIPCServerControlDBS.h: interface for the CIPCServerControlDBS class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPCSERVERCONTROLDBS_H__AB5B2377_3834_11D2_B58E_00C095EC9EFA__INCLUDED_)
#define AFX_IPCSERVERCONTROLDBS_H__AB5B2377_3834_11D2_B58E_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CIPCServerControlDBS : public CIPCServerControl  
{
	// construction/destruction
public:
	CIPCServerControlDBS();
	virtual ~CIPCServerControlDBS();

public:
	virtual void OnRequestDataBaseConnection(const CConnectionRecord &c);
	virtual void OnRequestServerReset(const CConnectionRecord &c);
};

#endif // !defined(AFX_IPCSERVERCONTROLDBS_H__AB5B2377_3834_11D2_B58E_00C095EC9EFA__INCLUDED_)
