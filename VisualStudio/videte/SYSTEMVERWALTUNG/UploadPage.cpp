// UploadPage.cpp : implementation file
//

#include "stdafx.h"
#include "SystemVerwaltung.h"
#include "UploadPage.h"
#include ".\uploadpage.h"


// CUploadPage dialog

IMPLEMENT_DYNAMIC(CUploadPage, CSVPage)
CUploadPage::CUploadPage(CSVView* pParent) : CSVPage(CUploadPage::IDD)
, m_dwRecTime(0)
, m_dwMainTime(0)
{
	m_pParent = pParent;
	m_pInputList = pParent->GetDocument()->GetInputs();
	m_pProcessList = pParent->GetDocument()->GetProcesses();
	m_pSelectedProcess = NULL;
	m_iSelectedProcessItem = -1;
	Create(IDD,(CWnd*)m_pParent);
}

CUploadPage::~CUploadPage()
{
}

void CUploadPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PROCESSES, m_listProcesses);
	DDX_Control(pDX, IDC_EDIT_NAME, m_editName);
	DDX_Control(pDX, IDC_EDIT_RECTIME, m_editTime);
	DDX_Control(pDX, IDC_RESOLUTION, m_comboResolution);
	DDX_Control(pDX, IDC_COMPRESSION, m_comboCompression);
	DDX_Control(pDX, IDC_EDIT_SERVER, m_editServer);
	DDX_Control(pDX, IDC_EDIT_LOGIN, m_editLogin);
	DDX_Control(pDX, IDC_EDIT_PASSWORD, m_editPassword);
	DDX_Control(pDX, IDC_SAVE_MAINTIME, m_editDuration);
	DDX_Control(pDX, IDC_SAVE_HOLD, m_radioHold);
	DDX_Control(pDX, IDC_SAVE_FOREVER, m_radioForever);
	DDX_Text(pDX,IDC_EDIT_RECTIME,m_dwRecTime);
	DDX_Text(pDX,IDC_SAVE_MAINTIME,m_dwMainTime);
	DDX_Control(pDX, IDC_EDIT_FILENAME, m_editFilename);
}


BEGIN_MESSAGE_MAP(CUploadPage, CSVPage)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_PROCESSES, OnLvnItemchangedListProcesses)
	ON_EN_CHANGE(IDC_EDIT_NAME, OnEnChangeEditName)
	ON_EN_CHANGE(IDC_EDIT_RECTIME, OnEnChangeRectime)
	ON_CBN_SELCHANGE(IDC_RESOLUTION, OnCbnSelchangeResolution)
	ON_CBN_SELCHANGE(IDC_COMPRESSION, OnCbnSelchangeCompression)
	ON_EN_CHANGE(IDC_EDIT_SERVER, OnEnChangeEditServer)
	ON_EN_CHANGE(IDC_EDIT_LOGIN, OnEnChangeEditLogin)
	ON_EN_CHANGE(IDC_EDIT_PASSWORD, OnEnChangeEditPassword)
	ON_BN_CLICKED(IDC_SAVE_HOLD, OnBnClickedSaveHold)
	ON_BN_CLICKED(IDC_SAVE_FOREVER, OnBnClickedSaveForever)
	ON_EN_CHANGE(IDC_SAVE_MAINTIME, OnEnChangeSaveMaintime)
	ON_EN_CHANGE(IDC_EDIT_FILENAME, OnEnChangeEditFilename)
END_MESSAGE_MAP()


// CUploadPage message handlers
// Implementation
///////////////////////////////////////////////////////////////////
void CUploadPage::SaveChanges()
{
	WK_TRACE_USER(_T("FTP-HTTP upload settings changed\n"));
	m_pProcessList->Save(GetProfile());
}
///////////////////////////////////////////////////////////////////
void CUploadPage::CancelChanges()
{
	m_pProcessList->Reset();
	m_pProcessList->Load(GetProfile());
	FillProcesses();
	EnableExceptNew();
	SetModified(FALSE,FALSE);
}
///////////////////////////////////////////////////////////////////
BOOL CUploadPage::CanNew()
{
	return TRUE;
}
///////////////////////////////////////////////////////////////////
BOOL CUploadPage::IsDataValid()
{
	ControlToProcess();
	return TRUE;
}
///////////////////////////////////////////////////////////////////
BOOL CUploadPage::CanDelete()
{
	return m_listProcesses.GetItemCount();
}
///////////////////////////////////////////////////////////////////
void CUploadPage::OnNew()
{
	ControlToProcess();

	// Clear all temporary data for new process
	m_pSelectedProcess = NULL; // already saved, new one will be selected
	m_iSelectedProcessItem = -1;

	CProcess* pProcess = m_pProcessList->AddProcess();
	CString sName;
	sName.LoadString(IDS_PROZESS_UPLOAD);
	CString sTemp;
	sTemp.Format(_T("%s %d"),sName,m_listProcesses.GetItemCount()+1);
	pProcess->SetUpload(sTemp,
						0,
						60, 
						RESOLUTION_2CIF,
						COMPRESSION_3,
						TRUE,
						_T("ftp://"),
						_T(""),
						_T(""),
						_T("nnnnnnnnnnnnnnnnYYYYMMDD"));

	int iIndex = InsertProcess(pProcess);
	SetModified();
	SelectProcessItem(iIndex);
}
///////////////////////////////////////////////////////////////////
void CUploadPage::OnDelete()
{
	if ( (m_iSelectedProcessItem==-1) || (m_pSelectedProcess==NULL))
	{
		return;
	}

	if (m_pParent->GetDocument()->CheckActivationsWithProcess(m_pSelectedProcess->GetID()))
	{
		m_pProcessList->DeleteProcess(m_pSelectedProcess);
		m_pSelectedProcess = NULL;
		// calc new selection
		int	newCount = m_listProcesses.GetItemCount()-1;	// pre-subtract before DeleteItem
		int i = m_iSelectedProcessItem;	// new selection

		// range check
		if (i > newCount-1 ) {	// outside or last one?
			i=newCount-1;
		}
		if (m_listProcesses.DeleteItem(m_iSelectedProcessItem))	// remove from listCtrl
		{
			SelectProcessItem(i);
			SetModified();
			EnableExceptNew();
			m_listProcesses.SetFocus();
		}
	}
}
///////////////////////////////////////////////////////////////////
BOOL CUploadPage::StretchElements()
{
	return TRUE;
}
///////////////////////////////////////////////////////////////////
BOOL CUploadPage::OnInitDialog()
{
	CSVPage::OnInitDialog();

	InitProcessList();

	CString sFill;

	int i;
	sFill.LoadString(IDS_COMP1);
	i = m_comboCompression.AddString(sFill);
	m_comboCompression.SetItemData(i,COMPRESSION_1);

	sFill.LoadString(IDS_COMP2);
	i = m_comboCompression.AddString(sFill);
	m_comboCompression.SetItemData(i,COMPRESSION_2);

	sFill.LoadString(IDS_COMP3);
	i = m_comboCompression.AddString(sFill);
	m_comboCompression.SetItemData(i,COMPRESSION_3);

	sFill.LoadString(IDS_COMP4);
	i = m_comboCompression.AddString(sFill);
	m_comboCompression.SetItemData(i,COMPRESSION_4);

	sFill.LoadString(IDS_COMP5);
	i = m_comboCompression.AddString(sFill);
	m_comboCompression.SetItemData(i,COMPRESSION_5);

	i = m_comboResolution.AddString(_T("2CIF"));
	m_comboResolution.SetItemData(i,RESOLUTION_2CIF);
	i = m_comboResolution.AddString(_T("CIF"));
	m_comboResolution.SetItemData(i,RESOLUTION_CIF);

	CancelChanges();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
///////////////////////////////////////////////////////////////////
void CUploadPage::EnableExceptNew()
{
	BOOL bEnableProcesses = (m_listProcesses.GetItemCount() > 0);
	m_listProcesses.EnableWindow(bEnableProcesses);

	BOOL bEnable = bEnableProcesses && (m_pSelectedProcess != NULL);

	m_editName.EnableWindow(bEnable);
	m_editTime.EnableWindow(bEnable);
	
	m_comboResolution.EnableWindow(bEnable);
	m_comboCompression.EnableWindow(bEnable);
	m_editServer.EnableWindow(bEnable);
	m_editLogin.EnableWindow(bEnable);
	m_editPassword.EnableWindow(bEnable);
	m_editDuration.EnableWindow(bEnable);
	m_radioHold.EnableWindow(bEnable);
	m_radioForever.EnableWindow(bEnable);
	m_editFilename.EnableWindow(bEnable);
}
///////////////////////////////////////////////////////////////////
void CUploadPage::InitProcessList()
{
	LONG dw = ListView_GetExtendedListViewStyle(m_listProcesses.m_hWnd);
	dw |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	ListView_SetExtendedListViewStyle(m_listProcesses.m_hWnd,dw);

	CRect rect;
	int w[] = {10          ,10                ,10         ,15					   ,15                  ,10                ,10                ,10				  ,10};
	int h[] = {IDC_STATIC_NAME,IDC_TXT_RECTIME,IDC_TXTRESOLUTION,IDC_TXTCOMPRESSION,0			        ,IDC_TXT_NOTFOREVER,IDC_STATIC_SERVER,IDC_STATIC_LOGIN,IDC_STATIC_FILENAME	};
	int ht[]= {0           ,0                 ,0          ,0                       ,IDS_ACTIVATION_STATE,0                 ,0				  ,0			  ,0};
	const int nColumns = sizeof(w)/sizeof(int);
	m_listProcesses.GetWindowRect(rect);

	LV_COLUMN	lvcolumn;
	CString s;

	lvcolumn.mask	= LVCF_FMT | LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH;
	lvcolumn.fmt	= LVCFMT_LEFT;

	for (int i = 0; i < nColumns; i++)  // add the columns to the list control
	{
		lvcolumn.cx = MulDiv(rect.Width(), w[i], 100);
		if (h[i]) 
		{
			GetDlgItemText(h[i], s);
		}
		else if (ht[i]) 
		{
			s.LoadString(ht[i]);
		}
		s.Replace(_T(":"), _T(""));
		s.Replace(_T("&"), _T(""));
		lvcolumn.pszText  = (LPTSTR)LPCTSTR(s);
		lvcolumn.iSubItem = i;

		m_listProcesses.InsertColumn(i, &lvcolumn);  // assumes return value is OK.
	}
}
///////////////////////////////////////////////////////////////////
void CUploadPage::FillProcesses()
{
	m_listProcesses.DeleteAllItems();
	m_pSelectedProcess = NULL;

	CProcess* pProcess;
	for (int i=0 ; i<m_pProcessList->GetSize() ; i++)
	{
		if (m_pProcessList->GetAt(i)->IsUpload())
		{
			pProcess = m_pProcessList->GetAt(i);
			InsertProcess(pProcess);
		}
	}
	int iSelect = (m_listProcesses.GetItemCount() > 0) ? 0 : -1;
	SelectProcessItem(iSelect);
}
///////////////////////////////////////////////////////////////////
int CUploadPage::InsertProcess(CProcess* pProcess)
{
	int iIndex = -1;
	if (pProcess)
	{
		CString sText;
		LV_ITEM lvis;
		lvis.mask = LVIF_TEXT|LVIF_PARAM;
		int iCount = m_listProcesses.GetItemCount();
		lvis.iItem = iCount;
		lvis.iSubItem = 0;
		lvis.lParam = (long)pProcess;
		lvis.pszText = (LPTSTR)LPCTSTR(pProcess->GetName());
		lvis.cchTextMax = pProcess->GetName().GetLength();
		iIndex = m_listProcesses.InsertItem(&lvis);
		lvis.iItem = iIndex;
		SetListItemText(iIndex,pProcess);

	}
	else
	{
		WK_TRACE_ERROR(_T("CUploadPage::InsertProcess pProcess = NULL\n"));
	}
	return iIndex;
}
////////////////////////////////////////////////////////////////
void CUploadPage::SetListItemText(int i, CProcess* pProcess)
{
	CString sText;
	sText.Format(_T("%d"),pProcess->GetPause()/1000+1);
	m_listProcesses.SetItemText(i, 1, sText);

	Resolution res = pProcess->GetResolution();
	switch (res) 
	{
	case RESOLUTION_2CIF:
		sText = _T("2CIF");
		break;
	case RESOLUTION_CIF:
		sText = _T("CIF");
		break;
	}
	m_listProcesses.SetItemText(i, 2, sText);

	switch (pProcess->GetCompression()) 
	{
	case COMPRESSION_1:
		sText.LoadString(IDS_COMP1);
		break;
	case COMPRESSION_2:
		sText.LoadString(IDS_COMP2);
		break;
	case COMPRESSION_3:
		sText.LoadString(IDS_COMP3);
		break;
	case COMPRESSION_4:
		sText.LoadString(IDS_COMP4);
		break;
	case COMPRESSION_5:
		sText.LoadString(IDS_COMP5);
		break;
	}
	m_listProcesses.SetItemText(i, 3, sText);

	if (pProcess->IsHold()) 
		GetDlgItemText(IDC_SAVE_HOLD, sText);
	else                    
		GetDlgItemText(IDC_SAVE_FOREVER, sText);
	sText.Replace(_T("&"), _T(""));
	m_listProcesses.SetItemText(i, 4, sText);

	sText.Format(_T("%d"),pProcess->GetDuration());
	m_listProcesses.SetItemText(i, 5, sText);

	m_listProcesses.SetItemText(i, 6, pProcess->GetServerName());
	m_listProcesses.SetItemText(i, 7, pProcess->GetLogin());
	m_listProcesses.SetItemText(i,8,pProcess->GetFileName());
}
///////////////////////////////////////////////////////////////////
void CUploadPage::SelectProcessItem(int i)
{
	ControlToProcess();
	m_iSelectedProcessItem = i;
	if (m_iSelectedProcessItem != -1)
	{
		m_pSelectedProcess = (CProcess*)m_listProcesses.GetItemData(m_iSelectedProcessItem);
	}
	else
	{
		m_pSelectedProcess = NULL;
	}
	ProcessToControl();
	m_listProcesses.SetItemState(m_iSelectedProcessItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
}
///////////////////////////////////////////////////////////////////
void CUploadPage::ControlToProcess()
{
	if (m_pSelectedProcess)
	{
		UpdateData();

		Resolution res = (Resolution)m_comboResolution.GetItemData(m_comboResolution.GetCurSel());
		Compression comp = (Compression)m_comboCompression.GetItemData(m_comboCompression.GetCurSel());

		CString sName,sLogin,sServer,sPassword,sFilename;
		m_editName.GetWindowText(sName);
		m_editLogin.GetWindowText(sLogin);
		m_editServer.GetWindowText(sServer);
		m_editPassword.GetWindowText(sPassword);
		m_editFilename.GetWindowText(sFilename);

		BOOL bHold = m_radioHold.GetCheck() == BST_CHECKED;
		m_pSelectedProcess->SetUpload(sName,m_dwMainTime,m_dwRecTime,res,comp,bHold,sServer,sLogin,sPassword,sFilename);
	}
}
///////////////////////////////////////////////////////////////////
void CUploadPage::ProcessToControl()
{
	if (m_pSelectedProcess)
	{
		m_dwMainTime = m_pSelectedProcess->GetDuration();
		m_dwRecTime = m_pSelectedProcess->GetPause()/1000+1;

		m_editServer.SetWindowText(m_pSelectedProcess->GetServerName());
		m_editLogin.SetWindowText(m_pSelectedProcess->GetLogin());
		m_editPassword.SetWindowText(m_pSelectedProcess->GetPassword());
		m_editName.SetWindowText(m_pSelectedProcess->GetName());
		m_editFilename.SetWindowText(m_pSelectedProcess->GetFileName());

		Resolution res = m_pSelectedProcess->GetResolution();
		for (int i=0;i<m_comboResolution.GetCount();i++)
		{
			if (res == (Resolution)m_comboResolution.GetItemData(i))
			{
				m_comboResolution.SetCurSel(i);
				break;
			}
		}
		Compression comp = m_pSelectedProcess->GetCompression();
		for (int i=0;i<m_comboCompression.GetCount();i++)
		{
			if (comp == (Compression)m_comboCompression.GetItemData(i))
			{
				m_comboCompression.SetCurSel(i);
				break;
			}
		}
		if (m_pSelectedProcess->IsHold())
		{
			m_radioHold.SetCheck(BST_CHECKED);
			m_radioForever.SetCheck(BST_UNCHECKED);
		}
		else
		{
			m_radioHold.SetCheck(BST_UNCHECKED);
			m_radioForever.SetCheck(BST_CHECKED);
		}
	}
	else
	{
	}
	UpdateData(FALSE);
	EnableExceptNew();
}
/////////////////////////////////////////////////////////////////////////////////////
void CUploadPage::OnLvnItemchangedListProcesses(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if (pNMLV->iItem != m_iSelectedProcessItem)
	{
		if (pNMLV->uNewState & LVIS_SELECTED)
		{
			SelectProcessItem(pNMLV->iItem);
		}
	}
	*pResult = 0;
}

void CUploadPage::OnEnChangeEditName()
{
	SetModified();
	CString sText;
	GetDlgItemText(IDC_EDIT_NAME, sText);
	sText.Replace(_T("&"), _T(""));
	m_listProcesses.SetItemText(m_iSelectedProcessItem, 0, sText);
}

void CUploadPage::OnEnChangeRectime()
{
	SetModified();
	CString sText;
	GetDlgItemText(IDC_EDIT_RECTIME, sText);
	sText.Replace(_T("&"), _T(""));
	m_listProcesses.SetItemText(m_iSelectedProcessItem, 1, sText);
}

void CUploadPage::OnCbnSelchangeResolution()
{
	SetModified();
	CString s;
	m_comboResolution.GetWindowText(s);
	m_listProcesses.SetItemText(m_iSelectedProcessItem,2,s);
}

void CUploadPage::OnCbnSelchangeCompression()
{
	SetModified();
	CString s;
	m_comboCompression.GetWindowText(s);
	m_listProcesses.SetItemText(m_iSelectedProcessItem,3,s);
}

void CUploadPage::OnEnChangeEditServer()
{
	SetModified();
	CString s;
	m_editServer.GetWindowText(s);
	m_listProcesses.SetItemText(m_iSelectedProcessItem,6,s);
}

void CUploadPage::OnEnChangeEditLogin()
{
	SetModified();
	CString s;
	m_editLogin.GetWindowText(s);
	m_listProcesses.SetItemText(m_iSelectedProcessItem,7,s);
}

void CUploadPage::OnEnChangeEditPassword()
{
	SetModified();
}

void CUploadPage::OnBnClickedSaveHold()
{
	SetModified();
	CString sText;
	GetDlgItemText(IDC_SAVE_HOLD, sText);
	sText.Replace(_T("&"), _T(""));
	m_listProcesses.SetItemText(m_iSelectedProcessItem, 4, sText);
}

void CUploadPage::OnBnClickedSaveForever()
{
	SetModified();
	CString sText;
	GetDlgItemText(IDC_SAVE_FOREVER, sText);
	sText.Replace(_T("&"), _T(""));
	m_listProcesses.SetItemText(m_iSelectedProcessItem, 4, sText);
}

void CUploadPage::OnEnChangeSaveMaintime()
{
	SetModified();
	CString sText;
	GetDlgItemText(IDC_SAVE_MAINTIME, sText);
	sText.Replace(_T("&"), _T(""));
	m_listProcesses.SetItemText(m_iSelectedProcessItem, 5, sText);
}

void CUploadPage::OnEnChangeEditFilename()
{
	SetModified();
	CString sText;
	m_editFilename.GetWindowText(sText);
	sText.Replace(_T("&"), _T(""));
	m_listProcesses.SetItemText(m_iSelectedProcessItem, 8, sText);
}
