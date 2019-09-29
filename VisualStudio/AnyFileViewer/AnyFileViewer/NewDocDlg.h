#pragma once


// CNewDocDlg dialog

class CNewDocDlg : public CDialog
{
    DECLARE_DYNAMIC(CNewDocDlg)

public:
    CNewDocDlg(CWnd* pParent = NULL);   // standard constructor
    virtual ~CNewDocDlg();

// Dialog Data
    enum { IDD = IDD_NEW_DOC };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnBnClickedOk();
   afx_msg void OnBnClickedCancel();
   UINT m_nNewDocSize;
};
