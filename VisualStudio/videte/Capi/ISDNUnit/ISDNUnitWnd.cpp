/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ISDNUnitWnd.cpp $
// ARCHIVE:		$Archive: /Project/Capi/ISDNUnit/ISDNUnitWnd.cpp $
// CHECKIN:		$Date: 3.03.06 11:46 $
// VERSION:		$Revision: 56 $
// LAST CHANGE:	$Modtime: 3.03.06 11:45 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#include "stdafx.h"
#include "resource.h"
#include "ISDNUnit.h"

#include "IsdnUnitWnd.h"
#include ".\isdnunitwnd.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIsdnUnitWnd construction/destruction
CIsdnUnitWnd::CIsdnUnitWnd()
{
	CString sAppName;
	CString sModuleName;
	// NOT YET BOOL bSecondInstance = MyGetApp()->IsSecondInstance();
	sAppName = GetAppnameFromId(theApp.GetApplicationId());
	sModuleName = sAppName;

	/*
  DWORD dwExStyle,      // extended window style
  LPCTSTR lpClassName,  // pointer to registered class name
  LPCTSTR lpWindowName, // pointer to window name
  DWORD dwStyle,        // window style
  int x,                // horizontal position of window
  int y,                // vertical position of window
  int nWidth,           // window width  
  int nHeight,          // window height
  HWND hWndParent,      // handle to parent or owner window
  HMENU hMenu,          // handle to menu, or child-window identifier
  HINSTANCE hInstance,  // handle to application instance
  LPVOID lpParam        // pointer to window-creation data
	*/
	CreateEx(WS_EX_OVERLAPPEDWINDOW,
			 sAppName,
			 sModuleName,
			 WS_OVERLAPPEDWINDOW,
			 CW_USEDEFAULT, CW_USEDEFAULT,	// x, y
             CW_USEDEFAULT, CW_USEDEFAULT,	// width, height
             NULL, NULL, NULL);

	// Routine fuer Icon in der Taskleiste
	m_bFirstIconUpdate = TRUE;
	m_pdbdExDevice = NULL;
	UpdateIcon(TRUE);
}
/////////////////////////////////////////////////////////////////////////////
CIsdnUnitWnd::~CIsdnUnitWnd()
{
	WK_DELETE(m_pdbdExDevice);
}
/////////////////////////////////////////////////////////////////////////////
// CIsdnUnitWnd diagnostics
#ifdef _DEBUG
void CIsdnUnitWnd::AssertValid() const
{
	CWnd::AssertValid();
}

void CIsdnUnitWnd::Dump(CDumpContext& dc) const
{
	CWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CIsdnUnitWnd, CWnd)
	//{{AFX_MSG_MAP(CIsdnUnitWnd)
	ON_WM_DESTROY()
	ON_COMMAND(ID_ABOUT, OnAbout)
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
	ON_COMMAND(ID_CLOSE_ALL_ISDN_CONNECTIONS, OnCloseAllIsdnConnections)
	ON_UPDATE_COMMAND_UI(ID_CLOSE_ALL_ISDN_CONNECTIONS, OnUpdateCloseAllIsdnConnections)
	ON_COMMAND(ID_CLOSE_ONE_ISDN_CHANNEL, OnCloseOneIsdnChannel)
	ON_UPDATE_COMMAND_UI(ID_CLOSE_ONE_ISDN_CHANNEL, OnUpdateCloseOneIsdnChannel)
	ON_COMMAND(ID_CLOSE_MODUL, OnCloseModul)
	ON_WM_CLOSE()
	ON_COMMAND(ID_SHOW_STATE, OnShowState)
	ON_WM_QUERYOPEN()
	ON_WM_SYSCOMMAND()
	ON_WM_DEVMODECHANGE()
	ON_WM_DEVICECHANGE()
	ON_COMMAND(ID_HELP_FINDER, CWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, CWnd::OnHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CWnd::OnHelpFinder)
	ON_MESSAGE(WM_SELFCHECK, DoConfirmSelfcheck)
	ON_MESSAGE(WM_LANGUAGE_CHANGED, OnLanguageChanged)	
	ON_MESSAGE(WM_ISDN_B_CHANNELS, OnISDN_B_Channels)	
	ON_MESSAGE(ISDNUNITWND_CAPI_GET_MESSAGE_ERROR, OnCapiGetMessageError)
	ON_MESSAGE(ISDNUNITWND_UPDATE_ICON, OnUpdateIcon)
	ON_COMMAND(ID_OPEN_SECOND_B_CHANNEL, OnOpenSecondBChannel)
	ON_UPDATE_COMMAND_UI(ID_OPEN_SECOND_B_CHANNEL, OnUpdateOpenSecondBChannel)
	//}}AFX_MSG_MAP
	// Global help commands
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DefWindowProc wird nur benoetigt fuer Icon in der Taskleiste
LRESULT CIsdnUnitWnd::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (message==WM_USER)
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
				COemGuiApi::DoUpdatePopupMenu(AfxGetMainWnd(), pM);

				SetForegroundWindow();		// Siehe ID: Q135788 
				pM->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this);
				PostMessage(WM_NULL, 0, 0); // Siehe ID: Q135788
			}
			break;
		}
	}
	
	return CWnd::DefWindowProc(message, wParam, lParam);
}
/////////////////////////////////////////////////////////////////////////////
void CIsdnUnitWnd::OnAbout() 
{
	COemGuiApi::AboutDialog(this);
}
/////////////////////////////////////////////////////////////////////////////
// Disabled because all Settings in SystemVerwaltung
/*
void CIsdnUnitWnd::OnSettings() 
{
	if (Login())
	{
		theApp.ShowSettingsDlg();
	}
}
*/
/////////////////////////////////////////////////////////////////////////////
// Disabled because only used by OnSettings
/*
BOOL CIsdnUnitWnd::Login()
{
	CString sUser;
	CString sPassword;
	CSecID  idUser;
	CUserArray UserArray;
	BOOL bRet = FALSE;			 
	CWK_Profile prof;

	COEMLoginDialog dlg(this);
										   
	if (IDOK==dlg.DoModal())	  		 
	{									 
		bRet = TRUE;		 
	} 			  

	return bRet;
}
*/
void CIsdnUnitWnd::OnOpenSecondBChannel()
{
	CISDNConnection* pConnection = theApp.GetISDNConnection();
	if (   pConnection
		&& pConnection->GetNumChannels()  == 2
		&& pConnection->GetConnectState() == CONNECT_OPEN
		&& pConnection->GetNumBChannels() == 1)
	{
		pConnection->OpenSecondChannel();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIsdnUnitWnd::OnUpdateOpenSecondBChannel(CCmdUI *pCmdUI)
{
	BOOL bEnable = FALSE;
	CISDNConnection* pConnection = theApp.GetISDNConnection();
	if (pConnection)
	{
		if (   pConnection->GetConnectState() == CONNECT_OPEN
			&& pConnection->GetNumChannels()  == 2
			&& pConnection->GetNumBChannels() == 1)
		{
			bEnable = TRUE;
		}
	}
	else
	{
		// no trace because GUI update!
	}
	pCmdUI->Enable(bEnable);
}
/////////////////////////////////////////////////////////////////////////////
void CIsdnUnitWnd::OnCloseOneIsdnChannel()
{
	CISDNConnection* pConnection = theApp.GetISDNConnection();
	if (   pConnection
		&& pConnection->GetConnectState() == CONNECT_OPEN
		&& pConnection->GetNumBChannels() == 2)
	{
		pConnection->CloseOneChannel();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIsdnUnitWnd::OnUpdateCloseOneIsdnChannel(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	CISDNConnection* pConnection = theApp.GetISDNConnection();
	if (pConnection)
	{
		if (   pConnection->GetConnectState() == CONNECT_OPEN
			&& pConnection->GetNumBChannels() == 2)
		{
			bEnable = TRUE;
		}
	}
	else
	{
		// no trace because GUI update!
	}
	pCmdUI->Enable(bEnable);
}
/////////////////////////////////////////////////////////////////////////////
void CIsdnUnitWnd::OnCloseAllIsdnConnections() 
{
	if ( !theApp.CloseAllIsdnConnections(FALSE) )
	{
		CString sMsg;
		sMsg.LoadString(IDS_ISDN_NOT_CLOSED);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIsdnUnitWnd::OnUpdateCloseAllIsdnConnections(CCmdUI* pCmdUI) 
{
	BOOL bEnable = FALSE;
	CISDNConnection* pConnection = theApp.GetISDNConnection();
	if (pConnection)
	{
		if (pConnection->GetConnectState() == CONNECT_OPEN)
		{
			bEnable = TRUE;
		}
	}
	else
	{
		// no trace because GUI update!
	}
	pCmdUI->Enable(bEnable);
}
/////////////////////////////////////////////////////////////////////////////
void CIsdnUnitWnd::OnCloseModul() 
{
	theApp.CloseModule();
}
/////////////////////////////////////////////////////////////////////////////
void CIsdnUnitWnd::OnClose() 
{
	theApp.CloseModule();
}
/////////////////////////////////////////////////////////////////////////////
void CIsdnUnitWnd::OnAppExit() 
{
	theApp.CloseModule();
}
/////////////////////////////////////////////////////////////////////////////
void CIsdnUnitWnd::OnDestroy() 
{
	// Nur erforderlich, wenn Icon in der Systemtaskleiste
	NOTIFYICONDATA tnd;

	tnd.cbSize		= sizeof(NOTIFYICONDATA);
	tnd.hWnd		= m_hWnd;
	tnd.uID			= 1;
	tnd.uFlags		= 0;

	Shell_NotifyIcon(NIM_DELETE, &tnd);

	CWnd::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
void CIsdnUnitWnd::PostNcDestroy() 
{
	WK_DELETE(m_pdbdExDevice);
	delete this;
}
/////////////////////////////////////////////////////////////////////////////
void CIsdnUnitWnd::OnShowState() 
{
	theApp.ShowAboutDlg();
}
/////////////////////////////////////////////////////////////////////////////
long CIsdnUnitWnd::DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam)
{
	HWND hWnd = (HWND)wParam;

	if (hWnd && IsWindow(hWnd))
	{
		::PostMessage(hWnd, WM_SELFCHECK, theApp.GetApplicationId(), 0);
	}

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CIsdnUnitWnd::OnLanguageChanged(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = theApp.SetLanguageParameters((UINT)wParam, (DWORD)lParam);
	OnCapiGetMessageError(0, 0);
	return lResult;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CIsdnUnitWnd::OnISDN_B_Channels(WPARAM wParam, LPARAM lParam)
{
	if (wParam)
	{
		CISDNConnection* pConnection = theApp.GetISDNConnection();
		if (   pConnection
			&& pConnection->GetConnectState() == CONNECT_OPEN
			&& pConnection->GetNumChannels() == 2
			&& pConnection->GetNumBChannels() == 1)
		{
			pConnection->OpenSecondChannel();
		}
	}
	else
	{
		CISDNConnection* pConnection = theApp.GetISDNConnection();
		if (pConnection)
		{
			pConnection->CheckCloseOneChannel();
		}
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
long CIsdnUnitWnd::OnUpdateIcon(WPARAM wParam, LPARAM lParam)
{
	UpdateIcon((BOOL)wParam);

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CIsdnUnitWnd::UpdateIcon(BOOL bEnabled)
{
	NOTIFYICONDATA tnd;

	HICON hIcon = NULL;
	if (bEnabled)
	{
		if (theApp.RunAsPTUnit())
		{
			hIcon = theApp.LoadIcon(IDR_MAINFRAME_PT);
		}
		else
		{
			hIcon = theApp.LoadIcon(IDR_MAINFRAME);
		}
	}
	else
	{
		if (theApp.RunAsPTUnit())
		{
			hIcon = theApp.LoadIcon(IDR_MAINFRAME_PT_NO_DEVICE);
		}
		else
		{
			hIcon = theApp.LoadIcon(IDR_MAINFRAME_NO_DEVICE);
		}
	}

	CString sTip = COemGuiApi::GetApplicationName(theApp.GetApplicationId());
	if (sTip.IsEmpty()) 
	{
		if (theApp.RunAsPTUnit())
		{
			sTip = _T("\"PTUnit\"");
		}
		else
		{
			sTip = _T("\"ISDNUnit\"");
		}
	}

	if (bEnabled == FALSE)
	{
		CString sNoDevice;
		sNoDevice.LoadString(IDS_NO_DEVICE_FOUND);
		sTip += sNoDevice;
	}
	else
	{
		CISDNConnection* pConnection = theApp.GetISDNConnection();
		if (pConnection
			&& pConnection->GetConnectState() == CONNECT_OPEN)
		{
			CString sMsg;
			sMsg.Format(_T(" %dB -> %s"), pConnection->GetNumBChannels(), pConnection->GetRemoteNumber());
			sTip += sMsg;
		}
	}

	tnd.cbSize		= sizeof(NOTIFYICONDATA);
	tnd.hWnd		= m_hWnd;
	tnd.uID			= 1;

	tnd.uFlags		= NIF_MESSAGE|NIF_ICON|NIF_TIP;
	tnd.uCallbackMessage = WM_USER;
	tnd.hIcon		= hIcon;

	lstrcpyn(tnd.szTip, sTip, sTip.GetLength()+1);
	
	if (m_bFirstIconUpdate)
	{
		Shell_NotifyIcon(NIM_ADD, &tnd);
		m_bFirstIconUpdate = FALSE;
	}
	else
	{
		Shell_NotifyIcon(NIM_MODIFY, &tnd);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIsdnUnitWnd::OnDevModeChange(LPTSTR lpDeviceName) 
{
	CWnd::OnDevModeChange(lpDeviceName);
	
	WK_TRACE(_T("OnDevModeChange %s\n"), lpDeviceName);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CIsdnUnitWnd::OnDeviceChange(UINT nEventType, DWORD dwData)
{
	BOOL bRet = TRUE;
	DEV_BROADCAST_HDR* pHdr = (DEV_BROADCAST_HDR*)dwData;
	if (pHdr)
	{
		if (pHdr->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE_EX)
		{
			CDevIntEx dbdEx((PDEV_BROADCAST_DEVICEINTERFACE_EX)pHdr);

			CGuid		guidClass		= dbdEx.GetGuidClass();
			CGuid		guidDevClass	= dbdEx.GetGuidDevClass();
			CString	sClassGUID		= guidClass.GetString();
			CString	sDevClassGUID	= guidDevClass.GetString();
			CString	sDevice			= dbdEx.GetDeviceName();
			CString	sClass			= dbdEx.GetClass();
			CString	sManufacturer	= dbdEx.GetManufacturer();
			CString	sService		= dbdEx.GetService();

			WK_TRACE(_T("Device Changed\n"));
			WK_TRACE(_T("\t ClassGUID    %s\n"), sClassGUID);
			WK_TRACE(_T("\t DevClassGUID %s\n"), sDevClassGUID);
			WK_TRACE(_T("\t Device       %s\n"), sDevice);
			WK_TRACE(_T("\t Class        %s\n"), sClass);
			WK_TRACE(_T("\t Manufacturer %s\n"), sManufacturer);
			WK_TRACE(_T("\t Service      %s\n"), sService);

			// OOPS gf
			// Which device classes should be watched
			// which are valid for ISDN connections
			// only "Network" devices?
			if (guidDevClass == GUID_DEVCLASS_NETWORK)
			{
				if (nEventType == DBT_DEVICEARRIVAL)
				{
					WK_TRACE(_T("Device arrived\n"));
					if (theApp.IsDeviceFound() == FALSE)
					{
						if (theApp.InitCapiThread())
						{
							// device found, store its info
							WK_DELETE(m_pdbdExDevice);
							m_pdbdExDevice = new CDevIntEx(dbdEx.GetData());
						}
					}
					else
					{
						WK_TRACE_WARNING(_T("Device arrived while already found\n"));
					}
				}
				else if (nEventType == DBT_DEVICEREMOVECOMPLETE)
				{
					WK_TRACE(_T("Device removed\n"));
					if (theApp.IsDeviceFound())
					{
						if (m_pdbdExDevice)
						{
							if (dbdEx == m_pdbdExDevice)
							{
								if (theApp.StopCapiThread())
								{
									WK_DELETE(m_pdbdExDevice);
									// maybe there is another device available?
									if (theApp.InitCapiThread())
									{
										// device found, but unknown, which one
									}
								}
							}
							else
							{
								// not the device we found before, so ignore it
							}
						}
						else
						{
							// OOPS gf
							// What, if the device was connected at start of unit,
							// which means m_pdbdExDevice == NULL, because we did not got any info
							// and is removed now?
							// Stop and re-init CapiThread to check for availability?
						}
					}
					else
					{
						WK_TRACE_WARNING(_T("Device removed while no one found before\n"));
					}
				}
				else
				{
					CString s;
					switch (nEventType)
					{
						case DBT_DEVNODES_CHANGED:				s = _T("DBT_DEVNODES_CHANGED");			break;
						case DBT_CONFIGCHANGECANCELED:		s = _T("DBT_CONFIGCHANGECANCELED");		break;
						case DBT_CONFIGCHANGED:					s = _T("DBT_CONFIGCHANGED");				break;
//						case DBT_CUSTOMEVENT:					s = _T("DBT_CUSTOMEVENT");					break;
						case DBT_DEVICEARRIVAL:					s = _T("DBT_DEVICEARRIVAL");				break;
						case DBT_DEVICEQUERYREMOVE:			s = _T("DBT_DEVICEQUERYREMOVE");			break;
						case DBT_DEVICEQUERYREMOVEFAILED:	s = _T("DBT_DEVICEQUERYREMOVEFAILED");	break;
						case DBT_DEVICEREMOVECOMPLETE:		s = _T("DBT_DEVICEREMOVECOMPLETE");		break;
						case DBT_DEVICEREMOVEPENDING:			s = _T("DBT_DEVICEREMOVEPENDING");		break;
						case DBT_DEVICETYPESPECIFIC:			s = _T("DBT_DEVICETYPESPECIFIC");			break;
						case DBT_QUERYCHANGECONFIG:			s = _T("DBT_QUERYCHANGECONFIG");			break;
						case DBT_USERDEFINED:					s = _T("DBT_USERDEFINED");					break;
					}
					WK_TRACE_WARNING(_T("Device unknown event type %s\n"), s);
				}
			}
			else
			{
					WK_TRACE(_T("Device type not valid, DevClassGUID %s\n"), sDevClassGUID);
			}
		}
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
long CIsdnUnitWnd::OnCapiGetMessageError(WPARAM wParam, LPARAM lParam)
{
	// something gone really wrong, reinit Capi-Thread
	theApp.StopCapiThread();
	theApp.InitCapiThread();
	return 0;
}
