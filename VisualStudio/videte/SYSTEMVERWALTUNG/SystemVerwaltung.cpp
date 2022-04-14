// SystemVerwaltung.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "SystemVerwaltung.h"

#include "MainFrm.h"
#include "SVDoc.h"
#include "SVView.h"

#include "wk_msg.h"
#include "oemgui\oemgui.h"

#include <shlwapi.h>
#include <Winsock2.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
CString StringCharsNotInSet(const CString s, const CString& sCharSet)
{
	CString sRet;
	for (int i=0; i<s.GetLength(); i++)
	{
		if (-1 == sCharSet.Find(s[i]))
		{
			sRet += s[i];
		}
	}
	return sRet;
}
/////////////////////////////////////////////////////////////////////////////
int CheckDigits(CString& s)
{
	int r = -1;
	int i,c;
	c = s.GetLength();

	for (i=0;i<c;i++)
	{
		if (!isdigit(s[i]))
		{
			s = s.Left(i);
			r = i;
			break;
		}
	}

	return r;
}
/////////////////////////////////////////////////////////////////////////////
BOOL AddNoDoubleToWordArray(WORD w, CWordArray &wa)
{
	int i, n = wa.GetSize();
	for (i=0;i<n;i++)
	{
		if (wa[i] == w) return FALSE;
	}
	wa.Add(w);
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void RemoveLastCharacter(CString&s, TCHAR c)
{
	if (s.GetLength() && s.GetAt(s.GetLength()-1) == c)
	{
		s = s.Left(s.GetLength()-1);
	}
}
/////////////////////////////////////////////////////////////////////////////
int IsCameraValid(const CString& sCameras, CString& sWrong, int& iStart, DWORD dwMaxCams/*=ULONG_MAX*/)
{
	// Return values:
	//  1 : No. of cameras
	//  0 : No camera found
	// -1 : Invalid characters
	// -2 : Invalid camera
	// -3 : Invalid range
	
	int iReturn = 0;

	iStart = 0;
	// any invalid chars?
	sWrong = StringCharsNotInSet(sCameras, _T("1234567890;- "));
	if (sWrong.GetLength() == 0)
	{
		// any cams?
		if (sCameras.FindOneOf(_T("123456789")) != -1)
		{
			CStringArray saCamsRange, saCamSingle;
			CString sCamsRange, sCamSingleStart, sCamSingleEnd;
			DWORD dwStart = 0;
			DWORD dwEnd = 0;

			SplitString(sCameras, saCamsRange, _T(';'));
			for (int i=0 ; i<saCamsRange.GetSize() ; i++)
			{
				sCamSingleStart = _T("");
				sCamSingleEnd = _T("NONE");
				sCamsRange = saCamsRange.GetAt(i);
				if (sCamsRange.Find('-') != -1)
				{
					SplitString(sCamsRange, saCamSingle, _T('-'));
					int nSizeSingle = saCamSingle.GetSize();
					if (nSizeSingle > 0 &&  (nSizeSingle & 1) == 0)
					{
						sCamSingleStart	= saCamSingle.GetAt(nSizeSingle-2);
						sCamSingleEnd = saCamSingle.GetAt(nSizeSingle-1);
						if (   (sCamSingleStart.IsEmpty())
							|| (sCamSingleEnd.IsEmpty())
							)
						{
							iReturn = -3; // Invalid range
							sWrong = sCamsRange;
							break;
						}
					}
					else
					{
						iReturn = -3; // Invalid range
						sWrong = sCamsRange;
						break;
					}
				}
				else
				{
					sCamSingleStart = sCamsRange;
				}

				// Space between digits?
				if (sCamSingleStart.Find(' ') != -1)
				{
					iReturn = -2; // Invalid camera
					sWrong = sCamSingleStart;
					break;
				}
				else if (sCamSingleEnd.Find(' ') != -1)
				{
					iReturn = -2; // Invalid camera
					sWrong = sCamSingleEnd;
					break;
				}
				else
				{
					_stscanf(sCamSingleStart, _T("%lu"), &dwStart);
					if (   (dwStart == 0)						
						|| (dwStart > dwMaxCams)
						)
					{
						iReturn = -2; // Invalid camera
						sWrong = sCamSingleStart;
						break;
					}
					
					// Range or single camera?
					if (sCamSingleEnd != _T("NONE"))
					{
						_stscanf(sCamSingleEnd, _T("%lu"), &dwEnd);
						if (   (dwEnd == 0)						
							|| (dwEnd > dwMaxCams)
							)
						{
							iReturn = -2; // Invalid camera
							sWrong = sCamSingleEnd;
							break;
						}
						if (dwStart >= dwEnd)
						{
							iReturn = -3; // Invalid range
							sWrong = sCamsRange;
							break;
						}
						iReturn += dwEnd - dwStart +1;
					}
					else
					{
						iReturn++;
					}
				}
				// increase iStart for finding sWrong in sCameras
				iStart += saCamsRange.GetAt(i).GetLength() + 1;
			} // for
		}
		else
		{
			iReturn = 0; // no cam found
			sWrong = sCameras;
		}
	}
	else
	{
		iReturn = -1; // invalid characters
		sWrong = sWrong.Left(1);
	}
	return iReturn;
}
/////////////////////////////////////////////////////////////////////////////
void DDV_Validate_IP_adr(CDataExchange* pDX, CString const& value, UINT nID/*=0*/)
{
	if (pDX->m_bSaveAndValidate && !value.IsEmpty())
	{
		CWK_String sAdr(value);
		DWORD dwIP = inet_addr(sAdr);
		if (dwIP == INADDR_NONE)
		{
			CString prompt, sControl(value);
			if (nID)
			{
				pDX->m_pDlgWnd->GetDlgItemText(nID, sControl);
			}
			prompt.FormatMessage(AFX_IDP_ARCH_BADINDEX, sControl);
			AfxMessageBox(prompt, MB_ICONEXCLAMATION);
			pDX->Fail();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void DDV_MinChars(CDataExchange* pDX, CString const& value, int nMin)
{
	if (pDX->m_bSaveAndValidate && value.GetLength() < nMin)
	{
		AfxMessageBox(IDP_NOEMPTY_FIELDS, MB_ICONEXCLAMATION);
		pDX->Fail();
	}
}
/////////////////////////////////////////////////////////////////////////////
void DDX_Text(CDataExchange* pDX, LPCTSTR sFmt, UINT nErrorID, int nIDC, int& value)
{
	CString sText;
	pDX->m_idLastControl = nIDC;
	if (pDX->m_bSaveAndValidate)
	{
		pDX->m_pDlgWnd->GetDlgItemText(nIDC, sText);
		int nRet = _stscanf(sText, sFmt, &value);
		if (nRet != 1)
		{
			CString sMsg;
			sText.Format(sFmt, value);
			pDX->m_pDlgWnd->SetDlgItemText(nIDC, sText);
			sMsg.LoadString(nErrorID);
			sMsg += _T("\n(") + sText + _T(")");
			AfxMessageBox(sMsg, MB_ICONEXCLAMATION);
			pDX->Fail();
		}
	}
	else
	{
		sText.Format(sFmt, value);
		pDX->m_pDlgWnd->SetDlgItemText(nIDC, sText);
	}
}
/////////////////////////////////////////////////////////////////////////////
// CSVApp

BEGIN_MESSAGE_MAP(CSVApp, CWinApp)
	//{{AFX_MSG_MAP(CSVApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	// Standard print setup command
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSVApp construction

CSVApp::CSVApp()
{
	m_pUser = NULL;
	m_pPermission = NULL;
	m_bRunAnyLinkUnit = FALSE;
	m_pDongle = NULL;
	m_pGetComputerNameEx = NULL;

	m_bUpdateLanguageDependencies = FALSE;
	m_hKernel32 = LoadLibrary(_T("KERNEL32.DLL"));
	if (m_hKernel32)
	{
		m_pGetComputerNameEx = (GETCOMPUTERNAMEEX_FNC)::GetProcAddress(m_hKernel32, GETCOMPUTERNAMEEX);
	}
}
/////////////////////////////////////////////////////////////////////////////
CSVApp::~CSVApp()
{
	if (m_hKernel32)
	{
		FreeLibrary(m_hKernel32);
	}
}
/////////////////////////////////////////////////////////////////////////////
// The one and only CSVApp object

CSVApp theApp;

/////////////////////////////////////////////////////////////////////////////
void CSVApp::RegisterWindowClass()
{
	WNDCLASS  wndclass ;

	// register window class
	wndclass.style =         0;
	wndclass.lpfnWndProc =   DefWindowProc;
	wndclass.cbClsExtra =    0;
	wndclass.cbWndExtra =    0;
	wndclass.hInstance =     m_hInstance;
	wndclass.hIcon =         LoadIcon(MAKEINTRESOURCE(IDR_MAINFRAME));
	wndclass.hCursor =       LoadStandardCursor(IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1) ;
	wndclass.lpszMenuName =  0L;
	wndclass.lpszClassName = WK_APP_NAME_SUPERVISOR;

	AfxRegisterClass(&wndclass);
}
/////////////////////////////////////////////////////////////////////////////
// CSVApp initialization
BOOL CSVApp::InitInstance()
{
	AFX_MODULE_STATE* pState = AfxGetModuleState();
	CWK_Profile wkp;
#if _MFC_VER >= 0x0710
	// m_hLangResourceDLL may be initialized in CWinApp::InitInstance()
	// Do not call InitInstance() of the base class
	CLoadResourceDll ResourceDll;
	m_hLangResourceDLL = ResourceDll.DetachResourceDllHandle();
	if (m_hLangResourceDLL == NULL)
	{
		WK_TRACE(_T("Did not find any ResourceDLL\n"));
		ASSERT(FALSE);
		return FALSE;
	}
	AfxSetResourceHandle(m_hLangResourceDLL);
	// Set the CodePage for MBCS conversion (Registry Import Export)
	CWK_String::SetCodePage(_ttoi(COemGuiApi::GetCodePage()));
	wkp.SetCodePage(CWK_String::GetCodePage());
	DWORD dwCPBits = COemGuiApi::GetCodePageBits();
	if (dwCPBits)
	{
		SetFontFaceNamesFromCodePageBits(dwCPBits);
	}

	m_bProfessional = GetIntValue(PROFESSIONAL_MODE, FALSE);

#endif	// _MFC_VER >= 0x0710

	if ( WK_ALONE(WK_APP_NAME_SUPERVISOR)==FALSE )
	{
		HWND hWndFirstInstance = FindWindow(WK_APP_NAME_SUPERVISOR,NULL);
		if (hWndFirstInstance)
		{
			SetWindowPos(hWndFirstInstance,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
			SetWindowPos(hWndFirstInstance,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
		}
		return FALSE;
	}

	AfxInitRichEdit();

	InitDebugger(_T("SystemVerwaltung.log"),WAI_SUPERVISOR);

	BOOL bTraceMilliSeconds = (BOOL)wkp.GetInt(_T("Debug"), _T("TraceMilliSeconds"), FALSE);
	if (bTraceMilliSeconds)
	{
		SET_WK_STAT_TICK_COUNT(1);
	}
	WK_TRACE_USER(_T("SystemVerwaltung wird gestartet\n"));
	m_nCmdShow = SW_SHOWMAXIMIZED;

	if (dwCPBits)
	{
		CFont *pFont = CSkinDialog::GetFixedGlobalFont();
		LOGFONT lf;
		if (pFont)
		{
			pFont->GetLogFont(&lf);
			WK_TRACE(_T("fixed font :%s\n"), lf.lfFaceName);
		}
		pFont = CSkinDialog::GetVariableGlobalFont();
		if (pFont)
		{
			pFont->GetLogFont(&lf);
			WK_TRACE(_T("variable font :%s\n"), lf.lfFaceName);
		}
	}
	m_pDongle = new CWK_Dongle();
	if (!m_pDongle->IsValid())
	{
		AfxMessageBox(IDP_INVALID_DONGLE);
		return FALSE;
	}

#ifdef _DEBUG
	if (m_pDongle->IsReceiver())
	{
		CWK_Profile wkpSystem(CWK_Profile::WKP_ABSOLUT, HKEY_LOCAL_MACHINE,_T(""),_T(""));
		CString str = wkpSystem.GetString(_T("SYSTEM\\CurrentControlSet\\Control\\Session Manager\\SFC"), _T("ProgramFilesDir"), NULL);
		m_sApplicationPath = str + _T("\\IdipReceiver\\");
	}
	else
	{
		m_sApplicationPath = _T("C:\\Security\\");
	}
#else
	m_sApplicationPath = m_pszHelpFilePath;
	int nFind = m_sApplicationPath.ReverseFind(_T('\\'));
	if (nFind != -1)
	{
		m_sApplicationPath = m_sApplicationPath.Left(nFind+1);
	}
#endif

	CString sTitle;
	sTitle = COemGuiApi::GetApplicationName(WAI_SUPERVISOR);
	if (m_pDongle->IsDemo())
	{
		sTitle += _T(" DEMO");
	}
	if (m_pszAppName)
	{
		free((void*)m_pszAppName);
	}
	m_pszAppName = _tcsdup(sTitle);    // human readable title
	pState->m_lpszCurrentAppName = m_pszAppName;
	m_bRunAnyLinkUnit = m_pDongle->RunAnyLinkUnit();

	RegisterWindowClass();

	// worker thread to check comm ports
	CWinThread* pThread = AfxBeginThread(EvaluateCommPortThreadProc,0);

	BOOL bLoggedIn = FALSE;
	CString s = m_lpCmdLine;
	CStringArray saParameters;
	SplitCommandLine(m_lpCmdLine, saParameters, CMD_PARAM_USER);
	SplitCommandLine(m_lpCmdLine, saParameters, CMD_PARAM_PASSWORD);
	if (saParameters.GetSize() >= 2)
	{
		CUserArray UserArray;
		UserArray.Load(wkp);
		CSecID idUser = UserArray.IsValid(saParameters.GetAt(0), saParameters.GetAt(1));

		if (idUser.IsUserID())
		{
			CUser* pUser;
			pUser = UserArray.GetUser(idUser);
			if (pUser)
			{
				CPermissionArray PermissionArray;
				CPermission* pPermission;
				WK_DELETE(m_pUser);
				m_pUser = new CUser(*pUser);

				PermissionArray.Load(wkp);
				pPermission = PermissionArray.GetPermission(m_pUser->GetPermissionID());
				if (pPermission)
				{
					WK_DELETE(m_pPermission);
					m_pPermission = new CPermission(*pPermission);
					bLoggedIn = TRUE;
				}
			}
		}
	}

	saParameters.RemoveAll();
	SplitCommandLine(m_lpCmdLine, saParameters, CMD_PARAM_UPDATE_LD);
	if (saParameters.GetSize())	// update language dependencies
	{
		LoginAsSuperVisor();
		bLoggedIn = TRUE;
		m_bUpdateLanguageDependencies = TRUE;
		m_nCmdShow = SW_HIDE;
	}


	if (!bLoggedIn)
	{
		if (!Login())
		{
			WaitForSingleObject(pThread->m_hThread, 10000);
			return FALSE;
		}
	}
// gf todo
// Einige Flags sind völlig irrelevant für die SV, die Abfrage müßte genauer sein
	if (m_pPermission)
	{
		if ((m_pPermission->GetFlags() & WK_SV_ACCESS) == 0)
		{
			// nothing allowed
			AfxMessageBox(IDP_ACCESS_DENIED);
			return IDCANCEL;
		}
	}
	else
	{
		if (!m_bUpdateLanguageDependencies)
		{
			AfxMessageBox(IDP_ACCESS_DENIED);
			return IDCANCEL;
		}
	}
	// bitmap fuer small icons laden
	m_SmallImage.Create(IDB_IMAGES,27,1,RGB(0,255,255));
	// bitmap fuer state icons laden
	m_StateImage.Create(IDB_STATE,16,1,RGB(255,0,0));
	// bitmap fuer 3state icons laden
	m_3StateImage.Create(IDB_3STATE,48,1,RGB(255,0,0));
	// bitmap für radio icons
	m_RadioImage.Create(IDB_RADIO,16,1,RGB(255,0,0));

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#if _MFC_VER < 0x0700
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif // _AFXDLL
#endif // _MFC_VER < 0x0700

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CSVDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CSVView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	InitHTML();
	ReadOpenDirPath();
	WK_CreateDirectory(m_sOpenDir);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	COemGuiApi::ToggleKeyboardInput(SELECT_USER_DEFINED);

/*	TODO
	m_bInstruction = m_pDongle->AllowHTMLNotification();
*/

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
// CSVApp commands
void CSVApp::OnAppAbout()
{
	COemGuiApi::AboutDialog(m_pMainWnd);
}
/////////////////////////////////////////////////////////////////////////////
void CSVApp::ReadOpenDirPath()
{
	CWK_Profile wkp;
	m_sOpenDir = wkp.GetString(SECTION_COMMON, COMMON_OPENDIR, _T("c:\\out"));
}
/////////////////////////////////////////////////////////////////////////////
void CSVApp::InitHTML()
{
	CString sLH;
	sLH.Format(_T("%lx"),SECID_LOCAL_HOST);

	if (!DoesFileExist(CNotificationMessage::GetWWWRoot()))
	{
		CreateDirectory(CNotificationMessage::GetWWWRoot(),NULL);
	}
	m_sLocalWWW = CNotificationMessage::GetWWWRoot() + _T("\\") + sLH;
	if (!DoesFileExist(m_sLocalWWW))
	{
		CreateDirectory(m_sLocalWWW,NULL);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVApp::CreateKeyboardLayouts(LPCTSTR sLang1, LPCTSTR sLang2)
{
	COemGuiApi::GetKeyboardInput(0).SetLangID(sLang1);
	COemGuiApi::GetKeyboardInput(1).SetLangID(sLang2);
	if (COemGuiApi::GetKeyboardInput(0) == COemGuiApi::GetKeyboardInput(1))
	{
		COemGuiApi::GetKeyboardInput(0).Unload();
		COemGuiApi::GetKeyboardInput(1).Unload();
	}
	else
	{
		COemGuiApi::ToggleKeyboardInput(COemGuiApi::GetCodePageBits() != 0 ? 1 : 0);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSVApp::LoginAsSuperVisor()
{
	CWK_Profile wkp; // ML 1.9.99 Sollte nicht lieber GetProfile() benutzt werden?
	WK_DELETE(m_pUser);
	CPermissionArray PermissionArray;
	CPermission* pPermission;

	PermissionArray.Load(wkp);
	pPermission = PermissionArray.GetSuperVisor();
	WK_DELETE(m_pPermission);
	m_pPermission = new CPermission(*pPermission);
	
	m_pUser = new CUser();
	m_pUser->SetName(_T("SuperVisor"));
	m_pUser->SetPermissionID(m_pPermission->GetID());
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSVApp::Login()
{
//#ifdef _DEBUG
//	return LoginAsSuperVisor();
//#endif

	CWK_Profile wkp;
	CString sUser;
	CString sPassword;
	CString sText;
	BOOL bRet = FALSE;
	CPermission* pPermission;
	CUserArray ua;
	CPermissionArray pa;

	ua.Load(wkp);
	pa.Load(wkp);

	pPermission = pa.GetSpecialReceiver();
	if (pPermission)
	{
		WK_DELETE(m_pUser);
		WK_DELETE(m_pPermission);
		m_pPermission = new CPermission(*pPermission);
		m_pUser = new CUser();
		m_pUser->SetName(_T("SpecialReceiver"));
		sText.Format(IDS_LOGIN_SUCCESFUL, (LPCTSTR)m_pUser->GetName());
		WK_TRACE_USER(sText + _T("\n"));
		return TRUE;
	}

	if (ua.GetSize()==0)
	{
		WK_DELETE(m_pUser);

		pPermission = pa.GetSuperVisor();
		
		if (sPassword==pPermission->GetPassword())
		{
			WK_DELETE(m_pPermission);
			m_pPermission = new CPermission(*pPermission);
			m_pUser = new CUser();
			m_pUser->SetName(_T("SuperVisor"));
			sText.Format(IDS_LOGIN_SUCCESFUL, (LPCTSTR)m_pUser->GetName());
			WK_TRACE_USER(sText + _T("\n"));
			return TRUE;
		}
	}

	COEMLoginDialog dlg;
	dlg.SetMonitorFlags(wkp.GetInt(REG_KEY_SUPERVISOR,REG_KEY_USE_MONITOR, 0));
	dlg.SetShowTopMost(TRUE);
							    
	if (IDOK==dlg.DoModal())
	{
		WK_DELETE(m_pUser);
		m_pUser = new CUser(dlg.GetUser());
		WK_DELETE(m_pPermission);
		m_pPermission = new CPermission(dlg.GetPermission());

		bRet = (m_pUser && m_pUser->GetID()!=SECID_NO_ID) &&
			   (m_pPermission && m_pPermission->GetID()!=SECID_NO_ID);
	}
	else
	{
		bRet = FALSE;
	}

	if (m_pUser) 
	{
		sUser = m_pUser->GetName();
	}
	if (bRet)
	{
		sText.Format(IDS_LOGIN_SUCCESFUL, sUser);
		WK_TRACE_USER(sText + _T("\n"));
	}
	else
	{
		sText.Format(IDS_LOGIN_DENIED, sUser);
		WK_TRACE_USER(sText + _T("\n"));
	}

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
int CSVApp::ExitInstance() 
{
	WK_DELETE(m_pUser);
	WK_DELETE(m_pPermission);
	WK_DELETE(m_pDongle);
	
	WK_TRACE_USER(_T("SystemVerwaltung wird beendet\n"));
	CloseDebugger();

	CWinApp::ExitInstance();	// Free m_hLangResourceDLL
	return 0L;
}
/////////////////////////////////////////////////////////////////////////////
UINT CSVApp::EvaluateCommPortThreadProc(LPVOID pVoid)
{
	CWK_Profile wkp;

	DWORD dwMask = ::GetCommPortInfo();
	wkp.WriteInt(SECTION_COMMON, COMMON_COMM_PORT_INFO, dwMask);
#ifndef _DEBUG
	CString sComPortInfoEx = GetCommPortInfoEx();
	if (!sComPortInfoEx.IsEmpty())
	{
		wkp.WriteString(SECTION_COMMON, COMMON_COMM_PORT_INFO_EX, sComPortInfoEx);
	}
#endif
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
int	CSVApp::ImageByExType(ExOutputType e) const
{
	int iImage = IMAGE_NULL;
	switch (e) 
	{
	case EX_OUTPUT_RELAY:
		iImage = IMAGE_RELAY;			
		break;
	case EX_OUTPUT_CAMERA_FBAS:	
		iImage = IMAGE_CAMERA_FBAS;	
		break;
	case EX_OUTPUT_CAMERA_SVHS:		
		iImage = IMAGE_CAMERA_SVHS;		
		break;
	default:
		iImage = IMAGE_NULL;
	}
	return iImage;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSVApp::SplitCommandLine(const CString sCommandLine, CStringArray& saParameters, LPCTSTR szParam)
{
	CString sCL = _T(" ") + sCommandLine;
	int nFind1, nFind2;
	nFind1 = sCL.Find(szParam);
	if (nFind1 != -1)
	{
		nFind1 += _tcslen(szParam);
		nFind2 = sCL.Find(_T(" -"), nFind1);
		if (nFind2 != -1 && nFind2 >= nFind1)
		{
			saParameters.Add(sCL.Mid(nFind1, nFind2-nFind1));
		}
		else
		{
			saParameters.Add(sCL.Mid(nFind1));
		}
		return TRUE;
	}

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
int CSVApp::DoMessageBox(LPCTSTR lpszPrompt, UINT nType, UINT nIDPrompt)
{
	CString sMsg;
	if (lpszPrompt)
	{
		sMsg = lpszPrompt;
	}
	else if (nIDPrompt)
	{
		sMsg.LoadString(nIDPrompt);
	}
	return COemGuiApi::MessageBox(sMsg, 0, nType);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSVApp::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message)
	{
		case WM_KEYUP:
		if (pMsg->wParam == VK_F11)
		{
			COemGuiApi::ToggleKeyboardInput();
			return TRUE;
		}
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		{
			HWND hWnd = FindWindow(WK_APP_NAME_IDIP_CLIENT, NULL);
			if (hWnd)
			{
				PostMessage(hWnd, WM_RESET_AUTO_LOGOUT, 0, 0);
			}
		}break;
	}

	return CWinApp::PreTranslateMessage(pMsg);
}
/////////////////////////////////////////////////////////////////////////////
CString CSVApp::GetHTMLCodePage()
{
	CString str;
#ifdef _UNICODE
	CStringArray ar;
	if (GetCodePageInfo(CWK_String::GetCodePage(), ar))
	{
		str = ar.GetAt(OEM_CP_WEBCHARSET);
		return str;
	}
#endif
	str.Format(_T("windows-%d"), CWK_String::GetCodePage());
	return str;
}
/////////////////////////////////////////////////////////////////////////////
int CSVApp::GetIntValue(LPCTSTR sValue, int nDefault)
{
	CWK_Profile wkp;
	int bIntValue = wkp.GetInt(REG_KEY_SUPERVISOR, sValue, -1);
	if (bIntValue == -1)
	{
		bIntValue = nDefault;
		wkp.WriteInt(REG_KEY_SUPERVISOR, sValue, bIntValue);
	}
	return bIntValue;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSVApp::StartApplication(WK_ApplicationId wai, WORD wStartParam)
{
	HWND hWnd = FindWindow(WK_APP_NAME_LAUNCHER, NULL);
	if (hWnd)
	{
		LPARAM lParam = MAKELONG(wStartParam, SECID_NO_SUBID);
		::PostMessage(hWnd, LAUNCHER_APPLICATION, wai, lParam);
		return TRUE;
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSVApp::IsNetExeOK()
{
	CString sSystem;
	GetSystemDirectory(sSystem.GetBufferSetLength(_MAX_PATH), _MAX_PATH);
	sSystem.ReleaseBuffer();
	sSystem += _T("\\net.exe");
	return DoesFileExist(sSystem);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSVApp::GetComputerNameEx(COMPUTER_NAME_FORMAT NameType, LPTSTR lpBuffer, LPDWORD nSize)
{
	if (m_pGetComputerNameEx)
	{
		return m_pGetComputerNameEx(NameType, lpBuffer, nSize);
	}
	if (NameType == ComputerNameNetBIOS)
	{
		return GetComputerName(lpBuffer, nSize);
	}
	return FALSE;
}
