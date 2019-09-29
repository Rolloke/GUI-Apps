#pragma once


// CFormatConvertDlg dialog

class CFormatConvertDlg : public CDialog
{
    DECLARE_DYNAMIC(CFormatConvertDlg)

public:
    CFormatConvertDlg();   // standard constructor
    virtual ~CFormatConvertDlg();

// Dialog Data
    enum { IDD = IDD_FORMAT_CONVERT };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    virtual void OnCancel();


    void DisplayBinary();
    void DisplayHEX();
public:
    void UpdateAll();
protected:

    afx_msg void OnCbnSelchangeComboFormat1();
    afx_msg void OnCbnSelchangeComboFormat2();
    afx_msg void OnEnChangeEditFormat1();
    afx_msg void OnEnChangeEditFormat2();
    afx_msg void OnEnChangeEditFormatHex();
    afx_msg void OnEnChangeEditFormatBinary();
    DECLARE_MESSAGE_MAP()

    int                 m_nSelFmt1;
    int                 m_nSelFmt2;
    BYTE             m_bData[1024];
    int              m_nDataLen;
    BOOL             m_bInFunction;
};
