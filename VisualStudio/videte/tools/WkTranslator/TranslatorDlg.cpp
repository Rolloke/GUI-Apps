// TranslatorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WkTranslator.h"
#include "TranslatorDlg.h"
#include ".\translatordlg.h"


// CTranslatorDlg dialog

IMPLEMENT_DYNAMIC(CTranslatorDlg, CDialog)
CTranslatorDlg::CTranslatorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTranslatorDlg::IDD, pParent)
{
	m_bMBCS = TRUE;
	m_nCodePage = theApp.m_wCodePage;
	m_sTransText= _T("");
	m_sOrgText = _T("");
}

CTranslatorDlg::~CTranslatorDlg()
{
}

void CTranslatorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDT_ORG_TEXT, m_sOrgText);
	DDX_Text(pDX, IDC_EDT_TRANS_TEXT, m_sTransText);
	DDX_Check(pDX, IDC_CK_MBCS, m_bMBCS);
	DDX_Text(pDX, IDC_EDT_CODEPAGE, m_nCodePage);
}


BEGIN_MESSAGE_MAP(CTranslatorDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_CONVERT, OnBnClickedBtnConvert)
	ON_BN_CLICKED(IDC_BTN_CODEPAGE, OnBnClickedBtnCodepage)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_CK_MBCS, OnBnClickedCkMbcs)
END_MESSAGE_MAP()


// CTranslatorDlg message handlers

void CTranslatorDlg::OnBnClickedBtnConvert()
{
	UpdateData();
	if (!m_sOrgText.IsEmpty())
	{
		int i, nLength = m_sOrgText.GetLength();
		if (m_bMBCS)
		{
			char *pszTxt = new char[nLength+1];
			for (i=0; i<nLength; i++)
			{
				pszTxt[i] = (char) m_sOrgText.GetAt(i);
			}
			nLength = MultiByteToWideChar(theApp.m_wCodePage, 0, (char*)pszTxt, nLength, m_sTransText.GetBufferSetLength(nLength), nLength);
			m_sTransText.ReleaseBuffer(nLength);
		}
		else
		{
			int nMaxLen = nLength*2;
			char *pszTxt = new char[nMaxLen];
			nLength = WideCharToMultiByte(theApp.m_wCodePage, 0, m_sOrgText, nLength, pszTxt, nMaxLen, NULL, NULL);
			m_sTransText.GetBufferSetLength(nLength);
			for (i=0; i<nLength; i++)
			{
				m_sTransText.SetAt(i, pszTxt[i]);
			}
			delete pszTxt;
		}
		UpdateData(FALSE);
	}
}

void CTranslatorDlg::OnBnClickedBtnCodepage()
{
	AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_GLOSSARY_SELECT_CODEPAGE, (LPARAM)m_hWnd);
	m_nCodePage = theApp.m_wCodePage;
	UpdateData(FALSE);
}

void CTranslatorDlg::OnBnClickedOk()
{
	OnOK();
}

void CTranslatorDlg::OnBnClickedCkMbcs()
{
	CDataExchange dx(this, TRUE);
	DDX_Check(&dx, IDC_CK_MBCS, m_bMBCS);
}

