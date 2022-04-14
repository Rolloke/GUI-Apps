// DlgLoadCompressed.cpp : implementation file
//

#include "stdafx.h"
#include "LogView.h"
#include "DlgLoadCompressed.h"
#include ".\dlgloadcompressed.h"


/////////////////////////////////////////////////////////////////////////////
// CDlgLoadCompressed dialog
IMPLEMENT_DYNAMIC(CDlgLoadCompressed, CDialog)
CDlgLoadCompressed::CDlgLoadCompressed(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgLoadCompressed::IDD, pParent)
{
}
/////////////////////////////////////////////////////////////////////////////
CDlgLoadCompressed::~CDlgLoadCompressed()
{
}
/////////////////////////////////////////////////////////////////////////////
void CDlgLoadCompressed::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FILE_LIST, m_List);
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDlgLoadCompressed, CDialog)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_FILE_LIST, OnGetDispInfoList)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(ID_EDIT_SELECT_ALL, OnBnClickedEditSelectAll)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CDlgLoadCompressed message handlers
BOOL CDlgLoadCompressed::OnInitDialog()
{
	CDialog::OnInitDialog();
	if (!m_sHeadLine.IsEmpty())
	{
		SetWindowText(m_sHeadLine);
	}
	CRect rc;
	m_List.GetClientRect(&rc);
	m_List.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,
		LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT,
		LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);

	m_List.InsertColumn(0, _T(""), LVCFMT_LEFT, rc.Width());
	if (m_pFiles)
	{
		m_List.SetItemCount(m_pFiles->GetCount());
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CDlgLoadCompressed::OnGetDispInfoList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pFiles)
	{
		LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
		if(pDispInfo->item.mask & LVIF_TEXT)
		{
			if (pDispInfo->item.iSubItem == 0)
			{
				const CString &str = m_pFiles->GetAt(pDispInfo->item.iItem);
				int nFind = str.ReverseFind(_T('\\'));
				if (nFind == -1) nFind = 0;
				else			 nFind++;
				_tcsncpy(pDispInfo->item.pszText, str.Mid(nFind), pDispInfo->item.cchTextMax);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDlgLoadCompressed::OnBnClickedOk()
{
	if (m_pFiles)
	{
		int i, nItem = -1, nCount = m_List.GetSelectedCount();
		CStringArray sa;
		if (m_sHeadLine.IsEmpty() && nCount > 20)
		{
			CString str;
			str.Format(IDS_MSG_LOAD_ALL, nCount);
			if (AfxMessageBox(str, MB_YESNO|MB_ICONINFORMATION) == IDNO)
			{
				return;
			}
		}
		for (i=0; i<nCount; i++)
		{
			nItem = m_List.GetNextItem(nItem, LVNI_SELECTED);
			sa.Add(m_pFiles->GetAt(nItem));
		}
		m_pFiles->RemoveAll();
		m_pFiles->Append(sa);
	}
	OnOK();
}
/////////////////////////////////////////////////////////////////////////////
void CDlgLoadCompressed::OnBnClickedCancel()
{
	if (m_pFiles)
	{
		m_pFiles->RemoveAll();
	}
	OnCancel();
}
/////////////////////////////////////////////////////////////////////////////
void CDlgLoadCompressed::OnBnClickedEditSelectAll()
{
	int i, nCount = m_List.GetItemCount();
	for (i=0; i<nCount; i++)
	{
		m_List.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
	}
}
/////////////////////////////////////////////////////////////////////////////
