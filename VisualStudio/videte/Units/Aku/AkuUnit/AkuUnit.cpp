/////////////////////////////////////////////////////////////////////////////
// PROJECT:		AkuUnit
// FILE:		$Workfile: AkuUnit.cpp $
// ARCHIVE:		$Archive: /Project/Units/Aku/AkuUnit/AkuUnit.cpp $
// CHECKIN:		$Date: 27.02.04 9:53 $
// VERSION:		$Revision: 23 $
// LAST CHANGE:	$Modtime: 27.02.04 9:43 $
// BY AUTHOR:	$Author: Georg.feddern $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "wk_names.h"
#include "wk_util.h"
#include "CIpc.h"
#include "AkuUnit.h"
#include "AkuUnitDlg.h"
#include "WK_RuntimeError.h"
#include "WK_Dongle.h"
#include "Ini2Reg.h"
#include "DAAku.h"
#include <io.h>
#include <sys/stat.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAkuUnitApp

BEGIN_MESSAGE_MAP(CAkuUnitApp, CWinApp)
	//{{AFX_MSG_MAP(CAkuUnitApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAkuUnitApp construction

CAkuUnitApp::CAkuUnitApp()
{
	InitPathes();
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAkuUnitApp object

CAkuUnitApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CAkuUnitApp initialization
BOOL CAkuUnitApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
	// Unit nur einmal starten

	if (WK_ALONE(WK_APP_NAME_AKUUNIT)==FALSE)
		return FALSE;

	InitDebugger(_T("AkuUnit.log"), WAI_AKUUNIT);

	// Dongle vorhanden?
	CWK_Dongle dongle(WAI_AKUUNIT); 
	if (dongle.IsExeOkay() == FALSE)
	{
		return FALSE;
	}
	
	WK_STAT_LOG(_T("Reset"), 0, _T("ServerActive"));
	WK_STAT_LOG(_T("Reset"), 1, _T("ServerActive"));

	// Gerätetreiber öffenen
	if (!OpenDevice())
	{
		CString sError;
		sError.LoadString(IDS_RTE_DRIVER_NOT_LOADED);

		CWK_RunTimeError RTerr(WAI_AKUUNIT, REL_ERROR, RTE_OS_EXCEPTION, sError, 0, 0);
		RTerr.Send();
		return FALSE;
	}

	// Sofern eine AkuUnit.ini im Windows-Verzeichnis existiert, die wichtigsten
	// Einstellungen aus der Ini-Datei in die Registry übernehmen.
	CopyIni2Reg(m_sWindowsDirectory);
	
	CAkuUnitDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
int CAkuUnitApp::ExitInstance()
{
	CloseDevice();
	
	WK_STAT_LOG(_T("Reset"), 0, _T("ServerActive"));
	
	CloseDebugger();
	return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CAkuUnitApp::CopyIni2Reg(CString &sPath)
{
	CFileStatus Status;
	DWORD		dwVal	= 0;
	CString		sPathPlusFileNew;
	CString		sPathPlusFileOld;
	CString		sFileNew =_T("AkuUnit.ini");
	CString		sFileOld =_T("OldAkuUnit.ini");
 
	CWK_Profile prof;

	if (sPath.IsEmpty())
		return FALSE;

	sPathPlusFileNew = sPath + _T("\\") + sFileNew;
	sPathPlusFileOld = sPath + _T("\\") + sFileOld;

	if (!CFile::GetStatus(sPathPlusFileNew, Status))
		return FALSE;

	WK_TRACE(_T("Find %s, copy Data to Registry\n"), sPathPlusFileNew);

	// Kopiere die wichtigsten Einstellungen aus der alten INI-Datei in die Registry
	// sofern die INI-Datei vorhanden ist und benenne sie anschließend um.
	// Aku-Basisadresse aus der AkuUnit.ini in Registry übertragen
	dwVal = GetPrivateProfileInt(_T("AKUUNIT"), _T("AkuIOBase0"), 0x00, sFileNew);
	MyWritePrivateProfileInt(_T("System"), _T("AkuIOBase0"), dwVal, _T("AkuUnit"));

	dwVal = GetPrivateProfileInt(_T("AKUUNIT"), _T("AkuIOBase1"), 0x00, sFileNew);
	MyWritePrivateProfileInt(_T("System"), _T("AkuIOBase1"), dwVal, _T("AkuUnit"));

	dwVal = GetPrivateProfileInt(_T("AKUUNIT"), _T("AkuIOBase2"), 0x00, sFileNew);
	MyWritePrivateProfileInt(_T("System"), _T("AkuIOBase2"), dwVal, _T("AkuUnit"));
	
	dwVal = GetPrivateProfileInt(_T("AKUUNIT"), _T("AkuIOBase3"), 0x00, sFileNew);
	MyWritePrivateProfileInt(_T("System"), _T("AkuIOBase3"), dwVal, _T("AkuUnit"));
	
	// Schreibschutz entfernen
	_tchmod(sPathPlusFileNew, _S_IREAD | _S_IWRITE);
	_tchmod(sPathPlusFileOld, _S_IREAD | _S_IWRITE);
	
	// MiCoUnit.ini in MiCoUnitOld.ini umbenennen
	if (_trename(sPathPlusFileNew, sPathPlusFileOld) == 0)
	{
		WK_TRACE(_T("Rename %s to %s\n"), sPathPlusFileNew, sPathPlusFileOld);
	}
	else
	{
		WK_TRACE_WARNING(_T("Can't rename %s to %s.\n"), sPathPlusFileNew, sPathPlusFileOld);
		if (_tremove(sPathPlusFileOld) == 0)
		{
			WK_TRACE(_T("Remove %s\n"), sPathPlusFileOld);
			if (_trename(sPathPlusFileNew, sPathPlusFileOld) == 0)
			{
				WK_TRACE(_T("Rename %s to %s\n"), sPathPlusFileNew, sPathPlusFileOld);
			}
			else
				WK_TRACE_WARNING(_T("Can't rename %s to %s.\n"), sPathPlusFileNew, sPathPlusFileOld);
		}
		else
		{
			WK_TRACE_WARNING(_T("Can't remove %s\n"), sPathPlusFileOld);	
		}
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CAkuUnitApp::InitPathes()
{
	TCHAR szPath[_MAX_PATH];
	CString sPath;

	GetModuleFileName(m_hInstance,szPath,sizeof(szPath));
	sPath = szPath;
	// set important dir informations only once
	int p;

	m_sWorkingDirectory = szPath;
	p = m_sWorkingDirectory.ReverseFind(_T('\\'));
	if (p!=-1)
	{
		m_sWorkingDirectory = m_sWorkingDirectory.Left(p);
	}
	else
	{
		::GetCurrentDirectory(_MAX_PATH,szPath);
		m_sWorkingDirectory = szPath;
	}

	m_sWorkingDirectory.MakeLower();

	::GetWindowsDirectory(szPath,_MAX_PATH);
	m_sWindowsDirectory = szPath;
	
	::GetSystemDirectory(szPath,_MAX_PATH);
	m_sSystemDirectory = szPath;

	::GetCurrentDirectory(_MAX_PATH,szPath);
	m_sCurrentDirectory = szPath;

}
