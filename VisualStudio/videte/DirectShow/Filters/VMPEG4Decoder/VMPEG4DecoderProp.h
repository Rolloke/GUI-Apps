// VMPEG4DecoderProp.h: interface for the CVMPEG4DecoderProp class
// 
//////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////
//  CVMPEG4DecoderProp: property page of CVMPEG4Decoder
//  It provides a dialogbox to setup the CVMPEG4Decoder.
///////////////////////////////////////////////////////////////////////
class CVMPEG4DecoderProp : public CBasePropertyPage
{
public:
    static CUnknown * WINAPI CreateInstance(LPUNKNOWN lpunk, HRESULT *phr);

private:
    // Constructor
    CVMPEG4DecoderProp(LPUNKNOWN lpunk, HRESULT *phr);
    
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
    IVMPEG4Decoder *m_pIVMPEG4Decoder;  // interface of the filter
    VMPEG4DecoderParams m_VMPEG4DecoderParams; // Filter parameters
};

