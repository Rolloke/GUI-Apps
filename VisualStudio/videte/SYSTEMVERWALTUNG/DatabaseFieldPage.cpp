// DatabaseFieldPage.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "DatabaseFieldPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDatabaseFieldPage dialog
CDatabaseFieldPage::CDatabaseFieldPage(CSVView* pParent)
	: CSVPage(CDatabaseFieldPage::IDD)
{
	//{{AFX_DATA_INIT(CDatabaseFieldPage)
	m_bUse = FALSE;
	m_sName = _T("");
	m_dwLength = 0;
	m_sDefaultValue = _T("");
	m_iType = -1;
	m_sID = _T("");
	//}}AFX_DATA_INIT

	m_pParent = pParent;
	m_pDatabaseFields = pParent->GetDocument()->GetDatabaseFields();
	m_iSelectedItem = -1;

	m_pSDIProtocols = pParent->GetDocument()->GetSDIControls();

	Create(IDD,(CWnd*)m_pParent);
}
/////////////////////////////////////////////////////////////////////////////
void CDatabaseFieldPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDatabaseFieldPage)
	DDX_Control(pDX, IDC_EDIT_DBF_ID, m_editID);
	DDX_Control(pDX, IDC_TXT_DBF_ID, m_txtID);
	DDX_Control(pDX, IDC_TXT_DBF_TYPE, m_txtType);
	DDX_Control(pDX, IDC_TXT_DBF_NAME, m_txtName);
	DDX_Control(pDX, IDC_TXT_DBF_LENGHT, m_txtLength);
	DDX_Control(pDX, IDC_TXT_DBF_DEFAULT, m_txtDefault);
	DDX_Control(pDX, IDC_BTN_DBF_DEFAULT_VALUES, m_btnDefaultValues);
	DDX_Control(pDX, IDC_COMBO_DBF_TYPE, m_cbType);
	DDX_Control(pDX, IDC_EDIT_DBF_LENGTH, m_editLength);
	DDX_Control(pDX, IDC_EDIT_DBF_NAME, m_editName);
	DDX_Control(pDX, IDC_CHECK_DBF_USE, m_btnUse);
	DDX_Control(pDX, IDC_LIST_DBF_DATABASE_FIELDS, m_DatabaseFields);
	DDX_Check(pDX, IDC_CHECK_DBF_USE, m_bUse);
	DDX_Text(pDX, IDC_EDIT_DBF_NAME, m_sName);
	DDX_Text(pDX, IDC_EDIT_DBF_LENGTH, m_dwLength);
	DDX_Text(pDX, IDC_TXT_DBF_DEFAULT_VALUE, m_sDefaultValue);
	DDX_CBIndex(pDX, IDC_COMBO_DBF_TYPE, m_iType);
	DDX_Text(pDX, IDC_EDIT_DBF_ID, m_sID);
	DDV_MaxChars(pDX, m_sID, 8);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDatabaseFieldPage, CSVPage)
	//{{AFX_MSG_MAP(CDatabaseFieldPage)
	ON_BN_CLICKED(IDC_CHECK_DBF_USE, OnCheckUse)
	ON_BN_CLICKED(IDC_BTN_DBF_DEFAULT_VALUES, OnBtnDefaultValues)
	ON_EN_CHANGE(IDC_EDIT_DBF_NAME, OnChangeEditName)
	ON_EN_CHANGE(IDC_EDIT_DBF_LENGTH, OnChangeEditLength)
	ON_CBN_SELCHANGE(IDC_COMBO_DBF_TYPE, OnSelchangeComboType)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DBF_DATABASE_FIELDS, OnItemchangedListDatabaseFields)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_DBF_DATABASE_FIELDS, OnRclickListDatabaseFields)
	ON_NOTIFY(NM_RDBLCLK, IDC_LIST_DBF_DATABASE_FIELDS, OnRdblclkListDatabaseFields)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_DBF_DATABASE_FIELDS, OnDblclkListDatabaseFields)
	ON_NOTIFY(NM_CLICK, IDC_LIST_DBF_DATABASE_FIELDS, OnClickListDatabaseFields)
	ON_EN_CHANGE(IDC_EDIT_DBF_ID, OnChangeEditID)
	ON_BN_CLICKED(IDC_BTN_DBF_DEFAULT_VALUES_ALL, OnBnClickedBtnDbfDefaultValuesAll)
	ON_BN_CLICKED(IDC_BTN_DBF_CREATE_DEFAULT, OnBnClickedBtnDbfCreateDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
BOOL CDatabaseFieldPage::CanNew()
{
	BOOL bReturn = FALSE;
	bReturn = TRUE;
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDatabaseFieldPage::CanDelete()
{
	return m_DatabaseFields.GetItemCount() > 0 ? TRUE : FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDatabaseFieldPage::StretchElements()
{
	StretchButtonHeight(FALSE);
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CDatabaseFieldPage::EnableExceptNew()
{
	if (   m_DatabaseFields.GetItemCount() > 0
		&& m_pSelectedDatabaseField != NULL)
	{
		BOOL bIsDefaultField = IsDefaultField(m_pSelectedDatabaseField);
		BOOL bDefault = (m_sName == GetDefaultValue(m_pSelectedDatabaseField->GetName()));
		BOOL bEnableName = m_bUse;
		BOOL bEnableID   = !bIsDefaultField;
		BOOL bEnableType = !bIsDefaultField;
		BOOL bEnableLen  = !bIsDefaultField;
		BOOL bEnableUse  = bIsDefaultField;
		if (bEnableLen)
		{
			bEnableLen = (m_pSelectedDatabaseField->IsString());
		}

		if (!bIsDefaultField && m_pSelectedDatabaseField->GetFlags())
		{
			DWORD dwFlags = m_pSelectedDatabaseField->GetFlags();
			bEnableUse  = (FIELD_FLAG_CAN_DEACTIATE&dwFlags) ? TRUE  : FALSE;
			bEnableName = (FIELD_FLAG_NO_EDIT_NAME &dwFlags) ? FALSE : TRUE;
			bEnableID   = (FIELD_FLAG_NO_EDIT_VALUE&dwFlags) ? FALSE : TRUE;
			bEnableType = (FIELD_FLAG_NO_EDIT_TYPE &dwFlags) ? FALSE : TRUE;
			if (m_pSelectedDatabaseField->IsString())
			{
				bEnableLen = (FIELD_FLAG_NO_EDIT_MAXLEN&dwFlags) ? FALSE : TRUE;
			}
			else
			{
				bEnableLen = FALSE;
			}
		}

		m_DatabaseFields.EnableWindow(TRUE);
		m_btnDefaultValues.EnableWindow(bIsDefaultField && !bDefault && m_bUse && bEnableName);
		m_btnUse.EnableWindow(bEnableUse);
		m_editName.EnableWindow(bEnableName);
		m_editID.EnableWindow(bEnableID);
		m_cbType.EnableWindow(bEnableType);
		m_editLength.EnableWindow(bEnableLen);

		int iShow = (bIsDefaultField ? SW_SHOW : SW_HIDE);
		m_txtDefault.ShowWindow(iShow);
	}
	else
	{
		m_DatabaseFields.EnableWindow(FALSE);
		m_btnDefaultValues.EnableWindow(FALSE);
		m_btnUse.EnableWindow(FALSE);
		m_editName.EnableWindow(FALSE);
		m_editID.EnableWindow(FALSE);
		m_cbType.EnableWindow(FALSE);
		m_editLength.EnableWindow(FALSE);
		m_txtDefault.ShowWindow(SW_HIDE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDatabaseFieldPage::SaveChanges()
{
	if (IsDataValid())
	{
		WK_TRACE_USER(_T("Datenbankfelder-Einstellungen wurden geändert\n"));
		UpdateData();
		CWK_Profile& wkp = GetProfile();
		m_pDatabaseFields->Save(SECTION_DATABASE_FIELDS, wkp);

		CIPCField* pField = NULL;
		for (int i=0 ; i<m_pDatabaseFields->GetSize() ; i++)
		{
			pField = m_pDatabaseFields->GetAt(i);
			if (IsDefaultField(pField))
			{
				wkp.WriteString(szFieldMapSection, pField->GetName(), pField->GetValue());
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDatabaseFieldPage::CancelChanges()
{
	ClearDatabaseFieldListBox();
	m_pDatabaseFields->DeleteAll();
	m_pDatabaseFields->Load(SECTION_DATABASE_FIELDS, GetProfile());
	FillDatabaseFieldListBox();
	SelectItem((m_DatabaseFields.GetItemCount()>0) ? 0 : -1);
	EnableExceptNew();
	if (m_DatabaseFields.GetItemCount()>=0)
	{
		m_DatabaseFields.SetFocus();
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDatabaseFieldPage::IsDataValid()
{
	CIPCField *pField1, *pField2;
	CString sName1, sName2, sID1, sID2, sType, sTypeChar;
	WORD wLength = 0;
	for (int i=0 ;i<m_DatabaseFields.GetItemCount() ; i++)
	{
		pField1 = (CIPCField*)m_DatabaseFields.GetItemData(i);
		// Check for empty names
		sName1 = pField1->GetValue();
		if (sName1.IsEmpty())
		{
			AfxMessageBox(IDP_DATABASE_FIELD_NAME_EMPTY, MB_ICONSTOP);
			SelectItem(i);
			m_editName.SetFocus();
			m_editName.SetSel(0, -1);
			return FALSE;
		}
		// Check for valid ID
		sID1 = pField1->GetName();
		if (sID1.GetLength() != 8)
		{
			AfxMessageBox(IDP_DATABASE_FIELD_ID_INVALID, MB_ICONSTOP);
			SelectItem(i);
			m_editID.SetFocus();
			m_editID.SetSel(0, -1);
			return FALSE;
		}
		// Check for valid length
		wLength = pField1->GetMaxLength();
		if (wLength == 0)
		{
			AfxMessageBox(IDP_DATABASE_FIELD_LENGTH_INVALID, MB_ICONSTOP);
			SelectItem(i);
			m_editLength.SetFocus();
			m_editLength.SetSel(0, -1);
			return FALSE;
		}
		// Check for valid Type
		sTypeChar = pField1->GetType();
		m_mapTypes.Lookup(sTypeChar, sType);
		if (sType.IsEmpty())
		{
			AfxMessageBox(IDP_DATABASE_FIELD_TYPE_INVALID, MB_ICONSTOP);
			SelectItem(i);
			m_cbType.SetFocus();
			return FALSE;
		}
		// Check for doubled data
		for (int j=i+1 ; j<m_DatabaseFields.GetItemCount() ; j++)
		{
			pField2 = (CIPCField*)m_DatabaseFields.GetItemData(j);
			// Check for double database field data
			if (sName1 != szDontUse)
			{
				sName2 = pField2->GetValue();
				if (sName1 == sName2)
				{
					CString sFormat,sMsg;
					sFormat.LoadString(IDP_DATABASE_FIELD_NAME_DOUBLE);
					sMsg.Format(sFormat, sName1);
					AfxMessageBox(sMsg, MB_ICONSTOP);
					SelectItem(j);
					m_editName.SetFocus();
					m_editName.SetSel(0, -1);
					return FALSE;
				}
			}
			// Check for double database field IDs
			sID2 = pField2->GetName();
			if (sID1 == sID2)
			{
				CString sFormat,sMsg;
				sFormat.LoadString(IDP_DATABASE_FIELD_ID_DOUBLE);
				sMsg.Format(sFormat, sID1);
				AfxMessageBox(sMsg, MB_ICONSTOP);
				SelectItem(j);
				m_editID.SetFocus();
				m_editID.SetSel(0, -1);
				return FALSE;
			}
		}
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CDatabaseFieldPage::OnNew()
{
	if (IsDataValid())
	{
		CIPCField* pField = AddNew();
		if (pField)
		{
			int iIndex = InsertItem(pField);
			if (iIndex != -1)
			{
				SelectItem(iIndex);
				SetModified(TRUE, TRUE);
				m_pParent->AddInitApp(WK_APP_NAME_DB_SERVER);
				m_pParent->AddInitApp(WK_APP_NAME_IDIP_CLIENT);
				EnableExceptNew();
				m_editName.SetFocus();
				m_editName.SetSel(0,-1);
			}
		}
		else
		{}// gf todo error
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDatabaseFieldPage::OnDelete()
{
	if ( (m_iSelectedItem==-1) || (m_pSelectedDatabaseField==NULL))
	{
		return;
	}

	// m_iSelected may be changed by DeleteItem, so save the current selection
	int iNewSel = m_iSelectedItem;	// new selection
	m_pDatabaseFields->Delete(m_pSelectedDatabaseField);
	m_pSelectedDatabaseField = NULL;
	m_DatabaseFields.DeleteItem(m_iSelectedItem);
	m_iSelectedItem = -1;

	// calc new selection
	int newCount = m_DatabaseFields.GetItemCount();
	// range check
	if (iNewSel > newCount-1) 
	{	// outside ?
		iNewSel = newCount-1;
	}

	SelectItem(iNewSel);
	SetModified(TRUE, TRUE);
	m_pParent->AddInitApp(WK_APP_NAME_DB_SERVER);
	m_pParent->AddInitApp(WK_APP_NAME_IDIP_CLIENT);
	EnableExceptNew();
	m_DatabaseFields.SetFocus();
}
//////////////////////////////////////////////////////////////////////
BOOL CDatabaseFieldPage::OnInitDialog() 
{
	CSVPage::OnInitDialog();

	InitDatabaseListControl();
	FillTypeCombo();

	CancelChanges();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
//////////////////////////////////////////////////////////////////////
void CDatabaseFieldPage::InitDatabaseListControl()
{
	m_DatabaseFields.SetImageList(theApp.GetStateImage(), LVSIL_SMALL);

	CRect			rect;
	LV_COLUMN		lvcolumn;
	const iColumns = 5;
	int w[iColumns] = {30,17,17,10,26};	// CAVET add sum has to be 100
	CString s[iColumns];

	m_DatabaseFields.GetWindowRect(&rect);
	m_txtName.GetWindowText(s[0]);
	m_txtID.GetWindowText(s[1]);
	m_txtType.GetWindowText(s[2]);
	m_txtLength.GetWindowText(s[3]);
	m_txtDefault.GetWindowText(s[4]);

	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_LEFT;

	for (int i = 0; i < iColumns; i++)  // add the columns to the list control
	{
		s[i].Replace(_T(":"), _T(""));
		s[i].Replace(_T("&"), _T(""));
		lvcolumn.cx = (rect.Width() * w[i])/100;
		lvcolumn.pszText = s[i].GetBuffer(0);
		lvcolumn.iSubItem = i;
		m_DatabaseFields.InsertColumn(i, &lvcolumn);  // assumes return value is OK.
		s[i].ReleaseBuffer();
	}
	
	LONG dw = m_DatabaseFields.GetExtendedStyle();
	dw |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	m_DatabaseFields.SetExtendedStyle(dw);
//	m_DatabaseFields.ModifyStyle(LVS_SINGLESEL,0);
}
//////////////////////////////////////////////////////////////////////
void CDatabaseFieldPage::FillTypeCombo()
{
	m_cbType.ResetContent();
	CString sText;
	CString sType;
	TCHAR chType;
	int iIndex = -1;

	sText.LoadString(IDS_STRING);
	chType = _T('C');
	sType = _T('C');
	iIndex = m_cbType.AddString(sText);
	m_cbType.SetItemData(iIndex, (DWORD)sType.GetAt(0));
	m_mapTypes.SetAt(&chType, sText);
	m_mapTypes.SetAt(sType, sText);

	sText.LoadString(IDS_DATE);
	sType = _T('D');
	iIndex = m_cbType.AddString(sText);
	m_cbType.SetItemData(iIndex, (DWORD)sType.GetAt(0));
	m_mapTypes.SetAt(sType, sText);

	sText.LoadString(IDS_TIME);
	sType = _T('T');
	iIndex = m_cbType.AddString(sText);
	m_cbType.SetItemData(iIndex, (DWORD)sType.GetAt(0));
	m_mapTypes.SetAt(sType, sText);
}

//////////////////////////////////////////////////////////////////////
CIPCField* CDatabaseFieldPage::AddNew()
{
	CIPCField* pField = new CIPCField();
	pField->SetType(_T('C'));
	BOOL bOK = FALSE;
	DWORD dwMaxEntry = 0;
	DWORD dwFound = 0;
	CIPCField* pFieldFound = NULL;
	CString sID;

	// first try to find the maximum and add behind
	for (int i=0 ; i<m_pDatabaseFields->GetSize() ; i++)
	{
		pFieldFound = m_pDatabaseFields->GetAt(i);
		sID = pFieldFound->GetName();
		if (_stscanf(sID, _T("DBD_%u"), &dwFound) == 1)
		{
			dwMaxEntry = max(dwMaxEntry, dwFound);
		}
	}
	if (dwMaxEntry < MAXWORD)
	{
		CString sName;
		sName.Format(_T("DBD_%04u"), dwMaxEntry+1);
		pField->SetName(sName);
		m_pDatabaseFields->Add(pField);
		bOK = TRUE;
	}

	// maximum already reached?
	if (bOK == FALSE)
	{
		// second try to find the first free one
		BOOL bFreeOne = TRUE;
		for (DWORD dw=1 ; !bOK && dw<=MAXWORD ; dw++)
		{
			for (int i=0 ; bFreeOne && i<m_pDatabaseFields->GetSize() ; i++)
			{
				pField = m_pDatabaseFields->GetAt(i);
				if (_stscanf(pField->GetName(), _T("DBD_%u"), &dwFound) == 1)
				{
					if (dw == dwFound)
					{
						bFreeOne = FALSE;
					}
				}
			}
			if (bFreeOne)
			{
				CString sName;
				sName.Format(_T("DBD_%04u"), dw);
				pField->SetName(sName);
				m_pDatabaseFields->Add(pField);
				bOK = TRUE;
			}
		}
	}
	if (bOK == FALSE)
	{
		WK_DELETE(pField);
	}
	return pField;
}
//////////////////////////////////////////////////////////////////////
void CDatabaseFieldPage::FillDatabaseFieldListBox()
{
	for (int i=0 ; i<m_pDatabaseFields->GetSize() ; i++)
	{
		InsertItem(m_pDatabaseFields->GetAt(i));
	}
}
//////////////////////////////////////////////////////////////////////
void CDatabaseFieldPage::ClearDatabaseFieldListBox()
{
	m_DatabaseFields.DeleteAllItems();
	m_bUse = FALSE;
	m_sName.Empty();
	m_sID.Empty();
	m_dwLength = 0;
	m_iType = -1;
}
//////////////////////////////////////////////////////////////////////
int  CDatabaseFieldPage::InsertItem(CIPCField* pDatabaseField)
{
	LV_ITEM lvis;
	CString sName, sID, sLength, sType, sDefaultValue, sTypeChar;

	sName = pDatabaseField->GetValue();
	sID = pDatabaseField->GetName();
	sLength.Format(_T("%u"), pDatabaseField->GetMaxLength());
	sTypeChar = pDatabaseField->GetType();
	m_mapTypes.Lookup(sTypeChar, sType);
	sDefaultValue = GetDefaultValue(sID);
	int iImage = (sName == szDontUse)? 0:1;
	if (IsDefaultField(pDatabaseField))
	{
		if (iImage == 0)
		{
			sName = sDefaultValue;
		}
	}

	int iIndex = m_DatabaseFields.GetItemCount();

	lvis.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
	lvis.pszText = (LPTSTR)LPCTSTR(sName);
	lvis.cchTextMax = sName.GetLength();
	lvis.lParam = (LPARAM)pDatabaseField;
	lvis.iItem = iIndex;
	lvis.iSubItem = 0;
	lvis.iImage = iImage;

	iIndex = m_DatabaseFields.InsertItem(&lvis);
	lvis.iItem = iIndex;

	m_DatabaseFields.SetItemText(iIndex,1,sID);
	m_DatabaseFields.SetItemText(iIndex,2,sType);
	m_DatabaseFields.SetItemText(iIndex,3,sLength);
	m_DatabaseFields.SetItemText(iIndex,4,sDefaultValue);

	return iIndex;
}
//////////////////////////////////////////////////////////////////////
void CDatabaseFieldPage::SelectItem(int iIndex)
{
/*	// We have to consider multiple selection, so deselect all first
	for (int i=0 ; i<m_Archivs.GetItemCount() ; i++)
	{
		m_Archivs.SetItemState(i,0,LVIS_SELECTED|LVIS_FOCUSED);
	}
*/	
	if (iIndex >= 0)
	{
		m_iSelectedItem = iIndex;
		m_DatabaseFields.SetItemState(m_iSelectedItem,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
		m_pSelectedDatabaseField = (CIPCField*)m_DatabaseFields.GetItemData(m_iSelectedItem);
		SelectedItemToControl();
	}
	else
	{
		m_iSelectedItem = -1;
		m_pSelectedDatabaseField = NULL;
		m_sName.Empty();
		m_sID.Empty();
		m_dwLength = 0;
		m_iType = -1;
		m_sDefaultValue.Empty();
		UpdateData(FALSE);
	}
}
//////////////////////////////////////////////////////////////////////
void CDatabaseFieldPage::SelectedItemToControl()
{
	if (m_pSelectedDatabaseField)
	{
		m_sName = m_pSelectedDatabaseField->GetValue();
		m_bUse = (m_sName != szDontUse);
		m_sID = m_pSelectedDatabaseField->GetName();
/*		CString sID = m_sID;
		if (sID.GetLength() > 4)
		{
			m_sID = sID.Mid(4,4);
		}
		else
		{
			m_sID = _T("");
		}
*/		m_dwLength = m_pSelectedDatabaseField->GetMaxLength();
		CString sType, sTypeChar;
		sTypeChar = m_pSelectedDatabaseField->GetType();
		m_mapTypes.Lookup(sTypeChar, sType);
		m_iType = m_cbType.SelectString(-1, sType);
		m_sDefaultValue = GetDefaultValue(m_sID);
		if (!m_bUse)
		{
			m_sName = m_sDefaultValue;
		}
		UpdateData(FALSE);
		EnableExceptNew();
	}
}
//////////////////////////////////////////////////////////////////////
void CDatabaseFieldPage::OnChange()
{
	EnableExceptNew();
	SetModified(TRUE,TRUE);
	m_pParent->AddInitApp(WK_APP_NAME_DB_SERVER);
	m_pParent->AddInitApp(WK_APP_NAME_IDIP_CLIENT);
}
//////////////////////////////////////////////////////////////////////
BOOL CDatabaseFieldPage::IsDefaultField(CIPCField* pField)
{
	BOOL bReturn = FALSE;
	if (pField)
	{
		if (   (pField->GetName() == CIPCField::m_sfDate)
			|| (pField->GetName() == CIPCField::m_sfTime)
			|| (pField->GetName() == CIPCField::m_sfGaNr)
			|| (pField->GetName() == CIPCField::m_sfTaNr)
			|| (pField->GetName() == CIPCField::m_sfKtNr)
			|| (pField->GetName() == CIPCField::m_sfBcNr)
			|| (pField->GetName() == CIPCField::m_sfAmnt)
			|| (pField->GetName() == CIPCField::m_sfCurr)
			)
		{
			bReturn = TRUE;
		}
	}
	return bReturn;
}
//////////////////////////////////////////////////////////////////////
typedef BOOL (*SDICONTROLFUNCTION)(SDIControlType);
//////////////////////////////////////////////////////////////////////
BOOL HasProperty(SDICONTROLFUNCTION pSDIControlFunction, CDWordArray* pArray)
{
	BOOL bFound = FALSE;
	for (int i=0;i<pArray->GetSize() && !bFound;i++)
	{
		bFound = pSDIControlFunction((SDIControlType)pArray->GetAt(i));
	}
	return bFound;
}
//////////////////////////////////////////////////////////////////////
CString CDatabaseFieldPage::GetDefaultValue(CString sName)
{
	CString sDefault;

	if   (sName == CIPCField::m_sfDate)
	{
		sDefault.LoadString(IDS_DTP_DATE);
	}
	else if (sName == CIPCField::m_sfTime)
	{
		sDefault.LoadString(IDS_DTP_TIME);
	}
	else if (sName == CIPCField::m_sfGaNr)
	{
		sDefault.LoadString(IDS_GA_NR);
	}
	else if (sName == CIPCField::m_sfTaNr)
	{ 
		sDefault.LoadString(IDS_TA_NR);
	}
	else if (sName == CIPCField::m_sfKtNr)
	{
		sDefault.LoadString(IDS_KTO_NR);
	}
	else if (sName == CIPCField::m_sfBcNr)
	{
		sDefault.LoadString(IDS_BANKLEITZAHL);
	}
	else if (sName == CIPCField::m_sfAmnt)
	{
		sDefault.LoadString(IDS_VALUE);
	}
	else if (sName == CIPCField::m_sfCurr)
	{
		sDefault.LoadString(IDS_CURRENCY);
	}
	return sDefault;
}
//////////////////////////////////////////////////////////////////////
void CDatabaseFieldPage::OnCheckUse() 
{
	if (m_pSelectedDatabaseField == NULL)
	{
		return;
	}
	CDataExchange dx(this, TRUE);
	DDX_Check(&dx, IDC_CHECK_DBF_USE, m_bUse);
	CString sNewValue;
	DWORD dwFlags = m_pSelectedDatabaseField->GetFlags();
	if (m_bUse)
	{
		sNewValue = GetDefaultValue(m_pSelectedDatabaseField->GetName());
		if (dwFlags & FIELD_FLAG_CAN_DEACTIATE)
		{
			const CString& sValue = m_pSelectedDatabaseField->GetValue();
			((CString*)&sValue)->Replace(0, _T('\\'));
			int nFind = sValue.Find(_T("\\"));
			if (nFind != -1)
			{
				sNewValue = sValue.Mid(nFind+1);
			}
			dwFlags &= ~FIELD_FLAG_UNUSED;
		}
	}
	else
	{
		sNewValue = szDontUse;
		if (dwFlags & FIELD_FLAG_CAN_DEACTIATE)
		{
			dwFlags |= FIELD_FLAG_UNUSED;
			sNewValue += _T("\\") + m_pSelectedDatabaseField->GetValue();
		}
	}
	m_pSelectedDatabaseField->SetValue(sNewValue);
	m_pSelectedDatabaseField->SetFlags(dwFlags);

	LV_ITEM lvis;
	lvis.mask = LVIF_IMAGE;
	lvis.iItem = m_iSelectedItem;
	lvis.iSubItem = 0;
	lvis.iImage = (m_bUse ? 1 : 0);
	m_DatabaseFields.SetItem(&lvis);

	dx.m_bSaveAndValidate  = FALSE;
	DDX_Check(&dx, IDC_CHECK_DBF_USE, m_bUse);
	OnChange();
}
//////////////////////////////////////////////////////////////////////
void CDatabaseFieldPage::OnBtnDefaultValues() 
{
	if (m_pSelectedDatabaseField)
	{
		UpdateData();
		m_sName = GetDefaultValue(m_pSelectedDatabaseField->GetName());
		m_pSelectedDatabaseField->SetValue(m_sName);
		m_DatabaseFields.SetItemText(m_iSelectedItem, 0, m_sName);
		UpdateData(FALSE);
		OnChange();
	}
}
//////////////////////////////////////////////////////////////////////
void CDatabaseFieldPage::OnBnClickedBtnDbfDefaultValuesAll()
{
	int i, n = m_DatabaseFields.GetItemCount();
	for (i=0; i<n; i++)
	{
		CIPCField*	pDBField = (CIPCField*)m_DatabaseFields.GetItemData(i);
		if (pDBField)
		{
			CString sDefault;
			sDefault = GetDefaultValue(pDBField->GetName());
			if (!sDefault.IsEmpty())
			{
				if (pDBField->GetValue() != szDontUse)
				{
					pDBField->SetValue(sDefault);
				}
				m_DatabaseFields.SetItemText(i, 0, sDefault);
				if (m_pSelectedDatabaseField == pDBField)
				{
					m_sName = sDefault;
					UpdateData(FALSE);
				}
			}
		}
	}
	OnChange();
}
//////////////////////////////////////////////////////////////////////
void CDatabaseFieldPage::OnBnClickedBtnDbfCreateDefault()
{
	LPCTSTR pszName[] = 
	{
		CIPCField::m_sfDate,
		CIPCField::m_sfTime,
		CIPCField::m_sfGaNr,
		CIPCField::m_sfTaNr,
		CIPCField::m_sfKtNr,
		CIPCField::m_sfBcNr,
		CIPCField::m_sfAmnt,
		CIPCField::m_sfCurr,
		NULL
	};
	UINT nValueID[] = 
	{
		IDS_DTP_DATE,
		IDS_DTP_TIME,
		IDS_GA_NR,
		IDS_TA_NR,
		IDS_KTO_NR,
		IDS_BANKLEITZAHL,
		IDS_VALUE,
		IDS_CURRENCY
	};
	WORD wMaxLen[] =
	{
		8,
		6,
		6,
		4,
		16,
		16,
		8,
		3
	};
	char cType[] =
	{
		'D',
		'T',
		'C',
		'C',
		'C',
		'C',
		'C',
		'C'
	};

	CString sDefault;
	int i;
	BOOL bChanged = FALSE;
	for (i=0; pszName[i]!=NULL; i++)
	{
		if (m_pDatabaseFields->GetCIPCField(pszName[i]) == NULL)
		{
			sDefault.LoadString(nValueID[i]);
			CIPCField*pField = new CIPCField(pszName[i], sDefault, wMaxLen[i], cType[i]);
			m_pDatabaseFields->Add(pField);
			InsertItem(pField);
			bChanged = TRUE;
		}
	}
	if (bChanged)
	{
		OnChange();
	}
	SelectItem(0);
}
//////////////////////////////////////////////////////////////////////
void CDatabaseFieldPage::OnChangeEditName() 
{
	if (m_pSelectedDatabaseField)
	{
		UpdateData();
		m_pSelectedDatabaseField->SetValue(m_sName);
		m_DatabaseFields.SetItemText(m_iSelectedItem, 0, m_sName);
		OnChange();
	}
}
//////////////////////////////////////////////////////////////////////
void CDatabaseFieldPage::OnChangeEditID() 
{
	// Save previous value before update
	CString sIDPrev = m_sID;
	UpdateData();
	int iStart, iCaret;
	m_editID.GetSel(iStart, iCaret);
	m_sID.MakeUpper();
	CString sAllowed(_T("1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ"));
	BOOL bOK = FALSE;
	if (m_sID.GetLength() > 4)
	{
		if (m_sID.Left(4) == _T("DBD_"))
		{
			CString sID = m_sID.Mid(4);
			if (StringContainsOnly(sID, sAllowed))
			{
				bOK = TRUE;
			}
			else
			{
				sID = StringOnlyCharsInSet(sID, sAllowed);
				m_sID = _T("DBD_") + sID;
				UpdateData(FALSE);
				iCaret = min(iStart, m_sID.GetLength());
				m_editID.SetSel(iCaret, iCaret);
				MessageBeep((UINT)(-1));
			}
		}
		else
		{
			m_sID = sIDPrev;
			UpdateData(FALSE);
			iCaret = sIDPrev.GetLength();
			m_editID.SetSel(iCaret, iCaret);
			MessageBeep((UINT)(-1));
		}
	}
	else if (m_sID.GetLength() < 4)
	{
		m_sID = _T("DBD_");
		UpdateData(FALSE);
		m_editID.SetSel(4, -1);
		MessageBeep((UINT)(-1));
	}

	if (bOK && m_pSelectedDatabaseField)
	{
		m_pSelectedDatabaseField->SetName(m_sID);
		m_DatabaseFields.SetItemText(m_iSelectedItem, 1, m_sID);
		UpdateData(FALSE);
		m_editID.SetSel(iCaret, iCaret);
	}
	OnChange();
}
//////////////////////////////////////////////////////////////////////
void CDatabaseFieldPage::OnChangeEditLength() 
{
	if (m_pSelectedDatabaseField)
	{
		UpdateData();
		m_pSelectedDatabaseField->SetMaxLength((WORD)m_dwLength);
		CString sLength;
		sLength.Format(_T("%u"), m_dwLength);
		m_DatabaseFields.SetItemText(m_iSelectedItem, 3, sLength);
		OnChange();
	}
}
//////////////////////////////////////////////////////////////////////
void CDatabaseFieldPage::OnSelchangeComboType() 
{
	if (m_pSelectedDatabaseField)
	{
		UpdateData();
		CString sType;
		CWK_String sTypeChar = (TCHAR)m_cbType.GetItemData(m_iType);
		LPCSTR szTypeChar = (LPCSTR)sTypeChar;
		m_pSelectedDatabaseField->SetType(szTypeChar[0]);
		m_mapTypes.Lookup(sTypeChar, sType);
		m_DatabaseFields.SetItemText(m_iSelectedItem, 2, sType);
		// Date and Time have definite length, otherwise 
		CString sLength;
		if (m_pSelectedDatabaseField->IsDate())
		{
			m_dwLength = 8;
			UpdateData(FALSE);
			m_pSelectedDatabaseField->SetMaxLength((WORD)m_dwLength);
			sLength.Format(_T("%u"), m_dwLength);
			m_DatabaseFields.SetItemText(m_iSelectedItem, 3, sLength);
		}
		else if (m_pSelectedDatabaseField->IsTime())
		{
			m_dwLength = 6;
			UpdateData(FALSE);
			m_pSelectedDatabaseField->SetMaxLength((WORD)m_dwLength);
			sLength.Format(_T("%u"), m_dwLength);
			m_DatabaseFields.SetItemText(m_iSelectedItem, 3, sLength);
		}
		else
		{ // nothing to do, field will be enabled in EnableExceptNew
		}
		OnChange();
	}
}
//////////////////////////////////////////////////////////////////////
void CDatabaseFieldPage::OnItemchangedListDatabaseFields(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (pNMListView->iItem != m_iSelectedItem)
	{
		if (pNMListView->uNewState & LVIS_SELECTED)
		{
			m_pSelectedDatabaseField = (CIPCField*)pNMListView->lParam;
			m_iSelectedItem = pNMListView->iItem;
			SelectedItemToControl();
			UpdateData(FALSE);
			EnableExceptNew();
		}
	}

	*pResult = 0;
}
//////////////////////////////////////////////////////////////////////
void CDatabaseFieldPage::OnClickListDatabaseFields(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem!=m_iSelectedItem)
	{
		// Kann sein, dass ins leere Feld geklickt wurde
		// daher Select-Markierung wiederherstellen
		m_DatabaseFields.SetItemState(m_iSelectedItem,
									LVIS_SELECTED|LVIS_FOCUSED,
									LVIS_SELECTED|LVIS_FOCUSED);
	}	
	else
	{
		UINT nFlags;
		int nItem = m_DatabaseFields.HitTest(pNMListView->ptAction, &nFlags);
		if (   nItem == m_iSelectedItem 
			&& m_pSelectedDatabaseField 
			&& IsDefaultField(m_pSelectedDatabaseField))
		{
			if (nFlags == LVHT_ONITEMICON)
			{
				m_bUse = !m_bUse;
				CDataExchange dx(this, FALSE);
				DDX_Check(&dx, IDC_CHECK_DBF_USE, m_bUse);
				OnCheckUse();
			}
		}
	}
	*pResult = 0;
}
//////////////////////////////////////////////////////////////////////
void CDatabaseFieldPage::OnDblclkListDatabaseFields(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem!=m_iSelectedItem)
	{
		// Kann sein, dass ins leere Feld geklickt wurde
		// daher Select-Markierung wiederherstellen
		m_DatabaseFields.SetItemState(m_iSelectedItem,
									LVIS_SELECTED|LVIS_FOCUSED,
									LVIS_SELECTED|LVIS_FOCUSED);
	}	
	*pResult = 0;
}
//////////////////////////////////////////////////////////////////////
void CDatabaseFieldPage::OnRclickListDatabaseFields(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem!=m_iSelectedItem)
	{
		// Kann sein, dass ins leere Feld geklickt wurde
		// daher Select-Markierung wiederherstellen
		m_DatabaseFields.SetItemState(m_iSelectedItem,
									LVIS_SELECTED|LVIS_FOCUSED,
									LVIS_SELECTED|LVIS_FOCUSED);
	}	
	*pResult = 0;
}
//////////////////////////////////////////////////////////////////////
void CDatabaseFieldPage::OnRdblclkListDatabaseFields(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem!=m_iSelectedItem)
	{
		// Kann sein, dass ins leere Feld geklickt wurde
		// daher Select-Markierung wiederherstellen
		m_DatabaseFields.SetItemState(m_iSelectedItem,
									LVIS_SELECTED|LVIS_FOCUSED,
									LVIS_SELECTED|LVIS_FOCUSED);
	}	
	*pResult = 0;
}
