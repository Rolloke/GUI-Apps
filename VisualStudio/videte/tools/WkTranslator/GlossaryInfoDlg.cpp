// GlossaryInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WkTranslator.h"
#include "GlossaryInfoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CWkTranslatorApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CGlossaryInfoDlg dialog
CGlossaryInfoDlg::CGlossaryInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGlossaryInfoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGlossaryInfoDlg)
	m_sFormat     = _T("");
	m_bReplace    = TRUE;
	m_bStop       = TRUE;
	//}}AFX_DATA_INIT
	m_bCancel = FALSE;
	m_bStep   = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CGlossaryInfoDlg::SetGlossaryCount(int iCount)
{
	m_sFormat.Format(_T("%i"), iCount);
	m_txtGlossaryNo.SetWindowText(m_sFormat);
//	m_txtGlossaryNo.UpdateWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CGlossaryInfoDlg::SetTextCount(int iCount)
{
	m_sFormat.Format(_T("%i"), iCount);
	m_txtTextNo.SetWindowText(m_sFormat);
}


/////////////////////////////////////////////////////////////////////////////
void CGlossaryInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGlossaryInfoDlg)
	DDX_Control(pDX, IDC_TXT_TRANSLATE_NR, m_cTranslate);
	DDX_Control(pDX, IDC_TXT_FILENAME, m_cFilename);
	DDX_Control(pDX, IDC_TXT_STRINGS_NR, m_txtTextNo);
	DDX_Control(pDX, IDC_TXT_GLOSSARY_NR, m_txtGlossaryNo);
	DDX_Control(pDX, IDC_TXT_TRANSLATION, m_cTranslatedText);
	DDX_Control(pDX, IDC_TXT_ORIGINAL, m_cOriginalText);
	DDX_Check(pDX, IDC_CK_REPLACE, m_bReplace);
	//}}AFX_DATA_MAP

	DDX_Text(pDX, IDC_TXT_GLOSSARY_NR, m_sFormat);
	DDX_Text(pDX, IDC_TXT_STRINGS_NR, m_sFormat);
	DDX_Text(pDX, IDC_TXT_TRANSLATE_NR, m_sFormat);
	DDX_Check(pDX, IDC_CK_STOP, (int&)m_bStop);
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CGlossaryInfoDlg, CDialog)
	//{{AFX_MSG_MAP(CGlossaryInfoDlg)
	ON_BN_CLICKED(IDC_CK_STOP, OnCkStop)
	ON_BN_CLICKED(IDC_CK_REPLACE, OnCkReplace)
	ON_BN_CLICKED(IDC_STEP, OnStep)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CGlossaryInfoDlg message handlers

BOOL CGlossaryInfoDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_bStop       = TRUE;
	
	// TODO: Add extra initialization here
	CenterWindow();

	theApp.SetFontForChildren(m_hWnd);
	SetBtnText();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGlossaryInfoDlg::Create()
{
	CDialog::Create(IDD, AfxGetMainWnd());
}

void CGlossaryInfoDlg::OnCancel() 
{
	m_bCancel = TRUE;	
	m_bStop   = FALSE;
}

void CGlossaryInfoDlg::OnCkStop() 
{
	CDataExchange dx(this, TRUE);
	DDX_Check(&dx, IDC_CK_STOP, (int&)m_bStop);
	SetBtnText();
}

void CGlossaryInfoDlg::SetBtnText()
{
	CString str;
	str.LoadString(m_bStop ? IDS_START : IDS_STOP);
	GetDlgItem(IDC_CK_STOP)->SetWindowText(str); 
}

void CGlossaryInfoDlg::OnCkReplace() 
{
	CDataExchange dx(this, TRUE);
	DDX_Check(&dx, IDC_CK_REPLACE, m_bReplace);
}

void CGlossaryInfoDlg::OnStep() 
{
	m_bStep = TRUE;
}

void CGlossaryInfoDlg::SetFileName(LPCTSTR str)
{
	m_cFilename.SetWindowText(str);
}
