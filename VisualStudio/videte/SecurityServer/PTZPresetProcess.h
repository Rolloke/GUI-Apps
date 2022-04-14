// VideoOutProcess.h: interface for the CVideoOutProcess class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include "process.h"

class CPTZPresetProcess :
	public CProcess
{
public:
	CPTZPresetProcess(CProcessScheduler* pScheduler,CActivation *pActivation);
	virtual ~CPTZPresetProcess();

	// overrides
public:
	virtual void Run();
};
