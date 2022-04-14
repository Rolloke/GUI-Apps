// VideoOutProcess.h: interface for the CVideoOutProcess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIDEOOUTPROCESS_H__B2C92C80_90D7_477D_A602_966075BF3C3D__INCLUDED_)
#define AFX_VIDEOOUTPROCESS_H__B2C92C80_90D7_477D_A602_966075BF3C3D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Process.h"

class CVideoOutProcess : public CProcess  
{
public:
	CVideoOutProcess(CProcessScheduler* pScheduler,CActivation *pActivation);
	virtual ~CVideoOutProcess();

	// overrides
public:
	virtual void Run();
	virtual void OnPause();

	void DoClientVideoOut(CSecID id);
	void DoActivityVideoOut(CSecID id);

protected:
	void RunSequencer();
	void RunActivity();
	void SwitchVideoOut(CSecID id);

private:
	int				m_iCurrentCamera;
	DWORD			m_dwNextSwitch;
#ifdef _VO_QUEUE
	CDWordArray		m_ActivityIDs;
#else
	CSecID			m_idActivity;
#endif
};

#endif // !defined(AFX_VIDEOOUTPROCESS_H__B2C92C80_90D7_477D_A602_966075BF3C3D__INCLUDED_)
