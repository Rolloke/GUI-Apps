// RemoteCurveControlPropPage.cpp : Implementation of the CRemoteCurveControlPropPage property page class.

#include "stdafx.h"
#include "RemoteCurveControl.h"
#include "RemoteCurveControlPropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CRemoteCurveControlPropPage, COlePropertyPage)



// Message map

BEGIN_MESSAGE_MAP(CRemoteCurveControlPropPage, COlePropertyPage)
END_MESSAGE_MAP()



// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CRemoteCurveControlPropPage, "REMOTECURVECON.RemoteCurveConPropPage.1",
    0xe0fc5c76, 0xedc7, 0x4847, 0xbf, 0x7f, 0x30, 0x7f, 0xe2, 0xa9, 0x17, 0x98)



// CRemoteCurveControlPropPage::CRemoteCurveControlPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CRemoteCurveControlPropPage

BOOL CRemoteCurveControlPropPage::CRemoteCurveControlPropPageFactory::UpdateRegistry(BOOL bRegister)
{
    if (bRegister)
        return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
            m_clsid, IDS_REMOTECURVECONTROL_PPG);
    else
        return AfxOleUnregisterClass(m_clsid, NULL);
}



// CRemoteCurveControlPropPage::CRemoteCurveControlPropPage - Constructor

CRemoteCurveControlPropPage::CRemoteCurveControlPropPage() :
    COlePropertyPage(IDD, IDS_REMOTECURVECONTROL_PPG_CAPTION)
{
}



// CRemoteCurveControlPropPage::DoDataExchange - Moves data between page and properties

void CRemoteCurveControlPropPage::DoDataExchange(CDataExchange* pDX)
{
    DDP_PostProcessing(pDX);
}



// CRemoteCurveControlPropPage message handlers
