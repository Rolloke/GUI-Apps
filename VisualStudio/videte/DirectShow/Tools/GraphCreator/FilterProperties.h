#pragma once


// CFilterProperties dialog

class CFilterProperties : public CPropertyPage
{
	DECLARE_DYNAMIC(CFilterProperties)

public:
	CFilterProperties();
	virtual ~CFilterProperties();

// Dialog Data
	enum { IDD = IDD_FILTER_PROPERTIES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
   virtual BOOL OnInitDialog();
};
