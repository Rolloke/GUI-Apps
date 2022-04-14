// DVHookDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DVHook.h"
#include "DVHookDlg.h"
#include "PasswordDlg.h"
#include "SearchFiles.h"
#include "MsgFlagsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDVHookDlg dialog
#define REPORT CDVHookDlg::OutputDebug

#define INVALID_VALUE 0xffffffff
#define WM_PASSWORD_DLG       (WM_USER + 1)
#define WM_CHECKFOREGROUNDWND (WM_USER + 2)
#define WM_FIND_FILE_DLG      (WM_USER + 3)
#define WM_POSTMESSAGE        (WM_USER + 4)
#define WM_CLOSETIMER         (WM_USER + 5)

#define WILDCARD_PATH 0x8000

extern CDVHookApp theApp;

CDVHookDlg* CDVHookDlg::m_pThis = NULL;

/////////////////////////////////////////////////////////////////////////////
CDVHookDlg::CDVHookDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDVHookDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDVHookDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pThis = this;
	m_bRun	= TRUE;
	m_hWndForeground = NULL;
	// Debugthread anlegen.
	m_pWatchThread = AfxBeginThread(WatchThread, this);
	if (m_pWatchThread)
	{
		m_pWatchThread->m_bAutoDelete = FALSE;
		m_pWatchThread->SetThreadPriority(THREAD_PRIORITY_NORMAL);
	}
	m_dwFlags = m_prof.GetInt(_T("DV\\DVHook\\"), _T("Output"), 0x00000001);
	m_rcEdit.SetRectEmpty();
	m_nTimer	     = 0;
	m_bHidden     = false;
	m_bHideAgain  = false;
}

/////////////////////////////////////////////////////////////////////////////
CDVHookDlg::~CDVHookDlg()
{
	m_bRun = FALSE;

	m_Hook.DisableGetMessageHook();
	m_Hook.DisableCallWndProcHook();
	m_Hook.DisableSysMsgFilterHook();
	m_Hook.DisableMsgFilterHook();
	m_Hook.DisableCBTHook();
	m_Hook.DisableShellHook();

   // Warte bis 'DebugThread' beendet ist.
	if (m_pWatchThread)
	{
		WaitForSingleObject(m_pWatchThread->m_hThread, 2000);
		delete(m_pWatchThread); //Autodelete = FALSE;
		m_pWatchThread = NULL;
	}
	
	while (m_Allocated.GetCount())
	{
		free(m_Allocated.RemoveHead());
	}
}

/////////////////////////////////////////////////////////////////////////////
void CDVHookDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDVHookDlg)
	DDX_Control(pDX, IDC_EDT_OUTPUT, m_Output);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDVHookDlg, CDialog)
	//{{AFX_MSG_MAP(CDVHookDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_SELFCHECK, DoConfirmSelfcheck)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_FLAGS, OnBtnFlags)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
	ON_WM_CREATE()
	ON_COMMAND(ID_SHOW_HOOK_DLG, OnShowHookDlg)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_USER, OnUserMessage)
	ON_MESSAGE(WM_PASSWORD_DLG, OnPasswordDlg)
	ON_MESSAGE(WM_CHECKFOREGROUNDWND, CheckForegroundWindow)
	ON_MESSAGE(WM_FIND_FILE_DLG, OnFindFileDlg)
	ON_MESSAGE(WM_POSTMESSAGE, OnPostMessage)
	ON_MESSAGE(WM_CLOSETIMER, OnCloseTimer)
	ON_MESSAGE(WM_TRAYCLICKED, OnTrayClicked)	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
void CDVHookDlg::OnDestroy() 
{
	NOTIFYICONDATA tnd;

	tnd.cbSize		= sizeof(NOTIFYICONDATA);
	tnd.hWnd		= m_hWnd;
	tnd.uID			= 1;
	tnd.uFlags		= 0;

	Shell_NotifyIcon(NIM_DELETE, &tnd);
	PostQuitMessage(0);
	CDialog::OnDestroy();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDVHookDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	HWND hWndDummy = ::GetDlgItem(m_hWnd, IDC_DUMMY);
	::SetWindowLong(hWndDummy, GWL_USERDATA, m_dwFlags);
	m_Hook.SetDVHookDlgHWND(hWndDummy);
	m_Hook.EnableGetMessageHook();
	m_Hook.EnableCallWndProcHook();
	m_Hook.EnableSysMsgFilterHook();
	m_Hook.EnableMsgFilterHook();
	m_Hook.EnableCBTHook();
	m_Hook.EnableShellHook();
	CRect rc;
	GetClientRect(&rc);
	CWnd *pWnd = GetDlgItem(IDC_EDT_OUTPUT);
	if (pWnd)
	{
		pWnd->GetWindowRect(&m_rcEdit);
		ScreenToClient(&m_rcEdit);
		m_rcEdit.right  = rc.right  - m_rcEdit.right;
		m_rcEdit.bottom = rc.bottom - m_rcEdit.bottom;
	}

	pWnd = GetDlgItem(IDOK);
	if (pWnd)
	{
		pWnd->GetWindowRect(&m_rcBtnOK);
		ScreenToClient(&m_rcBtnOK);
		m_rcBtnOK.left   = rc.right  - m_rcBtnOK.left;
		m_rcBtnOK.top    = rc.bottom - m_rcBtnOK.top;
		m_rcBtnOK.right  = rc.right  - m_rcBtnOK.right;
		m_rcBtnOK.bottom = rc.bottom - m_rcBtnOK.bottom;
	}

	pWnd = GetDlgItem(IDC_BTN_FLAGS);
	if (pWnd)
	{
		pWnd->GetWindowRect(&m_rcBtnFlags);
		ScreenToClient(&m_rcBtnFlags);
		m_rcBtnFlags.left   = rc.right  - m_rcBtnFlags.left;
		m_rcBtnFlags.top    = rc.bottom - m_rcBtnFlags.top;
		m_rcBtnFlags.right  = rc.right  - m_rcBtnFlags.right;
		m_rcBtnFlags.bottom = rc.bottom - m_rcBtnFlags.bottom;
	}
	CheckHidden();
	m_nTimer = SetTimer(1, 1000, NULL);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDVHookDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	if ((cx != 0) && (cy != 0) && !m_rcEdit.IsRectNull())
	{
		CRect rc;
		rc.left   = m_rcEdit.left;
		rc.top    = m_rcEdit.top;
		rc.right  = cx - m_rcEdit.right;
		rc.bottom = cy - m_rcEdit.bottom;
		GetDlgItem(IDC_EDT_OUTPUT)->MoveWindow(&rc);

		rc.left   = cx - m_rcBtnOK.left;
		rc.top    = cy - m_rcBtnOK.top;
		rc.right  = cx - m_rcBtnOK.right;
		rc.bottom = cy - m_rcBtnOK.bottom;
		GetDlgItem(IDOK)->MoveWindow(&rc);

		rc.left   = cx - m_rcBtnFlags.left;
		rc.top    = cy - m_rcBtnFlags.top;
		rc.right  = cx - m_rcBtnFlags.right;
		rc.bottom = cy - m_rcBtnFlags.bottom;
		GetDlgItem(IDC_BTN_FLAGS)->MoveWindow(&rc);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

/////////////////////////////////////////////////////////////////////////////
void CDVHookDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
/////////////////////////////////////////////////////////////////////////////
HCURSOR CDVHookDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

/////////////////////////////////////////////////////////////////////////////
long CDVHookDlg::DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam)
{
	HWND hWnd = (HWND)wParam;

	if (hWnd && IsWindow(hWnd))
		::PostMessage(hWnd, WM_SELFCHECK, WAI_DV_HOOK, 0);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : CDVHookDlg
 Function   : WatchThread
 Description: Checks periodically for windows that are comming into the foreground.

 Parameters:   
  pData: (E): unused pointer (LPVOID)

 Result type:  (UINT)
 <TITLE WatchThread>
*********************************************************************************************/
UINT CDVHookDlg::WatchThread(LPVOID pData)
{
	while (m_pThis->m_bRun)
	{
#ifdef _DEBUG
		Sleep(500);
#else
		Sleep(20);
#endif
		m_pThis->CheckForegroundWindow(0, 0);
	}
	return 0;
}

bool CDVHookDlg::CheckClose(CWnd *pWnd, CString &sValue, LPCTSTR sFileName, LPCTSTR sTitle)
{
	if (sValue.CompareNoCase(DV_CLOSE) == 0)
	{
		pWnd->PostMessage(WM_CLOSE, 0, 0);
		REPORT(_T("%s Window: %s<%s>\n"), sValue, sFileName, sTitle);
		return true;
	}
	return false;
}

bool CDVHookDlg::CheckHide(CWnd *pWnd, CString &sValue, LPCTSTR sFileName, LPCTSTR sTitle)
{
	if (sValue.CompareNoCase(DV_HIDE) == 0)
	{
		pWnd->SetWindowPos(&wndBottom, 0, 0, 0,0, SWP_HIDEWINDOW|SWP_NOSIZE|SWP_NOMOVE);
		if (pWnd == this)
		{
			m_bHidden = true;
		}
		REPORT(_T("%s Window: %s<%s>\n"), sValue, sFileName, sTitle);
		return true;
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : CDVHookDlg
 Function   : CheckForegroundWindow
 Description: Checks the windows that ar comming into foreground and closes, hides, controls
              or ignores them. Unknown windows are closed by default.

 Parameters:   
  [wParamW]: (E): Handle of a window  (WPARAM)
  [lParamW]: (E): (IDOK, IDCANCEL)  (LPARAM)

 Result type: Message result  (LRESULT)
 <TITLE CheckForegroundWindow>
*********************************************************************************************/
LRESULT CDVHookDlg::CheckForegroundWindow(WPARAM wParamW, LPARAM lParamW)
{
	CWnd* pWnd = GetForegroundWindow();
	if (wParamW) pWnd = CWnd::FromHandle((HWND)wParamW);
	if (pWnd)
	{
		// Neues Vordergrundfenster?
		if (IsWindow(pWnd->m_hWnd) && (pWnd->m_hWnd != m_hWndForeground))
		{
			m_hWndForeground = pWnd->m_hWnd;
			// Welcher Applikation gehört das Fenster?
			CString sFileName = GetFileNameFromWindowHandle(pWnd->m_hWnd);
			sFileName.MakeLower();

			// Pfadangabe entfernen
			int nIndex = sFileName.ReverseFind('\\');
			if (nIndex != -1)
				sFileName = sFileName.Mid(nIndex+1);
			
			// Soll das Fenster geschlossen, versteckt, oder ignoriert werden?
			CString sValue = m_prof.GetString(_T("DV\\DVHook\\"), sFileName, DV_CLOSE);
			CString sTitle;

			pWnd->GetWindowText(sTitle);
			WK_TRACE(_T("%s\n%s: %s\n"), sFileName, sTitle, sValue);

			if (CheckClose(pWnd, sValue, sFileName, sTitle))
			{
				return 0;
			}
			else if (CheckHide(pWnd, sValue, sFileName, sTitle))
			{
				return 0;
			}
			else if ((sValue.CompareNoCase(DV_CONTROL)==0) && (!sTitle.IsEmpty()))
			{
				CString sPath, sParameter, swParam, slParam, sDlgItem, sMessage;
				WPARAM  wParam;
				LPARAM  lParam;
				int     i, nDlgItem;
				CWnd   *pWndChild = NULL;
				sPath.Format(_T("DV\\DVHook\\%s<%s>\\"), sFileName, sTitle);
				
				if (wParamW)
				{
					POSITION pos = m_Allocated.Find((void*)lParamW);
					REPORT(_T("Returned: "));
					if (pos)
					{
						sPath += _T("File\\");
						m_prof.WriteString(sPath, _T("lParam01"), (LPCTSTR)lParamW);
						m_prof.WriteString(sPath, _T("Message01"), _T("SETTEXT"));
						m_Allocated.RemoveAt(pos);
						free((void *)lParamW);
					}
					else if (LOWORD(lParamW) == IDOK)
					{
						REPORT(_T("OK "));
						WORD wTime = HIWORD(lParamW);
						if (wTime)
						{
							if (wTime & WILDCARD_PATH)
							{
								SetWildCardPath(sPath);
								wTime &= ~WILDCARD_PATH;
							}
							CTimeSpan tspan(0, 0, 0, wTime);
							CTime time = CTime::GetCurrentTime() + tspan;
							m_prof.WriteInt(sPath, DV_TIMEOUT, (int)time.GetTime());
						}
						sPath += _T("Ok\\");
					}
					else if (LOWORD(lParamW) == IDCANCEL)
					{
						REPORT(_T("Not OK "));
						sPath += _T("NotOk\\");
					}
				}

				for (i=1; ; i++)												// Enum all messages
				{
					sParameter.Format(_T("Message%02d"), i);
					sMessage = m_prof.GetString(sPath, sParameter, NULL);
					sMessage.MakeUpper();
					if (sMessage.IsEmpty())						// No Message ?
					{
						if (i==1)
						{
							sParameter.Format(_T("%s<%s>"), sFileName, sTitle);
							sValue = m_prof.GetString(_T("DV\\DVHook\\"), sParameter, DV_IGNORE);
							if (CheckClose(pWnd, sValue, sFileName, sTitle))
							{
								return 0;
							}
							else if (CheckHide(pWnd, sValue, sFileName, sTitle))
							{
								return 0;
							}
							else
							{
								sParameter.Format(_T("%s<*>"), sFileName);
								sValue = m_prof.GetString(_T("DV\\DVHook\\"), sParameter, DV_CLOSE);
								if (CheckClose(pWnd, sValue, sFileName, _T("*")))
								{
									return 0;
								}
								else if (CheckHide(pWnd, sValue, sFileName, _T("*")))
								{
									return 0;
								}
							}
							REPORT(_T("%s %s: No Message\n"), sValue, sParameter);
							if ((sPath.Find(_T("<*>")) == -1) && SetWildCardPath(sPath))// Wildcard prüfen
							{
								i = 0;
								continue;
							}
						}
						break;
					}
					if (i==1)
					{
						REPORT(_T("%s application popup from %s<%s>\n"), sValue, sFileName, sTitle);
					}
					
					REPORT(_T(" %02d:Msg:%s"), i, sMessage);
					sParameter.Format(_T("wParam%02d"), i);					// Get wParam
					wParam = m_prof.GetInt(sPath, sParameter, INVALID_VALUE);
					if (wParam == INVALID_VALUE)
					{
						swParam = m_prof.GetString(sPath, sParameter, NULL);
						if (!swParam.IsEmpty())
						{
							wParam  = (WPARAM)LPCTSTR(swParam);
							REPORT(_T(", wParam: %s"), LPCTSTR(swParam));
						}
						else
						{
							wParam = 0;
							REPORT(_T(", wParam: 0"));
						}
					}
					else
					{
						REPORT(_T(", wParam: %x"), wParam);
					}
					sParameter.Format(_T("lParam%02d"), i);					// Get lParam
					lParam = m_prof.GetInt(sPath, sParameter, INVALID_VALUE);
					if (lParam == INVALID_VALUE)
					{
						slParam = m_prof.GetString(sPath, sParameter, NULL);
						if (!slParam.IsEmpty())
						{
							lParam  = (WPARAM)LPCTSTR(slParam);
							REPORT(_T(", lParam: %s"), LPCTSTR(slParam));
						}
						else
						{
							lParam = 0;
							REPORT(_T(", lParam: 0"));
						}
					}
					else
					{
						REPORT(_T(", lParam: %x"), lParam);
					}
					
					sParameter.Format(_T("DlgItem%02d"), i);				// GetChildWindow
					nDlgItem = m_prof.GetInt(sPath, sParameter, 0);
					if (nDlgItem > 0)											// ChildItem ?
					{
						pWndChild = pWnd->GetDlgItem(nDlgItem);		// find it
						if (pWndChild)
						{
							REPORT(_T(", found Child: %x"), nDlgItem);
						}
					}
					else
					{
						sDlgItem = m_prof.GetString(sPath, sParameter, NULL);
						if (sDlgItem.IsEmpty())								// Window from name
						{
							pWndChild = NULL;									// No !
						}
						else if (sDlgItem.CompareNoCase(DV_PARENT) == 0)
						{
							REPORT(_T(", %s"), sDlgItem);
							pWndChild = pWnd;
						}
						else														// find it
						{
							DWORD dwValues[2] = {(DWORD)&sDlgItem, 0};
							::EnumChildWindows(pWnd->m_hWnd, FindWindowByName, (LPARAM)dwValues);
							if (dwValues[1])
							{
								pWndChild = CWnd::FromHandle((HWND)dwValues[1]);
								REPORT(_T(", found Child: %s"), sDlgItem);
							}
						}
					}

					REPORT(_T("\n"));
					if (sMessage == DV_PASSWORD)
					{
						StartPasswordDialog(pWnd, sPath, lParam);
					}
					else if ((sMessage == DV_FINDFILE) && !swParam.IsEmpty())
					{
						StartFindDialog(pWnd, swParam);
					}
					else if (CheckClose(pWnd, sMessage, sFileName, sTitle))
					{
						return 0;
					}
					if (((pWndChild == NULL) || (pWndChild == pWnd)) && (sMessage.CompareNoCase(DV_SHOW) == 0))
					{
						const CWnd *pWndInsertAfter = NULL;
						if      (slParam.CompareNoCase(DV_TOP)       == 0) pWndInsertAfter = &wndTop; 
						else if (slParam.CompareNoCase(DV_TOPMOST)   == 0) pWndInsertAfter = &wndTopMost;
						else if (slParam.CompareNoCase(DV_BOTTOM)    == 0) pWndInsertAfter = &wndBottom;
						else if (slParam.CompareNoCase(DV_NOTOPMOST) == 0) pWndInsertAfter = &wndNoTopMost;
						else
						{
							if (wParam != 0) pWndInsertAfter = &wndTop;		 // Show = true
							else             pWndInsertAfter = &wndBottom;   // Show = false
						}
						pWnd->SetWindowPos(pWndInsertAfter, 0, 0, 0, 0, ((wParam!=0) ? (SWP_SHOWWINDOW|SWP_FRAMECHANGED):SWP_HIDEWINDOW)|SWP_NOSIZE|SWP_NOMOVE);
					}
					else if (pWndChild)
					{
						if (sMessage.CompareNoCase(DV_ENABLE) == 0)
						{
							pWndChild->EnableWindow(wParam);
						}
						else if (sMessage.CompareNoCase(DV_SHOW) == 0)
						{
							::ShowWindow(pWnd->m_hWnd, (wParam!=0) ? SW_SHOW:SW_HIDE);
						}
						else if (CheckClose(pWndChild, sMessage, sFileName, _T("Child of ") + sTitle))
						{
							return 0;
						}
						else if (sMessage == DV_GETTEXT)
						{
							GetText(pWndChild, sPath, swParam, slParam);
						}
						else if (sMessage == DV_SETTEXT)
						{
							SetText(pWndChild, wParam, swParam, slParam);
						}
						else
						{
							int nMessage = 0;
							if      (sMessage.Find(DV_COMMAND)    != -1) nMessage = WM_COMMAND;
							else if (sMessage.Find(DV_SYSCOMMAND) != -1) nMessage = WM_SYSCOMMAND;
							if (nMessage)
							{
								if (sMessage.Find(_T("POST")))
								{
									pWndChild->PostMessage(nMessage, wParam, lParam);
								}
								else if (sMessage.Find(_T("INVOKE")) != -1)
								{
									MSG *pmsg = new MSG;
									ZeroMemory(pmsg, sizeof(MSG));
									pmsg->hwnd    = pWndChild->m_hWnd;
									pmsg->message = nMessage;
									pmsg->wParam  = wParam;
									pmsg->lParam  = lParam;
									pWndChild->PostMessage(WM_POSTMESSAGE, (WPARAM)pWnd->m_hWnd, (LPARAM)pmsg);
								}
								else
								{
									pWndChild->SendMessage(nMessage, wParam, lParam);
								}
							}
						}
					}
				}
			}
			else
			{
				if (pWnd == this)
				{
					m_bHidden = false;
					CheckHidden();
				}
				REPORT(_T("%s %s<%s>\n"), sValue, sFileName, sTitle);
			}
		}
	}
	return 1;
}

void CDVHookDlg::StartFindDialog(CWnd *pWnd, CString &sSearchFile)
{
	if (!m_FindStartedFor.Find(pWnd->m_hWnd))
	{
		m_FindStartedFor.AddHead(pWnd->m_hWnd);
		::SetWindowPos(m_hWnd, HWND_NOTOPMOST, 0, 0, 0,0, SWP_SHOWWINDOW|SWP_FRAMECHANGED|SWP_NOSIZE|SWP_NOMOVE);
		::EnableWindow(pWnd->m_hWnd, false);
		LPARAM lP = (LPARAM)_tcsdup(sSearchFile);
		m_Allocated.AddHead((void*)lP);
		PostMessage(WM_FIND_FILE_DLG, (WPARAM)pWnd->m_hWnd, lP);
	}
}

void CDVHookDlg::StartPasswordDialog(CWnd *pWnd, CString&sPath, LPARAM lParam)
{
	ASSERT(pWnd);
	if (m_PWdStartedFor.Find(pWnd->m_hWnd) == NULL)
	{
		m_PWdStartedFor.AddHead(pWnd->m_hWnd);
		CTime time(m_prof.GetInt(sPath, DV_TIMEOUT, 0));
		CTime tnow = CTime::GetCurrentTime();
		if (tnow > time)
		{
			::SetWindowPos(m_hWnd, HWND_NOTOPMOST, 0, 0, 0,0, SWP_SHOWWINDOW|SWP_FRAMECHANGED|SWP_NOSIZE|SWP_NOMOVE);
			::EnableWindow(pWnd->m_hWnd, false);
			if (sPath.Find(_T("<*>")) != -1)
			{
				lParam |= WILDCARD_PATH;
			}
			PostMessage(WM_PASSWORD_DLG, (WPARAM)pWnd->m_hWnd, lParam);
		}
	}
}

void CDVHookDlg::SetText(CWnd *pWnd, WPARAM wParam, CString &swParam, CString& slParam)
{
	CString strDummy, strText;
	if      (!swParam.IsEmpty()) strText = swParam;
	else if (!slParam.IsEmpty()) strText = slParam;
	
	if (wParam == 1)
	{
		REPORT(_T(", Via Clipboard:"));
		if (!strText.IsEmpty() && OpenClipboard())
		{
			EmptyClipboard();
			HGLOBAL hLMem = GlobalAlloc(GHND, strText.GetLength()+1);
			if (hLMem)
			{
				_tcscpy((_TCHAR*)GlobalLock(hLMem), strText);
				GlobalUnlock(hLMem);
				if (::SetClipboardData(CF_TEXT, hLMem) == NULL)
				{
					LocalFree(hLMem);
					hLMem = NULL;
				}
			}
			CloseClipboard();
			if (hLMem)
			{
				::SendMessage(pWnd->m_hWnd, EM_SETSEL, 0, (LPARAM)-1);
				::SendMessage(pWnd->m_hWnd, WM_PASTE, 0, 0);
			}
		}
	}
	else
	{
		REPORT(_T(", Direct:"));
		pWnd->SetWindowText(strText);
	}
	REPORT(_T("%s"), strText);
}

void CDVHookDlg::GetText(CWnd* pWnd, CString& sRegPath, CString& sKey, CString& sParseString)
{
	if (!sKey.IsEmpty())
	{
		CString str;
		pWnd->GetWindowText(str);
		if (str.IsEmpty())
		{
			::SendMessage(pWnd->m_hWnd, EM_SETSEL, 0, (LPARAM)-1);
			::SendMessage(pWnd->m_hWnd, WM_COPY, 0, 0);
			if (OpenClipboard())
			{
				char * text= (char*)::GetClipboardData(CF_TEXT);
				if (text)
				{
					str = text;
				}
				CloseClipboard();
			}
		}
		
		if (!sParseString.IsEmpty())
		{
			REPORT(_T(", Parsestring: %s, %d"), sParseString, sParseString.GetLength());
			if (sParseString.Find(_T("%s")) != -1)
			{
				_TCHAR szText[1024];
				_stscanf(str, sParseString, szText);
				str = szText;
			}
			else
			{
				int nPos = sParseString.Find(_T("::"));
				if (nPos != -1)
				{
					int nStart = str.Find(sParseString.Left(nPos));
					if (nStart != -1)
					{
						nStart += sParseString.Left(nPos).GetLength();
						int nEnd   = str.Find(sParseString.Right(sParseString.GetLength()-nPos-2), nStart);
						if (nEnd != -1)
						{
							str = str.Mid(nStart, nEnd - nStart);
						}
					}
				}
			}
		}
		REPORT(_T(", %s"), str);
		m_prof.WriteString(sRegPath, sKey, str);
	}
}

/*********************************************************************************************
 Class      : CDVHookDlg
 Function   : CALLBACK FindWindowByName
 Description: Searches for a child window by its name

 Parameters:   
  hwnd  : (E): child window handle  (HWND)
  lParam: (EA): Pointer to DWORD[2] array with:	(LPARAM)
               value[0] (pointer to CString) and 
					value[1] (found window handle)

 Result type:  0: stop searching, 1: continue searching (BOOL)
 <TITLE FindWindowByName>
*********************************************************************************************/
BOOL CALLBACK CDVHookDlg::FindWindowByName(HWND hwnd, LPARAM lParam)
{
	_TCHAR szName[MAX_PATH];
	::GetWindowText(hwnd, szName, MAX_PATH-1);
	CString *pStr = (CString *) ((DWORD*)lParam)[0];
	if (pStr->CompareNoCase(szName) == 0)
	{
		((DWORD*)lParam)[1] = (DWORD)hwnd;
		return 0;
	}
	return 1;
}

/////////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : CDVHookDlg
 Function   : GetFileNameFromWindowHandle
 Description: Determines the file name of the program by the window handle

 Parameters:   
  hWnd: (E): Window Handle  (HWND)

 Result type: Path to the program (CString)
 <TITLE GetFileNameFromWindowHandle>
*********************************************************************************************/
CString CDVHookDlg::GetFileNameFromWindowHandle(HWND hWnd)
{
	CString sFileName;
	if (hWnd)
	{
		DWORD dwPID = 0;
		GetWindowThreadProcessId(hWnd, &dwPID);
		if (dwPID != 0)
		{
			HANDLE  hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPID);
			if (hProcess)
			{
				HMODULE hModule = NULL;
				DWORD dwNeeded  = sizeof(hModule);
				if (EnumProcessModules(hProcess, &hModule, sizeof(hModule), &dwNeeded))
				{
					char szFileName[_MAX_PATH] = {0};
					if (GetModuleFileNameExA(hProcess, hModule, szFileName, sizeof(szFileName)))
						sFileName = szFileName;
				}
				CloseHandle(hProcess);
			}
		}
	}
	return sFileName;
}				

/*********************************************************************************************
 Class      : CDVHookDlg
 Function   : OnUserMessage
 Description: User message from Hook.dll to copy text messages from annother process through
				  a window handle to the DVHook dialog.

 Parameters:   
  wParam: (): unused  (WPARAM)
  lParam: (): unused  (LPARAM)

 Result type: Message result (LRESULT)
 <TITLE OnUserMessage>
*********************************************************************************************/
LRESULT CDVHookDlg::OnUserMessage(WPARAM wParam, LPARAM lParam)
{
	CString str;
	GetDlgItem(IDC_DUMMY)->GetWindowText(str);
	REPORT(_T("%s"), str);
	return 1;
}

/*********************************************************************************************
 Class      : CDVHookDlg
 Function   : OnPasswordDlg
 Description: Invokes a password dialog to control the behaviour of annother process window

 Parameters:   
  wParam: (E): process window handle to control  (WPARAM)
  lParam: (): unused (LPARAM)

 Result type: Message result (LRESULT)
 <TITLE OnPasswordDlg>
*********************************************************************************************/
LRESULT CDVHookDlg::OnPasswordDlg(WPARAM wParam, LPARAM lParam)
{
	CWinApp *pApp    = AfxGetApp();
	int nResult;
	CPasswordDlg dlg;

	dlg.m_pTempMain  = pApp->m_pMainWnd;
	pApp->m_pMainWnd = NULL;
	nResult = MAKELONG(dlg.DoModal(), lParam);
	CheckHidden();

	m_hWndForeground = (HWND)NULL;
	::EnableWindow((HWND)wParam, true);									// Das kontrollierte Fenster wieder enablen.
	::SetForegroundWindow((HWND)wParam);								// und in den Vordergrund setzen

	PostMessage(WM_CHECKFOREGROUNDWND, wParam, nResult);
	POSITION pos = m_PWdStartedFor.Find((void*)wParam);
	if (pos)
	{
		m_PWdStartedFor.RemoveAt(pos);
	}

	return 1;
}

/*********************************************************************************************
 Class      : CDVHookDlg
 Function   : OnFindFileDlg
 Description: Invokes a dialog to find a file in the setup paths "Installation Sources"
				  in the registry

 Parameters:   
  wParam: (E): Handle of the previous foreground window  (WPARAM)
  lParam: (E): File name string allocated with alloc()  (LPARAM)

 Result type: TRUE (LRESULT)
 <TITLE OnFindFileDlg>
*********************************************************************************************/
LRESULT CDVHookDlg::OnFindFileDlg(WPARAM wParam, LPARAM lParam)
{
	CWinApp *pApp    = AfxGetApp();
	int nResult;
	CSearchFiles dlg;

	POSITION pos = m_Allocated.Find((void*)lParam);
	if (pos)
	{
		dlg.m_sSearch = (_TCHAR*)lParam;
		free((void*)lParam);
		m_Allocated.RemoveAt(pos);
	}

	dlg.m_pTempMain  = pApp->m_pMainWnd;
	pApp->m_pMainWnd = NULL;
	nResult = dlg.DoModal();
	CheckHidden();
	if (nResult == IDOK)
	{
		nResult = (int)_tcsdup(dlg.m_sSearch);
		m_Allocated.AddHead((void*)nResult);
	}

	m_hWndForeground = (HWND)NULL;
	::EnableWindow((HWND)wParam, true);									// Das kontrollierte Fenster wieder enablen.
	::SetForegroundWindow((HWND)wParam);								// und in den Vordergrund setzen

	PostMessage(WM_CHECKFOREGROUNDWND, wParam, nResult);
	
	pos = m_FindStartedFor.Find((void*)wParam);
	if (pos)
	{
		m_FindStartedFor.RemoveAt(pos);
	}

	return 1;
}

LRESULT CDVHookDlg::OnCloseTimer(WPARAM wParam, LPARAM lParam)
{
	if (m_nTimer) KillTimer(m_nTimer);
	m_nTimer = 0;
	return 1;
}

LRESULT CDVHookDlg::OnPostMessage(WPARAM wParam, LPARAM lParam)
{
	MSG *pmsg = (MSG*) lParam;
	::SendMessage(pmsg->hwnd, pmsg->message, pmsg->wParam, pmsg->lParam);
	delete pmsg;
	m_hWndForeground = (HWND)wParam;
	::SetForegroundWindow((HWND)wParam);								// und in den Vordergrund setzen
//	PostMessage(WM_CHECKFOREGROUNDWND, wParam, 0);
	return 1;
}

/*********************************************************************************************
 Class      : CDVHookDlg
 Function   : OutputDebug
 Description: Reports text messages to the output window or a log-file.

 Parameters:   
  lpszFormat: (E): Format string  (LPCTSTR)
  [...]     : (E): optional further parameters depending on the format string

 Result type:  (void _cdecl)
 <TITLE OutputDebug>
*********************************************************************************************/
void _cdecl CDVHookDlg::OutputDebug(LPCTSTR lpszFormat, ...)
{
	if (m_pThis						  == NULL) return;
	if (m_pThis->m_hWnd			  == NULL) return;
	if (m_pThis->m_Output.m_hWnd == NULL) return;
   if (!(m_pThis->m_dwFlags & 0x00000003)) return;
	
   va_list args;
   va_start(args, lpszFormat);
   int nBuf;
   TCHAR szBuffer[1024];
	ZeroMemory(szBuffer, 1023);

   nBuf = _vstprintf(szBuffer, lpszFormat, args);

	_TCHAR *psz = szBuffer;
	while (psz)
	{
		psz = _tcsstr(psz, _T("\n"));
		if (psz)
		{
			int i, nLen = _tcslen(psz);
			for (i=nLen; i>0; i--)
				psz[i] = psz[i-1];
			psz[0] = '\r';
			psz = &psz[2];
		}
	}
	
   if (m_pThis->m_dwFlags & 0x00000002)
	{
		if (m_pThis->m_Output.GetWindowTextLength() > 28000)
		{
			m_pThis->m_Output.SetWindowText(NULL);
		}
		m_pThis->m_Output.SetSel(-1, -1);
		m_pThis->m_Output.ReplaceSel(szBuffer);
	}
	else if (m_pThis->m_dwFlags & 0x00000001)
	{
		WK_TRACE(_T("%s"), szBuffer);
	}
   va_end(args);
}

void CDVHookDlg::OnBtnFlags() 
{
	CMsgFlagsDlg dlg;
	dlg.m_dwFlags = m_dwFlags;
	if (dlg.DoModal() == IDOK)
	{
		m_dwFlags = dlg.m_dwFlags;
		HWND hWndDummy = ::GetDlgItem(m_hWnd, IDC_DUMMY);
		::SetWindowLong(hWndDummy, GWL_USERDATA, m_dwFlags);
		m_prof.WriteInt(_T("DV\\DVHook\\"), _T("Output"), m_dwFlags);
	}
}

BOOL CDVHookDlg::PreCreateWindow(CREATESTRUCT& cs) 
{
	return CDialog::PreCreateWindow(cs);
}


void CDVHookDlg::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == m_nTimer)
	{
		PostMessage(WM_CLOSETIMER, m_nTimer);
		CheckHidden();
	}
	
	CDialog::OnTimer(nIDEvent);
}

void CDVHookDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	if (m_nTimer && bShow)
	{
		SetWindowPos(NULL, 200, 200, 150, 65, SWP_SHOWWINDOW);
	}
	CheckHidden();
}

bool CDVHookDlg::SetWildCardPath(CString &sPath)
{
	int nB1 = sPath.Find('<');
	int nB2 = sPath.Find('>');
	if ((nB1 != -1) && (nB2 != -1))
	{
		CString sTemp = sPath.Left(nB1+1);
		sTemp += _T("*");
		sTemp += sPath.Mid(nB2);
		sPath = sTemp;
		return true;
	}
	return false;
}

void CDVHookDlg::CheckHidden()
{
	BOOL bIsVisible = IsWindowVisible();
	if (m_bHidden)
	{
		if (bIsVisible)
		{
			SetWindowPos(&wndBottom, 0, 0, 0,0, SWP_HIDEWINDOW|SWP_NOSIZE|SWP_NOMOVE);
			ShowWindow(SW_HIDE);
			EnableWindow(FALSE);
		}
	}
	else
	{
		if (!bIsVisible)
		{
			EnableWindow(TRUE);
			SetWindowPos(&wndBottom, 0, 0, 0, 0, SWP_SHOWWINDOW|SWP_NOSIZE|SWP_NOMOVE);
			ShowWindow(SW_SHOWNORMAL);
		}
	}
}

int CDVHookDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	if (m_bHidden)
	{
		NOTIFYICONDATA tnd;

		CString sTip;
		GetWindowText(sTip);

		tnd.cbSize = sizeof(NOTIFYICONDATA);
		tnd.hWnd	  = m_hWnd;
		tnd.uID    = 1;
		tnd.hIcon  = m_hIcon;
		tnd.uFlags = NIF_MESSAGE|NIF_ICON|NIF_TIP;
		tnd.uCallbackMessage = WM_TRAYCLICKED;
		lstrcpyn(tnd.szTip, sTip, sTip.GetLength()+1);
		Shell_NotifyIcon(NIM_ADD, &tnd);
	}
	else
	{
		SetWindowPos(&CWnd::wndTop, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);
	}
	return 0;
}

LRESULT CDVHookDlg::OnTrayClicked(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
		case WM_RBUTTONDOWN:
		{
			CMenu menu;
			CMenu* pM;
			CPoint pt;

			GetCursorPos(&pt);
			menu.LoadMenu(IDR_MAINFRAME);
			pM = menu.GetSubMenu(0);
			SetForegroundWindow();		// Siehe ID: Q135788 
			pM->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this);
			PostMessage(WM_NULL, 0, 0); // Siehe ID: Q135788
		}break;
	}
	return 0;
}

void CDVHookDlg::OnOK() 
{
	if (m_bHideAgain)
	{
		m_bHidden    = true;
		m_bHideAgain = false;
		CWK_Profile prof;
		theApp.InitHookValue(prof, _T("DVHook.exe<DVHook>"), DV_HIDE, true);
		CheckHidden();
	}
	else
	{
		CDialog::OnOK();
	}
}

void CDVHookDlg::OnShowHookDlg() 
{
	m_bHidden    = false;
	m_bHideAgain = true;
	CWK_Profile prof;
	theApp.InitHookValue(prof, _T("DVHook.exe<DVHook>"), DV_IGNORE, true);
	CheckHidden();
}
