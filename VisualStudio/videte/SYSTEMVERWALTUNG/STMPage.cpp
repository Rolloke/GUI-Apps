// STMPage.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "STMPage.h"

#include "SVDoc.h"
#include "SVView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static TCHAR BASED_CODE szMiCoGeneral[] = _T("MicoUnitPCI\\Device1\\General");

/////////////////////////////////////////////////////////////////////////////
// CSTMPage dialog


CSTMPage::CSTMPage(CSVView* pParent) : CSVPage(CSTMPage::IDD)
{
	m_pParent = pParent;
	m_pOutputList	= pParent->GetDocument()->GetOutputs();
	//{{AFX_DATA_INIT(CSTMPage)
	m_bSTM = FALSE;
	//}}AFX_DATA_INIT
	Create(IDD,pParent);
}


void CSTMPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSTMPage)
	DDX_Check(pDX, IDC_CHECK_DISTURB, m_bSTM);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSTMPage, CSVPage)
	//{{AFX_MSG_MAP(CSTMPage)
	ON_BN_CLICKED(IDC_CHECK_DISTURB, OnCheckDisturb)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSTMPage message handlers
BOOL CSTMPage::IsDataValid()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CSTMPage::SaveChanges()
{
	WK_TRACE_USER(_T("Störungsmelder-Einstellungen wurden geändert\n"));
	UpdateData();

	CString sDisturb;
	sDisturb.Format(_T("%d"),m_bSTM);

	COutputGroup* pOutputGroup = NULL;
	COutput* pOutput	= NULL;

	pOutputGroup = m_pOutputList->GetGroupBySMName(SM_COCO_OUTPUT_RELAY0);
	if ( pOutputGroup ) 
	{
		pOutput = pOutputGroup->GetOutput(3);
		if (pOutput)
		{
			pOutput->SetSTM(m_bSTM);
		}
		WritePrivateProfileString(_T("COCOISA"),_T("IgnoreRelais4"),(LPCTSTR)sDisturb,
			m_pParent->GetDocument()->GetCoCoIni());
	}
	else
	{
		pOutputGroup = m_pOutputList->GetGroupBySMName(SM_MICO_OUTPUT_RELAYS);
		if ( pOutputGroup ) 
		{
			pOutput = pOutputGroup->GetOutput(3);
			if (pOutput)
			{
				pOutput->SetSTM(m_bSTM);
			}
			GetProfile().WriteInt(szMiCoGeneral, _T("IgnoreRelais4"), m_bSTM);
		}
	}

	GetProfile().WriteInt(theApp.GetLauncherSection(), _T("UseRelais4"),m_bSTM);
	
	m_pOutputList->Save(GetProfile(),IsLocal());
}

/////////////////////////////////////////////////////////////////////////////
void CSTMPage::CancelChanges()
{
	m_bSTM = GetProfile().GetInt(theApp.GetLauncherSection(), _T("UseRelais4"),FALSE);
	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSTMPage::OnInitDialog() 
{
	CSVPage::OnInitDialog();

	CancelChanges();
	
	if (!IsLocal())
	{
		DisableAll();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CSTMPage::OnCheckDisturb() 
{
	SetModified();
}
