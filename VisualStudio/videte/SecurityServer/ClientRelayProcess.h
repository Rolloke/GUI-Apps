// ClientRelayProcess.h: interface for the CClientRelayProcess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLIENTRELAYPROCESS_H__D8D52266_1135_11D4_A0EA_004005A19028__INCLUDED_)
#define AFX_CLIENTRELAYPROCESS_H__D8D52266_1135_11D4_A0EA_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ClientProcess.h"

class CClientRelayProcess : public CClientProcess
{
	// construction / destruction
public:
	CClientRelayProcess(CProcessScheduler* pScheduler,CSecID idClient, const COutput &relay);
	virtual ~CClientRelayProcess();

	// attributes
public:
	inline virtual BOOL IsClientRelayProcess() const;
	inline virtual BOOL IsRelayProcess() const;
};
/////////////////////////////////////////////////////////////////////
inline BOOL CClientRelayProcess::IsClientRelayProcess() const
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////
inline BOOL CClientRelayProcess::IsRelayProcess() const
{
	return TRUE;
}

#endif // !defined(AFX_CLIENTRELAYPROCESS_H__D8D52266_1135_11D4_A0EA_004005A19028__INCLUDED_)
