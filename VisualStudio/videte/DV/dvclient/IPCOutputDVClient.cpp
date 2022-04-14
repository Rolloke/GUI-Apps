// IPCOutputDVClient.cpp: implementation of the CIPCOutputDVClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dvclient.h"
#include "IPCOutputDVClient.h"

#include "Server.h"
#include "MainFrame.h"
#include "LiveWindow.h"
#include "CPanel.h"
#include "CMDConfigDlg.h"
#include <NotificationMessage.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPCOutputDVClient::CIPCOutputDVClient(LPCTSTR shmName, CServer* pServer)
:CIPCOutputClient(shmName,FALSE)
{
	m_pServer = pServer;
#ifdef _UNICODE
	m_wCodePage = pServer->GetOutputCodePage();
#endif
	SetConnectRetryTime(50);
	StartThread();
}
//////////////////////////////////////////////////////////////////////
CIPCOutputDVClient::~CIPCOutputDVClient()
{
	StopThread();
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputDVClient::OnConfirmConnection()
{
	WK_TRACE(_T("CIPCOutputDVClient::OnConfirmConnection %s\n"),m_pServer->GetHostName());
	CIPCOutputClient::OnConfirmConnection();
	DoRequestGetValue(SECID_NO_ID,CSD_VOUT_COUNT,0);
	theApp.ForceOnIdle();
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputDVClient::OnRequestDisconnect()
{
	WK_TRACE(_T("CIPCOutputDVClient::OnRequestDisconnect %s\n"),GetShmName());
	DelayedDelete();
	AfxGetMainWnd()->PostMessage(WM_USER,MAKELONG(WPARAM_DISCONNECT,0),0);
	theApp.ForceOnIdle();
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputDVClient::OnConfirmInfoOutputs(WORD wGroupID, int iNumGroups, int numRecords, const CIPCOutputRecord records[])
{
	TRACE(_T("OnConfirmInfoOutputs\n"));
	CIPCOutputClient::OnConfirmInfoOutputs(wGroupID,iNumGroups,numRecords,records);
	CPanel* pPanel = theApp.GetPanel();
	if (pPanel)
	{
		pPanel->NotifyCIPCConnect(SC_OUTPUT);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputDVClient::OnAddRecord(const CIPCOutputRecord & rec)
{
	WK_TRACE(_T("output add record %s, %08lx, %04lx\n"),
		rec.GetName(),rec.GetID(),rec.GetFlags());
	CWnd* pWnd = theApp.GetMainWnd();
	if (pWnd)
	{
		if (rec.IsEnabled()	&& rec.IsOkay())
		{
			if (rec.IsCamera())
			{
				pWnd->PostMessage(WM_USER, MAKELONG(WPARAM_ADD_CAMERA,0), rec.GetID().GetID());
			}
			else
			{
				pWnd->PostMessage(WM_USER, MAKELONG(WPARAM_ADD_RELAIS,0), rec.GetID().GetID());
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputDVClient::OnUpdateRecord(const CIPCOutputRecord & rec)
{
	WK_TRACE(_T("output update record %s, %08lx, %04lx\n"),
		rec.GetName(),rec.GetID(),rec.GetFlags());
	CMainFrame* pMF = theApp.GetMainFrame();
	if (pMF)
	{
		if (rec.IsCamera())
		{
			CLiveWindow* pLW = pMF->GetLiveWindow(rec.GetID().GetSubID());
			if (pLW)
			{
				if (rec.IsEnabled() && rec.IsOkay())
				{
					pMF->PostMessage(WM_USER, MAKELONG(WPARAM_UPD_CAMERA,0), rec.GetID().GetID());
				}
				else
				{
					pMF->PostMessage(WM_USER, MAKELONG(WPARAM_DEL_CAMERA,0), rec.GetID().GetID());
				}
			}
			else
			{
				if (rec.IsEnabled() && rec.IsOkay())
				{
					pMF->PostMessage(WM_USER, MAKELONG(WPARAM_ADD_CAMERA,0), rec.GetID().GetID());
				}
			}
		}
		else
		{
			pMF->PostMessage(WM_USER, MAKELONG(WPARAM_UPD_RELAIS,0), rec.GetID().GetID());
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputDVClient::OnDeleteRecord(const CIPCOutputRecord & rec)
{
	WK_TRACE(_T("output delete record %s, %08lx, %04lx\n"),
		rec.GetName(),rec.GetID(),rec.GetFlags());
	CWnd* pWnd = theApp.GetMainWnd();
	if (pWnd)
	{
		if (rec.IsCamera())
		{
			pWnd->PostMessage(WM_USER, MAKELONG(WPARAM_DEL_CAMERA,0), rec.GetID().GetID());
		}
		else
		{
			pWnd->PostMessage(WM_USER, MAKELONG(WPARAM_DEL_RELAIS,0), rec.GetID().GetID());
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputDVClient::OnConfirmJpegEncoding(const CIPCPictureRecord &pict, DWORD dwUserID, CSecID idArchive)
{
	CMainFrame* pMF = theApp.GetMainFrame();
	if (pMF)
	{
		pMF->LivePictureData(pict, 0xffffffff, 0xffffffff, CSecID(dwUserID));
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputDVClient::OnIndicateVideo(const CIPCPictureRecord& pict,
										 DWORD dwMDX,
										 DWORD dwMDY,
										 DWORD dwUserData, CSecID idArchive)
{
	CMainFrame* pMF = theApp.GetMainFrame();
	if (pMF)
	{
		pMF->LivePictureData(pict, dwMDX, dwMDY, CSecID(dwUserData));
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputDVClient::OnConfirmGetValue(CSecID id,
											const CString &sKey,
											const CString &sValue,
											DWORD dwServerData)
{
	TRACE(_T("CIPCOutputDVClient::OnConfirmGetValue %08lx,%s=%s\n"),id.GetID(),sKey,sValue);
	CLiveWindow* pLW = theApp.GetMainFrame()->GetLiveWindow(id.GetSubID());
	if (pLW)
	{
		// Diese Anfrage nur an das Serverobjekt weiterleten!
		if (   sKey == CSD_AGC
			|| sKey == CSD_VIDEO_FORMAT
			|| sKey == CSD_VIDEO_NORM)
		{
			m_pServer->ConfirmGetValue(sKey,sValue);
		}
		else
		{
			pLW->ConfirmGetValue(sKey,sValue);
			
			// Diese Anfrage auch an das Serverobjekt weiterleten!
			if (   sKey == CSD_CARD_TYPE)
				m_pServer->ConfirmGetValue(sKey,sValue);
		}
	}
	else
	{
		m_pServer->ConfirmGetValue(sKey,sValue);
		// WK_TRACE(_T("no window for confirm get value %08lx,%s=%s\n"),id.GetID(),sKey,sValue);
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputDVClient::OnConfirmSetValue(CSecID id,
											const CString &sKey,
											const CString &sValue,
											DWORD dwServerData)
{
//	TRACE(_T("CIPCOutputDVClient::OnConfirmSetValue() %08lx,%s=%s\n"),id.GetID(),sKey,sValue);
	CLiveWindow* pLW = theApp.GetMainFrame()->GetLiveWindow(id.GetSubID());
	if (pLW)
	{
		// Diese Anfrage nur an das Serverobjekt weiterleten!
		if	(  sKey == CSD_AGC
			|| sKey == CSD_VIDEO_FORMAT
			|| sKey == CSD_VIDEO_NORM)
		{
			m_pServer->ConfirmSetValue(sKey,sValue);
		}
		else
		{
			pLW->ConfirmSetValue(sKey,sValue);
		}
	}
	else
	{
		m_pServer->ConfirmSetValue(sKey,sValue);
		// WK_TRACE(_T("no window for confirm set value %08lx,%s=%s\n"),id.GetID(),sKey,sValue);
	}
}
//////////////////////////////////////////////////////////////////////////////////////
int CIPCOutputDVClient::GetNrOfActiveCameras()
{
	int r = 0;

	for (int i=0;i<GetNumberOfOutputs();i++)
	{
		const CIPCOutputRecord& rec = GetOutputRecordFromIndex(i);
		if (  rec.IsEnabled() 
			&& rec.IsOkay()
			&& rec.IsCamera())
		{
			r++;
		}
	}

	return r;
}
//////////////////////////////////////////////////////////////////////////////////////
int CIPCOutputDVClient::GetNrOfCameras()
{
	int r = 0;

	for (int i=0;i<GetNumberOfOutputs();i++)
	{
		const CIPCOutputRecord& rec = GetOutputRecordFromIndex(i);
		if (rec.IsCamera())
		{
			r++;
		}
	}

	return r;
}
//////////////////////////////////////////////////////////////////////////////////////
CSecID CIPCOutputDVClient::GetCamIDFromNr(WORD wNr)
{
	CSecID id;
	for (int i=0;i<GetNumberOfOutputs();i++)
	{
		const CIPCOutputRecord& rec = GetOutputRecordFromIndex(i);
		if (   rec.IsCamera()
			&& rec.GetID().GetSubID() == wNr)
		{
			id = rec.GetID();
			break;
		}
	}
	return id;
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputDVClient::OnConfirmGetMask(CSecID camID, DWORD dwUserID, const CIPCActivityMask& mask)
{
	WK_TRACE(_T("OnConfirmGetMask %08lx\n"),camID.GetID());
	CPanel* pPanel = theApp.GetPanel();

	if (pPanel)
	{
		CMDConfigDlg* pMDDialog = pPanel->GetMDConfigDialog();
		if (pMDDialog)
		{
			pMDDialog->ConfirmGetMask(camID,mask);
		}
		else
		{
			WK_TRACE_ERROR(_T("OnConfirmGetMask %08lx no md config dialog\n"),camID.GetID());
		}
	}
	else
	{
		WK_TRACE_ERROR(_T("OnConfirmGetMask %08lx no panel\n"),camID.GetID());
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputDVClient::OnConfirmSetMask(CSecID camID, MaskType type, DWORD dwUserID)
{
	CPanel* pPanel = theApp.GetPanel();

	if (pPanel)
	{
		CMDConfigDlg* pMDDialog = pPanel->GetMDConfigDialog();
		if (pMDDialog)
		{
			pMDDialog->ConfirmSetMask(camID,type);
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputDVClient::OnRequestSelectCamera(CSecID camID)
{
	CPanel* pPanel = theApp.GetPanel();

	if (pPanel)
	{
		//only call SetCamera() if not the same camera is selected. This function 
		//OnRequestSelectCamera() is only called bey DVProcess::CallThread() and switches
		//a camera via connection to a receiver
		if(pPanel->GetActiveCamera() != camID.GetSubID())
		{
			pPanel->SetCamera(camID.GetSubID(), Live);
		}

	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputDVClient::OnRequestSetValue(	CSecID id, // might be used as group ID only
											const CString &sKey,
											const CString &sValue,
											DWORD dwServerData)
{
	if (id.IsNotificationMessageID())
	{
		if (sKey == NM_MESSAGE)
		{
			WK_TRACE(_T("new message is %s\n"),sValue);
			CMainFrame* pMF = theApp.GetMainFrame();
			if(pMF)
			{
				//save the text to show in debug window
				pMF->SetMaintenanceText(sValue);

				//lParam = 0 --> open debug window
				//lParam = 1 --> open maintenance text window
				pMF->PostMessage(WM_USER, WPARAM_ADD_DEBUG,(LPARAM)1);
			}
		}
	}
}