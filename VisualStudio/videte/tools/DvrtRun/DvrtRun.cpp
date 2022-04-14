// LauncherShutdown.cpp : Defines the entry point for the application.
//

#include "stdafx.h"


static HWND g_hWnd = NULL;
static HANDLE g_hProcess = NULL;
DWORD (WINAPI *lpfGetModuleFileNameEx)( HANDLE, HMODULE, LPTSTR, DWORD ) = NULL;

static BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
	TCHAR szModuleName[_MAX_PATH];
	ZeroMemory(szModuleName,_MAX_PATH);
	DWORD dwStyle = GetWindowLong(hWnd,GWL_STYLE);

	if ((dwStyle & WS_CHILD) == 0)
	{
		DWORD dwTempPID = 0;
		GetWindowThreadProcessId(hWnd, &dwTempPID);

		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, dwTempPID);
		UINT dwNumBytes = lpfGetModuleFileNameEx(hProcess,NULL,szModuleName,_MAX_PATH);
		if (dwNumBytes)
		{
#ifdef _DEBUG
			OutputDebugString(_T("GetModuleFileNameEx "));
			OutputDebugString(szModuleName);
			OutputDebugString(_T(" "));
			OutputDebugString((LPCTSTR)lParam);
			OutputDebugString(_T("\n"));
#endif
			if (0==strnicmp(szModuleName,(LPCTSTR)lParam,dwNumBytes))
			{
				g_hWnd = hWnd;
			}
		}
		g_hProcess = hProcess;
	}

	return g_hWnd == NULL;
}

BOOL WK_IS_RUNNING(LPCTSTR appName)
{
	TCHAR szEvent[_MAX_PATH];
	strcpy(szEvent,_T("AppMutex"));
	strcat(szEvent,appName);
	HANDLE hEvent= OpenEvent(EVENT_MODIFY_STATE,FALSE,szEvent);
	BOOL bIsRunning = (hEvent!=NULL);
	CloseHandle(hEvent);

	return bIsRunning;}

int APIENTRY WinMain(HINSTANCE /*hInstance*/,
                     HINSTANCE /*hPrevInstance*/,
                     LPSTR     lpCmdLine,
                     int       /*nCmdShow*/)
{
	if (strlen(lpCmdLine)>0)
	{
		HINSTANCE hLib = LoadLibrary(_T("PSAPI.DLL"));
		if (hLib)
		{
			lpfGetModuleFileNameEx =(DWORD (WINAPI *)(HANDLE, HMODULE, LPTSTR, DWORD )) GetProcAddress( hLib, "GetModuleFileNameExA" ) ;
			if (lpfGetModuleFileNameEx)
			{
				EnumWindows((WNDENUMPROC)EnumWindowsProc,(LPARAM)lpCmdLine);
				if (g_hWnd)
				{
					TCHAR szExecutable[_MAX_PATH];
					strncpy(szExecutable,lpCmdLine,_MAX_PATH);
					strlwr(szExecutable);
					if (NULL!= strstr(szExecutable,_T("ewfcontrol")))
					{
						PostMessage(g_hWnd,WM_USER,WM_QUIT,0);
					}
					else
					{
						PostMessage(g_hWnd,WM_QUIT,0,0);
					}
				}
				if (g_hProcess)
				{
					WaitForSingleObject(g_hProcess,2000);
					CloseHandle(g_hProcess);
				}
			}
			FreeLibrary(hLib);
		}
		Sleep(500);
	}
	else
	{
		HWND hWnd = FindWindow(NULL,_T("Adaptec DirectCD Wizard"));
		if (hWnd && IsWindow(hWnd))
		{
			SendMessage(hWnd,WM_COMMAND,0xE141,0);
		}
		BOOL bIsRunning = FALSE;
		TCHAR sTitle[1024];
		TCHAR sBuffer[256];

		ZeroMemory(sBuffer, sizeof(sBuffer));
		ZeroMemory(sTitle, sizeof(sTitle));
		if (WK_IS_RUNNING(WK_APP_NAME_LAUNCHER))
		{
			bIsRunning = TRUE;
			HWND hWnd = FindWindow(_T("SecurityLauncher"), NULL);
			if (hWnd != NULL)
			{
				GetWindowText(hWnd, sBuffer, sizeof(sBuffer));
				strcat(sTitle, sBuffer);
			}
		}
		else if (WK_IS_RUNNING(WK_APP_NAME_DVSTARTER))
		{
			bIsRunning = TRUE;
		}

		TCHAR szIni[5];

		// just flush the ini file
		WritePrivateProfileString(_T("Setup"), _T("Launcher"), itoa(bIsRunning,szIni,10), _T("dvrtrun.ini"));
		WritePrivateProfileString(_T("Setup"), _T("Title"), sBuffer, _T("dvrtrun.ini"));
		WritePrivateProfileString(NULL, NULL, NULL, _T("dvrtrun.ini"));
	}
	return 0;
}



