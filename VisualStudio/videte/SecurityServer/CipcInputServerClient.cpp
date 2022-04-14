/* GlobalReplace: pApp\-\> --> theApp. */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CipcInputServerClient.cpp $
// ARCHIVE:		$Archive: /Project/SecurityServer/CipcInputServerClient.cpp $
// CHECKIN:		$Date: 30.08.06 12:19 $
// VERSION:		$Revision: 132 $
// LAST CHANGE:	$Modtime: 30.08.06 11:58 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#include "stdafx.h"

#include "CipcInputServerClient.h"
#include "CIPCServerControlServerSide.h"
#include "CIPCServerControlClientSide.h"
#include "InputList.h"
#include "sec3.h"
#include "CIPCInputServer.h"
#include "CIPCstringdefs.h"
#include "wk_msg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

CIPCInputClientsArray::CIPCInputClientsArray()
{
#ifdef TRACE_LOCKS
	m_nCurrentThread = theApp.m_bTraceLocks ? 0 : -1;
#endif
}
/////////////////////////////////////////////////////////////////////////////////
CIPCInputServerClient* CIPCInputClientsArray::GetClientByID(CSecID id) const
{
	for (int i=0;i<GetSize();i++) 
	{
		if (GetAtFast(i)->GetID()==id) 
		{
			return GetAtFast(i);	// EXIT
		}
	}
	return NULL;
}
//////////////////////////////////////////////////////////////////////
int	CIPCInputClientsArray::GetNrOfLicensed()
{
	int r = 0;

	int i;
	CIPCInputServerClient* pInputClient = NULL;

	DWORD dwTick1 = GetTickCount();
	Lock(_T(__FUNCTION__));
	DWORD dwTick2 = GetTimeSpan(dwTick1);
	if (dwTick2>50)
	{
		WK_TRACE(_T("%s(%d) : lock in %d ms\n"),__FILE__,__LINE__,dwTick2);
	}

	for (i=0;i<GetSize();i++)
	{
		pInputClient = GetAtFast(i);
		if (   pInputClient 
			&& !pInputClient->GetSerial().IsEmpty()
			)
		{
			r++;
		}
	}
	Unlock();

	return r;
}
//////////////////////////////////////////////////////////////////////
BOOL CIPCInputClientsArray::IsConnected(const CString& sSerial,const CString& sSourceHost)
{
	int i;
	BOOL bFound = FALSE;
	CIPCInputServerClient* pInputClient = NULL;

	DWORD dwTick1 = GetTickCount();
	Lock(_T(__FUNCTION__));
	DWORD dwTick2 = GetTimeSpan(dwTick1);
	if (dwTick2>50)
	{
		WK_TRACE(_T("%s(%d) : lock in %d ms\n"),__FILE__,__LINE__,dwTick2);
	}

	for (i=0;i<GetSize();i++)
	{
		pInputClient = GetAtFast(i);
		if (   pInputClient
			&& pInputClient->GetIPCState() == CIPC_STATE_CONNECTED
			&& pInputClient->GetSerial()==sSerial
			&& sSourceHost != pInputClient->GetRemoteHost())
		{
			bFound = TRUE;
			break;
		}
	}
	Unlock();

	return bFound;
}
/////////////////////////////////////////////////////////////////////////////////
CIPCInputServerClient::CIPCInputServerClient(const CConnectionRecord &c,
											 CIPCServerControlServerSide *pControl,
											 LPCTSTR shmName,
											 WORD wNr)
	: CIPCInputFileUpdate(c, shmName, TRUE)
{
	m_ConnectionRecord = c;
	c.GetFieldValue(CRF_SERIAL,m_sSerial);
	m_dwTickInfoConfirmed = 0;
	m_dwConnectionTime = 0;
	m_pControl = pControl;
	m_id.Set(SECID_INPUT_CLIENT,wNr);
	const CString sPermission = c.GetPermission();
	CPermission *pPermission = theApp.GetPermissions().GetPermission(sPermission);
	
	if (pPermission) 
	{
		m_permissionID = pPermission->GetID();
	}

	m_sUserName = c.GetUser();
	m_bInfoConfirmed = FALSE;
	m_dwStarttimeForNotConnectedTimeout	= GetTickCount();
	m_dwNotConnectedTimeout = c.GetTimeout();

	SetConnectRetryTime(50);
}
/////////////////////////////////////////////////////////////////////////////////
CIPCInputServerClient::~CIPCInputServerClient()
{
	StopThread();
}
/////////////////////////////////////////////////////////////////////////////////
void CIPCInputServerClient::OnReadChannelFound()
{
	SetConnectRetryTime(500);	// once connected this could even be more
}
/////////////////////////////////////////////////////////////////////////////////
void CIPCInputServerClient::OnRequestConnection()
{
	CString sMsg;
	sMsg.Format(_T("%s,%s|%s|%s"),
		GetConnection().GetUser(),GetConnection().GetPermission(),
		GetConnection().GetSourceHost(),
		GetShmName());
	WK_STAT_LOG(_T("Client"),1,sMsg);
	m_dwStarttimeForNotConnectedTimeout = GetTickCount();
	CIPC::OnRequestConnection();
}
/////////////////////////////////////////////////////////////////////////////////
void CIPCInputServerClient::OnRequestGetFile(int iDestination, const CString &sFileName)
{
	if (   iDestination == RFU_STRING_INTERFACE 
		&& sFileName.GetLength() >= (int)_tcslen(SI_EXPORTREGISTRY)
		&& sFileName.Find(SI_EXPORTREGISTRY) == 0)
	{
		if (m_bAllowGetRegistry)
		{
			CString sTemp = sFileName + _T(":") + COemGuiApi::GetLanguageAbbreviation();
			CIPCInputFileUpdate::SIExportRegistry(sTemp);
		}
		else
		{
			DoIndicateError(INP_REQUEST_GET_FILE,SECID_NO_ID,CIPC_ERROR_GET_FILE, 6,sFileName);
		}
	}
	else
	{
		CIPCInputFileUpdate::OnRequestGetFile(iDestination, sFileName);
	}
}
/////////////////////////////////////////////////////////////////////////////////
void CIPCInputServerClient::OnRequestInfoInputs(WORD wGroupID)
{
	CHECK_ACCESS(INP_REQ_INFO);

	TRACE(_T("OnRequestInfoInputs %s %s\n"),GetShmName(), GetRemoteHost());

	int iNumRecords = 0;
	int iNumGroups = theApp.GetInputGroups().GetSize();
	CIPCInputServer* pInputGroup;

	for (int g=0;g<iNumGroups;g++) 
	{
		pInputGroup = theApp.GetInputGroups().GetGroupAt(g);
		if (pInputGroup)
		{
/*
			WK_TRACE(_T("Group %s %d Elements Hardware State %d\n"),
				pInputGroup->GetShmName(),
				pInputGroup->GetSize(),
				pInputGroup->GetHardwareState());
*/
			if (CString(SM_LAUNCHER_INPUT) == pInputGroup->GetShmName())
			{
				// virtuellen Daueralarm ausblenden
				iNumRecords += pInputGroup->GetSize()-1;
				if (pInputGroup->GetSize() > BIT_VALARM_ALARM_OFF_SPAN)
				{
					if (!theApp.HasAlarmOffSpanFeature())
					{
						// virtuellen Zeitspannen Alarm MNO ausblenden
						iNumRecords--;
					}
				}
			}
			else if (!pInputGroup->IsMD())
			{
				iNumRecords += pInputGroup->GetSize();
			}
		}
	}

	if (iNumRecords) 
	{
		CIPCInputRecord *records = new CIPCInputRecord[iNumRecords];
		int r=0; // record counter
		for (g=0,r=0;g<iNumGroups;g++) 
		{
			pInputGroup = theApp.GetInputGroups().GetGroupAt(g);
			if (!pInputGroup->IsMD())
			{
				DWORD dwOneBit=1;
				BYTE bFlags=0;
				for (int i=0;i<pInputGroup->GetSize();i++, dwOneBit<<1) 
				{
					const CInput *pInput = pInputGroup->GetInputAt(i);
					
					if (CString(SM_LAUNCHER_INPUT) == pInputGroup->GetShmName())
					{
						if (i == BIT_VALARM_ALWAYS_ALARM)
						{
							// virtuellen Daueralarm ausblenden
							continue;
						}
						if (i == BIT_VALARM_ALARM_OFF_SPAN)
						{
							if (!theApp.HasAlarmOffSpanFeature())
							{
								// virtuellen Zeitspannen Alarm MNO ausblenden
								continue;
							}
						}
					}
					
					bFlags = 0;
					// special commport flag
					if (pInput->wState & I_STATE_IS_COMM_PORT) 
					{
						bFlags |= IREC_IS_COMM_PORT;
					}

					switch (pInputGroup->GetState(i)) 
					{
						case GMS_ERROR:
							if (pInputGroup->GetFreeMask() & dwOneBit) 
							{
								bFlags |= IREC_STATE_ENABLED;
							}
							break;
						case GMS_INACTIVE:
						case GMS_ACTIVE:
							bFlags |= IREC_STATE_OKAY | IREC_STATE_ENABLED;
							break;
						case GMS_OFF:
							bFlags |= IREC_STATE_OKAY;
							break;
						default:
							WK_TRACE(_T("OnRequestInfoInputs:Wrong state\n"));
					}
					// IsActive is the real/physical state
					// signal ALARM if physical state is ALARM
					if (pInput->IsActive()) 
					{
						bFlags |= IREC_STATE_ALARM;
					}
					if (pInput->GetEdge()) 
					{
						bFlags |= IREC_STATE_EDGE;
					}
					if (pInput->GetTempDeactivateAllowed()) 
					{
						bFlags |= IREC_STATE_TEMP_DEACTIVATE;
					}
					if (r<iNumRecords)
					{
						records[r].Set(pInput->GetName(),pInput->GetID(),bFlags); 
						r++;
					}
				}
			}
		}
		DoConfirmInfoInputs(wGroupID, iNumGroups, iNumRecords,
								records);
		WK_DELETE_ARRAY(records);
	} else {
		// no records at all
		CIPCInputRecord dummyRecords[1];
		DoConfirmInfoInputs(wGroupID, iNumGroups, 0, dummyRecords);
	}
	m_dwTickInfoConfirmed = GetTickCount();
	m_bInfoConfirmed = TRUE;
}
/////////////////////////////////////////////////////////////////////////////////
void CIPCInputServerClient::OnRequestDisconnect()
{
	CString sMsg;
	sMsg.Format(_T("%s,%s|%s|%s"),
		GetConnection().GetUser(),
		GetConnection().GetPermission(),
		GetConnection().GetSourceHost(),
		GetShmName());
	WK_STAT_LOG(_T("Client"),0,sMsg);
	m_pControl->RemoveMe(this);
}
/////////////////////////////////////////////////////////////////////////////////
/*@MD 
@ITEm 1 default from CIPC
@ITEM 2 supports VersioInfo
*/
void CIPCInputServerClient::OnRequestVersionInfo(WORD wGroupID)
{
	DoConfirmVersionInfo(wGroupID,2);
}
/////////////////////////////////////////////////////////////////////////////////
BOOL CIPCInputServerClient::OnTimeoutWrite(DWORD dwCmd)	 // no CmdDone response
{
	if (GetNumTimeoutsWrite()>10) {
		WK_TRACE(_T("WriteTimeout, Disconnecting input client\n"));
		m_pControl->RemoveMe(this);	// OOPS
		return FALSE;
	} else {
		return TRUE;
	}
}
/////////////////////////////////////////////////////////////////////////////////
BOOL	CIPCInputServerClient::OnTimeoutCmdReceive()	// no new cmd received
{
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////////
void	CIPCInputServerClient::OnWaitFailed(DWORD dwCmd)
{
	// NOT YET
}
/////////////////////////////////////////////////////////////////////////////////
void CIPCInputServerClient::OnWriteCommData(CSecID id,
									const CIPCExtraMemory &data,
									DWORD dwBlockNr
									)
{
	CHECK_ACCESS(INP_WRITE_COMM_DATA);

	CIPCInputServer *pInputGroup = theApp.GetInputGroups().GetGroupByID(id);
	if (pInputGroup) 
	{
		pInputGroup->DoWriteCommData(id,data,dwBlockNr);
	} 
	else 
	{
		WK_TRACE(_T("CommUnit for %x not found\n"),id.GetID());
	}
}
/////////////////////////////////////////////////////////////////////////////////
void CIPCInputServerClient::OnRequestCameraControl(
								CSecID commID, CSecID camID,
								CameraControlCmd cmd,
								DWORD dwValue
								)
{
	CHECK_ACCESS(INP_REQUEST_CAMERA_CONTROL);

	COutput* pOutput = theApp.GetOutputGroups().GetOutputByID(camID);
	if (pOutput)
	{
		int iPTZInterface = pOutput->GetPTZInterface();
		if (iPTZInterface<33)
		{
			CIPCInputServer *pInputGroup = theApp.GetInputGroups().GetGroupByID(commID);
			if (pInputGroup) 
			{
				pInputGroup->DoRequestCameraControl(commID,camID, cmd, dwValue);
			} 
			else 
			{
				WK_TRACE(_T("CommUnit for %x not found\n"),commID.GetID());
			}
		}
		else if (iPTZInterface == PTZ_INTERFACE_HTTP)
		{
			// send ptz command over matching input connection
			CString sShmName = pOutput->GetGroup()->GetShmName();
			sShmName.Replace(_T("OutputCamera"),_T("Input"));
			CIPCInputServer *pInputGroup = theApp.GetInputGroups().GetGroupByShmName(sShmName);
			if (pInputGroup) 
			{
				pInputGroup->DoRequestCameraControl(commID,camID, cmd, dwValue);
			} 
			else 
			{
				WK_TRACE(_T("no input group for %08lx %s found\n"),camID.GetID(),NameOfEnum(cmd));
			}
		}
		else
		{
			// TODO other PTZ interfaces.
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////
/*@MD NYD*/
// REDUNDANT code in CIPCOutputServerClient
void CIPCInputServerClient::OnRequestGetValue(
						CSecID id, // might be used as group ID only
						const CString &sKey,
						DWORD dwServerData
					)
{
	CHECK_ACCESS(CIPC_BASE_REQUEST_GET_VALUE);

	CIPCInputServer *pGroup = theApp.GetInputGroups().GetGroupByID(id);
	if (pGroup) 
	{
		if (sKey==CSD_APPLICATION_NAME) 
		{
			const CString sShmName = pGroup->GetShmName();
			WK_ApplicationId appID;

			if (sShmName==SM_SIMUNIT_INPUT) {
				appID = WAI_SIMUNIT;
			} else if (sShmName==SM_GAUNIT_INPUT) {
				appID = WAI_GAUNIT;
			} else if (sShmName==SM_COMMUNIT_INPUT ) {
				appID = WAI_COMMUNIT;
			} else if (sShmName==SM_COCO_INPUT0 ) {
				appID = WAI_COCOUNIT;
			} else if (sShmName==SM_COCO_INPUT1 ) {	// OOPS
				appID = WAI_COCOUNIT;
			} else if (sShmName==SM_COCO_INPUT2 ) {	// OOPS
				appID = WAI_COCOUNIT;
			} else if (sShmName==SM_COCO_INPUT3 ) {	// OOPS
				appID = WAI_COCOUNIT;
			} else if (sShmName==SM_MICO_INPUT ) {
				appID = WAI_MICOUNIT;
			} else if (sShmName==SM_MICO_INPUT2 ) {
				appID = WAI_MICOUNIT_2;
			} else if (sShmName==SM_MICO_INPUT3 ) {
				appID = WAI_MICOUNIT_3;
			} else if (sShmName==SM_MICO_INPUT4 ) {
				appID = WAI_MICOUNIT_4;
			} else if (sShmName==SM_AKU_INPUT ) {
				appID = WAI_AKUUNIT;
			} else if (sShmName.Find(SM_SDIUNIT_INPUT)==0) {
				appID = WAI_SDIUNIT;
			} else {
				// unknown ?
				appID = WAI_INVALID;
				WK_TRACE_ERROR(_T("No ShmName --> AppName conversion for %s\n"),(LPCTSTR)sShmName);
			}
			CString sAppName = NameOfEnum(appID);
			// client can use StringToWK_ApplicationId(const CString &sId)
			DoConfirmGetValue(id,CSD_APPLICATION_NAME,sAppName,dwServerData);
		}
		else if (sKey == CSD_ALARM_SPAN_OFF_STATUS)
		{
			if (theApp.HasAlarmOffSpanFeature())
			{
				CInput* pInput = theApp.GetInputGroups().GetInputByID(id);
				if (pInput)
				{
					CString sValue(pInput->IsInAlarmOffSpan() ? CSD_ON : CSD_OFF);
					DoConfirmGetValue(id,CSD_ALARM_SPAN_OFF_STATUS,sValue,dwServerData);
				}
			}
		}
		else if (   0 == sKey.Find(CSD_CAM_CONTROL_TYPE)
				 || 0 == sKey.Find(CSD_CAM_CONTROL_FKT)
				 || 0 == sKey.Find(CSD_CAM_CONTROL_FKTEX))
		{
			DWORD dwCamID = 0;
			CString sID = sKey.Mid(sKey.GetLength()-9, 8);
			if (1==_stscanf((LPCTSTR)sID,_T("%08lx"),&dwCamID))
			{
				CSecID camID = dwCamID;
				if (camID.IsOutputID())
				{
					COutput* pOutput = theApp.GetOutputGroups().GetOutputByID(camID);
					if (   pOutput
						&& pOutput->GetPTZInterface()>32)
					{
						CString sShmName = pOutput->GetGroup()->GetShmName();
						sShmName.Replace(_T("OutputCamera"),_T("Input"));
						CIPCInputServer *pInputGroup = theApp.GetInputGroups().GetGroupByShmName(sShmName);
						if (pInputGroup) 
						{
							pGroup = pInputGroup;
						}
					}
				}
			}
			if (pGroup)
			{
				pGroup->DoRequestGetValue(id,sKey,m_id.GetID());
			}
		}
		else
		{
			pGroup->DoRequestGetValue(id,sKey,m_id.GetID());
		}
	} 
	else if (sKey == CSD_ALARM_SPAN_OFF_FEATURE)
	{
		if (theApp.HasAlarmOffSpanFeature())
		{
			CPermissionArray pa;
			CWK_Profile wkp;
			pa.Load(wkp);

			CPermission* pPermission = pa.GetPermission(m_permissionID);
			if (   pPermission 
				&& (pPermission->GetFlags() & WK_ALLOW_ALARM_OFF_SPAN)
				)
			{
				DoConfirmGetValue(id,CSD_ALARM_SPAN_OFF_FEATURE,CSD_ON,dwServerData);
			}
		}
	}
	else if (0 == sKey.CompareNoCase(CSD_IS_DV))
	{
		DoConfirmGetValue(id,sKey,CSD_OFF,dwServerData);
	}
	else 
	{
		WK_TRACE(_T("%s:OnRequestGetValue INVALID ID %x for key %s\n"),GetShmName(),id.GetID(),
			sKey);
	}

}
//////////////////////////////////////////////////////////////////////////////
void CIPCInputServerClient::OnRequestSetValue(
						CSecID id, // might be used as group ID only
						const CString &sKey,
						const CString &sValue,
						DWORD dwServerData
						)
{
	CHECK_ACCESS(CIPC_BASE_REQUEST_SET_VALUE);
	CIPCInputServer *pInput = theApp.GetInputGroups().GetGroupByID(id);
	if (pInput) 
	{
		pInput->DoRequestSetValue(id,sKey,sValue,m_id.GetID());
	}
	else 
	{
		WK_TRACE(_T("OnRequestSetValue INVALID ID %x\n"),id.GetID());
	}
}
//////////////////////////////////////////////////////////////////////////////
void CIPCInputServerClient::OnRequestResetApplication()
{
	// now reset myself
	if (theApp.GetMainWnd()) 
	{
		theApp.GetMainWnd()->PostMessage(WM_COMMAND,ID_EINSTELLUNGEN_SUPERVISOR);
	}
	HWND hWnd = FindWindow(WK_APP_NAME_LAUNCHER, NULL);
	if (hWnd)
	{
		::PostMessage(hWnd, LAUNCHER_APPLICATION, WAI_SUPERVISOR, MAKELONG(LAUNCHER_UPDATE_LANG_DEPENDS, 0));
	}
}
//////////////////////////////////////////////////////////////////////////////
void CIPCInputServerClient::OnRequestGetSystemTime()
{
	CSystemTime st;
	st.GetLocalTime();
	DoConfirmGetSystemTime(st);
	WK_TRACE(_T("get time requested by client %s confirming %s\n"),
		GetRemoteHost(),st.GetDateTime());
}
//////////////////////////////////////////////////////////////////////////////
void CIPCInputServerClient::OnRequestSetSystemTime(const CSystemTime& st)
{
	WK_TRACE(_T("set time %s requested by client %s\n"),
		st.GetDateTime(),GetRemoteHost());

	CPermissionArray pa;
	CWK_Profile wkp;
	pa.Load(wkp);

	CPermission* pPermission = pa.GetPermission(m_permissionID);
	if (   pPermission 
		&& (pPermission->GetFlags() & WK_ALLOW_DATE_TIME)
		)
	{
		CWnd* pWnd = theApp.GetMainWnd();
		if (WK_IS_WINDOW(pWnd)) 
		{
			CSystemTime my = st;
			if (SetLocalTime(&st))
			{
				DoConfirmSetSystemTime();

				WK_TRACE(_T("time set by client resetting\n"));
				pWnd->PostMessage(WM_COMMAND,ID_EINSTELLUNGEN_SUPERVISOR);

				// dem launcher bescheid dagen, daß wir die
				// uhrzeit geändert haben
				CWK_RunTimeError er(WAI_SECURITY_SERVER,REL_MESSAGE,RTE_TIME_CHANGED);
				er.Send();
				return;
			}
		}
	}

	CString s;
	s.LoadString(IDP_NO_PERMISSION_FOR_TIME);
	WK_TRACE_ERROR(_T("permission denied for time change\n"));
	DoIndicateError(GetLastCmd(),SECID_NO_ID,CIPC_ERROR_INVALID_PERMISSIONS,1,s);
}
//////////////////////////////////////////////////////////////////////////////
void CIPCInputServerClient::OnConfirmAlarm(CSecID id)
{
	WK_TRACE(_T("alarm %08lx confirmed by client %s\n"),
		id.GetID(),GetRemoteHost());
}
//////////////////////////////////////////////////////////////////////////////
void CIPCInputServerClient::OnRequestGetAlarmOffSpans(CSecID id)
{
	if (theApp.HasAlarmOffSpanFeature())
	{
		CInput* pInput = theApp.GetInputGroups().GetInputByID(id);
		if (pInput)
		{
			DoConfirmGetAlarmOffSpans(id,pInput->GetAlarmCallDeactivationSpans());
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
void CIPCInputServerClient::OnRequestSetAlarmOffSpans(CSecID id, 
													  int iNumRecords, 
													  const CSystemTimeSpan data[])
{
	if (theApp.HasAlarmOffSpanFeature())
	{
		CInput* pInput = theApp.GetInputGroups().GetInputByID(id);
		if (pInput)
		{
			pInput->SetAlarmOffSpans(iNumRecords,data,m_sUserName,GetRemoteHost());
			DoConfirmSetAlarmOffSpans(id);
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
void CIPCInputServerClient::OnRequestGetTimeZoneInformation()
{
	CTimeZoneInformations tzis;
	tzis.Load();
	TIME_ZONE_INFORMATION tzi;
	GetTimeZoneInformation(&tzi);

	int i = tzis.FindByStandardName(tzi.StandardName);

	if (i != -1)
	{
		CTimeZoneInformation* pTzi = tzis.GetAtFast(i);
		WK_TRACE(_T("confirm time zone information %s\n"),pTzi->GetDisplayName());
		DoConfirmGetTimeZoneInformation(*pTzi);
	}
	else
	{
		WK_TRACE(_T("time zone information not found\n"));
		DoIndicateError(INP_REQUEST_GET_ZIME_ZONE_INFORMATION,SECID_NO_ID,
			CIPC_ERROR_INVALID_VALUE,0);
	}
}
//////////////////////////////////////////////////////////////////////////////
void CIPCInputServerClient::OnRequestSetTimeZoneInformation(CTimeZoneInformation t)
{
	TIME_ZONE_INFORMATION tzi = t;
	SetTimeZoneInformation(&tzi);
	DoConfirmSetTimeZoneInformation();
}
//////////////////////////////////////////////////////////////////////////////
void CIPCInputServerClient::OnRequestGetTimeZoneInformations()
{
	CTimeZoneInformations tzis;
	tzis.Load();
	DoConfirmGetTimeZoneInformations(tzis);
}
