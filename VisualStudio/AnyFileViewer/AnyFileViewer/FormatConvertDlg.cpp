// FormatConvertDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AnyFileViewer.h"
#include "FormatConvertDlg.h"
#include "DisplayType.h"


// CFormatConvertDlg dialog

IMPLEMENT_DYNAMIC(CFormatConvertDlg, CDialog)
CFormatConvertDlg::CFormatConvertDlg()
    : CDialog(CFormatConvertDlg::IDD, NULL)
{
    m_nSelFmt1 = 0;
    m_nSelFmt2 = 0;
    m_bInFunction = FALSE;
    m_nDataLen = 0;
    memset(m_bData, 0, sizeof(m_bData));
}

CFormatConvertDlg::~CFormatConvertDlg()
{
}

void CFormatConvertDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFormatConvertDlg, CDialog)
    ON_CBN_SELCHANGE(IDC_COMBO_FORMAT1, OnCbnSelchangeComboFormat1)
    ON_CBN_SELCHANGE(IDC_COMBO_FORMAT2, OnCbnSelchangeComboFormat2)
    ON_EN_CHANGE(IDC_EDIT_FORMAT1, OnEnChangeEditFormat1)
    ON_EN_CHANGE(IDC_EDIT_FORMAT2, OnEnChangeEditFormat2)
    ON_EN_CHANGE(IDC_EDIT_FORMAT_HEX, OnEnChangeEditFormatHex)
    ON_EN_CHANGE(IDC_EDIT_FORMAT_BINARY, OnEnChangeEditFormatBinary)
END_MESSAGE_MAP()

// CFormatConvertDlg message handlers

BOOL CFormatConvertDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    int i;
    for (i=0; i<theApp.GetNoOfDataTypes(); i++)
    {
        if (theApp.GetDisplayType(i).isSizeFixed())
        {
            SendDlgItemMessage(IDC_COMBO_FORMAT1, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR)theApp.GetDisplayType(i).Type());
            SendDlgItemMessage(IDC_COMBO_FORMAT2, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR)theApp.GetDisplayType(i).Type());
        }
    }
    SendDlgItemMessage(IDC_COMBO_FORMAT1, CB_SETCURSEL, 0, m_nSelFmt1);
    SendDlgItemMessage(IDC_COMBO_FORMAT2, CB_SETCURSEL, 0, m_nSelFmt2);
    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CFormatConvertDlg::OnCbnSelchangeComboFormat1()
{
    if (m_bInFunction) return;
    m_bInFunction = TRUE;
    theApp.invertEndianess();
    m_nSelFmt1 = (int)SendDlgItemMessage(IDC_COMBO_FORMAT1, CB_GETCURSEL, 0, 0);
    m_nDataLen = theApp.GetDisplayType(m_nSelFmt1).GetByteLength();
    SetDlgItemText(IDC_EDIT_FORMAT1, theApp.GetDisplayType(m_nSelFmt1).Display(m_bData));
    DisplayBinary();
    DisplayHEX();
    m_nDataLen = theApp.GetDisplayType(m_nSelFmt1).GetByteLength();
    m_bInFunction = FALSE;
    theApp.invertEndianess();
}

void CFormatConvertDlg::OnCbnSelchangeComboFormat2()
{
    if (m_bInFunction) return;
    m_bInFunction = TRUE;
    theApp.invertEndianess();
    m_nSelFmt2 = (int)SendDlgItemMessage(IDC_COMBO_FORMAT2, CB_GETCURSEL, 0, 0);
    m_nDataLen = theApp.GetDisplayType(m_nSelFmt2).GetByteLength();
    SetDlgItemText(IDC_EDIT_FORMAT2, theApp.GetDisplayType(m_nSelFmt2).Display(m_bData));
    DisplayBinary();
    DisplayHEX();
    m_bInFunction = FALSE;
    theApp.invertEndianess();
}

void CFormatConvertDlg::UpdateAll()
{
    SetDlgItemText(IDC_EDIT_FORMAT1, theApp.GetDisplayType(m_nSelFmt1).Display(m_bData));
    DisplayBinary();
    DisplayHEX();
    SetDlgItemText(IDC_EDIT_FORMAT2, theApp.GetDisplayType(m_nSelFmt2).Display(m_bData));
}

void CFormatConvertDlg::OnEnChangeEditFormat1()
{
    if (m_bInFunction) return;
    m_bInFunction = TRUE;
    theApp.invertEndianess();
    CString str;
    GetDlgItemText(IDC_EDIT_FORMAT1, str);
    theApp.GetDisplayType(m_nSelFmt1).Write(m_bData, str);
    SetDlgItemText(IDC_EDIT_FORMAT2, theApp.GetDisplayType(m_nSelFmt2).Display(m_bData));
    DisplayBinary();
    DisplayHEX();
    m_bInFunction = FALSE;
    theApp.invertEndianess();
}

void CFormatConvertDlg::OnEnChangeEditFormat2()
{
    if (m_bInFunction) return;
    m_bInFunction = TRUE;
    theApp.invertEndianess();
    CString str;
    GetDlgItemText(IDC_EDIT_FORMAT2, str);
    theApp.GetDisplayType(m_nSelFmt2).Write(m_bData, str);
    SetDlgItemText(IDC_EDIT_FORMAT1, theApp.GetDisplayType(m_nSelFmt1).Display(m_bData));
    DisplayBinary();
    DisplayHEX();
    m_bInFunction = FALSE;
    theApp.invertEndianess();
}

void CFormatConvertDlg::OnEnChangeEditFormatHex()
{
    if (m_bInFunction) return;
    m_bInFunction = TRUE;
    theApp.invertEndianess();
    CString str;
    int i;
    GetDlgItemText(IDC_EDIT_FORMAT_HEX, str);
    CDisplayHEX2 *pDisplay = (CDisplayHEX2*)&theApp.GetDisplayType(CDisplayType::HEX2);
    for (i=0; i<m_nDataLen; i++)
    {
        pDisplay->Write(&m_bData[i], str.Mid(i*2));
    }

    SetDlgItemText(IDC_EDIT_FORMAT1, theApp.GetDisplayType(m_nSelFmt1).Display(m_bData));
    SetDlgItemText(IDC_EDIT_FORMAT2, theApp.GetDisplayType(m_nSelFmt2).Display(m_bData));
    DisplayBinary();
    m_bInFunction = FALSE;
    theApp.invertEndianess();
}

void CFormatConvertDlg::OnEnChangeEditFormatBinary()
{
    if (m_bInFunction) return;
    m_bInFunction = TRUE;
    theApp.invertEndianess();
    CString str;
    theApp.GetDisplayType(CDisplayType::Binary).SetBytes(m_nDataLen);
    GetDlgItemText(IDC_EDIT_FORMAT_BINARY, str);
    theApp.GetDisplayType(CDisplayType::Binary).Write(m_bData, str);
    SetDlgItemText(IDC_EDIT_FORMAT1, theApp.GetDisplayType(m_nSelFmt1).Display(m_bData));
    SetDlgItemText(IDC_EDIT_FORMAT2, theApp.GetDisplayType(m_nSelFmt2).Display(m_bData));
    DisplayHEX();
    m_bInFunction = FALSE;
    theApp.invertEndianess();
}

void CFormatConvertDlg::DisplayBinary()
{
    CDisplayBinary*pDisplay = (CDisplayBinary*)&theApp.GetDisplayType(CDisplayType::Binary);
    pDisplay->SetBytes(m_nDataLen);
    SetDlgItemText(IDC_EDIT_FORMAT_BINARY, pDisplay->Display(m_bData));
}

void CFormatConvertDlg::DisplayHEX()
{
    CDisplayHEX2 *pDisplay = (CDisplayHEX2*)&theApp.GetDisplayType(CDisplayType::HEX2);
    CString str;
    int i;
    for (i=0; i<m_nDataLen; i++)
    {
        str += pDisplay->Display(&m_bData[i]);
    }
    SetDlgItemText(IDC_EDIT_FORMAT_HEX, str);
}

void CFormatConvertDlg::OnCancel()
{
    DestroyWindow();
}
