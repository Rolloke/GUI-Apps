// VMPEG4DecoderProp.h: implementation of CVMPEG4DecoderProp class
// 
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include <streams.h>
#include <commctrl.h>
#include <olectl.h>
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>

#include "resource.h"
#include "iVMPEG4Decoder.h"
#include "VMPEG4Decoder.h"
#include "VMPEG4DecoderProp.h"


///////////////////////////////////////////////////////////////////////
// CreateInstance: Used by the DirectShow base classes to create 
// instances
///////////////////////////////////////////////////////////////////////
CUnknown *CVMPEG4DecoderProp::CreateInstance(LPUNKNOWN lpunk, HRESULT *phr)
{
    CUnknown *punk = new CVMPEG4DecoderProp(lpunk, phr);
    if (punk == NULL) {
    *phr = E_OUTOFMEMORY;
    }
    return punk;

}

///////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////
CVMPEG4DecoderProp::CVMPEG4DecoderProp(LPUNKNOWN pUnk, HRESULT *phr) :
    CBasePropertyPage(NAME("VMPEG4Decoder Property Page"),
                      pUnk,IDD_PROPERTIES,IDS_TITLE),
    m_pIVMPEG4Decoder(NULL),
    m_bIsInitialized(FALSE)
{
    ASSERT(phr);
}

///////////////////////////////////////////////////////////////////////
// OnReceiveMessage: Handles the messages for our property window
///////////////////////////////////////////////////////////////////////
BOOL CVMPEG4DecoderProp::OnReceiveMessage(HWND hwnd,
                                          UINT uMsg,
                                          WPARAM wParam,
                                          LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_INITDIALOG:  // init property page
            break;

        case WM_COMMAND:
        {
            if (m_bIsInitialized) {
                // Set dirty to indicate the setting has changed
                m_bDirty = TRUE;
                if (m_pPageSite)
                    m_pPageSite->OnStatusChange(PROPPAGESTATUS_DIRTY);
            }
            return (LRESULT) 1;
        }
    }
    return CBasePropertyPage::OnReceiveMessage(hwnd,uMsg,wParam,lParam);
}

///////////////////////////////////////////////////////////////////////
// OnConnect: Called when we connect to a transform filter
///////////////////////////////////////////////////////////////////////
HRESULT CVMPEG4DecoderProp::OnConnect(IUnknown *pUnknown)
{
    ASSERT(m_pIVMPEG4Decoder == NULL);

    HRESULT hr = pUnknown->QueryInterface(IID_IVMPEG4Decoder, (void **) &m_pIVMPEG4Decoder);
    if (FAILED(hr)) {
        return E_NOINTERFACE;
    }

    ASSERT(m_pIVMPEG4Decoder);
    m_bIsInitialized = FALSE ;

    return NOERROR;
}

///////////////////////////////////////////////////////////////////////
// OnDisconnect: Likewise called when we disconnect from a filter
///////////////////////////////////////////////////////////////////////
HRESULT CVMPEG4DecoderProp::OnDisconnect()
{
    if (m_pIVMPEG4Decoder == NULL) {
        return E_UNEXPECTED;
    }

    m_pIVMPEG4Decoder->Release();
    m_pIVMPEG4Decoder = NULL;
    return NOERROR;
}

///////////////////////////////////////////////////////////////////////
// OnActivate: We are being activated
///////////////////////////////////////////////////////////////////////
HRESULT CVMPEG4DecoderProp::OnActivate()
{
    ASSERT(m_pIVMPEG4Decoder);
    // Get the current setup of the filter
    m_pIVMPEG4Decoder->GetParams(&m_VMPEG4DecoderParams);
    // Set the control to the current setup
    SetControlValues();
    m_bIsInitialized = TRUE;

    return NOERROR;
}

///////////////////////////////////////////////////////////////////////
// OnDeactivate: We are being deactivated
///////////////////////////////////////////////////////////////////////
HRESULT CVMPEG4DecoderProp::OnDeactivate(void)
{
    ASSERT(m_pIVMPEG4Decoder);
    m_bIsInitialized = FALSE;
    
    return NOERROR;
}

///////////////////////////////////////////////////////////////////////
// OnApplyChanges: Apply any changes so far made
///////////////////////////////////////////////////////////////////////
HRESULT CVMPEG4DecoderProp::OnApplyChanges()
{
    ASSERT(m_pIVMPEG4Decoder);
    // Update the setup from controls and set it into the filter
    GetControlValues();
    m_pIVMPEG4Decoder->SetParams(&m_VMPEG4DecoderParams);

    return NOERROR;
}

///////////////////////////////////////////////////////////////////////
// GetControlValues: read the setup from current controls
///////////////////////////////////////////////////////////////////////
void CVMPEG4DecoderProp::GetControlValues()
{
    // param1
    m_VMPEG4DecoderParams.param1 = 
        IsDlgButtonChecked( m_Dlg, IDC_PARAMETERS1 ) == BST_CHECKED;
    
    // param2
    TCHAR   sz[STR_MAX_LENGTH];
    Edit_GetText(GetDlgItem(m_Dlg, IDC_PARAMETERS2), sz, STR_MAX_LENGTH);
    m_VMPEG4DecoderParams.param2 = atof(sz);
}

///////////////////////////////////////////////////////////////////////
// SetControlValues: set the current setup to the controls
///////////////////////////////////////////////////////////////////////
void CVMPEG4DecoderProp::SetControlValues()
{
    // param1
    CheckDlgButton(m_Dlg, IDC_PARAMETERS1, m_VMPEG4DecoderParams.param1);
    // param2
    TCHAR   sz[STR_MAX_LENGTH];
    _stprintf(sz, TEXT("%f"), m_VMPEG4DecoderParams.param2);
    Edit_SetText(GetDlgItem(m_Dlg, IDC_PARAMETERS2), sz);
}

