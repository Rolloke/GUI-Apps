// SocketPage.cpp : implementation file
//

#include "stdafx.h"
#include <direct.h>
#include "systemverwaltung.h"
#include "SocketPage.h"

#include "SVDoc.h"
#include "SVView.h"

#include "WKClasses/WK_Exec.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static TCHAR BASED_CODE szSocket[] = _T("SocketUnit");
/////////////////////////////////////////////////////////////////////////////
// CSocketPage dialog



CSocketPage::CSocketPage(CSVView* pParent) : CSVPage(CSocketPage::IDD)
{
	m_pParent = pParent;
	//{{AFX_DATA_INIT(CSocketPage)
	m_iBitrate = 1;
	m_bUseHostList = FALSE;
	//}}AFX_DATA_INIT
	m_bSocketUnit = FALSE;
	m_bUseDNS = FALSE;
	m_bDoCacheUnknownNames = TRUE;
	m_bDoCacheKnownNames = TRUE;
	m_bDoBeepOnIncoming = TRUE;
	m_bDoListen = TRUE;
	m_bAllowOutgoingCalls = TRUE;
	m_dwSyncRetryTimeSeconds = 1;
	m_bLoadingHostList = FALSE;

	//}}AFX_DATA_INIT
	Create(IDD,pParent);
}
void CSocketPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSocketPage)
	DDX_Control(pDX, IDC_USE_IPHOSTLIST, m_btnUseIPHostList);
	DDX_Control(pDX, IDC_EDIT_IPHOSTLIST, m_EditHostList);
	DDX_Control(pDX, IDC_TCPIP_TEST, m_btnTestTCP);
	DDX_Control(pDX, IDC_TXT_TCPIP_TEST_RESULT, m_ctlTestResult);
	DDX_Control(pDX, IDC_SYNC_RETRY_TIME, m_editSync);
	DDX_Control(pDX, IDC_RADIO_64_KB, m_btn64);
	DDX_Control(pDX, IDC_RADIO_128_KB, m_btn128);
	DDX_Control(pDX, IDC_RADIO_256_KB, m_btn256);
	DDX_Control(pDX, IDC_RADIO_512_KB, m_btn512);
	DDX_Control(pDX, IDC_RADIO_1MBIT, m_btn1MBit);
	DDX_Control(pDX, IDC_RADIO_2MBIT, m_btn2MBit);
	DDX_Control(pDX, IDC_RADIO_4MBIT, m_btn4MBit);
	DDX_Control(pDX, IDC_RADIO_8MBIT, m_btn8MBit);
	DDX_Control(pDX, IDC_USE_DNS, m_btnUseDNS);
	DDX_Control(pDX, IDC_DO_LISTEN, m_btnDoListen);
	DDX_Control(pDX, IDC_DO_CACHE_UNKNOWN_NAMES, m_btnDoCacheUnknownNames);
	DDX_Control(pDX, IDC_DO_CACHE_KNOWN_NAMES, m_btnDoCacheKnownNames);
	DDX_Control(pDX, IDC_DO_BEEP_ON_INCOMING, m_btnDoBeepOnIncoming);
	DDX_Control(pDX, IDC_ALLOW_OUTGOING_CALLS, m_btnAllowOutgoingCalls);
	DDX_Check(pDX, IDC_CHECK_SOCKET, m_bSocketUnit);
	DDX_Check(pDX, IDC_USE_DNS, m_bUseDNS);
	DDX_Check(pDX, IDC_DO_CACHE_UNKNOWN_NAMES, m_bDoCacheUnknownNames);
	DDX_Check(pDX, IDC_DO_CACHE_KNOWN_NAMES, m_bDoCacheKnownNames);
	DDX_Check(pDX, IDC_DO_BEEP_ON_INCOMING, m_bDoBeepOnIncoming);
	DDX_Check(pDX, IDC_DO_LISTEN, m_bDoListen);
	DDX_Check(pDX, IDC_ALLOW_OUTGOING_CALLS, m_bAllowOutgoingCalls);
	DDX_Text(pDX, IDC_SYNC_RETRY_TIME, m_dwSyncRetryTimeSeconds);
	DDX_Radio(pDX, IDC_RADIO_8MBIT, m_iBitrate);
	DDX_Check(pDX, IDC_USE_IPHOSTLIST, m_bUseHostList);
	//}}AFX_DATA_MAP

}


BEGIN_MESSAGE_MAP(CSocketPage, CSVPage)
	//{{AFX_MSG_MAP(CSocketPage)
	ON_BN_CLICKED(IDC_CHECK_SOCKET, OnCheckSocket)
	ON_BN_CLICKED(IDC_RADIO_64_KB, OnChange)
	ON_BN_CLICKED(IDC_TCPIP_TEST, OnTcpipTest)
	ON_BN_CLICKED(IDC_USE_IPHOSTLIST, OnUseIPHostList)
	ON_EN_CHANGE(IDC_EDIT_IPHOSTLIST, OnChangeEditIphostlist)
	ON_BN_CLICKED(IDC_RADIO_512_KB, OnChange)
	ON_BN_CLICKED(IDC_RADIO_256_KB, OnChange)
	ON_BN_CLICKED(IDC_RADIO_128_KB, OnChange)
	ON_BN_CLICKED(IDC_RADIO_1MBIT, OnChange)
	ON_BN_CLICKED(IDC_RADIO_2MBIT, OnChange)
	ON_BN_CLICKED(IDC_RADIO_4MBIT, OnChange)
	ON_BN_CLICKED(IDC_RADIO_8MBIT, OnChange)
	ON_BN_CLICKED(IDC_ALLOW_OUTGOING_CALLS, OnChange)
	ON_BN_CLICKED(IDC_DO_BEEP_ON_INCOMING, OnChange)
	ON_BN_CLICKED(IDC_DO_CACHE_KNOWN_NAMES, OnChange)
	ON_BN_CLICKED(IDC_DO_CACHE_UNKNOWN_NAMES, OnChange)
	ON_BN_CLICKED(IDC_DO_LISTEN, OnChange)
	ON_EN_CHANGE(IDC_SYNC_RETRY_TIME, OnChange)
	ON_BN_CLICKED(IDC_USE_DNS, OnChange)
	ON_BN_CLICKED(IDC_COPY_TCP_LIST, OnCopyTcpToList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSocketPage message handlers

BOOL CSocketPage::IsDataValid()
{
	if (!CheckIPList())
	{
		CString sErr;
		sErr.LoadString(IDP_ERROR_IN_IPLIST);
		AfxMessageBox(sErr,MB_ICONSTOP);
		return FALSE;
	}
	else
		return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CSocketPage::SaveChanges()
{
	WK_TRACE_USER(_T("TCP/IP-Modul-Einstellungen wurden geändert\n"));
	UpdateData();
	SaveHostListToRegistry();

	GetProfile().WriteString(theApp.GetModuleSection(),WK_APP_NAME_SOCKETS,m_bSocketUnit ? _T("SocketUnit.exe") : _T(""));

	GetProfile().WriteInt(szSocket,_T("UseDNS"),m_bUseDNS);
	GetProfile().WriteInt(szSocket,_T("CacheKnownNames"),m_bDoCacheKnownNames);
	GetProfile().WriteInt(szSocket,_T("CacheUnknownNames"),m_bDoCacheUnknownNames);

	GetProfile().WriteInt(szSocket,_T("BeepOnIncoming"),m_bDoBeepOnIncoming);
	GetProfile().WriteInt(szSocket,_T("DoListen"),m_bDoListen);
	GetProfile().WriteInt(szSocket,_T("AllowOutgoingCalls"),m_bAllowOutgoingCalls);
	GetProfile().WriteInt(szSocket,_T("SyncRetryTimeMS"),m_dwSyncRetryTimeSeconds*1000);

	GetProfile().WriteInt(szSocket,_T("UseHostList"), m_bUseHostList);

	int iSelectedBitrate = 256;
	switch (m_iBitrate)
	{
	case 0:
		iSelectedBitrate = 8192;
		break;
	case 1:
		iSelectedBitrate = 4096;
		break;
	case 2:
		iSelectedBitrate = 2048;
		break;
	case 3:
		iSelectedBitrate = 1024;
		break;
	case 4:
		iSelectedBitrate = 512;
		break;
	case 5:
		iSelectedBitrate = 256;
		break;
	case 6:
		iSelectedBitrate = 128;
		break;
	case 7:
		iSelectedBitrate = 64;
		break;
	}
	GetProfile().WriteInt(szSocket,_T("DefaultBitrate"),iSelectedBitrate);
}
/////////////////////////////////////////////////////////////////////////////
void CSocketPage::CancelChanges()
{
	CString sTemp;

	m_bUseDNS = GetProfile().GetInt(szSocket,_T("UseDNS"),FALSE);
	m_bDoCacheKnownNames = GetProfile().GetInt(szSocket,_T("CacheKnownNames"),TRUE);
	m_bDoCacheUnknownNames= GetProfile().GetInt(szSocket,_T("CacheUnknownNames"),TRUE);

	m_bUseHostList = GetProfile().GetInt(szSocket,_T("UseHostList"), FALSE);
	LoadHostListFromRegistry();

	sTemp = GetProfile().GetString(theApp.GetModuleSection(),WK_APP_NAME_SOCKETS,_T(""));
	m_bSocketUnit = sTemp.CompareNoCase(_T("SocketUnit.exe"))==0; 

	m_bDoBeepOnIncoming = GetProfile().GetInt(szSocket,_T("BeepOnIncoming"),TRUE);
	m_bDoListen = GetProfile().GetInt(szSocket,_T("DoListen"),TRUE);
	m_bAllowOutgoingCalls = GetProfile().GetInt(szSocket,_T("AllowOutgoingCalls"),TRUE);
	m_dwSyncRetryTimeSeconds = GetProfile().GetInt(szSocket,_T("SyncRetryTimeMS"),30000)/1000;
	int iSelectedBitrate = GetProfile().GetInt(szSocket,_T("DefaultBitrate"),256);

	m_iBitrate = 1;
	switch (iSelectedBitrate)
	{
	case 8192:
		m_iBitrate = 0;
		break;
	case 4096:
		m_iBitrate = 1;
		break;
	case 2048:
		m_iBitrate = 2;
		break;
	case 1024:
		m_iBitrate = 3;
		break;
	case 512:
		m_iBitrate = 4;
		break;
	case 256:
		m_iBitrate = 5;
		break;
	case 128:
		m_iBitrate = 6;
		break;
	case 64:
		m_iBitrate = 7;
		break;
	}
	
	UpdateData(FALSE);
	ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
void CSocketPage::OnCheckSocket() 
{
	SetModified();
	ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSocketPage::OnInitDialog() 
{
	CSVPage::OnInitDialog();

	// Einen nicht proportionalen Font wählen
	m_fixedFont.CreatePointFont(10*10, GetFixFontFaceName());
	m_EditHostList.SetFont(&m_fixedFont, TRUE);
 	m_EditHostList.SetEventMask(ENM_CHANGE);

	CancelChanges();
	ShowHide();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CSocketPage::ShowHide()
{
	UpdateData();

	m_btnAllowOutgoingCalls.EnableWindow(m_bSocketUnit);
	m_btnDoListen.EnableWindow(m_bSocketUnit);
	m_btnDoBeepOnIncoming.EnableWindow(m_bSocketUnit);
	
	m_btnUseDNS.EnableWindow(m_bSocketUnit);
	m_btnDoCacheUnknownNames.EnableWindow(m_bSocketUnit && m_bUseDNS);
	m_btnDoCacheKnownNames.EnableWindow(m_bSocketUnit && m_bUseDNS);

	m_btn64.EnableWindow(m_bSocketUnit);
	m_btn128.EnableWindow(m_bSocketUnit);
	m_btn256.EnableWindow(m_bSocketUnit);
	m_btn512.EnableWindow(m_bSocketUnit);
	m_btn1MBit.EnableWindow(m_bSocketUnit);
	m_btn2MBit.EnableWindow(m_bSocketUnit);
	m_btn4MBit.EnableWindow(m_bSocketUnit);
	m_btn8MBit.EnableWindow(m_bSocketUnit);

	m_editSync.EnableWindow(m_bSocketUnit);

	// Remote keinen TCP-Test erlauben
	m_btnTestTCP.EnableWindow(IsLocal());

	m_btnUseIPHostList.EnableWindow(m_bSocketUnit);
	m_EditHostList.EnableWindow(m_bUseHostList && m_bSocketUnit);

	if (m_bUseHostList && m_bSocketUnit)
		m_EditHostList.SetBackgroundColor(FALSE, RGB(255, 255, 255));
	else
		m_EditHostList.SetBackgroundColor(FALSE, RGB(192, 192, 192));

}

/////////////////////////////////////////////////////////////////////////////
void CSocketPage::OnChange() 
{
	if (m_bLoadingHostList)
	{
		SetModified(TRUE,FALSE);
		ShowHide();
	}
	m_pParent->AddInitApp(WK_APP_NAME_SOCKETS);
}

void CSocketPage::OnChangeEditIphostlist() 
{
	if (m_bLoadingHostList)
	{
		long nStartChar, nEndChar;
		m_EditHostList.GetSel(nStartChar, nEndChar);
		CheckLine(m_EditHostList.LineFromChar(nStartChar));
		SetModified(TRUE);
		ShowHide();
	}
	else
		CheckIPList();
}

void CSocketPage::OnTcpipTest() 
{
	CString sPingCommand(_T("ping.exe"));
	CString sLog = GetLogPath();
	CString sLogFile = sLog + (_T("\\Ping.log"));

	// system(_T("ping -n 100 127.0.0.1 >C:\\log\\ping.log"));
	// first remove possible old
	if (DoesFileExist(sLogFile)) 
	{
		BOOL bDeleted = DeleteFile(sLogFile);
		if (bDeleted) 
		{
			// fine
		}
		else
		{
			WK_TRACE_ERROR(_T("Could not delete '%s'\n"),(LPCTSTR)sLogFile);
		}
	}

	CString sCmd;
	sCmd.Format(_T("/C%s -n 1 127.0.0.1 >%s"),sPingCommand, sLogFile);

	WK_Exec (
		_T("command.com"),
		sCmd,
		sLog,
		TRUE,
		SW_SHOWMINNOACTIVE | SW_HIDE
		);
	
	// NOT YET check for C:\Log
	FILE *fp = _tfopen(sLogFile,_T("r"));

	if (fp==NULL) 
	{
		// NOT YET more detailed message
		CString sMsg;
		sMsg.LoadString(IDS_CANT_DO_IPTEST);
		m_ctlTestResult.SetWindowText(sMsg);
	}
	else 
	{
		BOOL bOkay = FALSE;
		char szOneLine[200];
		int iNumLines=0;

		while( fp && !feof( fp) ) 
		{
			if( fgets( szOneLine, sizeof(szOneLine)-1, fp) != NULL ) 
			{
				iNumLines++;
				CString sTmp = szOneLine;
				if (sTmp.Find(_T("Antwort von")) != -1
					|| sTmp.Find(_T("Reply from")) != -1) 
				{
					bOkay = TRUE;
				}
			}
		}
		fclose(fp);

		// analyze the result
		if (bOkay) 
		{
			CString sMsg;
			sMsg.LoadString(IDS_IPTEST_SUCCUESS);
			m_ctlTestResult.SetWindowText(sMsg);
		}
		else
		{
			if (iNumLines)
			{
				CString sMsg;
				sMsg.LoadString(IDS_IPTEST_FAILED);
				m_ctlTestResult.SetWindowText(sMsg);
			}
			else
			{
				// ping nicht gefunden, kann ein path Problem sein
				CString sMsg;
				sMsg.LoadString(IDS_CANT_START_PING);
				m_ctlTestResult.SetWindowText(sMsg);
			}
		}
	}	// end of ping.log found
}

/////////////////////////////////////////////////////////////////////////////////
BOOL CSocketPage::CheckIPList()
{
	BOOL	bRet = TRUE;

	if (m_bUseHostList) // && (m_EditHostList.GetTextLength() != 0))
	{
		for (int nI = 0; nI < m_EditHostList.GetLineCount(); nI++)
		{
			if (!CheckLine(nI))
			{
				bRet = FALSE;
				break;
			}
		}
	}
	return bRet;
}

/////////////////////////////////////////////////////////////////////////////////
BOOL CSocketPage::CheckLine(int nLine)
{
	TCHAR Buffer[1024] = {0};
	BOOL	bRet = TRUE;

	int nCnt = m_EditHostList.GetLine(nLine, Buffer, 1024);

	if (nCnt >= 3) // Mindestens ein Zeichen +CrLf
	{
		Buffer[nCnt-2] = 0;
		bRet = CheckIPSyntax(Buffer);

		if (bRet)
			SetSelection(nLine, RGB(0, 0, 255));
		else
			SetSelection(nLine, RGB(255, 0, 0));
	}

	return bRet;
}

/////////////////////////////////////////////////////////////////////////////////
void CSocketPage::SetSelection(int nLine, COLORREF colRGB)
{
	CHARFORMAT chfmt;
	long nStartChar, nEndChar;

	memset(&chfmt, 0, sizeof(chfmt));
	m_EditHostList.GetSel(nStartChar, nEndChar);
   	m_EditHostList.SetSel(m_EditHostList.LineIndex(nLine), m_EditHostList.LineIndex(nLine+1));

	chfmt.cbSize		= sizeof(chfmt);
	chfmt.dwMask		= CFM_COLOR;
	chfmt.crTextColor 	= colRGB;
	
	m_EditHostList.SetSelectionCharFormat(chfmt);
   	m_EditHostList.SetSel(nStartChar, nEndChar);
}

/////////////////////////////////////////////////////////////////////////////////
BOOL CSocketPage::SaveHostListToRegistry()
{
	TCHAR Buffer[1024] = {0};
	CString sLineNr;

	if (!m_bUseHostList)
		return TRUE;

	GetProfile().DeleteSection(_T("SocketUnit\\HostList"));

	for (int nI = 0; nI < m_EditHostList.GetLineCount(); nI++)
	{
		memset(Buffer, 0, 1024);
		int nCnt = m_EditHostList.GetLine(nI, Buffer, 1024);
		if (nCnt >= 2)
			Buffer[nCnt-2] = 0;
		sLineNr.Format(_T("%05d"), nI);
		GetProfile().WriteString(_T("SocketUnit\\HostList"), sLineNr, Buffer);
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////
BOOL CSocketPage::LoadHostListFromRegistry()
{
	CString sLineNr;
	CString sHost;
	int nI = 0;

   	m_EditHostList.SetSel(0, -1);

	BOOL bLoop = TRUE;
	
	while (bLoop)
	{
		sLineNr.Format(_T("%05d"), nI++);
		sHost = GetProfile().GetString(_T("SocketUnit\\HostList"), sLineNr, _T(""));

		sLineNr.Format(_T("%05d"), nI);
		if (GetProfile().GetString(_T("SocketUnit\\HostList"), sLineNr, _T("<<EndeDerListe>>")) != _T("<<EndeDerListe>>"))
		{
			m_EditHostList.ReplaceSel(sHost + _T("\r\n"));
		}
		else
		{
			m_EditHostList.ReplaceSel(sHost);  // Die letzte Zeile hat kein CrLf
			bLoop = FALSE;
		}
			
	
	};

	m_bLoadingHostList = TRUE;

	return TRUE;
}

void CSocketPage::OnUseIPHostList() 
{
	SetModified(TRUE);

	ShowHide();
}

/////////////////////////////////////////////////////////////////////////////////
BOOL CSocketPage::IsHostInHostList(const CString &sHost)
{
	TCHAR	szBuffer[1024] = {0};
	BOOL	bFind = FALSE;

	for (int nI = 0; nI < m_EditHostList.GetLineCount() && !bFind; nI++)
	{
		int nCnt = m_EditHostList.GetLine(nI, szBuffer, 1024);
		if (nCnt >= 2)
			szBuffer[nCnt-2] = 0;

		if (sHost == (CString)szBuffer)
			bFind = TRUE;
	}
	return bFind;
}

void CSocketPage::OnCopyTcpToList() 
{
	CHostArray	Hosts;

	Hosts.Load(GetProfile());
   	m_EditHostList.SetSel(0, 0);

	// Alle TCP-Hosts durchsuchen
	for (int nI = 0; nI < Hosts.GetSize(); nI++)
	{
		if (Hosts.GetAt(nI)->GetTyp() == _T("TCP/IP"))
		{
			CString sNumber	= Hosts.GetAt(nI)->GetNumber();
			sNumber = sNumber.Mid(4); // _T('tcp:') entfernen
			
			// Ist der Host schon in der Liste?
			if (Hosts.GetAt(nI)->IsTCPIP())
			{
				if (    CheckIPSyntax(sNumber)
					&& !IsHostInHostList(sNumber))
					m_EditHostList.ReplaceSel(sNumber + _T("\r\n"));
			}
		}
	}
}

BOOL CSocketPage::CheckIPSyntax(const CString &sHost)
{
	char	sRest[1024]  = {0};
	int		a,b,c,d;
	BOOL	bRet = FALSE;
	if (m_bUseDNS)
	{
		return TRUE;
	}
	if (sHost.IsEmpty())
		return FALSE;

	a=-1;
	b=-1;
	c=-1;
	d=-1;
	_stscanf(sHost, _T("%d.%d.%d.%d%s"), &a,&b,&c,&d,&sRest);
	if ((a<0) || (a>255) ||
		(b<0) || (b>255) ||
		(c<0) || (c>255) ||
		(d<0) || (d>255))
	{
		return FALSE;
	}
	else
	{
		if (((CString)sRest).IsEmpty())
			bRet =(sHost.SpanIncluding(_T("1234567890.")) == sHost);
		else 
			bRet = FALSE;

	}

	return bRet;
}