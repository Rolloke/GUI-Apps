// Sequencer.cpp: implementation of the CSequencer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Vision.h"

#include "CIPCOutputVision.h"
#include "VisionDoc.h"
#include "VisionView.h"
#include "Sequencer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CSequencer::CSequencer(CIPCOutputVision* pCIPCOutputVision)
{
	m_pCIPCOutputVision = pCIPCOutputVision;
	m_dwLastSwitch = 0;
	m_dwStart = GetTickCount();
}
//////////////////////////////////////////////////////////////////////
CSequencer::~CSequencer()
{

}
//////////////////////////////////////////////////////////////////////
void CSequencer::ActivateCamera(CSecID id)
{
	// in Main Thread
	m_CS.Lock();
	int i,c;
	BOOL bFound = FALSE;
	c = m_ActiveCameras.GetSize();
	for (i=0;i<c;i++)
	{
		if (id==m_ActiveCameras.GetAt(i))
		{
			bFound = TRUE;
			break;
		}
	}
	if (!bFound)
	{
		m_ActiveCameras.Add(id.GetID());
	}

	m_idActiveCamera = id;
	m_pCIPCOutputVision->RequestPictures(m_idActiveCamera,FALSE);
	m_dwLastSwitch = GetTickCount();
	m_CS.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CSequencer::ReactivateCamera(CSecID id, BOOL bSwitched)
{
	m_CS.Lock();
	m_pCIPCOutputVision->RequestPictures(id,TRUE);
	if (bSwitched)
	{
		m_idActiveCamera = id;
		m_dwLastSwitch = GetTickCount();
	}
	m_CS.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CSequencer::DeactivateCamera(CSecID id,BOOL bForceCoCoStop/*=FALSE*/)
{
	// in Main Thread
	// deact if act
	m_CS.Lock();
	if (m_idActiveCamera==id)
	{
		m_pCIPCOutputVision->UnrequestPictures(m_idActiveCamera,bForceCoCoStop);
	}
	// remove act cam from array
	int i,c,n;
	c = m_ActiveCameras.GetSize();
	for (i=0;i<c;i++)
	{
		if (id==m_ActiveCameras.GetAt(i))
		{
			m_ActiveCameras.RemoveAt(i);
			break;
		}
	}
	c = m_ActiveCameras.GetSize();
	// act next cam if any
	if (c>0)
	{
		n = i;
		if (n>=c)
		{
			n = 0;
		}

		m_idActiveCamera = m_ActiveCameras.GetAt(n);
		m_pCIPCOutputVision->RequestPictures(m_idActiveCamera,TRUE);
		m_dwLastSwitch = GetTickCount();
	}

	if (m_ActiveCameras.GetSize()==0)
	{
		// everything is off
		m_pCIPCOutputVision->StopH263Encoding();
	}
	m_CS.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CSequencer::Sequence()
{
	CServer* pServer = m_pCIPCOutputVision->GetServer();
	CVisionDoc* pDoc = pServer->GetDocument();
	if (pDoc->GetSequence())
	{
		// in Sequence Thread
		DWORD dwSequenceIntervall = pDoc->GetSequenceIntervall() * 1000;
		DWORD dwTick = GetTickCount();

		if ((dwTick-m_dwLastSwitch)>dwSequenceIntervall)
		{
			int i,n,c;

			m_CS.Lock();
			
			c = m_ActiveCameras.GetSize();
			n = 0;
			if (c>1)
			{
				// search active and get next cam
				// do the switch
				for (i=0;i<c;i++)
				{
					if (m_idActiveCamera == m_ActiveCameras.GetAt(i))
					{
						// found the active one
						n = i+1;
						break;
					}
				}
				// act next cam if any
				if (n>=c)
				{
					n = 0;
				}

				m_idActiveCamera = m_ActiveCameras.GetAt(n);

				pDoc->UpdateAllViews(NULL,MAKELONG(VDH_ACTIVATE_REQUEST_CAM,pServer->GetID()),
									(CObject*)m_pCIPCOutputVision->GetOutputRecordPtrFromSecID(m_idActiveCamera));
				m_pCIPCOutputVision->RequestPictures(m_idActiveCamera,FALSE);
				m_dwLastSwitch = dwTick;
			}
			m_CS.Unlock();
		}
	}
}
