#pragma once


// CMoveFindDlgBar dialog

class CMoveFindDlgBar : public CDialogBar
{
    DECLARE_DYNAMIC(CMoveFindDlgBar)

public:
    CMoveFindDlgBar(CWnd* pParent = NULL);   // standard constructor
    virtual ~CMoveFindDlgBar();

// Dialog Data
    enum { IDD = IDD_SEARCH_DLG };

protected:

    afx_msg void OnBnClickedBtnGotoPosition();
    afx_msg void OnUpdateDlgCtrl (CCmdUI *pCmdUI);
public:
    afx_msg void OnBnClickedBtnStepBy();
    afx_msg void OnBnClickedBtnReStepBy();
    DECLARE_MESSAGE_MAP()
};
