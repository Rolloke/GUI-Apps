// VideteTweak.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "VideteTweak.h"
#include "VideteTweakDlg.h"
#include "MemoryManegmentDlg.h"
#include "DriveAcceleration.h"
#include "SystemPolicies.h"
#include "LogOnOffSettings.h"
#include "SystemTimeOuts.h"
#include "MiscelaneousDlg.h"
#include "Services.h"
#include ".\videtetweak.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVideteTweakApp

BEGIN_MESSAGE_MAP(CVideteTweakApp, CWinApp)
	//{{AFX_MSG_MAP(CVideteTweakApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVideteTweakApp construction

CVideteTweakApp::CVideteTweakApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CVideteTweakApp object

CVideteTweakApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CVideteTweakApp initialization

BOOL CVideteTweakApp::InitInstance()
{

	CString sDir = m_pszHelpFilePath;
	int nFind = sDir.ReverseFind(_T('\\'));
	if (nFind != -1)
	{
		SetCurrentDirectory(sDir.Left(nFind));
	}
	
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

   char * psz = (char*)m_pszProfileName;
   if (psz)
   {
      char szPath[MAX_PATH];
      ::GetCurrentDirectory(MAX_PATH, szPath);
      int nLen = strlen(szPath);
      if (szPath[nLen-1] != '\\')
      {
         szPath[nLen++] = '\\';
         szPath[nLen++] = 0;
      }
      strcat(szPath, m_pszProfileName);
      free((void*)m_pszProfileName);
      m_pszProfileName = _tcsdup(szPath);
   }

	CPropertySheet      sheet;
	CVideteTweakDlg     vtd;
	CSystemPolicies     syspol;
	CMemoryManegmentDlg mmd;
	CDriveAcceleration  dacc;
	CLogOnOffSettings   lolos;
	CSystemTimeOuts     systo;
	CMiscelaneousDlg    misc;
	CServices           services;
	CAboutDlg           info;
	sheet.AddPage(&vtd);
	sheet.AddPage(&syspol);
	sheet.AddPage(&mmd);
	sheet.AddPage(&dacc);
	sheet.AddPage(&lolos);
	sheet.AddPage(&systo);
	sheet.AddPage(&misc);
	sheet.AddPage(&services);
	sheet.AddPage(&info);
	CString sCaption;
	sCaption.LoadString(IDS_CAPTION);
	sheet.SetTitle(sCaption);

	m_pMainWnd = &sheet;
	sheet.DoModal();
	

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
HKEY CVideteTweakApp::RegOpenKey(HKEY hKey, LPCTSTR sSection, REGSAM rsDesired)
{
	HKEY  hSecKey = NULL;
	LONG  lResult = RegOpenKeyEx(hKey, sSection, 0, rsDesired, &hSecKey);
	if (lResult == ERROR_SUCCESS)
	{
       	return hSecKey;
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
CString CVideteTweakApp::RegReadString(HKEY hKey, LPCTSTR sValue, LPCTSTR sDefault)
{
	DWORD dwCount, dwType;
	LONG  lResult = RegQueryValueEx(hKey, sValue, NULL, &dwType, NULL, &dwCount);
	if (lResult == ERROR_SUCCESS)
	{
		CString sParam;
		lResult = RegQueryValueEx(hKey, sValue, NULL, &dwType, (BYTE*)sParam.GetBufferSetLength(dwCount), &dwCount);
		sParam.ReleaseBuffer(dwCount);
		if (lResult == ERROR_SUCCESS)
		{
			return sParam;
		}
	}
	return sDefault;
}
/////////////////////////////////////////////////////////////////////////////
int CVideteTweakApp::RegReadInt(HKEY hKey, LPCTSTR sValue, int nDefault)
{
	DWORD dwCount, dwType, dwValue;
	LONG  lResult = RegQueryValueEx(hKey, sValue, NULL, &dwType, NULL, &dwCount);
	if (lResult == ERROR_SUCCESS)
	{
		if (dwType == REG_DWORD && dwCount == sizeof(DWORD))
		{
			lResult = RegQueryValueEx(hKey, sValue, NULL, &dwType, (LPBYTE)&dwValue, &dwCount);
			if (lResult == ERROR_SUCCESS)
			{
				return (int)dwValue;
			}
		}
		else if (dwType == REG_SZ)
		{
			CString sParam;
			lResult = RegQueryValueEx(hKey, sValue, NULL, &dwType, (LPBYTE)sParam.GetBufferSetLength(dwCount), &dwCount);
			sParam.ReleaseBuffer(dwCount);
			if (lResult == ERROR_SUCCESS)
			{
				return atoi(sParam);
			}
		}
	}
	return nDefault;
}

BOOL CVideteTweakApp::RegWriteString(HKEY hKey, LPCTSTR sName, LPCTSTR sValue)
{
	LONG  lResult = RegSetValueEx(hKey, sName, NULL, REG_SZ, (BYTE*)sValue, _tcslen(sValue)+1);
	return (lResult == ERROR_SUCCESS);
}

BOOL CVideteTweakApp::RegWriteInt(HKEY hKey, LPCTSTR sName, int nValue)
{
	DWORD dwValue = nValue, dwCount = sizeof(DWORD);
	LONG lResult = RegSetValueEx(hKey, sName, NULL, REG_DWORD, (BYTE*)&dwValue, dwCount);
	return (lResult == ERROR_SUCCESS);
}

BOOL CVideteTweakApp::RegWriteStringInt(HKEY hKey, LPCTSTR sName, int nValue)
{
	CString str;
	str.Format(_T("%d"), nValue);
    return RegWriteString(hKey, sName, str);
}
