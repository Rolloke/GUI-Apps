// OutputList.cpp: implementation of the COutputList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DVProcess.h"
#include "OutputList.h"

#include "CameraGroup.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COutputList::COutputList()
{

}
//////////////////////////////////////////////////////////////////////
COutputList::~COutputList()
{

}
//////////////////////////////////////////////////////////////////////
void COutputList::CalcClientIDs()
{
	Lock();
	WORD wNumCamera = 0;
	for (int i=0;i<GetSize();i++)
	{
		COutputGroup* pGroup = GetAtFast(i);
		pGroup->Lock();
		if (pGroup->HasCameras())
		{
			for (int j=0;j<pGroup->GetSize();j++)
			{
				CCamera* pCamera = (CCamera*)pGroup->GetAtFast(j);
				pCamera->SetClientID(CSecID(SECID_GROUP_OUTPUT,wNumCamera));
				if (pCamera->GetMotionDetector())
				{
					pCamera->GetMotionDetector()->SetClientNr((BYTE)wNumCamera);
				}
				if (pCamera->GetExternDetector())
				{
					pCamera->GetExternDetector()->SetClientNr((BYTE)wNumCamera);
				}
				if (pCamera->GetSuspectDetector())
				{
					pCamera->GetSuspectDetector()->SetClientNr((BYTE)wNumCamera);
				}

				wNumCamera++;
			}
		}
		pGroup->Unlock();
	}
	Unlock();
}
//////////////////////////////////////////////////////////////////////
void COutputList::Load(CWK_Profile& wkp)
{
	Lock();

	for (int i=0;i<GetSize();i++)
	{
		COutputGroup* pGroup = GetAtFast(i);
		pGroup->Load(wkp);
	}

	Unlock();
}
//////////////////////////////////////////////////////////////////////
void COutputList::Save(CWK_Profile& wkp)
{
	Lock();

	// First clear PTZ settings
	CString sSNKey;
	int j;
	DWORD dwComMask;
	DWORD dwBit = 1;
	CWK_Profile wkpDVRT;
	dwComMask = wkpDVRT.GetInt(_T("Common"),_T("CommPortInfo"), 0);
	for (dwBit = 1,j=1; dwBit; dwBit<<=1,j++)
	{
		if (dwComMask & dwBit)
		{
			sSNKey.Format(_T("COM%d"),j);
			wkpDVRT.DeleteSubSection(_T("CommUnit"), sSNKey);
		}
	}

	for (int i=0;i<GetSize();i++)
	{
		COutputGroup* pGroup = GetAtFast(i);
		pGroup->Save(wkp);
	}

	Unlock();
}
//////////////////////////////////////////////////////////////////////
void COutputList::StartCIPCThreads()
{
	for (int i=0;i<GetSize();i++)
	{
		GetAtFast(i)->StartThread();
	}
}
//////////////////////////////////////////////////////////////////////
void COutputList::WaitForConnect()
{
	BOOL bConnected = FALSE;
	int c = 100;
	while ( (c-->0) && (!bConnected))
	{
		bConnected = TRUE;
		for (int i=0;i<GetSize();i++)
		{
			bConnected &= GetAtFast(i)->GetIPCState()==CIPC_STATE_CONNECTED;
		}
		Sleep(50);
	}
}
/////////////////////////////////////////////////////////////////////////////
void COutputList::WaitForCameraScan()
{
	// first we have to calc the events
	COutputList cameragroups;

	for (int i=0;i<GetSize();i++)
	{
		COutputGroup* pOutputGroup = GetAtFast(i);
		if (   pOutputGroup->HasCameras()
			&& pOutputGroup->GetIPCState() == CIPC_STATE_CONNECTED)
		{
			cameragroups.Add(pOutputGroup);
		}
	}

	int c = 200;
	while (cameragroups.GetSize()>0
		   && c>0)
	{
		HANDLE* pHandles = new HANDLE[cameragroups.GetSize()];
		int h = 0;

		for (int i=0;i<cameragroups.GetSize();i++)
		{
			COutputGroup* pOutputGroup = cameragroups.GetAtFast(i);
			if (pOutputGroup->HasCameras())
			{
				CCameraGroup* pCG = (CCameraGroup*)pOutputGroup;
				pHandles[h++] = pCG->ScanEvent();
			}
		}

		DWORD dwWait = WaitForMultipleObjects(h,pHandles,FALSE,50);
		if (dwWait == WAIT_TIMEOUT)
		{
		}
		else if (dwWait == WAIT_ABANDONED)
		{
		}
		else
		{
			DWORD dwIndex = dwWait - WAIT_OBJECT_0;
			cameragroups.RemoveAt(dwIndex);
		}
		
		delete [] pHandles;
		c--;
	}

	if (c == 0)
	{
		WK_TRACE(_T("timeout waiting for camera scan\n"));
	}
}
//////////////////////////////////////////////////////////////////////
void COutputList::RescanCameras()
{
	Lock();
	for (int i=0;i<GetSize();i++)
	{
		COutputGroup* pOutputGroup = GetAtFast(i);
		if (pOutputGroup->HasCameras())
		{
			CCameraGroup* pCG = (CCameraGroup*)pOutputGroup;
			pCG->RescanCameras();
		}
	}
	Unlock();
}
//////////////////////////////////////////////////////////////////////
BOOL COutputList::IsScanning()
{
	BOOL bRet = FALSE;
	Lock();
	for (int i=0;i<GetSize();i++)
	{
		COutputGroup* pOutputGroup = GetAtFast(i);
		if (pOutputGroup->HasCameras())
		{
			CCameraGroup* pCG = (CCameraGroup*)pOutputGroup;
			bRet |= pCG->IsScanning();
		}
	}
	Unlock();
	return bRet;
}
//////////////////////////////////////////////////////////////////////
void COutputList::StartRequests()
{
	Lock();

	for (int i=0;i<GetSize();i++)
	{
		GetAtFast(i)->StartRequests();
	}

	Unlock();
}
//////////////////////////////////////////////////////////////////////
void COutputList::StopRequests()
{
	Lock();

	for (int i=0;i<GetSize();i++)
	{
		GetAtFast(i)->StopRequests();
	}

	Unlock();
}
//////////////////////////////////////////////////////////////////////
COutput* COutputList::GetOutputByClientID(CSecID id)
{
//	Lock();

	for (int i=0;i<GetSize();i++)
	{
		COutputGroup* pOutputGroup = GetAtFast(i);
//		pOutputGroup->Lock();
		for (int j=0;j<pOutputGroup->GetSize();j++)
		{
			COutput* pOutput = pOutputGroup->GetAtFast(j);
			if (pOutput->GetClientID() == id)
			{
//				pOutputGroup->Unlock();
//				Unlock();
				return pOutput;
			}
		}

//		pOutputGroup->Unlock();
	}

//	Unlock();

	return NULL;
}
//////////////////////////////////////////////////////////////////////
CCamera* COutputList::GetCameraByArchiveNr(WORD wArchiveNr)
{
	Lock();

	for (int i=0;i<GetSize();i++)
	{
		COutputGroup* pOutputGroup = GetAtFast(i);
		pOutputGroup->Lock();
		if (pOutputGroup->HasCameras())
		{
			CCameraGroup* pCameraGroup = (CCameraGroup*)pOutputGroup;

			for (int j=0;j<pCameraGroup->GetSize();j++)
			{
				CCamera* pCamera = (CCamera*)pCameraGroup->GetAtFast(j);
				if (   pCamera->GetMDAlarmNr() == wArchiveNr
					|| pCamera->GetAlarmNr() == wArchiveNr
					|| pCamera->GetPreAlarmNr() == wArchiveNr
					|| pCamera->GetAlarmListNr() == wArchiveNr
					)
				{
					pOutputGroup->Unlock();
					Unlock();
					return pCamera;
				}
			}
		}

		pOutputGroup->Unlock();
	}

	Unlock();

	return NULL;
}
//////////////////////////////////////////////////////////////////////
COutput* COutputList::GetOutputByUnitID(CSecID id)
{
	Lock();

	for (int i=0;i<GetSize();i++)
	{
		COutputGroup* pOutputGroup = GetAtFast(i);
		pOutputGroup->Lock();
		for (int j=0;j<pOutputGroup->GetSize();j++)
		{
			COutput* pOutput = pOutputGroup->GetAtFast(j);
			if (pOutput->GetUnitID() == id)
			{
				pOutputGroup->Unlock();
				Unlock();
				return pOutput;
			}
		}

		pOutputGroup->Unlock();
	}

	Unlock();

	return NULL;
}
//////////////////////////////////////////////////////////////////////
CCameraGroup* COutputList::GetCameraGroup(int iCard)
{
	CCameraGroup* pOutputGroup = NULL;
	Lock();

	for (int i=0;i<GetSize();i++)
	{
		if (   GetAtFast(i)->HasCameras()
			&& GetAtFast(i)->GetCardNr() == iCard)
		{
			pOutputGroup = (CCameraGroup*)GetAtFast(i);
			break;
		}
	}
	Unlock();
    return pOutputGroup;
}
//////////////////////////////////////////////////////////////////////
int COutputList::GetNrOfCameras()
{
	int r = 0;

	Lock();

	for (int i=0;i<GetSize();i++)
	{
		if (GetAtFast(i)->HasCameras())
		{
			r += GetAtFast(i)->GetSize();
		}
	}

	Unlock();

	return r;
}
//////////////////////////////////////////////////////////////////////
int COutputList::GetNrOfActiveCameras()
{
	int r = 0;

	Lock();

	for (int i=0;i<GetSize();i++)
	{
		r += GetAtFast(i)->GetNrOfActiveCameras();
	}

	Unlock();

	return r;
}
//////////////////////////////////////////////////////////////////////
void COutputList::ResetFramesPerDay()
{
	Lock();

	for (int i=0;i<GetSize();i++)
	{
		COutputGroup* pOutputGroup = GetAtFast(i);
        if (pOutputGroup->HasCameras())
		{
			CCameraGroup* pCameraGroup = (CCameraGroup*)pOutputGroup;

			for (int j=0;j<pCameraGroup->GetSize();j++)
			{
				CCamera* pOutput = (CCamera*)pCameraGroup->GetAtFast(j);
				pOutput->ResetFramesPerDay();
			}
		}
	}

	Unlock();
}
//////////////////////////////////////////////////////////////////////
void COutputList::ResetFramesPerWeek()
{
	Lock();

	for (int i=0;i<GetSize();i++)
	{
		COutputGroup* pOutputGroup = GetAtFast(i);
        if (pOutputGroup->HasCameras())
		{
			CCameraGroup* pCameraGroup = (CCameraGroup*)pOutputGroup;

			for (int j=0;j<pCameraGroup->GetSize();j++)
			{
				CCamera* pOutput = (CCamera*)pCameraGroup->GetAtFast(j);
				pOutput->ResetFramesPerWeek();
			}
		}
	}

	Unlock();
}
//////////////////////////////////////////////////////////////////////
void COutputList::SaveReferenceImage()
{
	Lock();
	for (int i=0;i<GetSize();i++)
	{
		GetAtFast(i)->SaveReferenceImage();
	}
	Unlock();
}
//////////////////////////////////////////////////////////////////////
void COutputList::RemoveClient(DWORD dwClientID)
{
	Lock();

	for (int i=0;i<GetSize();i++)
	{
		COutputGroup* pOutputGroup = GetAtFast(i);
        if (pOutputGroup->HasCameras())
		{
			CCameraGroup* pCameraGroup = (CCameraGroup*)pOutputGroup;

			for (int j=0;j<pCameraGroup->GetSize();j++)
			{
				CCamera* pOutput = (CCamera*)pCameraGroup->GetAtFast(j);
				pOutput->DeleteH263ClientQCIF(dwClientID);
				pOutput->DeleteMPEG4Client(dwClientID);
			}
		}
	}

	Unlock();
}
//////////////////////////////////////////////////////////////////////
void COutputList::ClearActiveClientCam()
{
	Lock();
	for (int i=0;i<GetSize();i++)
	{
		COutputGroup* pOutputGroup = GetAtFast(i);
		if (pOutputGroup->HasCameras())
		{
			CCameraGroup* pCameraGroup = (CCameraGroup*)pOutputGroup;
			pCameraGroup->SetClientActiveCam(SECID_NO_ID);
		}
	}

	Unlock();
}
//////////////////////////////////////////////////////////////////////
void COutputList::ResetAudioReferences()
{
	Lock();
	for (int i=0;i<GetSize();i++)
	{
		COutputGroup* pOutputGroup = GetAtFast(i);
		if (pOutputGroup->HasCameras())
		{
			CCameraGroup* pCameraGroup = (CCameraGroup*)pOutputGroup;
			for (int j=0;j<pCameraGroup->GetSize();j++)
			{
				CCamera*pCam = (CCamera*)pCameraGroup->GetAtFast(j);
				pCam->SetAudioInput(NULL);
				pCam->SetMedia(NULL);
			}
		}
	}
	Unlock();
}
//////////////////////////////////////////////////////////////////////
void COutputList::OnResolutionChanged(Resolution resolution)
{
	for (int i=0;i<GetSize();i++)
	{
		COutputGroup* pOutputGroup = GetAtFast(i);
		if (pOutputGroup->HasCameras())
		{
			CCameraGroup* pCameraGroup = (CCameraGroup*)pOutputGroup;
			pCameraGroup->OnResolutionChanged(resolution);
		}
	}
}
