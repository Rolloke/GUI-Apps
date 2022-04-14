// ActualImageProcess.h: interface for the CActualImageProcess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACTUALIMAGEPROCESS_H__D3F03193_F70F_11D2_B5D1_004005A19028__INCLUDED_)
#define AFX_ACTUALIMAGEPROCESS_H__D3F03193_F70F_11D2_B5D1_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Process.h"

class CIPCOutputActualImage;

class CActualImageProcess : public CProcess  
{
public:
	CActualImageProcess(CProcessScheduler* pScheduler,CActivation *pActivation);
	virtual ~CActualImageProcess();

	// overrides
public:
	virtual void Run();
	virtual void OnPause();

private:
	enum ActualImageState
	{
		AIS_INITIALIZED,
		AIS_CONNECTED,
		AIS_CONFIRMED,
		AIS_FINISHED,
		AIS_FAILED
	};

protected:
	void OnInitialized();
	void OnConnected();
	void OnConfirmed();
	void OnFinished();
	void OnFailed();

private:
	CIPCOutputActualImage* m_pCIPCOutputActualImage;
	ActualImageState m_eState;

};

#endif // !defined(AFX_ACTUALIMAGEPROCESS_H__D3F03193_F70F_11D2_B5D1_004005A19028__INCLUDED_)
