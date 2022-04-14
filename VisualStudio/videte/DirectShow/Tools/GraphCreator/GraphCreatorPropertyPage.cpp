#include "stdafx.h"
#include "graphcreatorpropertypage.h"
#include "resource.h"

#if GRAPH_CREATOR_PROPERTY_TEST

CFactoryTemplate g_Templates[] = {

    { L"Property Filter"
    , &CLSID_GraphCreatorPropertyFilter
    , 
    , NULL
    , NULL }
  ,
    { L"Property Page"
    , &CLSID_GraphCreatorPropertyPage
    , CGraphCreatorPropertyPage::CreateInstance }

};
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);


CUnknown * WINAPI CGraphCreatorPropertyPage::CreateInstance(LPUNKNOWN lpunk, HRESULT *phr)
{
    CUnknown *punk = new CGraphCreatorPropertyPage(lpunk, phr);
    if (punk == NULL)
    {
        *phr = E_OUTOFMEMORY;
    }

    return punk;
}

CGraphCreatorPropertyPage::CGraphCreatorPropertyPage(LPUNKNOWN lpunk, HRESULT *phr) 
   : CBasePropertyPage("Filter Properties", lpunk,
        IDD_FILTER_PROPERTIES,IDS_FILTER_PROPERTIES)
{
   ASSERT(phr);
   m_fWindowInActive = TRUE;
   m_pFilter = NULL;
   m_pPin = NULL;
}

CGraphCreatorPropertyPage::~CGraphCreatorPropertyPage()
{
   RELEASE_OBJECT(m_pFilter);
   RELEASE_OBJECT(m_pPin);
}

HRESULT CGraphCreatorPropertyPage::OnConnect(IUnknown *pUnknown)
{
    HRESULT hr = pUnknown->QueryInterface(IID_IBaseFilter,(void **)&m_pFilter);
    if (FAILED(hr)) 
    {
        return hr;
    }
    return NOERROR;
}

HRESULT CGraphCreatorPropertyPage::OnDisconnect()
{
   RELEASE_OBJECT(m_pFilter);
   RELEASE_OBJECT(m_pPin);
    return NOERROR;
}

HRESULT CGraphCreatorPropertyPage::OnActivate(void)
{
    // InitPropertiesDialog(m_hwnd);

    m_fWindowInActive = FALSE;
    
    return NOERROR;
}

HRESULT CGraphCreatorPropertyPage::OnDeactivate(void)
{
    m_fWindowInActive = TRUE;
    return NOERROR;
}

HRESULT CGraphCreatorPropertyPage::OnApplyChanges(void)
{
   return NOERROR;
}

BOOL CGraphCreatorPropertyPage::OnReceiveMessage(HWND hwnd
                                      , UINT uMsg
                                      , WPARAM wParam
                                      , LPARAM lParam)
{
    HRESULT hr = 0;

    if(m_fWindowInActive)
        return FALSE;


    return TRUE;
}
#endif