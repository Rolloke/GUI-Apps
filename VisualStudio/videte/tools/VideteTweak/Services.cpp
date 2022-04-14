// Services.cpp : implementation file
//

#include "stdafx.h"
#include "videtetweak.h"
#include "Services.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define PRODUCT_DTS  0
#define PRODUCT_IDIP 1
#define PRODUCT_ORG 2

#define SERVICE_DTS  "SeviceDTS"
#define SERVICE_IDIP "SeviceIdip"
#define SERVICE_ORG  "SeviceOriginal"

/////////////////////////////////////////////////////////////////////////////
// CServices property page

IMPLEMENT_DYNCREATE(CServices, CPropertyPage)

CServices::CServices() : CPropertyPage(CServices::IDD)
{
	//{{AFX_DATA_INIT(CServices)
	m_nProduct = PRODUCT_ORG;
	//}}AFX_DATA_INIT
	m_bChanged = false;
}

CServices::~CServices()
{
}

void CServices::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CServices)
	DDX_Control(pDX, IDC_LST_SERVICES, m_cServices);
	DDX_Radio(pDX, IDC_RD_DTS, m_nProduct);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CServices, CPropertyPage)
	//{{AFX_MSG_MAP(CServices)
	ON_BN_CLICKED(IDC_BTN_RESTORE, OnBtnRestore)
	ON_BN_CLICKED(IDC_BTN_SAVE, OnBtnSave)
	ON_NOTIFY(NM_RCLICK, IDC_LST_SERVICES, OnRclickLstServices)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CServices message handlers

void CServices::OnBtnRestore() 
{
	CDataExchange dx(this, true);
	DDX_Radio(&dx, IDC_RD_DTS, m_nProduct);
   CWinApp *pApp = AfxGetApp();
	char *pszSave = SERVICE_ORG;
	if (m_nProduct == PRODUCT_DTS)  pszSave = SERVICE_DTS;
	if (m_nProduct == PRODUCT_IDIP) pszSave = SERVICE_IDIP;
	
	int nItem, nCount = m_cServices.GetItemCount();
	for (nItem=0; nItem<nCount; nItem++)
	{
		DWORD dwData = pApp->GetProfileInt(pszSave, m_cServices.GetItemText(nItem, 2), 0);
		DWORD dwOld  = m_cServices.GetItemData(nItem);
		if ((dwData != 0) && (dwData != dwOld))
		{
			m_cServices.SetItemData(nItem, dwData);
			m_cServices.SetItemText(nItem, 1, GetStartText(dwData));
			m_bChanged = TRUE;
		}
	}
	if(m_bChanged)
	{
		SetModified();	
	}
}

void CServices::OnBtnSave() 
{
	CDataExchange dx(this, true);
	DDX_Radio(&dx, IDC_RD_DTS, m_nProduct);
   CWinApp *pApp = AfxGetApp();
	char *pszSave = SERVICE_ORG;
	if (m_nProduct == PRODUCT_DTS) pszSave = SERVICE_DTS;
	if (m_nProduct == PRODUCT_IDIP)pszSave = SERVICE_IDIP;

	int     nItem, nCount = m_cServices.GetItemCount();
	for (nItem=0; nItem<nCount; nItem++)
	{
		pApp->WriteProfileInt(pszSave, m_cServices.GetItemText(nItem, 2), m_cServices.GetItemData(nItem));
	}	
}

BOOL CServices::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	CString sHeadline;
	CRect   rc;
	LONG    lResult;
	DWORD   dwValue           = 0,
		     dwType, dwLen, dwIndex,
			  dwCount           = sizeof(DWORD);
	HKEY    hKey = NULL, hSecKey  = NULL;
	CHAR    pszName[MAX_PATH];
	int     nItem = 0;

   m_cServices.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,
      LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT,
      LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	
	sHeadline.LoadString(IDS_NAME);
	m_cServices.GetClientRect(&rc);
	LV_COLUMN lvc = {LVCF_TEXT|LVCF_WIDTH, 0, rc.right/2, (char*)LPCTSTR(sHeadline), sHeadline.GetLength(), 0, 0, 0};
	m_cServices.InsertColumn(0, &lvc);
	
	sHeadline.LoadString(IDS_START_TYPE);
	lvc.pszText = (char*)LPCTSTR(sHeadline);
	lvc.cchTextMax = sHeadline.GetLength();
	m_cServices.InsertColumn(1, &lvc);

	sHeadline.LoadString(IDS_KEY);
	lvc.pszText = (char*)LPCTSTR(sHeadline);
	lvc.cchTextMax = sHeadline.GetLength();
	m_cServices.InsertColumn(2, &lvc);

	sHeadline.LoadString(IDS_DESCRIPTION);
	lvc.pszText = (char*)LPCTSTR(sHeadline);
	lvc.cchTextMax = sHeadline.GetLength();
	m_cServices.InsertColumn(3, &lvc);

	lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, HKLM_SERVICES, 0, KEY_READ, &hKey);
	if (lResult == ERROR_SUCCESS)
	{
		for (dwIndex=0; ; dwIndex++)
		{
			dwLen   = MAX_PATH;
			lResult = RegEnumKeyEx(hKey, dwIndex, pszName, &dwLen, NULL, NULL, 0, NULL);
			if (lResult != ERROR_SUCCESS) break;
			HKEY  hSecKey = NULL;
			lResult = RegOpenKeyEx(hKey, pszName, 0, KEY_READ, &hSecKey);
			if (lResult == ERROR_SUCCESS)
			{
				CString sSection = pszName, sStart;
				lResult = RegQueryValueEx(hSecKey, SERVICE_START_TYPE, NULL, &dwType, (LPBYTE)&dwValue, &dwCount);
				if (lResult == ERROR_SUCCESS)
				{
					sStart = GetStartText(dwValue);
				}
				else sStart.Empty();
				dwLen   = MAX_PATH;
				lResult = RegQueryValueEx(hSecKey, SERVICE_DISP_NAME, NULL, &dwType, (LPBYTE)pszName, &dwLen);
				if ((lResult == ERROR_SUCCESS) && !sStart.IsEmpty())
				{
					nItem = m_cServices.InsertItem(nItem, pszName);
					m_cServices.SetItemText(nItem, 1, sStart);
					m_cServices.SetItemData(nItem, dwValue);
					m_cServices.SetItemText(nItem, 2, sSection);
					dwLen   = MAX_PATH;
					lResult = RegQueryValueEx(hSecKey, SERVICE_DESCRIPTIONTXT, NULL, &dwType, (LPBYTE)pszName, &dwLen);
					if (lResult == ERROR_SUCCESS)
					{
						m_cServices.SetItemText(nItem, 3, pszName);
					}
					nItem ++;
				}
				RegCloseKey(hSecKey);
			}
		}
		RegCloseKey(hKey);
	}

   CWinApp *pApp = AfxGetApp();
	if (pApp->GetProfileInt(SERVICE_ORG, m_cServices.GetItemText(0, 2), 0) == 0)
	{
		OnBtnSave();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CServices::OnRclickLstServices(NMHDR* pNMHDR, LRESULT* pResult) 
{
   CListCtrl*pWnd = (CListCtrl*) CWnd::FromHandle(pNMHDR->hwndFrom);
	CPoint pt;
	UINT   nFlags = 0;
	GetCursorPos(&pt);
	pWnd->ScreenToClient(&pt);
	int nHit = pWnd->HitTest(pt, &nFlags);
	if (nHit != -1) 
	{
		LVHITTESTINFO hti = {{pt.x, pt.y}, 0, nHit, 0};
		pWnd->SendMessage(LVM_SUBITEMHITTEST, 0, (LPARAM)&hti);
		if (hti.iSubItem == 0 || hti.iSubItem == 3)
		{
			CString s = m_cServices.GetItemText(nHit, 3);
			if (!s.IsEmpty())
			{
				AfxMessageBox(s);
			}
		}
		else
		{
			CString sStart;
			DWORD dwValue = m_cServices.GetItemData(nHit);
			if      (dwValue == 3)
			{
				dwValue = 2;
			}
			else if (dwValue == 2)
			{
				dwValue = 4;
			}
			else if (dwValue == 4)
			{
				dwValue = 3;
			}
			m_cServices.SetItemData(nHit, dwValue);
			m_cServices.SetItemText(nHit, 1, GetStartText(dwValue));
			m_bChanged = TRUE;
			SetModified();	
		}
	}	
	*pResult = 0;
}

BOOL CServices::OnApply() 
{
	if (m_bChanged)
	{
		LONG    lResult;
		DWORD   dwValue       = 0,
				  dwCount       = sizeof(DWORD);
		int     nItem, nCount = m_cServices.GetItemCount();
		HKEY    hKey = NULL, hSecKey  = NULL;
		lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, HKLM_SERVICES, 0, KEY_WRITE, &hKey);
		if (lResult == ERROR_SUCCESS)
		{
			for (nItem=0; nItem<nCount; nItem++)
			{
				lResult = RegOpenKeyEx(hKey, m_cServices.GetItemText(nItem, 2), 0, KEY_WRITE, &hSecKey);
				if (lResult == ERROR_SUCCESS)
				{
					dwValue = m_cServices.GetItemData(nItem);
					lResult = RegSetValueEx(hSecKey, SERVICE_START_TYPE, 0, REG_DWORD, (LPBYTE)&dwValue, dwCount);
					RegCloseKey(hSecKey);
				}
			}
			RegCloseKey(hKey);
		}
		SetModified(FALSE);
		m_bChanged = false;
	}
	
	return CPropertyPage::OnApply();
}

CString CServices::GetStartText(DWORD dwValue)
{
	CString sStart;
	if      (dwValue == 3) sStart.LoadString(IDS_MANUAL);
	else if (dwValue == 2) sStart.LoadString(IDS_AUTOMATIC);
	else if (dwValue == 4) sStart.LoadString(IDS_DEACTIVE);
	else                   sStart.Empty();
	return sStart;
}
