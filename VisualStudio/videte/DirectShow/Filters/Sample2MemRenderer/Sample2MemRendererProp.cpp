// Sample2MemRendererProp.h: implementation of CSample2MemRendererProp class
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
#include "iSample2MemRenderer.h"
#include "Sample2MemRenderer.h"
#include "Sample2MemRendererProp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////
// CreateInstance: Used by the DirectShow base classes to create 
// instances
///////////////////////////////////////////////////////////////////////
CUnknown *CSample2MemRendererProp::CreateInstance(LPUNKNOWN lpunk, HRESULT *phr)
{
    CUnknown *punk = new CSample2MemRendererProp(lpunk, phr);
    if (punk == NULL) {
    *phr = E_OUTOFMEMORY;
    }
    return punk;

}

///////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////
CSample2MemRendererProp::CSample2MemRendererProp(LPUNKNOWN pUnk, HRESULT *phr) :
    CBasePropertyPage(NAME("Sample2MemRenderer Property Page"),
                      pUnk,IDD_PROPERTIES,IDS_TITLE),
    m_pISample2MemRenderer(NULL),
    m_bIsInitialized(FALSE)
{
    ASSERT(phr);
}

///////////////////////////////////////////////////////////////////////
// OnReceiveMessage: Handles the messages for our property window
///////////////////////////////////////////////////////////////////////
BOOL CSample2MemRendererProp::OnReceiveMessage(HWND hwnd,
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
HRESULT CSample2MemRendererProp::OnConnect(IUnknown *pUnknown)
{
    ASSERT(m_pISample2MemRenderer == NULL);

    HRESULT hr = pUnknown->QueryInterface(IID_ISample2MemRenderer, (void **) &m_pISample2MemRenderer);
    if (FAILED(hr)) {
        return E_NOINTERFACE;
    }

    ASSERT(m_pISample2MemRenderer);
    m_bIsInitialized = FALSE ;

    return NOERROR;
}

///////////////////////////////////////////////////////////////////////
// OnDisconnect: Likewise called when we disconnect from a filter
///////////////////////////////////////////////////////////////////////
HRESULT CSample2MemRendererProp::OnDisconnect()
{
    if (m_pISample2MemRenderer == NULL) {
        return E_UNEXPECTED;
    }

    m_pISample2MemRenderer->Release();
    m_pISample2MemRenderer = NULL;
    return NOERROR;
}

///////////////////////////////////////////////////////////////////////
// OnActivate: We are being activated
///////////////////////////////////////////////////////////////////////
HRESULT CSample2MemRendererProp::OnActivate()
{
    ASSERT(m_pISample2MemRenderer);
    // Get the current setup of the filter
    m_pISample2MemRenderer->GetParams(&m_Sample2MemRendererParams);
    // Set the control to the current setup
    SetControlValues();
    m_bIsInitialized = TRUE;

    return NOERROR;
}

///////////////////////////////////////////////////////////////////////
// OnDeactivate: We are being deactivated
///////////////////////////////////////////////////////////////////////
HRESULT CSample2MemRendererProp::OnDeactivate(void)
{
    ASSERT(m_pISample2MemRenderer);
    m_bIsInitialized = FALSE;
    
    return NOERROR;
}

///////////////////////////////////////////////////////////////////////
// OnApplyChanges: Apply any changes so far made
///////////////////////////////////////////////////////////////////////
HRESULT CSample2MemRendererProp::OnApplyChanges()
{
    ASSERT(m_pISample2MemRenderer);
    // Update the setup from controls and set it into the filter
    GetControlValues();
    m_pISample2MemRenderer->SetParams(&m_Sample2MemRendererParams);

    return NOERROR;
}

///////////////////////////////////////////////////////////////////////
// GetControlValues: read the setup from current controls
///////////////////////////////////////////////////////////////////////
void CSample2MemRendererProp::GetControlValues()
{
	m_Sample2MemRendererParams.m_lMiscFlags = (IsDlgButtonChecked(m_Dlg, IDC_CK_RENDERER) == BST_CHECKED) ? AM_FILTER_MISC_FLAGS_IS_RENDERER: 0;
	m_Sample2MemRendererParams.m_bScheduleSamples = (IsDlgButtonChecked(m_Dlg, IDC_CK_SCHEDULE) == BST_CHECKED) ? TRUE : FALSE;
}

///////////////////////////////////////////////////////////////////////
// SetControlValues: set the current setup to the controls
///////////////////////////////////////////////////////////////////////
void CSample2MemRendererProp::SetControlValues()
{
	CheckDlgButton(m_Dlg, IDC_CK_RENDERER, m_Sample2MemRendererParams.m_lMiscFlags & AM_FILTER_MISC_FLAGS_IS_RENDERER? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg, IDC_CK_SCHEDULE, m_Sample2MemRendererParams.m_bScheduleSamples ? BST_CHECKED : BST_UNCHECKED);
}

