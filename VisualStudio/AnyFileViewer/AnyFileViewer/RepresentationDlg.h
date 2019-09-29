#pragma once
#include "afxwin.h"


// CRepresentationDlg-Dialogfeld

class CRepresentationDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CRepresentationDlg)

public:
    CRepresentationDlg(CWnd* pParent = NULL);   // Standardkonstruktor
    virtual ~CRepresentationDlg();

// Dialogfelddaten
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_REPRESENTATION };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
    virtual BOOL OnInitDialog(); 

    afx_msg void OnBnClickedOk();
    DECLARE_MESSAGE_MAP()

private:
    CComboBox mComboLocale;
    static BOOL CALLBACK enumLocale(LPTSTR, DWORD, LPARAM);

public:
    CString mDoubleFormat;
    CString mFloatFormat;
    CString mLocale;
    BOOL mDescriptionName;
    BOOL mDescriptionType;
    BOOL mSeparatorComma;
    BOOL mSeparatorCR;
    BOOL mSeparatorLF;
    BOOL mSeparatorSemicolon;
    BOOL mSeparatorTAB;
    CString mSeparatorExtra;
};
