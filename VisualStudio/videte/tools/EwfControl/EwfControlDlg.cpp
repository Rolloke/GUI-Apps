// EwfControlDlg.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "EwfControl.h"
#include "EwfControlDlg.h"
#include ".\ewfcontroldlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// autorun, if nessesary

#define COMMON_RAM_THRESHOLD	_T("EwfRamThreshold")

/////////////////////////////////////////////////////////////////////////////
// CEwfControlDlg Dialogfeld
CEwfControlDlg::CEwfControlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEwfControlDlg::IDD, pParent)
{
	m_bCommit  = FALSE;
	m_bDisable = FALSE;
	m_bEnable  = FALSE;
	m_nEndsession = RD_ES_REBOOT;

	m_bEwfEnabled = FALSE;
	m_bQueryEndSession = FALSE;
	m_bShellIconSet = FALSE;
	ZERO_INIT(m_ovEventLowSpace);
	m_pLowSpaceThread = NULL;
	m_llLowSpace = 0;
	m_bAsync = FALSE;
	m_bSilent = FALSE;
	m_nShutDownTimer = 0;
	m_nShutdownCounter = 0;
	m_nControlTimer = 0;
	m_dwCurrentDataSize = 0;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	CWK_Profile wkp;
	CString sProtect = wkp.GetString(SECTION_COMMON, COMMON_COMPACT_FLASH, NULL);
	if (sProtect.IsEmpty())
	{
		sProtect = theApp.GetProfileString(PROFILE_COMMON, PROFILE_DONTCHECK, _T("c:"));
	}
	m_nEwfRamThreshold = wkp.GetInt(SECTION_COMMON, COMMON_RAM_THRESHOLD, 25);
	m_cProtectedDrive = sProtect.GetAt(0);
	GlobalMemoryStatus(&m_MemoryStatus);
}
/////////////////////////////////////////////////////////////////////////////
CEwfControlDlg::~CEwfControlDlg()	// Destruktor
{
	if (m_pLowSpaceThread)
	{
		TerminateThread(m_pLowSpaceThread->m_hThread, 0);
		WK_DELETE(m_pLowSpaceThread);
	}
	if (m_ovEventLowSpace.hEvent)
	{
		CloseHandle(m_ovEventLowSpace.hEvent);
	}
	WK_DELETE(m_pEwf);
}
/////////////////////////////////////////////////////////////////////////////
void CEwfControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEwfControlDlg)
	DDX_Check(pDX, IDC_CK_COMMIT_DATA, m_bCommit);
	DDX_Check(pDX, IDC_CK_DISABLE_EWF, m_bDisable);
	DDX_Check(pDX, IDC_CK_ENABLE_EWF, m_bEnable);
	DDX_Radio(pDX, IDC_RD_LOGOFF, m_nEndsession);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CEwfControlDlg, CDialog)
	//{{AFX_MSG_MAP(CEwfControlDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_MESSAGE(WM_QUERYENDSESSION, OnQueryEndSession)
	ON_MESSAGE(WM_USER, OnUser)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CK_ENABLE_EWF, OnBnClickedCkEnableEwf)
	ON_BN_CLICKED(IDC_CK_DISABLE_EWF, OnBnClickedCkDisableEwf)
	ON_BN_CLICKED(IDC_CK_COMMIT_DATA, OnBnClickedCkCommitData)
	ON_BN_CLICKED(IDC_CLOSE, OnBnClickedClose)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_RD_LOGOFF, OnBnClickedRd)
	ON_BN_CLICKED(IDC_RD_REBOOT, OnBnClickedRd)
	ON_BN_CLICKED(IDC_RD_SHUTDOWN, OnBnClickedRd)
	//}}AFX_MSG_MAP
	ON_WM_ENDSESSION()
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CEwfControlDlg Meldungshandler
BOOL CEwfControlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CString sDrives;

	SetIcon(m_hIcon, TRUE);			// Großes Symbol verwenden
	SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden

	CWK_Profile wkp;
	if (wkp.GetInt(SECTION_COMMON, COMMON_DISPLAYEWFDIALOG, -1) == -1)
	{
		wkp.WriteInt(SECTION_COMMON, COMMON_DISPLAYEWFDIALOG, 0);
	}
	if (wkp.GetInt(SECTION_COMMON, COMMON_RAM_THRESHOLD, -1) == -1)
	{
		wkp.WriteInt(SECTION_COMMON, COMMON_RAM_THRESHOLD, 25);
	}

#ifdef _DEBUG
	//Test();
#endif
	GetDlgItemText(IDC_TXT_SHUTDOWN_TIME, m_sShutDown);
	m_pEwf = new CEwf(m_cProtectedDrive, m_bAsync);
	if (m_pEwf->IsValid())
	{
		if (InitEwfConfig(TRUE))
		{
			CString sFmt, str;
			GetDlgItemText(IDC_TXTV_EWF_COMMIT_MSG, sFmt);
			str.Format(sFmt, m_cProtectedDrive);
			SetDlgItemText(IDC_TXTV_EWF_COMMIT_MSG, str);
		}
		else
		{
			WK_TRACE(_T("Error InitEwfConfig: %s\n"), GetLastErrorString());
#ifndef _DEBUG
			if (   !m_bSilent 
				&& AfxMessageBox(IDS_EWF_ERR_CONFIG ,MB_YESNO|MB_ICONERROR) == IDNO)
			{	// RKE: fake um AutoRun nicht zu disablen
				m_bEwfEnabled = TRUE;
			}
			PostMessage(WM_USER, WM_QUIT);
#endif
		}
	}
	else
	{
		AfxMessageBox(IDS_EWF_ERR_NO_API, MB_OK|MB_ICONERROR);
		PostMessage(WM_USER, WM_QUIT);
	}

#ifdef _DEBUG
	PostMessage(WM_USER, MAKELONG(WM_SHOWWINDOW, SW_SHOW));
#endif

	return TRUE;  // Geben Sie TRUE zurück, außer ein Steuerelement soll den Fokus erhalten
}
/////////////////////////////////////////////////////////////////////////////
// Wenn Sie dem Dialogfeld eine Schaltfläche "Minimieren" hinzufügen, benötigen Sie 
//  den nachstehenden Code, um das Symbol zu zeichnen. Für MFC-Anwendungen, die das 
//  Dokument/Ansicht-Modell verwenden, wird dies automatisch ausgeführt.
void CEwfControlDlg::OnPaint() 
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
/////////////////////////////////////////////////////////////////////////////
// Die System ruft diese Funktion auf, um den Cursor abzufragen, der angezeigt wird, während der Benutzer
//  das minimierte Fenster mit der Maus zieht.
HCURSOR CEwfControlDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
/////////////////////////////////////////////////////////////////////////////
void CEwfControlDlg::OnBnClickedOk()
{
	UINT uExitWindows = 0;
	UINT uDoExitWindows = 0;
	UpdateData();
	switch (m_nEndsession)			// init the endsession parameters
	{
		case RD_ES_LOGOFF  : uExitWindows = EWX_LOGOFF   | EWX_FORCE; break;
		case RD_ES_REBOOT  : uExitWindows = EWX_REBOOT   | EWX_FORCE; break;
		case RD_ES_SHUTDOWN: uExitWindows = EWX_SHUTDOWN | EWX_FORCE; break;
	}

	if (m_pEwf)
	{

		if (m_bEwfEnabled)				// if the Ewf is enabled
		{
			if (m_bCommit)				
			{
				if (m_bDisable)
				{
					m_pEwf->CommitAndDisableLive();
				}
				else
				{
					m_pEwf->Commit();		// commit data
				}
				uDoExitWindows = uExitWindows;// needs reboot
			}

			if (m_bDisable && !m_bCommit)
			{
				BOOL bCommit = FALSE;
				if (m_pEwf->GetVolumeConfig())
				{
					if (m_pEwf->GetVolumeConfig()->Type == EWF_RAM_REG)
					{
						bCommit = TRUE;
					}
				}
				m_pEwf->Disable(bCommit);	  // disable Ewf
				uDoExitWindows = uExitWindows;// needs reboot
			}
		}
		else							// if Ewf is disabled
		{
			if (m_bEnable)
			{
				m_pEwf->Enable();		// enable Ewf
				uDoExitWindows = uExitWindows;// needs reboot
				WK_TRACE(_T("Enable disabled Ewf\n"));
			}
		}
	}

	if (m_bQueryEndSession)
	{
		uDoExitWindows = uExitWindows;
	}
	CheckAutoRun();

	if (uDoExitWindows)					// shall exit?
	{
		DoExitWindows(uDoExitWindows);	// do it
	}
	else if (m_bShellIconSet)			// if the shell icon is set
	{
		HideDialog();
	}
	else
	{
		OnBnClickedClose();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CEwfControlDlg::OnBnClickedCkEnableEwf()
{
	CDataExchange dx(this, TRUE);
	DDX_Check(&dx, IDC_CK_ENABLE_EWF, m_bEnable);
}
/////////////////////////////////////////////////////////////////////////////
void CEwfControlDlg::OnBnClickedCkDisableEwf()
{
	CDataExchange dx(this, TRUE);
	DDX_Check(&dx, IDC_CK_DISABLE_EWF, m_bDisable);
}
/////////////////////////////////////////////////////////////////////////////
void CEwfControlDlg::OnBnClickedCkCommitData()
{
	CDataExchange dx(this, TRUE);
	DDX_Check(&dx, IDC_CK_COMMIT_DATA, m_bCommit);
}
/////////////////////////////////////////////////////////////////////////////
void CEwfControlDlg::OnBnClickedClose()
{
	CheckAutoRun();
	OnOK();	// close means close
	DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CEwfControlDlg::OnBnClickedCancel()
{
	if (m_bShellIconSet)
	{
		HideDialog();
	}
	else
	{
		OnCancel();
		DestroyWindow();
	}
}
void CEwfControlDlg::OnEndSession(BOOL bEnding)
{
	if (m_pEwf)
	{
		m_pEwf->Commit();
	}
	CheckAutoRun();
	CDialog::OnEndSession(bEnding);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CEwfControlDlg::OnQueryEndSession(WPARAM, LPARAM lParam) 
{
	CWK_Profile wkp;
	if (wkp.GetInt(SECTION_COMMON, COMMON_DISPLAYEWFDIALOG, 0))
	{
		CDataExchange dx(this, FALSE);		// update dialog

		InitEwfConfig(FALSE);				// get Ewf parameters
		m_bQueryEndSession = TRUE;

		if (lParam & ENDSESSION_LOGOFF)		// determine end session parameters
		{									// unfortunately only loggoff is certain
			m_nEndsession = RD_ES_LOGOFF;
		}
		else								// otherwise presume shutdown
		{
			m_nEndsession = wkp.GetInt(SECTION_COMMON, COMMON_SHUTDOWN_SETTING, -1);
			if (m_nEndsession != -1)
			{
				wkp.DeleteEntry(SECTION_COMMON, COMMON_SHUTDOWN_SETTING);
			}
			else
			{
				m_nEndsession = RD_ES_REBOOT;
			}
		}

		DDX_Radio(&dx, IDC_RD_LOGOFF, m_nEndsession);
		
		if (m_bEwfEnabled)					// normally it is enabled
		{
			m_bCommit = TRUE;				// assume there is data to save
			DDX_Check(&dx, IDC_CK_COMMIT_DATA, m_bCommit);
		}

		ShowWindow(SW_SHOW);				// show dialog
		GetDlgItem(IDC_RD_LOGOFF)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_RD_REBOOT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_RD_SHUTDOWN)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_TXTV_EWF_COMMIT_MSG)->ShowWindow(SW_SHOW);

		m_nShutDownTimer = (UINT) SetTimer(100, 1000, NULL);
		m_nShutdownCounter = 6;
		OnTimer(m_nShutDownTimer);
		GetDlgItem(IDC_TXT_SHUTDOWN_TIME)->ShowWindow(SW_SHOW);

		return 0;
	}
	else
	{
		return Default();
	}
}
/////////////////////////////////////////////////////////////////////////////
HBRUSH CEwfControlDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (IDC_TXTV_EWF_COMMIT_MSG == pWnd->GetDlgCtrlID())
	{
		pDC->SetTextColor(RGB(255, 0, 0));
	}
	return hbr;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CEwfControlDlg::OnUser(WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
		case 0:
			if (lParam == WM_LBUTTONDOWN)
			{
				CWK_Profile wkp;
				if (wkp.GetInt(SECTION_COMMON, COMMON_DISPLAYEWFDIALOG, 0))
				{
					ShowWindow(SW_SHOW);
				}
			}break;
		case WM_SHOWWINDOW:
			ShowWindow(HIWORD(wParam));
			break;
		case WM_QUIT:
			OnBnClickedClose();
			break;
		case WM_SIZE:
		{
			AfxMessageBox(IDS_EWF_LOW_DISK_SPACE, MB_OK|MB_ICONERROR);
			ShowWindow(SW_SHOW);
		}break;
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CEwfControlDlg::OnDestroy()
{
	OnBnClickedRd();	// kills the shutdown timer if any
	
	if (m_nControlTimer)
	{
		KillTimer(m_nControlTimer);
		m_nControlTimer = 0;
	}

	if (m_bShellIconSet)
	{
		NOTIFYICONDATA nid;
		ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.hWnd   = m_hWnd;
		nid.hIcon  = m_hIcon;
		Shell_NotifyIcon(NIM_DELETE, &nid);
		if (nid.hIcon) ::DestroyIcon(nid.hIcon);
	}

	CWK_Profile wkp;
	wkp.DeleteEntry(SECTION_COMMON, COMMON_USED_PHYS_MEM);
	CDialog::OnDestroy();

}
/////////////////////////////////////////////////////////////////////////////
BOOL CEwfControlDlg::InitEwfConfig(BOOL bFirstTime)
{
	if (!bFirstTime && m_pEwf)
	{
		m_pEwf->UpdateState();
	}
	PEWF_VOLUME_CONFIG pVolumeConfig = m_pEwf->GetVolumeConfig();
	if (pVolumeConfig)
	{
		CString str;
		if (bFirstTime)
		{
			switch (pVolumeConfig->Type)					// evaluate Type
			{
				case EWF_DISK:								// DISK overlay
					str.LoadString(IDS_EWF_TYPE_DISC);
					break;		
				case EWF_RAM:								// RAM overlay, with an associated overlay volume store
					str.LoadString(IDS_EWF_TYPE_RAM);
					break;		
				case EWF_RAM_REG:							// RAM overlay, without an associated overlay volume store
					str.LoadString(IDS_EWF_TYPE_RAM_REG);
					break;		
			}
			SetDlgItemText(IDC_TXTV_TYPE, str);

			switch (pVolumeConfig->State)					// evaluate State
			{
				case EWF_ENABLED:							// The overlay is enabled on the volume
					str.LoadString(IDS_EWF_ENABLED);	
					SetDlgItemText(IDC_TXTV_STATE, str);
					InitEwfEnabled();
					break;

				case EWF_DISABLED:							// The overlay is disabled on the volume
					str.LoadString(IDS_EWF_DISABLED);	
					SetDlgItemText(IDC_TXTV_STATE, str);
					InitEwfDisabled();
					break;
			}
		}

		switch (pVolumeConfig->BootCommand.Command)			// evaluate Boot Command
		{
			case EWF_NO_CMD:								// no cmd set
				str.LoadString(IDS_EWF_CMD_NONE); break;
			case EWF_ENABLE:								// overlay will be enabled
				str.LoadString(IDS_EWF_ENABLE); break;
			case EWF_DISABLE:								// overlay will be disabled
				str.LoadString(IDS_EWF_DISABLE); break;
			case EWF_SET_LEVEL:								// overlay level will be set
				str.LoadString(IDS_EWF_CMD_SETLEVEL); break;
			case EWF_COMMIT:								// current level will be committed to the protected volume
				str.LoadString(IDS_EWF_CMD_COMMIT); break;
		}
		SetDlgItemText(IDC_TXTV_BOOT_CMD, str);
																// evaluate Level
		SetDlgItemInt(IDC_TXTV_MAX_LEVEL, pVolumeConfig->MaxLevels);
		SetDlgItemInt(IDC_TXTV_CURRENT_LEVEL, pVolumeConfig->CurrentLevel);
		if (pVolumeConfig->CurrentLevel < pVolumeConfig->MaxLevels)
		{
			CString sName(pVolumeConfig->LevelDescArray[pVolumeConfig->CurrentLevel].LevelName);
			SetDlgItemText(IDC_TXTV_LEVEL_NAME, sName);
//				pVolumeConfig->LevelDescArray[i].LevelEndTime
//				pVolumeConfig->LevelDescArray[i].LevelDataSize
		}
																// evaluate Memory sizes
		SetDlgItemInt(IDC_TXTV_CLUMP_SIZE, pVolumeConfig->ClumpSize);
		SetDlgItemInt(IDC_TXTV_MEM_MAP_SIZE, pVolumeConfig->MemMapSize);

		{
			CString sDeviceName(pVolumeConfig->VolumeDesc.DeviceName);
			SetDlgItemText(IDC_TXTV_VOLUME_DESC, sDeviceName);
		}

		str.Empty();
		CString sFmt;
		for (int i=0; i<EWF_VOLUME_ID_SIZE; i++)
		{
			sFmt.Format(_T("%02x "), pVolumeConfig->VolumeDesc.VolumeID[i]);
			str += sFmt;
		}
		SetDlgItemText(IDC_TXTV_VOLUMEID, str);
		PEWF_OVERLAY_STORE_CONFIG pOvlStorCfg = m_pEwf->GetOvlStoreConfig();
		if (pOvlStorCfg)// && pOvlStorCfg->NumVolumes > 0)
		{
			CString sDeviceName(pOvlStorCfg->VolumeDescArray[0].DeviceName);
			SetDlgItemText(IDC_TXTV_STOREV_DESC, sDeviceName);
		}
		else
		{
			CString sDeviceName;
			sDeviceName.Format(_T("%c:\\"), m_cProtectedDrive);
			SetDlgItemText(IDC_TXTV_STOREV_DESC, sDeviceName);
		}
		return TRUE;
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CEwfControlDlg::UpdateEwfDataSize()
{
	DWORD dwOldSize    = m_dwCurrentDataSize;
	DWORD dwOldSizeLow = m_dwCurrentDataSizeLow;
	PEWF_VOLUME_CONFIG pVolumeConfig = m_pEwf->GetVolumeConfig();
	if (pVolumeConfig)
	{
		CString str;
		if (pVolumeConfig->Type == EWF_DISK)
		{
			DWORD dwMapSize = (DWORD)(pVolumeConfig->DiskOverlay.DiskMapSize >> 20);		// MB
			m_dwCurrentDataSize = (DWORD)(pVolumeConfig->DiskOverlay.DiskDataSize >> 20);	// MB
																							// KB
			m_dwCurrentDataSizeLow = (DWORD)(pVolumeConfig->DiskOverlay.DiskDataSize & 0x000fffff)>>10;
			str.Format(_T("%d, %d MB"), dwMapSize, m_dwCurrentDataSize);
			SetDlgItemText(IDC_TXTV_RAM_SIZE, str);
		}
		else
		{
			m_dwCurrentDataSize = (DWORD)(pVolumeConfig->RamOverlay.RamDataSize >> 20);		// MB
																							// KB
			m_dwCurrentDataSizeLow = (DWORD)(pVolumeConfig->RamOverlay.RamDataSize & 0x000fffff)>>10;
			str.Format(_T("%d.%03d MB"), m_dwCurrentDataSize, MulDiv(m_dwCurrentDataSizeLow, 1000, 1024));
			SetDlgItemText(IDC_TXTV_RAM_SIZE, str);
		}
	}
	return (dwOldSize != m_dwCurrentDataSize || dwOldSizeLow != m_dwCurrentDataSizeLow) ? TRUE : FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CEwfControlDlg::InitEwfEnabled()
{
	m_bEwfEnabled = TRUE;
	if (m_pEwf)
	{
		PEWF_OVERLAY_STORE_CONFIG pOSC = m_pEwf->GetOvlStoreConfig();
		if (pOSC)
		{
			WK_TRACE(_T("OverLayStoreConfig:%s\n"), pOSC->VolumeDescArray[0].DeviceName);
		}
		if (m_llLowSpace > 0)
		{
			WK_TRACE(_T("Surveilling:%lu\n"), m_llLowSpace);
			if (m_pEwf->IsAsync())
			{
				WK_TRACE(_T("Asynchronus\n"));
				m_ovEventLowSpace.hEvent = CreateEvent(NULL,// Default security.
													FALSE,	// Auto-reset.
													FALSE,	// Non-signaled state.
													NULL);
				if (m_pEwf->RegisterLowSpaceNotification(m_llLowSpace, &m_ovEventLowSpace))
				{
					WK_TRACE(_T("RegisterLowSpaceNotification succeeded\n"), m_llLowSpace);
					m_pLowSpaceThread = AfxBeginThread(LowSpaceThreadFunc, (LPVOID)this);
				}
				else
				{
					WK_TRACE(_T("RegisterLowSpaceNotification failed: %d\n"), GetLastError());
					CloseHandle(m_ovEventLowSpace.hEvent);
					m_ovEventLowSpace.hEvent = NULL;
				}
			}
			else
			{
				WK_TRACE(_T("Synchronus\n"));
				m_pLowSpaceThread = AfxBeginThread(LowSpaceThreadFunc, (LPVOID)this);
			}
		}
	}
	GetDlgItem(IDC_CK_COMMIT_DATA)->EnableWindow(TRUE);
	GetDlgItem(IDC_CK_DISABLE_EWF)->EnableWindow(TRUE);
	GetDlgItem(IDC_CK_ENABLE_EWF)->EnableWindow(FALSE);

	UpdateEwfDataSize();
	// Icon installieren, wenn noch nicht installiert
	NOTIFYICONDATA nid;
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.uFlags = NIF_ICON|NIF_TIP|NIF_MESSAGE;
	nid.hWnd   = m_hWnd;
	nid.hIcon  = m_hIcon;
	CString str, sState, sType;
	GetDlgItemText(IDC_TXTV_TYPE, sType);
	GetDlgItemText(IDC_TXTV_STATE, sState);
	str.Format(IDS_EWF_TOOLTIP, sType, m_cProtectedDrive, sState);
	GetDlgItemText(IDC_TXTV_RAM_SIZE, sState);
	str += _T(" (") + sState + _T(")");
	_tcsncpy(nid.szTip, LPCTSTR(str), 63);
	nid.uID    = 0;
	SetIcon(nid.hIcon, true);
	nid.uCallbackMessage = WM_USER;

	CWK_Profile wkp;
	_TCHAR szDrive[] = _T(" ");
	szDrive[0] = m_cProtectedDrive;
	wkp.WriteString(SECTION_COMMON, COMMON_COMPACT_FLASH, szDrive);

	if (Shell_NotifyIcon(NIM_ADD, &nid))
	{
		PostMessage(WM_USER, MAKELONG(WM_SHOWWINDOW, SW_HIDE));
		m_bShellIconSet = TRUE;
		DWORD dwTime = 60*1000; // 1 min
		m_nControlTimer = (UINT) SetTimer(101, dwTime, NULL);
		PostMessage(WM_TIMER, m_nControlTimer); // log memory at start
	}
	else
	{
		if (wkp.GetInt(SECTION_COMMON, COMMON_DISPLAYEWFDIALOG, 0))
		{
			PostMessage(WM_USER, MAKELONG(WM_SHOWWINDOW, SW_SHOW));
		}
	}
	WK_TRACE(_T("Ewf on drive %c: is enabled, shellicon is set:%d\n"), m_cProtectedDrive, m_bShellIconSet);
}
/////////////////////////////////////////////////////////////////////////////
void CEwfControlDlg::InitEwfDisabled()
{
	m_bEwfEnabled = FALSE;
	GetDlgItem(IDC_CK_COMMIT_DATA)->EnableWindow(FALSE);
	GetDlgItem(IDC_CK_DISABLE_EWF)->EnableWindow(FALSE);
	GetDlgItem(IDC_CK_ENABLE_EWF)->EnableWindow(TRUE);

	WK_TRACE(_T("Ewf is disabled\n"));
	PostMessage(WM_USER, MAKELONG(WM_SHOWWINDOW, SW_SHOW));
	CheckAutoRun();
}
/////////////////////////////////////////////////////////////////////////////
UINT CEwfControlDlg::LowSpaceThreadFunc(LPVOID pParam)
{
	WK_TRACE(_T("Entering LowSpaceThreadFunc\n"));
	CEwfControlDlg*pThis = (CEwfControlDlg*) pParam;
	if (pThis->m_ovEventLowSpace.hEvent)
	{
		DWORD dwResult = WaitForSingleObject(pThis->m_ovEventLowSpace.hEvent, INFINITE);
		if (dwResult == WAIT_OBJECT_0)
		{
			pThis->PostMessage(WM_USER, WM_SIZE);
		}
	}
	else
	{
		BOOL bResult =pThis->m_pEwf->RegisterLowSpaceNotification(pThis->m_llLowSpace, NULL);
		if (bResult)
		{
			pThis->PostMessage(WM_USER, WM_SIZE);
		}
	}
	WK_TRACE(_T("Exiting LowSpaceThreadFunc\n"));
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CEwfControlDlg::DoExitWindows(UINT uFlags) 
{
	HANDLE hToken;					// handle to process token 
	TOKEN_PRIVILEGES tkp;			// ptr. to token structure 

	BOOL fResult;					// system shutdown flag 

	try
	{
		// Get the current process token handle 
		// so we can get debug privilege. 
		fResult = OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken) ;
		if (!fResult) throw 1;

		// Get the LUID for debug privilege. 
		fResult = LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid); 
		if (!fResult) throw 2;

		tkp.PrivilegeCount = 1;  // one privilege to set    
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 

		// Get shutdown privilege for this process. 
		fResult = AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES) NULL, 0); 
		if (!fResult) throw 3;
	}
	catch (int nPos)
	{
	    DWORD dwError = GetLastError();
		WK_TRACE(_T("Error DoExitWindows(), %d, last error: %d, %x\n"),nPos, dwError, dwError);
	}

	if (fResult)
	{
		fResult = ExitWindowsEx(uFlags,0);
		WK_TRACE(_T("ExitWindowsEx(%x): %d\n"), uFlags, fResult);
	}

	if (hToken != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hToken);
	}

	OnBnClickedClose();
}
/////////////////////////////////////////////////////////////////////////////
void CEwfControlDlg::HideDialog()
{
	ShowWindow(SW_HIDE);
	GetDlgItem(IDC_RD_LOGOFF)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_RD_REBOOT)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_RD_SHUTDOWN)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_TXTV_EWF_COMMIT_MSG)->ShowWindow(SW_HIDE);
}
/////////////////////////////////////////////////////////////////////////////
void CEwfControlDlg::CheckAutoRun()
{
    WK_TRACE(_T("CheckAutoRun() is disabled !\n"));
    return;

	CWK_Profile wkpSystem(CWK_Profile::WKP_ABSOLUT,HKEY_LOCAL_MACHINE, _T(""), _T(""));
	if (m_bEnable || m_bEwfEnabled)
	{
		CString sProgramPath = theApp.m_pszHelpFilePath;
		sProgramPath.MakeLower();
		sProgramPath.Replace(_T(".hlp"), _T(".exe"));
		wkpSystem.WriteString(HKLM_RUN, theApp.m_pszAppName, sProgramPath);
		WK_TRACE(_T("Setting Run variable: %s(%s)\n"), theApp.m_pszAppName, sProgramPath);
	}
	else
	{
		wkpSystem.DeleteEntry(HKLM_RUN, theApp.m_pszAppName);
		WK_TRACE(_T("Deleting Run variable: %s\n"), theApp.m_pszAppName);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CEwfControlDlg::OnTimer(UINT nIDEvent)
{
	if (m_nShutDownTimer == nIDEvent)
	{
		if (--m_nShutdownCounter == 0)
		{
			OnBnClickedOk();
		}
		else
		{
			CString sFmt;
			sFmt.Format(m_sShutDown, m_nShutdownCounter);
			SetDlgItemText(IDC_TXT_SHUTDOWN_TIME, sFmt);
		}
	}
	else if (m_nControlTimer == nIDEvent)
	{
		m_pEwf->UpdateState();
		if (UpdateEwfDataSize())
		{
			NOTIFYICONDATA nid;
			nid.cbSize = sizeof(NOTIFYICONDATA);
			nid.uFlags = NIF_TIP;
			nid.hWnd   = m_hWnd;
			nid.hIcon  = m_hIcon;
			nid.uID    = 0;
			CString str, sState, sType;
			GetDlgItemText(IDC_TXTV_TYPE, sType);
			GetDlgItemText(IDC_TXTV_STATE, sState);
			str.Format(IDS_EWF_TOOLTIP, sType, m_cProtectedDrive, sState);
			GetDlgItemText(IDC_TXTV_RAM_SIZE, sState);
			str += _T(" (") + sState + _T(")");
			_tcsncpy(nid.szTip, LPCTSTR(str), 63);
			Shell_NotifyIcon(NIM_MODIFY, &nid);
			DWORD dwTotal = (DWORD)(m_MemoryStatus.dwTotalPhys >> 10);		// KB
							// MB						KB
			DWORD dwSize = m_dwCurrentDataSize<<10|m_dwCurrentDataSizeLow;	// KB
			WK_STAT_LOG(_T("EWF"), dwSize, _T("Data Size"));
			float fPercentUsed = 100.0f * dwSize / dwTotal;
			if ((int)fPercentUsed > m_nEwfRamThreshold)
			{
				CString sMsg;
				sMsg.Format(IDS_EWF_ERR_RAM_USAGE, fPercentUsed);
				CWK_RunTimeError rte(WAI_EWF_CONTROL, REL_REBOOT_ERROR, RTE_EWF_RAM_OVERFLOW, sMsg, dwSize, dwTotal);
				rte.Send();
			}
//			WK_TRACE(_T("Physical Memory Used for EWF RAM_REG: %.2f%%\n"), fPercentUsed);
//			CWK_Profile wkp;
//			wkp.WriteInt(SECTION_COMMON, COMMON_USED_PHYS_MEM, (int)(fPercentUsed*10.0f));
		}
	}
	else
	{
		CDialog::OnTimer(nIDEvent);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CEwfControlDlg::OnBnClickedRd()
{
	if (m_nShutDownTimer)
	{
		KillTimer(m_nShutDownTimer);
		m_nShutDownTimer = 0;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CEwfControlDlg::PostNcDestroy()
{
	delete this;
}
/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CEwfControlDlg::Test()
{
	// rke only testing
	BOOL bReturn;
	TCHAR szVolume[MAX_PATH], szVolMountPoint[MAX_PATH];
	HANDLE hVolume = FindFirstVolume(szVolume, MAX_PATH);
	if (hVolume != INVALID_HANDLE_VALUE)
	{
		TRACE(_T("V:%s\n"), szVolume);
		while (FindNextVolume(hVolume, szVolume, MAX_PATH))
		{
			TRACE(_T("V:%s\n"), szVolume);
		}
		FindVolumeClose(hVolume);
	}


	const int nLen = 20000;
	TCHAR szDos[nLen];
	DWORD dwResult = QueryDosDevice(NULL, szDos, nLen);
	if (dwResult)
	{
		TRACE(_T("%s\n"), &szDos[0]);
		int i;
		for (i=0; i<nLen-1; i++)
		{
			if (szDos[i]==0)
			{
				if (szDos[i+1]==0)
				{
					break;
				}
				TRACE(_T("%s\n"), &szDos[i+1]);
			}
		}

		bReturn = GetVolumeNameForVolumeMountPoint(
					szVolume, // input volume mount point or directory
				szVolume, // output volume name buffer
				MAX_PATH // size of volume name buffer
			);
		

		HANDLE hVolumeMP = FindFirstVolumeMountPoint(szVolume, szVolMountPoint, MAX_PATH);
		if (hVolumeMP != INVALID_HANDLE_VALUE)
		{
			TRACE(_T("MP:%s\n"), szVolume);
			while (FindNextVolumeMountPoint(hVolumeMP, szVolMountPoint, MAX_PATH))
			{
				TRACE(_T("MP:%s\n"), szVolMountPoint);
			}
			FindVolumeMountPointClose(hVolumeMP);
		}
	}
}
#endif

