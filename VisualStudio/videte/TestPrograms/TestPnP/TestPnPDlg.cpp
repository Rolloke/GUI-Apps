// TestPnPDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TestPnP.h"
#include "TestPnPDlg.h"

#include <Setupapi.h>
#include <Winspool.h>

#include "include\dmoreg.h"
#include "common\namedguid.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// #define DEVICE_NOTIFY_ALL_INTERFACE_CLASSES  0x00000004

#define SAVE_DELETE(p) {if (p) {delete p; p=NULL;}}

#define SUCEEDED(a) (a==S_OK)
/////////////////////////////////////////////////////////////////////////////
// CTestPnPDlg dialog


CTestPnPDlg::CTestPnPDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestPnPDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTestPnPDlg)
	//}}AFX_DATA_INIT
	m_strGUID.Empty();
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_strDevName.Empty();
}

CTestPnPDlg::~CTestPnPDlg()
{
}

void CTestPnPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestPnPDlg)
	DDX_Control(pDX, IDC_COMBO_GUIDS, m_ComboGUID);
	DDX_Control(pDX, IDC_EDT_MESSAGES, m_Messages);
	DDX_Text(pDX, IDC_EDT_GUID, m_strGUID);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTestPnPDlg, CDialog)
	//{{AFX_MSG_MAP(CTestPnPDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_COMBO_GUIDS, OnSelchangeComboGuids)
	ON_BN_CLICKED(IDC_BTN_ENUM_DEVICES, OnBtnEnumDevices)
	ON_BN_CLICKED(IDC_BTN_CLEAR_OUTPUT, OnBtnClearOutput)
	ON_BN_CLICKED(IDC_BTN_SAVE_TO_FILE, OnBtnSaveToFile)
	ON_BN_CLICKED(IDC_BTN_TEST, OnBtnTest)
	ON_BN_CLICKED(IDC_BTN_PRINT, OnBtnPrint)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_WM_DEVICECHANGE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestPnPDlg message handlers


BOOL CTestPnPDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, FALSE);		// Set small icon
	SetIcon(m_hIcon, TRUE);			// Set big icon
#ifndef _DEBUG
	GetDlgItem(IDC_BTN_TEST)->ShowWindow(SW_HIDE);
#endif

//	SetTimer(123, 2000, NULL);
	return TRUE;  // return TRUE  unless you set the focus to a control
}



void CTestPnPDlg::OnBtnEnumDevices() 
{
	EnumDevices(_T("SYSTEM\\CurrentControlSet\\Control\\DeviceClasses\\"));
}

void CTestPnPDlg::EnumDevices(LPCTSTR strDevices)
{
	CString strKey, str, strClass, strGuid;
	HKEY hKey = NULL;
	DWORD dwIndex, dwType, dwLen, dwCount = 1;
	CHAR  pszName[MAX_PATH];

	LONG  lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, strDevices, 0, KEY_READ, &hKey);
	str = strDevices;
	str += _T("\r\n");
	Report(str);

	if (lResult == ERROR_SUCCESS)
	{
		for (dwIndex=0; ; dwIndex++)
		{
			dwLen   = MAX_PATH;
			lResult = RegEnumKeyEx(hKey, dwIndex, pszName, &dwLen, NULL, NULL, 0, NULL);
			if (lResult != ERROR_SUCCESS) break;
			CGuid guid(pszName);
			GetGUIDString(strGuid, &GUID(guid));
			if (strGuid == _T("GUID_NULL")) strGuid.Empty();
			HKEY  hSecKey = NULL;
			lResult = RegOpenKeyEx(hKey, pszName, 0, KEY_READ, &hSecKey);
			if (lResult == ERROR_SUCCESS)
			{
				dwCount = 0;
				lResult = RegQueryValueEx(hSecKey, "DeviceInstance", NULL, &dwType, NULL, &dwCount);
				dwCount++;
				if ((lResult == ERROR_SUCCESS) && (dwType == REG_SZ))
				{
					lResult = RegQueryValueEx(hSecKey, "DeviceInstance", NULL, &dwType, (LPBYTE)strKey.GetBufferSetLength(dwCount), &dwCount);
					strKey.ReleaseBuffer();
				}
			}
			if (hSecKey)
			{
				RegCloseKey(hSecKey);
			}
			if (lResult == ERROR_SUCCESS)
			{
				strKey = _T("SYSTEM\\CurrentControlSet\\Enum\\") + strKey;
				lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, strKey, 0, KEY_READ, &hSecKey);
				if (lResult == ERROR_SUCCESS)
				{
					dwCount = 0;
					lResult = RegQueryValueEx(hSecKey, "ClassGUID", NULL, &dwType, NULL, &dwCount);
					dwCount++;
					if ((lResult == ERROR_SUCCESS) && (dwType == REG_SZ))
					{
						lResult = RegQueryValueEx(hSecKey, "ClassGUID", NULL, &dwType, (LPBYTE)strClass.GetBufferSetLength(dwCount), &dwCount);
						strClass.ReleaseBuffer();
						CString strG;
						CGuid guid(strClass);
						GetGUIDString(strG, &GUID(guid));
						if (strG != _T("GUID_NULL"))
						{
							strClass = strG + _T(": ") + strClass;
						}
					}
				}
				if (hSecKey)
				{
					RegCloseKey(hSecKey);
				}
				if (lResult == ERROR_SUCCESS)
				{
					strKey = _T("SYSTEM\\CurrentControlSet\\Control\\Class\\") + strClass;
					lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, strKey, 0, KEY_READ, &hSecKey);
					if (lResult == ERROR_SUCCESS)
					{
						dwCount = 0;
						lResult = RegQueryValueEx(hSecKey, "", NULL, &dwType, NULL, &dwCount);
						dwCount++;
						if ((lResult == ERROR_SUCCESS) && (dwType == REG_SZ))
						{
							lResult = RegQueryValueEx(hSecKey, "", NULL, &dwType, (LPBYTE)str.GetBufferSetLength(dwCount), &dwCount);
							str.ReleaseBuffer();
							str += _T(": ");
							str += strGuid;
							str += strClass;
							str += _T("\r\n");

							Report(str);
						}
					}
					if (hSecKey)
					{
						RegCloseKey(hSecKey);
					}
				}
			}
			else
			{
				str = _T(strDevices);
				str += _T(pszName);
				str += _T("\\");
				EnumDevices(str);
			}
		}
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTestPnPDlg::OnPaint() 
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
HCURSOR CTestPnPDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

CString CTestPnPDlg::GetDriveType(UINT nType)
{
	CString strDriveType;
	for (int i=-1; i<32 && nType!=0; i++)
		nType >>= 1;
	char  szDrive[]   = "A:";
	szDrive[0] = 'A' + i;
	switch (::GetDriveType(szDrive))
	{
		case DRIVE_UNKNOWN:		strDriveType = _T("Unknown"); break;
		case DRIVE_NO_ROOT_DIR: strDriveType = _T("No root dir"); break;
		case DRIVE_FIXED:			strDriveType = _T("Fixed"); break;
		case DRIVE_REMOTE:		strDriveType = _T("Remote"); break;
		case DRIVE_RAMDISK:		strDriveType = _T("Ramdisk"); break;
		case DRIVE_REMOVABLE:	strDriveType = _T("Removeable"); break;
		case DRIVE_CDROM:			strDriveType = _T("CD-Rom"); break;
			break;
	}
	strDriveType += _T(", ");
	strDriveType += szDrive;
	return strDriveType;
}

BOOL CTestPnPDlg::OnDeviceChange(UINT nEventType, DWORD dwData)
{
	DEV_BROADCAST_HDR* pHdr = (DEV_BROADCAST_HDR*)dwData;
	BOOL bRet = TRUE;
	CString  str;

	if (pHdr)
	{
		if (pHdr->dbch_devicetype == DBT_DEVTYP_VOLUME)
		{
			DEV_BROADCAST_VOLUME *pDBV  = (DEV_BROADCAST_VOLUME*)pHdr;
			CString strDriveType = GetDriveType(pDBV->dbcv_unitmask);
			str.Format("Drive %s ", strDriveType);
			if (nEventType == DBT_DEVICEARRIVAL)
			{
#if(WINVER >= 0x040A)
				CString sFileName;
				CString sVolumeName;
				CString sRegKey;
				CString sDevName;
				int nFind = strDriveType.Find(", ");
				if (nFind != -1)
				{
					sFileName = strDriveType.Mid(nFind+2) + "\\";
				}
/*
				RegisterDeviceDetect(m_hWnd, sFileName);
				char szName[MAX_PATH], szPath[MAX_PATH];
				GetVolumeNamesAndKeys(sFileName, NULL, szPath, szName);
*/

				if (GetVolumeNamesAndKeys(sFileName, sVolumeName.GetBufferSetLength(MAX_PATH), sRegKey.GetBufferSetLength(MAX_PATH), sDevName.GetBufferSetLength(MAX_PATH)))
				{
					sVolumeName.ReleaseBuffer();
					sVolumeName = sVolumeName.Left(sVolumeName.GetLength()-1);
				}
				HANDLE hFile = CreateFile(sVolumeName,
														0,
														0,
														NULL,
														OPEN_EXISTING,
														FILE_ATTRIBUTE_SYSTEM,
														NULL);
				if (hFile != INVALID_HANDLE_VALUE)
				{
					DEV_BROADCAST_HANDLE dbh;
					ZeroMemory(&dbh, sizeof(DEV_BROADCAST_HANDLE));
					dbh.dbch_size       = sizeof(DEV_BROADCAST_HANDLE);
					dbh.dbch_devicetype = DBT_DEVTYP_HANDLE;
					dbh.dbch_handle     = hFile;
					m_FileHandles.AddHead(hFile);
//					BOOL nLock = LockFile(hFile, 0, 0, 1, 0);
					HDEVNOTIFY hdn = RegisterDeviceNotification(m_hWnd, &dbh, DEVICE_NOTIFY_WINDOW_HANDLE);
					if (hdn)
					{
						m_DevNotify.AddHead(hdn);
						CString sTemp;
						sTemp.Format(" File:%x, %x registered ", hFile, hdn);
						str += sTemp;
					}
				}

#endif
			}
		}
		else if (pHdr->dbch_devicetype == DBT_DEVTYP_VOLUME_EX)
		{
			if (nEventType == DBT_DEVICEQUERYREMOVE)
			{
				DEV_BROADCAST_VOLUME_EX *pDBVex = (DEV_BROADCAST_VOLUME_EX*)pHdr;
				CString str;
				str.Format("Gerät entfernen %s", pDBVex->dbcc_device);
				if (AfxMessageBox(str,MB_YESNO|MB_SYSTEMMODAL) == IDYES)
				{
					bRet = TRUE;
				}
				else
				{
					bRet = BROADCAST_QUERY_DENY;
				}
			}
		}
		else if (pHdr->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE_EX)
		{
			DEV_BROADCAST_DEVICEINTERFACE_EX *pDBDIex = (DEV_BROADCAST_DEVICEINTERFACE_EX*)pHdr;
			CString sClassGUID, sDevClassGUID;
			GetGUIDString(sClassGUID, &pDBDIex->dbcc_classguid);
			if (sClassGUID == _T("GUID_NULL"))
			{
				sClassGUID = ((CGuid*)&pDBDIex->dbcc_classguid)->GetString();
			}
			GetGUIDString(sDevClassGUID, &pDBDIex->dbcc_devclassguid);
			if (sDevClassGUID == _T("GUID_NULL"))
			{
				sDevClassGUID = ((CGuid*)&pDBDIex->dbcc_devclassguid)->GetString();
			}

			str.Format("Name:%s\r\nClass:%s; Manufacturer:%s; Service:%s\r\nRegKey:HKLM\\%s\r\nClassGuid:%s, %s\r\nDevGuid:%s, %s\r\n", 
				pDBDIex->dbcc_device,
				&pDBDIex->dbcc_device[pDBDIex->dbcc_offsetClass],
				&pDBDIex->dbcc_device[pDBDIex->dbcc_offsetMfg],
				&pDBDIex->dbcc_device[pDBDIex->dbcc_offsetService],
				&pDBDIex->dbcc_device[pDBDIex->dbcc_offsetKey],
				sClassGUID, ((CGuid*)&pDBDIex->dbcc_classguid)->GetString(),
				sDevClassGUID, ((CGuid*)&pDBDIex->dbcc_devclassguid)->GetString());


			if (m_strDevName.IsEmpty())
			{
				m_strDevName = _T(pDBDIex->dbcc_device);
			}
		}
#if(WINVER >= 0x040A)
		else if (pHdr->dbch_devicetype == DBT_DEVTYP_HANDLE)
		{
			DEV_BROADCAST_HANDLE *pDBH = (DEV_BROADCAST_HANDLE *)pHdr;
			CGuid g(pDBH->dbch_eventguid);
			str.Format("Handle %x, %s, %x, %s", pDBH->dbch_handle, pDBH->dbch_data, pDBH->dbch_hdevnotify, g.GetString());
			BOOL bRemoveHandles = FALSE;

			if (nEventType == DBT_DEVICEQUERYREMOVE)
			{
				if (AfxMessageBox("Jetzt rausschmeißen ?", MB_YESNO|MB_SYSTEMMODAL) == IDYES)
				{
					bRet = TRUE;
					bRemoveHandles = TRUE;
				}
				else
				{
					bRet = BROADCAST_QUERY_DENY;
				}
			}
			else if (nEventType == DBT_DEVICEREMOVECOMPLETE)
			{
				bRemoveHandles = TRUE;
			}

			if (bRemoveHandles)
			{
				POSITION pos;
				if (pDBH->dbch_handle)
				{
					pos = m_FileHandles.Find(pDBH->dbch_handle);
					if (pos)
					{
						m_FileHandles.RemoveAt(pos);
					}
//					UnlockFile(pDBH->dbch_handle, 0, 0, 1, 0);
					CloseHandle(pDBH->dbch_handle);
					pDBH->dbch_handle = NULL;
				}
				if (pDBH->dbch_hdevnotify)
				{
					pos = m_DevNotify.Find((void*)pDBH->dbch_hdevnotify);
					if (pos)
					{
						m_DevNotify.RemoveAt(pos);
					}
					UnregisterDeviceNotification((void*)pDBH->dbch_hdevnotify);
				}
			}
		}
#endif
		else if (pHdr->dbch_devicetype == DBT_DEVTYP_OEM)
		{
			DEV_BROADCAST_OEM *pDBO = (DEV_BROADCAST_OEM*) pHdr;
			str.Format("OEM %x, %x ", pDBO->dbco_identifier, pDBO->dbco_suppfunc);
		}
		else if (pHdr->dbch_devicetype == DBT_DEVTYP_PORT)
		{
			DEV_BROADCAST_PORT *pDBP = (DEV_BROADCAST_PORT*) pHdr;
			str.Format("PORT %s ", pDBP->dbcp_name);
		}
		else
		{
			str.Format("DeviceType:%d ", pHdr->dbch_devicetype);
		}

		switch (nEventType)
		{
			case DBT_DEVICEARRIVAL:           str += _T("arrival"); break;
			case DBT_DEVICEREMOVECOMPLETE:    str += _T("remove complete"); break;
			case DBT_DEVICEQUERYREMOVE:       str += _T("query remove"   ); break;
			case DBT_DEVICEREMOVEPENDING:	    str += _T("remove pending" ); break;
			case DBT_DEVICEQUERYREMOVEFAILED: str += _T("remove failed"  ); break;
			default:
			{
				CString sTemp;
				sTemp.Format(", Event:%x, %d", nEventType, nEventType);
				str += sTemp;
			} break;
		}
		str += _T("\r\n\r\n");
	}
	else
	{
/*
		str.Format("OnDeviceChange(%x) return 0x%08lx\r\n",dwData, bRet);
		switch (nEventType)
		{

			case DBT_DEVNODES_CHANGED: str += _T("DBT_DEVNODES_CHANGED\r\n"); break;
			case DBT_CONFIGCHANGECANCELED: str += _T("DBT_CONFIGCHANGECANCELED\r\n"); break;
			case DBT_CONFIGCHANGED: str += _T("DBT_CONFIGCHANGED\r\n"); break;
			case DBT_CUSTOMEVENT: str += _T("DBT_CUSTOMEVENT\r\n"); break;
			case DBT_DEVICEARRIVAL: str += _T("DBT_DEVICEARRIVAL\r\n"); break;
			case DBT_DEVICEQUERYREMOVE: str += _T("DBT_DEVICEQUERYREMOVE\r\n"); break;
			case DBT_DEVICEQUERYREMOVEFAILED: str += _T("DBT_DEVICEQUERYREMOVEFAILED\r\n"); break;
			case DBT_DEVICEREMOVECOMPLETE: str += _T("DBT_DEVICEREMOVECOMPLETE\r\n"); break;
			case DBT_DEVICEREMOVEPENDING: str += _T("DBT_DEVICEREMOVEPENDING\r\n"); break;
			case DBT_DEVICETYPESPECIFIC: str += _T("DBT_DEVICETYPESPECIFIC\r\n"); break;
			case DBT_QUERYCHANGECONFIG: str += _T("DBT_QUERYCHANGECONFIG\r\n"); break;
			case DBT_USERDEFINED: str += _T("DBT_USERDEFINED\r\n"); break;

		}
*/
	}
	Report(str);
	return bRet;
}



void CTestPnPDlg::OnDestroy() 
{
#if(WINVER >= 0x040A)
	while (m_DevNotify.GetCount())
	{
		HDEVNOTIFY hDN = (HDEVNOTIFY)m_DevNotify.RemoveHead();
		UnregisterDeviceNotification(hDN);
	}
	while (m_FileHandles.GetCount())
	{
		HANDLE hDN = (HANDLE)m_FileHandles.RemoveHead();
		CloseHandle(hDN);
	}
#endif
	CDialog::OnDestroy();
}


void CTestPnPDlg::OnOK() 
{
	UpdateData();
	LPCTSTR pstr;
	if (m_strGUID.IsEmpty())
	{
		m_strGUID = _T("All Classes");
		pstr = NULL;
	}
	else
	{
		pstr = m_strGUID;
	}
	if (RegisterDeviceDetect(m_hWnd, pstr))
	{
		m_strGUID += _T("\r\n");
		Report(m_strGUID);
	}
}

void CTestPnPDlg::Report(CString &str)
{
	m_Messages.SetSel(-1, -1);
	m_Messages.ReplaceSel(str);
}

void CTestPnPDlg::OnSelchangeComboGuids() 
{
	int nSel = m_ComboGUID.GetCurSel();
	if (nSel != CB_ERR)
	{
		m_ComboGUID.GetLBText(nSel, m_strGUID);
		UpdateData(FALSE);
		OnOK();
	}
}


/*
	HDEVINFO hdi = SetupDiGetClassDevs(&guid, NULL, m_hWnd, DIGCF_ALLCLASSES);
	if (hdi)
	{
		SP_DEVINFO_DATA dinfo;
		SP_DEVICE_INTERFACE_DATA dinterface;
		GUID guidX = guid;
		DWORD dwIndex = 0, dwError;
		BOOL  bRet;
		dinfo.cbSize = sizeof(SP_DEVINFO_DATA);
		dinterface.cbSize =sizeof(SP_DEVICE_INTERFACE_DATA);
		do
		{
			bRet = SetupDiEnumDeviceInterfaces(hdi, &dinfo, &guidX, dwIndex, &dinterface);
			dwError = GetLastError();
		} while(bRet);

		SetupDiDestroyDeviceInfoList(hdi);
	}
    HDEVINFO DeviceInfoSet;
    HDEVINFO NewDeviceInfoSet;
	 DWORD Err;

    // Create a device information set that will be the container for 
    // the device interfaces.

    DeviceInfoSet = SetupDiCreateDeviceInfoList(NULL, NULL);
    
    if(DeviceInfoSet == INVALID_HANDLE_VALUE) 
	 {
        Err = GetLastError();
//        printf( "SetupDiCreateDeviceInfoList failed: %lx.\n", Err );
        return 0;
    }

    // Retrieve the device information set for the interface class.

    NewDeviceInfoSet = SetupDiGetClassDevs(NULL, NULL, NULL, DIGCF_PRESENT|DIGCF_ALLCLASSES);

		 SetupDiGetClassDevsEx(&guid,
        NULL,
        NULL,
        DIGCF_PRESENT|DIGCF_DEVICEINTERFACE,
        (const char *)DeviceInfoSet,
        NULL);
    if(NewDeviceInfoSet == INVALID_HANDLE_VALUE) 
    {
        Err = GetLastError();
        printf( "SetupDiGetClassDevsEx failed: %lx.\n", Err );
        return 0;
    }
	SP_DEVINFO_DATA dinfo;
	SP_DEVICE_INTERFACE_DATA dinterface;
	GUID guidX = guid;
	DWORD dwIndex = 0, dwError;
	BOOL  bRet;
	dinfo.cbSize = sizeof(SP_DEVINFO_DATA);
	dinterface.cbSize =sizeof(SP_DEVICE_INTERFACE_DATA);
	do
	{
		bRet = SetupDiEnumDeviceInterfaces(NewDeviceInfoSet, &dinfo, &guidX, dwIndex, &dinterface);
		dwError = GetLastError();
	} while(bRet);

	SetupDiDestroyDeviceInfoList(NewDeviceInfoSet);
*/

/*
				CString str2;
				str2.Format("Device ID {%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}\r\n",
					pDBDI->dbcc_classguid.Data1, 
					pDBDI->dbcc_classguid.Data2, 
					pDBDI->dbcc_classguid.Data3, 
					pDBDI->dbcc_classguid.Data4[0], 
					pDBDI->dbcc_classguid.Data4[1], 
					pDBDI->dbcc_classguid.Data4[2], 
					pDBDI->dbcc_classguid.Data4[3], 
					pDBDI->dbcc_classguid.Data4[4], 
					pDBDI->dbcc_classguid.Data4[5], 
					pDBDI->dbcc_classguid.Data4[6], 
					pDBDI->dbcc_classguid.Data4[7]
					);
				DWORD dwError  = GetLastError();
				HDEVNOTIFY hDN = SetupDiGetClassDevs(&pDBDI->dbcc_classguid, NULL, NULL, DIGCF_PRESENT);
				if (hDN)
				{
					SP_DEVINFO_DATA          spDID;
					SP_DEVICE_INTERFACE_DATA spDINTD;
					SP_DEVICE_INTERFACE_DETAIL_DATA *pspDINTDD;
					DWORD dwSize=0;
					ZeroMemory(&spDID, sizeof(SP_DEVINFO_DATA));
					spDID.cbSize    = sizeof(SP_DEVINFO_DATA);
					spDID.ClassGuid = pDBDI->dbcc_classguid;
//					spDID.DevInst	 =
					ZeroMemory(&spDINTD, sizeof(SP_DEVICE_INTERFACE_DATA));
					spDINTD.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);


//					MakeGUID("{ce5939ae-ebde-11d0-b181-0000f8753ec4}", spDINTD.InterfaceClassGuid);	// Wechselmedien
					MakeGUID("{36FC9E60-C465-11CF-8056-444553540000}", spDINTD.InterfaceClassGuid);	// USB
//					MakeGUID("{4d36e965-e325-11Ce-bfc1-08002be10318}", spDINTD.InterfaceClassGuid);	// CD-Rom
//					MakeGUID("{4d36e967-e325-11Ce-bfc1-08002be10318}", spDINTD.InterfaceClassGuid);	// Diskdrive
					spDINTD.Flags = SPINT_DEFAULT;

					HKEY hKey = SetupDiOpenDeviceInterfaceRegKey(hDN, &spDINTD, 0, KEY_READ);
					if (hKey != INVALID_HANDLE_VALUE) RegCloseKey(hKey);

					SetupDiEnumDeviceInterfaces(hDN, &spDID, , 0, &spDINTD);
					dwError   = GetLastError();

					BOOL bReturn = SetupDiGetDeviceInterfaceDetail(hDN, &spDINTD, NULL, 0, &dwSize, &spDID);
					dwError   = GetLastError();
					dwSize = 2000;
					if (dwSize)
					{
						dwSize += (sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA) + sizeof(TCHAR));
						pspDINTDD = (SP_DEVICE_INTERFACE_DETAIL_DATA*) new BYTE[dwSize];
						SetupDiGetDeviceInterfaceDetail(hDN, &spDINTD, pspDINTDD, dwSize, &dwSize, &spDID);
						dwError   = GetLastError();
						delete pspDINTDD;
					}

					SetupDiDestroyDeviceInfoList(hDN);
				}
*/



void CTestPnPDlg::OnBtnClearOutput() 
{
	m_Messages.SetWindowText("");
	m_strDevName.Empty();
}

void CTestPnPDlg::OnBtnSaveToFile() 
{
	CTestPnPApp * pApp = (CTestPnPApp*)AfxGetApp();
	CString strDefaultName;
	SetCurrentDirectory(pApp->m_strProgramPath);
	if (m_strDevName.IsEmpty())
	{
		strDefaultName = pApp->m_strProgramPath + "Test.txt";
	}
	else
	{
		char szNotAllowed[] = "\\/:*?\"<>|";
		int i, nLen = strlen(szNotAllowed);
		for (i=0; i<nLen; i++)
		{
			m_strDevName.Replace(szNotAllowed[i], '#');
		}
		strDefaultName = pApp->m_strProgramPath + m_strDevName + ".txt";
	}

   CFileDialog dialog(FALSE, "EXE", strDefaultName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "Text Datei (*.txt)|*.txt||", this);
	if (dialog.DoModal()	== IDOK)
	{
		CFile file;
		if (file.Open(dialog.GetPathName(), CFile::modeCreate|CFile::modeWrite))
		{
			CString str;
			m_Messages.GetWindowText(str);
			file.Write(LPCTSTR(str), str.GetLength());
		}
	}
}
unsigned int CALLBACK SetupHookProc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	if (WM_INITDIALOG == uiMsg)
	{
		::EnableWindow(::GetDlgItem(hdlg, 0x470), false);
		::EnableWindow(::GetDlgItem(hdlg, 0x420), false);
		::EnableWindow(::GetDlgItem(hdlg, 0x421), false);
		return TRUE;
	}
	return FALSE;
}

void CTestPnPDlg::OnBtnTest() 
{
	CPrintDialog dlg(FALSE);
	dlg.m_pd.Flags &= ~PD_RETURNDC;
	dlg.m_pd.Flags |= PD_HIDEPRINTTOFILE;
	dlg.m_pd.Flags |=	PD_PRINTSETUP;
	dlg.m_pd.Flags |=	PD_NONETWORKBUTTON;
	dlg.m_pd.Flags |=	PD_ENABLESETUPHOOK;
	dlg.m_pd.lpfnSetupHook = SetupHookProc;

	dlg.DoModal();
}

void CTestPnPDlg::OnBtnPrint() 
{
	CString str;
	m_Messages.GetWindowText(str);
	if (str.IsEmpty())
	{
		return;
	}
	CPrintDialog dlg(FALSE);
	dlg.m_pd.Flags &= ~PD_RETURNDC;
	if (dlg.DoModal() == IDOK)
	{
		DEVMODE *pDM = dlg.GetDevMode();
		pDM->dmOrientation  = DMORIENT_PORTRAIT;
		pDM->dmPrintQuality = DMRES_DRAFT;
		::GlobalUnlock(dlg.m_pd.hDevMode);
		CString sMsg;
		const int nLen = 1023;
		char pszBuffer[nLen+1];

		HDC hDCPrint = dlg.CreatePrinterDC();
		if (hDCPrint)
		{
			CDC dcPrint;
			dcPrint.Attach(hDCPrint);
			dcPrint.m_bPrinting = TRUE;

			DOCINFO di;
			ZeroMemory(&di, sizeof(DOCINFO));
			di.cbSize = sizeof(DOCINFO);
			di.lpszDocName = "TestPnP";

			if (dcPrint.StartDoc(&di) != SP_ERROR)
			{
				if (dcPrint.StartPage() >= 0)
				{	
					// Geeigneten nicht proportionalen Font auswählen
					CFont font;
					font.CreatePointFont(8*10, "Courier New", &dcPrint);
					CFont* pOldFont = (CFont*)dcPrint.SelectObject(&font);
					// Einen Stift für die Trennlinien auswählen
					CPen pen(PS_SOLID, 4, RGB(0,0,0));
					CPen* pOldPen = dcPrint.SelectObject(&pen);

					CSize  szPrint(dcPrint.GetDeviceCaps(HORZRES), dcPrint.GetDeviceCaps(VERTRES));
					CPoint ptOffset(dcPrint.GetDeviceCaps(PHYSICALOFFSETX), dcPrint.GetDeviceCaps(PHYSICALOFFSETY)); 
					CRect  rcPage(ptOffset, szPrint);
					rcPage.DeflateRect(200,200,200, 200);
					dcPrint.DPtoLP(&rcPage);
					
					int nYPos	= 0;
					int nXPos	= 0;
					int nIndex	= 0;
					int nPage	= 0;
					CRect rcText(rcPage.left, rcPage.top, rcPage.right, rcPage.bottom);
					do
					{
						int nMaxLinesPerPage, nMaxPages;
						if (nIndex == 0)
						{
							strcpy(pszBuffer, "TestPnP");
							dcPrint.DrawText(pszBuffer, rcText, DT_CALCRECT | DT_LEFT|DT_WORDBREAK);
							dcPrint.DrawText(pszBuffer, rcText, DT_LEFT|DT_WORDBREAK);				
							nMaxLinesPerPage = max(rcPage.Height() / rcText.Height() - 4, 0);
							nMaxPages = m_Messages.GetLineCount() / (nMaxLinesPerPage) + 1;
						}
						else
						{
							dcPrint.StartPage();
							nYPos = 0;
						}
						
						// Aktuelle Seitennummer ebenfalls ausgeben
						sMsg.Format("%d/%d", ++nPage, nMaxPages);					
						CRect rcText2(rcPage.left, rcPage.top, rcPage.right, rcPage.bottom);
						dcPrint.DrawText(sMsg, rcText2, DT_RIGHT|DT_SINGLELINE);				

						// Waagerechte Trennlinie zeichnen
						dcPrint.MoveTo(rcPage.left, rcPage.top+rcText.Height());
						dcPrint.LineTo(rcPage.right + rcPage.Width(), rcPage.top+rcText.Height());

						nXPos = 0;
						for (int nJ = 0; nJ < nMaxLinesPerPage; nJ++)
						{
							// Eine Zeile weiter					
							nYPos += rcText.Height(); 

							// Text ausgeben.
							int nBytes = m_Messages.GetLine(nIndex++, pszBuffer, nLen);
							pszBuffer[nBytes] = 0;
							CRect rcText(rcPage.left+nXPos, rcPage.top+nYPos, rcPage.right, rcPage.top);
							dcPrint.DrawText(pszBuffer, rcText, DT_CALCRECT | DT_LEFT|DT_WORDBREAK);
							dcPrint.DrawText(pszBuffer, rcText, DT_LEFT|DT_WORDBREAK);				
						}	
						dcPrint.EndPage();
					}
					while (nIndex < m_Messages.GetLineCount());						
					dcPrint.SelectObject(pOldFont);
					pOldFont->DeleteObject();
				
					dcPrint.SelectObject(pOldPen);
					pOldPen->DeleteObject();
				}
				dcPrint.EndDoc();
			}
			dcPrint.Detach();
			DeleteDC(hDCPrint);
		}
	}
}

void CTestPnPDlg::OnTimer(UINT nIDEvent) 
{
//	FlashWindow(TRUE);
//	KillTimer(nIDEvent);
	CDialog::OnTimer(nIDEvent);
}
