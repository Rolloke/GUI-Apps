			m_pIpBook[0]->StartThread();// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "WK_Names.h"
#include "SocketUnit.h"

#include "MainFrm.h"

#ifdef _DTS
#include "IPBook.h"
#endif

#ifdef _BETA
#include "IPNetBook.h"
#include ".\mainfrm.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SYSCOMMAND()
	ON_WM_ASKCBFORMATNAME()
	ON_WM_TIMER()
	ON_MESSAGE(WM_SELFCHECK, DoConfirmSelfcheck)
	ON_MESSAGE(WM_TRAYCLICKED, OnTrayClicked)	
	ON_MESSAGE(WM_LANGUAGE_CHANGED, OnLanguageChanged)	
	ON_COMMAND(ID_VIEW_BITRATE, OnViewBitrate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_BITRATE, OnUpdateViewBitrate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_bIsServer=FALSE;
#ifdef _DTS	
	for(int iN=0;iN<10;iN++)
	{
		m_lLocalIp[iN]=0;
		m_pIpBook[iN]=NULL;
	}
	m_uIpBookTimer = 0;
#endif
	m_iNrOfIP = 0;
	m_uStatisticTimer = 0;
}

/////////////////////////////////////////////////////////////////////////////

CMainFrame::~CMainFrame()
{
}

/////////////////////////////////////////////////////////////////////////////

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	SetWindowText(AfxGetApp()->m_pszAppName);

	GetSystemMenu(FALSE)->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND|MF_GRAYED);
	
	// Icon ins Systemtray
	NOTIFYICONDATA tnd;
	
	CString sTip = COemGuiApi::GetApplicationName(WAI_SOCKET_UNIT);
	
	tnd.cbSize		= sizeof(NOTIFYICONDATA);
	tnd.hWnd		= m_hWnd;
	tnd.uID			= 1;
	
	tnd.uFlags		= NIF_MESSAGE|NIF_ICON|NIF_TIP;
	tnd.uCallbackMessage = WM_TRAYCLICKED;
	tnd.hIcon		= theApp.LoadIcon(IDR_MAINFRAME);
	
	lstrcpyn(tnd.szTip, sTip, sTip.GetLength()+1);
	Shell_NotifyIcon(NIM_ADD, &tnd);

#ifdef _DTS
	CWK_Dongle Dongle;
	m_bIsServer = Dongle.GetProductCode() == IPC_DTS;
	if((Dongle.GetProductCode() == IPC_DTS_IP_RECEIVER)||(Dongle.GetProductCode() == IPC_DTS_RECEIVER))
	{
		GetLocalIPList(m_iNrOfIP);	//Mehrere IP Adressen vorhanden
		WK_TRACE(_T("Lokale IP-Adresseinträge: %i\n"),m_iNrOfIP);

		for(int iN=0; iN<m_iNrOfIP; iN++)
		{ 
			m_pIpBook[iN]=new CIPBook();
			m_pIpBook[iN]->StartThread();
			m_pIpBook[iN]->StartIPBook(inet_ntoa(*(struct in_addr*)&m_lLocalIp[iN]));
			CString sIP = inet_ntoa(*(struct in_addr*)&m_lLocalIp[iN]);
			WK_TRACE(_T("Starte IP-Book auf <%s>\n"), sIP);
		}
	}
	else
	{
		//Ein Sender hat nur eine IP-Adresse
		m_pIpBook[0] = new CIPBook();
		if (m_pIpBook[0])
#ifdef _IPDEBUG
			m_pIpBook[0]->StartThread();
			m_pIpBook[0]->StartIPBook(_T("169.254.95.232"));
			TRACE(_T("ACHTUNG: IP HARDKODIERT AUFGRUND VON PC-KONFIGURATION BEI RAMON!!!!!!!!!!!!!!!!!!!!!!\n"));
#else
			m_pIpBook[0]->StartThread();
			m_pIpBook[0]->StartIPBook();
#endif
	}
	if(!m_bIsServer)
	{
		m_uIpBookTimer = SetTimer(1,5*1000,0);
	}
#endif
	double dSent, dReceived;
	int iSendQueue;
	if (theApp.GetStatistic(dSent, dReceived,iSendQueue))
	{
		CWK_Profile wkp;
		m_DlgStatistic.m_nTimeStep = wkp.GetInt(SOCKET_UNIT_STATISTICS, STAT_TIMER, 1000);
		m_uStatisticTimer = SetTimer(2, m_DlgStatistic.m_nTimeStep, NULL);
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	BOOL ret;
	
	ret = CFrameWnd::PreCreateWindow(cs);

	cs.lpszClass = WK_APP_NAME_SOCKETS;
	cs.style &= ~FWS_ADDTOTITLE;
	return ret;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
LRESULT CMainFrame::DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam)
{
	HWND hWnd = (HWND)wParam;
	
	if (hWnd && IsWindow(hWnd))
	{
		::PostMessage(hWnd, WM_SELFCHECK, WAI_SOCKET_UNIT, 0);
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnDestroy() 
{

#ifdef _DTS
	
	CWK_Dongle Dongle;
	
	if((Dongle.GetProductCode() == IPC_DTS_IP_RECEIVER)||(Dongle.GetProductCode() == IPC_DTS_RECEIVER))
	{
		for(int iN=0; iN<m_iNrOfIP; iN++)
		{
			if(m_pIpBook[iN])
			{
				m_pIpBook[iN]->StopIPBook();
				WK_DELETE(m_pIpBook[iN]);
			}
		}
	}
	else
	{

		if (m_pIpBook[0])
			m_pIpBook[0]->StopIPBook();
			WK_DELETE(m_pIpBook[0]);
	}
#endif
#ifdef _BETA
	if (m_uIpBookTimer)
	{
		KillTimer(m_uIpBookTimer);
		m_uIpBookTimer = 0;
	}

	if (m_pIpBook)
		m_pIpBook->StopIPBook();
	WK_DELETE(m_pIpBook);

#endif

	if (m_uStatisticTimer)
	{
		KillTimer(m_uStatisticTimer);
		m_uStatisticTimer = 0;
	}

	// Nur erforderlich, wenn Icon in der Systemtaskleiste
	NOTIFYICONDATA tnd;

	tnd.cbSize		= sizeof(NOTIFYICONDATA);
	tnd.hWnd		= m_hWnd;
	tnd.uID			= 1;
	tnd.uFlags		= 0;

	Shell_NotifyIcon(NIM_DELETE, &tnd);
	// rke: bis hier wird das Fensterhandle noch gebraucht.
	CFrameWnd::OnDestroy();
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
			pM->InsertMenu(0, MF_SEPARATOR, 0, (LPCTSTR)NULL);
			pM->InsertMenu(1, MF_STRING, ID_APP_ABOUT, (LPCTSTR)sAbout);
			
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
void CMainFrame::OnAskCbFormatName(UINT nMaxCount, LPTSTR lpszString) 
{
	// TODO: Add your message handler code here and/or call default
	
	CFrameWnd::OnAskCbFormatName(nMaxCount, lpszString);
}
/////////////////////////////////////////////////////////////////////////////////////

void CMainFrame::GetLocalIPList(int &iNrOfIP)
{

/*						Eine andere Methode alle lokalen IP-Adressen zu ermitteln
						Hierzu wird aber #include "Iphlpapi.h" und Iphlpapi.lib benötigt
						
		PMIB_IPADDRTABLE pIpAddrTable=NULL;
		ULONG dwSize=0;
	
		if(GetIpAddrTable(pIpAddrTable,&dwSize,FALSE) != NO_ERROR)
		{
			if(dwSize)
			{
				BYTE* pBuffer = new BYTE[dwSize];
				pIpAddrTable = (PMIB_IPADDRTABLE)pBuffer;
				if(GetIpAddrTable(pIpAddrTable,&dwSize,FALSE) == NO_ERROR)
				{
					iNrOfIP = pIpAddrTable->dwNumEntries;
					for(int iN=0; iN<iNrOfIP; iN++)
					{
						m_lLocalIp[iN]=pIpAddrTable->table[iN].dwAddr;
					}
				}
				delete pBuffer;
			}
		}*/
	
		char szHostName[128];
		CWK_String str;
		if( gethostname(szHostName, 128) == 0 )
		{
			// Get host adresses
			struct hostent * pHost;
			int i;
			
			pHost = gethostbyname(szHostName);
			
			for( i=0; pHost!= NULL && (pHost->h_addr_list[i] != NULL); i++ )
			{
				str.Empty();
				int j;
				
				for( j = 0; j < pHost->h_length; j++ )
				{
					CString addr;
					
					if( j > 0 )
						str += _T(".");
					
					addr.Format(_T("%u"), (unsigned int)((unsigned char*)pHost->h_addr_list[i])[j]);
					str += addr;
				}
				m_lLocalIp[i] = inet_addr(str);
			}
			iNrOfIP = i;
		}
}
/////////////////////////////////////////////////////////////////////////////

void CMainFrame::OnTimer(UINT nIDEvent)
{
#ifdef _DTS
	if (m_uIpBookTimer == nIDEvent)
	{
		int iNrOfIP=0;

		GetLocalIPList(iNrOfIP);	//Mehrere IP Adressen vorhanden
		CString sIP;
		BOOL bFound = FALSE;

		if(iNrOfIP==m_iNrOfIP)
		{
			return;					//keine Änderung
		}
		else if(iNrOfIP<m_iNrOfIP)	//Eine IP Adresse ist nicht mehr gültig
		{
			for(int iN=0; iN<m_iNrOfIP; iN++)
			{
				for (int iM=0; iM<iNrOfIP; iM++)
				{
					sIP = inet_ntoa(*(struct in_addr*)&m_lLocalIp[iM]);
					if(m_pIpBook[iN]->GetAdapterIP()== sIP)
					{
						bFound=TRUE;
					}
				}
				if(!bFound)
				{
					WK_DELETE(m_pIpBook[iN]);
				}
				bFound=FALSE;
			}
		}
		else	//Eine IP Adresse ist dazu gekommen
		{
			for(int iN=0; iN<iNrOfIP; iN++)		
			{
				sIP = inet_ntoa(*(struct in_addr*)&m_lLocalIp[iN]);
				for (int iM=0; iM<m_iNrOfIP; iM++)
				{
					if(m_pIpBook[iM]->GetAdapterIP()== sIP)
					{
						bFound=TRUE;
					}
				}
				if(!bFound)
				{
					m_pIpBook[iN]=new CIPBook();
					m_pIpBook[iN]->StartThread();
					m_pIpBook[iN]->StartIPBook(inet_ntoa(*(struct in_addr*)&m_lLocalIp[iN]));
					CString sIP = inet_ntoa(*(struct in_addr*)&m_lLocalIp[iN]);
					WK_TRACE(_T("Starte IP-Book auf <%s>\n"), sIP);
				}
				bFound=FALSE;
			}
		}
		m_iNrOfIP=iNrOfIP;
	}
#endif
	if (m_uStatisticTimer == nIDEvent)
	{
		double dSent, dReceived;
		int iSendQueue = 0;
		if (theApp.GetStatistic(dSent, dReceived,iSendQueue))
		{
			CString sText;
			dSent *= 8.0;		// calculate MBits
			dReceived *= 8.0;
			sText.Format(_T("Sent:%.3f, Received:%.3f MBits/sec %d in Queue"), dSent, dReceived,iSendQueue);
			m_wndStatusBar.SetPaneText(0, sText);
			if (m_DlgStatistic.m_hWnd)
			{
				m_DlgStatistic.AddBitRate(dSent, dReceived,iSendQueue);
			}
		}
	}
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
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnViewBitrate()
{
	if (m_DlgStatistic.m_hWnd)
	{
	}
	else
	{
		m_DlgStatistic.Create(CDlgStatistic::IDD, this);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateViewBitrate(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_uStatisticTimer != 0);
}
