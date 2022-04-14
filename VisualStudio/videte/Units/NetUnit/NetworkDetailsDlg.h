#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#define CONNECTION_TYPE_INPUT    1
#define CONNECTION_TYPE_OUTPUT   2
#define CONNECTION_TYPE_AUDIO    3
#define CONNECTION_TYPE_DATABASE 4

class CIPCControlInterface;
class CGateway;
// CNetworkDetailsDlg dialog

class CNetworkDetailsDlg : public CDialog
{
	DECLARE_DYNAMIC(CNetworkDetailsDlg)

//Konstruktor / Destruktor/////////////////////////////////////////////////////////////////////////
public:
	CNetworkDetailsDlg(CWnd* pParent = NULL);   // standard constructor
	CNetworkDetailsDlg(CIPCControlInterface* pCIPCControlInterface, CWnd* pParent /*=NULL*/);
	virtual ~CNetworkDetailsDlg();

//Attribute////////////////////////////////////////////////////////////////////////////////////////


//Methoden/////////////////////////////////////////////////////////////////////////////////////////
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNetUnitDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog(void);
	//}}AFX_VIRTUAL

protected:
	BOOL InitDialogMembers(BOOL bCheckRemoteHosts);
	static UINT NetworkStatusThreadProc(LPVOID pParam);	// Thread-Prozedur zur Anzeige des Status' einer Netzwerkverbindung

	DWORD GetBitrateViaLocalAddressAndType(CList<CGateway*,CGateway*> *lGatewayList, 
                                           CString sLocalAddress,
                                           int iConnectionType);

	DWORD GetBitrateViaRemoteHostnameAndType(CList<CGateway*,CGateway*> *lGatewayList, 
                                             CString sRemoteHostname,
                                             int iConnectionType);

	CString GetLocalAddressViaRemoteHostnameAndType(CList<CGateway*,CGateway*>* lGatewayList,
                                                    CString sRemoteHostname,
                                                    DWORD dwType);

	CString GetRemoteAddressViaRemoteHostnameAndType(CList<CGateway*,CGateway*>* lGatewayList,
                                                     CString sRemoteHostname,
                                                     DWORD dwType);

	DWORD GetMaxBitrate(CList<CGateway*,CGateway*> *lGatewayList,
                        CString sCurNetworkAdapter);

	// Generated message map functions
	//{{AFX_MSG(CNetUnitDlg)
	afx_msg void OnCbnSelchangeComboNetworkadapter();
	afx_msg void OnBnClickedButtonCloseCurCon();
	afx_msg void OnBnClickedOk();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dialog Data
	enum { IDD = IDD_DIALOG_NETWORKDETAILS };

private:
	CProgressCtrl         m_progressCtrlBitrateSum;			// ProgressCtrl zur Anzeige gesamten Bitrate
	CProgressCtrl         m_progressCtrlBitrateInput;		// ProgressCtrl zur Anzeige der Input-Bitrate
	CProgressCtrl         m_progressCtrlBitrateOutput;		// ProgressCtrl zur Anzeige der Output-Bitrate
	CProgressCtrl         m_progressCtrlBitrateAudio;		// ProgressCtrl zur Anzeige der Audio-Bitrate
	CProgressCtrl         m_progressCtrlBitrateDatabase;	// ProgressCtrl zur Anzeige der Database-Bitrate
	CIPCControlInterface* m_pCIPCControlInterface;			// CIPCControlInterface
	CWinThread*           m_pNetworkStatusThread;			// Thread fuer die Anzeige des Status' einer Netzwerkverbindung
	BOOL                  m_bNetworkDetailsThreadsRunning;	// ueberprueft das Starten und Beenden der Netwokdetails-Threads
	CComboBox             m_comboBoxRemoteHost;				// ComboBox fuer die Netzwerkadapter
	CString               m_sCurRemoteHost;					// aktuelle Gegenstation
};
