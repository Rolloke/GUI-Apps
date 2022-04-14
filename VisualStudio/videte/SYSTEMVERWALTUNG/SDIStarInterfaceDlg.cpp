// SDIStarInterfaceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SystemVerwaltung.h"
#include "SDIAlarmType.h"
#include "SVView.h"
#include "SVDoc.h"
#include "SDIStarInterfaceDlg.h"
#include ".\sdistarinterfacedlg.h"


// CSDIStarInterfaceDlg dialog

IMPLEMENT_DYNAMIC(CSDIStarInterfaceDlg, CDialog)
CSDIStarInterfaceDlg::CSDIStarInterfaceDlg(SDIControlType eType,
										   int iCom,
										   CSDIProtocolArray* pProtocolArray,
										   CSDIAlarmTypeArray* pAlarmTypeArray,
										   CComParameters* pComParameters,
										   CSVView* pView)
	: CDialog(CSDIStarInterfaceDlg::IDD, pView)
{
	m_iSelectedItem = -1;
}

CSDIStarInterfaceDlg::~CSDIStarInterfaceDlg()
{
	m_Devices.DeleteAll();
}

void CSDIStarInterfaceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_ID, m_ctrlID);
	DDX_Control(pDX, IDC_COMBO_TYP, m_ctrlType);
	DDX_Control(pDX, IDC_LIST_CONTROLS, m_ctrlControls);
	DDX_Control(pDX, IDC_EDIT_NAME, m_editName);
	DDX_Control(pDX, IDC_BUTTON_NEW, m_btnNew);
	DDX_Control(pDX, IDC_BUTTON_DELETE, m_btnDelete);
}


BEGIN_MESSAGE_MAP(CSDIStarInterfaceDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_NEW, OnBnClickedButtonNew)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnBnClickedButtonDelete)
	ON_EN_CHANGE(IDC_EDIT_NAME, OnEnChangeEditName)
	ON_CBN_SELCHANGE(IDC_COMBO_ID, OnCbnSelchangeComboId)
	ON_CBN_SELCHANGE(IDC_COMBO_TYP, OnCbnSelchangeComboTyp)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_CONTROLS, OnLvnItemchangedListControls)
END_MESSAGE_MAP()


// CSDIStarInterfaceDlg message handlers
void CSDIStarInterfaceDlg::InitListBox()
{
	CWordArray waWidth;

	CRect rect;
	m_ctrlControls.GetWindowRect(&rect);
	int iWidth = rect.Width()-3;
	waWidth.Add(10);
	waWidth.Add(40);
	waWidth.Add(50);

	CStringArray saHeader;
	CString sHeader;

	GetDlgItemText(IDC_STATIC_ID,sHeader);
	saHeader.Add(sHeader);
	GetDlgItemText(IDC_STATIC_TYP,sHeader);
	saHeader.Add(sHeader);
	GetDlgItemText(IDC_STATIC_NAME,sHeader);
	saHeader.Add(sHeader);

	LV_COLUMN		lvcolumn;
	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_LEFT;

	CString s;
	for (int i = 0; i < waWidth.GetSize(); i++)  // add the columns to the list control
	{
		s = saHeader.GetAt(i);
		s.Replace(_T(":"), _T(""));
		s.Replace(_T("&"), _T(""));
		lvcolumn.cx = (iWidth * waWidth[i])/100;
		lvcolumn.pszText = (LPTSTR)LPCTSTR(s);
		lvcolumn.iSubItem = i;
		m_ctrlControls.InsertColumn(i, &lvcolumn);  // assumes return value is OK.
	}

	LONG dw = ListView_GetExtendedListViewStyle(m_ctrlControls.m_hWnd);
	dw |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	ListView_SetExtendedListViewStyle(m_ctrlControls.m_hWnd,dw);

}
/////////////////////////////////////////////////////////////////////
void CSDIStarInterfaceDlg::FillListBox()
{
	m_ctrlControls.DeleteAllItems();
	
	for (int i=0;i<m_Devices.GetSize();i++)
	{
		CSDIStarDevice* pDevice = m_Devices.GetAtFast(i);
		AddToList(pDevice);
	}
}
/////////////////////////////////////////////////////////////////////
int CSDIStarInterfaceDlg::AddToList(CSDIStarDevice* pDevice)
{
	CString sID;
	CString sTyp = CSDIStarDevice::UINameOfDevice(pDevice->GetType());
	sID.Format(_T("%02d"),pDevice->GetID());
	int index = m_ctrlControls.InsertItem(m_ctrlControls.GetItemCount(),sID);
	m_ctrlControls.SetItemData(index,pDevice->GetID());
	m_ctrlControls.SetItemText(index,1,sTyp);
	m_ctrlControls.SetItemText(index,2,pDevice->GetName());
	return index;
}
/////////////////////////////////////////////////////////////////////
void CSDIStarInterfaceDlg::EnableExceptNew()
{
	int iNr = m_ctrlControls.GetItemCount();
	BOOL bEnable = iNr>0;

	m_ctrlID.EnableWindow(bEnable);
	m_ctrlType.EnableWindow(bEnable);
	m_btnDelete.EnableWindow(bEnable);
	m_editName.EnableWindow(bEnable);
}
//////////////////////////////////////////////////////////////////////
void CSDIStarInterfaceDlg::SelectItem(int i)
{
	if (i>=0)
	{
		CString sTyp;
		m_iSelectedItem = i;
		m_ctrlControls.SetItemState(m_iSelectedItem,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
		DWORD dwID = m_ctrlControls.GetItemData(m_iSelectedItem);
		CSDIStarDevice* pDevice = m_Devices.GetSDIStarDevice(dwID);
		if (pDevice)
		{
			m_editName.SetWindowText(pDevice->GetName());
			m_ctrlID.SetCurSel(dwID);
			m_ctrlType.SetCurSel(pDevice->GetType());
		}

	}
	else
	{
		m_iSelectedItem = -1;
		m_editName.SetWindowText(_T(""));
		m_ctrlID.SetCurSel(-1);
		m_ctrlType.SetCurSel(-1);
	}
	UpdateData(FALSE);
}
//////////////////////////////////////////////////////////////////////
BOOL CSDIStarInterfaceDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CSkinDialog::SetChildWindowFont(m_hWnd);

	InitListBox();
	FillListBox();
	EnableExceptNew();

	CString sType;
	for (StarInterfaceType type = SIT_FINGER_005;;type=(StarInterfaceType)(type+1))
	{
		sType = CSDIStarDevice::UINameOfDevice(type);
		if (!sType.IsEmpty())
		{
			int index = m_ctrlType.AddString(sType);
			m_ctrlType.SetItemData(index,type);
		}
		else
		{
			break;
		}
	}

	for (int i=0;i<10;i++)
	{
		CString sID;
		sID.Format(_T("%02d"),i);
		m_ctrlID.AddString(sID);
	}
	if (m_ctrlControls.GetItemCount()>0)
	{
		SelectItem(0);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSDIStarInterfaceDlg::OnBnClickedButtonNew()
{
	// TODO: Add your control notification handler code here
	CSDIStarDevice* pDevice = NULL;

	BOOL bFound = TRUE;
	DWORD dwID = 0;

	while (bFound)
	{
		bFound = FALSE;
		for (int i=0;i<m_Devices.GetSize()&&!bFound;i++)
		{
			pDevice = m_Devices.GetAtFast(i);
			bFound = pDevice->GetID() == dwID;
		}
		if (bFound)
		{
			dwID++;
		}
	}
	pDevice = new CSDIStarDevice(dwID,_T(""),SIT_UNKNOWN);
	m_Devices.Add(pDevice);
	m_iSelectedItem = AddToList(pDevice);
	SelectItem(m_iSelectedItem);

	EnableExceptNew();
	m_editName.SetFocus();
	m_editName.SetSel(0,-1);
}

void CSDIStarInterfaceDlg::OnBnClickedButtonDelete()
{
	if (m_iSelectedItem!=-1)
	{
		DWORD dwID = m_ctrlControls.GetItemData(m_iSelectedItem);
		CSDIStarDevice* pDevice = NULL;
		BOOL bFound = FALSE;
		for (int i=0;i<m_Devices.GetSize()&&!bFound;i++)
		{
			pDevice = m_Devices.GetAtFast(i);
			bFound = pDevice->GetID() == dwID;
			if (bFound)
			{
				m_Devices.RemoveAt(i);
				WK_DELETE(pDevice);
			}
		}
		m_ctrlControls.DeleteItem(m_iSelectedItem);
		if (m_ctrlControls.GetItemCount()>0)
		{
			SelectItem(0);
		}
	}
	EnableExceptNew();
}

void CSDIStarInterfaceDlg::OnEnChangeEditName()
{
	CString sName; 
	m_editName.GetWindowText(sName);
	if (m_iSelectedItem!=-1)
	{
		DWORD dwID = m_ctrlControls.GetItemData(m_iSelectedItem);
		CSDIStarDevice* pDevice = m_Devices.GetSDIStarDevice(dwID);
		if (pDevice)
		{
			pDevice->SetName(sName);
		}
		m_ctrlControls.SetItemText(m_iSelectedItem,2,sName);
	}
}

void CSDIStarInterfaceDlg::OnCbnSelchangeComboId()
{
	int iID = m_ctrlID.GetCurSel();
	CSDIStarDevice* pDevice = NULL;

	if (m_iSelectedItem!=-1)
	{
		BOOL bFound = FALSE;
		for (int i=0;i<m_Devices.GetSize()&&!bFound;i++)
		{
			pDevice = m_Devices.GetAtFast(i);
			bFound = pDevice->GetID() == (DWORD)iID;
		}
		if (!bFound)
		{
			DWORD dwID = m_ctrlControls.GetItemData(m_iSelectedItem);
			CSDIStarDevice* pDevice = m_Devices.GetSDIStarDevice(dwID);
			if (pDevice)
			{
				pDevice->SetID(iID);
			}
			CString sID;
			sID.Format(_T("%02d"),iID);
			m_ctrlControls.SetItemText(m_iSelectedItem,0,sID);
			m_ctrlControls.SetItemData(m_iSelectedItem,iID);
		}
	}
}

void CSDIStarInterfaceDlg::OnCbnSelchangeComboTyp()
{
	int iType = m_ctrlType.GetCurSel();
	if (m_iSelectedItem!=-1)
	{
		DWORD dwID = m_ctrlControls.GetItemData(m_iSelectedItem);
		CSDIStarDevice* pDevice = m_Devices.GetSDIStarDevice(dwID);
		if (pDevice)
		{
			pDevice->SetType((StarInterfaceType)iType);
		}
		CString sTyp = CSDIStarDevice::UINameOfDevice((StarInterfaceType)iType);
		m_ctrlControls.SetItemText(m_iSelectedItem,1,sTyp);
	}
}

void CSDIStarInterfaceDlg::OnLvnItemchangedListControls(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMListView = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if (pNMListView->iItem!=m_iSelectedItem)
	{
		if (pNMListView->uNewState & LVIS_SELECTED)
		{
			SelectItem(pNMListView->iItem);
		}
	}
	*pResult = 0;
}
