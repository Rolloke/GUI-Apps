// IPCFetch.cpp: implementation of the CIPCFetch class.
//
//////////////////////////////////////////////////////////////////////

#include "stdcipc.h"
#include "WK_Names.h"
#include "IPCFetch.h"

//////////////////////////////////////////////////////////////////////
static TCHAR BASED_CODE szModules[]	= _T("SecurityLauncher\\Modules");
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CIPCFetch::CIPCFetch()
{
	m_bFetching = FALSE;
	m_bCancel = FALSE;
	m_Type = SC_NONE;
	m_pConnection = NULL;
}
//////////////////////////////////////////////////////////////////////
CIPCFetch::~CIPCFetch()
{
	if (m_bFetching)
	{
		Cancel();
		if (m_pConnection)
		{
			m_pConnection->StopThread();
			WK_DELETE(m_pConnection);
		}
	}
}
//////////////////////////////////////////////////////////////////////
CIPCFetchResult CIPCFetch::InternalFetch(const CConnectionRecord& c,
										  const CString& sModulName,
										  const CString& sUnitName)
{
	CIPCFetchResult	result;
	if (WK_IS_RUNNING(sModulName)) 
	{
		m_pConnection = new CIPCServerControlClientSide(m_Type, sUnitName, c);
		m_pConnection->SetConnectRetryTime(20);
		m_pConnection->StartThread();

		HANDLE hHandles[2];

		hHandles[0] = m_evCancel;
		hHandles[1] = m_pConnection->ResultEvent();

		DWORD r = WaitForMultipleObjects(2,hHandles,FALSE,c.GetTimeout());
		
		if (r==WAIT_TIMEOUT)
		{
			result = CIPCFetchResult(_T(""),
									CIPC_ERROR_TIMEOUT,
									0,
									_T("Timeout waiting for fetch result"),
									0,
									SECID_NO_ID
#ifdef _UNICODE
									, (WORD)CWK_String::GetCodePage()
#endif
									);
			TRACE(_T("timeout waiting for fetch result"));
			m_pConnection->DelayedDelete();
		}
		else if (r==WAIT_FAILED)
		{
			WK_TRACE(_T("fetch wait failed\n"));
			result = CIPCFetchResult(_T(""),
									CIPC_ERROR_TIMEOUT,
									0,
									_T("fetch wait failed"),
									0,
									SECID_NO_ID
#ifdef _UNICODE
									, (WORD)CWK_String::GetCodePage()
#endif
									);
		}
		else if (r==WAIT_OBJECT_0)
		{
			m_pConnection->Cancel();
			// cancelled
			result = CIPCFetchResult(_T(""),
									CIPC_ERROR_CANCELLED,
									0,
									_T("Connection cancelled"),
									0,
									SECID_NO_ID
#ifdef _UNICODE
									, (WORD)CWK_String::GetCodePage()
#endif
									);
		}
		else if (r==WAIT_OBJECT_0+1)
		{
			// success
			result = m_pConnection->GetResult();
		}

		m_pConnection->StopThread();
		WK_DELETE(m_pConnection);
	} 
	else 
	{
		CIPCServerControlClientSide::ModuleNotRunning(result,sModulName);
	}

	return result;
}
//////////////////////////////////////////////////////////////////////
CIPCFetchResult CIPCFetch::DoFetch(const CConnectionRecord& c)
{

	if (m_bFetching)
	{
		return CIPCFetchResult(_T(""),
							   CIPC_ERROR_BUSY,
							   0,
							   _T(""),
							   0,
							   SECID_NO_ID
#ifdef _UNICODE
							   , (WORD)CWK_String::GetCodePage()
#endif
							   );
	}

	m_bCancel = FALSE;
	m_bFetching = TRUE;

	CConnectionRecord cr(c);
	CString sUnitName, sModulName;
	CIPCFetchResult result;
	
	CIPCServerControlClientSide::CalcRemoteNames(cr.GetDestinationHost(),
												 sUnitName, sModulName, 
												 m_Type);

	if (   GetDebugger()
		&& GetDebugger()->m_ApplicationID != WAI_ISDN_UNIT
		&& GetDebugger()->m_ApplicationID != WAI_SOCKET_UNIT
		&& GetDebugger()->m_ApplicationID != WAI_PRESENCE_UNIT
		&& GetDebugger()->m_ApplicationID != WAI_TOBS_UNIT)
	{
		CString sVersion;
		CString sExe;
		GetModuleFileName(AfxGetApp()->m_hInstance,sExe.GetBufferSetLength(_MAX_PATH),_MAX_PATH);
		sExe.ReleaseBuffer();
		sVersion = WK_GetFileVersion(sExe);
		cr.SetFieldValue(CRF_VERSION, sVersion);
	}
#ifdef _UNICODE
	CString sCP;
	sCP.Format(_T("%d"), CODEPAGE_UNICODE);
	cr.SetFieldValue(CRF_CODEPAGE, sCP);
#endif		

	result = InternalFetch(cr,sModulName,sUnitName);

	// special to try a second link unit (PTUnit/ISDNUnit)
	if (   !m_bCancel
		&& result.GetError()==CIPC_ERROR_UNABLE_TO_CONNECT
		&& result.GetErrorCode()==0	// OOPS, this is ISDN busy
		&& (sModulName==WK_APPNAME_PRESUNIT || sModulName==WK_APP_NAME_ISDN)
		) 
	{
		if (TrySecondUnit(sModulName))
		{
			if (sModulName==WK_APP_NAME_PTUNIT) 
			{
				sModulName = WK_APP_NAME_PTUNIT2;
				sUnitName = WK_SMServerControlPT2;
			} 
			else 
			{
				sModulName = WK_APP_NAME_ISDN2;
				sUnitName = WK_SMServerControlIsdn2;
			}
			result = InternalFetch(cr,sModulName,sUnitName);
		}
	}	// end of special for second linkUnit

	m_bFetching = FALSE;

	return result;
}
//////////////////////////////////////////////////////////////////////
CIPCFetchResult CIPCFetch::FetchInput(const CConnectionRecord &c)
{
	m_Type = SC_INPUT;
	return DoFetch(c);
}
//////////////////////////////////////////////////////////////////////
CIPCFetchResult CIPCFetch::FetchOutput(const CConnectionRecord &c)
{
	m_Type = SC_OUTPUT;
	return DoFetch(c);
}
//////////////////////////////////////////////////////////////////////
CIPCFetchResult CIPCFetch::FetchDatabase(const CConnectionRecord &c)
{
	m_Type = SC_DATABASE;
	return DoFetch(c);
}
//////////////////////////////////////////////////////////////////////
BOOL CIPCFetch::FetchServerReset(const CConnectionRecord &c)
{
	m_Type = SC_RESET_SERVER;
	CIPCFetchResult fr = DoFetch(c);
	return fr.IsOkay();
}
//////////////////////////////////////////////////////////////////////
CIPCFetchResult CIPCFetch::FetchAlarmConnection(const CConnectionRecord &c)
{
	m_Type = SC_CLIENT;
	return DoFetch(c);
}
//////////////////////////////////////////////////////////////////////
CIPCFetchResult CIPCFetch::FetchDataCarrier(const CConnectionRecord &c)
{
	m_Type = SC_DATA_CARRIER;
	return DoFetch(c);
}
//////////////////////////////////////////////////////////////////////
CIPCFetchResult CIPCFetch::FetchAudio(const CConnectionRecord &c)
{
	m_Type = SC_AUDIO;
	return DoFetch(c);
}
//////////////////////////////////////////////////////////////////////
BOOL CIPCFetch::FetchUpdate(int iDestination,
							 const CString &sFileName,
							 const void *pData,
							 DWORD dwDataLen,
							 BOOL bNeedsReboot)
{
	BOOL bOkay = FALSE;
	m_bCancel = FALSE;

	if (WK_IS_RUNNING(WK_APP_NAME_LAUNCHER)) 
	{
		m_pConnection = new CIPCServerControlClientSide(SM_LAUNCHER,
														iDestination,
														sFileName,
														pData,
														dwDataLen,
														bNeedsReboot);
		DWORD dwTimeout = 15*1000;
		m_pConnection->SetConnectTimeout(dwTimeout);
		m_pConnection->SetConnectRetryTime(100);
		m_pConnection->StartThread();

		HANDLE hHandles[2];

		hHandles[0] = m_evCancel;
		hHandles[1] = m_pConnection->ResultEvent();

		DWORD r = WaitForMultipleObjects(2,hHandles,FALSE,dwTimeout);
		
		if (r==WAIT_TIMEOUT)
		{
			bOkay = FALSE;
		}
		else if (r==WAIT_FAILED)
		{
			WK_TRACE(_T("fetch wait failed\n"));
			bOkay = FALSE;
		}
		else if (r==WAIT_OBJECT_0)
		{
			m_pConnection->Cancel();
			// cancelled
			bOkay = FALSE;
		}
		else if (r==WAIT_OBJECT_0+1)
		{
			// success
			bOkay = TRUE;
		}
		m_pConnection->StopThread();
		WK_DELETE(m_pConnection);
	} 
	else 
	{
		WK_TRACE(_T("Launcher not running!\n"));
	}

	return bOkay;
}
//////////////////////////////////////////////////////////////////////
void CIPCFetch::Cancel()
{
	m_bCancel = TRUE;
	m_evCancel.SetEvent();
}
//////////////////////////////////////////////////////////////////////
BOOL CIPCFetch::TrySecondUnit(const CString& sModulName)
{
	BOOL bReturn = FALSE;
	// Is second unit allowed as module
	BOOL bSecondUnitAllowed = FALSE;
	CWK_Profile wkp;
	CString sSecondUnit;
	if (sModulName==WK_APP_NAME_PTUNIT) 
	{
		sSecondUnit = wkp.GetString(szModules,WK_APP_NAME_PTUNIT2,_T(""));
		bSecondUnitAllowed = !sSecondUnit.IsEmpty();
	} 
	else if (sModulName==WK_APP_NAME_ISDN) 
	{
		sSecondUnit = wkp.GetString(szModules,WK_APP_NAME_ISDN2,_T(""));
		bSecondUnitAllowed = !sSecondUnit.IsEmpty();
	}
	else
	{
		// not allowed anyway
	}

	// is second unit allowed for this fetch type
	if (bSecondUnitAllowed)
	{
		if (   m_Type == SC_INPUT
			|| m_Type == SC_OUTPUT
			|| m_Type == SC_AUDIO
			|| m_Type == SC_CLIENT
			|| m_Type == SC_DATABASE
			)
		{
			bReturn = TRUE;
		}
		else
		{
			// not allowed for this type
		}
	}
	return bReturn;
}
