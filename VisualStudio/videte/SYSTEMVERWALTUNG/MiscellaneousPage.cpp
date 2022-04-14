// LanguagePage.cpp : implementation file
//

#include "stdafx.h"
#include "SystemVerwaltung.h"
#include "MiscellaneousPage.h"

#define VALUE_SIMPLE 0	// REG_SZ, REG_DWORD
#define VALUE_SIGNED 1	// REG_DWORD
#define VALUE_FLOAT  2	// REG_BINARY
#define VALUE_BOOL   3	// REG_DWORD (ja=1, nein=0)
#define VALUE_HEX    4	// REG_DWORD
#define VALUE_DOUBLE 5	// REG_BINARY

#define SIZE_UPDATE 10
/////////////////////////////////////////////////////////////////////////////
// class CMiscellaneousEntry
CMiscellaneousEntry::CMiscellaneousEntry()
{
	m_dwValue = 0;
	m_dwDefaultValue = 0;
	m_wRegType  = REG_NONE;
	m_wValueType = VALUE_SIMPLE;
	m_Wai = WAI_INVALID;
}
/////////////////////////////////////////////////////////////////////////////
CMiscellaneousEntry::CMiscellaneousEntry(CString sName, CString sSection, CString sKey, DWORD dwDefault, WORD wRegType, WORD wValueType, WK_ApplicationId wai)
{
	m_sName = sName;
	m_sSection = sSection;
	m_sKey = sKey;
	m_dwDefaultValue = dwDefault;
	m_wRegType = wRegType;
	m_wValueType = wValueType;
	m_Wai = wai;
}
/////////////////////////////////////////////////////////////////////////////
CMiscellaneousEntry::CMiscellaneousEntry(UINT nIDName, CString sSection, CString sKey, DWORD dwDefault, WORD wRegType, WORD wValueType, WK_ApplicationId wai)
{
	m_sName.LoadString(nIDName);
	m_sSection = sSection;
	m_sKey = sKey;
	m_dwDefaultValue = dwDefault;
	m_wRegType = wRegType;
	m_wValueType = wValueType;
	m_Wai = wai;
}
/////////////////////////////////////////////////////////////////////////////
CMiscellaneousEntry::~CMiscellaneousEntry()
{
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMiscellaneousEntry::Load(CWK_Profile &wkp)
{
	BOOL bReturn = TRUE;
	switch (m_wRegType)
	{
		case REG_DWORD:
			m_dwValue = wkp.GetInt(m_sSection, m_sKey, m_dwDefaultValue);
			break;
		case REG_SZ:
			m_sValue = wkp.GetString(m_sSection, m_sKey, NULL);
			break; 
		case REG_BINARY:
			{
				BYTE*pData = NULL;
				if (wkp.GetBinary(m_sSection, m_sKey, &pData, (UINT*)&m_dwValue))
				{
					if (m_dwValue == sizeof(double))
					{
						m_sValue.Format(_T("%f"), *((double*)pData));
						ASSERT(m_wValueType == VALUE_DOUBLE);
					}
					else if (m_dwValue == sizeof(float))
					{
						m_sValue.Format(_T("%f"), *((float*)pData));
						ASSERT(m_wValueType == VALUE_FLOAT);
					}
				}
				else if (m_wValueType == VALUE_DOUBLE)
				{
					m_sValue = _T("0.0");
					m_dwValue = sizeof(double);
				}
				else if (m_wValueType == VALUE_FLOAT)
				{
					m_sValue = _T("0.0");
					m_dwValue = sizeof(float);
				}
				else
				{
					ASSERT(FALSE);
				}
				WK_DELETE(pData);
			}
			break;
		default:
			bReturn = FALSE;
			break;
	}
	m_bChanged = FALSE;
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMiscellaneousEntry::Save(CWK_Profile &wkp)
{
	BOOL bReturn = m_bChanged;
	if (m_bChanged)
	{
		switch (m_wRegType)
		{
			case REG_DWORD:
				wkp.WriteInt(m_sSection, m_sKey, m_dwValue);
				break;
			case REG_SZ:
				wkp.WriteString(m_sSection, m_sKey, m_sValue);
				break;
			case REG_BINARY:
				if (m_wValueType == VALUE_DOUBLE)
				{
					double dValue;
					if (_stscanf(m_sValue, _T("%f"), &dValue) == 1)
					{
						wkp.WriteBinary(m_sSection, m_sKey, (BYTE*)&dValue, sizeof(double));
					}
					else
					{
						bReturn = FALSE;
					}
				}
				else if (m_wValueType == VALUE_FLOAT)
				{
					float fValue;
					if (_stscanf(m_sValue, _T("%f"), &fValue) == 1)
					{
						wkp.WriteBinary(m_sSection, m_sKey, (BYTE*)&fValue, sizeof(float));
					}
					else
					{
						bReturn = FALSE;
					}
				}
				else
				{
					bReturn = FALSE;
				}
				break;
			default:
				bReturn = FALSE;
				break;
		}
		m_bChanged = FALSE;
	}
	return bReturn;
}

/////////////////////////////////////////////////////////////////////////////
// class CMiscellaneousList
CMiscellaneousList::CMiscellaneousList()
{
	m_pnWidth = NULL;
}
CMiscellaneousList::~CMiscellaneousList()
{
	if (m_pnWidth)
	{
		free(m_pnWidth);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMiscellaneousList::DoDataExchange(CDataExchange* pDX)
{
	CMiscellaneousEntry*pEntry = (CMiscellaneousEntry*)GetItemData(m_Selected.Cell.wItem);
	if (   pEntry
		&& (    m_Selected.Cell.wSubItem == 1
		    || (m_Selected.Cell.wSubItem == 2
			    && pDX->m_bSaveAndValidate==FALSE)))
	{
		BOOL bChanged = FALSE;
		switch (pEntry->m_wRegType)
		{
			case REG_DWORD:
			{
				DWORD *pValue = m_Selected.Cell.wSubItem == 1 ? &pEntry->m_dwValue : &pEntry->m_dwDefaultValue;
				DWORD dwValue = *pValue;
				if (pEntry->m_wValueType == VALUE_SIGNED)
				{
					DDX_Text(pDX, GetSelectionID(), (long&)*pValue);
				}
				else if (pEntry->m_wValueType == VALUE_HEX)
				{
					DDX_Text(pDX, _T("0x%08x"), AFX_IDP_PARSE_REAL, GetSelectionID(), (int&)*pValue);
				}
				else if (pEntry->m_wValueType == VALUE_BOOL)
				{
					CString s;
					if (pDX->m_bSaveAndValidate)
					{
						CString sYes;
						sYes.LoadString(IDS_YES);
						DDX_Text(pDX, GetSelectionID(), s);
						*pValue = s == sYes;
					}
					else
					{
						s.LoadString(*pValue ? IDS_YES : IDS_NO);
						DDX_Text(pDX, GetSelectionID(), s);
					}
				}
				else
				{
					DDX_Text(pDX, GetSelectionID(), *pValue);
				}
				bChanged = dwValue != *pValue;
			}
			break;
			case REG_SZ:
			{
				CString sValue = pEntry->m_sValue;
				DDX_Text(pDX, GetSelectionID(), pEntry->m_sValue);
				bChanged = sValue != pEntry->m_sValue;
			}
			break;
			case REG_BINARY:
			{
				CString sValue = pEntry->m_sValue;
				DDX_Text(pDX, GetSelectionID(), pEntry->m_sValue);
				bChanged = sValue != pEntry->m_sValue;
			}
			break;
		}
		if (!pDX->m_bSaveAndValidate)
		{
			pDX->m_bSaveAndValidate = 3;
		}
		else if (bChanged)
		{
			CWnd *pWnd = GetParent();
			ASSERT_KINDOF(CMiscellaneousPage, pWnd);
			((CMiscellaneousPage*)pWnd)->SetModified(1, 0);
			pEntry->m_bChanged = TRUE;
		}
	}
	// RKE: call base class function at the end
	CEdtSubItemListCtrl::DoDataExchange(pDX);
	if (pDX->m_bSaveAndValidate == 3)
	{
		pDX->m_bSaveAndValidate = 0;
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMiscellaneousList::CanEditSubItem(int nItem, int nSubItem)
{
	return (nSubItem == 1) && GetItemData(nItem) ? TRUE : FALSE;
}
/////////////////////////////////////////////////////////////////////////////
eEditType CMiscellaneousList::GetEditType(int nItem, int nSubItem)
{
	CMiscellaneousEntry*pEntry = (CMiscellaneousEntry*)GetItemData(m_Selected.Cell.wItem);
	if (pEntry && pEntry->m_wValueType == VALUE_BOOL)
	{
		return COMBO_BOX;
	}

	return EDIT_FIELD;
}
/////////////////////////////////////////////////////////////////////////////
void CMiscellaneousList::InitComboBox(CSubItemCombo*pCombo, const CString& sText)
{
//	CMiscellaneousEntry*pEntry = (CMiscellaneousEntry*)GetItemData(m_Selected.Cell.wItem);
//	if (pEntry && pEntry->m_wValueType == VALUE_BOOL)
	CString s;
	s.LoadString(IDS_NO);
	pCombo->InsertString(0, s);
	s.LoadString(IDS_YES);
	pCombo->InsertString(1, s);

	pCombo->SetDroppedHeight(100);

	int iCursel = pCombo->FindString(0, sText);
	if (iCursel != CB_ERR)
	{
		pCombo->SetCurSel(iCursel);
	}

}
/////////////////////////////////////////////////////////////////////////////
DWORD CMiscellaneousList::GetItemStyle(int nItem, int nSubItem, DWORD dwStyle)
{
	return dwStyle;
}
void CMiscellaneousList::DrawSubItem(CDC*pDC, int nItem, int nSubItem, CRect&rcSubItem, CString sText, LPARAM lParam)
{
	if (   lParam == 0 
		&& (   nItem    != m_Selected.Cell.wItem 
		    || nSubItem != m_Selected.Cell.wSubItem))
	{
		// GetSysColorBrush(COLOR_INFOBK)
		CBrush br(RGB(255,255,225));
		pDC->FillRect(rcSubItem, &br);
	}
	CEdtSubItemListCtrl::DrawSubItem(pDC, nItem, nSubItem, rcSubItem, sText, lParam);
}
/////////////////////////////////////////////////////////////////////////////
int CMiscellaneousList::InsertColumn(LPCTSTR pszName, int nWidth)
{
	int nSub = GetSubItemCount();
	m_pnWidth = (int*)realloc(m_pnWidth, sizeof(int)*(nSub+1));
	m_pnWidth[nSub] = nWidth;	// nWidth in percent
	CRect rc;
	GetClientRect(&rc);
	nWidth = MulDiv(rc.Width(), nWidth, 100); // calculate real width
	return CListCtrl::InsertColumn(nSub, pszName, LVCFMT_LEFT, nWidth);
}
/////////////////////////////////////////////////////////////////////////////
int CMiscellaneousList::InsertEntry(CMiscellaneousEntry*pEntry)
{
	int nItem = GetItemCount();
	nItem = InsertItem(nItem, pEntry->m_sName);
	SetItemData(nItem, (DWORD_PTR)pEntry);
	return nItem;
}
/////////////////////////////////////////////////////////////////////////////
void CMiscellaneousList::Load(CWK_Profile &wkp)
{
	CMiscellaneousEntry*pEntry;
	int i, n = GetItemCount();

	CSubItemEdit*pEdit = new CSubItemEdit(this);
	CRect rcSubItem(0,0,0,0);
	DWORD dwStyle = WS_CHILD|ES_AUTOHSCROLL|WS_BORDER;
	pEdit->Create(dwStyle, rcSubItem, this, 0);
	CDataExchange dx(this, FALSE);
	TRY
	{
		for (i=0; i<n; i++)
		{
			pEntry = (CMiscellaneousEntry*)GetItemData(i);
			if (pEntry)
			{
				pEntry->Load(wkp);
				m_Selected.Cell.wItem = (WORD) i;
				m_Selected.Cell.wSubItem = 1;
				pEdit->SetDlgCtrlID(GetSelectionID());
				DoDataExchange(&dx);
				m_Selected.Cell.wSubItem = 2;
				pEdit->SetDlgCtrlID(GetSelectionID());
				DoDataExchange(&dx);
			}
		}
	}
	CATCH(CException, e)
	{
		_TCHAR sError[256];
		e->GetErrorMessage(sError, 256);
		WK_TRACE(_T("%s\n"), sError);
		delete e;
	}
	END_CATCH

	pEdit->SendMessage(WM_STOP_EDITING, TRUE, VK_ESCAPE);
}
/////////////////////////////////////////////////////////////////////////////
void CMiscellaneousList::Save(CWK_Profile &wkp)
{
	CMiscellaneousEntry*pEntry;
	m_saInitApps.RemoveAll();
	int i, n = GetItemCount();
	for (i=0; i<n; i++)
	{
		pEntry = (CMiscellaneousEntry*)GetItemData(i);
		if (pEntry)
		{
			if (   pEntry->Save(wkp) 
				&& pEntry->m_Wai != WAI_INVALID)
			{
				CString sApp = GetAppnameFromId(pEntry->m_Wai);
				if (FindString(m_saInitApps, sApp,  TRUE, FALSE) == -1)
				{
					m_saInitApps.Add(sApp);
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
const CStringArray& CMiscellaneousList::GetInitApplications() const
{
	return m_saInitApps;
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CMiscellaneousList, CEdtSubItemListCtrl)
	ON_NOTIFY_REFLECT(LVN_DELETEITEM, OnLvnDeleteitem)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
int CMiscellaneousList::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CEdtSubItemListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CMiscellaneousList::OnLvnDeleteitem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	CMiscellaneousEntry*pEntry = (CMiscellaneousEntry*)pNMLV->lParam;
	WK_DELETE(pEntry);
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CMiscellaneousList::OnSize(UINT nType, int cx, int cy)
{
	CEdtSubItemListCtrl::OnSize(nType, cx, cy);
	if (m_pnWidth)
	{
		int i;
		for (i=0; i<GetSubItemCount(); i++)
		{
			SetColumnWidth(i, MulDiv(cx, m_pnWidth[i], 100));
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
// CMiscellaneousPage dialog
IMPLEMENT_DYNAMIC(CMiscellaneousPage, CSVPage)
CMiscellaneousPage::CMiscellaneousPage(CSVView* pParent /*=NULL*/)
	 : CSVPage(CMiscellaneousPage::IDD)
{
	m_pParent = pParent;
	Create(IDD,pParent);
}
/////////////////////////////////////////////////////////////////////////////
CMiscellaneousPage::~CMiscellaneousPage()
{
}
/////////////////////////////////////////////////////////////////////////////
void CMiscellaneousPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMiscellaneousPage)
	DDX_Control(pDX, IDC_MISC_LIST, m_List);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CMiscellaneousPage message handlers
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CMiscellaneousPage, CSVPage)
	//{{AFX_MSG_MAP(CMiscellaneousPage)
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
CString InsertSpaces(CString&s)
{
	CString sOut;
	int i, nLen = s.GetLength();
	for (i=0; i<nLen; i++)
	{
		if (i && isupper(s.GetAt(i)))
		{
			sOut += _T(" ");
		}
		sOut += s.GetAt(i);
	}
	return sOut;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMiscellaneousPage::OnInitDialog()
{
	CSVPage::OnInitDialog();

	CString s1, s2;
	CStringArray sa;
	int i;
	s1.LoadString(IDS_NAME);
	s2.LoadString(IDS_DESCRIPTION);
	m_List.InsertColumn(s1 + _T("/") + s2, 60);

	s1.LoadString(IDS_KEY_VALUE);
	s2.LoadString(IDS_ACTIVATION_STATE);
	m_List.InsertColumn(s1 + _T("/") + s2, 20);

	s1.LoadString(IDS_DEFAULT_VALUE);
	m_List.InsertColumn(s1, 20);
	if (theApp.m_bProfessional)
	{
//		m_List.InsertColumn(nSubItem++, _T("Typ"), LVCFMT_LEFT, nWidth);
//		m_List.InsertColumn(nSubItem++, _T("Anwendung"), LVCFMT_LEFT, nWidth);
	}

	m_List.SetExtendedStyle(LVS_EX_GRIDLINES);

	s1.LoadString(IDS_CLIENT_PRESENTATION);
	m_List.InsertItem(m_List.GetItemCount(), s1);
	m_List.InsertEntry(new CMiscellaneousEntry(IDS_LOAD_HOSTMAP, IDIP_CLIENT_SETTINGS, SHOW_HOSTMAPATLOGGIN, 1, REG_DWORD, VALUE_BOOL, WAI_IDIP_CLIENT));
	m_List.InsertEntry(new CMiscellaneousEntry(IDS_LOAD_CAMERA_MAP, IDIP_CLIENT_SETTINGS, SHOW_CAMEREAMAPATCONNECTION, 1, REG_DWORD, VALUE_BOOL, WAI_IDIP_CLIENT));
	m_List.InsertEntry(new CMiscellaneousEntry(IDS_CONNECT_TO_LOCAL_HOST, IDIP_CLIENT_SETTINGS, CONNECT_TO_LOCAL_SERVER, 1, REG_DWORD, VALUE_BOOL, WAI_IDIP_CLIENT));
	m_List.InsertEntry(new CMiscellaneousEntry(IDS_CLOSE_ALARM_WINDOW, IDIP_CLIENT_SETTINGS, CLOSEALARMWINDOW, 0, REG_DWORD, VALUE_BOOL, WAI_IDIP_CLIENT));
	m_List.InsertEntry(new CMiscellaneousEntry(IDS_SHOW_ALARM_MENU, IDIP_CLIENT_SETTINGS, SHOWALARMMENU, 0, REG_DWORD, VALUE_BOOL, WAI_IDIP_CLIENT));
	m_List.InsertEntry(new CMiscellaneousEntry(IDS_CONFIRM_DISCONNECT, IDIP_CLIENT_SETTINGS, CONFIRMDISCONNECT, 0, REG_DWORD, VALUE_BOOL, WAI_IDIP_CLIENT));
	m_List.InsertEntry(new CMiscellaneousEntry(IDS_VERIFYUSER_TO_CLOSENOTE, IDIP_CLIENT_SETTINGS, VERIFYUSERTOCLOSENOTE, 0, REG_DWORD, VALUE_BOOL, WAI_IDIP_CLIENT));

	if (theApp.m_bProfessional)
	{
		m_List.InsertEntry(new CMiscellaneousEntry(_T("Show all host entries when the map is edited"), IDIP_CLIENT_SETTINGS, _T("InsertKnownHost"), 1, REG_DWORD, VALUE_BOOL, WAI_IDIP_CLIENT));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("Set local language settings according to client language"), IDIP_CLIENT_SETTINGS, _T("SetLocale"), 0, REG_DWORD, VALUE_BOOL, WAI_IDIP_CLIENT));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("Show tree views left"), IDIP_CLIENT_SETTINGS, _T("ObjectViewsLeft"), 0, REG_DWORD, VALUE_BOOL, WAI_IDIP_CLIENT));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("Show control bar on top"), IDIP_CLIENT_SETTINGS, _T("DialogBarTop"), 0, REG_DWORD, VALUE_BOOL, WAI_IDIP_CLIENT));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("Keep proportion of live pictures"), IDIP_CLIENT_SETTINGS, _T("KeepPictProportion"), 1, REG_DWORD, VALUE_BOOL, WAI_IDIP_CLIENT));

		m_List.InsertItem(m_List.GetItemCount(), _T("Client problem treatment"));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("Switch of extended windows keyboard keys"), IDIP_CLIENT_SETTINGS, _T("RegisterHotKey"), 0, REG_DWORD, VALUE_BOOL));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("Allow PTZ diagonal control"), IDIP_CLIENT_SETTINGS, _T("AllowPTZDiagonal"), 1, REG_DWORD, VALUE_BOOL, WAI_IDIP_CLIENT));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("Show Menu tool tips"), IDIP_CLIENT_SETTINGS, _T("ShowMenuToolTips"), 1, REG_DWORD, VALUE_BOOL, WAI_IDIP_CLIENT));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("Request GOPs"), IDIP_CLIENT_SETTINGS, _T("CanGOP"), 1, REG_DWORD, VALUE_BOOL, WAI_IDIP_CLIENT));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("Use multi media timer"), IDIP_CLIENT_SETTINGS, _T("UseMMTimer"), 1, REG_DWORD, VALUE_BOOL, WAI_IDIP_CLIENT));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("Warn if audio is blocked after x sent samples"), IDIP_CLIENT_SETTINGS, _T("WarnIfAudioIsBlockedAtNo"), 16, REG_DWORD, WAI_IDIP_CLIENT));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("Use AV Codec"), IDIP_CLIENT_SETTINGS, _T("UseAVCodec"), 1, REG_DWORD, VALUE_BOOL, WAI_IDIP_CLIENT));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("Decode pictures in extra thread 0x1: Live, 0x10: Database"), IDIP_CLIENT_SETTINGS, _T("RunLiveWindowThread"), 0x00000011, REG_DWORD, VALUE_HEX, WAI_IDIP_CLIENT));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("Always display MPEG4 in extra thread"), IDIP_CLIENT_SETTINGS, _T("UseGOPThreadAlways"), 1, REG_DWORD, VALUE_BOOL, WAI_IDIP_CLIENT));
	
		m_List.InsertItem(m_List.GetItemCount(), _T("Client error reports"));
		GetProfile().EnumRegEntries(IDIP_CLIENT_SETTINGS, sa);
		for (i=0; i<sa.GetCount(); i++)
		{
			s1 = sa.GetAt(i); 
			if (   _tcsncicmp(s1, _T("trace"), 5) == 0
				|| _tcsncicmp(s1, _T("stat") , 4) == 0)
			{
				s2 = InsertSpaces(s1);
				m_List.InsertEntry(new CMiscellaneousEntry(s2, IDIP_CLIENT_SETTINGS, s1, 0, REG_DWORD, VALUE_BOOL, WAI_IDIP_CLIENT));
			}
		}

		m_List.InsertItem(m_List.GetItemCount(), _T("Client perfomance / maintenance"));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("Minimum performance level"), IDIP_CLIENT_SETTINGS, _T("MinPerformanceLevel"), 1, REG_DWORD, VALUE_SIMPLE, WAI_IDIP_CLIENT));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("Lower CPU usage threshold in %"), IDIP_CLIENT_SETTINGS, _T("LowerCPUusageThreshold"), 70, REG_DWORD, VALUE_SIMPLE, WAI_IDIP_CLIENT));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("Upper CPU usage threshold in %"), IDIP_CLIENT_SETTINGS, _T("UpperCPUusageThreshold"), 85, REG_DWORD, VALUE_SIMPLE, WAI_IDIP_CLIENT));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("Regard network bit rate to adjust frame rate"), IDIP_CLIENT_SETTINGS, _T("UseNetworkFrameRate"), 0, REG_DWORD, VALUE_BOOL, WAI_IDIP_CLIENT));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("Show CPU Usage bits 0x1:Level meter, 0x10:trace change, 0x100:log avg usage, 0x10000:log avg video timeout"), IDIP_CLIENT_SETTINGS, _T("Show_CPU_Usage"), 0, REG_DWORD, VALUE_HEX));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("Show frame rate"), IDIP_CLIENT_SETTINGS, _T("ShowCocoFPS"), 0, REG_DWORD, VALUE_BOOL, WAI_IDIP_CLIENT));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("Show debug window"), IDIP_CLIENT_SETTINGS, _T("AddDebugWindow"), 0, REG_DWORD, VALUE_BOOL, WAI_IDIP_CLIENT));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("Show status bar"), IDIP_CLIENT_SETTINGS, _T("ShowStatusBar"), 0, REG_DWORD, VALUE_BOOL));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("Show title info bits 0x1:show, 0x10:in picture, 0x100:debug info, 0x1100:minimum debug info"), IDIP_CLIENT_SETTINGS, _T("ShowSmallTitles"), 1, REG_DWORD, VALUE_HEX, WAI_IDIP_CLIENT));
	}

	s1.LoadString(IDS_TOUCH_SCREEN);
	m_List.InsertItem(m_List.GetItemCount(), s1);
	m_List.InsertEntry(new CMiscellaneousEntry(IDS_DISABLE_DOUBLECLICK, IDIP_CLIENT_SETTINGS, DISABLEDBLCLICK, 0, REG_DWORD, VALUE_BOOL, WAI_IDIP_CLIENT));
	m_List.InsertEntry(new CMiscellaneousEntry(IDS_DISABLE_RIGHTCLICK, IDIP_CLIENT_SETTINGS, DISABLERIGHTCLICK, 0, REG_DWORD, VALUE_BOOL, WAI_IDIP_CLIENT));
	m_List.InsertEntry(new CMiscellaneousEntry(IDS_DISABLE_SMALL, IDIP_CLIENT_SETTINGS, DISABLESMALL, 0, REG_DWORD, VALUE_BOOL, WAI_IDIP_CLIENT));
	m_List.InsertEntry(new CMiscellaneousEntry(IDS_DISABLE_ZOOM, IDIP_CLIENT_SETTINGS, DISABLEZOOM, 0, REG_DWORD, VALUE_BOOL, WAI_IDIP_CLIENT));

	if (theApp.m_bProfessional)
	{
		m_List.InsertItem(m_List.GetItemCount(), _T("ISDN Unit (restart Unit)"));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("Unit1: Close one B-Channel at second connection"), ISDN_UNIT_SETTINGS, _T("CloseOneChannel"), 0, REG_DWORD, VALUE_BOOL, WAI_ISDN_UNIT));
		sa.RemoveAll();
		GetProfile().EnumRegEntries(ISDN_UNIT_DEBUG_SETTINGS, sa);
		for (i=0; i<sa.GetCount(); i++)
		{
			s1 = sa.GetAt(i);
			if (_tcsncicmp(s1, _T("CapiDebugTrace"), 5) == 0)
			{
				m_List.InsertEntry(new CMiscellaneousEntry(_T("Both: Capi Tracebits (1:Connection, 2:Send, 4:Receive)"), ISDN_UNIT_DEBUG_SETTINGS, s1, 0, REG_DWORD, VALUE_HEX, WAI_ISDN_UNIT));
			}
			else
			{
				s2 = InsertSpaces(s1);
				m_List.InsertEntry(new CMiscellaneousEntry(_T("Unit1: ") + s2, ISDN_UNIT_DEBUG_SETTINGS, s1, 0, REG_DWORD, VALUE_BOOL, WAI_ISDN_UNIT));
			}
		}
		m_List.InsertEntry(new CMiscellaneousEntry(_T("Unit2: Close one B-Channel at second connection"), ISDN_UNIT2_SETTINGS, _T("CloseOneChannel"), 0, REG_DWORD, VALUE_BOOL, WAI_ISDN_UNIT_2));
		sa.RemoveAll();
		GetProfile().EnumRegEntries(ISDN_UNIT2_DEBUG_SETTINGS, sa);
		for (i=0; i<sa.GetCount(); i++)
		{
			s1 = sa.GetAt(i); 
			s2 = InsertSpaces(s1);
			m_List.InsertEntry(new CMiscellaneousEntry(_T("Unit2: ") + s2, ISDN_UNIT2_DEBUG_SETTINGS, s1, 0, REG_DWORD, VALUE_BOOL, WAI_ISDN_UNIT));
		}

		m_List.InsertItem(m_List.GetItemCount(), _T("Socket Unit (restart Unit)"));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("Number of values for average bitrate calculation"), SOCKET_UNIT_STATISTICS, STAT_VALUES, 0, REG_DWORD, VALUE_SIMPLE, WAI_SOCKET_UNIT));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("Timer to refresh bitrate display [ms]"), SOCKET_UNIT_STATISTICS, STAT_TIMER, 1000, REG_DWORD, VALUE_SIMPLE, WAI_SOCKET_UNIT));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("Maximum value for bitrate in diagram [MBits]"), SOCKET_UNIT_STATISTICS, STAT_MAXIMUM, 100, REG_DWORD, VALUE_SIMPLE, WAI_SOCKET_UNIT));

		m_List.InsertItem(m_List.GetItemCount(), _T("Database Maintenance (restart DBS)"));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("Allow network storage"), SECTION_DBS, _T("AllowNetworkStorage"), 0, REG_DWORD, VALUE_BOOL, WAI_IDIP_CLIENT));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("Enable debugger for call stack"), SECTION_DBS, _T("EnableDebugger"), 0, REG_DWORD, VALUE_BOOL, WAI_IDIP_CLIENT));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("Show threads in view"), SECTION_DBS, _T("EnableThreadPool"), 0, REG_DWORD, VALUE_BOOL, WAI_IDIP_CLIENT));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("Show save statistics grafically "), SECTION_DBS, _T("CreateScopeView"), 0, REG_DWORD, VALUE_BOOL, WAI_IDIP_CLIENT));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("Max Fps range in grafic"), SECTION_DBS, _T("RangeMaxFps"), 250, REG_DWORD, VALUE_SIMPLE, WAI_IDIP_CLIENT));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("Max MB ps range in grafic"), SECTION_DBS, _T("RangeMaxMBps"), 5, REG_DWORD, VALUE_SIMPLE, WAI_IDIP_CLIENT));
		sa.RemoveAll();
		GetProfile().EnumRegEntries(SECTION_DBS, sa);
		for (i=0; i<sa.GetCount(); i++)
		{
			s1 = sa.GetAt(i); 
			if (   _tcsncicmp(s1, _T("trace"), 5) == 0)
			{
				s2 = InsertSpaces(s1);
				m_List.InsertEntry(new CMiscellaneousEntry(s2, SECTION_DBS, s1, 0, REG_DWORD, VALUE_BOOL, WAI_IDIP_CLIENT));
			}
		}

		m_List.InsertItem(m_List.GetItemCount(), _T("Server maintenance (restart server)"));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("Adjust Start Time"), SERVER_DEBUG_SETTINGS, _T("AdjustStartTime"), 0, REG_DWORD, VALUE_BOOL, WAI_SECURITY_SERVER));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("Allow any overlay client"), SERVER_DEBUG_SETTINGS, _T("AllowAnyOverlayClient"), 0, REG_DWORD, VALUE_BOOL, WAI_SECURITY_SERVER));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("Allow get registry"), SERVER_DEBUG_SETTINGS, _T("AllowGetRegistry"), 1, REG_DWORD, VALUE_BOOL, WAI_IDIP_CLIENT));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("Disable request multiplexing"), SERVER_DEBUG_SETTINGS, _T("DisableRequestMultiplexing"), 0, REG_DWORD, VALUE_BOOL, WAI_SECURITY_SERVER));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("Disable request optimization"), SERVER_DEBUG_SETTINGS, _T("DisableRequestOptimization"), 0, REG_DWORD, VALUE_BOOL, WAI_SECURITY_SERVER));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("SaVic FPS"), SERVER_DEBUG_SETTINGS, _T("SaVicFPS"), 12, REG_DWORD, VALUE_SIMPLE, WAI_SECURITY_SERVER));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("MiCo FPS"), SERVER_DEBUG_SETTINGS, _T("MiCoFPS"), 16, REG_DWORD, VALUE_SIMPLE, WAI_SECURITY_SERVER));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("CoCo FPS"), SERVER_DEBUG_SETTINGS, _T("CoCoFPS"), 12, REG_DWORD, VALUE_SIMPLE, WAI_SECURITY_SERVER));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("CoCo client pictures per Second"), SERVER_DEBUG_SETTINGS, _T("CoCoClientPicsPerSecond"), 1, REG_DWORD, VALUE_SIMPLE, WAI_SECURITY_SERVER));
		sa.RemoveAll();
		GetProfile().EnumRegEntries(SERVER_DEBUG_SETTINGS, sa);
		for (i=0; i<sa.GetCount(); i++)
		{
			s1 = sa.GetAt(i); 
			if (   _tcsncicmp(s1, _T("trace"), 5) == 0)
			{
				s2 = InsertSpaces(s1);
				m_List.InsertEntry(new CMiscellaneousEntry(s2, SERVER_DEBUG_SETTINGS, s1, 0, REG_DWORD, VALUE_BOOL, WAI_SECURITY_SERVER));
			}
		}

		m_List.InsertItem(m_List.GetItemCount(), _T("Launcher maintenance (restart software)"));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("Disable HDD write cache"), SECTION_LAUNCHER, _T("DisableHDWriteCache"), 0, REG_DWORD, VALUE_BOOL));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("Number of Harddisks for SMART und temperature surveilance"), SECTION_LAUNCHER, _T("MaxCheckedDrives"), 7, REG_DWORD, VALUE_SIMPLE));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("Maximum drive temperature"), SECTION_LAUNCHER, _T("MaxTemperatureHDD"), 55, REG_DWORD, VALUE_SIGNED));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("Execute Videte Check Disk at program start"), SECTION_LAUNCHER, _T("RunVideteCheckDisk"), 1, REG_DWORD, VALUE_BOOL));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("Show CRT error as runtime error"), SECTION_LAUNCHER, _T("ShowCrtErrorAsRte"), 0, REG_DWORD, VALUE_BOOL));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("Version"), SECTION_LAUNCHER, _T("Version"), 0, REG_SZ, VALUE_BOOL));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("Time zone"), SECTION_LAUNCHER, _T("Time"), 0, REG_SZ, VALUE_BOOL));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("Delete temp files at start"), SECTION_LAUNCHER, _T("DeleteTempFiles"), 1, REG_DWORD, VALUE_BOOL));

		m_List.InsertItem(m_List.GetItemCount(), _T("System Management"));
		m_List.InsertEntry(new CMiscellaneousEntry(_T("Professional mode"), REG_KEY_SUPERVISOR, PROFESSIONAL_MODE, 0, REG_DWORD, VALUE_BOOL));
	}

	Initialize();
	m_List.SelectSubItem(1, 1);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CMiscellaneousPage::Initialize()
{
	CWK_Profile&wkp = GetProfile();
	m_List.Load(wkp);
}
/////////////////////////////////////////////////////////////////////////////
void CMiscellaneousPage::SaveChanges()
{
//	if (m_List.ValidateAll())	// only if there is any validation
	{
		m_List.Save(GetProfile());
		int i;
		const CStringArray& sa = m_List.GetInitApplications();
		for (i=0; i<sa.GetCount(); i++)
		{
			m_pParent->AddInitApp(sa.GetAt(i));
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMiscellaneousPage::CancelChanges()
{
	Initialize();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMiscellaneousPage::StretchElements()
{
	return TRUE;
}

void CMiscellaneousPage::OnSize(UINT nType, int cx, int cy)
{
	CSVPage::OnSize(nType, cx, cy);
}
