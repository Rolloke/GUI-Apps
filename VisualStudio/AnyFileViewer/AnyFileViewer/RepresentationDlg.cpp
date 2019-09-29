// RepresentationDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "AnyFileViewer.h"
#include "RepresentationDlg.h"
#include "afxdialogex.h"


// CRepresentationDlg-Dialogfeld

IMPLEMENT_DYNAMIC(CRepresentationDlg, CDialogEx)

CRepresentationDlg::CRepresentationDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(IDD_REPRESENTATION, pParent)
    , mDoubleFormat(_T(""))
    , mFloatFormat(_T(""))
    , mDescriptionName(FALSE)
    , mDescriptionType(FALSE)
    , mSeparatorComma(FALSE)
    , mSeparatorCR(FALSE)
    , mSeparatorLF(FALSE)
    , mSeparatorSemicolon(FALSE)
    , mSeparatorTAB(FALSE)
    , mSeparatorExtra(_T(""))
{
}

CRepresentationDlg::~CRepresentationDlg()
{
}

void CRepresentationDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDT_DOUBLE_FORMAT, mDoubleFormat);
    DDX_Text(pDX, IDC_EDT_FLOAT_FORMAT, mFloatFormat);
    DDX_Control(pDX, IDC_COMBO_LOCALE, mComboLocale);
    DDX_Check(pDX, IDC_CK_DESC_NAME, mDescriptionName);
    DDX_Check(pDX, IDC_CK_DESC_TYPE, mDescriptionType);
    DDX_Check(pDX, IDC_CK_SEP_COMMA, mSeparatorComma);
    DDX_Check(pDX, IDC_CK_SEP_CR, mSeparatorCR);
    DDX_Check(pDX, IDC_CK_SEP_LF, mSeparatorLF);
    DDX_Check(pDX, IDC_CK_SEP_SEMICOLON, mSeparatorSemicolon);
    DDX_Check(pDX, IDC_CK_SEP_TAB, mSeparatorTAB);
    DDX_Text(pDX, IDC_EDT_SEP_EXTRA, mSeparatorExtra);
}


BEGIN_MESSAGE_MAP(CRepresentationDlg, CDialogEx)
    ON_BN_CLICKED(IDOK, &CRepresentationDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CRepresentationDlg-Meldungshandler


void CRepresentationDlg::OnBnClickedOk()
{
    mComboLocale.GetWindowText(mLocale);
    CDialogEx::OnOK();
}

BOOL CALLBACK CRepresentationDlg::enumLocale(LPTSTR aName, DWORD /* aParam */, LPARAM aPointer)
{
    CRepresentationDlg* pThis = reinterpret_cast<CRepresentationDlg*>(aPointer);
    if (_tcsstr(aName, _T("-")))
    {
        pThis->mComboLocale.AddString(aName);
    }
    return TRUE;
}

BOOL CRepresentationDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    CString str;
    EnumSystemLocalesEx((LOCALE_ENUMPROCEX)CRepresentationDlg::enumLocale, LCID_INSTALLED | LCID_SUPPORTED, reinterpret_cast<LPARAM>(this), NULL);
    int fLocaleIndex = mComboLocale.FindString(0, mLocale);
    if (fLocaleIndex != CB_ERR)
    {
        mComboLocale.SetCurSel(fLocaleIndex);
    }
    return TRUE;  // return TRUE unless you set the focus to a control
                  // AUSNAHME: OCX-Eigenschaftenseite muss FALSE zurückgeben.
}
