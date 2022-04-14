// SdiAlarmNumberDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SystemVerwaltung.h"
#include "SdiAlarmNumberDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CAlarmListDesignaPM100: implement DoDataExchange for data validation
class CAlarmListDesignaPM100_Abacus : public CEdtSubItemListCtrl
{
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL CanEditSubItem(int, int nSubItem);
	virtual eEditType GetEditType(int nItem, int nSubItem);
	virtual void InitComboBox(CSubItemCombo*pCombo, const CString& sText);
	virtual DWORD GetItemStyle(int nItem, int nSubItem, DWORD dwStyle);
};
/////////////////////////////////////////////////////////////////////////////
// CAlarmListDesignaPM100XML: implement DoDataExchange for data validation
class CAlarmListDesignaPM100XML : public CEdtSubItemListCtrl
{
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL CanEditSubItem(int, int nSubItem);
	virtual eEditType GetEditType(int nItem, int nSubItem);
	virtual void InitComboBox(CSubItemCombo*pCombo, const CString& sText);
	virtual DWORD GetItemStyle(int nItem, int nSubItem, DWORD dwStyle);
};
/////////////////////////////////////////////////////////////////////////////
void CAlarmListDesignaPM100_Abacus::DoDataExchange(CDataExchange* pDX)
{
	int nValue;
	switch(m_Selected.Cell.wSubItem)
	{
		case 1:	// BFR
		case 2:	// TCC
			DDX_Text(pDX, _T("%02x"), AFX_IDP_PARSE_REAL, GetSelectionID(), nValue);
			DDV_MinMaxInt(pDX, nValue, 0x01, 0xff);
			break;
		case 3:	// Alarmnummer
			//DDX_Text(pDX, _T("%04x"), AFX_IDP_PARSE_REAL, GetSelectionID(), nValue);
			break;
	}
	// RKE: call base class function at the end
	CEdtSubItemListCtrl::DoDataExchange(pDX);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CAlarmListDesignaPM100_Abacus::CanEditSubItem(int, int nSubItem)
{
	return (nSubItem != 0) ? TRUE : FALSE;
}
/////////////////////////////////////////////////////////////////////////////
eEditType CAlarmListDesignaPM100_Abacus::GetEditType(int nItem, int nSubItem)
{
	return (nSubItem == 3) ? COMBO_BOX : EDIT_FIELD;
}
/////////////////////////////////////////////////////////////////////////////
DWORD CAlarmListDesignaPM100_Abacus::GetItemStyle(int nItem, int nSubItem, DWORD dwStyle)
{
	if (nSubItem == 3)
	{
		dwStyle |= CBS_SORT;
	}
	return dwStyle;
}
/////////////////////////////////////////////////////////////////////////////
void CAlarmListDesignaPM100_Abacus::InitComboBox(CSubItemCombo*pCombo, const CString& sText)
{
	CString s,a, sSelected;
	DWORD dwSelected = 0, dwAlarm;
	int i;

	i = sText.Find(_T(", "));
	if (i != 0)
	{
		if (_stscanf(sText.Mid(i+2), _T("%d"), &dwSelected) != 1)
		{
			dwSelected = 0;
		}
	}

	for (dwAlarm=1, i=0; dwAlarm<90; dwAlarm++)
	{
		a = CSDIControl::GetPM100AlarmDescription(dwAlarm);
		if (!a.IsEmpty())
		{
			s.Format(_T("%s, %d"),a,dwAlarm);
			i = pCombo->AddString(s);
			pCombo->SetItemData(i,dwAlarm);
			if (dwAlarm == dwSelected)
			{
				sSelected = s;
			}
		}
	}

	pCombo->SetDroppedHeight(100);
	i = pCombo->FindString(0, sSelected);
	if (i != CB_ERR)
	{
		pCombo->SetCurSel(i);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CAlarmListDesignaPM100XML::DoDataExchange(CDataExchange* pDX)
{
	int nValue;
	switch(m_Selected.Cell.wSubItem)
	{
	case 1:	// BFR
	case 2:	// TCC
		DDX_Text(pDX, _T("%02x"), AFX_IDP_PARSE_REAL, GetSelectionID(), nValue);
		DDV_MinMaxInt(pDX, nValue, 0x01, 0xff);
		break;
	case 3:	// Alarmnummer
		//DDX_Text(pDX, _T("%04x"), AFX_IDP_PARSE_REAL, GetSelectionID(), nValue);
		break;
	}
	// RKE: call base class function at the end
	CEdtSubItemListCtrl::DoDataExchange(pDX);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CAlarmListDesignaPM100XML::CanEditSubItem(int, int nSubItem)
{
	return (nSubItem != 0) ? TRUE : FALSE;
}
/////////////////////////////////////////////////////////////////////////////
eEditType CAlarmListDesignaPM100XML::GetEditType(int nItem, int nSubItem)
{
	return (nSubItem == 3) ? COMBO_BOX : EDIT_FIELD;
}
/////////////////////////////////////////////////////////////////////////////
DWORD CAlarmListDesignaPM100XML::GetItemStyle(int nItem, int nSubItem, DWORD dwStyle)
{
	if (nSubItem == 3)
	{
		dwStyle |= CBS_SORT;
	}
	return dwStyle;
}
/////////////////////////////////////////////////////////////////////////////
void CAlarmListDesignaPM100XML::InitComboBox(CSubItemCombo*pCombo, const CString& sText)
{
	int i;
	CString s,a, sSelected;
	DWORD dwSelected = 0, dwAlarm;
	i = sText.Find(_T(", "));
	if (i != 0)
	{
		if (_stscanf(sText.Mid(i+2), _T("%d"), &dwSelected) != 1)
		{
			dwSelected = 0;
		}
	}

	for (dwAlarm=1, i=0; dwAlarm<200; dwAlarm++)
	{
		a = CSDIControl::GetPM100XMLDescription(dwAlarm);
		if (!a.IsEmpty())
		{
			s.Format(_T("%s, %d"),a,dwAlarm);
			i = pCombo->AddString(s);
			pCombo->SetItemData(i,dwAlarm);
			if (dwAlarm == dwSelected)
			{
				sSelected = s;
			}
		}
	}

	pCombo->SetDroppedHeight(100);
	i = pCombo->FindString(0, sSelected);
	if (i != CB_ERR)
	{
		pCombo->SetCurSel(i);
	}
}


/////////////////////////////////////////////////////////////////////////////
// CAlarmListSchneider: implement DoDataExchange for data validation
class CAlarmListSchneider: public CEdtSubItemListCtrl
{
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL CanEditSubItem(int, int nSubItem);
};
/////////////////////////////////////////////////////////////////////////////
void CAlarmListSchneider::DoDataExchange(CDataExchange* pDX)
{
	int nValue;
	switch(m_Selected.Cell.wSubItem)
	{
		case 1:	// 
			DDX_Text(pDX, GetSelectionID(), nValue);
			break;
	}
	// RKE: call base class function at the end
	CEdtSubItemListCtrl::DoDataExchange(pDX);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CAlarmListSchneider::CanEditSubItem(int, int nSubItem)
{
	return (nSubItem != 0) ? TRUE : FALSE;
}
/////////////////////////////////////////////////////////////////////////////
// CSDIAlarmNumberDialog dialog
IMPLEMENT_DYNAMIC(CSDIAlarmNumberDialog, CWK_Dialog)

/////////////////////////////////////////////////////////////////////////////
CSDIAlarmNumberDialog::CSDIAlarmNumberDialog(SDIControlType eType, CSDIAlarmNumbers* pAlarmNumbers, CInputGroup* pInputGroup)
	: CWK_Dialog(CSDIAlarmNumberDialog::IDD)
{
	m_pList = NULL;
	m_eType = eType;
	m_pAlarmNumbers = pAlarmNumbers;
	m_pInputGroup = pInputGroup;
}
/////////////////////////////////////////////////////////////////////////////
CSDIAlarmNumberDialog::~CSDIAlarmNumberDialog()
{
	WK_DELETE(m_pList);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIAlarmNumberDialog::DoDataExchange(CDataExchange* pDX)
{
	CWK_Dialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SDI_LIST_ALARM_NO, *m_pList);
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CSDIAlarmNumberDialog, CWK_Dialog)
	ON_BN_CLICKED(IDC_SDI_BTN_NEW, OnBnClickedSdiBtnNew)
	ON_BN_CLICKED(IDC_SDI_BTN_DELETE, OnBnClickedSdiBtnDelete)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_NOTIFY(LVN_ITEMACTIVATE, IDC_SDI_LIST_ALARM_NO, OnLvnItemActivateSdiListAlarmNo)
	ON_BN_CLICKED(IDC_SDI_BTN_SAVE, OnBnClickedSdiBtnSave)
	ON_BN_CLICKED(IDC_SDI_BTN_LOAD, OnBnClickedSdiBtnLoad)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CSDIAlarmNumberDialog message handlers
BOOL CSDIAlarmNumberDialog::OnInitDialog()
{
	CRect rc;
	int nWidth;

	switch (m_eType) 
	{
	case SDICT_DESIGNA_PM_100_ALARM:
	case SDICT_DESIGNA_PM_ABACUS:
		m_pList = new CAlarmListDesignaPM100_Abacus;
		break;
	case SDICT_DESIGNA_PM_100_XML:
		m_pList = new CAlarmListDesignaPM100XML;
		break;
	case SDICT_SCHNEIDER_INTERCOM:
		m_pList = new CAlarmListSchneider;
		break;
	default:
		ASSERT(0);
		break;
	}


	CWK_Dialog::OnInitDialog();

	m_pList->GetClientRect(&rc);
	nWidth = rc.Width();
	m_pList->SetExtendedStyle(LVS_EX_GRIDLINES);

	switch (m_eType) 
	{
	case SDICT_DESIGNA_PM_100_ALARM:
	case SDICT_DESIGNA_PM_ABACUS:
		{
			CString sColumn;
			int i, nWidths[4] = {10, 10, 10, 70};
			UINT nIDs[4] = {IDS_NUMBER, IDS_DESIGNA_BFR, IDS_DESIGNA_TCC, IDS_ALARM_NR};
			for (i=0; i<4; i++)
			{
				if (!sColumn.LoadString(nIDs[i]))
				{
					sColumn.Format(_T("%d"), i+1);
				}
				m_pList->InsertColumn(i, sColumn, LVCFMT_LEFT, MulDiv(nWidths[i], nWidth, 100));
			}
		}
		break;
	case SDICT_DESIGNA_PM_100_XML:
		{
			CString sColumn;
			int i, nWidths[4] = {10, 10, 10, 70};
			UINT nIDs[4] = {IDS_NUMBER, IDS_DESIGNA_BFR, IDS_DESIGNA_TCC, IDS_ALARM_NR};
			for (i=0; i<4; i++)
			{
				if (!sColumn.LoadString(nIDs[i]))
				{
					sColumn.Format(_T("%d"), i+1);
				}
				m_pList->InsertColumn(i, sColumn, LVCFMT_LEFT, MulDiv(nWidths[i], nWidth, 100));
			}
		}
		break;
	case SDICT_SCHNEIDER_INTERCOM:
		{
			CString sColumn;
			int i, nWidths[3] = {20, 30,50};
			UINT nIDs[3]	  = {IDS_NUMBER, IDS_SUBSCRIBER, IDS_COMMENT};
			for (i=0; i<3; i++)
			{
				if (!sColumn.LoadString(nIDs[i]))
				{
					sColumn.Format(_T("%d"), i+1);
				}
				m_pList->InsertColumn(i, sColumn, LVCFMT_LEFT, MulDiv(nWidths[i], nWidth, 100));
			}
		}
		break;
	default:
		ASSERT(0);
		break;
	}

	InitList();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CSDIAlarmNumberDialog::OnBnClickedSdiBtnNew()
{
	CString sItem;
	int i, nItems = m_pList->GetItemCount();
	LVFINDINFO lvfi = {LVFI_STRING, NULL, NULL, {0, 0}, 0};
	for (i=1; i<=nItems+1; i++)
	{
		sItem.Format(_T("%d"), i);
		lvfi.psz = sItem;
		if (m_pList->FindItem(&lvfi) == -1)
		{
			nItems = m_pList->InsertItem(i-1, sItem);
			break;
		}
	}
	m_pList->SelectSubItem(nItems, 1);
	m_pList->EditSubItem();
}
/////////////////////////////////////////////////////////////////////////////
void CSDIAlarmNumberDialog::OnBnClickedSdiBtnDelete()
{
	int nItem, nSubItem;
	if (m_pList->GetSelection(nItem, nSubItem))
	{
		m_pList->DeleteItem(nItem);
		if (m_pList->GetItemCount())
		{
			m_pList->SelectSubItem(0, 1);
		}
		else
		{
			GetDlgItem(IDC_SDI_BTN_DELETE)->EnableWindow(FALSE);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIAlarmNumberDialog::OnBnClickedOk()
{
	if (m_pList->ValidateAll())
	{
		SaveList();
		OnOK();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIAlarmNumberDialog::OnLvnItemActivateSdiListAlarmNo(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	GetDlgItem(IDC_SDI_BTN_DELETE)->EnableWindow(IsBetween(pNMIA->iItem, 0, m_pList->GetItemCount()-1));
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CSDIAlarmNumberDialog::OnBnClickedSdiBtnSave()
{
	COemFileDialog dlg(this);
	CString sLoad,sExt;
	SaveList();

	GetWindowText(sExt);
	sLoad.LoadString(IDS_SAVE);
	dlg.SetProperties(FALSE, sLoad, sLoad);
	dlg.SetInitialDirectory(theApp.m_sOpenDir);
	dlg.AddFilter(sExt,_T("anr"));
	if (IDOK==dlg.DoModal())
	{
		m_pAlarmNumbers->Save(dlg.GetPathname());
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIAlarmNumberDialog::OnBnClickedSdiBtnLoad()
{
	COemFileDialog dlg(this);
	CString sLoad,sExt;

	sLoad.LoadString(IDS_LOAD);
	GetWindowText(sExt);
	dlg.SetProperties(TRUE,sLoad,sLoad);
	dlg.SetInitialDirectory(theApp.m_sOpenDir);
	dlg.AddFilter(sExt, _T("anr"));
	if (IDOK==dlg.DoModal())
	{
		m_pAlarmNumbers->Load(dlg.GetPathname());
		InitList();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIAlarmNumberDialog::InitList()
{
	m_pList->DeleteAllItems();
	int i, nCount = m_pAlarmNumbers->GetSize();

	switch (m_eType) 
	{
	case SDICT_DESIGNA_PM_100_ALARM:
	case SDICT_DESIGNA_PM_ABACUS:
		{
			CString a;
			for (i=0; i<nCount; i++)
			{
				CString s = m_pAlarmNumbers->GetAt(i);
				m_pList->InsertItem(i, CWK_Profile::GetStringFromString(s, INI_NR, _T("")));
				m_pList->SetItemText(i, 1, CWK_Profile::GetStringFromString(s, INI_BFR_NUMBER, _T("")));
				m_pList->SetItemText(i, 2, CWK_Profile::GetStringFromString(s, INI_TCC_NUMBER, _T("")));
				DWORD dwAlarm = m_pAlarmNumbers->GetAlarmNr(i);
				a = CSDIControl::GetPM100AlarmDescription(dwAlarm);
				if (!a.IsEmpty())
				{
					s.Format(_T("%s, %d"),a,dwAlarm);
				}
				m_pList->SetItemText(i, 3, s);
			}
		}
		break;
	case SDICT_DESIGNA_PM_100_XML:
		{
			CString a;
			for (i=0; i<nCount; i++)
			{
				CString s = m_pAlarmNumbers->GetAt(i);
				m_pList->InsertItem(i, CWK_Profile::GetStringFromString(s, INI_NR, _T("")));
				m_pList->SetItemText(i, 1, CWK_Profile::GetStringFromString(s, INI_BFR_NUMBER, _T("")));
				m_pList->SetItemText(i, 2, CWK_Profile::GetStringFromString(s, INI_TCC_NUMBER, _T("")));
				DWORD dwAlarm = m_pAlarmNumbers->GetAlarmNr(i);
				a = CSDIControl::GetPM100XMLDescription(dwAlarm);
				if (!a.IsEmpty())
				{
					s.Format(_T("%s, %d"),a,dwAlarm);
				}
				m_pList->SetItemText(i, 3, s);
			}
		}
		break;
	case SDICT_SCHNEIDER_INTERCOM:
		{
			CString a;
			for (i=0; i<nCount; i++)
			{
				CString s = m_pAlarmNumbers->GetAt(i);
				m_pList->InsertItem(i, CWK_Profile::GetStringFromString(s, INI_NR, _T("")));
				m_pList->SetItemText(i, 1, CWK_Profile::GetStringFromString(s, INI_SUBSCRIBER, _T("")));
				m_pList->SetItemText(i, 2, CWK_Profile::GetStringFromString(s, INI_COMMENT, _T("")));
			}
		}
		break;
	default:
		ASSERT(0);
		break;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIAlarmNumberDialog::SaveList()
{
	if (m_pAlarmNumbers)
	{
		m_pAlarmNumbers->RemoveAll();
		int i, nCount = m_pList->GetItemCount();

		switch (m_eType) 
		{
		case SDICT_DESIGNA_PM_100_ALARM:
		case SDICT_DESIGNA_PM_100_XML:
		case SDICT_DESIGNA_PM_ABACUS:
			for (i=0; i<nCount; i++)
			{
				CString sBFR,sTCC;
				CString sItem3 = m_pList->GetItemText(i, 3);
				int pos = sItem3.Find(_T(','));
				CString sANr = sItem3.Mid(pos+1);
				CString sComment, sCommentTemp = sItem3.Left(pos);

				sBFR = m_pList->GetItemText(i, 1);
				sTCC = m_pList->GetItemText(i, 2);
				sANr.TrimLeft();
				sANr.TrimRight();
				sCommentTemp.TrimLeft();
				sCommentTemp.TrimRight();
				sComment.Format(_T("BFR %s TCC %s %s"),sBFR,sTCC,sCommentTemp);

				m_pAlarmNumbers->AddDesignaAlarm(m_pList->GetItemText(i, 0),	// sNr
												sBFR, // sBFR		
												sTCC, // sTCC
												sANr, // sANr
												_T(""), // sSNr
												sComment);// sComment
			}
			break;
		case SDICT_SCHNEIDER_INTERCOM:
			for (i=0; i<nCount; i++)
			{
				CString sNr,sSubcriberNr,sComment;
				sNr = m_pList->GetItemText(i,0);
				sSubcriberNr = m_pList->GetItemText(i,1);
				sComment = m_pList->GetItemText(i,2);
				m_pAlarmNumbers->AddSchneiderIntercom(sNr,sSubcriberNr,sComment);
			}
			break;
		default:
			break;
		}

	}
}
