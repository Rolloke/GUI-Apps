// FindAddress.cpp : implementation file
//

#include "stdafx.h"
#include "AnalyseMapFile.h"
#include "FindAddress.h"
#include ".\findaddress.h"


// CFindAddress dialog

IMPLEMENT_DYNAMIC(CFindAddress, CDialog)
CFindAddress::CFindAddress(CWnd* pParent /*=NULL*/)
	: CDialog(CFindAddress::IDD, pParent)
	, m_sSearchString(_T(""))
{
}

CFindAddress::~CFindAddress()
{
}

void CFindAddress::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SEARCH_STRING, m_sSearchString);
}


BEGIN_MESSAGE_MAP(CFindAddress, CDialog)
END_MESSAGE_MAP()


// CFindAddress message handlers

BOOL CFindAddress::OnInitDialog()
{
	if (m_sSearchString.IsEmpty() && m_sTitle.IsEmpty())
	{
		m_sSearchString = _T("0x");
	}

	CDialog::OnInitDialog();

	if (!m_sTitle.IsEmpty())
	{
		SetWindowText(m_sTitle);
	}
	CWnd *pCtrl = GetDlgItem(IDC_EDIT_SEARCH_STRING);
	GotoDlgCtrl(pCtrl);
	pCtrl->SetFocus();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
