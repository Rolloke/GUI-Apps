// EditColumnTexts.cpp : implementation file
//

#include "stdafx.h"
#include "MSDNEdit.h"
#include "EditColumnTexts.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditColumnTexts dialog


CEditColumnTexts::CEditColumnTexts(CWnd* pParent /*=NULL*/)
	: CDialog(CEditColumnTexts::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditColumnTexts)
	m_strEditText = _T("");
	m_strEditWhat = _T("");
	m_nNumberOfItems = 0;
	m_bDeleteText = FALSE;
	//}}AFX_DATA_INIT
   m_bNumber  = false;
   m_bVersion = false;
}


void CEditColumnTexts::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditColumnTexts)
	DDX_Text(pDX, IDC_EDT_EDIT_TEXT, m_strEditText);
	DDX_Text(pDX, IDC_TXT_NO_OF_SELECTED, m_nNumberOfItems);
	DDX_Check(pDX, IDC_CK_DELETE_TEXT, m_bDeleteText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditColumnTexts, CDialog)
	//{{AFX_MSG_MAP(CEditColumnTexts)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditColumnTexts message handlers

BOOL CEditColumnTexts::OnInitDialog() 
{
	CDialog::OnInitDialog();
   CDataExchange dx(this, true);
	if (m_bNumber)
   {
      GetDlgItem(IDC_EDT_EDIT_TEXT)->ModifyStyle(0, ES_NUMBER);
   }
   if (m_bVersion)
   {
      int nNum;
      DDX_Text(&dx, IDC_EDT_EDIT_TEXT, nNum);
      m_strEditText.Format("%d.%d.%d.%d", (nNum&0xff000000)>>24, (nNum&0x00ff0000)>>16,(nNum&0x0000ff00)>>8,(nNum&0x000000ff));
      dx.m_bSaveAndValidate = false;
      DDX_Text(&dx, IDC_EDT_EDIT_TEXT, m_strEditText);
      dx.m_bSaveAndValidate = true;
   }
   CString str;
   DDX_Text(&dx, IDC_TXT_EDIT_WHAT, str);
   str += m_strEditWhat;
   dx.m_bSaveAndValidate = false;
   DDX_Text(&dx, IDC_TXT_EDIT_WHAT, str);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEditColumnTexts::OnOK() 
{
	CDialog::OnOK();
   if (m_bVersion)
   {
      int n1, n2, n3, n4;
      sscanf(m_strEditText, "%d.%d.%d.%d", &n1, &n2, &n3 ,&n4);
      n1 = n1*0x01000000+n2*0x00010000+n3*0x00000100+n4;
      m_strEditText.Format("%d", n1);
   }
}
