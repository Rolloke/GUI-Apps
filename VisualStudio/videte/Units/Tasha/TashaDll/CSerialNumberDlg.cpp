// CSerialNumberDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TashaDll.h"
#include "CSerialNumberDlg.h"
#include ".\cserialnumberdlg.h"


// CSerialNumberDlg dialog

/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CSerialNumberDlg, CDialog)
CSerialNumberDlg::CSerialNumberDlg(CWnd* pParent /*=NULL*/, CString sOldSN, CString sTemplateSN)
	: CDialog(CSerialNumberDlg::IDD, pParent)
{
	m_sTemplateSN	= sTemplateSN;
	m_sOldSN		= sOldSN;
	if (!m_sTemplateSN.IsEmpty())
		m_sNewSN = m_sTemplateSN;
}

/////////////////////////////////////////////////////////////////////////////
CSerialNumberDlg::~CSerialNumberDlg()
{
}

/////////////////////////////////////////////////////////////////////////////
void CSerialNumberDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SN, m_sNewSN);
	DDX_Control(pDX, IDC_EDIT_SN, m_CtrlSN);
	DDV_MaxChars(pDX, m_sNewSN, 17);
	DDX_Text(pDX, IDC_EDIT_SN_OLD, m_sOldSN);
}


/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CSerialNumberDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CSerialNumberDlg message handlers

/////////////////////////////////////////////////////////////////////////////
CString CSerialNumberDlg::GetNewSN(void)
{
	return m_sNewSN;
}


/////////////////////////////////////////////////////////////////////////////
BOOL CSerialNumberDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	UpdateData(FALSE);

	// Focus auf das Editcontrol setzen
	m_CtrlSN.SetFocus();
	if (m_sTemplateSN.GetLength() == 17)
	{
		m_sTemplateSN.MakeLower();

		int nStartIndex = m_sTemplateSN.Find('x');
		int nEndIndex   = m_sTemplateSN.ReverseFind('x');
		if ((nStartIndex != -1) && (nEndIndex != -1) && (nEndIndex >= nStartIndex))
			m_CtrlSN.SetSel(nStartIndex,nEndIndex+1);
	}

	return FALSE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
void CSerialNumberDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	if (m_sNewSN.GetLength() != 17)
	{
		MessageBox(_T("Die Seriennummer muﬂ exakt 17 Zeichen lang sein!"));
		// Focus auf das Editcontrol setzen
		m_CtrlSN.SetFocus();
		return;
	}

	// Die Seriennummer darf nur Ziffern enthalten
	if (m_sNewSN.SpanIncluding(_T("0123456789")) != m_sNewSN)
	{
		MessageBox(_T("Die Seriennummer darf nur Ziffern enthalten!"));
		// Focus auf das Editcontrol setzen
		m_CtrlSN.SetFocus();
		return;
	}

	CDialog::OnOK();
}
