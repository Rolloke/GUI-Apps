#pragma once


// CMainDlgBar dialog

class CMainDlgBar : public CDialogBar
{
    DECLARE_DYNAMIC(CMainDlgBar)

public:
    CMainDlgBar(CWnd* pParent = NULL);   // standard constructor
    virtual ~CMainDlgBar();

// Dialog Data
    enum { IDD = IDD_MAINFRAME };

protected:
    afx_msg void OnCbnSelchangeComboDataType();
    afx_msg void OnEnChangeEdtColumns();
    afx_msg void OnEnChangeEdtOffset();
    afx_msg void OnUpdateDlgCtrl (CCmdUI *pCmdUI);
    afx_msg void OnCbnSelchangeComboFormatfiles();
    DECLARE_MESSAGE_MAP()
public:
};
