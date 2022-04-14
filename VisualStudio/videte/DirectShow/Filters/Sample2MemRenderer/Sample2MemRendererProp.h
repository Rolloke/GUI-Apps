// Sample2MemRendererProp.h: interface for the CSample2MemRendererProp class
// 
//////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////
//  CSample2MemRendererProp: property page of CSample2MemRenderer
//  It provides a dialogbox to setup the CSample2MemRenderer.
///////////////////////////////////////////////////////////////////////
class CSample2MemRendererProp : public CBasePropertyPage
{
public:
    static CUnknown * WINAPI CreateInstance(LPUNKNOWN lpunk, HRESULT *phr);

private:
    // Constructor
    CSample2MemRendererProp(LPUNKNOWN lpunk, HRESULT *phr);
    
    // Handle the messages in the dialogbox
    BOOL OnReceiveMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
    // Connect the property page to filter (pUnknown) or Disconnect
    HRESULT OnConnect(IUnknown *pUnknown);
    HRESULT OnDisconnect();

    // When activate or deactivate or apply changes the property page
    HRESULT OnActivate();
    HRESULT OnDeactivate();
    HRESULT OnApplyChanges();

    // Helper functions
    void    GetControlValues(); // Read parameters from controls
    void    SetControlValues(); // Set controls according to parameters

    BOOL m_bIsInitialized; // Used to ignore startup messages
    ISample2MemRenderer *m_pISample2MemRenderer;  // interface of the filter
    Sample2MemRendererParams m_Sample2MemRendererParams; // Filter parameters
};

