// ActualImageProcess.cpp: implementation of the CActualImageProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sec3.h"
#include "ActualImageProcess.h"

#include "CIPCOutputActualImage.h"
#include "CipcOutputServerClient.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CActualImageProcess::CActualImageProcess(CProcessScheduler* pScheduler,CActivation *pActivation)
: CProcess(pScheduler)
{
	m_pCIPCOutputActualImage = NULL;
	m_eState = AIS_INITIALIZED;
	m_pActivation = pActivation;
	pActivation->SetActiveProcess(this);
	InitTimer();
	m_type = SPT_ACTUAL_IMAGE;
}
//////////////////////////////////////////////////////////////////////
CActualImageProcess::~CActualImageProcess()
{
	WK_DELETE(m_pCIPCOutputActualImage);
}
//////////////////////////////////////////////////////////////////////
void CActualImageProcess::Run()
{
	switch (m_eState)
	{
	case AIS_INITIALIZED:
		TRACE(_T("AIS_INITIALIZED\n"));
		OnInitialized();
		break;
	case AIS_CONNECTED:
		TRACE(_T("AIS_CONNECTED\n"));
		OnConnected();
		break;
	case AIS_CONFIRMED:
		TRACE(_T("AIS_CONFIRMED\n"));
		OnConfirmed();
		break;
	case AIS_FINISHED:
		TRACE(_T("AIS_FINISHED\n"));
		OnFinished();
		break;
	case AIS_FAILED:
		TRACE(_T("AIS_FAILED\n"));
		OnFailed();
		break;
	default:
		break;
	}
}
//////////////////////////////////////////////////////////////////////
void CActualImageProcess::OnPause()
{
}
//////////////////////////////////////////////////////////////////////
void CActualImageProcess::OnInitialized()
{
	// just connect to myself
	CConnectionRecord c;
	c.SetDestinationHost(LOCAL_LOOP_BACK);
	c.SetSourceHost(LOCAL_LOOP_BACK);
	c.SetPermission(_T("SuperVisor"));

	CIPCOutputServerClient *pNewClient = theApp.GetServerControl()->AddOutputClient(c,FALSE,_T(""));
	Sleep(10);
	m_pCIPCOutputActualImage = new CIPCOutputActualImage(this, pNewClient->GetShmName());
	m_eState = AIS_CONNECTED;
}
//////////////////////////////////////////////////////////////////////
void CActualImageProcess::OnConnected()
{
	if (   m_pCIPCOutputActualImage
		&& m_pCIPCOutputActualImage->IsReady())
	{
		m_eState = AIS_CONFIRMED;
	}
}
//////////////////////////////////////////////////////////////////////
void CActualImageProcess::OnConfirmed()
{
	// disconnecting
	WK_DELETE(m_pCIPCOutputActualImage);
	m_eState = AIS_FINISHED;
}
//////////////////////////////////////////////////////////////////////
void CActualImageProcess::OnFinished()
{
	SetState(SPS_TERMINATED,WK_GetTickCount());
}
//////////////////////////////////////////////////////////////////////
void CActualImageProcess::OnFailed()
{
	SetState(SPS_TERMINATED,WK_GetTickCount());
}
