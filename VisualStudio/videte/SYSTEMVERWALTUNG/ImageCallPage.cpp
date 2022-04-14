// ImageCallPage.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "ImageCallPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImageCallPage dialog

CImageCallPage::CImageCallPage(CSVView* pParent) : CSVPage(CImageCallPage::IDD)
{
	m_pParent = pParent;
	m_pProcessList = pParent->GetDocument()->GetProcesses();
	m_pPermissionArray = pParent->GetDocument()->GetPermissions();
	m_pImageCallProcess = NULL;
	//{{AFX_DATA_INIT(CImageCallPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	Create(IDD,(CWnd*)m_pParent);
}


void CImageCallPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImageCallPage)
	DDX_Control(pDX, IDC_COMBO_PERMISSION_PROCESS, m_cbPermission);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CImageCallPage, CSVPage)
	//{{AFX_MSG_MAP(CImageCallPage)
	ON_CBN_SELCHANGE(IDC_COMBO_PERMISSION_PROCESS, OnSelchangeComboPermission)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImageCallPage message handlers
BOOL CImageCallPage::IsDataValid()
{
	// still always valid
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CImageCallPage::InitProcess()
{
	BOOL bFound = FALSE;
	for (int i=0;i<m_pProcessList->GetSize();i++)
	{
		if (m_pProcessList->GetAt(i)->IsImageCall())
		{
			m_pImageCallProcess = m_pProcessList->GetAt(i);
			bFound = TRUE;
			break;
		}

	}

	if (!bFound)
	{
		m_pImageCallProcess = m_pProcessList->AddProcess();
		CString sName;
		sName.LoadString(IDS_IMAGE_CALL);
		m_pImageCallProcess->SetImageCall(sName,SECID_SUPERVISOR_PERMISSION);
		m_pProcessList->Save(GetProfile());
	}
}
/////////////////////////////////////////////////////////////////////////////
void CImageCallPage::SaveChanges()
{
	WK_TRACE_USER(_T("Referenzbildprozess-Einstellungen wurden geändert\n"));
	UpdateData();
	ControlToProcess(m_pImageCallProcess);
	m_pProcessList->Save(GetProfile());
}
/////////////////////////////////////////////////////////////////////////////
void CImageCallPage::CancelChanges()
{
	m_pProcessList->Reset();
	m_pProcessList->Load(GetProfile());

	InitProcess();

	ProcessToControl(m_pImageCallProcess);
}
/////////////////////////////////////////////////////////////////////////////
void CImageCallPage::ProcessToControl(CProcess* pProcess)
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
}
/////////////////////////////////////////////////////////////////////////////
void CImageCallPage::ControlToProcess(CProcess* pProcess)
{
	int i = m_cbPermission.GetCurSel();
	if (i!=CB_ERR)
	{
		DWORD dw = m_cbPermission.GetItemData(i);
		m_pImageCallProcess->SetImageCall(m_pImageCallProcess->GetName(),CSecID(dw));
	}
}
/////////////////////////////////////////////////////////////////////////////
void CImageCallPage::FillPermissions()
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
BOOL CImageCallPage::OnInitDialog() 
{
	CSVPage::OnInitDialog();

	FillPermissions();
	CancelChanges();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CImageCallPage::OnSelchangeComboPermission() 
{
	SetModified();
}
