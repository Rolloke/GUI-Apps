#pragma once


// CFindAddress dialog

class CFindAddress : public CDialog
{
	DECLARE_DYNAMIC(CFindAddress)

public:
	CFindAddress(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFindAddress();

// Dialog Data
	enum { IDD = IDD_SEARCH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CString m_sSearchString;
	CString m_sTitle;
};
