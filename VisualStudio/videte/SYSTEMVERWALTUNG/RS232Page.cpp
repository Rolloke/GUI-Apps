/* GlobalReplace: WK_WriteProfileInt --> GetProfile().WriteInt */
/* GlobalReplace: GetProfile().WriteInt --> GetProfile().WriteInt */
/* GlobalReplace: WK_GetProfileInt --> GetProfile().GetInt */
// RS232Page.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "RS232Page.h"

#include "SVDoc.h"
#include "SVView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRS232Page dialog
#define DEVICE_IDENTIFIER _T("RS 232 Transparent")

CRS232Page::CRS232Page(CSVView* pParent) : CSVPage(CRS232Page::IDD)
{
	m_pParent = pParent;
	m_pOutputList	= pParent->GetDocument()->GetOutputs();
	//{{AFX_DATA_INIT(CRS232Page)
	m_bRS232 = FALSE;
	//}}AFX_DATA_INIT
	Create(IDD,(CWnd*)pParent);
}



void CRS232Page::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRS232Page)
	DDX_Control(pDX, IDC_COMBO_RS232, m_cbRS232);
	DDX_Control(pDX, IDC_CHECK_RS232, m_checkRS232);
	DDX_Check(pDX, IDC_CHECK_RS232, m_bRS232);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRS232Page, CSVPage)
	//{{AFX_MSG_MAP(CRS232Page)
	ON_BN_CLICKED(IDC_CHECK_RS232, OnCheckRs232)
	ON_CBN_SELCHANGE(IDC_COMBO_RS232, OnSelchangeComboRs232)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRS232Page message handlers
BOOL CRS232Page::OnInitDialog() 
{
	CSVPage::OnInitDialog();

	int i;
	DWORD dwComMask;
	DWORD dwBit = 1;
	CString sFill;

	CString sDevice;
	BOOL bCanBeUsed = TRUE;

	dwComMask = m_pParent->GetDocument()->GetCommPortInfo();

	for (dwBit = 1,i=1; dwBit && dwBit<=dwComMask; dwBit<<=1,i++)
	{
		if (dwComMask & dwBit)
		{
			bCanBeUsed = TRUE;
			sDevice = IsCommPortUsed(GetProfile(), i);
			if (!sDevice.IsEmpty() && sDevice != DEVICE_IDENTIFIER)
			{
				bCanBeUsed = FALSE;
			}

			// Wenn Com nicht belegt, dann in Listbox eintragen
			if (bCanBeUsed) 
			{
				sFill.Format(_T("COM%d"),i);
				m_cbRS232.AddString(sFill);
			}
		}
	}

	CancelChanges();

	EnableDlgControls();

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRS232Page::IsDataValid()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CRS232Page::SaveChanges()
{
	WK_TRACE_USER(_T("RS232-Einstellungen wurden geändert\n"));
	int ii;
	BOOL bCommUnit = FALSE;

	SetCommPortUsed(GetProfile(), REMOVE_DEVICE, DEVICE_IDENTIFIER);

	if (m_bRS232==FALSE)
	{
		GetProfile().WriteInt(WK_APP_NAME_COMMUNIT,_T("COM"),0);
	}
	else
	{
		ii = m_cbRS232.GetCurSel();
		if (ii!=CB_ERR)
		{
			CString sCom;
			int iCom;
			m_cbRS232.GetLBText(ii,sCom);
			_stscanf(sCom,_T("COM%d"),&iCom);
			GetProfile().WriteInt(WK_APP_NAME_COMMUNIT,_T("COM"),iCom);
			SetCommPortUsed(GetProfile(), iCom, DEVICE_IDENTIFIER);
		}
		bCommUnit = TRUE;
	}

	COutputGroup* pOutputGroup;
	const COutput* pOutput;
	int i,j,d,c;

	c = m_pOutputList->GetSize();
	for (i=0;i<c;i++)
	{
		pOutputGroup = m_pOutputList->GetGroupAt(i);
		d = pOutputGroup->GetSize();
		for (j=0;j<d;j++)
		{
			pOutput = pOutputGroup->GetOutput(j);
			if (pOutput)
			{
				if (pOutput->GetSN())
				{
					bCommUnit = TRUE;
				}
			}
		}
	}

	GetProfile().WriteInt(WK_APP_NAME_COMMUNIT,_T("Enabled"),bCommUnit);
	// don't delete the key
	GetProfile().WriteString(theApp.GetModuleSection(),WK_APP_NAME_COMMUNIT,bCommUnit ? _T("CommUnit.exe") : _T(""));
}
/////////////////////////////////////////////////////////////////////////////
void CRS232Page::CancelChanges()
{
	m_bRS232 = (GetProfile().GetInt(_T("CommUnit"),_T("COM"),0) > 0) ? TRUE : FALSE;

	UpdateData(FALSE);
	if (m_bRS232)
	{
		int i,iSel;
		CString sCom;
		i = GetProfile().GetInt(_T("CommUnit"),_T("COM"),0);
		sCom.Format(_T("COM%d"),i);
		iSel = m_cbRS232.FindStringExact(0,sCom);
		m_cbRS232.SetCurSel(iSel);
	}
	else
	{
		m_cbRS232.SetCurSel(-1);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRS232Page::EnableDlgControls()
{
	m_cbRS232.EnableWindow(m_bRS232);
}
/////////////////////////////////////////////////////////////////////////////
void CRS232Page::OnCheckRs232() 
{
	UpdateData();
	SetModified();
	m_cbRS232.SetCurSel(-1);
	m_cbRS232.EnableWindow(m_bRS232);
}
/////////////////////////////////////////////////////////////////////////////
void CRS232Page::OnSelchangeComboRs232() 
{
	SetModified();
}
