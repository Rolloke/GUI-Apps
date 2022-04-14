// NetworkDetailsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NetUnit.h"
#include "NetworkDetailsDlg.h"
#include "nfc\ipcservercontrolinterface.h"
#include "nfc\Gateway.h"
#include "nfc\NIC.h"


// CNetworkDetailsDlg dialog

IMPLEMENT_DYNAMIC(CNetworkDetailsDlg, CDialog)

/*------
*
* Name: CNetworkDetailsDlg()
*
* Zweck: Standard-Konstruktor
*
* Eingabe-/Ausgabeparameter: (I) (CWnd*) pParent: parent window
*
* Rueckgabewert: -
*
* Datum: 08.04.2004
*
* letzte Aenderung: 08.04.2004
*
* Autor: Andreas Hagen
*
------*/
CNetworkDetailsDlg::CNetworkDetailsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNetworkDetailsDlg::IDD, pParent)
{
	m_pCIPCControlInterface         = NULL;
	m_bNetworkDetailsThreadsRunning = FALSE;
	m_sCurRemoteHost                = _T("");
}



/*------
*
* Name: CNetworkDetailsDlg()
*
* Zweck: Konstruktor #2
*
* Eingabe-/Ausgabeparameter: (I) (CIPCControlInterface*) pCIPCControlInterface: CIPCControlInterface
*                            (I) (CWnd*) pParent: parent window
*
* Rueckgabewert: -
*
* Datum: 08.04.2004
*
* letzte Aenderung: 08.04.2004
*
* Autor: Andreas Hagen
*
------*/
CNetworkDetailsDlg::CNetworkDetailsDlg(CIPCControlInterface* pCIPCControlInterface, CWnd* pParent /*=NULL*/)
: CDialog(CNetworkDetailsDlg::IDD, pParent)
{
	m_pCIPCControlInterface         = pCIPCControlInterface;
	m_pNetworkStatusThread          = NULL;
	m_bNetworkDetailsThreadsRunning = FALSE;
	m_sCurRemoteHost                = _T("");
}



/*------
*
* Name: ~CNetworkDetailsDlg()
*
* Zweck: Destruktor
*
* Eingabe-/Ausgabeparameter: (I) (CIPCControlInterface*) pCIPCControlInterface: CIPCControlInterface
*                            (I) (CWnd*) pParent: parent window
*
* Rueckgabewert: -
*
* Datum: 08.04.2004
*
* letzte Aenderung: 13.04.2004
*
* Autor: Andreas Hagen
*
------*/
CNetworkDetailsDlg::~CNetworkDetailsDlg()
{
	m_bNetworkDetailsThreadsRunning = FALSE;

	// NetworkStatus-Thread: erst auf Beendigung warten und dann loeschen
	if(m_pNetworkStatusThread)
	{
		WaitForSingleObject(m_pNetworkStatusThread->m_hThread, 5000);
		delete m_pNetworkStatusThread;
	}

	Sleep(200);
}



/*------
*
* Name: DoDataExchange()
*
* Zweck: DataExchange
*
* Eingabe-/Ausgabeparameter: -
*
* Rueckgabewert: -
*
* Datum: 08.04.2004
*
* letzte Aenderung: 08.04.2004
*
* Autor: Andreas Hagen
*
------*/
void CNetworkDetailsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_BITRATE_SUM, m_progressCtrlBitrateSum);
	DDX_Control(pDX, IDC_PROGRESS_BITRATE_INPUT, m_progressCtrlBitrateInput);
	DDX_Control(pDX, IDC_PROGRESS_BITRATE_OUTPUT, m_progressCtrlBitrateOutput);
	DDX_Control(pDX, IDC_PROGRESS_BITRATE_AUDIO, m_progressCtrlBitrateAudio);
	DDX_Control(pDX, IDC_PROGRESS_BITRATE_DATABASE, m_progressCtrlBitrateDatabase);
	DDX_Control(pDX, IDC_COMBO_NETWORKADAPTER, m_comboBoxRemoteHost);
}


BEGIN_MESSAGE_MAP(CNetworkDetailsDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_NETWORKADAPTER, OnCbnSelchangeComboNetworkadapter)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE_CUR_CON, OnBnClickedButtonCloseCurCon)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CNetworkDetailsDlg message handlers

/*------
*
* Name: OnInitDialog()
*
* Zweck: Dialog-Initialisierung
*
* Eingabe-/Ausgabeparameter: -
*
* Rueckgabewert: -
*
* Datum: 08.04.2004
*
* letzte Aenderung: 19.04.2004
*
* Autor: Andreas Hagen
*
------*/
BOOL CNetworkDetailsDlg::OnInitDialog(void)
{
	CDialog::OnInitDialog();

/*
	CList<CGateway*,CGateway*> *lGatewayList;
	CGateway* pGateway = NULL;
	CString sCurRemoteHost;
	CNIC NIC;
	DWORD dwRemoteHostNICType;
	BOOL bNICTypeOk = TRUE;

	// Gateway-List aus m_pCIPCControlInterface uebernehmen
	lGatewayList = m_pCIPCControlInterface->GetGatewayList();

	// Netzwerkadapter-ComboBox initialisieren
	m_comboBoxRemoteHost.ResetContent();

	// alle Gegenstationen ermitteln und in die Combobox eintragen
	POSITION pos = lGatewayList->GetHeadPosition();
	while(pos != NULL)
	{
		pGateway = lGatewayList->GetNext(pos);
		sCurRemoteHost = pGateway->GetRemoteHostname();

		dwRemoteHostNICType = NIC.GetNICType(pGateway->GetLocalAddress());

		if(dwRemoteHostNICType == MIB_IF_TYPE_ETHERNET)
			sCurRemoteHost = _T("tcp:") + sCurRemoteHost;
		else if(dwRemoteHostNICType == MIB_IF_TYPE_PPP)
			sCurRemoteHost = _T("dun:") + sCurRemoteHost;
		else
			bNICTypeOk = FALSE;		

		if(bNICTypeOk)
		{
			// sCurRemoteHost in der ComboBox suchen
			if(m_comboBoxRemoteHost.FindString(0, sCurRemoteHost) == CB_ERR)
			{	// sCurRemoteHost wurde noch nicht eingetragen
				m_comboBoxRemoteHost.AddString(sCurRemoteHost);
			}
		}
	}

	m_comboBoxRemoteHost.SetCurSel(0);
*/

	// Dialog-Felder initialisieren
	InitDialogMembers(TRUE);

	// Anzeige-Thread starten
	if(!m_pNetworkStatusThread && m_pCIPCControlInterface)
	{
		m_bNetworkDetailsThreadsRunning = TRUE;
		m_pNetworkStatusThread = AfxBeginThread(NetworkStatusThreadProc, (LPVOID)this);//, THREAD_PRIORITY_LOWEST);
		if(m_pNetworkStatusThread == NULL)
		{
			TRACE(_T("NetworkStatus-Thread konnte nicht erstellt werden!\n"));
		}
		else
		{
			// m_pNetworkStatusThread soll im Destruktor geloescht werden
			m_pNetworkStatusThread->m_bAutoDelete = FALSE;
		}
	}

	return false;
}



/*------
*
* Name: NetworkStatusThreadProc()
*
* Zweck: Thread-Prozedur zur Anzeige des Status' einer Netzwerkverbindung
*
* Eingabe-/Ausgabeparameter: (LPVOID) pParam
*
* Rueckgabewert: (UINT)
*
* Datum: 13.04.2004
*
* letzte Aenderung: 19.04.2004
*
* Autor: Andreas Hagen
*
------*/
UINT CNetworkDetailsDlg::NetworkStatusThreadProc(LPVOID pParam)
{
	CNetworkDetailsDlg* pNDDlg = (CNetworkDetailsDlg*) pParam;
	DWORD dwMaxBitrate      = 0;
	DWORD dwUsedBitrate     = 0;
	DWORD dwInputBitrate    = 0;
	DWORD dwOutputBitrate   = 0;
	DWORD dwAudioBitrate    = 0;
	DWORD dwDatabaseBitrate = 0;
	CList<CGateway*,CGateway*> *lGatewayList;
	CString sCurRemoteHost   = _T("");
	CString sBitrateSum      = _T("0 kBit/s");
	CString sBitrateInput    = _T("0 kBit/s");
	CString sBitrateOutput   = _T("0 kBit/s");
	CString sBitrateAudio    = _T("0 kBit/s");
	CString sBitrateDatabase = _T("0 kBit/s");

	if(pNDDlg)
	{
		// Gateway-List aus m_pCIPCControlInterface uebernehmen
		lGatewayList = pNDDlg->m_pCIPCControlInterface->GetGatewayList();

		// Ist die GatewayList leer?
		if(!lGatewayList->IsEmpty())
		{	// GatewayList ist nicht leer
			while(pNDDlg->m_hWnd && pNDDlg->m_bNetworkDetailsThreadsRunning)
			{
				// maximale Bitrate der Verbindung ermitteln
				dwMaxBitrate = pNDDlg->GetMaxBitrate(lGatewayList, pNDDlg->m_sCurRemoteHost);

				// Range der einzelnen ProgressCtrls setzen
				pNDDlg->m_progressCtrlBitrateSum.SetRange(0, (short)(dwMaxBitrate/1000));
				pNDDlg->m_progressCtrlBitrateInput.SetRange(0, (short)(dwMaxBitrate/1000));
				pNDDlg->m_progressCtrlBitrateOutput.SetRange(0, (short)(dwMaxBitrate/1000));
				pNDDlg->m_progressCtrlBitrateAudio.SetRange(0, (short)(dwMaxBitrate/1000));
				pNDDlg->m_progressCtrlBitrateDatabase.SetRange(0, (short)(dwMaxBitrate/1000));

				// Inputbitrate ermitteln und anzeigen
				dwInputBitrate = pNDDlg->GetBitrateViaRemoteHostnameAndType(lGatewayList,
                                                                            //sCurRemoteHost,
																			pNDDlg->m_sCurRemoteHost,
                                                                            CONNECTION_TYPE_INPUT);

				pNDDlg->m_progressCtrlBitrateInput.SetPos(dwInputBitrate/1000);

				sBitrateInput.Format(_T("%d kBit/s"), dwInputBitrate/1000);
				pNDDlg->SetDlgItemText(IDC_STATIC_BITRATEINPUT_NR, sBitrateInput);

				// Outputbitrate ermitteln und anzeigen
				dwOutputBitrate = pNDDlg->GetBitrateViaRemoteHostnameAndType(lGatewayList,
                                                                             //sCurRemoteHost,
                                                                             pNDDlg->m_sCurRemoteHost,
                                                                             CONNECTION_TYPE_OUTPUT);

				pNDDlg->m_progressCtrlBitrateOutput.SetPos(dwOutputBitrate/1000);

				sBitrateOutput.Format(_T("%d kBit/s"), dwOutputBitrate/1000);
				pNDDlg->SetDlgItemText(IDC_STATIC_BITRATEOUTPUT_NR, sBitrateOutput);

				// Audiobitrate ermitteln und anzeigen
				dwAudioBitrate = pNDDlg->GetBitrateViaRemoteHostnameAndType(lGatewayList,
                                                                            //sCurRemoteHost,
																			pNDDlg->m_sCurRemoteHost,
                                                                            CONNECTION_TYPE_AUDIO);

				pNDDlg->m_progressCtrlBitrateAudio.SetPos(dwAudioBitrate/1000);

				sBitrateAudio.Format(_T("%d kBit/s"), dwAudioBitrate/1000);
				pNDDlg->SetDlgItemText(IDC_STATIC_BITRATEAUDIO_NR, sBitrateAudio);

				// Databasebitrate ermitteln und anzeigen
				dwDatabaseBitrate = pNDDlg->GetBitrateViaRemoteHostnameAndType(lGatewayList,
                                                                               //sCurRemoteHost,
																			   pNDDlg->m_sCurRemoteHost,
                                                                               CONNECTION_TYPE_DATABASE);

				pNDDlg->m_progressCtrlBitrateDatabase.SetPos(dwDatabaseBitrate/1000);

				sBitrateDatabase.Format(_T("%d kBit/s"), dwDatabaseBitrate/1000);
				pNDDlg->SetDlgItemText(IDC_STATIC_BITRATEDATABASE_NR, sBitrateDatabase);

				// Gesamtbitrate ermitteln und anzeigen
				//for(int k=0; k<iCountGateway; k++)
				//{
				//	dwUsedBitrate += pGateway[k]->GetNetInterface()->GetSyncTCPSocket()->GetDataSizePerSec();
				//}
				dwUsedBitrate = dwInputBitrate+dwOutputBitrate+dwAudioBitrate+dwDatabaseBitrate;

				pNDDlg->m_progressCtrlBitrateSum.SetPos(dwUsedBitrate/1000);

				sBitrateSum.Format(_T("%d kBit/s"), dwUsedBitrate/1000);
				pNDDlg->SetDlgItemText(IDC_STATIC_BITRATESUM_NR, sBitrateSum);

				//TRACE(_T("Verbindung: %s..."), pGateway[0]->GetLocalAddress());
				//TRACE(_T("Verbindung: %s..."), pNDDlg->m_sCurRemoteHost);
				//TRACE(_T("Gesamt: %d kBit/s...Input: %d kBit/s...Output: %d kBit/s...Audio: %d kBit/s...Database: %d kBit/s\n"), dwUsedBitrate/1024, dwInputBitrate/1024, dwOutputBitrate/1024, dwAudioBitrate/1024, dwDatabaseBitrate/1024);

				// reset
				dwUsedBitrate     = 0;
				dwInputBitrate    = 0;
				dwOutputBitrate   = 0;
				dwAudioBitrate    = 0;
				dwDatabaseBitrate = 0;
				//}
				//else
				//{
				//	TRACE(_T("[CNetworkDetailsDlg::NetworkStatusThreadProc] Fehler beim Ermitteln der selektierten Gegenstation\n"));
				//}

				Sleep(100);
			}
		}
		else
		{
			TRACE(_T("[CNetworkDetailsDlg::NetworkStatusThreadProc] Gateway-List ist leer\n"));
		}
	}

	return 0x0DEC;
}



/*------
*
* Name: GetBitrateViaLocalAddressAndType()
*
* Zweck: Sucht die Bitrate einer Verbindung gemaess LocalAddress und Verbindungstyp in der Gateway-List
*
* Eingabe-/Ausgabeparameter: (CList<CGateway*,CGateway*>*) lGatewayList: Zeiger auf die Gateway-List
*                            (CString) sLocalAddress: Netzwerkadapter, ueber den die Verbindung etabliert wurde
*                            (int)   iConnectionType: Verbindungstyp (also CIPC_INPUT_CLIENT, CIPC_OUTPUT_CLIENT etc.)
*
* Rueckgabewert: (DWORD) ermittelte Bitrate
*
* Datum: 16.04.2004
*
* letzte Aenderung: 16.04.2004
*
* Autor: Andreas Hagen
*
------*/
DWORD CNetworkDetailsDlg::GetBitrateViaLocalAddressAndType(CList<CGateway*,CGateway*> *lGatewayList, 
														   CString sLocalAddress,
														   int iConnectionType)
{
	DWORD dwBitrate;
	int iCountGateway;
	int iCIPCConnectionType;
	CGateway* pGateway;
    
	dwBitrate           = 0;
	iCountGateway       = 0;
	iCIPCConnectionType = 0;

	// Anzahl der Gateways ermitteln
	iCountGateway = lGatewayList->GetCount();

	// Gateway gemaess sLocalAddress und iConnectionType in der GatewayList suchen
	POSITION pos = lGatewayList->GetHeadPosition();
	for(int i=0; pos != NULL && i < iCountGateway; i++)
	{
		pGateway = lGatewayList->GetNext(pos);

		if(iConnectionType == CONNECTION_TYPE_INPUT)
		{
			if(pGateway->IsServer())
				iCIPCConnectionType = CIPC_INPUT_SERVER;
			else
				iCIPCConnectionType = CIPC_INPUT_CLIENT;
		}
		else if(iConnectionType == CONNECTION_TYPE_OUTPUT)
		{
			if(pGateway->IsServer())
				iCIPCConnectionType = CIPC_OUTPUT_SERVER;
			else
				iCIPCConnectionType = CIPC_OUTPUT_CLIENT;
		}
		else if(iConnectionType == CONNECTION_TYPE_AUDIO)
		{
			if(pGateway->IsServer())
				iCIPCConnectionType = CIPC_AUDIO_SERVER;
			else
				iCIPCConnectionType = CIPC_AUDIO_CLIENT;
		}
		else if(iConnectionType == CONNECTION_TYPE_DATABASE)
		{
			if(pGateway->IsServer())
				iCIPCConnectionType = CIPC_DATABASE_SERVER;
			else
				iCIPCConnectionType = CIPC_DATABASE_CLIENT;
		}

		if(    pGateway->GetLocalAddress() == sLocalAddress 
			&& pGateway->GetConnectionTypeA() == iCIPCConnectionType)
		{
			dwBitrate = pGateway->GetNetInterface()->GetSyncTCPSocket()->GetDataSizePerSec();
			break;
		}
	}
	
	return dwBitrate;
}



/*------
*
* Name: GetBitrateViaRemoteHostnameAndType()
*
* Zweck: Sucht die Bitrate einer Verbindung gemaess Gegenstationsname und Verbindungstyp in der Gateway-List
*
* Eingabe-/Ausgabeparameter: (CList<CGateway*,CGateway*>*) lGatewayList: Zeiger auf die Gateway-List
*                            (CString) sRemoteHostname: Name der Gegenstation
*                            (int)     iConnectionType: Verbindungstyp (also CIPC_INPUT_CLIENT, CIPC_OUTPUT_CLIENT etc.)
*
* Rueckgabewert: (DWORD) ermittelte Bitrate
*
* Datum: 19.04.2004
*
* letzte Aenderung: 19.04.2004
*
* Autor: Andreas Hagen
*
* Hinweis: zusammengehoerende CIPC-Connection-Types:
*
*               Receiver                             idip/DTS
*               --------                             --------
*               CIPC_INPUT_CLIENT    <->   CIPC_INPUT_SERVER
*               CIPC_OUTPUT_CLIENT   <->   CIPC_OUTPUT_SERVER
*
*          Daraus ergibt sich, dass beim Receiver anstatt der Input-Anzeige
*          die Output-Anzeige ausschlaegt. Dies ist zwar richtig, jedoch irrefuehrend.
*          Deshalb sind die Input/Output-Anzeigen beim Receiver vertauscht.
*
------*/
DWORD CNetworkDetailsDlg::GetBitrateViaRemoteHostnameAndType(CList<CGateway*,CGateway*> *lGatewayList, 
													  	     CString sRemoteHostname,
														     int iConnectionType)
{
	DWORD dwBitrate;
	int iCountGateway;
	int iCIPCConnectionType;
	int iConnectionMedium;
	CGateway* pGateway;
	CNIC NIC;

	dwBitrate           = 0;
	iCountGateway       = 0;
	iCIPCConnectionType = 0;
	iConnectionMedium   = 0;

	// TCP oder DUN-Verbindung
	if (sRemoteHostname.Find(_T("tcp:")) != -1)
	{
		iConnectionMedium = MIB_IF_TYPE_ETHERNET;
	}
	else if(sRemoteHostname.Find(_T("dun:")) != -1)
	{
		iConnectionMedium = MIB_IF_TYPE_PPP;
	}

	// "tcp:" oder "dun:" entfernen
	sRemoteHostname = sRemoteHostname.Mid(4);

	// Anzahl der Gateways ermitteln
	iCountGateway = lGatewayList->GetCount();

	// Gateway gemaess sRemoteHostname und iConnectionType in der GatewayList suchen
	POSITION pos = lGatewayList->GetHeadPosition();
	for(int i=0; pos != NULL && i < iCountGateway; i++)
	{
		pGateway = lGatewayList->GetNext(pos);

		if(iConnectionType == CONNECTION_TYPE_INPUT)
		{
			if(pGateway->IsServer())
				iCIPCConnectionType = CIPC_INPUT_SERVER;
			else
				iCIPCConnectionType = CIPC_OUTPUT_CLIENT;
		}
		else if(iConnectionType == CONNECTION_TYPE_OUTPUT)
		{
			if(pGateway->IsServer())
				iCIPCConnectionType = CIPC_OUTPUT_SERVER;
			else
				iCIPCConnectionType = CIPC_INPUT_CLIENT;
		}
		else if(iConnectionType == CONNECTION_TYPE_AUDIO)
		{
			if(pGateway->IsServer())
				iCIPCConnectionType = CIPC_AUDIO_SERVER;
			else
				iCIPCConnectionType = CIPC_AUDIO_CLIENT;
		}
		else if(iConnectionType == CONNECTION_TYPE_DATABASE)
		{
			if(pGateway->IsServer())
				iCIPCConnectionType = CIPC_DATABASE_SERVER;
			else
				iCIPCConnectionType = CIPC_DATABASE_CLIENT;
		}

		if(    pGateway->GetRemoteHostname()  == sRemoteHostname 
			&& pGateway->GetConnectionTypeA() == iCIPCConnectionType
			&& NIC.GetNICType(pGateway->GetLocalAddress()) == (DWORD)iConnectionMedium)
		{
			dwBitrate = pGateway->GetNetInterface()->GetSyncTCPSocket()->GetDataSizePerSec();
			break;
		}
	}

	return dwBitrate;
}



/*------
*
* Name: OnCbnSelchangeComboNetworkadapter()
*
* Zweck: SelchangeComboNetworkadapter-Handler
*
* Eingabe-/Ausgabeparameter: -
*
* Rueckgabewert: -
*
* Datum: 16.04.2004
*
* letzte Aenderung: 16.04.2004
*
* Autor: Andreas Hagen
*
------*/
void CNetworkDetailsDlg::OnCbnSelchangeComboNetworkadapter()
{
	TRACE(_T("[CNetworkDetailsDlg::OnCbnSelchangeComboNetworkadapter] Networkadapter changed\n"));

	// momentan selektierte Gegenstation ermitteln
	if(m_comboBoxRemoteHost.GetCount() != 0)
	{
		m_comboBoxRemoteHost.GetLBText(m_comboBoxRemoteHost.GetCurSel(), m_sCurRemoteHost);
	
		InitDialogMembers(FALSE);
	}
}



/*------
*
* Name: GetMaxBitrate()
*
* Zweck: Liefert die maximale Bitrate des aktuellen Netzwerkadapters zurueck
*
* Eingabe-/Ausgabeparameter: (I) (CList<CGateway*,CGateway*>*) lGatewayList: Zeiger auf die Gateway-List
*                            (I) (CString) sCurRemoteHostname: aktuelle Gegenstation
*
* Rueckgabewert: (DWORD) maximale Bitrate
*
* Datum: 16.04.2004
*
* letzte Aenderung: 19.04.2004
*
* Autor: Andreas Hagen
*
------*/
DWORD CNetworkDetailsDlg::GetMaxBitrate(CList<CGateway*,CGateway*> *lGatewayList,
										CString sCurRemoteHostname)
{
	DWORD dwBitrate;
	int iConnectionMedium;
	CGateway* pGateway;
	CNIC NIC;

	dwBitrate         = 0;
	pGateway          = NULL;
	iConnectionMedium = 0;

	// TCP oder DUN-Verbindung
	if (sCurRemoteHostname.Find(_T("tcp:")) != -1)
	{
		iConnectionMedium = MIB_IF_TYPE_ETHERNET;
	}
	else if(sCurRemoteHostname.Find(_T("dun:")) != -1)
	{
		iConnectionMedium = MIB_IF_TYPE_PPP;
	}

	// "tcp:" oder "dun:" entfernen
	sCurRemoteHostname = sCurRemoteHostname.Mid(4);

	POSITION pos = lGatewayList->GetHeadPosition();
	while(pos != NULL)
	{
		pGateway = lGatewayList->GetNext(pos);
		if(    pGateway->GetRemoteHostname() == sCurRemoteHostname
			&& NIC.GetNICType(pGateway->GetLocalAddress()) == (DWORD)iConnectionMedium)
		{
			//if(iConnectionMedium == MIB_IF_TYPE_ETHERNET)
			//{
				//dwBitrate = pGateway->GetNetUnitParameter().m_dwSelectedBitrate;
			//}
			//else if(iConnectionMedium == MIB_IF_TYPE_PPP)
			//{
				dwBitrate = NIC.GetBitrate(pGateway->GetLocalAddress());
			//}
			break;
		}
	}

	return dwBitrate;
}



/*------
*
* Name: OnBnClickedButtonCloseCurCon()
*
* Zweck: CloseCurCon-Handler
*
* Eingabe-/Ausgabeparameter: -
*
* Rueckgabewert: -
*
* Datum: 19.04.2004
*
* letzte Aenderung: 19.04.2004
*
* Autor: Andreas Hagen
*
------*/
void CNetworkDetailsDlg::OnBnClickedButtonCloseCurCon()
{
	CString sCurRemoteHost;
	DWORD dwConMedium;

	sCurRemoteHost = _T("");
	dwConMedium    = 0;

	// momentan selektierte Gegenstation ermitteln
	if(m_comboBoxRemoteHost.GetCount() != 0)
	{
		m_comboBoxRemoteHost.GetLBText(m_comboBoxRemoteHost.GetCurSel(), sCurRemoteHost);

		// TCP oder DUN-Verbindung
		if (sCurRemoteHost.Find(_T("tcp:")) != -1)
		{
			dwConMedium = MIB_IF_TYPE_ETHERNET;
		}
		else if(m_sCurRemoteHost.Find(_T("dun:")) != -1)
		{
			dwConMedium = MIB_IF_TYPE_PPP;
		}

		// "tcp:" oder "dun:" entfernen
		sCurRemoteHost = sCurRemoteHost.Mid(4);

		if(sCurRemoteHost != _T(""))
		{
			// alle Gateways zur Gegenstation beenden
			//m_pCIPCControlInterface->CloseAllConsViaRemoteHostname(sCurRemoteHost);
			m_pCIPCControlInterface->CloseAllConsViaRemoteHostnameAndMedium(sCurRemoteHost, dwConMedium);
		}
		else
		{
			TRACE(_T("[CNetworkDetailsDlg::OnBnClickedButtonCloseCurCon] kein Gegenstationsname vorhanden\n"));
		}
	}
	else
	{
		TRACE(_T("[CNetworkDetailsDlg::OnBnClickedButtonCloseCurCon] ComboBox der Gegenstationen ist leer\n"));
	}
}


/*------
*
* Name: InitDialogMembers()
*
* Zweck: Setzt die Werte relevanter Dialog-Felder
*
* Eingabe-/Ausgabeparameter: (BOOL) bCheckRemoteHosts: legt fest, ob die ComboBox der Gegenstation
*                                                      aktualisiert werden soll
*
* Rueckgabewert: (BOOL)
*
* Datum: 16.04.2004
*
* letzte Aenderung: 19.04.2004
*
* Autor: Andreas Hagen
*
------*/
BOOL CNetworkDetailsDlg::InitDialogMembers(BOOL bCheckRemoteHosts)
{
	CString sCurRemoteHost;
	CString sMaxBitrate;
	CString sLocalAddress;
	CString sRemoteAddress;
	DWORD dwMaxBitrate;
	DWORD dwRemoteHostNICType;
	CList<CGateway*,CGateway*> *lGatewayList;
	CGateway* pGateway;
	CNIC NIC;
	BOOL bNICTypeOk;
	BOOL bRet;
	int iConnectionMedium;

	sCurRemoteHost    = _T("");
	sMaxBitrate       = _T("");
	sLocalAddress     = _T("");
	sRemoteAddress    = _T("");
	dwMaxBitrate      = 0;
	pGateway          = NULL;
	bNICTypeOk        = TRUE;
	bRet              = TRUE;
	iConnectionMedium = 0;

	// Gateway-List aus m_pCIPCControlInterface uebernehmen
	lGatewayList = m_pCIPCControlInterface->GetGatewayList();

	if(bCheckRemoteHosts)
	{
		// Netzwerkadapter-ComboBox initialisieren
		m_comboBoxRemoteHost.ResetContent();

		// alle Gegenstationen ermitteln und in die Combobox eintragen
		POSITION pos = lGatewayList->GetHeadPosition();
		while(pos != NULL)
		{
			pGateway = lGatewayList->GetNext(pos);

			//TRACE(_T("#### conType: %s\n"), DefineToString(pGateway->GetConnectionTypeA()));

			sCurRemoteHost = pGateway->GetRemoteHostname();

			dwRemoteHostNICType = NIC.GetNICType(pGateway->GetLocalAddress());

			if(dwRemoteHostNICType == MIB_IF_TYPE_ETHERNET)
				sCurRemoteHost = _T("tcp:") + sCurRemoteHost;
			else if(dwRemoteHostNICType == MIB_IF_TYPE_PPP)
				sCurRemoteHost = _T("dun:") + sCurRemoteHost;
			else
				bNICTypeOk = FALSE;		

			if(bNICTypeOk)
			{
				// sCurRemoteHost in der ComboBox suchen
				if(m_comboBoxRemoteHost.FindString(0, sCurRemoteHost) == CB_ERR)
				{	// sCurRemoteHost wurde noch nicht eingetragen
					m_comboBoxRemoteHost.AddString(sCurRemoteHost);
				}
			}
		}

		m_comboBoxRemoteHost.SetCurSel(0);
	}


	// momentan selektierte Gegenstation ermitteln
	if(m_comboBoxRemoteHost.GetCount() != 0)
	{
		m_comboBoxRemoteHost.GetLBText(m_comboBoxRemoteHost.GetCurSel(), m_sCurRemoteHost);

		if(m_sCurRemoteHost != _T(""))
		{
			// TCP oder DUN-Verbindung
			if (m_sCurRemoteHost.Find(_T("tcp:")) != -1)
			{
				iConnectionMedium = MIB_IF_TYPE_ETHERNET;
			}
			else if(m_sCurRemoteHost.Find(_T("dun:")) != -1)
			{
				iConnectionMedium = MIB_IF_TYPE_PPP;
			}

			// "tcp:" oder "dun:" entfernen
			sCurRemoteHost = m_sCurRemoteHost.Mid(4);

			// maximale Bitrate ermitteln und setzen
			dwMaxBitrate = GetMaxBitrate(lGatewayList, m_sCurRemoteHost);
			sMaxBitrate.Format(_T("%d kBit/s"), dwMaxBitrate/1000);
			SetDlgItemText(IDC_STATIC_MAX_BITRATE_NR, sMaxBitrate);

			// lokale IP-Adresse ermitteln und setzen
			sLocalAddress = GetLocalAddressViaRemoteHostnameAndType(lGatewayList, sCurRemoteHost, iConnectionMedium);
			SetDlgItemText(IDC_STATIC_LOCAL_IP_ADDRESS_NR, sLocalAddress);

			// Remote-IP-Adresse ermitteln und setzen
			sRemoteAddress = GetRemoteAddressViaRemoteHostnameAndType(lGatewayList, sCurRemoteHost, iConnectionMedium);
			SetDlgItemText(IDC_STATIC_REMOTE_IP_ADDRESS_NR, sRemoteAddress);


			// Verbindungstyp setzen
			if(iConnectionMedium == MIB_IF_TYPE_ETHERNET)
			{
				SetDlgItemText(IDC_STATIC_CON_TYPE_NR, _T("TCP/IP"));
			}
			else if(iConnectionMedium == MIB_IF_TYPE_PPP)
			{
				SetDlgItemText(IDC_STATIC_CON_TYPE_NR, _T("DUN"));
			}
		}
	}

	return bRet;
}



/*------
*
* Name: GetLocalAddressViaRemoteHostnameAndType()
*
* Zweck: Sucht die lokale IP-Adresse gemaess Gegenstationsname und Netzwerktyp
*        in der Gateway-List.
*
* Eingabe-/Ausgabeparameter: (CList<CGateway*,CGateway*>*) lGatewayList: Zeiger auf die Gateway-List
*                            (CString) sRemoteHostname: Name der Gegenstation
*                            (DWORD)            dwType: Netzwerktyp (also MIB_IF_TYPE_ETHERNET oder MIB_IF_TYPE_PPP)
*
* Rueckgabewert: (CString) lokale IP-Adresse
*
* Datum: 16.04.2004
*
* letzte Aenderung: 19.04.2004
*
* Autor: Andreas Hagen
*
------*/
CString CNetworkDetailsDlg::GetLocalAddressViaRemoteHostnameAndType(CList<CGateway*,CGateway*>* lGatewayList,
																	CString sRemoteHostname,
																	DWORD dwType)
{
	CString sLocalAddress;
	CGateway* pGateway;
	CNIC NIC;

	pGateway      = NULL;
	sLocalAddress = _T("");

	POSITION pos = lGatewayList->GetHeadPosition();
	while(pos != NULL)
	{
		pGateway = lGatewayList->GetNext(pos);
		if(    pGateway->GetRemoteHostname() == sRemoteHostname
			&& NIC.GetNICType(pGateway->GetLocalAddress()) == dwType)
		{
			sLocalAddress = pGateway->GetLocalAddress();
			break;
		}
	}

	return sLocalAddress;
}



/*------
*
* Name: GetRemoteAddressViaRemoteHostnameAndType()
*
* Zweck: Sucht die IP-Adresse Gegenstation gemaess Gegenstationsname und Netzwerktyp
*        in der Gateway-List.
*
* Eingabe-/Ausgabeparameter: (CList<CGateway*,CGateway*>*) lGatewayList: Zeiger auf die Gateway-List
*                            (CString) sRemoteHostname: Name der Gegenstation
*                            (DWORD)            dwType: Netzwerktyp (also MIB_IF_TYPE_ETHERNET oder MIB_IF_TYPE_PPP)
*
* Rueckgabewert: (CString) IP-Adresse der Gegennstation
*
* Datum: 16.04.2004
*
* letzte Aenderung: 19.04.2004
*
* Autor: Andreas Hagen
*
------*/
CString CNetworkDetailsDlg::GetRemoteAddressViaRemoteHostnameAndType(CList<CGateway*,CGateway*>* lGatewayList,
																 	 CString sRemoteHostname,
																	 DWORD dwType)
{
	CString sRemoteAddress;
	CGateway* pGateway;
	CNIC NIC;

	pGateway       = NULL;
	sRemoteAddress = _T("");

	POSITION pos = lGatewayList->GetHeadPosition();
	while(pos != NULL)
	{
		pGateway = lGatewayList->GetNext(pos);
		if(    pGateway->GetRemoteHostname() == sRemoteHostname
			&& NIC.GetNICType(pGateway->GetLocalAddress()) == dwType)
		{
			sRemoteAddress = pGateway->GetRemoteAddress();
			break;
		}
	}

	return sRemoteAddress;
}



void CNetworkDetailsDlg::OnBnClickedOk()
{
	m_bNetworkDetailsThreadsRunning = FALSE;

	//Sleep(200);

	// NetworkStatus-Thread: erst auf Beendigung warten und dann loeschen
	//if(m_pNetworkStatusThread)
	//{
	//	//WaitForSingleObject(m_pNetworkStatusThread->m_hThread, 5000);
	//	WaitForSingleObject(m_pNetworkStatusThread->m_hThread, INFINITE);
	//	delete m_pNetworkStatusThread;
	//}

	OnOK();
}
