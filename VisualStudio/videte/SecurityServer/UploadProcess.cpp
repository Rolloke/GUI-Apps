#include "stdafx.h"
#include "sec3.h"
#include "Uploadprocess.h"
#include "ProcessSchedulerVideoBase.h"
#include "cipcoutputserver.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
CUploadProcess::CUploadProcess(CProcessSchedulerVideoBase* pScheduler,
							   CActivation *pActivation, 
							   CompressionType ct,
							   int iNumPicturesRecorded)
   :CProcess(pScheduler)
{
	m_pActivation = pActivation;
	pActivation->SetActiveProcess(this);

	InitTimer();
	m_CompressionType = ct;
	const COutput* pOutput = pScheduler->GetParent()->GetOutputAt(pActivation->GetOutputID().GetSubID());
	m_sCameraName = pOutput->GetName();
}
//////////////////////////////////////////////////////////////////////////
CUploadProcess::~CUploadProcess(void)
{
}
//////////////////////////////////////////////////////////////////////////
void CUploadProcess::PictureData(const CIPCPictureRecord& pict,DWORD dwMDX, DWORD dwMDY, DWORD dwUserID)
{
	if (GetState()==SPS_PAUSED)
	{
		return;
	}
	/*
	TRACE(_T("CUploadProcess::PictureData %08lx %s %s-Frame\n"),pict.GetCamID().GetID(),
		pict.GetTimeStamp().GetTime(),SPT2TCHAR(pict.GetPictureType()));*/

	theApp.m_UploadThread.AddPicture(pict,this);
}
//////////////////////////////////////////////////////////////////////////
void CUploadProcess::OnStateChanging(SecProcessState newState, BOOL bShutDown)
{
//	TRACE(_T("State=%s\n"),NameOfEnum(newState));
}
