// VideoOutProcess.cpp: implementation of the CVideoOutProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sec3.h"
#include "VideoOutProcess.h"
#include "ProcessSchedulerVideo.h"
#include "ProcessListView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVideoOutProcess::CVideoOutProcess(CProcessScheduler* pScheduler,CActivation *pActivation)
:CProcess(pScheduler)
{
	m_pActivation = pActivation;
	pActivation->SetActiveProcess(this);
	InitTimer();
	m_type = SPT_VIDEO_OUT;
	m_iCurrentCamera = 0;
	m_dwNextSwitch = 0;
}
//////////////////////////////////////////////////////////////////////
CVideoOutProcess::~CVideoOutProcess()
{

}
//////////////////////////////////////////////////////////////////////
void CVideoOutProcess::Run()
{
	switch (GetMacro().GetVideoOutType())
	{
	case 0:
		// client
		// nothing todo
		break;
	case 1:
		// sequencer
		RunSequencer();
		break;
	case 2:
		// activity
		RunActivity();
		break;
	}
}
//////////////////////////////////////////////////////////////////////
void CVideoOutProcess::OnPause()
{
	// nothing todo
}
//////////////////////////////////////////////////////////////////////
void CVideoOutProcess::RunSequencer()
{
	if (m_dwNextSwitch == 0)
	{
		CIPCOutputServer* pCIPCOutputServer = ((CProcessSchedulerVideo*)GetScheduler())->GetParent();
		WORD wSubID = 0;
		if (m_iCurrentCamera<GetMacro().GetIDs().GetSize())
		{
			wSubID = GetMacro().GetIDs().GetAt(m_iCurrentCamera);
			m_iCurrentCamera++;
		}
		else
		{
			m_iCurrentCamera = 0;
			wSubID = GetMacro().GetIDs().GetAt(m_iCurrentCamera);
			m_iCurrentCamera++;
		}

		SwitchVideoOut(CSecID(pCIPCOutputServer->GetGroupID(),(WORD)(wSubID)));
		if (GetMacro().GetDwellTime()>0)
		{
			m_dwNextSwitch = GetMacro().GetDwellTime()-1;
		}
		if (   GetMacro().GetIDs().GetSize() == 1
			&& GetMacro().GetDwellTime()>0)
		{
			// nur eine Kamera!
			// dann Process einfach mal beenden
			SetState(SPS_TERMINATED,WK_GetTickCount());
		}
	}
	else
	{
		m_dwNextSwitch--;
	}
}
//////////////////////////////////////////////////////////////////////
void CVideoOutProcess::RunActivity()
{
	if (GetScheduler()->GetParentCIPC()==theApp.GetOutputDummy())
	{
		TRACE(_T("RunActivity Quad %d m_idActivity=%08lx\n"),m_dwNextSwitch,m_idActivity.GetID());
	}

	if (m_dwNextSwitch == 0)
	{
		Lock(_T(__FUNCTION__));
#ifdef _VO_QUEUE
		if (m_ActivityIDs.GetSize())
#else
		if (m_idActivity!=SECID_NO_ID)
#endif
		{
			CSecID id;
			
#ifdef _VO_QUEUE
			id = m_ActivityIDs.GetAt(0);
			m_ActivityIDs.RemoveAt(0);
#else
			id = m_idActivity;
			m_idActivity = SECID_NO_ID;
#endif
			SwitchVideoOut(id);
			m_dwNextSwitch = GetMacro().GetDwellTime();
			// do the quad switch if any too
			if (GetScheduler()->GetParentCIPC()!=theApp.GetOutputDummy())
			{
				theApp.GetOutputDummy()->DoActivityVideoOut(id,GetMacro().GetVideoOutPort());
			}
		}
		else
		{
			// idle !? Quad into Quad view
			if (GetScheduler()->GetParentCIPC()==theApp.GetOutputDummy())
			{
				SwitchVideoOut(SECID_NO_ID);
			}
		}
		Unlock();
	}
	else
	{
		m_dwNextSwitch--;
	}
}
//////////////////////////////////////////////////////////////////////
void CVideoOutProcess::DoClientVideoOut(CSecID id)
{
	if (GetMacro().GetVideoOutType() == 0)
	{
		TRACE("CVideoOutProcess::DoClientVideoOut %08lx\n",id.GetID());
		// is a client process
		// darf die Kamera ueberhaupt geschaltet werden?
		// in den Einstellungen wird ab 1 gezählt, intern ab 0
		WORD wSubID = id.GetSubID();
		BOOL bFound = FALSE;
		for (int i=0;i<GetMacro().GetIDs().GetSize() && !bFound;i++)
		{
			bFound = GetMacro().GetIDs().GetAt(i) == wSubID;
		}

		if (bFound)
		{
			SwitchVideoOut(id);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CVideoOutProcess::DoActivityVideoOut(CSecID id)
{
	if (GetMacro().GetVideoOutType() == 2)
	{
		WORD wSubID = id.GetSubID();
		BOOL bFound = FALSE;
		for (int i=0;i<GetMacro().GetIDs().GetSize() && !bFound;i++)
		{
			bFound = GetMacro().GetIDs().GetAt(i) == wSubID;
		}
		if (bFound)
		{
//			TRACE("CVideoOutProcess::DoActivityVideoOut %08lx\n",id.GetID());
#ifdef _VO_QUEUE
			Lock(_T(__FUNCTION__));
			bFound = FALSE;
			for (i=0;i<m_ActivityIDs.GetSize() && !bFound;i++)
			{
				bFound = m_ActivityIDs.GetAt(i) == id.GetID();
			}
			if (!bFound)
			{
				m_ActivityIDs.Add(id.GetID());
			}
			Unlock();
#else
			m_idActivity = id;
#endif
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CVideoOutProcess::SwitchVideoOut(CSecID id)
{
	CSecID idGroup(GetMacro().GetVideoOutGroupID(),0);

	CIPCOutputServer* pCIPCOutputServer = ((CProcessSchedulerVideo*)GetScheduler())->GetParent();
	pCIPCOutputServer->SwitchVideoOut(id,GetMacro().GetVideoOutPort());

	if (m_pActivation)
	{
		m_pActivation->SetOutputID(id);
	}
	UpdateProcessListView(TRUE,this);
}

