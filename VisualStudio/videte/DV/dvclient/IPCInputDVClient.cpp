// IPCInputDVClient.cpp: implementation of the CIPCInputDVClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dvclient.h"
#include "IPCInputDVClient.h"

#include "Server.h"
#include "MainFrame.h"
#include "LiveWindow.h"
#include "CPanel.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CIPCInputDVClient::CIPCInputDVClient(LPCTSTR shmName, CServer* pServer)
 : CIPCInputClient(shmName,FALSE)
{
	m_pServer		= pServer;
	m_idComPortInput	= SECID_NO_ID;
#ifdef _UNICODE
	m_wCodePage = pServer->GetInputCodePage();
#endif
	SetConnectRetryTime(50);
	StartThread();
}
//////////////////////////////////////////////////////////////////////
CIPCInputDVClient::~CIPCInputDVClient()
{
	StopThread();
}
//////////////////////////////////////////////////////////////////////
void CIPCInputDVClient::OnConfirmConnection()
{
	WK_TRACE(_T("CIPCInputDVClient::OnConfirmConnection %s\n"),m_pServer->GetHostName());
	CIPCInputClient::OnConfirmConnection();
	CPanel* pPanel = theApp.GetPanel();
	if (pPanel)
	{
		pPanel->NotifyCIPCConnect(SC_INPUT);
	}
	if (m_pServer->GetMajorVersion() == 0)
	{
		// new unicode versions will send version info
		// in fetchresult, so major version is not 0
		DoRequestGetFile(RFU_STRING_INTERFACE,_T("GetFileVersion c:\\dv\\dvprocess.exe"));
	}
	theApp.ForceOnIdle();
}
//////////////////////////////////////////////////////////////////////
void CIPCInputDVClient::OnRequestDisconnect()
{
	WK_TRACE(_T("CIPCInputDVClient::OnRequestDisconnect %s\n"),GetShmName());
	DelayedDelete();
//ML	AfxGetMainWnd()->PostMessage(WM_USER);
	AfxGetMainWnd()->PostMessage(WM_USER,MAKELONG(WPARAM_DISCONNECT,0),0);
}
//////////////////////////////////////////////////////////////////////
void CIPCInputDVClient::OnConfirmInfoInputs(WORD wGroupID, int iNumGroups, int numRecords, const CIPCInputRecord records[])
{
	CIPCInputClient::OnConfirmInfoInputs(wGroupID,iNumGroups,numRecords,records);

	// browse InputRecords for CommPort Input
	for (int i=0 ; i<GetNumberOfInputs() ; i++)
	{
		const CIPCInputRecord& rec = GetInputRecordFromIndex(i);
		if (rec.IsCommPort())
		{
			m_idComPortInput = rec.GetID();
			break;
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCInputDVClient::OnAddRecord(const CIPCInputRecord& rec)
{
	if (rec.GetIsEnabled() 
		&& rec.GetIsOkay())
	{
		CWnd* pWnd = theApp.GetMainWnd();
		if (pWnd)
		{
			pWnd->PostMessage(WM_USER,
							  MAKELONG(WPARAM_ADD_INPUT,0),
							  rec.GetID().GetID());
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCInputDVClient::OnUpdateRecord(const CIPCInputRecord& rec)
{
	CWnd* pWnd = theApp.GetMainWnd();
	if (pWnd)
	{
		pWnd->PostMessage(WM_USER,
						  MAKELONG(WPARAM_UPD_INPUT,0),
						  rec.GetID().GetID());
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCInputDVClient::OnDeleteRecord(const CIPCInputRecord& rec)
{
	CWnd* pWnd = theApp.GetMainWnd();
	if (pWnd)
	{
		pWnd->PostMessage(WM_USER,
						  MAKELONG(WPARAM_DEL_INPUT,0),
						  rec.GetID().GetID());
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCInputDVClient::OnIndicateAlarmNr(CSecID id,
										   BOOL bAlarm,
										   DWORD dwData1,
										   DWORD dwData2)
{
	CIPCInputClient::OnIndicateAlarmNr(id,bAlarm,dwData1,dwData2);
	CMainFrame* pMF = theApp.GetMainFrame();
	if (pMF)
	{

//		WORD wCameraNr = id.GetSubID() & 0x000F;

		// 2. JaCob besitzt die Kameras 17...32 die auf die Kameras 9...15 gemapt werden.
		WORD wCameraNr = (WORD)(id.GetSubID() & 0x001F);

		pMF->GetDisplayWindows()->Lock();
        CLiveWindow* pLW = pMF->GetLiveWindow(wCameraNr);
		if (pLW)
		{
			pLW->IndicateAlarmNr(bAlarm,LOWORD(dwData1),HIWORD(dwData1));
		}
		pMF->GetDisplayWindows()->Unlock();

		// Receiver: Inform MainFrame AFTER LiveWindow!
		pMF->PostMessage(WM_USER, MAKELONG(WPARAM_IND_ALARM,bAlarm), id.GetID());
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CIPCInputDVClient::GetAlarmState(WORD wCameraNr)
{
	BOOL bRet = FALSE;

	// OR all correcponding inputs
	for (int i=0;i<GetNumberOfInputs();i++)
	{
		const CIPCInputRecord& rec = GetInputRecordFromIndex(i);
		WORD wNr = (WORD)(rec.GetID().GetSubID() & 0x001F);
		if (wNr >=15)
		{
			wNr-=8;
		}
		if (wNr == wCameraNr)
		{
			bRet |= rec.GetIsActive();
		}
	}

	return bRet;
}

//////////////////////////////////////////////////////////////////////
void CIPCInputDVClient::OnConfirmGetSystemTime(const CSystemTime& st)
{
	if (m_pServer)
		m_pServer->ConfirmGetSystemTime(st);
}

//////////////////////////////////////////////////////////////////////
void CIPCInputDVClient::OnConfirmSetSystemTime()
{
	if (m_pServer)
		m_pServer->ConfirmSetSystemTime(TRUE);
}

//////////////////////////////////////////////////////////////////////
void CIPCInputDVClient::OnConfirmGetTimeZoneInformations(const CTimeZoneInformations& tzi)
{
	if (m_pServer)
		m_pServer->ConfirmGetTimeZones(tzi);
}

//////////////////////////////////////////////////////////////////////
void CIPCInputDVClient::OnConfirmGetTimeZoneInformation(CTimeZoneInformation TimeZone)
{
	if (m_pServer)
		m_pServer->ConfirmGetTimeZone(TimeZone);
}

//////////////////////////////////////////////////////////////////////
void CIPCInputDVClient::OnConfirmSetTimeZoneInformation()
{
	if (m_pServer)
		m_pServer->ConfirmSetTimeZone();
}
//////////////////////////////////////////////////////////////////////
void CIPCInputDVClient::OnConfirmGetValue(CSecID id,
											const CString &sKey,
											const CString &sValue,
											DWORD dwServerData)
{
	if (m_pServer && (id==SECID_NO_ID))
	{
		m_pServer->ConfirmGetValue(sKey,sValue);
	}
	else if (0==sKey.Find(CSD_CAM_CONTROL_TYPE))
	{
		CameraControlType cct;
		DWORD dwID = 0;
		if (1 == _stscanf(sKey,CSD_CAM_CONTROL_TYPE_FMT,&dwID))
		{
			cct = NameToCameraControlType(sValue);
			CLiveWindow*pLW = theApp.GetMainFrame()->GetLiveWindow(LOWORD(dwID));
			if (pLW)
			{
				pLW->PostMessage(WM_USER, WPARAM_CAMERA_CT, (LPARAM)cct);
			}
		}
	}
	else if (0==sKey.Find(CSD_CAM_CONTROL_FKT))
	{
		DWORD dwID = 0;
		DWORD dwF  = 0;
		if (1 == _stscanf(sKey, CSD_CAM_CONTROL_FKT_FMT,&dwID))
		{
			_stscanf(sValue,_T("%08x"),&dwF);
			if (dwF & PTZF_EXTENDED)
			{
				CString sKey;
				sKey.Format(CSD_CAM_CONTROL_FKTEX_FMT, dwID);
				DoRequestGetValue(GetComPortInputID(), sKey);
			}
			CLiveWindow*pLW = theApp.GetMainFrame()->GetLiveWindow(LOWORD(dwID));
			if (pLW && dwF)
			{
				pLW->PostMessage(WM_USER, WPARAM_CAMERA_CF, dwF);
			}
		}
		if (1 == _stscanf(sKey, CSD_CAM_CONTROL_FKTEX_FMT,&dwID))
		{
			_stscanf(sValue,_T("%08x"), &dwF);
			CLiveWindow*pLW = theApp.GetMainFrame()->GetLiveWindow(LOWORD(dwID));
			if (pLW && dwF)
			{
				pLW->PostMessage(WM_USER,WPARAM_CAMERA_CF_EX,dwF);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputDVClient::OnConfirmSetValue(CSecID id,
											const CString &sKey,
											const CString &sValue,
											DWORD dwServerData)
{
	if (m_pServer && (id==SECID_NO_ID))
		m_pServer->ConfirmSetValue(sKey,sValue);
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputDVClient::OnIndicateError(DWORD dwCmd, 
										 CSecID id, 
										 CIPCError error, 
										 int iErrorCode,
										 const CString &sErrorMessage)
{
	switch (dwCmd)
	{
	case INP_REQUEST_SET_SYSTEM_TIME:
		switch (error)
		{
		case CIPC_ERROR_INVALID_PERMISSIONS:
			switch (iErrorCode)
			{
			case 1:
				// NT Berechtigung zum Setzen der Zeit fehlt
				break;
			case 2:
				// User Berechtigung zum Setzen der Zeit fehlt
				break;
			}
			break;
		}
		// egal was für ein Code, dem User Bescheid sagen
		if (m_pServer)
			m_pServer->ConfirmSetSystemTime(FALSE);
		break;
	}
}
//typedef BOOL (APIENTRY* GET_DISK_FREE_SPACE_EX_PTR)(LPCTSTR,PULARGE_INTEGER,PULARGE_INTEGER,PULARGE_INTEGER);

typedef BOOL (APIENTRY* VER_QUERY_VALUE_A_PTR)(const LPVOID,LPSTR,LPVOID*,PUINT);

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputDVClient::OnConfirmGetFile(int iDestination,
										 const CString &sFileName,
										 const void *pData,
										 DWORD dwDataLen)
{
	if (iDestination == CFU_GET_FILE_VERSION)
	{
		CString sVersion;
		UINT  dwValueLen;
		PBYTE pCopyData;
		void* pValueData;

		pCopyData = new BYTE[dwDataLen];
		CopyMemory(pCopyData,pData,dwDataLen);

		struct LANGANDCODEPAGE 
		{
			WORD wLanguage;
			WORD wCodePage;
		} *lpTranslate;
		UINT cbTranslate = 0;

		// Read the list of languages and code pages.

		VerQueryValue(pCopyData, 
					  TEXT("\\VarFileInfo\\Translation"),
					  (LPVOID*)&lpTranslate,
					  &cbTranslate);

		for (UINT i=0; i < (cbTranslate/sizeof(struct LANGANDCODEPAGE)); i++ )
		{
			CString sLanguage;
			sLanguage.Format(TEXT("\\StringFileInfo\\%04x%04x\\FileVersion"),
							lpTranslate[i].wLanguage,
							lpTranslate[i].wCodePage);
			
			// Retrieve file description for language and code page "i". 
			if (VerQueryValue(pCopyData, 
							  sLanguage.GetBuffer(0), 
							  &pValueData, 
							  &dwValueLen))
			{
				sVersion = (const char*)pValueData;
				sLanguage.ReleaseBuffer();
				break;
			}
			else
			{
				sLanguage.ReleaseBuffer();
			}
		}

		if (sVersion.IsEmpty())
		{
			sVersion = ExtractFileVersion((BYTE*)pData,dwDataLen);
		}

		m_pServer->SetVersion(sVersion);

		WK_DELETE_ARRAY(pCopyData);
	}
}


