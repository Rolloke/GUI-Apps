// PushSourceProp.h: implementation of CPushSourceProp class
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
#include "iPushSource.h"
#include "PushSource.h"
#include "PushSourceProp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////
// CreateInstance: Used by the DirectShow base classes to create 
// instances
///////////////////////////////////////////////////////////////////////
CUnknown *CPushSourceProp::CreateInstance(LPUNKNOWN lpunk, HRESULT *phr)
{
    CUnknown *punk = new CPushSourceProp(lpunk, phr);
    if (punk == NULL) 
	{
		*phr = E_OUTOFMEMORY;
    }
    return punk;

}

///////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////
CPushSourceProp::CPushSourceProp(LPUNKNOWN pUnk, HRESULT *phr) :
    CBasePropertyPage(NAME("PushSource Property Page"),
                      pUnk,IDD_PROPERTIES,IDS_TITLE),
    m_pIPushSource(NULL),
    m_bIsInitialized(FALSE)
{
    ASSERT(phr);
}

///////////////////////////////////////////////////////////////////////
// OnReceiveMessage: Handles the messages for our property window
///////////////////////////////////////////////////////////////////////
BOOL CPushSourceProp::OnReceiveMessage(HWND hwnd,
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
            if (m_bIsInitialized) 
			{
                // Set dirty to indicate the setting has changed
                m_bDirty = TRUE;
                if (m_pPageSite)
				{
                    m_pPageSite->OnStatusChange(PROPPAGESTATUS_DIRTY);
				}
            }
			if (HIWORD(wParam) == BN_CLICKED)
			{
				switch (LOWORD(wParam))
				{
					case IDC_CK_CAPTURE:
						GetControlValues();
						if (m_PushSourceParams.m_bCapture)
						{
							m_PushSourceParams.m_bPreview = 0;
							m_PushSourceParams.m_bStill   = 0;
							SetControlValues();
						}
						CheckStates();
					break;
					case IDC_CK_PREVIEW:
						GetControlValues();
						if (m_PushSourceParams.m_bPreview)
						{
							m_PushSourceParams.m_bCapture = 0;
							m_PushSourceParams.m_bStill   = 0;
							SetControlValues();
						}
						CheckStates();
					break;
					case IDC_CK_STILL:
						GetControlValues();
						if (m_PushSourceParams.m_bStill)
						{
							m_PushSourceParams.m_bPreview = 0;
							m_PushSourceParams.m_bCapture = 0;
							SetControlValues();
						}
						CheckStates();
					break;
					case IDC_CK_GENERATE_TIME_STAMPS:
						GetControlValues();
						CheckStates();
					break;
				}
			}
            return (LRESULT) 1;
        }
    }
    return CBasePropertyPage::OnReceiveMessage(hwnd,uMsg,wParam,lParam);
}

///////////////////////////////////////////////////////////////////////
// OnConnect: Called when we connect to a transform filter
///////////////////////////////////////////////////////////////////////
HRESULT CPushSourceProp::OnConnect(IUnknown *pUnknown)
{
	ASSERT(m_pIPushSource == NULL);

	HRESULT hr = pUnknown->QueryInterface(IID_IPushSource, (void **) &m_pIPushSource);
	if (FAILED(hr)) 
	{
		return E_NOINTERFACE;
	}

	ASSERT(m_pIPushSource);
	m_bIsInitialized = FALSE ;

	return NOERROR;
}

///////////////////////////////////////////////////////////////////////
// OnDisconnect: Likewise called when we disconnect from a filter
///////////////////////////////////////////////////////////////////////
HRESULT CPushSourceProp::OnDisconnect()
{
	if (m_pIPushSource == NULL) 
	{
		return E_UNEXPECTED;
	}

	m_pIPushSource->Release();
	m_pIPushSource = NULL;
	return NOERROR;
}

///////////////////////////////////////////////////////////////////////
// OnActivate: We are being activated
///////////////////////////////////////////////////////////////////////
HRESULT CPushSourceProp::OnActivate()
{
	ASSERT(m_pIPushSource);
	// TODO! CPushSourceProp::OnActivate()
	m_pIPushSource->GetParams(&m_PushSourceParams);
	m_pIPushSource->SetPropertyPageHWND(m_Dlg);

	SetControlValues();
	CheckStates();
	m_bIsInitialized = TRUE;

	return NOERROR;
}

///////////////////////////////////////////////////////////////////////
// OnDeactivate: We are being deactivated
///////////////////////////////////////////////////////////////////////
HRESULT CPushSourceProp::OnDeactivate(void)
{
	ASSERT(m_pIPushSource);
	m_bIsInitialized = FALSE;
	m_pIPushSource->SetPropertyPageHWND(NULL);

	return NOERROR;
}

///////////////////////////////////////////////////////////////////////
// OnApplyChanges: Apply any changes so far made
///////////////////////////////////////////////////////////////////////
HRESULT CPushSourceProp::OnApplyChanges()
{
	ASSERT(m_pIPushSource);
	// Update the setup from controls and set it into the filter
	GetControlValues();

	m_pIPushSource->SetParams(&m_PushSourceParams);
	REFERENCE_TIME  rt = MILLISECONDS_TO_100NS_UNITS(GetDlgItemInt(m_Dlg, IDC_EDT_STREAM_OFFSET, NULL, true));
	m_pIPushSource->SetStreamOffset(rt);

	return NOERROR;
}

///////////////////////////////////////////////////////////////////////
// GetControlValues: read the setup from current controls
///////////////////////////////////////////////////////////////////////
void CPushSourceProp::GetControlValues()
{
	m_PushSourceParams.m_bCapture = (IsDlgButtonChecked(m_Dlg, IDC_CK_CAPTURE) == BST_CHECKED) ? TRUE : FALSE;
	if (m_PushSourceParams.m_bCapture && IsDlgButtonChecked(m_Dlg, IDC_CK_CAPTURE_PRIV) == BST_CHECKED)
	{
		m_PushSourceParams.m_bCapture = 2;
	}
	m_PushSourceParams.m_bPreview = (IsDlgButtonChecked(m_Dlg, IDC_CK_PREVIEW) == BST_CHECKED) ? TRUE : FALSE;
	if (m_PushSourceParams.m_bPreview && IsDlgButtonChecked(m_Dlg, IDC_CK_PREVIEW_PRIV) == BST_CHECKED)
	{
		m_PushSourceParams.m_bPreview = 2;
	}
	m_PushSourceParams.m_bStill = (IsDlgButtonChecked(m_Dlg, IDC_CK_STILL) == BST_CHECKED) ? TRUE : FALSE;
	if (m_PushSourceParams.m_bStill && IsDlgButtonChecked(m_Dlg, IDC_CK_STILL_PRIV) == BST_CHECKED)
	{
		m_PushSourceParams.m_bStill = 2;
	}

	m_PushSourceParams.m_bLiveSource = (IsDlgButtonChecked(m_Dlg, IDC_CK_LIVE_SOURCE) == BST_CHECKED) ? TRUE : FALSE;
	m_PushSourceParams.m_bInternalRM = (IsDlgButtonChecked(m_Dlg, IDC_CK_INTERNAL_RM) == BST_CHECKED) ? TRUE : FALSE;
	
	m_PushSourceParams.m_lMiscFlags  = (IsDlgButtonChecked(m_Dlg, IDC_CK_SOURCE)      == BST_CHECKED) ? AM_FILTER_MISC_FLAGS_IS_SOURCE : 0;
	m_PushSourceParams.m_bGenerateTimeStamps = (IsDlgButtonChecked(m_Dlg, IDC_CK_GENERATE_TIME_STAMPS) == BST_CHECKED) ? TRUE : FALSE;
	m_PushSourceParams.m_nOffsetInMS = GetDlgItemInt(m_Dlg, IDC_EDT_OFFSET, NULL, true);
}

///////////////////////////////////////////////////////////////////////
// SetControlValues: set the current setup to the controls
///////////////////////////////////////////////////////////////////////
void CPushSourceProp::SetControlValues()
{
	CheckDlgButton(m_Dlg, IDC_CK_CAPTURE, m_PushSourceParams.m_bCapture ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg, IDC_CK_PREVIEW, m_PushSourceParams.m_bPreview ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg, IDC_CK_STILL  , m_PushSourceParams.m_bStill   ? BST_CHECKED : BST_UNCHECKED);

	CheckDlgButton(m_Dlg, IDC_CK_CAPTURE_PRIV, m_PushSourceParams.m_bCapture == 2 ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg, IDC_CK_PREVIEW_PRIV, m_PushSourceParams.m_bPreview == 2 ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg, IDC_CK_STILL_PRIV  , m_PushSourceParams.m_bStill   == 2 ? BST_CHECKED : BST_UNCHECKED);

	CheckDlgButton(m_Dlg, IDC_CK_LIVE_SOURCE, m_PushSourceParams.m_bLiveSource ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg, IDC_CK_INTERNAL_RM, m_PushSourceParams.m_bInternalRM ? BST_CHECKED : BST_UNCHECKED);
	
	CheckDlgButton(m_Dlg, IDC_CK_SOURCE, m_PushSourceParams.m_lMiscFlags & AM_FILTER_MISC_FLAGS_IS_SOURCE ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg, IDC_CK_GENERATE_TIME_STAMPS, m_PushSourceParams.m_bGenerateTimeStamps ? BST_CHECKED : BST_UNCHECKED);
	REFERENCE_TIME rt;
	m_pIPushSource->GetStreamOffset(&rt);
	SetDlgItemInt(m_Dlg, IDC_EDT_STREAM_OFFSET, UNITS_100NS_TO_MILLISECONDS(rt), TRUE);
	SetDlgItemInt(m_Dlg, IDC_EDT_OFFSET, m_PushSourceParams.m_nOffsetInMS, TRUE);

	CheckDlgButton(m_Dlg, IDC_CK_PREFER_OWN_ALLOCATOR, m_PushSourceParams.m_bPreferOwnAllocator? BST_CHECKED : BST_UNCHECKED);
}

void CPushSourceProp::CheckStates()
{
	EnableWindow(GetDlgItem(m_Dlg, IDC_CK_CAPTURE_PRIV), m_PushSourceParams.m_bCapture);
	EnableWindow(GetDlgItem(m_Dlg, IDC_CK_PREVIEW_PRIV), m_PushSourceParams.m_bPreview);
	EnableWindow(GetDlgItem(m_Dlg, IDC_CK_STILL_PRIV  ), m_PushSourceParams.m_bStill);
	EnableWindow(GetDlgItem(m_Dlg, IDC_EDT_STREAM_OFFSET), m_PushSourceParams.m_bGenerateTimeStamps);
}
