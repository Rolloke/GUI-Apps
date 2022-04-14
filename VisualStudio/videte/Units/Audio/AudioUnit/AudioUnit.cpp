/////////////////////////////////////////////////////////////////////////////
// PROJECT:		AudioUnit
// FILE:		$Workfile: AudioUnit.cpp $
// ARCHIVE:		$Archive: /Project/Units/Audio/AudioUnit/AudioUnit.cpp $
// CHECKIN:		$Date: 31.08.06 9:12 $
// VERSION:		$Revision: 42 $
// LAST CHANGE:	$Modtime: 31.08.06 9:09 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//
// IMPORTANT:
// To compile this Project please regard the dependencies:
// windows: Wininet.lib, strmiids.lib, comctl32.lib, Winmm.lib, ole32.lib, oleaut32.lib, uuid.lib
// videte: CIPC.lib, OemGui.lib, WKClasses.lib, vimage.lib, DeviceDetect.lib
// DirectShow dependencies: Compile the following projects in the given order
// - Filter/baseclasses: Strmbasd.lib
// - Common: CommonDirectShow.lib
// Filters have to be created and registered
// - Filters/PushSorce: pushsource.ax
// - Filters/Dump: dump.ax
// - Filters/Wavesplitter: wavesplitter.ax
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AudioUnit.h"
#include "AudioUnitDlg.h"

#include "common\dshowutil.h"

#include "oemgui\oemguiapi.h"

#include "filters\PushSource\iPushsource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define BASE_DVRT                _T("software\\dvrt\\")
#define PUSH_SOURCE_REGISTERED   2
#define REGKEY_AUDIOUNIT_DEFINED 4
#define REGKEY_BASEDVRT_DEFINED  8

/////////////////////////////////////////////////////////////////////////////
// CAudioUnitApp

BEGIN_MESSAGE_MAP(CAudioUnitApp, CWinApp)
	//{{AFX_MSG_MAP(CAudioUnitApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAudioUnitApp construction

CAudioUnitApp::CAudioUnitApp()
{
	InitVideteLocaleLanguage();
	m_pAudioUnitDlg = NULL;
	m_AppID         = WAI_INVALID;
	m_nInstance     = -1;
	m_bAudioDebug   = 0;
	m_dwReadOnly = 0;
	m_dwThreadID = GetCurrentThreadId();
}

CAudioUnitApp::~CAudioUnitApp()
{
//	WK_DELETE(m_pAudioUnitDlg);
	m_pAudioUnitDlg = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAudioUnitApp object

CAudioUnitApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CAudioUnitApp initialization
BOOL CAudioUnitApp::InitInstance()
{
	CString sCmdLine(m_lpCmdLine);
	if (sCmdLine.Find(_T("/?")) != -1)
	{
		CString str = 
		_T("CmdLine parameters are:\n\n")
		_T("/r Register shell media file types (wav, avi, wma)\n")
		_T("    to register more use \"audiounit.ini\" file:\n")
        _T("    [EXT]\n")
        _T("    \"1\"=\".au\"\n")
        _T("    \"2\"=\".aif\"\n\n")
		_T("/u Unregister shell media file types\n\n")
        _T("[path\\]filename.ext to replay valid media file\n\n")
        _T("/? this help as message box");

		AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}
	if      (!WK_IS_RUNNING(GetAppnameFromId(WAI_AUDIOUNIT_1)))
	{
		m_nInstance = 1;
		m_AppID = WAI_AUDIOUNIT_1;
	}
	else if (!WK_IS_RUNNING(GetAppnameFromId(WAI_AUDIOUNIT_2)))
	{
		m_nInstance = 2;
		m_AppID = WAI_AUDIOUNIT_2;
	}
	else if (!WK_IS_RUNNING(GetAppnameFromId(WAI_AUDIOUNIT_3)))
	{
		m_nInstance = 3;
		m_AppID = WAI_AUDIOUNIT_3;
	}
	else if (!WK_IS_RUNNING(GetAppnameFromId(WAI_AUDIOUNIT_4)))
	{
		m_nInstance = 4;
		m_AppID = WAI_AUDIOUNIT_4;
	}
	else
	{
		return FALSE;
	}

#if _MFC_VER >= 0x0710
	SetLanguageParameters(0, 0);
#else
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif	// _AFXDLL
#endif	// _MFC_VER >= 0x0710

	if (WK_IS_RUNNING(WK_DB_SERVER_READ_ONLY))
	{
		m_dwReadOnly = 1;
		CWK_Profile::SetReadOnlyMode(TRUE);

		CWK_Profile wkpAbs(CWK_Profile::WKP_ABSOLUT, HKEY_LOCAL_MACHINE, _T(""), _T(""));
		CString str = _T("SOFTWARE\\Classes\\CLSID\\") + GetString(CLSID_PushSource);
		str = wkpAbs.GetString(str, _T(""), NULL);
		if (str == _T("PushSource Filter"))
		{
			m_dwReadOnly |= PUSH_SOURCE_REGISTERED;
		}
		else
		{
			CString sPath;
			GetModuleFileName(NULL, sPath.GetBuffer(MAX_PATH), MAX_PATH);
			sPath.ReleaseBuffer();

			int nFind = sPath.ReverseFind(_T('\\'));
			if (nFind != -1)
			{
				sPath = sPath.Left(nFind);
			}
			ExecuteProgram(_T("regsvr32.exe /s pushsource.ax"), sPath);
 		}
	}

	CString sAppName = GetAppnameFromId(m_AppID);
	WK_ALONE(sAppName);
	CWK_RunTimeError::SetDefaultID(m_AppID);

	// Logfilename erzeugen
	CString sLogFile = sAppName;

	if (WK_IS_RUNNING(WK_DB_SERVER_READ_ONLY))
	{
		sLogFile = sLogFile+_T("RO.log");
	}
	else
	{
		sLogFile = sLogFile+_T(".log");
	}
	
	// Debugger öffnen
	InitDebugger(sLogFile, m_AppID, NULL);

	WK_TRACE(_T("Starting AudioUnit...\n")); 
	WK_PRINT_VERSION(7, 4, 16, 31, _T("$Date: 31.08.06 9:12 $"), // @AUTO_UPDATE
		0);	// DO NOT TOUCH THIS LINE

#ifdef _USE_AS_MEDIAPLAYER
	if (m_lpCmdLine && m_lpCmdLine[0])
	{
		m_sParam = m_lpCmdLine;
		m_sParam.MakeLower();
		BOOL bRegister = FALSE;
		BOOL bUnregister = FALSE;
		if (m_sParam.Find(_T("/r")) != -1)
		{
			bRegister = TRUE;
			WK_TRACE(_T("RegisterShellFileTypes\n"));
		}
		else if (m_sParam.Find(_T("/u")) != -1)
		{
			bUnregister = TRUE;
			WK_TRACE(_T("UnregisterShellFileTypes\n"));
		}

		if (bRegister || bUnregister)
		{
			CString strFileTypeId, strDDESection, sExt, sTemp;
			CString sIniFile(m_pszHelpFilePath);
			int i;
			sIniFile.MakeLower();
			sIniFile.Replace(_T(".hlp"), _T(".ini"));
			CWK_Profile wkpClassRoot(CWK_Profile::WKP_ABSOLUT, HKEY_CLASSES_ROOT, _T(""), _T(""));
			CWK_Profile wkpFileExt(CWK_Profile::WKP_INIFILE_ABSOLUT, NULL, _T(""), sIniFile);

			m_pDocManager = new CDocManager();
			CSingleDocTemplate *pDT = new CSingleDocTemplate(IDS_FT_MP3, RUNTIME_CLASS(CDocument), RUNTIME_CLASS(CFrameWnd), RUNTIME_CLASS(CView));
			m_pDocManager->AddDocTemplate(pDT);
			CStringArray ar;
			ar.Add(_T(".wav"));
			ar.Add(_T(".avi"));
			ar.Add(_T(".wma"));
			for (i=1; ; i++)
			{
				sExt = wkpFileExt.GetString(_T("Ext"), i, NULL);
				if (sExt.IsEmpty())
				{
					break;
				}
				if (FindString(ar, sExt, FALSE, TRUE) == -1)
				{
					ar.Add(sExt);
				}
			}

			if (bRegister)
			{
				RegisterShellFileTypes();
				pDT->GetDocString(strFileTypeId, CDocTemplate::regFileTypeId);
				strDDESection = strFileTypeId + _T("\\shell\\open\\ddeexec");
				wkpClassRoot.DeleteSection(strDDESection);

				for (i=0; i<ar.GetCount(); i++)
				{
					sExt = ar.GetAt(i);
					if (sExt.IsEmpty())
					{
						break;
					}
					sTemp = wkpClassRoot.GetString(sExt, _T(""), NULL);
					if (!sTemp.IsEmpty() && wkpClassRoot.GetString(sExt, _T("OldType"), NULL).IsEmpty())
					{
						wkpClassRoot.WriteString(sExt, _T("OldType"), sTemp);
					}
					wkpClassRoot.WriteString(sExt, _T(""), strFileTypeId);
				}
			}
			if (bUnregister)
			{
				CString sExt, sTemp;
				UnregisterShellFileTypes();
				for (i=0; i<ar.GetCount(); i++)
				{
					sExt = ar.GetAt(i);
					if (sExt.IsEmpty())
					{
						break;
					}
					sTemp = wkpClassRoot.GetString(sExt, _T("OldType"), NULL);
					if (!sTemp.IsEmpty())
					{
						wkpClassRoot.WriteString(sExt, _T(""), sTemp);
					}
				}
			}

			WK_DELETE(m_pDocManager);
			return FALSE;
		}

//		m_sParam = m_lpCmdLine;
		m_sParam.TrimLeft();
		m_sParam.TrimRight();
		if (m_sParam.GetAt(0) == _T('\"'))
		{
			m_sParam = m_sParam.Mid(1);
		}
		if (m_sParam.GetAt(m_sParam.GetLength()-1) == _T('\"'))
		{
			m_sParam.SetAt(m_sParam.GetLength()-1, 0);
		}
		WK_TRACE(_T("%s\n"), m_sParam);
		if (GetFileAttributes(m_sParam) == 0xffffffff)
		{
			m_sParam.Empty();
			WK_TRACE(_T("Failed\n"));
		}
	}
#endif // _USE_AS_MEDIAPLAYER

	free((void*)m_pszAppName);
	m_pszAppName = _tcsdup(COemGuiApi::GetApplicationName(m_AppID));
	AFX_MODULE_STATE*pState = AfxGetModuleState();
	if (pState)
	{
		if (pState->m_lpszCurrentAppName)
		{
			pState->m_lpszCurrentAppName = m_pszAppName;
		}
	}

	m_pAudioUnitDlg = new CAudioUnitDlg();
	if (m_pAudioUnitDlg && m_pAudioUnitDlg->Create(IDD_AUDIOUNIT_DIALOG))
	{
		m_pMainWnd = m_pAudioUnitDlg;
		return TRUE;
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
int CAudioUnitApp::ExitInstance() 
{
	WK_TRACE(_T("ExitInstance\n"));
	CloseDebugger();
	
	if (m_dwReadOnly)
	{
		if (!(m_dwReadOnly&PUSH_SOURCE_REGISTERED))
		{
			CString sPath;
			GetModuleFileName(NULL, sPath.GetBufferSetLength(MAX_PATH), MAX_PATH);
			sPath.ReleaseBuffer();
			int nFind = sPath.ReverseFind(_T('\\'));
			if (nFind != -1)
			{
				sPath = sPath.Left(nFind);
			}
			ExecuteProgram(_T("regsvr32.exe /u /s pushsource.ax"), sPath);
		}
/*
		CWK_Profile wkpAbs(CWK_Profile::WKP_ABSOLUT, HKEY_LOCAL_MACHINE, _T(""), _T(""));
		CString str = BASE_DVRT;
		if (!(m_dwReadOnly&REGKEY_BASEDVRT_DEFINED))
		{
			wkpAbs.DeleteSection(str);
		}
		else if (!(m_dwReadOnly&REGKEY_AUDIOUNIT_DEFINED))
		{
			str += AUDIO_UNIT;
			wkpAbs.DeleteSection(str);
		}
*/
	}
	return CWinApp::ExitInstance();
}
/////////////////////////////////////////////////////////////////////////////
int CAudioUnitApp::ReadAndInitIntKey(CWK_Profile &wkp, LPCTSTR sSection, LPCTSTR sKey, int nDefault)
{
	int nValue = wkp.GetInt(sSection, sKey, -1);
	if (nValue == -1)
	{
		nValue = nDefault;
		if (!m_dwReadOnly)
		{
			wkp.WriteInt(sSection, sKey, nValue);
		}
	}
	
	WK_TRACE(_T("%s\\%s\\%s:%d\n"), wkp.GetSection(), sSection, sKey, nValue);
	return nValue;
}
#if _MFC_VER >= 0x0710
int CAudioUnitApp::SetLanguageParameters(UINT uCodePage, DWORD dwCPbits)
{
	// TODO! RKE: extract resource for multilang
	// m_hLangResourceDLL may be initialized in CWinApp::InitInstance()
	// Do not call InitInstance() of the base class
//	CLoadResourceDll ResourceDll;
//	m_hLangResourceDLL = ResourceDll.DetachResourceDllHandle();
//	if (m_hLangResourceDLL == NULL)
//	{
//		WK_TRACE(_T("Did not find any ResourceDLL\n"));
//		ASSERT(FALSE);
//		return FALSE;
//	}
//	AfxSetResourceHandle(m_hLangResourceDLL);
	
	if (uCodePage == 0)
	{
		uCodePage = _ttoi(COemGuiApi::GetCodePage());
	}
	else
	{
		COemGuiApi::ChangeLanguageResource();
		InitVideteLocaleLanguage();
	}

//	Set the CodePage for MBCS conversion, if necessary
	CWK_String::SetCodePage(uCodePage);
	CWK_Profile wkp;
	wkp.SetCodePage(CWK_String::GetCodePage());

	if (dwCPbits == 0)
	{
		dwCPbits = COemGuiApi::GetCodePageBits();
	}
	if (dwCPbits)
	{
		SetFontFaceNamesFromCodePageBits(dwCPbits);
	}
	else
	{
		CSkinDialog::DoUseGlobalFonts(FALSE);
	}
	return 0;
}
#endif