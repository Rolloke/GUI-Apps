#pragma once

// RemoteTextReceiverPropPage.h : Declaration of the CRemoteTextReceiverPropPage property page class.


// CRemoteTextReceiverPropPage : See RemoteTextReceiverPropPage.cpp for implementation.

class CRemoteTextReceiverPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CRemoteTextReceiverPropPage)
	DECLARE_OLECREATE_EX(CRemoteTextReceiverPropPage)

// Constructor
public:
	CRemoteTextReceiverPropPage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_REMOTETEXTRECEIVER };

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	DECLARE_MESSAGE_MAP()
};

