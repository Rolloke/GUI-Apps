// ActualImagePage.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "ActualImagePage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CActualImagePage dialog


CActualImagePage::CActualImagePage(CSVView* pParent) : CSVPage(CActualImagePage::IDD)
{
	m_pParent = pParent;
	m_pActualImageProcess = NULL;
	m_pProcessList = pParent->GetDocument()->GetProcesses();

	//{{AFX_DATA_INIT(CActualImagePage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	Create(IDD,(CWnd*)m_pParent);
}


void CActualImagePage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CActualImagePage)
	DDX_Control(pDX, IDC_COMBO_COMP, m_cbCompression);
	DDX_Control(pDX, IDC_COMBO_RES, m_cbResolution);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CActualImagePage, CSVPage)
	//{{AFX_MSG_MAP(CActualImagePage)
	ON_CBN_SELCHANGE(IDC_COMBO_RES, OnSelchangeComboRes)
	ON_CBN_SELCHANGE(IDC_COMBO_COMP, OnSelchangeComboComp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
BOOL CActualImagePage::IsDataValid()
{
	// still always valid
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CActualImagePage::InitProcess()
{
	BOOL bFound = FALSE;
	for (int i=0;i<m_pProcessList->GetSize();i++)
	{
		if (m_pProcessList->GetAt(i)->IsActualImage())
		{
			m_pActualImageProcess = m_pProcessList->GetAt(i);
			bFound = TRUE;
			break;
		}

	}

	if (!bFound)
	{
		m_pActualImageProcess = m_pProcessList->AddProcess();
		CString sName;
		sName.LoadString(IDS_ACTUAL_IMAGE);
		m_pActualImageProcess->SetActualImage(sName,RESOLUTION_2CIF,COMPRESSION_1);
		m_pProcessList->Save(GetProfile());
	}
}
/////////////////////////////////////////////////////////////////////////////
void CActualImagePage::SaveChanges()
{
	WK_TRACE_USER(_T("Istbildprozess-Einstellungen wurden geändert\n"));
	UpdateData();
	ControlToProcess(m_pActualImageProcess);
	m_pProcessList->Save(GetProfile());
}
/////////////////////////////////////////////////////////////////////////////
void CActualImagePage::CancelChanges()
{
	m_pProcessList->Reset();
	m_pProcessList->Load(GetProfile());

	InitProcess();

	ProcessToControl(m_pActualImageProcess);
}
/////////////////////////////////////////////////////////////////////////////
// CActualImagePage message handlers

BOOL CActualImagePage::OnInitDialog() 
{
	CSVPage::OnInitDialog();
	
	CString sFill;

	sFill.LoadString(IDS_FINE);
	m_cbResolution.AddString(sFill);
	sFill.LoadString(IDS_COARSE);
	m_cbResolution.AddString(sFill);

	sFill.LoadString(IDS_COMP1);
	m_cbCompression.AddString(sFill);
	sFill.LoadString(IDS_COMP2);
	m_cbCompression.AddString(sFill);
	sFill.LoadString(IDS_COMP3);
	m_cbCompression.AddString(sFill);
	sFill.LoadString(IDS_COMP4);
	m_cbCompression.AddString(sFill);
	sFill.LoadString(IDS_COMP5);
	m_cbCompression.AddString(sFill);

	CancelChanges();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CActualImagePage::OnSelchangeComboRes() 
{
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CActualImagePage::OnSelchangeComboComp() 
{
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CActualImagePage::ProcessToControl(CProcess* pProcess)
{
	switch (pProcess->GetResolution())
	{
	case RESOLUTION_2CIF:
		m_cbResolution.SetCurSel(0);
		break;
	case RESOLUTION_QCIF:
		m_cbResolution.SetCurSel(1);
		break;
	}

	switch (pProcess->GetCompression())
	{
	case COMPRESSION_1:
		m_cbCompression.SetCurSel(0);
		break;
	case COMPRESSION_2:
		m_cbCompression.SetCurSel(1);
		break;
	case COMPRESSION_3:
		m_cbCompression.SetCurSel(2);
		break;
	case COMPRESSION_4:
		m_cbCompression.SetCurSel(3);
		break;
	case COMPRESSION_5:
		m_cbCompression.SetCurSel(4);
		break;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CActualImagePage::ControlToProcess(CProcess* pProcess)
{
	Resolution res;
	Compression comp;

	int iCurSel;

	iCurSel = m_cbResolution.GetCurSel();

	switch (iCurSel)
	{
	case 0:
		res = RESOLUTION_2CIF;
		break;
	case 1:
		res = RESOLUTION_QCIF;
		break;
	default:
		res = RESOLUTION_2CIF;
		break;
	}

	iCurSel = m_cbCompression.GetCurSel();

	switch (iCurSel)
	{
	case 0:
		comp = COMPRESSION_1;
		break;
	case 1:
		comp = COMPRESSION_2;
		break;
	case 2:
		comp = COMPRESSION_3;
		break;
	case 3:
		comp = COMPRESSION_4;
		break;
	case 4:
		comp = COMPRESSION_5;
		break;
	default:
		comp = COMPRESSION_1;
		break;
	}

	m_pActualImageProcess->SetActualImage(m_pActualImageProcess->GetName(),res,comp);
}
