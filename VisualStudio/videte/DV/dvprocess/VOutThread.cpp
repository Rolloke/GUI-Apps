// VOutThread.cpp: implementation of the CVOutThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dvprocess.h"
#include "VOutThread.h"

#include "CameraGroup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVOutThread::CVOutThread()
 : CWK_Thread(_T("VOutThread"),NULL)
{
	m_dwStatTime = 4*60*60*1000;
	for (int iCard=0;iCard<NR_OF_CARDS;iCard++)
	{
		for (int iPort=0;iPort<NR_OF_VOUT;iPort++)
		{
			m_dwVOutPortTick[iCard][iPort] = 0;
			m_wCamSubID[iCard][iPort] = (WORD)-1;
			m_dwVOutPortTime[iCard][iPort] = 5*1000;
		}
	}
}

CVOutThread::~CVOutThread()
{

}
//////////////////////////////////////////////////////////////////////
BOOL CVOutThread::StartThread()
{
	DWORD dwTick = WK_GetTickCount();
	// Loop over all Codecs
	for (int iCard=0;iCard<NR_OF_CARDS;iCard++)
	{
		// Loop over all VOuts
		for (int iPort=0;iPort<NR_OF_VOUT;iPort++)
		{
			// first init the Ticks
			m_dwVOutPortTick[iCard][iPort] = dwTick;
		}
	}
	return CWK_Thread::StartThread();
}
//////////////////////////////////////////////////////////////////////
BOOL CVOutThread::Run(LPVOID lpContext)
{
	DWORD dwTick = WK_GetTickCount();
	BOOL bSwitched = FALSE;

	theApp.CheckAlarmOutput(dwTick);

	// Loop over all Codecs
	for (int iCard=0;iCard<NR_OF_CARDS;iCard++)
	{
		// Loop over all VOuts
		for (int iPort=0;iPort<NR_OF_VOUT;iPort++)
		{
			switch (theApp.GetVOutModePort(iCard,iPort))
			{
			case VOUT_SEQUENCE:
				// is this VOut in sequencing mode
				bSwitched |= Sequence(iCard,iPort,dwTick);
				break;
			case VOUT_ACTIVITY:
				bSwitched |= Activity(iCard,iPort,dwTick);
				break;
			default:
				break;
			}
		}
	}

	if (bSwitched)
	{
		Sleep(100);
	}

	return TRUE;
}
//////////////////////////////////////////////////////////////////////
BOOL CVOutThread::Sequence(int iCard, int iPort, DWORD dwTick)
{
	BOOL bSwitched = FALSE;
	// is the time over
	if (WK_GetTimeSpan(m_dwVOutPortTick[iCard][iPort])>m_dwVOutPortTime[iCard][iPort])
	{
		// the real switch
		// find out the right new cam
		CCameraGroup* pCameraGroup = theApp.GetOutputGroups().GetCameraGroup(iCard);
		if (pCameraGroup)
		{
			WORD wFirstCam = (WORD)-1;
			int iNrOfActiveCameras = pCameraGroup->GetNrOfActiveCameras();
			int c=-1;
			if (iNrOfActiveCameras>0)
			{
				int iActives = 0;
				for (c=0; c<pCameraGroup->GetSize(); c++)
				{
					CCamera* pCamera = (CCamera*)pCameraGroup->GetAtFast(c);
					if (pCamera->IsActive())
					{
						if (wFirstCam == (WORD)-1)
						{
							wFirstCam = pCamera->GetSubID();
						}
						if (m_wCamSubID[iCard][iPort] == (WORD)-1)
						{
							// zum Anfang erste aktive
							m_wCamSubID[iCard][iPort] = pCamera->GetSubID();
							break;
						}
						if (pCamera->GetSubID() == m_wCamSubID[iCard][iPort])
						{
							// found current one
							// take the next one
							m_wCamSubID[iCard][iPort] = (WORD)-1;
							if (iActives+1 == iNrOfActiveCameras)
							{
								c = -1;
							}
						}
						iActives++;
					}
					else
					{
						if (pCamera->GetSubID() == m_wCamSubID[iCard][iPort])
						{
							// war aktiv, jetzt kein signal mehr
							// erste camera nehmen!
							if (wFirstCam != (WORD)-1)
							{
								m_wCamSubID[iCard][iPort] = wFirstCam;
							}
							else
							{
								// keine erste camera
								// ID ungültig machen
								m_wCamSubID[iCard][iPort] = (WORD)-1;
							}
						}
						else
						{
							wFirstCam = (WORD)-1;
						}
					}
				}
				CCamera* pCamera = NULL;
				if (IsBetween(c, 0, pCameraGroup->GetSize()-1))
				{
					pCamera = (CCamera*)pCameraGroup->GetAtFast(c);
				}
				if (pCamera)
				{
					DWORD dwDwellMs = pCamera->GetDwellTime()*1000;
					if (dwDwellMs>0)
					{
						CString sValue;
						sValue.Format(_T("PORT%d"),iPort);
						CSecID idCam(pCameraGroup->GetID(),m_wCamSubID[iCard][iPort]);
						if (theApp.GetSettings().TraceVOUT())
						{
							WK_TRACE(_T("Sequence %s CAM %08lx\n"),sValue,idCam.GetID());
						}
						pCameraGroup->DoRequestSetValue(idCam,
														CSD_V_OUT,
														sValue,
														0 /*server data*/);
						bSwitched = TRUE;
						m_dwVOutPortTick[iCard][iPort] = dwTick;
						// set the new dwell Time
						m_dwVOutPortTime[iCard][iPort] = dwDwellMs;
					}
				}
			}
		}
	}

	return bSwitched;
}
//////////////////////////////////////////////////////////////////////
void CVOutThread::AddActivity(CSecID idCam, int iCard)
{
	BOOL bActivity = FALSE;

	for (int iPort=0;iPort<NR_OF_VOUT;iPort++)
	{
		bActivity = bActivity || (theApp.GetVOutModePort(iCard,iPort) == VOUT_ACTIVITY);
	}

	if (   IsRunning()
		&& bActivity
		&& iCard>=0 
		&& iCard<NR_OF_CARDS)
	{
		m_csActivities[iCard].Lock();
		BOOL bFound = FALSE;
		for (int i=0;i<m_dwActivities[iCard].GetSize()&&!bFound; i++)
		{
			if (m_dwActivities[iCard].GetAt(i) == idCam.GetID())
			{
				bFound = TRUE;
			}
		}
		if (!bFound)
		{
			// don't add a cam twice
			m_dwActivities[iCard].Add(idCam.GetID());
//			TRACE(_T("add activity %08lx %d\n"),idCam.GetID(),m_dwActivities[iCard].GetSize());
		}
		m_csActivities[iCard].Unlock();
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CVOutThread::Activity(int iCard, int iPort, DWORD dwTick)
{
	BOOL bSwitched = FALSE;

	if (WK_GetTimeSpan(m_dwVOutPortTick[iCard][iPort])>3*1000)
	{
		m_csActivities[iCard].Lock();
		if (m_dwActivities[iCard].GetSize())
		{
			CSecID idCam(m_dwActivities[iCard].GetAt(0));
			CCameraGroup* pCameraGroup = theApp.GetOutputGroups().GetCameraGroup(iCard);
			if (pCameraGroup)
			{
				m_dwActivities[iCard].RemoveAt(0);
//				TRACE(_T("rem activity %08lx %d\n"),idCam.GetID(),m_dwActivities[iCard].GetSize());
				CString sValue;
				sValue.Format(_T("PORT%d"),iPort);
				if (theApp.GetSettings().TraceVOUT())
				{
					WK_TRACE(_T("Activity %s CAM %08lx\n"),sValue,idCam.GetID());
				}
				pCameraGroup->DoRequestSetValue(idCam,
											CSD_V_OUT,
											sValue,
											0 /*server data*/);
				bSwitched = TRUE;
				m_dwVOutPortTick[iCard][iPort] = dwTick;
			}
		}
		m_csActivities[iCard].Unlock();
	}

	return bSwitched;
}
//////////////////////////////////////////////////////////////////////
BOOL CVOutThread::StopThread()
{
	BOOL bRet;
	bRet = CWK_Thread::StopThread();
	for (int iCard=0;iCard<NR_OF_CARDS;iCard++)
	{
		for (int iPort=0;iPort<NR_OF_VOUT;iPort++)
		{
			m_dwVOutPortTick[iCard][iPort] = 0;
			m_wCamSubID[iCard][iPort] = (WORD)-1;
			m_csActivities[iCard].Lock();
			m_dwActivities[iCard].RemoveAll();
			m_csActivities[iCard].Unlock();
		}
	}
	return bRet;
}
