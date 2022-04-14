// RelayProcess.h: interface for the CRelayProcess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RELAYPROCESS_H__D8D52267_1135_11D4_A0EA_004005A19028__INCLUDED_)
#define AFX_RELAYPROCESS_H__D8D52267_1135_11D4_A0EA_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Process.h"

class CRelayProcess : public CProcess  
{
public:
	// automatic relay process
	CRelayProcess(CProcessScheduler* pScheduler,CActivation *pActivation);
	virtual ~CRelayProcess();

};

#endif // !defined(AFX_RELAYPROCESS_H__D8D52267_1135_11D4_A0EA_004005A19028__INCLUDED_)
