#include "StdAfx.h"
#include ".\ntservice.h"

///////////////////////////////////////////////////////////////////////////
CNTService::CNTService(const CString& sServiceName, const CString& sDisplayName, const CString& sServiceHelp)
{
	m_sServiceName	= sServiceName;
	m_sDisplayName	= sDisplayName;
	m_sServiceHelp	= sServiceHelp;
}

CNTService::~CNTService(void)
{
}

///////////////////////////////////////////////////////////////////////////
void WINAPI CNTService::ServiceMain(DWORD argc, LPTSTR *argv)
{
    m_pThis->m_ServiceStatus.dwServiceType				= SERVICE_WIN32; 
    m_pThis->m_ServiceStatus.dwCurrentState				= SERVICE_START_PENDING; 
    m_pThis->m_ServiceStatus.dwControlsAccepted			= SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE; 
    m_pThis->m_ServiceStatus.dwWin32ExitCode			= 0; 
    m_pThis->m_ServiceStatus.dwServiceSpecificExitCode	= 0; 
    m_pThis->m_ServiceStatus.dwCheckPoint				= 0; 
    m_pThis->m_ServiceStatus.dwWaitHint					= 0; 
 
	m_pThis->m_ServiceStatusHandle = RegisterServiceCtrlHandler(m_pThis->m_sServiceName, ServiceCtrlHandler);  
    if (m_pThis->m_ServiceStatusHandle)
	{
		m_pThis->m_ServiceStatus.dwCurrentState       = SERVICE_RUNNING; 
		m_pThis->m_ServiceStatus.dwCheckPoint         = 0; 
		m_pThis->m_ServiceStatus.dwWaitHint           = 0;  

		if (SetServiceStatus (m_pThis->m_ServiceStatusHandle, &m_pThis->m_ServiceStatus)) 
		{ 	
			m_pThis->OnRun();
			m_pThis->m_ServiceStatus.dwCurrentState  = SERVICE_STOPPED; 
			SetServiceStatus(m_pThis->m_ServiceStatusHandle,&m_pThis->m_ServiceStatus);
		}
	}

    return; 
}

///////////////////////////////////////////////////////////////////////////
void WINAPI CNTService::ServiceCtrlHandler(DWORD Opcode)
{
    switch(Opcode) 
    { 
        case SERVICE_CONTROL_PAUSE: 
            m_pThis->m_ServiceStatus.dwCurrentState = SERVICE_PAUSE_PENDING; 
			SetServiceStatus(m_pThis->m_ServiceStatusHandle,&m_pThis->m_ServiceStatus);
            
			m_pThis->OnPause();
            
			m_pThis->m_ServiceStatus.dwCurrentState = SERVICE_PAUSED; 
			SetServiceStatus(m_pThis->m_ServiceStatusHandle,&m_pThis->m_ServiceStatus);
			break; 
 
        case SERVICE_CONTROL_CONTINUE: 
            m_pThis->m_ServiceStatus.dwCurrentState = SERVICE_CONTINUE_PENDING; 
 			SetServiceStatus(m_pThis->m_ServiceStatusHandle,&m_pThis->m_ServiceStatus);

			m_pThis->OnContinue();

			m_pThis->m_ServiceStatus.dwCurrentState = SERVICE_RUNNING; 
 			SetServiceStatus(m_pThis->m_ServiceStatusHandle,&m_pThis->m_ServiceStatus);
			break; 
 
        case SERVICE_CONTROL_STOP: 
            m_pThis->m_ServiceStatus.dwWin32ExitCode = 0; 
            m_pThis->m_ServiceStatus.dwCurrentState  = SERVICE_STOP_PENDING; 
            m_pThis->m_ServiceStatus.dwCheckPoint    = 0; 
            m_pThis->m_ServiceStatus.dwWaitHint      = 0; 

			m_pThis->OnStop();
			SetServiceStatus(m_pThis->m_ServiceStatusHandle,&m_pThis->m_ServiceStatus);
 			break;
 
        case SERVICE_CONTROL_INTERROGATE: 
            break; 
    }      
    return; 
}

///////////////////////////////////////////////////////////////////////////
BOOL CNTService::InstallService()
{
	BOOL bResult = FALSE;

	TCHAR strDir[1024];
	SC_HANDLE schSCManager,schService;

	GetCurrentDirectory(1024,strDir);
	
#ifdef _DEBUG	
	_tcscat(strDir, _T("\\Debug\\ControlService.exe")); 
	
#else
	_tcscat(strDir, _T("\\")); 
	_tcscat(strDir, m_sServiceName); 
#endif
	schSCManager = ::OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);  
 
	if (schSCManager) 
	{
		LPCTSTR lpszBinaryPathName=strDir;

		schService = ::CreateService(schSCManager,m_sServiceName, m_sDisplayName,           // service name to display 
			SERVICE_ALL_ACCESS,				// desired access 
			SERVICE_WIN32_OWN_PROCESS |		// service type 
			SERVICE_INTERACTIVE_PROCESS,	//  "
			SERVICE_AUTO_START	,			// start type 
			SERVICE_ERROR_NORMAL,			// error control type 
			lpszBinaryPathName,				// service's binary 
			NULL,							// no load ordering group 
			NULL,							// no tag identifier 
			NULL,							// no dependencies 
			NULL,							// LocalSystem account 
			NULL);							// no password 
	 
		if (schService) 
		{
			SERVICE_DESCRIPTION Desc;
			Desc.lpDescription = m_sServiceHelp.GetBuffer(0);
			ChangeServiceConfig2(schService, SERVICE_CONFIG_DESCRIPTION, (LPVOID)&Desc);
			m_sServiceHelp.ReleaseBuffer();

			bResult = TRUE;
			::CloseServiceHandle(schService); 
		}
		::CloseServiceHandle(schSCManager);
	}

	return bResult;
}

///////////////////////////////////////////////////////////////////////////
BOOL CNTService::DeleteService()
{
	BOOL bResult = FALSE;

	SC_HANDLE schSCManager;
	SC_HANDLE hService;

	schSCManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
 
	if (schSCManager) 
	{	
		hService = ::OpenService(schSCManager, m_sServiceName,SERVICE_ALL_ACCESS);

		if (hService) 
		{
			if (::DeleteService(hService))
				bResult = TRUE;
		
			::CloseServiceHandle(hService);
		}
		::CloseServiceHandle(schSCManager);
	}

	return bResult;
}

///////////////////////////////////////////////////////////////////////////
BOOL CNTService::StartService()
{
	BOOL bResult = FALSE;

	SERVICE_STATUS		m_ssStatus;

	SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (schSCManager)
	{
		SC_HANDLE schService = OpenService(schSCManager, m_sServiceName, SERVICE_ALL_ACCESS);

		if (schService)
		{
			// try to start the service
			TRACE(_T("Starting up %s."), m_sDisplayName);
			if (::StartService(schService, 0, 0))
			{
				Sleep(1000);

				while (::QueryServiceStatus(schService, &m_ssStatus))
				{
					if (m_ssStatus.dwCurrentState == SERVICE_START_PENDING )
					{
						TRACE(_T("."));
						Sleep( 1000 );
					}
					else
						break;
				}

				if( m_ssStatus.dwCurrentState == SERVICE_RUNNING )
					bResult = TRUE;
                else
                    TRACE(_T("\n%s failed to start.\n"), m_sDisplayName);
			}
			else
				TRACE(_T("\n%s failed to start: %d\n"), m_sDisplayName, GetLastError());

			::CloseServiceHandle(schService);
		}
		else
			TRACE(_T("OpenService failed - %d\n"), GetLastError());

        ::CloseServiceHandle(schSCManager);
    }
	else
	{
		TRACE(_T("OpenSCManager failed - %d\n"), GetLastError());
	}

	return bResult;
}

///////////////////////////////////////////////////////////////////////////
BOOL CNTService::EndService()
{
	BOOL bResult = FALSE;

	SERVICE_STATUS		m_ssStatus;

	SC_HANDLE schSCManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (schSCManager)
	{
		SC_HANDLE schService = ::OpenService(schSCManager, m_sServiceName, SERVICE_ALL_ACCESS);

		if (schService)
		{
			// try to stop the service
			if (::ControlService(schService, SERVICE_CONTROL_STOP, &m_ssStatus))
			{
				TRACE(_T("Stopping %s."), m_sDisplayName);
				Sleep(1000);

				while(::QueryServiceStatus(schService, &m_ssStatus))
				{
					if (m_ssStatus.dwCurrentState == SERVICE_STOP_PENDING)
					{
						TRACE(_T("."));
						Sleep(1000);
					}
					else
						break;
				}

				if (m_ssStatus.dwCurrentState == SERVICE_STOPPED)
					bResult = TRUE;
                else
                    TRACE(_T("\n%s failed to stop.\n"), m_sDisplayName);
			}

			::CloseServiceHandle(schService);
		}
		else
			TRACE(_T("OpenService failed - %d\n"), GetLastError());

        ::CloseServiceHandle(schSCManager);
    }
	else
		TRACE(_T("OpenSCManager failed - %d\n"), GetLastError());

	return bResult;
}

///////////////////////////////////////////////////////////////////////////
BOOL CNTService::PauseService()
{
	BOOL bResult = FALSE;

	SERVICE_STATUS		m_ssStatus;

	SC_HANDLE schSCManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (schSCManager)
	{
		SC_HANDLE schService = ::OpenService(schSCManager, m_sServiceName, SERVICE_ALL_ACCESS);

		if (schService)
		{
			// try to stop the service
			if (::ControlService(schService, SERVICE_CONTROL_PAUSE, &m_ssStatus))
			{
				TRACE(_T("Pausing %s."), m_sDisplayName);
				Sleep(1000);

				while(::QueryServiceStatus(schService, &m_ssStatus))
				{
					if (m_ssStatus.dwCurrentState == SERVICE_PAUSE_PENDING)
					{
						TRACE(_T("."));
						Sleep(1000);
					}
					else
						break;
				}

				if (m_ssStatus.dwCurrentState == SERVICE_PAUSED)
					bResult = TRUE;
                else
                    TRACE(_T("\n%s failed to stop.\n"), m_sDisplayName);
			}

			::CloseServiceHandle(schService);
		}
		else
			TRACE(_T("OpenService failed - %d\n"), GetLastError());

        ::CloseServiceHandle(schSCManager);
    }
	else
		TRACE(_T("OpenSCManager failed - %d\n"), GetLastError());

	return bResult;
}

///////////////////////////////////////////////////////////////////////////
BOOL CNTService::ContinueService()
{
	BOOL bResult = FALSE;

	SERVICE_STATUS		m_ssStatus;

	SC_HANDLE schSCManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (schSCManager)
	{
		SC_HANDLE schService = ::OpenService(schSCManager, m_sServiceName, SERVICE_ALL_ACCESS);

		if (schService)
		{
			// try to stop the service
			if (::ControlService(schService, SERVICE_CONTROL_CONTINUE, &m_ssStatus))
			{
				TRACE(_T("Pausing %s."), m_sDisplayName);
				Sleep(1000);

				while(::QueryServiceStatus(schService, &m_ssStatus))
				{
					if (m_ssStatus.dwCurrentState == SERVICE_CONTINUE_PENDING)
					{
						TRACE(_T("."));
						Sleep(1000);
					}
					else
						break;
				}

				if (m_ssStatus.dwCurrentState == SERVICE_RUNNING)
					bResult = TRUE;
                else
                    TRACE(_T("\n%s failed to stop.\n"), m_sDisplayName);
			}

			::CloseServiceHandle(schService);
		}
		else
			TRACE(_T("OpenService failed - %d\n"), GetLastError());

        ::CloseServiceHandle(schSCManager);
    }
	else
		TRACE(_T("OpenSCManager failed - %d\n"), GetLastError());

	return bResult;
}

///////////////////////////////////////////////////////////////////////////
BOOL CNTService::GetStatus(SERVICE_STATUS& Status)
{
	BOOL bResult = FALSE;

	SC_HANDLE schSCManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (schSCManager)
	{
		SC_HANDLE schService = ::OpenService(schSCManager, m_sServiceName, SERVICE_ALL_ACCESS);

		if (schService)
		{
			bResult = ::QueryServiceStatus(schService, &Status);

			::CloseServiceHandle(schService);
		}
		else
			TRACE(_T("OpenService failed - %d\n"), GetLastError());

        ::CloseServiceHandle(schSCManager);
    }
	else
		TRACE(_T("OpenSCManager failed - %d\n"), GetLastError());

	return bResult;
}

///////////////////////////////////////////////////////////////////////////
BOOL CNTService::StartDispatcher()
{
	BOOL bResult = FALSE;
 
	// Default implementation creates a single threaded service.
	// Override this method and provide more table entries for
	// a multithreaded service (one entry for each thread).
	SERVICE_TABLE_ENTRY DispatchTable[]={{m_sServiceName.GetBuffer(0),ServiceMain},{NULL,NULL}};  
	m_sServiceName.ReleaseBuffer();

	bResult = StartServiceCtrlDispatcher(DispatchTable);

	return bResult;
}

///////////////////////////////////////////////////////////////////////////
BOOL CNTService::OnStop()
{
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////
BOOL CNTService::OnPause()
{
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////
BOOL CNTService::OnContinue()
{
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////
BOOL CNTService::OnRun()
{
	return TRUE;
}
