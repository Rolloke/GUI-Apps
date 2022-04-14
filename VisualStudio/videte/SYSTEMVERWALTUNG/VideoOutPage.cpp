// VideoOutPage.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "VideoOutPage.h"

#include "SVDoc.h"
#include "SVView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static TCHAR BASED_CODE szSectionQUnitGeneral[] = _T("QUnit\\General");

/////////////////////////////////////////////////////////////////////////////
// CVideoOutPage property page
IMPLEMENT_DYNAMIC(CVideoOutPage, CSVPage)
/////////////////////////////////////////////////////////////////////////////
CVideoOutPage::CVideoOutPage(CSVView* pParent) : CSVPage(CVideoOutPage::IDD)
{
	//{{AFX_DATA_INIT(CVideoOutPage)
	m_sName = _T("");
	m_sCameras = _T("");
	m_dwDwellTime = 0;
	m_bAllCameras = FALSE;
	//}}AFX_DATA_INIT
	m_pParent = pParent;
	m_pProcessList			= pParent->GetDocument()->GetProcesses();
	m_pOutputList			= pParent->GetDocument()->GetOutputs();
	m_pSelectedProcess		= NULL;
	m_iSelectedProcessItem	= -1;
	m_iSelectedColumn		= 0;	// used for sort
	m_eQuadType = QCT_UNKNOWN;


	Create(IDD,(CWnd*)m_pParent);
}
/////////////////////////////////////////////////////////////////////////////
CVideoOutPage::~CVideoOutPage()
{
}
/////////////////////////////////////////////////////////////////////////////
void CVideoOutPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVideoOutPage)
	DDX_Control(pDX, IDC_CHECK_ALL_CAMERAS, m_btnAllCameras);
	DDX_Control(pDX, IDC_VOUT_TXT_TYPE, m_txtType);
	DDX_Control(pDX, IDC_VOUT_TXT_OUT, m_txtOutput);
	DDX_Control(pDX, IDC_VOUT_TXT_NAME, m_txtName);
	DDX_Control(pDX, IDC_VOUT_TXT_EXTENSIONCARD, m_txtExtensionCard);
	DDX_Control(pDX, IDC_VOUT_TXT_DWELLTIME, m_txtDwellTime);
	DDX_Control(pDX, IDC_VOUT_TXT_CAMERAS, m_txtCameras);
	DDX_Control(pDX, IDC_VOUT_EDIT_DWELLTIME, m_editDwellTime);
	DDX_Control(pDX, IDC_VOUT_EDIT_CAMERAS, m_editCameras);
	DDX_Control(pDX, IDC_VOUT_EDIT_NAME, m_editName);
	DDX_Control(pDX, IDC_VOUT_LIST_PROCESSES, m_listProcesses);
	DDX_Control(pDX, IDC_VOUT_LIST_TYPE, m_listTypes);
	DDX_Control(pDX, IDC_VOUT_LIST_EXTENSIONCARD, m_listExtensionCard);
	DDX_Control(pDX, IDC_VOUT_LIST_OUT, m_listOutputs);
	DDX_Text(pDX, IDC_VOUT_EDIT_NAME, m_sName);
	DDX_Text(pDX, IDC_VOUT_EDIT_CAMERAS, m_sCameras);
	DDX_Text(pDX, IDC_VOUT_EDIT_DWELLTIME, m_dwDwellTime);
	DDX_Check(pDX, IDC_CHECK_ALL_CAMERAS, m_bAllCameras);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CVideoOutPage, CSVPage)
	//{{AFX_MSG_MAP(CVideoOutPage)
	ON_NOTIFY(NM_CLICK, IDC_VOUT_LIST_PROCESSES, OnClickVideoOutListProcesses)
	ON_NOTIFY(NM_DBLCLK, IDC_VOUT_LIST_PROCESSES, OnDblclkVideoOutListProcesses)
	ON_NOTIFY(NM_RCLICK, IDC_VOUT_LIST_PROCESSES, OnRclickVideoOutListProcesses)
	ON_NOTIFY(NM_RDBLCLK, IDC_VOUT_LIST_PROCESSES, OnRdblclkVideoOutListProcesses)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_VOUT_LIST_PROCESSES, OnItemchangedVideoOutListProcesses)
	ON_EN_CHANGE(IDC_VOUT_EDIT_CAMERAS, OnChangeVideoOutEditCameras)
	ON_EN_CHANGE(IDC_VOUT_EDIT_DWELLTIME, OnChangeVideoOutEditDwelltime)
	ON_EN_CHANGE(IDC_VOUT_EDIT_NAME, OnChangeVideoOutEditName)
	ON_BN_CLICKED(IDC_CHECK_ALL_CAMERAS, OnCheckAllCameras)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_VOUT_LIST_PROCESSES, OnColumnclickVoutListProcesses)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
BOOL CVideoOutPage::StretchElements()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CVideoOutPage::CanNew()
{
	return (m_listExtensionCard.GetItemCount() > 0);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CVideoOutPage::CanDelete()
{
	return (m_listProcesses.GetItemCount() > 0);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CVideoOutPage::OnInitDialog() 
{
	CSVPage::OnInitDialog();

	// Init list controls
	InitListControls();

	FillTypes();
	FillExtensionCards();
	FillOutputs();

	CancelChanges(); // will fill processes anyway
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CVideoOutPage::InitListControls()
{
	LONG dw = ListView_GetExtendedListViewStyle(m_listProcesses.m_hWnd);
	dw |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	ListView_SetExtendedListViewStyle(m_listProcesses.m_hWnd,dw);
//	m_listProcesses.ModifyStyle(LVS_SINGLESEL,0);

	const int		iColumns = 6;
	CRect			rect;
	LV_COLUMN		lvcolumn;
	CString sArray[iColumns];
	int i;
	int w[] = {20,20,20,10,15,15};	// CAVET add sum has to be 100

	// Init process listctrl
	m_listProcesses.GetClientRect(&rect);
	m_txtName.GetWindowText(sArray[0]);
	m_txtType.GetWindowText(sArray[1]);
	m_txtExtensionCard.GetWindowText(sArray[2]);
	m_txtOutput.GetWindowText(sArray[3]);
	m_txtCameras.GetWindowText(sArray[4]);
	m_txtDwellTime.GetWindowText(sArray[5]);

	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_LEFT;

	CString s;
	for (i = 0 ; i < iColumns ; i++)  // add the columns to the list control
	{
		s = sArray[i];
		s.Replace(_T(":"), _T(""));
		s.Replace(_T("&"), _T(""));
		lvcolumn.cx = (rect.Width() * w[i])/100;
		lvcolumn.pszText = (LPTSTR)LPCTSTR(s);
		lvcolumn.iSubItem = i;
		m_listProcesses.InsertColumn(i, &lvcolumn);  // assumes return value is OK.
	}

	// enable string and bitmp
	m_headerArrowBitmap.LoadBitmap(IDB_DOWN);
	HD_ITEM hdItem;
	memset(&hdItem,0,sizeof(HD_ITEM));
	hdItem.mask = HDI_BITMAP | HDI_FORMAT;
	m_listProcesses.GetHeaderCtrl()->GetItem(m_iSelectedColumn ,&hdItem);
	hdItem.mask = HDI_BITMAP | HDI_FORMAT;;
	hdItem.fmt |= HDF_BITMAP;
	hdItem.hbm = m_headerArrowBitmap;
	m_listProcesses.GetHeaderCtrl()->SetItem(m_iSelectedColumn ,&hdItem);

	lvcolumn.mask = LVCF_SUBITEM | LVCF_WIDTH;
	lvcolumn.iSubItem = 0;


	// Init type listctrl
	m_listTypes.SetStateInfo(theApp.GetRadioImage(),0,16);
	m_listTypes.GetClientRect(&rect);
	lvcolumn.cx = rect.Width();
	m_listTypes.InsertColumn(0,&lvcolumn);

	// Init group listctrl
	m_listExtensionCard.SetStateInfo(theApp.GetRadioImage(),0,16);
	m_listExtensionCard.GetClientRect(&rect);
	lvcolumn.cx = rect.Width();
	m_listExtensionCard.InsertColumn(0,&lvcolumn);

	// Init Output listctrl
	m_listOutputs.SetStateInfo(theApp.GetRadioImage(),0,16);
	m_listOutputs.GetClientRect(&rect);
	lvcolumn.cx = rect.Width();
	m_listOutputs.InsertColumn(0,&lvcolumn);
}
/////////////////////////////////////////////////////////////////////////////
void CVideoOutPage::EnableExceptNew()
{
	BOOL bEnable = m_listProcesses.GetItemCount();
	
	m_listProcesses.EnableWindow(bEnable);
	m_editName.EnableWindow(bEnable);
	m_listTypes.EnableWindow(bEnable);
	m_listExtensionCard.EnableWindow(bEnable);
	m_listOutputs.EnableWindow(bEnable);
	m_editCameras.EnableWindow(bEnable && !m_bAllCameras);
	m_btnAllCameras.EnableWindow(bEnable);
	
	BOOL bEnableDwellTime = FALSE;
	CProcess::VOutSubType subType = (CProcess::VOutSubType)m_listTypes.GetRadioItemData();
	if (   (subType == CProcess::VOut_Sequencer)
		|| (subType == CProcess::VOut_Activation)
		)
	{
		bEnableDwellTime = TRUE;
	}
	m_editDwellTime.EnableWindow(bEnableDwellTime);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CVideoOutPage::IsDataValid()
{
	// first actualize data from dialog
	ControlToProcess();
	
	CProcess	*pProcess, *pProcess2;
	CString		sName;
	CProcess::VOutSubType	subType;
	CSecID		idGroupCard;
	DWORD		dwOutput;
	CString		sCameras;
	DWORD		dwDwellTime;

	DWORD dwMaxCams = 0;
	COutputGroup* pGroup = NULL;
	BOOL bValid = TRUE;
	CString sWrong;
	int iIndex = 0;

	for (int i=0 ; i<m_listProcesses.GetItemCount() ; i++)
	{
		pProcess = (CProcess*)m_listProcesses.GetItemData(i);
		pProcess->GetVideoOut(sName, subType, idGroupCard, dwOutput, sCameras, dwDwellTime);

		// name empty or double
		if (sName.IsEmpty())
		{
			bValid = FALSE;
		}
		for (int j=i+1 ; j<m_listProcesses.GetItemCount() ; j++)
		{
			pProcess2 = (CProcess*)m_listProcesses.GetItemData(j);
			if (sName == pProcess2->GetName())
			{
				bValid = FALSE;
			}
		}
		if (bValid == FALSE)
		{
			SelectProcessItem(i);
			AfxMessageBox(IDP_NAME_INVALID, MB_ICONSTOP);
			m_editName.SetFocus();
			m_editName.SetSel(0, -1);
			return FALSE;
		}
		// type empty
		if (subType == CProcess::VOut_NoSubType)
		{
			SelectProcessItem(i);
			AfxMessageBox(IDP_VOUT_SUBTYPE_INVALID, MB_ICONSTOP);
			return FALSE;
		}
		// Any Extension card
		if (idGroupCard == SECID_NO_ID)
		{
			SelectProcessItem(i);
			AfxMessageBox(IDP_VOUT_NO_EXTENSIONCARD, MB_ICONSTOP);
			return FALSE;
		}
		// Any output
		if (dwOutput == (DWORD)(-1))
		{
			SelectProcessItem(i);
			AfxMessageBox(IDP_VOUT_NO_OUTPUT, MB_ICONSTOP);
			return FALSE;
		}
		// Any camera
		if (sCameras.IsEmpty())
		{
			AfxMessageBox(IDP_NO_CAMERAS_SELECTED, MB_ICONSTOP);
			bValid = FALSE;
		}
		else
		{
			dwMaxCams = 0;
			pGroup = m_pOutputList->GetGroupByID(idGroupCard);
			if (pGroup)
			{
				dwMaxCams = pGroup->GetSize();
			}
			else if (idGroupCard.GetID() != QCT_UNKNOWN)
			{
				dwMaxCams = 4;
			}
			int iRet = IsCameraValid(sCameras, sWrong, iIndex, dwMaxCams);
			switch (iRet)
			{
			case 0:
				AfxMessageBox(IDP_NO_CAMERAS_SELECTED, MB_ICONSTOP);
				bValid = FALSE;
				break;
			case -1:
				AfxMessageBox(IDP_CAMERAS_INVALID_CHAR, MB_ICONSTOP);
				bValid = FALSE;
				break;
			case -2:
				AfxMessageBox(IDP_CAMERAS_INVALID_CAM, MB_ICONSTOP);
				bValid = FALSE;
				break;
			case -3:
				AfxMessageBox(IDP_CAMERAS_INVALID_RANGE, MB_ICONSTOP);
				bValid = FALSE;
				break;
			default:
				bValid = TRUE;
				break;
			}
		}
		if (bValid == FALSE)
		{
			SelectProcessItem(i);
			m_editCameras.SetFocus();
			iIndex = m_sCameras.Find(sWrong, iIndex);
			if (iIndex != -1)
			{
				m_editCameras.SetSel(iIndex, iIndex+sWrong.GetLength());
			}
			else
			{
				m_editCameras.SetSel(0, -1);
			}
			return FALSE;
		}
		// Dwell time always valid (0 means infinite)
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CVideoOutPage::ProcessToControl()
{
	// CAVEAT set m_bAllCameras to FALSE, it is evaluated later
	m_bAllCameras = FALSE;
	TRACE("m_dwDwellTime before get %u\n", m_dwDwellTime);
	CProcess::VOutSubType subType = CProcess::VOut_NoSubType;
	CSecID idGroupCard = SECID_NO_ID;
	DWORD dwOutput = (DWORD)(-1);
	if (m_pSelectedProcess)
	{
		m_pSelectedProcess->GetVideoOut(m_sName, subType, idGroupCard, dwOutput, m_sCameras, m_dwDwellTime);
		TRACE("m_dwDwellTime got %u\n", m_dwDwellTime);
	}
	else
	{
		m_sCameras = "";
		m_dwDwellTime = 0;
	}

	// CAVEAT first update dialog controls, SetRadioItem may call UpdateData()
	UpdateData(FALSE);
	
	m_listTypes.SetRadioItemFromlParam((LPARAM)subType);
	DWORD dwCardID = idGroupCard.GetID();
	m_listExtensionCard.SetRadioItemFromlParam((LPARAM)dwCardID);
	m_listOutputs.SetRadioItemFromlParam(dwOutput);
	DWORD dwMaxCams = 0;
	COutputGroup* pGroup = m_pOutputList->GetGroupByID(idGroupCard);
	if (pGroup)
	{
		dwMaxCams = pGroup->GetSize();
		CString sCams;
		sCams.Format(_T("1-%u"), dwMaxCams);
		m_bAllCameras = (sCams == m_sCameras);
	}
	else if (idGroupCard.GetID()!=QCT_UNKNOWN)
	{
		CString sCams;
		sCams.Format(_T("1-%u"), dwMaxCams);
		m_bAllCameras = (sCams == m_sCameras);
	}


	TRACE("m_dwDwellTime before update data %u\n", m_dwDwellTime);
	UpdateData(FALSE);
	TRACE("m_dwDwellTime after update data %u\n", m_dwDwellTime);
	// Undo any changes to the modified state
	SetModified(FALSE, FALSE);
	EnableExceptNew();
}
/////////////////////////////////////////////////////////////////////////////
void CVideoOutPage::ControlToProcess()
{
	if (m_pSelectedProcess)
	{
		UpdateData();

		CProcess::VOutSubType subType = (CProcess::VOutSubType)m_listTypes.GetRadioItemData();
		CSecID idGroupCard;
		idGroupCard.Set((DWORD)m_listExtensionCard.GetRadioItemData());
		DWORD dwOutput = (DWORD)m_listOutputs.GetRadioItemData();
		m_pSelectedProcess->SetVideoOut(m_sName, subType, idGroupCard, dwOutput, m_sCameras, m_dwDwellTime);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVideoOutPage::CancelChanges()
{
	m_pProcessList->Reset();
	m_pProcessList->Load(GetProfile());

	if (IsLocal())
	{
		m_eQuadType = (QuadControlType)GetProfile().GetInt(_T("Quad"),_T("Type"),QCT_UNKNOWN);
	}
	FillExtensionCards();
	FillProcesses();
	m_iSelectedColumn = 0;	// default sorting
	m_listProcesses.SortItems(MyCompareFunction , (DWORD) this);
	SetModified(FALSE, FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CVideoOutPage::SaveChanges()
{
	WK_TRACE_USER(_T("Videoausgang-Einstellungen wurden geändert\n"));
	m_pProcessList->Save(GetProfile());
}
/////////////////////////////////////////////////////////////////////////////
void CVideoOutPage::OnNew()
{
	ControlToProcess();

	CString	sName;
	CProcess::VOutSubType subType = CProcess::VOut_Sequencer;
	CSecID idGroupCard = SECID_NO_ID;
	if (m_listExtensionCard.GetItemCount())
	{
		idGroupCard.Set(m_listExtensionCard.GetItemData(0));
	}
	DWORD dwOutput = 0;
	CString sCameras;
	DWORD dwDwellTime = 5;
	// Take previous settings for new process
	if (m_pSelectedProcess)
	{
		m_pSelectedProcess->GetVideoOut(sName, subType, idGroupCard, dwOutput, sCameras, dwDwellTime);
	}
	// but be sure to use the default name
	sName.LoadString(IDS_VIDEO_OUT);
	CProcess* pProcess = m_pProcessList->AddProcess();
	pProcess->SetVideoOut(sName, subType, idGroupCard, dwOutput, sCameras, dwDwellTime);
	int iIndex = InsertProcess(pProcess);

	// First select process
	SelectProcessItem(iIndex);
	m_editName.SetFocus();
	m_editName.SetSel(0, -1);
	// Set Modified at very last, because SelectProcessItem -> ProcessToControl resets Modified state!
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CVideoOutPage::OnDelete()
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
			m_listProcesses.SetFocus();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVideoOutPage::OnItemChangedStatusList(CStatusList* pStatusList, int nListItem)
{
	if (m_pSelectedProcess==NULL)
		return;
	
	if (pStatusList == &m_listTypes)
	{
		OnItemChangedTypesList(nListItem);
	}
	else if (pStatusList == &m_listExtensionCard)
	{
		OnItemChangedExtensionCardList(nListItem);
	}
	else if (pStatusList == &m_listOutputs)
	{
		OnItemChangedOutputsList(nListItem);
	}
	else 
	{
		TRACE(_T("Invalid CStatusList* for OnItemChangedStatusList\n"));
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVideoOutPage::FillProcesses()
{
	m_listProcesses.DeleteAllItems();
	m_pSelectedProcess = NULL;

	int iCount = m_pProcessList->GetSize();
	CProcess* pProcess = NULL;
	for (int j=0 ; j<iCount ; j++)
	{
		pProcess = m_pProcessList->GetAt(j);
		if (pProcess && pProcess->IsVideoOut())
		{
			InsertProcess(pProcess);
		}
	} // for
	int iSelect = (m_listProcesses.GetItemCount() > 0) ? 0 : (-1);
	SelectProcessItem(iSelect);
}
/////////////////////////////////////////////////////////////////////////////
void CVideoOutPage::FillTypes()
{
	m_listTypes.DeleteAllItems();

	CString s;
	LV_ITEM lvis;
	CProcess::VOutSubType subType;
	int iCount;

	lvis.mask = LVIF_TEXT|LVIF_PARAM|LVIF_STATE;
	lvis.iSubItem = 0;
	lvis.state=INDEXTOSTATEIMAGEMASK(1);
	lvis.stateMask=LVIS_STATEIMAGEMASK;

	subType = CProcess::VOut_Client;
	iCount = m_listTypes.GetItemCount();
	s = CProcess::VOutSubType2String(subType);
	lvis.iItem = iCount;
	lvis.pszText = s.GetBuffer(0);
	lvis.cchTextMax = s.GetLength();
	lvis.lParam = subType;
	m_listTypes.InsertItem(&lvis);
	s.ReleaseBuffer();

	subType = CProcess::VOut_Sequencer;
	iCount = m_listTypes.GetItemCount();
	s = CProcess::VOutSubType2String(subType);
	lvis.iItem = iCount;
	lvis.pszText = s.GetBuffer(0);
	lvis.cchTextMax = s.GetLength();
	lvis.lParam = subType;
	m_listTypes.InsertItem(&lvis);
	s.ReleaseBuffer();

	subType = CProcess::VOut_Activation;
	iCount = m_listTypes.GetItemCount();
	s = CProcess::VOutSubType2String(subType);
	lvis.iItem = iCount;
	lvis.pszText = s.GetBuffer(0);
	lvis.cchTextMax = s.GetLength();
	lvis.lParam = subType;
	m_listTypes.InsertItem(&lvis);
	s.ReleaseBuffer();

	// set previous selection, will be ignored if not furthermore in list
	if (m_pSelectedProcess)
	{
		m_listTypes.SetRadioItemFromlParam(m_pSelectedProcess->GetVOutSubType());
		OnItemChangedTypesList(m_listTypes.GetRadioItem());
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVideoOutPage::FillExtensionCards()
{
	m_listExtensionCard.DeleteAllItems();

	CString s;
	LV_ITEM lvis;
	int iCount;
	CSecID idGroupCard;

	lvis.mask = LVIF_TEXT|LVIF_PARAM|LVIF_STATE;
	lvis.iSubItem = 0;
	lvis.state=INDEXTOSTATEIMAGEMASK(1);
	lvis.stateMask=LVIS_STATEIMAGEMASK;

	COutputGroup* pGroup = NULL;
	for (int i=0 ; i<m_pOutputList->GetSize() ; i++)
	{
		pGroup = m_pOutputList->GetGroupAt(i);
		if (pGroup && pGroup->CanVideoOut())
		{
			iCount = m_listOutputs.GetItemCount();
			s = pGroup->GetName();
			lvis.iItem = iCount;
			lvis.pszText = s.GetBuffer(0);
			lvis.cchTextMax = s.GetLength();
			lvis.lParam = pGroup->GetID().GetID();
			m_listExtensionCard.InsertItem(&lvis);
			s.ReleaseBuffer();
		}
	}

	if (m_eQuadType!=QCT_UNKNOWN)
	{
		iCount = m_listOutputs.GetItemCount();
		s = NameOfEnumQuadRealName(m_eQuadType);
		lvis.iItem = iCount;
		lvis.pszText = s.GetBuffer(0);
		lvis.cchTextMax = s.GetLength();
		lvis.lParam = CSecID((WORD)m_eQuadType,0).GetID();
		m_listExtensionCard.InsertItem(&lvis);
		s.ReleaseBuffer();
	}

	// set previous selection, will be ignored if not furthermore in list
	if (m_pSelectedProcess)
	{
		m_listExtensionCard.SetRadioItemFromlParam(m_pSelectedProcess->GetVOutGroupID().GetID());
		OnItemChangedTypesList(m_listExtensionCard.GetRadioItem());
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVideoOutPage::FillOutputs()
{
	m_listOutputs.DeleteAllItems();

	CString s;
	LV_ITEM lvis;
	int iCount;
	DWORD dwOutput; // GUI shows 1...n, LPARAM is 0...n-1

	lvis.mask = LVIF_TEXT|LVIF_PARAM|LVIF_STATE;
	lvis.iSubItem = 0;
	lvis.state=INDEXTOSTATEIMAGEMASK(1);
	lvis.stateMask=LVIS_STATEIMAGEMASK;

	DWORD dwNrOfOutputs = 2;

	if (GetDongle().RunTashaUnit1())
	{
		dwNrOfOutputs = 4;
	}
	else if (GetDongle().RunQUnit())
	{
		int iNrOfVideoOuts = 1;
		dwNrOfOutputs = GetProfile().GetInt(szSectionQUnitGeneral,_T("NrOfVideoOuts"), iNrOfVideoOuts);
	}

	for (dwOutput=1;dwOutput<=dwNrOfOutputs;dwOutput++)
	{
		iCount = m_listOutputs.GetItemCount();
		s.Format(_T("%u"), dwOutput);
		lvis.iItem = iCount;
		lvis.pszText = s.GetBuffer(0);
		lvis.cchTextMax = s.GetLength();
		lvis.lParam = dwOutput-1;
		m_listOutputs.InsertItem(&lvis);
		s.ReleaseBuffer();
	}

	// set previous selection, will be ignored if not furthermore in list
	if (m_pSelectedProcess)
	{
		m_listOutputs.SetRadioItemFromlParam(m_pSelectedProcess->GetVOutOutput());
		OnItemChangedTypesList(m_listOutputs.GetRadioItem());
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVideoOutPage::SelectProcessItem(int i)
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
/////////////////////////////////////////////////////////////////////////////
void CVideoOutPage::SelectProcessItemByProcess(CProcess* pProcessSearch)
{
	CProcess* pProcess = NULL;
	for (int i=0 ; i<m_listProcesses.GetItemCount() ; i++)
	{
		pProcess = (CProcess*)m_listProcesses.GetItemData(i);
		if (pProcess == pProcessSearch)
		{
			SelectProcessItem(i);
			break;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
int CVideoOutPage::InsertProcess(CProcess* pProcess)
{
	LV_ITEM lvis;
	CString s;

	// Get data from process
	CString	sName;
	CProcess::VOutSubType subType;
	CSecID idGroupCard;
	DWORD dwOutput;
	CString sCameras;
	DWORD dwDwellTime;
	pProcess->GetVideoOut(sName, subType, idGroupCard, dwOutput, sCameras, dwDwellTime);

	// Am Ende einfuegen, daher ListControl-Count holen
	int iCount = m_listProcesses.GetItemCount();
	int iIndex = -1;

	// Item einfuegen
	s = sName;
	if ( s.IsEmpty() ) {
		s.LoadString(IDS_NONAME);
	}
	lvis.mask = LVIF_TEXT | LVIF_PARAM;
	lvis.pszText = s.GetBuffer(0);
	lvis.cchTextMax = s.GetLength();
	lvis.lParam = (LPARAM)pProcess;
	lvis.iItem = iCount;
	lvis.iSubItem = 0;
	iIndex = m_listProcesses.InsertItem(&lvis);
	s.ReleaseBuffer();

	// SubType setzen
	s = CProcess::VOutSubType2String(subType);
	m_listProcesses.SetItemText(iIndex, 1, s);

	// ExtensionCard setzen
	s = "";
	COutputGroup* pGroup = m_pOutputList->GetGroupByID(idGroupCard);
	if (pGroup)
	{
		s = pGroup->GetName();
	}
	else if (idGroupCard.GetID() != QCT_UNKNOWN)
	{
		s = NameOfEnumQuadRealName((QuadControlType)idGroupCard.GetID());
	}
	m_listProcesses.SetItemText(iIndex,2,s);

	// Ausgang setzen
	s = "";
	if (dwOutput != (DWORD)(-1))
	{
		s.Format(_T("%u"), dwOutput+1);
	}
	m_listProcesses.SetItemText(iIndex,3,s);

	// Kameras setzen
	s = sCameras;
	m_listProcesses.SetItemText(iIndex,4,s);

	// Verweildauer setzen
	s.Format(_T("%u"), dwDwellTime);
	m_listProcesses.SetItemText(iIndex,5,s);

	return iIndex;
}
/////////////////////////////////////////////////////////////////////////////
void CVideoOutPage::OnClickVideoOutListProcesses(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (   (pNMListView->iItem == -1)
		&& (m_iSelectedProcessItem != -1)
		)
	{
		// Es wurde ins leere Feld geklickt,
		// daher Select-Markierung wiederherstellen
		m_listProcesses.SetItemState(m_iSelectedProcessItem,
									LVIS_SELECTED|LVIS_FOCUSED,
									LVIS_SELECTED|LVIS_FOCUSED);
	}
	EnableExceptNew();
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CVideoOutPage::OnDblclkVideoOutListProcesses(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (   (pNMListView->iItem == -1)
		&& (m_iSelectedProcessItem != -1)
		)
	{
		// Es wurde ins leere Feld geklickt,
		// daher Select-Markierung wiederherstellen
		m_listProcesses.SetItemState(m_iSelectedProcessItem,
									LVIS_SELECTED|LVIS_FOCUSED,
									LVIS_SELECTED|LVIS_FOCUSED);
	}	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CVideoOutPage::OnRclickVideoOutListProcesses(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (   (pNMListView->iItem == -1)
		&& (m_iSelectedProcessItem != -1)
		)
	{
		// Es wurde ins leere Feld geklickt,
		// daher Select-Markierung wiederherstellen
		m_listProcesses.SetItemState(m_iSelectedProcessItem,
									LVIS_SELECTED|LVIS_FOCUSED,
									LVIS_SELECTED|LVIS_FOCUSED);
	}	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CVideoOutPage::OnRdblclkVideoOutListProcesses(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (   (pNMListView->iItem == -1)
		&& (m_iSelectedProcessItem != -1)
		)
	{
		// Es wurde ins leere Feld geklickt,
		// daher Select-Markierung wiederherstellen
		m_listProcesses.SetItemState(m_iSelectedProcessItem,
									LVIS_SELECTED|LVIS_FOCUSED,
									LVIS_SELECTED|LVIS_FOCUSED);
	}	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CVideoOutPage::OnItemchangedVideoOutListProcesses(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem != m_iSelectedProcessItem)
	{
		if (pNMListView->uNewState & LVIS_SELECTED)
		{
			SelectProcessItem(pNMListView->iItem);
		}
	}
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CVideoOutPage::OnChangeVideoOutEditName() 
{
	SetModified();
	UpdateData();
	m_listProcesses.SetItemText(m_iSelectedProcessItem, 0, m_sName);
}
/////////////////////////////////////////////////////////////////////////////
void CVideoOutPage::OnItemChangedTypesList(int nListItem)
{
	SetModified();
	EnableExceptNew();
	int iIndex = m_listTypes.GetRadioItem();
	CString sText = m_listTypes.GetItemText(iIndex, 0);
	m_listProcesses.SetItemText(m_iSelectedProcessItem, 1, sText);
	SetDwellTime();
}
/////////////////////////////////////////////////////////////////////////////
void CVideoOutPage::OnItemChangedExtensionCardList(int nListItem)
{
	SetModified();
	int iIndex = m_listExtensionCard.GetRadioItem();
	CString sText = m_listExtensionCard.GetItemText(iIndex, 0);
	m_listProcesses.SetItemText(m_iSelectedProcessItem, 2, sText);
	SetCamerasString();
}
/////////////////////////////////////////////////////////////////////////////
void CVideoOutPage::OnItemChangedOutputsList(int nListItem)
{
	SetModified();
	int iIndex = m_listOutputs.GetRadioItem();
	CString sText = m_listOutputs.GetItemText(iIndex, 0);
	m_listProcesses.SetItemText(m_iSelectedProcessItem, 3, sText);
}
/////////////////////////////////////////////////////////////////////////////
void CVideoOutPage::OnChangeVideoOutEditCameras() 
{
	SetModified();
	m_editCameras.GetWindowText(m_sCameras);
	m_listProcesses.SetItemText(m_iSelectedProcessItem, 4, m_sCameras);
}
/////////////////////////////////////////////////////////////////////////////
void CVideoOutPage::OnCheckAllCameras() 
{
	SetModified();
	UpdateData();
	EnableExceptNew();
	SetCamerasString();
	if (m_bAllCameras == FALSE)
	{
		m_editCameras.SetFocus();
		m_editCameras.SetSel(0, -1);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVideoOutPage::OnChangeVideoOutEditDwelltime() 
{
	SetModified();
	UpdateData();
	CString sText;
	sText.Format(_T("%u"), m_dwDwellTime);
	m_listProcesses.SetItemText(m_iSelectedProcessItem, 5, sText);
}
/////////////////////////////////////////////////////////////////////////////
void CVideoOutPage::SetCamerasString() 
{
	if (m_bAllCameras)
	{
		DWORD dwMaxCams = 0;
		CSecID idGroupCard;
		idGroupCard.Set((DWORD)m_listExtensionCard.GetRadioItemData());
		COutputGroup* pGroup = m_pOutputList->GetGroupByID(idGroupCard);
		if (pGroup)
		{
			dwMaxCams = pGroup->GetSize();
		}
		else if (m_eQuadType!=QCT_UNKNOWN)
		{
			dwMaxCams = 4;
		}
		m_sCameras.Format(_T("1-%u"), dwMaxCams);
		m_listProcesses.SetItemText(m_iSelectedProcessItem, 4, m_sCameras);
		m_editCameras.SetWindowText(m_sCameras);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVideoOutPage::SetDwellTime()
{
	CProcess::VOutSubType subType = (CProcess::VOutSubType)m_listTypes.GetRadioItemData();
	if (subType == CProcess::VOut_Client)
	{
		m_dwDwellTime = 0;
		CString sDwellTime;
		sDwellTime.Format(_T("%u"), m_dwDwellTime);
		m_listProcesses.SetItemText(m_iSelectedProcessItem, 5, sDwellTime);
		m_editDwellTime.SetWindowText(sDwellTime);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVideoOutPage::OnColumnclickVoutListProcesses(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	// remove the bitmap from the current column
	HD_ITEM hdItem;
	memset(&hdItem,0,sizeof(HD_ITEM));
	hdItem.mask = HDI_FORMAT;
	m_listProcesses.GetHeaderCtrl()->GetItem(m_iSelectedColumn ,&hdItem);
	hdItem.mask = HDI_FORMAT;
	hdItem.fmt &= ~HDF_BITMAP;
	m_listProcesses.GetHeaderCtrl()->SetItem(m_iSelectedColumn ,&hdItem);

	// take the newly activated column
	m_iSelectedColumn = pNMListView->iSubItem;

	HBITMAP hbmp = m_headerArrowBitmap;
	if (hbmp) {
		// enable string and bitmp
		memset(&hdItem,0,sizeof(HD_ITEM));
		hdItem.mask = HDI_BITMAP | HDI_FORMAT;;
		m_listProcesses.GetHeaderCtrl()->GetItem(m_iSelectedColumn ,&hdItem);
		hdItem.mask = HDI_BITMAP | HDI_FORMAT;;
		hdItem.fmt |= HDF_BITMAP;
		hdItem.hbm = m_headerArrowBitmap;
		m_listProcesses.GetHeaderCtrl()->SetItem(m_iSelectedColumn ,&hdItem);
	}

	m_listProcesses.SortItems(MyCompareFunction, (DWORD)this);
	SelectProcessItemByProcess(m_pSelectedProcess);
	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
int CALLBACK CVideoOutPage::MyCompareFunction(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CVideoOutPage* pPage = (CVideoOutPage*)lParamSort;

	CProcess* pProcessA = (CProcess*)lParam1;	// input from MFC (item data)
	CProcess* pProcessB = (CProcess*)lParam2;	// input from MFC (item data)

	// get according strings
	// name
	CString sNameA = pProcessA->GetName();
	CString sNameB = pProcessB->GetName();

	// Type
	CString sTypeA = CProcess::VOutSubType2String(pProcessA->GetVOutSubType());
	CString sTypeB = CProcess::VOutSubType2String(pProcessB->GetVOutSubType());

	// Extension card
	CSecID idGroupA = pProcessA->GetVOutGroupID();
	CSecID idGroupB = pProcessB->GetVOutGroupID();
	COutputGroup* pGroupA = pPage->GetOutputList()->GetGroupByID(idGroupA);
	COutputGroup* pGroupB = pPage->GetOutputList()->GetGroupByID(idGroupB);

	CString sExtensionCardA,sExtensionCardB;
	
	if (pGroupA)
	{
		sExtensionCardA = pGroupA->GetName();
	}
	else if (idGroupA.GetID() != QCT_UNKNOWN)
	{
		sExtensionCardA = NameOfEnumQuadRealName((QuadControlType)idGroupA.GetID());
	}
	if (pGroupB)
	{
		sExtensionCardB = pGroupB->GetName();
	}
	else if (idGroupB.GetID() != QCT_UNKNOWN)
	{
		sExtensionCardB = NameOfEnumQuadRealName((QuadControlType)idGroupB.GetID());
	}

	// Video output
	DWORD dwOutputA = pProcessA->GetVOutOutput();
	DWORD dwOutputB = pProcessB->GetVOutOutput();

	// Cameras
	CString sCamerasA = pProcessA->GetCameras();
	CString sCamerasB = pProcessB->GetCameras();

	// Dwell time
	DWORD dwDwelltimeA = pProcessA->GetSwitchTime();
	DWORD dwDwelltimeB = pProcessB->GetSwitchTime();

	// The comparison function must return 
	//	a negative value if the first item should precede the second, 
	//	a positive value if the first item should follow the second, 
	//	or zero if the two items are equivalent.

	// switch sort style by column
	int cmp = 0;
	switch (pPage->m_iSelectedColumn)
	{
		case 1:	// Type
		cmp = sTypeA.CompareNoCase(sTypeB);
		break;
		//////////////////////////////
		case 2:	// Extension card
		cmp = sExtensionCardA.CompareNoCase(sExtensionCardB);
		break;
		//////////////////////////////
		case 3:	// Video output
		cmp = dwOutputA - dwOutputB;
		break;
		//////////////////////////////
		case 4:	// Cameras
		cmp = sCamerasA.CompareNoCase(sCamerasB);
		break;
		//////////////////////////////
		case 5:	// Dwell time
		cmp = dwDwelltimeA - dwDwelltimeB;
		break;
		default: // Name
		cmp = sNameA.CompareNoCase(sNameB);
	}	// end of column switch

	return cmp;
}
