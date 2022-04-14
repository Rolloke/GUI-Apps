// MemoryManegmentDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VideteTweak.h"
#include "MemoryManegmentDlg.h"
#include ".\memorymanegmentdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MEM_MAN_SAVED "SavedMemManagement"
/////////////////////////////////////////////////////////////////////////////
// CMemoryManegmentDlg dialog


CMemoryManegmentDlg::CMemoryManegmentDlg()
	: CPropertyPage(CMemoryManegmentDlg::IDD)
{
	//{{AFX_DATA_INIT(CMemoryManegmentDlg)
	m_bDisableKernelPaging = FALSE;
	m_bUnloadDllAlways = FALSE;
	m_n2ndLeveCacheSize = 0;
	m_bSystemCacheSettings = 0;
	//}}AFX_DATA_INIT
	m_nFileCacheSize = 0;
	m_bChanged = false;
}


void CMemoryManegmentDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMemoryManegmentDlg)
	DDX_Check(pDX, IDC_KERNEL_PAGING_DISABLE, m_bDisableKernelPaging);
	DDX_Check(pDX, IDC_UNLOAD_DLL_ALWAYS, m_bUnloadDllAlways);
	DDX_Text(pDX, IDC_EDT_2ND_LEVEL_CACHE, m_n2ndLeveCacheSize);
	DDX_Text(pDX, IDC_EDT_FILE_CACHE_SETTINGS, m_nFileCacheSize);
	DDV_MinMaxInt(pDX, m_n2ndLeveCacheSize, 0, 8192);
	DDX_Check(pDX, IDC_LARGE_SYSTEM_CACHE, m_bSystemCacheSettings);
	DDX_Text(pDX, IDC_EDT_LANMAN_CACHE_SIZE, m_nLanManCacheSize);
	DDV_MinMaxInt(pDX, m_nLanManCacheSize, 1, 3);
	DDX_Control(pDX, IDC_COMBO_MEM, m_ctrlMem);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMemoryManegmentDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CMemoryManegmentDlg)
	ON_BN_CLICKED(IDC_BTN_RESTORE, OnBtnRestore)
	ON_BN_CLICKED(IDC_BTN_SAVE, OnBtnSave)
	ON_BN_CLICKED(IDC_KERNEL_PAGING_DISABLE, OnChange)
	ON_BN_CLICKED(IDC_UNLOAD_DLL_ALWAYS, OnChange)
	ON_EN_CHANGE(IDC_EDT_2ND_LEVEL_CACHE, OnChange)
	ON_EN_CHANGE(IDC_EDT_LANMAN_CACHE_SIZE, OnChange)
	ON_CBN_SELCHANGE(IDC_LARGE_SYSTEM_CACHE, OnChange)
	ON_MESSAGE(WM_HELP, OnHelp)
	ON_BN_CLICKED(IDC_LARGE_SYSTEM_CACHE, OnChange)
	ON_BN_CLICKED(IDC_BTN_OPTIMIZE_FILE_CACHE_SIZE, OnBnClickedBtnOptimizeFileCacheSize)
	ON_CBN_SELCHANGE(IDC_COMBO_MEM, OnCbnSelchangeComboMem)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMemoryManegmentDlg message handlers

BOOL CMemoryManegmentDlg::OnInitDialog() 
{
	HKEY  hSecKey = NULL;
	hSecKey = theApp.RegOpenKey(HKEY_LOCAL_MACHINE, HKLM_MEMORY_MNGMNT_KEY, KEY_READ);
	if (hSecKey)
	{
		m_bDisableKernelPaging	= theApp.RegReadInt(hSecKey, DISABLE_KERNEL_PAGING, 0);
		m_bSystemCacheSettings	= theApp.RegReadInt(hSecKey, USE_LARGE_SYST_CACHE, 0);
		m_nFileCacheSize		= theApp.RegReadInt(hSecKey, FILE_CACHE_SIZE, 0) >> 20;
		m_n2ndLeveCacheSize		= theApp.RegReadInt(hSecKey, SECOND_LEVEL_DATA_CACHE, 0);
	}
	REG_CLOSE_KEY(hSecKey);

	hSecKey = theApp.RegOpenKey(HKEY_LOCAL_MACHINE, HKLM_EXPLORER, KEY_READ);
	if (hSecKey)
	{
		m_bUnloadDllAlways = theApp.RegReadInt(hSecKey, UNLOAD_DLL_ALWAYS, 0);
	}
	REG_CLOSE_KEY(hSecKey);

	hSecKey = theApp.RegOpenKey(HKEY_LOCAL_MACHINE, HKLM_LANMAN_SERV_P_KEY, KEY_READ);
	if (hSecKey)
	{
		m_nLanManCacheSize = theApp.RegReadInt(hSecKey, LANMAN_CACHE_SIZE, 1);
	}
	REG_CLOSE_KEY(hSecKey);

	CPropertyPage::OnInitDialog();

	switch (m_nLanManCacheSize)
	{
		case 1: m_ctrlMem.SetCurSel(0); break;
		case 2: m_ctrlMem.SetCurSel(1); break;
		case 3: m_ctrlMem.SetCurSel(m_bSystemCacheSettings ? 2 : 3); break;
	}

	int nTest = theApp.GetProfileInt(MEM_MAN_SAVED, DISABLE_KERNEL_PAGING, -1);
	if (nTest==-1)
	{
		OnBtnSave();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CMemoryManegmentDlg::OnApply() 
{
	if (m_bChanged)
	{
		HKEY  hSecKey = NULL;

		hSecKey = theApp.RegOpenKey(HKEY_LOCAL_MACHINE, HKLM_MEMORY_MNGMNT_KEY, KEY_WRITE|KEY_READ);
		if (hSecKey)
		{
			theApp.RegWriteInt(hSecKey, DISABLE_KERNEL_PAGING, m_bDisableKernelPaging);
			if (m_nFileCacheSize)
			{
				theApp.RegWriteInt(hSecKey, FILE_CACHE_SIZE, m_nFileCacheSize << 20);
			}
			if (m_n2ndLeveCacheSize)
			{
				theApp.RegWriteInt(hSecKey, SECOND_LEVEL_DATA_CACHE, m_n2ndLeveCacheSize);
			}
			else
			{
				RegDeleteValue(hSecKey, SECOND_LEVEL_DATA_CACHE);
			}
			theApp.RegWriteInt(hSecKey, USE_LARGE_SYST_CACHE, m_bSystemCacheSettings);
		}
		REG_CLOSE_KEY(hSecKey);
	
		hSecKey = theApp.RegOpenKey(HKEY_LOCAL_MACHINE, HKLM_EXPLORER, KEY_WRITE|KEY_READ);
		if (hSecKey)
		{
			if (m_bUnloadDllAlways)
			{
				theApp.RegWriteInt(hSecKey, UNLOAD_DLL_ALWAYS, m_bUnloadDllAlways);
			}
			else
			{
				RegDeleteValue(hSecKey, UNLOAD_DLL_ALWAYS);
			}
		}
		REG_CLOSE_KEY(hSecKey);

		hSecKey = theApp.RegOpenKey(HKEY_LOCAL_MACHINE, HKLM_LANMAN_SERV_P_KEY, KEY_WRITE|KEY_READ);
		if (hSecKey)
		{
			theApp.RegWriteInt(hSecKey, LANMAN_CACHE_SIZE, m_nLanManCacheSize);

		}
		REG_CLOSE_KEY(hSecKey);

		SetModified(FALSE);
		m_bChanged = false;
	}
	return CPropertyPage::OnApply();
}

void CMemoryManegmentDlg::OnChange() 
{
	SetModified();	
	m_bChanged = true;
}

void CMemoryManegmentDlg::OnBtnRestore() 
{
	m_bDisableKernelPaging = theApp.GetProfileInt(MEM_MAN_SAVED, DISABLE_KERNEL_PAGING, 0);
	m_bSystemCacheSettings = theApp.GetProfileInt(MEM_MAN_SAVED, USE_LARGE_SYST_CACHE, 0);
	m_nFileCacheSize = theApp.GetProfileInt(MEM_MAN_SAVED, FILE_CACHE_SIZE, 0)>>20;
	m_n2ndLeveCacheSize = theApp.GetProfileInt(MEM_MAN_SAVED, SECOND_LEVEL_DATA_CACHE, 0);
	m_bUnloadDllAlways = theApp.GetProfileInt(MEM_MAN_SAVED, UNLOAD_DLL_ALWAYS, 0);
	m_nLanManCacheSize = theApp.GetProfileInt(MEM_MAN_SAVED, LANMAN_CACHE_SIZE, 1);
	UpdateData(FALSE);
	OnChange();
}

void CMemoryManegmentDlg::OnBtnSave() 
{
	if (UpdateData())
	{
		theApp.WriteProfileInt(MEM_MAN_SAVED, DISABLE_KERNEL_PAGING, m_bDisableKernelPaging);
		theApp.WriteProfileInt(MEM_MAN_SAVED, USE_LARGE_SYST_CACHE, m_bSystemCacheSettings);
		int nSize = m_nFileCacheSize << 20;
		theApp.WriteProfileInt(MEM_MAN_SAVED, FILE_CACHE_SIZE, nSize);
		theApp.WriteProfileInt(MEM_MAN_SAVED, SECOND_LEVEL_DATA_CACHE, m_n2ndLeveCacheSize);
		theApp.WriteProfileInt(MEM_MAN_SAVED, UNLOAD_DLL_ALWAYS, m_bUnloadDllAlways);
		theApp.WriteProfileInt(MEM_MAN_SAVED, LANMAN_CACHE_SIZE, m_nLanManCacheSize);
	}	
}

LRESULT CMemoryManegmentDlg::OnHelp(WPARAM /*wParam*/, LPARAM lParam)
{
   HELPINFO *pHI = (HELPINFO*) lParam;
	CString str;
	switch (pHI->iCtrlId)
	{
		case IDC_KERNEL_PAGING_DISABLE: str.Format("HKLM\\%s:\n%s (REG_DWORD)", HKLM_MEMORY_MNGMNT_KEY, DISABLE_KERNEL_PAGING);  break;
		case IDC_EDT_2ND_LEVEL_CACHE: str.Format("HKLM\\%s:\n%s (REG_DWORD)", HKLM_MEMORY_MNGMNT_KEY, SECOND_LEVEL_DATA_CACHE); break;
		case IDC_LARGE_SYSTEM_CACHE:
			str.Format("HKLM\\%s:\n%s (REG_DWORD)", HKLM_MEMORY_MNGMNT_KEY, USE_LARGE_SYST_CACHE);
			if (m_bSystemCacheSettings)
			{
				str += _T("\nEstablishes a standard size file-system cache of approximately 8 MB. The system allows changed pages to remain in physical memory until the number of available pages drops to approximately 1,000. This setting is recommended for servers running applications that do their own memory caching, such as Microsoft SQL Server, and for applications that perform best with ample memory, such as Internet Information Services.");
			}
			else
			{
				str += _T("\nEstablishes a large system cache working set that can expand to physical memory, minus 4 MB, if needed. The system allows changed pages to remain in physical memory until the number of available pages drops to approximately 250. This setting is recommended for most computers running Windows 2000 Server on large networks.");
			}

			break;
		case IDC_EDT_FILE_CACHE_SETTINGS: str.Format("HKLM\\%s:\n%s (REG_DWORD)", HKLM_MEMORY_MNGMNT_KEY, FILE_CACHE_SIZE); break;
		case IDC_UNLOAD_DLL_ALWAYS: str.Format("HKLM\\%s:\n%s (REG_DWORD)", HKLM_EXPLORER, UNLOAD_DLL_ALWAYS); break;
		case IDC_EDT_LANMAN_CACHE_SIZE: str.Format("HKLM\\%s:\n%s (REG_DWORD)", HKLM_LANMAN_SERV_P_KEY, LANMAN_CACHE_SIZE); break;
	}
	if (!str.IsEmpty())
	{
		AfxMessageBox(str);
	}
	return 1;
}

void CMemoryManegmentDlg::OnBnClickedBtnOptimizeFileCacheSize()
{
	MEMORYSTATUS mem;
	ZeroMemory(&mem, sizeof(mem));
	mem.dwLength = sizeof(mem);
	GlobalMemoryStatus(&mem);
	mem.dwTotalPhys >>= 20;
	if (mem.dwTotalPhys <= 64)
	{
		mem.dwTotalPhys -= 7;
	}
	else if (mem.dwTotalPhys >= 512)
	{
		mem.dwTotalPhys -= 64;
	}
	else
	{
		mem.dwTotalPhys -= 16;
	}
	m_nFileCacheSize = mem.dwTotalPhys / 2;
	CDataExchange dx(this, FALSE);
	DDX_Text(&dx, IDC_EDT_FILE_CACHE_SETTINGS, m_nFileCacheSize);
}

void CMemoryManegmentDlg::OnCbnSelchangeComboMem()
{
	int nSel = m_ctrlMem.GetCurSel();
	switch (nSel)
	{
		case 0:
			m_bSystemCacheSettings = 0;
			m_nLanManCacheSize = 1;
			break;
		case 1:
			m_bSystemCacheSettings = 0;
			m_nLanManCacheSize = 2;
			break;
		case 2:
			m_bSystemCacheSettings = 1;
			m_nLanManCacheSize = 3;
			OnBnClickedBtnOptimizeFileCacheSize();
			break;
		case 3:
			m_bSystemCacheSettings = 0;
			m_nLanManCacheSize = 3;
			break;
	}
	CDataExchange dx(this, FALSE);
	DDX_Check(&dx, IDC_LARGE_SYSTEM_CACHE, m_bSystemCacheSettings);
	DDX_Text(&dx, IDC_EDT_LANMAN_CACHE_SIZE, m_nLanManCacheSize);
}
