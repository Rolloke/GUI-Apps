// IPCameraUnitDlg.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "IPCameraUnit.h"
#include "IPCameraUnitDlg.h"

#include "CameraParameterDlg.h"

#include "DSCaptureCamera.h"
#include "DsIpCameraAxis.h"
#include "DsIpCameraPanasonic.h"

#include "IPCamSocket.h"
#include "CIPCOutputIPcamUnit.h"
#include "CIPCInputIpCamUnit.h"
#include "CIPCInputIpCamUnitMDAlarm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TIMER_RESET			10
#define TIMER_INPUT_REQUEST	11
#define TIMER_CHECK			12

#define	TEXT_CTRL			0
#define	LIST_CTRL			1
#define	LIST_CTRL_INPUT		2
#define	LIST_CTRL_OUTPUT	3
#define	STATIC_CTRL_PREVIEW	4

#define LIST_ID			0
#define LIST_NAME		1
#define LIST_TYPE		2
#define LIST_LOCATION	3
#define LIST_STATE		4

#define SETMEDIASTATE   1
#define SETFILTERS		2
#define DELETE_THREAD	3
#define UPDATE_THREAD	4

#define DEBUG_PREVIEW		_T("Preview")
#define DEBUG_REGISTERGRAPH	_T("RegisterGraph")


// TODO! RKE: delete all these test defines
// defines to test this unit in tasha behaviour
#ifdef _TEST_TASHA
#undef SM_IP_CAMERA_OUTPUT_CAMERA
#undef SM_IP_CAMERA_OUTPUT_RELAY
#undef SM_IP_CAMERA_INPUT
#undef SM_IP_CAMERA_MD_INPUT
#define SM_IP_CAMERA_OUTPUT_CAMERA SM_TASHA_OUTPUT_CAMERAS
#define SM_IP_CAMERA_OUTPUT_RELAY  SM_TASHA_OUTPUT_RELAYS
#define SM_IP_CAMERA_INPUT         SM_TASHA_INPUT
#define SM_IP_CAMERA_MD_INPUT      SM_TASHA_MD_INPUT
#endif

CameraParameters::CameraParameters(DWORD dwUserData,
				Resolution res/*=RESOLUTION_NONE*/, 
				Compression comp/*=COMPRESSION_NONE*/, 
				CompressionType ct/*=COMPRESSION_UNKNOWN*/,
				int iFPS/*=0*/, int iIFrameInterval/*=0*/)
{
	m_dwUserData = dwUserData;
	m_res = res;
	m_comp = comp;
	m_ct = ct;
	m_iFPS = iFPS;
	m_iIFrameInterval = iIFrameInterval;
};

/////////////////////////////////////////////////////////////////////////////
// CIpCameraUnitDlg Dialogfeld
CIpCameraUnitDlg::CIpCameraUnitDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIpCameraUnitDlg::IDD, pParent)
	, m_nFps(0)
{
	m_strKey.Format(IPCAM_UNIT_DEVICE, theApp.GetInstanceCount());
	m_strKey = WK_PROFILE_ROOT + m_strKey;
	CWK_Profile wkp(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, m_strKey, _T(""));

	m_sUserName = _T("");
	m_sPassword = _T("");
	m_pHttpServer = NULL;
	m_pUpnpThread = NULL;
	m_pOutputIPcamera = NULL;
	m_pOutputIPrelais = NULL;
	m_pOutputControl = NULL;
	m_pInputIp = NULL;
	m_pInputIpMd = NULL;
	m_sSecID = _T("20010000");
	m_sProxyName = _T("");
	m_sProxyBypass =_T("");

	m_hIcon[0] = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hIcon[1] = AfxGetApp()->LoadIcon(IDR_INITIALISED);
	m_hIcon[2] = AfxGetApp()->LoadIcon(IDR_MOTION_DETECTED);

	m_uResetTimer		= 0;
	m_uInputRequestTimer= 0;
	m_uCheckTimer	    = 0;
	m_bIsShuttingDown	= FALSE;
	m_bThreadIsStarting = FALSE;
	m_nShowCtrl			= LIST_CTRL;
	m_pFrames           = NULL;
	m_pSpans            = NULL;
	m_bUpdateImageSize  = TRUE;
	m_bSet = FALSE;
	m_sURL = _T("");
	m_nPort = 80;
	m_nFps = 5;
	m_sRequest = _T("");
	m_pVideoStateCheck = NULL;

#ifdef _DEBUG
	m_bSet         = theApp.GetProfileInt(SETTINGS_SECTION, P_HTTP_VERB_SET, 0);
	m_sURL         = theApp.GetProfileString(SETTINGS_SECTION, P_URL, _T("192.168.0.233"));
	m_nPort        = theApp.GetProfileInt(SETTINGS_SECTION, P_PORT, 80);
	m_nFps         = theApp.GetProfileInt(SETTINGS_SECTION, P_FPS, 5);
	m_sRequest     = theApp.GetProfileString(SETTINGS_SECTION, P_REQUEST, NULL);
	m_sUserName    = theApp.GetProfileString(SETTINGS_SECTION, P_USERNAME, NULL);
	m_sPassword    = theApp.GetProfileString(SETTINGS_SECTION, P_PASSWORD, NULL);
	m_sProxyName   = theApp.GetProfileString(SETTINGS_SECTION, P_PROXY_NAME, NULL);
	m_sProxyBypass = theApp.GetProfileString(SETTINGS_SECTION, P_PROXY_BYPASS, NULL);
	m_sSecID       = theApp.GetProfileString(SETTINGS_SECTION, P_SEC_ID, m_sSecID);
	Decode(m_sPassword);
	m_bStandalone = TRUE;
#else
	m_bStandalone = wkp.GetInt(DEBUG_SETTINGS, STAND_ALONE, TRUE);
#endif
	if (m_bStandalone)
	{
		m_pFrames = new CStatistics(50);
		m_pSpans  = new CStatistics(25);
	}
}
/////////////////////////////////////////////////////////////////////////////
CIpCameraUnitDlg::~CIpCameraUnitDlg()	// Destruktor
{
	CWK_Profile wkp(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, m_strKey, _T(""));
	
	WK_DELETE(m_pOutputIPcamera);
	WK_DELETE(m_pOutputIPrelais);
	WK_DELETE(m_pInputIp);
	WK_DELETE(m_pInputIpMd);
	WK_DELETE(m_pHttpServer);
	WK_DELETE(m_pOutputControl);
	WK_DELETE(m_pFrames);
	WK_DELETE(m_pSpans);

#ifdef _DEBUG
	theApp.WriteProfileInt(SETTINGS_SECTION, P_HTTP_VERB_SET, m_bSet);
	theApp.WriteProfileString(SETTINGS_SECTION, P_URL, m_sURL);
	theApp.WriteProfileInt(SETTINGS_SECTION, P_PORT, m_nPort);
	theApp.WriteProfileInt(SETTINGS_SECTION, P_FPS, m_nFps);
	theApp.WriteProfileString(SETTINGS_SECTION, P_REQUEST, m_sRequest);
	theApp.WriteProfileString(SETTINGS_SECTION, P_USERNAME, m_sUserName);
	theApp.WriteProfileString(SETTINGS_SECTION, P_SEC_ID, m_sSecID);
	Encode(m_sPassword);
	theApp.WriteProfileString(SETTINGS_SECTION, P_PASSWORD, m_sPassword);
	theApp.WriteProfileString(SETTINGS_SECTION, P_PROXY_NAME, m_sProxyName);
	theApp.WriteProfileString(SETTINGS_SECTION, P_PROXY_BYPASS, m_sProxyBypass);
#else
	wkp.WriteInt(DEBUG_SETTINGS, STAND_ALONE, m_bStandalone);
#endif
}
/////////////////////////////////////////////////////////////////////////////
void CIpCameraUnitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Radio(pDX, IDC_RD_SET, m_bSet);
	DDX_Text(pDX, IDC_EDT_URL, m_sURL);
	DDX_Text(pDX, IDC_EDT_REQUEST, m_sRequest);
	DDX_Control(pDX, IDC_EDT_ANSWER, m_ctrlAnswer);
	DDX_Text(pDX, IDC_EDT_PORT, m_nPort);
	DDV_MinMaxUInt(pDX, m_nPort, 0, 65536);
	DDX_Radio(pDX, IDC_RD_TEXT, m_nShowCtrl);
	DDX_Control(pDX, IDC_THREAD_LIST, m_ThreadList);
	DDX_Text(pDX, IDC_EDT_USER_NAME, m_sUserName);
	DDX_Text(pDX, IDC_EDT_PASSWORD, m_sPassword);
	DDX_Control(pDX, IDC_IN_OUT_LIST, m_IOlist);
	DDX_Text(pDX, IDC_EDT_SEC_ID, m_sSecID);
	DDX_Text(pDX, IDC_EDT_PROXY, m_sProxyName);
	DDX_Text(pDX, IDC_EDT_PROXY_BYPASS, m_sProxyBypass);
	DDX_Text(pDX, IDC_EDT_SET_FPS, m_nFps);
	DDV_MinMaxInt(pDX, m_nFps, 1, 50);
	DDX_Control(pDX, IDC_COMBO_RESOLUTION, m_cResolution);
	DDX_Control(pDX, IDC_COMBO_COMPRESSION_TYPE, m_cCompressionType);
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CIpCameraUnitDlg, CDialog)
	//{{AFX_MSG_MAP(CIpCameraUnitDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_DEVICECHANGE()
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BTN_START, OnBnClickedBtnStart)
	ON_BN_CLICKED(IDC_BTN_PAUSE, OnBnClickedBtnPause)
	ON_BN_CLICKED(IDC_BTN_STOP, OnBnClickedBtnStop)
	ON_BN_CLICKED(IDC_BTN_SEND, OnBnClickedBtnSend)
	ON_BN_CLICKED(IDC_RD_LIST, OnBnClickedRdList)
	ON_BN_CLICKED(IDC_RD_TEXT, OnBnClickedRdText)
	ON_BN_CLICKED(IDC_RD_PREVIEW, OnBnClickedRdPreview)
	ON_BN_CLICKED(IDC_CK_REGISTER_GRAPH, OnBnClickedCkRegisterGraph)
	ON_BN_CLICKED(IDC_BTN_CAMERA_SETTINGS, OnBnClickedBtnCameraSettings)
	ON_NOTIFY(NM_CLICK, IDC_THREAD_LIST, OnNMClickThreadList)
	ON_NOTIFY(LVN_DELETEITEM, IDC_THREAD_LIST, OnLvnDeleteitemThreadList)
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
	ON_UPDATE_COMMAND_UI(ID_APP_EXIT, OnUpdateAppExit)
	ON_COMMAND(ID_SETTINGS, OnSettings)
	ON_UPDATE_COMMAND_UI(ID_SETTINGS, OnUpdateSettings)
	ON_COMMAND(ID_ABOUT, OnAbout)
	ON_UPDATE_COMMAND_UI(ID_ABOUT, OnUpdateAbout)
	ON_MESSAGE(WM_GRAPHNOTIFY, OnGraphNotify)
	ON_MESSAGE(WM_HRESULT_ERROR, OnHresultError)
	ON_MESSAGE(WM_TRAYCLICKED, OnTrayClicked)	
	ON_MESSAGE(WM_SELFCHECK, OnConfirmSelfcheck)
	ON_MESSAGE(WM_INET_STATUS, OnInternetStatus)
	ON_MESSAGE(WM_THREAD_STARTED, OnThreadStarted)
	ON_MESSAGE(WM_SET_CAMERA_ACTIVE, OnSetCameraActive)
	ON_BN_CLICKED(IDC_CK_LOGS_TO_EDT, OnBnClickedCkLogsToEdt)
	ON_BN_CLICKED(IDC_RD_INPUT_LIST, OnBnClickedRdInputList)
	ON_BN_CLICKED(IDC_RD_RELAIS_LIST, OnBnClickedRdRelaisList)
	ON_BN_CLICKED(IDC_BTN_START_THREAD, OnBnClickedBtnStartThread)
	ON_BN_CLICKED(IDC_BTN_STOP_THREAD, OnBnClickedBtnStopThread)
	ON_BN_CLICKED(IDC_BTN_SET_PARAM, OnBnClickedBtnSetParam)
	ON_MESSAGE(WK_WINTHREAD_STOP_THREAD, OnWinthreadStopThread)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
void CIpCameraUnitDlg::ReportError(ErrorStruct*ps, BOOL bMsgBox)
{
	ASSERT(ps != NULL);
	if (FAILED(ps->m_hr))
	{
		CString str, sError, sDescription;
		GetErrorStrings(ps->m_hr, sError, sDescription);
		CString sFile(ps->m_sFile);
		str.Format(_T("Error initialising Filters\n%s(%d) :\nError: %s (%x):\n %s\n"), sFile, ps->m_nLine, sError, ps->m_hr, sDescription);
		WK_TRACE(str);
		if (m_bStandalone && bMsgBox)
		{
			AfxMessageBox(str, MB_ICONERROR|MB_OK);
		}
	}
	delete ps;
}
/////////////////////////////////////////////////////////////////////////////
void CIpCameraUnitDlg::ShowHide()
{
	int nShowThreadlist = SW_HIDE;
	int nShowIOlist  = SW_HIDE;
	int nShowPreview = SW_HIDE;
	BOOL bEnableThreadListCtrls = FALSE;
	if (m_nShowCtrl == LIST_CTRL)
	{
		nShowThreadlist = SW_SHOW;
		bEnableThreadListCtrls = TRUE;
		EnableStateButtons();
	}
	else
	{
		GetDlgItem(IDC_BTN_START)->EnableWindow(bEnableThreadListCtrls);
		GetDlgItem(IDC_BTN_PAUSE)->EnableWindow(bEnableThreadListCtrls);
		GetDlgItem(IDC_BTN_PAUSE)->EnableWindow(bEnableThreadListCtrls);
	}

	if (m_nShowCtrl == STATIC_CTRL_PREVIEW)
	{
		nShowPreview = SW_SHOW;
		CDsCamera*pThread = GetSelectedThread();
		if (pThread)
		{
			pThread->SetFilters(PREVIEWED, (LPARAM) GetDlgItem(IDC_STAT_PICTURE));
		}
	}
	else
	{
		CDsCamera*pThread = GetSelectedThread();
		if (pThread)
		{
			pThread->SetFilters(UNPREVIEWED);
		}
	}
	GetDlgItem(IDC_EDT_ANSWER)->ShowWindow(m_nShowCtrl == TEXT_CTRL ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_THREAD_LIST)->ShowWindow(nShowThreadlist);
	GetDlgItem(IDC_STAT_PICTURE)->ShowWindow(nShowPreview);

	GetDlgItem(IDC_EDT_SEC_ID)->EnableWindow(bEnableThreadListCtrls);
	GetDlgItem(IDC_BTN_START_THREAD)->EnableWindow(bEnableThreadListCtrls);
	GetDlgItem(IDC_BTN_STOP_THREAD)->EnableWindow(bEnableThreadListCtrls);
	GetDlgItem(IDC_CK_REGISTER_GRAPH)->EnableWindow(bEnableThreadListCtrls);
	GetDlgItem(IDC_COMBO_COMPRESSION_TYPE)->EnableWindow(bEnableThreadListCtrls);
	GetDlgItem(IDC_COMBO_RESOLUTION)->EnableWindow(bEnableThreadListCtrls);
	GetDlgItem(IDC_EDT_SET_FPS)->EnableWindow(bEnableThreadListCtrls);
	GetDlgItem(IDC_BTN_SET_PARAM)->EnableWindow(bEnableThreadListCtrls);

	if (m_nShowCtrl == LIST_CTRL_INPUT)
	{
		m_IOlist.DeleteAllItems();
		if (m_pInputIp)
		{
			const CStringArray &saInputs = m_pInputIp->GetInputs();
			int i, n = (int)saInputs.GetCount(), nItem;
			CString s;
			DWORD dwMask, dwAlarmState = m_pInputIp->GetAlarmState();
			for (i=0, dwMask=1; i<n; i++,dwMask<<=1)
			{
				const CString &sInput = saInputs[i];
				s.Format(_T("%d"), i+1);
				nItem = m_IOlist.InsertItem(i, s);
				m_IOlist.SetItemText(nItem, LIST_NAME, CWK_Profile::GetStringFromString(sInput, INI_COMMENT, NULL));
				m_IOlist.SetItemText(nItem, LIST_TYPE, CWK_Profile::GetStringFromString(sInput, INI_INPUT_TYPE, NULL));
				s = CWK_Profile::GetStringFromString(sInput, INI_URL, _T("url")) + _T("|") +
					CWK_Profile::GetStringFromString(sInput, INI_PORT, _T("0")) + _T("|") +
					CWK_Profile::GetStringFromString(sInput, INI_INPUT_NR, _T("1"));
				m_IOlist.SetItemText(nItem, LIST_LOCATION, s);
				m_IOlist.SetItemText(nItem, LIST_STATE, dwMask & dwAlarmState ? _T("1") : _T("0"));
			}
		}
		nShowIOlist = SW_SHOW;
	}
	else if (m_nShowCtrl == LIST_CTRL_OUTPUT)
	{
		m_IOlist.DeleteAllItems();
		if (m_pOutputIPrelais)
		{
			const CStringArray &saRelais = m_pOutputIPrelais->GetRelais();
			int i, n = (int)saRelais.GetCount(), nItem;
			CString s;
			DWORD dwMask, dwRelaisState = m_pOutputIPrelais->GetRelaisState();
			for (dwMask=1, i=0; i<n; i++, dwMask<<=1)
			{
				const CString &sRelais = saRelais[i];
				s.Format(_T("%d"), i+1);
				nItem = m_IOlist.InsertItem(i, s);
				m_IOlist.SetItemText(nItem, LIST_NAME, CWK_Profile::GetStringFromString(sRelais, INI_COMMENT, NULL));
				m_IOlist.SetItemText(nItem, LIST_TYPE, CWK_Profile::GetStringFromString(sRelais, INI_RELAIS_TYPE, NULL));
				s = CWK_Profile::GetStringFromString(sRelais, INI_URL, _T("url")) + _T("|") +
					CWK_Profile::GetStringFromString(sRelais, INI_PORT, _T("0")) + _T("|") +
					CWK_Profile::GetStringFromString(sRelais, INI_RELAIS_NR, _T("1"));
				m_IOlist.SetItemText(nItem, LIST_LOCATION, s);
				m_IOlist.SetItemText(nItem, LIST_STATE, dwMask & dwRelaisState ? _T("1") : _T("0"));
			}
		}
		m_IOlist.ShowWindow(SW_SHOW);
		nShowIOlist = SW_SHOW;
	}
	m_IOlist.ShowWindow(nShowIOlist);
}
void CIpCameraUnitDlg::NotifyIOList(CSecID id, BOOL bOn)
{
	WORD wSubID = id.GetSubID();
	if (id.IsInputID() && m_nShowCtrl == LIST_CTRL_INPUT)
	{
		if (wSubID == SECID_NO_SUBID)
		{
			ShowHide();
		}
		else if (wSubID < m_IOlist.GetItemCount())
		{
			m_IOlist.SetItemText(wSubID, LIST_STATE, bOn ? _T("1") : _T("0"));
		}
	}
	else if (id.IsOutputID() && m_nShowCtrl == LIST_CTRL_OUTPUT)
	{
		if (wSubID == SECID_NO_SUBID)
		{
			ShowHide();
		}
		else if (wSubID < m_IOlist.GetItemCount())
		{
			m_IOlist.SetItemText(wSubID, LIST_STATE, bOn ? _T("1") : _T("0"));
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void	CIpCameraUnitDlg::CheckHidden()
{
	if (m_bStandalone == FALSE)
	{
		ShowWindow(SW_HIDE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIpCameraUnitDlg::EnableStateButtons()
{
	CDirectShowThread*pDST = GetSelectedThread();
	if (pDST)
	{
		OAFilterState State = pDST->GetMediaState();
		GetDlgItem(IDC_BTN_START)->EnableWindow(State != State_Running);
		GetDlgItem(IDC_BTN_PAUSE)->EnableWindow(State != State_Paused);
		GetDlgItem(IDC_BTN_STOP )->EnableWindow(State != State_Stopped);
		CheckDlgButton(IDC_CK_REGISTER_GRAPH, pDST->IsRegistered() ? BST_CHECKED : BST_UNCHECKED);
		GetDlgItem(IDC_BTN_STOP_THREAD)->EnableWindow();
	}
	else
	{
		GetDlgItem(IDC_BTN_STOP_THREAD)->EnableWindow();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIpCameraUnitDlg::StartCaptureCamera(const CString &sName)
{
	DWORD dwFlags = DSTF_RECEIVE_NOTIFICATIONS|DSTF_INIT_FILTERS_AT_START;
	CDsCamera *pThread = new CDsCaptureCamera(sName, dwFlags);
	pThread->SetCameraSubID(1);
	InsertThread(pThread);
}
/////////////////////////////////////////////////////////////////////////////
void CIpCameraUnitDlg::InsertThread(CDsCamera*pThread)
{
	CString sTxt;
//	m_ThreadList.Lock();

	sTxt.Format(_T("%d"), pThread->GetCameraSubID());
	int nItem = m_ThreadList.InsertItem((UINT)LVIF_PARAM|LVIF_TEXT, (int)0, sTxt, 0, 0, 0, (LPARAM)pThread);	// LIST_ID
//	m_ThreadList.SetItemData(nItem, (DWORD_PTR));
	m_ThreadList.SetItemText(nItem, LIST_NAME, pThread->GetName());
	m_ThreadList.SetItemText(nItem, LIST_TYPE, pThread->GetType());
	m_ThreadList.SetItemText(nItem, LIST_LOCATION, pThread->GetLocation());
	m_ThreadList.SetItemText(nItem, LIST_STATE, GetMediaStateText(pThread->GetMediaState()));
	m_ThreadList.SetSelectionMark(nItem);

//	m_ThreadList.Unlock();

	if (m_bThreadIsStarting == FALSE)
	{
		m_bThreadIsStarting = TRUE;		// prohibit starting threads at the same time
		pThread->StartThread();
	}
}
/////////////////////////////////////////////////////////////////////////////
CString CIpCameraUnitDlg::GetMediaStateText(OAFilterState State)
{
	CString sState;
	UINT nID = 0;
	switch (State)
	{
		case State_Running: nID = IDC_BTN_START; break;
		case State_Paused : nID = IDC_BTN_PAUSE; break;
		case State_Stopped: nID = IDC_BTN_STOP; break;
	}
	GetDlgItemText(nID, sState);
	return sState;
}
//////////////////////////////////////////////////////////////////////////////////////
CIPCOutputIPcamUnit*CIpCameraUnitDlg::GetOutput(WORD wID) const
{
	if (wID == CSV_CONTROL_ID)
	{
		return m_pOutputControl;
	}
	return m_pOutputIPcamera;
}
/////////////////////////////////////////////////////////////////////////////
void CIpCameraUnitDlg::RemoveThread(CDsCamera*pThread)
{
	m_ThreadList.Lock();

	int i, n = m_ThreadList.GetItemCount();
	for (i=0; i<n; i++)
	{
		CDirectShowThread*pDST = (CDirectShowThread*) m_ThreadList.GetItemData(i);
		if (pDST == pThread)
		{
			m_ThreadList.DeleteItem(i);
			break;
		}
	}
	m_ThreadList.Unlock();
}
//////////////////////////////////////////////////////////////////////////
void CIpCameraUnitDlg::InitResetTimer()
{
	if (m_uResetTimer == 0)
	{
		m_uResetTimer = (UINT)SetTimer(TIMER_RESET, 1000, NULL);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIpCameraUnitDlg::StartInputRequestTimer()
{
	if (m_uInputRequestTimer == 0)
	{
		m_uInputRequestTimer = (UINT)SetTimer(TIMER_INPUT_REQUEST, 1000, NULL);
	}
}
/////////////////////////////////////////////////////////////////////////////
void	CIpCameraUnitDlg::StopInputRequestTimer()
{
	if (m_uInputRequestTimer)
	{
		KillTimer(m_uInputRequestTimer);
		m_uInputRequestTimer = 0;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIpCameraUnitDlg::SafeKillTimer(UINT & uTimerID)
{
	if (uTimerID)
	{
		KillTimer(uTimerID);
		uTimerID = 0;
	}
}
//////////////////////////////////////////////////////////////////////////
CDsCamera* CIpCameraUnitDlg::GetThreadByID(WORD wID)
{
	CDsCamera* pThreadFind= NULL;

	if (!IsShuttingDown())
	{
		int i, n = m_ThreadList.GetItemCount();
		for (i=0; i<n; i++)
		{
			CDsCamera* pThread = (CDsCamera*)m_ThreadList.GetItemData(i);
			if (pThread && pThread->GetCameraSubID() == wID)
			{
				pThreadFind = pThread;
				break;
			}
		}
	}

	return pThreadFind;
}
//////////////////////////////////////////////////////////////////////////
CDsCamera* CIpCameraUnitDlg::GetSelectedThread()
{
	CDsCamera*pThread = NULL;
	
	if (m_ThreadList.GetItemCount())
	{
		int nSel = m_ThreadList.GetSelectionMark();
		if (nSel == -1)
		{
			nSel = m_ThreadList.SetSelectionMark(0);
		}
		pThread = (CDsCamera*)m_ThreadList.GetItemData(nSel);
	}

	return pThread;
}
//////////////////////////////////////////////////////////////////////////
BOOL CIpCameraUnitDlg::IsCameraActive(WORD wID)
{
	CDsCamera*pThread = GetThreadByID(wID);
	if (pThread)
	{
		return pThread->GetMediaState() == State_Running;
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////
BOOL CIpCameraUnitDlg::IsCameraInitialised(WORD wID)
{
	CDsCamera*pThread = GetThreadByID(wID);
	if (pThread)
	{
		return pThread->IsInitialised();
	}
	
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////
BOOL CIpCameraUnitDlg::IsCameraPresent(WORD wID)
{
	CDsCamera*pThread = GetThreadByID(wID);
	if (pThread && pThread->GetMediaState() == State_Running)
	{
		return TRUE;
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////
BOOL CIpCameraUnitDlg::SetCameraActive(CIPCOutputIPcamUnit*pOutput, WORD wID, bool bActive, CameraParameters*pCP)
{
	m_bUpdateImageSize = TRUE;
	CDsCamera*pThread = GetThreadByID(wID);
	if (pOutput == NULL)
	{
		pCP->m_res  = RESOLUTION_CIF;
		pCP->m_comp = COMPRESSION_10;
		pCP->m_ct   = COMPRESSION_JPEG;
		int nSel = m_cResolution.GetCurSel();
		if (nSel != -1)
		{
			pCP->m_res = (Resolution)m_cResolution.GetItemData(nSel);
		}
		nSel = m_cCompressionType.GetCurSel();
		if (nSel != -1)
		{
			pCP->m_ct = (CompressionType)m_cCompressionType.GetItemData(nSel);
		}
		pCP->m_iFPS = m_nFps;
		pCP->m_iIFrameInterval = 0;
	}
	if (pThread == NULL && bActive && !IsShuttingDown())
	{
		CWK_Profile wkp;
		CString sSection, sCamera, sType, sName, sUrl, sRotation, sComment;
		int nCamera   = -1;
		int nPort     = -1;
		if (pOutput)
		{
			sSection.Format(_T("%s\\") SEC_NAME_GROUP, SEC_NAME_OUTPUTGROUPS, pOutput->GetGroupID());
		}
		else
		{
			CString sGroupID;
			GetDlgItemText(IDC_EDT_SEC_ID, m_sSecID);
			DWORD dwGroup, n = _stscanf(m_sSecID, _T("%x"), &dwGroup);
			if (n==1)
			{
				sSection.Format(_T("%s\\") SEC_NAME_GROUP, SEC_NAME_OUTPUTGROUPS, HIWORD(dwGroup));
				wID = LOWORD(dwGroup);
				if (GetThreadByID(wID))
				{
					return FALSE;
				}
			}
			else
			{
				return FALSE;
			}
		}

		sCamera   = wkp.GetString(sSection, wID+1, NULL);
		sType     = wkp.GetStringFromString(sCamera, INI_CAMERA_TYPE, NULL);
		sName     = wkp.GetStringFromString(sCamera, INI_NAME, sType);
		sComment  = wkp.GetStringFromString(sCamera, INI_COMMENT, sType);
		sUrl      = wkp.GetStringFromString(sCamera, INI_URL, NULL);
		sRotation = wkp.GetStringFromString(sCamera, INI_TURN, _T("0"));
		nCamera   = wkp.GetNrFromString(sCamera, INI_CAMERA_NR, nCamera);
		nPort     = wkp.GetNrFromString(sCamera, INI_PORT, nPort);
		CDsCamera *pThread = NULL;
		DWORD dwFlags = DSTF_INIT_FILTERS_AT_START;
		if (sType.CompareNoCase(CSD_CAM_CAPTURE) == 0)
		{
			dwFlags = DSTF_RECEIVE_NOTIFICATIONS|DSTF_INIT_FILTERS_AT_START|DSTF_START_AFTER_INIT|DSTF_INIT_MEDIA_CONTROL;
			pThread = new CDsCaptureCamera(sName, dwFlags);
			pThread->SetCameraSubID(wID);
		}
		else if (sType.CompareNoCase(CSD_CAM_AXIS) == 0)
		{
#ifdef _DEBUG
			// TODO! RKE: remove later
			dwFlags |= DSTF_NO_GRAPH_INIT;
#endif
			CDsIpCameraAxis *pAxis = new CDsIpCameraAxis(sComment, dwFlags);
			pThread = pAxis;
			pAxis->SetURLandPort(sUrl, nPort);
			pAxis->SetCameraSubID(wID);
			pAxis->SetCameraNo(nCamera);
		}
		else if (sType.CompareNoCase(CSD_CAM_PANASONIC) == 0)
		{
#ifdef _DEBUG
			// TODO! RKE: remove later
			dwFlags |= DSTF_NO_GRAPH_INIT;
#endif
			CDsIpCameraPanasonic *pPanasonic = new CDsIpCameraPanasonic(sName, dwFlags);
			pThread = pPanasonic;
			pPanasonic->SetURLandPort(sUrl, nPort);
			pPanasonic->SetCameraSubID(wID);
		}
		// TODO! RKE: implement other camera protocolls
		if (pThread)
		{
			pThread->SetUserData(pCP->m_dwUserData);
			pThread->SetResolution(pCP->m_res);
			pThread->SetCompression(pCP->m_comp);
			pThread->SetCompressionType(pCP->m_ct);
			pThread->SetFPS(pCP->m_iFPS);
			pThread->SetIFrameInterval(pCP->m_iIFrameInterval);
			pThread->SetRotation(sRotation);
			InsertThread(pThread);
			if (!(dwFlags & DSTF_START_AFTER_INIT))
			{
//				pThread->SetMediaState(State_Running);
			}
		}
	}
	else if (pThread)
	{
		if (bActive)
		{
			pThread->SetUserData(pCP->m_dwUserData);
			pThread->SetResolution(pCP->m_res);
			pThread->SetCompression(pCP->m_comp);
			pThread->SetCompressionType(pCP->m_ct);
			pThread->SetFPS(pCP->m_iFPS);
			pThread->SetIFrameInterval(pCP->m_iIFrameInterval);
		}
		return pThread->SetMediaState(bActive ? State_Running : State_Stopped);
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
DWORD CIpCameraUnitDlg::GetActiveCameraMask(BOOL bRunning)
{
	DWORD dwActive = 0, dwFlag;

	int i, n = m_ThreadList.GetItemCount();
	for (i=0; i<n; i++)
	{
		CDsCamera*pDST = (CDsCamera*) m_ThreadList.GetItemData(i);
		dwFlag = 1 << pDST->GetCameraSubID();
		if (bRunning)
		{
			if (pDST->GetMediaState() == State_Running)
			{
				dwActive |= dwFlag;
			}
		}
		else
		{
			dwActive |= dwFlag;
		}
	}

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CIpCameraUnitDlg::OnRequestGetValue(CSecID id, const CString &sKey, DWORD dwUserData)
{
	CIPCOutputIPcamUnit *pOutput = GetOutput(id.GetSubID());
	if (pOutput)
	{
		if (sKey == CSD_CAM_TYPE)
		{
			CString s, sValue;
			s.LoadString(IDS_TYPE_CAPTURE);
			s += _T(":") CSD_CAM_CAPTURE;
			sValue += s;
			s.LoadString(IDS_TYPE_AXIS);
			s += _T(":") CSD_CAM_AXIS;
			sValue += _T(",") + s;
			s.LoadString(IDS_TYPE_PANASONIC);
			s += _T(":") CSD_CAM_PANASONIC;
			sValue += _T(",") + s;
			pOutput->DoConfirmGetValue(id, sKey, sValue, dwUserData);
			return TRUE;
		}
		else if (sKey == CSD_CAM_AXIS)
		{
			CString s, sValue;
			if (m_pUpnpThread == NULL)
			{
				int i, n = (int)m_saUpnpDevices.GetCount();
				CStringArray sa;
				for (i=0; i<n; i++)
				{
					sa.RemoveAll();
					SplitString(m_saUpnpDevices[i], sa, _T("\n"));
					if (sa[UPNPDEV_FRIENDLYNAME].Find(CSD_CAM_AXIS) != -1)
					{
						s = sa[UPNPDEV_PRESENTATIONURL].Mid(6);
						s.Replace(_T("/"), _T(""));
						sValue += s + _T(":") + sa[UPNPDEV_FRIENDLYNAME] + _T(",");
					}
				}
				RemoveLastCharacter(sValue, _T(','));
				pOutput->DoConfirmGetValue(id, sKey, sValue, dwUserData);
				return TRUE;
			}
			else
			{
				// error
			}
		}
		else if (sKey == CSD_CAM_PANASONIC)
		{
			// TODO! RKE: implement Panasonic
		}

		CDsCamera*pThread = GetThreadByID(id.GetSubID());
		if (pThread)
		{
			pThread->PostThreadMessage(WM_REQUEST_GET_VALUE, (WPARAM)_tcsdup(sKey), (LPARAM)dwUserData);
			return TRUE;
		}
		else
		{
			pOutput->DoIndicateError(0, id, CIPC_ERROR_CAMERA_NOT_PRESENT, 0);
		}
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CIpCameraUnitDlg::OnRequestSetValue(CSecID id, const CString &sKey, const CString &sValue, DWORD dwUserData)
{
	CIPCOutputIPcamUnit *pOutput = GetOutput(id.GetSubID());
	if (sKey == CSD_CAM_TYPE)
	{
		CDsCamera*pThread = GetThreadByID(id.GetSubID());
		if (sValue == CSD_OFF)
		{
			RemoveThread(pThread);
		}
		else if (pThread == NULL)
		{
			CStringArray sa;
			SplitString(sValue, sa, _T(":"));
			if (sa.GetCount())
			{
				if (sa[0] == CSD_CAM_AXIS)
				{
					CDsIpCameraAxis *pThread = new CDsIpCameraAxis(_T("VideoClient"), DSTF_RECEIVE_NOTIFICATIONS);
					pThread->SetURLandPort(sa[1], dwUserData);
					pThread->SetCameraSubID(id.GetSubID());
					InsertThread(pThread);
				}
				else if (sa[0] == CSD_CAM_PANASONIC)
				{
					CDsIpCameraPanasonic*pThread = new CDsIpCameraPanasonic(_T("VideoClient"), DSTF_RECEIVE_NOTIFICATIONS);
					pThread->SetURLandPort(sa[1], dwUserData);
					pThread->SetCameraSubID(id.GetSubID());
					InsertThread(pThread);
				}
				else if (sa[0] == CSD_CAM_CAPTURE)
				{
					CString sName = sValue;
					if (sa.GetCount() >= 2)
					{
						sName = sa[1];
					}
					CDsCaptureCamera*pThread = new CDsCaptureCamera(sName, DSTF_RECEIVE_NOTIFICATIONS);
					pThread->SetCameraSubID(id.GetSubID());
					InsertThread(pThread);
				}
				if (pOutput)
				{
					pOutput->DoConfirmSetValue(id, sKey, sValue, 0);
				}
			}
		}
		return TRUE;
	}
	CDsCamera*pThread = GetThreadByID(id.GetSubID());
	if (pThread)
	{
		CStringArray *psa = new CStringArray;
		psa->Add(sKey);
		psa->Add(sValue);
		pThread->PostThreadMessage(WM_REQUEST_SET_VALUE, (WPARAM)psa, (LPARAM)dwUserData);
		return TRUE;
	}
	else if (pOutput)
	{
		pOutput->DoIndicateError(0, id, CIPC_ERROR_CAMERA_NOT_PRESENT, 0);
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CIpCameraUnitDlg::SetTrayIcon(WORD wColor)
{
	ASSERT(theApp.IsInMainThread());	
	static WORD wCurColor = ICON_NOCOLOR;
	if (wColor != wCurColor)
	{
		wCurColor = wColor;
		HICON hIcon = m_hIcon[0];
		if ((wColor >= ICON_MAIN) && (wColor <= ICON_MD))
		{
			hIcon = m_hIcon[wColor-1];
		}
		if (m_bStandalone)
		{
			::PostMessage(m_hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
		}
		else
		{
			// Icon ins Systemtray
			NOTIFYICONDATA tnd;

			CString sTip = COemGuiApi::GetApplicationName(theApp.GetApplicationId());

			tnd.cbSize				= sizeof(NOTIFYICONDATA);
			tnd.hWnd				= m_hWnd;
			tnd.uID					= 1;
			tnd.uFlags				= NIF_MESSAGE|NIF_ICON|NIF_TIP;
			tnd.uCallbackMessage	= WM_TRAYCLICKED;
			tnd.hIcon				= hIcon;

			lstrcpyn(tnd.szTip, sTip, min(64, sTip.GetLength()+1));

			Shell_NotifyIcon(NIM_MODIFY, &tnd);
			if (::IsWindowVisible(m_hWnd))
			{
				::PostMessage(m_hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIpCameraUnitDlg::SetButtonIcon(int nBtnID, int nIconID)
{
	HANDLE hIcon = ::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(nIconID), IMAGE_ICON, 16,16, LR_DEFAULTCOLOR);
	SendDlgItemMessage(nBtnID, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)hIcon);
	CWnd *pWnd = GetDlgItem(nBtnID);
	if (pWnd && !theApp.IsInMainThread())
	{
		pWnd->UpdateWindow();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIpCameraUnitDlg::SetDSThread(int nWhat, long lParam)
{
	UINT  nSelCount = m_ThreadList.GetSelectedCount();
	if (nSelCount > 1)
	{
		CDWordArray dwa;
		POSITION pos = m_ThreadList.GetFirstSelectedItemPosition();
		while (pos)
		{
			int nSel = m_ThreadList.GetNextSelectedItem(pos);
			CDsCamera*pDST = (CDsCamera*) m_ThreadList.GetItemData(nSel);
			if (pDST)
			{
				if (nWhat == SETMEDIASTATE)
				{
					pDST->SetMediaState((OAFilterState)lParam);
				}
				else if (nWhat == SETFILTERS)
				{
					pDST->SetFilters((int)lParam);
				}
				else if (nWhat == DELETE_THREAD)
				{
					m_ThreadList.Lock();
					m_ThreadList.DeleteItem(nSel);
					m_ThreadList.Unlock();
					pos = m_ThreadList.GetFirstSelectedItemPosition();
				}
				else if (nWhat == UPDATE_THREAD)
				{
					CameraParameters cp((DWORD)0);
					SetCameraActive(NULL, pDST->GetCameraSubID(), true, &cp);
				}
			}
		}
	}
	else if (m_ThreadList.GetItemCount())
	{
		CDsCamera*pDST = GetSelectedThread();
		if (pDST)
		{
			if (nWhat == SETMEDIASTATE)
			{
				pDST->SetMediaState((OAFilterState)lParam);
			}
			else if (nWhat == SETFILTERS)
			{
				pDST->SetFilters((int)lParam);
			}
			else if (nWhat == DELETE_THREAD)
			{
				m_ThreadList.Lock();
				m_ThreadList.DeleteItem(m_ThreadList.GetSelectionMark());
				m_ThreadList.Unlock();
			}
			else if (nWhat == UPDATE_THREAD)
			{
				CameraParameters cp((DWORD)0);
				SetCameraActive(NULL, pDST->GetCameraSubID(), true, &cp);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
UINT CALLBACK CIpCameraUnitDlg::UpnpThread(void*pParam)
{
	CIpCameraUnitDlg*pThis = (CIpCameraUnitDlg*)pParam;
    HRESULT hr = S_OK;
	BOOL bInit = FALSE;
	IUPnPDeviceFinderCallback* pUPnPDeviceFinderCallback = NULL;
    IUPnPDeviceFinder* pUPnPDeviceFinder = NULL;
	LONG lFindData = 0;
	
	pThis->m_saUpnpDevices.RemoveAll();

	try
	{
		hr = CoInitialize(0);
		HRESULT_EXCEPTION(hr);
		bInit = TRUE;

		pUPnPDeviceFinderCallback = new CIpCamDeviceFindCB(pThis);
		if (pUPnPDeviceFinderCallback == NULL) throw HRESULT_ERROR_AT_POS(E_OUTOFMEMORY);
        pUPnPDeviceFinderCallback->AddRef();

        hr = CoCreateInstance(CLSID_UPnPDeviceFinder, NULL, CLSCTX_INPROC_SERVER, 
            IID_IUPnPDeviceFinder, reinterpret_cast<void**>(&pUPnPDeviceFinder));
		HRESULT_EXCEPTION(hr);

		 
        hr = pUPnPDeviceFinder->CreateAsyncFind(L"upnp:rootdevice", 0, pUPnPDeviceFinderCallback, &lFindData); 
		HRESULT_EXCEPTION(hr);

		hr = pUPnPDeviceFinder->StartAsyncFind(lFindData);
		HRESULT_EXCEPTION(hr);

        MSG Message;	// Message loop for find messages
        BOOL bGetMessage = 0;
        while(   0  != (bGetMessage = GetMessage(&Message, NULL, 0, 0))
			  && -1 !=  bGetMessage)
        {
            DispatchMessage(&Message);      
        }
	    pUPnPDeviceFinder->CancelAsyncFind(lFindData);
	}
	catch (ErrorStruct *ps)
	{
		pThis->ReportError(ps, FALSE);
	}

	RELEASE_OBJECT(pUPnPDeviceFinder);
 	RELEASE_OBJECT(pUPnPDeviceFinderCallback);

	if (bInit)
	{
		CoUninitialize();
	}
	pThis->m_pUpnpThread = NULL;
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
UINT CIpCameraUnitDlg::StartUpnpThread()
{
	if (m_pUpnpThread == NULL)
	{
		m_pUpnpThread = AfxBeginThread((AFX_THREADPROC)UpnpThread, (void*)this);
		if (m_pUpnpThread)
		{
			m_pUpnpThread->m_bAutoDelete = TRUE;
			return m_pUpnpThread->m_nThreadID;
		}
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CIpCameraUnitDlg::PostNcDestroy()
{
	delete this;
}
/////////////////////////////////////////////////////////////////////////////
// CIpCameraUnitDlg Meldungshandler
BOOL CIpCameraUnitDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Symbol für dieses Dialogfeld festlegen. Wird automatisch erledigt
	//  wenn das Hauptfenster der Anwendung kein Dialogfeld ist
	SetIcon(m_hIcon[0], TRUE);			// Großes Symbol verwenden
	SetIcon(m_hIcon[0], FALSE);		// Kleines Symbol verwenden

	SetButtonIcon(IDC_BTN_START, IDC_BTN_START);
	SetButtonIcon(IDC_BTN_STOP , IDC_BTN_STOP);
	SetButtonIcon(IDC_BTN_PAUSE, IDC_BTN_PAUSE);

	CString sTxt;
	int nIDs[]    = {IDS_ID, IDS_NAME, IDS_TYPE, IDS_LOCATION, IDS_STATE};
	int nWidths[] = {7     , 35      , 22      , 22          , 14};
	int i, n = sizeof(nIDs) / sizeof(int);
	CRect rc;

	m_ThreadList.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT );
	m_IOlist.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT );
	m_ThreadList.GetClientRect(&rc);
	for (i=0; i<n; i++)
	{
		sTxt.LoadString(nIDs[i]);
		m_ThreadList.InsertColumn(i, sTxt, LVCFMT_LEFT, MulDiv(rc.right, nWidths[i], 100));
		m_IOlist.InsertColumn(i, sTxt, LVCFMT_LEFT, MulDiv(rc.right, nWidths[i], 100));
	}

	CString sUnitSMname, sInstance;
	if (theApp.GetInstanceCount() > 1)
	{
		sInstance.Format(_T("%d"), theApp.GetInstanceCount());
	}

	sUnitSMname = SM_IP_CAMERA_OUTPUT_CAMERA + sInstance;
	m_pOutputIPcamera = new CIPCOutputIPcamUnit(this, sUnitSMname);

	sUnitSMname = SM_IP_CAMERA_OUTPUT_RELAY + sInstance;
	m_pOutputIPrelais = new CIPCOutputIPcamUnit(this, sUnitSMname);

	sUnitSMname = SM_IP_CAMERA_INPUT + sInstance;
	m_pInputIp = new CIPCInputIpCamUnit(this, sUnitSMname);
	m_pInputIp->SetTimeoutCmdReceive(500);

	sUnitSMname = SM_IP_CAMERA_MD_INPUT + sInstance;
	m_pInputIpMd = new CIPCInputIpCamUnitMDAlarm(this, sUnitSMname);

	ShowHide();

	m_uCheckTimer = (UINT)SetTimer(TIMER_CHECK, 1000, NULL);

	for (i=RESOLUTION_2CIF; i<RESOLUTION_LAST; i++)
	{
		n = m_cResolution.AddString(ShortNameOfEnum((Resolution)i));
		m_cResolution.SetItemData(n, i);
	}

	for (i=COMPRESSION_H263; i<COMPRESSION_YUV_420; i++)
	{
		sTxt = NameOfEnum((CompressionType)i);
		sTxt = sTxt.Mid(sTxt.Find(_T("_"))+1);
		n = m_cCompressionType.AddString(sTxt);
		m_cCompressionType.SetItemData(n, i);
	}

	StartUpnpThread();

	RegisterDeviceDetect(m_hWnd, GUID_DEVINTCLASS_VIDEO);

//	StartVideoStateCheckThread();

	return TRUE;  // Geben Sie TRUE zurück, außer ein Steuerelement soll den Fokus erhalten
}
/////////////////////////////////////////////////////////////////////////////
// Wenn Sie dem Dialogfeld eine Schaltfläche "Minimieren" hinzufügen, benötigen Sie 
//  den nachstehenden Code, um das Symbol zu zeichnen. Für MFC-Anwendungen, die das 
//  Dokument/Ansicht-Modell verwenden, wird dies automatisch ausgeführt.
void CIpCameraUnitDlg::OnPaint() 
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
		dc.DrawIcon(x, y, m_hIcon[0]);
	}
	else
	{
		CDialog::OnPaint();
	}
}
/////////////////////////////////////////////////////////////////////////////
// Die System ruft diese Funktion auf, um den Cursor abzufragen, der angezeigt wird, während der Benutzer
//  das minimierte Fenster mit der Maus zieht.
HCURSOR CIpCameraUnitDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon[0]);
}
/////////////////////////////////////////////////////////////////////////////
void CIpCameraUnitDlg::OnBnClickedBtnStart()
{
	SetDSThread(SETMEDIASTATE, State_Running);
}
/////////////////////////////////////////////////////////////////////////////
void CIpCameraUnitDlg::OnBnClickedBtnPause()
{
	SetDSThread(SETMEDIASTATE, State_Paused);
}
/////////////////////////////////////////////////////////////////////////////
void CIpCameraUnitDlg::OnBnClickedBtnStop()
{
	SetDSThread(SETMEDIASTATE, State_Stopped);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CIpCameraUnitDlg::OnGraphNotify(WPARAM wParam, LPARAM lParam)
{
	CDirectShowThread*pThread = (CDirectShowThread*)lParam;
	if (LOWORD(wParam) == DST_NOTIFICATION)
	{
		switch (HIWORD(wParam))
		{
			case EC_DEVICE_LOST:
				ASSERT_KINDOF(CDsCamera, pThread);
				RemoveThread((CDsCamera*)pThread);
				break;
			case EC_MEDIA_STATE_CHANGED:
			{
				int i, nTotalFps = 0, n = m_ThreadList.GetItemCount();
				for (i=0; i<n; i++)
				{
					CDsCamera*pDST = (CDsCamera*) m_ThreadList.GetItemData(i);
					if (pDST == pThread)
					{
						m_ThreadList.SetItemText(i, LIST_STATE, GetMediaStateText(pThread->GetMediaState()));
						TRACE(_T("OnGraphNotify: media state changed on thread %s: %s\n"), pThread->GetName(), GetTickCountEx());
					}
					if (pDST && pDST->GetMediaState() == State_Running)
					{
						nTotalFps += pDST->GetFPS();
					}
				}

				TRACE(_T("Total requested Fps %d\n"), nTotalFps);
				if (nTotalFps > 0)
				{
					nTotalFps *= 2;
					if (m_pFrames)
					{
						m_pFrames->SetNumValues(nTotalFps);
					}
					if (m_pSpans)
					{
						m_pSpans->SetNumValues(nTotalFps);
					}
				}
				EnableStateButtons();
			} break;
		}

	}
	else
	{
		pThread->PostThreadMessage(WM_GRAPHNOTIFY, wParam, lParam);
	}

	return 0;
}
#define TRACE_EVENT(x)  case x: TRACE(_T(#x) _T(" %s\n"), GetTickCountEx()); break;
/////////////////////////////////////////////////////////////////////////////
LRESULT CIpCameraUnitDlg::OnInternetStatus(WPARAM dwContext, LPARAM dwStatus)
{
	switch (dwStatus)
	{
		TRACE_EVENT(INTERNET_STATUS_RESOLVING_NAME);
		TRACE_EVENT(INTERNET_STATUS_NAME_RESOLVED);
		TRACE_EVENT(INTERNET_STATUS_CONNECTING_TO_SERVER);
		TRACE_EVENT(INTERNET_STATUS_CONNECTED_TO_SERVER);
		TRACE_EVENT(INTERNET_STATUS_SENDING_REQUEST);
		TRACE_EVENT(INTERNET_STATUS_REQUEST_SENT);
		TRACE_EVENT(INTERNET_STATUS_RECEIVING_RESPONSE);
		TRACE_EVENT(INTERNET_STATUS_RESPONSE_RECEIVED);
		TRACE_EVENT(INTERNET_STATUS_CTL_RESPONSE_RECEIVED);
		TRACE_EVENT(INTERNET_STATUS_PREFETCH);
		TRACE_EVENT(INTERNET_STATUS_CLOSING_CONNECTION);
		TRACE_EVENT(INTERNET_STATUS_CONNECTION_CLOSED);
		TRACE_EVENT(INTERNET_STATUS_HANDLE_CREATED);
		TRACE_EVENT(INTERNET_STATUS_HANDLE_CLOSING);
		TRACE_EVENT(INTERNET_STATUS_DETECTING_PROXY );
		TRACE_EVENT(INTERNET_STATUS_REQUEST_COMPLETE);
		TRACE_EVENT(INTERNET_STATUS_REDIRECT);
		TRACE_EVENT(INTERNET_STATUS_INTERMEDIATE_RESPONSE);
		TRACE_EVENT(INTERNET_STATUS_USER_INPUT_REQUIRED);
		TRACE_EVENT(INTERNET_STATUS_STATE_CHANGE);
		TRACE_EVENT(INTERNET_STATUS_COOKIE_SENT);
		TRACE_EVENT(INTERNET_STATUS_COOKIE_RECEIVED);
		TRACE_EVENT(INTERNET_STATUS_PRIVACY_IMPACTED);
		TRACE_EVENT(INTERNET_STATUS_P3P_HEADER);
		TRACE_EVENT(INTERNET_STATUS_P3P_POLICYREF);
		TRACE_EVENT(INTERNET_STATUS_COOKIE_HISTORY);
	}
	return 0;
}
#undef TRACE_EVENT
/////////////////////////////////////////////////////////////////////////////
LRESULT CIpCameraUnitDlg::OnThreadStarted(WPARAM, LPARAM)
{
	Sleep(100);							// give the previous thread a little time
	m_bThreadIsStarting = FALSE;		// thread has been started

	int i, n = m_ThreadList.GetItemCount();
	for (i=0; i<n; i++)
	{
		CDsCamera*pDST = (CDsCamera*) m_ThreadList.GetItemData(i);
		if (!pDST->IsThreadRunning())	// search next unstarted thread
		{
			m_bThreadIsStarting = TRUE;	// prohibit starting threads at the same time
			pDST->StartThread();		// and start it
		}
	}

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CIpCameraUnitDlg::OnHresultError(WPARAM wParam, LPARAM lParam)
{
	ReportError((ErrorStruct*)wParam, lParam ? true:false);
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
int CIpCameraUnitDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (m_bStandalone == FALSE)
	{
		// Icon ins Systemtray
		NOTIFYICONDATA tnd;

		CString sTip = COemGuiApi::GetApplicationName(theApp.GetApplicationId());

		tnd.cbSize = sizeof(NOTIFYICONDATA);
		tnd.hWnd	  = m_hWnd;
		tnd.uID    = 1;
		tnd.hIcon  = m_hIcon[0];
		tnd.uFlags = NIF_MESSAGE|NIF_ICON|NIF_TIP;
		tnd.uCallbackMessage = WM_TRAYCLICKED;
		lstrcpyn(tnd.szTip, sTip, sTip.GetLength()+1);
		Shell_NotifyIcon(NIM_ADD, &tnd);
	}

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CIpCameraUnitDlg::OnDestroy()
{
	m_bIsShuttingDown = TRUE;

	StopVideoStateCheckThread();

	SafeKillTimer(m_uResetTimer);
	SafeKillTimer(m_uInputRequestTimer);
	SafeKillTimer(m_uCheckTimer);

	WK_DELETE(m_pOutputIPcamera);
	WK_DELETE(m_pOutputIPrelais);
	WK_DELETE(m_pInputIp);
	WK_DELETE(m_pInputIpMd);
	WK_DELETE(m_pHttpServer);
	WK_DELETE(m_pOutputControl);
	WK_DELETE(m_pFrames);

	if (m_pUpnpThread)
	{
		HANDLE hThread = m_pUpnpThread->m_hThread;
		m_pUpnpThread->PostThreadMessage(WM_QUIT, 0, 0);
		WaitForSingleObject(hThread, INFINITE);
	}

	m_ThreadList.Lock();
	m_ThreadList.DeleteAllItems();
	m_ThreadList.Unlock();

	if (m_bStandalone != TRUE)
	{
		NOTIFYICONDATA tnd;

		tnd.cbSize		= sizeof(NOTIFYICONDATA);
		tnd.hWnd		= m_hWnd;
		tnd.uID			= 1;
		tnd.uFlags		= 0;

		Shell_NotifyIcon(NIM_DELETE, &tnd);
		PostQuitMessage(0);
	}
	
	SetDebuggerWindowHandle(NULL);
	CDialog::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CIpCameraUnitDlg::OnDeviceChange(UINT nEventType, DWORD dwData)
{
	DEV_BROADCAST_HDR *pDBH = (DEV_BROADCAST_HDR*)(LONG_PTR)dwData;
	if (pDBH)
	{
		if (pDBH->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE_EX)
		{
			CDevIntEx diex((DEV_BROADCAST_DEVICEINTERFACE_EX*)(LONG_PTR)dwData);
			if (diex.GetGuidClass() == AM_KSCATEGORY_VIDEO)
			{
				if (nEventType == DBT_DEVICEARRIVAL)
				{
					CString sCamera = diex.GetDeviceName();
					int nFind = sCamera.Find(_T("#"));
					if (nFind != -1)
					{
						sCamera = sCamera.Left(nFind);
						sCamera.TrimRight();
					}
					StartCaptureCamera(sCamera);
				}
				else if (nEventType == DBT_DEVICEREMOVECOMPLETE)
				{
				}
			}
		}
	}
	return TRUE;	
}
/////////////////////////////////////////////////////////////////////////////
void CIpCameraUnitDlg::OnBnClickedBtnSend()
{
	UpdateData();
	CString aValue;
	CHttpFile* pFile = NULL;
	try
	{
		if (m_pHttpServer)
		{
			if (m_pHttpServer->GetServerName() != m_sURL)
			{
				WK_DELETE(m_pHttpServer);

			}
		}
		if (m_pHttpServer == NULL)
		{
			LPCTSTR pszUser     = NULL;
			LPCTSTR pszPassword = NULL;
			if (!m_sUserName.IsEmpty())
			{
				pszUser = m_sUserName;
			}
			if (!m_sPassword.IsEmpty())
			{
				pszPassword = m_sPassword;
			}
			m_pHttpServer = theApp.GetInternetSession()->GetHttpConnection(m_sURL, (INTERNET_PORT)m_nPort, pszUser, pszPassword);
		}

		pFile = m_pHttpServer->OpenRequest(m_bSet ? CHttpConnection::HTTP_VERB_GET: CHttpConnection::HTTP_VERB_PUT, m_sRequest);

		pFile->SendRequest();

		// Check the response to the request
		DWORD dwRet;
		CString sMsg = ReadHTTPAnswer(pFile, dwRet);
		sMsg.Replace(_T("\n"), _T("\r\n"));
		m_ctrlAnswer.SetWindowText(sMsg);

		if (dwRet != HTTP_STATUS_OK)
		{
			// The request failed
			AfxMessageBox(sMsg, MB_OK|MB_ICONERROR, 0);
		}
	}
	catch (CInternetException* pEx)
	{		
		theApp.ReportError(pEx, TRUE);
	}
	if (pFile)
	{
		pFile->Close();
        delete pFile;
	}
}
/////////////////////////////////////////////////////////////////////////////
CString CIpCameraUnitDlg::HTTPRequest(const CString &sUrl, int nPort, const CString &sRequest, DWORD &dwResult, BOOL bPost/*=0*/)
{
	CString sAnswer;
	CHttpConnection*pConnection = NULL;
	CHttpFile* pFile = NULL;
	try
	{
		LPCTSTR pszUser     = NULL;
		LPCTSTR pszPassword = NULL;
		if (!m_sUserName.IsEmpty())
		{
			pszUser = m_sUserName;
		}
		if (!m_sPassword.IsEmpty())
		{
			pszPassword = m_sPassword;
		}
		pConnection = theApp.GetInternetSession()->GetHttpConnection(sUrl, (INTERNET_PORT)nPort, pszUser, pszPassword);

		pFile = pConnection->OpenRequest(bPost ? CHttpConnection::HTTP_VERB_PUT : CHttpConnection::HTTP_VERB_GET, sRequest,
			        NULL, // pstrReferer
					1,
					NULL, // ppstrAcceptTypes
					NULL, // pstrVersion
					INTERNET_FLAG_RELOAD|INTERNET_FLAG_DONT_CACHE);
		pFile->SendRequest();

		// Check the response to the request
		sAnswer = ReadHTTPAnswer(pFile, dwResult);

		if (dwResult != HTTP_STATUS_OK)
		{
			TRACE(sAnswer);
//			AfxMessageBox(sAnswer, MB_OK|MB_ICONERROR, 0);
		}
	}
	catch (CInternetException* pEx)
	{		
		theApp.ReportError(pEx);
	}
	if (pFile)
	{
		pFile->Close();
        delete pFile;
	}
	if (pConnection)
	{
		pConnection->Close();
		delete pConnection;
	}
	return sAnswer;
}
/////////////////////////////////////////////////////////////////////////////
CString CIpCameraUnitDlg::ReadHTTPAnswer(CHttpFile*pFile, DWORD &dwResult, int nCount/*=50*/)
{
	pFile->QueryInfoStatusCode(dwResult);
		
	CString sMsg;
	BYTE szBuff[1024];
	UINT nRead;
	int i = 0;
	do 
	{
		int nLen = min(pFile->GetLength(), 1023);
		if (nLen == 0)
		{
			break;
		}
		nRead = pFile->Read(szBuff, nLen);
		szBuff[nRead] = '\0';
		CString s(szBuff);
		sMsg += s;
		Sleep(0);
	}while (nRead > 0 && i++<nCount);

	if (dwResult != HTTP_STATUS_OK)
	{
		WK_TRACE(_T("%s\n%s"), NameOfHTTPStatus(dwResult), sMsg);
	}
	
	return sMsg;
}
/////////////////////////////////////////////////////////////////////////////
void CIpCameraUnitDlg::AddPictureStatistic(long lLen)
{
	if (m_pFrames)
	{
		m_pFrames->AddValue(lLen);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIpCameraUnitDlg::AddSpanStatistic(long lLen)
{
	if (m_pSpans)
	{
		m_pSpans->AddValue(lLen);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIpCameraUnitDlg::UpdateImageSize(CSize sz)
{
	Resolution r = EnumOfSize(sz);
	CString s, sRes = ShortNameOfEnum(r);
	s.Format(_T("%s: (%dx%d)"), sRes, sz.cx, sz.cy);
	SetDlgItemText(IDC_TXT_PICTURE_SIZE, s);
	m_bUpdateImageSize = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CIpCameraUnitDlg::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == m_uResetTimer)
	{
		SafeKillTimer(nIDEvent);
	
		m_ThreadList.Lock();
		m_ThreadList.DeleteAllItems();
		m_ThreadList.Unlock();

		if (m_pOutputIPcamera)
		{
			m_pOutputIPcamera->ConfirmReset();
		}
		if (m_pOutputIPrelais)
		{
			m_pOutputIPrelais->ConfirmReset();
		}
		StartUpnpThread();
	}
	else if (nIDEvent == m_uInputRequestTimer && m_pInputIp)
	{
		m_pInputIp->CheckInputs();
	}
	else if (nIDEvent == m_uCheckTimer)
	{
		if (m_pFrames && m_pSpans)
		{
			CString s;
			s.Format(_T("Fps: %.1f, Bps:%d, Read timespans: %d"), m_pFrames->GetFPS() ,m_pFrames->GetBitrate(), m_pSpans->GetAverageValue());
			SetDlgItemText(IDC_TXT_FPS, s);
		}

		int i, n = m_ThreadList.GetItemCount();
		for (i=0; i<n; i++)
		{
			CDirectShowThread*pDST = (CDirectShowThread*) m_ThreadList.GetItemData(i);
			if (pDST->GetOutStandingConfirms() > 5)
			{
				m_ThreadList.Lock();
				m_ThreadList.DeleteItem(i);
				m_ThreadList.Unlock();
				break;
			}
			pDST->DoSelfCheck();
		}
	}

	CDialog::OnTimer(nIDEvent);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CIpCameraUnitDlg::OnTrayClicked(WPARAM wParam, LPARAM lParam)
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
			
			COemGuiApi::DoUpdatePopupMenu(theApp.GetDlg(), pM);
			SetForegroundWindow();		// Siehe ID: Q135788 
			pM->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this);
			PostMessage(WM_NULL, 0, 0); // Siehe ID: Q135788
			break;
		}
	}

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CIpCameraUnitDlg::OnCancel()
{
	if	(m_bStandalone)
	{
		OnOK(); // close dialog
	}
	else
	{
		ShowWindow(SW_HIDE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIpCameraUnitDlg::OnOK()
{
	CDialog::OnOK();
	m_bIsShuttingDown = TRUE;

	m_ThreadList.Lock();
	m_ThreadList.DeleteAllItems();
	m_ThreadList.Unlock();

	DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CIpCameraUnitDlg::OnAppExit()
{
	DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CIpCameraUnitDlg::OnUpdateAppExit(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
}
/////////////////////////////////////////////////////////////////////////////
void CIpCameraUnitDlg::OnSettings()
{
//	COEMLoginDialog dlg(this);
//	dlg.DoModal();
//	CPermission perm = dlg.GetPermission();

	SetWindowPos(&CWnd::wndTop, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);
	SetForegroundWindow(); 
}
/////////////////////////////////////////////////////////////////////////////
void CIpCameraUnitDlg::OnUpdateSettings(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
}
/////////////////////////////////////////////////////////////////////////////
void CIpCameraUnitDlg::OnAbout()
{
	COemGuiApi::AboutDialog(this);
}
/////////////////////////////////////////////////////////////////////////////
void CIpCameraUnitDlg::OnUpdateAbout(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CIpCameraUnitDlg::OnConfirmSelfcheck(WPARAM wParam, LPARAM lParam)
{
	HWND hWnd = (HWND)wParam;

	if (hWnd && IsWindow(hWnd))
	{
		::PostMessage(hWnd, WM_SELFCHECK, (WPARAM)theApp.GetApplicationId(), 0);
	}

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CIpCameraUnitDlg::OnSetCameraActive(WPARAM wParam, LPARAM lParam)
{
	CameraParameters*pCP = (CameraParameters*) wParam;
	WORD wCamSubID = LOWORD(lParam);
	bool bActive   = HIWORD(lParam) ? true : false;
	if (pCP)
	{
		SetCameraActive(GetOutput(wCamSubID), wCamSubID, bActive, pCP);
		delete pCP;
	}
	else
	{
		CameraParameters cp((DWORD)0);
		SetCameraActive(GetOutput(wCamSubID), wCamSubID, bActive, &cp);
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CIpCameraUnitDlg::OnBnClickedRdText()
{
	m_nShowCtrl = TEXT_CTRL;
	ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
void CIpCameraUnitDlg::OnBnClickedRdList()
{
	m_nShowCtrl = LIST_CTRL;
	ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
void CIpCameraUnitDlg::OnBnClickedRdInputList()
{
	m_nShowCtrl = LIST_CTRL_INPUT;
	ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
void CIpCameraUnitDlg::OnBnClickedRdRelaisList()
{
	m_nShowCtrl = LIST_CTRL_OUTPUT;
	ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
void CIpCameraUnitDlg::OnBnClickedRdPreview()
{
	m_nShowCtrl = STATIC_CTRL_PREVIEW;
	ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
void CIpCameraUnitDlg::OnBnClickedCkRegisterGraph()
{
	SetDSThread(SETFILTERS, IsDlgButtonChecked(IDC_CK_REGISTER_GRAPH) == BST_CHECKED ? REGISTERED : UNREGISTERED);
}
/////////////////////////////////////////////////////////////////////////////
void CIpCameraUnitDlg::OnNMClickThreadList(NMHDR *pNMHDR, LRESULT *pResult)
{
	EnableStateButtons();
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CIpCameraUnitDlg::OnLvnDeleteitemThreadList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if (pNMLV->lParam)
	{
		CDirectShowThread*pThread = (CDirectShowThread*) pNMLV->lParam;
		if (pThread->IsRunning())
		{
			pThread->StopThread();
		}
	}
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CIpCameraUnitDlg::OnBnClickedBtnCameraSettings()
{
	m_pOutputControl = new CIPCOutputIPcamUnit(this, SM_IP_CAMERA_OUTPUT_CAMERA _T("Control"));
	CCameraParameterDlg dlg;
	dlg.DoModal();
	WK_DELETE(m_pOutputControl);
	RemoveThread(GetThreadByID(CSV_CONTROL_ID));
}
/////////////////////////////////////////////////////////////////////////////
void CIpCameraUnitDlg::OnBnClickedCkLogsToEdt()
{
	if (IsDlgButtonChecked(IDC_CK_LOGS_TO_EDT))
	{
		SetDebuggerWindowHandle(GetDlgItem(IDC_EDT_ANSWER)->m_hWnd);
	}
	else
	{
		SetDebuggerWindowHandle(NULL);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIpCameraUnitDlg::OnBnClickedBtnStartThread()
{
	if (UpdateData())
	{
		CameraParameters cp((DWORD)0);
		SetCameraActive(NULL, (WORD)-1, true, &cp);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIpCameraUnitDlg::OnBnClickedBtnStopThread()
{
	SetDSThread(DELETE_THREAD, 0);
}
/////////////////////////////////////////////////////////////////////////////
void CIpCameraUnitDlg::OnBnClickedBtnSetParam()
{
	if (UpdateData())
	{
		SetDSThread(UPDATE_THREAD, 0);
	}
}
//////////////////////////////////////////////////////////////////////////
LRESULT CIpCameraUnitDlg::OnWinthreadStopThread(WPARAM wParam, LPARAM lParam)
{
	if (wParam == TRUE)
	{
		m_pOutputControl = new CIPCOutputIPcamUnit(this, SM_IP_CAMERA_OUTPUT_CAMERA _T("Control"));
		return wParam;
	}
	else if (wParam == FALSE)
	{
		WK_DELETE(m_pOutputControl);
		RemoveThread(GetThreadByID(CSV_CONTROL_ID));
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CIpCameraUnitDlg::StartVideoStateCheckThread()
{
	BOOL bResult = FALSE;

	if (m_pVideoStateCheck == NULL)
	{
		m_pVideoStateCheck = new CVideoStateCheckThread(_T("VideoStateCheck"));
		if (m_pVideoStateCheck)
			bResult = m_pVideoStateCheck->StartThread();
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CIpCameraUnitDlg::StopVideoStateCheckThread()
{
	BOOL bResult = FALSE;

	if (m_pVideoStateCheck)
	{
		bResult = m_pVideoStateCheck->StopThread();
		m_pVideoStateCheck = NULL;
		bResult = TRUE;
	}

	return bResult;
}
