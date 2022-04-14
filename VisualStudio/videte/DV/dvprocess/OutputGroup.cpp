// OutputGroup.cpp: implementation of the COutputGroup class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DVProcess.h"
#include "OutputGroup.h"
#include "camera.h"
#include "OutputClient.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
COutputGroup::COutputGroup(LPCTSTR shmName, int wGroup)
  : CIPCOutput(shmName, TRUE)
{
	m_wGroup = (WORD)wGroup;
	CString sShm(shmName);
	WK_TRACE(_T("connecting to %s\n"),sShm);
	DWORD dwUnit = 1;
	CString sName,sNr;
	swscanf(sShm,_T("%s%d"),sName.GetBuffer(_MAX_PATH),dwUnit);
	sName.ReleaseBuffer();
	sName.MakeLower();
	sNr.Format(_T("%d"),dwUnit);

	m_bIsJaCob = FALSE;
	m_bIsSaVic = FALSE;
	m_bIsTasha = FALSE;
	m_bIsQ = FALSE;
	if (-1!=sName.Find(_T("savic")))
	{
		m_sAppname = WK_APP_NAME_SAVICUNIT1;
		m_bIsSaVic = TRUE;
	}
	else if (-1!=sName.Find(_T("mico")))
	{
		m_sAppname = WK_APP_NAME_JACOBUNIT1;
		m_bIsJaCob = TRUE;
	}
	else if (-1!=sName.Find(_T("tasha")))
	{
		m_sAppname = WK_APP_NAME_TASHAUNIT1;
		m_bIsTasha = TRUE;
	}
	else if (-1!=sName.Find(_T("qunit")))
	{
		m_sAppname = WK_APP_NAME_QUNIT;
		m_bIsQ = TRUE;
	}
	m_sAppname.Replace(_T("1"),sNr);
	WK_TRACE(_T("%s appname is %s\n"),sShm,m_sAppname);
}
//////////////////////////////////////////////////////////////////////
COutputGroup::~COutputGroup()
{
}
//////////////////////////////////////////////////////////////////////
BOOL COutputGroup::IsJacob() const
{
	return m_bIsJaCob;
}
//////////////////////////////////////////////////////////////////////
BOOL COutputGroup::IsSavic() const
{
	return m_bIsSaVic;
}
//////////////////////////////////////////////////////////////////////
BOOL COutputGroup::IsTasha() const
{
	return m_bIsTasha;
}
//////////////////////////////////////////////////////////////////////
BOOL COutputGroup::IsQ() const
{
	return m_bIsQ;
}
//////////////////////////////////////////////////////////////////////
BOOL COutputGroup::HasCameras() const
{
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
int COutputGroup::GetNrOfActiveCameras()
{
	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////
CString COutputGroup::GetAppname() const
{
	return m_sAppname;
}
//////////////////////////////////////////////////////////////////////
COutput* COutputGroup::GetOutputByClientID(CSecID id)
{
	Lock();
	for (int j=0;j<GetSize();j++)
	{
		COutput* pOutput = GetAtFast(j);
		if (pOutput->GetClientID() == id)
		{
			Unlock();
			return pOutput;
		}
	}

	Unlock();

	return NULL;
}
//////////////////////////////////////////////////////////////////////
void COutputGroup::Load(CWK_Profile& wkp)
{
	CString sSection;
	CString sKey;
	CString sEntry;
	sSection.Format(_T("OutputGroups\\Group%04lx"),GetID());

	for (int i=0;i<GetSize();i++)
	{
		COutput* pOutput = GetAtFast(i);
		sKey.Format(_T("%d"),i);
		sEntry = wkp.GetString(sSection,sKey,_T(""));
		pOutput->FromString(sEntry);
	}
}
//////////////////////////////////////////////////////////////////////
void COutputGroup::Save(CWK_Profile& wkp)
{
	CString sSection;
	CString sKey;
	sSection.Format(_T("OutputGroups\\Group%04lx"), GetID());

	int iOldComport = -1;
	for (int i=0; i<GetSize(); i++)
	{
		COutput* pOutput = GetAtFast(i);
		sKey.Format(_T("%d"),i);
		wkp.WriteString(sSection,sKey,pOutput->ToString());
		CWK_Profile wkpDVRT;

		// sn save under CommUnit
		if (HasCameras())
		{
			CCamera* pCamera = (CCamera*)pOutput;
			int iComPort = pCamera->GetComPort();
			if (iComPort != 0)
			{
				CString sSection;
				sSection.Format(_T("CommUnit\\COM%d"), iComPort);
				CameraControlType typeExisting = (CameraControlType)wkpDVRT.GetInt(sSection,_T("Typ"), (int)CCT_UNKNOWN);
				CameraControlType type         = pCamera->GetType();
				if (   (typeExisting == CCT_UNKNOWN && type != CCT_UNKNOWN)
					|| (typeExisting != CCT_UNKNOWN && type == typeExisting))
				{
					CString sValue, sKey, sTemp;
					CSecID idCamera = pCamera->GetUnitID();
					DWORD dwPTZID = pCamera->GetPTZID();
					int iNum = wkpDVRT.GetInt(sSection,_T("Num"),0);
					sValue.Format(_T("%08x"), idCamera.GetID());

					for (int j=0 ; j<iNum ; j++)
					{
						sKey.Format(_T("SID%d"), j);
						sTemp = wkpDVRT.GetString(sSection,sKey,_T(""));
						if (sTemp == sValue)
						{
							iNum = j;
							break;
						}
					}
					wkpDVRT.WriteInt(sSection,_T("Typ"),(int)type);
					sKey.Format(_T("SID%d"), iNum);
					wkpDVRT.WriteString(sSection, sKey, sValue);
					sKey.Format(_T("CID%d"), iNum);
					wkpDVRT.WriteInt(sSection, sKey, dwPTZID);
					wkpDVRT.WriteInt(sSection,_T("Num"), ++iNum);
					iOldComport = -1;
				}
				else if (type != CCT_UNKNOWN)
				{
					int iCom=iComPort;
					if (iOldComport == -1)
					{
						iOldComport = iComPort;
					}
					DWORD dwComBit, dwMask   = GetCommPortInfo();
					if (dwMask)
					{
						for (iCom++; ; iCom++)
						{
							if (iCom > 32)
							{
								iCom = 0;
							}
							else
							{
								dwComBit = dwComBit=1<<(iCom-1);
								if (dwMask&dwComBit)
								{
									iComPort = iCom;
									pCamera->SetComPort(iComPort);
									break;
								}
							}
						}
						if (iCom <= 32 && iOldComport != iComPort)
						{
							i--;
							continue;
						}
						else
						{
							WK_TRACE(_T("no free Port for Cam:%s, %s (%d)\n"), pCamera->GetName(), NameOfEnum(type), type);
							theApp.m_bErrorOccurred = TRUE;
						}
					}
				}
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
void COutputGroup::StartRequests()
{
	// base class does nothing
}
//////////////////////////////////////////////////////////////////////
void COutputGroup::StopRequests()
{
	// base class does nothing
}
//////////////////////////////////////////////////////////////////////
void COutputGroup::SaveReferenceImage()
{
	// base class does nothing
}
//////////////////////////////////////////////////////////////////////
void COutputGroup::OnReadChannelFound()
{
	DoRequestConnection();
}
//////////////////////////////////////////////////////////////////////
void COutputGroup::OnConfirmConnection()
{
	CIPC::OnConfirmConnection();
	WK_TRACE(_T("connected to %s %04hx\n"),GetShmName(),m_wGroup);
	DoRequestSetGroupID(m_wGroup);
}
//////////////////////////////////////////////////////////////////////
void COutputGroup::OnRequestDisconnect()
{
	WK_TRACE(_T("disconnect from %s\n"),GetShmName());
}
//////////////////////////////////////////////////////////////////////
void COutputGroup::OnConfirmSetGroupID(WORD wGroupID)
{
	DoRequestHardware(m_wGroup);
}
//////////////////////////////////////////////////////////////////////
void COutputGroup::OnConfirmHardware(WORD wGroupID, int errorCode,
									 BOOL bCanSWCompress,
									 BOOL bCanBWDecompress,
									 BOOL bCanColorCompress,
									 BOOL bCanColorDecompress,
									 BOOL bCanOverlay
						)
{
	m_iHardware = errorCode;
	
	if (m_iHardware==0) 
	{
		DoRequestReset(m_wGroup);
	}
	COutputClients &clients=theApp.GetOutputClients();
	clients.Lock();
	for (int ci=0;ci<clients.GetSize();ci++) 
	{
		CIPCOutputDVClient* pCIPCOutputDVClient = clients[ci]->GetCIPC();
		if (   pCIPCOutputDVClient
			&& pCIPCOutputDVClient->GetIPCState() == CIPC_STATE_CONNECTED)
		{
			pCIPCOutputDVClient->DoConfirmHardware(	wGroupID, 
													m_iHardware,
													bCanSWCompress,
													bCanBWDecompress,
													bCanColorCompress,
													bCanColorDecompress,
													bCanOverlay
													);
		}
	}
	clients.Unlock();
}
//////////////////////////////////////////////////////////////////////
void COutputGroup::OnConfirmReset(WORD wGroupID)
{
}
//////////////////////////////////////////////////////////////////////
void COutputGroup::OnConfirmGetValue(CSecID id,
									const CString &sKey,
									const CString &sValue,
									DWORD dwServerData)
{
	if (dwServerData != 0)
	{
		COutputClients &clients = theApp.GetOutputClients();
		clients.Lock();
		COutputClient* pClient = clients.GetOutputClient(dwServerData);
		if (pClient && pClient->IsConnected()) 
		{
			if (IsInArray(id.GetSubID(), GetSize()))
			{
				COutput* pOutput = GetAtFast(id.GetSubID());
				pClient->GetCIPC()->DoConfirmGetValue(pOutput->GetClientID(),sKey,sValue,0);
			} 
			else if (id.GetSubID() == SECID_NO_SUBID)
			{
				pClient->GetCIPC()->DoConfirmGetValue(id, sKey, sValue, 0);
			}
			else
			{
				WK_TRACE_ERROR(_T("OnConfirmGetValue(%s,%s) Wrong Subid %x\n"), sKey,sValue, id.GetSubID());
			}
		}
		else // client already disappeared
		{
			WK_TRACE_ERROR(_T("OnConfirmGetValue(%s,%s) client %x not found\n"), sKey,sValue,dwServerData);
		}
		clients.Unlock();
	}
	else
	{
		// request get value was process itself
		WK_TRACE(_T("confirm get %s = %s\n"),sKey,sValue);
	}
}
//////////////////////////////////////////////////////////////////////
void COutputGroup::OnConfirmSetValue(CSecID id,
									const CString &sKey,
									const CString &sValue,
									DWORD dwServerData)
{
	if (dwServerData != 0)
	{
		COutputClients &clients = theApp.GetOutputClients();
		clients.Lock();
		COutputClient* pClient = clients.GetOutputClient(dwServerData);
		if (pClient && pClient->IsConnected()&& IsInArray(id.GetSubID(), GetSize())) 
		{
			COutput* pOutput = GetAtFast(id.GetSubID());
			pClient->GetCIPC()->DoConfirmSetValue(
                pOutput->GetClientID(),sKey,sValue,0);
		} 
		else 
		{
			// client already disappeared
			WK_TRACE_ERROR(_T("OnConfirmSetValue(%s,%s) client %x not found\n"),
				sKey,sValue,dwServerData);
		}
		clients.Unlock();
	}
	else
	{
		// request set value was process itself
		if (   sKey != CSD_V_OUT
			&& sKey != CSD_DIB_TRANSFER
			&& (-1 == sKey.Find(_T("LED"))))
		{
			WK_TRACE(_T("confirm set %08lx %s = %s\n"),id.GetID(),sKey,sValue);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void COutputGroup::OnConfirmSetRelay(CSecID relayID, BOOL bClosed)
{
	COutputClients &clients = theApp.GetOutputClients();
	clients.Lock();

	for (int i=0;i<clients.GetSize();i++)
	{
		COutputClient* pClient = clients.GetAtFast(i);
		if (pClient->IsConnected()) 
		{
			pClient->GetCIPC()->DoConfirmSetRelay(relayID,bClosed);
		} 
	}
	clients.Unlock();
}
//////////////////////////////////////////////////////////////////////
void COutputGroup::OnIndicateError(DWORD dwCmd, CSecID id, 
								 CIPCError error, int iErrorCode,
								 const CString &sErrorMessage)
{
	WK_TRACE(_T("OnIndicateError %s, %s, %s\n"),GetShmName(),NameOfCmd(dwCmd),sErrorMessage);
}
//////////////////////////////////////////////////////////////////////
void COutputGroup::OnRequestSetValue(CSecID id,
									const CString &sKey,
									const CString &sValue,
									DWORD dwServerData)
{
	if (sKey == CSD_RESET)
	{
		theApp.SetReset(TRUE,FALSE,FALSE);
	}
}
