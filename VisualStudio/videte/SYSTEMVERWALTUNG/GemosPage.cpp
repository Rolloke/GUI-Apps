// GemosPage.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "GemosPage.h"
#include "ComConfigurationDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static TCHAR szSection[] = _T("GemosUnit");


/////////////////////////////////////////////////////////////////////////////
// CGemosPage dialog

CGemosPage::CGemosPage(CSVView* pParent) 
	: CSVPage(CGemosPage::IDD), m_ComParameters(szSection)
{
	m_pParent = pParent;
	m_pInputList	= pParent->GetDocument()->GetInputs();
	//{{AFX_DATA_INIT(CGemosPage)
	m_sComGemos = _T("");
	m_bGemosUnit = FALSE;
	//}}AFX_DATA_INIT
	Create(IDD,m_pParent);
}


void CGemosPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGemosPage)
	DDX_Control(pDX, IDC_COM_PARAMETER, m_btnComParameters);
	DDX_Control(pDX, IDC_COM_GEMOS, m_cbComGemos);
	DDX_CBString(pDX, IDC_COM_GEMOS, m_sComGemos);
	DDX_Check(pDX, IDC_CHECK_GEMOS, m_bGemosUnit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGemosPage, CSVPage)
	//{{AFX_MSG_MAP(CGemosPage)
	ON_BN_CLICKED(IDC_COM_PARAMETER, OnComParameter)
	ON_BN_CLICKED(IDC_CHECK_GEMOS, OnCheckGemos)
	ON_CBN_SELCHANGE(IDC_COM_GEMOS, OnSelchangeComGemos)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGemosPage message handlers
// Overrides
/////////////////////////////////////////////////////////////////////////////
BOOL CGemosPage::IsDataValid()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CGemosPage::SaveChanges()
{
	UpdateData();
	SetCommPortUsed(GetProfile(), REMOVE_DEVICE, szSection);
	m_ComParameters.SaveToRegistry(GetProfile());
	GetProfile().WriteString(theApp.GetModuleSection(),WK_APPNAME_GEMOSUNIT,
		m_bGemosUnit ? _T("GemosUnit.exe") : _T(""));
	if (m_bGemosUnit)
	{
		int iCom=0;
		if (1 == _stscanf(m_sComGemos,_T("COM%d"),&iCom))
		{
			GetProfile().WriteInt(szSection,_T("COM"),iCom);
		}
		SetCommPortUsed(GetProfile(), iCom, szSection);
		m_pParent->AddInitApp(WK_APPNAME_GEMOSUNIT);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CGemosPage::CancelChanges()
{
	m_ComParameters.LoadFromRegistry(GetProfile());
	CString sTemp;
	sTemp = GetProfile().GetString(theApp.GetModuleSection(),WK_APPNAME_GEMOSUNIT,_T(""));
	m_bGemosUnit = sTemp.CompareNoCase(_T("GemosUnit.exe"))==0; 

	int iCom = GetProfile().GetInt(szSection,_T("COM"),0);

	m_sComGemos.Format(_T("COM%d"),iCom);

	UpdateData(FALSE);

	EnableDlgControls();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CGemosPage::OnInitDialog() 
{
	CSVPage::OnInitDialog();
	
	int i;
	DWORD dwComMask;
	DWORD dwBit = 1;
	CString sFill;
	
	dwComMask = m_pParent->GetDocument()->GetCommPortInfo();

	CString sDevice;
	BOOL bCanBeUsed = TRUE;

	for (dwBit = 1,i=1; dwBit && dwBit<=dwComMask; dwBit<<=1,i++)
	{
		if (dwBit & dwComMask)
		{
			bCanBeUsed = TRUE;
			sDevice = IsCommPortUsed(GetProfile(), i);
			if (!sDevice.IsEmpty() && sDevice != szSection)
			{
				bCanBeUsed = FALSE;
			}
			if (bCanBeUsed) 
			{
				sFill.Format(_T("COM%d"),i);
				m_cbComGemos.AddString(sFill);
			}
		}
	}	

	CancelChanges();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CGemosPage::OnComParameter() 
{
	UpdateData();
	CComConfigurationDialog dlg(m_sComGemos,&m_ComParameters,this);
	if (IDOK==dlg.DoModal())
	{
		SetModified();
		m_pParent->AddInitApp(WK_APPNAME_GEMOSUNIT);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CGemosPage::EnableDlgControls()
{
	m_btnComParameters.EnableWindow(m_bGemosUnit);
	m_cbComGemos.EnableWindow(m_bGemosUnit);
}
/////////////////////////////////////////////////////////////////////////////
void CGemosPage::OnCheckGemos() 
{
	UpdateData();
	EnableDlgControls();
	SetModified();
	m_pParent->AddInitApp(WK_APPNAME_GEMOSUNIT);
}
/////////////////////////////////////////////////////////////////////////////
void CGemosPage::OnSelchangeComGemos() 
{
	SetModified();
	m_pParent->AddInitApp(WK_APPNAME_GEMOSUNIT);
}
