#include "afxwin.h"
#if !defined(AFX_EULALICENSEDLG_H__709FA5A7_1133_11D5_8CDD_004005A1D890__INCLUDED_)
#define AFX_EULALICENSEDLG_H__709FA5A7_1133_11D5_8CDD_004005A1D890__INCLUDED_

#include "WK_Dongle.h"
#include "EulaDongle.h"
#include "wkclasses\rsa.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EULALicenseDlg.h : header file
//

#define SERIAL	_T("Software\\dvrt\\Version")

/////////////////////////////////////////////////////////////////////////////
// CEULALicenseDlg dialog

class CEULALicenseDlg : public CDialog
{
// Construction
public:
	CEULALicenseDlg(CWnd* pParent = NULL);   // standard constructor
	CString GetSerialFromReg(); 
	CString GetExpansionFromReg();
	BOOL IsSerialCorrect(CString sSerial);
	BOOL IsExpansionCorrect(DWORD& dwDongleDecode, CString sLicense, const CString sExpansion);
	void CryptDongle(); //write new features to dongle
	__int64 even(__int64 n);
	__int64 RealIsPrim (__int64 p);

// Dialog Data
	//{{AFX_DATA(CEULALicenseDlg)
	enum { IDD = IDD_LICENSE_DIALOG };
	CButton	m_ctrlOK;
	CEdit	m_ctrlEditLicense;
	CString	m_ValueLicense;
	DWORD	m_dwDongleDecode;
	CEdit m_ctrlEditExpansion;
	CString m_ValueExpansion;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEULALicenseDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	HICON m_hIcon;
	// Generated message map functions
	//{{AFX_MSG(CEULALicenseDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditLicense();
	afx_msg void OnPaint();
	//}}AFX_MSG
	afx_msg LRESULT OnUser(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
	
	BOOL SaveSerialToReg(CString sSerial);
	BOOL SaveExpansionToReg(CString sCode);
	void LoadDongleDCF(CString &sDcf); 
	BOOL DoesFileExist(LPCTSTR szFileOrDirectory);
private:
	CString	m_sCode;
	CString m_sDongleDcf;
	BOOL	m_bDTS;
	int		m_iReceiverType;
public:
	CStatic m_ctrlStaticExpansion;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EULALICENSEDLG_H__709FA5A7_1133_11D5_8CDD_004005A1D890__INCLUDED_)
