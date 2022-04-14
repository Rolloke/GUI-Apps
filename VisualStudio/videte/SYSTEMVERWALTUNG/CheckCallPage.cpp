// CheckCallPage.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "CheckCallPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCheckCallPage dialog

CCheckCallPage::CCheckCallPage(CSVView* pParent) : CSVPage(CCheckCallPage::IDD)
{
	m_pParent = pParent;
	m_pProcessList = pParent->GetDocument()->GetProcesses();
	m_pPermissionArray = pParent->GetDocument()->GetPermissions();
	m_pCheckCallProcess = NULL;
	//{{AFX_DATA_INIT(CCheckCallPage)
	m_bSetTime = FALSE;
	//}}AFX_DATA_INIT
	Create(IDD,(CWnd*)m_pParent);
}


void CCheckCallPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCheckCallPage)
	DDX_Control(pDX, IDC_COMBO_PERMISSION_PROCESS, m_cbPermission);
	DDX_Check(pDX, IDC_CHECK_TIME, m_bSetTime);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCheckCallPage, CSVPage)
	//{{AFX_MSG_MAP(CCheckCallPage)
	ON_CBN_SELCHANGE(IDC_COMBO_PERMISSION_PROCESS, OnSelchangeComboPermission)
	ON_BN_CLICKED(IDC_CHECK_TIME, OnCheckTime)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCheckCallPage message handlers

BOOL CCheckCallPage::OnInitDialog() 
{
	CSVPage::OnInitDialog();
	
	FillPermissions();
	CancelChanges();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCheckCallPage::IsDataValid()
{
	// still always valid
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CCheckCallPage::ProcessToControl(CProcess* pProcess)
{
	int i,c;
	DWORD dw;
	dw = pProcess->GetPermission().GetID();
	m_cbPermission.SetCurSel(-1);
	c = m_cbPermission.GetCount();
	for (i=0;i<c;i++)
	{
		if (m_cbPermission.GetItemData(i)==dw)
		{
			m_cbPermission.SetCurSel(i);
			break;
		}
	}
	m_bSetTime = pProcess->GetSetTime();
	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CCheckCallPage::ControlToProcess(CProcess* pProcess)
{
	UpdateData();
	int i = m_cbPermission.GetCurSel();
	if (i!=CB_ERR)
	{
		DWORD dw = m_cbPermission.GetItemData(i);
		m_pCheckCallProcess->SetCheckCall(m_pCheckCallProcess->GetName(),
										  CSecID(dw),
										  m_bSetTime);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCheckCallPage::FillPermissions()
{
	m_cbPermission.ResetContent();

	int i,j,c;
	CString s;
	DWORD dw;
	CPermission* pPermission;

	c = m_pPermissionArray->GetSize();
	
	for (i=0; i<c; i++)
	{
		pPermission = (CPermission*)m_pPermissionArray->GetAt(i);
		s = pPermission->GetName();
		dw = pPermission->GetID().GetID();
		j = m_cbPermission.AddString(s);
		m_cbPermission.SetItemData(j,dw);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCheckCallPage::SaveChanges()
{
	WK_TRACE_USER(_T("Kontrollanrufprozess-Einstellungen wurden geändert\n"));
	UpdateData();
	ControlToProcess(m_pCheckCallProcess);
	m_pProcessList->Save(GetProfile());
}
/////////////////////////////////////////////////////////////////////////////
void CCheckCallPage::CancelChanges()
{
	m_pProcessList->Reset();
	m_pProcessList->Load(GetProfile());

	InitProcess();

	ProcessToControl(m_pCheckCallProcess);
}
/////////////////////////////////////////////////////////////////////////////
void CCheckCallPage::InitProcess()
{
	BOOL bFound = FALSE;
	for (int i=0;i<m_pProcessList->GetSize();i++)
	{
		if (m_pProcessList->GetAt(i)->IsCheckCall())
		{
			m_pCheckCallProcess = m_pProcessList->GetAt(i);
			bFound = TRUE;
			break;
		}

	}

	if (!bFound)
	{
		m_pCheckCallProcess = m_pProcessList->AddProcess();
		CString sName;
		sName.LoadString(IDS_CHECK_CALL);
		m_pCheckCallProcess->SetCheckCall(sName,
										  SECID_SUPERVISOR_PERMISSION,
										  m_bSetTime);
		m_pProcessList->Save(GetProfile());
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCheckCallPage::OnSelchangeComboPermission() 
{
	SetModified();
}

void CCheckCallPage::OnCheckTime() 
{
	SetModified();
}
