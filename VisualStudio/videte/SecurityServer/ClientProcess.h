// ClientProcess.h: interface for the CClientProcess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLIENTPROCESS_H__D8D52261_1135_11D4_A0EA_004005A19028__INCLUDED_)
#define AFX_CLIENTPROCESS_H__D8D52261_1135_11D4_A0EA_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Process.h"


class CClientProcess : public CProcess  
{
	// construction / destruction
public:
	CClientProcess(CProcessScheduler* pScheduler,CSecID idClient);
	virtual ~CClientProcess();

	// attributes
public:
	inline virtual BOOL IsClientProcess() const;

	// overrides
public:
	virtual CString GetStateName() const;
	virtual CSecID	GetClientID() const;
	virtual BOOL Terminate(BOOL bShutDown=FALSE);
			void ClearClientID();
	virtual CString Format();

	// data member
protected:
	CSecID	m_idClient;
};
/////////////////////////////////////////////////////////////////////////
inline BOOL CClientProcess::IsClientProcess() const
{
	return TRUE;
}
#endif // !defined(AFX_CLIENTPROCESS_H__D8D52261_1135_11D4_A0EA_004005A19028__INCLUDED_)
