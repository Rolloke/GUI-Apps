// GASetupDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GAUnit.h"
#include "GASetupDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// try to find out how many serial coms the PC  has
// use Win95 registry
UINT GetNumberOfComms()
{
	HKEY hK;
	CString strValue;
	char szB[2];
	int i=1;
	DWORD dwType, dwCount;
	CString sC1,sC2;


    if (ERROR_SUCCESS==RegOpenKeyEx(HKEY_LOCAL_MACHINE,"hardware\\devicemap\\serialcomm",
									  0,KEY_QUERY_VALUE,&hK))
		{

		while(1)
			{
			sC1 = "COM";
            sC1 += itoa(i,szB,10);
		    LONG lResult = RegQueryValueEx(hK, (LPCTSTR)sC1, NULL, &dwType,NULL, &dwCount);
		    if (lResult == ERROR_SUCCESS)
		        {
			    ASSERT(dwType == REG_SZ);
			    lResult = RegQueryValueEx(hK, (LPCTSTR)sC1, NULL, &dwType,
				                          (LPBYTE)strValue.GetBuffer(dwCount/sizeof(TCHAR)), &dwCount);
			    strValue.ReleaseBuffer();
				i++;
		        }
			else
			    break;
			}
		RegCloseKey(hK);
		}
	else
		i = 4;

	return i-1;
}

/////////////////////////////////////////////////////////////////////////////
// CGASetupDlg dialog

CGASetupDlg::CGASetupDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGASetupDlg::IDD, pParent)
{
	m_CommNumber = 0;
}

void CGASetupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGASetupDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CGASetupDlg, CDialog)
	//{{AFX_MSG_MAP(CGASetupDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGASetupDlg message handlers

BOOL CGASetupDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	UINT nCommCount = GetNumberOfComms();
	UINT i;
	CString sCom,sFill;
	char	szBuf[2];
	CComboBox* pCB;

	pCB = (CComboBox*)GetDlgItem(IDC_COM);

	sCom.LoadString(IDS_COM);
	
	for (i=0;i<nCommCount;i++)
		{
		sFill = sCom;
		sFill += itoa(i+1,szBuf,10);
		pCB->AddString(sFill);
		}	

	if (m_CommNumber)
		pCB->SetCurSel(m_CommNumber-1);

	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CGASetupDlg::OnOK() 
{
	CComboBox* pCB;
	UINT c;

	pCB = (CComboBox*)GetDlgItem(IDC_COM);
	c = pCB->GetCurSel();

	if (c!=CB_ERR)
		{
		m_CommNumber = c+1;
		CDialog::OnOK();
		}
}
