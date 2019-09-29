#pragma once


// CFormatViewDlgBar dialog

class CFormatViewDlgBar : public CDialogBar
{
    DECLARE_DYNAMIC(CFormatViewDlgBar)

public:
    CFormatViewDlgBar(CWnd* pParent = NULL);   // standard constructor
    virtual ~CFormatViewDlgBar();

// Dialog Data
    enum { IDD = IDD_FORMAT_VIEW };

protected:

    afx_msg void OnBnClickedBtnInsert();
    afx_msg void OnCbnSelchangeComboDataTypeInsert();
    afx_msg void OnUpdateDlgCtrl (CCmdUI *pCmdUI);
    afx_msg void OnUpdateVariableType (CCmdUI *pCmdUI);
    afx_msg void OnUpdateInsertPosition(CCmdUI *pCmdUI);
    afx_msg void OnUpdateBtnInsert (CCmdUI *pCmdUI);
    DECLARE_MESSAGE_MAP()
public:
};
