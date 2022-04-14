// DetermineIPAddressDlg.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "DetermineIPAddress.h"
#include "DetermineIPAddressDlg.h"

#include <IpHlpApi.h>
#include <Winbase.h>
#include ".\determineipaddressdlg.h"

#include "wk_util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define REG_CLOSE_KEY(hKey) {if(hKey){RegCloseKey(hKey); hKey=NULL;}}


#define NETWORKCARD_NT	_T("Software\\Microsoft\\Windows NT\\CurrentVersion\\NetworkCards\\")
#define SERVICES		_T("System\\CurrentControlSet\\Services")
#define SERVICENAME		_T("ServiceName")
#define IPADDRESS		_T("IPAddress")
#define SUBNETMASK		_T("SubnetMask")
#define DEFAULTGATEWAY	_T("DefaultGateway")
#define	ENABLEDHCP		_T("EnableDHCP")

// CAboutDlg-Dialogfeld für Anwendungsbefehl 'Info'

#define NAME_OF_ENUM(X) case X: return _T(#X);

CString NameOfEnum(COMPUTER_NAME_FORMAT eCNF)
{
	switch (eCNF)
	{
		NAME_OF_ENUM(ComputerNameNetBIOS)
		NAME_OF_ENUM(ComputerNamePhysicalNetBIOS)
		NAME_OF_ENUM(ComputerNameDnsHostname)
		NAME_OF_ENUM(ComputerNameDnsDomain)
		NAME_OF_ENUM(ComputerNameDnsFullyQualified)
		NAME_OF_ENUM(ComputerNamePhysicalDnsHostname)
		NAME_OF_ENUM(ComputerNamePhysicalDnsDomain)
		NAME_OF_ENUM(ComputerNamePhysicalDnsFullyQualified)
		NAME_OF_ENUM(ComputerNameMax)
	}
	return _T("");
}
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialogfelddaten
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung

// Implementierung
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CDetermineIPAddressDlg Dialogfeld



CDetermineIPAddressDlg::CDetermineIPAddressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDetermineIPAddressDlg::IDD, pParent)
	, m_sCompAndDnsName(_T(""))
	, m_sIPAddress(_T(""))
	, m_sSubnetMask(_T(""))
	, m_bEnableDHCP(-1)
	, m_sIPAddressDHCPserver(_T(""))
	, m_Cmd(_T("cmd.exe"), NULL, NULL, WKCPF_TERMINATE|WKCPF_OUTPUT|WKCPF_INPUT|WKCPF_OUTPUT_CHECK|WKCPF_START_THREAD)
	, m_dwIP(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nIPAdapterIndex = -1;
}

void CDetermineIPAddressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_Edit1);
	DDX_Text(pDX, IDC_EDT_NAME, m_sCompAndDnsName);
	DDX_Text(pDX, IDC_EDT_IP_ADDRESS, m_sIPAddress);
	DDX_Text(pDX, IDC_EDT_SUBNET_MASK, m_sSubnetMask);
	DDX_Check(pDX, IDC_CK_ENABLE_DHCP, m_bEnableDHCP);
	DDX_Text(pDX, IDC_EDT_IP_ADDRESS_DHCP, m_sIPAddressDHCPserver);
	DDX_IPAddress(pDX, IDC_IPADDRESS1, m_dwIP);
}

BEGIN_MESSAGE_MAP(CDetermineIPAddressDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_DET_IP, OnBnClickedDetIp)
	ON_BN_CLICKED(IDC_DET_DHCP, OnBnClickedDetDhcp)
	ON_BN_CLICKED(IDC_DET_CLEAR_TXT, OnBnClickedDetClearTxt)
	ON_BN_CLICKED(IDC_BTN_SET_NAMES, OnBnClickedBtnSetNames)
	ON_BN_CLICKED(IDC_BTN_TEST, OnBnClickedBtnTest)
	ON_BN_CLICKED(IDC_BTN_SET_ADAPTER_PARAMS, OnBnClickedBtnSetAdapterParams)
	ON_BN_CLICKED(IDC_CK_ENABLE_DHCP, OnBnClickedCkEnableDhcp)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_TEST2, OnBnClickedBtnTest2)
	ON_BN_CLICKED(IDC_BTN_TEST3, OnBnClickedBtnTest3)
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_IPADDRESS1, OnIpnFieldchangedIpaddress1)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDetermineIPAddressDlg Meldungshandler

BOOL CDetermineIPAddressDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Hinzufügen des Menübefehls "Info..." zum Systemmenü.

	// IDM_ABOUTBOX muss sich im Bereich der Systembefehle befinden.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Symbol für dieses Dialogfeld festlegen. Wird automatisch erledigt
	//  wenn das Hauptfenster der Anwendung kein Dialogfeld ist
	SetIcon(m_hIcon, TRUE);			// Großes Symbol verwenden
	SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden
	
	m_Cmd.SetStdOutMsgWnd(m_hWnd, WM_TIMER, 100, 0);

	EnableControls();
	
	return TRUE;  // Geben Sie TRUE zurück, außer ein Steuerelement soll den Fokus erhalten
}

void CDetermineIPAddressDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// Wenn Sie dem Dialogfeld eine Schaltfläche "Minimieren" hinzufügen, benötigen Sie 
//  den nachstehenden Code, um das Symbol zu zeichnen. Für MFC-Anwendungen, die das 
//  Dokument/Ansicht-Modell verwenden, wird dies automatisch ausgeführt.

void CDetermineIPAddressDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // Gerätekontext zum Zeichnen

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Symbol in Clientrechteck zentrieren
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Symbol zeichnen
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// Die System ruft diese Funktion auf, um den Cursor abzufragen, der angezeigt wird, während der Benutzer
//  das minimierte Fenster mit der Maus zieht.
HCURSOR CDetermineIPAddressDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDetermineIPAddressDlg::OutputLastError(LPCTSTR lpszError, DWORD dwError/*=0*/)
{
	LPVOID lpMsgBuf;
	if (dwError == 0) dwError = GetLastError();
	if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, dwError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf, 0, NULL )) 
	{
		OutputString(_T("%s:\n %d:%s"), lpszError, dwError, (LPCTSTR)lpMsgBuf);
	}
	LocalFree( lpMsgBuf );
}

void CDetermineIPAddressDlg::OutputString(LPCTSTR lpszFormat, ...)
{
	CString sOut;
	va_list args;
	va_start(args, lpszFormat);
	int nBuf;
	TCHAR* pszBuffer = sOut.GetBufferSetLength(1024);
	ZeroMemory(pszBuffer, 1024);

	nBuf = _vstprintf(pszBuffer, lpszFormat, args);
	va_end(args);
	sOut.ReleaseBuffer();
	sOut.Replace(_T("\n"), _T("\r\n"));

	m_Edit1.SetSel(-1, -1);
	m_Edit1.ReplaceSel(sOut);
}

void CDetermineIPAddressDlg::OnBnClickedDetIp()
{
	BOOL	bResult = FALSE;
	HKEY	hKey     = NULL;
	HKEY  hSecKey  = NULL;
	DWORD	dwType   = 0,
			dwIndex,
			dwLen    = 0;
	LONG	lResult  = 0;
	_TCHAR  szName[MAX_PATH];
	CString sNetCard;
	CString sIP, sFmt, sSNM;

	m_sSubnetMask.Empty();
	m_sIPAddress.Empty();
	bResult = GetDefaultGateway(sIP);
	OutputString(_T("DefaultGateway: %d: %s\n"), bResult, sIP);

	PMIB_IPADDRTABLE pIpAddrTable = NULL;
//	MIB_IPADDRTABLE IpAddrTable;
	DWORD dwDummy, dwSize = sizeof(DWORD);
	GetIpAddrTable((PMIB_IPADDRTABLE)&dwDummy, &dwSize, FALSE);
	if (dwSize > 0)
	{		 
		pIpAddrTable = (PMIB_IPADDRTABLE) new BYTE[dwSize];
		
		// Hole die erste IP Adresse, die nicht 127.0.0.1 lautet
		if (pIpAddrTable && (GetIpAddrTable(pIpAddrTable, &dwSize, FALSE) == NO_ERROR))
		{
			for (DWORD dwI = 0; dwI < pIpAddrTable->dwNumEntries; dwI++)
			{
				DWORD dwIP = 0;
				dwIP = pIpAddrTable->table[dwI].dwAddr;
				
				sIP.Format(_T("%03d.%03d.%03d.%03d"), (dwIP & 0x000000ff) >> 0,
												(dwIP & 0x0000ff00) >> 8,						
												(dwIP & 0x00ff0000) >> 16,					
												(dwIP & 0xff000000) >> 24);				

				OutputString(_T("%d: IP: %s\n"), dwI, sIP);

				dwIP = pIpAddrTable->table[dwI].dwMask;
				sSNM.Format(_T("%03d.%03d.%03d.%03d"), (dwIP & 0x000000ff) >> 0,
												(dwIP & 0x0000ff00) >> 8,						
												(dwIP & 0x00ff0000) >> 16,					
												(dwIP & 0xff000000) >> 24);				

				OutputString(_T("%d: SubnetMask: %s\n"), dwI, sSNM);
				if (pIpAddrTable->table[dwI].dwAddr != 0x0100007f)
				{
					if (m_sIPAddress.IsEmpty())
					{
						m_sIPAddress = sIP;
						m_sSubnetMask = sSNM;
					}
				}

//					break;
			}
		}
		WK_DELETE(pIpAddrTable);
	}



	CString sParam;
	try
	{
		lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, NETWORKCARD_NT, 0, KEY_READ, &hKey);
		if (lResult != ERROR_SUCCESS)			throw (int) __LINE__;

		for (dwIndex=0; ; dwIndex++)
		{
			dwLen   = MAX_PATH;												// Key ermitteln
			lResult = RegEnumKeyEx(hKey, dwIndex, szName, &dwLen, NULL, NULL, 0, NULL);
			if (lResult != ERROR_SUCCESS)		throw (int) 0;			// beenden, wenn kein weiterer existiert
																					// Oeffnen
			lResult = RegOpenKeyEx(hKey, szName, 0, KEY_READ, &hSecKey);
			if (lResult != ERROR_SUCCESS)		throw (int) __LINE__;			// 

			lResult = RegQueryValueEx(hSecKey, SERVICENAME, NULL, &dwType, NULL,&dwLen);
			if (lResult != ERROR_SUCCESS)	throw (int) __LINE__;
			
			lResult = RegQueryValueEx(hSecKey, SERVICENAME, NULL, &dwType, (LPBYTE)sNetCard.GetBuffer(dwLen/sizeof(TCHAR)), &dwLen);
			sNetCard.ReleaseBuffer();
			REG_CLOSE_KEY(hSecKey);
			if (lResult != ERROR_SUCCESS)	continue;

			CString sKey;
			COsVersionInfo osv;
			if (osv.IsWinXP())
			{
				sKey.Format(_T("%s\\Tcpip\\Parameters\\Interfaces\\%s\\"), SERVICES, sNetCard);
			}
			else
			{
				sKey.Format(_T("%s\\%s\\Parameters\\tcpip"), SERVICES, sNetCard);
			}

			lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, sKey, 0, KEY_READ, &hSecKey);
			if (lResult != ERROR_SUCCESS)	continue;
			OutputString(_T("\n") + sKey+_T("\n"));

			for (int nP=0; nP<28; nP++)
			{
				switch (nP)
				{
					case 0: sParam = IPADDRESS; break;
					case 1: sParam = SUBNETMASK; break;
					case 2: sParam = DEFAULTGATEWAY; break;
					case 3: sParam = ENABLEDHCP; break;
					case 4: sParam = _T("DhcpDomain"); break;
					case 5: sParam = _T("DhcpIPAddress"); break;
					case 6: sParam = _T("DhcpNameServer"); break;
					case 7: sParam = _T("DhcpServer"); break;
					case 8: sParam = _T("DhcpSubnetMask"); break;
					case 9: sParam = _T("DhcpSubnetMaskOpt"); break;
					case 10: sParam = _T("Domain"); break;
					case 11: sParam = _T("EnableDeadGWDetect"); break;
					case 12: sParam = _T("IPAutoconfigurationAddress"); break;
					case 13: sParam = _T("IPAutoconfigurationMask"); break;
					case 14: sParam = _T("IPAutoconfigurationSeed"); break;
					case 15: sParam = _T("Lease"); break;
					case 16: sParam = _T("LeaseObtainedTime"); break;
					case 17: sParam = _T("LeaseTerminatesTime"); break;
					case 18: sParam = _T("NameServer"); break;
					case 19: sParam = _T("NTEContextList"); break;
					case 20: sParam = _T("RawIPAllowedProtocols"); break;
					case 21: sParam = _T("RegisterAdapterName"); break;
					case 22: sParam = _T("RegistrationEnabled"); break;
					case 23: sParam = _T("T1"); break;
					case 24: sParam = _T("T2"); break;
					case 25: sParam = _T("TCPAllowedPorts"); break;
					case 26: sParam = _T("UDPAllowedPorts"); break;
					case 27: sParam = _T("UseZeroBroadcast"); break;
				}
				dwLen = MAX_PATH-1;
				lResult = RegQueryValueEx(hSecKey, sParam, NULL, &dwType, (LPBYTE)szName, &dwLen);
				if (lResult == ERROR_SUCCESS)
				{
					if (dwType == REG_MULTI_SZ || dwType == REG_SZ)
					{
						OutputString(_T("%s: %s\n"), sParam, szName);
//						DTS Formating with leading zeros
//						{
//							int n1=0, n2=0, n3=0, n4=0;
//							if (4==_stscanf(szName, _T("%d.%d.%d.%d"), &n1, &n2, &n3, &n4))
//							{
//								sIP.Format(_T("IP: %03d.%03d.%03d.%03d"), n1, n2, n3, n4);
//								OutputString(sIP);
//							}
//						}
						bResult = TRUE;
					}
					else if (dwType == REG_DWORD)
					{
						DWORD *pdw = (DWORD*)szName;
						OutputString(_T("%s: %d\n"), sParam, *pdw);
					}
				}
			}
			REG_CLOSE_KEY(hSecKey);
			sKey.Format(_T("SYSTEM\\CurrentControlSet\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}\\%s\\Connection"), sNetCard);
			lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, sKey, 0, KEY_READ, &hSecKey);
			if (lResult == ERROR_SUCCESS)
			{
				sParam = _T("Name");
				dwLen = MAX_PATH-1;
				lResult = RegQueryValueEx(hSecKey, sParam, NULL, &dwType, (LPBYTE)szName, &dwLen);
				if (lResult == ERROR_SUCCESS)
				{
					if (dwType == REG_MULTI_SZ || dwType == REG_SZ)
					{
						OutputString(_T("%s: %s\n"), sParam, szName);
					}
				}
			}
			REG_CLOSE_KEY(hSecKey);

//			if (psKeyList) psKeyList->AddHead(sKey);
		}
	}
	catch (int nError)
	{
		if (nError)
		{
			OutputString(_T("Error OnGetIP(%s): %d, %d\n"), sParam, nError, GetLastError());
		}
	}

	REG_CLOSE_KEY(hKey);
	REG_CLOSE_KEY(hSecKey);

	UpdateData(FALSE);
/*
	// --------------------------------------------
	ULONG OutBufLen	= 0;
	if (GetAdaptersInfo(NULL, &OutBufLen) == ERROR_BUFFER_OVERFLOW)
	{
		PIP_ADAPTER_INFO pAdapterInfoStart	= (PIP_ADAPTER_INFO) new BYTE[OutBufLen];
		PIP_ADAPTER_INFO pAdapterInfo		= pAdapterInfoStart;
		if (GetAdaptersInfo(pAdapterInfo, &OutBufLen) == ERROR_SUCCESS)
		{
			do
			{
			 	if (pAdapterInfo->Type == IF_ETHERNET_ADAPTERTYPE)
				{
					DWORD dwIndex = pAdapterInfo->Index;
					break;
				}
				pAdapterInfo = pAdapterInfo->Next;

			}while (pAdapterInfo != NULL);

		}
		WK_DELETE(pAdapterInfoStart);
	}
	DWORD dwNum = 0;

	// --------------------------------------------
	if (GetNumberOfInterfaces(&dwNum) == NO_ERROR)
		TRACE(_T("Numbers of Interfaces=%d\n"), dwNum);

	// --------------------------------------------
	dwSize = 0;
	if (GetIfTable(NULL, &dwSize, TRUE) != NO_ERROR)
	{
		PMIB_IFTABLE  pIfTable	= (PMIB_IFTABLE) new BYTE[dwSize];
		if (GetIfTable(pIfTable, &dwSize, TRUE) == NO_ERROR)
		{
			MIB_IFROW IfRow;
			for (DWORD dwIndex = 0; dwIndex < pIfTable->dwNumEntries; dwIndex++)
			{
				IfRow = pIfTable->table[dwIndex];
			}

		}
		WK_DELETE(pIfTable);
	}

	// --------------------------------------------
	dwSize = 0;
	if (GetInterfaceInfo(NULL, &dwSize) == ERROR_INSUFFICIENT_BUFFER)
	{
		PIP_INTERFACE_INFO pIfInfo	= (PIP_INTERFACE_INFO) new BYTE[dwSize];
		if (GetInterfaceInfo(pIfInfo, &dwSize) == NO_ERROR)
		{
			IP_ADAPTER_INDEX_MAP AdapterMap;
			for (LONG dwIndex = 0; dwIndex < pIfInfo->NumAdapters; dwIndex++)
			{
				AdapterMap = pIfInfo->Adapter[dwIndex];
			}
		}
		WK_DELETE(pIfInfo);
	}
	
	// --------------------------------------------
	dwSize = 0;
	if (GetIpNetTable(NULL, &dwSize, TRUE) != NO_ERROR)
	{
		PMIB_IPNETTABLE pIpNetTable = (PMIB_IPNETTABLE) new BYTE[dwSize];

		if (GetIpNetTable(pIpNetTable, &dwSize, TRUE) == NO_ERROR)
		{
			MIB_IPNETROW   IpNetRow;
			for (DWORD dwIndex = 0; dwIndex < pIpNetTable->dwNumEntries; dwIndex++)
			{
				IpNetRow = pIpNetTable->table[dwIndex];
				CString sIP;
				DWORD dwIP = IpNetRow.dwAddr;
				sIP.Format(_T("%03d%03d%03d%03d"), (dwIP & 0x000000ff) >> 0,
								   (dwIP & 0x0000ff00) >> 8,						
								   (dwIP & 0x00ff0000) >> 16,					
								   (dwIP & 0xff000000) >> 24);
				TRACE(_T("IP=%s\n"), sIP);
			}
		}
	}


	// --------------------------------------------
	char  szHost[255];
	if (gethostname(szHost, sizeof(szHost)) == 0)
	{
		HOSTENT *phe;
		phe = gethostbyname(szHost);
		if (phe)
			phe = gethostbyname(phe->h_name);
	}
*/
}

void CDetermineIPAddressDlg::OnBnClickedDetDhcp()
{
	CString str, strX;
	HRESULT hr;
    IPAddr  ipAddr;
    ULONG   pulMac[2];
    ULONG   ulLen;
	
	m_sIPAddressDHCPserver.Empty();

    ipAddr = inet_addr ("192.168.0.39");
    memset (pulMac, 0xff, sizeof (pulMac));
    ulLen = 6;
    
    hr = SendARP (ipAddr, 0, pulMac, &ulLen);
	OutputString(_T("Return %08x, length %8d\n"), hr, ulLen);
	strX = _T("Mac:");
	PBYTE pbHexMac = (PBYTE) pulMac;
	if (hr == S_OK)
	{
		for (ULONG i = 0; i < ulLen - 1; ++i) 
		{
			str.Format(_T("%02X:"), pbHexMac[i]);
			strX += str;
		}
	}
	strX += _T("\n");
	OutputString(strX);

	FIXED_INFO * FixedInfo;
	ULONG    ulOutBufLen;
	DWORD    dwRetVal;
	IP_ADDR_STRING * pIPAddr;

	FixedInfo = (FIXED_INFO *) GlobalAlloc(GPTR, sizeof(FIXED_INFO));
	ulOutBufLen = sizeof( FIXED_INFO );
   
	if( ERROR_BUFFER_OVERFLOW == GetNetworkParams(FixedInfo, &ulOutBufLen ) ) 
	{
		GlobalFree(FixedInfo);
		FixedInfo = (FIXED_INFO *) GlobalAlloc(GPTR, ulOutBufLen);
	}

	if (dwRetVal = GetNetworkParams(FixedInfo, &ulOutBufLen)) 
	{
		OutputString(_T("Call to GetNetworkParams failed. Return Value: %08x\n"), dwRetVal);
	}
	else 
	{
		_TCHAR szCompName[64];
		COMPUTER_NAME_FORMAT eCNF;
		int *pi = (int*) &eCNF;
		DWORD dwSize;
		for ((*pi)=0; (*pi)<9; (*pi)++)
		{
			dwSize = 64;
			GetComputerNameEx(eCNF, szCompName, &dwSize);
			if (eCNF == ComputerNameDnsHostname)
			{
				m_sCompAndDnsName = szCompName;
			}
			OutputString(_T("%s: %s\n"), NameOfEnum(eCNF), szCompName);
		}

		OutputString(_T("\nHost Name: %s\n"), (LPCTSTR)CWK_String(FixedInfo->HostName));
		OutputString(_T(" Domain Name: %s\n"), (LPCTSTR)CWK_String(FixedInfo->DomainName));
		OutputString(_T(" NodeType     : %d\n"), FixedInfo->NodeType);
		OutputString(_T(" EnableDns    : %d\n"), FixedInfo->EnableDns);
		OutputString(_T(" EnableProxy  : %d\n"), FixedInfo->EnableProxy);
		OutputString(_T(" EnableRouting: %d\n"), FixedInfo->EnableRouting);

		OutputString(_T("DNS Servers:\n"));

		pIPAddr = &FixedInfo->DnsServerList;
		while ( pIPAddr ) 
		{
			OutputString(_T(" %s\n"), (LPCTSTR)CWK_String(pIPAddr->IpAddress.String));
			m_sIPAddressDHCPserver += (LPCTSTR)CWK_String(pIPAddr->IpAddress.String);
			m_sIPAddressDHCPserver += _T(", ");
			pIPAddr = pIPAddr ->Next;
		}

		// Declare and initialize variables
		PIP_INTERFACE_INFO pInfo = NULL;
		ULONG ulOutBufLen = sizeof(DWORD);
		DWORD dwRetVal = 0, dwDummy = 0;


		// Make an initial call to GetInterfaceInfo to get
		// the necessary size in the ulOutBufLen variable
		if ( GetInterfaceInfo((IP_INTERFACE_INFO *)&dwDummy, &ulOutBufLen) == ERROR_INSUFFICIENT_BUFFER) 
		{
			pInfo = (IP_INTERFACE_INFO *) new BYTE[ulOutBufLen];
		}

		// Make a second call to GetInterfaceInfo to get
		// the actual data we need
		if ((dwRetVal = GetInterfaceInfo(pInfo, &ulOutBufLen)) == NO_ERROR )
		{
			OutputString(_T(" Adapter Name: %ws\n"), pInfo->Adapter[0].Name);
			OutputString(_T(" Adapter Index: %ld\n"), pInfo->Adapter[0].Index);
			OutputString(_T(" Num Adapters: %ld\n"), pInfo->NumAdapters);

			CString s(pInfo->Adapter[0].Name);
			int nFind  = s.Find(_T("{"));
			if (nFind != -1)
			{
				m_sAdapterKey.Format(_T("%s\\Tcpip\\Parameters\\Interfaces\\%s"), SERVICES, s.Mid(nFind));
				HKEY hSecKey = NULL;
				LONG lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, m_sAdapterKey, 0, KEY_READ, &hSecKey);
				if (lResult == ERROR_SUCCESS)
				{
					DWORD dwLen = sizeof(m_bEnableDHCP), dwType, dwValue;
					lResult = RegQueryValueEx(hSecKey, ENABLEDHCP, NULL, &dwType, (LPBYTE)&dwValue, &dwLen);
					if (lResult == ERROR_SUCCESS)
					{
						m_bEnableDHCP = dwValue;
					}
				}
				else
				{
					m_sAdapterKey.Empty();
				}
				EnableControls();
				REG_CLOSE_KEY(hSecKey);
			}
		}
		else 
		{
			OutputLastError(_T("GetInterfaceInfo failed."), dwRetVal);
		}
		WK_DELETE(pInfo);

		UpdateData(FALSE);
	}
}

void CDetermineIPAddressDlg::OnBnClickedDetClearTxt()
{
	m_Edit1.SetWindowText(_T(""));
}

BOOL CDetermineIPAddressDlg::GetDefaultGateway(CString& sGateway)
{
	BOOL	bResult = FALSE;

	PMIB_IPFORWARDTABLE pIPForwardTbl = NULL;
	MIB_IPFORWARDTABLE IPForwardTbl;
	DWORD dwSize = sizeof(IPForwardTbl);
	GetIpForwardTable(&IPForwardTbl, &dwSize, FALSE);
	if (dwSize > 0)
	{
		pIPForwardTbl = (PMIB_IPFORWARDTABLE) new BYTE[dwSize];

		if (pIPForwardTbl && GetIpForwardTable(pIPForwardTbl, &dwSize, FALSE) == NO_ERROR)
		{
			for (DWORD dwI = 0; dwI < pIPForwardTbl->dwNumEntries; dwI++)
			{
				MIB_IPFORWARDROW    IPForwardRow = pIPForwardTbl->table[dwI];
				if ((IPForwardRow.dwForwardDest == 0)	 &&
					(IPForwardRow.dwForwardMask == 0))
				{
					DWORD dwGateway = IPForwardRow.dwForwardNextHop;
					if (dwGateway != 0)
					{
						sGateway.Format(_T("%03d%.03d%.03d%.03d"), (dwGateway & 0x000000ff) >> 0,
															(dwGateway & 0x0000ff00) >> 8,						
															(dwGateway & 0x00ff0000) >> 16,					
															(dwGateway & 0xff000000) >> 24);				
						bResult = TRUE;
					}						
					break;
				}
			}
		}
		WK_DELETE(pIPForwardTbl);
	}

	return bResult;
}

void CDetermineIPAddressDlg::OnBnClickedBtnSetNames()
{
	UpdateData();
	if (!m_sCompAndDnsName.IsEmpty())
	{
		// The name may be set also with fixed IP adresses, if the HOST supports DNS
		// the name will be recognized.
		CString sMsg;
		sMsg.Format(_T("Change the Name of the Computer and Dns to :%s ?"), m_sCompAndDnsName);
		if (AfxMessageBox(sMsg, MB_ICONQUESTION|MB_YESNO) == IDYES)
		{
			_TCHAR szDomain[128];
			CString sFullname, sUpperName(m_sCompAndDnsName);
			sUpperName.MakeUpper();
			DWORD dwSize = 128;
			GetComputerNameEx(ComputerNameDnsDomain, szDomain, &dwSize);
			sFullname = m_sCompAndDnsName + _T(".") + szDomain;
			sUpperName.MakeUpper();
 
			SetComputerNameEx(ComputerNamePhysicalNetBIOS, sUpperName) &&
			SetComputerNameEx(ComputerNamePhysicalDnsHostname, m_sCompAndDnsName);
		}
	}
}

void CDetermineIPAddressDlg::EnableControls()
{
	BOOL bEnable = m_sAdapterKey.IsEmpty() ? FALSE : TRUE;
	GetDlgItem(IDC_EDT_IP_ADDRESS)->EnableWindow(bEnable && !m_bEnableDHCP);
	GetDlgItem(IDC_EDT_SUBNET_MASK)->EnableWindow(bEnable && !m_bEnableDHCP);
	GetDlgItem(IDC_CK_ENABLE_DHCP)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDT_IP_ADDRESS_DHCP)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDT_SUBNET_MASK_DHCP)->EnableWindow(FALSE);
}

void CDetermineIPAddressDlg::OnBnClickedBtnSetAdapterParams()
{
	if (!m_sAdapterKey.IsEmpty() && UpdateData())
	{
		HKEY hSecKey = NULL;
		LONG lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, m_sAdapterKey, 0, KEY_WRITE, &hSecKey);
		if (lResult == ERROR_SUCCESS)
		{
			OutputString(_T("SetAdapterParams\n EnableDHCP:%d\n"), m_bEnableDHCP);
			DWORD dwLen = sizeof(m_bEnableDHCP);
			lResult = RegSetValueEx(hSecKey, ENABLEDHCP, 0, REG_DWORD, (LPBYTE)&m_bEnableDHCP, sizeof(DWORD));
			if (m_bEnableDHCP)
			{
				CString sAddress = _T("0.0.0.0");
				lResult = RegSetValueEx(hSecKey, IPADDRESS, 0, REG_MULTI_SZ, (LPBYTE)LPCTSTR(sAddress), sAddress.GetLength()*sizeof(_TCHAR));
				lResult = RegSetValueEx(hSecKey, SUBNETMASK, 0, REG_MULTI_SZ, (LPBYTE)LPCTSTR(sAddress), sAddress.GetLength()*sizeof(_TCHAR));
			}
			else
			{
				CString sAddress;
				int n1=0, n2=0, n3=0, n4=0;
				if (4==_stscanf(m_sIPAddress, _T("%d.%d.%d.%d"), &n1, &n2, &n3, &n4))
				{
					sAddress.Format(_T("%d.%d.%d.%d"), n1, n2, n3, n4);
				}
				OutputString(_T(" %s, %x.%x.%x.%x: %s\n"), m_sIPAddress, n1, n2, n3, n4, sAddress);
				lResult = RegSetValueEx(hSecKey, IPADDRESS, 0, REG_MULTI_SZ, (LPBYTE)LPCTSTR(sAddress), sAddress.GetLength()*sizeof(_TCHAR));
				if (4==_stscanf(m_sSubnetMask, _T("%d.%d.%d.%d"), &n1, &n2, &n3, &n4))
				{
					sAddress.Format(_T("%d.%d.%d.%d"), n1, n2, n3, n4);
				}
				OutputString(_T(" %s, %x.%x.%x.%x: %s\n"), m_sSubnetMask, n1, n2, n3, n4, sAddress);
				lResult = RegSetValueEx(hSecKey, SUBNETMASK, 0, REG_MULTI_SZ, (LPBYTE)LPCTSTR(sAddress), sAddress.GetLength()*sizeof(_TCHAR));
			}
		}
		else
		{
			m_sAdapterKey.Empty();
		}
		EnableControls();
		REG_CLOSE_KEY(hSecKey);
	}
}

void CDetermineIPAddressDlg::OnBnClickedCkEnableDhcp()
{
	CDataExchange dx(this, TRUE);
	DDX_Check(&dx, IDC_CK_ENABLE_DHCP, m_bEnableDHCP);
	EnableControls();
}

void CDetermineIPAddressDlg::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == 100)
	{
		CWK_String*pstr;
		while ((pstr= m_Cmd.GetAndRemoveOutputString()) != NULL)
		{
			OutputString(*pstr + _T("\n"));
			WK_DELETE(pstr);
		}
	}

	CDialog::OnTimer(nIDEvent);
}

#define TEST_NAME_FROM_ADDRESS 1

// #define TEST_PING 1
// #define TEST_ADAPTER 1
// #define TEST_INTERFACE 1
// #define TEST_IPADDRTABLE 1
// #define TEST_IPNETTABLE 1

void CDetermineIPAddressDlg::OnBnClickedBtnTest()
{

#ifdef TEST_NAME_FROM_ADDRESS
	if (UpdateData())
	{
		struct hostent FAR *pHost = NULL;
		m_dwIP = inet_addr("192.168.0.35");
		if (m_dwIP!=INADDR_NONE) 
		{	// dotted IP number try lookup
			pHost = gethostbyaddr ((const char FAR *)&m_dwIP, 4, AF_INET);
			if (pHost)
			{
				CString sName(pHost->h_name);
				OutputString(_T("%s\n"), sName);
			}
		}
		pHost = gethostbyname("rolfxp");
		if (pHost)
		{
			CString sName(pHost->h_name);
			OutputString(_T("%s\n"), sName);
		}


		int retVal;
		sockaddr_in saGNI;
		char hostName[256];
		char servInfo[256];
		u_short port;
		port = 1111;

		//-----------------------------------------
		// Set up sockaddr_in structure which is passed
		// to the getnameinfo function
		saGNI.sin_family = AF_INET;
		saGNI.sin_addr.s_addr = inet_addr("192.168.0.40");
		saGNI.sin_port = htons(port);

		//-----------------------------------------
		// Call getnameinfo
		retVal = getnameinfo((SOCKADDR *)&saGNI, 
		sizeof(sockaddr), 
		hostName, 
		256, 
		servInfo, 
		256, 
		NI_NAMEREQD);
		if (retVal != 0)
		{
			CString sName(hostName);
			CString sInfo(servInfo);
			OutputString(_T("%s, %s\n"), sName, sInfo);
		}

	}
#endif
#ifdef TEST_PING
	if (UpdateData())
	{
		CString sPing;
		int n1=0, n2=0, n3=0, n4=0;
		if (4==_stscanf(m_sIPAddress, _T("%d.%d.%d.%d"), &n1, &n2, &n3, &n4))
		{
			sPing.Format(_T("ping %d.%d.%d.%d"), n1, n2, n3, n4);
			m_Cmd.WriteStdIn(sPing, TRUE);
		}
	}
#else
	DWORD dwRetVal;
	CString sFmt;
	DWORD dwDummy, dwSize;

	UpdateData();

#ifdef TEST_ADAPTER
	IP_ADAPTER_INFO *pAI = NULL, *pAInext;
	dwSize = sizeof(DWORD);
	dwRetVal = GetAdaptersInfo((IP_ADAPTER_INFO*)&dwDummy, &dwSize);
	if (dwRetVal == ERROR_BUFFER_OVERFLOW)
	{
		pAI = (IP_ADAPTER_INFO*) new BYTE[dwSize];
	}
	dwRetVal = GetAdaptersInfo(pAI, &dwSize);
	
	OutputString(_T("AdapterInfo:\n"));
	pAInext = pAI;

	while (pAInext)
	{
		OutputString(_T(" Name: %s\n"), CString(pAInext->AdapterName));
		OutputString(_T(" Description: %s\n"), CString(pAInext->Description));

		OutputString(_T(" Index: %d\n"), pAInext->Index);
		OutputString(_T(" Type: %d\n"), pAInext->Type);
		OutputString(_T(" DHCP: %d\n"), pAInext->DhcpEnabled);
		m_nIPAdapterIndex = pAInext->Index;

		OutputString(_T(" IpAddressList:\n"));
		OutputString(_T("  IP address: %s\n"), CString(pAInext->IpAddressList.IpAddress.String));
		OutputString(_T("  IP mask: %s\n"), CString(pAInext->IpAddressList.IpMask.String));
		OutputString(_T("  Context: %d\n"), pAInext->IpAddressList.Context);

		OutputString(_T(" DHCPServer:\n"));
		OutputString(_T("  IP address: %s\n"), CString(pAInext->DhcpServer.IpAddress.String));
		OutputString(_T("  IP mask: %s\n"), CString(pAInext->DhcpServer.IpMask.String));
		OutputString(_T("  Context: %d\n"), pAInext->DhcpServer.Context);

		OutputString(_T(" PrimaryWinsServer:\n"));
		OutputString(_T("  IP address: %s\n"), CString(pAInext->PrimaryWinsServer.IpAddress.String));
		OutputString(_T("  IP mask: %s\n"), CString(pAInext->PrimaryWinsServer.IpMask.String));
		OutputString(_T("  Context: %d\n"), pAInext->PrimaryWinsServer.Context);

		OutputString(_T(" SecondaryWinsServer:\n"));
		OutputString(_T("  IP address: %s\n"), CString(pAInext->SecondaryWinsServer.IpAddress.String));
		OutputString(_T("  IP mask: %s\n"), CString(pAInext->SecondaryWinsServer.IpMask.String));
		OutputString(_T("  Context: %d\n"), pAInext->SecondaryWinsServer.Context);

		pAInext = pAInext->Next;
	};
	WK_DELETE(pAI);
#endif
#ifdef TEST_INTERFACE
	DWORD dwNumIf = 0;
	IP_INTERFACE_INFO *pII = NULL;
	MIB_IFROW ifrow;
	ZERO_INIT(ifrow);
	OutputString(_T("InterfaceInfo:\n"));
	dwRetVal = GetNumberOfInterfaces(&dwNumIf);
	OutputString(_T("Number Of Interfaces:%d (%d)\n"), dwNumIf, dwRetVal);
	dwSize = sizeof(DWORD);
	dwRetVal = GetInterfaceInfo((IP_INTERFACE_INFO*)&dwDummy, &dwSize);
	if (dwRetVal == ERROR_INSUFFICIENT_BUFFER)
	{
		pII = (IP_INTERFACE_INFO*) new BYTE[dwSize];
	}
	dwRetVal = GetInterfaceInfo(pII, &dwSize);
	if (dwRetVal == NO_ERROR)
	{
		OutputString(_T("Number Of Adapters:%d \n"), pII->NumAdapters);
		for (LONG i=0; i<pII->NumAdapters; i++)
		{
			OutputString(_T("Index:%d, "), pII->Adapter[i].Index);
			CString str(pII->Adapter[i].Name);
			OutputString(_T("Name:%s\n"), LPCTSTR(str));
			ifrow.dwIndex = pII->Adapter[i].Index;
			dwRetVal = GetIfEntry(&ifrow);
			if (dwRetVal == NO_ERROR)
			{

			}
		}
	}
MIB_IF_OPER_STATUS_OPERATIONAL
	WK_DELETE(pII);
#endif
#ifdef TEST_IPADDRTABLE
	PMIB_IPADDRTABLE pIPAddrTable = NULL;

	// Before calling AddIPAddress we use GetIpAddrTable to get
	// an adapter to which we can add the IP.

	// Make an initial call to GetIpAddrTable to get the
	// necessary size into the dwSize variable
	OutputString(_T("IpAddrTable:\n"));
	dwSize = sizeof(DWORD);
	if (GetIpAddrTable((MIB_IPADDRTABLE *)&dwDummy, &dwSize, 0) == ERROR_INSUFFICIENT_BUFFER) 
	{
		pIPAddrTable = (PMIB_IPADDRTABLE) new BYTE[dwSize];
	}

	// Make a second call to GetIpAddrTable to get the
	// actual data we want
	if ( (dwRetVal = GetIpAddrTable( pIPAddrTable, &dwSize, 0 )) == NO_ERROR ) 
	{ 
		for (DWORD i=0; i<pIPAddrTable->dwNumEntries; i++)
		{
			OutputString(_T(" Address: %08xh, %03d.%03d.%03d.%03d\n"),
				pIPAddrTable->table[i].dwAddr,
				 pIPAddrTable->table[i].dwAddr&0x000000ff,
				(pIPAddrTable->table[i].dwAddr&0x0000ff00)>>8,
				(pIPAddrTable->table[i].dwAddr&0x00ff0000)>>16,
				(pIPAddrTable->table[i].dwAddr&0xff000000)>>24
				);

			OutputString(_T(" Mask:    %08xh, "), pIPAddrTable->table[i].dwMask);
			OutputString(_T(" Index:   %ldd\n"), pIPAddrTable->table[i].dwIndex);
			OutputString(_T(" BCast:   %08xh, "), pIPAddrTable->table[i].dwBCastAddr);
			OutputString(_T(" Reasm:   %ldd\n"), pIPAddrTable->table[i].dwReasmSize);
			if (i==0)
			{
				m_nIPAdapterIndex = pIPAddrTable->table[i].dwIndex;
			}
		}
	}
	else 
	{
		OutputLastError(_T("Call to GetIpAddrTable failed."), dwRetVal);
	}
	WK_DELETE(pIPAddrTable);
#endif

#ifdef TEST_IPNETTABLE
	MIB_IPNETTABLE *pIPNETTABLE = NULL;
	OutputString(_T("IpNetTable:\n"));
	dwSize = sizeof(DWORD);
	dwRetVal = GetIpNetTable((MIB_IPNETTABLE*)&dwDummy, &dwSize, FALSE);
	if (dwRetVal == ERROR_INSUFFICIENT_BUFFER)
	{
		pIPNETTABLE = (MIB_IPNETTABLE*) new BYTE[dwSize];
	}
	dwRetVal = GetIpNetTable(pIPNETTABLE, &dwSize, FALSE);
	if (dwRetVal == NO_ERROR)
	{
		MIB_IPNETROW ipnetrow;
		ZERO_INIT(ipnetrow);

		for (DWORD i=0; i<pIPNETTABLE->dwNumEntries; i++)
		{
			OutputString(_T(" Address: %08xh, %03d.%03d.%03d.%03d\n"),
				pIPNETTABLE->table[i].dwAddr,
				 pIPNETTABLE->table[i].dwAddr&0x000000ff,
				(pIPNETTABLE->table[i].dwAddr&0x0000ff00)>>8,
				(pIPNETTABLE->table[i].dwAddr&0x00ff0000)>>16,
				(pIPNETTABLE->table[i].dwAddr&0xff000000)>>24
				);

			OutputString(_T(" Type:    %ldd, "), pIPNETTABLE->table[i].dwType);
			OutputString(_T(" Index:   %ldd, "), pIPNETTABLE->table[i].dwIndex);
			OutputString(_T(" PAdrLen: %ldd\n"), pIPNETTABLE->table[i].dwPhysAddrLen);
			OutputString(_T(" PAdr: "));

			for (int j=0; j<MAXLEN_PHYSADDR; j++)
			{
				OutputString(_T("%02x "), pIPNETTABLE->table[i].bPhysAddr[j]);
			}
			OutputString(_T("\n"));
			if (i==0)
			{
				ipnetrow.dwIndex = pIPNETTABLE->table[i].dwIndex;
				memcpy(ipnetrow.bPhysAddr, pIPNETTABLE->table[i].bPhysAddr, MAXLEN_PHYSADDR);
				ipnetrow.dwPhysAddrLen = pIPNETTABLE->table[i].dwPhysAddrLen;
				ipnetrow.dwType = pIPNETTABLE->table[i].dwType;
			}
		}

		if (!m_sIPAddress.IsEmpty())
		{
			CWK_String sIPAddress(m_sIPAddress);
			CWK_String sSubnetMask(m_sSubnetMask);
			ipnetrow.dwAddr = inet_addr(sIPAddress);
			dwRetVal = DeleteIpNetEntry(&ipnetrow);
			dwRetVal = SetIpNetEntry(&ipnetrow);
			if (dwRetVal == NO_ERROR) 
			{
				OutputString(_T("SetIpNetEntry ok\n"));
			}
			else 
			{
				OutputLastError(_T("SetIpNetEntry Error. %d\n"), dwRetVal);
			}
		}
	}
	else 
	{
		OutputLastError(_T("Call to GetIpNetTable failed."), dwRetVal);
	}
	WK_DELETE(pIPNETTABLE);
#endif
#endif
}

void CDetermineIPAddressDlg::OnBnClickedBtnTest2()
{
	if (   UpdateData()
		&& !m_sIPAddress.IsEmpty() 
		&& !m_sSubnetMask.IsEmpty()
		&& m_bEnableDHCP == FALSE)
	{
		UINT iaIPAddress;
		UINT imIPMask;
		CWK_String sIPAddress(m_sIPAddress);
		CWK_String sSubnetMask(m_sSubnetMask);
		iaIPAddress = inet_addr(sIPAddress);
		imIPMask = inet_addr(sSubnetMask);

		// Variables where handles to the added IP will be returned
		DWORD dwRetVal;
		ULONG NTEContext = 0;
		ULONG NTEInstance = 0;
		dwRetVal = AddIPAddress(iaIPAddress, imIPMask, m_nIPAdapterIndex, &NTEContext, &NTEInstance);
		if (dwRetVal == NO_ERROR) 
		{
			m_dwaNTEContexts.Add(NTEContext);
			OutputString(_T("IP address added Index:%d, Context:%d, Instance:%d\n"), m_nIPAdapterIndex, NTEContext, NTEInstance);
		}
		else 
		{
			OutputLastError(_T("Error adding IP address."), dwRetVal);
		}
	}
}

void CDetermineIPAddressDlg::OnBnClickedBtnTest3()
{
	if (m_dwaNTEContexts.GetSize())
	{
		DWORD dwRetVal;
		ULONG NTEContext = m_dwaNTEContexts.GetAt(0);
		// Delete the IP we just added using the NTEContext
		// variable where the handle was returned	
		if ((dwRetVal = DeleteIPAddress(NTEContext)) == NO_ERROR)
		{
			m_dwaNTEContexts.RemoveAt(0);
			OutputString(_T(" IP Address Deleted Index:%d, Context:%d.\n"), m_nIPAdapterIndex, NTEContext);
		}
		else
		{		
			OutputLastError(_T(" Call to DeleteIPAddress failed."), dwRetVal);
		}
	}
}

void CDetermineIPAddressDlg::OnIpnFieldchangedIpaddress1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMIPADDRESS pIPAddr = reinterpret_cast<LPNMIPADDRESS>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CDetermineIPAddressDlg::OnBnClickedOk()
{
	UpdateData(TRUE);
//	OnOK();
}
