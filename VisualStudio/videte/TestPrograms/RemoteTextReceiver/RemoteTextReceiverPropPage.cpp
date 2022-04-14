// RemoteTextReceiverPropPage.cpp : Implementation of the CRemoteTextReceiverPropPage property page class.

#include "stdafx.h"
#include "RemoteTextReceiver.h"
#include "RemoteTextReceiverPropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CRemoteTextReceiverPropPage, COlePropertyPage)



// Message map

BEGIN_MESSAGE_MAP(CRemoteTextReceiverPropPage, COlePropertyPage)
END_MESSAGE_MAP()



// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CRemoteTextReceiverPropPage, "REMOTETEXTRECE.RemoteTextRecePropPage.1",
	0xc932b919, 0x18a2, 0x4c7e, 0x82, 0x6c, 0xc4, 0x6a, 0xd3, 0xbd, 0x4, 0x3f)



// CRemoteTextReceiverPropPage::CRemoteTextReceiverPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CRemoteTextReceiverPropPage

BOOL CRemoteTextReceiverPropPage::CRemoteTextReceiverPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_REMOTETEXTRECEIVER_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}



// CRemoteTextReceiverPropPage::CRemoteTextReceiverPropPage - Constructor

CRemoteTextReceiverPropPage::CRemoteTextReceiverPropPage() :
	COlePropertyPage(IDD, IDS_REMOTETEXTRECEIVER_PPG_CAPTION)
{
}



// CRemoteTextReceiverPropPage::DoDataExchange - Moves data between page and properties

void CRemoteTextReceiverPropPage::DoDataExchange(CDataExchange* pDX)
{
	DDP_PostProcessing(pDX);
}



// CRemoteTextReceiverPropPage message handlers
