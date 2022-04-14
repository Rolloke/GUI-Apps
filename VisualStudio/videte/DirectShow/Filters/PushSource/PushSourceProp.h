// PushSourceProp.h: interface for the CPushSourceProp class
// 
//////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////
//  CPushSourceProp: property page of CPushSource
//  It provides a dialogbox to setup the CPushSource.
///////////////////////////////////////////////////////////////////////
class CPushSourceProp : public CBasePropertyPage
{
public:
	void CheckStates();
    static CUnknown * WINAPI CreateInstance(LPUNKNOWN lpunk, HRESULT *phr);

private:
	// Constructor
	CPushSourceProp(LPUNKNOWN lpunk, HRESULT *phr);

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
	IPushSource *m_pIPushSource;  // interface of the filter
	PushSourceParams m_PushSourceParams;
};

