// DriveAcceleration.cpp : implementation file
//

#include "stdafx.h"
#include "VideteTweak.h"
#include "DriveAcceleration.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDriveAcceleration property page

#define DRIVES_SAVED	"SavedDrives"

IMPLEMENT_DYNCREATE(CDriveAcceleration, CPropertyPage)

CDriveAcceleration::CDriveAcceleration() : CPropertyPage(CDriveAcceleration::IDD)
{
	//{{AFX_DATA_INIT(CDriveAcceleration)
	m_bClearPageFileAtSchutdown = FALSE;
	m_bDisable8dot3Filenames = FALSE;
	m_bDisableSystemEventLog = FALSE;
	m_nMFTCacheSize = -1;
	m_bEnlargeCDRomCache = FALSE;
	m_bNtfsDisableLastAccessUpdate = FALSE;
	m_bEnableBigLBA = FALSE;
	m_nKeyCacheValidationPeriod = 0;
	//}}AFX_DATA_INIT
	m_bChanged = false;
}

CDriveAcceleration::~CDriveAcceleration()
{
}

void CDriveAcceleration::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDriveAcceleration)
	DDX_Check(pDX, IDC_CK_CLEAR_PAGEFILE_AT_SHUDOWN, m_bClearPageFileAtSchutdown);
	DDX_Check(pDX, IDC_CK_DISABLE_8DOT3_FILENAMES, m_bDisable8dot3Filenames);
	DDX_Check(pDX, IDC_CK_DISABLE_SYSTEM_PROTOCOL, m_bDisableSystemEventLog);
	DDX_CBIndex(pDX, IDC_COMBO_MFT_SIZE, m_nMFTCacheSize);
	DDX_Check(pDX, IDC_CK_ENLARGE_CDROM_CACHE, m_bEnlargeCDRomCache);
	DDX_Check(pDX, IDC_CK_DO_NOT_UPDATE_LAST_ACCESS_TIME, m_bNtfsDisableLastAccessUpdate);
	DDX_Check(pDX, IDC_CK_ENABLE_BIG_LBA, m_bEnableBigLBA);
	//}}AFX_DATA_MAP

	DDX_Text(pDX, IDC_EDT_KEY_CACHE_VALIDATION_PERIOD, m_nKeyCacheValidationPeriod);
	if (m_nKeyCacheValidationPeriod)
	{
		DDV_MinMaxInt(pDX, m_nKeyCacheValidationPeriod, 10, 1440);
	}
}


BEGIN_MESSAGE_MAP(CDriveAcceleration, CPropertyPage)
	//{{AFX_MSG_MAP(CDriveAcceleration)
	ON_BN_CLICKED(IDC_CK_CLEAR_PAGEFILE_AT_SHUDOWN, OnChange)
	ON_BN_CLICKED(IDC_BTN_RESTORE, OnBtnRestore)
	ON_BN_CLICKED(IDC_BTN_SAVE, OnBtnSave)
	ON_MESSAGE(WM_HELP, OnHelp)
	ON_BN_CLICKED(IDC_CK_DISABLE_8DOT3_FILENAMES, OnChange)
	ON_BN_CLICKED(IDC_CK_DISABLE_SYSTEM_PROTOCOL, OnChange)
	ON_BN_CLICKED(IDC_CK_ENLARGE_CDROM_CACHE, OnChange)
	ON_BN_CLICKED(IDC_CK_DO_NOT_UPDATE_LAST_ACCESS_TIME, OnChange)
	ON_BN_CLICKED(IDC_CK_ENABLE_BIG_LBA, OnChange)
	ON_CBN_EDITCHANGE(IDC_COMBO_MFT_SIZE, OnChange)
	ON_CBN_SELCHANGE(IDC_COMBO_MFT_SIZE, OnChange)
	ON_EN_CHANGE(IDC_EDT_KEY_CACHE_VALIDATION_PERIOD, OnChange)
	ON_BN_CLICKED(IDC_BTN_SET, OnBtnImageSetttings)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDriveAcceleration message handlers

BOOL CDriveAcceleration::OnInitDialog() 
{
	HKEY  hSecKey = NULL;
	hSecKey = theApp.RegOpenKey(HKEY_LOCAL_MACHINE, HKLM_FILESYSTEM, KEY_READ);
	if (hSecKey)
	{
		m_nMFTCacheSize					= theApp.RegReadInt(hSecKey, NTFS_MFT_SIZE, 0);
		m_bDisable8dot3Filenames		= theApp.RegReadInt(hSecKey, NTFS_DISABLE_8DOT3_FN, 0);
		m_bNtfsDisableLastAccessUpdate	= theApp.RegReadInt(hSecKey, NTFS_DISABLE_L_A_UPDT, 0);
	}
	REG_CLOSE_KEY(hSecKey);

	hSecKey = theApp.RegOpenKey(HKEY_LOCAL_MACHINE, HKLM_WBEM_CIMOM, KEY_READ);
	if (hSecKey)
	{
		m_bDisableSystemEventLog = !theApp.RegReadInt(hSecKey, ENABLE_EVENTS, 0);
	}
	REG_CLOSE_KEY(hSecKey);

	hSecKey = theApp.RegOpenKey(HKEY_LOCAL_MACHINE, HKLM_EFS, KEY_READ);
	if (hSecKey)
	{
		m_nKeyCacheValidationPeriod = theApp.RegReadInt(hSecKey, KEY_CACHE_VAL_PER, 0) / 60;
	}
	REG_CLOSE_KEY(hSecKey);

	hSecKey = theApp.RegOpenKey(HKEY_LOCAL_MACHINE, HKLM_MEMORY_MNGMNT_KEY, KEY_READ);
	if (hSecKey)
	{
		m_bClearPageFileAtSchutdown = theApp.RegReadInt(hSecKey, CLEAR_PAGEFILE_ASD, 0);
	}
	REG_CLOSE_KEY(hSecKey);

	hSecKey = theApp.RegOpenKey(HKEY_LOCAL_MACHINE, HKLM_CONTROL_CDFS, KEY_READ);
	if (hSecKey)
	{
		DWORD dwType, dwValue, dwCount = sizeof(DWORD);
		LONG lResult = RegQueryValueEx(hSecKey, CACHE_SIZE, NULL, &dwType, (LPBYTE)&dwValue, &dwCount);
		if ((lResult == ERROR_SUCCESS) && (dwType == REG_BINARY) && (dwValue == 0x0000ffff))
		{
			m_bEnlargeCDRomCache = TRUE;
		}
	}
	REG_CLOSE_KEY(hSecKey);
	
	hSecKey = theApp.RegOpenKey(HKEY_LOCAL_MACHINE, HKLM_ATAPI_PARAMS, KEY_READ);
	if (hSecKey)
	{
		m_bEnableBigLBA = theApp.RegReadInt(hSecKey, ENABLEBIGLBA, 0);
	}
	REG_CLOSE_KEY(hSecKey);

	CPropertyPage::OnInitDialog();
	
	int nTest = theApp.GetProfileInt(DRIVES_SAVED, NTFS_MFT_SIZE, -1);
	if (nTest==-1)
	{
		OnBtnSave();
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDriveAcceleration::OnApply() 
{
	if (m_bChanged)
	{
		HKEY  hSecKey = NULL;
		hSecKey = theApp.RegOpenKey(HKEY_LOCAL_MACHINE, HKLM_FILESYSTEM, KEY_WRITE);
		if (hSecKey)
		{
			if (m_nMFTCacheSize)
			{
				theApp.RegWriteInt(hSecKey, NTFS_MFT_SIZE, m_nMFTCacheSize);
			}
			else
			{
				RegDeleteValue(hSecKey, NTFS_MFT_SIZE);
			}
			if (m_bDisable8dot3Filenames)
			{
				theApp.RegWriteInt(hSecKey, NTFS_DISABLE_8DOT3_FN, m_bDisable8dot3Filenames);
			}
			else
			{
				RegDeleteValue(hSecKey, NTFS_DISABLE_8DOT3_FN);
			}
			if (m_bNtfsDisableLastAccessUpdate)
			{
				theApp.RegWriteInt(hSecKey, NTFS_DISABLE_L_A_UPDT, m_bNtfsDisableLastAccessUpdate);
			}
			else
			{
				RegDeleteValue(hSecKey, NTFS_DISABLE_L_A_UPDT);
			}
		}
		REG_CLOSE_KEY(hSecKey);

		hSecKey = theApp.RegOpenKey(HKEY_LOCAL_MACHINE, HKLM_WBEM_CIMOM, KEY_WRITE);
		if (hSecKey)
		{
			theApp.RegWriteInt(hSecKey, ENABLE_EVENTS, !m_bDisableSystemEventLog);
		}
		REG_CLOSE_KEY(hSecKey);

		hSecKey = theApp.RegOpenKey(HKEY_LOCAL_MACHINE, HKLM_EFS, KEY_WRITE);
		if (hSecKey)
		{
			if (m_nKeyCacheValidationPeriod)
			{
				theApp.RegWriteInt(hSecKey, KEY_CACHE_VAL_PER, m_nKeyCacheValidationPeriod*60);
			}
			else
			{
				RegDeleteValue(hSecKey, KEY_CACHE_VAL_PER);
			}
		}
		REG_CLOSE_KEY(hSecKey);

		hSecKey = theApp.RegOpenKey(HKEY_LOCAL_MACHINE, HKLM_MEMORY_MNGMNT_KEY, KEY_WRITE);
		if (hSecKey)
		{
			theApp.RegWriteInt(hSecKey, CLEAR_PAGEFILE_ASD, m_bClearPageFileAtSchutdown);
		}
		REG_CLOSE_KEY(hSecKey);

		hSecKey = theApp.RegOpenKey(HKEY_LOCAL_MACHINE, HKLM_ATAPI_PARAMS, KEY_WRITE);
		if (hSecKey)
		{
			theApp.RegWriteInt(hSecKey, ENABLEBIGLBA, m_bEnableBigLBA);
		}
		REG_CLOSE_KEY(hSecKey);

		hSecKey = theApp.RegOpenKey(HKEY_LOCAL_MACHINE, HKLM_CONTROL_CDFS, KEY_WRITE);
		if (hSecKey)
		{
			if (m_bEnlargeCDRomCache)
			{
				DWORD dwCount = sizeof(DWORD);
				DWORD dwValue = 0x0000ffff;
				RegSetValueEx(hSecKey, CACHE_SIZE, NULL, REG_BINARY, (BYTE*)&dwValue, dwCount);
				theApp.RegWriteInt(hSecKey, PREFETCH_TAIL, 0x00004000);
				theApp.RegWriteInt(hSecKey, PREFETCH, 0x00004000);
			}
			else
			{
				RegDeleteValue(hSecKey, CACHE_SIZE);
				RegDeleteValue(hSecKey, PREFETCH_TAIL);
				RegDeleteValue(hSecKey, PREFETCH);
			}
		}
		REG_CLOSE_KEY(hSecKey);

		SetModified(FALSE);
		m_bChanged = false;
	}
	return CPropertyPage::OnApply();
}

void CDriveAcceleration::OnChange() 
{
	SetModified();	
	m_bChanged = true;
}

void CDriveAcceleration::OnBtnRestore() 
{
	m_nMFTCacheSize= theApp.GetProfileInt(DRIVES_SAVED, NTFS_MFT_SIZE, 0);
	m_bDisable8dot3Filenames = theApp.GetProfileInt(DRIVES_SAVED, NTFS_DISABLE_8DOT3_FN, 0);
	m_bNtfsDisableLastAccessUpdate = theApp.GetProfileInt(DRIVES_SAVED, NTFS_DISABLE_L_A_UPDT, 0);

	m_bDisableSystemEventLog = theApp.GetProfileInt(DRIVES_SAVED, ENABLE_EVENTS, 0);

	m_nKeyCacheValidationPeriod = theApp.GetProfileInt(DRIVES_SAVED, KEY_CACHE_VAL_PER, 0);

	m_bClearPageFileAtSchutdown = theApp.GetProfileInt(DRIVES_SAVED, CLEAR_PAGEFILE_ASD, 0);

	m_bEnableBigLBA = theApp.GetProfileInt(DRIVES_SAVED, ENABLEBIGLBA, 0);

	m_bEnlargeCDRomCache = theApp.GetProfileInt(DRIVES_SAVED, CACHE_SIZE, 0);
	UpdateData(FALSE);
	OnChange();
}

void CDriveAcceleration::OnBtnSave() 
{
	if (UpdateData())
	{
		theApp.WriteProfileInt(DRIVES_SAVED, NTFS_MFT_SIZE, m_nMFTCacheSize);
		theApp.WriteProfileInt(DRIVES_SAVED, NTFS_DISABLE_8DOT3_FN, m_bDisable8dot3Filenames);
		theApp.WriteProfileInt(DRIVES_SAVED, NTFS_DISABLE_L_A_UPDT, m_bNtfsDisableLastAccessUpdate);

		theApp.WriteProfileInt(DRIVES_SAVED, ENABLE_EVENTS, m_bDisableSystemEventLog);

		theApp.WriteProfileInt(DRIVES_SAVED, KEY_CACHE_VAL_PER, m_nKeyCacheValidationPeriod);

		theApp.WriteProfileInt(DRIVES_SAVED, CLEAR_PAGEFILE_ASD, m_bClearPageFileAtSchutdown);

		theApp.WriteProfileInt(DRIVES_SAVED, ENABLEBIGLBA, m_bEnableBigLBA);

		theApp.WriteProfileInt(DRIVES_SAVED, CACHE_SIZE, m_bEnlargeCDRomCache);
	}
}

LRESULT CDriveAcceleration::OnHelp(WPARAM /*wParam*/, LPARAM lParam)
{
   HELPINFO *pHI = (HELPINFO*) lParam;
	CString str;
	switch (pHI->iCtrlId)
	{
		case IDC_COMBO_MFT_SIZE: str.Format("HKLM\\%s:\n%s (REG_DWORD)", HKLM_FILESYSTEM, NTFS_MFT_SIZE); break;
		case IDC_CK_DISABLE_8DOT3_FILENAMES: str.Format("HKLM\\%s:\n%s (REG_DWORD)", HKLM_FILESYSTEM, NTFS_DISABLE_8DOT3_FN);  break;
		case IDC_CK_DO_NOT_UPDATE_LAST_ACCESS_TIME: str.Format("HKLM\\%s:\n%s (REG_DWORD)", HKLM_FILESYSTEM, NTFS_DISABLE_L_A_UPDT); break;
		case IDC_CK_DISABLE_SYSTEM_PROTOCOL: str.Format("HKLM\\%s:\n%s (REG_DWORD)", HKLM_WBEM_CIMOM, ENABLE_EVENTS); break;
		case IDC_EDT_KEY_CACHE_VALIDATION_PERIOD: str.Format("HKLM\\%s:\n%s (REG_DWORD)", HKLM_EFS, KEY_CACHE_VAL_PER); break;
		case IDC_CK_CLEAR_PAGEFILE_AT_SHUDOWN: str.Format("HKLM\\%s:\n%s (REG_DWORD)", HKLM_MEMORY_MNGMNT_KEY, CLEAR_PAGEFILE_ASD); break;
		case IDC_CK_ENABLE_BIG_LBA: str.Format("HKLM\\%s:\n%s (REG_DWORD)", HKLM_ATAPI_PARAMS, ENABLEBIGLBA); break;
		case IDC_CK_ENLARGE_CDROM_CACHE: str.Format("HKLM\\%s:\n%s (REG_BINARY)\n%s (REG_DWORD)\n%s (REG_DWORD)", HKLM_CONTROL_CDFS, CACHE_SIZE, PREFETCH_TAIL, PREFETCH); break;
	}
	if (!str.IsEmpty())
	{
		AfxMessageBox(str);
	}
	return 1;
}

void CDriveAcceleration::OnBtnImageSetttings() 
{
	m_bDisable8dot3Filenames = TRUE;
	m_bEnlargeCDRomCache = TRUE;
	m_bNtfsDisableLastAccessUpdate = TRUE;
	m_bEnableBigLBA = TRUE;
	UpdateData(FALSE);
	OnChange();
}
