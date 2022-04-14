// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "SMSUnit.h"

#include "MainFrm.h"
#include "IPCServerControlSMS.h"

#include "CapiSMS_Vodafone.h"
#include "CapiSMS_T_Mobile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame
IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_WM_SYSCOMMAND()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_PAGER, OnPager)
	ON_MESSAGE(WM_SELFCHECK, DoConfirmSelfcheck)
	ON_MESSAGE(WM_TRAYCLICKED, OnTrayClicked)	
	ON_MESSAGE(CAPI_SMS_MSG, OnCapiSMSMessage)
	ON_MESSAGE(WM_LANGUAGE_CHANGED, OnLanguageChanged)	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};
/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction
/////////////////////////////////////////////////////////////////////////////
CMainFrame::CMainFrame()
{
	m_pCapiSMS = NULL;
	m_pCapiWnd = NULL;
	m_dwTimeOut = 0;
	m_dwStart = 0;
	m_uTimerID = 0;
	m_bProviderResponse = FALSE;
	m_bIsNewCapiSMS = FALSE;
	m_bConfirmed	= FALSE;
}
/////////////////////////////////////////////////////////////////////////////
CMainFrame::~CMainFrame()
{
	WK_DELETE(m_pCapiSMS);
	if (m_pCapiWnd)
	{
		m_pCapiWnd->DestroyWindow();
		WK_DELETE(m_pCapiWnd);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnDestroy() 
{
	CFrameWnd::OnDestroy();
	
	// Nur erforderlich, wenn Icon in der Systemtaskleiste
	NOTIFYICONDATA tnd;

	tnd.cbSize		= sizeof(NOTIFYICONDATA);
	tnd.hWnd		= m_hWnd;
	tnd.uID			= 1;
	tnd.uFlags		= 0;

	Shell_NotifyIcon(NIM_DELETE, &tnd);
}
/////////////////////////////////////////////////////////////////////////////
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// create a view to occupy the client area of the frame
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	GetSystemMenu(FALSE)->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND|MF_GRAYED);

	// Icon ins Systemtray
	NOTIFYICONDATA tnd;

	CString sTip = COemGuiApi::GetApplicationName(WAI_SMS_UNIT);

	tnd.cbSize		= sizeof(NOTIFYICONDATA);
	tnd.hWnd		= m_hWnd;
	tnd.uID			= 1;

	tnd.uFlags		= NIF_MESSAGE|NIF_ICON|NIF_TIP;
	tnd.uCallbackMessage = WM_TRAYCLICKED;
	tnd.hIcon		= theApp.LoadIcon(IDR_MAINFRAME);

	lstrcpyn(tnd.szTip, sTip, sTip.GetLength()+1);
	Shell_NotifyIcon(NIM_ADD, &tnd);

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = WK_APPNAME_SMSUNIT;
	cs.lpszName = AfxGetApp()->m_pszAppName;
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics
#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnSetFocus(CWnd* pOldWnd)
{
	// forward focus to the view window
	m_wndView.SetFocus();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnAppAbout() 
{
	COemGuiApi::AboutDialog(this);
}
/////////////////////////////////////////////////////////////////////////////
afx_msg LONG CMainFrame::OnPager(UINT msg, LONG b) 
{
	CString s;

	if (msg == MSG_ONCONNECT)
	{
		s = _T("Connected");
	} 
	else if (msg == MSG_ONCANWRITE)
	{
		s = _T("MSG_ONCANWRITE");
	} 
	else if (msg == MSG_ONCANREAD)
	{
		//strcpy(buf,_T("Can Read"));
	}
	else if (msg == MSG_PROVRESP)
	{
		m_bProviderResponse = TRUE;
		CWK_String sTxt(m_Pager.get_providerresp());
		s.Format(_T("MSG_PROVRESP: %s"), LPCTSTR(sTxt));
	} 
	else if (msg == MSG_ERROR)
	{
		CWK_String sTxt(m_Pager.get_last_error());
		s.Format(_T("MSG_ERROR: %s"), LPCTSTR(sTxt));
		theApp.m_pCIPCServerControlSMS->DoIndicateError(s);
		m_bProviderResponse = FALSE;
	} 
	else if (msg == MSG_ONERROR)
	{
		CWK_String sTxt(m_Pager.get_last_error());
		s.Format(_T("MSG_ONERROR: %s"), LPCTSTR(sTxt));
		if (theApp.m_pCIPCServerControlSMS)
		{
			if (m_bProviderResponse)
			{
				// OK Disconnect
				theApp.m_pCIPCServerControlSMS->DoConfirmAlarmConnection(_T("SMSInput"),
																		 _T("SMSOutput"),
																		 _T(""),
																		 _T(""),
																		 0
#ifdef _UNICODE
																		 , MAKELONG(CODEPAGE_UNICODE, 0)
#endif
																		 );
				m_bProviderResponse = FALSE;
			}
			else
			{
				// Error Disconnect
				theApp.m_pCIPCServerControlSMS->DoIndicateError(s);
			}
		}
		KillMyTimer();
	} 
	else if (msg == MSG_MESSID)
	{
		CWK_String sTxt(m_Pager.get_msg_id());
		s.Format(_T("MSG_MESSID: %s"), LPCTSTR(sTxt));
	} 
	else if (msg == MSG_ONNONSTD)
	{
		s = _T("MSG_ONNONSTD");
	} 
	else if (msg == MSG_ONDISCONNECT)
	{
		s = _T("MSG_ONDISCONNECT");
		WK_TRACE(_T("confirming alarm connection\n"));
		if (theApp.m_pCIPCServerControlSMS)
		{
			theApp.m_pCIPCServerControlSMS->DoConfirmAlarmConnection(_T("SMSInput"),
																	 _T("SMSOutput"),
																	 _T(""),
																	 _T(""),
																	 0
#ifdef _UNICODE
																	 , MAKELONG(CODEPAGE_UNICODE, 0)
#endif
																	 );
		}
		m_bProviderResponse = FALSE;
		KillMyTimer();
	}
	m_wndView.Add(s);

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::SendSMS(const CString& sReceiverID, const CString& sMessage, DWORD dwTimeOut)
{
	
	m_wndView.Clear();

	BOOL bSend = FALSE;
	CString sProviderName;

	m_bProviderResponse = FALSE;
	m_bConfirmed	= FALSE;

	BOOL bProviderValid = FALSE;

	// Search for provider and do the settings
	if (   0==sReceiverID.Find(_T("0170")) 
		|| 0==sReceiverID.Find(_T("0171"))
		|| 0==sReceiverID.Find(_T("0175"))
		|| 0==sReceiverID.Find(_T("0160"))
		|| 0==sReceiverID.Find(_T("0161")))
	{
		// Telekom D1
		sProviderName = _T("T-mobile");
		bProviderValid = TRUE;
#ifdef _DEBUG
		m_bIsNewCapiSMS = TRUE;
		m_pCapiSMS = new CCapiSMS_T_Mobile(this);
		m_pCapiSMS->SetProviderID(theApp.m_sPrefix + theApp.m_sD1);
		m_pCapiSMS->SetOwnNumber(theApp.m_sOwnNumber);
#else
		m_Pager.set_providerid(theApp.m_sD1);
		m_Pager.set_mode(CAPI|TAP);
		m_Pager.set_x75_t70nl(_T("ON"));
#endif
	}
	else if (   0==sReceiverID.Find(_T("0177"))
			 || 0==sReceiverID.Find(_T("0178"))
			 || 0==sReceiverID.Find(_T("0163")))
	{
		// E Plus
		sProviderName = _T("E-Plus");
		bProviderValid = TRUE;
		m_Pager.set_providerid(theApp.m_sEPlus);
		m_Pager.set_mode(CAPI|TAP);
		m_Pager.set_x75_t70nl(_T("OFF"));
	}
	else /*if (   0==sReceiverID.Find(_T("0172")) 
		|| 0==sReceiverID.Find(_T("0173"))
		|| 0==sReceiverID.Find(_T("0174"))
		|| 0==sReceiverID.Find(_T("0162"))
		)*/
	{
		// Vodafone D2
		sProviderName = _T("Vodafone D2");
		bProviderValid = TRUE;
		m_bIsNewCapiSMS = TRUE;
		m_pCapiSMS = new CCapiSMS_Vodafone(this);
		m_pCapiSMS->SetProviderID(theApp.m_sPrefix + theApp.m_sD2);
		m_pCapiSMS->SetOwnNumber(theApp.m_sOwnNumber);
	}

	if (bProviderValid)
	{
		if (   m_bIsNewCapiSMS
			&& m_pCapiSMS
			)
		{
			m_pCapiWnd = new CHiddenCapiWnd(m_pCapiSMS);
			m_pCapiWnd->m_hWnd = NULL;
			if (m_pCapiWnd->CreateEx(0, AfxRegisterWndClass(0),
				(_T("Capi Notify Window")), WS_OVERLAPPED, 0, 0, 0, 0,
				NULL, NULL))
			{
				if (m_pCapiSMS->InitCapi(m_pCapiWnd->GetSafeHwnd()))
				{
					m_wndView.Add(_T("Provider is ") + sProviderName + _T(" ") + m_pCapiSMS->GetProviderID());
					m_wndView.Add(_T("Own number is ") + m_pCapiSMS->GetOwnNumber());
					m_wndView.Add(_T("Receiver id is ") + sReceiverID);

					CSystemTime st;
					st.GetLocalTime();

					CString sMsg;
					sMsg.Format(_T("Sending <%s> to <%s> at %s"), sMessage, sReceiverID, st.GetDateTime());
					m_wndView.Add(sMsg);
					
					bSend = m_pCapiSMS->SendSMS(sReceiverID, sMessage);
					if (bSend)
					{
						m_wndView.Add(_T("Calling SMSC, waiting for response"));
						m_dwStart = GetTickCount();
						m_dwTimeOut = dwTimeOut;
						m_uTimerID = SetTimer(1,100,NULL);
					}
					else
					{
						m_wndView.Add(_T("Call to SMSC failed, Capi not ready"));
					}
				}
				else
				{
					WK_DELETE(m_pCapiSMS);
					m_pCapiWnd->DestroyWindow();
					WK_DELETE(m_pCapiWnd);
					m_bIsNewCapiSMS = FALSE;
					m_wndView.Add(_T("SendSMS failed, Capi init failed"));
				}
			}
			else
			{
				WK_DELETE(m_pCapiSMS);
				m_pCapiWnd->DestroyWindow();
				WK_DELETE(m_pCapiWnd);
				m_bIsNewCapiSMS = FALSE;
				m_wndView.Add(_T("SendSMS failed, CapiWnd create failed"));
			}
		}
		else
		{ // old Pager
			m_wndView.Add(_T("Provider is ") + sProviderName + _T(" ") + m_Pager.get_providerid());
			m_wndView.Add(_T("setting mode to ") + m_Pager.get_mode());
			m_wndView.Add(_T("setting X.75 T70 to ") + m_Pager.get_x75_t70nl());
			// dial prefix and PTT/PBX
			m_wndView.Add(_T("setting dial prefix to ") + theApp.m_sPrefix);
			m_Pager.set_dialprefix(theApp.m_sPrefix);
			if (theApp.m_sPrefix.IsEmpty())
			{
				m_Pager.set_line(_T("PTT"));
			}
			else
			{
				m_Pager.set_line(_T("PBX"));
			}
			m_wndView.Add(_T("setting line to ") + m_Pager.get_line());
			m_wndView.Add(_T("setting own number to ") + theApp.m_sOwnNumber);
			m_Pager.set_msn(theApp.m_sOwnNumber);

			m_wndView.Add(_T("setting receiver id ")+sReceiverID);
			m_Pager.set_receiverid(sReceiverID);

			CString sCopy(sMessage);

			// Ae=[ Oe=Ü Ue=] ae={ oe=ü ue=} ss=~
			sCopy.Replace(_T('Ä'),_T('['));
			sCopy.Replace(_T('ä'),_T('{'));
			sCopy.Replace(_T('ß'),_T('~'));
			sCopy.Replace(_T('Ü'),_T(']'));
			sCopy.Replace(_T('ü'),_T('}'));
			sCopy.Replace(_T('Ö'),_T('Ü'));
			sCopy.Replace(_T('ö'),_T('ü'));

			CSystemTime st;
			st.GetLocalTime();

			CString sM;
			sM.Format(_T("sending <%s> to <%s> at %s"),
				sMessage,
				sReceiverID,
				st.GetDateTime());
			m_wndView.Add(sM);
			
			bSend = m_Pager.send(this,sCopy);
			if (bSend)
			{
				m_wndView.Add(_T("send success waiting for response"));
				m_dwTimeOut = dwTimeOut;
				m_uTimerID = SetTimer(1,100,NULL);
				m_dwStart = GetTickCount();
			}
			else
			{
				m_Pager.delete_msg(this);
			}
		}
	}
	else
	{
		m_wndView.Add(_T("No Provider found for ") + sReceiverID);
	}
	return bSend;
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnTimer(UINT nIDEvent) 
{
	if (m_uTimerID == nIDEvent)
	{
		if (GetTimeSpan(m_dwStart) > m_dwTimeOut)
		{
			if (m_pCapiSMS)
			{
				m_pCapiSMS->CloseConnection();
			}
			else
			{
				m_Pager.delete_msg(this);
				m_Pager.cancel(this);
			}

			KillMyTimer();

			CString s;
			s.Format(_T("Timeout sending message %d ms"),m_dwTimeOut);
			m_wndView.Add(s);
		}
	}

	CFrameWnd::OnTimer(nIDEvent);
}

/////////////////////////////////////////////////////////////////////////////
long CMainFrame::DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam)
{
	HWND hWnd = (HWND)wParam;

	if (hWnd && IsWindow(hWnd))
		::PostMessage(hWnd, WM_SELFCHECK, WAI_SMS_UNIT, 0);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CMainFrame::OnTrayClicked(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
		case WM_LBUTTONDBLCLK:
			ShowWindow(SW_RESTORE); //SW_SHOWNORMAL);
			break;
		case WM_RBUTTONDOWN:
		{
			CMenu menu;
			CMenu* pM;
			CPoint pt;

			GetCursorPos(&pt);
			menu.LoadMenu(IDR_MAINFRAME);
			pM = menu.GetSubMenu(0);
			CString sAbout;
			menu.GetMenuString(ID_APP_ABOUT, sAbout, MF_BYCOMMAND);
			pM->InsertMenu(ID_APP_EXIT, MF_STRING | MF_BYCOMMAND, ID_APP_ABOUT, (LPCTSTR)sAbout);
			pM->InsertMenu(ID_APP_EXIT, MF_SEPARATOR | MF_BYCOMMAND, 0, (LPCTSTR)NULL);

			SetForegroundWindow();		// Siehe ID: Q135788 
			pM->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this);
			PostMessage(WM_NULL, 0, 0); // Siehe ID: Q135788
			break;
		}
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam) 
{
	CFrameWnd::OnSysCommand(nID, lParam);

	if (nID == SC_MINIMIZE)
		ShowWindow(SW_HIDE);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CMainFrame::OnCapiSMSMessage(WPARAM wParam, LPARAM lParam)
{
	BOOL bDisconnected = FALSE;
	CString sMsg;
	BOOL bConfirm = FALSE;
	switch (wParam)
	{
		case CAPI_ERROR:
			bDisconnected = TRUE;
			bConfirm = TRUE;
			sMsg = _T("Capi error, FAILED");
			break;
		case CAPI_SMS_CONNECTED:
			sMsg = _T("Connected, sending message");
			break;
		case CAPI_SMS_DISCONNECTED:
			bDisconnected = TRUE;
			bConfirm = TRUE;
			sMsg = _T("Disconnected");
			break;
		case CAPI_SMS_REJECTED:
			bDisconnected = TRUE;
			bConfirm = TRUE;
			sMsg = _T("SMSC rejected");
			break;
		case CAPI_SMS_USER_BUSY:
			bDisconnected = TRUE;
			bConfirm = TRUE;
			sMsg = _T("SMSC busy");
			break;
		case CAPI_SMS_NOT_AVAILABLE:
			bDisconnected = TRUE;
			bConfirm = TRUE;
			sMsg = _T("SMSC not available");
			break;
		case CAPI_SMS_SEND_OK:
			sMsg.Format(_T("Send ok %u, waiting for response"), lParam);
			break;
		case CAPI_SMS_SEND_FAILED:
			sMsg.Format(_T("Send failed %u, disconnecting"), lParam);
			break;
		case CAPI_SMS_RESPONSE_OK:
			bConfirm = TRUE;
			m_bProviderResponse = TRUE;
			sMsg = _T("SMS accepted and confirmed");
			break;
		case CAPI_SMS_RESPONSE_ERROR:
			bConfirm = TRUE;
			sMsg.Format(_T("SMS not accepted ERROR: %s"), m_pCapiSMS->GetProviderError());
			break;
	}

	if (   (m_bConfirmed == FALSE)
		&& bConfirm
		)
	{
		if (theApp.m_pCIPCServerControlSMS)
		{
			WK_TRACE(_T("Confirm SMS to Server %i\n"),m_bProviderResponse);
			if (m_bProviderResponse)
			{
				// OK
				theApp.m_pCIPCServerControlSMS->DoConfirmAlarmConnection(_T("SMSInput"),
																		 _T("SMSOutput"),
																		 _T(""),
																		 _T(""),
																		 0
#ifdef _UNICODE
																		 , MAKELONG(CODEPAGE_UNICODE, 0)
#endif
																		 );
			}
			else
			{
				// Error
				theApp.m_pCIPCServerControlSMS->DoIndicateError(sMsg);
			}
			m_bConfirmed = TRUE;
		}
	}
	KillMyTimer();

	if (m_bIsNewCapiSMS && bDisconnected)
	{
		WK_DELETE(m_pCapiSMS);
		m_pCapiWnd->DestroyWindow();
		WK_DELETE(m_pCapiWnd);
		m_bIsNewCapiSMS = FALSE;
	}

	m_wndView.Add(sMsg);
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::PollConnection()
{
	if (m_pCapiSMS)
	{
		m_pCapiSMS->PollConnection();
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::CanSend()
{
	return (   (m_uTimerID == NULL)
			&& (m_bIsNewCapiSMS == FALSE)
			&& (m_pCapiSMS == NULL)
			);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::KillMyTimer()
{
	BOOL bRet = KillTimer(m_uTimerID);
	if (bRet)
	{
		m_uTimerID = 0;
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CMainFrame::OnLanguageChanged(WPARAM wParam, LPARAM lParam)
{
#if _MFC_VER >= 0x0710
	return theApp.SetLanguageParameters((UINT)wParam, (DWORD)lParam);
#else
	return 0;
#endif

}
