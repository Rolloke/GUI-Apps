#pragma once

// RemoteCurveControlPropPage.h : Declaration of the CRemoteCurveControlPropPage property page class.


// CRemoteCurveControlPropPage : See RemoteCurveControlPropPage.cpp for implementation.

class CRemoteCurveControlPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CRemoteCurveControlPropPage)
	DECLARE_OLECREATE_EX(CRemoteCurveControlPropPage)

// Constructor
public:
	CRemoteCurveControlPropPage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_REMOTECURVECONTROL };

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	DECLARE_MESSAGE_MAP()
};

