// AudioSOM.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "AudioSOM.h"
#include "AudioSOMDlg.h"


#include "strmif.h"
#include "mmsystem.h"
#include "mtype.h"
#include "common\mfcutil.h"
#include "common\dshowutil.h"

#include "Synth\isynth.h"
#include "Scope\iscope.h"
#include "..\..\..\DirectShow\Filters\Dump\idump.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include <math.h>
// CAudioSOMApp

BEGIN_MESSAGE_MAP(CAudioSOMApp, CWinApp)
   ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CAudioSOMApp construction

CAudioSOMApp::CAudioSOMApp()
{
   EnableHtmlHelp();

   // TODO: add construction code here,
   // Place all significant initialization in InitInstance
}


// The one and only CAudioSOMApp object

CAudioSOMApp theApp;


// CAudioSOMApp initialization
BOOL CAudioSOMApp::InitInstance()
{
   // InitCommonControls() is required on Windows XP if an application
   // manifest specifies use of ComCtl32.dll version 6 or later to enable
   // visual styles.  Otherwise, any window creation will fail.
   InitCommonControls();

   CWinApp::InitInstance();

   AfxEnableControlContainer();

   SetRegistryKey(_T("KEsoft"));
#ifdef _DEBUG
   double dValue[1000];
   int i, n = 1000;
   for (i=0; i<n; i++)
   {
      dValue[i] = sin((i)*0.01);
   }
#endif

   // check filter registered
   InstallFilter(GetString(CLSID_SynthFilter), CString("synth.ax"));

   InstallFilter(GetString(CLSID_Scope), CString("scope.ax"));

   InstallFilter(GetString(CLSID_Dump), CString("wavdest.ax"));
   //InstallFilter(GetString(CLSID_Dump), CString("dump.ax"));

   CAudioSOMDlg dlg;
   m_pMainWnd = &dlg;
   INT_PTR nResponse = dlg.DoModal();
   if (nResponse == IDOK)
   {
   }
   else if (nResponse == IDCANCEL)
   {
   }

   // Since the dialog has been closed, return FALSE so that we exit the
   //  application, rather than start the application's message pump.
   return FALSE;
}

void CAudioSOMApp::InstallFilter(const CString& sID, const CString &sPath)
{
   HKEY hkey;
   LONG lResult = -1;
   DWORD dwType=0, dwSize = 64*sizeof(TCHAR);
   TCHAR szData[64];
   BOOL bInstalled = FALSE;
   CString skey = _T("SOFTWARE\\Classes\\CLSID\\") + sID;
   RegOpenKeyEx(HKEY_LOCAL_MACHINE, skey, 0, KEY_READ, &hkey);
   if (hkey)
   {
      lResult = RegQueryValueEx(hkey, _T(""), NULL, &dwType, (LPBYTE)szData, &dwSize);
      if (lResult == ERROR_SUCCESS)
      {
         bInstalled = TRUE;
      }
      RegCloseKey(hkey);
   }

   if (!bInstalled)
   {
      CStringA sCmd;
      sCmd = "regsvr32 -s ";
      sCmd += sPath;
      system(sCmd);
   }
}

