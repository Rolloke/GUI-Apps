// IPCControl.cpp: implementation of the CIPCControl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IdipClient.h"
#include "mainfrm.h"
#include "CIPCControl.h"
#include "server.h"
#include "IdipClientDoc.h"
#include "ViewIdipClient.h"
#include "ViewCamera.h"
#include "ViewRelais.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CIPCControl::CIPCControl(LPCTSTR shmName, CServer* pServer)
	: CIPC(shmName,FALSE)
{
	m_pServer = pServer;
	m_dwTick = GetTickCount();
	StartThread();
}
//////////////////////////////////////////////////////////////////////
CIPCControl::~CIPCControl()
{
	StopThread();
}
//////////////////////////////////////////////////////////////////////
BOOL CIPCControl::Run(DWORD dwTimeOut)
{
	DWORD dwTick = GetTimeSpan(m_dwTick);
	if (   GetIPCState() == CIPC_STATE_WRITE_CREATED
		&& dwTick>1000)
	{
		WK_TRACE(_T("disconnecting unconnected control %s\n"), m_pServer->GetName());
		DoDisconnect();
	}

	return CIPC::Run(dwTimeOut);
}
//////////////////////////////////////////////////////////////////////
void CIPCControl::OnReadChannelFound()
{
	DoRequestConnection();
}
//////////////////////////////////////////////////////////////////////
void CIPCControl::OnConfirmConnection()
{
	WK_TRACE(_T("CIPCControl %s connected\n"),m_pServer->GetName());
	CViewCamera* pOV = theApp.GetMainFrame()->GetViewCamera();
	if (pOV)
	{
		pOV->PostMessage(WM_USER,MAKELONG(WPARAM_CONTROL_CONNECTED, m_pServer->GetID()));
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCControl::OnRequestDisconnect()
{
	WK_TRACE(_T("CIPCControl %s disconnected\n"),m_pServer->GetName());
	m_pServer->SetAlarmID(SECID_NO_ID);
	DelayedDelete();
	CViewCamera* pOV = theApp.GetMainFrame()->GetViewCamera();
	if (pOV)
	{
		pOV->PostMessage(WM_USER,MAKELONG(WPARAM_CONTROL_DISCONNECTED, m_pServer->GetID()));
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCControl::OnRequestGetValue(CSecID id, // might be used as group ID only
								   const CString &sKey,
								   DWORD dwServerData)
{
}
//////////////////////////////////////////////////////////////////////
void CIPCControl::OnConfirmGetValue(CSecID id, // might be used as group ID only
									const CString &sKey,
									const CString &sValue,
									DWORD dwServerData
									)
{
}
//////////////////////////////////////////////////////////////////////
void CIPCControl::OnConfirmSetValue(CSecID id, // might be used as group ID only
									const CString &sKey,
									const CString &sValue,
									DWORD dwServerData
									)
{
}
//////////////////////////////////////////////////////////////////////
void CIPCControl::OnRequestSetValue(CSecID id, // might be used as group ID only
									const CString &sKey,
									const CString &sValue,
									DWORD dwServerData
									)
{
	// it's for gemos
	WK_TRACE(_T("OnRequestSetValue(%08lx,%s,%s,%d)\n"),id.GetID(),sKey,sValue,dwServerData);
	if (id == SECID_NO_ID)
	{
		if (m_pServer->IsControl())
		{
			if (0==sKey.CompareNoCase(_T("cam")))
			{
				WK_TRACE(_T("control switch cam %s %s\n"), sValue, dwServerData ? _T("on") : _T("off"));
				DWORD dwCamSubID = 0;
				if (1 == _stscanf(sValue,_T("%d"),&dwCamSubID))
				{
					CViewCamera* pView = theApp.GetMainFrame()->GetViewCamera();
					if (pView && m_pServer->IsConnectedLive())
					{
						CSecID camID;
						camID = m_pServer->GetOutput()->GetCamID((WORD)dwCamSubID);

						if (camID.IsOutputID())
						{
							m_pServer->SetAlarmID(camID);

							if (dwServerData)
							{
								pView->PostMessage(WM_USER, MAKELONG(WPARAM_CONTROL_SELECT_CAM, m_pServer->GetID()), camID.GetID());
							}
							else
							{
								pView->PostMessage(WM_USER, MAKELONG(WPARAM_CONTROL_UNSELECT_CAM, m_pServer->GetID()), camID.GetID());
							}
						}
						else
						{
							DoConfirmSetValue(id, _T("err"), sValue, dwServerData);
						}
					}
					else
					{
						DoConfirmSetValue(id, _T("err"), sValue, dwServerData);
					}
				}
				else
				{
					WK_TRACE_ERROR(_T("OnRequestSetValue invalid value for key %s\n"),sKey);
					DoConfirmSetValue(id,_T("err"), sValue, dwServerData);
				}
			}
			else if (0==sKey.CompareNoCase(_T("relay")))
			{
				WK_TRACE(_T("control switch relay %s %s\n"),sValue,
					dwServerData ? _T("on") : _T("off"));
				DWORD dwRelaySubID = 0;
				if (1 == _stscanf(sValue,_T("%d"),&dwRelaySubID))
				{
					CViewRelais* pView = theApp.GetMainFrame()->GetViewRelais();
					if (pView)
					{
						CSecID relayID;
						relayID = m_pServer->GetOutput()->GetRelayIDFromNr((WORD)dwRelaySubID);
						
						if (dwServerData)
						{
							pView->PostMessage(WM_USER,
											   MAKELONG(WPARAM_CONTROL_SELECT_RELAY,m_pServer->GetID()),
											   relayID.GetID());
						}
						else
						{
							pView->PostMessage(WM_USER,
											   MAKELONG(WPARAM_CONTROL_UNSELECT_RELAY,m_pServer->GetID()),
											   relayID.GetID());
						}
					}
					else
					{
						DoConfirmSetValue(id,_T("err"),sValue,dwServerData);
					}
				}
				else
				{
					WK_TRACE_ERROR(_T("OnRequestSetValue invalid value for key %s\n"),sKey);
					DoConfirmSetValue(id,_T("err"),sValue,dwServerData);
				}
			}
			else if (0==sKey.CompareNoCase(_T("fullscreen")))
			{
				WK_TRACE(_T("control switch fullscreen %s\n"), dwServerData ? _T("on") : _T("off"));
				theApp.GetMainFrame()->ChangeFullScreenModus((dwServerData ? TRUE : FALSE)|FULLSCREEN_FORCE|FULLSCREEN_POST);
			}
			else
			{
				WK_TRACE_ERROR(_T("OnRequestSetValue invalid key <%s>\n"), sKey);
				DoConfirmSetValue(id,_T("err"),sValue,dwServerData);
			}
		}
	}
	else
	{
		DoConfirmSetValue(id,_T("err"),sValue,dwServerData);
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCControl::ConfirmCamera(CSecID idCam, GemosInputState eState)
{
	int nCamNr = m_pServer->GetOutput()->GetNrFromCamID(idCam);
	if (nCamNr != -1)
	{
		CString sValue;
		sValue.Format(_T("%d"),nCamNr);
		DoConfirmSetValue(SECID_NO_ID,_T("cam"),sValue, eState);
	}
}

//ML 29.9.99 Begin Insertation{
void CIPCControl::ConfirmRelay(CSecID idRelay, GemosInputState eState)
{
	int nRelayNr = m_pServer->GetOutput()->GetNrFromRelayID(idRelay);
	if (nRelayNr != -1)
	{
		CString sValue;
		sValue.Format(_T("%d"),nRelayNr);
		DoConfirmSetValue(SECID_NO_ID,_T("relay"),sValue, eState);
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCControl::ConfirmAlarm(CSecID idAlarm, GemosInputState eState)
{
	int nAlarmNr = m_pServer->GetInput()->GetNrFromAlarmID(idAlarm);
	if (nAlarmNr != -1)
	{
		CString sValue;
		sValue.Format(_T("%d"),nAlarmNr);
		DoConfirmSetValue(SECID_NO_ID,_T("alarm"),sValue, eState);
	}
}

//ML 29.9.99 End Insertation}
