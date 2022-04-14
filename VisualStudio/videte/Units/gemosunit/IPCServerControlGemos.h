// IPCServerControlGemos.h: interface for the CIPCServerControlGemos class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPCSERVERCONTROLGEMOS_H__BDA78037_4B4D_11D3_9979_004005A19028__INCLUDED_)
#define AFX_IPCSERVERCONTROLGEMOS_H__BDA78037_4B4D_11D3_9979_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CIPCServerControlGemos : public CIPCServerControl  
{
	// construction / destruction
public:
	CIPCServerControlGemos();
	virtual ~CIPCServerControlGemos();

	// overrides
public:
	virtual void OnRequestServerReset(const CConnectionRecord &c);
};

#endif // !defined(AFX_IPCSERVERCONTROLGEMOS_H__BDA78037_4B4D_11D3_9979_004005A19028__INCLUDED_)
