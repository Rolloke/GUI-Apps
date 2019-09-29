#pragma once


// CSearchAddressInModule dialog

class CSearchAddressInModule : public CDialog
{
	DECLARE_DYNAMIC(CSearchAddressInModule)

public:
	CSearchAddressInModule(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSearchAddressInModule();

// Dialog Data
	enum { IDD = IDD_SEARCH_ADR_IN_MODULE };

// virtual
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// implementation
	void FormatOutputData(FindSymbols&fs);
	void ReadInputData(FindSymbols&fs);

// Message Map
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedClearParams();
	afx_msg void OnBnClickedBtnBrowseModuleName();
	afx_msg void OnBnClickedCkShowDecimal();
	afx_msg void OnDeltaposSpinAddress(NMHDR *pNMHDR, LRESULT *pResult);
	DECLARE_MESSAGE_MAP()

private:
	CString m_sAddress;
	CString m_sOffset;
	CString m_sFunctionName;
	BOOL m_bShowDecimal;
	CString m_sSymbolSize;
public:
	CString m_sModuleName;
	CString m_sBaseAddress;
	CString m_sModuleSize;
	afx_msg void OnBnClickedCalcAddress();
};
