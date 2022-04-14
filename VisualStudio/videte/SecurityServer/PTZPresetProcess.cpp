#include "stdafx.h"
#include "sec3.h"
#include "ptzpresetprocess.h"
#include "cipcinputserver.h"

//////////////////////////////////////////////////////////////////////
CPTZPresetProcess::CPTZPresetProcess(CProcessScheduler* pScheduler,CActivation *pActivation)
:CProcess(pScheduler)
{
	m_pActivation = pActivation;
	pActivation->SetActiveProcess(this);
	InitTimer();
	m_type = SPT_VIDEO_OUT;
}
//////////////////////////////////////////////////////////////////////
CPTZPresetProcess::~CPTZPresetProcess(void)
{
}
//////////////////////////////////////////////////////////////////////
void CPTZPresetProcess::Run()
{
	CameraControlCmd ccc = m_pActivation->GetCameraControlCmd();
	CSecID camID = m_pActivation->GetOutputID();

	if (ccc != CCC_INVALID)
	{
		CIPCInputServer* pInputGroup = theApp.GetInputGroups().GetGroupByShmName(SM_COMMUNIT_INPUT);
		if (   pInputGroup
			&& pInputGroup->GetSize()>0) 
		{
			WK_TRACE(_T("set camera %08lx to position %s\n"),camID.GetID(),NameOfEnum(ccc));
			pInputGroup->DoRequestCameraControl(pInputGroup->GetInputAt(0)->GetID(),camID, ccc, 0);
		}
		else
		{
			WK_TRACE(_T("no PTZ Preset communit not connected\n"));
		}
	}
	else
	{
		WK_TRACE(_T("no PTZ Preset illegal PTZ position\n"));
	}
	SetState(SPS_TERMINATED,WK_GetTickCount());
}

