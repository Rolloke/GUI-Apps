#pragma once
#include "resource.h"
#include "afxwin.h"

// CSerialNumberDlg dialog

class CSerialNumberDlg : public CDialog
{
	DECLARE_DYNAMIC(CSerialNumberDlg)

public:
	CSerialNumberDlg(CWnd* pParent = NULL, CString sCurrentSN=_T(""), CString sTemplateSN=_T(""));   // standard constructor
	virtual ~CSerialNumberDlg();

// Dialog Data
	enum { IDD = IDD_SERIAL_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	// Neue Seriennummer
	CString m_sTemplateSN;
	CString m_sOldSN;
	CString	m_sNewSN;

public:
	CString GetNewSN(void);
protected:
	CEdit m_CtrlSN;
public:
//	virtual BOOL OnInitDialog();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
