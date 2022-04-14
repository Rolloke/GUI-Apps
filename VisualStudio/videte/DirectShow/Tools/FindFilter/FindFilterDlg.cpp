// FindFilterDlg.cpp : implementation file
//

#include "stdafx.h"

#ifdef GRAPH_CREATOR
  #include "..\graphcreator\graphcreator.h"
#endif

#ifdef FIND_FILTER
  #include "FindFilter.h"
#endif

#include "FindFilterDlg.h"

#include "include\dmoreg.h"
#include "common\namedguid.h"
#include <uuids.h>
#include <dshow.h>
#include ".\findfilterdlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define FILTER_NAME 0
#define FILTER_GUID 1

void ReportMediaType(AM_MEDIA_TYPE*);


static const GUID *g_Categories[] =
{
 &CLSID_AudioInputDeviceCategory,
 &CLSID_AudioCompressorCategory,
 &CLSID_AudioRendererCategory,
 &CLSID_DeviceControlCategory,
 &CLSID_LegacyAmFilterCategory,
 &CLSID_TransmitCategory,
 &CLSID_MidiRendererCategory,
 &CLSID_VideoInputDeviceCategory,
 &CLSID_VideoCompressorCategory,
 &CLSID_VideoEffects1Category,
 &CLSID_VideoEffects2Category,
 &KSCATEGORY_DATADECOMPRESSOR,
 &AM_KSCATEGORY_CAPTURE,
 &KSCATEGORY_COMMUNICATIONSTRANSFORM,
 &AM_KSCATEGORY_CROSSBAR,
 &KSCATEGORY_DATATRANSFORM,
 &KSCATEGORY_INTERFACETRANSFORM,
 &KSCATEGORY_MIXER,
 &AM_KSCATEGORY_RENDER,
 &KSCATEGORY_AUDIO_DEVICE,
 &AM_KSCATEGORY_SPLITTER,
 &AM_KSCATEGORY_TVAUDIO,
 &AM_KSCATEGORY_TVTUNER,
 &AM_KSCATEGORY_VBICODEC,
 &CLSID_DVDHWDecodersCategory
};


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About
CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	m_nErrorCode = 0;
	m_sErrorText = _T("");
	//}}AFX_DATA_INIT
}
/////////////////////////////////////////////////////////////////////////////
void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
	if (pDX->m_bSaveAndValidate)
	{
		CString sErrorCode;
		DDX_Text(pDX, IDC_EDT_ERROR_CODE, sErrorCode);
		m_nErrorCode = sErrorCode.Find(_T("0x"));
		if (m_nErrorCode != -1)
		{
			sErrorCode = sErrorCode.Mid(m_nErrorCode+2);
		}
		_stscanf(sErrorCode, _T("%lx"), &m_nErrorCode);
	}
	
	DDX_Text(pDX, IDC_EDT_ERROR_TEXT, m_sErrorText);
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_BN_CLICKED(IDC_BTN_ERROR_CODE, OnBtnErrorCode)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CFindFilterDlg dialog
CFindFilterDlg::CFindFilterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFindFilterDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFindFilterDlg)
	m_sFilterGUID = _T("");
	m_sSearchString = _T("");
	m_bMatchCase = FALSE;
	m_sCategoryGUID = _T("");
	//}}AFX_DATA_INIT

	m_bAllCategories = FALSE;
	m_bExactMatch    = FALSE;
	m_nSearchBy      = FILTER_NAME;

	m_nSelectedCategory = CB_ERR;
	m_nSelectedFilter   = CB_ERR;
	m_pMoniker = NULL;

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}
/////////////////////////////////////////////////////////////////////////////
CFindFilterDlg::~CFindFilterDlg()
{
}
/////////////////////////////////////////////////////////////////////////////
void CFindFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFindFilterDlg)
	DDX_Control(pDX, IDC_BTN_SEARCH, m_btnSearch);
	DDX_Control(pDX, IDC_LIST_FOUND_FILTERS, m_cFoundFilters);
	DDX_Control(pDX, IDC_COMBO_FILTERS, m_cFilters);
	DDX_Control(pDX, IDC_COMBO_CATEGORIES, m_cCategories);
	DDX_Text(pDX, IDC_EDT_FILTER_GUID, m_sFilterGUID);
	DDX_Text(pDX, IDC_EDT_SEARCH_STRING, m_sSearchString);
	DDX_Text(pDX, IDC_EDT_CATEGORY_GUID, m_sCategoryGUID);
	DDX_Check(pDX, IDC_CK_MATCH_CASE, m_bMatchCase);
	DDX_Radio(pDX, IDC_RD_NAME, m_nSearchBy);
	DDX_Check(pDX, IDC_CK_ALL_CATEGORIES, m_bAllCategories);
	DDX_Check(pDX, IDC_CK_EXACT_MATCH, m_bExactMatch);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CFindFilterDlg, CDialog)
	//{{AFX_MSG_MAP(CFindFilterDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_COMBO_FILTERS, OnSelchangeComboFilters)
	ON_CBN_SELCHANGE(IDC_COMBO_CATEGORIES, OnSelchangeComboCategories)
	ON_BN_CLICKED(IDC_CK_ALL_CATEGORIES, OnCkAllCategories)
	ON_BN_CLICKED(IDC_CK_EXACT_MATCH, OnCkExactMatch)
	ON_BN_CLICKED(IDC_RD_GUID, OnRdNameGuid)
	ON_BN_CLICKED(IDC_BTN_SEARCH, OnBtnSearch)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_FOUND_FILTERS, OnDblclkListFoundFilters)
	ON_BN_CLICKED(IDC_CK_MATCH_CASE, OnCkMatchCase)
	ON_BN_CLICKED(IDC_BTN_SAVE, OnBtnSave)
	ON_BN_CLICKED(IDC_RD_NAME, OnRdNameGuid)
	ON_BN_CLICKED(IDC_BTN_DETECT_GUID_NAME, OnBtnDetectGuidName)
	ON_MESSAGE(WM_APP, OnUserMessage)
	ON_BN_CLICKED(IDC_SHOW_INTERFACES, OnBnClickedShowInterfaces)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
#ifdef FIND_FILTER
	ON_MESSAGE(WM_GRAPHNOTIFY, OnGraphNotify)
	ON_WM_SYSCOMMAND()
#endif
#ifdef GRAPH_CREATOR
	ON_BN_CLICKED(IDC_BTN_SET_NAME, OnBnClickedBtnSetName)
	ON_BN_CLICKED(IDC_BTN_GET_EXTRA_INFO, OnBnClickedBtnGetExtraInfo)
#endif
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CFindFilterDlg message handlers
BOOL CFindFilterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
#ifdef FIND_FILTER
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
#endif
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	HRESULT hr;
	int i, j, k, nCount;
	_TCHAR *szCatName[] = 
	{
		_T("Audio Capture Sources"),
		_T("Audio Compressors"),
		_T("Audio Renderers"),
		_T("Device Control Filters"),
		_T("DirectShow Filters"),
		_T("External Renderers"),
		_T("Midi Renderers"),
		_T("Video Capture Sources"),
		_T("Video Compressors"),
		_T("Video Effects (1 input)"),
		_T("Video Effects (2 inputs)"),
		_T("WDM Stream Decompression Devices"),
		_T("WDM Streaming Capture Devices"),
		_T("WDM Streaming Communication Transforms"),
		_T("WDM Streaming Crossbar Devices"),
		_T("WDM Streaming Data Transforms"),
		_T("WDM Streaming Interface Transforms"),
		_T("WDM Streaming Mixer Devices"),
		_T("WDM Streaming Rendering Devices"),
		_T("WDM Streaming System Audio Devices"),
		_T("WDM Streaming Tee/Splitter Devices"),
		_T("WDM Streaming TV Audio Devices"),
		_T("WDM Streaming TV Tuner Devices"),
		_T("WDM Streaming VBI Codecs"),
		_T("DVD HW Decoders"),
		NULL
	};

	CRect rc;
	m_cFoundFilters.GetClientRect(&rc);
	rc.right /= 4;
	m_cFoundFilters.InsertColumn(0, _T(""), LVCFMT_LEFT, rc.right);
	m_cFoundFilters.InsertColumn(1, _T(""), LVCFMT_LEFT, rc.right*3);
	m_cFoundFilters.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,
      LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT,
      LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);

	hr = CoInitialize(NULL);
	if (SUCCEEDED(hr))
	{
		for (i=0; szCatName[i] != NULL; i++)
		{
			m_cCategories.InsertString(i, szCatName[i]);
		}
		EnumFiltersWithMonikerToList(NULL, &CLSID_ActiveMovieCategories, &m_cFilters);
		nCount = m_cFilters.GetCount();
		for (j=0; j<nCount; j++)
		{
			m_cFilters.SetCurSel(j);
			OnSelchangeComboFilters();
			GUID guid = GetCurrentGUID();
			if (guid != GUID_NULL)
			{
				for (k=0; szCatName[k] != NULL; k++)
				{
					if (guid == *g_Categories[k])
					{
						break;
					}
				}
				if (szCatName[k] == NULL)
				{
					CString sName;
					m_cFilters.GetLBText(j, sName);
					m_cCategories.InsertString(i, sName);
					GUID *pguid = new GUID;
					*pguid = guid;
					m_cCategories.SetItemData(i++, (DWORD)pguid);
				}
			}
		}
 
		m_cCategories.SetCurSel(0);
		OnSelchangeComboCategories();
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}
/////////////////////////////////////////////////////////////////////////////
void CFindFilterDlg::OnDestroy() 
{
	int i, nCount = m_cCategories.GetCount();
	for (i=0; i<nCount; i++)
	{
		if (m_cCategories.GetItemData(i))
		{
			delete (GUID*)m_cCategories.GetItemData(i);
		}
	}
	ClearFilterListWithMoniker(m_cFilters);
	CoUninitialize();
	CDialog::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
#ifdef FIND_FILTER
void CFindFilterDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
#endif
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
/////////////////////////////////////////////////////////////////////////////
void CFindFilterDlg::OnPaint() 
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
/////////////////////////////////////////////////////////////////////////////
// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFindFilterDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}
/////////////////////////////////////////////////////////////////////////////
#ifdef FIND_FILTER
LRESULT CFindFilterDlg::OnGraphNotify(WPARAM wParam,  LPARAM lParam)
{
/*
	LONG evCode, evParam1, evParam2;
	HRESULT hr=S_OK;
	if (m_pME)
	{
		while(SUCCEEDED(m_pME->GetEvent(&evCode, (LONG_PTR *) &evParam1, (LONG_PTR *) &evParam2, 0)))
		{
			CString str;
			switch (evCode)
			{
				case EC_DEVICE_LOST: break;
				case EC_RECEIVE_MEDIA_SAMPLE:
				{
					IMediaSample *pSample    = (IMediaSample*)evParam1;
					long          nPackageNo = (long)evParam2;
					str.Format(_T("Record(%d) received (%d Bytes)\r\n"), nPackageNo, pSample->GetActualDataLength());
					pSample->Release();
				}break;
				default:
					str.Format(_T("Event(%d, %x), %x, %x\r\n"), evCode, evCode, evParam1, evParam2);
					break;
			}
			m_cReports.SetSel(-1, -1);
			m_cReports.ReplaceSel(str);
			// Free event parameters to prevent memory leaks
			hr = m_pME->FreeEventParams(evCode, evParam1, evParam2);
		}
	}
*/
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CFindFilterDlg::ReportError(ErrorStruct*ps)
{
	ASSERT(ps != NULL);
	if (FAILED(ps->m_hr))
	{
		CString str, sError, sDescription;
		GetErrorStrings(ps->m_hr, sError, sDescription);
		str.Format(_T("Error initialising Filters\n%s(%d) :\nError: %s (%x):\n %s\n"), ps->m_sFile, ps->m_nLine, sError, ps->m_hr, sDescription);
		TRACE(_T("%s\n"), str);
//		TRACE(_T("%s(%d) : %s\n"), ps->m_sFile, ps->m_nLine, pstrError);
		AfxMessageBox(str, MB_ICONERROR|MB_OK);
	}
	delete ps;
}
#endif
/////////////////////////////////////////////////////////////////////////////
LRESULT CFindFilterDlg::OnUserMessage(WPARAM wParam, LPARAM lParam)
{
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CFindFilterDlg::OnSelchangeComboFilters() 
{
	int nSel = m_cFilters.GetCurSel();
	if ((nSel != CB_ERR) && (m_nSelectedFilter != nSel))
	{
		m_nSelectedFilter = nSel;
		VARIANT varName={0};
		IPropertyBag *pPropBag;
		IMoniker *pMoniker = (IMoniker*)m_cFilters.GetItemData(nSel);
		if (pMoniker)
		{
			HRESULT hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, 
												(void **)&pPropBag);
			if (SUCCEEDED(hr))
			{
				// Read filter name from property bag
				varName.vt = VT_BSTR;
				hr = pPropBag->Read(L"CLSID", &varName, 0);
				if (SUCCEEDED(hr))
				{
					CString str(varName.bstrVal);
					SysFreeString(varName.bstrVal);
					GUID guid = MakeGUID(str);
					char *psz =  GuidNames[guid];
					CString strName(psz);
					if (strName.Find(_T("Unknown")) != -1)
					{
						GetGUIDString(strName, &guid);
					}
					if (   strName == _T("GUID_NULL")
						|| strName.GetLength() <= 2) strName = _T("Unknown");
					m_sFilterGUID = strName + _T(" : ") + str;
				}
				else
				{
					m_sFilterGUID.Empty();
				}
			}
			else
			{
				m_sFilterGUID.Empty();
			}
		}
		else
		{
			m_sFilterGUID.Empty();
		}
	}
	else
	{
		m_sFilterGUID.Empty();
	}
	CDataExchange dx(this, FALSE);
	DDX_Text(&dx, IDC_EDT_FILTER_GUID, m_sFilterGUID);
}
/////////////////////////////////////////////////////////////////////////////
void CFindFilterDlg::OnSelchangeComboCategories() 
{
	int nSel = m_cCategories.GetCurSel();
	if ((nSel != CB_ERR) && (nSel != m_nSelectedCategory))
	{
		m_nSelectedCategory = nSel;
		ClearFilterListWithMoniker(m_cFilters);
		GUID *pguid;
		if (m_cCategories.GetItemData(nSel))
		{
			pguid = (GUID*)m_cCategories.GetItemData(nSel);
		}
		else
		{
			pguid = (GUID*)g_Categories[nSel];
		}
		EnumFiltersWithMonikerToList(NULL, pguid, &m_cFilters);

		CString strGUID;
		CString strName;
		GetGUIDString(strName, pguid);
		strGUID = GetString(*pguid);
		if (strName == _T("GUID_NULL")) strName = _T("Unknown");
		m_sCategoryGUID = strName + _T(" : ") + strGUID;
		CDataExchange dx(this, FALSE);
		DDX_Text(&dx, IDC_EDT_CATEGORY_GUID, m_sCategoryGUID);

		m_nSelectedFilter = CB_ERR;
		m_cFilters.SetCurSel(0);
		OnSelchangeComboFilters();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CFindFilterDlg::OnCkAllCategories() 
{
	CDataExchange dx(this, TRUE);
	DDX_Check(&dx, IDC_CK_ALL_CATEGORIES, m_bAllCategories);
}
/////////////////////////////////////////////////////////////////////////////
void CFindFilterDlg::OnCkExactMatch() 
{
	CDataExchange dx(this, TRUE);
	DDX_Check(&dx, IDC_CK_EXACT_MATCH, m_bExactMatch);
}
/////////////////////////////////////////////////////////////////////////////
void CFindFilterDlg::OnCkMatchCase() 
{
	CDataExchange dx(this, TRUE);
	DDX_Check(&dx, IDC_CK_MATCH_CASE, m_bMatchCase);
}
/////////////////////////////////////////////////////////////////////////////
void CFindFilterDlg::OnRdNameGuid() 
{
	CDataExchange dx(this, TRUE);
	DDX_Radio(&dx, IDC_RD_NAME, m_nSearchBy);
	GetDlgItem(IDC_CK_EXACT_MATCH)->EnableWindow((m_nSearchBy == FILTER_NAME));
}
/////////////////////////////////////////////////////////////////////////////
void CFindFilterDlg::OnBtnSearch() 
{
	UpdateData();
	m_btnSearch.EnableWindow(FALSE);
	if (m_sSearchString.IsEmpty())
	{
		AfxMessageBox(IDS_EMPTY_SEARCH_STRING);
		return;
	}
	else if (!m_bExactMatch && (m_nSearchBy == FILTER_NAME))
	{
		int nFind, nStart = 0;
		CString str;
		while ((nFind = m_sSearchString.Find(_T("&&"), nStart)) != -1)
		{
			str = m_sSearchString.Mid(nStart, nFind-nStart);
			str.TrimRight();
			m_saSearchAnd.Add(str);
			nStart = nFind+2;
		}

		if (nStart != 0)	// letzten And-Wert hinzufügen
		{
			str = m_sSearchString.Mid(nStart);
			str.TrimLeft();
			m_saSearchAnd.Add(str);
		}
		else					// keine Ands 
		{
			while ((nFind = m_sSearchString.Find(_T("||"), nStart)) != -1)
			{
				str = m_sSearchString.Mid(nStart, nFind-nStart);
				str.TrimRight();
				m_saSearchOr.Add(str);
				nStart = nFind+2;
			}
			if (nStart != 0)	// letzten Or-Wert hinzufügen
			{
				str = m_sSearchString.Mid(nStart);
				str.TrimLeft();
				m_saSearchOr.Add(str);
			}
		}
	}

	if (m_nSearchBy == FILTER_GUID)
	{
		m_guidSearch = MakeGUID(m_sSearchString);
	}
	m_cFoundFilters.DeleteAllItems();
	if (m_bAllCategories)
	{
		int i, nCat = m_cCategories.GetCount();
		for (i=0; i<nCat; i++)
		{
			m_cCategories.SetCurSel(i);
			OnSelchangeComboCategories();
			SearchInCurrenCategory();
		}
	}
	else
	{
		SearchInCurrenCategory();
	}
	m_btnSearch.EnableWindow(TRUE);
}
/////////////////////////////////////////////////////////////////////////////
void CFindFilterDlg::SearchInCurrenCategory()
{
	if (m_nSearchBy == FILTER_NAME)
	{
		int nFind = CB_ERR, nCount = m_cFilters.GetCount(), nOld = -1;
		do 
		{
			if (m_bExactMatch)
			{
				nFind = m_cFilters.FindStringExact(nFind, m_sSearchString);
			}
			else
			{
				nFind = FindFilterName(nFind);
			}
			if (nFind == CB_ERR) break;
			if (nFind <= nOld)   break;

			m_cFilters.SetCurSel(nFind);
			OnSelchangeComboFilters();
			AddToFindList();
			nOld = nFind;
		} while(nFind < nCount);
	}
	else if (m_nSearchBy == FILTER_GUID)
	{
		int i = 0, nFind,
			 nFilter = m_cFilters.GetCount(),
			 nCurSel = m_cFilters.GetCurSel();
		if (nCurSel != 0)
		{
			m_cFilters.SetCurSel(0);
			OnSelchangeComboFilters();
		}
		do 
		{
			if (!m_sFilterGUID.IsEmpty())
			{
				nFind = m_sFilterGUID.Find(m_sSearchString);
				if (nFind != -1)
				{
					AddToFindList();
				}
				else
				{
					if ((m_guidSearch != GUID_NULL) && (m_guidSearch == GetCurrentGUID()))
					{
						AddToFindList();
					}
				}
			}
			m_cFilters.SetCurSel(++i);
			OnSelchangeComboFilters();
		} while(i < nFilter);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CFindFilterDlg::AddToFindList()
{
	int     nItem = m_cFoundFilters.GetItemCount();
	int     nSel  = m_cFilters.GetCurSel();
	CString sName;
	m_cFilters.GetLBText(nSel, sName);
	nItem = m_cFoundFilters.InsertItem(nItem, sName);
	m_cFoundFilters.SetItem(nItem, 1, LVIF_TEXT, m_sFilterGUID, m_cCategories.GetCurSel(), 0, 0, m_cFilters.GetCurSel());
	m_cFoundFilters.SetItem(nItem, 0, LVIF_PARAM|LVIF_IMAGE, NULL, m_cCategories.GetCurSel(), 0, 0, m_cFilters.GetCurSel());
}
/////////////////////////////////////////////////////////////////////////////
void CFindFilterDlg::OnDblclkListFoundFilters(NMHDR* pNMHDR, LRESULT* pResult) 
{
   CPoint ptCursor, ptScreen;
   UINT Flags;
   LV_ITEM lvItem = { LVIF_STATE|LVIF_PARAM|LVIF_IMAGE, -1, 0, 0, LVIS_SELECTED, NULL, 0, 0, NULL};
   GetCursorPos(&ptCursor);
   ptScreen = ptCursor;
   m_cFoundFilters.ScreenToClient(&ptCursor);
   ptCursor.x = 2;

   lvItem.iItem = m_cFoundFilters.HitTest(ptCursor, &Flags);
   if (lvItem.iItem != -1)
   {
		m_cFoundFilters.GetItem(&lvItem);
		m_cCategories.SetCurSel(lvItem.iImage);
		OnSelchangeComboCategories();
		m_cFilters.SetCurSel(lvItem.lParam);
		OnSelchangeComboFilters();
	}
	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
GUID CFindFilterDlg::GetCurrentGUID()
{
	if (!m_sFilterGUID.IsEmpty())
	{
		int nFind = m_sFilterGUID.Find(_T(" : "));
		if (nFind != -1)
		{
			m_sFilterGUID = m_sFilterGUID.Mid(nFind+2);
			return MakeGUID(m_sFilterGUID);
		}
	}
	return GUID_NULL;
}
/////////////////////////////////////////////////////////////////////////////
int CFindFilterDlg::FindFilterName(int nStartAfter)
{
	CString sName, sSearch = m_sSearchString;
	int i, j, nFind,
		 nSizeAnd = m_saSearchAnd.GetSize(),
		 nSizeOr  = m_saSearchOr.GetSize(), 
		 nCount   = m_cFilters.GetCount();
	if (nStartAfter <  -1    ) return CB_ERR;
	if (nStartAfter >= nCount) return CB_ERR;

	if (!m_bMatchCase && !nSizeAnd && !nSizeOr)
	{
		sSearch.MakeUpper();
	}
	
	for (i=nStartAfter+1; i<nCount; i++)
	{
		m_cFilters.GetLBText(i, sName);
		if (!m_bMatchCase)
		{
			sName.MakeUpper();
		}
		
		if (nSizeOr)
		{
			for (j=0; j<nSizeOr; j++)
			{
				sSearch = m_saSearchOr.GetAt(j);
				if (!m_bMatchCase) sSearch.MakeUpper();
				nFind = sName.Find(sSearch);
				if (nFind != -1)
				{
					return i;
				}
			}
		}
		else if (nSizeAnd)
		{
			int nCounterAnd = 0;
			for (j=0; j<nSizeAnd; j++)
			{
				sSearch = m_saSearchAnd.GetAt(j);
				if (!m_bMatchCase) sSearch.MakeUpper();
				nFind = sName.Find(sSearch);
				if (nFind != -1)
				{
					nCounterAnd++;
				}
			}
			if (nCounterAnd == nSizeAnd)
			{
				return i;
			}
		}
		else
		{
			nFind = sName.Find(sSearch);
			if (nFind != -1)
			{
				return i;
			}
		}
	}
	return CB_ERR;
}
/////////////////////////////////////////////////////////////////////////////
void CFindFilterDlg::OnOK() 
{
	int nCurSel = m_cFilters.GetCurSel();
	if (nCurSel != CB_ERR)
	{
		m_pMoniker = (IMoniker*)m_cFilters.GetItemData(nCurSel);
		m_cFilters.GetLBText(nCurSel, m_sFilterName);
		if (m_pMoniker)
		{
			m_pMoniker->AddRef();
		}
	}
#ifdef FIND_FILTER
	CDialog::OnOK();
#endif
}
/////////////////////////////////////////////////////////////////////////////
void CFindFilterDlg::OnBtnSave() 
{
	CFileDialog dialog(FALSE, _T("EXE"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Text Datei (*.txt)|*.txt||"), this);
	if (dialog.DoModal() == IDOK)
	{
		CFile file;
		if (file.Open(dialog.GetPathName(), CFile::modeCreate|CFile::modeWrite))
		{
			CArchive ar(&file, CArchive::store);
			int  nCount;
			CString str, strSpace = _T("   "), strNL = _T("\r\n");
			CString strGUID;
			CString strName, strText;
			LV_ITEM lvItem = { LVIF_STATE|LVIF_PARAM|LVIF_IMAGE, -1, 0, 0, LVIS_SELECTED, NULL, 0, 0, NULL};
			str = _T("Suchtext: ") + m_sSearchString + strNL;
			ar.WriteString(str);
			nCount = m_cFoundFilters.GetItemCount();
			for (lvItem.iItem=0; lvItem.iItem<nCount; lvItem.iItem++)
			{
				ar.WriteString(strNL);
				m_cFoundFilters.GetItem(&lvItem);
				str.Format(_T("%d: %s"), lvItem.iItem+1, m_cFoundFilters.GetItemText(lvItem.iItem, 0));
				ar.WriteString(str + strNL);
				str = strSpace + m_cFoundFilters.GetItemText(lvItem.iItem, 1) + strNL;
				ar.WriteString(str);

				GUID *pguid;
				if (m_cCategories.GetItemData(lvItem.iImage))
				{
					pguid = (GUID*)m_cCategories.GetItemData(lvItem.iImage);
				}
				else
				{
					pguid = (GUID*)g_Categories[lvItem.iImage];
				}
				GetGUIDString(strName, pguid);
				strGUID = GetString(*pguid);
				m_cCategories.GetLBText(lvItem.iImage, strText);
				if (strName == _T("GUID_NULL")) strName = _T("Unknown");
				str = strSpace + _T("Kategorie: ") + strText + _T(": ") + strName + _T(" : ") + strGUID + strNL;
				ar.WriteString(str);
			}
		}		
	}
}
/////////////////////////////////////////////////////////////////////////////
void CFindFilterDlg::OnBtnDetectGuidName() 
{
	UpdateData();
	GUID guid = MakeGUID(m_sSearchString);
	GetGUIDString(m_sSearchString, &guid);
	if (m_sSearchString == _T("GUID_NULL")) m_sSearchString.LoadString(IDS_UNKNOWN);
	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CAboutDlg::OnBtnErrorCode() 
{
	UpdateData();
	CString sError, sDescription;
	GetErrorStrings((HRESULT)m_nErrorCode, sError, sDescription);
	m_sErrorText = sError + _T("\r\n") + sDescription;
	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CFindFilterDlg::OnBnClickedShowInterfaces()
{
	CString str;
	CRegKey reg;

	int nCurSel = m_cFilters.GetCurSel();
	if (nCurSel != CB_ERR)
	{
		IMoniker *pMoniker = (IMoniker*)m_cFilters.GetItemData(nCurSel);
		if (pMoniker)
		{
			IUnknown *pUnk = NULL;
			HRESULT hr = pMoniker->BindToObject(0, 0, IID_IUnknown, (void**)&pUnk);
#ifdef GRAPH_CREATOR
			CStringArray sa;
			theApp.GetInterfaces(pUnk, sa);
			ConcatenateStrings(sa, str, _T('\n'));
#endif
#ifdef FIND_FILTER
			if (   pUnk != NULL 
				&& ERROR_SUCCESS == reg.Open( HKEY_CLASSES_ROOT, _T("Interface")))
			{
				TCHAR szKeyName[1024] = _T("");
				for ( int i = 0; ERROR_SUCCESS == RegEnumKey( reg.m_hKey, i, szKeyName, 1024 ); i++)
				{
					USES_CONVERSION;
					CLSID clsID = CLSID_NULL;
					if ( SUCCEEDED( CLSIDFromString( T2OLE(szKeyName), &clsID ) ) )
					{
						IUnknown * spObject = NULL;
						if ( SUCCEEDED( pUnk->QueryInterface( clsID, (void**)&spObject ) ) )
						{
							TCHAR szValue[1024] = _T(""); 
							LONG ncbValue = 1024;
							if ( ERROR_SUCCESS == RegQueryValue( reg.m_hKey, szKeyName, szValue, &ncbValue ) )
							{
								CString sVal(szValue);
								str += sVal + _T("\r\n");
							}
						}
						RELEASE_OBJECT(spObject);
					}
				}
				reg.Close();
			}
#endif
			RELEASE_OBJECT(pUnk);
		}
	}
	AfxMessageBox(str);
}
/////////////////////////////////////////////////////////////////////////////
void CFindFilterDlg::OnBnClickedOk()
{
#ifdef GRAPH_CREATOR
	OnOK();
	m_pParentWnd->SendMessage(WM_USER, WPARAM_INSERT_FILTER);	
#endif
}
/////////////////////////////////////////////////////////////////////////////

void CFindFilterDlg::OnBnClickedCancel()
{
	OnCancel();
#ifdef GRAPH_CREATOR
	DestroyWindow();
#endif
}
/////////////////////////////////////////////////////////////////////////////
#ifdef GRAPH_CREATOR
void CFindFilterDlg::OnBnClickedBtnSetName()
{
	int nSel = m_cFilters.GetCurSel();
	if (nSel != CB_ERR)
	{
		CString sTxt, sOldName, sNewName;
		m_cFilters.GetLBText(nSel, sOldName);
		GetDlgItemText(IDC_EDT_SEARCH_STRING, sNewName);
		sTxt.Format(IDS_REQUES_CHANGE_FILTER_NAME, sOldName, sNewName);
		if (AfxMessageBox(sTxt, MB_YESNO|MB_ICONQUESTION, 0) == IDYES)
		{
			VARIANT varName={0};
			IPropertyBag *pPropBag;
			IMoniker *pMoniker = (IMoniker*)m_cFilters.GetItemData(nSel);
			if (pMoniker)
			{
				HRESULT hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, 
													(void **)&pPropBag);
				if (SUCCEEDED(hr))
				{
					// Read filter name from property bag
					varName.vt = VT_BSTR;
					varName.bstrVal = sNewName.AllocSysString();
					hr = pPropBag->Write(L"FriendlyName", &varName);
					SysFreeString(varName.bstrVal);
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CFindFilterDlg::OnBnClickedBtnGetExtraInfo()
{
	int nSel = m_cFilters.GetCurSel();
	if (nSel != CB_ERR)
	{
		VARIANT varName={0};
		IPropertyBag *pPropBag;
		IMoniker *pMoniker = (IMoniker*)m_cFilters.GetItemData(nSel);
		if (pMoniker)
		{
			m_cFoundFilters.DeleteAllItems();
			LPOLESTR bstr = NULL;
			HRESULT hr = pMoniker->GetDisplayName(NULL, NULL, &bstr);
			if (SUCCEEDED(hr))
			{
				CString sName(bstr), sItem = _T("DisplayName");
				int nItem = m_cFoundFilters.GetItemCount();
				nItem = m_cFoundFilters.InsertItem(nItem, sItem);
				m_cFoundFilters.SetItemText(nItem, 1, sName);
			}
			hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pPropBag);
			if (SUCCEEDED(hr))
			{
				varName.vt = VT_BSTR;
				hr = pPropBag->Read(L"ExtensionDLL", &varName, 0);
				if (SUCCEEDED(hr))
				{
					CString sName(varName.bstrVal), sItem = _T("ExtensionDLL");
					int nItem = m_cFoundFilters.GetItemCount();
					nItem = m_cFoundFilters.InsertItem(nItem, sItem);
					m_cFoundFilters.SetItemText(nItem, 1, sName);
					SysFreeString(varName.bstrVal);
				}
			}
		}
	}
}
#endif

