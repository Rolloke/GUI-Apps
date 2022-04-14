// DirectShowThread.cpp : implementation file
//

#include "stdafx.h"
#include "IPCameraUnit.h"
#include "VideoStateCheckThread.h"
#include "IPCameraUnitDlg.h"
#include "DSCamera.h"
#include "CIPCOutputIPcamUnit.h"

/////////////////////////////////////////////////////////////////////////////
// CDirectShowThread
IMPLEMENT_DYNAMIC(CVideoStateCheckThread, CWK_WinThread)

/////////////////////////////////////////////////////////////////////////////
CVideoStateCheckThread::CVideoStateCheckThread(const CString&sName) :
	CWK_WinThread(sName)
{
	m_dwStopTimeout = 5000;
	WK_TRACE(_T("creating CVideoStateCheckThread(%s)\n"), sName);
	m_bAutoDelete = TRUE;

	for (int nI = 0; nI < MAX_IP_CAMERAS; nI++)
		m_dwCurrentVideoState[nI] = CAMS_VIDEO_OK;
}

/////////////////////////////////////////////////////////////////////////////
CVideoStateCheckThread::~CVideoStateCheckThread()
{

}

/////////////////////////////////////////////////////////////////////////////
BOOL CVideoStateCheckThread::InitInstance()
{
	CWinThread::InitInstance();
	SetThreadTimer(VIDEOSTATE_TIMER, 10000);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
int CVideoStateCheckThread::ExitInstance()
{
	KillThreadTimer(VIDEOSTATE_TIMER);

	return CWinThread::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CVideoStateCheckThread, CWK_WinThread)
	//{{AFX_MSG_MAP(CVideoStateCheckThread)
	ON_THREAD_MESSAGE(WM_TIMER, OnTimer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
void CVideoStateCheckThread::OnTimer(WPARAM lEvent, LPARAM lTime)
{
	if (lEvent == VIDEOSTATE_TIMER)
	{
		CIpCameraUnitDlg* pDlg = theApp.GetDlg();
		if (pDlg)
		{
			pDlg->m_ThreadList.Lock();
			int i, n = pDlg->m_ThreadList.GetItemCount();
			for (i=0; i<n; i++)
			{
				CDsCamera*pDST = (CDsCamera*) pDlg->m_ThreadList.GetItemData(i);
				if (pDST && pDST->IsThreadRunning())
				{
					WORD  wCamID   = pDST->GetCameraSubID();
					Sleep(1000);
					WK_TRACE(_T("GetVideoStatus() from Cam %d start\n"), wCamID);
					DWORD dwResult = pDST->GetVideoStatus();
					WK_TRACE(_T("GetVideoStatus() from Cam %d ready\n"), wCamID);
					if (dwResult != m_dwCurrentVideoState[wCamID])
					{
						m_dwCurrentVideoState[wCamID] = dwResult;
						CIPCOutputIPcamUnit*pOutput = pDlg->GetOutput(pDST->GetCameraSubID());
						if (pOutput && pOutput->IsConnected())
							pOutput->IndicationVideoState(pDST->GetCameraSubID(), (dwResult == CAMS_VIDEO_OK) ? TRUE : FALSE);
					}
				}
			}
			pDlg->m_ThreadList.Unlock();
		}
	}
}
