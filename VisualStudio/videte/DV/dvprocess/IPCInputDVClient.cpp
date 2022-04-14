// IPCInputDVClient.cpp: implementation of the CIPCInputDVClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DVProcess.h"
#include "IPCInputDVClient.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPCInputDVClient::CIPCInputDVClient(CInputClient* pClient, LPCTSTR shmName)
	: CIPCInputFileUpdate(pClient->GetConnectionRecord(),shmName, TRUE)
{
	m_pClient = pClient;
	m_dwTime = GetTickCount();
	StartThread();
}
//////////////////////////////////////////////////////////////////////
CIPCInputDVClient::~CIPCInputDVClient()
{
	StopThread();
}
//////////////////////////////////////////////////////////////////////
BOOL CIPCInputDVClient::IsTimedOut()
{
	if (GetIPCState() == CIPC_STATE_WRITE_CREATED)
	{
		if (GetTimeSpan(m_dwTime)>30*1000)
		{
			return TRUE;
		}
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
void CIPCInputDVClient::OnRequestDisconnect()
{
	DelayedDelete();
	theApp.GetInputClients().ClientDisconnected(m_pClient->GetID());
	CWnd* pWnd = AfxGetMainWnd();
	if (WK_IS_WINDOW(pWnd))
	{
		pWnd->PostMessage(WM_USER);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCInputDVClient::OnRequestInfoInputs(WORD wGroupID)
{
	if (theApp.IsResetting())
	{
		DoIndicateError(GetLastCmd(), SECID_NO_ID, CIPC_ERROR_ACCESS_DENIED, 3,	NULL);
		return;
	}
	CInputList& inputList = theApp.GetInputGroups();

	inputList.Lock();

	int iNumRecords = 0;
	int iNumGroups = inputList.GetSize();
	int g;
	
	for (g=0;g<iNumGroups;g++) 
	{
		iNumRecords += inputList.GetAtFast(g)->GetSize();
	}

	if (iNumRecords) 
	{
		CIPCInputRecord *records = new CIPCInputRecord[iNumRecords];
		int r=0; // record counter
		
		for (g=0,r=0;g<iNumGroups;g++) 
		{
			const CInputGroup *oneGroup = inputList.GetAtFast(g);
			DWORD dwOneBit=1;
			
			for (int i=0;i<oneGroup->GetSize();i++, dwOneBit<<1) 
			{
				const CInput *pInput = oneGroup->GetAtFast(i);
				BYTE bFlags = IREC_STATE_ENABLED|IREC_STATE_OKAY;

				if (pInput->GetOutstandingPicts()>0) 
				{
					bFlags |= IREC_STATE_ALARM;
				}
				// special commport flag
				if (pInput->IsPTZ()) 
				{
					bFlags |= IREC_IS_COMM_PORT;
				}
				ASSERT(r<iNumRecords);
				records[r].Set(pInput->GetName(),
							   pInput->GetClientID(),
							   bFlags); 
				r++;
			}
		}
		DoConfirmInfoInputs(wGroupID, iNumGroups, iNumRecords,
								records);
		WK_DELETE_ARRAY(records);
	} 
	else 
	{
		// no records at all
		CIPCInputRecord dummyRecords[1];
		DoConfirmInfoInputs(wGroupID, iNumGroups, 0, dummyRecords);
	}

	inputList.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CIPCInputDVClient::OnRequestGetValue(CSecID id,
										    const CString &sKey,
										    DWORD dwServerData)
{
	if (   (id == SECID_NO_ID)
		|| (0 == sKey.Find(_T("Timer")))
	   )
	{
		theApp.OnRequestGetValue(id,sKey,dwServerData,this);
	}
	else
	{
		CInput* pPTZInput = theApp.GetInputGroups().GetPTZInput();
		if (pPTZInput && pPTZInput->GetClientID() == id) 
		{
			CInputGroup* pGroup = pPTZInput->GetGroup();
			if (pGroup) 
			{
				pGroup->DoRequestGetValue(id, sKey, 0);
			}
			else
			{
				WK_TRACE_ERROR(_T("No group for PTZInput\n"));
			}
		} 
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCInputDVClient::OnRequestSetValue(CSecID id,
										   const CString &sKey,
										   const CString &sValue,
										   DWORD dwServerData)
{
	TRACE(_T("OnRequestSetValue %s,%s\n"),sKey,sValue);
	if (   id == SECID_NO_ID
		&& (0 == sKey.CompareNoCase(CSD_DV_CONFIG))
		)
	{
		m_pClient->SetSupervisor(sValue == CSD_ON);
	}
	else if (   (id == SECID_NO_ID)
		|| (0 == sKey.Find(_T("Timer")))
		)
	{
		theApp.OnRequestSetValue(id,sKey,sValue,dwServerData,this);
	}
}
//////////////////////////////////////////////////////////////////////////////
void CIPCInputDVClient::OnRequestGetSystemTime()
{
	CSystemTime st;
	st.GetLocalTime();
	DoConfirmGetSystemTime(st);
}
//////////////////////////////////////////////////////////////////////////////
void CIPCInputDVClient::OnRequestSetSystemTime(const CSystemTime& st)
{
	CSystemTime my = st;
	WK_TRACE(_T("try setting local time to %s\n"),my.GetDateTime());
	if (SetLocalTime(&my))
	{
		WK_TRACE(_T("setting local time to %s\n"),my.GetDateTime());
		DoConfirmSetSystemTime();
	}
	else
	{
		DoIndicateError(GetLastCmd(),SECID_NO_ID,CIPC_ERROR_INVALID_PERMISSIONS,1);
	}
}
//////////////////////////////////////////////////////////////////////////////
void CIPCInputDVClient::OnRequestResetApplication()
{
	CWnd* pWnd = AfxGetMainWnd();
	if (WK_IS_WINDOW(pWnd))
	{
		pWnd->PostMessage(WM_COMMAND,ID_FILE_RESET);
	}
}
//////////////////////////////////////////////////////////////////////////////
void CIPCInputDVClient::OnRequestGetTimeZoneInformation()
{
	CTimeZoneInformations tzis;
	tzis.Load();
	TIME_ZONE_INFORMATION tzi;
	GetTimeZoneInformation(&tzi);

	//first try to get standard name on english OS
	int i = tzis.FindByStandardName(tzi.StandardName);

	if(i == -1)
	{
		//try to get systemtime on german OS
		//because first try failed
		CString sTimeZone = tzi.StandardName;
		CString sSubKey = SKEY_NT;
		sSubKey += _T("\\") + sTimeZone;
		HKEY hKey = NULL;
		LONG lRetVal = RegOpenKeyEx(HKEY_LOCAL_MACHINE, sSubKey, 0, KEY_ALL_ACCESS, &hKey);
		if (lRetVal == ERROR_SUCCESS)
		{
			DWORD dwType = 0;
			DWORD dwLen  = 0;
			CString sValue = _T("");

			// Normalzeitname einlesen
			LONG lResult = RegQueryValueEx(hKey, _T("Std"), NULL, &dwType, NULL, &dwLen);
			if(lResult == ERROR_SUCCESS)
			{
				LONG lResult = RegQueryValueEx(hKey, _T("Std"), NULL, &dwType, (LPBYTE)sValue.GetBuffer(dwLen),&dwLen);			
				sValue.ReleaseBuffer();
				if (lResult == ERROR_SUCCESS)
				{
					i = tzis.FindByStandardName(sValue);
				}
			}
		}
		RegCloseKey(hKey);
	}

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
void CIPCInputDVClient::OnRequestSetTimeZoneInformation(CTimeZoneInformation t)
{
	TIME_ZONE_INFORMATION tzi = t;
	if (SetTimeZoneInformation(&tzi))
	{
		WK_TRACE(_T("timezone changed\n"));
	}
	else
	{
		WK_TRACE_ERROR(_T("cannot set timezone %s\n"),GetLastErrorString());
	}
	DoConfirmSetTimeZoneInformation();
}
//////////////////////////////////////////////////////////////////////////////
void CIPCInputDVClient::OnRequestGetTimeZoneInformations()
{
	CTimeZoneInformations tzis;
	tzis.Load();
	DoConfirmGetTimeZoneInformations(tzis);
}
//////////////////////////////////////////////////////////////////////////////
void CIPCInputDVClient::OnRequestCameraControl(CSecID commID, 
											   CSecID camID,
											   CameraControlCmd cmd,
											   DWORD dwValue)
{
	CInput* pPTZInput = theApp.GetInputGroups().GetPTZInput();
	if (pPTZInput) 
	{
		CInputGroup* pGroup = pPTZInput->GetGroup();
		if (pGroup) 
		{
			// gf todo CamID umrechnen auf Group
			COutput* pOutput = theApp.GetOutputGroups().GetOutputByClientID(camID);
			if (pOutput)
			{
				pGroup->DoRequestCameraControl(commID, pOutput->GetUnitID(), cmd, dwValue);
			}
			else
			{
				WK_TRACE_ERROR(_T("No output for CameraControl %08x\n"), camID.GetID());
			}
		}
		else
		{
			WK_TRACE_ERROR(_T("No group for PTZInput\n"));
		}
	} 
	else 
	{
		WK_TRACE(_T("CommUnit for %08x not found\n"), commID.GetID());
	}
}
