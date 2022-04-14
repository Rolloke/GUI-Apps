#pragma once
#include "cprop.h"

#if GRAPH_CREATOR_PROPERTY_TEST

//{2ea99df9-9455-487c-be94-130f043fdf50};
DEFINE_GUID(CLSID_GraphCreatorPropertyPage,
0x2ea99df9, 0x9455, 0x487c, 0xbe, 0x94, 0x13, 0x0f, 0x04, 0x3f, 0xdf, 0x50);

DEFINE_GUID(CLSID_GraphCreatorPropertyFilter,
0x2ea99df9, 0x9455, 0x487c, 0xbe, 0x94, 0x13, 0x0f, 0x04, 0x3f, 0xdf, 0x51);

//class CGraphCreatorPropertyFilter: public CBaseFilter 
//{
//public:
//    CGraphCreatorPropertyFilter(TCHAR *pName, LPUNKNOWN lpunk, CLSID clsid, HRESULT *phr);
//#ifdef UNICODE
//    CGraphCreatorPropertyFilter(CHAR *pName, LPUNKNOWN lpunk, CLSID clsid, HRESULT *phr);
//#endif
//    ~CGraphCreatorPropertyFilter();
//};

class CGraphCreatorPropertyPage :
   public CBasePropertyPage
{
public:
   static CUnknown * WINAPI CreateInstance(LPUNKNOWN lpunk, HRESULT *phr);
   CGraphCreatorPropertyPage(LPUNKNOWN lpunk, HRESULT *phr);
   virtual ~CGraphCreatorPropertyPage(void);

    HRESULT OnConnect(IUnknown *pUnknown);
    HRESULT OnDisconnect();
    HRESULT OnActivate();
    HRESULT OnDeactivate();
    HRESULT OnApplyChanges();

    virtual BOOL OnReceiveMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

private:
    BOOL    m_fWindowInActive;          // TRUE ==> dialog is being destroyed
    IBaseFilter *m_pFilter;
    IPin*m_pPin;
};
#endif