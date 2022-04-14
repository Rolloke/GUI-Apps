// PermissionPage.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "PermissionPage.h"

#include "globals.h"
#include "SecID.h"
#include "OutputList.h"
#include "OutputGroup.h"

#include "PasswordCheckDialog.h"

#include "SVDoc.h"
#include "SVView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
1(hoch)
2
3
4
5(niedrig)
*/

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CPermissionPage property page
IMPLEMENT_DYNAMIC(CPermissionPage, CSVPage)

CPermissionPage::CPermissionPage(CSVView* pParent) : CSVPage(CPermissionPage::IDD)
{
	m_pParent = pParent;
	//{{AFX_DATA_INIT(CPermissionPage)
	m_Name = _T("");
	//}}AFX_DATA_INIT
	m_pPermissionArray = pParent->GetDocument()->GetPermissions();
	m_pOutputList = pParent->GetDocument()->GetOutputs();
	m_pMediaList		= pParent->GetDocument()->GetMedias();
	m_pArchivInfoArray = pParent->GetDocument()->GetArchivs();
	m_pUserArray = pParent->GetDocument()->GetUsers();
	m_pTimers = pParent->GetDocument()->GetTimers();

	m_iSelectedItem = -1;
	m_pSelectedPermission = NULL;
	m_sPassword = _T("");
	m_bEnableSaveFlag = FALSE;

	m_bEnableAudio		= GetDongle().RunAudioUnit();

	CString sVersion = pParent->GetDocument()->GetProfile().GetString(_T("Version"),_T("Number"),_T(""));

	m_bEnableTimer = sVersion > _T("5.1.2.1007");

	Create(IDD,(CWnd*)m_pParent);
}

CPermissionPage::~CPermissionPage()
{
}

void CPermissionPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPermissionPage)
	DDX_Control(pDX, IDC_CHECK_PTZ_CONFIG, m_btnPTZConfig);
	DDX_Control(pDX, IDC_CHECK_ALARM_OFF_SPAN, m_btnAlarmOffSpan);
	DDX_Control(pDX, IDC_CHECK_IMAGECOMPARE, m_btnImageCompare);
	DDX_Control(pDX, IDC_CHECK_SHUTDOWN, m_btnShutDown);
	DDX_Control(pDX, IDC_CHECK_SETTINGS, m_btnSettings);
	DDX_Control(pDX, IDC_CHECK_SECURITY, m_btnSecurity);
	DDX_Control(pDX, IDC_CHECK_SDICONFIG, m_btnSdiConfig);
	DDX_Control(pDX, IDC_CHECK_LOGVIEW, m_btnLogView);
	DDX_Control(pDX, IDC_CHECK_DATETIME, m_btnDateTime);
	DDX_Control(pDX, IDC_CHECK_DATA_EXPORT, m_btnDataExport);
	DDX_Control(pDX, IDC_CHECK_BACKUP, m_btnBackup);
	DDX_Control(pDX, IDC_CHECK_ARCHIV, m_btnArchiv);
	DDX_Control(pDX, IDC_CHECK_PERMISSION, m_btnPermission);
	DDX_Control(pDX, IDC_CHECK_HOST, m_btnHost);
	DDX_Control(pDX, IDC_CHECK_USER, m_btnUser);
	DDX_Control(pDX, IDC_CHECK_LINK, m_btnLink);
	DDX_Control(pDX, IDC_CHECK_PROZESS, m_btnProcess);
	DDX_Control(pDX, IDC_CHECK_TIMER, m_btnTimer);
	DDX_Control(pDX, IDC_CHECK_OUTPUT, m_btnOutput);
	DDX_Control(pDX, IDC_CHECK_INPUT, m_btnInput);
	DDX_Control(pDX, IDC_CHECK_HARDWARE, m_btnHardware);
	DDX_Control(pDX, IDC_PASSWORD, m_btnPassword);
	DDX_Control(pDX, IDC_NAME_PERMISSION, m_editName);
	DDX_Control(pDX, IDC_COMBO_PRIORITY, m_cbPriority);
	DDX_Control(pDX, IDC_COMBO_TIMER, m_cbTimer);
	DDX_Control(pDX, IDC_LIST_ARCHIVS, m_Archivs);
	DDX_Control(pDX, IDC_LIST_OUTPUTS, m_Outputs);
	DDX_Control(pDX, IDC_LIST_PERMISSION, m_Permissions);
	DDX_Text(pDX, IDC_NAME_PERMISSION, m_Name);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPermissionPage, CSVPage)
	//{{AFX_MSG_MAP(CPermissionPage)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_PERMISSION, OnItemchangedListPermission)
	ON_EN_CHANGE(IDC_NAME_PERMISSION, OnChangeName)
	ON_BN_CLICKED(IDC_PASSWORD, OnPassword)
	ON_BN_CLICKED(IDC_CHECK_HARDWARE, OnCheck)
	ON_CBN_SELCHANGE(IDC_COMBO_PRIORITY, OnSelchangeComboPriority)
	ON_CBN_SELCHANGE(IDC_COMBO_TIMER, OnSelchangeComboTimer)
	ON_BN_CLICKED(IDC_CHECK_INPUT, OnCheck)
	ON_BN_CLICKED(IDC_CHECK_OUTPUT, OnCheck)
	ON_BN_CLICKED(IDC_CHECK_TIMER, OnCheck)
	ON_BN_CLICKED(IDC_CHECK_PROZESS, OnCheck)
	ON_BN_CLICKED(IDC_CHECK_LINK, OnCheck)
	ON_BN_CLICKED(IDC_CHECK_USER, OnCheck)
	ON_BN_CLICKED(IDC_CHECK_PERMISSION, OnCheck)
	ON_BN_CLICKED(IDC_CHECK_HOST, OnCheck)
	ON_BN_CLICKED(IDC_CHECK_ARCHIV, OnCheck)
	ON_BN_CLICKED(IDC_CHECK_BACKUP, OnCheck)
	ON_BN_CLICKED(IDC_CHECK_SDICONFIG, OnCheck)
	ON_BN_CLICKED(IDC_CHECK_SHUTDOWN, OnCheck)
	ON_BN_CLICKED(IDC_CHECK_DATA_EXPORT, OnCheck)
	ON_BN_CLICKED(IDC_CHECK_SETTINGS, OnCheck)
	ON_BN_CLICKED(IDC_CHECK_SECURITY, OnCheck)
	ON_BN_CLICKED(IDC_CHECK_LOGVIEW, OnCheck)
	ON_BN_CLICKED(IDC_CHECK_DATETIME, OnCheck)
	ON_BN_CLICKED(IDC_CHECK_IMAGECOMPARE, OnCheck)
	ON_BN_CLICKED(IDC_CHECK_ALARM_OFF_SPAN, OnCheck)
	ON_BN_CLICKED(IDC_CHECK_PTZ_CONFIG, OnCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
void CPermissionPage::EnableExceptNew()
{
	BOOL bEnable;
	int c;
	c = m_Permissions.GetItemCount();
	bEnable = (c>0);

	if (m_pSelectedPermission)
	{
		if (m_pSelectedPermission->IsSuperVisor())
		{
			bEnable = FALSE;
		}

		if (m_pSelectedPermission->GetPriority() < theApp.m_pPermission->GetPriority())
		{
			bEnable = FALSE;
		}
	}
	else
	{
		bEnable = FALSE;
	}

	
	m_editName.EnableWindow(bEnable);
	m_btnPassword.EnableWindow(bEnable);

	m_btnHardware.EnableWindow(bEnable);
	m_btnInput.EnableWindow(bEnable);
	m_btnOutput.EnableWindow(bEnable);
	m_btnTimer.EnableWindow(bEnable);
	m_btnProcess.EnableWindow(bEnable);
	m_btnLink.EnableWindow(bEnable);
	m_btnUser.EnableWindow(bEnable);
	m_btnPermission.EnableWindow(bEnable);
	m_btnHost.EnableWindow(bEnable);
	m_btnArchiv.EnableWindow(bEnable);
	m_btnBackup.EnableWindow(bEnable);
	m_btnSdiConfig.EnableWindow(bEnable);
	m_btnShutDown.EnableWindow(bEnable);
 	m_btnDataExport.EnableWindow(bEnable);
	m_btnSecurity.EnableWindow(bEnable);
	m_btnSettings.EnableWindow(bEnable);
	m_btnLogView.EnableWindow(bEnable);
	m_btnDateTime.EnableWindow(bEnable);
	m_btnImageCompare.EnableWindow(bEnable);
	m_btnAlarmOffSpan.EnableWindow(bEnable);

	CString sVersion = GetProfile().GetString(_T("Version"),_T("Number"),_T(""));

	m_btnImageCompare.ShowWindow(FALSE);

	m_Outputs.EnableWindow(bEnable);
	m_Archivs.EnableWindow(bEnable);
	m_cbPriority.EnableWindow(bEnable);
	m_cbTimer.EnableWindow(bEnable && m_bEnableTimer);
	m_btnPTZConfig.EnableWindow(bEnable);

	if (m_pSelectedPermission)
	{
		if (m_pSelectedPermission->IsSuperVisor())
		{
			m_btnPassword.EnableWindow(TRUE);
			m_Permissions.EnableWindow();
			m_Permissions.SetFocus();
		}
		else
		{
			DWORD dwPFlags = m_pParent->GetDocument()->GetFlags();
			m_btnHardware.EnableWindow(bEnable && ((dwPFlags & WK_ALLOW_HARDWARE)>0));
			m_btnInput.EnableWindow(bEnable && ((dwPFlags & WK_ALLOW_INPUT)>0));
			m_btnOutput.EnableWindow(bEnable && ((dwPFlags & WK_ALLOW_OUTPUT)>0));
			m_btnTimer.EnableWindow(bEnable && ((dwPFlags & WK_ALLOW_TIMER)>0));
			m_btnProcess.EnableWindow(bEnable && ((dwPFlags & WK_ALLOW_PROZESS)>0));
			m_btnLink.EnableWindow(bEnable && ((dwPFlags & WK_ALLOW_LINK)>0));
			m_btnUser.EnableWindow(bEnable && ((dwPFlags & WK_ALLOW_USER)>0));
			m_btnPermission.EnableWindow(bEnable && ((dwPFlags & WK_ALLOW_PERMISSION)>0));
			m_btnHost.EnableWindow(bEnable && ((dwPFlags & WK_ALLOW_HOST)>0));
			m_btnArchiv.EnableWindow(bEnable && ((dwPFlags & WK_ALLOW_ARCHIV)>0));
			m_btnBackup.EnableWindow(bEnable && ((dwPFlags & WK_ALLOW_BACKUP)>0));
			m_btnSdiConfig.EnableWindow(bEnable && ((dwPFlags & WK_ALLOW_SDICONFIG)>0));
			m_btnShutDown.EnableWindow(bEnable && ((dwPFlags & WK_ALLOW_SHUTDOWN)>0));
			m_btnDataExport.EnableWindow(bEnable && ((dwPFlags & WK_ALLOW_DATA_EXPORT)>0));
			m_btnSecurity.EnableWindow(bEnable && ((dwPFlags & WK_ALLOW_SECURITY)>0));
			m_btnSettings.EnableWindow(bEnable && ((dwPFlags & WK_ALLOW_SETTINGS)>0));
			m_btnLogView.EnableWindow(bEnable && ((dwPFlags & WK_ALLOW_LOGVIEW)>0));
			m_btnDateTime.EnableWindow(bEnable && ((dwPFlags & WK_ALLOW_DATE_TIME)>0));
			m_btnAlarmOffSpan.EnableWindow(bEnable && ((dwPFlags & WK_ALLOW_ALARM_OFF_SPAN)>0));
			m_btnPTZConfig.EnableWindow(bEnable && ((dwPFlags & WK_ALLOW_PTZ_CONFIG)>0));

			m_Outputs.EnableWindow(bEnable && ((dwPFlags & WK_ALLOW_OUTPUT)>0));
			m_Archivs.EnableWindow(bEnable && ((dwPFlags & WK_ALLOW_OUTPUT)>0));
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPermissionPage::SelectPermission(int i, BOOL bSetSel)
{
	if (i>=0)
	{
		m_bEnableSaveFlag = FALSE;

		m_iSelectedItem = i;
		if (m_pSelectedPermission)
		{
			int i;
			ListToArchivs(m_pSelectedPermission);		
			ListToOutputs(m_pSelectedPermission);		
			CheckBoxToFlags(m_pSelectedPermission);		
			i = m_cbPriority.GetCurSel();
			if (i!=CB_ERR)	
			{
				CString s;
				m_cbPriority.GetLBText(i,s);
				m_pSelectedPermission->SetPriority(_ttoi(s));
			}
			i = m_cbTimer.GetCurSel();
			if (i!=CB_ERR)	
			{
				DWORD dwData = m_cbTimer.GetItemData(i);
				CSecID id(dwData);
				if (id.IsTimerID())
				{
					m_pSelectedPermission->SetTimer(id);
				}
			}
		}
		if (bSetSel)
			m_Permissions.SetItemState(m_iSelectedItem,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
		m_pSelectedPermission = (CPermission*)m_Permissions.GetItemData(m_iSelectedItem);
		ArchivsToList(m_pSelectedPermission);		
		OutputsToList(m_pSelectedPermission);		
		FlagsToCheckBox(m_pSelectedPermission);

		EnableExceptNew();

		{
			CString s;
			s.Format(_T("%d"),m_pSelectedPermission->GetPriority());
			int iSel = m_cbPriority.FindString(0,s);
			m_cbPriority.SetCurSel(iSel);
		}
		{
			for (int i=0;i<m_cbTimer.GetCount();i++)
			{
				DWORD dwData = m_cbTimer.GetItemData(i);
				if (m_pSelectedPermission->GetTimer() == dwData)
				{
					m_cbTimer.SetCurSel(i);
					break;
				}
			}
		}
		m_Name = m_pSelectedPermission->GetName();
		m_sPassword = m_pSelectedPermission->GetPassword();
		m_bEnableSaveFlag = TRUE;
	}
	else
	{
		ArchivsToList(NULL);		
		OutputsToList(NULL);		
		FlagsToCheckBox(NULL);
		m_Name.Empty();
		m_sPassword.Empty();
		m_cbPriority.SetCurSel(-1);
	}
	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
// CPermissionPage message handlers
BOOL CPermissionPage::OnInitDialog() 
{
	CSVPage::OnInitDialog();
	
	m_Outputs.SetStateInfo(theApp.GetStateImage(),1,16);
	m_Archivs.SetStateInfo(theApp.Get3StateImage(),3,16);
	m_Outputs.SetImageList(theApp.GetSmallImage(),LVSIL_SMALL);
	
	CRect	  rect;
	LV_COLUMN lvc;

	m_Permissions.GetWindowRect(&rect);
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.iSubItem = 0;
	lvc.pszText  = NULL;
	lvc.cx       = rect.Width()-4;
	lvc.fmt      = LVCFMT_LEFT;
	m_Permissions.InsertColumn(0,&lvc);

	m_Outputs.GetWindowRect(&rect);
	lvc.cx       = rect.Width()-4;
	m_Outputs.InsertColumn(0,&lvc);

	m_Archivs.GetWindowRect(&rect);
	lvc.cx       = rect.Width()-4;
	m_Archivs.InsertColumn(0,&lvc);

	m_btnBackup.ShowWindow(GetDongle().RunCDRWriter() ? SW_SHOW : SW_HIDE);
	m_btnSdiConfig.ShowWindow(GetDongle().RunSDIConfig() ? SW_SHOW : SW_HIDE);
	BOOL bAlarmOffSpans = GetDongle().AllowAlarmOffSpans();
	m_btnAlarmOffSpan.ShowWindow(bAlarmOffSpans ? SW_SHOW : SW_HIDE);

	Initialize();


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CPermissionPage::Initialize() 
{
	if (theApp.m_pPermission)
	{
		CString s;
		for (int i=theApp.m_pPermission->GetPriority();i<=5;i++)
		{
			s.Format(_T("%d"),i);
			m_cbPriority.AddString(s);
		}
	}

	FillOutputs();
	FillArchivs();
	FillPermissions();
	FillTimer();

	EnableExceptNew();

	int	c = m_Permissions.GetItemCount();
	SelectPermission((c>0) ? 0 : -1);

	m_Permissions.SetFocus();
}
/////////////////////////////////////////////////////////////////////////////
void CPermissionPage::FillPermissions()
{
	ClearPermissions();
	
	int i,c;
	CPermission* pPermission;
	
	c = m_pPermissionArray->GetSize();
	if (c==0)
	{
		ArchivsToList(NULL);
		OutputsToList(NULL);
	}
	else
	{
		pPermission = (CPermission*)m_pPermissionArray->GetAt(0);
		InsertPermission(pPermission,TRUE);
		for (i=1; i<c; i++)
		{
			pPermission = (CPermission*)m_pPermissionArray->GetAt(i);
			InsertPermission(pPermission);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPermissionPage::ClearPermissions()
{
	m_Permissions.DeleteAllItems();
	m_Name.Empty();
	m_iSelectedItem = -1;
	m_pSelectedPermission = NULL;
	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CPermissionPage::FillOutputs()
{
	m_Outputs.DeleteAllItems();
	int iMaxGroups = m_pOutputList->GetSize();
	int iCount;
	for (int iGrpNr=0 ; iGrpNr<iMaxGroups ; iGrpNr++)
	{
		COutputGroup* pGrp = m_pOutputList->GetGroupAt(iGrpNr);
		if (pGrp)
		{
			iCount = pGrp->GetSize();
			for (int j=0 ; j<iCount ; j++)
			{
				const COutput *pOutput = pGrp->GetOutput(j);
				InsertOutput(pOutput);
			}
		}
	}
	if (m_bEnableAudio)
	{
		iMaxGroups = m_pMediaList->GetSize();
		for (iGrpNr=0 ; iGrpNr<iMaxGroups ; iGrpNr++)
		{
			CMediaGroup* pGrp = m_pMediaList->GetGroupAt(iGrpNr);
			if (pGrp)
			{
				iCount = pGrp->GetSize();
				for (int j=0 ; j<iCount ; j++)
				{
					const CMedia *pMedia = pGrp->GetMedia(j);
					InsertMedia(pMedia);
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPermissionPage::FillArchivs()
{
	m_Archivs.DeleteAllItems();

	int i,c;
	CArchivInfo* pArchivInfo;
	
	c = m_pArchivInfoArray->GetSize();
	for (i=0; i<c; i++)
	{
		pArchivInfo = (CArchivInfo*)m_pArchivInfoArray->GetAt(i);
		InsertArchivInfo(pArchivInfo);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPermissionPage::FillTimer()
{

	CString s;
	s.LoadString(IDS_NOTIMER);

	int index = m_cbTimer.AddString(s);
	m_cbTimer.SetItemData(index,SECID_NO_ID);


	for (int i=0;i<m_pTimers->GetSize();i++)
	{
		CSecTimer* pTimer = m_pTimers->GetAtFast(i);
		if (pTimer)
		{
			index = m_cbTimer.AddString(pTimer->GetName());
			m_cbTimer.SetItemData(index,pTimer->GetID().GetID());
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
int CPermissionPage::InsertPermission(CPermission* pPermission, BOOL bSelectIt)
{
	LV_ITEM lvis;
	CString s;
	int i,c;

	s = pPermission->GetName();

	c = m_Permissions.GetItemCount();

	lvis.mask = LVIF_TEXT | LVIF_PARAM;
	lvis.pszText = s.GetBuffer(0);
	lvis.cchTextMax = s.GetLength();
	lvis.lParam = (LPARAM)pPermission;
	lvis.iItem = c;
	lvis.iSubItem = 0;

	i = m_Permissions.InsertItem(&lvis);
	s.ReleaseBuffer();

	if (bSelectIt)
	{
		lvis.iSubItem = 0;
		lvis.mask = LVIF_STATE;
		lvis.state = LVIS_SELECTED;
		lvis.stateMask = LVIS_SELECTED;
		m_Permissions.SetItem(&lvis);
	}
	return i;
}
/////////////////////////////////////////////////////////////////////////////
void CPermissionPage::InsertOutput(const COutput* pOutput)
{
	CString sName = pOutput->GetName();
	if (sName.IsEmpty()) 
	{
		sName.LoadString(IDS_NONAME);
	}
	// OOPS display deactivated outputs or hide them?
	if ( (pOutput->GetExOutputType()!=EX_OUTPUT_OFF) && (sName.GetLength()>0))  // OLD  && pOutput->GetOutputType()!=OUTPUT_OFF)
	{
		LV_ITEM lvis;	
		lvis.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM|LVIF_STATE;
		int iCountOutp = m_Outputs.GetItemCount();
		lvis.iItem = iCountOutp;
		lvis.iSubItem = 0;
		lvis.lParam = pOutput->GetID().GetID(); // OLD j+iGrpNr*O_MAX_OUTPUTS_IN_GRP;
		lvis.pszText = sName.GetBuffer(0);
		lvis.cchTextMax = sName.GetLength();
		lvis.iImage = theApp.ImageByExType(pOutput->GetExOutputType());
		lvis.stateMask=LVIS_STATEIMAGEMASK;
		lvis.state=INDEXTOSTATEIMAGEMASK(1);
		m_Outputs.InsertItem(&lvis);
		sName.ReleaseBuffer();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPermissionPage::InsertMedia(const CMedia* pMedia)
{
	CString sName = pMedia->GetName();
	if (sName.IsEmpty()) 
	{
		sName.LoadString(IDS_NONAME);
	}
	// OOPS display deactivated outputs or hide them?
	if (!pMedia->GetType().IsEmpty())
	{
		LV_ITEM lvis;	
		lvis.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM|LVIF_STATE;
		int iCountOutp = m_Outputs.GetItemCount();
		lvis.iItem = iCountOutp;
		lvis.iSubItem = 0;
		lvis.lParam = pMedia->GetID().GetID(); // OLD j+iGrpNr*O_MAX_OUTPUTS_IN_GRP;
		lvis.pszText = sName.GetBuffer(0);
		lvis.cchTextMax = sName.GetLength();

		lvis.iImage = IMAGE_AUDIO;
		if (pMedia->GetType() == SEC_MEDIA_TYPE_INPUT)
			lvis.iImage = IMAGE_AUDIO_IN;
		else if (pMedia->GetType() == SEC_MEDIA_TYPE_OUTPUT)
			lvis.iImage = IMAGE_AUDIO_OUT;
		
		lvis.stateMask=LVIS_STATEIMAGEMASK;
		lvis.state=INDEXTOSTATEIMAGEMASK(1);
		
		m_Outputs.InsertItem(&lvis);
		sName.ReleaseBuffer();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPermissionPage::InsertArchivInfo(CArchivInfo* pArchivInfo)
{
	LV_ITEM lvis;
	CString s;
	int i,c;

	s = pArchivInfo->GetName();

	c = m_Archivs.GetItemCount();

	lvis.mask = LVIF_TEXT | LVIF_PARAM|LVIF_STATE;
	lvis.pszText = s.GetBuffer(0);
	lvis.cchTextMax = s.GetLength();
	lvis.lParam = (LPARAM)(pArchivInfo->GetID().GetID());
	lvis.iItem = c;
	lvis.iSubItem = 0;
	lvis.stateMask=LVIS_STATEIMAGEMASK;
	lvis.state=INDEXTOSTATEIMAGEMASK(1);

	i = m_Archivs.InsertItem(&lvis);
	s.ReleaseBuffer();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPermissionPage::CanNew()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPermissionPage::StretchElements()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPermissionPage::CanDelete()
{
	if (m_Permissions.GetItemCount()==0)
	{
		return FALSE;
	}
	if (m_pSelectedPermission==NULL)
	{
		return FALSE;
	}

	if (m_pSelectedPermission->IsSuperVisor())
	{
		return FALSE;
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CPermissionPage::OnNew() 
{
	CPermission* pPermission = m_pPermissionArray->AddPermission();
	CString s;
	int i;

	s.LoadString(IDS_NEWPERMISSION);
	pPermission->SetName(s);
	i = InsertPermission(pPermission,TRUE);
	SelectPermission(i);
	SetModified();

	m_editName.SetFocus();
	m_editName.SetSel(0,-1);
}
/////////////////////////////////////////////////////////////////////////////
void CPermissionPage::OutputsToList(CPermission* pPermission)
{
	int i,c;
	CSecID id;
	DWORD dw;

	c = m_Outputs.GetItemCount();
	if (pPermission)
	{
		for (i=0;i<c;i++)
		{
			dw = m_Outputs.GetItemData(i);
			id = dw;
			m_Outputs.SetItemStatus(i,pPermission->IsOutputAllowed(id));
		}
	}
	else
	{
		for (i=0;i<c;i++)
		{
			m_Outputs.SetItemStatus(i,0);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPermissionPage::ListToOutputs(CPermission* pPermission)
{
	int i,c;
	UINT uState;
	DWORD dwID;

	pPermission->DeleteAllOutputs();
	c = m_Outputs.GetItemCount();
	for (i=0;i<c;i++)
	{
		uState = m_Outputs.GetItemStatus(i);
		if (uState)
		{
			dwID = m_Outputs.GetItemData(i);
			pPermission->AddOutput(dwID);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPermissionPage::ArchivsToList(CPermission* pPermission)
{
	int i,c;
	CSecID id;
	DWORD dw;

	c = m_Archivs.GetItemCount();
	if (pPermission)
	{
		for (i=0;i<c;i++)
		{
			dw = m_Archivs.GetItemData(i);
			id = dw;
			m_Archivs.SetItemStatus(i,pPermission->GetArchivFlags(id));
		}
	}
	else
	{
		for (i=0;i<c;i++)
		{
			m_Archivs.SetItemStatus(i,0);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPermissionPage::ListToArchivs(CPermission* pPermission)
{
	int i,c;
	UINT uState;
	DWORD dwID;

	pPermission->DeleteAllArchivs();
	c = m_Archivs.GetItemCount();
	for (i=0;i<c;i++)
	{
		uState = m_Archivs.GetItemStatus(i);
		if (uState)
		{
			dwID = m_Archivs.GetItemData(i);
			pPermission->AddArchiv(dwID,uState);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPermissionPage::FlagsToCheckBox(CPermission* pPermission)
{
	DWORD dw = 0L;
	if (pPermission)
	{
		dw = pPermission->GetFlags();
	}

	m_btnHardware.SetCheck((dw & WK_ALLOW_HARDWARE)>0);
	m_btnInput.SetCheck((dw & WK_ALLOW_INPUT)>0);
	m_btnOutput.SetCheck((dw & WK_ALLOW_OUTPUT)>0);
	m_btnTimer.SetCheck((dw & WK_ALLOW_TIMER)>0);
	m_btnProcess.SetCheck((dw & WK_ALLOW_PROZESS)>0);
	m_btnLink.SetCheck((dw & WK_ALLOW_LINK)>0);
	m_btnUser.SetCheck((dw & WK_ALLOW_USER)>0);
	m_btnPermission.SetCheck((dw & WK_ALLOW_PERMISSION)>0);
	m_btnHost.SetCheck((dw & WK_ALLOW_HOST)>0);
	m_btnArchiv.SetCheck((dw & WK_ALLOW_ARCHIV)>0);
	m_btnBackup.SetCheck((dw & WK_ALLOW_BACKUP)>0);
	m_btnSdiConfig.SetCheck((dw & WK_ALLOW_SDICONFIG)>0);
	m_btnShutDown.SetCheck((dw & WK_ALLOW_SHUTDOWN)>0);
	m_btnDataExport.SetCheck((dw & WK_ALLOW_DATA_EXPORT)>0);
	m_btnSecurity.SetCheck((dw & WK_ALLOW_SECURITY)>0);
	m_btnSettings.SetCheck((dw & WK_ALLOW_SETTINGS)>0);
	m_btnLogView.SetCheck((dw & WK_ALLOW_LOGVIEW)>0);
	m_btnDateTime.SetCheck((dw & WK_ALLOW_DATE_TIME)>0);
	m_btnImageCompare.SetCheck((dw & WK_ALLOW_IMAGECOMPARE)>0);
	m_btnAlarmOffSpan.SetCheck((dw & WK_ALLOW_ALARM_OFF_SPAN)>0);
	m_btnPTZConfig.SetCheck((dw & WK_ALLOW_PTZ_CONFIG)>0);

}
/////////////////////////////////////////////////////////////////////////////
void CPermissionPage::CheckBoxToFlags(CPermission* pPermission)
{
	DWORD dw = 0L;

	if (m_btnHardware.GetCheck())
		dw |= WK_ALLOW_HARDWARE;
	if (m_btnInput.GetCheck())
		dw |= WK_ALLOW_INPUT;
	if (m_btnOutput.GetCheck())
		dw |= WK_ALLOW_OUTPUT;
	if (m_btnTimer.GetCheck())
		dw |= WK_ALLOW_TIMER;
	if (m_btnProcess.GetCheck())
		dw |= WK_ALLOW_PROZESS;
	if (m_btnLink.GetCheck())
		dw |= WK_ALLOW_LINK;
	if (m_btnUser.GetCheck())
		dw |= WK_ALLOW_USER;
	if (m_btnPermission.GetCheck())
		dw |= WK_ALLOW_PERMISSION;
	if (m_btnHost.GetCheck())
		dw |= WK_ALLOW_HOST;
	if (m_btnArchiv.GetCheck())
		dw |= WK_ALLOW_ARCHIV;
	if (m_btnBackup.GetCheck())
		dw |= WK_ALLOW_BACKUP;
	if (m_btnSdiConfig.GetCheck())
		dw |= WK_ALLOW_SDICONFIG;
	if (m_btnShutDown.GetCheck())
		dw |= WK_ALLOW_SHUTDOWN;
	if (m_btnDataExport.GetCheck())
		dw |= WK_ALLOW_DATA_EXPORT;
	if (m_btnSecurity.GetCheck())
		dw |= WK_ALLOW_SECURITY;
	if (m_btnSettings.GetCheck())
		dw |= WK_ALLOW_SETTINGS;
	if (m_btnLogView.GetCheck())
		dw |= WK_ALLOW_LOGVIEW;
	if (m_btnDateTime.GetCheck())
		dw |= WK_ALLOW_DATE_TIME;
	if (m_btnImageCompare.GetCheck())
		dw |= WK_ALLOW_IMAGECOMPARE;
	if (m_btnAlarmOffSpan.GetCheck())
		dw |= WK_ALLOW_ALARM_OFF_SPAN;
	if (m_btnPTZConfig.GetCheck())
		dw |= WK_ALLOW_PTZ_CONFIG;
	pPermission->SetFlags(dw);
}
/////////////////////////////////////////////////////////////////////////////
void CPermissionPage::OnDelete() 
{
	int i,c;
	CUser* pUser;
	BOOL bUser = FALSE;

	if ( (m_iSelectedItem==-1) || (m_pSelectedPermission==NULL))
	{
		return;
	}

	// check if we still have users with this permission
	c = m_pUserArray->GetSize();
	for (i=0;i<c;i++)
	{
		pUser = (CUser*)m_pUserArray->GetAt(i);
		if (pUser->GetPermissionID()==m_pSelectedPermission->GetID())
		{
			// we still hav at least one user with the permission to delete
			bUser = TRUE;
			break;
		}
	}
	if (bUser)
	{
		if (IDOK==AfxMessageBox(IDP_DELETE_USERS_WITH_PERMISSION,MB_OKCANCEL))
		{
			// delete all users with permission to delete
			for (i=c-1;i>=0;i--)
			{
				pUser = (CUser*)m_pUserArray->GetAt(i);
				if (pUser->GetPermissionID()==m_pSelectedPermission->GetID())
				{
					m_pUserArray->DeleteUser(pUser);
				}
			}
#ifdef _UNICODE
			if (m_pParent->GetDocument()->IsLocal())
			{
				m_pUserArray->m_bUseBinaryFormat = TRUE;
			}
			else
			{
				m_pUserArray->m_bUseBinaryFormat = m_pParent->GetDocument()->UseBinaryFormat();
			}
#endif
			m_pUserArray->Save(GetProfile());
		}
		else
		{
			return;
		}
	}

	m_pPermissionArray->DeletePermission(m_pSelectedPermission);
	m_pSelectedPermission = NULL;
	// calc new selection
	int	newCount = m_Permissions.GetItemCount()-1;	// pre-subtract before DeleteItem
	i = m_iSelectedItem;	// new selection

	// range check
	if (i > newCount-1 ) {	// outside ?
		i=newCount-1;
	} else if (newCount==0) {	// last one ?
		i = -1;
	}

	if (m_Permissions.DeleteItem(m_iSelectedItem))	// remove from listCtrl
	{
		SelectPermission(i);
		SetModified();
		EnableExceptNew();
		m_Permissions.SetFocus();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPermissionPage::OnItemchangedListPermission(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (pNMListView->iItem!=m_iSelectedItem)
	{
		if (pNMListView->uNewState & LVIS_SELECTED)
		{
			SelectPermission(pNMListView->iItem,FALSE);
		}
	}
	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CPermissionPage::OnItemChangedStatusList(CStatusList* pStatusList, int nListItem)
{
	if (m_pSelectedPermission==NULL)
	{
		return;
	}

	if (!m_bEnableSaveFlag)
	{
		return;
	}

	if (   (pStatusList == &m_Outputs)
		|| (pStatusList == &m_Archivs)
		)
	{
		SetModified();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPermissionPage::OnStatusChangedStatusList(int nDlgItemID, int nListItem)
{
	if (GetDlgItem(nDlgItemID) == &m_Archivs)
	{
//		TRACE(_T("BLABLA\n"));
		UINT uStatus = m_Archivs.GetItemStatus(nListItem);
		if (uStatus == 2)
		{
			m_Archivs.SetItemStatus(nListItem, 3);
		}
		if (uStatus == 6)
		{
			m_Archivs.SetItemStatus(nListItem, 7);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPermissionPage::OnChangeName() 
{
	UpdateData();

	if (m_pSelectedPermission)
	{

		m_pSelectedPermission->SetName(m_Name);
		LV_ITEM lvis;
		CString s;

		s = m_pSelectedPermission->GetName();
		lvis.mask = LVIF_TEXT;
		lvis.iItem = m_iSelectedItem;
		lvis.iSubItem = 0;
		lvis.pszText = s.GetBuffer(0);
		lvis.cchTextMax = s.GetLength();

		m_Permissions.SetItem(&lvis);
		s.ReleaseBuffer();
	}
	else
	{
		//no Permission selected -> create and select a new one
		CPermission* pPermission = m_pPermissionArray->AddPermission();

		pPermission->SetName(m_Name);
		InsertPermission(pPermission,TRUE);

	}
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CPermissionPage::OnPassword() 
{
	if (!m_sPassword.IsEmpty())
	{
		CPasswordCheckDialog dlg(this);
		if (IDOK==dlg.DoModal())
		{
			if (dlg.m_Password != m_sPassword)
			{	
				AfxMessageBox(IDP_PASSWORD_INCORRECT);
				return;
			}
		}
		else
		{
			return;
		}
	}
	COEMPasswordDialog dlg(this, m_Name, FALSE, FALSE);
	
	if (IDOK==dlg.DoModal())
	{
		m_sPassword = dlg.GetPassword1();
		m_pSelectedPermission->SetPassword(m_sPassword);
		SetModified();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPermissionPage::OnCheck() 
{
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CPermissionPage::CancelChanges()
{
	ClearPermissions();
	m_pPermissionArray->Load(GetProfile());
	FillPermissions();
	int	c = m_Permissions.GetItemCount();
	SelectPermission((c>0) ? 0 : -1);
	EnableExceptNew();
	if (c>=0)
		m_Permissions.SetFocus();
}
/////////////////////////////////////////////////////////////////////////////
void CPermissionPage::SaveChanges()
{
	WK_TRACE_USER(_T("Berechtigungsstufen-Einstellungen wurden geändert\n"));
	if (m_pSelectedPermission)
	{
		ListToArchivs(m_pSelectedPermission);		
		ListToOutputs(m_pSelectedPermission);		
		CheckBoxToFlags(m_pSelectedPermission);		
	}
	if (m_pParent->GetDocument()->IsLocal())
	{
		m_pPermissionArray->m_bUseBinaryFormat = TRUE;
	}
	else
	{
		m_pPermissionArray->m_bUseBinaryFormat = m_pParent->GetDocument()->UseBinaryFormat();
	}
	m_pPermissionArray->Save(GetProfile());
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPermissionPage::IsDataValid()
{
	// we have to check for double permission namess
	// and for permissions with no name
	CString sName1,sName2, sTemp, sMsg;
	int i,j,c;

	c = m_pPermissionArray->GetSize();

	for (i=0;i<c;i++)
	{
		sName1 = m_pPermissionArray->GetAt(i)->GetName();
		// check for empty, 'spaced' or ASCII_255 name
		sTemp = sName1;
		sTemp.TrimLeft();
		sTemp.TrimRight();
		if (sTemp.IsEmpty()) {
			AfxMessageBox(IDP_PERMISSION_NO_NAME, MB_ICONSTOP);
			return FALSE;
		}
		// check for double permission names
		for (j=i+1;j<c;j++)
		{
			sName2 = m_pPermissionArray->GetAt(j)->GetName();
			if (sName1 == sName2)
			{
				sMsg.Format(IDP_DOUBLE_PERMISSION, sName1);
				AfxMessageBox(sMsg,MB_ICONSTOP);
				return FALSE;
			}
		}
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CPermissionPage::OnSelchangeComboPriority() 
{
	if (m_pSelectedPermission)
	{
		int iP = m_cbPriority.GetCurSel();
		if (iP!=CB_ERR)	
		{
			CString s;
			m_cbPriority.GetLBText(iP,s);
			m_pSelectedPermission->SetPriority(_ttoi(s));
		}
		SetModified();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPermissionPage::OnSelchangeComboTimer() 
{
	if (m_pSelectedPermission)
	{
		int iP = m_cbTimer.GetCurSel();
		if (iP!=CB_ERR)	
		{
			DWORD dwData = m_cbTimer.GetItemData(iP);
			CSecID id(dwData);
			if (id.IsTimerID())
			{
				m_pSelectedPermission->SetTimer(id);
			}
		}
		SetModified();
	}
}
